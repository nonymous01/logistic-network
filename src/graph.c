#include "../include/project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// ====================
// Fonctions de base du graphe
// ====================

Graph* createGraph(int V) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    g->V = V;
    g->array = (AdjList*)calloc(V, sizeof(AdjList));
    
    // Initialisation des attributs des nœuds
    for (int i = 0; i < V; i++) {
        g->array[i].head = NULL;
        // Initialisation par défaut des attributs
        strcpy(g->array[i].attr.name, "Unnamed");
        g->array[i].attr.latitude = 0.0;
        g->array[i].attr.longitude = 0.0;
        g->array[i].attr.type = 0;
        g->array[i].attr.capacity = 0;
        g->array[i].attr.opening_time = 0;
        g->array[i].attr.closing_time = 1440; // 24h en minutes
    }
    
    return g;
}

AdjListNode* newAdjNode(int dest, EdgeAttr attr) {
    AdjListNode* node = (AdjListNode*)malloc(sizeof(AdjListNode));
    node->dest = dest;
    node->attr = attr;
    node->next = NULL;
    return node;
}

void addEdge(Graph* g, int src, int dest, EdgeAttr attr) {
    // Vérification des indices
    if (src >= g->V || dest >= g->V) {
        fprintf(stderr, "Erreur: Indice de sommet invalide\n");
        return;
    }
    
    // Création du nouveau nœud
    AdjListNode* newNode = newAdjNode(dest, attr);
    
    // Ajout en tête de liste
    newNode->next = g->array[src].head;
    g->array[src].head = newNode;
}

void removeEdge(Graph* g, int src, int dest) {
    if (src >= g->V || dest >= g->V) {
        fprintf(stderr, "Erreur: Indice de sommet invalide\n");
        return;
    }
    
    AdjListNode** current = &(g->array[src].head);
    while (*current) {
        if ((*current)->dest == dest) {
            AdjListNode* temp = *current;
            *current = (*current)->next;
            free(temp);
            return;
        }
        current = &((*current)->next);
    }
}

void printGraph(Graph* g) {
    for (int i = 0; i < g->V; i++) {
        printf("Sommet %d (%s):\n", i, g->array[i].attr.name);
        AdjListNode* current = g->array[i].head;
        while (current) {
            printf("  -> %d (%.1f km, %.1f h, %.1f €)\n", 
                   current->dest,
                   current->attr.distance,
                   current->attr.time,
                   current->attr.cost);
            current = current->next;
        }
    }
}

void freeGraph(Graph* g) {
    if (!g) return;
    
    for (int i = 0; i < g->V; i++) {
        AdjListNode* current = g->array[i].head;
        while (current) {
            AdjListNode* temp = current;
            current = current->next;
            free(temp);
        }
    }
    
    free(g->array);
    free(g);
}

// ====================
// Algorithmes fondamentaux
// ====================

void DFS(Graph* g, int start, int* visited) {
    visited[start] = 1;
    printf("Visite du sommet %d\n", start);
    
    AdjListNode* current = g->array[start].head;
    while (current) {
        if (!visited[current->dest]) {
            DFS(g, current->dest, visited);
        }
        current = current->next;
    }
}

void BFS(Graph* g, int start, int* visited) {
    int* queue = (int*)malloc(g->V * sizeof(int));
    int front = 0, rear = 0;
    
    visited[start] = 1;
    queue[rear++] = start;
    
    while (front < rear) {
        int current = queue[front++];
        printf("Visite du sommet %d\n", current);
        
        AdjListNode* node = g->array[current].head;
        while (node) {
            if (!visited[node->dest]) {
                visited[node->dest] = 1;
                queue[rear++] = node->dest;
            }
            node = node->next;
        }
    }
    
    free(queue);
}

int hasCycleUtil(Graph* g, int v, int* visited, int* recStack) {
    if (!visited[v]) {
        visited[v] = 1;
        recStack[v] = 1;
        
        AdjListNode* current = g->array[v].head;
        while (current) {
            if (!visited[current->dest] && hasCycleUtil(g, current->dest, visited, recStack))
                return 1;
            else if (recStack[current->dest])
                return 1;
            current = current->next;
        }
    }
    recStack[v] = 0;
    return 0;
}

