# Mapio
_Projet Programmation Système - L3 Info - Université de Bordeaux_

##Sujet
[Lien](http://dept-info.labri.fr/ENSEIGNEMENT/prs/feuilles-de-td/projet.pdf).

Deadline rapport : **Vendredi 6 Janvier - 12:00**

Soutenances : **Mardi 10 et Mercredi 11 Janvier**

##Répartition des tâches

###Première Partie : Sauvegarde et chargement des cartes
- ~~[E] `map_save()`~~
- ~~[A] `map_load()`~~
- ~~[E] `maputil.c` : Commandes `--get`~~
- ~~[E] `maputil.c` : Commandes `--setwidth` et `--setheight`~~ [Annulé]
- ~~[A] `maputil.c` : Commande `--pruneobjects`~~ [Annulé]
- ~~[A] `maputil.c` : Chargement complet du fichier sous forme de structures~~
- ~~[A] `maputil.c` : Commande `--pruneobjects` en utilisant `load_map_file()`~~
- ~~[E] `maputil.c` : Commandes `--setwidth` et `--setheight` en utilisant `load_map_file()`~~
- ~~[E] `maputil.c` : Prise en charge de plusieurs commandes en une seule exécution~~
- ~~[E] `maputil.c` : Commande `--setobjects`~~

###Deuxième Partie : Gestion des temporisateurs
- ~~[E] `tempo.c` : Mise en place du daemon~~

##TODOs
- `map_save()` et `map_load()` : utiliser des fonctions bas niveau plutôt que les `fopen()`, etc...
- ~~`maputil` : rendre possible l'utilisation de plusieurs commandes en une seule ligne~~ [Planifié]
- ~~`maputil` : vérifier le fichier utilisé avant de le manipuler~~ [Planifié]

##Checklist
Fonctionnalité                              | Difficulté | Etat
------------------------------------------- | --- | ---------------
Sauvegarde                                  | +   | **Terminé**
Chargement                                  | +   | **Terminé**
Informations élémentaires                   | +   | **Terminé**
Modification de la taille de la carte       | ++  | **Terminé**
Remplacement des objets d'une carte         | +++ | **Terminé**
Suppression des objets inutilisés           | ++  | **Terminé**
Réception des signaux par un thread daemon  | +   | **Terminé**
Implémentation simple                       | +   | *A faire*
Implémentation complète                     | +++ | *A faire*
Mise en service dans le jeu                 | ++  | *A faire*

##Membres
**Apodeus** : Romain Ordonez

**Echoffee** : Adrien Halnaut
 
