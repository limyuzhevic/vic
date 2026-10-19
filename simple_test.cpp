#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <optional>

// Simple test to verify syntax
using ConfigValue = std::variant<int, double, bool, std::string>;

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

int main() {
    std::cout << "Simple test passed!" << std::endl;
    return 0;
}
