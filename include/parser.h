//
// Created by lenovo on 2025/12/13.
//

#ifndef BOOKSTORE_2025_PARSER_H
#define BOOKSTORE_2025_PARSER_H
#include <iostream>
#include <string>
#include <vector>
#include <utility>

class CommandParser {
public:
    // show
    static bool parseShowCommand(const std::vector<std::string>& tokens,
                                std::string& type, std::string& value);

    // modify
    static bool parseModifyCommand(const std::vector<std::string>& tokens,
                                  std::vector<std::pair<std::string, std::string>>& modifications);

    static std::string extractQuotedContent(const std::string& str);

    static bool validateParameterFormat(const std::string& param);


    static std::string getParameterType(const std::string& param);


    static std::string getParameterValue(const std::string& param);

    static std::vector<std::string> splitKeywords(const std::string& keywords);

    static bool validateISBNParam(const std::string& param);

    static bool validateQuotedParam(const std::string& param, const std::string& prefix);

    static bool validatePriceParam(const std::string& param);
};

#endif //BOOKSTORE_2025_PARSER_H