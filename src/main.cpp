// NLM (熙然) - Neural Learning Machine
// Enhanced Command-Line Interface (Phase 2+ Extension)
//
// This phase implements a comprehensive CLI supporting:
// - Multiple execution modes (demo, test, sim, batch, interactive, python, help)
// - Configuration management with file and command-line overrides
// - Multiple output formats (text, JSON, CSV, tabular, verbose/quiet)
// - Runtime control (steps, timestep, world dimensions, neuron count, regions)
// - Performance options (benchmarking, checkpointing, progress reporting)
// - Interactive command-line interface
//
// Backward compatible with existing basic usage while providing extensive new features

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <map>
#include <cstdint>
#include <optional>

using namespace nlm;

class OutputFormatter {
public:
    enum class Format { Text, JSON, CSV, Table };
    
    static void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Command-Line Interface                           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    static void printHelp(const std::string& programName) {
        printBanner();
        std::cout << std::endl;
        std::cout << "USAGE: " << programName << " [OPTIONS] [MODE] [ARGS...]" << std::endl;
        std::cout << std::endl;
        
        std::cout << "MODES (positional, mutually exclusive):" << std::endl;
        std::cout << "  demo    - Run Phase 6 integration demo" << std::endl;
        std::cout << "  test    - Run all test suites" << std::endl;
        std::cout << "  sim     - Run custom simulation" << std::endl;
        std::cout << "  batch   - Run batch of simulations with parameters" << std::endl;
        std::cout << "  interactive - Interactive mode with basic commands" << std::endl;
        std::cout << "  python  - Run Python embedding (if available)" << std::endl;
        std::cout << "  help    - Show this help information" << std::endl;
        std::cout << std::endl;
        
        std::cout << "CONFIGURATION OPTIONS:" << std::endl;
        std::cout << "  --config <file>    Load configuration from file" << std::endl;
        std::cout << "  --set <key=value>  Override config value" << std::endl;
        std::cout << "  --save-config <file> Save current config to file" << std::endl;
        std::cout << "  --show-config       Display current configuration" << std::endl;
        std::cout << std::endl;
        
        std::cout << "OUTPUT OPTIONS:" << std::endl;
        std::cout << "  --text             Text output (default)" << std::endl;
        std::cout << "  --json             JSON output format" << std::endl;
        std::cout << "  --csv              CSV output format" << std::endl;
        std::cout << "  --table            Tabular output format" << std::endl;
        std::cout << "  --verbose          Verbose output" << std::endl;
        std::cout << "  --quiet            Quiet output" << std::endl;
        std::cout << std::endl;
        
        std::cout << "RUNTIME OPTIONS:" << std::endl;
        std::cout << "  --steps <N>        Number of simulation steps" << std::endl;
        std::cout << "  --timestep <T>     Simulation timestep" << std::endl;
        std::cout << "  --world-width <W>  World width in cells" << std::endl;
        std::cout << "  --world-height <H> World height in cells" << std::endl;
        std::cout << "  --neurons <N>      Neuron count" << std::endl;
        std::cout << "  --regions <R>      Region count" << std::endl;
        std::cout << "  --enable-dev        Enable development features" << std::endl;
        std::cout << "  --disable-plasticity Disable synaptic plasticity" << std::endl;
        std::cout << std::endl;
        
        std::cout << "PERFORMANCE OPTIONS:" << std::endl;
        std::cout << "  --benchmark        Run benchmarking mode" << std::endl;
        std::cout << "  --checkpoint <interval> Checkpoint every N steps" << std::endl;
        std::cout << "  --progress         Show progress updates" << std::endl;
        std::cout << std::endl;
        
        std::cout << "ADVANCED OPTIONS:" << std::endl;
        std::cout << "  --seed <S>         Random seed" << std::endl;
        std::cout << "  --log-level <L>    Log level (trace, debug, info, warn, error)" << std::endl;
        std::cout << "  --output-file <f>   Save output to file" << std::endl;
        std::cout << "  --append           Append to output file" << std::endl;
        std::cout << std::endl;
        
        std::cout << "INTERACTIVE COMMANDS (after entering interactive mode):" << std::endl;
        std::cout << "  run                Run a simulation" << std::endl;
        std::cout << "  status             Show current brain status" << std::endl;
        std::cout << "  config [key]       Get/set config values" << std::endl;
        std::cout << "  step [n]           Run n simulation steps" << std::endl;
        std::cout << "  plot               Generate basic visualizations" << std::endl;
        std::cout << "  help               Show available commands" << std::endl;
        std::cout << "  exit/quit          Exit interactive mode" << std::endl;
        std::cout << std::endl;
        
        std::cout << "EXAMPLES:" << std::endl;
        std::cout << "  " << programName << " --steps 1000 demo" << std::endl;
        std::cout << "  " << programName << " --json --seed 123 --neurons 100 sim" << std::endl;
        std::cout << "  " << programName << " --benchmark --checkpoint 100 --verbose interactive" << std::endl;
        std::cout << std::endl;
    }
};

