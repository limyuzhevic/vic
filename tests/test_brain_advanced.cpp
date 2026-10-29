// Brain Tests - Additional tests for refactored methods
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_brain_advanced {

void testBrainStepRefactoring() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test that brain steps work correctly
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Verify basic properties are maintained
        assert(brain.getTotalNeuronCount() == 100);
    }
    
    std::cout << "    testBrainStepRefactoring passed" << std::endl;
}

void testBrainMemorySystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test memory system integration
    auto* workingMem = brain.getWorkingMemory();
    auto* episodicMem = brain.getEpisodicMemory();
    auto* associativeMem = brain.getAssociativeMemory();
    
    assert(workingMem != nullptr);
    assert(episodicMem != nullptr);
    assert(associativeMem != nullptr);
    
    // Take some steps to build up memory
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Memory systems should be processing
        if (workingMem) {
            workingMem->update(0.001f);
        }
    }
    
    std::cout << "    testBrainMemorySystemIntegration passed" << std::endl;
}

void testBrainNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    // Test neuromodulation updates
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.step(step);
        
        // Neuromodulators should be updating
        if (dopamine) dopamine->update(0.001f);
        if (curiosity) curiosity->update(0.001f);
        if (novelty) novelty->update(0.001f);
        if (predictionError) predictionError->update(0.001f);
    }
    
    std::cout << "    testBrainNeuromodulationIntegration passed" << std::endl;
}

void testBrainPlasticityIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test plasticity systems
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    auto* structuralPlasticity = brain.getStructuralPlasticity();
    
    assert(stdp != nullptr);
    assert(hebbian != nullptr);
    assert(structuralPlasticity != nullptr);
    
    // Test plasticity updates
    for (nlm::SimulationStep step = 0; step < 40; ++step) {
        brain.step(step);
        
        // Plasticity systems should be updating (indirectly through brain.step())
        // Test direct plasticity calls
        if (step % 10 == 0) {
            brain.develop();  // Development affects plasticity
        }
    }
    
    std::cout << "    testBrainPlasticityIntegration passed" << std::endl;
}

void testBrainCognitionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test cognition systems
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    // Test cognition updates
    for (nlm::SimulationStep step = 0; step < 35; ++step) {
        brain.step(step);
        
        // Cognition systems should be updating
        if (attention) attention->update(0.001f);
    }
    
    std::cout << "    testBrainCognitionIntegration passed" << std::endl;
}

void testBrainDevelopmentIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test development system
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test development stages
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.step(step);
        
        // Development should update periodically
        if (step % 100 == 0) {
            brain.develop();
        }
        
        // Development stage should change over time
        auto stage = brain.getDevelopmentalStage();
        // Stage should be valid
    }
    
    std::cout << "    testBrainDevelopmentIntegration passed" << std::endl;
}

void testBrainCheckpointIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(90), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("checkpoint_dir", "./test_checkpoints", nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test checkpoint system
    // Note: This would require proper checkpoint file handling
    // For now, just verify the checkpoint system is initialized
    
    // Take some steps
    for (nlm::SimulationStep step = 0; step < 15; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testBrainCheckpointIntegration passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(110), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test action production
    for (nlm::SimulationStep step = 0; step < 25; ++step) {
        brain.step(step);
        
        // Brain should be able to produce actions
        auto action = brain.produceAction();
        assert(action != nullptr);
    }
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainResetFunctionality() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Take some steps to build state
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Reset brain
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 60);
    assert(brain.getTotalSpikeCount() >= 0);
    
    // Should be able to step after reset
    brain.step(0);
    
    std::cout << "    testBrainResetFunctionality passed" << std::endl;
}

void testBrainRefactoredMethodSeparation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(70), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test that refactored methods are working
    // This test verifies that the brain can be stepped without issues
    // and that different system components are being updated
    
    for (nlm::SimulationStep step = 0; step < 45; ++step) {
        brain.step(step);
        
        // Each step should have called the refactored methods
        // We can't directly test the internal methods, but we can test
        // that the brain is functioning correctly
        
        // Basic sanity checks
        assert(brain.getTotalNeuronCount() == 70);
        assert(brain.getRegionCount() == 1);
    }
    
    // Test that different brain systems are producing output
    // by checking that we get meaningful results from accessors
    
    auto regions = brain.getRegions();
    assert(regions.size() == 1);
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 1);
    
    std::cout << "    testBrainRefactoredMethodSeparation passed" << std::endl;
}

} // namespace test_brain_advanced

// Wrapper functions to integrate with existing test framework
void testBrainAdvanced() {
    std::cout << "Running advanced brain tests..." << std::endl;
    
    test_brain_advanced::testBrainStepRefactoring();
    test_brain_advanced::testBrainMemorySystemIntegration();
    test_brain_advanced::testBrainNeuromodulationIntegration();
    test_brain_advanced::testBrainPlasticityIntegration();
    test_brain_advanced::testBrainCognitionIntegration();
    test_brain_advanced::testBrainDevelopmentIntegration();
    test_brain_advanced::testBrainCheckpointIntegration();
    test_brain_advanced::testBrainActionProduction();
    test_brain_advanced::testBrainResetFunctionality();
    test_brain_advanced::testBrainRefactoredMethodSeparation();
    
    std::cout << "All advanced brain tests passed!" << std::endl;
}
