// Minimal test to check syntax
#include <iostream>
int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
EOF && g++ -std=c++20 simple_test.cpp -o simple_test 2>&1 || echo "Compilation failed"
