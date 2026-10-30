// Phase 6 Demo Implementation

#include "Phase6Demo.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <ctime>

using namespace nlm;

Phase6Demo::Phase6Demo() {
    // Initialize log level mapping
    logLevelMap["debug"] = Logger::Level::Debug;
    logLevelMap["info"] = Logger::Level::Info;
    logLevelMap["warning"] = Logger::Level::Warning;
    logLevelMap["error"] = Logger::Level::Error;
}

Phase6Demo::~Phase6Demo() = default;

DemoOptions Phase6Demo::parseArguments(int argc, char* argv[]) {
    DemoOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        }
        else if (arg == "--version") {
            std::cout << "NLM Phase 6 Demo v0.1.0" << std::endl;
            exit(0);
        }
        else if (arg.substr(0, 9) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                options.neuronCount = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.neuronCount = std::stoull(argv[++i]);
            }
        }
        else if (arg.substr(0, 8) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                options.maxSteps = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.maxSteps = std::stoull(argv[++i]);
            }
        }
        else if (arg.substr(0, 10) == "--checkpoint") {
            if (arg.find('=') != std::string::npos) {
                options.checkpointPath = arg.substr(arg.find('=') + 1);
                options.enableCheckpointing = true;
            } else if (i + 1 < argc) {
                options.checkpointPath = argv[++i];
                options.enableCheckpointing = true;
            }
        }
        else if (arg == "--no-checkpoint") {
            options.enableCheckpointing = false;
        }
        else if (arg == "--no-replay") {
            options.enableReplay = false;
        }
        else if (arg == "--no-development") {
            options.enableDevelopment = false;
        }
        else if (arg == "--verbose") {
            options.verbose = true;
        }
        else if (arg.substr(0, 8) == "--config") {
            if (arg.find('=') != std::string::npos) {
                options.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        }
        else if (arg.substr(0, 8) == "--seed") {
            if (arg.find('=') != std::string::npos) {
                options.customSeed = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.customSeed = std::stoull(argv[++i]);
            }
        }
        else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printHelp();
            exit(1);
        }
    }
    
    return options;
}

void Phase6Demo::printHelp() const {
    std::cout << "=== NLM Phase 6 Demo Help ===" << std::endl;
    std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --neurons <count>        Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps <count>          Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --checkpoint <path>      Checkpoint file path (default: ./checkpoint.bin)" << std::endl;
    std::cout << "  --no-checkpoint           Disable checkpointing" << std::endl;
    std::cout << "  --no-replay               Disable replay functionality" << std::endl;
    std::cout << "  --no-development          Disable developmental processes" << std::endl;
    std::cout << "  --verbose                 Enable verbose logging" << std::endl;
    std::cout << "  --config <file>          Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --seed <value>           Random seed for reproducibility (default: 42)" << std::endl;
    std::cout << "  --help, -h               Show this help message" << std::endl;
    std::cout << "  --version                Show version information" << std::endl << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_phase6_demo --neurons 1000 --steps 5000" << std::endl;
    std::cout << "  nlm_phase6_demo --config my_config.cfg --verbose" << std::endl;
    std::cout << "  nlm_phase6_demo --no-checkpoint --no-development" << std::endl;
}

int Phase6Demo::run(const DemoOptions& options) {
    // Validate options
    if (!validateOptions(options)) {
        return 1;
    }
    
    // Initialize logging
    initializeLogging();
    
    // Print configuration
    printConfiguration(options);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
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
    
    // Setup experiment configuration
    Phase6Config config = setupExperimentConfig(options);
    
    // Run integration test with timing
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6IntegrationResult result = runIntegrationTest(experiment, options);
    
    // Display results
    printResults(result);
    
    return 0;
}

void Phase6Demo::printConfiguration(const DemoOptions& options) const {
    std::cout << "=== NLM Phase 6 Demo Configuration ===" << std::endl;
    std::cout << "Neurons: " << options.neuronCount << std::endl;
    std::cout << "Max Steps: " << options.maxSteps << std::endl;
    std::cout << "Checkpoint: " << options.checkpointPath << std::endl;
    std::cout << "Enable Checkpointing: " << (options.enableCheckpointing ? "Yes" : "No") << std::endl;
    std::cout << "Enable Replay: " << (options.enableReplay ? "Yes" : "No") << std::endl;
    std::cout << "Enable Development: " << (options.enableDevelopment ? "Yes" : "No") << std::endl;
    std::cout << "Verbose: " << (options.verbose ? "Yes" : "No") << std::endl;
    std::cout << "Config File: " << options.configFile << std::endl;
    std::cout << "Custom Seed: " << options.customSeed << std::endl;
    std::cout << "================================" << std::endl;
}

Phase6IntegrationResult Phase6Demo::runIntegrationTest(Phase6IntegratedExperiment& experiment, const DemoOptions& options) {
    Phase6Config config;
    config.neuronCount = options.neuronCount;
    config.maxSteps = options.maxSteps;
    config.enableCheckpointing = options.enableCheckpointing;
    config.enableReplay = options.enableReplay;
    config.enableDevelopment = options.enableDevelopment;
    config.checkpointPath = options.checkpointPath;
    
    auto startWall = std::chrono::high_resolution_clock::now();
    
    Phase6IntegrationResult result = experiment.run(config);
    
    auto endWall = std::chrono::high_resolution_clock::now();
    result.totalWallClockTime = std::chrono::duration<double>(endWall - startWall).count();
    
    return result;
}

void Phase6Demo::printResults(const Phase6IntegrationResult& result) const {
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
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
}

bool Phase6Demo::validateOptions(const DemoOptions& options) const {
    if (options.neuronCount == 0) {
        std::cerr << "Error: neuron count must be greater than 0" << std::endl;
        return false;
    }
    
    if (options.maxSteps == 0) {
        std::cerr << "Error: max steps must be greater than 0" << std::endl;
        return false;
    }
    
    return true;
}

void Phase6Demo::initializeLogging() {
    Logger::getInstance().setLevel(Logger::Level::Info);
}

Phase6Config Phase6Demo::setupExperimentConfig(const DemoOptions& options) const {
    Phase6Config config;
    config.neuronCount = options.neuronCount;
    config.maxSteps = options.maxSteps;
    config.enableCheckpointing = options.enableCheckpointing;
    config.enableReplay = options.enableReplay;
    config.enableDevelopment = options.enableDevelopment;
    config.checkpointPath = options.checkpointPath;
    return config;
}

void Phase6Demo::displayProgress(size_t step, size_t maxSteps, time_t startTime) const {
    double elapsed = calculateElapsedTime(startTime);
    double progress = static_cast<double>(step) / maxSteps * 100.0;
    
    std::cout << "Progress: " << std::fixed << std::setprecision(1) << progress << "% "
              << "(Step " << step << "/" << maxSteps << ") "
              << "Elapsed: " << std::fixed << std::setprecision(1) << elapsed << "s"
              << std::endl;
}

double Phase6Demo::calculateElapsedTime(time_t startTime) const {
    time_t current = std::time(nullptr);
    return std::difftime(current, startTime);
}
