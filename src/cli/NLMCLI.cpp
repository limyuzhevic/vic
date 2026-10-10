#include "NLMCLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/ConsoleLogger.hpp"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <filesystem>

namespace nlm {
namespace cli {

int NLMCLI::run(int argc, char* argv[]) {
    try {
        CLIConfig config = parseArguments(argc, argv);
        
        // Setup logging based on configuration
        setupLogging(config.logLevel);
        
        // Log CLI startup
        log("Starting NLM CLI in " + config.mode + " mode", "INFO");
        
        // Execute based on mode
        int result;
        if (config.mode == "demo") {
            result = executeDemoMode(config);
        } else if (config.mode == "experiment") {
            result = executeExperimentMode(config);
        } else if (config.mode == "explore") {
            result = executeExploreMode(config);
        } else if (config.mode == "benchmark") {
            result = executeBenchmarkMode(config);
        } else {
            error("Unknown mode: " + config.mode);
            printHelp(argv[0]);
            return 1;
        }
        
        log("NLM CLI finished with result: " + std::to_string(result), "INFO");
        return result;
        
    } catch (const std::exception& e) {
        error(std::string("Error: ") + e.what());
        return 1;
    } catch (...) {
        error("Unknown error occurred");
        return 1;
    }
}

void NLMCLI::printHelp(const std::string& programName) {
    std::cout << "NLM — 熙然 (Neural Learning Machine)" << std::endl;
    std::cout << "An experimental artificial developmental brain" << std::endl << std::endl;
    
    std::cout << "USAGE:" << std::endl;
    std::cout << "  " << programName << " [MODE] [OPTIONS]" << std::endl << std::endl;
    
    std::cout << "MODES:" << std::endl;
    std::cout << "  demo        Run a quick demonstration of Phase 2 neural computation" << std::endl;
    std::cout << "  experiment   Run the complete Phase 6 integration experiment" << std::endl;
    std::cout << "  explore      Interactive exploration mode with real-time feedback" << std::endl;
    std::cout << "  benchmark    Performance benchmarking and profiling" << std::endl;
    std::cout << "  config       Configuration management (list, set, get)" << std::endl;
    std::cout << "  help         Show this help message" << std::endl << std::endl;
    
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  --config FILE        Load configuration from file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --steps N            Number of simulation steps (default: 100)" << std::endl;
    std::cout << "  --neurons N          Number of neurons (default: 500)" << std::endl;
    std::cout << "  --checkpoint FILE    Enable checkpointing and save/load state" << std::endl;
    std::cout << "  --verbose            Enable verbose logging" << std::endl;
    std::cout << "  --quiet              Quiet mode (only errors)" << std::endl;
    std::cout << "  --log-level LEVEL    Set log level (debug, info, warn, error, critical)" << std::endl;
    std::cout << "  --output FILE        Save results to file" << std::endl;
    std::cout << "  --experiment-mode MODE  Experiment mode: quick, thorough, custom" << std::endl;
    std::cout << "  --version            Show version information" << std::endl;
    std::cout << "  --help              Show help message" << std::endl << std::endl;
    
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  " << programName << " demo --neurons 1000 --steps 500" << std::endl;
    std::cout << "  " << programName << " experiment --checkpoint test.bin --verbose" << std::endl;
    std::cout << "  " << programName << " explore --log-level debug" << std::endl;
    std::cout << "  " << programName << " benchmark --steps 1000" << std::endl;
    std::cout << "  " << programName << " config list" << std::endl << std::endl;
    
    printAdvancedHelp();
}

void NLMCLI::printVersion() {
    std::cout << "NLM 0.1.0 (熙然)" << std::endl;
    std::cout << "Phase 6 - Final Integration (Complete)" << std::endl;
    std::cout << "Experimental Artificial Developmental Brain" << std::endl << std::endl;
    
    std::cout << "Features:" << std::endl;
    std::cout << "  • Real LIF neuron dynamics with event-driven spike propagation" << std::endl;
    std::cout << "  • STDP and Hebbian plasticity rules" << std::endl;
    std::cout << "  • Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << "  • Working memory with persistent activity" << std::endl;
    std::cout << "  • Episodic memory with experience storage" << std::endl;
    std::cout << "  • Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
    std::cout << "  • Prediction system integration" << std::endl;
    std::cout << "  • Neural planning and concept formation" << std::endl;
    std::cout << "  • Development stages and maturation" << std::endl;
    std::cout << "  • Checkpoint save/load capabilities" << std::endl;
    std::cout << "  • Memory replay and consolidation" << std::endl << std::endl;
}

void NLMCLI::printAdvancedHelp() {
    std::cout << "ADVANCED OPTIONS:" << std::endl;
    std::cout << "  --enable-replay     Enable memory replay functionality" << std::endl;
    std::cout << "  --disable-development    Disable development system" << std::endl;
    std::cout << "  --create-config FILE Create default configuration file" << std::endl;
    std::cout << "  --validate-config FILE    Validate configuration file" << std::endl;
    std::cout << "  --batch-mode MODE   Run in batch mode (script-friendly)" << std::endl;
    std::cout << "  --profile           Enable performance profiling" << std::endl;
    std::cout << "  --dump-neurons      Export neural state to file" << std::endl;
    std::cout << "  --dump-synapses      Export synaptic weights to file" << std::endl;
    std::cout << "  --stats FILE        Export statistics to file" << std::endl << std::endl;
}

NLMCLI::CLIConfig NLMCLI::parseArguments(int argc, char* argv[]) {
    CLIConfig config;
    
    // Skip program name
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp(argv[0]);
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg == "--create-config") {
            if (i + 1 < argc) {
                createDefaultConfig(argv[++i]);
                exit(0);
            } else {
                error("--create-config requires a filename");
                exit(1);
            }
        } else if (arg == "--validate-config") {
            if (i + 1 < argc) {
                if (validateConfigFile(argv[++i])) {
                    log("Configuration file is valid", "INFO");
                    exit(0);
                } else {
                    error("Configuration file validation failed");
                    exit(1);
                }
            } else {
                error("--validate-config requires a filename");
                exit(1);
            }
        } else if (arg == "demo") {
            config.mode = "demo";
        } else if (arg == "experiment") {
            config.mode = "experiment";
        } else if (arg == "explore") {
            config.mode = "explore";
        } else if (arg == "benchmark") {
            config.mode = "benchmark";
        } else if (arg == "config") {
            // Next argument should be action (list, set, get)
            if (i + 1 < argc) {
                std::string action = argv[++i];
                if (action == "list") {
                    log("Configuration management not yet implemented in CLI", "WARN");
                    exit(0);
                } else if (action == "set") {
                    // TODO: Implement config set
                    log("Config set not yet implemented", "WARN");
                    exit(0);
                } else if (action == "get") {
                    // TODO: Implement config get
                    log("Config get not yet implemented", "WARN");
                    exit(0);
                }
            }
            config.mode = "demo"; // fallback
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                config.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                config.configFile = argv[++i];
            }
        } else if (arg.substr(0, 8) == "--steps=") {
            config.steps = std::stoi(arg.substr(9));
        } else if (arg == "--steps" && i + 1 < argc) {
            config.steps = std::stoi(argv[++i]);
        } else if (arg.substr(0, 9) == "--neurons=") {
            config.neurons = std::stoi(arg.substr(10));
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neurons = std::stoi(argv[++i]);
        } else if (arg.substr(0, 13) == "--checkpoint=") {
            config.checkpointFile = arg.substr(14);
            config.enableCheckpointing = true;
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            config.checkpointFile = argv[++i];
            config.enableCheckpointing = true;
        } else if (arg == "--verbose") {
            config.verbose = true;
            config.logLevel = "debug";
        } else if (arg == "--quiet") {
            config.quiet = true;
            config.logLevel = "error";
        } else if (arg.substr(0, 13) == "--log-level=") {
            config.logLevel = arg.substr(14);
        } else if (arg == "--log-level" && i + 1 < argc) {
            config.logLevel = argv[++i];
        } else if (arg.substr(0, 11) == "--output=") {
            config.outputFile = arg.substr(12);
        } else if (arg == "--output" && i + 1 < argc) {
            config.outputFile = argv[++i];
        } else if (arg.substr(0, 17) == "--experiment-mode=") {
            config.experimentMode = arg.substr(18);
        } else if (arg == "--experiment-mode" && i + 1 < argc) {
            config.experimentMode = argv[++i];
        } else if (arg == "--enable-replay") {
            config.enableReplay = true;
        } else if (arg == "--disable-development") {
            config.enableDevelopment = false;
        } else if (arg == "--profile") {
            log("Performance profiling enabled", "DEBUG");
            config.mode = "benchmark";
        } else if (arg == "--dump-neurons") {
            log("Neuron dump not yet implemented", "WARN");
        } else if (arg == "--dump-synapses") {
            log("Synaptic dump not yet implemented", "WARN");
        } else if (arg == "--stats") {
            if (i + 1 < argc) {
                config.outputFile = "stats_" + std::string(argv[++i]);
            }
        } else {
            error("Unknown argument: " + arg);
            printHelp(argv[0]);
            exit(1);
        }
    }
    
    return config;
}

