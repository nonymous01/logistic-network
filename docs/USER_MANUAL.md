# Manuel Utilisateur - Logistic Network

## 1. Installation

### 1.1 Prérequis
- Windows 10 ou supérieur
- Visual Studio 2019 ou supérieur
- Git

### 1.2 Installation
1. Cloner le dépôt :
```bash
git clone https://github.com/votre-username/logistic-network.git
cd logistic-network
```

2. Compiler le projet :
```bash
make
```

3. Lancer le serveur :
```bash
./bin/logistic_network.exe
```

## 2. Utilisation

### 2.1 Interface Web
Accédez à l'interface web à l'adresse : `http://localhost:6060`

#### 2.1.1 Dashboard
- Visualisation du réseau en temps réel
- Statistiques des livraisons
- Contrôles des véhicules

#### 2.1.2 Panneau de Contrôle
- Ajout/Suppression de nœuds
- Configuration des véhicules
- Gestion des colis

### 2.2 Fonctionnalités

#### 2.2.1 Visualisation
- Zoom/Dézoom
- Pan
- Sélection d'éléments
- Filtres

#### 2.2.2 Gestion
- Création de routes
- Attribution de colis
- Optimisation automatique

## 3. Configuration

### 3.1 Fichier de Configuration
```json
{
    "server": {
        "port": 6060,
        "web_root": "./web",
        "data_path": "./data"
    },
    "network": {
        "update_interval": 1000,
        "optimization": true
    }
}
```

### 3.2 Options
- Port du serveur
- Intervalle de mise à jour
- Activation de l'optimisation

## 4. Dépannage

### 4.1 Problèmes Courants

#### 4.1.1 Serveur ne démarre pas
- Vérifier les ports utilisés
- Vérifier les permissions
- Vérifier les dépendances

#### 4.1.2 Interface ne se charge pas
- Vérifier la connexion
- Vérifier le navigateur
- Vérifier les logs

### 4.2 Logs
Les logs sont disponibles dans :
- Console du serveur
- Fichiers de log
- Console du navigateur

## 5. Sécurité

### 5.1 Bonnes Pratiques
- Changer le port par défaut
- Limiter les accès
- Sauvegarder régulièrement

### 5.2 Configuration Sécurisée
- HTTPS
- Authentification
- Autorisations

## 6. Support

### 6.1 Documentation
- Documentation technique
- API Reference
- FAQ

### 6.2 Contact
- GitHub Issues
- Forum 