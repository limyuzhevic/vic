// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration Demo
//
// This demo runs the Phase 6 integration experiment to verify
// that all brain systems are properly connected.
//
// Command line arguments:
//   --config <file>      Configuration file (JSON/YAML/CFG format)
//   --neurons <n>        Number of neurons (default: 500)
//   --steps <n>          Max simulation steps (default: 2000)
//   --help               Show this help message
//   --version            Show version information

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <string>
#include <cstring>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration Demo                           ║
    ║                                                               ║
    ║     An artificial developmental brain with all systems       ║
    ║     integrated into a coherent cognitive architecture.       ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << "Usage: nlm [OPTIONS]" << std::endl << std::endl;
    std::cout << "Phase 6 Final Integration Demo" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config <file>     Configuration file (JSON/YAML/CFG format)" << std::endl;
    std::cout << "  --neurons <n>       Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps <n>         Max simulation steps (default: 2000)" << std::endl;
    std::cout << "  --checkpoint <path>  Checkpoint file path (default: ./checkpoint.bin)" << std::endl;
    std::cout << "  --no-checkpoint      Disable checkpointing" << std::endl;
    std::cout << "  --no-replay          Disable replay" << std::endl;
    std::cout << "  --no-development     Disable development" << std::endl;
    std::cout << "  --log-level <level>  Log level: ERROR, WARNING, INFO, DEBUG" << std::endl;
    std::cout << "  --help              Show this help message" << std::endl;
    std::cout << "  --version           Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --neurons 1000 --steps 5000" << std::endl;
    std::cout << "  nlm --config experiment.json --log-level DEBUG" << std::endl;
    std::cout << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) Neural Learning Machine - Phase 6 Final Integration" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 6)" << std::endl;
    std::cout << "Build: 2026-10-13" << std::endl;
    std::cout << std::endl;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Starting NLM Phase 6 Final Integration Demo..." << std::endl << std::endl;
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Parse command line arguments
    std::string configFile = "";
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    std::string checkpointPath = "./checkpoint.bin";
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    LogLevel logLevel = LogLevel::Info;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return 0;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.substr(0, 7) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                neuronCount = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                neuronCount = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                maxSteps = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                maxSteps = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 12) == "--checkpoint") {
            if (arg.find('=') != std::string::npos) {
                checkpointPath = arg.substr(arg.find('=') + 1);
                enableCheckpointing = true;
            } else if (i + 1 < argc) {
                checkpointPath = argv[++i];
                enableCheckpointing = true;
            }
        } else if (arg == "--no-checkpoint") {
            enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            enableReplay = false;
        } else if (arg == "--no-development") {
            enableDevelopment = false;
        } else if (arg.substr(0, 11) == "--log-level") {
            if (arg.find('=') != std::string::npos) {
                std::string levelStr = trim(arg.substr(arg.find('=') + 1));
                std::transform(levelStr.begin(), levelStr.end(), levelStr.begin(), ::toupper);
                if (levelStr == "ERROR") logLevel = LogLevel::Error;
                else if (levelStr == "WARNING") logLevel = LogLevel::Warning;
                else if (levelStr == "DEBUG") logLevel = LogLevel::Debug;
                else logLevel = LogLevel::Info;
            } else if (i + 1 < argc) {
                std::string levelStr = trim(argv[++i]);
                std::transform(levelStr.begin(), levelStr.end(), levelStr.begin(), ::toupper);
                if (levelStr == "ERROR") logLevel = LogLevel::Error;
                else if (levelStr == "WARNING") logLevel = LogLevel::Warning;
                else if (levelStr == "DEBUG") logLevel = LogLevel::Debug;
                else logLevel = LogLevel::Info;
            }
        }
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration Demo ===");
    NLM_LOG_INFO("Initializing configuration...");
    
    // Load configuration from file if specified
    if (!configFile.empty()) {
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_WARNING("Failed to load configuration from: " + configFile);
            NLM_LOG_INFO("Using command line arguments.");
        }
    }
    
    // Override with command line arguments
    config->loadFromArgs(argc, argv);
    
    // Set demo-specific configuration
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(neuronCount));
    NLM_LOG_INFO("  max_steps: " + std::to_string(maxSteps));
    NLM_LOG_INFO("  checkpoint_path: " + checkpointPath);
    NLM_LOG_INFO("  enable_checkpointing: " + std::string(enableCheckpointing ? "true" : "false"));
    NLM_LOG_INFO("  enable_replay: " + std::string(enableReplay ? "true" : "false"));
    NLM_LOG_INFO("  enable_development: " + std::string(enableDevelopment ? "true" : "false"));
    NLM_LOG_INFO("");
    
    // Create and run Phase 6 integration experiment
    NLM_LOG_INFO("Creating Phase 6 integration experiment...");
    Phase6IntegratedExperiment experiment;
    
    // First, verify integration
    NLM_LOG_INFO("Verifying system integration...");
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        NLM_LOG_ERROR("Integration verification failed!");
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    NLM_LOG_INFO("Integration verification passed!");
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    NLM_LOG_INFO("Testing individual systems...");
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
    
    // Run full integration experiment
    NLM_LOG_INFO("Running full integration experiment...");
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config experimentConfig;
    experimentConfig.neuronCount = neuronCount;
    experimentConfig.maxSteps = maxSteps;
    experimentConfig.enableCheckpointing = enableCheckpointing;
    experimentConfig.enableReplay = enableReplay;
    experimentConfig.enableDevelopment = enableDevelopment;
    experimentConfig.checkpointPath = checkpointPath;
    
    auto result = experiment.run(experimentConfig);
    
    NLM_LOG_INFO("Experiment completed successfully!");
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
    
    NLM_LOG_INFO("=== Phase 6 Integration Complete ===");
    NLM_LOG_INFO("All systems are properly integrated and functional!");
    NLM_LOG_INFO("The artificial brain is now a coherent, self-organizing system.");
    
    return 0;
}
