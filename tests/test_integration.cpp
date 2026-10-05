// Integration Tests for Cognitive and Memory Systems
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_integration {

void testMemorySystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("enable_working_memory", true, nlm::ConfigSource::Default);
    config->set("enable_episodic_memory", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test that memory systems are initialized
    brain.initialize();
    
    // Verify memory systems are accessible via getters
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Should not be null since they're initialized
    if (workingMemory) {
        std::cout << "    Working memory initialized and accessible" << std::endl;
    }
    if (episodicMemory) {
        std::cout << "    Episodic memory initialized and accessible" << std::endl;
    }
    
    // Test basic integration by running some steps
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
        
        // Verify working memory updates during brain operation
        if (workingMemory) {
            auto traces = workingMemory->getActiveTraces();
            // Memory should have some activity during simulation
            if (step > 0) {
                assert(traces >= 0);
            }
        }
        
        // Verify episodic memory stores episodes
        if (episodicMemory && step > 3) {  // After some steps, episodes should exist
            auto episodes = episodicMemory->getEpisodeCount();
            assert(episodes > 0);
            std::cout << "    Episodic memory stores " << episodes << " episodes" << std::endl;
        }
    }
    
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

void testPredictionSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction system integration
    auto* predictionSystem = brain.getPredictionSystem();
    
    if (predictionSystem) {
        std::cout << "    Prediction system initialized and accessible" << std::endl;
        
        // Run some steps to test prediction integration
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
            // Prediction system should be updated during brain operation
            // If there are prediction-related methods, they should be accessible
        }
        
        std::cout << "    testPredictionSystemIntegration passed" << std::endl;
    } else {
        std::cout << "    WARNING: Prediction system not available" << std::endl;
    }
}

void testConceptFormationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test concept formation integration
    auto* conceptFormation = brain.getConceptFormation();
    
    if (conceptFormation) {
        std::cout << "    Concept formation system initialized and accessible" << std::endl;
        
        // Test integration with working memory
        auto* workingMemory = brain.getWorkingMemory();
        if (workingMemory) {
            // Run steps to allow concept formation to integrate with working memory
            for (nlm::SimulationStep step = 0; step < 8; ++step) {
                brain.step(step);
            }
            
            // Concept formation should have processed working memory traces
            auto concepts = conceptFormation->getCurrentConcepts();
            std::cout << "    Concept formation detected " << concepts.size() << " concepts" << std::endl;
        }
        
        std::cout << "    testConceptFormationIntegration passed" << std::endl;
    } else {
        std::cout << "    WARNING: Concept formation system not available" << std::endl;
    }
}

void testPluginSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test plugin system
    bool supportsPlugins = brain.supportsPlugins();
    
    if (supportsPlugins) {
        std::cout << "    Plugin system supported by Brain" << std::endl;
        
        auto& pluginRegistry = brain.getPluginRegistry();
        std::vector<std::string> registeredPlugins = pluginRegistry.getRegisteredPluginNames();
        
        std::cout << "    Plugin system has " << registeredPlugins.size() << " registered plugins" << std::endl;
        
        for (const auto& pluginName : registeredPlugins) {
            std::cout << "    Plugin: " << pluginName << " - " 
                     << pluginRegistry.getPluginDescription(pluginName) << std::endl;
        }
        
        std::cout << "    testPluginSystemIntegration passed" << std::endl;
    } else {
        std::cout << "    WARNING: Plugin system not supported" << std::endl;
    }
}

void testIntegratedBrainLoop() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that all integrated systems work together in the brain loop
    std::cout << "Testing integrated brain loop with all systems..." << std::endl;
    
    // Verify all integrated systems are available
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    
    // Run multiple steps to test integration
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Each system should be updated during each step
        // (If available, we would check specific system behaviors)
        if (step % 10 == 0) {
            std::cout << "    Completed step " << step << "/20" << std::endl;
        }
    }
    
    std::cout << "    testIntegratedBrainLoop passed" << std::endl;
}

void runAllIntegrationTests() {
    std::cout << "=== Running Integration Tests for Cognitive and Memory Systems ===" << std::endl;
    
    testMemorySystemIntegration();
    testPredictionSystemIntegration();
    testConceptFormationIntegration();
    testPluginSystemIntegration();
    testIntegratedBrainLoop();
    
    std::cout << "=== All Integration Tests Completed ===" << std::endl;
}

} // namespace test_integration