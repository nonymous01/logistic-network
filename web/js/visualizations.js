function updateNetworkGraph(network) {
    if (!network || !network.nodes || !network.edges) {
        console.error('Données réseau invalides');
        return;
    }
    
    const container = document.getElementById('networkGraph');
    if (!container) return;

    // Effacer le contenu précédent
    container.innerHTML = '';

    // Vérifier si D3 est chargé
    if (typeof d3 === 'undefined') {
        container.innerHTML = '<div class="text-center p-4">D3.js non chargé - Impossible d\'afficher le réseau</div>';
        return;
    }

    try {
        const width = container.clientWidth;
        const height = 300;

        const svg = d3.select(container)
            .append('svg')
            .attr('width', width)
            .attr('height', height);

        // Simulation de forces
        const simulation = d3.forceSimulation(network.nodes)
            .force('link', d3.forceLink(network.edges).id(d => d.id).distance(100))
            .force('charge', d3.forceManyBody().strength(-300))
            .force('center', d3.forceCenter(width / 2, height / 2));

        // Dessiner les liens
        const link = svg.append('g')
            .selectAll('line')
            .data(network.edges)
            .enter()
            .append('line')
            .attr('stroke', '#999')
            .attr('stroke-width', 2);

        // Dessiner les nœuds
        const node = svg.append('g')
            .selectAll('circle')
            .data(network.nodes)
            .enter()
            .append('circle')
            .attr('r', 10)
            .attr('fill', d => d.type === 'warehouse' ? '#3B82F6' : '#10B981');

        // Ajouter les étiquettes
        const label = svg.append('g')
            .selectAll('text')
            .data(network.nodes)
            .enter()
            .append('text')
            .text(d => d.name)
            .attr('font-size', 12)
            .attr('dx', 15)
            .attr('dy', 4);

        // Mettre à jour les positions
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
    } catch (error) {
        console.error('Erreur lors de la création du graphe:', error);
        container.innerHTML = '<div class="text-center p-4">Erreur d\'affichage du réseau</div>';
    }
}