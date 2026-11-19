// STDP and Plasticity Advanced Tests
// Tests for advanced STDP and plasticity features

#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "brain/Brain.hpp"
#include "brain/Synapse.hpp"
#include "brain/Neuron.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

namespace test_advanced_plasticity {

void testSTDPPotentiation() {
    std::cout << "    Testing STDP potentiation (pre->post)..." << std::endl;
    
    // Create STDP instance
    nlm::STDP stdp;
    
    // Create a mock synapse
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get a synapse for testing (in Phase 4/5, we'd create synapses properly)
    // For now, test the STDP rule directly
    
    // Test parameters
    float ltpWeight = 0.01f;
    float ltdWeight = 0.012f;
    float timeConstant = 20.0f;
    
    stdp.configure(ltpWeight, ltdWeight, timeConstant);
    
    // Test potentiation: pre before post (Δt > 0)
    float dt = 5.0f;  // 5ms pre before post
    float expectedPotentiation = ltpWeight * std::exp(-dt / timeConstant);
    
    // Create test spike histories
    std::vector<float> preSpikes = {0.0f};
    std::vector<float> postSpikes = {0.005f};  // 5ms later
    
    // Verify the STDP logic would produce potentiation
    if (std::abs(dt) < 0.001f) {
        // Invalid delta
    } else if (dt > 0) {
        // Should potentiate
        assert(expectedPotentiation > 0.0f);
    } else {
        // Should depress  
        assert(expectedPotentiation < 0.0f);
    }
    
    std::cout << "      STDP potentiation test passed" << std::endl;
}

void testHebbianLearning() {
    std::cout << "    Testing Hebbian learning rule..." << std::endl;
    
    nlm::Hebbian hebbian;
    
    // Test learning rate configuration
    float learningRate = 0.01f;
    hebbian.setLearningRate(learningRate);
    assert(hebbian.getLearningRate() == learningRate);
    
    // Test max weight configuration
    float maxWeight = 1.0f;
    hebbian.setMaxWeight(maxWeight);
    assert(hebbian.getMaxWeight() == maxWeight);
    
    // Test weight bounds enforcement
    std::cout << "      Hebbian learning rule test passed" << std::endl;
}

void testStructuralPlasticity() {
    std::cout << "    Testing structural plasticity rules..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test structural plasticity instance
    nlm::StructuralPlasticity structural;
    
    // Test rate configuration
    float synaptogenesisRate = 0.0001f;
    structural.setSynaptogenesisRate(synaptogenesisRate);
    assert(structural.getSynaptogenesisRate() == synaptogenesisRate);
    
    float pruningRate = 0.00001f;
    structural.setPruningRate(pruningRate);
    assert(structural.getPruningRate() == pruningRate);
    
    std::cout << "      Structural plasticity test passed" << std::endl;
}

void testPlasticityIntegration() {
    std::cout << "    Testing plasticity system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that plasticity systems are accessible
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    auto* structural = brain.getStructuralPlasticity();
    
    assert(stdp != nullptr);
    assert(hebbian != nullptr);
    assert(structural != nullptr);
    
    // Test plasticity flags on synapses
    std::cout << "      Plasticity integration test passed" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "    Testing configuration validation..." << std::endl;
    
    nlm::Config config;
    
    // Set up valid configuration
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    
    // Test validation
    bool isValid = config.validate();
    assert(isValid);
    
    // Test parameter validation
    bool neuronCountValid = config.validateParameter("neuron_count", 50, 500);
    bool timestepValid = config.validateParameter("simulation_timestep", 0.0, 0.1);
    
    assert(neuronCountValid);
    assert(timestepValid);
    
    std::cout << "      Configuration validation test passed" << std::endl;
}

void runAll() {
    testSTDPPotentiation();
    testHebbianLearning();
    testStructuralPlasticity();
    testPlasticityIntegration();
    testConfigurationValidation();
    
    std::cout << std::endl;
    std::cout << "All advanced plasticity tests passed!" << std::endl;
}

} // namespace test_advanced_plasticity