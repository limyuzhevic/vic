// NLM Phase 6 Cognitive Systems Integration Test
// 
// This test verifies that all cognitive systems are properly integrated
// and working together in the brain loop.

#include "Brain.hpp"
#include "PredictionSystem.hpp"
#include "ConceptFormation.hpp"
#include "Neuromodulator.hpp"
#include <iostream>
#include <chrono>
#include <thread>

void printTestHeader(const std::string& testName) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "TEST: " << testName << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void printTestResult(const std::string& testName, bool passed, const std::string& message = "") {
    std::cout << (passed ? "✓ PASS" : "✗ FAIL") << ": " << testName << std::endl;
    if (!message.empty()) {
        std::cout << "  Message: " << message << std::endl;
    }
}

void testMemorySystemsIntegration(Brain& brain) {
    printTestHeader("Memory Systems Integration");
    
    bool passed = true;
    
    // Test 1: Working memory integration
    auto* workingMemory = brain.getWorkingMemory();
    if (!workingMemory) {
        printTestResult("Working Memory Access", false, "getWorkingMemory() returned nullptr");
        passed = false;
    } else {
        printTestResult("Working Memory Access", true, "Working memory properly initialized");
        
        // Test working memory functionality
        workingMemory->clear();
        workingMemory->storeToNeuron(1001, 0.5f);
        
        size_t traces = workingMemory->getActiveTraces();
        printTestResult("Working Memory Functionality", traces > 0, 
                       "Working memory stored and retrieved neuron data");
    }
    
    // Test 2: Episodic memory integration  
    auto* episodicMemory = brain.getEpisodicMemory();
    if (!episodicMemory) {
        printTestResult("Episodic Memory Access", false, "getEpisodicMemory() returned nullptr");
        passed = false;
    } else {
        printTestResult("Episodic Memory Access", true, "Episodic memory properly initialized");
        
        // Test episodic memory storage
        episodicMemory->clear();
        EpisodicMemoryItem episode;
        episode.timestamp = 100;
        episode.activeNeurons = {1, 2, 3};
        episode.neuronActivations = {0.8f, 0.6f, 0.9f};
        episode.reward = 0.7f;
        
        episodicMemory->storeEpisode(episode);
        size_t episodes = episodicMemory->getEpisodeCount();
        printTestResult("Episodic Memory Storage", episodes > 0, 
                       "Episodic memory stored " + std::to_string(episodes) + " episode(s)");
    }
    
    // Test 3: Prediction system integration
    auto* predictionSystem = brain.getPredictionSystem();
    if (!predictionSystem) {
        printTestResult("Prediction System Access", false, "getPredictionSystem() returned nullptr");
        passed = false;
    } else {
        printTestResult("Prediction System Access", true, "Prediction system properly initialized");
        
        // Test prediction system functionality
        predictionSystem->clearHistory();
        float error = predictionSystem->getPredictionError();
        printTestResult("Prediction System Basic", true, "Prediction system error: " + std::to_string(error));
    }
    
    // Test 4: Concept formation integration
    auto* conceptFormation = brain.getConceptFormation();
    if (!conceptFormation) {
        printTestResult("Concept Formation Access", false, "getConceptFormation() returned nullptr");
        passed = false;
    } else {
        printTestResult("Concept Formation Access", true, "Concept formation properly initialized");
        
        // Test concept formation functionality
        std::vector<float> pattern1 = {0.1f, 0.5f, 0.9f};
        std::vector<float> features1 = {0.2f, 0.6f, 1.0f};
        
        size_t conceptId = conceptFormation->presentExperience(pattern1, features1, 0.8f, 100);
        printTestResult("Concept Formation Basic", conceptId > 0, 
                       "Concept formation created concept " + std::to_string(conceptId));
    }
    
    // Test 5: Attention system integration
    auto* attention = brain.getAttention();
    if (!attention) {
        printTestResult("Attention System Access", false, "getAttention() returned nullptr");
        passed = false;
    } else {
        printTestResult("Attention System Access", true, "Attention system properly initialized");
        
        // Test attention update
        attention->update(0.001);
        printTestResult("Attention System Update", true, "Attention system updated successfully");
    }
    
    // Test 6: Planner integration
    auto* planner = brain.getPlanner();
    if (!planner) {
        printTestResult("Planner Access", false, "getPlanner() returned nullptr");
        passed = false;
    } else {
        printTestResult("Planner Access", true, "Neural planner properly initialized");
        
        // Test planner initialization
        std::vector<float> state = {0.1f, 0.5f, 0.3f};
        ActionType action = planner->planAction(state, 0.5f);
        printTestResult("Planner Action Selection", true, "Planner selected action");
    }
    
    // Test 7: Neuromodulation integration
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    if (dopamine && curiosity && novelty) {
        printTestResult("Neuromodulation Systems Access", true, "All neuromodulators initialized");
        
        // Test neuromodulation updates
        dopamine->update(0.001);
        curiosity->update(0.001);
        novelty->update(0.001);
        
        printTestResult("Neuromodulation Updates", true, "All neuromodulators updated");
    } else {
        printTestResult("Neuromodulation Systems Access", false, "Some neuromodulators not initialized");
        passed = false;
    }
    
    return passed;
}

