// NLM (熙然) - Neural Learning Machine
// Advanced Command Line Interface
//
// This provides enhanced command-line interface for controlling the NLM brain,
// including debugging tools, configuration management, and advanced commands.

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "environment/SimpleWorld.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <thread>
#include <csignal>
#include <sstream>

using namespace nlm;

// Global state for CLI
class NLMCLI {
public:
    NLMCLI() : brain(nullptr), config(nullptr), world(nullptr), running(false) {
        // Setup signal handlers
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
    }
    
    ~NLMCLI() {
        stop();
    }
    
    bool initialize(int argc, char** argv);
    void run();
    void stop() { running = false; }
    
    void printHelp() const;
    void printBrainStatus() const;
    void printConfig() const;
    void printMemoryStats() const;
    void printPerformanceStats() const;
    
    // Command handlers
    void handleHelp(const std::vector<std::string>& args);
    void handleLoad(const std::vector<std::string>& args);
    void handleSave(const std::vector<std::string>& args);
    void handleStep(const std::vector<std::string>& args);
    void handleRun(const std::vector<std::string>& args);
    void handleReset(const std::vector<std::string>& args);
    void handleStatus(const std::vector<std::string>& args);
    void handleInject(const std::vector<std::string>& args);
    void handleCheckpoint(const std::vector<std::string>& args);
    void handleDebug(const std::vector<std::string>& args);
    void handleStats(const std::vector<std::string>& args);
    void handleConfig(const std::vector<std::string>& args);
    void handleWorld(const std::vector<std::string>& args);
    void handleSensory(const std::vector<std::string>& args);
    void handleExit(const std::vector<std::string>& args);
    void handleQuit(const std::vector<std::string>& args);
    
    void executeCommand(const std::string& cmdLine);
    
private:
    static void signalHandler(int signal) {
        if (instance) {
            instance->stop();
        }
    }
    
    static NLMCLI* instance;
    
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    std::shared_ptr<SimpleWorld> world;
    bool running;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    std::map<std::string, std::function<void(const std::vector<std::string>&)>> commandHandlers;
};

NLMCLI* NLMCLI::instance = nullptr;

bool NLMCLI::initialize(int argc, char** argv) {
    instance = this;
    
    // Initialize logger with console output
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(true);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Advanced Command Line Interface ===");
    NLM_LOG_INFO("Type 'help' for available commands.");
    
    // Load configuration
    config = std::make_shared<Config>();
    
    // Default configuration
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    bool runMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--run") {
            runMode = true;
        } else if (arg == "--debug") {
            consoleLogger->setLevel(LogLevel::Debug);
        } else if (arg == "--quiet") {
            consoleLogger->setLevel(LogLevel::Error);
        }
    }
    
    // Load config from file
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration (no config file found: " + configFile + ")");
    }
    
    // Parse remaining command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            // Already handled
        } else if (arg == "--run") {
            // Already handled
        } else if (arg == "--debug") {
            // Already handled
        } else if (arg == "--quiet") {
            // Already handled
        } else if (arg.find('=') != std::string::npos) {
            // Key=value argument
            size_t pos = arg.find('=');
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            
            // Try to parse value as different types
            bool parsed = false;
            
            try {
                int intVal = std::stoi(value);
                config->set(key, intVal, ConfigSource::CommandLine);
                parsed = true;
            } catch (...) {}
            
            if (!parsed) {
                try {
                    double doubleVal = std::stod(value);
                    config->set(key, doubleVal, ConfigSource::CommandLine);
                    parsed = true;
                } catch (...) {}
            }
            
            if (!parsed) {
                if (value == "true" || value == "false") {
                    bool boolVal = (value == "true");
                    config->set(key, boolVal, ConfigSource::CommandLine);
                    parsed = true;
                }
            }
            
            if (!parsed) {
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain...");
    brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return false;
    }
    
    NLM_LOG_INFO("NLM Brain initialized successfully");
    
    // Initialize world
    world = std::make_shared<SimpleWorld>();
    world->initialize(16, 16);
    
    // Setup command handlers
    commandHandlers["help"] = [this](const std::vector<std::string>& args) { handleHelp(args); };
    commandHandlers["load"] = [this](const std::vector<std::string>& args) { handleLoad(args); };
    commandHandlers["save"] = [this](const std::vector<std::string>& args) { handleSave(args); };
    commandHandlers["step"] = [this](const std::vector<std::string>& args) { handleStep(args); };
    commandHandlers["run"] = [this](const std::vector<std::string>& args) { handleRun(args); };
    commandHandlers["reset"] = [this](const std::vector<std::string>& args) { handleReset(args); };
    commandHandlers["status"] = [this](const std::vector<std::string>& args) { handleStatus(args); };
    commandHandlers["inject"] = [this](const std::vector<std::string>& args) { handleInject(args); };
    commandHandlers["checkpoint"] = [this](const std::vector<std::string>& args) { handleCheckpoint(args); };
    commandHandlers["debug"] = [this](const std::vector<std::string>& args) { handleDebug(args); };
    commandHandlers["stats"] = [this](const std::vector<std::string>& args) { handleStats(args); };
    commandHandlers["config"] = [this](const std::vector<std::string>& args) { handleConfig(args); };
    commandHandlers["world"] = [this](const std::vector<std::string>& args) { handleWorld(args); };
    commandHandlers["sensory"] = [this](const std::vector<std::string>& args) { handleSensory(args); };
    commandHandlers["exit"] = [this](const std::vector<std::string>& args) { handleExit(args); };
    commandHandlers["quit"] = [this](const std::vector<std::string>& args) { handleQuit(args); };
    
    NLM_LOG_INFO("NLM Advanced CLI initialized successfully.");
    return true;
}

