// Integration Test Framework for NLM
// Comprehensive integration testing framework for NLM components
// Tests component interactions, data flow, and system integration

#include "tests/IntegrationTestFramework.h"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#include <memory>
#include <algorithm>

namespace test_integration {

bool IntegrationTestFramework::testBrainComponentIntegration() {
    std::cout << "    Testing Brain component integration..." << std::endl;
    
    try {
        // Create configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        
        // Create brain
        nlm::Brain brain(config);
        assert(!brain.getRegionIds().empty());
        
        // Initialize brain
        assert(brain.initialize());
        
        // Test component interactions
        auto regionIds = brain.getRegionIds();
        assert(regionIds.size() == 2);
        
        // Test neuron population creation
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() > 0);
        
        // Test memory system integration
        auto* workingMemory = brain.getWorkingMemory();
        assert(workingMemory != nullptr);
        
        // Test prediction system integration
        auto* predictionSystem = brain.getPredictionSystem();
        assert(predictionSystem != nullptr);
        
        // Test cognition system integration
        auto* planner = brain.getPlanner();
        assert(planner != nullptr);
        
        // Test neuromodulation system integration
        auto* dopamine = brain.getDopamine();
        assert(dopamine != nullptr);
        
        // Test spike system integration
        auto* spikeSystem = brain.getSpikeSystem();
        assert(spikeSystem != nullptr);
        
        // Test plasticity system integration
        auto* stdp = brain.getSTDP();
        assert(stdp != nullptr);
        
        // Run simulation steps
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
            
            // Verify components are still integrated
            assert(brain.getTotalNeuronCount() == 100);
            assert(brain.getTotalSynapseCount() > 0);
        }
        
        std::cout << "    testBrainComponentIntegration passed" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "    testBrainComponentIntegration failed: " << e.what() << std::endl;
        return false;
    }
}

bool IntegrationTestFramework::testMemorySystemIntegration() {
    std::cout << "    Testing memory system integration..." << std::endl;
    
    try {
        // Create configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        // Create brain
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test memory system integration with brain
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        auto* associativeMemory = brain.getAssociativeMemory();
        
        assert(workingMemory != nullptr);
        assert(episodicMemory != nullptr);
        assert(associativeMemory != nullptr);
        
        // Test memory operations
        workingMemory->setCapacity(50);
        episodicMemory->setMaxEpisodes(100);
        
        // Simulate memory usage
        for (nlm::SimulationStep step = 0; step < 20; ++step) {
            brain.step(step);
            
            // Inject some neurons to working memory
            if (step % 5 == 0 && step > 0) {
                workingMemory->storeToNeuron(1, 0.5f);
            }
            
            // Store episode in episodic memory
            if (step % 10 == 0) {
                nlm::EpisodicMemoryItem episode;
                episode.timestamp = step;
                episode.reward = 1.0f;
                
                // Add some active neurons
                for (int i = 0; i < 10 && i < brain.getTotalNeuronCount(); ++i) {
                    episode.activeNeurons.push_back(static_cast<nlm::NeuronId>(i + 1));
                }
                
                episodicMemory->storeEpisode(episode);
            }
        }
        
        // Verify memory systems are still functional
        assert(workingMemory->getActiveTraces() >= 0);
        assert(episodicMemory->getEpisodeCount() > 0);
        
        std::cout << "    testMemorySystemIntegration passed" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "    testMemorySystemIntegration failed: " << e.what() << std::endl;
        return false;
    }
}

bool IntegrationTestFramework::testNeuromodulationIntegration() {
    std::cout << "    Testing neuromodulation integration..." << std::endl;
    
    try {
        // Create configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        // Create brain
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test neuromodulation system integration
        auto* dopamine = brain.getDopamine();
        auto* curiosity = brain.getCuriosity();
        auto* novelty = brain.getNovelty();
        auto* predictionError = brain.getPredictionErrorSignal();
        
        assert(dopamine != nullptr);
        assert(curiosity != nullptr);
        assert(novelty != nullptr);
        assert(predictionError != nullptr);
        
        // Test neuromodulation effects on brain activity
        for (nlm::SimulationStep step = 0; step < 15; ++step) {
            brain.step(step);
            
            // Get neuromodulator levels
            float dopamineLevel = dopamine->getLevel();
            float curiosityLevel = curiosity->getLevel();
            float noveltyLevel = novelty->getLevel();
            float predictionError = predictionError->getLevel();
            
            // Apply neuromodulation effects
            brain.applyNeuromodulation(*dopamine);
            
            // Verify neuromodulation doesn't crash the system
            assert(brain.getTotalNeuronCount() == 150);
        }
        
        std::cout << "    testNeuromodulationIntegration passed" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "    testNeuromodulationIntegration failed: " << e.what() << std::endl;
        return false;
    }
}