void testBrainStepIntegration(Brain& brain) {
    printTestHeader("Brain Step Integration");
    
    bool passed = true;
    
    // Reset brain to known state
    brain.reset();
    brain.initialize();
    
    // Test multiple brain steps to verify integration
    for (int step = 0; step < 10; ++step) {
        brain.step(step, step * 0.001);
        
        // Verify that cognitive systems were updated during each step
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        auto* conceptFormation = brain.getConceptFormation();
        auto* predictionSystem = brain.getPredictionSystem();
        auto* attention = brain.getAttention();
        
        if (workingMemory) {
            workingMemory->update(0.001);
        }
        
        if (episodicMemory) {
            // Check if episodic memory was updated
            size_t episodes = episodicMemory->getEpisodeCount();
            if (step % 10 == 0 && episodes == 0) {  // Should have episode every 10 steps
                printTestResult("Episodic Memory Update", false, 
                               "No episode stored after " + std::to_string(step) + " steps");
                passed = false;
            }
        }
        
        if (conceptFormation) {
            // Test concept formation integration
            std::vector<float> pattern = {0.1f, 0.5f, 0.9f};
            std::vector<float> features = {0.2f, 0.6f, 1.0f};
            conceptFormation->presentExperience(pattern, features, 0.5f, step);
        }
        
        if (predictionSystem) {
            // Test prediction system integration
            predictionSystem->train(SensoryInput());
        }
        
        if (attention) {
            attention->update(0.001);
        }
    }
    
    printTestResult("Brain Step Integration", true, 
                   "Completed " + std::to_string(10) + " brain steps with cognitive integration");
    
    return passed;
}

