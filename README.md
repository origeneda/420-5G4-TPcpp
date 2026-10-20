# TPcpp
par Dan Endy Origene

Pour Interface et Expérience Utilisateur, j'ai choisi la fonctionalité de numérotation automatique des ID utilisateurs.

Pour Gestion des Données, j'ai choisi la fonctionalité de tri des résultats par titre, auteur pour l’affichage (utilisation de la fonction de tri de la STL).

Question 1
La notion est celle du unique_ptr qui est un pointeur intelligent (en-tête <memory>) et qui est l'unique propriétaire de l'objet alloué sur le tas. Quand le unique_ptr est détruit (par exemple quand on fait books.erase(it) ou quand la Library est détruite), il appelle automatiquement delete : on n'a jamais besoin d'écrire delete, et on évite les fuites de mémoire. C'est le principe du RAII (Resource Acquisition Is Initialization). Un unique_ptr ne peut pas être copié, seulement déplacé (std::move), ce qui garantit qu'il n'y a qu'un seul propriétaire. make_unique<Book>(book) crée l'objet et le pointeur en une seule étape. Le reste du programme reçoit des pointeurs bruts Book* (via .get()), qui permettent d'utiliser l'objet sans en être propriétaire : c'est pour ça que getAllBooks() retourne un vector<Book*>. Voici l'exemple d'utilisation dans le TP:
auto it = find_if(books.begin(), books.end(),
    [&isbn](const unique_ptr<Book>& book) {
        return book->getISBN() == isbn;
    });

Question 2 
Je stockerais les données dans une base PostgreSQL, avec trois tables principales : livres, usagers et emprunts. J’ajouterais des index sur l’ISBN, le titre et l’auteur pour accélérer les recherches, et j’utiliserais des transactions pour empêcher qu’un même livre soit emprunté deux fois.Le C++ garderait quand même sa place pour les traitements lourds comme l’import en lot, les statistiques ou la déduplication. Il pourrait se connecter directement à PostgreSQL grâce à la bibliothèque libpqxx. Concrètement, la classe FileManager serait remplacée par une classe DatabaseManager qui garderait les mêmes méthodes (loadLibraryData, saveLibraryData), mais qui exécuterait des requêtes SQL au lieu de lire et d’écrire des fichiers. Une autre piste serait d’exposer le code C++ existant à Python avec pybind11 : l’API web pourrait alors appeler directement les classes Library et Book.

# Construire le projet
Vous pouvez utiliser un dev container de base C++ de VScode.
Le projet utilise cmake, pensez à l'inclure dans votre dev container.

Voici les lignes de commandes pour compiler le projet:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

# Répertoire data

Il contient 2 fichiers `books.txt`et `users.txt` que vous pouvez utilisez pour tester votre code.
Pour ca il suffit de donner chemin vers le repertoire data avec l'application `bibliotheque -d <chemin verrs data>`