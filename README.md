# Mapio
_Projet Programmation Système - L3 Info - Université de Bordeaux_

##Sujet
[Lien](http://dept-info.labri.fr/ENSEIGNEMENT/prs/feuilles-de-td/projet.pdf).

Deadline rapport : **Vendredi 6 Janvier - 12:00**

Soutenances : **Mardi 10 et Mercredi 11 Janvier**

##Répartition des tâches

###Première Partie : Sauvegarde et chargement des cartes
- ~~[E] `map_save()` *Difficulté : +*~~
- ~~[A] `map_load()` *Difficulté : +*~~
- ~~[E] `maputil.c` : Commandes `--get` *Difficulté : +*~~
- ~~[E] `maputil.c` : Commandes `--setwidth` et `--setheight` *Difficulté : ++*~~ [Annulé]
- ~~[A] `maputil.c` : Commande `--pruneobjects` *Difficulté : ++*~~ [Annulé]
- [A] `maputil.c` : Chargement complet du fichier sous forme de structures
- [E] `maputil.c` : Prise en charge de plusieurs commandes en une seule exécution
- [?] `maputil.c` : Commande `--setobjects` *Difficulté : +++*

###Deuxième Partie : Gestion des temporisateurs
*A venir*

##TODOs
- `map_save()` et `map_load()` : utiliser des fonctions bas niveau plutôt que les `fopen()`, etc...
- ~~`maputil` : rendre possible l'utilisation de plusieurs commandes en une seule ligne~~ [Planifié]
- ~~`maputil` : vérifier le fichier utilisé avant de le manipuler~~ [Planifié]

##Checklist
Fonctionnalité                              | Difficulté | Etat
------------------------------------------- | --- | ---------------
Sauvegarde                                  | +   | **Terminé**
Chargement                                  | +   | **Terminé**
Informations élémentaires                   | +   | Presque terminé
Modification de la taille de la carte       | ++  | A optimiser
Remplacement des objets d'une carte         | +++ | *A faire*
Suppression des objets inutilisés           | ++  | A optimiser
Réception des signaux par un thread daemon  | +   | *A faire*
Implémentation simple                       | +   | *A faire*
Implémentation complète                     | +++ | *A faire*
Mise en service dans le jeu                 | ++  | *A faire*

##Membres
**Apodeus** : Romain Ordonez

**Echoffee** : Adrien Halnaut
