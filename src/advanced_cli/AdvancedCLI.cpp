// Advanced Command Line Interface for Power Users
// This module provides enhanced command-line functionality for advanced users

#include "src/core/Config/Config.hpp"
#include "src/brain/CommandManager.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>

namespace nlm {

class AdvancedCLI {
public:
    AdvancedCLI() {
        setupCommands();
        setupAliases();
    }
    
    void run() {
        printBanner();
        
        std::string line;
        std::cout << "\nNLM Advanced CLI Ready. Type 'help' for commands or 'exit' to quit.\n" << std::endl;
        
        while (true) {
            std::cout << "nlm> ";
            std::getline(std::cin, line);
            
            if (line.empty()) continue;
            
            // Check for exit
            if (line == "exit" || line == "quit") {
                std::cout << "Exiting NLM..." << std::endl;
                break;
            }
            
            // Check for help
            if (line == "help" || line == "?") {
                showHelp();
                continue;
            }
            
            // Process command
            processCommand(line);
        }
    }
    
private:
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::function<void(const std::vector<std::string>&)> handler;
        std::string category;
        bool requiresBrain;
    };
    
    std::vector<Command> commands;
    std::map<std::string, std::string> aliases;
    std::shared_ptr<nlm::Brain> currentBrain;
    std::shared_ptr<nlm::Config> currentConfig;
    std::shared_ptr<CommandManager> commandManager;
    
    void setupCommands() {
        // Core commands
        commands.push_back({
            "create", "Create a new brain with configuration",
            "create [config_file]",
            [this](const std::vector<std::string>& args) { cmdCreate(args); },
            "Brain Control", true
        });
        
        commands.push_back({
            "load", "Load brain state from file",
            "load <filepath>",
            [this](const std::vector<std::string>& args) { cmdLoad(args); },
            "Brain Control", true
        });
        
        commands.push_back({
            "save", "Save brain state to file",
            "save <filepath>",
            [this](const std::vector<std::string>& args) { cmdSave(args); },
            "Brain Control", true
        });
        
        commands.push_back({
            "reset", "Reset brain state",
            "reset",
            [this](const std::vector<std::string>& args) { cmdReset(args); },
            "Brain Control", true
        });
        
        commands.push_back({
            "step", "Run simulation steps",
            "step <count> [timestep]",
            [this](const std::vector<std::string>& args) { cmdStep(args); },
            "Brain Control", true
        });
        
        commands.push_back({
            "inject", "Inject current into neurons",
            "inject <type> <amount> [neuron_id]",
            [this](const std::vector<std::string>& args) { cmdInject(args); },
            "Sensory/Motor", true
        });
        
        commands.push_back({
            "status", "Show brain status and statistics",
            "status",
            [this](const std::vector<std::string>& args) { cmdStatus(args); },
            "Brain Control", true
        });
        
        // Configuration commands
        commands.push_back({
            "config", "Manage configuration",
            "config <get|set|load|save> [key] [value]",
            [this](const std::vector<std::string>& args) { cmdConfig(args); },
            "Configuration", false
        });
        
        commands.push_back({
            "validate", "Validate current configuration",
            "validate",
            [this](const std::vector<std::string>& args) { cmdValidate(args); },
            "Configuration", false
        });
        
        commands.push_back({
            "validate_all", "Validate all configuration keys",
            "validate_all",
            [this](const std::vector<std::string>& args) { cmdValidateAll(args); },
            "Configuration", false
        });
        
        // Command management commands
        commands.push_back({
            "command", "Manage commands and history",
            "command <history|undo|redo|clear> [command_name]",
            [this](const std::vector<std::string>& args) { cmdCommand(args); },
            "Command Management", true
        });
        
        commands.push_back({
            "profile", "Start/stop profiling",
            "profile <start|stop|save> [filename]",
            [this](const std::vector<std::string>& args) { cmdProfile(args); },
            "Performance", false
        });
        
        // Experiment commands
        commands.push_back({
            "experiment", "Run predefined experiments",
            "experiment <phase2|phase3|phase4|phase5|phase6> [iterations]",
            [this](const std::vector<std::string>& args) { cmdExperiment(args); },
            "Experiments", true
        });
        
        commands.push_back({
            "benchmark", "Run performance benchmarks",
            "benchmark <duration> [output_file]",
            [this](const std::vector<std::string>& args) { cmdBenchmark(args); },
            "Experiments", true
        });
        
        // Utility commands
        commands.push_back({
            "help", "Show help information",
            "help [command]",
            [this](const std::vector<std::string>& args) { cmdHelp(args); },
            "Utilities", false
        });
        
        commands.push_back({
            "alias", "Manage command aliases",
            "alias <list|add|remove> [alias] [command]",
            [this](const std::vector<std::string>& args) { cmdAlias(args); },
            "Utilities", false
        });
        
        commands.push_back({
            "exit", "Exit the application",
            "exit",
            [this](const std::vector<std::string>& args) { cmdExit(args); },
            "Utilities", false
        });
    }
    
