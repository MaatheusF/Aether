/* public/js/services/api_status.js
   Indicador global de conexão com o Core — ponto + rótulo no rodapé da
   rail (ver partials/_rail.html.twig), presente em toda a área logada.
   Consulta GET /api/core/status a cada 60s.
*/
(function () {
    const dotStatus = document.getElementById('core-status-dot');
    const labelStatus = document.getElementById('core-status-label');
    if (!dotStatus || !labelStatus) return;

    function setCoreStatus(status) {
        const online = status.status === 'UP';

        dotStatus.classList.toggle('ok', online);
        dotStatus.classList.toggle('danger', !online);
        labelStatus.classList.toggle('ok', online);
        labelStatus.classList.toggle('danger', !online);
        labelStatus.textContent = online ? 'Core online' : 'Core offline';
    }

    async function getCoreStatus() {
        try {
            const resp = await fetch(`${window.AETHER_CORE_BASE_URL}/api/core/status`);
            if (!resp.ok) throw new Error('Core unavailable');
            const data = await resp.json();
            setCoreStatus(data);
            return data;
        } catch (erro) {
            console.error('Erro ao consultar o Core:', erro);
            setCoreStatus({ status: 'error', message: erro.message });
            return { status: 'error', message: erro.message };
        }
    }

    document.addEventListener('DOMContentLoaded', getCoreStatus);
    setInterval(getCoreStatus, 60000);
})();
