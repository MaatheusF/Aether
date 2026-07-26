/* public/js/services/camera_snapshot.js
   Atualiza periodicamente qualquer <img data-camera-snapshot> com o
   snapshot JPEG mais recente da câmera, via GET
   /api/cameras/:canal/snapshot no Core (ver CameraController).

   cache: 'no-store' evita que o browser reutilize o snapshot anterior.
   O object URL anterior é revogado a cada troca pra não vazar memória —
   a página fica aberta por longos períodos num dashboard.
*/
(function () {
    const REFRESH_MS = 5000;
    const canaisEmAndamento = new Set();

    async function atualizarSnapshot(img) {
        const canal = img.dataset.cameraChannel;

        if (canaisEmAndamento.has(canal)) return;
        canaisEmAndamento.add(canal);

        const url = `${window.AETHER_CORE_BASE_URL}/api/cameras/${canal}/snapshot`;

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

            if (anterior) URL.revokeObjectURL(anterior);
        } catch (erro) {
            console.error(`Erro ao atualizar snapshot da câmera ${canal}:`, erro);
        } finally {
            canaisEmAndamento.delete(canal);
        }
    }

    document.querySelectorAll('[data-camera-snapshot]').forEach((img) => {
        atualizarSnapshot(img);
        setInterval(() => atualizarSnapshot(img), REFRESH_MS);
    });
})();
