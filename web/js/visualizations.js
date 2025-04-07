// Configuration des couleurs
const colors = {
    primary: '#3B82F6',
    secondary: '#10B981',
    accent: '#8B5CF6',
    warning: '#F59E0B',
    danger: '#EF4444'
};

// Initialisation des graphiques
document.addEventListener('DOMContentLoaded', () => {
    initializeNetworkGraph();
    initializeDeliveryChart();
    initializeVehicleStatusChart();
});

// Visualisation du réseau logistique avec D3.js
function initializeNetworkGraph() {
    const container = d3.select('#networkGraph');
    const width = container.node().getBoundingClientRect().width;
    const height = 400;

    const svg = container.append('svg')
        .attr('width', width)
        .attr('height', height);

    // Simulation de données (à remplacer par les données réelles)
    const nodes = [
        { id: 1, name: 'Entrepôt Central', type: 'warehouse' },
        { id: 2, name: 'Point A', type: 'delivery' },
        { id: 3, name: 'Point B', type: 'delivery' },
        { id: 4, name: 'Point C', type: 'delivery' }
    ];

    const links = [
        { source: 1, target: 2 },
        { source: 1, target: 3 },
        { source: 1, target: 4 }
    ];

    // Simulation de force
    const simulation = d3.forceSimulation(nodes)
        .force('link', d3.forceLink(links).id(d => d.id))
        .force('charge', d3.forceManyBody().strength(-300))
        .force('center', d3.forceCenter(width / 2, height / 2));

    // Création des liens
    const link = svg.append('g')
        .selectAll('line')
        .data(links)
        .enter()
        .append('line')
        .attr('stroke', '#999')
        .attr('stroke-width', 2);

    // Création des nœuds
    const node = svg.append('g')
        .selectAll('circle')
        .data(nodes)
        .enter()
        .append('circle')
        .attr('r', 10)
        .attr('fill', d => d.type === 'warehouse' ? colors.primary : colors.secondary);

    // Ajout des labels
    const label = svg.append('g')
        .selectAll('text')
        .data(nodes)
        .enter()
        .append('text')
        .text(d => d.name)
        .attr('font-size', 12)
        .attr('dx', 15)
        .attr('dy', 4);

    // Mise à jour de la position
    simulation.on('tick', () => {
        link
            .attr('x1', d => d.source.x)
            .attr('y1', d => d.source.y)
            .attr('x2', d => d.target.x)
            .attr('y2', d => d.target.y);

        node
            .attr('cx', d => d.x)
            .attr('cy', d => d.y);

        label
            .attr('x', d => d.x)
            .attr('y', d => d.y);
    });
}

// Graphique des livraisons avec Chart.js
function initializeDeliveryChart() {
    const ctx = document.createElement('canvas');
    document.querySelector('#packagesList').appendChild(ctx);

    new Chart(ctx, {
        type: 'bar',
        data: {
            labels: ['En attente', 'En cours', 'Livrés', 'Retardés'],
            datasets: [{
                label: 'Statut des colis',
                data: [12, 19, 3, 5],
                backgroundColor: [
                    colors.warning,
                    colors.primary,
                    colors.secondary,
                    colors.danger
                ],
                borderWidth: 1
            }]
        },
        options: {
            responsive: true,
            scales: {
                y: {
                    beginAtZero: true
                }
            }
        }
    });
}

// Graphique du statut des véhicules
function initializeVehicleStatusChart() {
    const ctx = document.createElement('canvas');
    document.querySelector('#vehiclesList').appendChild(ctx);

    new Chart(ctx, {
        type: 'doughnut',
        data: {
            labels: ['En route', 'En chargement', 'En pause', 'Hors service'],
            datasets: [{
                data: [30, 10, 5, 2],
                backgroundColor: [
                    colors.primary,
                    colors.secondary,
                    colors.warning,
                    colors.danger
                ]
            }]
        },
        options: {
            responsive: true,
            plugins: {
                legend: {
                    position: 'bottom'
                }
            }
        }
    });
}

// Fonction pour mettre à jour les visualisations avec de nouvelles données
function updateVisualizations(data) {
    // Mettre à jour le graphique de livraison
    // Mettre à jour le graphique des véhicules
    // Mettre à jour le graphe du réseau
} 