// NLM Phase 6 Unified Main Entry Point
// This is the unified Phase 6 entry point that demonstrates all integrated systems
// Replaces the Phase 2 main.cpp and serves as the primary interface for users

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

// Command line argument parsing
struct CommandLineArgs {
    std::string configFile = "configs/default.cfg";
    size_t neuronCount = 0;
    size_t maxSteps = 0;
    bool enableCheckpointing = false;
    bool enableReplay = false;
    bool enableDevelopment = false;
    std::string outputPath = "";
    bool runQuickTests = false;
    bool runFullExperiment = true;
    std::string experimentMode = "demo";
};

CommandLineArgs parseCommandLineArgs(int argc, char* argv[]) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--quick" || arg == "-q") {
            args.runQuickTests = true;
            args.runFullExperiment = false;
        } else if (arg == "--full" || arg == "-f") {
            args.runQuickTests = false;
            args.runFullExperiment = true;
        } else if (arg == "--experiment" || arg == "-e") {
            if (i + 1 < argc) {
                args.experimentMode = argv[++i];
            }
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg == "--neurons" || arg == "-n") {
            if (i + 1 < argc) {
                args.neuronCount = std::stoul(argv[++i]);
            }
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                args.maxSteps = std::stoul(argv[++i]);
            }
        } else if (arg == "--checkpoint" || arg == "-k") {
            args.enableCheckpointing = true;
            if (i + 1 < argc) {
                args.outputPath = argv[++i];
            }
        } else if (arg == "--replay" || arg == "-r") {
            args.enableReplay = true;
        } else if (arg == "--development" || arg == "-d") {
            args.enableDevelopment = true;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 6 Unified Main - Usage:\n"
                      << "  --quick, -q              Run quick integration tests only\n"
                      << "  --full, -f              Run full integration experiment\n"
                      << "  --experiment <mode>      Set experiment mode\n"
                      << "  --config <file>, -c      Configuration file\n"
                      << "  --neurons <count>, -n    Number of neurons\n"
                      << "  --steps <count>, -s      Maximum simulation steps\n"
                      << "  --checkpoint <path>, -k  Enable checkpointing\n"
                      << "  --replay, -r             Enable replay feature\n"
                      << "  --development, -d        Enable development system\n"
                      << "  --help, -h               Show this help message\n";
            exit(0);
        }
    }
    
    return args;
}

void printWelcome() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                               ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase integrates all brain systems into a unified loop║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsageSummary(const CommandLineArgs& args) {
    std::cout << "\nNLM Phase 6 Unified Main - Configuration:\n";
    std::cout << "  Config file: " << args.configFile << "\n";
    std::cout << "  Neuron count: " << (args.neuronCount > 0 ? std::to_string(args.neuronCount) : "default") << "\n";
    std::cout << "  Max steps: " << (args.maxSteps > 0 ? std::to_string(args.maxSteps) : "default") << "\n";
    std::cout << "  Checkpointing: " << (args.enableCheckpointing ? "enabled" : "disabled") << "\n";
    if (args.enableCheckpointing && !args.outputPath.empty()) {
        std::cout << "  Checkpoint path: " << args.outputPath << "\n";
    }
    std::cout << "  Replay: " << (args.enableReplay ? "enabled" : "disabled") << "\n";
    std::cout << "  Development: " << (args.enableDevelopment ? "enabled" : "disabled") << "\n";
    std::cout << "  Mode: " << args.experimentMode << "\n";
}

void runQuickIntegrationTests(nlm::Phase6IntegratedExperiment& experiment) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  QUICK INTEGRATION TESTS" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    // Integration verification
    std::cout << "\n--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
    }
    
    std::cout << "✓ Integration verification passed!" << std::endl;
    
    // Test individual systems
    std::cout << "\n--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    
    std::cout << "\n--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    
    std::cout << "\n--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    
    std::cout << "\n--- Replay Test ---" << std::endl;
    experiment.testReplay();
    
    std::cout << "\n✓ All quick integration tests completed successfully!" << std::endl;
}

void runFullIntegrationExperiment(nlm::Phase6IntegratedExperiment& experiment,
                                  const CommandLineArgs& args) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  FULL INTEGRATION EXPERIMENT" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    nlm::Phase6Config config;
    
    if (args.neuronCount > 0) {
        config.neuronCount = args.neuronCount;
    }
    
    if (args.maxSteps > 0) {
        config.maxSteps = args.maxSteps;
    }
    
    config.enableCheckpointing = args.enableCheckpointing;
    config.enableReplay = args.enableReplay;
    config.enableDevelopment = args.enableDevelopment;
    
    if (!args.outputPath.empty()) {
        config.checkpointPath = args.outputPath;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::cout << "\nRunning Phase 6 integration experiment..." << std::endl;
    std::cout << "  Configuration: " << config.neuronCount << " neurons, " 
              << config.maxSteps << " steps" << std::endl;
    
    auto result = experiment.run(config);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  EXPERIMENT RESULTS" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    std::cout << "\n=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Wall clock time: " << duration << "s" << std::endl;
    
    std::cout << "\n=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    
    if (result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated && 
        result.neuromodulationIntegrated && result.predictionIntegrated && 
        result.developmentIntegrated && result.checkpointingWorks) {
        std::cout << "\n✓ SUCCESS: All systems integrated and functional!" << std::endl;
        std::cout << "The NLM brain is a functioning artificial neural substrate" << std::endl;
        std::cout << "capable of changing its own synaptic connections through experience." << std::endl;
    } else {
        std::cout << "\n⚠ WARNING: Some systems may not be fully integrated." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    printWelcome();
    
    std::cout << "Initializing NLM Phase 6: Final Integration..." << std::endl;
    
    // Parse command line arguments
    CommandLineArgs args = parseCommandLineArgs(argc, argv);
    
    // Initialize logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Create and initialize experiment
    nlm::Phase6IntegratedExperiment experiment;
    
    printUsageSummary(args);
    
    // Run experiments based on command line arguments
    if (args.runQuickTests) {
        runQuickIntegrationTests(experiment);
    }
    
    if (args.runFullExperiment) {
        runFullIntegrationExperiment(experiment, args);
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  PHASE 6 COMPLETE" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "\nThe NLM Phase 6 unified main provides a comprehensive entry point" << std::endl;
    std::cout << "demonstrating all integrated brain systems working together." << std::endl;
    std::cout << "\nFor more information, see the documentation in the docs/ directory." << std::endl;
    
    return 0;
}
