// Integration test main
#include "test_integration.cpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Integration Test ===" << std::endl;
    std::cout << "Testing system integration and interoperability..." << std::endl;
    
    try {
        test_integration::testIntegration();
        std::cout << "\n✓ Integration test passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Integration test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Integration test failed with unknown exception" << std::endl;
        return 1;
    }
}
