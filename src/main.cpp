// NLM Main Entry Point
// Multi-Purpose Neuromorphic Neural Network Interface
// Provides command-line tools for brain simulation, visualization, and experiment control

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <chrono>
#include <filesystem>

// Core systems
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/experiments/Phase6IntegratedExperiment.hpp"
#include "src/experiments/Phase5Experiment.hpp"
#include "src/experiments/Phase4Experiment.hpp"

namespace nlm {

class NLMCLI {
public:
    enum class Command {
        Run,
        Visualize,
        Config,
        Test,
        Help,
        Version
    };

    NLMCLI();
    ~NLMCLI() = default;

    // Parse command line arguments
    bool parseArguments(int argc, char* argv[]);

    // Execute parsed command
    bool executeCommand();

    // Print help information
    static void printHelp();

    // Print version information
    static void printVersion();

private:
    Command command_;
    std::string configFile_;
    std::string outputFile_;
    std::vector<std::string> args_;

    // Command implementations
    bool runCommand();
    bool visualizeCommand();
    bool configCommand();
    bool testCommand();
    
    // Configuration handling
    std::shared_ptr<Config> loadConfig(const std::string& configFile = "");
    bool saveConfig(const std::string& configFile, const std::shared_ptr<Config>& config);
    
    // Brain control
    bool createAndRunBrain(const std::shared_ptr<Config>& config);
    bool runIntegrationTest(const std::shared_ptr<Config>& config);
    
    // Visualization
    bool generateVisualization(const std::string& outputPath);
    
    // Testing
    bool runTests();
};

// Default configuration template
std::shared_ptr<Config> createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Brain configuration
    config->set("neuron_count", 1000);
    config->set("region_count", 4);
    config->set("connection_probability", 0.1f);
    config->set("simulation_timestep", 0.001);
    
    // Memory systems
    config->set("working_memory_capacity", 100);
    config->set("episodic_memory_max_episodes", 1000);
    config->set("associative_memory_size", 500);
    
    // Neuromodulation
    config->set("dopamine_baseline", 0.1f);
    config->set("curiosity_drive", 0.5f);
    config->set("novelty_threshold", 0.5f);
    
    // Plasticity
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    // Development
    config->set("enable_development", true);
    config->set("development_rate", 0.01f);
    
    // Prediction
    config->set("prediction_window", 100);
    config->set("prediction_threshold", 0.5f);
    
    // Cognition
    config->set("planning_depth", 3);
    config->set("concept_formation_threshold", 0.7f);
    config->set("attention_inhibition_strength", 0.5f);
    config->set("attention_excitation_strength", 1.5f);
    
    // Checkpointing
    config->set("checkpoint_dir", "./checkpoints");
    config->set("checkpoint_interval", 1000);
    config->set("max_checkpoints", 10);
    
    // Replay
    config->set("replay_interval", 100);
    config->set("consolidation_interval", 1000);
    
    return config;
}

// Additional command implementations (simplified)
NLMCLI::NLMCLI() : command_(Command::Help) {}

bool NLMCLI::parseArguments(int argc, char* argv[]) {
    if (argc < 2) {
        return false;
    }
    
    std::string cmd = argv[1];
    
    if (cmd == "run") {
        command_ = Command::Run;
        if (argc > 2) configFile_ = argv[2];
    } else if (cmd == "visualize") {
        command_ = Command::Visualize;
        if (argc > 2) outputFile_ = argv[2];
    } else if (cmd == "config") {
        command_ = Command::Config;
        if (argc > 2) configFile_ = argv[2];
    } else if (cmd == "test") {
        command_ = Command::Test;
    } else if (cmd == "help") {
        command_ = Command::Help;
    } else if (cmd == "version") {
        command_ = Command::Version;
    } else {
        return false;
    }
    
    return true;
}

