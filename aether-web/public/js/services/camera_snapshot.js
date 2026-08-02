/* public/js/services/camera_snapshot.js
   Atualiza periodicamente qualquer <img data-camera-snapshot> com o
   snapshot JPEG mais recente da câmera, via GET
   /api/horus/cameras/:canal/snapshot no Core (ver CameraController).

   cache: 'no-store' evita que o browser reutilize o snapshot anterior.
   O object URL anterior é revogado a cada troca pra não vazar memória —
   a página fica aberta por longos períodos num dashboard.

   Cada <img> pode ter um irmão [data-camera-status-dot] (ver
   modulos/horus.html.twig) que reflete a atualização do snapshot: verde
   enquanto a última atualização bem-sucedida tiver menos de STALE_MS,
   vermelho a partir daí — mesmo que a imagem continue tentando atualizar
   em segundo plano.
*/
(function () {
    const REFRESH_MS = 5000; // Tempo de atualização da camera (Por virar uma configuração no futuro)
    const STALE_MS = 10000;  // Tempo para definir a camera como desatualizada (Por virar uma configuração no futuro)
    const canaisEmAndamento = new Set();

    function dotDe(img) {
        return img.parentElement?.querySelector('[data-camera-status-dot]') ?? null;
    }

    function marcarAtualizacao(img) {
        img.dataset.lastUpdate = String(Date.now());
        const dot = dotDe(img);
        if (!dot) return;
        dot.classList.remove('bg-aether-critico');
        dot.classList.add('bg-aether-broto');
    }

    function verificarAtualizacao(img) {
        const dot = dotDe(img);
        if (!dot) return;

        const ultima = Number(img.dataset.lastUpdate || 0);
        const estaVelho = !ultima || (Date.now() - ultima) > STALE_MS;

        dot.classList.toggle('bg-aether-critico', estaVelho);
        dot.classList.toggle('bg-aether-broto', !estaVelho);
    }

    async function atualizarSnapshot(img) {
        const canal = img.dataset.cameraChannel;

        if (canaisEmAndamento.has(canal)) return;
        canaisEmAndamento.add(canal);

        const url = `${window.AETHER_CORE_BASE_URL}/api/horus/cameras/${canal}/snapshot`;

        try {
            const resp = await fetch(url, { cache: 'no-store' });
            if (!resp.ok) {
                console.error(`Falha ao buscar snapshot da câmera ${canal}:`, resp.status);
                return;
            }

            const blob = await resp.blob();
            const objectUrl = URL.createObjectURL(blob);
            const anterior = img.dataset.objectUrl;

            img.src = objectUrl;
            img.dataset.objectUrl = objectUrl;
            marcarAtualizacao(img);

            if (anterior) URL.revokeObjectURL(anterior);
        } catch (erro) {
            console.error(`Erro ao atualizar snapshot da câmera ${canal}:`, erro);
        } finally {
            canaisEmAndamento.delete(canal);
        }
    }

    const imgs = document.querySelectorAll('[data-camera-snapshot]');

    imgs.forEach((img) => {
        atualizarSnapshot(img);
        setInterval(() => atualizarSnapshot(img), REFRESH_MS);
    });

    // Um único timer verifica o frescor de todas as câmeras, em vez de um
    // setInterval por imagem — mesmo resultado, menos timers rodando.
    setInterval(() => imgs.forEach(verificarAtualizacao), 1000);
})();
