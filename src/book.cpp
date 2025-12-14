#include "../include/book.h"
#include "../include/parser.h"
#include "../include/token.h"
#include <unordered_set>

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

std::ostream& operator<<(std::ostream& os, const BookData& book) {
    os << book.toString();
    return os;
}

bool BookData::hasKeyword(const std::string& keyword) const {
    if (keyword.empty()) return false;

    std::string keywordsStr = getKeywords();
    if (keywordsStr.empty()) return false;

    size_t start = 0;
    size_t end = keywordsStr.find('|');

    while (true) {
        std::string kw;
        if (end == std::string::npos) {
            kw = keywordsStr.substr(start);
        } else {
            kw = keywordsStr.substr(start, end - start);
        }

        if (kw == keyword) return true;

        if (end == std::string::npos) break;
        start = end + 1;
        end = keywordsStr.find('|', start);
    }

    return false;
}


BookSystem::BookSystem(const std::string& baseFileName)
    : isbnMap(baseFileName + "_isbn"),
      nameIndex(baseFileName + "_name"),
      authorIndex(baseFileName + "_author"),
      keywordIndex(baseFileName + "_keyword") ,
      financeSystem(baseFileName) {};

bool BookSystem::isValidISBNStr(const std::string& isbn) {
    if (isbn.empty() || isbn.length() > 20) return false;
    for (char c : isbn) {
        if (c < 32 || c > 126) return false; // 不可见字符
    }
    return true;
}

bool BookSystem::isValidBookNameStr(const std::string& name) {
    if (name.empty() || name.length() > 60) return false;
    for (const char c : name) {
        if (c < 32 || c > 126 || c == '\"') return false;
    }
    return true;
}

bool BookSystem::isValidAuthorStr(const std::string& author) {
    if (author.empty() || author.length() > 60) return false;
    for (char c : author) {
        if (c < 32 || c > 126 || c == '\"') return false;
    }
    return true;
}

bool BookSystem::isValidKeywordsStr(const std::string& keywords) const {    //这里传入的关键词格式是 "[Keyword]"
    if (keywords.empty() || keywords.length() > 60) return false;

    if (keywords.find('\"') != std::string::npos) return false;

    std::vector<std::string> kwList = splitKeywords(keywords);
    if (kwList.empty()) return false;

    for (const auto& kw : kwList) {
        if (kw.empty()) return false;
    }

    std::sort(kwList.begin(), kwList.end());
    for (size_t i = 1; i < kwList.size(); i++) {
        if (kwList[i] == kwList[i-1]) return false;
    }
    return true;
}

bool BookSystem::isValidPriceStr(const std::string& priceStr) {
    if (priceStr.empty() || priceStr.length() > 13) return false;

    int dotCount = 0;
    bool hasDigit = false;

    for (char c : priceStr) {
        if (c == '.') {
            dotCount++;
            if (dotCount > 1) return false;
        } else if (isdigit(c)) {
            hasDigit = true;
        } else {
            return false;
        }
    }

    if (!hasDigit) return false;

    const size_t dotPos = priceStr.find('.');
    if (dotPos != std::string::npos) {
        if (priceStr.length() - dotPos - 1 > 2) return false;
    }

    try {
        double price = std::stod(priceStr);
        if (price < 0) return false;
    } catch (...) {
        return false;
    }

    return true;
}

bool BookSystem::isValidQuantityStr(const std::string& quantityStr) const {
    if (quantityStr.empty() || quantityStr.length() > 10) return false;

    for (char c : quantityStr) {
        if (!isdigit(c)) return false;
    }

    try {
        long long qty = std::stoll(quantityStr);
        return qty > 0 && qty <= 2147483647LL;
    } catch (...) {
        return false;
    }
}


