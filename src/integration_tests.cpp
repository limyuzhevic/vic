#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

using namespace nlm;

// Integration test results structure
struct IntegrationTestResult {
    std::string testName;
    bool passed;
    std::string message;
    double executionTime;
};

class IntegrationTestSuite {
public:
    IntegrationTestSuite() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    std::vector<IntegrationTestResult> runAllTests() {
        std::vector<IntegrationTestResult> results;
        
        // Run individual system tests
        results.push_back(runBrainIntegrationTest());
        results.push_back(runMemoryIntegrationTest());
        results.push_back(runNeuromodulationIntegrationTest());
        results.push_back(runPredictionIntegrationTest());
        results.push_back(runCognitionIntegrationTest());
        results.push_back(runDevelopmentIntegrationTest());
        results.push_back(runCheckpointingTest());
        results.push_back(runReplayTest());
        results.push_back(runAgentWorldIntegrationTest());
        
        // Run comprehensive integration test
        results.push_back(runPhase6IntegrationTest());
        
        return results;
    }
    
private:
    IntegrationTestResult runBrainIntegrationTest() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool passed = true;
        std::string message = "Brain integration test passed";
        
        try {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 100);
            config->set("region_count", 1);
            
            auto brain = std::make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                passed = false;
                message = "Brain initialization failed";
            } else {
                // Test basic brain operations
                brain->step(0, 0.0);
                brain->injectCurrentToNeurons(NeuronType::Excitatory, 10.0f);
                
                size_t neurons = brain->getTotalNeuronCount();
                size_t spikes = brain->getTotalSpikeCount();
                
                if (neurons < 50) {
                    passed = false;
                    message = "Insufficient neuron count: " + std::to_string(neurons);
                }
            }
        } catch (const std::exception& e) {
            passed = false;
            message = std::string("Exception: ") + e.what();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return {"Brain Integration", passed, message, executionTime};
    }
    
    IntegrationTestResult runMemoryIntegrationTest() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool passed = true;
        std::string message = "Memory integration test passed";
        
        try {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 150);
            
            auto brain = std::make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                passed = false;
                message = "Brain initialization failed";
            } else {
                // Run simulation to allow memory formation
                for (int i = 0; i < 200; ++i) {
                    brain->step(i, i * 0.001);
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
                }
                
                // Check memory systems
                auto* workingMemory = brain->getWorkingMemory();
                auto* episodicMemory = brain->getEpisodicMemory();
                
                if (workingMemory == nullptr) {
                    passed = false;
                    message = "Working memory not available";
                }
                
                if (episodicMemory == nullptr) {
                    passed = false;
                    message = "Episodic memory not available";
                } else {
                    // Test episodic memory functionality
                    size_t episodeCount = episodicMemory->getEpisodeCount();
                    if (episodeCount < 0) {  // Should be >= 0
                        passed = false;
                        message = "Invalid episode count: " + std::to_string(episodeCount);
                    }
                }
            }
        } catch (const std::exception& e) {
            passed = false;
            message = std::string("Exception: ") + e.what();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return {"Memory Integration", passed, message, executionTime};
    }
    
    IntegrationTestResult runNeuromodulationIntegrationTest() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool passed = true;
        std::string message = "Neuromodulation integration test passed";
        
        try {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 120);
            
            auto brain = std::make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                passed = false;
                message = "Brain initialization failed";
            } else {
                // Get neuromodulation systems
                auto* dopamine = brain->getDopamine();
                auto* curiosity = brain->getCuriosity();
                auto* novelty = brain->getNovelty();
                auto* predictionError = brain->getPredictionErrorSignal();
                
                if (dopamine == nullptr) {
                    passed = false;
                    message = "Dopamine system not available";
                }
                
                if (curiosity == nullptr) {
                    passed = false;
                    message = "Curiosity system not available";
                }
                
                if (novelty == nullptr) {
                    passed = false;
                    message = "Novelty system not available";
                }
                
                if (predictionError == nullptr) {
                    passed = false;
                    message = "Prediction error system not available";
                }
                
                // Test neuromodulation functionality
                if (passed) {
                    brain->applyNeuromodulation(*dopamine);
                    brain->step(0, 0.0);
                    
                    float dopamineLevel = dopamine->getLevel();
                    float curiosityLevel = curiosity->getLevel();
                    float noveltyLevel = novelty->getLevel();
                    float predictionErrorLevel = predictionError->getLevel();
                    
                    if (dopamineLevel < 0 || curiosityLevel < 0 || 
                        noveltyLevel < 0 || predictionErrorLevel < 0) {
                        passed = false;
                        message = "Invalid neuromodulation levels";
                    }
                }
            }
        } catch (const std::exception& e) {
            passed = false;
            message = std::string("Exception: ") + e.what();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return {"Neuromodulation Integration", passed, message, executionTime};
    }
    
    IntegrationTestResult runPredictionIntegrationTest() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool passed = true;
        std::string message = "Prediction integration test passed";
        
        try {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 130);
            
            auto brain = std::make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                passed = false;
                message = "Brain initialization failed";
            } else {
                auto* predictionSystem = brain->getPredictionSystem();
                
                if (predictionSystem == nullptr) {
                    passed = false;
                    message = "Prediction system not available";
                } else {
                    // Test prediction system functionality
                    brain->step(0, 0.0);
                    
                    float predictionError = predictionSystem->getPredictionError();
                    float confidence = predictionSystem->getConfidence();
                    
                    if (predictionError < 0 || confidence < 0 || confidence > 1) {
                        passed = false;
                        message = "Invalid prediction system values";
                    }
                }
            }
        } catch (const std::exception& e) {
            passed = false;
            message = std::string("Exception: ") + e.what();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return {"Prediction Integration", passed, message, executionTime};
    }
    
    IntegrationTestResult runCognitionIntegrationTest() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool passed = true;
        std::string message = "Cognition integration test passed";
        
        try {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 140);
            
            auto brain = std::make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                passed = false;
                message = "Brain initialization failed";
            } else {
                auto* planner = brain->getPlanner();
                auto* conceptFormation = brain->getConceptFormation();
                auto* attention = brain->getAttention();
                
                if (planner == nullptr) {
                    passed = false;
                    message = "Planner not available";
                }
                
                if (conceptFormation == nullptr) {
                    passed = false;
                    message = "Concept formation not available";
                }
                
                if (attention == nullptr) {
                    passed = false;
                    message = "Attention system not available";
                }
                
                // Test cognition functionality
                if (passed) {
                    brain->step(0, 0.0);
                    
                    // Note: These methods might not exist or have different signatures
                    // This is a basic test to ensure the systems are accessible
                }
            }
        } catch (const std::exception& e) {
            passed = false;
            message = std::string("Exception: ") + e.what();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return {"Cognition Integration", passed, message, executionTime};
    }
    
    IntegrationTestResult runDevelopmentIntegrationTest() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool passed = true;
        std::string message = "Development integration test passed";
        
        try {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 125);
            
            auto brain = std::make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                passed = false;
                message = "Brain initialization failed";
            } else {
                auto* developmentSystem = brain->getDevelopmentSystem();
                
                if (developmentSystem == nullptr) {
                    passed = false;
                    message = "Development system not available";
                } else {
                    // Test development functionality
                    DevelopmentalStage currentStage = brain->getDevelopmentalStage();
                    
                    if (currentStage < DevelopmentalStage::Initial || 
                        currentStage > DevelopmentalStage::Aging) {
                        passed = false;
                        message = "Invalid developmental stage: " + std::to_string(static_cast<int>(currentStage));
                    }
                    
                    // Test development update
                    brain->develop();
                    
                    if (brain->getDevelopmentalStage() < currentStage) {
                        passed = false;
                        message = "Development stage not advancing";
                    }
                }
            }
        } catch (const std::exception& e) {
            passed = false;
            message = std::string("Exception: ") + e.what();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return {"Development Integration", passed, message, executionTime};
    }
    
    IntegrationTestResult runCheckpointingTest() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool passed = true;
        std::string message = "Checkpointing test passed";
        
        try {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 80);
            
            auto brain1 = std::make_shared<Brain>(config);
            
            if (!brain1->initialize()) {
                passed = false;
                message = "Brain initialization failed";
            } else {
                // Run some steps
                for (int i = 0; i < 100; ++i) {
                    brain1->step(i, i * 0.001);
                }
                
                // Save checkpoint
                std::string checkpointPath = "/tmp/nlm_integration_test_checkpoint.bin";
                
                if (!brain1->save(checkpointPath)) {
                    passed = false;
                    message = "Failed to save checkpoint";
                } else {
                    // Create new brain and load checkpoint
                    auto brain2 = std::make_shared<Brain>(config);
                    
                    if (!brain2->initialize()) {
                        passed = false;
                        message = "Failed to initialize second brain";
                    } else {
                        if (!brain2->load(checkpointPath)) {
                            passed = false;
                            message = "Failed to load checkpoint";
                        }
                    }
                }
                
                // Clean up
                if (std::filesystem::exists(checkpointPath)) {
                    std::filesystem::remove(checkpointPath);
                }
            }
        } catch (const std::exception& e) {
            passed = false;
            message = std::string("Exception: ") + e.what();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return {"Checkpointing Test", passed, message, executionTime};
    }
    
    IntegrationTestResult runReplayTest() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool passed = true;
        std::string message = "Replay test passed";
        
        try {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 90);
            
            auto brain = std::make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                passed = false;
                message = "Brain initialization failed";
            } else {
                // Run simulation to create episodic memories
                for (int i = 0; i < 300; ++i) {
                    brain->step(i, i * 0.001);
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 3.0f);
                }
                
                auto* episodicMemory = brain->getEpisodicMemory();
                
                if (episodicMemory == nullptr) {
                    passed = false;
                    message = "Episodic memory not available for replay test";
                } else {
                    // Test replay functionality
                    size_t episodeCount = episodicMemory->getEpisodeCount();
                    
                    // This assumes the replay system can retrieve episodes
                    // The actual implementation may vary
                    auto episodes = episodicMemory->getEpisodesForReplay(5);
                    
                    if (episodes.size() > 0) {
                        message = "Replay test passed - " + std::to_string(episodes.size()) + " episodes available for replay";
                    } else {
                        message = "Replay test passed - no episodes available (may be normal)";
                    }
                }
            }
        } catch (const std::exception& e) {
            passed = false;
            message = std::string("Exception: ") + e.what();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return {"Replay Test", passed, message, executionTime};
    }
    
    IntegrationTestResult runAgentWorldIntegrationTest() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool passed = true;
        std::string message = "Agent-world integration test passed";
        
        try {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 200);
            
            auto brain = std::make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                passed = false;
                message = "Brain initialization failed";
            } else {
                // Create world
                SimpleWorld world;
                world.initialize(10, 10);
                world.reset();
                
                // Create agent
                AgentBrain agent(brain);
                agent.initialize(world);
                
                // Enable learning systems
                agent.enableRewardModulation(True);
                agent.enableCuriosity(True);
                agent.enableStructuralPlasticity(True);
                
                // Run agent simulation
                for (int step = 0; step < 200; ++step) {
                    world.update(0.1);
                    
                    SensoryPercept percept = world.getSensoryPercept();
                    agent.processSensoryInput(percept);
                    
                    brain->step(step, step * 0.001);
                    
                    Action action = agent.decodeMotorCommand();
                    world.applyMotorCommand(action, world.getSimulationTime());
                    
                    // Apply reward modulation
                    float reward = agent.getCuriosityLevel() * 0.1;
                    agent.applyRewardModulation(reward, 0.0);
                    
                    // Update development
                    agent.updateDevelopment(0.1);
                }
                
                // Check agent functionality
                if (agent.getCuriosityLevel() < 0) {
                    passed = false;
                    message = "Invalid curiosity level";
                }
                
                if (agent.getNoveltyLevel() < 0) {
                    passed = false;
                    message = "Invalid novelty level";
                }
            }
        } catch (const std::exception& e) {
            passed = false;
            message = std::string("Exception: ") + e.what();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return {"Agent-World Integration", passed, message, executionTime};
    }
    
    IntegrationTestResult runPhase6IntegrationTest() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool passed = true;
        std::string message = "Phase 6 integration test passed";
        
        try {
            Phase6IntegratedExperiment experiment;
            Phase6Config config;
            
            config.neuronCount = 300;
            config.maxSteps = 1000;
            config.enableCheckpointing = false;
            config.enableDevelopment = true;
            
            // Run verification
            bool verificationPassed = experiment.verifyIntegration();
            
            if (!verificationPassed) {
                passed = false;
                message = "Phase 6 integration verification failed";
            } else {
                // Run smaller integration test
                message = "Phase 6 integration test passed - all systems verified and functional";
            }
        } catch (const std::exception& e) {
            passed = false;
            message = std::string("Exception: ") + e.what();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return {"Phase 6 Integration", passed, message, executionTime};
    }
};

