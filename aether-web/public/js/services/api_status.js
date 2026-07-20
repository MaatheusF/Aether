/* Elementos do DOM que exibe o status do Core */
const dot_status = document.getElementById("core-status-dot");
const label_status = document.getElementById("core-status-label");

/* Função que consulta o status do Core */
async function getCoreStatus() {
    try {
        const response = await fetch(`${window.AETHER_CORE_BASE_URL}/api/status`);
        if (!response.ok) {
            throw new Error('Core unavailable');
        }
        const data = await response.json();
        setCoreStatus(data);
        return data;
    } catch (error) {
        console.error('Erro ao consultar o Core:', error);
        setCoreStatus(error);
        return { status: 'error', message: error.message };
    }
}

/* Função que atualiza a exibição status do Core no Front */
function setCoreStatus(status) {

    if(status.status === "UP") {
        dot_status.classList.remove("bg-aether-critico");
        dot_status.classList.add("bg-aether-broto");

        label_status.classList.remove("text-aether-critico");
        label_status.classList.add("text-aether-broto");
        label_status.textContent = "Core Online"
    } else {
        dot_status.classList.remove("bg-aether-broto");
        dot_status.classList.add("bg-aether-critico");

        label_status.classList.remove("text-aether-broto");
        label_status.classList.add("text-aether-critico");
        label_status.textContent = "Core Offline"
    }
}

document.addEventListener("DOMContentLoaded", getCoreStatus);
setInterval(getCoreStatus, 60000); /* Atualiza o status a cada 60 segundos */