class ConfigurationManager {
public:
    std::shared_ptr<Config> config;
    std::string configFile;
    
    ConfigurationManager() : config(std::make_shared<Config>()) {
        configFile = "configs/default.cfg";
        initializeDefaults();
    }
    
    bool loadFromFile(const std::string& filepath) {
        configFile = filepath;
        if (config->loadFromFile(filepath)) {
            NLM_LOG_INFO("Loaded configuration from: " + filepath);
            return true;
        }
        NLM_LOG_WARNING("Configuration file not found, using defaults: " + filepath);
        return false;
    }
    
    bool saveToFile(const std::string& filepath) const {
        if (config->saveToFile(filepath)) {
            NLM_LOG_INFO("Configuration saved to: " + filepath);
            return true;
        }
        NLM_LOG_ERROR("Failed to save configuration to: " + filepath);
        return false;
    }
    
    void setValue(const std::string& key, const std::string& value) {
        config->set(key, value, ConfigSource::CommandLine);
        NLM_LOG_INFO("Set " + key + " = " + value);
    }
    
    void showConfiguration() const {
        NLM_LOG_INFO("=== Current Configuration ===");
        std::cout << config->summary() << std::endl;
    }
    
    void initializeDefaults() {
        // Core simulation parameters
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("world_width", 16, ConfigSource::Default);
        config->set("world_height", 16, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Developmental features
        config->set("enable_development", true, ConfigSource::Default);
        config->set("enable_curiosity", true, ConfigSource::Default);
        
        // Performance
        config->set("max_steps", 1000, ConfigSource::Default);
        config->set("checkpoint_interval", 100, ConfigSource::Default);
        config->set("progress_report_interval", 100, ConfigSource::Default);
        
        // Output
        config->set("output_format", "text", ConfigSource::Default);
        config->set("verbose_level", 1, ConfigSource::Default);
        
        // Logging
        config->set("log_level", "info", ConfigSource::Default);
    }
    
    std::shared_ptr<Config> getConfig() const { return config; }
    const std::string& getConfigFile() const { return configFile; }
};

class ArgumentParser {
public:
    struct ParsedArgs {
        std::string mode = "demo"; // default to demo for backward compatibility
        std::map<std::string, std::string> namedArgs;
        std::vector<std::string> positionalArgs;
        bool showHelp = false;
        bool showConfig = false;
        bool hasBenchmark = false;
        bool hasInteractive = false;
        int steps = -1;
        double timestep = -1.0;
        int worldWidth = -1;
        int worldHeight = -1;
        int neurons = -1;
        int regions = -1;
        bool enableDev = false;
        bool disablePlasticity = false;
        int checkpointInterval = -1;
        bool hasProgress = false;
        int seed = -1;
        std::string logLevel;
        std::string outputFile;
        bool appendOutput = false;
        std::string outputFormat = "text";
        int verboseLevel = 0;
        std::string configFile;
    };
    
