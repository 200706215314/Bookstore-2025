//
// Created by lenovo on 2025/12/13.
//
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
using namespace std;

//show or modify   先tokenize一次 再quote一次
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
        } else if (c == ' ' && !inQuotes) {
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

std::vector<std::string> splitKeywords(const std::string& keywords) {
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

int main() {
    string in;
    getline(cin, in);   //不能用cin>>
    // cerr << in.find("=");
    vector<string> v = splitKeywords(in);
    for (auto i : v) {
        cout << i << endl;
    }
    // string a, b;
    // bool flag = parse_argument(in, a, b);
    // cout << a << "    " << b;

    return 0;
}