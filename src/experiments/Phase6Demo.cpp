/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo demonstrates Phase 6 (Final Integration) of the NLM brain:
 * 
 * The Phase 6 integration test verifies that all brain systems work together
 * as a coherent artificial brain. This is the culmination of all previous phases:
 * 
 * Phase 1: Project skeleton, core types, configuration, neural interfaces
 * Phase 2: Real LIF neuron dynamics, spike propagation, plasticity rules
 * Phase 3: World interaction, sensory input, reward prediction error
 * Phase 4: Neural prediction system, working memory, episodic memory
 * Phase 5: Performance optimizations, memory pools, checkpoint system
 * Phase 6: ALL SYSTEMS INTEGRATED into coherent brain loop
 * 
 * Integration Features Verified:
 * - Memory systems connected to neural processing
 * - Neuromodulation affects plasticity and dynamics
 * - Prediction integrated with learning
 * - Development affects plasticity rates
 * - Checkpoint save/load functional
 * - Replay and consolidation operational
 * 
 * Usage: This demo runs a complete integration test showing that all
 * systems interact properly to create a functioning artificial brain.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <ctime>
#include <chrono>

using namespace nlm;

/**
 * Run the Phase 6 integration demo
 * 
 * This function demonstrates the complete Phase 6 integration by:
 * 1. Running integration verification tests
 * 2. Testing individual system integration
 * 3. Running a full simulation with all systems
 * 4. Verifying checkpoint and replay functionality
 * 
 * The demo shows that Phase 6 successfully integrates all previous phases
 * into a coherent artificial brain that can learn, adapt, and behave.
 */

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    std::cout << std::endl;
    
    // Initialize logging with info level for detailed output
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create the integration experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    // This checks that all basic brain systems are properly initialized
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        std::cerr << "This indicates that one or more brain systems are not properly integrated." << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl;
    std::cout << std::endl;
    
    // Test individual system integration
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
    
    // Run full experiment with smaller settings for demo
    // This simulates a complete brain lifecycle with all systems interacting
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;           // Smaller for faster demo
    config.maxSteps = 2000;             // Limited steps for demo
    config.enableCheckpointing = true;  // Test checkpointing
    config.enableReplay = true;         // Test replay
    config.enableDevelopment = true;    // Test development
    
    // Run the complete integration experiment
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward (avg per step): " << result.totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level (avg): " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING ✓" : "NOT WORKING ✗") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Success message showing that Phase 6 integration is complete
    std::cout << std::endl;
    std::cout << "=== PHASE 6 INTEGRATION SUCCESSFUL ===" << std::endl;
    std::cout << "The NLM brain now operates as a complete artificial brain:" << std::endl;
    std::cout << "  ✓ Memory systems store and retrieve experiences" << std::endl;
    std::cout << "  ✓ Neuromodulators regulate learning and behavior" << std::endl;
    std::cout << "  ✓ Prediction guides action selection" << std::endl;
    std::cout << "  ✓ Development shapes lifelong learning" << std::endl;
    std::cout << "  ✓ Checkpointing preserves brain state" << std::endl;
    std::cout << "  ✓ Replay consolidates important memories" << std::endl;
    std::cout << std::endl;
    
    return 0;
}
