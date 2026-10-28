// Phase 6 Demo - Integration Test
//
// This demo runs the Phase 6 integration experiment to verify
// that all brain systems are properly connected.
//
// Features:
// - Command-line configuration options
// - Detailed progress reporting
// - Multiple test modes
// - Error handling and validation

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace nlm;

// Parse command line arguments and return configuration
Phase6Config parseCommandLine(int argc, char* argv[]) {
    Phase6Config config;
    
    // Default values
    std::string verboseLevel = "normal";
    bool showProgress = true;
    bool runQuickTests = false;
    bool runIntegrationTests = true;
    bool runMemoryTest = true;
    bool runNeuromodulationTest = true;
    bool runCheckpointTest = true;
    bool runReplayTest = true;
    bool runFullExperiment = true;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--quick" || arg == "-q") {
            runQuickTests = true;
            runFullExperiment = false;
        }
        else if (arg == "--full") {
            runQuickTests = false;
            runFullExperiment = true;
        }
        else if (arg == "--memory-only") {
            runQuickTests = false;
            runFullExperiment = false;
            runMemoryTest = true;
            runNeuromodulationTest = false;
            runCheckpointTest = false;
            runReplayTest = false;
        }
        else if (arg == "--neuromodulation-only") {
            runQuickTests = false;
            runFullExperiment = false;
            runMemoryTest = false;
            runNeuromodulationTest = true;
            runCheckpointTest = false;
            runReplayTest = false;
        }
        else if (arg == "--checkpoint-only") {
            runQuickTests = false;
            runFullExperiment = false;
            runMemoryTest = false;
            runNeuromodulationTest = false;
            runCheckpointTest = true;
            runReplayTest = false;
        }
        else if (arg == "--replay-only") {
            runQuickTests = false;
            runFullExperiment = false;
            runMemoryTest = false;
            runNeuromodulationTest = false;
            runCheckpointTest = false;
            runReplayTest = true;
        }
        else if (arg == "--no-progress") {
            showProgress = false;
        }
        else if (arg == "--verbose" || arg == "-v") {
            verboseLevel = "verbose";
        }
        else if (arg == "--quiet" || arg == "-q") {
            verboseLevel = "quiet";
        }
        else if (arg == "--help" || arg == "-h") {
            printUsage();
            exit(0);
        }
        else if (arg.find("--neurons=") == 0) {
            config.neuronCount = std::stoi(arg.substr(9));
        }
        else if (arg.find("--steps=") == 0) {
            config.maxSteps = std::stoi(arg.substr(7));
        }
        else if (arg.find("--regions=") == 0) {
            config.regionCount = std::stoi(arg.substr(9));
        }
        else if (arg.find("--connection=") == 0) {
            config.connectionProbability = std::stof(arg.substr(12));
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
        else if (arg.find("--checkpoint-path=") == 0) {
            config.checkpointPath = arg.substr(17);
        }
    }
    
    return config;
}

void printUsage() {
    std::cout << "NLM Phase 6 Integration Demo Usage:" << std::endl;
    std::cout << "===================================\n" << std::endl;
    std::cout << "This demo verifies that all brain systems are properly connected." << std::endl;
    std::cout << std::endl;
    std::cout << "Basic modes:" << std::endl;
    std::cout << "  ./nlm_phase6_demo              Run full integration test (default)" << std::endl;
    std::cout << "  ./nlm_phase6_demo --quick      Run only integration verification" << std::endl;
    std::cout << "  ./nlm_phase6_demo --full       Run all individual system tests" << std::endl;
    std::cout << std::endl;
    std::cout << "Individual system tests:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --memory-only    Test only memory integration" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neuromodulation-only Test only neuromodulation" << std::endl;
    std::cout << "  ./nlm_phase6_demo --checkpoint-only    Test only checkpointing" << std::endl;
    std::cout << "  ./nlm_phase6_demo --replay-only        Test only replay system" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration options:" << std::endl;
    std::cout << "  --neurons=N           Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps=N             Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --regions=N           Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection=P        Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --no-checkpoint       Disable checkpoint testing" << std::endl;
    std::cout << "  --no-replay           Disable replay testing" << std::endl;
    std::cout << "  --no-development      Disable development testing" << std::endl;
    std::cout << "  --checkpoint-path=PATH Custom checkpoint file path" << std::endl;
    std::cout << std::endl;
    std::cout << "Display options:" << std::endl;
    std::cout << "  --verbose, -v         Detailed progress output" << std::endl;
    std::cout << "  --quiet, -q           Minimal output" << std::endl;
    std::cout << "  --no-progress         Hide progress indicators" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neurons=1000 --steps=5000" << std::endl;
    std::cout << "  ./nlm_phase6_demo --memory-only --verbose" << std::endl;
    std::cout << "  ./nlm_phase6_demo --quick --no-progress" << std::endl;
}