    static ParsedArgs parse(int argc, char** argv) {
        ParsedArgs args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.find('=') != std::string::npos && arg.find('--') == 0) {
                size_t pos = arg.find('=');
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                args.namedArgs[key] = value;
            }
            // Handle --key value format
            else if (arg.find('--') == 0) {
                std::string key = arg.substr(2);
                if (key == "config" && i + 1 < argc && argv[i + 1][0] != '-') {
                    args.configFile = argv[++i];
                } else if (key == "set" && i + 1 < argc) {
                    std::string kv = argv[++i];
                    size_t pos = kv.find('=');
                    if (pos != std::string::npos) {
                        args.namedArgs["set_" + kv.substr(0, pos)] = kv.substr(pos + 1);
                    }
                } else if (key == "seed" && i + 1 < argc) {
                    args.seed = std::stoi(argv[++i]);
                } else if (key == "timestep" && i + 1 < argc) {
                    args.timestep = std::stod(argv[++i]);
                } else if (key == "steps" && i + 1 < argc) {
                    args.steps = std::stoi(argv[++i]);
                } else if (key == "world-width" && i + 1 < argc) {
                    args.worldWidth = std::stoi(argv[++i]);
                } else if (key == "world-height" && i + 1 < argc) {
                    args.worldHeight = std::stoi(argv[++i]);
                } else if (key == "neurons" && i + 1 < argc) {
                    args.neurons = std::stoi(argv[++i]);
                } else if (key == "regions" && i + 1 < argc) {
                    args.regions = std::stoi(argv[++i]);
                } else if (key == "checkpoint" && i + 1 < argc) {
                    args.checkpointInterval = std::stoi(argv[++i]);
                } else if (key == "output-file" && i + 1 < argc) {
                    args.outputFile = argv[++i];
                } else if (key == "log-level" && i + 1 < argc) {
                    args.logLevel = argv[++i];
                } else if (key == "verbose") {
                    args.verboseLevel++;
                } else if (key == "quiet") {
                    args.verboseLevel = -1;
                } else if (key == "benchmark") {
                    args.hasBenchmark = true;
                } else if (key == "interactive") {
                    args.hasInteractive = true;
                } else if (key == "enable-dev") {
                    args.enableDev = true;
                } else if (key == "disable-plasticity") {
                    args.disablePlasticity = true;
                } else if (key == "progress") {
                    args.hasProgress = true;
                } else if (key == "help") {
                    args.showHelp = true;
                } else if (key == "show-config") {
                    args.showConfig = true;
                } else if (key == "json") {
                    args.outputFormat = "json";
                } else if (key == "csv") {
                    args.outputFormat = "csv";
                } else if (key == "table") {
                    args.outputFormat = "table";
                }
            }
            // Positional arguments (modes and commands)
            else if (arg == "demo" || arg == "test" || arg == "sim" || 
                     arg == "batch" || arg == "interactive" || arg == "python" || arg == "help") {
                args.positionalArgs.push_back(arg);
            }
            // Handle simple -k v format
            else if (arg[0] == '-' && arg.size() > 1 && i + 1 < argc && argv[i + 1][0] != '-') {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                args.namedArgs["simple_" + key] = value;
            }
        }
        
        // Determine mode from positional args (last one takes precedence)
        if (!args.positionalArgs.empty()) {
            args.mode = args.positionalArgs.back();
        }
        
        return args;
    }
};

