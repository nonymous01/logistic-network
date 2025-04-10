async function fetchData() {
    try {
        const response = await fetch('http://localhost:6060/data');
        if (!response.ok) {
            throw new Error(`Erreur HTTP: ${response.status}`);
        }
        return await response.json();
    } catch (error) {
        console.error('Erreur:', error);
        showError(error.message);
        return null;
    }
}

function updateDashboard(data) {
    if (!data) return;
    
    // Mettre à jour les statistiques
    document.getElementById('delivered').textContent = data.statistics.delivered_packages;
    document.getElementById('active').textContent = data.statistics.active_vehicles;
    document.getElementById('avg-time').textContent = data.statistics.average_delivery_time.toFixed(1);
    document.getElementById('efficiency').textContent = data.statistics.network_efficiency.toFixed(1);
    
    console.log('Dashboard mis à jour');
}

function showError(message) {
    const errorDiv = document.getElementById('error');
    errorDiv.textContent = message;
    errorDiv.style.display = 'block';
}

// Actualiser toutes les 5 secondes
async function refreshData() {
    const data = await fetchData();
    updateDashboard(data);
}

// Démarrer
document.addEventListener('DOMContentLoaded', () => {
    refreshData();
    setInterval(refreshData, 5000);
});