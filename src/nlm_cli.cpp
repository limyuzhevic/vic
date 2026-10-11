// NLM CLI - Neural Learning Machine Command Line Interface
// Comprehensive CLI for controlling and monitoring the NLM brain simulation
// 
// Features:
// - Basic simulation control (start/stop, step, run)
// - Advanced brain configuration and inspection
// - Performance monitoring and statistics
// - Configuration management (load/save profiles)
// - Development and testing tools
// - Interactive and batch modes
// - Comprehensive help system

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <ctime>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

using namespace nlm;

class CommandParser {
public:
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::string helpText;
        std::vector<std::string> aliases;
        bool requiresSimulation;
        bool isInteractive;
        std::function<bool(const std::vector<std::string>&, void*)> handler;
    };

    CommandParser() {
        initializeCommands();
    }

    bool parseCommand(const std::string& input, std::string& commandName, std::vector<std::string>& args) {
        std::istringstream iss(input);
        std::string token;
        
        if (!(iss >> token)) {
            return false;
        }
        
        commandName = token;
        args.clear();
        
        std::string arg;
        while (iss >> arg) {
            // Handle quoted arguments
            if (!arg.empty() && (arg[0] == '"' || arg[0] == '\'')) {
                std::string quoted = arg;
                while (iss >> arg && (arg.back() == '"' || arg.back() == '\'')) {
                    quoted += " " + arg;
                }
                // Remove surrounding quotes
                if (quoted.size() >= 2 && quoted[0] == quoted.back()) {
                    quoted = quoted.substr(1, quoted.size() - 2);
                }
                args.push_back(quoted);
            } else {
                args.push_back(arg);
            }
        }
        
        // Convert to lowercase for command matching
        std::transform(commandName.begin(), commandName.end(), commandName.begin(), ::tolower);
        return true;
    }

    const Command* getCommand(const std::string& name) const {
        std::string lowerName = name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        
        auto it = commands.find(lowerName);
        if (it != commands.end()) {
            return &it->second;
        }
        
        // Check aliases
        for (const auto& pair : commands) {
            for (const auto& alias : pair.second.aliases) {
                if (alias == lowerName) {
                    return &pair.second;
                }
            }
        }
        return nullptr;
    }

    std::vector<std::string> getAllCommandNames() const {
        std::vector<std::string> names;
        for (const auto& pair : commands) {
            names.push_back(pair.first);
        }
        return names;
    }

    void displayHelp(const std::string& commandName = "") const {
        std::cout << std::endl;
        std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        
        if (commandName.empty()) {
            std::cout << "║                                NLM CLI Help                                   ║" << std::endl;
            std::cout << "╠═══════════════════════════════════════════════════════════════════════════════╣" << std::endl;
            
            for (const auto& pair : commands) {
                const auto& cmd = pair.second;
                std::cout << "║ " << std::left << std::setw(20) << cmd.name << " - " << cmd.description << " ║" << std::endl;
            }
            
            std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << std::endl;
            std::cout << "For detailed help on a specific command, use: help <command>" << std::endl;
        } else {
            const Command* cmd = getCommand(commandName);
            if (cmd) {
                std::cout << "║                                 Command: " << cmd->name << "                                    ║" << std::endl;
                std::cout << "╠═══════════════════════════════════════════════════════════════════════════════╣" << std::endl;
                std::cout << "║ Usage: " << cmd->usage << "                                 ║" << std::endl;
                std::cout << "║ Description: " << cmd->description << "                                   ║" << std::endl;
                if (!cmd->helpText.empty()) {
                    std::cout << "║ " << std::left << std::setw(70) << cmd->helpText << " ║" << std::endl;
                }
                if (!cmd->aliases.empty()) {
                    std::cout << "║ Aliases: " << std::left << std::setw(70) << cmd->aliasesToString() << " ║" << std::endl;
                }
                std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
            } else {
                std::cout << "║                                  Command not found: " << commandName << "                                 ║" << std::endl;
                std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
            }
        }
        std::cout << std::endl;
    }

