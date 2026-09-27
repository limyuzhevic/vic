// Main test runner for all configuration tests
#include <iostream>
#include "test_config.cpp"
#include "test_configuration_manager.cpp"
#include "test_nested_configuration.cpp"
#include "test_schema.cpp"

void runAllTests() {
    std::cout << "Running all configuration tests..." << std::endl << std::endl;
    
    std::cout << "=== Configuration Tests ===" << std::endl;
    test_config::runAll();
    
    std::cout << std::endl << "=== Configuration Manager Tests ===" << std::endl;
    test_configuration_manager::runAll();
    
    std::cout << std::endl << "=== Nested Configuration Tests ===" << std::endl;
    test_nested_configuration::runAll();
    
    std::cout << std::endl << "=== Schema Tests ===" << std::endl;
    test_schema::runAll();
    
    std::cout << std::endl << "=== All tests completed ===" << std::endl;
}

int main() {
    try {
        runAllTests();
        std::cout << "\nAll configuration tests completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nTest failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\nTest failed with unknown exception." << std::endl;
        return 1;
    }
}
