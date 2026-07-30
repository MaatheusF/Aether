/* public/js/services/horus_gate_control.js

   Aciona o portão do Horus via um endpoint do próprio Symfony (mesma
   origem)

   Contrato de atributos esperado no botão:
     - data-url          : rota Symfony que recebe o POST (renderizada
                            via path() no Twig, nunca hardcoded aqui —
                            este arquivo é servido estático e não passa
                            pelo parser do Twig).
     - data-cooldown-ms   : opcional, tempo em ms que o botão fica
                            desabilitado após o clique (padrão 4000).
                            Evita reacionar enquanto o portão físico
                            ainda está em movimento.
*/
(function () {
    const botao = document.getElementById('btn-portao-atuador');
    if (!botao) return;

    botao.addEventListener('click', async () => {
        const textoOriginal = botao.textContent;
        botao.disabled = true;
        botao.textContent = 'Enviando...';

        try {
            const resp = await fetch(botao.dataset.url, { method: 'POST' });
            if (!resp.ok) throw new Error(`Symfony respondeu ${resp.status}`);
        } catch (erro) {
            console.error('Erro ao enviar comando de debug ao portao:', erro);
            botao.classList.add('atuador-erro');
            setTimeout(() => botao.classList.remove('atuador-erro'), 1300);
        } finally {
            setTimeout(() => {
                botao.disabled = false;
                botao.textContent = textoOriginal;
            }, Number(botao.dataset.cooldownMs) || 4000);
        }
    });
})();
