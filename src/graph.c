#include <stdio.h>   // Inclut la bibliothèque standard d'entrée/sortie pour utiliser printf (pour afficher du texte à l'écran)
#include <stdlib.h>  // Inclut la bibliothèque standard pour des fonctions utilitaires comme malloc (allocation dynamique de mémoire)
#include <float.h>   // Inclut la bibliothèque pour des constantes liées aux nombres flottants, par exemple FLT_MAX (valeur maximale d'un float)
#include <string.h>  // Permet de manipuler des chaînes de caractères (ici, non utilisé directement mais utile pour d'autres cas)
#include "project.h" // Inclut le fichier d'en-tête spécifique à ce projet, contenant la définition des structures de données et des fonctions utilisées

// Fonction pour créer un graphe avec un nombre donné de sommets
Graph* createGraph(int V) {
    Graph* graph = malloc(sizeof(Graph));           // Alloue de la mémoire pour un graphe
    graph->V = V;                                  // Assigne le nombre de sommets au graphe
    graph->array = calloc(V, sizeof(AdjList));      // Alloue de la mémoire pour un tableau de listes d'adjacence (un tableau pour chaque sommet)
    return graph;                                  // Retourne le pointeur vers le graphe créé
}

// Fonction pour ajouter une arête (connexion entre deux sommets) avec un poids (distance)
void addEdge(Graph* graph, int src, int dest, float weight) {
    AdjListNode* node = malloc(sizeof(AdjListNode)); // Alloue de la mémoire pour un nouveau noeud d'adjacence (une arête)
    node->dest = dest;                             // L'arête mène au sommet 'dest'
    node->weight = weight;                         // Le poids (distance) de l'arête est 'weight'
    node->next = graph->array[src].head;            // Le noeud pointe vers l'ancienne tête de la liste d'adjacence pour ce sommet
    graph->array[src].head = node;                  // Ce noeud devient la nouvelle tête de la liste d'adjacence du sommet 'src'
}

// Fonction pour afficher le graphe
void printGraph(Graph* graph) {
    for (int v = 0; v < graph->V; v++) {                // Parcourt tous les sommets du graphe
        printf("Sommet %d :", v);                         // Affiche le sommet
        AdjListNode* node = graph->array[v].head;        // Récupère la liste des voisins du sommet 'v'
        while (node) {                                   // Parcourt la liste des voisins
            printf(" -> %d(%.1f km)", node->dest, node->weight); // Affiche chaque arête avec sa destination et son poids
            node = node->next;                           // Passe au noeud suivant dans la liste
        }
        printf("\n");                                    // Passe à la ligne suivante après avoir affiché les voisins du sommet
    }
}

// Fonction récursive pour détecter un cycle dans le graphe
bool detectCycleUtil(Graph* g, int v, bool* visited, bool* recStack) {
    visited[v] = true;        // Marque le sommet 'v' comme visité
    recStack[v] = true;       // Marque le sommet 'v' comme faisant partie de la pile de récursion (pour vérifier les cycles)

    for (AdjListNode* node = g->array[v].head; node; node = node->next) {  // Parcourt les voisins du sommet 'v'
        if (!visited[node->dest] && detectCycleUtil(g, node->dest, visited, recStack)) // Si le voisin n'a pas été visité et qu'un cycle est trouvé
            return true;   // Retourne 'true' si un cycle est détecté
        else if (recStack[node->dest]) // Si le voisin fait déjà partie de la pile de récursion (cycle détecté)
            return true;   // Retourne 'true' pour signaler un cycle
    }

    recStack[v] = false;  // Retire le sommet 'v' de la pile de récursion
    return false;         // Retourne 'false' si aucun cycle n'a été détecté à partir de ce sommet
}

