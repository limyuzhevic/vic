#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <map>
#include <vector>
#include <algorithm>

namespace nlm {

class CommandLineInterface {
public:
    CommandLineInterface() : config_(std::make_shared<Config>()) {
        setupCommands();
    }
    
    void run(int argc, char** argv) {
        if (argc < 2) {
            printHelp();
            return;
        }
        
        std::string command = argv[1];
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);
        
        auto it = commands_.find(command);
        if (it != commands_.end()) {
            try {
                (this->*it->second)(argc, argv);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Command failed: ") + e.what());
                return;
            }
        } else {
            NLM_LOG_ERROR("Unknown command: " + command);
            printHelp();
        }
    }
    
private:
    struct Command {
        std::string name;
        std::string description;
        void (CommandLineInterface::*handler)(int, char**);
    };
    
    std::shared_ptr<Config> config_;
    std::vector<Command> commands_;
    
    void setupCommands() {
        // Core simulation commands
        commands_.push_back({"run", "Run a simulation", &CommandLineInterface::handleRunCommand});
        commands_.push_back({"config", "Configuration management", &CommandLineInterface::handleConfigCommand});
        commands_.push_back({"checkpoint", "Checkpoint management", &CommandLineInterface::handleCheckpointCommand});
        commands_.push_back({"stats", "Display brain statistics", &CommandLineInterface::handleStatsCommand});
        
        // Brain management commands
        commands_.push_back({"brain", "Brain management", &CommandLineInterface::handleBrainCommand});
        commands_.push_back({"reset", "Reset brain state", &CommandLineInterface::handleResetCommand});
        commands_.push_back({"save", "Save brain state", &CommandLineInterface::handleSaveCommand});
        commands_.push_back({"load", "Load brain state", &CommandLineInterface::handleLoadCommand});
        
        // Experiment commands
        commands_.push_back({"experiment", "Run experiment", &CommandLineInterface::handleExperimentCommand});
        commands_.push_back({"replay", "Replay episodes", &CommandLineInterface::handleReplayCommand});
        
        // Monitoring and analysis
        commands_.push_back({"monitor", "Monitor simulation", &CommandLineInterface::handleMonitorCommand});
        commands_.push_back({"analyze", "Analyze results", &CommandLineInterface::handleAnalyzeCommand});
        
        // System commands
        commands_.push_back({"help", "Show help", &CommandLineInterface::handleHelpCommand});
        commands_.push_back({"exit", "Exit program", &CommandLineInterface::handleExitCommand});
    }
    
