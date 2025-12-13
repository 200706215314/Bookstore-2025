#include "../include/book.h"

BookData::BookData() :Price(0.0), Stock(0){
    memset(ISBN, 0, sizeof(ISBN));
    memset(BookName, 0, sizeof(BookName));
    memset(Author, 0, sizeof(Author));
    memset(Keywords, 0, sizeof(Keywords));
}

BookData::BookData(const std::string& isbn)  :Price(0.0), Stock(0){
    setISBN(isbn);
    memset(BookName, 0, sizeof(BookName));
    memset(Author, 0, sizeof(Author));
    memset(Keywords, 0, sizeof(Keywords));
}

BookData::BookData(const std::string& isbn, const std::string& name, const std::string& author, const std::string& keywords, double price, long long stock)
    :Price(price), Stock(stock){
    setISBN(isbn);
    setKeywords(keywords);
    setBookName(name);
    setAuthor(author);
}

std::string BookData::getISBN() const {
    return std::string(ISBN);
}

std::string BookData::getAuthor() const {
    return std::string(Author);
}

std::string BookData::getBookName() const {
    return std::string(BookName);
}

std::string BookData::getKeywords() const {
    return std::string(Keywords);
}

double BookData::getPrice() const {
    return Price;
}

long long BookData::getStock() const {
    return Stock;
}

void BookData::setISBN(const std::string& isbn) {
    strcpy(ISBN, isbn.c_str());
}
void BookData::setAuthor(const std::string& author) {
    strcpy(Author, author.c_str());
}
void BookData::setBookName(const std::string& name) {
    strcpy(BookName, name.c_str());
}
void BookData::setKeywords(const std::string& keywords) {
    strcpy(Keywords, keywords.c_str());
}
void BookData::setPrice(double price) {
    Price = price;
}
void BookData::setStock(long long stock) {
    Stock = stock;
}

bool BookData::increaseStock(long long quantity) {
    if (quantity <= 0) return false;
    Stock += quantity;
    return true;
}

bool BookData::decreaseStock(long long quantity) {
    if (quantity <= 0 || quantity > Stock) return false;
    Stock -= quantity;
    return true;
}

bool BookData::isValid() const {
    return strlen(ISBN) > 0;
}

std::vector<std::string> BookData::getAllKeywords() const {
    std::vector<std::string> res;
    std::string s = "";
    for (char c : Keywords) {
        if (c == '|') {
            res.push_back(s);
            s.clear();
        }
        else {
            s += c;
        }
    }
    if (!s.empty()) {
        res.push_back(s);
    }
    return res;
}

std::string BookData::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << getISBN() << "\t"
        << getBookName() << "\t"
        << getAuthor() << "\t"
        << getKeywords() << "\t"
        << Price << "\t"
        << Stock;
    return oss.str();
}




