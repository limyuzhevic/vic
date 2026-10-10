// NLM CLI Tool
// Command-line interface for NLM brain simulation with configuration support

#include "core/Config/Config.hpp"
#include "core/Types/Types.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <memory>
#include <chrono>

// Command-line argument parsing
class CLIParser {
public:
    struct Command {
        std::string name;
        std::string description;
        std::vector<std::string> args;
    };
    
    CLIParser(int argc, char** argv) {
        parseArgs(argc, argv);
    }
    
    bool hasCommand(const std::string& cmd) const {
        return commands.find(cmd) != commands.end();
    }
    
    std::string getCommand(const std::string& cmd) const {
        auto it = commands.find(cmd);
        return it != commands.end() ? it->second.name : "";
    }
    
    std::string getArg(const std::string& cmd, const std::string& arg) const {
        auto cmdIt = commands.find(cmd);
        if (cmdIt == commands.end()) return "";
        
        for (size_t i = 0; i < cmdIt->second.args.size(); i += 2) {
            if (cmdIt->second.args[i] == arg) {
                if (i + 1 < cmdIt->second.args.size()) {
                    return cmdIt->second.args[i + 1];
                }
            }
        }
        return "";
    }
    
    bool hasArg(const std::string& cmd, const std::string& arg) const {
        auto cmdIt = commands.find(cmd);
        if (cmdIt == commands.end()) return false;
        
        for (size_t i = 0; i < cmdIt->second.args.size(); i += 2) {
            if (cmdIt->second.args[i] == arg) {
                return true;
            }
        }
        return false;
    }
    
    const std::map<std::string, Command>& getAllCommands() const {
        return commands;
    }
    
    std::string getValue(const std::string& key) const {
        auto it = values.find(key);
        return it != values.end() ? it->second : "";
    }
    
    bool hasValue(const std::string& key) const {
        return values.find(key) != values.end();
    }
    
    void printHelp() const {
        std::cout << "\n=== NLM CLI Tool ===\n" << std::endl;
        std::cout << "Usage: nlm-cli [command] [options]\n" << std::endl;
        std::cout << "\nCommands:" << std::endl;
        
        for (const auto& pair : commands) {
            const auto& cmd = pair.second;
            std::cout << "  " << cmd.name << std::endl;
            std::cout << "    " << cmd.description << std::endl;
            
            if (!cmd.args.empty()) {
                std::cout << "    Arguments:" << std::endl;
                for (size_t i = 0; i < cmd.args.size(); i += 2) {
                    std::cout << "      --" << cmd.args[i] << " <value>" << std::endl;
                }
            }
            std::cout << std::endl;
        }
        
        std::cout << "Global options:" << std::endl;
        std::cout << "  --config <file>    Configuration file path" << std::endl;
        std::cout << "  --verbose           Enable verbose output" << std::endl;
        std::cout << "  --log-level <level> Log level (trace, debug, info, warn, error, critical)" << std::endl;
        std::cout << "  --help              Show this help message" << std::endl;
    }
    
private:
    void parseArgs(int argc, char** argv) {
        std::string currentCommand;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Check for global options
            if (arg == "--help") {
                showHelp = true;
                continue;
            }
            
            if (arg.substr(0, 2) == "--") {
                // Global option
                std::string key = arg.substr(2);
                size_t pos = key.find('=');
                if (pos != std::string::npos) {
                    std::string value = key.substr(pos + 1);
                    key = key.substr(0, pos);
                    values[key] = value;
                } else if (i + 1 < argc) {
                    std::string nextArg = argv[++i];
                    if (nextArg.substr(0, 2) != "--") {
                        values[key] = nextArg;
                    } else {
                        values[key] = "true";
                        i--; // Put back the argument
                    }
                } else {
                    values[key] = "true";
                }
                continue;
            }
            
            // Check if this is a command (starts with dash but not global option)
            if (arg[0] == '-') {
                // Command
                currentCommand = arg.substr(1);
                if (commands.find(currentCommand) == commands.end()) {
                    // Register new command
                    Command cmd;
                    cmd.name = currentCommand;
                    cmd.args = std::vector<std::string>();
                    commands[currentCommand] = cmd;
                }
            } else {
                // Positional argument (value for current command)
                if (!currentCommand.empty()) {
                    commands[currentCommand].args.push_back(arg);
                } else {
                    // Not in a command context, treat as value
                    values["command"] = arg;
                }
            }
        }
    }
    
    bool showHelp = false;
    std::map<std::string, Command> commands;
    std::map<std::string, std::string> values;
    