bool NLMCLI::executeCommand() {
    switch (command_) {
        case Command::Run: return runCommand();
        case Command::Visualize: return visualizeCommand();
        case Command::Config: return configCommand();
        case Command::Test: return testCommand();
        case Command::Help: printHelp(); return true;
        case Command::Version: printVersion(); return true;
    }
    return false;
}

// Enhanced configuration creation with validation
class AdvancedConfig {
public:
    struct ValidatedConfig {
        size_t neuronCount;
        float connectionProbability;
        float timestep;
        std::string checkpointDir;
        bool enableDevelopment;
        bool enableReplay;
        bool enableCheckpointing;
        size_t workingMemoryCapacity;
        float dopamineBaseline;
        float curiosityDrive;
        
        // Validation results
        bool isValid() const;
        std::string getValidationError() const;
    };
    
    static ValidatedConfig createValidatedConfig(const std::shared_ptr<Config>& config);
    static ValidatedConfig getAdvancedDefaultConfig();
    
    static void validateConfig(const ValidatedConfig& config);
    static void printConfigSummary(const ValidatedConfig& config);
};

// Forward declaration for the simplified version
struct NLMCLI::ValidatedConfig {
    size_t neuronCount;
    float connectionProbability;
    float timestep;
    std::string checkpointDir;
    bool enableDevelopment;
    bool enableReplay;
    bool enableCheckpointing;
    size_t workingMemoryCapacity;
    float dopamineBaseline;
    float curiosityDrive;
    
    ValidatedConfig() : neuronCount(1000), connectionProbability(0.1f), timestep(0.001),
                       checkpointDir("./checkpoints"), enableDevelopment(true),
                       enableReplay(true), enableCheckpointing(true),
                       workingMemoryCapacity(100), dopamineBaseline(0.1f), curiosityDrive(0.5f) {}
    
    bool isValid() const {
        return neuronCount > 0 && neuronCount <= 1000000 &&
               connectionProbability >= 0.0f && connectionProbability <= 1.0f &&
               timestep > 0.0f && timestep <= 0.1f &&
               workingMemoryCapacity > 0;
    }
    
    std::string getValidationError() const {
        if (neuronCount == 0 || neuronCount > 1000000)
            return "Neuron count must be between 1 and 1,000,000";
        if (connectionProbability < 0.0f || connectionProbability > 1.0f)
            return "Connection probability must be between 0.0 and 1.0";
        if (timestep <= 0.0f || timestep > 0.1f)
            return "Timestep must be between 0.0001 and 0.1";
        if (workingMemoryCapacity == 0)
            return "Working memory capacity must be positive";
        return "";
    }
};

ValidatedConfig AdvancedConfig::createValidatedConfig(const std::shared_ptr<Config>& config) {
    ValidatedConfig validated;
    
    validated.neuronCount = config->getOr<size_t>("neuron_count", 1000);
    validated.connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    validated.timestep = config->getOr<float>("simulation_timestep", 0.001f);
    validated.checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    validated.enableDevelopment = config->getOr<bool>("enable_development", true);
    validated.enableReplay = config->getOr<bool>("enable_replay", true);
    validated.enableCheckpointing = config->getOr<bool>("enable_checkpointing", true);
    validated.workingMemoryCapacity = config->getOr<size_t>("working_memory_capacity", 100);
    validated.dopamineBaseline = config->getOr<float>("dopamine_baseline", 0.1f);
    validated.curiosityDrive = config->getOr<float>("curiosity_drive", 0.5f);
    
    return validated;
}

ValidatedConfig AdvancedConfig::getAdvancedDefaultConfig() {
    ValidatedConfig config;
    // Advanced defaults with better ranges
    config.neuronCount = 5000;
    config.connectionProbability = 0.15f;
    config.timestep = 0.001f;
    config.checkpointDir = "./checkpoints_advanced";
    config.enableDevelopment = true;
    config.enableReplay = true;
    config.enableCheckpointing = true;
    config.workingMemoryCapacity = 500;
    config.dopamineBaseline = 0.05f;
    config.curiosityDrive = 0.3f;
    return config;
}

