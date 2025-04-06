# Définition de la variable CC pour spécifier le compilateur à utiliser.
# Dans ce cas, on utilise "gcc" (le compilateur C de GNU).
CC = gcc

# Définition des options de compilation dans la variable CFLAGS.
# -std=c99 : Utiliser le standard C99 (qui introduit des fonctionnalités modernes en C).
# -Wall : Activer tous les avertissements (pour attraper les erreurs possibles).
# -Wextra : Activer des avertissements supplémentaires pour plus de détails.
# -O2 : Optimisation du code pour de meilleures performances (niveau 2).
# -Iinclude : Ajouter le dossier "include" pour rechercher les fichiers d'en-tête.
CFLAGS = -std=c99 -Wall -Wextra -O2 -Iinclude

# Définition de la variable SRC contenant les fichiers source du projet.
# Ces fichiers sont en C et sont responsables de la logique de ton programme.
SRC = src/main.c src/graph.c

# Définition de la variable OBJ, qui remplace l'extension ".c" de chaque fichier source par ".o".
# Cela crée des fichiers objets à partir des fichiers source. Les fichiers objets sont des versions compilées des fichiers source.
OBJ = $(SRC:.c=.o)

# Définition de la variable EXEC, qui définit le nom de l'exécutable que nous allons générer.
EXEC = logistic_network

# Cette règle crée l'exécutable à partir des fichiers objets.
# Quand on tape "make", cette règle sera exécutée.
# $(EXEC) dépend des fichiers objets définis dans OBJ. 
# Le compilateur gcc est utilisé pour créer l'exécutable à partir des fichiers objets.
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

# Cette règle exécute l'exécutable généré après la compilation.
# Quand on tape "make run", l'exécutable sera lancé. Il correspond à "./logistic_network" sous Linux.
run: $(EXEC)
	./$(EXEC)

# Cette règle permet de nettoyer le projet en supprimant les fichiers générés pendant la compilation.
# Cela supprime les fichiers objets (.o), l'exécutable généré et les fichiers DOT et PNG (graphes générés).
# Quand on tape "make clean", ces fichiers seront supprimés.
clean:
	rm -f $(OBJ) $(EXEC) graph.dot graph.png
