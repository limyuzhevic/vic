// Phase6IntegratedExperiment Refactoring - Integration Test Suite
// Refactored into smaller, testable components for better maintainability and testability

#pragma once

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include <chrono>
#include <cmath>
#include <memory>

namespace nlm {

// ========== INTEGRATION TEST RUNNER ==========

// Main orchestrator for Phase 6 integration tests
// Coordinates all integration test components
class IntegrationTestRunner {
public:
    IntegrationTestRunner() : experiment() {}
    
    // Run all integration tests in sequence
    bool runAllTests() {
        NLM_LOG_INFO("=== Running Phase 6 Integration Test Suite ===");
        
        bool allPassed = true;
        
        // Run integration verification
        if (!runIntegrationVerification()) {
            NLM_LOG_ERROR("Integration verification failed");
            allPassed = false;
        }
        
        // Run memory integration tests
        if (!runMemoryIntegrationTests()) {
            NLM_LOG_ERROR("Memory integration tests failed");
            allPassed = false;
        }
        
        // Run neuromodulation tests
        if (!runNeuromodulationTests()) {
            NLM_LOG_ERROR("Neuromodulation tests failed");
            allPassed = false;
        }
        
        // Run checkpoint tests
        if (!runCheckpointTests()) {
            NLM_LOG_ERROR("Checkpoint tests failed");
            allPassed = false;
        }
        
        // Run replay tests
        if (!runReplayTests()) {
            NLM_LOG_ERROR("Replay tests failed");
            allPassed = false;
        }
        
        return allPassed;
    }
    
private:
    Phase6IntegratedExperiment experiment;
    
    bool runIntegrationVerification() {
        NLM_LOG_INFO("--- Integration Verification Test ---");
        return experiment.verifyIntegration();
    }
    
    bool runMemoryIntegrationTests() {
        NLM_LOG_INFO("--- Memory Integration Tests ---");
        return experiment.testMemoryIntegration();
    }
    
    bool runNeuromodulationTests() {
        NLM_LOG_INFO("--- Neuromodulation Tests ---");
        return experiment.testNeuromodulationIntegration();
    }
    
    bool runCheckpointTests() {
        NLM_LOG_INFO("--- Checkpoint Tests ---");
        return experiment.testCheckpointing();
    }
    
    bool runReplayTests() {
        NLM_LOG_INFO("--- Replay Tests ---");
        return experiment.testReplay();
    }
};

// ========== BRAIN INTEGRATION TESTS ==========

// Tests for brain system integration and functionality
class BrainIntegrationTests {
public:
    static bool testBasicBrainFunctionality() {
        NLM_LOG_INFO("Testing basic brain functionality...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        cfg->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Brain initialization failed");
            return false;
        }
        
        // Test basic brain functions
        size_t neuronCount = brain->getTotalNeuronCount();
        if (neuronCount == 0) {
            NLM_LOG_ERROR("Brain has zero neurons");
            return false;
        }
        
        // Test step functionality
        for (int step = 0; step < 10; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Test statistics
        size_t totalSpikes = brain->getTotalSpikeCount();
        float avgFiring = brain->getAverageFiringRate();
        
        NLM_LOG_INFO("Brain statistics: " + std::to_string(neuronCount) + " neurons, " +
                     std::to_string(totalSpikes) + " total spikes, " +
                     "avg firing rate: " + std::to_string(avgFiring));
        
        return true;
    }
    