std::vector<std::string> BookSystem::splitKeywords(const std::string& keywords) const {    //要求传入不带“” 的keyword
    std::vector<std::string> result;
    if (keywords.empty()) return result;

    size_t start = 0;
    size_t end = keywords.find('|');

    while (true) {
        std::string kw;
        if (end == std::string::npos) {
            kw = keywords.substr(start);
        } else {
            kw = keywords.substr(start, end - start);
        }

        if (!kw.empty()) {
            result.push_back(kw);
        }

        if (end == std::string::npos) break;
        start = end + 1;
        end = keywords.find('|', start);
    }

    return result;
}

void BookSystem::addToIndices(const BookData& book) {
    const ISBNIndex isbn(book.getISBN());
    const NameAuthorIndex name(book.getBookName());
    NameAuthorIndex author(book.getAuthor());
    std::vector<std::string> keywords = book.getAllKeywords();

    if (!name.empty()) {
        nameIndex.insert(name, isbn);
    }

    if (!author.empty()) {
        authorIndex.insert(author, isbn);
    }

    for (const auto& keyword : keywords) {
        if (!keyword.empty()) {
            KeywordIndex kwIndex(keyword);
            keywordIndex.insert(kwIndex, isbn);
        }
    }
}

void BookSystem::removeFromIndices(const BookData& book) {
    ISBNIndex isbn(book.getISBN());
    NameAuthorIndex name(book.getBookName());
    NameAuthorIndex author(book.getAuthor());
    std::vector<std::string> keywords = book.getAllKeywords();

    if (!name.empty()) {
        std::vector<ISBNIndex> isbns = nameIndex.find(name);
        for (const auto& storedIsbn : isbns) {
            if (storedIsbn == isbn) {
                nameIndex.remove(name, storedIsbn);
            }
        }
    }

    if (!author.empty()) {
        std::vector<ISBNIndex> isbns = authorIndex.find(author);
        for (const auto& storedIsbn : isbns) {
            if (storedIsbn == isbn) {
                authorIndex.remove(author, storedIsbn);
            }
        }
    }

    for (const auto& keyword : keywords) {
        if (!keyword.empty()) {
            KeywordIndex kwIndex(keyword);
            std::vector<ISBNIndex> isbns = keywordIndex.find(kwIndex);
            for (const auto& storedIsbn : isbns) {
                if (storedIsbn == isbn) {
                    keywordIndex.remove(kwIndex, storedIsbn);
                }
            }
        }
    }
}

void BookSystem::updateIndices(const BookData& oldBook, const BookData& newBook) {
ISBNIndex isbn(oldBook.getISBN());
    ISBNIndex newIsbn(newBook.getISBN());

    // ISBN改变
    if (isbn != newIsbn) {
        removeFromIndices(oldBook);
        addToIndices(newBook);
        return;
    }

    if (oldBook.getBookName() != newBook.getBookName()) {
        NameAuthorIndex oldName(oldBook.getBookName());
        NameAuthorIndex newName(newBook.getBookName());

        if (!oldName.empty()) {
            std::vector<ISBNIndex> isbns = nameIndex.find(oldName);
            for (const auto& storedIsbn : isbns) {
                if (storedIsbn == isbn) {
                    nameIndex.remove(oldName, storedIsbn);
                    break;
                }
            }
        }
        if (!newName.empty()) {
            nameIndex.insert(newName, isbn);
        }
    }

    // 作者改变
    if (oldBook.getAuthor() != newBook.getAuthor()) {
        NameAuthorIndex oldAuthor(oldBook.getAuthor());
        NameAuthorIndex newAuthor(newBook.getAuthor());

        if (!oldAuthor.empty()) {
            std::vector<ISBNIndex> isbns = authorIndex.find(oldAuthor);
            for (const auto& storedIsbn : isbns) {
                if (storedIsbn == isbn) {
                    authorIndex.remove(oldAuthor, storedIsbn);
                    break;
                }
            }
        }

        if (!newAuthor.empty()) {
            authorIndex.insert(newAuthor, isbn);
        }
    }

    std::vector<std::string> oldKeywords = oldBook.getAllKeywords();
    std::vector<std::string> newKeywords = newBook.getAllKeywords();

    std::unordered_set<std::string> oldSet(oldKeywords.begin(), oldKeywords.end());
    std::unordered_set<std::string> newSet(newKeywords.begin(), newKeywords.end());

    if (oldSet != newSet) {
        for (const auto& keyword : oldKeywords) {
            if (newSet.find(keyword) == newSet.end()) {
                KeywordIndex kwIndex(keyword);
                std::vector<ISBNIndex> isbns = keywordIndex.find(kwIndex);
                for (const auto& storedIsbn : isbns) {
                    if (storedIsbn == isbn) {
                        keywordIndex.remove(kwIndex, storedIsbn);
                        break;
                    }
                }
            }
        }

        for (const auto& keyword : newKeywords) {
            if (oldSet.find(keyword) == oldSet.end()) {
                KeywordIndex kwIndex(keyword);
                keywordIndex.insert(kwIndex, isbn);
            }
        }
    }
}


