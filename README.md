# Landtales 2 - Roguelike RPG en 2D

**Groupe 6 :**  
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

## Technologies utilisées

- **SDL 2** : Pour la gestion des graphiques, des entrées utilisateur et de l'interface.
- **Fichiers** : Pour la gestion et la sauvegarde des données de progression, des salles et des configurations.

## Conclusion

**Landtales 2** se distingue par ses nombreuses fonctionnalités et modes de jeu variés, offrant une expérience complète à la fois pour les joueurs solo et multijoueur. Sa gestion de la progression, ses niveaux générés procéduralement, et son éditeur de niveaux garantissent une grande rejouabilité et une personnalisation poussée. Ce jeu est une aventure captivante pour ceux qui recherchent un défi dynamique, que ce soit en solo ou entre amis.
