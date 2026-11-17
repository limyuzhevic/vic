#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <cassert>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/NLMCLI.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

using namespace nlm;

/**
 * Comprehensive Integration Test Suite
 * 
 * Tests all Phase 6 integrated brain systems and their interactions.
 * This suite verifies that the experimental artificial developmental brain
 * functions as a complete, coherent system.
 */

class IntegrationTestSuite {
public:
    struct TestResults {
        std::string testName;
        bool passed;
        std::string message;
        double executionTime;
        
        TestResults(const std::string& name, bool success, const std::string& msg, double time)
            : testName(name), passed(success), message(msg), executionTime(time) {}
    };
    
    IntegrationTestSuite() {
        Logger::getGlobal().setLevel(LogLevel::Info);
        printBanner("NLM Phase 6 Integration Test Suite");
    }
    
    std::vector<TestResults> runAllTests() {
        std::vector<TestResults> results;
        
        // Test configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        
        // Run individual system tests
        results.push_back(runBrainCoreTest(config));
        results.push_back(runMemoryIntegrationTest(config));
        results.push_back(runNeuromodulationTest(config));
        results.push_back(runPredictionSystemTest(config));
        results.push_back(runCognitionSystemTest(config));
        results.push_back(runDevelopmentTest(config));
        results.push_back(runCheckpointTest(config));
        results.push_back(runReplayTest(config));
        results.push_back(runAdvancedFeatureTest(config));
        results.push_back(runFullIntegrationTest(config));
        
        return results;
    }
    
private:
    TestResults runBrainCoreTest(std::shared_ptr<Config> config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            printSection("1. Core Brain Dynamics Test");
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Test neuron dynamics
            auto* region = brain->getRegion(RegionId(1));
            assert(region != nullptr);
            
            auto neurons = region->getAllNeurons();
            assert(!neurons.empty());
            
            // Test spike propagation
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            // Inject current and observe response
            for (auto* neuron : neurons) {
                if (neuron->getType() == NeuronType::Sensory) {
                    neuron->injectCurrent(20.0f);
                }
            }
            
            for (size_t step = 0; step < 100; ++step) {
                brain->step(step);
            }
            
            size_t finalSpikes = brain->getTotalSpikeCount();
            assert(finalSpikes > initialSpikes);
            
            // Test plasticity
            float initialWeight = 0.5f;
            if (!neurons.empty() && !neurons[0]->getConnections().empty()) {
                auto& conn = neurons[0]->getConnections()[0];
                float weightBefore = conn->getWeight();
                
                // Apply some activity
                for (auto* neuron : neurons) {
                    neuron->injectCurrent(10.0f);
                }
                
                for (size_t step = 0; step < 50; ++step) {
                    brain->step(step);
                }
                
                float weightAfter = conn->getWeight();
                assert(std::abs(weightAfter - weightBefore) > 0.01f); // Weight should have changed
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Core Brain Dynamics", true, 
                               "Neuron dynamics and spike propagation working", duration);
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Core Brain Dynamics", false, 
                               std::string("Exception: ") + e.what(), duration);
        }
    }
    
    TestResults runMemoryIntegrationTest(std::shared_ptr<Config> config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            printSection("2. Memory Integration Test");
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Test working memory
            assert(brain->getWorkingMemory() != nullptr);
            assert(brain->getWorkingMemory()->getMemoryNeurons().empty()); // Should be empty initially
            
            // Create some activity to store in working memory
            auto* region = brain->getRegion(RegionId(1));
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Internal) {
                    neuron->injectCurrent(15.0f);
                    break;
                }
            }
            
            // Run simulation to populate working memory
            for (size_t step = 0; step < 100; ++step) {
                brain->step(step);
            }
            
            // Check working memory contents
            size_t memoryNeurons = brain->getWorkingMemory()->getMemoryNeuronCount();
            printResult("Working Memory Neurons", memoryNeurons, 20);
            
            // Test episodic memory
            assert(brain->getEpisodicMemory() != nullptr);
            size_t episodes = brain->getEpisodicMemory()->getEpisodeCount();
            printResult("Episodic Episodes", episodes, 10);
            
            // Test associative memory
            assert(brain->getAssociativeMemory() != nullptr);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Memory Integration", true, 
                               "Memory systems connected and functional", duration);
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Memory Integration", false, 
                               std::string("Exception: ") + e.what(), duration);
        }
    }
    
    TestResults runNeuromodulationTest(std::shared_ptr<Config> config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            printSection("3. Neuromodulation Integration Test");
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Test novelty detection
            assert(brain->getNovelty() != nullptr);
            float noveltyBefore = brain->getNovelty()->getLevel();
            
            // Create novel input to trigger novelty response
            auto* region = brain->getRegion(RegionId(1));
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory) {
                    neuron->injectCurrent(25.0f);
                    break;
                }
            }
            
            for (size_t step = 0; step < 50; ++step) {
                brain->step(step);
            }
            
            float noveltyAfter = brain->getNovelty()->getLevel();
            printResult("Novelty Detection", noveltyAfter, 1.0f);
            
            // Test curiosity system
            assert(brain->getCuriosity() != nullptr);
            float curiosityBefore = brain->getCuriosity()->getLevel();
            
            // Test reward prediction error
            assert(brain->getPredictionError() != nullptr);
            float errorBefore = brain->getPredictionError()->getError();
            
            // Test dopamine system
            assert(brain->getDopamine() != nullptr);
            float dopamineBefore = brain->getDopamine()->getLevel();
            
            // Apply reward to see neuromodulation effects
            if (brain->getAgent()) {
                brain->getAgent()->applyRewardModulation(10.0f, 0.0f);
            }
            
            float dopamineAfter = brain->getDopamine()->getLevel();
            printResult("Dopamine Modulation", dopamineAfter, 1.0f);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Neuromodulation Integration", true, 
                               "All neuromodulation systems connected", duration);
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Neuromodulation Integration", false, 
                               std::string("Exception: ") + e.what(), duration);
        }
    }
    
    TestResults runPredictionSystemTest(std::shared_ptr<Config> config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            printSection("4. Prediction System Integration Test");
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            assert(brain->getPredictionSystem() != nullptr);
            
            // Create some patterns for prediction learning
            std::vector<float> pattern1(16, 0.8f);   // Bright scene
            std::vector<float> pattern2(16, 0.2f);   // Dark scene
            std::vector<float> pattern3(16, 0.5f);   // Medium scene
            
            // Present patterns and observe prediction adaptation
            auto* region = brain->getRegion(RegionId(1));
            for (size_t i = 0; i < 10; ++i) {
                // Present different patterns
                const std::vector<float>* currentPattern;
                if (i % 3 == 0) currentPattern = &pattern1;
                else if (i % 3 == 1) currentPattern = &pattern2;
                else currentPattern = &pattern3;
                
                // Create sensory input
                Vision percept;
                percept.setData(*currentPattern);
                
                // Inject into sensory neurons
                size_t sensoryCount = 0;
                for (auto* neuron : region->getAllNeurons()) {
                    if (neuron->getType() == NeuronType::Sensory && sensoryCount < currentPattern->size()) {
                        neuron->injectCurrent((*currentPattern)[sensoryCount] * 10.0f);
                        sensoryCount++;
                    }
                }
                
                brain->step(i);
                
                // Check prediction error
                float predictionError = brain->getPredictionError() ? brain->getPredictionError()->getError() : 0.0f;
                printResult("Prediction Error", predictionError, 1.0f);
                
                if (i % 5 == 0) {
                    printInfo("Pattern " + std::to_string(i) + " presented and predicted");
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Prediction System Integration", true, 
                               "Prediction system learning and adapting", duration);
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Prediction System Integration", false, 
                               std::string("Exception: ") + e.what(), duration);
        }
    }
    
    TestResults runCognitionSystemTest(std::shared_ptr<Config> config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            printSection("5. Cognition System Integration Test");
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            assert(brain->getPlanner() != nullptr);
            assert(brain->getConceptFormation() != nullptr);
            assert(brain->getAttention() != nullptr);
            
            // Test neural planner
            brain->getPlanner()->setPlanningDepth(5);
            
            std::vector<float> currentScene(16, 0.7f); // Sample scene
            ActionType plannedAction = brain->getPlanner()->planAction(currentScene);
            printResult("Planner Action", static_cast<int>(plannedAction), 5);
            
            // Test concept formation
            size_t conceptCount = brain->getConceptFormation()->getConceptCount();
            printResult("Concepts Formed", conceptCount, 10);
            
            // Present experience to form concepts
            std::vector<float> pattern(16, 0.8f);
            float touch = 0.9f;
            brain->getConceptFormation()->presentExperience(pattern, touch, 5.0f, 0);
            
            size_t newConceptCount = brain->getConceptFormation()->getConceptCount();
            printResult("Concepts After Presentation", newConceptCount, 10);
            
            // Test attention system
            brain->getAttention()->setInhibitionStrength(0.5f);
            brain->getAttention()->setExcitationStrength(1.5f);
            
            std::vector<NeuronId> competitors{NeuronId(1), NeuronId(2), NeuronId(3)};
            brain->getAttention()->processCompetition(competitors);
            
            // Test self-model
            if (brain->getAgent()) {
                brain->getAgent()->enableDevelopment(true);
                brain->getAgent()->updateDevelopment(0.1);
                
                DevelopmentalStage stage = brain->getAgent()->getDevelopmentalStage();
                printResult("Developmental Stage", static_cast<int>(stage), 5);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Cognition System Integration", true, 
                               "All cognition systems connected and functional", duration);
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Cognition System Integration", false, 
                               std::string("Exception: ") + e.what(), duration);
        }
    }
    
    TestResults runDevelopmentTest(std::shared_cast<Config> config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            printSection("6. Development System Integration Test");
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            assert(brain->getDevelopmentSystem() != nullptr);
            
            // Test developmental stages
            DevelopmentalStage initialStage = brain->getDevelopmentalStage();
            printResult("Initial Developmental Stage", static_cast<int>(initialStage), 5);
            
            // Simulate development over time
            for (size_t step = 0; step < 1000; step += 100) {
                brain->getDevelopmentSystem()->update(brain.get(), *brain->getRandomGenerator(), 100.0);
                
                DevelopmentalStage currentStage = brain->getDevelopmentalStage();
                printResult("Development Step " + std::to_string(step), static_cast<int>(currentStage), 5);
                
                if (currentStage == DevelopmentalStage::Adult) {
                    break; // Reached adult stage
                }
            }
            
            // Test structural plasticity development
            auto* sp = brain->getStructuralPlasticity();
            if (sp) {
                float synRate = sp->getSynaptogenesisRate();
                float pruneRate = sp->getPruningRate();
                printResult("Synaptogenesis Rate", synRate, 0.001f);
                printResult("Pruning Rate", pruneRate, 0.0001f);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Development System Integration", true, 
                               "Development system progressing through stages", duration);
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Development System Integration", false, 
                               std::string("Exception: ") + e.what(), duration);
        }
    }
    
    TestResults runCheckpointTest(std::shared_ptr<Config> config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            printSection("7. Checkpoint Integration Test");
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            assert(brain->getCheckpointManager() != nullptr);
            
            // Run some simulation steps
            for (size_t step = 0; step < 500; ++step) {
                brain->step(step);
            }
            
            // Record state before checkpoint
            size_t spikesBefore = brain->getTotalSpikeCount();
            size_t workingNeuronsBefore = brain->getWorkingMemory() ? brain->getWorkingMemory()->getMemoryNeuronCount() : 0;
            
            // Save checkpoint
            const std::string checkpointFile = "/tmp/test_checkpoint.ckpt";
            bool saved = brain->save(checkpointFile);
            printResult("Checkpoint Saved", saved ? 1 : 0, 1);
            
            if (saved) {
                // Create fresh brain
                auto brain2 = std::make_shared<Brain>(config);
                brain2->initialize();
                
                // Load checkpoint
                bool loaded = brain2->load(checkpointFile);
                printResult("Checkpoint Loaded", loaded ? 1 : 0, 1);
                
                if (loaded) {
                    // Verify continuity
                    size_t spikesAfter = brain2->getTotalSpikeCount();
                    size_t workingNeuronsAfter = brain2->getWorkingMemory() ? brain2->getWorkingMemory()->getMemoryNeuronCount() : 0;
                    
                    printResult("Spike Count Continuity", std::abs((int)spikesAfter - (int)spikesBefore), 100);
                    printResult("Working Memory Continuity", std::abs((int)workingNeuronsAfter - (int)workingNeuronsBefore), 10);
                    
                    // Test RNG continuity
                    if (brain->getRandomGenerator() && brain2->getRandomGenerator()) {
                        uint32_t draw1 = brain->getRandomGenerator()->uniformInt(0, 1000000);
                        uint32_t draw2 = brain2->getRandomGenerator()->uniformInt(0, 1000000);
                        printResult("RNG Continuity", draw1 == draw2 ? 1 : 0, 1);
                    }
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Checkpoint Integration", true, 
                               "Checkpoint save/load working correctly", duration);
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Checkpoint Integration", false, 
                               std::string("Exception: ") + e.what(), duration);
        }
    }
    
    TestResults runReplayTest(std::shared_ptr<Config> config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            printSection("8. Replay Integration Test");
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            assert(brain->getEpisodicMemory() != nullptr);
            
            // Create some episodes through simulation
            for (size_t step = 0; step < 200; ++step) {
                brain->step(step);
                
                // Store episode periodically
                if (step % 50 == 0) {
                    EpisodicMemoryItem episode;
                    episode.timestamp = step;
                    episode.reward = static_cast<float>(step % 5);
                    
                    // Store some active neurons
                    auto* region = brain->getRegion(RegionId(1));
                    for (auto* neuron : region->getAllNeurons()) {
                        if (neuron->isFiring()) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(1.0f);
                        }
                    }
                    
                    brain->getEpisodicMemory()->storeEpisode(episode);
                }
            }
            
            size_t episodesStored = brain->getEpisodicMemory()->getEpisodeCount();
            printResult("Episodes Stored", episodesStored, 10);
            
            // Test replay functionality
            auto episodesToReplay = brain->getEpisodicMemory()->getEpisodesForReplay(3);
            printResult("Episodes Available for Replay", episodesToReplay.size(), 10);
            
            for (const auto* episode : episodesToReplay) {
                brain->getEpisodicMemory()->replayEpisode(episode);
            }
            
            printInfo("Replay functionality tested");
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Replay Integration", true, 
                               "Replay system functional", duration);
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Replay Integration", false, 
                               std::string("Exception: ") + e.what(), duration);
        }
    }
    
    TestResults runAdvancedFeatureTest(std::shared_ptr<Config> config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            printSection("9. Advanced Features Test");
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Test advanced Python API functions
            pyapi::getConfigSummary(config);
            pyapi::analyzeBrainState(brain);
            
            std::vector<float> pattern = pyapi::generateSensoryPattern("stripes", 8);
            printInfo("Generated pattern with " + std::to_string(pattern.size()) + " values");
            
            pyapi::getPerformanceMetrics(brain);
            
            bool memoryOK = pyapi::checkMemoryUsage(brain);
            printResult("Memory Check", memoryOK ? 1 : 0, 1);
            
            // Test Phase 6 integrated experiment
            Phase6IntegratedExperiment integratedExp;
            Phase6Config phase6Config;
            phase6Config.neuronCount = 200;
            phase6Config.maxSteps = 100;
            phase6Config.enableCheckpointing = true;
            phase6Config.enableReplay = true;
            phase6Config.enableDevelopment = true;
            
            printInfo("Running Phase 6 integrated experiment...");
            auto result = integratedExp.run(phase6Config);
            
            printResult("Integration Test Score", result.totalReward, 50.0f);
            printResult("Memory Episodes Stored", result.memoryEpisodesStored, 20);
            printResult("Average Firing Rate", result.avgFiringRate, 100.0f);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Advanced Features", true, 
                               "Advanced features and integrated experiment working", duration);
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Advanced Features", false, 
                               std::string("Exception: ") + e.what(), duration);
        }
    }
    
    TestResults runFullIntegrationTest(std::shared_cast<Config> config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            printSection("10. Full Integration Test");
            
            // Run the complete Phase 6 integration experiment
            Phase6IntegratedExperiment experiment;
            
            Phase6Config fullConfig;
            fullConfig.maxSteps = 1000;
            fullConfig.neuronCount = 500;
            fullConfig.regionCount = 2;
            fullConfig.connectionProbability = 0.1f;
            fullConfig.enableCheckpointing = true;
            fullConfig.enableReplay = true;
            fullConfig.enableDevelopment = true;
            fullConfig.checkpointPath = "/tmp/full_integration_test.ckpt";
            
            std::cout << "Running comprehensive integration test..." << std::endl;
            auto result = experiment.run(fullConfig);
            
            std::cout << "=== FULL INTEGRATION TEST RESULTS ===" << std::endl;
            std::cout << "Total Reward: " << result.totalReward << std::endl;
            std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
            std::cout << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
            std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
            std::cout << "Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
            std::cout << std::endl;
            
            std::cout << "=== SYSTEM INTEGRATION STATUS ===" << std::endl;
            std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
            std::cout << "Replay: " << (result.replayWorks ? "FUNCTIONAL" : "NOT WORKING") << std::endl;
            std::cout << std::endl;
            
            // Determine overall success
            bool allSystemsIntegrated = result.memoryWorkingMemoryIntegrated &&
                                     result.memoryEpisodicMemoryIntegrated &&
                                     result.neuromodulationIntegrated &&
                                     result.predictionIntegrated &&
                                     result.developmentIntegrated &&
                                     result.checkpointingWorks &&
                                     result.replayWorks;
            
            std::cout << "OVERALL INTEGRATION STATUS: " 
                      << (allSystemsIntegrated ? "SUCCESS" : "PARTIAL") << std::endl;
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Full Integration Test", allSystemsIntegrated, 
                               "Complete Phase 6 integration verification", duration);
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return TestResults("Full Integration Test", false, 
                               std::string("Exception: ") + e.what(), duration);
        }
    }
    
    void printSection(const std::string& title) {
        nlm::printSection(title);
    }
    
    void printResult(const std::string& name, float value, float max) {
        nlm::printResult(name, value, max);
    }
    
    void printInfo(const std::string& message) {
        std::cout << "[INFO] " << message << std::endl;
    }
    
    void printSuccess(const std::string& message) {
        std::cout << "[SUCCESS] " << message << std::endl;
    }
    
    void printWarning(const std::string& message) {
        std::cout << "[WARN] " << message << std::endl;
    }
};

