#include <iostream>
#include <limits>
#include <string>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <ctime>

#include "library.h"
#include "filemanager.h"

using namespace std;

void clearScreen() {
    system("cls || clear");
}

void pauseForInput() {
    cout << "\nAppuyez sur Entrée pour continuer...";
    cin.ignore();
    cin.get();
}

void displayMenu() {
    cout << "\n=== SYSTÈME DE GESTION DE BIBLIOTHÈQUE PERSONNELLE ===\n";
    cout << "1.  Ajouter un Livre\n";
    cout << "2.  Supprimer un Livre\n";
    cout << "3.  Rechercher des Livres par Titre\n";
    cout << "4.  Rechercher des Livres par Auteur\n";
    cout << "5.  Afficher Tous les Livres\n";
    cout << "6.  Afficher les Livres Disponibles\n";
    cout << "7.  Ajouter un Utilisateur\n";
    cout << "8.  Afficher Tous les Utilisateurs\n";
    cout << "9.  Emprunter un Livre\n";
    cout << "10. Retourner un Livre\n";
    cout << "11. Statistiques de la Bibliothèque\n";
    cout << "12. Sauvegarder les Données\n";
    cout << "13. Créer une Sauvegarde\n";
    cout << "0.  Quitter\n";
    cout << "======================================================\n";
    cout << "Entrez votre choix : ";
}

string getInput(const string& prompt) {
    string input;
    cout << prompt;
    getline(cin, input);
    return input;
}

