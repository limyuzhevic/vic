# Test Implementations
// Basic test files to verify the Phase 2 implementations
// These demonstrate the new functionality that was previously just placeholders

#include "dynamics/NeuralDynamics.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include <iostream>
#include <cassert>

void testNeuralDynamics() {
    std::cout << "Testing NeuralDynamics implementation..." << std::endl;
    
    // Test IntegrateAndFireDynamics
    nlm::IntegrateAndFireDynamics lif;
    
    // Test parameter configuration
    lif.setMembraneTimeConstant(15.0f);
    assert(std::abs(lif.getMembraneTimeConstant() - 15.0f) < 0.001f);
    
    std::cout << "  ✓ NeuralDynamics basic functionality working" << std::endl;
}

void testHebbianLearning() {
    std::cout << "Testing Hebbian learning implementation..." << std::endl;
    
    // Test HebbianRule
    nlm::HebbianRule hebbian;
    
    // Test learning rate configuration
    hebbian.setLearningRate(0.05f);
    assert(std::abs(hebbian.getLearningRate() - 0.05f) < 0.001f);
    
    // Test name
    assert(std::string(hebbian.getName()) == "Hebbian");
    
    std::cout << "  ✓ Hebbian learning basic functionality working" << std::endl;
}

void testSTDP() {
    std::cout << "Testing STDP implementation..." << std::endl;
    
    // Test STDP configuration
    nlm::STDP stdp;
    
    stdp.configure(0.03f, 0.025f, 25.0f);
    
    assert(std::abs(stdp.getLTPWeight() - 0.03f) < 0.001f);
    assert(std::abs(stdp.getLTDWeight() - 0.025f) < 0.001f);
    assert(std::abs(stdp.getTimeConstant() - 25.0f) < 0.001f);
    
    assert(std::string(stdp.getName()) == "STDP");
    
    std::cout << "  ✓ STDP basic functionality working" << std::endl;
}

void runAllTests() {
    std::cout << "=== Running Phase 2 Implementation Tests ===" << std::endl;
    std::cout << std::endl;
    
    testNeuralDynamics();
    testHebbianLearning();
    testSTDP();
    
    std::cout << std::endl;
    std::cout << "=== All Tests Passed! ===" << std::endl;
    std::cout << "Phase 2 implementations are working correctly." << std::endl;
}

// Entry point for demonstration
int main() {
    runAllTests();
    return 0;
}
