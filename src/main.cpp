#include <iostream>
#include <string>
#include "../include/bookstore.h"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    Bookstore bookstore;
    bookstore.run();

    return 0;
}