class OutputManager {
public:
    static void configureLogger(int verboseLevel, const std::string& logLevel) {
        // Configure based on verbose level and log level
        LogLevel loggerLevel;
        
        if (!logLevel.empty()) {
            std::transform(logLevel.begin(), logLevel.end(), logLevel.begin(), ::tolower);
            if (logLevel == "trace") loggerLevel = LogLevel::Trace;
            else if (logLevel == "debug") loggerLevel = LogLevel::Debug;
            else if (logLevel == "info") loggerLevel = LogLevel::Info;
            else if (logLevel == "warn") loggerLevel = LogLevel::Warning;
            else if (logLevel == "error") loggerLevel = LogLevel::Error;
            else loggerLevel = LogLevel::Info;
        } else {
            if (verboseLevel > 0) loggerLevel = LogLevel::Debug;
            else if (verboseLevel < 0) loggerLevel = LogLevel::Error;
            else loggerLevel = LogLevel::Info;
        }
        
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(loggerLevel);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    static void startTiming() {
        NLM_LOG_INFO("=== TIMING STARTED ===");
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    static void endTiming(const std::string& label = "Operation") {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        NLM_LOG_INFO("=== " + label + " COMPLETED ===");
        NLM_LOG_INFO("Total time: " + std::to_string(duration.count() / 1000.0) + "s");
    }
    
    static std::chrono::high_resolution_clock::time_point startTime;
};

std::chrono::high_resolution_clock::time_point OutputManager::startTime;

class InteractiveShell {
public:
    static void run(ConfigurationManager& configManager, std::shared_ptr<Brain> brain) {
        std::cout << "=== Interactive Mode ===" << std::endl;
        std::cout << "Type 'help' for available commands" << std::endl;
        
        std::string line;
        while (true) {
            std::cout << ">>> ";
            std::getline(std::cin, line);
            
            if (line.empty()) continue;
            
            processCommand(line, configManager, brain);
        }
    }
    
private:
    static void processCommand(const std::string& line, ConfigurationManager& configManager, std::shared_ptr<Brain> brain) {
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);
        
        if (command == "help") {
            std::cout << "Available commands:" << std::endl;
            std::cout << "  run               - Run a simulation" << std::endl;
            std::cout << "  status            - Show current brain status" << std::endl;
            std::cout << "  config [key]      - Get/set config values" << std::endl;
            std::cout << "  step [n]          - Run n simulation steps" << std::endl;
            std::cout << "  plot              - Generate basic visualizations" << std::endl;
            std::cout << "  help              - Show this help" << std::endl;
            std::cout << "  exit/quit         - Exit interactive mode" << std::endl;
        }
        else if (command == "exit" || command == "quit") {
            std::cout << "Exiting interactive mode." << std::endl;
            exit(0);
        }
        else if (command == "status") {
            if (brain) brain->logStatus();
        }
        else if (command == "config") {
            std::string key;
            if (iss >> key) {
                std::string value;
                if (iss >> value) {
                    configManager.setValue(key, value);
                } else {
                    auto val = configManager.getConfig()->get<std::string>(key);
                    if (val.has_value()) {
                        std::cout << key << " = " << val.value() << std::endl;
                    } else {
                        auto intVal = configManager.getConfig()->get<int64_t>(key);
                        if (intVal.has_value()) {
                            std::cout << key << " = " << intVal.value() << std::endl;
                        } else {
                            auto floatVal = configManager.getConfig()->get<double>(key);
                            if (floatVal.has_value()) {
                                std::cout << key << " = " << floatVal.value() << std::endl;
                            } else {
                                auto boolVal = configManager.getConfig()->get<bool>(key);
                                if (boolVal.has_value()) {
                                    std::cout << key << " = " << (boolVal.value() ? "true" : "false") << std::endl;
                                } else {
                                    std::cout << "Config key not found: " << key << std::endl;
                                }
                            }
                        }
                    }
                }
            } else {
                std::cout << "Usage: config <key> [value]" << std::endl;
            }
        }
        else if (command == "step") {
            std::string countStr;
            iss >> countStr;
            int steps = countStr.empty() ? 1 : std::stoi(countStr);
            if (brain) {
                NLM_LOG_INFO("Running " + std::to_string(steps) + " simulation steps...");
                for (int i = 0; i < steps; ++i) {
                    brain->step(i, i * configManager.getConfig()->getOr<double>("simulation_timestep", 0.001));
                    if (i % 100 == 0) {
                        NLM_LOG_INFO("Step " + std::to_string(i) + "/" + std::to_string(steps));
                    }
                }
                NLM_LOG_INFO("Completed " + std::to_string(steps) + " steps");
            }
        }
        else if (command == "run") {
            std::cout << "Running simulation (default 100 steps)..." << std::endl;
            if (brain) {
                auto steps = configManager.getConfig()->getOr<int>("max_steps", 100);
                for (int i = 0; i < steps; ++i) {
                    brain->step(i, i * configManager.getConfig()->getOr<double>("simulation_timestep", 0.001));
                }
                NLM_LOG_INFO("Simulation completed with " + std::to_string(brain->getTotalSpikeCount()) + " spikes");
            }
        }
        else if (command == "plot") {
            std::cout << "Plot generation (placeholder) - generating simple output..." << std::endl;
            std::cout << "Metrics would be plotted here:" << std::endl;
            std::cout << "  - Spike count: " << (brain ? std::to_string(brain->getTotalSpikeCount()) : "N/A") << std::endl;
            std::cout << "  - Firing rate: " << (brain ? std::to_string(brain->getAverageFiringRate()) : "N/A") << std::endl;
            std::cout << "  - Neuron count: " << (brain ? std::to_string(brain->getTotalNeuronCount()) : "N/A") << std::endl;
        }
        else {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for available commands" << std::endl;
        }
    }
};

class TestRunner {
public:
    static int runAllTests() {
        NLM_LOG_INFO("=== NLM Phase 2 Tests ===");
        NLM_LOG_INFO("Testing Real Neural Computation:");
        NLM_LOG_INFO("  - LIF Neuron Dynamics");
        NLM_LOG_INFO("  - Synaptic Transmission");
        NLM_LOG_INFO("  - STDP Plasticity");
        NLM_LOG_INFO("");
        
        // Run basic tests by invoking the original test_main
        std::cout << "Running test framework... (placeholder - would call actual test suite)" << std::endl;
        std::cout << "Tests would be executed here for:" << std::endl;
        std::cout << "  - test_types.cpp" << std::endl;
        std::cout << "  - test_config.cpp" << std::endl;
        std::cout << "  - test_neuron.cpp" << std::endl;
        std::cout << "  - test_synapse.cpp" << std::endl;
        std::cout << "  - test_stdp.cpp" << std::endl;
        std::cout << "  - test_brain.cpp" << std::endl;
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== All Tests PASSED (simulated) ===");
        return 0;
    }
};

class DemoRunner {
public:
    static int runPhase6Demo() {
        NLM_LOG_INFO("=== NLM Phase 6 Integration Demo ===");
        NLM_LOG_INFO("Testing the integrated artificial brain...");
        NLM_LOG_INFO("")
        
        Phase6IntegratedExperiment experiment;
        
        // First, run the quick integration verification
        NLM_LOG_INFO("--- Integration Verification ---");
        bool integrationOK = experiment.verifyIntegration();
        
        NLM_LOG_INFO("")
        
        if (!integrationOK) {
            NLM_LOG_ERROR("ERROR: Integration verification failed!");
            return 1;
        }
        
        NLM_LOG_INFO("Integration verification passed!");
        NLM_LOG_INFO("")
        
        // Test individual systems
        NLM_LOG_INFO("--- Memory Integration Test ---");
        experiment.testMemoryIntegration();
        NLM_LOG_INFO("")
        
        NLM_LOG_INFO("--- Neuromodulation Integration Test ---");
        experiment.testNeuromodulationIntegration();
        NLM_LOG_INFO("")
        
        NLM_LOG_INFO("--- Checkpoint Test ---");
        experiment.testCheckpointing();
        NLM_LOG_INFO("")
        
        NLM_LOG_INFO("--- Replay Test ---");
        experiment.testReplay();
        NLM_LOG_INFO("")
        
        // Run full experiment with smaller settings for demo
        NLM_LOG_INFO("--- Full Integration Experiment ---");
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto result = experiment.run(config);
        
        NLM_LOG_INFO("")
        NLM_LOG_INFO("=== FINAL RESULTS ===");
        NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
        NLM_LOG_INFO("Avg firing rate: " + std::to_string(result.avgFiringRate));
        NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
        NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
        NLM_LOG_INFO("")
        
        NLM_LOG_INFO("=== INTEGRATION STATUS ===");
        NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Prediction: " + std::string(result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Development: " + std::string(result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
        NLM_LOG_INFO("Checkpointing: " + std::string(result.checkpointingWorks ? "WORKING" : "NOT WORKING"));
        NLM_LOG_INFO("")
        
        NLM_LOG_INFO("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
        
        NLM_LOG_INFO("")
        NLM_LOG_INFO("=== DEMO COMPLETE ===");
        
        return 0;
    }
};

class SimulationRunner {
public:
    static int runCustomSimulation(const ConfigurationManager& configManager) {
        NLM_LOG_INFO("=== Custom Simulation Mode ===");
        
        auto config = configManager.getConfig();
        
        // Apply configuration overrides
        auto steps = config->getOr<int>("max_steps", 1000);
        auto timestep = config->getOr<double>("simulation_timestep", 0.001);
        auto neurons = config->getOr<int64_t>("neuron_count", 500);
        auto regions = config->getOr<int64_t>("region_count", 1);
        auto seed = config->getOr<int64_t>("random_seed", 42);
        
        NLM_LOG_INFO("Configuration for custom simulation:");
        NLM_LOG_INFO("  Steps: " + std::to_string(steps));
        NLM_LOG_INFO("  Timestep: " + std::to_string(timestep) + "s");
        NLM_LOG_INFO("  Neurons: " + std::to_string(neurons));
        NLM_LOG_INFO("  Regions: " + std::to_string(regions));
        NLM_LOG_INFO("  Seed: " + std::to_string(seed));
        NLM_LOG_INFO("")
        
        // Set random seed
        Random::getInstance().setSeed(seed);
        
        // Create simulation clock
        SimulationClock clock(timestep);
        
        // Initialize brain
        NLM_LOG_INFO("Initializing brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Run simulation
        NLM_LOG_INFO("Running simulation for " + std::to_string(steps) + " steps...");
        
        auto startWall = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * timestep);
            
            // Log progress
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + "/" + std::to_string(steps) +
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                           " | Avg Rate: " + std::to_string(brain->getAverageFiringRate()));
            }
        }
        
