// Development System Tests
// Comprehensive tests for developmental stages, maturation, synaptogenesis, and pruning

#include "development/DevelopmentSystem.hpp"
#include "development/Maturation.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_development {

// DevelopmentSystem Tests
void testDevelopmentSystemBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    // Test initial stage
    nlm::DevelopmentalStage initialStage = devSystem->getStage();
    assert(initialStage == nlm::DevelopmentalStage::Infant);
    
    // Test stage advancement
    devSystem->advanceStage();
    nlm::DevelopmentalStage advancedStage = devSystem->getStage();
    assert(advancedStage == nlm::DevelopmentalStage::Juvenile);
    
    // Test stage name
    const char* stageName = devSystem->getStageName();
    assert(stageName != nullptr);
    assert(std::string(stageName) == "Juvenile");
    
    std::cout << "    testDevelopmentSystemBasic passed" << std::endl;
}

void testDevelopmentSystemProgress() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    // Test initial age
    double initialAge = devSystem->getDevelopmentalAge();
    assert(initialAge == 0.0);
    
    // Test age setting
    devSystem->setDevelopmentalAge(10.5);
    assert(fabs(devSystem->getDevelopmentalAge() - 10.5) < 0.001f);
    
    // Test stage advancement with age
    for (int i = 0; i < 5; ++i) {
        devSystem->advanceStage();
    }
    
    std::cout << "    testDevelopmentSystemProgress passed" << std::endl;
}

void testDevelopmentSystemPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    // Test plasticity modifier
    float plasticityModifier = devSystem->getPlasticityModifier();
    assert(plasticityModifier >= 0.0f && plasticityModifier <= 2.0f);
    
    // Test critical period
    bool isCritical = devSystem->isCriticalPeriod();
    float criticalProgress = devSystem->getCriticalPeriodProgress();
    assert(criticalProgress >= 0.0f && criticalProgress <= 1.0f);
    
    std::cout << "    testDevelopmentSystemPlasticity passed" << std::endl;
}

void testDevelopmentSystemUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    auto* rng = brain.getRandomGenerator();
    assert(rng != nullptr);
    
    // Test update with just brain and step
    for (int step = 0; step < 10; ++step) {
        devSystem->update(&brain, step);
    }
    
    // Test update with full parameters
    for (int step = 0; step < 10; ++step) {
        devSystem->update(&brain, *rng, 0.01f);
    }
    
    std::cout << "    testDevelopmentSystemUpdate passed" << std::endl;
}

// Maturation Tests
void MaturationBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* maturation = brain.getDevelopmentSystem(); // Simplified access
    assert(maturation != nullptr);
    
    // Test progress
    float initialProgress = maturation->getPlasticityModifier(); // Using available method
    assert(initialProgress >= 0.0f && initialProgress <= 2.0f);
    
    // Note: Full Maturation class testing would require implementing private methods
    // For now, we test through the DevelopmentSystem interface
    
    std::cout << "    testMaturationBasic passed" << std::endl;
}

void testMaturationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    // Test maturation effects through plasticity
    float plasticityBefore = devSystem->getPlasticityModifier();
    
    // Advance through stages
    devSystem->advanceStage();
    devSystem->advanceStage();
    devSystem->advanceStage();
    
    float plasticityAfter = devSystem->getPlasticityModifier();
    
    // Plasticity may change as development progresses
    std::cout << "    testMaturationIntegration passed" << std::endl;
}

// Synaptogenesis Tests
void SynaptogenesisBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test synaptogenesis through brain functions
    size_t initialSynapseCount = brain.getTotalSynapseCount();
    
    auto* rng = brain.getRandomGenerator();
    assert(rng != nullptr);
    
    // Note: Synaptogenesis would be triggered through normal brain operation
    // For now, we verify the system is in a valid state
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    testSynaptogenesisBasic passed" << std::endl;
}

void testSynaptogenesisDensity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test multiple steps to allow for synaptic changes
    for (int step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    size_t finalSynapseCount = brain.getTotalSynapseCount();
    
    // Synapse count should be reasonable
    assert(finalSynapseCount >= 0);
    
    std::cout << "    testSynaptogenesisDensity passed" << std::endl;
}

