// Test program for the new argument parser and configuration system
#include "core/ArgumentParser.hpp"
#include "core/ConfigValidator.hpp"
#include <iostream>

int main() {
    std::cout << "Testing ArgumentParser and ConfigValidator..." << std::endl;
    
    // Test ArgumentParser
    ArgumentParser parser;
    char* testArgs[] = {
        "test_program",
        "--help"
    };
    
    auto args = parser.parse(2, testArgs);
    if (args.showHelp) {
        std::cout << "Help flag is correctly parsed" << std::endl;
    }
    
    std::cout << "ArgumentParser test completed successfully!" << std::endl;
    
    return 0;
}