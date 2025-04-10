#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project.h"

void printNetworkState(const NetworkState* state) {
    printf("\n=== État du réseau ===\n");
    
    // Afficher les véhicules
    printf("\nVéhicules (%d):\n", state->vehicle_count);
    for (int i = 0; i < state->vehicle_count; i++) {
        printf("  - Véhicule %d:\n", state->vehicles[i].id);
        printf("    Type: %s\n", state->vehicles[i].type);
        printf("    Statut: %s\n", state->vehicles[i].status);
        printf("    Localisation: %s\n", state->vehicles[i].current_location);
        printf("    Batterie: %.2f%%\n", state->vehicles[i].current_battery);
        printf("    Nœud actuel: %d\n", state->vehicles[i].current_node);
    }
    
    // Afficher les colis
    printf("\nColis (%d):\n", state->package_count);
    for (int i = 0; i < state->package_count; i++) {
        printf("  - Colis %d:\n", state->packages[i].id);
        printf("    Statut: %s\n", state->packages[i].status);
        printf("    Priorité: %d\n", state->packages[i].priority);
        printf("    Source: %s\n", state->packages[i].source);
        printf("    Destination: %s\n", state->packages[i].destination);
        printf("    Poids: %.2f\n", state->packages[i].weight);
    }
    
    // Afficher le réseau
    printf("\nRéseau:\n");
    printf("  Nœuds (%d):\n", state->node_count);
    for (int i = 0; i < state->node_count; i++) {
        printf("    - Nœud %d: %s (%s)\n", 
               state->nodes[i].id, 
               state->nodes[i].name,
               state->nodes[i].type);
    }
    
    printf("  Arêtes (%d):\n", state->edge_count);
    for (int i = 0; i < state->edge_count; i++) {
        printf("    - %d -> %d (distance: %.2f, temps: %.2f, coût: %.2f)\n",
               state->edges[i].source,
               state->edges[i].target,
               state->edges[i].distance,
               state->edges[i].time,
               state->edges[i].cost);
    }
    
    // Afficher les statistiques
    printf("\nStatistiques:\n");
    printf("  Colis livrés: %d\n", state->stats.delivered_packages);
    printf("  Véhicules actifs: %d\n", state->stats.active_vehicles);
    printf("  Temps moyen de livraison: %.2f\n", state->stats.average_delivery_time);
    printf("  Efficacité du réseau: %.2f%%\n", state->stats.network_efficiency);
}

int main() {
    printf("Début du programme\n");
    
    // Lire l'état initial
    const char* filename = "C:\\Users\\SIMPLON\\Desktop\\logistic-network\\data\\network_state.json";
    printf("Tentative de lecture du fichier: %s\n", filename);
    
    NetworkState* state = readNetworkState(filename);
    if (!state) {
        printf("Erreur lors de la lecture du fichier JSON\n");
        return 1;
    }
    
    printf("Fichier JSON lu avec succès\n");
    
    // Afficher l'état initial
    printf("État initial:\n");
    printNetworkState(state);
    
    // Modifier quelques valeurs
    if (state->vehicle_count > 0) {
        state->vehicles[0].current_battery = 75.5;
        strcpy(state->vehicles[0].status, "en_route");
    }
    
    if (state->package_count > 0) {
        strcpy(state->packages[0].status, "en_livraison");
        state->packages[0].assigned_vehicle = 1;
    }
    
    state->stats.delivered_packages += 1;
    state->stats.network_efficiency = 95.5;
    
    // Écrire l'état modifié
    writeNetworkState("data/network_state_updated.json", state);
    
    // Libérer la mémoire
    freeNetworkState(state);
    
    printf("\nÉtat modifié écrit dans network_state_updated.json\n");
    return 0;
} 