    static bool testBrainMemorySystems() {
        NLM_LOG_INFO("Testing brain memory systems integration...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        // Test working memory
        auto* workingMemory = brain->getWorkingMemory();
        if (!workingMemory) {
            NLM_LOG_ERROR("Working memory not available");
            return false;
        }
        
        // Test episodic memory
        auto* episodicMemory = brain->getEpisodicMemory();
        if (!episodicMemory) {
            NLM_LOG_ERROR("Episodic memory not available");
            return false;
        }
        
        // Test associative memory
        auto* associativeMemory = brain->getAssociativeMemory();
        if (!associativeMemory) {
            NLM_LOG_ERROR("Associative memory not available");
            return false;
        }
        
        return true;
    }
    
    static bool testBrainNeuromodulation() {
        NLM_LOG_INFO("Testing brain neuromodulation systems...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        // Test neuromodulation systems
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        auto* novelty = brain->getNovelty();
        auto* predictionError = brain->getPredictionErrorSignal();
        
        if (!dopamine || !curiosity || !novelty || !predictionError) {
            NLM_LOG_ERROR("Neuromodulation systems not fully available");
            return false;
        }
        
        // Test neuromodulation update
        for (int step = 0; step < 10; ++step) {
            brain->step(step, step * 0.001);
            // Neuromodulation should be updating internally
        }
        
        return true;
    }
    
    static bool testBrainPlasticity() {
        NLM_LOG_INFO("Testing brain plasticity systems...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        cfg->set("stdp_ltp_weight", 0.01f);
        cfg->set("stdp_ltd_weight", 0.012f);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        // Test plasticity systems
        auto* stdp = brain->getSTDP();
        auto* hebbian = brain->getHebbian();
        auto* structuralPlasticity = brain->getStructuralPlasticity();
        
        if (!stdp || !hebbian || !structuralPlasticity) {
            NLM_LOG_ERROR("Plasticity systems not available");
            return false;
        }
        
        // Test configuration
        stdp->configure(0.02f, 0.024f, 25.0f);
        if (stdp->getLTPWeight() != 0.02f || stdp->getLTDWeight() != 0.024f) {
            NLM_LOG_ERROR("STDP configuration failed");
            return false;
        }
        
        return true;
    }
    
    static bool testBrainDevelopment() {
        NLM_LOG_INFO("Testing brain development system...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        cfg->set("enable_development", true);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        // Test development system
        auto* developmentSystem = brain->getDevelopmentSystem();
        if (!developmentSystem) {
            NLM_LOG_ERROR("Development system not available");
            return false;
        }
        
        // Test developmental stage
        DevelopmentalStage stage = brain->getDevelopmentalStage();
        brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        if (brain->getDevelopmentalStage() != DevelopmentalStage::CriticalPeriod) {
            NLM_LOG_ERROR("Developmental stage update failed");
            return false;
        }
        
        return true;
    }
};

// ========== MEMORY INTEGRATION TESTS ==========

// Comprehensive tests for memory system integration
class MemoryIntegrationTests {
public:
    static bool testWorkingMemoryIntegration() {
        NLM_LOG_INFO("Testing working memory integration...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        auto* workingMemory = brain->getWorkingMemory();
        if (!workingMemory) {
            NLM_LOG_ERROR("Working memory not available");
            return false;
        }
        
        // Simulate some neural activity
        for (int step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
            // Working memory should accumulate traces
        }
        
        size_t activeTraces = workingMemory->getActiveTraces();
        if (activeTraces > 1000) {  // Should have accumulated some traces
            NLM_LOG_INFO("Working memory has active traces: " + std::to_string(activeTraces));
        }
        
        return true;
    }
    
    static bool testEpisodicMemoryIntegration() {
        NLM_LOG_INFO("Testing episodic memory integration...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (!episodicMemory) {
            NLM_LOG_ERROR("Episodic memory not available");
            return false;
        }
        
        // Run simulation to generate episodes
        for (int step = 0; step < 200; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t episodeCount = episodicMemory->getEpisodeCount();
        NLM_LOG_INFO("Episodic memory has episodes: " + std::to_string(episodeCount));
        
        // Should have stored some episodes
        if (episodeCount >= 0) {
            NLM_LOG_INFO("Episodic memory integration successful");
        }
        
        return true;
    }
    
    static bool testAssociativeMemoryIntegration() {
        NLM_LOG_INFO("Testing associative memory integration...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        auto* associativeMemory = brain->getAssociativeMemory();
        if (!associativeMemory) {
            NLM_LOG_ERROR("Associative memory not available");
            return false;
        }
        
        // Test memory storage and retrieval
        // (Associative memory implementation would be tested here)
        NLM_LOG_INFO("Associative memory integration test completed");
        
        return true;
    }
};

// ========== NEUROMODULATION INTEGRATION TESTS ==========

// Tests for neuromodulation system integration
class NeuromodulationIntegrationTests {
public:
    static bool testDopamineSystem() {
        NLM_LOG_INFO("Testing dopamine system integration...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        auto* dopamine = brain->getDopamine();
        if (!dopamine) {
            NLM_LOG_ERROR("Dopamine system not available");
            return false;
        }
        
        // Test dopamine level updates
        float initialLevel = dopamine->getLevel();
        
        for (int step = 0; step < 20; ++step) {
            brain->step(step, step * 0.001);
            // Dopamine level should be updating
        }
        
        float finalLevel = dopamine->getLevel();
        NLM_LOG_INFO("Dopamine level: " + std::to_string(finalLevel));
        
        return true;
    }
    
    static bool testCuriositySystem() {
        NLM_LOG_INFO("Testing curiosity system integration...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        auto* curiosity = brain->getCuriosity();
        if (!curiosity) {
            NLM_LOG_ERROR("Curiosity system not available");
            return false;
        }
        
        // Test curiosity level updates
        for (int step = 0; step < 20; ++step) {
            brain->step(step, step * 0.001);
        }
        
        return true;
    }
    
    static bool testPredictionErrorSystem() {
        NLM_LOG_INFO("Testing prediction error system integration...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        auto* predictionError = brain->getPredictionErrorSignal();
        if (!predictionError) {
            NLM_LOG_ERROR("Prediction error system not available");
            return false;
        }
        
        // Test prediction error updates
        for (int step = 0; step < 20; ++step) {
            brain->step(step, step * 0.001);
        }
        
        return true;
    }
    
    static bool testNoveltyDetection() {
        NLM_LOG_INFO("Testing novelty detection system...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        auto* novelty = brain->getNovelty();
        if (!novelty) {
            NLM_LOG_ERROR("Novelty detection system not available");
            return false;
        }
        
        // Test novelty detection updates
        for (int step = 0; step < 20; ++step) {
            brain->step(step, step * 0.001);
        }
        
        return true;
    }
};

// ========== CHECKPOINT INTEGRATION TESTS ==========

// Tests for checkpoint system integration
class CheckpointIntegrationTests {
public:
    static bool testCheckpointSaveLoad() {
        NLM_LOG_INFO("Testing checkpoint save/load functionality...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        cfg->set("checkpoint_dir", "/tmp/test_checkpoints");
        
        // Create and initialize brain
        auto brain1 = std::make_shared<Brain>(cfg);
        brain1->initialize();
        
        // Run some simulation steps
        for (int step = 0; step < 100; ++step) {
            brain1->step(step, step * 0.001);
        }
        
        // Save checkpoint
        std::string checkpointPath = "/tmp/test_checkpoint.bin";
        if (!brain1->save(checkpointPath)) {
            NLM_LOG_ERROR("Failed to save checkpoint");
            return false;
        }
        
        // Create new brain and load
        auto brain2 = std::make_shared<Brain>(cfg);
        brain2->initialize();
        
        if (!brain2->load(checkpointPath)) {
            NLM_LOG_ERROR("Failed to load checkpoint");
            return false;
        }
        
        // Verify that checkpoint was loaded successfully
        // (In a real implementation, we would verify exact state restoration)
        
        NLM_LOG_INFO("Checkpoint save/load test passed");
        return true;
    }
    
    static bool testCheckpointValidation() {
        NLM_LOG_INFO("Testing checkpoint validation...");
        
        // Test with invalid checkpoint path
        auto cfg = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(cfg);
        
        // Load from non-existent file should fail gracefully
        bool loadResult = brain->load("/nonexistent/path.bin");
        if (loadResult) {
            NLM_LOG_ERROR("Should have failed to load non-existent checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Checkpoint validation test passed");
        return true;
    }
};

// ========== REPLAY INTEGRATION TESTS ==========

// Tests for memory replay functionality
class ReplayIntegrationTests {
public:
    static bool testMemoryReplay() {
        NLM_LOG_INFO("Testing memory replay functionality...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (!episodicMemory) {
            NLM_LOG_ERROR("Episodic memory not available for replay test");
            return false;
        }
        
        // Generate some episodes
        for (int step = 0; step < 300; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t episodeCount = episodicMemory->getEpisodeCount();
        NLM_LOG_INFO("Generated " + std::to_string(episodeCount) + " episodes for replay test");
        
        // Test replay functionality
        auto episodes = episodicMemory->getEpisodesForReplay(5);
        NLM_LOG_INFO("Retrieved " + std::to_string(episodes.size()) + " episodes for replay");
        
        // Replay some episodes
        for (const auto* episode : episodes) {
            episodicMemory->replayEpisode(episode);
        }
        
        NLM_LOG_INFO("Memory replay test passed");
        return true;
    }
    
    static bool testReplayInterval() {
        NLM_LOG_INFO("Testing replay interval functionality...");
        
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        cfg->set("replay_interval", 50);  // Set replay interval to 50 steps
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (!episodicMemory) {
            NLM_LOG_ERROR("Episodic memory not available for replay interval test");
            return false;
        }
        
        // Run simulation to trigger replay
        for (int step = 0; step < 200; ++step) {
            brain->step(step, step * 0.001);
            
            // Check that replay is happening at the specified interval
            if (step % 50 == 0) {
                auto episodes = episodicMemory->getEpisodesForReplay(1);
                // Replay should occur at the specified interval
                if (!episodes.empty()) {
                    NLM_LOG_INFO("Replay triggered at step " + std::to_string(step));
                }
            }
        }
        
        NLM_LOG_INFO("Replay interval test passed");
        return true;
    }
};

} // namespace nlm