void NLMCLI::printHelp() const {
    NLM_LOG_INFO("=== Available Commands ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Basic Commands:");
    NLM_LOG_INFO("  help                    - Show this help message");
    NLM_LOG_INFO("  load <file>             - Load brain state from checkpoint file");
    NLM_LOG_INFO("  save <file>             - Save current brain state to checkpoint file");
    NLM_LOG_INFO("  step [count]            - Advance simulation by specified steps (default: 1)");
    NLM_LOG_INFO("  run <steps>             - Run simulation for specified steps");
    NLM_LOG_INFO("  reset                   - Reset brain to initial state");
    NLM_LOG_INFO("  status                  - Show current brain status");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Brain Control Commands:");
    NLM_LOG_INFO("  inject <neuron_type> <current> - Inject current into neurons of specified type");
    NLM_LOG_INFO("  checkpoint [file]       - Save checkpoint (with optional file path)");
    NLM_LOG_INFO("  debug                   - Toggle debug logging");
    NLM_LOG_INFO("  stats                   - Show performance statistics");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Commands:");
    NLM_LOG_INFO("  config [key] [value]    - Get/set configuration values");
    NLM_LOG_INFO("  world [width] [height]  - Set world dimensions");
    NLM_LOG_INFO("  sensory <mode>          - Set sensory processing mode");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Session Commands:");
    NLM_LOG_INFO("  exit/quit               - Exit the NLM CLI");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 6 Integration Commands:");
    NLM_LOG_INFO("  verify                  - Verify Phase 6 integration");
    NLM_LOG_INFO("  memory                  - Test memory systems");
    NLM_LOG_INFO("  neuromod                 - Test neuromodulation systems");
    NLM_LOG_INFO("");
}

void NLMCLI::printBrainStatus() const {
    NLM_LOG_INFO("=== Brain Status ===");
    NLM_LOG_INFO("Step: " + std::to_string(currentStep));
    NLM_LOG_INFO("Time: " + std::to_string(currentTime) + " ms");
    NLM_LOG_INFO("Neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    
    if (brain->getWorkingMemory()) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(brain->getWorkingMemory()->getActiveTraces()));
    }
    
    if (brain->getEpisodicMemory()) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(brain->getEpisodicMemory()->getEpisodeCount()));
    }
    
    if (brain->getDopamine()) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(brain->getDopamine()->getLevel()));
    }
}