private:
    std::string Command::aliasesToString() const {
        std::string result;
        for (size_t i = 0; i < aliases.size(); ++i) {
            if (i > 0) result += ", ";
            result += aliases[i];
        }
        return result;
    }

    void initializeCommands() {
        // Basic Simulation Control
        registerCommand("start", "Start the simulation", "start [duration] [steps]", 
                       "Begin or resume neural simulation. Without arguments, runs in auto-mode. "
                       "With duration/steps, runs for specified time or step count.", 
                       {"run", "begin"}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set by SimulationController
        
        registerCommand("stop", "Stop the simulation", "stop", 
                       "Stop the currently running simulation. Saves current state as last checkpoint.", 
                       {"pause", "halt"}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("step", "Execute one simulation step", "step [steps]", 
                       "Execute one or more simulation steps manually. Useful for debugging and observation.", 
                       {"tick"}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("run", "Run automated simulation", "run <duration> [steps]", 
                       "Run simulation for a specified duration (e.g., '1s', '5m', '2h') or step count. "
                       "Can be interrupted with 'stop'.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        // Advanced Configuration
        registerCommand("configure", "Configure brain parameters", "configure <key> <value>", 
                       "Set brain configuration parameters. Common keys: neuron_count, connection_probability, "
                       "stdp_ltp_weight, stdp_ltd_weight, simulation_timestep, region_count.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("inspect", "Inspect brain state and components", "inspect <component>", 
                       "Inspect brain components: neurons, synapses, memory, prediction, neuromodulation. "
                       "Without component shows overall status.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("save", "Save brain state to checkpoint", "save <filename>", 
                       "Save current brain state to a checkpoint file. Use 'load <filename>' to restore.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("load", "Load brain state from checkpoint", "load <filename>", 
                       "Load brain state from a checkpoint file. Resets current simulation state.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("neuromodulate", "Control neuromodulation signals", "neuromodulate <type> <intensity>", 
                       "Apply neuromodulatory signals: dopamine, curiosity, novelty, prediction_error. "
                       "Intensity ranges from 0.0 to 1.0.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("memory", "Manipulate working memory", "memory <action> [data]", 
                       "Working memory operations: store, recall, clear, list, capacity. "
                       "Data can be string or numeric values.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        // Performance and Monitoring
        registerCommand("stats", "Display real-time statistics", "stats [refresh_interval]", 
                       "Display live simulation statistics including neuron counts, firing rates, "
                       "synaptic activity, and memory usage. Press Ctrl+C to stop monitoring.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("profile", "Performance profiling", "profile <duration> [output_file]", 
                       "Profile simulation performance over time. Collects timing data, "
                       "memory usage, and event queue statistics for optimization analysis.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("monitor", "Monitor system resources", "monitor [interval]", 
                       "Monitor system resources: memory usage, CPU utilization, event queue depth. "
                       "Provides real-time performance metrics.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        // Configuration Management
        registerCommand("config", "Configuration management", "config <action> <file> [name]", 
                       "Manage configuration files: load, save, list, switch between profiles. "
                       "Useful for reproducible experiments.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("set", "Set runtime parameter", "set <key> <value>", 
                       "Set runtime configuration parameter that affects current simulation. "
                       "Changes take effect immediately.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("profile", "Profile a brain configuration", "profile <config_name>", 
                       "Run simulation with a named configuration to compare performance "
                       "and determine optimal settings.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        // Development and Testing
        registerCommand("test", "Run unit tests", "test [test_name]", 
                       "Run NLM unit tests. Without argument runs all tests. "
                       "With test_name runs specific test suite.", {}, false, true,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("benchmark", "Run performance benchmarks", "benchmark <neuron_count> <steps>", 
                       "Run performance benchmarks with specified network size. Measures "
                       "steps/sec, memory usage, and computational efficiency.", {}, false, true,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("leaktest", "Memory leak detection", "leaktest <duration> [threshold]", 
                       "Detect memory leaks during simulation. Monitors memory allocation/deallocation "
                       "patterns to identify resource leaks over time.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("integrate", "Integration testing", "integrate <test_type>", 
                       "Run integration tests to verify component interactions: memory_systems, "
                       "neuromodulation, prediction, all_systems. Ensures complete brain functionality.", {}, false, true,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        // Utility Commands
        registerCommand("help", "Show command help", "help [command]", 
                       "Display help information for commands or general CLI usage. "
                       "Without argument shows all available commands.", {}, false, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("exit", "Exit the application", "exit", 
                       "Save current state and exit the NLM CLI. Use 'quit' or Ctrl+D as well.", {}, false, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("quit", "Exit the application", "quit", 
                       "Alias for 'exit'. Save current state and exit the NLM CLI.", {}, false, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("clear", "Clear the screen", "clear", 
                       "Clear the terminal screen. Useful for keeping output manageable during long simulations.", {}, false, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("version", "Show version information", "version", 
                       "Display NLM version and build information, including compilation date and features.", {}, false, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
        
        registerCommand("status", "Show current simulation status", "status", 
                       "Display current simulation state including brain status, configuration, "
                       "performance metrics, and active processes.", {}, true, false,
                       [](const std::vector<std::string>& args) { return false; });  // Will be set
    }

    void registerCommand(const std::string& name, const std::string& description, const std::string& usage,
                        const std::string& helpText, const std::vector<std::string>& aliases,
                        bool requiresSimulation, bool isInteractive,
                        std::function<bool(const std::vector<std::string>&)> handler) {
        Command cmd;
        cmd.name = name;
        cmd.description = description;
        cmd.usage = usage;
        cmd.helpText = helpText;
        cmd.aliases = aliases;
        cmd.requiresSimulation = requiresSimulation;
        cmd.isInteractive = isInteractive;
        cmd.handler = handler;
        
        commands[name] = cmd;
    }

private:
    std::map<std::string, Command> commands;
};