
#ifndef BOOKSTORE_2025_BOOKSTORE_H
#define BOOKSTORE_2025_BOOKSTORE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <sstream>
#include "account.h"
#include "book.h"
#include "parser.h"

class Bookstore {
private:
    AccountSystem accountSystem;
    BookSystem bookSystem;
    std::string currentUserID;
    std::string selectedISBN;

    std::vector<std::string> tokenize(const std::string& input);
    bool parseShowCommand(const std::vector<std::string>& tokens,
                         std::string& type, std::string& value);
    bool parseModifyCommand(const std::vector<std::string>& tokens,
                           std::vector<std::pair<std::string, std::string>>& modifications);

    bool checkCommandPrivilege(const std::string& command, int& requiredPrivilege);

public:
    Bookstore();
    ~Bookstore();

    void run();

    bool processCommand(const std::vector<std::string>& tokens);

    bool handleAccountCommand(const std::vector<std::string>& tokens);
    bool handleBookCommand(const std::vector<std::string>& tokens);
    bool handleFinanceCommand(const std::vector<std::string>& tokens);

};


#endif //BOOKSTORE_2025_BOOKSTORE_H