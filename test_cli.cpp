#include "core/CLIHandler.hpp"
#include <iostream>

int main(int argc, char** argv) {
    std::cout << "=== Testing NLM CLI Handler ===" << std::endl;
    
    nlm::CLIHandler cliHandler(argc, argv);
    
    bool success = cliHandler.parseCommandLine();
    
    if (success) {
        std::cout << "✓ CLI parsing completed successfully" << std::endl;
        return 0;
    } else {
        std::cout << "✗ CLI parsing failed" << std::endl;
        return 1;
    }
}
