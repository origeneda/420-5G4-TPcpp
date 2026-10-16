#include <sstream>
#include <algorithm>

#include "book.h"

using namespace std;

// Default constructor
Book::Book() : title(""), author(""), isbn(""), isAvailable(true), borrowerId("") {}

// Parameterized constructor
Book::Book(const string& title, const string& author, const string& isbn)
    : title(title), author(author), isbn(isbn), isAvailable(true), borrowerId("") {}

// Getters
string Book::getTitle() const { return title; }
string Book::getAuthor() const { return author; }
string Book::getISBN() const { return isbn; }
bool Book::getAvailability() const { return isAvailable; }
string Book::getBorrowerId() const { return borrowerId; }

// Setters
void Book::setTitle(const string& title) { this->title = title; }
void Book::setAuthor(const string& author) { this->author = author; }
void Book::setISBN(const string& isbn) { this->isbn = isbn; }
void Book::setAvailability(bool available) { this->isAvailable = available; }
void Book::setBorrowerId(const string& id) { this->borrowerId = id; }

// méthode checkOut
void Book::checkOut(const string& borrowerId) {
    if (isAvailable) {
        this->borrowerId = borrowerId;
        isAvailable = false;
    }
}

// méthode returnBook
void Book::returnBook(){
    if (!isAvailable){
        borrowerId="";
        isAvailable=true;
    }
}

// méthode toString
string Book::toString() const {
    string availability = isAvailable ? "Disponible" : "Emprunté par ID: " + borrowerId;
    string result =  "Titre: " + title + "\nAuteur: " + author + "\nISBN: " + isbn + "\nDisponibilité: " + availability;
    return result;
}

// méthode toFileFormat
string Book::toFileFormat() const {
    string availability = isAvailable ? "1" : "0";
    string result = title + "," + author + "," + isbn + "," + availability + "," + borrowerId;
    return result;
}

// méthode fromFileFormat
void Book::fromFileFormat(const string& line) {
    stringstream ss(line);
    string availability;
    getline(ss, title, ',');
    getline(ss, author, ',');
    getline(ss, isbn, ',');
    getline(ss, availability, ',');
    isAvailable = (availability == "1");
    getline(ss, borrowerId, ',');
}