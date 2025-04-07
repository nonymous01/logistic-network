// Configuration de l'API
const API_URL = 'http://localhost:8080/api';

// Fonction pour récupérer les données des véhicules
async function fetchVehicles() {
    try {
        const response = await fetch(`${API_URL}/vehicles`);
        const vehicles = await response.json();
        updateVehiclesList(vehicles);
    } catch (error) {
        console.error('Erreur lors de la récupération des véhicules:', error);
    }
}

// Fonction pour mettre à jour la liste des véhicules
function updateVehiclesList(vehicles) {
    const vehiclesList = document.getElementById('vehiclesList');
    vehiclesList.innerHTML = '';

    vehicles.forEach(vehicle => {
        const vehicleElement = document.createElement('div');
        vehicleElement.className = 'p-2 border rounded hover:bg-gray-50';
        vehicleElement.innerHTML = `
            <div class="flex justify-between items-center">
                <div>
                    <span class="font-semibold">Véhicule ${vehicle.id}</span>
                    <span class="text-sm text-gray-500">(${vehicle.type})</span>
                </div>
                <div class="flex items-center space-x-2">
                    <span class="text-sm ${vehicle.status === 'active' ? 'text-green-500' : 'text-red-500'}">
                        ${vehicle.status === 'active' ? 'En circulation' : 'Inactif'}
                    </span>
                    <div class="w-2 h-2 rounded-full ${vehicle.status === 'active' ? 'bg-green-500' : 'bg-red-500'}"></div>
                </div>
            </div>
            <div class="text-sm text-gray-600 mt-1">
                Position: ${vehicle.current_location}
                <br>
                Batterie: ${vehicle.current_battery}%
            </div>
        `;
        vehiclesList.appendChild(vehicleElement);
    });
}

// Fonction pour récupérer les données des colis
async function fetchPackages() {
    try {
        const response = await fetch(`${API_URL}/packages`);
        const packages = await response.json();
        updatePackagesList(packages);
    } catch (error) {
        console.error('Erreur lors de la récupération des colis:', error);
    }
}

// Fonction pour mettre à jour la liste des colis
function updatePackagesList(packages) {
    const packagesList = document.getElementById('packagesList');
    packagesList.innerHTML = '';

    packages.forEach(pkg => {
        const packageElement = document.createElement('div');
        packageElement.className = 'p-2 border rounded hover:bg-gray-50';
        packageElement.innerHTML = `
            <div class="flex justify-between items-center">
                <div>
                    <span class="font-semibold">Colis ${pkg.id}</span>
                    <span class="text-sm text-gray-500">(${pkg.priority} priorité)</span>
                </div>
                <div class="flex items-center space-x-2">
                    <span class="text-sm ${pkg.status === 'delivered' ? 'text-green-500' : 'text-yellow-500'}">
                        ${pkg.status === 'delivered' ? 'Livré' : 'En cours'}
                    </span>
                </div>
            </div>
            <div class="text-sm text-gray-600 mt-1">
                De: ${pkg.source} → À: ${pkg.destination}
                <br>
                Poids: ${pkg.weight}kg
            </div>
        `;
        packagesList.appendChild(packageElement);
    });
}

// Fonction pour récupérer les statistiques
async function fetchStatistics() {
    try {
        const response = await fetch(`${API_URL}/statistics`);
        const stats = await response.json();
        updateStatistics(stats);
    } catch (error) {
        console.error('Erreur lors de la récupération des statistiques:', error);
    }
}

// Fonction pour mettre à jour les statistiques
function updateStatistics(stats) {
    document.getElementById('deliveredPackages').textContent = stats.delivered_packages;
    document.getElementById('activeVehicles').textContent = stats.active_vehicles;
    document.getElementById('avgDeliveryTime').textContent = `${stats.avg_delivery_time} min`;
    document.getElementById('networkEfficiency').textContent = `${stats.network_efficiency}%`;
}

// Fonction pour rafraîchir toutes les données
async function refreshData() {
    await Promise.all([
        fetchVehicles(),
        fetchPackages(),
        fetchStatistics()
    ]);
}

// Événement de chargement de la page
document.addEventListener('DOMContentLoaded', () => {
    // Rafraîchir les données initiales
    refreshData();

    // Configurer le bouton de rafraîchissement
    document.getElementById('refreshBtn').addEventListener('click', refreshData);

    // Rafraîchir automatiquement toutes les 30 secondes
    setInterval(refreshData, 30000);
}); 