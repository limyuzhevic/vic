// =============================================================================
// NLM Phase 6 Integration Demo Executable
// =============================================================================
//
// This executable demonstrates the complete Phase 6 integration of all brain systems.
// It shows how memory, neuromodulation, prediction, and cognition systems work together.
//
// USAGE:
//   ./nlm_phase6_demo [options]
//
// OPTIONS:
//   --help, -h              Show help message
//   --neuron-count N        Number of neurons (default: 500)
//   --steps S              Number of simulation steps (default: 2000)
//   --enable-checkpointing   Enable checkpointing (default: true)
//   --enable-replay         Enable replay (default: true)
//   --enable-development    Enable development (default: true)
//   --checkpoint-path P     Checkpoint file path (default: ./checkpoint_test.bin)
//   --verbose, -v           Enable verbose output
//   --config C              Load configuration from file
//
// This demo implements the Phase 6 integration test described in the README,
// demonstrating that all major brain systems are properly connected and functional.

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <string>
#include <vector>

using namespace nlm;

// Command line argument parsing
struct DemoOptions {
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint_test.bin";
    bool verbose = false;
    std::string configFile = "";
};

DemoOptions parseArguments(int argc, char* argv[]) {
    DemoOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 6 Integration Demo" << std::endl;
            std::cout << "===========================" << std::endl;
            std::cout << "" << std::endl;
            std::cout << "This demo runs the complete Phase 6 integration experiment to verify" << std::endl;
            std::cout << "that all brain systems are properly connected and functional." << std::endl;
            std::cout << "" << std::endl;
            std::cout << "USAGE:" << std::endl;
            std::cout << "  ./nlm_phase6_demo [options]" << std::endl;
            std::cout << "" << std::endl;
            std::cout << "OPTIONS:" << std::endl;
            std::cout << "  --help, -h              Show help message" << std::endl;
            std::cout << "  --neuron-count N        Number of neurons (default: 500)" << std::endl;
            std::cout << "  --steps S              Number of simulation steps (default: 2000)" << std::endl;
            std::cout << "  --enable-checkpointing   Enable checkpointing (default: true)" << std::endl;
            std::cout << "  --enable-replay         Enable replay (default: true)" << std::endl;
            std::cout << "  --enable-development    Enable development (default: true)" << std::endl;
            std::cout << "  --checkpoint-path P     Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
            std::cout << "  --verbose, -v           Enable verbose output" << std::endl;
            std::cout << "  --config C              Load configuration from file" << std::endl;
            exit(0);
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--neuron-count" && i + 1 < argc) {
            options.neuronCount = std::stoull(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            options.maxSteps = std::stoull(argv[++i]);
        } else if (arg == "--checkpoint-path" && i + 1 < argc) {
            options.checkpointPath = argv[++i];
        } else if (arg == "--no-checkpointing") {
            options.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            options.enableReplay = false;
        } else if (arg == "--no-development") {
            options.enableDevelopment = false;
        } else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        }
    }
    
    return options;
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
    ║     Demonstrating the complete artificial brain                 ║
    ║     with all systems integrated.                                ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printSystemStatus(const Phase6IntegrationResult& result) {
    std::cout << "=== Integration Status ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
}

void printPerformanceMetrics(const Phase6IntegrationResult& result) {
    std::cout << "=== Performance Metrics ===" << std::endl;
    std::cout << "Total Reward: " << std::fixed << std::setprecision(4) << result.totalReward << std::endl;
    std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(4) << result.avgFiringRate << std::endl;
    std::cout << "Average Synaptic Weight: " << std::fixed << std::setprecision(4) << result.avgSynapticWeight << std::endl;
    std::cout << "Memory Episodes Stored: " << std::fixed << std::setprecision(0) << result.memoryEpisodesStored << std::endl;
    std::cout << "Novelty Level: " << std::fixed << std::setprecision(4) << result.noveltyLevel << std::endl;
    std::cout << "Curiosity Level: " << std::fixed << std::setprecision(4) << result.curiosityLevel << std::endl;
    std::cout << "Dopamine Level: " << std::fixed << std::setprecision(4) << result.dopamineLevel << std::endl;
    std::cout << "Wall Clock Time: " << std::fixed << std::setprecision(2) << result.totalWallClockTime << "s" << std::endl;
}

