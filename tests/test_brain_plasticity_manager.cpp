// BrainPlasticityManager Comprehensive Tests
// Tests STDP, Hebbian, StructuralPlasticity

#include "brain/BrainPlasticityManager.hpp"
#include "brain/BrainNeuralCore.hpp"
#include "core/Config/Config.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "core/Random/Random.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

namespace test_brain_plasticity_manager {

void testBrainPlasticityManagerCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::BrainPlasticityManager plasticity(config);
    
    // Create neural core for testing
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    
    assert(plasticity.initialize());
    
    std::cout << "    testBrainPlasticityManagerCreation passed" << std::endl;
}

void testSTDPIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainPlasticityManager plasticity(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(plasticity.initialize());
    
    // Get STDP component
    nlm::STDP* stdp = plasticity.getSTDP();
    assert(stdp != nullptr);
    
    // Test STDP configuration
    assert(stdp->getLTPWeight() == 0.01f);
    assert(stdp->getLTDWeight() == 0.012f);
    assert(stdp->getTimeConstant() == 20.0f);
    
    // Configure STDP
    plasticity.configureSTDP(0.05f, 0.06f, 30.0f);
    
    assert(stdp->getLTPWeight() == 0.05f);
    assert(stdp->getLTDWeight() == 0.06f);
    assert(stdp->getTimeConstant() == 30.0f);
    
    // Test STDP potentiation
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive dt) - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // ms (10ms after pre)
    
    stdp->update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-before-post
    assert(finalWeight > initialWeight);
    
    std::cout << "    testSTDPIntegration passed" << std::endl;
}

void testHebbianIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainPlasticityManager plasticity(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(plasticity.initialize());
    
    // Get Hebbian component
    nlm::Hebbian* hebbian = plasticity.getHebbian();
    assert(hebbian != nullptr);
    
    // Test Hebbian configuration
    assert(plasticity.getHebbianLearningRate() > 0.0f);
    
    // Set Hebbian learning rate
    plasticity.setHebbianLearningRate(0.5f);
    assert(plasticity.getHebbianLearningRate() == 0.5f);
    
    std::cout << "    testHebbianIntegration passed" << std::endl;
}

void testStructuralPlasticityIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::BrainPlasticityManager plasticity(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(plasticity.initialize());
    
    // Get StructuralPlasticity component
    nlm::StructuralPlasticity* structural = plasticity.getStructuralPlasticity();
    assert(structural != nullptr);
    
    // Test structural plasticity configuration
    assert(plasticity.getSynaptogenesisRate() > 0.0f);
    assert(plasticity.getPruningRate() > 0.0f);
    
    // Configure structural plasticity rates
    plasticity.setSynaptogenesisRate(0.1f);
    plasticity.setPruningRate(0.05f);
    
    assert(plasticity.getSynaptogenesisRate() == 0.1f);
    assert(plasticity.getPruningRate() == 0.05f);
    
    // Test structural plasticity update (requires neural core and random generator)
    nlm::RandomGenerator rng(42);
    std::unique_ptr<nlm::RandomGenerator> rngPtr = std::make_unique<nlm::RandomGenerator>(rng);
    
    plasticity.updateStructuralPlasticity(&neuralCore, rngPtr);
    
    std::cout << "    testStructuralPlasticityIntegration passed" << std::endl;
}

void testNeuromodulationPlasticityInteraction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainPlasticityManager plasticity(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(plasticity.initialize());
    
    // Create a neuromodulator signal (simplified)
    nlm::Neuromodulator signal;
    signal.setType(nlm::ModulatorType::Dopamine);
    signal.setValue(1.0f);  // Reward signal
    
    // Apply neuromodulation effects
    plasticity.applyNeuromodulation(signal);
    
    std::cout << "    testNeuromodulationPlasticityInteraction passed" << std::endl;
}

void testDevelopmentPlasticityInteraction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainPlasticityManager plasticity(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(plasticity.initialize());
    
    // Create a developmental stage (simplified)
    nlm::DevelopmentalStage stage;
    stage.age = 10.0; // 10ms developmental age
    
    // Apply developmental effects
    plasticity.applyDevelopment(stage);
    
    std::cout << "    testDevelopmentPlasticityInteraction passed" << std::endl;
}

void testPlasticityReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainPlasticityManager plasticity(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(plasticity.initialize());
    
    // Apply some plasticity
    nlm::Neuromodulator signal;
    signal.setType(nlm::ModulatorType::Dopamine);
    signal.setValue(0.5f);
    plasticity.applyNeuromodulation(signal);
    
    // Reset plasticity
    plasticity.reset();
    
    std::cout << "    testPlasticityReset passed" << std::endl;
}

void testPlasticityStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainPlasticityManager plasticity(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(plasticity.initialize());
    
    // Test parameter access
    float ltpWeight = plasticity.getLTPWeight();
    float ltdWeight = plasticity.getLTDWeight();
    float timeConstant = plasticity.getTimeConstant();
    
    assert(ltpWeight > 0.0f);
    assert(ltdWeight > 0.0f);
    assert(timeConstant > 0.0f);
    
    std::cout << "    testPlasticityStatus passed" << std::endl;
}

void runAll() {
    std::cout << "Running BrainPlasticityManager tests..." << std::endl;
    
    testBrainPlasticityManagerCreation();
    testSTDPIntegration();
    testHebbianIntegration();
    testStructuralPlasticityIntegration();
    testNeuromodulationPlasticityInteraction();
    testDevelopmentPlasticityInteraction();
    testPlasticityReset();
    testPlasticityStatus();
    
    std::cout << "\nAll BrainPlasticityManager tests passed!" << std::endl;
}

} // namespace test_brain_plasticity_manager
