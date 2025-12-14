#include "../include/parser.h"
#include "../include/token.h"
#include <algorithm>
#include <cctype>
#include <regex>

std::string CommandParser::extractQuotedContent(const std::string& str) {
    const size_t firstQuote = str.find('\"');
    if (firstQuote == std::string::npos) return "";

    const size_t lastQuote = str.rfind('\"');
    if (lastQuote == std::string::npos || lastQuote <= firstQuote) return "";

    return str.substr(firstQuote + 1, lastQuote - firstQuote - 1);
}

bool CommandParser::validateParameterFormat(const std::string& param) {
    if (param.empty()) return false;

    if (param.find("-ISBN=") == 0) {
        return validateISBNParam(param);
    }
    else if (param.find("-name=") == 0) {
        return validateQuotedParam(param, "-name=");
    }
    else if (param.find("-author=") == 0) {
        return validateQuotedParam(param, "-author=");
    }
    else if (param.find("-keyword=") == 0) {
        return validateQuotedParam(param, "-keyword=");
    }
    else if (param.find("-price=") == 0) {
        return validatePriceParam(param);
    }
    return false;
}

bool CommandParser::validateISBNParam(const std::string& param) {
    if (param.length() <= 6) return false;

    const std::string value = param.substr(6);
    if (value.empty()) return false;

    if (value.find('\"') != std::string::npos) return false;

    return true;
}

std::string CommandParser::getParameterType(const std::string& param) {
    if (param.find("-ISBN=") == 0) return "ISBN";
    if (param.find("-name=") == 0) return "name";
    if (param.find("-author=") == 0) return "author";
    if (param.find("-keyword=") == 0) return "keyword";
    if (param.find("-price=") == 0) return "price";
    return "";
}

bool CommandParser::validateQuotedParam(const std::string& param, const std::string& prefix) {

    if (param.length() <= prefix.length() + 2) return false;
    if (param.find(prefix) != 0) return false;

    if (param[prefix.length()] != '\"') return false;

    if (param.back() != '\"') return false;

    std::string content = extractQuotedContent(param.substr(prefix.length() - 1));
    if (content.empty()) return false;

    return true;
}

bool CommandParser::validatePriceParam(const std::string& param) {
    if (param.length() <= 7) return false;

    std::string value = param.substr(7);
    if (value.empty()) return false;

    if (value.find('\"') != std::string::npos) return false;

    int dotCount = 0;
    bool hasDigit = false;

    for (char c : value) {
        if (c == '.') {
            dotCount++;
            if (dotCount > 1) return false;
        } else if (std::isdigit(c)) {
            hasDigit = true;
        } else {
            return false;
        }
    }

    if (!hasDigit) return false;

    size_t dotPos = value.find('.');
    if (dotPos != std::string::npos) {
        if (value.length() - dotPos - 1 > 2) return false;
    }

    return true;
}

std::string CommandParser::getParameterValue(const std::string& param) {
    std::string type = getParameterType(param);

    if (type == "ISBN" || type == "price") {
        size_t equalPos = param.find('=');
        if (equalPos == std::string::npos) return "";
        return param.substr(equalPos + 1);
    }
    else if (type == "name" || type == "author" || type == "keyword") {
        size_t equalPos = param.find('=');
        if (equalPos == std::string::npos) return "";
        return extractQuotedContent(param.substr(equalPos));
    }

    return "";
}

std::vector<std::string> CommandParser::splitKeywords(const std::string& keywords) {
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

bool CommandParser::parseShowCommand(const std::vector<std::string>& tokens,
                                    std::string& type, std::string& value) {
    if (tokens.size() == 1) {
        type = "";
        value = "";
        return true;
    }

    if (tokens.size() != 2) return false;

    const std::string param = tokens[1];

    if (!validateParameterFormat(param)) {
        return false;
    }

    type = getParameterType(param);

    if (value.empty()) return false;

    if (type == "name" || type == "author" || type == "keyword") {
        parse_argument(param, type, value);
    }
    if (type == "ISBN" || type == "price") {
        parse_single_argument(param, type, value);
    }

    if (type == "keyword") {
        const std::vector<std::string> keywords = splitKeywords(param);
        if (keywords.size() != 1) {
            return false;
        }
    }

    return true;
}

bool CommandParser::parseModifyCommand(const std::vector<std::string>& tokens,
                                      std::vector<std::pair<std::string, std::string>>& modifications) {
    if (tokens.size() < 2) return false;

    std::vector<bool> paramTypes(5, false);

    for (size_t i = 1; i < tokens.size(); i++) {
        std::string param = tokens[i];

        if (!validateParameterFormat(param)) {
            // std::cerr << 1;
            return false;
        }

        std::string type = getParameterType(param);
        std::string value;
        if (type == "name" || type == "author" || type == "keyword") {
            parse_argument(param, type, value);
        }
        if (type == "ISBN" || type == "price") {
            parse_single_argument(param, type, value);
        }

        if (value.empty()) return false;

        if (type == "ISBN") {
            if (paramTypes[0]) return false;
            modifications.emplace_back("ISBN", value);
            paramTypes[0] = true;
        }
        else if (type == "name") {
            if (paramTypes[1]) return false;
            modifications.emplace_back("name", value);
            paramTypes[1] = true;
        }
        else if (type == "author") {
            if (paramTypes[2]) return false;
            modifications.emplace_back("author", value);
            paramTypes[2] = true;
        }
        else if (type == "keyword") {
            if (paramTypes[3]) return false;
            modifications.emplace_back("keyword", value);
            paramTypes[3] = true;

            std::vector<std::string> keywords = splitKeywords(value);
            std::sort(keywords.begin(), keywords.end());
            for (size_t j = 1; j < keywords.size(); j++) {
                if (keywords[j] == keywords[j-1]) {
                    return false;
                }
            }
        }
        else if (type == "price") {
            if (paramTypes[4]) return false;
            modifications.emplace_back("price", value);
            paramTypes[4] = true;
        }
        else {
            return false;
        }
    }

    return true;
}