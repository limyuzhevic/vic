// CLI Command Line Interface tests
// Test the unified CLI functionality

#include <iostream>
#include "cli/CLI.hpp"

void testCLI() {
    std::cout << "Testing CLI implementation..." << std::endl;
    
    // Test CLI instantiation
    nlm::UnifiedCLI cli;
    
    std::cout << "CLI test passed!" << std::endl;
}