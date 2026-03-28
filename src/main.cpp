#include <string>
#include <iostream>
#include "vector.hpp"
#include "b_plus_tree.hpp"
#include "mystring.hpp"

int n, val;
std::string ind, command;
sjtu::b_plus_tree<my_string, int> tree;

int main() {
    std::cin >> n;
    while (n--) {
        std::cin >> command;
        if (command == "insert") {
            std::cin >> ind >> val;
            tree.insert(my_string(ind), val);
        } else if (command == "delete") {
            std::cin >> ind >> val;
            tree.erase(my_string(ind), val);
        } else if (command == "find") {
            std::cin >> ind;
            sjtu::vector<int> result = tree.find(my_string(ind));
            if (result.empty()) {
                std::cout << "null\n";
                continue;
            }
            for (size_t i = 0; i < result.size(); ++i) {
                std::cout << result[i] << ' ';
            }
            std::cout << '\n';
        }
    }

    return 0;
}