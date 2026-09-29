// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration Demo
//
// This phase demonstrates the fully integrated artificial brain with:
// - All memory systems (working, episodic, semantic, procedural, associative)
// - Neuromodulation systems (dopamine, curiosity, novelty, etc.)
// - Prediction and cognition systems
// - Developmental stages and structural plasticity
// - Checkpoint save/load functionality
// - Memory replay and consolidation
// - Closed-loop brain-world interaction

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase integrates all systems into a coherent brain.    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void runIntegrationDemo() {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    auto startWall = std::chrono::high_resolution_clock::now();
    
    try {
        // Create Phase 6 integrated experiment
        Phase6IntegratedExperiment experiment;
        
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            std::cerr << "The brain system is not properly integrated." << std::endl;
            std::cerr << "Please check the logs for details on what systems are missing or broken." << std::endl;
            return;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
        
        // Test individual systems with enhanced logging
        std::cout << "--- Memory Integration Test ---" << std::endl;
        bool memoryTestResult = experiment.testMemoryIntegration();
        if (!memoryTestResult) {
            std::cerr << "WARNING: Memory integration test failed!" << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        bool neuromodulationTestResult = experiment.testNeuromodulationIntegration();
        if (!neuromodulationTestResult) {
            std::cerr << "WARNING: Neuromodulation integration test failed!" << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "--- Checkpoint Test ---" << std::endl;
        bool checkpointTestResult = experiment.testCheckpointing();
        if (!checkpointTestResult) {
            std::cerr << "ERROR: Checkpoint test failed!" << std::endl;
            std::cerr << "This is a critical issue - checkpoint save/load is not working." << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "--- Replay Test ---" << std::endl;
        bool replayTestResult = experiment.testReplay();
        if (!replayTestResult) {
            std::cerr << "WARNING: Replay test failed!" << std::endl;
        }
        std::cout << std::endl;
        
        // Run full integration experiment with smaller settings for demo
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto result = experiment.run(config);
        
        // Enhanced validation of experiment results
        bool experimentSucceeded = validateIntegrationResults(result, memoryTestResult, 
                                                              neuromodulationTestResult, 
                                                              checkpointTestResult, replayTestResult);
        
        if (!experimentSucceeded) {
            std::cerr << "ERROR: Integration experiment failed validation!" << std::endl;
            std::cerr << "The brain systems are not properly integrated." << std::endl;
            return;
        }
        
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        // Detailed integration status report
        std::cout << "=== DETAILED INTEGRATION STATUS ===" << std::endl;
        int connectedSystems = 0;
        int totalSystems = 6;
        
        std::cout << "Working Memory:   " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        if (result.memoryWorkingMemoryIntegrated) connectedSystems++;
        
        std::cout << "Episodic Memory:  " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        if (result.memoryEpisodicMemoryIntegrated) connectedSystems++;
        
        std::cout << "Neuromodulation:  " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        if (result.neuromodulationIntegrated) connectedSystems++;
        
        std::cout << "Prediction:        " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        if (result.predictionIntegrated) connectedSystems++;
        
        std::cout << "Development:       " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        if (result.developmentIntegrated) connectedSystems++;
        
        std::cout << "Checkpointing:     " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
        if (result.checkpointingWorks) connectedSystems++;
        
        std::cout << std::endl;
        std::cout << "=== Integration Report ===" << std::endl;
        std::cout << "Systems Connected: " << connectedSystems << "/" << totalSystems << " (" 
                  << (connectedSystems * 100 / totalSystems) << "%)" << std::endl;
        
        if (connectedSystems == totalSystems) {
            std::cout << std::endl;
            std::cout << "=== SUCCESS: Phase 6 Integration Complete ===" << std::endl;
            std::cout << "The NLM brain is fully functional with all systems integrated:" << std::endl;
            std::cout << "✓ Memory systems (working, episodic, semantic, procedural)" << std::endl;
            std::cout << "✓ Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
            std::cout << "✓ Prediction and cognition systems" << std::endl;
            std::cout << "✓ Development and structural plasticity" << std::endl;
            std::cout << "✓ Checkpoint save/load and replay" << std::endl;
            std::cout << "✓ Closed-loop brain-world interaction" << std::endl;
            std::cout << std::endl;
            std::cout << "The artificial brain can now learn from experience, adapt over time, " << std::endl;
            std::cout << "and develop increasingly complex behaviors through interaction with its environment." << std::endl;
        } else {
            std::cerr << std::endl;
            std::cerr << "=== WARNING: Incomplete Integration ===" << std::endl;
            std::cerr << "The brain system has " << (totalSystems - connectedSystems) 
                      << " disconnected or non-functional system(s)." << std::endl;
            std::cerr << "This may be expected depending on the experiment configuration." << std::endl;
        }
        
    } catch (const std::exception& e) {
        auto endWall = std::chrono::high_resolution_clock::now();
        std::cerr << std::endl;
        std::cerr << "=== FATAL ERROR ===" << std::endl;
        std::cerr << "The Phase 6 integration demo encountered an exception:" << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << std::endl;
        std::cerr << "Wall clock time before error: " << std::chrono::duration<double>(endWall - startWall).count() << "s" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Please check the system logs for more details." << std::endl;
        return;
    }
    
    auto endWall = std::chrono::high_resolution_clock::now();
    double wallTime = std::chrono::duration<double>(endWall - startWall).count();
    
    std::cout << std::endl;
    std::cout << "Wall clock time: " << wallTime << "s" << std::endl;
}

// Enhanced validation function for experiment results
bool validateIntegrationResults(const Phase6IntegrationResult& result, bool memoryTest, 
                               bool neuromodulationTest, bool checkpointTest, bool replayTest) {
    // Critical systems that must be connected
    bool criticalMemorySystems = result.memoryWorkingMemoryIntegrated && 
                                  result.memoryEpisodicMemoryIntegrated;
    
    // Optional but important systems
    bool keySystems = result.neuromodulationIntegrated && 
                      result.predictionIntegrated && 
                      result.developmentIntegrated;
    
    // Reliability systems
    bool reliabilitySystems = result.checkpointingWorks;
    
    // Test results validation
    bool allTestsPassed = memoryTest && neuromodulationTest && replayTest;
    
    // Overall integration success criteria
    bool integrationSuccessful = criticalMemorySystems && 
                                keySystems && 
                                reliabilitySystems && 
                                allTestsPassed;
    
    // Additional sanity checks
    if (integrationSuccessful) {
        // Check for reasonable activity levels
        if (result.totalReward < 0 || result.totalReward > 1000) {
            std::cerr << "WARNING: Reward value seems out of range: " << result.totalReward << std::endl;
        }
        
        if (result.avgFiringRate < 0 || result.avgFiringRate > 10.0) {
            std::cerr << "WARNING: Average firing rate seems out of range: " << result.avgFiringRate << std::endl;
        }
        
        if (result.memoryEpisodesStored < 0) {
            std::cerr << "WARNING: Memory episodes stored is negative: " << result.memoryEpisodesStored << std::endl;
        }
    }
    
    return integrationSuccessful;
}