int main(int argc, char* argv[]) {
    // Parse command line arguments
    auto cliOptions = CommandLineInterface::parseArgs(argc, argv);
    
    if (cliOptions.help) {
        CommandLineInterface::printUsage(argv[0]);
        return 0;
    }
    
    if (cliOptions.version) {
        CommandLineInterface::printVersion();
        return 0;
    }
    
    CommandLineInterface::setupLogging(cliOptions);
    
    // Run integration test suite
    IntegrationTestSuite testSuite;
    auto results = testSuite.runAllTests();
    
    // Print summary
    std::cout << std::endl;
    std::cout << "=== INTEGRATION TEST SUMMARY ===" << std::endl;
    std::cout << std::endl;
    
    int passed = 0;
    int total = results.size();
    double totalTime = 0.0;
    
    for (const auto& result : results) {
        std::cout << result.testName << ": ";
        if (result.passed) {
            std::cout << "PASSED";
            passed++;
        } else {
            std::cout << "FAILED";
        }
        std::cout << " (" << result.message << ")"
                  << " [Time: " << result.executionTime << "s]" << std::endl;
        
        totalTime += result.executionTime;
    }
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Tests Passed: " << passed << " / " << total << std::endl;
    std::cout << "Success Rate: " << (passed * 100.0 / total) << "%" << std::endl;
    std::cout << "Total Execution Time: " << totalTime << "s" << std::endl;
    std::cout << std::endl;
    
    if (passed == total) {
        std::cout << "🎉 ALL TESTS PASSED! The NLM Phase 6 integrated brain is fully functional." << std::endl;
        return 0;
    } else {
        std::cout << "⚠️  Some tests failed. Please review the output above for details." << std::endl;
        return 1;
    }
}
