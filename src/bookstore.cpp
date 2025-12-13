#include "../include/bookstore.h"

Bookstore::Bookstore() 
    : accountSystem("account_data"),
      bookSystem("book_data") {}

Bookstore::~Bookstore() = default;

std::vector<std::string> Bookstore::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::string tmp = "";
    bool inQuotes = false;
    
    for (char c : input) {
        if (c == '\"') {
            inQuotes = !inQuotes;
            tmp += c;
        } else if (c == ' ' && !inQuotes) {
            if (!tmp.empty()) {
                tokens.push_back(tmp);
                tmp.clear();
            }
        } else {
            tmp += c;
        }
    }
    
    if (!tmp.empty()) {
        tokens.push_back(tmp);
    }
    
    return tokens;
}

bool Bookstore::parseShowCommand(const std::vector<std::string>& tokens, 
                                std::string& type, std::string& value) {
    if (tokens.size() == 1) {
        type = "";
        value = "";
        return true;
    }
    
    if (tokens.size() != 2) return false;
    
    std::string param = tokens[1];
    
    if (param.find("-ISBN=") == 0) {
        type = "ISBN";
        value = param.substr(6);
    } else if (param.find("-name=\"") == 0 && param.back() == '\"') {
        type = "name";
        value = param.substr(7, param.length() - 8); // 移除-name="和结尾的"
    } else if (param.find("-author=\"") == 0 && param.back() == '\"') {
        type = "author";
        value = param.substr(9, param.length() - 10);
    } else if (param.find("-keyword=\"") == 0 && param.back() == '\"') {
        type = "keyword";
        value = param.substr(10, param.length() - 11);
    } else {
        return false;
    }
    
    return true;
}

bool Bookstore::parseModifyCommand(const std::vector<std::string>& tokens,
                                  std::vector<std::pair<std::string, std::string>>& modifications) {
    if (tokens.size() < 2) return false;
    
    for (size_t i = 1; i < tokens.size(); i++) {
        std::string param = tokens[i];
        
        if (param.find("-ISBN=") == 0) {
            modifications.emplace_back("ISBN", param.substr(6));
        } else if (param.find("-name=\"") == 0 && param.back() == '\"') {
            modifications.emplace_back("name", param.substr(7, param.length() - 8));
        } else if (param.find("-author=\"") == 0 && param.back() == '\"') {
            modifications.emplace_back("author", param.substr(9, param.length() - 10));
        } else if (param.find("-keyword=\"") == 0 && param.back() == '\"') {
            modifications.emplace_back("keyword", param.substr(10, param.length() - 11));
        } else if (param.find("-price=") == 0) {
            modifications.emplace_back("price", param.substr(7));
        } else {
            return false;
        }
    }
    
    return true;
}

bool Bookstore::checkCommandPrivilege(const std::string& command, int& requiredPrivilege) {
    // 根据标准要求设置权限
    if (command == "su" || command == "register") {
        requiredPrivilege = 0;
    } else if (command == "logout" || command == "passwd" || 
               command == "show" || command == "buy") {
        requiredPrivilege = 1;
    } else if (command == "useradd" || command == "select" || 
               command == "modify" || command == "import") {
        requiredPrivilege = 3;
    } else if (command == "delete" || command == "show finance" || 
               command == "log" || command == "report finance" || 
               command == "report employee") {
        requiredPrivilege = 7;
    } else {
        return false;
    }
    
    return true;
}

void Bookstore::run() {
    while (!std::cin.eof()) {
        std::string line;
        std::getline(std::cin, line);

        if (line.empty()) continue;

        std::vector<std::string> tokens = tokenize(line);
        if (tokens.empty()) continue;
        
        std::string command = tokens[0];

        if (command == "quit" || command == "exit") {
            if (tokens.size() != 1) {
                std::cout << "Invalid" << std::endl;
            } else {
                break;
            }
            continue;
        }

        int requiredPrivilege = 0;
        if (!checkCommandPrivilege(command, requiredPrivilege)) {
            std::cout << "Invalid" << std::endl;
            continue;
        }
        
        if (!accountSystem.hasPrivilege(requiredPrivilege)) {
            std::cout << "Invalid" << std::endl;
            continue;
        }
        // 处理指令
        if (!processCommand(tokens)) {
            std::cout << "Invalid" << std::endl;
        }
    }
}