void NLMCLI::printConfig() const {
    NLM_LOG_INFO("=== Configuration ===");
    std::vector<std::string> keys = config->getKeys();
    
    for (const auto& key : keys) {
        auto source = config->getKeys()[0];  // Get source for this key
        NLM_LOG_INFO("  " + key + " = " + config->summary());
    }
}

void NLMCLI::printMemoryStats() const {
    NLM_LOG_INFO("=== Memory Statistics ===");
    
    if (brain->getWorkingMemory()) {
        NLM_LOG_INFO("Working Memory:");
        NLM_LOG_INFO("  Capacity: " + std::to_string(brain->getWorkingMemory()->getCapacity()));
        NLM_LOG_INFO("  Active traces: " + std::to_string(brain->getWorkingMemory()->getActiveTraces()));
        NLM_LOG_INFO("  Decay rate: " + std::to_string(brain->getWorkingMemory()->getDecayRate()));
    }
    
    if (brain->getEpisodicMemory()) {
        NLM_LOG_INFO("Episodic Memory:");
        NLM_LOG_INFO("  Max episodes: " + std::to_string(brain->getEpisodicMemory()->getEpisodeCount()));
        NLM_LOG_INFO("  Replay enabled: " + std::string(brain->getEpisodicMemory()->isReplayEnabled() ? "true" : "false");
    }
    
    if (brain->getAssociativeMemory()) {
        NLM_LOG_INFO("Associative Memory:");
        NLM_LOG_INFO("  Association count: " + std::to_string(brain->getAssociativeMemory()->getAssociationCount()));
    }
}

void NLMCLI::printPerformanceStats() const {
    NLM_LOG_INFO("=== Performance Statistics ===");
    // TODO: Implement performance statistics collection
    NLM_LOG_INFO("Performance monitoring not yet implemented.");
}

void NLMCLI::handleHelp(const std::vector<std::string>& args) {
    printHelp();
}

void NLMCLI::handleLoad(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        NLM_LOG_ERROR("Usage: load <filepath>");
        return;
    }
    
    std::string filepath = args[0];
    
    NLM_LOG_INFO("Loading brain state from: " + filepath);
    
    if (brain->load(filepath)) {
        NLM_LOG_INFO("Brain state loaded successfully.");
    } else {
        NLM_LOG_ERROR("Failed to load brain state.");
    }
}

void NLMCLI::handleSave(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        NLM_LOG_ERROR("Usage: save <filepath>");
        return;
    }
    
    std::string filepath = args[0];
    
    NLM_LOG_INFO("Saving brain state to: " + filepath);
    
    if (brain->save(filepath)) {
        NLM_LOG_INFO("Brain state saved successfully.");
    } else {
        NLM_LOG_ERROR("Failed to save brain state.");
    }
}

void NLMCLI::handleStep(const std::vector<std::string>& args) {
    int steps = 1;
    if (args.size() >= 1) {
        try {
            steps = std::stoi(args[0]);
        } catch (...) {
            NLM_LOG_ERROR("Invalid step count: " + args[0]);
            return;
        }
    }
    
    if (steps < 0) {
        NLM_LOG_ERROR("Step count must be positive.");
        return;
    }
    
    NLM_LOG_INFO("Advancing simulation by " + std::to_string(steps) + " steps.");
    
    for (int i = 0; i < steps; ++i) {
        currentStep++;
        currentTime = currentStep * config->getOr<double>("simulation_timestep", 0.001);
        
        // Get sensory input from world
        if (world) {
            // For now, use simple observation
            SensoryPercept percept;
            percept.position = {world->getWidth() / 2, world->getHeight() / 2};
            percept.intensity = 0.5f;
            
            // Process sensory input
            if (brain) {
                // TODO: Add proper sensory processing interface
            }
        }
        
        brain->step(currentStep, currentTime);
        
        if ((currentStep % 100) == 0) {
            printBrainStatus();
        }
    }
    
    NLM_LOG_INFO("Advanced by " + std::to_string(steps) + " steps.");
}

