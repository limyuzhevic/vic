// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase implements the complete integrated artificial brain with:
// - All memory systems (working, episodic, associative) connected
// - Neuromodulation integration (dopamine, curiosity, novelty)
// - Prediction system integration
// - Complete cognitive mechanisms (planning, concept formation, attention)
// - Development integration (structural plasticity modulated by age)
// - Checkpoint save/load functionality
// - Memory replay and consolidation systems
// - Complete brain loop integration

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase6Demo.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Complete integrated artificial brain with all systems      ║
    ║     working together as a unified cognitive architecture.       ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Command-line options
struct CLIOptions {
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    bool verbose = false;
    bool quiet = false;
    uint64_t maxSteps = 10000;
    uint64_t neuronCount = 1000;
    uint64_t regionCount = 1;
    float connectionProbability = 0.1f;
    bool runPhase6Demo = false;
    bool runIntegrationTest = false;
    bool runMinimal = false;
    bool enableCheckpointing = true;
    bool enableDevelopment = true;
    bool enableReplay = true;
    std::string checkpointPath = "checkpoint.bin";
};

CLIOptions parseCommandLine(int argc, char** argv) {
    CLIOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage();
            exit(0);
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            exit(0);
        } else if (arg == "--demo" || arg == "-d") {
            options.runPhase6Demo = true;
        } else if (arg == "--integration" || arg == "-i") {
            options.runIntegrationTest = true;
        } else if (arg == "--minimal" || arg == "-m") {
            options.runMinimal = true;
        } else if (arg == "--verbose" || arg == "-V") {
            options.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                options.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg.substr(0, 9) == "--output") {
            if (arg.find('=') != std::string::npos) {
                options.outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.outputFile = argv[++i];
            }
        } else if (arg.substr(0, 13) == "--max-steps") {
            if (arg.find('=') != std::string::npos) {
                options.maxSteps = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.maxSteps = std::stoull(argv[++i]);
            }
        } else if (arg.substr(0, 14) == "--neuron-count") {
            if (arg.find('=') != std::string::npos) {
                options.neuronCount = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.neuronCount = std::stoull(argv[++i]);
            }
        } else if (arg.substr(0, 13) == "--region-count") {
            if (arg.find('=') != std::string::npos) {
                options.regionCount = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.regionCount = std::stoull(argv[++i]);
            }
        } else if (arg.substr(0, 22) == "--connection-prob") {
            if (arg.find('=') != std::string::npos) {
                options.connectionProbability = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.connectionProbability = std::stof(argv[++i]);
            }
        } else if (arg.substr(0, 22) == "--checkpoint") {
            if (arg.find('=') != std::string::npos) {
                options.checkpointPath = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.checkpointPath = argv[++i];
            }
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage();
            exit(1);
        }
    }
    
    return options;
}

void printUsage() {
    std::cout << R"(
=== NLM — 熙然 Usage ===

COMMANDS:
  nlm --demo                 Run Phase 6 integration demo
  nlm --integration          Run integration verification test
  nlm --minimal              Run minimal simulation

OPTIONS:
  -h, --help                 Show this help message
  -v, --version              Show version information
  -V, --verbose              Enable verbose output
  -q, --quiet                Suppress non-essential output
  --config FILE              Load configuration from file
  --output FILE              Write results to file
  --max-steps N              Set maximum simulation steps
  --neuron-count N           Set number of neurons (default: 1000)
  --region-count N           Set number of brain regions (default: 1)
  --connection-prob P        Set synaptic connection probability (default: 0.1)
  --checkpoint FILE          Set checkpoint file path

EXAMPLES:
  nlm --demo --max-steps 2000 --neuron-count 500
  nlm --integration --config configs/experiment.cfg
  nlm --minimal --quiet --checkpoint mybrain.bin

For more information, visit: https://github.com/nlm-project/nlm
)" << std::endl;
}

void printVersion() {
    std::cout << "NLM — 熙然 v0.1.0 (Phase 6)" << std::endl;
    std::cout << "Neural Learning Machine — Final Integration" << std::endl;
}

void runMinimalSimulation(const CLIOptions& options) {
    // Minimal simulation for quick testing
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount));
    config->set("region_count", static_cast<int64_t>(options.regionCount));
    config->set("connection_probability", options.connectionProbability);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return;
    }
    
    NLM_LOG_INFO("Running minimal simulation with " + std::to_string(options.maxSteps) + " steps");
    
    for (uint64_t step = 0; step < options.maxSteps; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 1000 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    NLM_LOG_INFO("Minimal simulation complete");
}