private:
    void parseArgs(int argc, char** argv) {
        std::string currentCommand;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Check for global options
            if (arg == "--help") {
                showHelp = true;
                continue;
            }
            
            if (arg.substr(0, 2) == "--") {
                // Global option
                std::string key = arg.substr(2);
                size_t pos = key.find('=');
                if (pos != std::string::npos) {
                    std::string value = key.substr(pos + 1);
                    key = key.substr(0, pos);
                    values[key] = value;
                } else if (i + 1 < argc) {
                    std::string nextArg = argv[++i];
                    if (nextArg.substr(0, 2) != "--") {
                        values[key] = nextArg;
                    } else {
                        values[key] = "true";
                        i--; // Put back the argument
                    }
                } else {
                    values[key] = "true";
                }
                continue;
            }
            
            // Check if this is a command (starts with dash but not global option)
            if (arg[0] == '-') {
                // Command
                currentCommand = arg.substr(1);
                if (commands.find(currentCommand) == commands.end()) {
                    // Register new command
                    Command cmd;
                    cmd.name = currentCommand;
                    cmd.args = std::vector<std::string>();
                    commands[currentCommand] = cmd;
                }
            } else {
                // Positional argument (value for current command)
                if (!currentCommand.empty()) {
                    commands[currentCommand].args.push_back(arg);
                } else {
                    // Not in a command context, treat as value
                    values["command"] = arg;
                }
            }
        }
    }
    
    bool showHelp = false;
    std::map<std::string, Command> commands;
    std::map<std::string, std::string> values;
};

// NLM CLI Application
class NLMCLI {
public:
    NLMCLI(int argc, char** argv) : parser(argc, argv) {}
    
    bool run() {
        if (parser.hasValue("help")) {
            parser.printHelp();
            return true;
        }
        
        // Load configuration
        std::string configFile = "configs/default.cfg";
        if (parser.hasValue("config")) {
            configFile = parser.getValue("config");
        }
        
        config = std::make_shared<Config>();
        
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Override with command line arguments
        config->loadFromArgs(0, nullptr); // Will be called from commands
        
        // Set environment variables if specified
        std::unordered_map<std::string, std::string> envMap;
        config->loadFromEnvironment(envMap);
        
        // Execute commands
        if (parser.hasCommand("create")) {
            return runCreateCommand();
        } else if (parser.hasCommand("step")) {
            return runStepCommand();
        } else if (parser.hasCommand("save")) {
            return runSaveCommand();
        } else if (parser.hasCommand("load")) {
            return runLoadCommand();
        } else if (parser.hasCommand("reset")) {
            return runResetCommand();
        } else if (parser.hasCommand("demo")) {
            return runDemoCommand();
        } else if (parser.hasCommand("experiment")) {
            return runExperimentCommand();
        } else if (parser.hasCommand("config")) {
            return runConfigCommand();
        } else if (parser.hasCommand("import")) {
            return runImportCommand();
        } else if (parser.hasCommand("export")) {
            return runExportCommand();
        } else {
            // Default: run demo
            NLM_LOG_WARN("No command specified, running default demo.");
            return runDemoCommand();
        }
    }
    
private:
    bool runCreateCommand() {
        NLM_LOG_INFO("Creating NLM brain simulation...");
        
        // Set parameters from command line
        std::string neurons = parser.getArg("create", "neurons");
        std::string regions = parser.getArg("create", "regions");
        std::string timestep = parser.getArg("create", "timestep");
        
        if (!neurons.empty()) {
            config->set("neuron_count", std::stoi(neurons), ConfigSource::CommandLine);
        }
        
        if (!regions.empty()) {
            config->set("region_count", std::stoi(regions), ConfigSource::CommandLine);
        }
        
        if (!timestep.empty()) {
            config->set("simulation_timestep", std::stod(timestep), ConfigSource::CommandLine);
        }
        
        // Initialize brain
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return false;
        }
        
