// NLM Test Main
// Phase 2: Real Neural Computation Tests

#include <iostream>
#include "TestFramework.hpp"

int main() {
    try {
        nlm::test::TestRunner::getInstance().runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test execution failed: " << e.what() << std::endl;
        return 1;
    }
}