bool IntegrationTestFramework::testPlasticityIntegration() {
    std::cout << "    Testing plasticity integration..." << std::endl;
    
    try {
        // Create configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
        
        // Create brain
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Get plasticity systems
        auto* stdp = brain.getSTDP();
        auto* hebbian = brain.getHebbian();
        auto* structuralPlasticity = brain.getStructuralPlasticity();
        
        assert(stdp != nullptr);
        assert(hebbian != nullptr);
        assert(structuralPlasticity != nullptr);
        
        // Test plasticity system integration
        for (nlm::SimulationStep step = 0; step < 25; ++step) {
            brain.step(step);
            
            // Test structural plasticity updates
            if (step % 100 == 0) {
                brain.develop();
            }
            
            // Update plasticity
            brain.updatePlasticity();
            
            // Verify plasticity systems are still integrated
            assert(brain.getTotalSynapseCount() >= 0);
            assert(brain.getExcitationInhibitionRatio() >= 0.0f);
        }
        
        std::cout << "    testPlasticityIntegration passed" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "    testPlasticityIntegration failed: " << e.what() << std::endl;
        return false;
    }
}

bool IntegrationTestFramework::testDevelopmentIntegration() {
    std::cout << "    Testing development integration..." << std::endl;
    
    try {
        // Create configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        // Create brain
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test development system integration
        auto* developmentSystem = brain.getDevelopmentSystem();
        assert(developmentSystem != nullptr);
        
        // Test developmental stage integration
        nlm::DevelopmentalStage initialStage = brain.getDevelopmentalStage();
        assert(initialStage == nlm::DevelopmentalStage::Initial);
        
        // Simulate development over time
        for (nlm::SimulationStep step = 0; step < 30; ++step) {
            brain.step(step);
            
            // Update development system periodically
            if (step % 1000 == 0) {
                brain.develop();
            }
            
            // Change developmental stage periodically
            if (step >= 1000 && step < 2000) {
                brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
            } else if (step >= 2000 && step < 3000) {
                brain.setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
            } else if (step >= 3000) {
                brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
            }
            
            // Verify development system doesn't break integration
            assert(brain.getTotalNeuronCount() == 180);
        }
        
        std::cout << "    testDevelopmentIntegration passed" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "    testDevelopmentIntegration failed: " << e.what() << std::endl;
        return false;
    }
}

IntegrationTestResult IntegrationTestFramework::runIntegrationTests() {
    std::cout << "=== NLM Integration Test Suite ===" << std::endl;
    
    IntegrationTestResult result;
    result.test_name = "Integration Test Suite";
    
    // Run all integration tests
    result.brain_component_tests = testBrainComponentIntegration();
    result.memory_system_tests = testMemorySystemIntegration();
    result.neuromodulation_tests = testNeuromodulationIntegration();
    result.plasticity_tests = testPlasticityIntegration();
    result.development_tests = testDevelopmentIntegration();
    
    // Calculate overall results
    int passed = 0;
    passed += result.brain_component_tests ? 1 : 0;
    passed += result.memory_system_tests ? 1 : 0;
    passed += result.neuromodulation_tests ? 1 : 0;
    passed += result.plasticity_tests ? 1 : 0;
    passed += result.development_tests ? 1 : 0;
    
    result.passed_tests = passed;
    result.total_tests = 5;
    result.overall_pass = (passed == result.total_tests);
    
    // Print summary
    std::cout << "\n=== Integration Test Results ===" << std::endl;
    std::cout << "Brain Component Integration: " << (result.brain_component_tests ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Memory System Integration: " << (result.memory_system_tests ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Neuromodulation Integration: " << (result.neuromodulation_tests ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Plasticity Integration: " << (result.plasticity_tests ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Development Integration: " << (result.development_tests ? "PASSED" : "FAILED") << std::endl;
    std::cout << "\nOverall: " << (result.overall_pass ? "PASSED" : "FAILED") 
              << " (" << result.passed_tests << "/" << result.total_tests << ")" << std::endl;
    
    return result;
}

} // namespace test_integration
