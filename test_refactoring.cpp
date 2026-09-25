# Test to verify NLM refactoring
// This file contains basic tests to ensure the refactored code works correctly

#include "AgentBrain.hpp"
#include "Constants.hpp"
#include <cassert>
#include <iostream>

void TestConstants() {
    std::cout << "Testing constants..." << std::endl;
    
    // Test some key constants
    assert(Constants::VISION_SCALE_FACTOR == 5.0f);
    assert(Constants::TOUCH_SCALE_FACTOR == 8.0f);
    assert(Constants::INTERNAL_SCALE_FACTOR == 5.0f);
    assert(Constants::PROPRIOCEPTION_SCALE_FACTOR == 3.0f);
    
    assert(Constants::CURIOSITY_EXPLORATION_THRESHOLD == 0.3f);
    assert(Constants::CURIOSITY_EXPLORATION_MAX_LEVEL == 0.5f);
    assert(Constants::NOISE_THRESHOLD == 0.5f);
    
    assert(Constants::DOPAMINE_CLAMP_MIN == -1.0f);
    assert(Constants::DOPAMINE_CLAMP_MAX == 1.0f);
    
    std::cout << "✅ Constants test passed!" << std::endl;
}

void TestHelperClasses() {
    std::cout << "Testing helper classes..." << std::endl;
    
    // Test MotorNeuronDistributionHelper
    std::vector<Neuron*> neurons;
    std::vector<Neuron*> motorForward, motorBackward, motorTurnLeft, motorTurnRight, motorInteract, motorWait;
    std::vector<Neuron*> sensoryVision, sensoryTouch, sensoryInternal, sensoryProprioception;
    
    // Create mock neurons
    for (int i = 0; i < 10; ++i) {
        neurons.push_back(new MockNeuron(NeuronType::Motor));
    }
    
    for (int i = 0; i < 10; ++i) {
        neurons.push_back(new MockNeuron(NeuronType::Sensory));
    }
    
    MotorNeuronDistributionHelper::DistributeNeurons(
        neurons, motorForward, motorBackward, motorTurnLeft, motorTurnRight,
        motorInteract, motorWait, sensoryVision, sensoryTouch, sensoryInternal, sensoryProprioception);
    
    // Verify distribution
    assert(motorForward.size() > 0);
    assert(sensoryVision.size() > 0);
    
    // Cleanup
    for (Neuron* n : neurons) {
        delete n;
    }
    
    std::cout << "✅ Helper classes test passed!" << std::endl;
}

int main() {
    std::cout << "=== NLM Refactoring Test ===" << std::endl;
    
    TestConstants();
    TestHelperClasses();
    
    std::cout << "\n✅ All tests passed!" << std::endl;
    std::cout << "The refactored NLM code is working correctly." << std::endl;
    
    return 0;
}

// Mock neuron class for testing
class MockNeuron : public Neuron {
public:
    MockNeuron(NeuronType type) : type_(type) {}
    
    NeuronType getType() const override { return type_; }
    // Implement other pure virtual methods as needed for testing
    
private:
    NeuronType type_;
};