#include <iostream>
#include <string>
#include "../include/token.h"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    while (!std::cin.eof()) {
        std::string desire;
        std::getline(std::cin, desire);

        const std::vector<std::string> tokens = tokenize(desire);
        if (tokens.empty()) {
            continue;
        }

        std::string command = tokens[0];

        if (command == "exit" || command == "quit") {
            if (tokens.size() > 1) {
                std::cout << "Invalid\n";
            }
            else {
                break;
            }
        }
        if (command == "su") {
            if (tokens.size() < 2 || tokens.size() > 3) {
                std::cout << "Invalid\n";
                continue;
            }

            std::string user_id = tokens[1];
            std::string password = "";
            if (tokens.size() == 3) {
                password = tokens[2];
            }
        }
    }



    return 0;
}