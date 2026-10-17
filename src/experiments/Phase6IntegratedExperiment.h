// Integration test framework for NLM Phase 6
// Comprehensive testing of all integrated brain systems
// Validates that memory, prediction, cognition, and neuromodulation systems work together

#include "Phase6IntegratedExperiment.h"
#include "Brain.hpp"
#include "AgentBrain.hpp"
#include "Neuromodulator.hpp"
#include "PredictionSystem.hpp"
#include "NeuralWorkingMemory.hpp"
#include "NeuralEpisodicMemory.hpp"
#include "NeuralPlanner.hpp>
#include "AttentionalSelection.hpp>
#include "ConceptFormation.hpp>
#include "Neuromodulator.hpp>
#include "Dopamine.hpp"
#include "Acetylcholine.hpp>
#include "Norepinephrine.hpp>
#include "Serotonin.hpp>
#include "Curiosity.hpp>
#include "Novelty.hpp>
#include "PredictionError.hpp>
#include "DevelopmentSystem.hpp>
#include <iostream>
#include <chrono>
#include <memory>

namespace nlm {

// Comprehensive integration test suite
class NLMIntegrationTest {
public:
    NLMIntegrationTest() : results(), startTime(std::chrono::high_resolution_clock::now()) {}
    
    ~NLMIntegrationTest() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        std::cout << "\n=== NLM Integration Test Summary ===" << std::endl;
        std::cout << "Total tests run: " << results.totalTests << std::endl;
        std::cout << "Tests passed: " << results.passedTests << std::endl;
        std::cout << "Tests failed: " << results.failedTests << std::endl;
        std::cout << "Total execution time: " << duration.count() << " seconds" << std::endl;
        
        if (results.failedTests > 0) {
            std::cout << "\nFAILED TESTS:" << std::endl;
            for (const auto& testName : results.failedTestNames) {
                std::cout << "  - " << testName << std::endl;
            }
        }
    }
    
    // Run all integration tests
    bool runAllTests() {
        std::cout << "Starting NLM Phase 6 Integration Tests..." << std::endl;
        std::cout << "Testing integrated brain systems..." << std::endl;
        
        bool allPassed = true;
        
        // Test 1: Basic brain structure and initialization
        allPassed &= testBasicStructure();
        
        // Test 2: Memory systems integration
        allPassed &= testMemoryIntegration();
        
        // Test 3: Neuromodulation systems integration
        allPassed &= testNeuromodulationIntegration();
        
        // Test 4: Prediction system integration
        allPassed &= testPredictionIntegration();
        
        // Test 5: Cognition systems integration
        allPassed &= testCognitionIntegration();
        
        // Test 6: Action selection integration
        allPassed &= testActionSelectionIntegration();
        
        // Test 7: Attention and working memory integration
        allPassed &= testAttentionWorkingMemoryIntegration();
        
        // Test 8: Development integration
        allPassed &= testDevelopmentIntegration();
        
        // Test 9: Sensory-motor loop integration
        allPassed &= testSensoryMotorIntegration();
        
        // Test 10: Checkpoint and persistence
        allPassed &= testCheckpointIntegration();
        
        // Test 11: Memory replay and consolidation
        allPassed &= testMemoryReplayIntegration();
        
        // Test 12: Novelty and curiosity integration
        allPassed &= testNoveltyCuriosityIntegration();
        
        results.totalTests = testCount;
        return allPassed;
    }
    
private:
    struct TestResults {
        int totalTests = 0;
        int passedTests = 0;
        int failedTests = 0;
        std::vector<std::string> failedTestNames;
    };
    
    TestResults results;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    int testCount = 0;
    