int hasCycle(Graph* g) {
    int* visited = (int*)calloc(g->V, sizeof(int));
    int* recStack = (int*)calloc(g->V, sizeof(int));
    
    for (int i = 0; i < g->V; i++) {
        if (hasCycleUtil(g, i, visited, recStack)) {
            free(visited);
            free(recStack);
            return 1;
        }
    }
    
    free(visited);
    free(recStack);
    return 0;
}

int* findConnectedComponents(Graph* g) {
    int* components = (int*)malloc(g->V * sizeof(int));
    int* visited = (int*)calloc(g->V, sizeof(int));
    int component_id = 0;
    
    for (int i = 0; i < g->V; i++) {
        if (!visited[i]) {
            // Utilisation d'une pile pour DFS itératif
            int* stack = (int*)malloc(g->V * sizeof(int));
            int top = 0;
            stack[top++] = i;
            
            while (top > 0) {
                int v = stack[--top];
                if (!visited[v]) {
                    visited[v] = 1;
                    components[v] = component_id;
                    
                    AdjListNode* current = g->array[v].head;
                    while (current) {
                        if (!visited[current->dest]) {
                            stack[top++] = current->dest;
                        }
                        current = current->next;
                    }
                }
            }
            
            free(stack);
            component_id++;
        }
    }
    
    free(visited);
    return components;
}

// ====================
// Fonctions utilitaires
// ====================

float calculateRouteCost(Graph* g, int* route, int route_length) {
    float total_cost = 0.0;
    
    for (int i = 0; i < route_length - 1; i++) {
        AdjListNode* current = g->array[route[i]].head;
        while (current) {
            if (current->dest == route[i + 1]) {
                total_cost += current->attr.cost;
                break;
            }
            current = current->next;
        }
    }
    
    return total_cost;
}

int isRouteValid(Graph* g, int* route, int route_length, 
                Package* packages, int num_packages) {
    // Vérification de la connectivité
    for (int i = 0; i < route_length - 1; i++) {
        int found = 0;
        AdjListNode* current = g->array[route[i]].head;
        while (current) {
            if (current->dest == route[i + 1]) {
                found = 1;
                break;
            }
            current = current->next;
        }
        if (!found) return 0;
    }
    
    // Vérification des contraintes temporelles
    int current_time = 0;
    for (int i = 0; i < route_length; i++) {
        NodeAttr node = g->array[route[i]].attr;
        if (current_time < node.opening_time || current_time > node.closing_time) {
            return 0;
        }
        
        if (i < route_length - 1) {
            AdjListNode* edge = g->array[route[i]].head;
            while (edge && edge->dest != route[i + 1]) {
                edge = edge->next;
            }
            if (edge) {
                current_time += (int)(edge->attr.time * 60); // Conversion en minutes
            }
        }
    }
    
    return 1;
}

void updateEdgeAttributes(Graph* g, int time_of_day, int season) {
    // Mise à jour des attributs des arêtes en fonction de l'heure et de la saison
    for (int i = 0; i < g->V; i++) {
        AdjListNode* current = g->array[i].head;
        while (current) {
            // Facteurs de variation temporelle
            float time_factor = 1.0;
            if (time_of_day >= 7 && time_of_day <= 9) { // Heure de pointe matin
                time_factor = 1.5;
            } else if (time_of_day >= 17 && time_of_day <= 19) { // Heure de pointe soir
                time_factor = 1.3;
            }
            
            // Facteurs saisonniers
            float season_factor = 1.0;
            if (season == 0) { // Saison sèche
                season_factor = 0.9;
            } else if (season == 1) { // Saison des pluies
                season_factor = 1.2;
            }
            
            // Mise à jour des attributs
            current->attr.time *= time_factor * season_factor;
            current->attr.cost *= time_factor;
            current->attr.reliability *= season_factor;
            
            current = current->next;
        }
    }
} 