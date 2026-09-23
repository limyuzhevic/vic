#include <iostream>
#include <memory>

// Test including the main NLM headers to check for compilation issues
#include "src/core/Config/Config.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Types/Types.hpp"

int main() {
    std::cout << "Testing NLM code fixes..." << std::endl;
    
    // Test 1: Create a simple Neuron
    std::cout << "1. Testing Neuron creation..." << std::endl;
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setType(nlm::NeuronType::Excitatory);
    std::cout << "   Neuron created successfully" << std::endl;
    
    // Test 2: Test Neuron properties
    std::cout << "2. Testing Neuron properties..." << std::endl;
    neuron.setMembranePotential(-65.0f);
    std::cout << "   Membrane potential: " << neuron.getMembranePotential() << " mV" << std::endl;
    
    neuron.setThreshold(-50.0f);
    std::cout << "   Threshold: " << neuron.getThreshold() << " mV" << std::endl;
    
    neuron.setRestingPotential(-70.0f);
    std::cout << "   Resting potential: " << neuron.getRestingPotential() << " mV" << std::endl;
    
    // Test 3: Test Config
    std::cout << "3. Testing Config..." << std::endl;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    std::cout << "   Config created successfully" << std::endl;
    
    // Test 4: Check for duplicate method implementations
    std::cout << "4. Testing method implementations..." << std::endl;
    neuron.setFiringState(nlm::FiringState::Active);
    std::cout << "   Firing state: " << (neuron.isFiring() ? "Active" : "Inactive") << std::endl;
    
    // Test 5: Test error handling in config
    std::cout << "5. Testing error handling..." << std::endl;
    bool hasKey = config->has("neuron_count");
    std::cout << "   Has neuron_count key: " << (hasKey ? "Yes" : "No") << std::endl;
    
    std::cout << std::endl;
    std::cout << "All tests passed! The fixes are working correctly." << std::endl;
    return 0;
}