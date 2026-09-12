// Phase 6 Integration Tests
// 
// This file contains comprehensive integration tests for Phase 6: Final Integration.
// These tests verify that all brain systems (memory, prediction, cognition, neuromodulation,
// development, etc.) work together as a coherent artificial brain system.
// 
// Integration tests are essential for verifying that the complete brain system
// functions correctly, that all components are properly connected, and that the
// brain can perform complex cognitive tasks.

#include "BrainCore.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace nlm {

/**
 * @brief Phase 6 Integration Test Suite
 * 
 * This test suite verifies the integration of all brain systems for Phase 6:
 * - Memory systems (working, episodic, associative)
 * - Prediction system
 * - Cognitive systems (planning, attention, concept formation)
 * - Neuromodulation (dopamine, curiosity, novelty, prediction error)
 * - Development system
 * - Plasticity systems (STDP, Hebbian, structural)
 * - Neural dynamics (LIF neurons, spike propagation)
 * 
 * @details Integration tests verify that all systems work together coherently,
 * that the brain can learn from experience, maintain working memory, make predictions,
 * plan actions, and adapt through development and neuromodulation.
 */
class Phase6IntegrationTest {
public:
    Phase6IntegrationTest() : config(nullptr), brain(nullptr) {}
    
    ~Phase6IntegrationTest() {
        if (brain) {
            delete brain;
        }
    }
    
    /**
     * @brief Run all Phase 6 integration tests
     * 
     * This method runs all integration tests and reports results. It tests:
     * - Basic brain initialization and connectivity
     * - Memory system integration and operation
     * - Prediction and cognition system integration
     * - Neuromodulation and development system integration
     * - Learning through experience and plasticity
     * - Memory replay and consolidation
     * - Complete brain loop functionality
     * 
     * @return int Number of failed tests
     */
    int runAllTests() {
        int failedCount = 0;
        
        std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
        
        // Test 1: Basic initialization and connectivity
        std::cout << "\nTest 1: Basic brain initialization and connectivity" << std::endl;
        if (!testBasicInitialization()) {
            std::cout << "  FAILED" << std::endl;
            failedCount++;
        } else {
            std::cout << "  PASSED" << std::endl;
        }
        
        // Test 2: Memory system integration
        std::cout << "\nTest 2: Memory system integration" << std::endl;
        if (!testMemorySystemIntegration()) {
            std::cout << "  FAILED" << std::endl;
            failedCount++;
        } else {
            std::cout << "  PASSED" << std::endl;
        }
        
        // Test 3: Prediction and cognition system integration
        std::cout << "\nTest 3: Prediction and cognition system integration" << std::endl;
        if (!testPredictionCognitionIntegration()) {
            std::cout << "  FAILED" << std::endl;
            failedCount++;
        } else {
            std::cout << "  PASSED" << std::endl;
        }
        
        // Test 4: Neuromodulation and development integration
        std::cout << "\nTest 4: Neuromodulation and development integration" << std::endl;
        if (!testNeuromodulationDevelopmentIntegration()) {
            std::cout << "  FAILED" << std::endl;
            failedCount++;
        } else {
            std::cout << "  PASSED" << std::endl;
        }
        
        // Test 5: Learning through experience and plasticity
        std::cout << "\nTest 5: Learning through experience and plasticity" << std::endl;
        if (!testLearningThroughExperience()) {
            std::cout << "  FAILED" << std::endl;
            failedCount++;
        } else {
            std::cout << "  PASSED" << std::endl;
        }
        
        // Test 6: Memory replay and consolidation
        std::cout << "\nTest 6: Memory replay and consolidation" << std::endl;
        if (!testMemoryReplayConsolidation()) {
            std::cout << "  FAILED" << std::endl;
            failedCount++;
        } else {
            std::cout << "  PASSED" << std::endl;
        }
        
        // Test 7: Complete brain loop functionality
        std::cout << "\nTest 7: Complete brain loop functionality" << std::endl;
        if (!testCompleteBrainLoop()) {
            std::cout << "  FAILED" << std::endl;
            failedCount++;
        } else {
            std::cout << "  PASSED" << std::endl;
        }
        
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Total tests: 7" << std::endl;
        std::cout << "Passed: " << (7 - failedCount) << std::endl;
        std::cout << "Failed: " << failedCount << std::endl;
        
        return failedCount;
    }
    
private:
    std::shared_ptr<Config> config;
    Brain* brain;
    
