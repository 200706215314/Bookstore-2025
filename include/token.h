#ifndef BOOKSTORE_2025_TOKEN_H
#define BOOKSTORE_2025_TOKEN_H

#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> tokenize(const std::string &s);

bool isValidArgument(const std::string &s);

std::vector<std::string> tokenizeWithQuotes(const std::string &s);
std::vector<std::string> tokenizeCommand(const std::string& s);

bool parse_single_argument(const std::string& arg,
                          std::string& param_type,
                          std::string& param_value);

bool parse_argument(const std::string& arg, std::string& param_type, std::string& param_value);

#endif //BOOKSTORE_2025_TOKEN_H