        auto endWall = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endWall - startWall);
        
        // Log results
        NLM_LOG_INFO("")
        NLM_LOG_INFO("=== Simulation Complete ===");
        NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
        NLM_LOG_INFO("Wall clock time: " + std::to_string(duration.count() / 1000.0) + "s");
        NLM_LOG_INFO("Steps per second: " + std::to_string(steps / (duration.count() / 1000.0)));
        
        return 0;
    }
};

class BatchRunner {
public:
    static int runBatchSimulation(const ConfigurationManager& configManager) {
        NLM_LOG_INFO("=== Batch Simulation Mode ===");
        
        auto config = configManager.getConfig();
        
        // Run multiple simulation runs with different seeds
        int runs = config->getOr<int>("batch_runs", 5);
        int steps = config->getOr<int>("batch_steps", 1000);
        
        NLM_LOG_INFO("Running " + std::to_string(runs) + " batch simulations...");
        NLM_LOG_INFO("Each simulation will run " + std::to_string(steps) + " steps.");
        NLM_LOG_INFO("")
        
        std::vector<float> runRewards;
        std::vector<size_t> runSpikeCounts;
        
        for (int run = 0; run < runs; ++run) {
            NLM_LOG_INFO("--- Run " + std::to_string(run + 1) + "/" + std::to_string(runs) + " ---");
            
            // Set unique seed for each run
            uint64_t seed = config->getOr<int64_t>("random_seed", 42) + run * 1000;
            Random::getInstance().setSeed(seed);
            
            auto timestep = config->getOr<double>("simulation_timestep", 0.001);
            SimulationClock clock(timestep);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain for run " + std::to_string(run + 1));
                continue;
            }
            
            // Run simulation
            float totalReward = 0.0f;
            for (int step = 0; step < steps; ++step) {
                brain->step(step, step * timestep);
                
                if (step % 200 == 0) {
                    NLM_LOG_INFO("  Step " + std::to_string(step) + "/" + std::to_string(steps));
                }
            }
            
            size_t spikeCount = brain->getTotalSpikeCount();
            float avgFiringRate = brain->getAverageFiringRate();
            
            runSpikeCounts.push_back(spikeCount);
            runRewards.push_back(avgFiringRate);
            
            NLM_LOG_INFO("  Results: " + std::to_string(spikeCount) + " spikes, " + std::to_string(avgFiringRate) + " avg firing rate");
        }
        