void NLMCLI::handleRun(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        NLM_LOG_ERROR("Usage: run <steps>");
        return;
    }
    
    try {
        int steps = std::stoi(args[0]);
        if (steps < 0) {
            NLM_LOG_ERROR("Step count must be positive.");
            return;
        }
        
        NLM_LOG_INFO("Running simulation for " + std::to_string(steps) + " steps.");
        
        int originalStep = currentStep;
        for (int i = 0; i < steps; ++i) {
            currentStep++;
            currentTime = currentStep * config->getOr<double>("simulation_timestep", 0.001);
            
            brain->step(currentStep, currentTime);
            
            if ((currentStep % 1000) == 0) {
                NLM_LOG_INFO("Step " + std::to_string(currentStep) + 
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                           " | Reward: " + std::to_string(0.0f));  // TODO: Get actual reward
            }
        }
        
        NLM_LOG_INFO("Completed " + std::to_string(steps) + " steps.");
        
    } catch (...) {
        NLM_LOG_ERROR("Invalid step count: " + args[0]);
    }
}

void NLMCLI::handleReset(const std::vector<std::string>& args) {
    NLM_LOG_INFO("Resetting brain...");
    
    if (brain) {
        brain->reset();
        brain->initialize();
    }
    
    currentStep = 0;
    currentTime = 0.0;
    
    NLM_LOG_INFO("Brain reset completed.");
}

void NLMCLI::handleStatus(const std::vector<std::string>& args) {
    printBrainStatus();
}

void NLMCLI::handleInject(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        NLM_LOG_ERROR("Usage: inject <neuron_type> <current>");
        return;
    }
    
    // Parse neuron type
    NeuronType neuronType;
    if (args[0] == "sensory") {
        neuronType = NeuronType::Sensory;
    } else if (args[0] == "internal") {
        neuronType = NeuronType::Internal;
    } else if (args[0] == "motor") {
        neuronType = NeuronType::Motor;
    } else {
        NLM_LOG_ERROR("Invalid neuron type: " + args[0]);
        return;
    }
    
    // Parse current
    float current;
    try {
        current = std::stof(args[1]);
    } catch (...) {
        NLM_LOG_ERROR("Invalid current value: " + args[1]);
        return;
    }
    
    NLM_LOG_INFO("Injecting " + std::to_string(current) + " current into " + args[0] + " neurons.");
    
    if (brain) {
        brain->injectCurrentToNeurons(neuronType, current);
    }
}

void NLMCLI::handleCheckpoint(const std::vector<std::string>& args) {
    std::string filepath = "/tmp/nlm_checkpoint.bin";
    
    if (args.size() >= 1) {
        filepath = args[0];
    }
    
    NLM_LOG_INFO("Creating checkpoint: " + filepath);
    
    if (brain->save(filepath)) {
        NLM_LOG_INFO("Checkpoint created successfully.");
    } else {
        NLM_LOG_ERROR("Failed to create checkpoint.");
    }
}

void NLMCLI::handleDebug(const std::vector<std::string>& args) {
    // Toggle debug logging
    auto consoleLogger = std::static_pointer_cast<ConsoleLogger>(
        std::dynamic_pointer_cast<ILogger>(Logger::getGlobal().loggers[0]));
    
    if (consoleLogger) {
        LogLevel currentLevel = consoleLogger->getLevel();
        LogLevel newLevel = (currentLevel == LogLevel::Debug) ? LogLevel::Info : LogLevel::Debug;
        
        consoleLogger->setLevel(newLevel);
        NLM_LOG_INFO("Debug logging level: " + std::string(newLevel == LogLevel::Debug ? "enabled" : "disabled"));
    }
}

void NLMCLI::handleStats(const std::vector<std::string>& args) {
    printPerformanceStats();
    printMemoryStats();
}

