#include <stdio.h>   // Inclut la bibliothèque pour les entrées/sorties (comme printf)
#include <stdlib.h>  // Inclut la bibliothèque pour les fonctions utilitaires comme malloc (allocation de mémoire)
#include <float.h>   // Inclut la bibliothèque pour les constantes liées aux nombres à virgule flottante, comme FLT_MAX (valeur maximale pour un float)
#include "project.h" // Inclut le fichier d'en-tête de votre projet qui contient la définition des structures et des fonctions du graphe

// Fonction principale qui montre un exemple d'utilisation du graphe
void exampleUsage(void) {
    int V = 5;  // Déclare une variable pour le nombre de sommets dans le graphe, ici 5
    Graph* g = createGraph(V);  // Crée un graphe avec 5 sommets

    // Ajout d'arêtes entre les sommets du graphe
    addEdge(g, 0, 1, 10.5);  // Ajoute une arête entre le sommet 0 et le sommet 1 avec un poids de 10.5
    addEdge(g, 1, 2, 10.5);  // Ajoute une arête entre le sommet 1 et le sommet 2 avec un poids de 10.5
    addEdge(g, 2, 3, 10.5);  // Ajoute une arête entre le sommet 2 et le sommet 3 avec un poids de 10.5
    addEdge(g, 3, 4, 10.5);  // Ajoute une arête entre le sommet 3 et le sommet 4 avec un poids de 10.5
    addEdge(g, 4, 0, 10.5);  // Ajoute une arête entre le sommet 4 et le sommet 0 avec un poids de 10.5, formant un cycle

    printf("=== Exemple d'utilisation ===\n");  // Affiche un titre pour l'exemple d'utilisation
    printGraph(g);  // Affiche le contenu du graphe, c'est-à-dire les sommets et leurs arêtes

    // Vérification si le graphe contient un cycle et affichage du résultat
    printf("Cycle détecté: %s\n", detectCycle(g) ? "Oui" : "Non");

    // Allocation de mémoire pour une matrice 2D qui contiendra les distances entre les sommets
    float** dist = malloc(V * sizeof(float*));  // Alloue de la mémoire pour un tableau 2D de V lignes
    for (int i = 0; i < V; i++) dist[i] = malloc(V * sizeof(float));  // Alloue de la mémoire pour chaque ligne du tableau

    // Calcul des plus courts chemins entre tous les sommets avec l'algorithme de Floyd-Warshall
    floydWarshall(g, dist);

    // Affichage des distances calculées entre chaque paire de sommets
    printf("Plus courts chemins (matrice):\n");
    for (int i = 0; i < V; i++) {  // Parcours des lignes de la matrice
        for (int j = 0; j < V; j++) {  // Parcours des colonnes de la matrice
            if (dist[i][j] == FLT_MAX)  // Si la distance est infinie (pas de chemin direct entre les sommets)
                printf("INF ");  // Affiche "INF" pour indiquer qu'il n'y a pas de chemin entre les sommets
            else
                printf("%.1f ", dist[i][j]);  // Affiche la distance entre les sommets i et j
        }
        printf("\n");  // Retour à la ligne après avoir imprimé une ligne de la matrice
    }

    // Appel de la fonction qui affiche une solution pour le problème du voyageur de commerce (TSP)
    tsp(g);

    // Génération d'un fichier DOT représentant le graphe
    printf("Génération du fichier DOT en cours...\n");
    generateDot(g, "graph.dot");  // Génère un fichier DOT (format de description graphique)
    printf("Fichier DOT généré avec succès : graph.dot\n");  // Indique que le fichier DOT a été généré

    // Génération d'une image PNG à partir du fichier DOT avec Graphviz
    printf("Génération de l'image PNG...\n");
    system("dot -Tpng graph.dot -o graph.png");  // Exécute une commande système pour convertir le fichier DOT en une image PNG
    printf("Image générée : graph.png\n");  // Indique que l'image a été générée avec succès

    // Libération de la mémoire allouée pour la matrice des distances
    for (int i = 0; i < V; i++) free(dist[i]);  // Libère la mémoire pour chaque ligne de la matrice
    free(dist);  // Libère la mémoire allouée pour la matrice 2D

    // Libération de la mémoire allouée pour le graphe et ses arêtes
    freeGraph(g);
}

// Fonction principale qui lance l'exemple d'utilisation
int main(void) {
    exampleUsage();  // Appelle la fonction exampleUsage pour exécuter l'exemple de fonctionnement du graphe
    return 0;  // Retourne 0 pour indiquer que le programme s'est terminé sans erreur
}