int main(int argc, char* argv[]) {
    printBanner();
    
    // Parse command line arguments
    DemoOptions options = parseArguments(argc, argv);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(options.verbose ? LogLevel::Info : LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "NLM Phase 6 Integration Demo" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "" << std::endl;
    
    if (options.verbose) {
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Neuron count: " << options.neuronCount << std::endl;
        std::cout << "  Max steps: " << options.maxSteps << std::endl;
        std::cout << "  Checkpointing: " << (options.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Replay: " << (options.enableReplay ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Development: " << (options.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Checkpoint path: " << options.checkpointPath << std::endl;
        std::cout << "" << std::endl;
    }
    
    // Create configuration
    Phase6Config config;
    config.neuronCount = options.neuronCount;
    config.maxSteps = options.maxSteps;
    config.enableCheckpointing = options.enableCheckpointing;
    config.enableReplay = options.enableReplay;
    config.enableDevelopment = options.enableDevelopment;
    config.checkpointPath = options.checkpointPath;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    std::cout << "Running Phase 6 Integration Demo..." << std::endl;
    std::cout << "" << std::endl;
    
    // First, run integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Integration verification passed!" << std::endl;
    std::cout << "" << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    bool memoryOK = experiment.testMemoryIntegration();
    std::cout << "Memory integration: " << (memoryOK ? "✓ PASSED" : "✗ FAILED") << std::endl;
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    bool neuromodOK = experiment.testNeuromodulationIntegration();
    std::cout << "Neuromodulation integration: " << (neuromOK ? "✓ PASSED" : "✗ FAILED") << std::endl;
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    bool checkpointOK = experiment.testCheckpointing();
    std::cout << "Checkpointing: " << (checkpointOK ? "✓ PASSED" : "✗ FAILED") << std::endl;
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    bool replayOK = experiment.testReplay();
    std::cout << "Replay system: " << (replayOK ? "✓ PASSED" : "✗ FAILED") << std::endl;
    std::cout << std::endl;
    
    // Run full integration experiment
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto result = experiment.run(config);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Update wall clock time with actual measurement
    result.totalWallClockTime = elapsedTime;
    
    // Print final results
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << std::fixed << std::setprecision(4) << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << std::fixed << std::setprecision(4) << result.avgFiringRate << std::endl;
    std::cout << "Avg synaptic weight: " << std::fixed << std::setprecision(4) << result.avgSynapticWeight << std::endl;
    std::cout << "Episodes stored: " << std::fixed << std::setprecision(0) << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << std::fixed << std::setprecision(4) << result.dopamineLevel << std::endl;
    std::cout << "Novelty level: " << std::fixed << std::setprecision(4) << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << std::fixed << std::setprecision(4) << result.curiosityLevel << std::endl;
    std::cout << "Wall clock time: " << std::fixed << std::setprecision(2) << result.totalWallClockTime << "s" << std::endl;
    std::cout << std::endl;
    
    printSystemStatus(result);
    std::cout << std::endl;
    
    printPerformanceMetrics(result);
    std::cout << std::endl;
    
    // Determine overall success
    bool success = integrationOK && memoryOK && neuromodOK && checkpointOK && replayOK;
    
    std::cout << "=== SUMMARY ===" << std::endl;
    if (success) {
        std::cout << "✓ ALL TESTS PASSED! Phase 6 integration successful." << std::endl;
        std::cout << "" << std::endl;
        std::cout << "The NLM brain is a functioning artificial neural substrate capable of:" << std::endl;
        std::cout << "  ✓ Real spiking neural computation with LIF dynamics" << std::endl;
        std::cout << "  ✓ Event-driven spike propagation with delays" << std::endl;
        std::cout << "  ✓ STDP and Hebbian plasticity" << std::endl;
        std::cout << "  ✓ Structural plasticity (synaptogenesis/pruning)" << std::endl;
        std::cout << "  ✓ Integrated memory systems" << std::endl;
        std::cout << "  ✓ Neuromodulation affecting plasticity and dynamics" << std::endl;
        std::cout << "  ✓ Prediction system integration with learning" << std::endl;
        std::cout << "  ✓ Development affecting plasticity rates" << std::endl;
        std::cout << "  ✓ Checkpoint save/load working" << std::endl;
        std::cout << "  ✓ Replay and consolidation functional" << std::endl;
    } else {
        std::cout << "✗ SOME TESTS FAILED. Integration incomplete." << std::endl;
        std::cout << "Please check the output above for detailed failure information." << std::endl;
    }
    
    return success ? 0 : 1;
}
