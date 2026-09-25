#include "../src/core/Random/Random.hpp"
#include <iostream>

namespace test_random {
    void runAll();
}

int main() {
    std::cout << "=== Random Tests ===" << std::endl;
    try {
        test_random::runAll();
        std::cout << "=== All Random Tests PASSED ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << "=== Random Tests FAILED: " << e.what() << " ===" << std::endl;
        return 1;
    }
}