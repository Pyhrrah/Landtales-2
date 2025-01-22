# Landtales 2 - Roguelike RPG en 2D

**Groupe 6 2A3:**  
Rémy THIBAUT  
Bryan MATHUREL  
Elias MATHURIN

## Présentation

**Landtales 2** est un jeu vidéo de type Roguelike RPG en 2D, développé en langage C avec la bibliothèque SDL 2. Le projet offre une expérience de jeu riche et variée grâce à trois fonctionnalités principales : le mode solo avec génération procédurale, l'éditeur de niveaux, et le mode multijoueur local.

## Fonctionnalités principales

### 1. Mode Solo avec Génération Procédurale
Le mode solo permet au joueur de plonger dans des salles générées de manière procédurale à chaque partie. Grâce à une seed unique, chaque session offre une expérience différente avec des ennemis variés et des niveaux à explorer. 

**Objectif :**  
- En cas de mort, le joueur ne perd pas toute sa progression.  
- Il revient à un point de spawn et peut utiliser les ressources acquises pour acheter des améliorations pour les parties suivantes, favorisant ainsi une progression dynamique et continue.

### 2. Sauvegarde
Toutes les données du joueur, telles que la progression, les améliorations et les salles générées, sont sauvegardées dans des fichiers. Cela permet une gestion flexible et légère des données, avec la possibilité de continuer à jouer là où le joueur s'était arrêté.

### 3. Mode Éditeur de Niveaux
Le mode éditeur permet aux utilisateurs de créer et personnaliser leurs propres salles. Grâce à une interface intuitive, les joueurs peuvent :
- Placer des objets et modifier les éléments du décor.
- Tester leurs créations avant de les sauvegarder.

Les salles créées peuvent être partagées avec d'autres joueurs pour diversifier l'expérience de jeu.

### 4. Gestion des Salles
Les utilisateurs peuvent éditer, modifier ou supprimer les salles à tout moment dans le menu de l'éditeur. Les salles sont stockées dans des fichiers, ce qui permet une gestion simple et rapide.

### 5. Mode Multijoueur Local
Le mode multijoueur local permet à deux joueurs de s'affronter en temps réel sur un même réseau local (ex. : dans une école). Un joueur agit comme hôte en créant un serveur auquel l'autre joueur peut se connecter en entrant l'IP et le port associés.

**Communication :**  
Une connexion client-serveur en C avec SDL 2 permet de gérer les échanges entre les joueurs et garantit une expérience compétitive.

**Expérience de jeu :**  
Les joueurs peuvent s'affronter dans des combats dynamiques et stratégiques, ajoutant une dimension compétitive à l'expérience.

## Utilisation du projet

### Prérequis
Avant de pouvoir utiliser **Landtales 2**, assurez-vous d'avoir installé les prérequis suivants :
- **SDL2** : La bibliothèque principale pour la gestion des graphiques et des entrées.
- **SDL2_ttf**, **SDL2_image**, **SDL2_net** : Extensions nécessaires pour gérer les polices, les images et les communications réseau.
- **GCC** : Le compilateur C utilisé pour construire le projet.
- **make** : L'outil permettant la compilation et le build du programme à partir de plusieurs fichiers de codes.

#### Installation des dépendances
Voici les commandes pour installer les dépendances sur **Ubuntu** et **Fedora**.

- **Pour Ubuntu :**
```bash
sudo apt update
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-net-dev libsdl2-mixer-dev build-essential libavformat-dev libavcodec-dev libavutil-dev libswresample-dev
sudo apt install make
```


#### Téléchargement du projet 

Pour utiliser le projet, il faut d'abord le télécharger depuis ce repository Github : 
```bash
git clone https://github.com/Pyhrrah/Landtales-2.git
cd Landtales-2
```

#### Compilation du programme

Vous pouvez ensuite lancer la commande **make** afin de compiler le projet (make clean peut être nécessaire une première fois avant de lancer make)

```bash
make clean
make 
```

Le programme Landtales 2 devrait être alors correctement compilé. 

Vous pouvez désormais le lancer avec la commande : 

```bash
./Landtales
```

#### Plugins 

Landtales 2 propose différents plugins pour customiser votre façon de jouer : 

- Un plugin pour s'amuser à générer une map comme bon nous semble. 

Pour modifier le fichier, rendez-vous dans 

```bash
cd src/plugins/map_editor.c
```

Pour compiler ce plugin, écrivez la commande suivante : 

```bash
gcc -shared -o plugins/map_editor.so -fPIC src/plugins/map_editor.c
```

- Un plugin pour ajouter de nouveaux bonus 

- Un plugin pour choisir personnaliser le template des sprite du jeu

**NB** : Pour des raisons de cohérences et de fonctionnement, nous partons du principe que vous suivrez les indications marquées dans chaque fichier de plugins. 


