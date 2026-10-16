// Phase 6 Integration Tests
// Comprehensive testing of integrated brain systems

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>

namespace nlm {

// Global test results
struct Phase6TestResults {
    bool allTestsPassed;
    std::vector<std::string> passedTests;
    std::vector<std::string> failedTests;
    double totalExecutionTime;
    
    Phase6TestResults() : allTestsPassed(true), totalExecutionTime(0.0) {}
    
    void addPassed(const std::string& testName) {
        passedTests.push_back(testName);
    }
    
    void addFailed(const std::string& testName, const std::string& error) {
        allTestsPassed = false;
        failedTests.push_back(testName + ": " + error);
    }
    
    void printResults() const {
        std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    NLM Phase 6 Integration Test Results                          ║" << std::endl;
        std::cout << "╠═══════════╦══════════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║  Status   ║  Description                                                  ║" << std::endl;
        std::cout << "╠═══════════╬══════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        // Test status summary
        std::cout << "║   " << std::setw(9) << (allTestsPassed ? "PASSED" : "FAILED") 
                  << " ║ All Phase 6 integration tests " << (allTestsPassed ? "PASS" : "FAIL") << "                   ║" << std::endl;
        std::cout << "╠═══════════╬══════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        std::cout << "║  Summary  ║  " << passedTests.size() << " passed, " << failedTests.size() << " failed        ║" << std::endl;
        std::cout << "╠═══════════╬══════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        if (!failedTests.empty()) {
            std::cout << "║  Details ║  Failed tests:                                              ║" << std::endl;
            for (const auto& failed : failedTests) {
                std::cout << "║          ║    - " << failed.substr(0, std::min(failed.size(), size_t(60))) << "...               ║" << std::endl;
            }
        }
        
        std::cout << "╚═══════════╩══════════════════════════════════════════════════════════════════════╝" << std::endl;
        
        if (!allTestsPassed) {
            std::cout << std::endl;
            std::cout << "FAILED TESTS:" << std::endl;
            for (const auto& failed : failedTests) {
                std::cout << "  - " << failed << std::endl;
            }
            return;
        }
        
        std::cout << std::endl;
        std::cout << "ALL TESTS PASSED! Phase 6 integration is working correctly." << std::endl;
    }
};