std::vector<BookData> BookSystem::getAllBooksFromMap() const {
    // 直接使用 Map 的遍历功能
    return isbnMap.getAllValues();
}

bool BookSystem::showBooks(const std::string& type, const std::string& value) {  //param_type  param_value
    std::vector<BookData> results;

    if (type.empty()) {
        results = getAllBooks();
    } else if (type == "ISBN") {
        // std::cerr << "test1  " << value;
        results = searchByISBN(value);
    } else if (type == "name") {
        results = searchByName(value);
    } else if (type == "author") {
        results = searchByAuthor(value);
    } else if (type == "keyword") {
        results = searchByKeyword(value);
    } else {
        return false;
    }

    std::sort(results.begin(), results.end());   // 按ISBN排序

    // for (auto i : results) {
    //     std::cerr << "test2" << "  ";
    //     std::cerr << i;
    // }

    if (results.empty()) {
        std::cout << "\n";
    } else {
        for (const auto& book : results) {
            std::cout << book.toString() << "\n";
        }
    }
    return true;
}

bool BookSystem::buyBook(const std::string& isbnStr, long long quantity, double& total) {
    if (!isValidISBNStr(isbnStr) || quantity <= 0) return false;

    BookData book = getBookByISBNStr(isbnStr);
    if (!book.isValid()) return false;

    if (book.getStock() < quantity) return false;

    total = book.getPrice() * quantity;

    book.decreaseStock(quantity);

    ISBNIndex isbn(isbnStr);
    isbnMap.remove(isbn, book);
    isbnMap.insert(isbn, book);

    addFinanceRecord(total, 0.0);

    return true;
}

bool BookSystem::selectBook(const std::string& isbnStr) {
    if (!isValidISBNStr(isbnStr)) return false;

    const ISBNIndex isbn(isbnStr);

    if (!bookExists(isbn)) {
        return createBook(isbn);
    }

    return true;
}

