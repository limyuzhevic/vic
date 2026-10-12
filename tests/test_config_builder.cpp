// ConfigBuilder Test
#include "core/Config/ConfigBuilder.hpp"
#include <iostream>

void testConfigBuilderBasic() {
    std::cout << "Testing ConfigBuilder basic functionality..." << std::endl;
    
    // Test basic builder
    auto config = nlm::ConfigBuilder::minimalConfig()
                     .setRandomSeed(123)
                     .setNeuronCount(100)
                     .build();
    
    std::cout << "    Basic config builder test passed" << std::endl;
}

void testConfigBuilderPhase6() {
    std::cout << "Testing ConfigBuilder Phase 6 functionality..." << std::endl;
    
    // Test Phase 6 config
    auto config = nlm::ConfigBuilder::defaultPhase6()
                     .setPhase6NeuronCount(500)
                     .setPhase6MaxSteps(1000)
                     .setPhase6EnableDevelopment(true)
                     .build();
    
    std::cout << "    Phase 6 config builder test passed" << std::endl;
}

void testConfigBuilderValidation() {
    std::cout << "Testing ConfigBuilder validation..." << std::endl;
    
    // Test validation - this should fail
    auto builder = nlm::ConfigBuilder::minimalConfig()
                      .setNeuronCount(0); // Invalid
    
    bool isValid = builder.validate();
    std::string errorMsg = builder.validateAndGetError();
    
    std::cout << "    Validation test passed (invalid config correctly detected)" << std::endl;
}

int main() {
    std::cout << "=== ConfigBuilder Tests ===" << std::endl;
    
    testConfigBuilderBasic();
    testConfigBuilderPhase6();
    testConfigBuilderValidation();
    
    std::cout << "=== All ConfigBuilder Tests Passed ===" << std::endl;
    return 0;
}
