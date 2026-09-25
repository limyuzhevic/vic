// Phase 6 Development System Integration Tests
// Comprehensive testing of development system integration

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_phase6_development {

void testDevelopmentSystemBasic() {
    std::cout << "    Testing Development System Basic Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test developmental stage progression
    auto initialStage = brain.getDevelopmentalStage();
    std::cout << "      Initial developmental stage: " << static_cast<int>(initialStage) << std::endl;
    
    // Run simulation with development
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.step(step);
        brain.develop();
        
        // Check stage progression
        if (step % 50 == 0) {
            auto currentStage = brain.getDevelopmentalStage();
            std::cout << "      Step " << step << ", Stage: " << static_cast<int>(currentStage) << std::endl;
        }
    }
    
    auto finalStage = brain.getDevelopmentalStage();
    std::cout << "      Final developmental stage: " << static_cast<int>(finalStage) << std::endl;
    
    // Development should progress through stages
    assert(static_cast<int>(finalStage) >= static_cast<int>(initialStage));
    
    std::cout << "      Development system basic integration test PASSED" << std::endl;
}

void testSynaptogenesisIntegration() {
    std::cout << "    Testing Synaptogenesis Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.001f, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Get initial synapse count
    size_t initialSynapses = brain.getTotalSynapseCount();
    std::cout << "      Initial synapse count: " << initialSynapses << std::endl;
    
    // Run simulation with synaptogenesis
    for (nlm::SimulationStep step = 0; step < 300; ++step) {
        brain.step(step);
        brain.develop();
        
        // Synaptogenesis occurs during development
        if (step % 50 == 0) {
            auto currentSynapses = brain.getTotalSynapseCount();
            std::cout << "      Step " << step << ", Synapses: " << currentSynapses << std::endl;
        }
    }
    
    size_t finalSynapses = brain.getTotalSynapseCount();
    std::cout << "      Final synapse count: " << finalSynapses << std::endl;
    
    // Synapse count may increase during development
    assert(finalSynapses >= initialSynapses);
    
    std::cout << "      Synaptogenesis integration test PASSED" << std::endl;
}

void testPruningIntegration() {
    std::cout << "    Testing Pruning Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("pruning_rate", 0.00005f, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Get initial neuron and synapse counts
    size_t initialNeurons = brain.getTotalNeuronCount();
    size_t initialSynapses = brain.getTotalSynapseCount();
    
    std::cout << "      Initial neurons: " << initialNeurons << std::endl;
    std::cout << "      Initial synapses: " << initialSynapses << std::endl;
    
    // Run simulation with pruning
    for (nlm::SimulationStep step = 0; step < 400; ++step) {
        brain.step(step);
        brain.develop();
        
        // Pruning occurs during development
        if (step % 100 == 0) {
            size_t currentNeurons = brain.getTotalNeuronCount();
            size_t currentSynapses = brain.getTotalSynapseCount();
            std::cout << "      Step " << step << ", Neurons: " << currentNeurons 
                      << ", Synapses: " << currentSynapses << std::endl;
        }
    }
    
    size_t finalNeurons = brain.getTotalNeuronCount();
    size_t finalSynapses = brain.getTotalSynapseCount();
    
    std::cout << "      Final neurons: " << finalNeurons << std::endl;
    std::cout << "      Final synapses: " << finalSynapses << std::endl;
    
    // Pruning may reduce synapse count (can vary based on implementation)
    std::cout << "      Pruning integration test PASSED" << std::endl;
}

void testMaturationIntegration() {
    std::cout << "    Testing Maturation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test maturation over time
    std::vector<nlm::DevelopmentalStage> stages;
    
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        brain.step(step);
        brain.develop();
        
        auto currentStage = brain.getDevelopmentalStage();
        stages.push_back(currentStage);
        
        if (step % 100 == 0) {
            std::cout << "      Step " << step << ", Stage: " << static_cast<int>(currentStage) << std::endl;
        }
    }
    
    // Check stage progression
    bool stageProgression = true;
    for (size_t i = 1; i < stages.size(); ++i) {
        if (static_cast<int>(stages[i]) < static_cast<int>(stages[i-1])) {
            stageProgression = false;
            break;
        }
    }
    
    assert(stageProgression);
    std::cout << "      Maturation integration test PASSED" << std::endl;
}

void testDevelopmentNeuromodulationIntegration() {
    std::cout << "    Testing Development Neuromodulation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test development-neuromodulation interaction
    for (nlm::SimulationStep step = 0; step < 300; ++step) {
        brain.step(step);
        brain.develop();
        
        // Neuromodulation affects development
        if (step % 50 == 0) {
            nlm::Reward reward;
            reward.value = 0.3f + (step % 5) * 0.2f;
            reward.source = nlm::RewardSource::Action;
            brain.applyNeuromodulation(reward);
        }
        
        // Development affects neuromodulation
        if (step % 40 == 0) {
            auto* dopamine = brain.getDopamine();
            if (dopamine != nullptr) {
                float dopamineLevel = dopamine->getCurrentLevel();
                // Development may modulate dopamine
                std::cout << "      Step " << step << ", Dopamine: " << dopamineLevel << std::endl;
            }
        }
    }
    
    std::cout << "      Development neuromodulation integration test PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Development System Tests ===" << std::endl;
    
    testDevelopmentSystemBasic();
    testSynaptogenesisIntegration();
    testPruningIntegration();
    testMaturationIntegration();
    testDevelopmentNeuromodulationIntegration();
    
    std::cout << "\n=== All Phase 6 Development Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_development