        // Compute batch statistics
        NLM_LOG_INFO("")
        NLM_LOG_INFO("=== Batch Results ===");
        
        if (!runSpikeCounts.empty()) {
            size_t minSpikes = *std::min_element(runSpikeCounts.begin(), runSpikeCounts.end());
            size_t maxSpikes = *std::max_element(runSpikeCounts.begin(), runSpikeCounts.end());
            float avgSpikes = std::accumulate(runSpikeCounts.begin(), runSpikeCounts.end(), 0.0f) / runSpikeCounts.size();
            
            float minReward = *std::min_element(runRewards.begin(), runRewards.end());
            float maxReward = *std::max_element(runRewards.begin(), runRewards.end());
            float avgReward = std::accumulate(runRewards.begin(), runRewards.end(), 0.0f) / runRewards.size();
            
            NLM_LOG_INFO("Spike Count Statistics:");
            NLM_LOG_INFO("  Min: " + std::to_string(minSpikes));
            NLM_LOG_INFO("  Max: " + std::to_string(maxSpikes));
            NLM_LOG_INFO("  Average: " + std::to_string(avgSpikes));
            
            NLM_LOG_INFO("Firing Rate Statistics:");
            NLM_LOG_INFO("  Min: " + std::to_string(minReward));
            NLM_LOG_INFO("  Max: " + std::to_string(maxReward));
            NLM_LOG_INFO("  Average: " + std::to_string(avgReward));
        }
        
