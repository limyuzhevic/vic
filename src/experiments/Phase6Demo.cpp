/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <chrono>

using namespace nlm;
#include <chrono>

// Utility function to get current timestamp as string
std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Timing utility for performance monitoring
#include <chrono>

// Timing function
class Timer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::string name;
    
public:
    Timer(const std::string& timerName) : name(timerName) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    ~Timer() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        NLM_LOG_INFO("Timer " + name + ": " + std::to_string(duration.count()) + "ms");
    }
};

// Configuration helper functions
Phase6Config loadPhase6ConfigFromArgs(int argc, char* argv[]) {
    Phase6Config config;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--neuron-count" && i + 1 < argc) {
            config.neuronCount = std::stoull(argv[++i]);
        } else if (arg == "--max-steps" && i + 1 < argc) {
            config.maxSteps = std::stoull(argv[++i]);
        } else if (arg == "--region-count" && i + 1 < argc) {
            config.regionCount = std::stoull(argv[++i]);
        } else if (arg == "--connection-prob" && i + 1 < argc) {
            config.connectionProbability = std::stof(argv[++i]);
        } else if (arg == "--checkpoint-path" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        } else if (arg == "--enable-checkpointing") {
            config.enableCheckpointing = true;
        } else if (arg == "--enable-replay") {
            config.enableReplay = true;
        } else if (arg == "--enable-development") {
            config.enableDevelopment = true;
        } else if (arg == "--disable-checkpointing") {
            config.enableCheckpointing = false;
        } else if (arg == "--disable-replay") {
            config.enableReplay = false;
        } else if (arg == "--disable-development") {
            config.enableDevelopment = false;
        } else if (arg == "--help" || arg == "-h") {
            printUsage();
            exit(0);
        }
    }
    
    return config;
}

void printUsage() {
    std::cout << "=== NLM Phase 6 Demo Usage ===" << std::endl;
    std::cout << "Usage: ./nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --neuron-count N        Number of neurons (default: 500)" << std::endl;
    std::cout << "  --max-steps S           Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --region-count R        Number of neural regions (default: 1)" << std::endl;
    std::cout << "  --connection-prob P     Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --checkpoint-path P     Path for checkpoint file (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "  --enable-checkpointing  Enable checkpoint save/load (default: true)" << std::endl;
    std::cout << "  --disable-checkpointing Disable checkpoint save/load" << std::endl;
    std::cout << "  --enable-replay         Enable memory replay (default: true)" << std::endl;
    std::cout << "  --disable-replay        Disable memory replay" << std::endl;
    std::cout << "  --enable-development    Enable developmental plasticity (default: true)" << std::endl;
    std::cout << "  --disable-development   Disable developmental plasticity" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neuron-count 1000 --max-steps 5000" << std::endl;
    std::cout << "  ./nlm_phase6_demo --disable-checkpointing --disable-replay" << std::endl;
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Integration test demonstrating all brain systems working   ║
    ║     together as a coherent artificial brain.                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main(int argc, char* argv[]) {
    printBanner();
    
    // Log startup
    NLM_LOG_INFO("=== NLM Phase 6 Integration Demo ===");
    NLM_LOG_INFO("Starting integration test at " + getTimestamp());
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Load configuration from command line arguments
    Phase6Config config = loadPhase6ConfigFromArgs(argc, argv);
    
    NLM_LOG_INFO("Loaded configuration: " + std::to_string(config.neuronCount) + " neurons, " 
              + std::to_string(config.maxSteps) + " steps, "
              + (config.enableCheckpointing ? "checkpointing enabled, " : "checkpointing disabled, ")
              + (config.enableReplay ? "replay enabled, " : "replay disabled, ")
              + (config.enableDevelopment ? "development enabled" : "development disabled"));
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    std::cout << "Configuration: " << config.neuronCount << " neurons, " 
              << config.maxSteps << " steps, "
              << (config.enableCheckpointing ? "checkpointing enabled, " : "checkpointing disabled, ")
              << (config.enableReplay ? "replay enabled, " : "replay disabled, ")
              << (config.enableDevelopment ? "development enabled" : "development disabled")
              << std::endl << std::endl;
    
    // Create experiment
    NLM_LOG_INFO("Creating Phase 6 integrated experiment...");
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    NLM_LOG_INFO("Running integration verification test...");
    std::cout << "--- Integration Verification ---" << std::endl;
    
    bool integrationOK;
    {
        Timer verificationTimer("Integration Verification");
        integrationOK = experiment.verifyIntegration();
    }
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        NLM_LOG_ERROR("Integration verification failed!");
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    NLM_LOG_INFO("Integration verification completed successfully");
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    NLM_LOG_INFO("Testing memory integration...");
    
    {
        Timer memoryTimer("Memory Integration");
        bool memoryTestResult = experiment.testMemoryIntegration();
    }
    
    std::cout << std::endl;
    if (!memoryTestResult) {
        NLM_LOG_ERROR("Memory integration test failed");
    }
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    NLM_LOG_INFO("Testing neuromodulation integration...");
    
    {
        Timer neuromodulationTimer("Neuromodulation Integration");
        bool neuromodulationTestResult = experiment.testNeuromodulationIntegration();
    }
    
    std::cout << std::endl;
    if (!neuromodulationTestResult) {
        NLM_LOG_ERROR("Neuromodulation integration test failed");
    }
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    NLM_LOG_INFO("Testing checkpoint save/load functionality...");
    
    {
        Timer checkpointTimer("Checkpoint Test");
        bool checkpointTestResult = experiment.testCheckpointing();
    }
    
    std::cout << std::endl;
    if (!checkpointTestResult) {
        NLM_LOG_ERROR("Checkpoint test failed");
    }
    
    std::cout << "--- Replay Test ---" << std::endl;
    NLM_LOG_INFO("Testing memory replay system...");
    
    {
        Timer replayTimer("Replay Test");
        bool replayTestResult = experiment.testReplay();
    }
    
    std::cout << std::endl;
    if (!replayTestResult) {
        NLM_LOG_ERROR("Replay test failed");
    }
    
    // Run full experiment with configured settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    NLM_LOG_INFO("Running full integration experiment...");
    NLM_LOG_INFO("Configuration: " + std::to_string(config.neuronCount) + " neurons, " +
                 std::to_string(config.maxSteps) + " steps");
    
Timer fullExperimentTimer("Full Experiment");
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
    
    NLM_LOG_INFO("=== Experiment Complete ===");
    NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("Avg firing rate: " + std::to_string(result.avgFiringRate));
    NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
    NLM_LOG_INFO("Wall time: " + std::to_string(result.totalWallClockTime) + "s");
    
    NLM_LOG_INFO("=== NLM Phase 6 Integration Demo Complete ===");
    NLM_LOG_INFO("Integration test finished successfully at " + getTimestamp());
    
    return 0;
}