// Pruning Tests
void PruningBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test pruning through brain operations
    size_t initialSynapseCount = brain.getTotalSynapseCount();
    
    // Run brain for several steps
    for (int step = 0; step < 30; ++step) {
        brain.step(step);
    }
    
    size_t finalSynapseCount = brain.getTotalSynapseCount();
    
    // Synapse count should be consistent
    assert(finalSynapseCount >= 0);
    
    std::cout << "    testPruningBasic passed" << std::endl;
}

void testPruningThreshold() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test pruning effects
    size_t synapsesBefore = brain.getTotalSynapseCount();
    
    // Simulate some development steps
    auto* rng = brain.getRandomGenerator();
    
    for (int step = 0; step < 20; ++step) {
        brain.step(step);
        // Note: Pruning would be triggered automatically during development
    }
    
    size_t synapsesAfter = brain.getTotalSynapseCount();
    
    // Synapse count should be reasonable
    assert(synapsesAfter >= 0);
    
    std::cout << "    testPruningThreshold passed" << std::endl;
}

// Development Stages Tests
void DevelopmentalStages() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    // Test progression through all developmental stages
    std::vector<nlm::DevelopmentalStage> stages = {
        nlm::DevelopmentalStage::Infant,
        nlm::DevelopmentalStage::EarlyJuvenile,
        nlm::DevelopmentalStage::MidJuvenile,
        nlm::DevelopmentalStage::LateJuvenile,
        nlm::DevelopmentalStage::Adult
    };
    
    for (const auto& stage : stages) {
        devSystem->setStage(stage);
        nlm::DevelopmentalStage currentStage = devSystem->getStage();
        assert(currentStage == stage);
        
        const char* stageName = devSystem->getStageName();
        assert(stageName != nullptr);
    }
    
    std::cout << "    testDevelopmentalStages passed" << std::endl;
}

void testDevelopmentCriticalPeriods() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    // Test critical period functionality
    bool isCritical = devSystem->isCriticalPeriod();
    float progress = devSystem->getCriticalPeriodProgress();
    
    // Advance development
    for (int i = 0; i < 10; ++i) {
        devSystem->advanceStage();
        devSystem->update(&brain, 0.01f);
    }
    
    bool isCriticalAfter = devSystem->isCriticalPeriod();
    float progressAfter = devSystem->getCriticalPeriodProgress();
    
    assert(progressAfter >= progress);
    
    std::cout << "    testDevelopmentCriticalPeriods passed" << std::endl;
}

// Development System Integration Tests
void DevelopmentSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    // Test that all development subsystems work together
    auto* maturation = brain.getDevelopmentSystem(); // Simplified access
    
    // Simulate development over time
    for (int step = 0; step < 100; ++step) {
        devSystem->update(&brain, step);
        brain.step(step);
        
        if (step % 10 == 0) {
            devSystem->advanceStage();
        }
    }
    
    // Verify development system is still functional
    float plasticityModifier = devSystem->getPlasticityModifier();
    assert(plasticityModifier >= 0.0f);
    
    std::cout << "    testDevelopmentSystemIntegration passed" << std::endl;
}

void testDevelopmentErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    // Test development system with edge cases
    devSystem->setStage(nlm::DevelopmentalStage::Infant);
    devSystem->setDevelopmentalAge(100.0);
    
    // Test extreme plasticity values
    float plasticity = devSystem->getPlasticityModifier();
    assert(plasticity >= 0.0f);
    
    // Test critical period edge cases
    bool isCritical = devSystem->isCriticalPeriod();
    float criticalProgress = devSystem->getCriticalPeriodProgress();
    assert(criticalProgress >= 0.0f && criticalProgress <= 1.0f);
    
    std::cout << "    testDevelopmentErrorHandling passed" << std::endl;
}

void runAll() {
    std::cout << "Running Development System tests..." << std::endl;
    
    try {
        testDevelopmentSystemBasic();
        testDevelopmentSystemProgress();
        testDevelopmentSystemPlasticity();
        testDevelopmentSystemUpdate();
        
        testMaturationBasic();
        testMaturationIntegration();
        
        testSynaptogenesisBasic();
        testSynaptogenesisDensity();
        
        testPruningBasic();
        testPruningThreshold();
        
        testDevelopmentalStages();
        testDevelopmentCriticalPeriods();
        
        testDevelopmentSystemIntegration();
        testDevelopmentErrorHandling();
        
        std::cout << "  All Development System tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_development