# msh

## Les difficultés auxquelles nous avons été confrontés et comment nous les avons résolues

### 1. Implémentation des commandes internes

La première difficulté rencontrée était liée aux commandes internes du shell, notamment `cd`, `exit` et `help`.

Au départ, nous avions pensé utiliser `system()` pour exécuter toutes les commandes. Cependant, cette solution ne fonctionne pas correctement pour certaines commandes internes. Par exemple, si la commande `cd` est exécutée avec `system()`, elle modifie uniquement le répertoire courant du processus fils créé par `system()`, mais pas celui du shell principal.

Cela signifie que le changement de répertoire disparaît dès que le processus fils se termine.

Pour résoudre ce problème, nous avons implémenté les commandes internes directement dans notre programme, sans passer par `system()`. Les commandes `cd`, `exit`, `help` et `pwd` sont donc traitées dans le processus principal du shell.

---

### 2. Problème de segmentation fault avec la commande `clear`

Une autre difficulté rencontrée était l’apparition d’une erreur de segmentation lors de l’exécution de la commande `clear`.

Ce problème venait principalement de la manière dont les arguments étaient préparés avant d’être envoyés à `execvp`. Pour que `execvp` fonctionne correctement, le tableau d’arguments doit obligatoirement se terminer par une valeur `NULL`. Si ce `NULL` est absent ou mal placé, `execvp` peut lire au-delà du tableau, ce qui peut provoquer un comportement indéfini ou une erreur de segmentation.

Pour résoudre ce problème, nous avons corrigé la fonction de découpage de la commande afin que le tableau `args` se termine toujours par `NULL`. Ainsi, la commande `clear`, comme les autres commandes externes, peut être exécutée correctement avec `fork` et `execvp`.

---

### 3. Gestion correcte de la commande `cd`

La commande `cd` a posé une difficulté particulière, car elle doit modifier le répertoire courant du shell principal.

Dans une première version, lorsque l’utilisateur tapait simplement `cd` sans argument, le programme changeait le répertoire vers `/`. Ce comportement n’est pas celui d’un shell classique, car normalement `cd` sans argument ramène l’utilisateur vers son répertoire personnel.

Pour corriger cela, nous avons utilisé la variable d’environnement `HOME`. Ainsi, lorsque l’utilisateur tape `cd` sans argument, le shell se déplace automatiquement vers le dossier personnel de l’utilisateur.

---

### 4. Séparation des commandes internes et externes

Une autre difficulté était de bien distinguer les commandes internes des commandes externes.

Les commandes internes comme `cd`, `exit`, `help` et `pwd` doivent être exécutées directement par le shell. En revanche, les commandes externes comme `ls`, `cat`, `echo`, `mkdir` ou `clear` doivent être exécutées dans un processus fils.

Pour résoudre cela, nous avons ajouté une structure conditionnelle qui vérifie d’abord si la commande entrée correspond à une commande interne. Si c’est le cas, elle est exécutée directement. Sinon, elle est envoyée à la fonction responsable des commandes externes.

---

### 5. Exécution des commandes externes avec `fork` et `execvp`

Le shell devait permettre l’exécution des commandes externes sans arrêter le programme principal.

La difficulté était donc de lancer une commande externe tout en gardant le shell actif après son exécution.

Pour cela, nous avons utilisé `fork` pour créer un processus fils. Dans ce processus fils, la commande est exécutée avec `execvp`. Pendant ce temps, le processus parent attend la fin du fils avec `waitpid`. Cela permet au shell de reprendre ensuite son fonctionnement normal.

---

### 6. Gestion de la mémoire lors du découpage des commandes

Dans la première version, les arguments étaient stockés avec `malloc`, `realloc` et `strdup`. Cette méthode fonctionnait, mais elle rendait le code plus complexe, car il fallait ensuite libérer manuellement toute la mémoire utilisée.

Cette gestion manuelle augmentait aussi le risque d’erreurs, comme les fuites mémoire ou les accès invalides. C, fidèle à lui-même, transforme vite une simple commande `ls` en mini-crise existentielle.

Pour simplifier le programme, nous avons utilisé directement les pointeurs retournés par `strtok`. Les arguments pointent vers les différentes parties de la chaîne originale entrée par l’utilisateur. Cela évite les allocations inutiles et rend le code plus simple.

---

### 7. Gestion des commandes vides

Une autre difficulté concernait les commandes vides. Si l’utilisateur appuie simplement sur Entrée, le shell ne doit pas essayer d’exécuter une commande inexistante.

Pour résoudre ce problème, nous avons ajouté une vérification après le découpage de la commande. Si aucun argument n’est détecté, la fonction retourne immédiatement sans exécuter de commande.

---

### 8. Vérification du nombre d’arguments

Certaines commandes internes doivent respecter un nombre précis d’arguments.

Par exemple, `exit`, `help` et `pwd` ne prennent pas d’arguments dans notre version. La commande `cd`, quant à elle, accepte au maximum un seul argument représentant le chemin du répertoire cible.

Pour éviter les comportements incorrects, nous avons ajouté des vérifications avant l’exécution de chaque commande interne. Si l’utilisateur entre trop d’arguments, un message d’erreur clair est affiché.

---

### 9. Amélioration des messages d’erreur

Dans la première version, certaines erreurs étaient affichées avec des messages simples comme `fork failed` ou `execvp failed`. Ces messages indiquaient qu’une erreur avait eu lieu, mais ils ne donnaient pas la cause exacte.

Pour améliorer cela, nous avons utilisé la fonction `perror`. Cette fonction affiche un message plus précis basé sur l’erreur retournée par le système.

Cela rend le programme plus facile à tester, à comprendre et à corriger.

---

### 10. Gestion de la fin d’entrée avec `ctrl+d`

Lorsque l’utilisateur appuie sur `ctrl+d`, la fonction `fgets` retourne `NULL`. Sans gestion correcte de ce cas, le shell peut afficher un message d’erreur ou continuer de manière incorrecte.

Pour résoudre ce problème, nous avons vérifié le retour de `fgets`. Si la fonction retourne `NULL`, le shell affiche un message de sortie puis quitte proprement la boucle principale.