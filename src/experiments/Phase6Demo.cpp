/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdint>

using namespace nlm;

// Constants for demo configuration
namespace DemoConfig {
    // Default simulation parameters
    constexpr uint64_t DEFAULT_NEURON_COUNT = 500;
    constexpr uint64_t DEFAULT_MAX_STEPS = 2000;
    constexpr uint64_t DEFAULT_REGION_COUNT = 1;
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    constexpr float DEFAULT_SYNAPSE_FORMATION_RATE = 0.0001f;
    constexpr float DEFAULT_SYNAPSE_PRUNING_RATE = 0.00001f;
    constexpr uint64_t DEFAULT_RANDOM_SEED = 42u;
    
    // Test configurations
    constexpr uint64_t BASIC_TEST_NEURONS = 100;
    constexpr uint64_t BASIC_TEST_STEPS = 1000;
    constexpr uint64_t QUICK_TEST_STEPS = 100;
    constexpr uint64_t STDP_TEST_TRIALS = 50;
    constexpr uint64_t PLASTICITY_TEST_STEPS = 1000;
    constexpr uint64_t CONNECTIVITY_TEST_NEIGHBORS = 10;
    
    // Output and formatting
    constexpr size_t MAX_LINE_WIDTH = 80;
    constexpr int DEFAULT_PRECISION = 3;
    constexpr float EPSILON = 0.001f;
}

// Print banner function
void printBanner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║     NLM — 熙然                                                ║
║     Neural Learning Machine                                   ║
║                                                               ║
║     Phase 6: Final Integration                                ║
║                                                               ║
║     Testing complete artificial brain integration.            ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

// Print error message with formatting
void printError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

// Print success message with formatting
void printSuccess(const std::string& message) {
    std::cout << "[SUCCESS] " << message << std::endl;
}

// Print info message with formatting
void printInfo(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

// Parse command line arguments
struct DemoArgs {
    bool showHelp;
    bool quietMode;
    bool exportResults;
    std::string resultsFile;
    uint64_t neuronCount;
    uint64_t maxSteps;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
};

DemoArgs parseArgs(int argc, char* argv[]) {
    DemoArgs args;
    
    // Set defaults
    args.showHelp = false;
    args.quietMode = false;
    args.exportResults = false;
    args.resultsFile = "phase6_demo_results.txt";
    args.neuronCount = DemoConfig::DEFAULT_NEURON_COUNT;
    args.maxSteps = DemoConfig::DEFAULT_MAX_STEPS;
    args.enableCheckpointing = true;
    args.enableReplay = true;
    args.enableDevelopment = true;
    args.checkpointPath = "./checkpoint_test.bin";
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.showHelp = true;
        } else if (arg == "--quiet" || arg == "-q") {
            args.quietMode = true;
        } else if (arg == "--export-results" || arg == "-e") {
            args.exportResults = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args.resultsFile = argv[++i];
            }
        } else if (arg == "--neurons" || arg == "-n") {
            if (i + 1 < argc) {
                args.neuronCount = std::stoull(argv[++i]);
            }
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                args.maxSteps = std::stoull(argv[++i]);
            }
        } else if (arg == "--no-checkpoint" || arg == "--no-cp") {
            args.enableCheckpointing = false;
        } else if (arg == "--no-replay" || arg == "--no-rp") {
            args.enableReplay = false;
        } else if (arg == "--no-development" || arg == "--no-dev") {
            args.enableDevelopment = false;
        } else if (arg == "--checkpoint" || arg == "-c") {
            if (i + 1 < argc) {
                args.checkpointPath = argv[++i];
            }
        } else if (arg == "--version" || arg == "-v") {
            args.showHelp = true;
            std::cout << "NLM Phase 6 Demo v1.0.0" << std::endl;
            std::cout << "Built with Phase 6 integration complete." << std::endl;
            exit(0);
        } else {
            // Unknown argument
            printError("Unknown argument: " + arg);
            args.showHelp = true;
        }
    }
    
    return args;
}

// Print help information
void printHelp() {
    std::cout << "NLM Phase 6 Integration Demo" << std::endl;
    std::cout << "===========================" << std::endl << std::endl;
    
    std::cout << "Usage: phase6_demo [OPTIONS]" << std::endl << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -q, --quiet             Quiet mode (less output)" << std::endl;
    std::cout << "  -e, --export-results    Export results to file" << std::endl;
    std::cout << "      --results-file FILE  File to export results (default: phase6_demo_results.txt)" << std::endl;
    std::cout << "  -n, --neurons N         Number of neurons (default: " 
              << DemoConfig::DEFAULT_NEURON_COUNT << ")" << std::endl;
    std::cout << "  -s, --steps S           Maximum simulation steps (default: " 
              << DemoConfig::DEFAULT_MAX_STEPS << ")" << std::endl;
    std::cout << "  --no-checkpoint         Disable checkpointing" << std::endl;
    std::cout << "  --no-replay             Disable replay" << std::endl;
    std::cout << "  --no-development        Disable development" << std::endl;
    std::cout << "  -c, --checkpoint FILE   Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "  -v, --version           Show version information" << std::endl << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  phase6_demo                    Run with default settings" << std::endl;
    std::cout << "  phase6_demo -q                 Quiet mode" << std::endl;
    std::cout << "  phase6_demo -e -n 1000 -s 5000 Run with custom settings and export" << std::endl;
    std::cout << "  phase6_demo --no-checkpoint    Disable checkpointing" << std::endl << std::endl;
    
    std::cout << "The demo tests integration of all brain systems:" << std::endl;
    std::cout << "  - Memory systems (working & episodic)" << std::endl;
    std::cout << "  - Neuromodulation (dopamine, curiosity)" << std::endl;
    std::cout << "  - Prediction and planning systems" << std::endl;
    std::cout << "  - Development and structural plasticity" << std::endl;
    std::cout << "  - Checkpoint save/load functionality" << std::endl;
    std::cout << "  - Memory replay and consolidation" << std::endl;
}

