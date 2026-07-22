/* public/js/services/atuador_control.js
   Serviço genérico de acionamento de atuadores (toggles com role="switch").
   Cobre qualquer módulo — hoje Poseidon (cascata/névoa/bombeamento) e Horus
   (portão) — desde que o botão siga o contrato de atributos abaixo. Nenhuma
   lógica é específica de módulo; quem muda é só o dado nos atributos.

   Contrato de atributos esperado no <button role="switch">:
     - data-modulo      : slug do módulo ("poseidon", "horus", ...)
     - data-atuador      : nome do atuador dentro do módulo ("cascata", "portao", ...)
     - data-dispositivo  : slug do dispositivo — SÓ em módulos multi-dispositivo
                            (Poseidon). Omitir em módulos de instância única
                            (Horus) — a URL montada muda conforme a presença
                            deste atributo.
     - aria-checked      : estado atual (a semântica de "checked" é definida
                            por quem desenha o toggle — ligado no Poseidon,
                            travado no Horus — este script não presume nada
                            sobre o que "checked" significa, só inverte).

   Fluxo (ver docs/api-contract-core.md e openapi.yaml para o contrato HTTP):
     1. Clique → botão vira "pendente" imediatamente (sem esperar rede).
     2. POST para o Core com a ação desejada.
     3. Core responde 202 + comando_id (aceito, aguardando confirmação real).
     4. Polling curto no status do módulo até o atuador sair de "pending".
     5. Resolvido → aplica estado real. Sem confirmação em POLL_TIMEOUT_MS →
        reverte pro estado anterior e sinaliza erro (não fica "pendente" pra
        sempre).

   IMPORTANTE: os endpoints usados aqui ainda não existem no Core (ver
   conversa de arquitetura) — até existirem, todo clique vai cair no catch
   e mostrar o estado de erro. Isso é esperado, não é bug deste arquivo.
*/

(function () {
    const POLL_INTERVAL_MS = 1500;
    const POLL_TIMEOUT_MS = 15000;

    // Classes completas por estado — nunca montar por interpolação
    // (ex: `bg-aether-${cor}`): o Tailwind só gera CSS pra classes que
    // aparecem por extenso em algum arquivo escaneado, e um arquivo .js
    // não é escaneado da mesma forma que um .twig. Ver DesignSystem.md §5.1x.
    const TRACK = {
        on: 'relative w-11 h-6 rounded-full transition-colors bg-aether-broto/30 border border-aether-broto/50',
        off: 'relative w-11 h-6 rounded-full transition-colors bg-white/5 border border-aether-border',
        pending: 'relative w-11 h-6 rounded-full transition-colors bg-aether-ambar/20 border border-aether-ambar/40 opacity-70 cursor-wait',
    };
    const THUMB = {
        on: 'absolute top-0.5 right-0.5 w-4.5 h-4.5 rounded-full transition-all bg-aether-broto shadow-glow-broto',
        off: 'absolute top-0.5 left-0.5 w-4.5 h-4.5 rounded-full transition-all bg-slate-500',
        pending: 'absolute top-0.5 right-0.5 w-4.5 h-4.5 rounded-full transition-all bg-aether-ambar/80',
    };

    function aplicarEstado(botao, estado) {
        const thumb = botao.querySelector('span');
        botao.className = TRACK[estado];
        if (thumb) thumb.className = THUMB[estado];
        botao.setAttribute('aria-checked', estado === 'on' ? 'true' : 'false');
        botao.disabled = estado === 'pending';
    }

    function montarUrlAtuador(modulo, atuador, dispositivo) {
        const base = `${window.AETHER_CORE_BASE_URL}/api/modulos/${modulo}`;
        return dispositivo
            ? `${base}/dispositivos/${dispositivo}/atuadores/${atuador}`
            : `${base}/atuadores/${atuador}`;
    }

    function montarUrlStatus(modulo, dispositivo) {
        const base = `${window.AETHER_CORE_BASE_URL}/api/modulos/${modulo}`;
        return dispositivo ? `${base}/dispositivos/${dispositivo}/status` : `${base}/status`;
    }

    function marcarErro(botao, estadoAnterior) {
        aplicarEstado(botao, estadoAnterior);
        botao.classList.add('atuador-erro');
        setTimeout(() => botao.classList.remove('atuador-erro'), 1300);
    }

    async function pollAteResolver(botao, modulo, atuador, dispositivo, estadoAnterior) {
        const inicio = Date.now();

        while (Date.now() - inicio < POLL_TIMEOUT_MS) {
            await new Promise((resolve) => setTimeout(resolve, POLL_INTERVAL_MS));

            try {
                const resp = await fetch(montarUrlStatus(modulo, dispositivo));
                if (!resp.ok) continue;

                const corpo = await resp.json();
                const atuadores = corpo?.data?.actuators ?? corpo?.data?.atuadores ?? {};
                const estado = atuadores[atuador];

                if (estado === 'on' || estado === 'off') {
                    aplicarEstado(botao, estado);
                    return;
                }
                // Ainda "pending" do lado do Core — continua tentando até o timeout.
            } catch (erro) {
                console.error('Erro ao consultar status do atuador:', erro);
            }
        }

        // Sem confirmação dentro do prazo — não deixa o botão preso em "pendente".
        marcarErro(botao, estadoAnterior);
    }

    async function acionar(botao) {
        const modulo = botao.dataset.modulo;
        const atuador = botao.dataset.atuador;
        const dispositivo = botao.dataset.dispositivo || null;
        const ligado = botao.getAttribute('aria-checked') === 'true';
        const estadoAnterior = ligado ? 'on' : 'off';
        const acaoDesejada = ligado ? 'off' : 'on';

        aplicarEstado(botao, 'pending');

        try {
            const resp = await fetch(montarUrlAtuador(modulo, atuador, dispositivo), {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ acao: acaoDesejada }),
            });

            if (!resp.ok) {
                throw new Error(`Core respondeu ${resp.status} ao acionar ${modulo}/${atuador}`);
            }

            // Core aceitou (202) — aguarda a confirmação real via polling.
            pollAteResolver(botao, modulo, atuador, dispositivo, estadoAnterior);
        } catch (erro) {
            console.error('Erro ao acionar atuador:', erro);
            marcarErro(botao, estadoAnterior);
        }
    }

    document.querySelectorAll('[role="switch"][data-atuador]').forEach((botao) => {
        // Botões nativamente `disabled` (ex: bombeamento do Poseidon, hoje uma
        // demonstração permanente do visual "Pendente") não recebem listener.
        if (botao.disabled) return;
        botao.addEventListener('click', () => acionar(botao));
    });
})();