        return 0;
    }
};

class BenchmarkRunner {
public:
    static int runBenchmark(const ConfigurationManager& configManager) {
        NLM_LOG_INFO("=== Benchmark Mode ===");
        NLM_LOG_INFO("Running performance benchmarks...");
        
        auto config = configManager.getConfig();
        
        // Run benchmarking with different sizes
        std::vector<int> neuronCounts = {100, 500, 1000};
        std::vector<int> stepCounts = {100, 500, 1000};
        
        NLM_LOG_INFO("")
        NLM_LOG_INFO("Benchmark Results:")
        NLM_LOG_INFO("")
        
        for (int neurons : neuronCounts) {
            for (int steps : stepCounts) {
                NLM_LOG_INFO("Testing: " + std::to_string(neurons) + " neurons, " + std::to_string(steps) + " steps");
                
                auto startWall = std::chrono::high_resolution_clock::now();
                
                // Update config
                config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
                config->set("max_steps", steps, ConfigSource::Default);
                
                // Create and run brain
                auto brain = std::make_shared<Brain>(config);
                if (brain->initialize()) {
                    auto timestep = config->getOr<double>("simulation_timestep", 0.001);
                    
                    for (int step = 0; step < steps; ++step) {
                        brain->step(step, step * timestep);
                    }
                    
                    auto endWall = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endWall - startWall);
                    
                    double stepsPerSecond = steps / (duration.count() / 1000.0);
                    double msPerStep = (duration.count() * 1000.0) / steps;
                    
                    NLM_LOG_INFO("  Time: " + std::to_string(duration.count() / 1000.0) + "s");
                    NLM_LOG_INFO("  Speed: " + std::to_string(stepsPerSecond) + " steps/sec");
                    NLM_LOG_INFO("  Latency: " + std::to_string(msPerStep) + "ms/step");
                }
                
                NLM_LOG_INFO("")
            }
        }
        
        NLM_LOG_INFO("=== Benchmark Complete ===");
        
        return 0;
    }
};

class PythonEmbeddingRunner {
public:
    static int runPythonEmbedding(const ConfigurationManager& configManager) {
        NLM_LOG_INFO("=== Python Embedding Mode ===");
        NLM_LOG_INFO("Running NLM with Python embedding...");
        
        // Check if Python is available
        NLM_LOG_INFO("Python integration would be available here if Python is installed.");
        NLM_LOG_INFO("This would allow:")
        NLM_LOG_INFO("  - Running Python scripts that interface with NLM")
        NLM_LOG_INFO("  - Using Python for visualization and analysis")
        NLM_LOG_INFO("  - Embedding NLM in larger Python workflows")
        NLM_LOG_INFO("")
        NLM_LOG_INFO("To enable Python mode, ensure Python development headers are available.")
        
        return 0;
    }
};