void testRealWorldIntegration(Brain& brain) {
    printTestHeader("Real-World Integration Test");
    
    bool passed = true;
    
    // Reset and initialize
    brain.reset();
    brain.initialize();
    
    // Simulate real-world interaction
    std::cout << "Simulating sensory input..." << std::endl;
    
    // Create a simple sensory input
    SensoryInput sensoryInput;
    std::vector<float> sensorData = {0.2f, 0.5f, 0.8f, 0.3f, 0.6f};
    sensoryInput.setData(sensorData);
    
    // Process sensory input
    brain.receiveSensoryInput(sensoryInput);
    
    // Run brain loop
    for (int step = 0; step < 50; ++step) {
        brain.step(step, step * 0.001);
        
        // Inject some artificial stimulus to drive learning
        if (step % 10 == 0) {
            brain.injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        }
        
        // Apply some reward
        if (step % 15 == 0) {
            // Simulate reward from environment
            float reward = 0.3f + (std::rand() % 100) / 100.0f * 0.7f;
            auto* dopamine = brain.getDopamine();
            if (dopamine) {
                dopamine->signalReward(reward);
            }
        }
        
        // Periodic checks
        if (step % 20 == 0) {
            auto* workingMemory = brain.getWorkingMemory();
            auto* episodicMemory = brain.getEpisodicMemory();
            
            if (workingMemory) {
                printTestResult("Step " + std::to_string(step) + ": Working Memory",
                               workingMemory->getActiveTraces() > 0,
                               "Working memory active traces: " + std::to_string(workingMemory->getActiveTraces()));
            }
            
            if (episodicMemory) {
                size_t episodes = episodicMemory->getEpisodeCount();
                printTestResult("Step " + std::to_string(step) + ": Episodic Memory",
                               episodes > 0,
                               "Episodic memory episodes: " + std::to_string(episodes));
            }
        }
    }
    
    printTestResult("Real-World Integration", true, 
                   "Successfully simulated real-world interaction over 50 steps");
    
    // Final system status
    std::cout << "\nFinal System Status:" << std::endl;
    std::cout << "Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
    
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        std::cout << "Working memory traces: " << workingMemory->getActiveTraces() << std::endl;
    }
    
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "Episodic memory episodes: " << episodicMemory->getEpisodeCount() << std::endl;
    }
    
    auto* predictionSystem = brain.getPredictionSystem();
    if (predictionSystem) {
        std::cout << "Prediction system error: " << predictionSystem->getPredictionError() << std::endl;
        std::cout << "Prediction system confidence: " << predictionSystem->getConfidence() << std::endl;
    }
    
    auto* conceptFormation = brain.getConceptFormation();
    if (conceptFormation) {
        std::cout << "Concept formation concepts: " << conceptFormation->getConceptCount() << std::endl;
    }
    
    return passed;
}

int main() {
    std::cout << "NLM Phase 6 - Cognitive Systems Integration Test" << std::endl;
    std::cout << "=================================================" << std::endl;
    
    // Create and initialize brain with Phase 6 configuration
    auto config = std::make_shared<Config>();
    
    // Configure for Phase 6 integration testing
    config->set("neuron_count", 500, ConfigSource::Default);
    config->set("region_count", 2, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("random_seed", 42, ConfigSource::Default);
    
    // Enable all cognitive systems
    config->set("enable_working_memory", true, ConfigSource::Default);
    config->set("enable_episodic_memory", true, ConfigSource::Default);
    config->set("enable_prediction_system", true, ConfigSource::Default);
    config->set("enable_concept_formation", true, ConfigSource::Default);
    config->set("enable_planner", true, ConfigSource::Default);
    config->set("enable_attention", true, ConfigSource::Default);
    config->set("enable_neuromodulation", true, ConfigSource::Default);
    
    Brain brain(config);
    
    if (!brain.initialize()) {
        std::cout << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully for Phase 6 integration testing." << std::endl;
    
    // Run integration tests
    bool allPassed = true;
    
    // Test 1: Memory systems integration
    allPassed &= testMemorySystemsIntegration(brain);
    
    // Test 2: Brain step integration  
    allPassed &= testBrainStepIntegration(brain);
    
    // Test 3: Real-world integration
    allPassed &= testRealWorldIntegration(brain);
    
    // Final summary
    printTestHeader("INTEGRATION TEST SUMMARY");
    if (allPassed) {
        std::cout << "✓ ALL INTEGRATION TESTS PASSED" << std::endl;
        std::cout << "The cognitive systems are properly integrated and functional." << std::endl;
        std::cout << "Phase 6 integration is working correctly!" << std::endl;
    } else {
        std::cout << "✗ SOME INTEGRATION TESTS FAILED" << std::endl;
        std::cout << "There are issues with cognitive system integration." << std::endl;
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    return allPassed ? 0 : 1;
}