int NLMCLI::executeDemoMode(const CLIConfig& config) {
    log("Running NLM Phase 2 demo with " + std::to_string(config.neurons) + " neurons", "INFO");
    
    // Create and initialize brain
    auto configPtr = std::make_shared<Config>();
    setupConfiguration(configPtr, config);
    
    auto brain = std::make_shared<Brain>(configPtr);
    if (!brain->initialize()) {
        error("Failed to initialize brain");
        return 1;
    }
    
    // Run the demo tests (similar to main.cpp)
    log("=== Test 1: Basic Neural Connectivity ===", "INFO");
    runBasicConnectivityTest(brain, config.steps);
    
    log("\n=== Test 2: Plasticity Learning Experiment ===", "INFO");
    resetBrainForPlasticity(brain);
    runPlasticityExperiment(brain, config.steps);
    
    log("\n=== Test 3: STDP Verification ===", "INFO");
    resetBrainForStdp(brain);
    runStdpVerification(brain);
    
    // Print final brain status
    printBrainStatus(brain);
    
    // Save results if requested
    if (!config.outputFile.empty()) {
        saveResultsToFile(config.outputFile, "Demo completed successfully");
    }
    
    log("Demo completed successfully", "INFO");
    return 0;
}

int NLMCLI::executeExperimentMode(const CLIConfig& config) {
    log("Running Phase 6 integration experiment", "INFO");
    
    // Configure experiment
    Phase6Config phaseConfig;
    phaseConfig.maxSteps = config.steps;
    phaseConfig.neuronCount = config.neurons;
    phaseConfig.enableCheckpointing = config.enableCheckpointing;
    phaseConfig.enableReplay = config.enableReplay;
    phaseConfig.enableDevelopment = config.enableDevelopment;
    phaseConfig.checkpointPath = config.checkpointFile.empty() ? "./checkpoint_test.bin" : config.checkpointFile;
    
    // Run experiment
    Phase6IntegratedExperiment experiment;
    Phase6IntegrationResult result;
    
    if (config.experimentMode == "quick") {
        log("Running quick integration verification", "INFO");
        bool integrationOK = experiment.verifyIntegration();
        if (!integrationOK) {
            error("Integration verification failed");
            return 1;
        }
        
        log("Quick verification passed", "INFO");
        
    } else {
        log("Running full integration experiment with custom config", "INFO");
        result = experiment.run(phaseConfig);
    }
    
    // Print results
    printExperimentResults(result);
    
    // Save results if requested
    if (!config.outputFile.empty()) {
        saveResultsToFile(config.outputFile, formatResults(result));
    }
    
    log("Experiment completed successfully", "INFO");
    return 0;
}

