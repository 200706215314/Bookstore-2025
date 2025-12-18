//
// Created by lenovo on 2025/12/18.
//
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

bool isValidBookNameStr(const std::string& name) {
    if (name.empty() || name.length() > 60) return false;
    for (const char c : name) {
        if (static_cast<unsigned char>(c) < 32 || c == 127 || c == '\"') {
            return false;
        }
    }
    return true;
}

std::string parseEscapeSequences(const std::string& str) {
    std::string result;

    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            ++i; // 跳过反斜杠

            if (str[i] == 'x' && i + 2 < str.length()) {
                // 处理 \xHH 格式
                char hex1 = str[i + 1];
                char hex2 = str[i + 2];

                if (std::isxdigit(hex1) && std::isxdigit(hex2)) {
                    // 将两个十六进制字符转换为数字
                    int value;
                    std::string hexStr = {hex1, hex2};
                    std::stringstream ss;
                    ss << std::hex << hexStr;
                    ss >> value;

                    result += static_cast<char>(value);
                    i += 2; // 跳过两个十六进制字符
                } else {
                    // 不是有效的十六进制，保留原样
                    result += '\\';
                    result += str[i];
                }
            } else {
                // 处理其他转义序列
                switch (str[i]) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '\\': result += '\\'; break;
                case '\"': result += '\"'; break;
                default:
                    // 无效的转义序列，保留原样
                    result += '\\';
                    result += str[i];
                    break;
                }
            }
        } else {
            result += str[i];
        }
    }

    return result;
}

std::vector<std::string> tokenizeWithQuotes(const std::string &s) {
    std::vector<std::string> tokens;
    std::string currentToken;
    bool inQuotes = false;

    // 首先处理转义序列
    std::string processedInput = parseEscapeSequences(s);

    for (size_t i = 0; i < processedInput.length(); i++) {
        char c = processedInput[i];

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

std::vector<std::string> tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::string tmp = "";
    bool inQuotes = false;

    // 预处理：解析转义序列
    std::string processedInput;
    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] == '\\' && i + 1 < input.length()) {
            ++i;

            if (input[i] == 'x' && i + 2 < input.length() &&
                std::isxdigit(input[i+1]) && std::isxdigit(input[i+2])) {
                // 处理 \xHH
                std::string hexStr = input.substr(i+1, 2);
                int hexValue = std::stoi(hexStr, nullptr, 16);
                processedInput += static_cast<char>(hexValue);
                i += 2; // 跳过两个十六进制字符
                } else {
                    // 处理其他转义
                    switch (input[i]) {
                    case 'n': processedInput += '\n'; break;
                    case 't': processedInput += '\t'; break;
                    case 'r': processedInput += '\r'; break;
                    case '\\': processedInput += '\\'; break;
                    case '\"': processedInput += '\"'; break;
                    default:
                        // 保留原样
                        processedInput += '\\';
                        processedInput += input[i];
                        break;
                    }
                }
        } else {
            processedInput += input[i];
        }
    }

    // 然后进行正常的tokenize
    for (char c : processedInput) {
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


bool isValidInput(const std::string& text) {
    for (char c : text) {
        // 检查是否为ASCII字符
        if (!isascii(c)) return false;

        // 检查是否为不可见字符（包括空格是可⻅的）
        // iscntrl() 检查是否为控制字符（0-31, 127）
        if (iscntrl(static_cast<unsigned char>(c))) return false;

        // 检查是否为双引号
        if (c == '"') return false;
    }
    return true;
}

int main() {
    string s = "";
    // cout << s.empty();

    cin >> s;
    // tokenize(s);
    cout << isValidInput(s);

    return 0;
}