    void handleRunCommand(int argc, char** argv) {
        NLM_LOG_INFO("=== NLM Command Line Interface - Running Simulation ===");
        
        // Parse run command options
        int steps = 1000;
        double timestep = 0.001;
        std::string configFile = "configs/default.cfg";
        bool usePython = false;
        
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--steps" && i + 1 < argc) {
                steps = std::stoi(argv[++i]);
            } else if (arg == "--timestep" && i + 1 < argc) {
                timestep = std::stod(argv[++i]);
            } else if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            } else if (arg == "--python") {
                usePython = true;
            }
        }
        
        // Load configuration
        if (config_->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        }
        
        // Set simulation parameters
        config_->set("simulation_timestep", timestep, ConfigSource::Default);
        config_->set("neuron_count", 1000, ConfigSource::Default);
        
        if (usePython) {
            runPythonSimulation(steps, timestep);
        } else {
            runCppSimulation(steps, timestep);
        }
        
        NLM_LOG_INFO("Simulation completed successfully");
    }
    
    void runCppSimulation(int steps, double timestep) {
        NLM_LOG_INFO("Running C++ simulation for " + std::to_string(steps) + " steps");
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config_);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        brain->logStatus();
        
        // Run simulation
        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * timestep);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Final statistics
        brain->logStatus();
    }
    
    void runPythonSimulation(int steps, double timestep) {
        NLM_LOG_INFO("Running Python simulation via pybind11");
        
        // For now, just log that Python simulation is available
        NLM_LOG_INFO("Python simulation requires pybind11 bindings to be compiled");
        NLM_LOG_INFO("Use 'python run_python.py' for Python-based simulations");
        
        // TODO: Implement actual Python simulation interface
        // This would require pybind11 integration for command-line execution
        NLM_LOG_INFO("Python integration feature under development");
    }
    
    void handleConfigCommand(int argc, char** argv) {
        NLM_LOG_INFO("=== Configuration Management ===");
        
        if (argc < 3) {
            printConfigHelp();
            return;
        }
        
        std::string action = argv[2];
        std::transform(action.begin(), action.end(), action.begin(), ::tolower);
        
        if (action == "list") {
            handleConfigListCommand();
        } else if (action == "get" && argc >= 4) {
            handleConfigGetCommand(argv[3]);
        } else if (action == "set" && argc >= 5) {
            handleConfigSetCommand(argv[3], argv[4]);
        } else if (action == "load" && argc >= 4) {
            handleConfigLoadCommand(argv[3]);
        } else if (action == "save" && argc >= 4) {
            handleConfigSaveCommand(argv[3]);
        } else {
            printConfigHelp();
        }
    }
    
    void handleConfigListCommand() {
        NLM_LOG_INFO("Configuration parameters:");
        const auto& keys = config_->getKeys();
        for (const auto& key : keys) {
            if (config_->has(key)) {
                auto value = config_->getAny(key);
                NLM_LOG_INFO("  " + key + " = " + std::to_string(std::get<double>(value)));
            }
        }
    }
    
    void handleConfigGetCommand(const std::string& key) {
        if (config_->has(key)) {
            auto value = config_->getAny(key);
            NLM_LOG_INFO("Value of '" + key + "': " + std::to_string(std::get<double>(value)));
        } else {
            NLM_LOG_ERROR("Configuration parameter not found: " + key);
        }
    }
    
    void handleConfigSetCommand(const std::string& key, const std::string& value) {
        try {
            float floatValue = std::stof(value);
            config_->set(key, floatValue, ConfigSource::User);
            NLM_LOG_INFO("Set " + key + " = " + value);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Invalid value: ") + e.what());
        }
    }
    
    void handleConfigLoadCommand(const std::string& filename) {
        if (config_->loadFromFile(filename)) {
            NLM_LOG_INFO("Loaded configuration from: " + filename);
        } else {
            NLM_LOG_ERROR("Failed to load configuration from: " + filename);
        }
    }
    
    void handleConfigSaveCommand(const std::string& filename) {
        // Save configuration to file
        NLM_LOG_INFO("Saving configuration to: " + filename);
        NLM_LOG_INFO("Configuration save functionality to be implemented");
        // TODO: Implement configuration save functionality
    }
    
    void handleCheckpointCommand(int argc, char** argv) {
        NLM_LOG_INFO("=== Checkpoint Management ===");
        
        if (argc < 3) {
            printCheckpointHelp();
            return;
        }
        
        std::string action = argv[2];
        std::transform(action.begin(), action.end(), action.begin(), ::tolower);
        
        if (action == "save" && argc >= 4) {
            handleCheckpointSaveCommand(argv[3]);
        } else if (action == "load" && argc >= 4) {
            handleCheckpointLoadCommand(argv[3]);
        } else if (action == "list") {
            handleCheckpointListCommand();
        } else {
            printCheckpointHelp();
        }
    }
    
    void handleCheckpointSaveCommand(const std::string& filename) {
        NLM_LOG_INFO("Saving brain state to: " + filename);
        NLM_LOG_INFO("Checkpoint save functionality to be implemented");
        // TODO: Implement checkpoint save functionality
    }
    
    void handleCheckpointLoadCommand(const std::string& filename) {
        NLM_LOG_INFO("Loading brain state from: " + filename);
        NLM_LOG_INFO("Checkpoint load functionality to be implemented");
        // TODO: Implement checkpoint load functionality
    }
    
    void handleCheckpointListCommand() {
        NLM_LOG_INFO("Checkpoint files: (None found)");
        NLM_LOG_INFO("Checkpoint listing functionality to be implemented");
    }
    
    void handleBrainCommand(int argc, char** argv) {
        NLM_LOG_INFO("=== Brain Management ===");
        
        if (argc < 3) {
            printBrainHelp();
            return;
        }
        
        std::string action = argv[2];
        std::transform(action.begin(), action.end(), action.begin(), ::tolower);
        
        if (action == "create") {
            handleBrainCreateCommand();
        } else if (action == "info") {
            handleBrainInfoCommand();
        } else {
            printBrainHelp();
        }
    }
    
    void handleBrainCreateCommand() {
        NLM_LOG_INFO("Creating new brain configuration");
        config_->set("random_seed", 42, ConfigSource::User);
        config_->set("neuron_count", 1000, ConfigSource::User);
        config_->set("region_count", 2, ConfigSource::User);
        config_->set("connection_probability", 0.1, ConfigSource::User);
        NLM_LOG_INFO("Created brain with 1000 neurons and 2 regions");
    }
    
    void handleBrainInfoCommand() {
        NLM_LOG_INFO("Brain information:
  Neurons: 1000 (configured)
  Regions: 2 (configured)
  Connections: Variable (based on density)
  Plasticity: STDP, Hebbian, Structural
  Memory: Working, Episodic, Associative
  Neuromodulation: Dopamine, Curiosity, Novelty
  Prediction: Integrated
  Development: Stage-based plasticity
        );
    }
    
    void handleStatsCommand(int argc, char** argv) {
        NLM_LOG_INFO("=== Brain Statistics ===");
        NLM_LOG_INFO("Statistics display functionality to be implemented");
        NLM_LOG_INFO("Run simulation first, then use 'stats' to see detailed statistics");
    }
    
    void handleResetCommand(int argc, char** argv) {
        NLM_LOG_INFO("Resetting brain state");
        NLM_LOG_INFO("Brain reset functionality to be implemented");
    }
    
    void handleSaveCommand(int argc, char** argv) {
        if (argc < 3) {
            NLM_LOG_ERROR("Please specify filename for saving");
            return;
        }
        handleCheckpointSaveCommand(argv[2]);
    }
    
    void handleLoadCommand(int argc, char** argv) {
        if (argc < 3) {
            NLM_LOG_ERROR("Please specify filename for loading");
            return;
        }
        handleCheckpointLoadCommand(argv[2]);
    }
    
    void handleExperimentCommand(int argc, char** argv) {
        NLM_LOG_INFO("=== Experiment Management ===");
        NLM_LOG_INFO("Experiment functionality to be implemented");
        NLM_LOG_INFO("Available experiments: connectivity, plasticity, stdp, learning");
    }
    
    void handleReplayCommand(int argc, char** argv) {
        NLM_LOG_INFO("=== Memory Replay ===");
        NLM_LOG_INFO("Replay functionality to be implemented");
        NLM_LOG_INFO("Replays important episodes from episodic memory");
    }
    
    void handleMonitorCommand(int argc, char** argv) {
        NLM_LOG_INFO("=== Simulation Monitor ===");
        NLM_LOG_INFO("Monitoring functionality to be implemented");
        NLM_LOG_INFO("Real-time monitoring of brain activity");
    }
    
    void handleAnalyzeCommand(int argc, char** argv) {
        NLM_LOG_INFO("=== Result Analysis ===");
        NLM_LOG_INFO("Analysis functionality to be implemented");
        NLM_LOG_INFO("Analyze simulation results and learning patterns");
    }
    
    void printHelp() {
        NLM_LOG_INFO("=== NLM Command Line Interface ===");
        NLM_LOG_INFO("Available commands:");
        for (const auto& cmd : commands_) {
            NLM_LOG_INFO("  " + cmd.name + " - " + cmd.description);
        }
        NLM_LOG_INFO("Use 'help <command>' for detailed information about a specific command");
    }
    
    void printConfigHelp() {
        NLM_LOG_INFO("Configuration commands:");
        NLM_LOG_INFO("  config list    - List all configuration parameters");
        NLM_LOG_INFO("  config get <key> - Get value of configuration parameter");
        NLM_LOG_INFO("  config set <key> <value> - Set configuration parameter");
        NLM_LOG_INFO("  config load <file> - Load configuration from file");
        NLM_LOG_INFO("  config save <file> - Save configuration to file");
    }
    
    void printCheckpointHelp() {
        NLM_LOG_INFO("Checkpoint commands:");
        NLM_LOG_INFO("  checkpoint save <file> - Save brain state to checkpoint");
        NLM_LOG_INFO("  checkpoint load <file> - Load brain state from checkpoint");
        NLM_LOG_INFO("  checkpoint list    - List available checkpoints");
    }
    
    void printBrainHelp() {
        NLM_LOG_INFO("Brain commands:");
        NLM_LOG_INFO("  brain create  - Create new brain configuration");
        NLM_LOG_INFO("  brain info    - Display brain information");
    }
    
    void handleHelpCommand(int argc, char** argv) {
        if (argc < 3) {
            printHelp();
        } else {
            std::string command = argv[2];
            std::transform(command.begin(), command.end(), command.begin(), ::tolower);
            
            if (command == "config") {
                printConfigHelp();
            } else if (command == "checkpoint") {
                printCheckpointHelp();
            } else if (command == "brain") {
                printBrainHelp();
            } else {
                NLM_LOG_ERROR("Unknown command for help: " + command);
            }
        }
    }
    
    void handleExitCommand(int argc, char** argv) {
        NLM_LOG_INFO("Exiting NLM Command Line Interface");
        std::exit(0);
    }
};

void runCommandLineInterface(int argc, char** argv) {
    CommandLineInterface cli;
    cli.run(argc, argv);
}

} // namespace nlm