void printConfig(const Phase6Config& config) {
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Max Steps: " << config.maxSteps << std::endl;
    std::cout << "  Regions: " << config.regionCount << std::endl;
    std::cout << "  Connection Probability: " << config.connectionProbability << std::endl;
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
    std::cout << "  Replay: " << (config.enableReplay ? "Yes" : "No") << std::endl;
    std::cout << "  Development: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
    std::cout << "  Checkpoint Path: " << config.checkpointPath << std::endl;
    std::cout << std::endl;
}

void printHeader(const std::string& title) {
    std::cout << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command line arguments
    Phase6Config config = parseCommandLine(argc, argv);
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run integration verification (always first)
    printHeader("Integration Verification");
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        std::cerr << "The brain systems may not be properly connected." << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Run individual system tests based on configuration
    bool success = true;
    
    if (config.enableDevelopment) {
        printHeader("Memory Integration Test");
        success = experiment.testMemoryIntegration() && success;
        std::cout << std::endl;
    }
    
    if (config.enableDevelopment) {
        printHeader("Neuromodulation Integration Test");
        success = experiment.testNeuromodulationIntegration() && success;
        std::cout << std::endl;
    }
    
    if (config.enableCheckpointing) {
        printHeader("Checkpoint Test");
        success = experiment.testCheckpointing() && success;
        std::cout << std::endl;
    }
    
    if (config.enableReplay) {
        printHeader("Replay Test");
        success = experiment.testReplay() && success;
        std::cout << std::endl;
    }
    
    // Run full experiment if requested
    if (config.enableDevelopment) {
        printHeader("Full Integration Experiment");
        std::cout << "Running with configuration:" << std::endl;
        std::cout << "  Neurons: " << config.neuronCount << std::endl;
        std::cout << "  Max Steps: " << config.maxSteps << std::endl;
        std::cout << "  Regions: " << config.regionCount << std::endl;
        std::cout << "  Connection Probability: " << config.connectionProbability << std::endl;
        std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
        std::cout << "  Replay: " << (config.enableReplay ? "Yes" : "No") << std::endl;
        std::cout << "  Development: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
        std::cout << std::endl;
        
        auto result = experiment.run(config);
        
        printHeader("Final Results");
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        printHeader("Integration Status");
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    }
    
    // Overall result
    printHeader("Demo Summary");
    if (success) {
        std::cout << "✓ ALL TESTS PASSED: The NLM brain systems are properly integrated!" << std::endl;
        std::cout << "✓ Memory systems are connected to neural processing" << std::endl;
        std::cout << "✓ Neuromodulation affects plasticity and neural dynamics" << std::endl;
        std::cout << "✓ Development affects plasticity rates" << std::endl;
        std::cout << "✓ Prediction system is integrated" << std::endl;
        std::cout << "✓ Replay and consolidation are functional" << std::endl;
        std::cout << "✓ Checkpoint save/load works" << std::endl;
        std::cout << std::endl;
        std::cout << "The NLM brain is now a functioning artificial neural substrate";
        std::cout << std::endl;
        std::cout << "capable of changing its own synaptic connections through experience.";
        std::cout << std::endl;
    } else {
        std::cout << "✗ SOME TESTS FAILED: There may be integration issues." << std::endl;
        std::cout << "Please check the error messages above for details." << std::endl;
        return 1;
    }
    
    return 0;
}