    void setupAliases() {
        aliases["?"] = "help";
        aliases["ls"] = "status";
        aliases["st"] = "step";
        aliases["reset"] = "reset";
        aliases["run"] = "step";
        aliases["start"] = "create";
        aliases["stop"] = "reset";
        aliases["show"] = "status";
        aliases["stats"] = "status";
    }
    
    void printBanner() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║     NLM Advanced Command Line Interface v2.0                  ║" << std::endl;
        std::cout << "║     Enhanced version with full control and monitoring          ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
    }
    
    void showHelp() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                           AVAILABLE COMMANDS                    ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        
        std::string currentCategory = "";
        for (const auto& cmd : commands) {
            if (currentCategory != cmd.category) {
                currentCategory = cmd.category;
                std::cout << std::endl << "📂 " << currentCategory << ":" << std::endl;
            }
            
            std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
            std::cout << "    Usage: " << cmd.usage << std::endl;
        }
        
        std::cout << std::endl << "Aliases: " << std::endl;
        for (const auto& alias : aliases) {
            std::cout << "  " << alias.first << " -> " << alias.second << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    void processCommand(const std::string& input) {
        // Check for aliases
        std::string cmd = input;
        size_t space = cmd.find(' ');
        std::string cmdName;
        std::vector<std::string> args;
        
        if (space != std::string::npos) {
            cmdName = cmd.substr(0, space);
            args = split(cmd.substr(space + 1));
        } else {
            cmdName = cmd;
        }
        
        // Check alias
        if (aliases.find(cmdName) != aliases.end()) {
            cmdName = aliases[cmdName];
        }
        
        // Find and execute command
        auto it = std::find_if(commands.begin(), commands.end(),
                              [&cmdName](const Command& c) { return c.name == cmdName; });n        
        if (it != commands.end()) {
            try {
                it->handler(args);
            } catch (const std::exception& e) {
                std::cout << "❌ Error: " << e.what() << std::endl;
            }
        } else {
            std::cout << "❌ Unknown command: " << cmdName << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
    
    std::vector<std::string> split(const std::string& input) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(input);
        
        while (tokenStream >> token) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    void ensureBrainExists() {
        if (!currentBrain) {
            std::cout << "❌ No brain created. Use 'create' command first." << std::endl;
            throw std::runtime_error("No brain available");
        }
        
        if (!commandManager) {
            commandManager = std::make_shared<CommandManager>();
            // Add command manager to brain if available
        }
    }
    
    void ensureConfigExists() {
        if (!currentConfig) {
            currentConfig = std::make_shared<nlm::Config>();
            nlm::ConfigManager::getInstance().initializeDefaultConfig();
        }
    }
    
    // Brain Control Commands
    void cmdCreate(const std::vector<std::string>& args) {
        ensureConfigExists();
        
        if (args.empty()) {
            // Create with default config
            currentBrain = std::make_shared<nlm::Brain>(currentConfig);
            commandManager = std::make_shared<CommandManager>();
            
            if (currentBrain->initialize()) {
                std::cout << "✅ Brain created and initialized with default configuration." << std::endl;
            } else {
                std::cout << "❌ Failed to initialize brain." << std::endl;
            }
        } else {
            // Load config from file
            std::string configFile = args[0];
            if (currentConfig->loadFromFile(configFile)) {
                currentBrain = std::make_shared<nlm::Brain>(currentConfig);
                commandManager = std::make_shared<CommandManager>();
                
                if (currentBrain->initialize()) {
                    std::cout << "✅ Brain created and initialized with configuration from: " << configFile << std::endl;
                } else {
                    std::cout << "❌ Failed to initialize brain." << std::endl;
                }
            } else {
                std::cout << "❌ Failed to load configuration from: " << configFile << std::endl;
            }
        }
    }
    
    void cmdLoad(const std::vector<std::string>& args) {
        ensureBrainExists();
        
        if (args.empty()) {
            std::cout << "❌ Missing filepath. Usage: load <filepath>" << std::endl;
            return;
        }
        
        std::string filepath = args[0];
        if (currentBrain->load(filepath)) {
            std::cout << "✅ Brain state loaded from: " << filepath << std::endl;
        } else {
            std::cout << "❌ Failed to load brain state from: " << filepath << std::endl;
        }
    }
    
    void cmdSave(const std::vector<std::string>& args) {
        ensureBrainExists();
        
        if (args.empty()) {
            std::cout << "❌ Missing filepath. Usage: save <filepath>" << std::endl;
            return;
        }
        
        std::string filepath = args[0];
        if (currentBrain->save(filepath)) {
            std::cout << "✅ Brain state saved to: " << filepath << std::endl;
        } else {
            std::cout << "❌ Failed to save brain state to: " << filepath << std::endl;
        }
    }
    
    void cmdReset(const std::vector<std::string>& args) {
        ensureBrainExists();
        
        currentBrain->reset();
        currentBrain->initialize();
        std::cout << "✅ Brain reset and reinitialized." << std::endl;
    }
    
    void cmdStep(const std::vector<std::string>& args) {
        ensureBrainExists();
        
        int stepCount = 10;
        double timestep = 0.001;
        
        if (!args.empty()) {
            try {
                stepCount = std::stoi(args[0]);
            } catch (...) {
                std::cout << "❌ Invalid step count: " << args[0] << std::endl;
                return;
            }
        }
        
        if (args.size() > 1) {
            try {
                timestep = std::stod(args[1]);
            } catch (...) {
                std::cout << "❌ Invalid timestep: " << args[1] << std::endl;
                return;
            }
        }
        
        std::cout << "Running " << stepCount << " simulation steps with timestep " << timestep << "s..." << std::endl;
        
        // Execute as command if command manager exists
        if (commandManager) {
            auto command = std::make_unique<SimulationCommand>(currentBrain.get(), stepCount, timestep);
            commandManager->execute(std::move(command));
        } else {
            // Run steps directly
            for (int i = 0; i < stepCount; ++i) {
                currentBrain->step(i, i * timestep);
            }
        }
        
        std::cout << "✅ Completed " << stepCount << " simulation steps." << std::endl;
    }
    
    void cmdInject(const std::vector<std::string>& args) {
        ensureBrainExists();
        
        if (args.size() < 2) {
            std::cout << "❌ Missing arguments. Usage: inject <type> <amount> [neuron_id]" << std::endl;
            return;
        }
        
        std::string typeStr = args[0];
        double amount = 0.0;
        
        try {
            amount = std::stod(args[1]);
        } catch (...) {
            std::cout << "❌ Invalid amount: " << args[1] << std::endl;
            return;
        }
        
        nlm::NeuronType neuronType;
        if (typeStr == "sensory") {
            neuronType = nlm::NeuronType::Sensory;
        } else if (typeStr == "interneuron") {
            neuronType = nlm::NeuronType::Interneuron;
        } else if (typeStr == "motor") {
            neuronType = nlm::NeuronType::Motor;
        } else if (typeStr == "internal") {
            neuronType = nlm::NeuronType::Internal;
        } else {
            std::cout << "❌ Invalid neuron type: " << typeStr << std::endl;
            return;
        }
        
        if (args.size() >= 3) {
            // Inject into specific neuron
            try {
                int neuronId = std::stoi(args[2]);
                currentBrain->injectCurrent(nlm::NeuronId(neuronId), amount);
                std::cout << "✅ Injected " << amount << " current into neuron " << neuronId << std::endl;
            } catch (...) {
                std::cout << "❌ Invalid neuron ID: " << args[2] << std::endl;
            }
        } else {
            // Inject into all neurons of specified type
            currentBrain->injectCurrentToNeurons(neuronType, amount);
            std::cout << "✅ Injected " << amount << " current into all " << typeStr << " neurons" << std::endl;
        }
    }
    
    void cmdStatus(const std::vector<std::string>& args) {
        ensureBrainExists();
        
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                            BRAIN STATUS                          ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        
        std::cout << "📊 Statistics:" << std::endl;
        std::cout << "  Total Neurons:     " << currentBrain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total Synapses:    " << currentBrain->getTotalSynapseCount() << std::endl;
        std::cout << "  Total Spikes:      " << currentBrain->getTotalSpikeCount() << std::endl;
        std::cout << "  Active Neurons:    " << currentBrain->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing Neurons:   " << currentBrain->getFiringNeuronCount() << std::endl;
        std::cout << "  Exc/Inh Ratio:     " << currentBrain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "  Average Firing:   " << currentBrain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << std::endl;
        
        std::cout << "📋 Systems Status:" << std::endl;
        
        if (auto* spikeSystem = currentBrain->getSpikeSystem()) {
            std::cout << "  Spike System:      Operational" << std::endl;
        }
        
        if (auto* stdp = currentBrain->getSTDP()) {
            stdcout << "  STDP Plasticity:   Operational" << std::endl;
        }
        
        if (auto* hebbian = currentBrain->getHebbian()) {
            stdcout << "  Hebbian Plasticity: Operational" << std::endl;
        }
        
        if (auto* structPlas = currentBrain->getStructuralPlasticity()) {
            stdcout << "  Structural Plasticity: Operational" << std::endl;
        }
        
        if (commandManager && commandManager->canUndo()) {
            stdcout << "  Command History:   " << commandManager->getHistory().size() << " commands" << std::endl;
            stdcout << "                   Undo available: Yes" << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    // Configuration Commands
    void cmdConfig(const std::vector<std::string>& args) {
        ensureConfigExists();
        
        if (args.empty()) {
            std::cout << "❌ Missing subcommand. Usage: config <get|set|load|save> [key] [value>" << std::endl;
            return;
        }
        
        std::string subcommand = args[0];
        
        if (subcommand == "get") {
            if (args.size() < 2) {
                std::cout << "❌ Missing key. Usage: config get <key>" << std::endl;
                return;
            }
            
            std::string key = args[1];
            if (currentConfig->has(key)) {
                // Try to get as different types
                auto strVal = currentConfig->get<std::string>(key);
                if (strVal) {
                    std::cout << "config[" << key << "] = \"" << *strVal << "\"" << std::endl;
                } else {
                    auto intVal = currentConfig->get<int64_t>(key);
                    if (intVal) {
                        std::cout << "config[" << key << "] = " << *intVal << std::endl;
                    } else {
                        auto doubleVal = currentConfig->get<double>(key);
                        if (doubleVal) {
                            std::cout << "config[" << key << "] = " << *doubleVal << std::endl;
                        } else {
                            auto boolVal = currentConfig->get<bool>(key);
                            if (boolVal) {
                                std::cout << "config[" << key << "] = " << (*boolVal ? "true" : "false") << std::endl;
                            }
                        }
                    }
                }
            } else {
                std::cout << "❌ Key not found: " << key << std::endl;
            }
        } else if (subcommand == "set") {
            if (args.size() < 3) {
                std::cout << "❌ Missing key and value. Usage: config set <key> <value>" << std::endl;
                return;
            }
            
            std::string key = args[1];
            std::string value = args[2];
            
            // Try to determine type based on content
            if (value == "true" || value == "false") {
                bool boolVal = (value == "true");
                currentConfig->set(key, boolVal, nlm::ConfigSource::Runtime);
                std::cout << "✅ Set config[" << key << "] = " << (boolVal ? "true" : "false") << std::endl;
            } else {
                // Try to parse as number
                try {
                    if (value.find('.') != std::string::npos) {
                        double doubleVal = std::stod(value);
                        currentConfig->set(key, doubleVal, nlm::ConfigSource::Runtime);
                        std::cout << "✅ Set config[" << key << "] = " << doubleVal << std::endl;
                    } else {
                        int64_t intVal = std::stoll(value);
                        currentConfig->set(key, intVal, nlm::ConfigSource::Runtime);
                        std::cout << "✅ Set config[" << key << "] = " << intVal << std::endl;
                    }
                } catch (...) {
                    // Treat as string
                    currentConfig->set(key, value, nlm::ConfigSource::Runtime);
                    std::cout << "✅ Set config[" << key << "] = \"" << value << "\"" << std::endl;
                }
            }
        } else if (subcommand == "load") {
            if (args.empty()) {
                std::cout << "❌ Missing filepath. Usage: config load <filepath>" << std::endl;
                return;
            }
            
            std::string filepath = args[0];
            if (currentConfig->loadFromFile(filepath)) {
                std::cout << "✅ Configuration loaded from: " << filepath << std::endl;
            } else {
                std::cout << "❌ Failed to load configuration from: " << filepath << std::endl;
            }
        } else if (subcommand == "save") {
            if (args.empty()) {
                std::cout << "❌ Missing filepath. Usage: config save <filepath>" << std::endl;
                return;
            }
            
            std::string filepath = args[0];
            if (currentConfig->saveToFile(filepath)) {
                std::cout << "✅ Configuration saved to: " << filepath << std::endl;
            } else {
                std::cout << "❌ Failed to save configuration to: " << filepath << std::endl;
            }
        } else {
            std::cout << "❌ Unknown subcommand: " << subcommand << std::endl;
            std::cout << "Available subcommands: get, set, load, save" << std::endl;
        }
    }
    
    void cmdValidate(const std::vector<std::string>& args) {
        ensureConfigExists();
        
        if (currentConfig->isValid()) {
            std::cout << "✅ Configuration is valid." << std::endl;
        } else {
            std::cout << "❌ Configuration validation failed:" << std::endl;
            std::cout << currentConfig->getValidationErrors() << std::endl;
        }
    }
    
    void cmdValidateAll(const std::vector<std::string>& args) {
        auto missing = nlm::ConfigManager::getInstance().validateRequired();
        if (missing.empty()) {
            std::cout << "✅ All required configuration keys are present." << std::endl;
        } else {
            std::cout << "❌ Missing required configuration keys:" << std::endl;
            for (const auto& key : missing) {
                std::cout << "  - " << key << std::endl;
            }
        }
    }
    
    // Command Management Commands
    void cmdCommand(const std::vector<std::string>& args) {
        ensureBrainExists();
        
        if (args.empty()) {
            std::cout << "❌ Missing subcommand. Usage: command <history|undo|redo|clear> [command_name]" << std::endl;
            return;
        }
        
        std::string subcommand = args[0];
        
        if (subcommand == "history") {
            if (!commandManager) {
                std::cout << "❌ No command manager available." << std::endl;
                return;
            }
            
            auto history = commandManager->getHistory();
            std::cout << "Command History (" << history.size() << " commands):" << std::endl;
            for (size_t i = 0; i < history.size(); ++i) {
                std::cout << "  " << i << ". " << history[i]->getName() << std::endl;
            }
        } else if (subcommand == "undo") {
            if (!commandManager) {
                std::cout << "❌ No command manager available." << std::endl;
                return;
            }
            
            if (commandManager->undo()) {
                std::cout << "✅ Command undone successfully." << std::endl;
            } else {
                std::cout << "❌ No command to undo." << std::endl;
            }
        } else if (subcommand == "redo") {
            if (!commandManager) {
                std::cout << "❌ No command manager available." << std::endl;
                return;
            }
            
            if (commandManager->redo()) {
                std::cout << "✅ Command redone successfully." << std::endl;
            } else {
                std::cout << "❌ No command to redo." << std::endl;
            }
        } else if (subcommand == "clear") {
            if (!commandManager) {
                std::cout << "❌ No command manager available." << std::endl;
                return;
            }
            
            commandManager->clearHistory();
            std::cout << "✅ Command history cleared." << std::endl;
        } else {
            std::cout << "❌ Unknown subcommand: " << subcommand << std::endl;
            std::cout << "Available subcommands: history, undo, redo, clear" << std::endl;
        }
    }
    
    void cmdProfile(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cout << "❌ Missing subcommand. Usage: profile <start|stop|save> [filename>" << std::endl;
            return;
        }
        
        std::string subcommand = args[0];
        
        if (subcommand == "start") {
            nlm::profiling::setLevel(nlm::PerformanceLevel::Basic);
            std::cout << "✅ Profiling started." << std::endl;
        } else if (subcommand == "stop") {
            nlm::profiling::setLevel(nlm::PerformanceLevel::Off);
            std::cout << "✅ Profiling stopped." << std::endl;
        } else if (subcommand == "save") {
            std::string filename = "/tmp/profiler_results.txt";
            if (args.size() > 1) {
                filename = args[1];
            }
            nlm::profiling::saveToFile(filename);
            std::cout << "✅ Profiling results saved to: " << filename << std::endl;
        } else {
            std::cout << "❌ Unknown subcommand: " << subcommand << std::endl;
            std::cout << "Available subcommands: start, stop, save" << std::endl;
        }
    }
    
    // Experiment Commands
    void cmdExperiment(const std::vector<std::string>& args) {
        ensureBrainExists();
        
        if (args.empty()) {
            std::cout << "❌ Missing experiment name. Usage: experiment <phase2|phase3|phase4|phase5|phase6> [iterations]" << std::endl;
            return;
        }
        
        std::string experimentName = args[0];
        int iterations = 1;
        
        if (args.size() > 1) {
            try {
                iterations = std::stoi(args[1]);
            } catch (...) {
                std::cout << "❌ Invalid iterations: " << args[1] << std::endl;
                return;
            }
        }
        
        std::cout << "Running " << experimentName << " experiment for " << iterations << " iterations..." << std::endl;
        
        // Reset brain before experiment
        cmdReset({});
        
        // Run experiment
        for (int i = 0; i < iterations; ++i) {
            std::cout << "  Iteration " << (i + 1) << "/" << iterations << "..." << std::endl;
            
            // Run basic steps
            for (int step = 0; step < 100; ++step) {
                currentBrain->step(step, step * 0.001);
            }
        }
        
        std::cout << "✅ " << experimentName << " experiment completed successfully." << std::endl;
    }
    
    void cmdBenchmark(const std::vector<std::string>& args) {
        ensureBrainExists();
        
        if (args.empty()) {
            std::cout << "❌ Missing duration. Usage: benchmark <duration> [output_file>" << std::endl;
            return;
        }
        
        double duration = 10.0;
        std::string outputFile = "/tmp/benchmark_results.txt";
        
        try {
            duration = std::stod(args[0]);
        } catch (...) {
            std::cout << "❌ Invalid duration: " << args[0] << std::endl;
            return;
        }
        
        if (args.size() > 1) {
            outputFile = args[1];
        }
        
        std::cout << "Running benchmark for " << duration << " seconds..." << std::endl;
        
        // Start profiling
        nlm::profiling::setLevel(nlm::PerformanceLevel::Basic);
        
        // Run benchmark
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (double elapsed = 0.0; elapsed < duration; elapsed += 0.1) {
            // Run some operations
            currentBrain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
            
            for (int step = 0; step < 50; ++step) {
                currentBrain->step(step, step * 0.001);
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration<double>(endTime - startTime);
        
        // Stop profiling
        nlm::profiling::setLevel(nlm::PerformanceLevel::Off);
        nlm::profiling::saveToFile(outputFile);
        
        std::cout << "✅ Benchmark completed in " << totalDuration.count() << " seconds." << std::endl;
        std::cout << "Results saved to: " << outputFile << std::endl;
    }
    
    // Utility Commands
    void cmdHelp(const std::vector<std::string>& args) {
        if (args.empty()) {
            showHelp();
        } else {
            std::string commandName = args[0];
            auto it = std::find_if(commands.begin(), commands.end(),
                                  [&commandName](const Command& c) { return c.name == commandName; });
            
            if (it != commands.end()) {
                std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
                std::cout << "║                           COMMAND HELP                          ║" << std::endl;
                std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
                std::cout << std::endl;
                std::cout << "Command: " << it->name << std::endl;
                std::cout << "Description: " << it->description << std::endl;
                std::cout << "Usage: " << it->usage << std::endl;
                std::cout << "Category: " << it->category << std::endl;
                std::cout << "Requires Brain: " << (it->requiresBrain ? "Yes" : "No") << std::endl;
            } else {
                std::cout << "❌ Unknown command: " << commandName << std::endl;
            }
        }
    }
    
    void cmdAlias(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cout << "❌ Missing subcommand. Usage: alias <list|add|remove> [alias] [command>" << std::endl;
            return;
        }
        
        std::string subcommand = args[0];
        
        if (subcommand == "list") {
            std::cout << "Current aliases:" << std::endl;
            for (const auto& alias : aliases) {
                std::cout << "  " << alias.first << " -> " << alias.second << std::endl;
            }
        } else if (subcommand == "add") {
            if (args.size() < 3) {
                std::cout << "❌ Missing alias and command. Usage: alias add <alias> <command>" << std::endl;
                return;
            }
            
            std::string alias = args[1];
            std::string command = args[2];
            
            // Check if command exists
            auto cmdIt = std::find_if(commands.begin(), commands.end(),
                                     [&command](const Command& c) { return c.name == command; });
            
            if (cmdIt != commands.end()) {
                aliases[alias] = command;
                std::cout << "✅ Alias \"" << alias << "\" added -> \"" << command << "\"" << std::endl;
            } else {
                std::cout << "❌ Unknown command: " << command << std::endl;
            }
        } else if (subcommand == "remove") {
            if (args.size() < 2) {
                std::cout << "❌ Missing alias. Usage: alias remove <alias>" << std::endl;
                return;
            }
            
            std::string alias = args[1];
            
            if (aliases.find(alias) != aliases.end()) {
                aliases.erase(alias);
                std::cout << "✅ Alias \"" << alias << "\" removed." << std::endl;
            } else {
                std::cout << "❌ Alias not found: " << alias << std::endl;
            }
        } else {
            std::cout << "❌ Unknown subcommand: " << subcommand << std::endl;
            std::cout << "Available subcommands: list, add, remove" << std::endl;
        }
    }
    
    void cmdExit(const std::vector<std::string>& args) {
        std::cout << "Exiting NLM Advanced CLI..." << std::endl;
        // This will be handled by the main run loop
    }
};

// Main function for advanced CLI
void runAdvancedCLI() {
    AdvancedCLI cli;
    cli.run();
}