        brain->logStatus();
        NLM_LOG_INFO("Brain created successfully.");
        return true;
    }
    
    bool runStepCommand() {
        std::string stepsStr = parser.getArg("step", "steps");
        int steps = stepsStr.empty() ? 100 : std::stoi(stepsStr);
        
        NLM_LOG_INFO("Running " + std::to_string(steps) + " simulation steps...");
        
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * config->getOr<double>("simulation_timestep", 0.001));
            
            if (i % 10 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(i) + ": " + 
                           std::to_string(brain->getTotalSpikeCount()) + " spikes");
            }
        }
        
        NLM_LOG_INFO("Completed " + std::to_string(steps) + " steps.");
        return true;
    }
    
    bool runSaveCommand() {
        std::string filepath = parser.getArg("save", "filepath");
        if (filepath.empty()) {
            filepath = "brain_state.nlm";
            NLM_LOG_INFO("Using default filepath: " + filepath);
        }
        
        NLM_LOG_INFO("Saving brain state to: " + filepath);
        
        // Save configuration
        std::string configFile = filepath + ".cfg";
        if (config->saveToFile(configFile)) {
            NLM_LOG_INFO("Configuration saved to: " + configFile);
        }
        
        // Save brain state if supported
        if (brain) {
            // TODO: Implement brain state serialization
            NLM_LOG_INFO("Brain state saved (serialization not yet implemented).");
        }
        
        return true;
    }
    
    bool runLoadCommand() {
        std::string filepath = parser.getArg("load", "filepath");
        if (filepath.empty()) {
            filepath = "brain_state.nlm";
            NLM_LOG_INFO("Using default filepath: " + filepath);
        }
        
        NLM_LOG_INFO("Loading brain state from: " + filepath);
        
        // Load configuration
        std::string configFile = filepath + ".cfg";
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Configuration loaded from: " + configFile);
        }
        
        // Load brain state if supported
        if (brain) {
            // TODO: Implement brain state deserialization
            NLM_LOG_INFO("Brain state loaded (deserialization not yet implemented).");
        }
        
        return true;
    }
    
    bool runResetCommand() {
        NLM_LOG_INFO("Resetting brain...");
        
        if (brain) {
            brain->reset();
            brain->initialize();
            NLM_LOG_INFO("Brain reset successfully.");
        } else {
            NLM_LOG_WARN("No brain created to reset.");
        }
        
        return true;
    }
    
    bool runDemoCommand() {
        NLM_LOG_INFO("Running NLM demo...");
        
        // Create and run a basic demo using existing main.cpp logic
        return runBasicDemo();
    }
    
    bool runExperimentCommand() {
        std::string experimentName = parser.getArg("experiment", "name");
        if (experimentName.empty()) {
            experimentName = "demo";
        }
        
        NLM_LOG_INFO("Running experiment: " + experimentName);
        
        // TODO: Implement experiment running
        NLM_LOG_WARN("Experiment feature not yet implemented.");
        return true;
    }
    
    bool runConfigCommand() {
        NLM_LOG_INFO("Configuration:");
        std::cout << config->summary() << std::endl;
        
        // Save to file if requested
        if (parser.hasArg("config", "save")) {
            std::string filepath = parser.getArg("config", "filepath");
            if (filepath.empty()) {
                filepath = "configs/nlm.cfg";
            }
            
            if (config->saveToFile(filepath)) {
                NLM_LOG_INFO("Configuration saved to: " + filepath);
            }
        }
        
        return true;
    }
    
    bool runImportCommand() {
        std::string filepath = parser.getArg("import", "filepath");
        if (filepath.empty()) {
            NLM_LOG_ERROR("Import file path required for import command.");
            return false;
        }
        
        NLM_LOG_INFO("Importing brain state from: " + filepath);
        
        // Load configuration
        if (config->loadFromFile(filepath)) {
            NLM_LOG_INFO("Configuration imported successfully.");
        } else {
            NLM_LOG_ERROR("Failed to import configuration from: " + filepath);
            return false;
        }
        
        return true;
    }
    
    bool runExportCommand() {
        std::string filepath = parser.getArg("export", "filepath");
        if (filepath.empty()) {
            NLM_LOG_ERROR("Export file path required for export command.");
            return false;
        }
        
        NLM_LOG_INFO("Exporting brain state to: " + filepath);
        
        // Save configuration
        if (config->saveToFile(filepath)) {
            NLM_LOG_INFO("Configuration exported successfully.");
        } else {
            NLM_LOG_ERROR("Failed to export configuration to: " + filepath);
            return false;
        }
        
        return true;
    }
    
    bool runBasicDemo() {
        NLM_LOG_INFO("Running basic demo simulation...");
        
        // Initialize brain
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return false;
        }
        
        brain->logStatus();
        
        // Run a few steps
        NLM_LOG_INFO("Running simulation steps...");
        for (int step = 0; step < 50; ++step) {
            brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
            
            if (step % 10 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + ": " + 
                           std::to_string(brain->getTotalSpikeCount()) + " spikes");
            }
        }
        
        // Run plasticity experiment
        NLM_LOG_INFO("Running plasticity experiment...");
        runPlasticityDemo(brain);
        
        NLM_LOG_INFO("Demo completed successfully.");
        return true;
    }
    
    void runPlasticityDemo(std::shared_ptr<Brain> brain) {
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply input pattern
        for (int trial = 0; trial < 100; ++trial) {
            for (size_t i = 0; i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(trial, trial * config->getOr<double>("simulation_timestep", 0.001));
        }
    }
    
    CLIParser parser;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
};

int main(int argc, char** argv) {
    NLMCLI cli(argc, argv);
    return cli.run() ? 0 : 1;
}