// Retourne la date et l'heure courante au format AAAA-MM-JJ HH:MM:SS
string getCurrentDateTime() {
    time_t now = time(nullptr);
    tm localTime = *localtime(&now);
    ostringstream oss;
    oss << put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// Construit une ligne du journal : "AAAA-MM-JJ HH:MM:SS - [ACTION] details"
string makeLogLine(const string& action, const string& details) {
    return getCurrentDateTime() + " - [" + action + "] " + details;
}

// Détails d'un livre au format du fichier books.txt, avec le nom de l'emprunteur
string bookLogDetails(const Book& book, const string& borrowerName = "") {
    return book.getTitle() + "|" + book.getAuthor() + "|" + book.getISBN() + "|"
         + (book.getAvailability() ? "1" : "0") + "|" + borrowerName;
}

// Ajoute les lignes de la session à la fin du journal (créé s'il n'existe pas)
void saveJournal(const string& logFile, const vector<string>& journal) {
    ofstream file(logFile, ios::app);
    if (!file.is_open()) {
        cerr << "Erreur : Impossible d'écrire dans le journal " << logFile << ".\n";
        return;
    }
    for (const string& line : journal) {
        file << line << "\n";
    }
}

void printUsage(const char* programName) {
    cerr << "Usage: " << programName << " [--data-dir|-d <répertoire>]\n";
}

int main(int argc, char* argv[]) {
    string dataDir;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--data-dir" || arg == "-d") {
            if (i + 1 >= argc) {
                cerr << "Erreur : " << arg << " nécessite un répertoire.\n";
                printUsage(argv[0]);
                return 1;
            }
            dataDir = argv[++i];
        } else {
            cerr << "Erreur : argument inconnu '" << arg << "'.\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    string booksFile;
    string usersFile;
    if (!dataDir.empty()) {
        if (!filesystem::is_directory(dataDir)) {
            cerr << "Erreur : le répertoire " << dataDir << " n'existe pas.\n";
            printUsage(argv[0]);
            return 1;
        }
        booksFile = (filesystem::path(dataDir) / "books.txt").string();
        usersFile = (filesystem::path(dataDir) / "users.txt").string();
    }

    Library library;
    FileManager fileManager(booksFile, usersFile);

    // Journal d'activités : dans le répertoire de données s'il est fourni,
    // sinon dans le répertoire courant
    string logFile = dataDir.empty() ? "journal.log"
                                     : (filesystem::path(dataDir) / "journal.log").string();
    vector<string> journal; // lignes du journal de la session, écrites à la sortie
    
    // Load existing data
    if (!booksFile.empty() && !usersFile.empty()) {
        cout << "Chargement des données de la bibliothèque...\n";
        fileManager.loadLibraryData(library);
    } else {
        cout << "Aucun répertoire de données spécifié (--data-dir). Démarrage avec une bibliothèque vide.\n";
        cout << "Attention : les données ne seront pas sauvegardées.\n";
    }
    
    int choice;
    bool running = true;
    
    while (running) {
        displayMenu();
        
        if (!(cin >> choice)) {
            cout << "Saisie invalide. Veuillez entrer un nombre.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            pauseForInput();
            continue;
        }
        cin.ignore(); // Clear newline from buffer
        
        switch (choice) {
            case 1: { // Add Book
                string title = getInput("Entrez le titre du livre : ");
                string author = getInput("Entrez l'auteur du livre : ");
                string isbn = getInput("Entrez l'ISBN du livre : ");
                
                if (library.findBookByISBN(isbn)) {
                    cout << "Erreur : Un livre avec l'ISBN " << isbn << " existe déjà.\n";
                } else {
                    Book newBook(title, author, isbn);
                    library.addBook(newBook);
                    journal.push_back(makeLogLine("AJOUT LIVRE", bookLogDetails(newBook)));
                    cout << "Livre ajouté avec succès !\n";
                }
                pauseForInput();
                break;
            }
            
            case 2: { // Remove Book
                string isbn = getInput("Entrez l'ISBN du livre à supprimer : ");

                // On garde une copie du livre pour le journal avant de le supprimer
                Book* toRemove = library.findBookByISBN(isbn);
                Book removedBook = toRemove ? *toRemove : Book();

                if (library.removeBook(isbn)) {
                    journal.push_back(makeLogLine("SUPPRESSION LIVRE", bookLogDetails(removedBook)));
                    cout << "Livre supprimé avec succès !\n";
                } else {
                    cout << "Livre non trouvé.\n";
                }
                pauseForInput();
                break;
            }
            
            case 3: { // Search by Title
                string title = getInput("Entrez le titre à rechercher : ");
                auto results = library.searchBooksByTitle(title);
                
                if (results.empty()) {
                    cout << "Aucun livre trouvé avec ce titre.\n";
                } else {
                    cout << "\n=== RÉSULTATS DE RECHERCHE ===\n";
                    for (size_t i = 0; i < results.size(); ++i) {
                        cout << "\nRésultat " << (i + 1) << " :\n";
                        cout << results[i]->toString() << "\n";
                        cout << "-----------------------------\n";
                    }
                }
                pauseForInput();
                break;
            }
            
            case 4: { // Search by Author
                string author = getInput("Entrez l'auteur à rechercher : ");
                auto results = library.searchBooksByAuthor(author);
                
                if (results.empty()) {
                    cout << "Aucun livre trouvé de cet auteur.\n";
                } else {
                    cout << "\n=== RÉSULTATS DE RECHERCHE ===\n";
                    for (size_t i = 0; i < results.size(); ++i) {
                        cout << "\nRésultat " << (i + 1) << " :\n";
                        cout << results[i]->toString() << "\n";
                        cout << "-----------------------------\n";
                    }
                }
                pauseForInput();
                break;
            }
            
            case 5: // Display All Books
                library.displayAllBooks();
                pauseForInput();
                break;
            
            case 6: // Display Available Books
                library.displayAvailableBooks();
                pauseForInput();
                break;
            
            case 7: { // Add User
                string name = getInput("Entrez le nom de l'utilisateur : ");

                int maxNumero = 0;

                for (const auto& user : library.getAllUsers()) {
                    string id = user->getUserId();

                    // Garde que les ID au format "USR" + chiffres
                    if (id.size() > 3 && id.substr(0, 3) == "USR") {
                        try {
                            int numero = stoi(id.substr(3));
                            maxNumero = max(maxNumero, numero);
                        } catch (const exception&) {
                            // rien faire
                        }
                    }
                }

                ostringstream oss;
                oss << "USR" << setw(3) << setfill('0') << (maxNumero + 1);

                string userId = oss.str();

                if (library.findUserById(userId)) {
                    cout << "Erreur : Un utilisateur avec l'ID " << userId << " existe déjà.\n";
                } else {
                    User newUser(name, userId);
                    library.addUser(newUser);
                    journal.push_back(makeLogLine("AJOUT UTILISATEUR", name + "|" + userId));
                    cout << "Utilisateur ajouté avec succès ! ID : " << userId << "\n";
                }
                pauseForInput();
                break;
            }
            
            case 8: // Display All Users
                library.displayAllUsers();
                pauseForInput();
                break;
            
            case 9: { // Check Out Book
                string isbn = getInput("Entrez l'ISBN du livre à emprunter : ");
                string userId = getInput("Entrez l'ID de l'utilisateur : ");
                
                if (library.checkOutBook(isbn, userId)) {
                    journal.push_back(makeLogLine("EMPRUNT", bookLogDetails(*library.findBookByISBN(isbn),
                                                  library.findUserById(userId)->getName())));
                    cout << "Livre emprunté avec succès !\n";
                } else {
                    cout << "Erreur : Impossible d'emprunter le livre. Vérifiez l'ISBN, l'ID utilisateur et la disponibilité du livre.\n";
                }
                pauseForInput();
                break;
            }
            
            case 10: { // Return Book
                string isbn = getInput("Entrez l'ISBN du livre à retourner : ");

                // On récupère le nom de l'emprunteur avant le retour
                string borrowerName;
                Book* bookToReturn = library.findBookByISBN(isbn);
                if (bookToReturn) {
                    User* borrower = library.findUserById(bookToReturn->getBorrowerId());
                    if (borrower) borrowerName = borrower->getName();
                }

                if (library.returnBook(isbn)) {
                    journal.push_back(makeLogLine("RETOUR", bookLogDetails(*bookToReturn, borrowerName)));
                    cout << "Livre retourné avec succès !\n";
                } else {
                    cout << "Erreur : Impossible de retourner le livre. Vérifiez l'ISBN et que le livre est bien emprunté.\n";
                }
                pauseForInput();
                break;
            }
            
            case 11: { // Library Statistics
                cout << "\n=== STATISTIQUES DE LA BIBLIOTHÈQUE ===\n";
                cout << "Total des Livres : " << library.getTotalBooks() << "\n";
                cout << "Livres Disponibles : " << library.getAvailableBookCount() << "\n";
                cout << "Livres Empruntés : " << library.getCheckedOutBookCount() << "\n";
                cout << "Total des Utilisateurs : " << library.getAllUsers().size() << "\n";
                pauseForInput();
                break;
            }
            
            case 12: { // Save Data
                if (fileManager.saveLibraryData(library)) {
                    journal.push_back(makeLogLine("SAUVEGARDE", "Données sauvegardées"));
                    cout << "Données de la bibliothèque sauvegardées avec succès !\n";
                } else {
                    cout << "Erreur lors de la sauvegarde des données de la bibliothèque.\n";
                }
                pauseForInput();
                break;
            }
            
            case 13: { // Create Backup
                fileManager.createBackup();
                pauseForInput();
                break;
            }
            
            case 0: // Exit
                if (!booksFile.empty() && !usersFile.empty()) {
                    cout << "Sauvegarde des données avant la fermeture...\n";
                    if (fileManager.saveLibraryData(library)) {
                        journal.push_back(makeLogLine("SAUVEGARDE", "Sauvegarde automatique à la sortie"));
                    }
                } else {
                    cout << "Aucun répertoire de données spécifié (--data-dir) : les données ne sont pas sauvegardées.\n";
                }
                // Enregistrement du journal de la session
                saveJournal(logFile, journal);
                cout << "Merci d'avoir utilisé le Système de Gestion de Bibliothèque Personnelle !\n";
                running = false;
                break;
            
            default:
                cout << "Choix invalide. Veuillez réessayer.\n";
                pauseForInput();
                break;
        }
    }
    
    return 0;
}