void NLMCLI::handleConfig(const std::vector<std::string>& args) {
    if (args.size() == 0) {
        // List all config values
        NLM_LOG_INFO("=== Configuration Values ===");
        std::vector<std::string> keys = config->getKeys();
        for (const auto& key : keys) {
            // For simplicity, just show key exists
            NLM_LOG_INFO("  " + key);
        }
        return;
    }
    
    if (args.size() == 1) {
        // Get value for key
        std::string key = args[0];
        NLM_LOG_INFO("Value for key '" + key + "': " + config->summary());
        return;
    }
    
    if (args.size() >= 2) {
        // Set key=value
        std::string key = args[0];
        std::string value = args[1];
        
        // Try to parse as number
        try {
            int intVal = std::stoi(value);
            config->set(key, intVal, ConfigSource::Runtime);
            NLM_LOG_INFO("Set config '" + key + "' = " + value);
        } catch (...) {
            try {
                double doubleVal = std::stod(value);
                config->set(key, doubleVal, ConfigSource::Runtime);
                NLM_LOG_INFO("Set config '" + key + "' = " + value);
            } catch (...) {
                try {
                    bool boolVal = (value == "true" || value == "1");
                    config->set(key, boolVal, ConfigSource::Runtime);
                    NLM_LOG_INFO("Set config '" + key + "' = " + value);
                } catch (...) {
                    config->set(key, value, ConfigSource::Runtime);
                    NLM_LOG_INFO("Set config '" + key + "' = \"" + value + "\"");
                }
            }
        }
    }
}

void NLMCLI::handleWorld(const std::vector<std::string>& args) {
    if (!world) return;
    
    int width = world->getWidth();
    int height = world->getHeight();
    
    if (args.size() >= 1) {
        try {
            width = std::stoi(args[0]);
        } catch (...) {
            NLM_LOG_ERROR("Invalid world width: " + args[0]);
            return;
        }
    }
    
    if (args.size() >= 2) {
        try {
            height = std::stoi(args[1]);
        } catch (...) {
            NLM_LOG_ERROR("Invalid world height: " + args[1]);
            return;
        }
    }
    
    world->initialize(width, height);
    NLM_LOG_INFO("World dimensions set to: " + std::to_string(width) + " x " + std::to_string(height));
}

void NLMCLI::handleSensory(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        NLM_LOG_ERROR("Usage: sensory <mode>");
        return;
    }
    
    std::string mode = args[0];
    
    NLM_LOG_INFO("Sensory processing mode: " + mode);
    
    // TODO: Implement actual sensory mode changes
    // This would involve changing sensory system parameters
}

void NLMCLI::handleExit(const std::vector<std::string>& args) {
    NLM_LOG_INFO("Exiting NLM CLI...");
    stop();
}

void NLMCLI::handleQuit(const std::vector<std::string>& args) {
    handleExit(args);
}

void NLMCLI::executeCommand(const std::string& cmdLine) {
    // Parse command line
    std::vector<std::string> args;
    std::string arg;
    std::istringstream iss(cmdLine);
    
    while (iss >> arg) {
        args.push_back(arg);
    }
    
    if (args.empty()) {
        return;
    }
    
    std::string command = args[0];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    // Find command handler
    auto it = commandHandlers.find(command);
    if (it != commandHandlers.end()) {
        it->second(args);
    } else {
        NLM_LOG_ERROR("Unknown command: " + command);
        NLM_LOG_INFO("Type 'help' for available commands.");
    }
}

void NLMCLI::run() {
    running = true;
    currentStep = 0;
    currentTime = 0.0;
    
    NLM_LOG_INFO("=== NLM Advanced CLI ===");
    NLM_LOG_INFO("Type 'help' for available commands.");
    NLM_LOG_INFO("");
    
    while (running) {
        // Display prompt
        std::cout << ">>> ";
        std::cout.flush();
        
        // Read command
        std::string line;
        std::getline(std::cin, line);
        
        if (std::cin.eof()) {
            // EOF received (Ctrl+D)
            handleExit({});
            break;
        }
        
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Execute command
        executeCommand(line);
    }
    
    NLM_LOG_INFO("NLM CLI terminated.");
}

int main(int argc, char** argv) {
    // Create and initialize CLI
    NLMCLI cli;
    
    if (!cli.initialize(argc, argv)) {
        std::cerr << "Failed to initialize NLM CLI." << std::endl;
        return 1;
    }
    
    // Run CLI
    cli.run();
    
    return 0;
}