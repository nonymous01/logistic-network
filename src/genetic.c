#include "../include/project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <math.h>

// ====================
// Fonctions auxiliaires pour l'algorithme génétique
// ====================

float calculateFitness(Graph* g, int* route, int route_length,
                      Package* packages, int num_packages) {
    float total_cost = 0.0;
    float total_time = 0.0;
    float reliability = 1.0;
    int missed_windows = 0;
    
    // Calcul des coûts et temps
    for (int i = 0; i < route_length - 1; i++) {
        AdjListNode* current = g->array[route[i]].head;
        while (current) {
            if (current->dest == route[i + 1]) {
                total_cost += current->attr.cost;
                total_time += current->attr.time;
                reliability *= current->attr.reliability;
                break;
            }
            current = current->next;
        }
    }
    
    // Vérification des fenêtres de livraison
    int current_time = 0;
    for (int i = 0; i < route_length; i++) {
        for (int j = 0; j < num_packages; j++) {
            if (packages[j].destination == route[i]) {
                if (current_time < packages[j].delivery_window_start ||
                    current_time > packages[j].delivery_window_end) {
                    missed_windows++;
                }
            }
        }
        
        if (i < route_length - 1) {
            AdjListNode* edge = g->array[route[i]].head;
            while (edge && edge->dest != route[i + 1]) {
                edge = edge->next;
            }
            if (edge) {
                current_time += (int)(edge->attr.time * 60);
            }
        }
    }
    
    // Calcul du score de fitness
    float fitness = 1.0 / (total_cost + total_time * 10.0);
    fitness *= reliability;
    fitness /= (1.0 + missed_windows);
    
    return fitness;
}

void mutateRoute(int* route, int route_length) {
    // Mutation par échange de deux points
    int pos1 = rand() % route_length;
    int pos2 = rand() % route_length;
    
    int temp = route[pos1];
    route[pos1] = route[pos2];
    route[pos2] = temp;
}

void crossoverRoutes(int* parent1, int* parent2, int* child, int route_length) {
    // Croisement par sélection aléatoire
    for (int i = 0; i < route_length; i++) {
        child[i] = (rand() % 2) ? parent1[i] : parent2[i];
    }
}

// ====================
// Algorithme génétique principal
// ====================

void geneticAlgorithm(Graph* g, Package* packages, int num_packages,
                     Vehicle* vehicles, int num_vehicles, int population_size,
                     int generations) {
    // Initialisation de la population
    Chromosome* population = (Chromosome*)malloc(population_size * sizeof(Chromosome));
    Chromosome* new_population = (Chromosome*)malloc(population_size * sizeof(Chromosome));
    
    // Initialisation aléatoire de la population
    for (int i = 0; i < population_size; i++) {
        population[i].route = (int*)malloc(g->V * sizeof(int));
        new_population[i].route = (int*)malloc(g->V * sizeof(int));
        
        // Création d'une route aléatoire
        for (int j = 0; j < g->V; j++) {
            population[i].route[j] = j;
        }
        
        // Mélange aléatoire
        for (int j = g->V - 1; j > 0; j--) {
            int k = rand() % (j + 1);
            int temp = population[i].route[j];
            population[i].route[j] = population[i].route[k];
            population[i].route[k] = temp;
        }
        
        // Calcul du fitness initial
        population[i].fitness = calculateFitness(g, population[i].route, g->V,
                                              packages, num_packages);
    }
    
    // Évolution sur plusieurs générations
    for (int gen = 0; gen < generations; gen++) {
        // Sélection des parents par tournoi
        for (int i = 0; i < population_size; i++) {
            // Sélection de deux parents aléatoires
            int parent1 = rand() % population_size;
            int parent2 = rand() % population_size;
            
            // Croisement
            crossoverRoutes(population[parent1].route, population[parent2].route,
                          new_population[i].route, g->V);
            
            // Mutation avec une probabilité de 10%
            if ((float)rand() / RAND_MAX < 0.1) {
                mutateRoute(new_population[i].route, g->V);
            }
            
            // Calcul du fitness du nouvel individu
            new_population[i].fitness = calculateFitness(g, new_population[i].route, g->V,
                                                      packages, num_packages);
        }
        
        // Remplacement de la population
        for (int i = 0; i < population_size; i++) {
            if (new_population[i].fitness > population[i].fitness) {
                int* temp_route = population[i].route;
                population[i].route = new_population[i].route;
                new_population[i].route = temp_route;
                population[i].fitness = new_population[i].fitness;
            }
        }
        
        // Affichage de la meilleure solution
        float best_fitness = 0.0;
        int best_index = 0;
        
        for (int i = 0; i < population_size; i++) {
            if (population[i].fitness > best_fitness) {
                best_fitness = population[i].fitness;
                best_index = i;
            }
        }
        
        printf("Génération %d: Meilleur fitness = %f\n", gen + 1, best_fitness);
    }
    
    // Sélection de la meilleure solution
    float best_fitness = 0.0;
    int best_index = 0;
    
    for (int i = 0; i < population_size; i++) {
        if (population[i].fitness > best_fitness) {
            best_fitness = population[i].fitness;
            best_index = i;
        }
    }
    
    // Application de la meilleure solution aux véhicules
    int current_vehicle = 0;
    int packages_assigned = 0;
    
    while (packages_assigned < num_packages && current_vehicle < num_vehicles) {
        // Attribution des colis au véhicule courant
        int start_index = packages_assigned;
        int end_index = MIN(packages_assigned + vehicles[current_vehicle].capacity,
                          num_packages);
        
        // Mise à jour de la position du véhicule
        vehicles[current_vehicle].current_location = 
            packages[end_index - 1].destination;
        
        packages_assigned = end_index;
        current_vehicle++;
    }
    
    // Libération de la mémoire
    for (int i = 0; i < population_size; i++) {
        free(population[i].route);
        free(new_population[i].route);
    }
    free(population);
    free(new_population);
} 