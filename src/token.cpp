#include "../include/token.h"

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

}