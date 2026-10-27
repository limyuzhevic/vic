// Convenience functions test main
#include "test_convenience.cpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Convenience Functions Test ===" << std::endl;
    std::cout << "Testing new convenience functions and helper methods..." << std::endl;
    
    try {
        test_convenience::testConvenienceFunctions();
        std::cout << "\n✓ Convenience functions test passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Convenience functions test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Convenience functions test failed with unknown exception" << std::endl;
        return 1;
    }
}