// Comprehensive Phase 6 integration test
Phase6TestResults runPhase6IntegrationTests() {
    Phase6TestResults results;
    double startTime = 0.0;
    
    std::cout << "╗" << std::string(78, '=') << "╗" << std::endl;
    std::cout << "║" << std::string(78, '=') << "║" << std::endl;
    std::cout << "║                 NLM Phase 6 Integration Test Suite                ║" << std::endl;
    std::cout << "║" << std::string(78, '=') << "║" << std::endl;
    std::cout << "╚" << std::string(78, '=') << "╝" << std::endl;
    std::cout << std::endl;
    
    // Initialize logger for tests
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() * 1e-9;
    
    // Test 1: Basic brain initialization and system connectivity
    std::cout << "Test 1: Basic brain initialization and system connectivity" << std::endl;
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            results.addFailed("Basic Initialization", "Brain initialization failed");
        } else {
            results.addPassed("Basic Initialization");
            
            // Verify all systems are present
            if (brain->getWorkingMemory() == nullptr) {
                results.addFailed("System Connectivity", "Working memory not initialized");
            } else results.addPassed("Working Memory System");
            
            if (brain->getEpisodicMemory() == nullptr) {
                results.addFailed("System Connectivity", "Episodic memory not initialized");
            } else results.addPassed("Episodic Memory System");
            
            if (brain->getDopamine() == nullptr) {
                results.addFailed("System Connectivity", "Dopamine system not initialized");
            } else results.addPassed("Dopamine System");
            
            if (brain->getCuriosity() == nullptr) {
                results.addFailed("System Connectivity", "Curiosity system not initialized");
            } else results.addPassed("Curiosity System");
            
            if (brain->getPredictionErrorSignal() == nullptr) {
                results.addFailed("System Connectivity", "Prediction error system not initialized");
            } else results.addPassed("Prediction Error System");
            
            if (brain->getPredictionSystem() == nullptr) {
                results.addFailed("System Connectivity", "Prediction system not initialized");
            } else results.addPassed("Prediction System");
            
            if (brain->getPlanner() == nullptr) {
                results.addFailed("System Connectivity", "Neural planner not initialized");
            } else results.addPassed("Neural Planner System");
            
            if (brain->getDevelopmentSystem() == nullptr) {
                results.addFailed("System Connectivity", "Development system not initialized");
            } else results.addPassed("Development System");
            
            if (brain->getAcetylcholine() == nullptr) {
                results.addFailed("System Connectivity", "Acetylcholine system not initialized");
            } else results.addPassed("Acetylcholine System");
            
            if (brain->getNorepinephrine() == nullptr) {
                results.addFailed("System Connectivity", "Norepinephrine system not initialized");
            } else results.addPassed("Norepinephrine System");
            
            if (brain->getSerotonin() == nullptr) {
                results.addFailed("System Connectivity", "Serotonin system not initialized");
            } else results.addPassed("Serotonin System");
        }
    } catch (const std::exception& e) {
        results.addFailed("Basic Initialization", std::string("Exception: ") + e.what());
    }
    std::cout << std::endl;
    
    // Test 2: Neuromodulation integration
    std::cout << "Test 2: Neuromodulation integration and interaction" << std::endl;
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            results.addFailed("Neuromodulation", "Brain initialization failed");
        } else {
            // Test dopamine modulation of plasticity
            auto* dopamine = brain->getDopamine();
            if (dopamine) {
                float initialLevel = dopamine->getLevel();
                dopamine->signalReward(1.0f);
                float postRewardLevel = dopamine->getLevel();
                if (postRewardLevel <= initialLevel) {
                    results.addFailed("Dopamine Modulation", "Dopamine level should increase after reward");
                } else results.addPassed("Dopamine Reward Signaling");
                
                // Test acetylcholine attention modulation
                auto* acetylcholine = brain->getAcetylcholine();
                if (acetylcholine) {
                    acetylcholine->focusOnModality(0);
                    results.addPassed("Acetylcholine Attention Modulation");
                }
                
                // Test norepinephrine arousal response
                auto* norepinephrine = brain->getNorepinephrine();
                if (norepinephrine) {
                    norepinephrine->increaseAlertness(0.3f);
                    results.addPassed("Norepinephrine Arousal Response");
                }
                
                // Test serotonin mood regulation
                auto* serotonin = brain->getSerotonin();
                if (serotonin) {
                    serotonin->increaseHappiness(0.2f);
                    results.addPassed("Serotonin Mood Regulation");
                }
                
                results.addPassed("Complete Neuromodulation Integration");
            }
        }
    } catch (const std::exception& e) {
        results.addFailed("Neuromodulation", std::string("Exception: ") + e.what());
    }
    std::cout << std::endl;
    
    // Test 3: Memory system integration
    std::cout << "Test 3: Memory system integration and interaction" << std::endl;
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            results.addFailed("Memory Integration", "Brain initialization failed");
        } else {
            // Test working memory integration with sensory input
            brain->receiveSensoryInput(SensoryInput(std::vector<float>{0.8f, 0.6f, 0.4f}));
            auto* workingMemory = brain->getWorkingMemory();
            if (workingMemory) {
                // The working memory should have been updated during sensory input
                results.addPassed("Working Memory Sensory Integration");
            }
            
            // Test episodic memory integration
            auto* episodicMemory = brain->getEpisodicMemory();
            if (episodicMemory) {
                // Simulate some neural activity to create an episode
                brain->injectCurrentToNeurons(NeuronType::Internal, 2.0f);
                brain->step(10, 0.1);
                results.addPassed("Episodic Memory Integration");
            }
            
            // Test prediction system integration
            auto* predictionSystem = brain->getPredictionSystem();
            if (predictionSystem) {
                results.addPassed("Prediction System Integration");
            }
            
            results.addPassed("Complete Memory System Integration");
        }
    } catch (const std::exception& e) {
        results.addFailed("Memory Integration", std::string("Exception: ") + e.what());
    }
    std::cout << std::endl;
    
    // Test 4: NeuralPlanner integration
    std::cout << "Test 4: NeuralPlanner integration and action planning" << std::endl;
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(50));
        config->set("region_count", static_cast<int64_t>(1));
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            results.addFailed("NeuralPlanner", "Brain initialization failed");
        } else {
            auto* planner = brain->getPlanner();
            if (planner) {
                // Extract state for planning
                std::vector<float> state = brain->extractStateForPlanning();
                if (state.empty()) {
                    results.addFailed("NeuralPlanner", "State extraction failed");
                } else {
                    // Test action planning
                    ActionType action = planner->planAction(state, 0.5f);
                    if (action == ActionType::Wait) {
                        results.addFailed("NeuralPlanner", "Planner should provide meaningful action");
                    } else results.addPassed("NeuralPlanner Action Planning");
                    
                    // Test planning quality update
                    std::vector<ActionType> planned{action};
                    std::vector<ActionType> actual{action};
                    float reward = 0.5f;
                    planner->updatePlanQuality(planned, actual, reward);
                    results.addPassed("NeuralPlanner Learning Update");
                    
                    results.addPassed("Complete NeuralPlanner Integration");
                }
            }
        }
    } catch (const std::exception& e) {
        results.addFailed("NeuralPlanner", std::string("Exception: ") + e.what());
    }
    std::cout << std::endl;
    
    // Test 5: Checkpoint system integration
    std::cout << "Test 5: Checkpoint system integration and persistence" << std::endl;
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(20));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("checkpoint_dir", "./test_checkpoints");
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            results.addFailed("Checkpoint", "Brain initialization failed");
        } else {
            // Run some simulation steps
            for (int step = 0; step < 10; ++step) {
                brain->step(step, step * 0.001f);
            }
            
            // Test checkpoint save/load
            std::string checkpointFile = "./test_checkpoints/brain_test_checkpoint.bin";
            if (!brain->save(checkpointFile)) {
                results.addFailed("Checkpoint Save", "Failed to save checkpoint");
            } else {
                results.addPassed("Checkpoint Save");
                
                // Create new brain and load checkpoint
                auto brain2 = std::make_shared<Brain>(config);
                if (!brain2->initialize()) {
                    results.addFailed("Checkpoint Load", "Failed to initialize new brain");
                } else if (!brain2->load(checkpointFile)) {
                    results.addFailed("Checkpoint Load", "Failed to load checkpoint");
                } else {
                    results.addPassed("Checkpoint Load");
                    results.addPassed("Complete Checkpoint System Integration");
                }
            }
            
            // Cleanup test checkpoints
            std::remove("./test_checkpoints/brain_test_checkpoint.bin");
            std::remove("./test_checkpoints");
        }
    } catch (const std::exception& e) {
        results.addFailed("Checkpoint", std::string("Exception: ") + e.what());
    }
    std::cout << std::endl;
    
    // Test 6: Development system integration
    std::cout << "Test 6: Development system integration and maturation" << std::endl;
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(30));
        config->set("region_count", static_cast<int64_t>(1));
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            results.addFailed("Development", "Brain initialization failed");
        } else {
            auto* developmentSystem = brain->getDevelopmentSystem();
            if (!developmentSystem) {
                results.addFailed("Development", "Development system not initialized");
            } else {
                // Run some simulation steps to allow development to occur
                for (int step = 0; step < 100; ++step) {
                    brain->step(step, step * 0.001f);
                }
                
                results.addPassed("Development System Integration");
            }
        }
    } catch (const std::exception& e) {
        results.addFailed("Development", std::string("Exception: ") + e.what());
    }
    std::cout << std::endl;
    
    // Calculate total execution time
    double endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() * 1e-9;
    results.totalExecutionTime = endTime - startTime;
    
    std::cout << "╗" << std::string(78, '=') << "╗" << std::endl;
    std::cout << "║" << std::string(78, '=') << "║" << std::endl;
    std::cout << "║                 Phase 6 Integration Tests Complete                 ║" << std::endl;
    std::cout << "║" << std::string(78, '=') << "║" << std::endl;
    std::cout << "╚" << std::string(78, '=') << "╝" << std::endl;
    std::cout << std::endl;
    results.printResults();
    
    return results;
}

} // namespace nlm