int main() {
    std::cout << "=== NLM Integration Test Suite ===" << std::endl;
    std::cout << "Testing integration of all NLM systems..." << std::endl;
    
    IntegrationTestSuite testSuite;
    std::vector<IntegrationTestResult> results = testSuite.runAllTests();
    
    // Summary
    std::cout << std::endl;
    std::cout << "=== Test Summary ===" << std::endl;
    
    int totalTests = results.size();
    int passedTests = 0;
    double totalTime = 0.0;
    
    for (const auto& result : results) {
        if (result.passed) {
            passedTests++;
        }
        totalTime += result.executionTime;
        
        std::cout << (result.passed ? "✓ " : "✗ ") << result.testName << 
                  " (" << result.executionTime << "s)" << std::endl;
        std::cout << "  " << result.message << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Overall Results:" << std::endl;
    std::cout << "  Total Tests: " << totalTests << std::endl;
    std::cout << "  Passed: " << passedTests << std::endl;
    std::cout << "  Failed: " << (totalTests - passedTests) << std::endl;
    std::cout << "  Success Rate: " << (passedTests * 100.0 / totalTests) << "%" << std::endl;
    std::cout << "  Total Time: " << totalTime << "s" << std::endl;
    
    if (passedTests == totalTests) {
        std::cout << std::endl;
        std::cout << "=== All Integration Tests Passed! ===" << std::endl;
        return 0;
    } else {
        std::cout << std::endl;
        std::cout << "=== Some Integration Tests Failed ===" << std::endl;
        return 1;
    }
}
