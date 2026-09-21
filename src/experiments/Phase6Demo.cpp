/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Usage:
 *   ./nlm_phase6_demo [quick|full] [steps] [neurons]
 *   quick - Run only integration verification (default)
 *   full  - Run complete integration test with metrics
 *   steps - Maximum simulation steps (default: 2000 for quick, 10000 for full)
 *   neurons - Number of neurons in brain (default: 500 for quick, 1000 for full)
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <map>

using namespace nlm;

/**
 * Print a formatted banner for the demo
 */
void printBanner(const std::string& title) {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(70, '=') << "\n";
}

/**
 * Print configuration options
 */
void printConfig(const Phase6Config& config) {
    std::cout << "  Configuration:\n";
    std::cout << "    Max Steps: " << config.maxSteps << "\n";
    std::cout << "    Neurons: " << config.neuronCount << "\n";
    std::cout << "    Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << "\n";
    std::cout << "    Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << "\n";
    std::cout << "    Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << "\n";
    if (!config.checkpointPath.empty()) {
        std::cout << "    Checkpoint Path: " << config.checkpointPath << "\n";
    }
    std::cout << std::endl;
}

/**
 * Parse command line arguments
 */
Phase6Config parseArguments(int argc, char* argv[]) {
    Phase6Config config;
    std::string mode = "quick";  // Default mode
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "quick" || arg == "full") {
            mode = arg;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: ./nlm_phase6_demo [mode] [options]\n\n";
            std::cout << "Modes:\n";
            std::cout << "  quick  - Integration verification only\n";
            std::cout << "  full   - Complete integration test with metrics\n\n";
            std::cout << "Options:\n";
            std::cout << "  --steps=N      Maximum simulation steps\n";
            std::cout << "  --neurons=N    Number of neurons\n";
            std::cout << "  --no-checkpoint Disable checkpointing\n";
            std::cout << "  --no-replay     Disable replay\n";
            std::cout << "  --no-development Disable development\n";
            std::cout << "  --checkpoint PATH Checkpoint file path\n";
            std::cout << "  -h, --help     Show this help\n";
            exit(0);
        } else if (arg.find("--steps=") == 0) {
            size_t pos = arg.find("=") + 1;
            config.maxSteps = std::stoull(arg.substr(pos));
        } else if (arg.find("--neurons=") == 0) {
            size_t pos = arg.find("=") + 1;
            config.neuronCount = std::stoul(arg.substr(pos));
        } else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg.find("--checkpoint=") == 0) {
            size_t pos = arg.find("=") + 1;
            config.checkpointPath = arg.substr(pos);
        }
    }
    
    // Adjust config based on mode
    if (mode == "quick") {
        config.maxSteps = config.maxSteps == 10000 ? 2000 : config.maxSteps;
        config.neuronCount = config.neuronCount == 1000 ? 500 : config.neuronCount;
    } else {
        config.maxSteps = config.maxSteps == 10000 ? 5000 : config.maxSteps;
        config.neuronCount = config.neuronCount == 1000 ? 1000 : config.neuronCount;
    }
    
    return config;
}

/**
 * Print detailed results
 */
void printResults(const Phase6IntegrationResult& result) {
    printBanner("INTEGRATION RESULTS");
    
    std::cout << std::fixed << std::setprecision(2);
    
    // System integration status
    std::cout << "\nSystem Integration Status:\n";
    std::map<std::string, bool> systems = {
        {"Working Memory", result.memoryWorkingMemoryIntegrated},
        {"Episodic Memory", result.memoryEpisodicMemoryIntegrated},
        {"Neuromodulation", result.neuromodulationIntegrated},
        {"Prediction", result.predictionIntegrated},
        {"Development", result.developmentIntegrated},
        {"Checkpointing", result.checkpointingWorks},
        {"Replay", result.replayWorks}
    };
    
    for (const auto& [name, status] : systems) {
        std::cout << "  " << std::left << std::setw(20) << name << ": " 
                  << (status ? "✓ CONNECTED" : "✗ DISCONNECTED") << "\n";
    }
    
    // Performance metrics
    std::cout << "\nPerformance Metrics:\n";
    std::cout << "  Total Reward:    " << std::setw(8) << result.totalReward << "\n";
    std::cout << "  Avg Firing Rate: " << std::setw(8) << result.avgFiringRate << " Hz\n";
    std::cout << "  Avg Synaptic Wt: " << std::setw(8) << result.avgSynapticWeight << "\n";
    std::cout << "  Episodes Stored: " << std::setw(8) << result.memoryEpisodesStored << "\n";
    std::cout << "  Novelty Level:   " << std::setw(8) << result.noveltyLevel << "\n";
    std::cout << "  Curiosity Level: " << std::setw(8) << result.curiosityLevel << "\n";
    std::cout << "  Dopamine Level:  " << std::setw(8) << result.dopamineLevel << "\n";
    
    // Timing
    if (result.totalWallClockTime > 0) {
        std::cout << "  Wall Clock:     " << std::setw(8) << result.totalWallClockTime << " s\n";
    }
}