bool BookSystem::modifyBook(const std::string& selectedISBN,
                           const std::vector<std::pair<std::string, std::string>>& modifications) {  //pair<std::string, std::string> param_type  param_value
    // std::cerr << selectedISBN << "  " << bookExistsStr(selectedISBN);
    if (selectedISBN.empty() || !bookExistsStr(selectedISBN)) {
        // std::cerr << "  test3  \n";
        return false;
    }

    BookData originalBook = getBookByISBNStr(selectedISBN);
    BookData modifiedBook = originalBook;

    std::string originalISBN = selectedISBN;
    std::string newISBN = selectedISBN;

    bool isbnModified = false;
    bool nameModified = false;
    bool authorModified = false;
    bool keywordsModified = false;
    bool priceModified = false;

    for (const auto& mod : modifications) {
        const std::string& type = mod.first;
        const std::string& value = mod.second;

        if (type == "ISBN") {
            if (isbnModified) return false;
            if (!isValidISBNStr(value)) return false;
            if (value == selectedISBN) return false;
            if (bookExistsStr(value)) return false;

            modifiedBook.setISBN(value);
            newISBN = value;

            isbnModified = true;
        }
        else if (type == "name") {
            if (nameModified) return false;
            // std::cerr << "  test4  \n";
            if (!isValidBookNameStr(value)) return false;
            // std::cerr << "  test5  \n";
            modifiedBook.setBookName(value);
            // std::cerr << "  test6  \n";
            nameModified = true;
        }
        else if (type == "author") {
            if (authorModified) return false;
            if (!isValidAuthorStr(value)) return false;
            modifiedBook.setAuthor(value);
            authorModified = true;
        }
        else if (type == "keyword") {
            if (keywordsModified) return false;
            if (!isValidKeywordsStr(value)) return false;
            modifiedBook.setKeywords(value);
            keywordsModified = true;
        }
        else if (type == "price") {
            if (priceModified) return false;
            if (!isValidPriceStr(value)) return false;
            double price = std::stod(value);
            modifiedBook.setPrice(price);
            priceModified = true;
        }
        else {
            return false;
        }
    }

    if (!isbnModified && !nameModified && !authorModified &&
        !keywordsModified && !priceModified) {
        // std::cerr << "  test7  \n";
        return false;
    }

    updateIndices(originalBook, modifiedBook);

    ISBNIndex oldIsbn(originalISBN);
    ISBNIndex newIsbn(newISBN);

    // std::cerr << "  test8  \n";

    if (isbnModified) {
        isbnMap.remove(oldIsbn, originalBook);
        isbnMap.insert(newIsbn, modifiedBook);
    } else {
        isbnMap.remove(oldIsbn, originalBook);
        isbnMap.insert(oldIsbn, modifiedBook);
    }

    return true;
}

bool BookSystem::importBook(const std::string& selectedISBN, long long quantity, double totalCost) {
    if (selectedISBN.empty() || !bookExistsStr(selectedISBN)) return false;
    if (quantity <= 0 || totalCost <= 0) return false;

    BookData book = getBookByISBNStr(selectedISBN);

    book.increaseStock(quantity);

    ISBNIndex isbn(selectedISBN);
    isbnMap.remove(isbn, book);
    isbnMap.insert(isbn, book);

    addFinanceRecord(0.0, totalCost);

    return true;
}

BookData BookSystem::getBookByISBN(const ISBNIndex& isbn) {
    std::vector<BookData> books = isbnMap.find(isbn);
    if (!books.empty()) {
        return books[0];
    }
    return BookData();
}

BookData BookSystem::getBookByISBNStr(const std::string& isbnStr) {
    ISBNIndex isbn(isbnStr);
    return getBookByISBN(isbn);
}

std::vector<BookData> BookSystem::searchByISBN(const std::string& isbnStr) {
    std::vector<BookData> result;
    BookData book = getBookByISBNStr(isbnStr);
    if (book.isValid()) {
        result.push_back(book);
    }
    return result;
}

std::vector<BookData> BookSystem::searchByName(const std::string& name) {
    std::vector<BookData> result;
    NameAuthorIndex nameIdx(name);
    std::vector<ISBNIndex> isbns = nameIndex.find(nameIdx);

    for (const auto& isbn : isbns) {
        BookData book = getBookByISBN(isbn);
        if (book.isValid() && book.getBookName() == name) {
            result.push_back(book);
        }
    }

    return result;
}

std::vector<BookData> BookSystem::searchByAuthor(const std::string& author) {
    std::vector<BookData> result;
    NameAuthorIndex authorIdx(author);
    std::vector<ISBNIndex> isbns = authorIndex.find(authorIdx);

    for (const auto& isbn : isbns) {
        BookData book = getBookByISBN(isbn);
        if (book.isValid() && book.getAuthor() == author) {
            result.push_back(book);
        }
    }

    return result;
}

