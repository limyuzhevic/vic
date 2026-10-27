// API surface test main
#include "test_api.cpp"
#include <iostream>

int main() {
    std::cout << "=== NLM API Surface Test ===" << std::endl;
    std::cout << "Testing API completeness and usability..." << std::endl;
    
    try {
        test_api::testAPISurface();
        std::cout << "\n✓ API surface test passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ API surface test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n✗ API surface test failed with unknown exception" << std::endl;
        return 1;
    }
}
