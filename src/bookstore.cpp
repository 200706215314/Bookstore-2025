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

    // for (auto i : value) {
    //     std::cerr << i << " ";
    // }

    if (tokens.size() != 2) return false;
    
    std::string param = tokens[1];
    
    if (param.find("-ISBN=") == 0) {
        type = "ISBN";
        value = param.substr(6);
        if (value.empty()) return false;
    } else if (param.find("-name=\"") == 0 && param.back() == '\"') {
        type = "name";
        value = param.substr(7, param.length() - 8); // 移除-name="和结尾的"
        if (value.empty()) return false;
    } else if (param.find("-author=\"") == 0 && param.back() == '\"') {
        type = "author";
        value = param.substr(9, param.length() - 10);
        if (value.empty()) return false;
    } else if (param.find("-keyword=\"") == 0 && param.back() == '\"') {
        type = "keyword";
        value = param.substr(10, param.length() - 11);
        if (value.empty()) return false;
        if (value.find('|') != std::string::npos) return false;    // 检查是否为单个关键词
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

    bool Bookstore::checkCommandPrivilege(const std::vector<std::string>& tokens, int& requiredPrivilege) {
    if (tokens.empty()) return false;

    const std::string command = tokens[0];
    const std::string subcommand = tokens.size() >= 2 ? tokens[1] : "";

    if (command == "su" || command == "register") {
        requiredPrivilege = 0;
    }
    else if (command == "logout" || command == "passwd" || command == "buy") {
        requiredPrivilege = 1;
    }
    else if (command == "show") {
        // exit(1);
        if (subcommand == "finance") {
            requiredPrivilege = 7;
        } else {
            requiredPrivilege = 1;  // 普通show命令
        }
    }
    else if (command == "useradd" || command == "select" ||
              command == "modify" || command == "import") {
        requiredPrivilege = 3;
              }
    else if (command == "delete" || command == "log" ||
              command == "report") {
        requiredPrivilege = 7;
              }
    else {
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
        if (!checkCommandPrivilege(tokens, requiredPrivilege)) {  // 传递 tokens
            std::cout << "Invalid" << std::endl;
            continue;
        }
        
        if (!accountSystem.hasPrivilege(requiredPrivilege)) {
            std::cout << "Invalid" << std::endl;
            continue;
        }
        // 处理指令
        if (!processCommand(tokens)) {
            std::cout << "Invalid\n";
            // std::cout << "Invalid" << std::endl;
        }
    }
}

bool Bookstore::processCommand(const std::vector<std::string>& tokens) {
    if (tokens.empty()) return true;
    
    std::string command = tokens[0];
    std::string command_ = "";
    if (tokens.size() >= 2) {
        command_ = tokens[1];
    }

    
    if (command == "su" || command == "logout" || command == "register" ||
        command == "passwd" || command == "useradd" || command == "delete") {
        return handleAccountCommand(tokens);
    } else if ((command == "show" && command_ != "finance" )|| command == "buy" || command == "select" ||
               command == "modify" || command == "import") {
        // std::cerr << "test3" << handleBookCommand(tokens) << std::endl;
        return handleBookCommand(tokens);
    } else if (command == "show" && command_ == "finance") {
        return handleFinanceCommand(tokens);
    }
    
    return false;
}

bool Bookstore::handleAccountCommand(const std::vector<std::string>& tokens) {
    std::string command = tokens[0];
    
    try {
        if (command == "su") {
            // exit(1);
            if (tokens.size() == 2) {
                string tmp = "";
                return accountSystem.login(tokens[1], tmp);
            } else if (tokens.size() == 3) {
                return accountSystem.login(tokens[1], tokens[2]);
            }
        } else if (command == "logout") {
            //exit(1);
            if (tokens.size() != 1) return false;
            return accountSystem.logout();
        } else if (command == "register") {
            // exit(1);
            if (tokens.size() != 4) return false;
            return accountSystem.registerUser(tokens[1], tokens[2], tokens[3]);
        } else if (command == "passwd") {
            // exit(1);
            if (tokens.size() == 3) {
                return accountSystem.changePassword(tokens[1], "", tokens[2]);
            } else if (tokens.size() == 4) {
                return accountSystem.changePassword(tokens[1], tokens[2], tokens[3]);
            }
        } else if (command == "useradd") {
            // exit(1);
            if (tokens.size() != 5) return false;
            int privilege = std::stoi(tokens[3]);
            return accountSystem.addUser(tokens[1], tokens[2], privilege, tokens[4]);
        } else if (command == "delete") {
            // exit(1);
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
    // for (auto i : tokens) {
    //     std::cerr << i << " ";
    // }
    try {
        if (command == "show") {
            // exit(1);
            std::string type, value;
            if (!parseShowCommand(tokens, type, value)) return false;
            return bookSystem.showBooks(type, value);
        } else if (command == "buy") {
            // exit(1);
            if (tokens.size() != 3) return false;
            long long quantity = std::stoll(tokens[2]);
            double total = 0.0;
            bool success = bookSystem.buyBook(tokens[1], quantity, total);
            if (success) {
                std::cout << std::fixed << std::setprecision(2) << total << "\n";
            }
            return success;
        } else if (command == "select") {
            // exit(1);
            // std::cerr << "test1  " << tokens.size()  << std::endl;
            if (tokens.size() != 2) {
                return false;
            }

            const std::string& isbn = tokens[1];

            // std::cerr << isbn.empty();
            // 检查ISBN是否为空
            if (isbn.empty()) {
                return false;
            }

            // 检查ISBN格式
            if (!bookSystem.isValidISBNStr(isbn)) {
                return false;
            }
            // 调用BookSystem的selectBook
            bool success = bookSystem.selectBook(isbn);
            if (success) {
                accountSystem.selectBook(isbn);
            }
            return success;
        } else if (command == "modify") {
            // exit(1);
            std::vector<std::pair<std::string, std::string>> modifications;
            if (!parseModifyCommand(tokens, modifications)) {
                return false;
            }
            std::string selected_ISBN = accountSystem.getSelectedISBN();
            std::string new_ISBN = selected_ISBN;

            for (const auto& mod : modifications) {
                if (mod.first == "ISBN") {
                    new_ISBN = mod.second;
                    break;
                }
            }

            bool success = bookSystem.modifyBook(selected_ISBN, modifications);

            // 如果修改成功且ISBN被修改，更新所有登录用户的selectedISBN
            if (success && selected_ISBN != new_ISBN) {
                accountSystem.updateSelectedISBNForAll(selected_ISBN, new_ISBN);
            }

            return success;
        } else if (command == "import") {
            // exit(1);
            if (tokens.size() != 3) return false;

            std::string quantityStr = tokens[1];
            std::string totalCostStr = tokens[2];

            // 检查是否选中图书
            std::string selected_ISBN = accountSystem.getSelectedISBN();
            if (selected_ISBN.empty()) {
                return false;
            }

            // 验证 quantity 格式
            if (!isValidQuantityStr(quantityStr)) {
                return false;
            }

            // 验证 totalCost 格式
            if (!isValidTotalCostStr(totalCostStr)) {
                return false;
            }

            try {
                long long quantity = std::stoll(quantityStr);
                double totalCost = std::stod(totalCostStr);
                return bookSystem.importBook(selected_ISBN, quantity, totalCost);
            } catch (...) {
                return false;
            }
        }
    } catch (...) {
        return false;
    }
    
    return false;
}

bool Bookstore::handleFinanceCommand(const std::vector<std::string>& tokens) {
    // std::cerr << "test3 ";

    if (tokens.size() == 3) {
        try {
            const std::string& countStr = tokens[2];
            // 校验前导0
            if (countStr.length() > 1 && countStr[0] == '0') {
                return false;
            }
            int count = std::stoi(countStr);
            if (count < 0) return false;
            return bookSystem.showFinance(count);
        } catch (...) {
            return false;
        }
    }

    if (tokens[0] != "show" || tokens.size() < 2) {
        // std::cerr << "test4  ";
        return false;
    }
    
    std::string subcommand = tokens[1];
    if (subcommand != "finance") {
        // std::cerr << "test5  ";
        return false;
    }
    
    if (tokens.size() == 2) {
        // show finance
        // std::cerr << "test 6  ";
        return bookSystem.showFinance(-1);
    } else if (tokens.size() == 3) {
        // show finance [Count]
        // std::cerr << " test 7";
        try {
            int count = std::stoi(tokens[2]);
            // std::cerr << "test8 ";
            if (count < 0) {
                // std::cerr << "test 9  ";
                return false;
            }
            return bookSystem.showFinance(count);
        } catch (...) {
            // std::cerr << "test 10  ";
            return false;
        }
    }
    
    return false;
}

bool Bookstore::isValidTotalCostStr(const std::string& costStr) {
    if (costStr.empty() || costStr.length() > 13) return false;

    // 检查是否以 '.' 开头或结尾
    if (costStr[0] == '.' || costStr.back() == '.') {
        return false;
    }

    // 检查字符是否合法
    int dotCount = 0;
    bool hasDigit = false;

    for (size_t i = 0; i < costStr.length(); i++) {
        char c = costStr[i];
        if (c == '.') {
            dotCount++;
            if (dotCount > 1) return false;  // 超过一个小数点
        } else if (isdigit(c)) {
            hasDigit = true;
        } else {
            return false;  // 非法字符
        }
    }

    if (!hasDigit) return false;

    // 检查小数位数
    size_t dotPos = costStr.find('.');
    if (dotPos != std::string::npos) {
        size_t decimalDigits = costStr.length() - dotPos - 1;
        if (decimalDigits > 2) return false;  // 小数位数超过2位
    }

    // 检查数值是否为正数
    try {
        double cost = std::stod(costStr);
        return cost > 0.0;
    } catch (...) {
        return false;
    }
}

bool Bookstore::isValidQuantityStr(const std::string& quantityStr) {
    if (quantityStr.empty() || quantityStr.length() > 10) return false;

    // 检查是否有前导0（"0" 是合法的，但 "0123" 不合法）
    if (quantityStr.length() > 1 && quantityStr[0] == '0') {
        return false;
    }

    // 检查是否都是数字
    for (char c : quantityStr) {
        if (!isdigit(c)) return false;
    }

    // 检查数值范围
    try {
        long long qty = std::stoll(quantityStr);
        return qty > 0 && qty <= 2147483647LL;  // 不超过 int32 最大值
    } catch (...) {
        return false;
    }
}