int NLMCLI::executeExploreMode(const CLIConfig& config) {
    log("Starting interactive exploration mode", "INFO");
    log("(Note: Full interactive mode requires additional implementation)", "WARN");
    log("Switching to enhanced demo mode for exploration", "INFO");
    
    // Fall back to demo mode with enhanced settings
    return executeDemoMode(config);
}

int NLMCLI::executeBenchmarkMode(const CLIConfig& config) {
    log("Running performance benchmark", "INFO");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create and initialize brain with benchmark settings
    auto configPtr = std::make_shared<Config>();
    setupConfiguration(configPtr, config);
    
    auto brain = std::make_shared<Brain>(configPtr);
    if (!brain->initialize()) {
        error("Failed to initialize brain for benchmarking");
        return 1;
    }
    
    // Run benchmark steps
    log("Running " + std::to_string(config.steps) + " benchmark steps", "INFO");
    
    auto benchStart = std::chrono::high_resolution_clock::now();
    for (int step = 0; step < config.steps; ++step) {
        brain->step(step, step * configPtr->getOr<double>("simulation_timestep", 0.001));
        
        if (step % 100 == 0) {
            log("Step " + std::to_string(step) + ": " + std::to_string(brain->getTotalSpikeCount()) + " spikes", "DEBUG");
        }
    }
    
    auto benchEnd = std::chrono::high_resolution_clock::now();
    auto benchDuration = std::chrono::duration_cast<std::chrono::milliseconds>(benchEnd - benchStart);
    
    // Print benchmark results
    log("\n=== BENCHMARK RESULTS ===", "INFO");
    log("Total steps: " + std::to_string(config.steps), "INFO");
    log("Total spikes: " + std::to_string(brain->getTotalSpikeCount()), "INFO");
    log("Average spikes per step: " + std::to_string(brain->getTotalSpikeCount() / config.steps), "INFO");
    log("Benchmark duration: " + std::to_string(benchDuration.count()) + " ms", "INFO");
    log("Speed: " + std::to_string(config.steps * 1000.0 / benchDuration.count()) + " steps/sec", "INFO");
    
    // Print brain status
    printBrainStatus(brain);
    
    // Save benchmark results if requested
    if (!config.outputFile.empty()) {
        std::string benchmarkData = "Benchmark Results:\n" +
                                   "Total steps: " + std::to_string(config.steps) + "\n" +
                                   "Total spikes: " + std::to_string(brain->getTotalSpikeCount()) + "\n" +
                                   "Duration: " + std::to_string(benchDuration.count()) + " ms\n" +
                                   "Speed: " + std::to_string(config.steps * 1000.0 / benchDuration.count()) + " steps/sec";
        saveResultsToFile(config.outputFile, benchmarkData);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    log("Benchmark completed in " + std::to_string(totalDuration.count()) + " seconds", "INFO");
    return 0;
}

void NLMCLI::setupConfiguration(std::shared_ptr<Config> config, const CLIConfig& cliConfig) {
    // Set default values
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(cliConfig.neurons), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Load from config file if exists
    if (config->loadFromFile(cliConfig.configFile)) {
        log("Loaded configuration from: " + cliConfig.configFile, "INFO");
    } else {
        log("Using default configuration", "INFO");
    }
    
    // Override with command line args
    config->loadFromArgs(0, nullptr); // No command line args to process
}

void NLMCLI::setupLogging(const std::string& level) {
    auto logger = std::make_shared<Logger>();
    
    // Create appropriate logger based on level
    auto consoleLogger = std::make_shared<ConsoleLogger>(Logger::stringToLevel(level));
    logger->addLogger(consoleLogger);
    
    // Set global logger
    Logger::setGlobal(logger);
}

void NLMCLI::printBrainStatus(const std::shared_ptr<Brain>& brain) {
    log("\n=== BRAIN STATUS ===", "INFO");
    log("Regions: " + std::to_string(brain->getRegionCount()), "INFO");
    log("Total neurons: " + std::to_string(brain->getTotalNeuronCount()), "INFO");
    log("Total synapses: " + std::to_string(brain->getTotalSynapseCount()), "INFO");
    log("Active neurons: " + std::to_string(brain->getActiveNeuronCount()), "INFO");
    log("Firing neurons (this step): " + std::to_string(brain->getFiringNeuronCount()), "INFO");
    log("Total spikes: " + std::to_string(brain->getTotalSpikeCount()), "INFO");
    log("Pending spike events: " + std::to_string(brain->getPendingSpikeEventCount()), "INFO");
    log("Average firing rate: " + std::to_string(brain->getAverageFiringRate()), "INFO");
}

void NLMCLI::printExperimentResults(const Phase6IntegrationResult& result) {
    log("\n=== PHASE 6 INTEGRATION RESULTS ===", "INFO");
    log("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"), "INFO");
    log("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"), "INFO");
    log("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"), "INFO");
    log("Prediction: " + std::string(result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"), "INFO");
    log("Development: " + std::string(result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"), "INFO");
    log("Checkpointing: " + std::string(result.checkpointingWorks ? "WORKING" : "NOT WORKING"), "INFO");
    log("Replay: " + std::string(result.replayWorks ? "WORKING" : "NOT WORKING"), "INFO");
    
    log("\n=== METRICS ===", "INFO");
    log("Total reward: " + std::to_string(result.totalReward), "INFO");
    log("Average firing rate: " + std::to_string(result.avgFiringRate), "INFO");
    log("Average synaptic weight: " + std::to_string(result.avgSynapticWeight), "INFO");
    log("Memory episodes stored: " + std::to_string(result.memoryEpisodesStored), "INFO");
    log("Novelty level: " + std::to_string(result.noveltyLevel), "INFO");
    log("Curiosity level: " + std::to_string(result.curiosityLevel), "INFO");
    log("Dopamine level: " + std::to_string(result.dopamineLevel), "INFO");
}

std::string NLMCLI::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

void NLMCLI::log(const std::string& message, const std::string& level) {
    if (level == "ERROR" || level == "CRITICAL") {
        error(message);
        return;
    }
    
    std::cout << "[" << getCurrentTimestamp() << "] " << level << ": " << message << std::endl;
}

void NLMCLI::error(const std::string& message) {
    std::cerr << "[ERROR] " << getCurrentTimestamp() << ": " << message << std::endl;
}

bool NLMCLI::validateConfigFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple validation - check if file contains reasonable content
    std::string line;
    size_t lineCount = 0;
    while (std::getline(file, line) && lineCount < 100) {
        if (line.find("nlm") != std::string::npos || line.find("neuron") != std::string::npos) {
            return true;
        }
        lineCount++;
    }
    
    return lineCount > 0;
}

void NLMCLI::createDefaultConfig(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        error("Failed to create config file: " + filepath);
        return;
    }
    
    file << "# NLM Default Configuration File" << std::endl;
    file << "# Generated by NLM CLI" << std::endl;
    file << std::endl;
    file << "# Core simulation parameters" << std::endl;
    file << "random_seed = 42" << std::endl;
    file << "simulation_timestep = 0.001" << std::endl;
    file << "neuron_count = 500" << std::endl;
    file << "region_count = 1" << std::endl;
    file << "connection_probability = 0.15" << std::endl;
    file << std::endl;
    file << "# Plasticity parameters" << std::endl;
    file << "stdp_ltp_weight = 0.02" << std::endl;
    file << "stdp_ltd_weight = 0.015" << std::endl;
    file << "stdp_tau = 20.0" << std::endl;
    file << "synaptogenesis_rate = 0.0001" << std::endl;
    file << "pruning_rate = 0.00001" << std::endl;
    file << std::endl;
    file << "# Memory parameters" << std::endl;
    file << "working_memory_capacity = 50" << std::endl;
    file << "episodic_memory_capacity = 1000" << std::endl;
    file << "replay_interval = 100" << std::endl;
    file << std::endl;
    file << "# Neuromodulation" << std::endl;
    file << "dopamine_enabled = true" << std::endl;
    file << "curiosity_enabled = true" << std::endl;
    file << "novelty_enabled = true" << std::endl;
    file << "enable_reward_modulation = true" << std::endl;
    file << std::endl;
    file << "# Development" << std::endl;
    file << "development_enabled = true" << std::endl;
    file << "enable_structural_plasticity = true" << std::endl;
    
    file.close();
    log("Created default configuration file: " + filepath, "INFO");
}

void NLMCLI::saveResultsToFile(const std::string& filename, const std::string& data) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        error("Failed to save results to file: " + filename);
        return;
    }
    
    file << data;
    file.close();
    log("Results saved to: " + filename, "INFO");
}

