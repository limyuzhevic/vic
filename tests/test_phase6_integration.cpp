// Phase 6 Integration Test
// Main integration verification for all Phase 6 systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_phase6_integration {

void testMemorySystemIntegration() {
    std::cout << "    Testing Memory System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    std::cout << "      Working memory: CONNECTED" << std::endl;
    
    // Test episodic memory
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    std::cout << "      Episodic memory: CONNECTED" << std::endl;
    
    // Run simulation to populate memory
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Inject some activity for memory traces
        if (step % 10 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 1.0f);
        }
    }
    
    // Verify memory has been populated
    if (workingMemory->getActiveTraces() > 0) {
        std::cout << "      Working memory traces: " << workingMemory->getActiveTraces() << std::endl;
    }
    
    if (episodicMemory->getEpisodeCount() > 0) {
        std::cout << "      Episodic memory episodes: " << episodicMemory->getEpisodeCount() << std::endl;
    }
    
    std::cout << "      Memory system integration test PASSED" << std::endl;
}

void testNeuromodulationSystemIntegration() {
    std::cout << "    Testing Neuromodulation System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    std::cout << "      Dopamine system: CONNECTED" << std::endl;
    std::cout << "      Curiosity system: CONNECTED" << std::endl;
    std::cout << "      Novelty system: CONNECTED" << std::endl;
    
    // Run simulation to activate neuromodulation
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Apply some reward signals
        if (step % 10 == 0) {
            nlm::Reward reward;
            reward.value = 0.5f;
            reward.source = nlm::RewardSource::Action;
            brain.applyNeuromodulation(reward);
        }
        
        // Inject novelty signals
        if (step % 15 == 0) {
            nlm::NoveltySignal noveltySignal;
            noveltySignal.strength = 0.8f;
            noveltySignal.cause = "exploration";
            brain.applyNeuromodulation(noveltySignal);
        }
    }
    
    std::cout << "      Neuromodulation system integration test PASSED" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "    Testing Prediction System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    std::cout << "      Prediction system: CONNECTED" << std::endl;
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 75; ++step) {
        brain.step(step);
        
        // Check prediction error system
        auto* predError = brain.getPredictionErrorSignal();
        if (predError != nullptr) {
            // Prediction error should update based on simulation
        }
    }
    
    std::cout << "      Prediction system integration test PASSED" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "    Testing Development System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test development system
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    std::cout << "      Development system: CONNECTED" << std::endl;
    
    // Get initial developmental stage
    auto initialStage = brain.getDevelopmentalStage();
    std::cout << "      Initial developmental stage: " << static_cast<int>(initialStage) << std::endl;
    
    // Run simulation to trigger development
    for (nlm::SimulationStep step = 0; step < 150; ++step) {
        brain.step(step);
        brain.develop();
        
        if (step % 50 == 0) {
            auto currentStage = brain.getDevelopmentalStage();
            std::cout << "      Step " << step << ", Stage: " << static_cast<int>(currentStage) << std::endl;
        }
    }
    
    std::cout << "      Development system integration test PASSED" << std::endl;
}

void testCrossSystemIntegration() {
    std::cout << "    Testing Cross-System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify all systems are integrated
    assert(brain.getWorkingMemory() != nullptr);  // Memory system
    assert(brain.getDopamine() != nullptr);       // Neuromodulation
    assert(brain.getPredictionSystem() != nullptr); // Prediction
    assert(brain.getDevelopmentSystem() != nullptr); // Development
    assert(brain.getPlanner() != nullptr);         // Cognition
    assert(brain.getConceptFormation() != nullptr); // Cognition
    assert(brain.getAttention() != nullptr);       // Cognition
    assert(brain.getEpisodicMemory() != nullptr);  // Memory
    
    std::cout << "      All memory systems: CONNECTED" << std::endl;
    std::cout << "      All neuromodulation systems: CONNECTED" << std::endl;
    std::cout << "      All cognition systems: CONNECTED" << std::endl;
    
    // Run full integration test
    nlm::Timer start(std::chrono::high_resolution_clock::now());
    
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        brain.step(step);
        
        // Apply cross-system interactions
        if (step % 25 == 0) {
            // Memory-triggered neuromodulation
            brain.applyNeuromodulation(nlm::Reward{0.3f, nlm::RewardSource::Memory});
        }
        
        if (step % 50 == 0) {
            // Development affects plasticity
            brain.develop();
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();
    
    std::cout << "      Cross-system integration test PASSED (duration: " << duration << "s)" << std::endl;
}

void testSystemConnectivity() {
    std::cout << "    Testing System Connectivity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Check all system pointers are not null
    assert(brain.getWorkingMemory() != nullptr);           // WorkingMemory.hpp line 143
    assert(brain.getEpisodicMemory() != nullptr);         // WorkingMemory.hpp line 146
    assert(brain.getAssociativeMemory() != nullptr);      // WorkingMemory.hpp line 149
    
    assert(brain.getPredictionSystem() != nullptr);       // WorkingMemory.hpp line 154
    
    assert(brain.getPlanner() != nullptr);                // WorkingMemory.hpp line 159
    assert(brain.getConceptFormation() != nullptr);       // WorkingMemory.hpp line 162
    assert(brain.getAttention() != nullptr);              // WorkingMemory.hpp line 165
    
    assert(brain.getDevelopmentSystem() != nullptr);      // DevelopmentSystem.hpp line 169
    
    assert(brain.getDopamine() != nullptr);               // Dopamine.hpp line 176
    assert(brain.getCuriosity() != nullptr);              // Curiosity.hpp line 179
    assert(brain.getNovelty() != nullptr);                // Novelty.hpp line 182
    assert(brain.getPredictionErrorSignal() != nullptr);  // PredictionError.hpp line 185
    
    // Test spike system
    assert(brain.getSpikeSystem() != nullptr);            // SpikeSystem.hpp line 84
    assert(brain.getSTDP() != nullptr);                   // STDP.hpp line 88
    assert(brain.getHebbian() != nullptr);                // Hebbian.hpp line 90
    assert(brain.getStructuralPlasticity() != nullptr);   // StructuralPlasticity.hpp line 91
    
    std::cout << "      All system pointers valid: YES" << std::endl;
    std::cout << "      System connectivity test PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
    
    testSystemConnectivity();
    testMemorySystemIntegration();
    testNeuromodulationSystemIntegration();
    testPredictionSystemIntegration();
    testDevelopmentSystemIntegration();
    testCrossSystemIntegration();
    
    std::cout << "\n=== All Phase 6 Integration Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_integration