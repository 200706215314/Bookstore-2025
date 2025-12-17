#include "../include/token.h"
#include <sstream>

std::vector<std::string> tokenize(const std::string& s) {
    std::vector<std::string> tokens;
    std::string tmp = "";
    for (auto i : s) {
        if(i == ' ') {
            if (!tmp.empty()) {
                tokens.push_back(tmp);
                tmp.clear();
            }
            continue;
        }

        tmp += i;
    }
    if (!tmp.empty()) {
        tokens.push_back(tmp);
    }
    return tokens;
}

bool isValidArgument(const std::string &s) {
    return !s.empty();
}

std::vector<std::string> tokenizeWithQuotes(const std::string &s) {
    std::vector<std::string> tokens;
    std::string currentToken;
    bool inQuotes = false;

    for (size_t i = 0; i < s.length(); i++) {
        char c = s[i];

        if (c == '\"') {
            inQuotes = !inQuotes;
            currentToken += c;
        } else if (c == ' '  && !inQuotes) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        } else {
            currentToken += c;
        }
    }

    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }

    return tokens;
}

std::vector<std::string> tokenizeCommand(const std::string& s) {
    std::vector<std::string> tokens;
    std::istringstream iss(s);
    std::string token;

    while (iss >> token) {
        if (token.find('-') == 0) {
            if (token.find('\"') != std::string::npos) {
                if (token.back() == '\"') {
                    tokens.push_back(token);
                } else {
                    std::string rest;
                    while (iss >> rest) {
                        token += " " + rest;
                        if (rest.find('\"') != std::string::npos) {
                            break;
                        }
                    }
                    tokens.push_back(token);
                }
            } else {
                tokens.push_back(token);
            }
        } else {
            tokens.push_back(token);
        }
    }

    return tokens;
}

bool parse_single_argument(const std::string& arg,
                          std::string& param_type,
                          std::string& param_value) {
    if (arg.empty() || arg[0] != '-') {
        return false;
    }

    size_t equal_pos = arg.find('=');
    if (equal_pos == std::string::npos) {
        return false;
    }

    param_type = arg.substr(1, equal_pos - 1);
    if (param_type.empty()) {
        return false;
    }

    param_value = arg.substr(equal_pos + 1);
    return true;
}

bool parse_argument(const std::string& arg, std::string& param_type, std::string& param_value) {
    if (arg.empty() || arg[0] != '-') {
        return false;
    }

    size_t equal_pos = arg.find('=');
    if (equal_pos == std::string::npos) {
        return false;
    }

    if (equal_pos <= 1 || equal_pos >= arg.length() - 1) {
        return false;
    }

    param_type = arg.substr(1, equal_pos - 1);
    if (param_type.empty()) {
        return false;
    }

    param_value = arg.substr(equal_pos + 2, arg.length() - equal_pos - 3);
    return true;
}