std::vector<BookData> BookSystem::searchByKeyword(const std::string& keyword) {
    std::vector<BookData> result;
    KeywordIndex kwIdx(keyword);
    std::vector<ISBNIndex> isbns = keywordIndex.find(kwIdx);

    for (const auto& isbn : isbns) {
        BookData book = getBookByISBN(isbn);
        if (book.isValid() && book.hasKeyword(keyword)) {
            result.push_back(book);
        }
    }
    return result;
}

std::vector<BookData> BookSystem::getAllBooks() {
    return getAllBooksFromMap();
}

bool BookSystem::createBook(const ISBNIndex& isbn) {
    if (isbn.empty() || bookExists(isbn)) return false;

    BookData newBook(isbn.toString());
    isbnMap.insert(isbn, newBook);

    return true;
}

bool BookSystem::createBookFromStr(const std::string& isbnStr) {
    ISBNIndex isbn(isbnStr);
    return createBook(isbn);
}

bool BookSystem::bookExists(const ISBNIndex& isbn) {
    return !isbnMap.find(isbn).empty();
}

bool BookSystem::bookExistsStr(const std::string& isbnStr) {
    ISBNIndex isbn(isbnStr);
    return bookExists(isbn);
}



FinanceSystem::FinanceSystem(const std::string& baseFileName)
    : financeMap(baseFileName + "_finance") {
        std::vector<FinanceRecord> allRecords = financeMap.getAllValues();
        transactionCount = allRecords.size();
}

bool FinanceSystem::addFinanceRecord(double income, double expense) {
    if (income < 0 || expense < 0) return false;
    FinanceRecord record(income, expense);
    int transactionId = transactionCount + 1;
    financeMap.insert(transactionId, record);
    transactionCount = transactionId;
    return true;

}

bool FinanceSystem::showFinance(int count) const {
    if (count == 0) {
        std::cout << "\n";
        return true;
    }

    if (count > transactionCount && count != -1) {
        return false;
    }

    double totalIncome = 0.0;
    double totalExpense = 0.0;

    if (count == -1) {
        for (int i = 1; i <= transactionCount; i++) {
            std::vector<FinanceRecord> records = financeMap.find(i);
            if (!records.empty()) {
                totalIncome += records[0].income;
                totalExpense += records[0].expense;
            }
        }
    } else {
        int start = transactionCount - count + 1;
        if (start < 1) start = 1;

        for (int i = start; i <= transactionCount; i++) {
            std::vector<FinanceRecord> records = financeMap.find(i);
            if (!records.empty()) {
                totalIncome += records[0].income;
                totalExpense += records[0].expense;
            }
        }
    }

    std::cout << "+ " << formatDouble(totalIncome)
              << " - " << formatDouble(totalExpense) << "\n";

    return true;
}

std::pair<double, double> FinanceSystem::getFinanceSummary(int count) const {
    double totalIncome = 0.0;
    double totalExpense = 0.0;

    if (count == -1 || count >= transactionCount) {
        for (int i = 1; i <= transactionCount; i++) {
            std::vector<FinanceRecord> records = financeMap.find(i);
            if (!records.empty()) {
                totalIncome += records[0].income;
                totalExpense += records[0].expense;
            }
        }
    } else {
        int start = transactionCount - count + 1;
        if (start < 1) start = 1;

        for (int i = start; i <= transactionCount; i++) {
            std::vector<FinanceRecord> records = financeMap.find(i);
            if (!records.empty()) {
                totalIncome += records[0].income;
                totalExpense += records[0].expense;
            }
        }
    }

    return {totalIncome, totalExpense};
}

void FinanceSystem::updateTransactionCount() {
    std::vector<FinanceRecord> allRecords = financeMap.getAllValues();
    transactionCount = allRecords.size();
}

std::string FinanceSystem::formatDouble(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    std::string result = oss.str();
    return result;
}
