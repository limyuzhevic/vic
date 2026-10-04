/**
 * Phase 6 Demo - Integration Test
 *
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 *
 * Usage:
 *   ./nlm_phase6_demo [options]
 *   Options:
 *     --config <file>    Load configuration from file
 *     --neurons <n>      Number of neurons (default: 500)
 *     --steps <n>        Max steps (default: 2000)
 *     --no-checkpoint    Disable checkpointing
 *     --no-replay        Disable replay
 *     --no-development   Disable development
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace nlm;

// Helper function to parse command line arguments
bool parseArgs(int argc, char* argv[], Phase6Config& config, std::string& configFile) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: ./nlm_phase6_demo [options]\n"
                      << "  --config <file>    Load configuration from file\n"
                      << "  --neurons <n>      Number of neurons (default: 500)\n"
                      << "  --steps <n>        Max steps (default: 2000)\n"
                      << "  --no-checkpoint    Disable checkpointing\n"
                      << "  --no-replay        Disable replay\n"
                      << "  --no-development   Disable development\n"
                      << "  --checkpoint-path <path> Path for checkpoint files\n"
                      << "  --help, -h         Show this help message\n";
            return false;
        }
        
        if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        }
        else if (arg.find("--config=") == 0) {
            configFile = arg.substr(9);
        }
        else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoul(argv[++i]);
        }
        else if (arg.find("--neurons=") == 0) {
            config.neuronCount = std::stoul(arg.substr(10));
        }
        else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoull(argv[++i]);
        }
        else if (arg.find("--steps=") == 0) {
            config.maxSteps = std::stoull(arg.substr(8));
        }
        else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        }
        else if (arg == "--no-replay") {
            config.enableReplay = false;
        }
        else if (arg == "--no-development") {
            config.enableDevelopment = false;
        }
        else if (arg == "--checkpoint-path" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        }
        else if (arg.find("--checkpoint-path=") == 0) {
            config.checkpointPath = arg.substr(19);
        }
        else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::cerr << "Use --help for usage information.\n";
            return false;
        }
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Parse command line arguments
    Phase6Config config;
    std::string configFile;
    
    if (!parseArgs(argc, argv, config, configFile)) {
        return 1;
    }
    
    // Load configuration from file if specified
    if (!configFile.empty()) {
        std::cout << "Loading configuration from: " << configFile << std::endl;
        auto cfg = std::make_shared<Config>();
        if (cfg->loadFromFile(configFile)) {
            std::cout << "Configuration loaded successfully." << std::endl;
        } else {
            std::cout << "Using default configuration (file not found)." << std::endl;
        }
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    std::cout << "Configuration summary:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Max steps: " << config.maxSteps << std::endl;
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "enabled" : "disabled") << std::endl;
    std::cout << "  Replay: " << (config.enableReplay ? "enabled" : "disabled") << std::endl;
    std::cout << "  Development: " << (config.enableDevelopment ? "enabled" : "disabled") << std::endl;
    std::cout << "  Checkpoint path: " << config.checkpointPath << std::endl;
    std::cout << std::endl;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
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
    bool memoryOK = experiment.testMemoryIntegration();
    if (!memoryOK) {
        std::cerr << "WARNING: Memory integration test failed!" << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    bool neuromodulationOK = experiment.testNeuromodulationIntegration();
    if (!neuromodulationOK) {
        std::cerr << "WARNING: Neuromodulation integration test failed!" << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    bool checkpointOK = experiment.testCheckpointing();
    if (!checkpointOK) {
        std::cerr << "WARNING: Checkpoint test failed!" << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    bool replayOK = experiment.testReplay();
    if (!replayOK) {
        std::cerr << "WARNING: Replay test failed!" << std::endl;
    }
    std::cout << std::endl;
    
    // Run full experiment with provided settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Running with " << config.neuronCount << " neurons for up to " << config.maxSteps << " steps..." << std::endl;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
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
    
    // Overall status
    std::cout << std::endl;
    std::cout << "=== OVERALL STATUS ===" << std::endl;
    if (integrationOK && memoryOK && neuromodulationOK && checkpointOK && replayOK) {
        std::cout << "✓ ALL TESTS PASSED: Phase 6 integration is successful!" << std::endl;
    } else {
        std::cout << "⚠ SOME TESTS FAILED: Review output for details" << std::endl;
    }
    
    return 0;
}