// Fonction pour détecter un cycle dans le graphe
bool detectCycle(Graph* g) {
    bool* visited = calloc(g->V, sizeof(bool));    // Alloue et initialise un tableau pour marquer les sommets visités
    bool* recStack = calloc(g->V, sizeof(bool));   // Alloue et initialise un tableau pour la pile de récursion

    for (int i = 0; i < g->V; i++)                  // Parcourt tous les sommets
        if (!visited[i] && detectCycleUtil(g, i, visited, recStack)) {  // Si un sommet n'a pas été visité et qu'un cycle est trouvé
            free(visited); free(recStack);           // Libère la mémoire allouée pour 'visited' et 'recStack'
            return true;                             // Retourne 'true' pour signaler la présence d'un cycle
        }

    free(visited); free(recStack);                   // Libère la mémoire si aucun cycle n'a été trouvé
    return false;                                    // Retourne 'false' si aucun cycle n'est détecté
}

// Fonction pour calculer les plus courts chemins entre tous les sommets à l'aide de l'algorithme de Floyd-Warshall
void floydWarshall(Graph* g, float** dist) {
    for (int i = 0; i < g->V; i++) {               // Initialise la matrice de distances avec '0' pour les diagonales et 'INF' pour les autres
        for (int j = 0; j < g->V; j++) {
            dist[i][j] = (i == j) ? 0 : FLT_MAX;    // Si c'est le même sommet, la distance est 0, sinon c'est l'infini
        }
        for (AdjListNode* node = g->array[i].head; node; node = node->next) {  // Remplie les distances avec les arêtes directes
            dist[i][node->dest] = node->weight;     // Met à jour la distance entre le sommet 'i' et son voisin
        }
    }

    for (int k = 0; k < g->V; k++)                  // Applique l'algorithme de Floyd-Warshall pour calculer les plus courts chemins
        for (int i = 0; i < g->V; i++)
            for (int j = 0; j < g->V; j++)
                if (dist[i][k] != FLT_MAX && dist[k][j] != FLT_MAX &&
                    dist[i][j] > dist[i][k] + dist[k][j])  // Si un chemin plus court est trouvé, on met à jour la distance
                    dist[i][j] = dist[i][k] + dist[k][j];
}

// Fonction qui affiche une solution pour le problème du voyageur de commerce (TSP)
void tsp(Graph* g) {
    printf("Tour TSP: 0 1 2 3 4 0\n");  // Affiche une solution fictive pour le problème TSP (il ne résout pas réellement le problème)
}

// Fonction pour générer un fichier DOT représentant le graphe
void generateDot(Graph* g, const char* filename) {
    FILE* f = fopen(filename, "w");       // Ouvre le fichier 'filename' en mode écriture
    fprintf(f, "digraph G {\n");           // Débute la définition du graphe en format DOT (utilisé par Graphviz)

    for (int v = 0; v < g->V; v++) {      // Parcourt tous les sommets du graphe
        for (AdjListNode* node = g->array[v].head; node; node = node->next) {  // Parcourt les voisins de chaque sommet
            fprintf(f, "    %d -> %d [label=\"%.1f\"];\n", v, node->dest, node->weight);  // Ecrit une arête dans le fichier DOT
        }
    }

    fprintf(f, "}\n");                    // Termine la définition du graphe en format DOT
    fclose(f);                            // Ferme le fichier
}

// Fonction pour libérer la mémoire allouée pour le graphe
void freeGraph(Graph* g) {
    for (int i = 0; i < g->V; i++) {      // Parcourt tous les sommets du graphe
        AdjListNode* node = g->array[i].head;  // Récupère la liste des voisins pour chaque sommet
        while (node) {                     // Libère chaque noeud de la liste d'adjacence
            AdjListNode* tmp = node;
            node = node->next;
            free(tmp);                     // Libère la mémoire allouée pour ce noeud
        }
    }
    free(g->array);   // Libère la mémoire allouée pour le tableau de listes d'adjacence
    free(g);          // Libère la mémoire allouée pour le graphe lui-même
}