/**
 * Run quick verification mode
 */
bool runQuickVerification(Phase6IntegratedExperiment& experiment) {
    printBanner("QUICK INTEGRATION VERIFICATION");
    
    std::cout << "\nRunning integration verification...\n";
    bool integrationOK = experiment.verifyIntegration();
    
    if (integrationOK) {
        std::cout << "\n✓ Integration verification PASSED!\n";
        std::cout << "All brain systems are properly connected.\n";
    } else {
        std::cout << "\n✗ Integration verification FAILED!\n";
        std::cout << "Some systems are not properly integrated.\n";
    }
    
    return integrationOK;
}

/**
 * Run full integration test
 */
Phase6IntegrationResult runFullTest(Phase6IntegratedExperiment& experiment, Phase6Config config) {
    printBanner("FULL INTEGRATION TEST");
    printConfig(config);
    
    std::cout << "\nRunning full integration test...\n";
    std::cout << "This will test all brain systems together in a complete simulation.\n\n";
    
    auto startTime = std::chrono::system_clock::now();
    
    // Run the integration experiment
    Phase6IntegrationResult result = experiment.run(config);
    
    auto endTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    // Override wall clock time with actual measured time
    result.totalWallClockTime = static_cast<double>(duration.count());
    
    return result;
}

/**
 * Run individual system tests
 */
void runSystemTests(Phase6IntegratedExperiment& experiment) {
    printBanner("INDIVIDUAL SYSTEM TESTS");
    
    std::cout << "\nRunning individual system integration tests...\n";
    
    // Memory integration test
    std::cout << "--- Memory Integration Test ---\n";
    bool memoryOK = experiment.testMemoryIntegration();
    std::cout << (memoryOK ? "  ✓ Memory integration: OK" : "  ✗ Memory integration: FAILED") << "\n\n";
    
    // Neuromodulation integration test
    std::cout << "--- Neuromodulation Integration Test ---\n";
    bool neuromodOK = experiment.testNeuromodulationIntegration();
    std::cout << (neuromodOK ? "  ✓ Neuromodulation integration: OK" : "  ✗ Neuromodulation integration: FAILED") << "\n\n";
    
    // Checkpoint test
    std::cout << "--- Checkpoint Test ---\n";
    bool checkpointOK = experiment.testCheckpointing();
    std::cout << (checkpointOK ? "  ✓ Checkpoint system: OK" : "  ✗ Checkpoint system: FAILED") << "\n\n";
    
    // Replay test
    std::cout << "--- Replay Test ---\n";
    bool replayOK = experiment.testReplay();
    std::cout << (replayOK ? "  ✓ Replay system: OK" : "  ✗ Replay system: FAILED") << "\n\n";
    
    std::cout << "System tests completed.\n";
}

int main(int argc, char* argv[]) {
    // Print banner
    printBanner("NLM PHASE 6 INTEGRATION DEMO");
    std::cout << "\nTesting the integrated artificial brain...\n";
    
    // Parse command line arguments
    Phase6Config config = parseArguments(argc, argv);
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    bool success = true;
    
    // Run based on mode
    std::string mode = "quick";
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "quick" || std::string(argv[i]) == "full") {
            mode = argv[i];
            break;
        }
    }
    
    if (mode == "quick") {
        // Quick verification mode
        success = runQuickVerification(experiment);
        
        if (success) {
            runSystemTests(experiment);
        }
        
    } else {
        // Full integration test mode
        Phase6IntegrationResult result = runFullTest(experiment, config);
        
        // Print detailed results
        printResults(result);
        
        success = result.memoryWorkingMemoryIntegrated && 
                  result.memoryEpisodicMemoryIntegrated && 
                  result.neuromodulationIntegrated && 
                  result.predictionIntegrated && 
                  result.developmentIntegrated && 
                  result.checkpointingWorks && 
                  result.replayWorks;
    }
    
    // Final status
    printBanner("DEMO COMPLETE");
    
    if (success) {
        std::cout << "\n✓ Phase 6 Integration Demo PASSED!\n";
        std::cout << "\nThe integrated brain systems are working correctly:\n";
        std::cout << "  - Memory systems are properly connected\n";
        std::cout << "  - Neuromodulation affects neural dynamics\n";
        std::cout << "  - Development system is functional\n";
        std::cout << "  - Prediction system integrates with action selection\n";
        std::cout << "  - Replay and consolidation work correctly\n";
        std::cout << "  - Checkpoint save/load is operational\n";
        return 0;
    } else {
        std::cout << "\n✗ Phase 6 Integration Demo FAILED!\n";
        std::cout << "\nSome brain systems are not properly integrated.\n";
        std::cout << "Check the output above for details.\n";
        return 1;
    }
}