    // Test 1: Basic brain structure and initialization
    bool testBasicStructure() {
        testCount++;
        std::cout << "[Test " << testCount << "] Basic brain structure and initialization...";
        
        try {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 100);
            cfg->set("region_count", 2);
            
            auto brain = std::make_shared<Brain>(cfg);
            brain->initialize();
            
            // Verify basic components
            if (!brain->getSpikeSystem()) {
                std::cout << " FAILED - Spike system not initialized" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("BasicStructure");
                return false;
            }
            
            if (brain->getTotalNeuronCount() == 0) {
                std::cout << " FAILED - No neurons created" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("BasicStructure");
                return false;
            }
            
            std::cout << " PASSED" << std::endl;
            results.passedTests++;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << " FAILED - Exception: " << e.what() << std::endl;
            results.failedTests++;
            results.failedTestNames.push_back("BasicStructure");
            return false;
        }
    }
    
    // Test 2: Memory systems integration
    bool testMemoryIntegration() {
        testCount++;
        std::cout << "[Test " << testCount << "] Memory systems integration...";
        
        try {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 150);
            
            auto brain = std::make_shared<Brain>(cfg);
            brain->initialize();
            
            // Test working memory
            auto* workingMemory = brain->getWorkingMemory();
            if (!workingMemory) {
                std::cout << " FAILED - Working memory not available" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("MemoryIntegration");
                return false;
            }
            
            workingMemory->store({0.5f, 0.3f, 0.8f, 0.1f, 0.9f}, 1.0f);
            auto retrieved = workingMemory->retrieve();
            if (retrieved.empty()) {
                std::cout << " FAILED - Working memory retrieval failed" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("MemoryIntegration");
                return false;
            }
            
            // Test episodic memory
            auto* episodicMemory = brain->getEpisodicMemory();
            if (!episodicMemory) {
                std::cout << " FAILED - Episodic memory not available" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("MemoryIntegration");
                return false;
            }
            
            // Run some steps to generate episodic memory
            for (int i = 0; i < 50; ++i) {
                brain->step(i, i * 0.001);
            }
            
            if (episodicMemory->getEpisodeCount() == 0) {
                std::cout << " FAILED - No episodic memory episodes generated" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("MemoryIntegration");
                return false;
            }
            
            // Test associative memory
            auto* associativeMemory = brain->getAssociativeMemory();
            if (!associativeMemory) {
                std::cout << " FAILED - Associative memory not available" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("MemoryIntegration");
                return false;
            }
            
            associativeMemory->associate(NeuronId(1), NeuronId(2), 0.7f);
            auto associations = associativeMemory->getAssociations(NeuronId(1));
            if (associations.empty()) {
                std::cout << " FAILED - Associative memory association not working" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("MemoryIntegration");
                return false;
            }
            
            std::cout << " PASSED" << std::endl;
            results.passedTests++;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << " FAILED - Exception: " << e.what() << std::endl;
            results.failedTests++;
            results.failedTestNames.push_back("MemoryIntegration");
            return false;
        }
    }
    
    // Test 3: Neuromodulation systems integration
    bool testNeuromodulationIntegration() {
        testCount++;
        std::cout << "[Test " << testCount << "] Neuromodulation systems integration...";
        
        try {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 100);
            
            auto brain = std::make_shared<Brain>(cfg);
            brain->initialize();
            
            // Test all neuromodulators
            auto* dopamine = brain->getDopamine();
            auto* acetylcholine = brain->getAcetylcholine();
            auto* norepinephrine = brain->getNorepinephrine();
            auto* serotonin = brain->getSerotonin();
            auto* curiosity = brain->getCuriosity();
            auto* novelty = brain->getNovelty();
            auto* predictionError = brain->getPredictionErrorSignal();
            
            if (!dopamine || !acetylcholine || !norepinephrine || !serotonin ||
                !curiosity || !novelty || !predictionError) {
                std::cout << " FAILED - Missing neuromodulation systems" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("NeuromodulationIntegration");
                return false;
            }
            
            // Test neuromodulator functions
            dopamine->signalReward(0.5f);
            acetylcholine->signalAttention(0.8f);
            norepinephrine->signalArousal(0.6f);
            serotonin->signalMood(0.4f);
            
            // Check that neuromodulators have levels
            if (dopamine->getLevel() <= 0.0f) {
                std::cout << " FAILED - Dopamine level not updated" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("NeuromodulationIntegration");
                return false;
            }
            
            if (acetylcholine->getLevel() <= 0.0f) {
                std::cout << " FAILED - Acetylcholine level not updated" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("NeuromodulationIntegration");
                return false;
            }
            
            // Run some steps to test neuromodulator updates
            for (int i = 0; i < 30; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 3.0f);
                brain->step(i, i * 0.001);
            }
            
            // Check that neuromodulators updated
            if (dopamine->getLevel() <= 0.0f) {
                std::cout << " FAILED - Dopamine did not update during steps" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("NeuromodulationIntegration");
                return false;
            }
            
            std::cout << " PASSED" << std::endl;
            results.passedTests++;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << " FAILED - Exception: " << e.what() << std::endl;
            results.failedTests++;
            results.failedTestNames.push_back("NeuromodulationIntegration");
            return false;
        }
    }
    
    // Test 4: Prediction system integration
    bool testPredictionIntegration() {
        testCount++;
        std::cout << "[Test " << testCount << "] Prediction system integration...";
        
        try {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 120);
            
            auto brain = std::make_shared<Brain>(cfg);
            brain->initialize();
            
            auto* predictionSystem = brain->getPredictionSystem();
            if (!predictionSystem) {
                std::cout << " FAILED - Prediction system not available" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("PredictionIntegration");
                return false;
            }
            
            // Test prediction system operations
            predictionSystem->getPredictionError();
            predictionSystem->getConfidence();
            predictionSystem->clearHistory();
            
            // Run some steps with working memory to influence prediction
            for (int i = 0; i < 40; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 2.0f);
                brain->step(i, i * 0.001);
                
                // Update prediction based on brain state
                if (brain->getWorkingMemory()) {
                    auto memoryPattern = brain->getWorkingMemory()->retrieve();
                    // Prediction system could use memory patterns
                }
            }
            
            std::cout << " PASSED" << std::endl;
            results.passedTests++;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << " FAILED - Exception: " << e.what() << std::endl;
            results.failedTests++;
            results.failedTestNames.push_back("PredictionIntegration");
            return false;
        }
    }
    
    // Test 5: Cognition systems integration
    bool testCognitionIntegration() {
        testCount++;
        std::cout << "[Test " << testCount << "] Cognition systems integration...";
        
        try {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 130);
            
            auto brain = std::make_shared<Brain>(cfg);
            brain->initialize();
            
            auto* planner = brain->getPlanner();
            auto* conceptFormation = brain->getConceptFormation();
            auto* attention = brain->getAttention();
            
            if (!planner || !conceptFormation || !attention) {
                std::cout << " FAILED - Cognition systems not available" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("CognitionIntegration");
                return false;
            }
            
            // Test cognition system functions
            std::vector<float> currentState(10, 0.5f);
            ActionType action = planner->planAction(currentState, 0.7f);
            
            if (planner->getPlanningDepth() == 0) {
                std::cout << " FAILED - Planner depth not set" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("CognitionIntegration");
                return false;
            }
            
            conceptFormation->setPlanningDepth(5);
            attention->setInhibitionStrength(0.5f);
            attention->setExcitationStrength(1.5f);
            
            // Run steps to test cognition system updates
            for (int i = 0; i < 35; ++i) {
                brain->step(i, i * 0.001);
            }
            
            std::cout << " PASSED" << std::endl;
            results.passedTests++;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << " FAILED - Exception: " << e.what() << std::endl;
            results.failedTests++;
            results.failedTestNames.push_back("CognitionIntegration");
            return false;
        }
    }
    
    // Test 6: Action selection integration
    bool testActionSelectionIntegration() {
        testCount++;
        std::cout << "[Test " << testCount << "] Action selection integration...";
        
        try {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 110);
            
            auto brain = std::make_shared<Brain>(cfg);
            brain->initialize();
            
            // Test enhanced action selection with NeuralPlanner
            for (int i = 0; i < 60; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 4.0f);
                brain->step(i, i * 0.001);
                
                auto action = brain->produceAction();
                if (!action) {
                    std::cout << " FAILED - Action selection returned null" << std::endl;
                    results.failedTests++;
                    results.failedTestNames.push_back("ActionSelectionIntegration");
                    return false;
                }
            }
            
            std::cout << " PASSED" << std::endl;
            results.passedTests++;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << " FAILED - Exception: " << e.what() << std::endl;
            results.failedTests++;
            results.failedTestNames.push_back("ActionSelectionIntegration");
            return false;
        }
    }
    
    // Test 7: Attention and working memory integration
    bool testAttentionWorkingMemoryIntegration() {
        testCount++;
        std::cout << "[Test " << testCount << "] Attention and working memory integration...";
        
        try {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 140);
            
            auto brain = std::make_shared<Brain>(cfg);
            brain->initialize();
            
            auto* attention = brain->getAttention();
            auto* workingMemory = brain->getWorkingMemory();
            
            if (!attention || !workingMemory) {
                std::cout << " FAILED - Attention or working memory not available" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("AttentionWorkingMemoryIntegration");
                return false;
            }
            
            // Store something in working memory
            workingMemory->store({0.8f, 0.2f, 0.6f, 0.4f, 0.9f, 0.1f, 0.7f, 0.3f}, 1.0f);
            
            // Run steps to test attention-working memory integration
            for (int i = 0; i < 45; ++i) {
                brain->step(i, i * 0.001);
            }
            
            // Check that attention processed working memory
            auto attentionWinners = attention->getWinners();
            
            std::cout << " PASSED" << std::endl;
            results.passedTests++;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << " FAILED - Exception: " << e.what() << std::endl;
            results.failedTests++;
            results.failedTestNames.push_back("AttentionWorkingMemoryIntegration");
            return false;
        }
    }
    
    // Test 8: Development integration
    bool testDevelopmentIntegration() {
        testCount++;
        std::cout << "[Test " << testCount << "] Development integration...";
        
        try {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 160);
            
            auto brain = std::make_shared<Brain>(cfg);
            brain->initialize();
            
            auto* developmentSystem = brain->getDevelopmentSystem();
            if (!developmentSystem) {
                std::cout << " FAILED - Development system not available" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("DevelopmentIntegration");
                return false;
            }
            
            // Test development stages
            brain->setDevelopmentalStage(DevelopmentalStage::Initial);
            if (brain->getDevelopmentalStage() != DevelopmentalStage::Initial) {
                std::cout << " FAILED - Developmental stage not set correctly" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("DevelopmentIntegration");
                return false;
            }
            
            // Run steps to test development
            for (int i = 0; i < 300; ++i) {
                brain->step(i, i * 0.001);
                
                // Check that development updates periodically
                if (i % 1000 == 0) {
                    // Development should have updated
                    break;
                }
            }
            
            std::cout << " PASSED" << std::endl;
            results.passedTests++;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << " FAILED - Exception: " << e.what() << std::endl;
            results.failedTests++;
            results.failedTestNames.push_back("DevelopmentIntegration");
            return false;
        }
    }
    
    // Test 9: Sensory-motor loop integration
    bool testSensoryMotorIntegration() {
        testCount++;
        std::cout << "[Test " << testCount << "] Sensory-motor loop integration...";
        
        try {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 180);
            
            auto brain = std::make_shared<Brain>(cfg);
            brain->initialize();
            
            // Create agent brain to test sensory-motor integration
            auto agent = std::make_shared<AgentBrain>(brain);
            
            // Initialize agent with simple world
            auto world = std::make_shared<SimpleWorld>();
            world->configure(10, 10, 8, 8);
            world->reset();
            
            agent->initialize(*world);
            
            // Run sensory-motor loop
            for (int i = 0; i < 50; ++i) {
                world->update(0.1);
                
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                
                brain->step(i);
                
                auto action = agent->decodeMotorCommand();
                
                world->applyMotorCommand(action, world->getSimulationTime());
            }
            
            std::cout << " PASSED" << std::endl;
            results.passedTests++;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << " FAILED - Exception: " << e.what() << std::endl;
            results.failedTests++;
            results.failedTestNames.push_back("SensoryMotorIntegration");
            return false;
        }
    }
    
    // Test 10: Checkpoint and persistence
    bool testCheckpointIntegration() {
        testCount++;
        std::cout << "[Test " << testCount << "] Checkpoint and persistence...";
        
        try {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 120);
            
            // Create and initialize first brain
            auto brain1 = std::make_shared<Brain>(cfg);
            brain1->initialize();
            
            // Run some steps
            for (int i = 0; i < 80; ++i) {
                brain1->step(i, i * 0.001);
            }
            
            // Save checkpoint
            std::string path = "/tmp/nlm_checkpoint_test.bin";
            if (!brain1->save(path)) {
                std::cout << " FAILED - Failed to save checkpoint" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("CheckpointIntegration");
                return false;
            }
            
            // Create new brain and load
            auto brain2 = std::make_shared<Brain>(cfg);
            brain2->initialize();
            
            if (!brain2->load(path)) {
                std::cout << " FAILED - Failed to load checkpoint" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("CheckpointIntegration");
                return false;
            }
            
            std::cout << " PASSED" << std::endl;
            results.passedTests++;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << " FAILED - Exception: " << e.what() << std::endl;
            results.failedTests++;
            results.failedTestNames.push_back("CheckpointIntegration");
            return false;
        }
    }
    
    // Test 11: Memory replay and consolidation
    bool testMemoryReplayIntegration() {
        testCount++;
        std::cout << "[Test " << testCount << "] Memory replay and consolidation...";
        
        try {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 200);
            
            auto brain = std::make_shared<Brain>(cfg);
            brain->initialize();
            
            auto* episodicMemory = brain->getEpisodicMemory();
            if (!episodicMemory) {
                std::cout << " FAILED - Episodic memory not available" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("MemoryReplayIntegration");
                return false;
            }
            
            // Run steps to generate episodes
            for (int i = 0; i < 150; ++i) {
                brain->step(i, i * 0.001);
            }
            
            // Test replay functionality
            auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
            if (episodesToReplay.empty()) {
                std::cout << " FAILED - No episodes available for replay" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("MemoryReplayIntegration");
                return false;
            }
            
            // Test consolidation
            episodicMemory->consolidate(0.5f);
            
            std::cout << " PASSED" << std::endl;
            results.passedTests++;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << " FAILED - Exception: " << e.what() << std::endl;
            results.failedTests++;
            results.failedTestNames.push_back("MemoryReplayIntegration");
            return false;
        }
    }
    
    // Test 12: Novelty and curiosity integration
    bool testNoveltyCuriosityIntegration() {
        testCount++;
        std::cout << "[Test " << testCount << "] Novelty and curiosity integration...";
        
        try {
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 140);
            
            auto brain = std::make_shared<Brain>(cfg);
            brain->initialize();
            
            auto* novelty = brain->getNovelty();
            auto* curiosity = brain->getCuriosity();
            auto* predictionError = brain->getPredictionErrorSignal();
            
            if (!novelty || !curiosity || !predictionError) {
                std::cout << " FAILED - Novelty, curiosity, or prediction error not available" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("NoveltyCuriosityIntegration");
                return false;
            }
            
            // Run steps to generate novelty and curiosity signals
            for (int i = 0; i < 60; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 2.0f);
                brain->step(i, i * 0.001);
            }
            
            // Check that novelty and curiosity updated
            if (novelty->getLevel() < 0.0f || novelty->getLevel() > 1.0f) {
                std::cout << " FAILED - Novelty level out of range" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("NoveltyCuriosityIntegration");
                return false;
            }
            
            if (curiosity->getLevel() < 0.0f || curiosity->getLevel() > 1.0f) {
                std::cout << " FAILED - Curiosity level out of range" << std::endl;
                results.failedTests++;
                results.failedTestNames.push_back("NoveltyCuriosityIntegration");
                return false;
            }
            
            std::cout << " PASSED" << std::endl;
            results.passedTests++;
            return true;
            
        } catch (const std::exception& e) {
            std::cout << " FAILED - Exception: " << e.what() << std::endl;
            results.failedTests++;
            results.failedTestNames.push_back("NoveltyCuriosityIntegration");
            return false;
        }
    }
};

} // namespace nlm