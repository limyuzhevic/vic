// Memory management test main
#include "test_memory.cpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Memory Management Test ===" << std::endl;
    std::cout << "Testing memory safety and resource management..." << std::endl;
    
    try {
        test_memory::testMemoryManagement();
        std::cout << "\n✓ Memory management test passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Memory management test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Memory management test failed with unknown exception" << std::endl;
        return 1;
    }
}
