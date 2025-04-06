#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "project.h"

// ===========================
// Implémentation des fonctions
// ===========================

Graph* createGraph(int V) {
    Graph* g = malloc(sizeof(Graph));
    g->V = V;
    g->array = calloc(V, sizeof(AdjList));
    return g;
}

AdjListNode* newAdjNode(int dest, EdgeAttr attr) {
    AdjListNode* node = malloc(sizeof(AdjListNode));
    node->dest = dest;
    node->attr = attr;
    node->next = NULL;
    return node;
}

void addEdge(Graph* g, int src, int dest, EdgeAttr attr) {
    AdjListNode* node = newAdjNode(dest, attr);
    node->next = g->array[src].head;
    g->array[src].head = node;
}

void removeEdge(Graph* g, int src, int dest) {
    AdjListNode** cur = &g->array[src].head;
    while (*cur) {
        if ((*cur)->dest == dest) {
            AdjListNode* tmp = *cur;
            *cur = tmp->next;
            free(tmp);
            return;
        }
        cur = &(*cur)->next;
    }
}

void printGraph(Graph* g) {
    for (int i = 0; i < g->V; i++) {
        printf("Sommet %d :", i);
        for (AdjListNode* p = g->array[i].head; p; p = p->next) {
            printf(" -> %d(%.1f km)", p->dest, p->attr.distance);
        }
        printf("\n");
    }
}

void freeGraph(Graph* g) {
    for (int i = 0; i < g->V; i++) {
        AdjListNode* p = g->array[i].head;
        while (p) {
            AdjListNode* tmp = p;
            p = p->next;
            free(tmp);
        }
    }
    free(g->array);
    free(g);
}

// --- Parcours DFS ---
void DFS_util(Graph* g, int v, bool* visited) {
    visited[v] = true;
    for (AdjListNode* p = g->array[v].head; p; p = p->next) {
        if (!visited[p->dest])
            DFS_util(g, p->dest, visited);
    }
}

void DFS(Graph* g, int start, bool* visited) {
    for (int i = 0; i < g->V; i++) visited[i] = false;
    DFS_util(g, start, visited);
}

// --- Parcours BFS ---
void BFS(Graph* g, int start, bool* visited) {
    for (int i = 0; i < g->V; i++) visited[i] = false;
    int* queue = malloc(g->V * sizeof(int));
    int front = 0, back = 0;
    visited[start] = true;
    queue[back++] = start;
    while (front < back) {
        int u = queue[front++];
        for (AdjListNode* p = g->array[u].head; p; p = p->next) {
            if (!visited[p->dest]) {
                visited[p->dest] = true;
                queue[back++] = p->dest;
            }
        }
    }
    free(queue);
}

// --- Détection de cycle (DFS) ---
bool detectCycleUtil(Graph* g, int v, bool* visited, bool* recStack) {
    visited[v] = recStack[v] = true;
    for (AdjListNode* p = g->array[v].head; p; p = p->next) {
        int w = p->dest;
        if (!visited[w] && detectCycleUtil(g, w, visited, recStack))
            return true;
        else if (recStack[w])
            return true;
    }
    recStack[v] = false;
    return false;
}

bool detectCycle(Graph* g) {
    bool* visited = calloc(g->V, sizeof(bool));
    bool* recStack = calloc(g->V, sizeof(bool));
    for (int i = 0; i < g->V; i++) {
        if (!visited[i] && detectCycleUtil(g, i, visited, recStack)) {
            free(visited); free(recStack);
            return true;
        }
    }
    free(visited); free(recStack);
    return false;
}

// --- Floyd–Warshall ---
void floydWarshall(Graph* g, double** dist) {
    int V = g->V;
    // Initialisation
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j) dist[i][j] = 0;
            else dist[i][j] = INFINITY;
        }
        for (AdjListNode* p = g->array[i].head; p; p = p->next) {
            dist[i][p->dest] = p->attr.distance;
        }
    }
    // Itérations
    for (int k = 0; k < V; k++)
        for (int i = 0; i < V; i++)
            for (int j = 0; j < V; j++)
                if (dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
}

// --- Bellman–Ford ---
bool bellmanFord(Graph* g, int src, double* dist) {
    int V = g->V;
    for (int i = 0; i < V; i++) dist[i] = INFINITY;
    dist[src] = 0;
    // Relaxation
    for (int i = 1; i < V; i++) {
        for (int u = 0; u < V; u++) {
            for (AdjListNode* p = g->array[u].head; p; p = p->next) {
                int v = p->dest;
                double w = p->attr.distance;
                if (dist[u] + w < dist[v])
                    dist[v] = dist[u] + w;
            }
        }
    }
    // Détection de cycle négatif
    for (int u = 0; u < V; u++) {
        for (AdjListNode* p = g->array[u].head; p; p = p->next) {
            int v = p->dest;
            double w = p->attr.distance;
            if (dist[u] + w < dist[v])
                return false;
        }
    }
    return true;
}

// --- TSP heuristique (Nearest Neighbor) ---
int* tspNearestNeighbor(Graph* g, int start) {
    int V = g->V;
    bool* visited = calloc(V, sizeof(bool));
    int* tour = malloc((V+1)*sizeof(int));
    tour[0] = start; visited[start] = true;
    for (int i = 1; i < V; i++) {
        int last = tour[i-1];
        int next = -1; double best = INFINITY;
        for (AdjListNode* p = g->array[last].head; p; p = p->next) {
            if (!visited[p->dest] && p->attr.distance < best) {
                best = p->attr.distance;
                next = p->dest;
            }
        }
        tour[i] = next; visited[next] = true;
    }
    tour[V] = start; // retour au point de départ
    free(visited);
    return tour;
}

// --- Fonctions stub pour glouton et génétique ---
void greedyAssignment() { /* À implémenter */ }
void geneticAlgorithm() { /* À implémenter */ }

// =====================
// Programme principal
// =====================
int main(int argc, char* argv[]) {
    exampleUsage();
    return 0;
}

void exampleUsage() {
    printf("=== Exemple d'utilisation ===\n");
    // Création d'un petit graphe
    int V = 5;
    Graph* g = createGraph(V);
    EdgeAttr e = {10.5, 15, 100, 0, 0.9, 0, {1.0,1.2,0.8}};
    addEdge(g, 0, 1, e);
    addEdge(g, 1, 2, e);
    addEdge(g, 2, 3, e);
    addEdge(g, 3, 4, e);
    addEdge(g, 4, 0, e);

    printGraph(g);

    bool hasCycle = detectCycle(g);
    printf("Cycle détecté: %s\n", hasCycle?"Oui":"Non");

    // Floyd–Warshall
    double** dist = malloc(V*sizeof(double*));
    for (int i=0;i<V;i++) dist[i]=malloc(V*sizeof(double));
    floydWarshall(g, dist);
    printf("Plus courts chemins (matrice):\n");
    for (int i=0;i<V;i++) {
        for (int j=0;j<V;j++) printf("%.1f ", dist[i][j]);
        printf("\n");
    }

    // TSP heuristique
    int* tour = tspNearestNeighbor(g, 0);
    printf("Tour TSP: ");
    for (int i=0;i<=V;i++) printf("%d ", tour[i]);
    printf("\n");

    // Libération mémoire
    for(int i=0;i<V;i++) free(dist[i]); free(dist);
    free(tour);
    freeGraph(g);
}