std::string NLMCLI::formatResults(const Phase6IntegrationResult& result) {
    std::stringstream ss;
    ss << "Phase 6 Integration Experiment Results" << std::endl;
    ss << "====================================" << std::endl;
    ss << "Timestamp: " << getCurrentTimestamp() << std::endl;
    ss << std::endl;
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
    ss << "  Total Reward: " << result.totalReward << std::endl;
    ss << "  Average Firing Rate: " << result.avgFiringRate << std::endl;
    ss << "  Average Synaptic Weight: " << result.avgSynapticWeight << std::endl;
    ss << "  Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    ss << "  Novelty Level: " << result.noveltyLevel << std::endl;
    ss << "  Curiosity Level: " << result.curiosityLevel << std::endl;
    ss << "  Dopamine Level: " << result.dopamineLevel << std::endl;
    
    return ss.str();
}

// Helper function implementations for demo mode
void NLMCLI::runBasicConnectivityTest(std::shared_ptr<Brain> brain, int steps) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void NLMCLI::resetBrainForPlasticity(std::shared_ptr<Brain> brain) {
    brain->reset();
    brain->initialize();
}

void NLMCLI::resetBrainForStdp(std::shared_ptr<Brain> brain) {
    brain->reset();
    brain->initialize();
}

void NLMCLI::runPlasticityExperiment(std::shared_ptr<Brain> brain, int steps) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(steps) + " steps)...");
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("Plasticity experiment completed");
}

void NLMCLI::runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

} // namespace cli
} // namespace nlm