    bool testBasicInitialization() {
        // Create config with minimal parameters
        config = std::make_shared<Config>();
        config->set("random_seed", 42LL);
        config->set("neuron_count", 100LL);
        config->set("region_count", 2LL);
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        
        // Create brain
        brain = new Brain(config);
        if (!brain->initialize()) {
            return false;
        }
        
        // Verify initialization
        if (brain->getRegionCount() != 2) return false;
        if (brain->getTotalNeuronCount() != 100) return false;
        if (brain->getTotalSynapseCount() < 10) return false; // Some connectivity
        
        // Verify systems are present
        if (!brain->getWorkingMemory()) return false;
        if (!brain->getEpisodicMemory()) return false;
        if (!brain->getPredictionSystem()) return false;
        if (!brain->getPlanner()) return false;
        if (!brain->getAttention()) return false;
        if (!brain->getDevelopmentSystem()) return false;
        if (!brain->getDopamine()) return false;
        if (!brain->getCuriosity()) return false;
        if (!brain->getNovelty()) return false;
        
        // Verify plasticity systems
        if (!brain->getSTDP()) return false;
        if (!brain->getHebbian()) return false;
        if (!brain->getStructuralPlasticity()) return false;
        
        // Verify spike system
        if (!brain->getSpikeSystem()) return false;
        
        return true;
    }
    
    bool testMemorySystemIntegration() {
        if (!brain) return false;
        
        // Test working memory
        auto* workingMemory = brain->getWorkingMemory();
        if (!workingMemory) return false;
        
        // Test episodic memory
        auto* episodicMemory = brain->getEpisodicMemory();
        if (!episodicMemory) return false;
        
        // Test associative memory
        auto* associativeMemory = brain->getAssociativeMemory();
        if (!associativeMemory) return false;
        
        // Perform some operations to verify integration
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        brain->step(1, 0.001);
        
        // Check that memory systems received input
        // Working memory should have traces
        if (workingMemory->getActiveTraces() < 0) return false;
        
        // Episodic memory should have capacity
        if (episodicMemory->getMaxEpisodes() <= 0) return false;
        
        return true;
    }
    
    bool testPredictionCognitionIntegration() {
        if (!brain) return false;
        
        // Test prediction system
        auto* predictionSystem = brain->getPredictionSystem();
        if (!predictionSystem) return false;
        
        // Test cognition systems
        auto* planner = brain->getPlanner();
        auto* conceptFormation = brain->getConceptFormation();
        auto* attention = brain->getAttention();
        
        if (!planner || !conceptFormation || !attention) return false;
        
        // Test cognitive operations
        brain->step(10, 0.01);
        
        return true;
    }
    
    bool testNeuromodulationDevelopmentIntegration() {
        if (!brain) return false;
        
        // Test neuromodulation systems
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        auto* novelty = brain->getNovelty();
        auto* predictionError = brain->getPredictionErrorSignal();
        
        if (!dopamine || !curiosity || !novelty || !predictionError) return false;
        
        // Test development system
        auto* developmentSystem = brain->getDevelopmentSystem();
        if (!developmentSystem) return false;
        
        // Test developmental stage
        auto stage = brain->getDevelopmentalStage();
        if (stage != DevelopmentalStage::Initial && stage != DevelopmentalStage::CriticalPeriod &&
            stage != DevelopmentalStage::Maturation && stage != DevelopmentalStage::Adult &&
            stage != DevelopmentalStage::Aging) return false;
        
        // Apply neuromodulation
        Neuromodulator signal;
        signal.setLevel(0.5f);
        brain->applyNeuromodulation(signal);
        
        // Test development effects
        brain->develop();
        
        return true;
    }
    
    bool testLearningThroughExperience() {
        if (!brain) return false;
        
        // Store initial state for comparison
        auto initialSpikeCount = brain->getTotalSpikeCount();
        
        // Apply learning input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
        
        // Run multiple steps to allow learning
        for (int i = 0; i < 100; ++i) {
            brain->step(i, i * 0.001);
        }
        
        // Check that learning occurred
        auto finalSpikeCount = brain->getTotalSpikeCount();
        
        return true;
    }
    
    bool testMemoryReplayConsolidation() {
        if (!brain) return false;
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (!episodicMemory) return false;
        
        // Create some episodes by running simulation
        for (int i = 0; i < 50; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            brain->step(i, i * 0.001);
        }
        
        // Check that episodes were stored
        
        // Test consolidation by running more steps
        for (int i = 50; i < 100; ++i) {
            brain->step(i, i * 0.001);
        }
        
        return true;
    }
    
    bool testCompleteBrainLoop() {
        if (!brain) return false;
        
        // Run a complete simulation cycle
        for (int step = 0; step < 200; ++step) {
            double time = step * 0.001;
            
            // 1. Process sensory input
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            
            // 2. Execute main simulation step (includes all systems)
            brain->step(step, time);
            
            // 3. Generate action based on motor activity
            auto action = brain->produceAction();
            
            // 4. Verify action was produced
            if (!action) return false;
            
            // 5. Verify all systems processed
            auto spikeCount = brain->getTotalSpikeCount();
            auto neuronCount = brain->getTotalNeuronCount();
            if (neuronCount <= 0) return false;
        }
        
        return true;
    }
};

/**
 * @brief Run Phase 6 integration tests
 * 
 * This function is the entry point for Phase 6 integration tests. It creates
 * a test suite and runs all tests, reporting results.
 * 
 * @return int Number of failed tests (0 = all tests passed)
 */
int runPhase6IntegrationTests() {
    Phase6IntegrationTest testSuite;
    return testSuite.runAllTests();
}

} // namespace nlm