void displayResults(const Phase6IntegrationResult& result) {
    std::cout << "\n=== Phase 6 Integration Results ===\n" << std::endl;
    
    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  Total Reward: " << std::fixed << std::setprecision(4) << result.totalReward << std::endl;
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(4) << result.avgFiringRate << std::endl;
    std::cout << "  Average Synaptic Weight: " << std::fixed << std::setprecision(4) << result.avgSynapticWeight << std::endl;
    std::cout << "  Memory Episodes Stored: " << static_cast<int>(result.memoryEpisodesStored) << std::endl;
    
    std::cout << "\nNeuromodulation Levels:" << std::endl;
    std::cout << "  Dopamine Level: " << std::fixed << std::setprecision(4) << result.dopamineLevel << std::endl;
    std::cout << "  Curiosity Level: " << std::fixed << std::setprecision(4) << result.curiosityLevel << std::endl;
    std::cout << "  Novelty Level: " << std::fixed << std::setprecision(4) << result.noveltyLevel << std::endl;
    
    std::cout << "\nSystem Integration Status:" << std::endl;
    std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "  Prediction System: " << (result.predictionIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "  Development System: " << (result.developmentIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING ✓" : "NOT WORKING ✗") << std::endl;
    std::cout << "  Replay System: " << (result.replayWorks ? "WORKING ✓" : "NOT WORKING ✗") << std::endl;
    
    std::cout << "\nTiming:" << std::endl;
    std::cout << "  Wall clock time: " << std::fixed << std::setprecision(2) << result.totalWallClockTime << " seconds" << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    CLIOptions options = parseCommandLine(argc, argv);
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    if (options.quiet) {
        auto quietLogger = std::make_shared<ConsoleLogger>(LogLevel::Error);
        logger->addLogger(quietLogger);
    } else if (options.verbose) {
        auto verboseLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
        logger->addLogger(verboseLogger);
    } else {
        auto infoLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(infoLogger);
    }
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration ===");
    NLM_LOG_INFO("An experimental artificial developmental brain with integrated systems.");
    
    if (options.runPhase6Demo) {
        NLM_LOG_INFO("Running Phase 6 Demo (Integration Test)");
        Phase6Demo demo;
        demo.run(options.maxSteps, options.neuronCount);
    } else if (options.runIntegrationTest) {
        NLM_LOG_INFO("Running Integration Verification Test");
        Phase6IntegratedExperiment experiment;
        if (!experiment.verifyIntegration()) {
            NLM_LOG_ERROR("Integration verification failed!");
            return 1;
        }
        NLM_LOG_INFO("Integration verification passed!");
        
        // Run additional integration tests
        NLM_LOG_INFO("Testing memory integration...");
        experiment.testMemoryIntegration();
        
        NLM_LOG_INFO("Testing neuromodulation integration...");
        experiment.testNeuromodulationIntegration();
        
        NLM_LOG_INFO("Testing checkpoint system...");
        experiment.testCheckpointing();
        
        NLM_LOG_INFO("Testing replay system...");
        experiment.testReplay();
    } else if (options.runMinimal) {
        NLM_LOG_INFO("Running minimal simulation...");
        runMinimalSimulation(options);
    } else {
        // Default comprehensive test
        NLM_LOG_INFO("Running comprehensive Phase 6 integration test...");
        Phase6IntegratedExperiment experiment;
        
        // Configure experiment based on command line options
        Phase6Config config;
        config.neuronCount = options.neuronCount;
        config.maxSteps = options.maxSteps;
        config.enableCheckpointing = options.enableCheckpointing;
        config.enableDevelopment = options.enableDevelopment;
        config.enableReplay = options.enableReplay;
        config.checkpointPath = options.checkpointPath;
        
        auto result = experiment.run(config);
        
        // Display results
        displayResults(result);
        
        if (!result.memoryWorkingMemoryIntegrated || 
            !result.memoryEpisodicMemoryIntegrated ||
            !result.neuromodulationIntegrated ||
            !result.predictionIntegrated ||
            !result.developmentIntegrated) {
            NLM_LOG_ERROR("Integration test failed - some systems not properly connected");
            return 1;
        }
    }
    
    NLM_LOG_INFO("=== Phase 6 Complete ===");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    
    return 0;
}
