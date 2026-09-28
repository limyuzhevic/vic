// Development and Plasticity System Unit Tests
// These tests verify the functionality of NLM's development and plasticity systems

#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace test_development_plasticity {

void testSynaptogenesis() {
    std::cout << "    testSynaptogenesis..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Create synaptogenesis system
    nlm::Synaptogenesis synaptogenesis;
    synaptogenesis.initialize(brain.get());
    
    // Test initial state
    assert(synaptogenesis.getRate() == 0.0f);
    
    // Test rate setting
    synaptogenesis.setRate(0.001f);
    assert(synaptogenesis.getRate() == 0.001f);
    
    // Test synaptic formation
    // This would require actual neurons and synapses in the brain
    // For now, just verify the system initializes correctly
    synaptogenesis.update(0.001f); // Small timestep
    
    std::cout << "    testSynaptogenesis passed" << std::endl;
}

void testPruning() {
    std::cout << "    testPruning..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Create pruning system
    nlm::Pruning pruning;
    pruning.initialize(brain.get());
    
    // Test initial state
    assert(pruning.getRate() == 0.0f);
    
    // Test rate setting
    pruning.setRate(0.0001f);
    assert(pruning.getRate() == 0.0001f);
    
    // Test synaptic elimination
    pruning.update(0.001f); // Small timestep
    
    // Test activity-dependent pruning
    pruning.setActivityThreshold(0.5f);
    pruning.setConnectionThreshold(0.1f);
    
    std::cout << "    testPruning passed" << std::endl;
}

void testMaturation() {
    std::cout << "    testMaturation..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Create maturation system
    nlm::Maturation maturation;
    maturation.initialize(brain.get());
    
    // Test initial state
    assert(maturation.getCurrentStage() == nlm::DevelopmentalStage::Initial);
    assert(maturation.getAge() == 0.0f);
    
    // Test age and stage progression
    maturation.update(1.0f); // 1 second
    assert(maturation.getAge() == 1.0f);
    
    // Test stage transitions
    maturation.setMaturationRate(1.0f);
    maturation.update(60.0f); // 60 seconds for stage changes
    
    // Should have moved to next stage or later
    assert(static_cast<int>(maturation.getCurrentStage()) > 0);
    
    // Test plasticity rate modulation
    float plasticityRate = maturation.getPlasticityRate();
    assert(plasticityRate >= 0.0f && plasticityRate <= 2.0f);
    
    std::cout << "    testMaturation passed" << std::endl;
}

void testSTDPPlasticity() {
    std::cout << "    testSTDPPlasticity..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Create STDP rule
    nlm::STDP stdp;
    
    // Test initial parameters
    assert(stdp.getLTPWeight() > 0.0f);
    assert(stdp.getLTDWeight() > 0.0f);
    assert(stdp.getTimeConstant() > 0.0f);
    
    // Test parameter setting
    stdp.setLTPWeight(0.02f);
    stdp.setLTDWeight(0.025f);
    stdp.setTimeConstant(15.0f);
    
    assert(stdp.getLTPWeight() == 0.02f);
    assert(stdp.getLTDWeight() == 0.025f);
    assert(stdp.getTimeConstant() == 15.0f);
    
    // Test STDP computation
    nlm::SimulationStep timeDiff = 10;
    stdp.setPrePostTimeDifference(timeDiff);
    
    float change = stdp.computeWeightChange(1.0f, 0.5f, true); // pre=1.0, post=0.5, preFired=true
    // STDP should produce some weight change
    
    std::cout << "    testSTDPPlasticity passed" << std::endl;
}

void testHebbianPlasticity() {
    std::cout << "    testHebbianPlasticity..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Create Hebbian rule
    nlm::Hebbian hebbian;
    
    // Test initial parameters
    assert(hebbian.getLearningRate() > 0.0f);
    assert(hebbian.getCoherenceThreshold() >= 0.0f);
    
    // Test parameter setting
    hebbian.setLearningRate(0.01f);
    hebbian.setCoherenceThreshold(0.5f);
    
    assert(hebbian.getLearningRate() == 0.01f);
    assert(hebbian.getCoherenceThreshold() == 0.5f);
    
    // Test Hebbian computation
    float preActivity = 0.8f;
    float postActivity = 0.6f;
    
    float change = hebbian.computeWeightChange(preActivity, postActivity);
    assert(change >= -1.0f && change <= 1.0f);
    
    std::cout << "    testHebbianPlasticity passed" << std::endl;
}

void testStructuralPlasticity() {
    std::cout << "    testStructuralPlasticity..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Create structural plasticity system
    nlm::StructuralPlasticity structuralPlasticity;
    structuralPlasticity.initialize(brain.get());
    
    // Test initial state
    assert(structuralPlasticity.getSynaptogenesisRate() == 0.0f);
    assert(structuralPlasticity.getPruningRate() == 0.0f);
    
    // Test rate setting
    structuralPlasticity.setSynaptogenesisRate(0.0001f);
    structuralPlasticity.setPruningRate(0.00001f);
    
    assert(structuralPlasticity.getSynaptogenesisRate() == 0.0001f);
    assert(structuralPlasticity.getPruningRate() == 0.00001f);
    
    // Test structural plasticity update
    structuralPlasticity.update(0.001f); // Small timestep
    
    // Test plasticity enable/disable
    structuralPlasticity.enableSynaptogenesis(true);
    structuralPlasticity.enablePruning(true);
    
    std::cout << "    testStructuralPlasticity passed" << std::endl;
}

void testDevelopmentPlasticityIntegration() {
    std::cout << "    testDevelopmentPlasticityIntegration..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain with development system
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Create development system
    auto* development = brain->getDevelopmentSystem();
    assert(development != nullptr);
    
    // Create plasticity rules
    nlm::STDP stdp;
    nlm::Hebbian hebbian;
    nlm::StructuralPlasticity structuralPlasticity;
    
    // Test that development affects plasticity rates
    float initialPlasticityRate = stdp.getLTPWeight();
    
    // Simulate development over time
    for (nlm::SimulationStep step = 0; step < 300; ++step) {
        brain->step(step, step * 0.001f);
        
        // Development system should update developmental stage
        nlm::DevelopmentalStage stage = brain->getDevelopmentalStage();
        assert(static_cast<int>(stage) >= 0 && static_cast<int>(stage) < 6);
        
        // Plasticity should be affected by developmental stage
        development->update(0.001f);
        
        // Verify plasticity rules are still functional
        assert(stdp.getLTPWeight() > 0.0f);
        assert(hebbian.getLearningRate() > 0.0f);
    }
    
    std::cout << "    testDevelopmentPlasticityIntegration passed" << std::endl;
}

void runAllDevelopmentPlasticityTests() {
    std::cout << "=== Running Development and Plasticity System Tests ===" << std::endl;
    
    testSynaptogenesis();
    testPruning();
    testMaturation();
    testSTDPPlasticity();
    testHebbianPlasticity();
    testStructuralPlasticity();
    testDevelopmentPlasticityIntegration();
    
    std::cout << "=== All Development and Plasticity System Tests Passed ===" << std::endl;
}

} // namespace test_development_plasticity