void AdvancedConfig::validateConfig(const ValidatedConfig& config) {
    if (!config.isValid()) {
        throw std::runtime_error(config.getValidationError());
    }
}

void AdvancedConfig::printConfigSummary(const ValidatedConfig& config) {
    std::cout << "=== NLM Configuration Summary ===" << std::endl;
    std::cout << "Neurons: " << config.neuronCount << std::endl;
    std::cout << "Connection Probability: " << config.connectionProbability << std::endl;
    std::cout << "Simulation Timestep: " << config.timestep << " ms" << std::endl;
    std::cout << "Working Memory Capacity: " << config.workingMemoryCapacity << std::endl;
    std::cout << "Dopamine Baseline: " << config.dopamineBaseline << std::endl;
    std::cout << "Curiosity Drive: " << config.curiosityDrive << std::endl;
    std::cout << "Development Enabled: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
    std::cout << "Replay Enabled: " << (config.enableReplay ? "Yes" : "No") << std::endl;
    std::cout << "Checkpointing Enabled: " << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
    std::cout << "Checkpoint Directory: " << config.checkpointDir << std::endl;
    std::cout << "===============================" << std::endl;
}

// Core command implementation for run
bool NLMCLI::runCommand() {
    try {
        std::cout << "=== NLM Neural Network Simulation ===" << std::endl;
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Load configuration
        std::shared_ptr<Config> config;
        if (!configFile_.empty()) {
            config = std::make_shared<Config>();
            if (!config->loadFromFile(configFile_)) {
                std::cerr << "Failed to load configuration from " << configFile_ << std::endl;
                return false;
            }
        } else {
            config = createDefaultConfig();
        }
        
        // Create advanced configuration with validation
        auto validatedConfig = AdvancedConfig::createValidatedConfig(config);
        AdvancedConfig::validateConfig(validatedConfig);
        AdvancedConfig::printConfigSummary(validatedConfig);
        
        // Create and run brain
        if (!createAndRunBrain(config)) {
            std::cerr << "Brain execution failed" << std::endl;
            return false;
        }
        
        std::cout << "=== Simulation Complete ===" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during execution: " << e.what() << std::endl;
        return false;
    }
}

// Simplified brain creation and execution
bool NLMCLI::createAndRunBrain(const std::shared_ptr<Config>& config) {
    std::cout << "Creating NLM Brain with configuration..." << std::endl;
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        return false;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    
    // Run simulation
    const uint64_t maxSteps = config->getOr<uint64_t>("max_steps", 1000);
    const uint64_t stepsPerPrint = config->getOr<uint64_t>("print_interval", 100);
    
    std::cout << "Running simulation for " << maxSteps << " steps..." << std::endl;
    
    for (uint64_t step = 0; step < maxSteps; ++step) {
        brain->step(step);
        
        if (step % stepsPerPrint == 0) {
            std::cout << "Step " << step << "/" << maxSteps << std::endl;
        }
    }
    
    std::cout << "Simulation completed" << std::endl;
    brain->logStatus();
    
    return true;
}

int main(int argc, char* argv[]) {
    NLMCLI cli;
    
    if (!cli.parseArguments(argc, argv)) {
        std::cerr << "Invalid command line arguments" << std::endl;
        std::cout << "Usage: nlm <command> [options]" << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  run [config]  - Run neural network simulation" << std::endl;
        std::cout << "  visualize [output] - Generate visualization" << std::endl;
        std::cout << "  config [file] - Show or modify configuration" << std::endl;
        std::cout << "  test - Run system tests" << std::endl;
        std::cout << "  help - Show this help message" << std::endl;
        std::cout << "  version - Show version information" << std::endl;
        return 1;
    }
    
    if (!cli.executeCommand()) {
        std::cerr << "Command execution failed" << std::endl;
        return 1;
    }
    
    return 0;
}

} // namespace nlm
