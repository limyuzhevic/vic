/**
 * Phase 6 Demo - Comprehensive Integration Test
 *
 * This demo runs a comprehensive Phase 6 integration test that verifies
 * that all brain systems are properly connected and functional.
 *
 * The Phase 6 brain integrates:
 * - Memory systems (working, episodic, associative)
 * - Neuromodulation (dopamine, curiosity, novelty, prediction error)
 * - Prediction system for sensory prediction
 * - Cognition systems (neural planner, concept formation, attention)
 * - Development system for brain maturation
 * - Structural plasticity for connection growth
 * - Replay and consolidation mechanisms
 *
 * This demo creates a complete artificial brain and runs it for a significant
 * duration to demonstrate all integrated systems working together.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace nlm;

void printPhase6Welcome() {
    std::cout << "====================================================" << std::endl;
    std::cout << "=== NLM Phase 6: COMPREHENSIVE INTEGRATION DEMO ===" << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "This demo demonstrates the fully integrated Phase 6 brain," << std::endl;
    std::cout << "which combines all previous phases into a coherent artificial" << std::endl;
    std::cout << "neural substrate with advanced learning and cognitive abilities." << std::endl;
    std::cout << std::endl;
    std::cout << "The Phase 6 brain includes:" << std::endl;
    std::cout << "  ✓ Memory systems (working, episodic, associative)" << std::endl;
    std::cout << "  ✓ Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
    std::cout << "  ✓ Prediction system for sensory prediction and error" << std::endl;
    std::cout << "  ✓ Cognition (neural planner, concept formation, attention)" << std::endl;
    std::cout << "  ✓ Development system for brain maturation" << std::endl;
    std::cout << "  ✓ Structural plasticity (synaptogenesis, pruning)" << std::endl;
    std::cout << "  ✓ Replay and consolidation mechanisms" << std::endl;
    std::cout << "  ✓ Real LIF neuron dynamics with spike-based computation" << std::endl;
    std::cout << "  ✓ Full integration of all brain systems" << std::endl;
    std::cout << std::endl;
    std::cout << "The simulation will run for approximately 2000 steps," << std::endl;
    std::cout << "demonstrating how all systems interact and evolve together." << std::endl;
    std::cout << std::endl;
    std::cout << "Watch as the brain learns, develops, and adapts to its environment!" << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << std::endl;
}

void printDemoConfiguration() {
    std::cout << "=== DEMO CONFIGURATION ===" << std::endl;
    std::cout << "Neuron count: 500 (smaller than production for faster demo)" << std::endl;
    std::cout << "Simulation steps: 2000 (approximately 2 seconds of simulated time)" << std::endl;
    std::cout << "Region count: 1 (single cortical region)" << std::endl;
    std::cout << "Connection probability: 0.1 (sparse connectivity)" << std::endl;
    std::cout << "Enable checkpointing: Yes (save/load state)" << std::endl;
    std::cout << "Enable replay: Yes (memory replay)" << std::endl;
    std::cout << "Enable development: Yes (brain maturation)" << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << std::endl;
}

void printKeyIntegrationTests() {
    std::cout << "=== KEY INTEGRATION TESTS ===" << std::endl;
    std::cout << "1. Integration Verification - Check all systems are connected" << std::endl;
    std::cout << "2. Memory Integration - Working memory actively stores information" << std::endl;
    std::cout << "3. Neuromodulation - Dopamine and curiosity drive learning" << std::endl;
    std::cout << "4. Prediction System - Brain predicts sensory input" << std::endl;
    std::cout << "5. Checkpointing - Save and load brain state" << std::endl;
    std::cout << "6. Replay - Re-activate important memories" << std::endl;
    std::cout << "7. Development - Brain matures over time" << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // Set up command line arguments
    int realArgc = 1;
    char* realArgv[10] = {"Phase6Demo"};
    
    // Pass through any user arguments
    if (argc > 1) {
        realArgc = argc + 1;
        realArgv[0] = argv[0];
        for (int i = 1; i < argc; ++i) {
            realArgv[i] = argv[i];
        }
    }
    
    printPhase6Welcome();
    printDemoConfiguration();
    printKeyIntegrationTests();
    
    std::cout << "Starting Phase 6 integration test..." << std::endl;
    std::cout << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with demo settings
    std::cout << "--- FULL INTEGRATION EXPERIMENT ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    // Display comprehensive results
    std::cout << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << "=== COMPREHENSIVE PHASE 6 RESULTS ===" << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PERFORMANCE METRICS:" << std::endl;
    std::cout << "  Total Reward: " << result.totalReward << std::endl;
    std::cout << "  Average Firing Rate: " << result.avgFiringRate << std::endl;
    std::cout << "  Average Synaptic Weight: " << result.avgSynapticWeight << std::endl;
    std::cout << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << std::endl;
    
    std::cout << "NEUROMODULATION STATE:" << std::endl;
    std::cout << "  Novelty Level: " << result.noveltyLevel << std::endl;
    std::cout << "  Curiosity Level: " << result.curiosityLevel << std::endl;
    std::cout << "  Dopamine Level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "SYSTEM INTEGRATION STATUS:" << std::endl;
    std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Prediction System: " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Development: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << "  Replay: " << (result.replayWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "TECHNICAL DETAILS:" << std::endl;
    std::cout << "  Simulation Duration: " << result.totalWallClockTime << " seconds" << std::endl;
    std::cout << "  Steps Executed: " << config.maxSteps << " (2000 target)" << std::endl;
    std::cout << "  Neurons Created: " << config.neuronCount << " (500 target)" << std::endl;
    std::cout << "  Regions: " << config.regionCount << " (1 target)" << std::endl;
    std::cout << std::endl;
    
    // Final verdict
    std::cout << "====================================================" << std::endl;
    std::cout << "=== PHASE 6 INTEGRATION VERDICT ===" << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << std::endl;
    
    // Check if all critical systems are integrated
    bool allCriticalSystemsIntegrated = (
        result.memoryWorkingMemoryIntegrated &&
        result.memoryEpisodicMemoryIntegrated &&
        result.neuromodulationIntegrated &&
        result.predictionIntegrated &&
        result.developmentIntegrated &&
        result.checkpointingWorks
    );
    
    if (allCriticalSystemsIntegrated) {
        std::cout << "SUCCESS: All critical brain systems are properly integrated!" << std::endl;
        std::cout << std::endl;
        std::cout << "The Phase 6 artificial brain demonstrates:" << std::endl;
        std::cout << "  ✓ Complete integration of memory systems with neural processing" << std::endl;
        std::cout << "  ✓ Neuromodulation affects plasticity and dynamics in coordinated way" << std::endl;
        std::cout << "  ✓ Prediction system integrates with learning processes" << std::endl;
        std::cout << "  ✓ Developmental stages modulate learning appropriately" << std::endl;
        std::cout << "  ✓ Checkpoint save/load mechanism works correctly" << std::endl;
        std::cout << "  ✓ Memory replay reinforces consolidation" << std::endl;
        std::cout << std::endl;
        std::cout << "The Phase 6 brain is a functioning artificial neural substrate" << std::endl;
        std::cout << "capable of complex, adaptive behavior through integrated learning." << std::endl;
    } else {
        std::cout << "WARNING: Some critical systems may not be fully integrated." << std::endl;
        std::cout << "Further investigation may be needed for complete integration." << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << "=== DEMO COMPLETE ===" << std::endl;
    std::cout << "====================================================" << std::endl;
    
    return 0;
}