bool Bookstore::processCommand(const std::vector<std::string>& tokens) {
    if (tokens.empty()) return true;
    
    std::string command = tokens[0];
    
    if (command == "su" || command == "logout" || command == "register" ||
        command == "passwd" || command == "useradd" || command == "delete") {
        return handleAccountCommand(tokens);
    } else if (command == "show" || command == "buy" || command == "select" ||
               command == "modify" || command == "import") {
        return handleBookCommand(tokens);
    } else if (command == "show finance") {
        return handleFinanceCommand(tokens);
    }
    
    return false;
}

bool Bookstore::handleAccountCommand(const std::vector<std::string>& tokens) {
    std::string command = tokens[0];
    
    try {
        if (command == "su") {
            if (tokens.size() == 2) {
                string tmp = "";
                return accountSystem.login(tokens[1], tmp);
            } else if (tokens.size() == 3) {
                return accountSystem.login(tokens[1], tokens[2]);
            }
        } else if (command == "logout") {
            if (tokens.size() != 1) return false;
            return accountSystem.logout();
        } else if (command == "register") {
            if (tokens.size() != 4) return false;
            return accountSystem.registerUser(tokens[1], tokens[2], tokens[3]);
        } else if (command == "passwd") {
            if (tokens.size() == 3) {
                return accountSystem.changePassword(tokens[1], "", tokens[2]);
            } else if (tokens.size() == 4) {
                return accountSystem.changePassword(tokens[1], tokens[2], tokens[3]);
            }
        } else if (command == "useradd") {
            if (tokens.size() != 5) return false;
            int privilege = std::stoi(tokens[3]);
            return accountSystem.addUser(tokens[1], tokens[2], privilege, tokens[4]);
        } else if (command == "delete") {
            if (tokens.size() != 2) return false;
            return accountSystem.deleteUser(tokens[1]);
        }
    } catch (...) {
        return false;
    }
    
    return false;
}

bool Bookstore::handleBookCommand(const std::vector<std::string>& tokens) {
    std::string command = tokens[0];
    
    try {
        if (command == "show") {
            std::string type, value;
            if (!parseShowCommand(tokens, type, value)) return false;
            return bookSystem.showBooks(type, value);
        } else if (command == "buy") {
            if (tokens.size() != 3) return false;
            long long quantity = std::stoll(tokens[2]);
            double total = 0.0;
            bool success = bookSystem.buyBook(tokens[1], quantity, total);
            if (success) {
                std::cout << std::fixed << std::setprecision(2) << total << "\n";
            }
            return success;
        } else if (command == "select") {
            if (tokens.size() != 2) return false;
            bool success = bookSystem.selectBook(tokens[1]);
            if (success) {
                accountSystem.selectBook(tokens[1]);
            }
            return success;
        } else if (command == "modify") {
            std::vector<std::pair<std::string, std::string>> modifications;
            if (!parseModifyCommand(tokens, modifications)) return false;
            std::string selectedISBN = accountSystem.getSelectedISBN();
            return bookSystem.modifyBook(selectedISBN, modifications);
        } else if (command == "import") {
            if (tokens.size() != 3) return false;
            long long quantity = std::stoll(tokens[1]);
            double totalCost = std::stod(tokens[2]);
            std::string selectedISBN = accountSystem.getSelectedISBN();
            return bookSystem.importBook(selectedISBN, quantity, totalCost);
        }
    } catch (...) {
        return false;
    }
    
    return false;
}

bool Bookstore::handleFinanceCommand(const std::vector<std::string>& tokens) {
    if (tokens[0] != "show" || tokens.size() < 2) return false;
    
    std::string subcommand = tokens[1];
    if (subcommand != "finance") return false;
    
    if (tokens.size() == 2) {
        // show finance
        return bookSystem.showFinance(-1);
    } else if (tokens.size() == 3) {
        // show finance [Count]
        try {
            int count = std::stoi(tokens[2]);
            if (count < 0) return false;
            return bookSystem.showFinance(count);
        } catch (...) {
            return false;
        }
    }
    
    return false;
}