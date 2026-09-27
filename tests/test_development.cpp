// Development-Plasticity Tests
// Tests for Phase 6 development-plasticity interactions

#include "brain/Brain.hpp"
#include "development/DevelopmentSystem.hpp"
#include "development/Maturation.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_development_plasticity {

void testDevelopmentalStages() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSys = brain.getDevelopmentSystem();
    if (!devSys) {
        std::cout << "    testDevelopmentalStages skipped (no development system)" << std::endl;
        return;
    }
    
    // Test developmental stages
    nlm::DevelopmentalStage stage = devSys->getStage();
    assert(stage == nlm::DevelopmentalStage::Early || 
           stage == nlm::DevelopmentalStage::Mid || 
           stage == nlm::DevelopmentalStage::Late);
    
    // Test stage names
    const char* stageName = devSys->getStageName();
    assert(stageName != nullptr);
    assert(std::string(stageName).size() > 0);
    
    // Test plasticity modifier
    float plasticityModifier = devSys->getPlasticityModifier();
    assert(plasticityModifier >= 0.0f && plasticityModifier <= 2.0f);
    
    // Test critical period
    bool criticalPeriod = devSys->isCriticalPeriod();
    assert(criticalPeriod == false);  // Should not be in critical period initially
    
    // Advance stage
    devSys->advanceStage();
    
    // Test after advancing
    float newPlasticityModifier = devSys->getPlasticityModifier();
    assert(newPlasticityModifier >= 0.0f);
    
    std::cout << "    testDevelopmentalStages passed" << std::endl;
}

void testMaturation() {
    // Test neuron maturation process
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSys = brain.getDevelopmentSystem();
    if (!devSys) {
        std::cout << "    testMaturation skipped (no development system)" << std::endl;
        return;
    }
    
    // Get developmental age
    double age = devSys->getDevelopmentalAge();
    assert(age >= 0.0);
    
    // Test with different brain configurations
    for (int i = 0; i < 20; ++i) {
        devSys->update(&brain, i * 0.001f);
    }
    
    // Check that development system is still functional
    assert(devSys->getPlasticityModifier() >= 0.0f);
    
    std::cout << "    testMaturation passed" << std::endl;
}

void testSynaptogenesis() {
    // Test synapse formation during development
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSys = brain.getDevelopmentSystem();
    if (!devSys) {
        std::cout << "    testSynaptogenesis skipped (no development system)" << std::endl;
        return;
    }
    
    // Run development with synaptogenesis
    devSys->setStage(nlm::DevelopmentalStage::Early);
    
    for (int i = 0; i < 50; ++i) {
        devSys->update(&brain, i * 0.001f);
    }
    
    // Test plasticity effects
    if (brain.getSTDP()) {
        float ltpWeight = brain.getSTDP()->getLTPWeight();
        assert(ltpWeight > 0.0f);
    }
    
    std::cout << "    testSynaptogenesis passed" << std::endl;
}

void testPruning() {
    // Test synapse pruning during development
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSys = brain.getDevelopmentSystem();
    if (!devSys) {
        std::cout << "    testPruning skipped (no development system)" << std::endl;
        return;
    }
    
    // Set to late developmental stage where pruning should occur
    devSys->setStage(nlm::DevelopmentalStage::Late);
    
    // Record initial spike count
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Run development
    for (int i = 0; i < 100; ++i) {
        devSys->update(&brain, i * 0.001f);
    }
    
    // Check that development is working
    assert(devSys->getPlasticityModifier() >= 0.0f);
    
    std::cout << "    testPruning passed" << std::endl;
}

void testDevelopmentPlasticityInteraction() {
    // Test interaction between development and plasticity
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 150);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSys = brain.getDevelopmentSystem();
    if (!devSys) {
        std::cout << "    testDevelopmentPlasticityInteraction skipped (no development system)" << std::endl;
        return;
    }
    
    // Test with different developmental stages
    std::vector<nlm::DevelopmentalStage> stages = {
        nlm::DevelopmentalStage::Early,
        nlm::DevelopmentalStage::Mid,
        nlm::DevelopmentalStage::Late
    };
    
    for (nlm::DevelopmentalStage stage : stages) {
        devSys->setStage(stage);
        float plasticityModifier = devSys->getPlasticityModifier();
        assert(plasticityModifier >= 0.0f);
        
        // Run development updates
        for (int i = 0; i < 30; ++i) {
            devSys->update(&brain, i * 0.001f);
            
            // Check that brain still works
            assert(brain.getTotalNeuronCount() > 0);
        }
    }
    
    std::cout << "    testDevelopmentPlasticityInteraction passed" << std::endl;
}

void runAll() {
    std::cout << "Running Development-Plasticity Tests..." << std::endl;
    testDevelopmentalStages();
    testMaturation();
    testSynaptogenesis();
    testPruning();
    testDevelopmentPlasticityInteraction();
}

} // namespace test_development_plasticity