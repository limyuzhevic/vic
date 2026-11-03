#include "AdvancedTools.hpp"
#include "AdvancedBatchTools.hpp"
#include <iostream>

int main() {
    std::cout << "NLM Advanced Tools - Compilation Test" << std::endl;
    std::cout << std::endl;
    
    // Test that we can create instances of our classes
    // Note: We can't actually instantiate these without proper brain configuration,
    // but we can at least verify that the headers compile correctly
    
    std::cout << "✓ AdvancedTools.hpp compiles successfully" << std::endl;
    std::cout << "✓ AdvancedBatchTools.hpp compiles successfully" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Basic compilation test passed!" << std::endl;
    return 0;
}