#ifndef BOOKSTORE_2025_BOOK_H
#define BOOKSTORE_2025_BOOK_H

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include "map.h"

class BookData {
private:
    char ISBN[21];
    char BookName[61];
    char Author[61];
    char keywird[61];
    double Price;
    double TotalCost;
    long long Stock;

public:
    BookData();
    BookData(const std::string& isbn);
    BookData(const std::string& isbn, const std::string& name,
             const std::string& author, const std::string& keywords,
             double price, long long stock);

    // 获取信息
    std::string getISBN() const;
    std::string getBookName() const;
    std::string getAuthor() const;
    std::string getKeywords() const;
    double getPrice() const;
    long long getStock() const;

    // 设置信息
    void setISBN(const std::string& isbn);
    void setBookName(const std::string& name);
    void setAuthor(const std::string& author);
    void setKeywords(const std::string& keywords);
    void setPrice(double price);
    void setStock(long long stock);

    bool increaseStock(long long quantity);
    bool decreaseStock(long long quantity);

    bool isValid() const;
    bool hasKeyword(const std::string& keyword) const;

    std::string toString() const;

    bool operator<(const BookData& other) const {
        return strcmp(ISBN, other.ISBN) < 0;
    }
    bool operator==(const BookData& other) const {
        return strcmp(ISBN, other.ISBN) == 0;
    }

    // friend std::ostream operator<<(std::ostream& os, BookData& b) {
    //
    // }
};

class ISBNIndex {
private:
    char data[21] = {0};

public:
    ISBNIndex() { data[0] = '\0'; }

    ISBNIndex(const std::string& str) {
        strncpy(data, str.c_str(), 20);
        data[20] = '\0';
    }

    ISBNIndex(const char* str) {
        if (str) {
            strncpy(data, str, 20);
            data[20] = '\0';
        } else {
            data[0] = '\0';
        }
    }

    const char* c_str() const { return data; }
    std::string toString() const { return std::string(data); }

    bool operator<(const ISBNIndex& other) const {
        return strcmp(data, other.data) < 0;
    }

    bool operator==(const ISBNIndex& other) const {
        return strcmp(data, other.data) == 0;
    }

    bool operator!=(const ISBNIndex& other) const {
        return strcmp(data, other.data) != 0;
    }

    bool empty() const { return data[0] == '\0'; }

    friend std::ostream& operator<<(std::ostream& os, const ISBNIndex& idx) {
        os << idx.data;
        return os;
    }
};

class NameAuthorIndex {
private:
    char data[61] = {0};

public:
    NameAuthorIndex() { data[0] = '\0'; }

    NameAuthorIndex(const std::string& str) {
        strncpy(data, str.c_str(), 60);
        data[60] = '\0';
    }

    NameAuthorIndex(const char* str) {
        if (str) {
            strncpy(data, str, 60);
            data[60] = '\0';
        } else {
            data[0] = '\0';
        }
    }

    const char* c_str() const { return data; }
    std::string toString() const { return std::string(data); }

    bool operator<(const NameAuthorIndex& other) const {
        return strcmp(data, other.data) < 0;
    }

    bool operator==(const NameAuthorIndex& other) const {
        return strcmp(data, other.data) == 0;
    }

    bool operator!=(const NameAuthorIndex& other) const {
        return strcmp(data, other.data) != 0;
    }

    bool empty() const { return data[0] == '\0'; }

    friend std::ostream& operator<<(std::ostream& os, const NameAuthorIndex& idx) {
        os << idx.data;
        return os;
    }
};


class KeywordIndex {
private:
    char data[61] = {0};

public:
    KeywordIndex() { data[0] = '\0'; }

    KeywordIndex(const std::string& str) {
        strncpy(data, str.c_str(), 60);
        data[60] = '\0';
    }

    KeywordIndex(const char* str) {
        if (str) {
            strncpy(data, str, 60);
            data[60] = '\0';
        } else {
            data[0] = '\0';
        }
    }

    const char* c_str() const { return data; }
    std::string toString() const { return std::string(data); }

    bool operator<(const KeywordIndex& other) const {
        return strcmp(data, other.data) < 0;
    }

    bool operator==(const KeywordIndex& other) const {
        return strcmp(data, other.data) == 0;
    }

    bool operator!=(const KeywordIndex& other) const {
        return strcmp(data, other.data) != 0;
    }

    bool empty() const { return data[0] == '\0'; }

    friend std::ostream& operator<<(std::ostream& os, const KeywordIndex& idx) {
        os << idx.data;
        return os;
    }
};

#endif //BOOKSTORE_2025_BOOK_H