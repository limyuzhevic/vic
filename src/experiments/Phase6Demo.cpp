#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <string>

using namespace nlm;

/**
 * Enhanced Phase 6 Integration Demo
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected and generates meaningful rewards
 * through proper agent-world interaction.
 * 
 * Usage: ./nlm_phase6_demo [--help] [--version] [--config=<file>] 
 *                       [--neuron_count=<n>] [--max_steps=<n>]
 *                       [--enable_checkpointing] [--enable_replay]
 *                       [--enable_development] [--checkpoint_path=<path>]
 *                       [--reward_scale=<factor>] [--penalty_scale=<factor>]
 */

int main(int argc, char* argv[]) {
    // Check for help/version requests before processing other arguments
    bool helpRequested = false;
    bool versionRequested = false;
    std::string configFile;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle help/version requests
        if (arg == "--help" || arg == "-h" || arg == "--usage") {
            helpRequested = true;
            continue;
        } else if (arg == "--version" || arg == "-v") {
            versionRequested = true;
            continue;
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            }
            continue;
        }
        
        // Handle config file loading (legacy support)
        if (configFile.empty() && arg.find('=') != std::string::npos && 
            (arg.substr(0, 7) == "--config" || arg.substr(0, 2) == "-c")) {
            size_t pos = arg.find('=');
            configFile = arg.substr(pos + 1);
            continue;
        }
    }
    
    // Handle help/version requests
    if (helpRequested) {
        std::cout << "NLM (Neural Learning Machine) - Phase 6 Integration Demo" << std::endl;
        std::cout << "=============================================" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: ./nlm_phase6_demo [--help] [--version] [--config=<file>] [args...]" << std::endl;
        std::cout << std::endl;
        std::cout << "General Options:" << std::endl;
        std::cout << "  --help, -h          Show this help message" << std::endl;
        std::cout << "  --version, -v       Show version information" << std::endl;
        std::cout << "  --config=<file>, -c Load configuration from file" << std::endl;
        std::cout << std::endl;
        std::cout << "Phase 6 Configuration Options:" << std::endl;
        std::cout << "  --neuron_count=<n>     Number of neurons (default: 500)" << std::endl;
        std::cout << "  --max_steps=<n>        Maximum simulation steps" << std::endl;
        std::cout << "  --region_count=<n>     Number of neural regions (default: 1)" << std::endl;
        std::cout << "  --connection_probability=<p> Probability of connections (default: 0.1)" << std::endl;
        std::cout << "  --random_seed=<s>       Random seed for reproducible results" << std::endl;
        std::cout << "  --simulation_timestep=<t> Time step in seconds (default: 0.001)" << std::endl;
        std::cout << "  --enable_checkpointing   Enable checkpoint save/load functionality" << std::endl;
        std::cout << "  --enable_replay          Enable memory replay functionality" << std::endl;
        std::cout << "  --enable_development     Enable developmental plasticity" << std::endl;
        std::cout << "  --checkpoint_path=<path> Path for checkpoint files" << std::endl;
        std::cout << std::endl;
        std::cout << "Environment Options:" << std::endl;
        std::cout << "  --world_width=<w>       World width (default: 16.0)" << std::endl;
        std::cout << "  --world_height=<h>      World height (default: 16.0)" << std::endl;
        std::cout << "  --reward_scale=<f>      Scale factor for positive rewards" << std::endl;
        std::cout << "  --penalty_scale=<f>     Scale factor for negative penalties" << std::endl;
        std::cout << std::endl;
        std::cout << "Advanced Options:" << std::endl;
        std::cout << "  --verbose, -v           Enable verbose logging" << std::endl;
        std::cout << "  --quiet, -q             Disable logging output" << std::endl;
        std::cout << "  --debug                 Enable debug mode" << std::endl;
        return 0;
    }
    
    if (versionRequested) {
        std::cout << "NLM (Neural Learning Machine) Version 0.1.0" << std::endl;
        std::cout << "Phase 6: Final Integration" << std::endl;
        std::cout << "Built with C++20, CMake 3.16+" << std::endl;
        std::cout << std::endl;
        std::cout << "This is an experimental artificial developmental brain." << std::endl;
        std::cout << "The long-term goal is to create a neural system that begins in a" << std::endl;
        std::cout << "primitive developmental state and acquires increasingly complex" << std::endl;
        std::cout << "abilities through interaction with an environment." << std::endl;
        return 0;
    }
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain with meaningful reward generation..." << std::endl << std::endl;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        std::cerr << "The brain systems are not properly integrated. Exiting." << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems with more comprehensive tests
    std::cout << "--- Memory Integration Test ---" << std::endl;
    std::cout << "Testing working memory, episodic memory, and associative memory..." << std::endl;
    if (experiment.testMemoryIntegration()) {
        std::cout << "✓ Memory integration test passed!" << std::endl;
    } else {
        std::cout << "✗ Memory integration test failed!" << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    std::cout << "Testing dopamine, curiosity, novelty, and prediction error systems..." << std::endl;
    if (experiment.testNeuromodulationIntegration()) {
        std::cout << "✓ Neuromodulation integration test passed!" << std::endl;
    } else {
        std::cout << "✗ Neuromodulation integration test failed!" << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    std::cout << "Testing checkpoint save/load functionality..." << std::endl;
    if (experiment.testCheckpointing()) {
        std::cout << "✓ Checkpoint test passed!" << std::endl;
    } else {
        std::cout << "✗ Checkpoint test failed!" << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    std::cout << "Testing memory replay and consolidation..." << std::endl;
    if (experiment.testReplay()) {
        std::cout << "✓ Replay test passed!" << std::endl;
    } else {
        std::cout << "✗ Replay test failed!" << std::endl;
    }
    std::cout << std::endl;
    
    // Run full integration experiment with comprehensive configuration
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Running complete lifetime simulation with all integrated systems..." << std::endl;
    
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./phase6_checkpoint.bin";
    
    // Print experiment details
    std::cout << "Experiment Configuration:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Max Steps: " << config.maxSteps << std::endl;
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "enabled" : "disabled") << std::endl;
    std::cout << "  Replay: " << (config.enableReplay ? "enabled" : "disabled") << std::endl;
    std::cout << "  Development: " << (config.enableDevelopment ? "enabled" : "disabled") << std::endl;
    std::cout << std::endl;
    
    // Run the integration experiment
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Final assessment
    std::cout << std::endl;
    std::cout << "=== FINAL ASSESSMENT ===" << std::endl;
    bool hasMeaningfulLearning = (result.totalReward > 0.0f && result.totalReward < 1000.0f);
    bool hasIntegratedMemory = result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated;
    bool hasFunctionalNeuromodulation = result.neuromodulationIntegrated;
    bool hasDevelopmentalCapability = result.developmentIntegrated;
    bool hasReliableCheckpointing = result.checkpointingWorks;
    
    std::cout << "Meaningful reward generation: " << (hasMeaningfulLearning ? "YES" : "NO") << std::endl;
    std::cout << "Integrated memory systems: " << (hasIntegratedMemory ? "YES" : "NO") << std::endl;
    std::cout << "Functional neuromodulation: " << (hasFunctionalNeuromodulation ? "YES" : "NO") << std::endl;
    std::cout << "Developmental capability: " << (hasDevelopmentalCapability ? "YES" : "NO") << std::endl;
    std::cout << "Reliable checkpointing: " << (hasReliableCheckpointing ? "YES" : "NO") << std::endl;
    std::cout << std::endl;
    
    if (hasMeaningfulLearning && hasIntegratedMemory && hasFunctionalNeuromodulation && 
        hasDevelopmentalCapability && hasReliableCheckpointing) {
        std::cout << "✓ PHASE 6 INTEGRATION SUCCESSFUL!" << std::endl;
        std::cout << "All brain systems are properly integrated and generating meaningful rewards." << std::endl;
        return 0;
    } else {
        std::cout << "✗ PHASE 6 INTEGRATION INCOMPLETE!" << std::endl;
        std::cout << "Some systems are not properly integrated or generating meaningful rewards." << std::endl;
        std::cout << "See the output above for specific integration status." << std::endl;
        return 1;
    }
}