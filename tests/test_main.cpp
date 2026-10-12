// Test main
#include <iostream>

int main() {
    std::cout << "Running Config Validation Tests..." << std::endl;
    
    try {
        test_config::runAll();
        std::cout << std::endl;
        std::cout << "All Config Validation Tests PASSED!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << "Test FAILED with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << std::endl;
        std::cerr << "Test FAILED with unknown exception!" << std::endl;
        return 1;
    }
}