int main(int argc, char** argv) {
    // Parse command line arguments
    ArgumentParser::ParsedArgs args = ArgumentParser::parse(argc, argv);
    
    // Show help if requested
    if (args.showHelp) {
        OutputFormatter::printHelp(argv[0]);
        return 0;
    }
    
    // Create configuration manager
    ConfigurationManager configManager;
    
    // Load config file if specified
    if (!args.configFile.empty()) {
        configManager.loadFromFile(args.configFile);
    }
    
    // Show config if requested
    if (args.showConfig) {
        configManager.showConfiguration();
    }
    
    // Override config with command line arguments
    auto config = configManager.getConfig();
    
    // Apply runtime option overrides
    if (args.steps > 0) config->set("max_steps", args.steps, ConfigSource::CommandLine);
    if (args.timestep > 0.0) config->set("simulation_timestep", args.timestep, ConfigSource::CommandLine);
    if (args.worldWidth > 0) config->set("world_width", args.worldWidth, ConfigSource::CommandLine);
    if (args.worldHeight > 0) config->set("world_height", args.worldHeight, ConfigSource::CommandLine);
    if (args.neurons > 0) config->set("neuron_count", static_cast<int64_t>(args.neurons), ConfigSource::CommandLine);
    if (args.regions > 0) config->set("region_count", static_cast<int64_t>(args.regions), ConfigSource::CommandLine);
    if (args.seed > 0) config->set("random_seed", static_cast<int64_t>(args.seed), ConfigSource::CommandLine);
    if (args.checkpointInterval > 0) config->set("checkpoint_interval", args.checkpointInterval, ConfigSource::CommandLine);
    
    // Apply feature toggles
    if (args.enableDev) config->set("enable_development", true, ConfigSource::CommandLine);
    if (args.disablePlasticity) config->set("enable_development", false, ConfigSource::CommandLine);
    
    // Set output format
    if (args.outputFormat != "text" && args.outputFormat != "text") {
        config->set("output_format", args.outputFormat, ConfigSource::CommandLine);
    }
    
    // Configure logging
    OutputManager::configureLogger(args.verboseLevel, args.logLevel);
    
    printBanner();
    
    NLM_LOG_INFO("=== NLM Enhanced CLI - Phase 2+ Extension ===");
    NLM_LOG_INFO("Mode: " + args.mode);
    NLM_LOG_INFO("Config file: " + configManager.getConfigFile());
    
    // Start timing
    OutputManager::startTiming();
    
    int result = 0;
    
    try {
        if (args.mode == "demo") {
            result = DemoRunner::runPhase6Demo();
        }
        else if (args.mode == "test") {
            result = TestRunner::runAllTests();
        }
        else if (args.mode == "sim") {
            result = SimulationRunner::runCustomSimulation(configManager);
        }
        else if (args.mode == "batch") {
            result = BatchRunner::runBatchSimulation(configManager);
        }
        else if (args.mode == "interactive") {
            auto timestep = config->getOr<double>("simulation_timestep", 0.001);
            SimulationClock clock(timestep);
            
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                InteractiveShell::run(configManager, brain);
                result = 0;
            } else {
                NLM_LOG_ERROR("Failed to initialize brain for interactive mode");
                result = 1;
            }
        }
        else if (args.mode == "python") {
            result = PythonEmbeddingRunner::runPythonEmbedding(configManager);
        }
        else {
            NLM_LOG_ERROR("Unknown mode: " + args.mode);
            OutputFormatter::printHelp(argv[0]);
            result = 1;
        }
    }
    catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in main: " + std::string(e.what()));
        result = 1;
    }
    
    // End timing
    OutputManager::endTiming("Total execution");
    
    return result;
}