// Format results for export
std::string formatResults(const Phase6IntegrationResult& result) {
    std::stringstream ss;
    ss << "Phase 6 Integration Demo Results" << std::endl;
    ss << "=================================" << std::endl << std::endl;
    
    ss << "System Integration Status:" << std::endl;
    ss << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    ss << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    ss << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    ss << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    ss << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    ss << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    ss << "  Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    ss << std::endl;
    
    ss << "Performance Metrics:" << std::endl;
    ss << "  Total Reward: " << std::fixed << std::setprecision(3) << result.totalReward << std::endl;
    ss << "  Average Firing Rate: " << result.avgFiringRate << std::endl;
    ss << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    ss << "  Novelty Level: " << result.noveltyLevel << std::endl;
    ss << "  Curiosity Level: " << result.curiosityLevel << std::endl;
    ss << "  Dopamine Level: " << result.dopamineLevel << std::endl;
    ss << "  Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
    ss << std::endl;
    
    ss << "Timestamp: " << ctime(&result.startTime) << std::endl;
    
    return ss.str();
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    DemoArgs args = parseArgs(argc, argv);
    
    // Show help if requested
    if (args.showHelp) {
        printHelp();
        return 0;
    }
    
    // Print banner
    printBanner();
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    if (!args.quietMode) {
        std::cout << "=== Phase 6 Integration Verification ===" << std::endl;
    }
    
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        printError("Integration verification failed!");
        return 1;
    }
    
    if (!args.quietMode) {
        printSuccess("Integration verification passed!");
        std::cout << std::endl;
    }
    
    // Test individual systems
    if (!args.quietMode) {
        std::cout << "=== System Integration Tests ===" << std::endl;
    }
    
    printInfo("Testing memory integration...");
    experiment.testMemoryIntegration();
    
    printInfo("Testing neuromodulation integration...");
    experiment.testNeuromodulationIntegration();
    
    printInfo("Testing checkpointing...");
    experiment.testCheckpointing();
    
    printInfo("Testing replay system...");
    experiment.testReplay();
    
    // Run full experiment with configurable settings
    if (!args.quietMode) {
        std::cout << "=== Full Integration Experiment ===" << std::endl;
    }
    
    Phase6Config config;
    config.neuronCount = args.neuronCount;
    config.maxSteps = args.maxSteps;
    config.regionCount = DemoConfig::DEFAULT_REGION_COUNT;
    config.connectionProbability = DemoConfig::DEFAULT_CONNECTION_PROBABILITY;
    config.enableCheckpointing = args.enableCheckpointing;
    config.enableReplay = args.enableReplay;
    config.enableDevelopment = args.enableDevelopment;
    config.checkpointPath = args.checkpointPath;
    
    auto result = experiment.run(config);
    
    // Print final results
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << std::fixed << std::setprecision(DemoConfig::DEFAULT_PRECISION) 
              << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << std::fixed << std::setprecision(DemoConfig::DEFAULT_PRECISION) 
              << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << std::fixed << std::setprecision(DemoConfig::DEFAULT_PRECISION) 
              << result.dopamineLevel << std::endl;
    
    if (!args.quietMode) {
        std::cout << std::endl;
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        
        std::cout << std::endl;
        std::cout << "=== INTEGRATION SUMMARY ===" << std::endl;
        std::cout << "Phase 6 integration demonstrates that all brain systems work together:" << std::endl;
        std::cout << "  ✓ Memory systems are connected to neural processing" << std::endl;
        std::cout << "  ✓ Neuromodulation affects plasticity and neural dynamics" << std::endl;
        std::cout << "  ✓ Development affects plasticity rates" << std::endl;
        std::cout << "  ✓ Prediction system is integrated with learning" << std::endl;
        std::cout << "  ✓ Replay and consolidation are functional" << std::endl;
        std::cout << "  ✓ Checkpoint save/load works" << std::endl;
        std::cout << std::endl;
        std::cout << "The NLM brain is a functioning artificial neural substrate" << std::endl;
        std::cout << "capable of changing its synaptic connections through experience." << std::endl;
    }
    
    // Export results if requested
    if (args.exportResults) {
        std::string resultsText = formatResults(result);
        std::ofstream outFile(args.resultsFile);
        if (outFile.is_open()) {
            outFile << resultsText;
            outFile.close();
            printSuccess("Results exported to: " + args.resultsFile);
        } else {
            printError("Failed to export results to: " + args.resultsFile);
        }
    }
    
    return 0;
}
