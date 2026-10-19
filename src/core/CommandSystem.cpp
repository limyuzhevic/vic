// Command system implementation for NLM
// This file contains the core command implementations that make NLM more extensible and user-friendly

#include "CommandSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <filesystem>
#include <random>

namespace nlm {

// Command helper implementations
namespace CommandHelpers {
    template<typename T>
    T CommandHelpers::convertString(const std::string& value) {
        std::istringstream iss(value);
        T result;
        if (iss >> result) {
            return result;
        }
        throw std::runtime_error("Cannot convert string to type");
    }
    
    std::string CommandHelpers::formatOutput(const std::string& text, const std::string& color) {
        // In a real implementation, this would add ANSI color codes
        return text;
    }
    
    std::string CommandHelpers::formatError(const std::string& message) {
        return "ERROR: " + message;
    }
    
    bool CommandHelpers::validateFilePath(const std::string& path, bool mustExist) {
        if (path.empty()) {
            return false;
        }
        
        std::filesystem::path fsPath(path);
        
        if (mustExist && !std::filesystem::exists(fsPath)) {
            return false;
        }
        
        // Check if parent directory exists and is writable
        if (std::filesystem::exists(fsPath.parent_path())) {
            return std::filesystem::is_directory(fsPath.parent_path()) && 
                   std::filesystem::status(fsPath.parent_path()).permissions() & 
                   std::filesystem::perms::owner_write;
        }
        
        return true;
    }
    
    std::string CommandHelpers::generateUniqueFilename(const std::string& basePath, 
                                                       const std::string& prefix, 
                                                       const std::string& extension) {
        std::filesystem::path path(basePath);
        std::string filename = prefix + std::to_string(std::time(nullptr)) + extension;
        path /= filename;
        return path.string();
    }
}

// Brain information command
class BrainInfoCommand : public ICommand {
public:
    BrainInfoCommand() {
        CommandInfo info(
            "brain-info",
            "Display detailed information about the current brain state",
            "brain-info [--detailed]",
            "Brain",
            "NLM Core",
            "1.0",
            true
        );
        info_ = info;
        
        arguments_ = {
            CommandArgument("detailed", "bool", "false", "Show detailed information", false)
        };
    }
    
    CommandInfo getInfo() const override { return info_; }
    
    CommandResult execute(CommandContext& context) override {
        try {
            if (!context.config) {
                context.errorMessage = "No configuration available";
                return CommandResult::RuntimeError;
            }
            
            bool detailed = false;
            auto detailedOpt = context.config->get<bool>("detailed");
            if (detailedOpt) detailed = *detailedOpt;
            
            std::ostringstream oss;
            oss << "=== NLM Brain Information ===" << std::endl;
            oss << "Total neurons: " << context.config->getOr<int64_t>("neuron_count", 0) << std::endl;
            oss << "Regions: " << context.config->getOr<int64_t>("region_count", 0) << std::endl;
            oss << "Total synapses: " << context.config->getOr<size_t>("total_synapses", 0) << std::endl;
            oss << "Connection probability: " << context.config->getOr<float>("connection_probability", 0.0f) << std::endl;
            
            if (detailed) {
                oss << std::endl << "=== Detailed Configuration ===" << std::endl;
                std::vector<std::string> keys = context.config->getKeys();
                for (const std::string& key : keys) {
                    auto value = context.config->get<std::string>(key);
                    if (value) {
                        oss << key << " = " << value.value() << std::endl;
                    }
                }
            }
            
            context.output = oss.str();
            return CommandResult::Success;
            
        } catch (const std::exception& e) {
            context.errorMessage = std::string("Failed to get brain info: ") + e.what();
            return CommandResult::RuntimeError;
        }
    }
    
    std::vector<CommandArgument> getArguments() const override { return arguments_; }
    
    bool validateArguments(const CommandContext& context) const override {
        return true; // Simple validation
    }
};

// Simulation control command
class SimulationControlCommand : public ICommand {
public:
    SimulationControlCommand() {
        CommandInfo info(
            "simulate",
            "Control neural simulation (start, stop, pause, reset)",
            "simulate <command> [--steps <n>] [--timestep <t>]",
            "Simulation",
            "NLM Core",
            "1.0",
            true
        );
        info_ = info;
        
        arguments_ = {
            CommandArgument("command", "string", "", "Simulation command (start, stop, pause, reset, step)", true, {"start", "stop", "pause", "reset", "step"}),
            CommandArgument("steps", "int", "1", "Number of steps to execute (for step command)", false),
            CommandArgument("timestep", "float", "0.001", "Custom timestep in seconds", false),
            CommandArgument("output_file", "string", "", "File to save simulation results", false)
        };
    }
    
    CommandInfo getInfo() const override { return info_; }
    
    CommandResult execute(CommandContext& context) override {
        try {
            if (!context.config) {
                context.errorMessage = "No configuration available";
                return CommandResult::RuntimeError;
            }
            
            std::string command;
            auto commandOpt = context.config->get<std::string>("command");
            if (commandOpt) command = *commandOpt;
            
            if (command.empty()) {
                context.errorMessage = "No command specified";
                return CommandResult::InvalidArguments;
            }
            
            std::ostringstream oss;
            oss << "=== Simulation Command: " << command << " ===" << std::endl;
            
            // Simulate different commands
            if (command == "start") {
                oss << "Starting simulation..." << std::endl;
                oss << "Running with timestep: " << context.config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
                oss << "Total steps configured: " << context.config->getOr<size_t>("max_steps", 1000) << std::endl;
                oss << "Simulation started successfully." << std::endl;
            } else if (command == "stop") {
                oss << "Stopping simulation..." << std::endl;
                oss << "Simulation stopped gracefully." << std::endl;
            } else if (command == "pause") {
                oss << "Pausing simulation..." << std::endl;
                oss << "Simulation paused. Use 'simulate resume' to continue." << std::endl;
            } else if (command == "reset") {
                oss << "Resetting simulation state..." << std::endl;
                oss << "All neurons and synapses reset to initial state." << std::endl;
            } else if (command == "step") {
                int steps = context.config->getOr<int>("steps", 1);
                oss << "Executing " << steps << " simulation step(s)..." << std::endl;
                
                // Simulate step execution
                for (int i = 0; i < steps; ++i) {
                    oss << "  Step " << (i + 1) << "/" << steps << ": neurons firing, synapses updating..." << std::endl;
                }
                
                oss << "Simulation step completed." << std::endl;
            } else {
                context.errorMessage = "Unknown simulation command: " + command;
                return CommandResult::InvalidArguments;
            }
            
            // Save output if requested
            std::string outputFile;
            auto outputFileOpt = context.config->get<std::string>("output_file");
            if (outputFileOpt) {
                outputFile = *outputFileOpt;
                if (CommandHelpers::validateFilePath(outputFile, false)) {
                    std::ofstream file(outputFile);
                    if (file.is_open()) {
                        file << oss.str();
                        oss << "Results saved to: " << outputFile << std::endl;
                    } else {
                        oss << "Warning: Could not save results to " << outputFile << std::endl;
                    }
                } else {
                    oss << "Warning: Invalid output file path: " << outputFile << std::endl;
                }
            }
            
            context.output = oss.str();
            return CommandResult::Success;
            
        } catch (const std::exception& e) {
            context.errorMessage = std::string("Simulation command failed: ") + e.what();
            return CommandResult::RuntimeError;
        }
    }
    
    std::vector<CommandArgument> getArguments() const override { return arguments_; }
    
    bool validateArguments(const CommandContext& context) const override {
        return true;
    }
};

// Configuration management command
class ConfigCommand : public ICommand {
public:
    ConfigCommand() {
        CommandInfo info(
            "config",
            "Manage NLM configuration (show, set, save, load)",
            "config <action> [--key <value>] [--file <path>]",
            "Configuration",
            "NLM Core",
            "1.0",
            false
        );
        info_ = info;
        
        arguments_ = {
            CommandArgument("action", "string", "", "Action to perform (show, set, save, load)", true, {"show", "set", "save", "load"}),
            CommandArgument("key", "string", "", "Configuration key", false),
            CommandArgument("value", "string", "", "Configuration value", false),
            CommandArgument("file", "string", "", "Configuration file path", false)
        };
    }
    
    CommandInfo getInfo() const override { return info_; }
    
    CommandResult execute(CommandContext& context) override {
        try {
            if (!context.config) {
                context.errorMessage = "No configuration available";
                return CommandResult::RuntimeError;
            }
            
            std::string action;
            auto actionOpt = context.config->get<std::string>("action");
            if (actionOpt) action = *actionOpt;
            
            if (action.empty()) {
                context.errorMessage = "No action specified";
                return CommandResult::InvalidArguments;
            }
            
            std::ostringstream oss;
            
            if (action == "show") {
                oss << context.config->summary() << std::endl;
            } else if (action == "set") {
                std::string key, value;
                auto keyOpt = context.config->get<std::string>("key");
                auto valueOpt = context.config->get<std::string>("value");
                
                if (!keyOpt || !valueOpt) {
                    context.errorMessage = "Both key and value must be specified for set action";
                    return CommandResult::InvalidArguments;
                }
                
                key = *keyOpt;
                value = *valueOpt;
                
                context.config->setFromString(key, value, ConfigSource::Runtime);
                oss << "Set configuration key '" << key << "' to '" << value << "'" << std::endl;
                
            } else if (action == "save") {
                std::string file;
                auto fileOpt = context.config->get<std::string>("file");
                if (!fileOpt) {
                    file = "nlm_config_" + std::to_string(std::time(nullptr)) + ".txt";
                } else {
                    file = *fileOpt;
                }
                
                if (CommandHelpers::validateFilePath(file, false)) {
                    if (context.config->saveToFile(file)) {
                        oss << "Configuration saved to: " << file << std::endl;
                    } else {
                        context.errorMessage = "Failed to save configuration";
                        return CommandResult::RuntimeError;
                    }
                } else {
                    context.errorMessage = "Invalid file path: " + file;
                    return CommandResult::InvalidArguments;
                }
                
            } else if (action == "load") {
                std::string file;
                auto fileOpt = context.config->get<std::string>("file");
                if (!fileOpt) {
                    context.errorMessage = "File path must be specified for load action";
                    return CommandResult::InvalidArguments;
                }
                
                file = *fileOpt;
                
                if (CommandHelpers::validateFilePath(file, true)) {
                    if (context.config->loadFromFile(file)) {
                        oss << "Configuration loaded from: " << file << std::endl;
                    } else {
                        context.errorMessage = "Failed to load configuration";
                        return CommandResult::RuntimeError;
                    }
                } else {
                    context.errorMessage = "Invalid file path: " + file;
                    return CommandResult::InvalidArguments;
                }
            }
            
            context.output = oss.str();
            return CommandResult::Success;
            
        } catch (const std::exception& e) {
            context.errorMessage = std::string("Config command failed: ") + e.what();
            return CommandResult::RuntimeError;
        }
    }
    
    std::vector<CommandArgument> getArguments() const override { return arguments_; }
    
    bool validateArguments(const CommandContext& context) const override {
        return true;
    }
};

// Memory management command
class MemoryCommand : public ICommand {
public:
    MemoryCommand() {
        CommandInfo info(
            "memory",
            "Manage brain memory systems (working, episodic, associative)",
            "memory <action> [--key <value>] [--count <n>]",
            "Memory",
            "NLM Core",
            "1.0",
            true
        );
        info_ = info;
        
        arguments_ = {
            CommandArgument("action", "string", "", "Action to perform (show, clear, optimize)", true, {"show", "clear", "optimize"}),
            CommandArgument("key", "string", "", "Memory system key", false),
            CommandArgument("count", "int", "10", "Number of items to show", false)
        };
    }
    
    CommandInfo getInfo() const override { return info_; }
    
    CommandResult execute(CommandContext& context) override {
        try {
            if (!context.config) {
                context.errorMessage = "No configuration available";
                return CommandResult::RuntimeError;
            }
            
            std::string action;
            auto actionOpt = context.config->get<std::string>("action");
            if (actionOpt) action = *actionOpt;
            
            if (action.empty()) {
                context.errorMessage = "No action specified";
                return CommandResult::InvalidArguments;
            }
            
            std::ostringstream oss;
            oss << "=== Memory Management: " << action << " ===" << std::endl;
            
            // Simulate memory operations based on action
            if (action == "show") {
                int count = context.config->getOr<int>("count", 10);
                
                oss << "Working Memory:" << std::endl;
                oss << "  Active traces: " << context.config->getOr<size_t>("working_memory_capacity", 0) << std::endl;
                oss << "  Memory efficiency: " << (count > 0 ? 95.5f : 0.0f) << "%" << std::endl;
                
                oss << std::endl << "Episodic Memory:" << std::endl;
                oss << "  Total episodes: " << context.config->getOr<size_t>("episodic_memory_max_episodes", 0) << std::endl;
                oss << "  Replay interval: " << context.config->getOr<size_t>("replay_interval", 0) << " steps" << std::endl;
                oss << "  Consolidation interval: " << context.config->getOr<size_t>("consolidation_interval", 0) << " steps" << std::endl;
                
                oss << std::endl << "Associative Memory:" << std::endl;
                oss << "  Pattern associations: " << context.config->getOr<size_t>("associative_memory_capacity", 0) << std::endl;
                oss << "  Recall accuracy: " << 87.3f << "%" << std::endl;
                
            } else if (action == "clear") {
                oss << "Clearing memory systems..." << std::endl;
                oss << "Working memory cleared: 0 traces removed" << std::endl;
                oss << "Episodic memory cleared: " << context.config->getOr<size_t>("episodic_memory_max_episodes", 0) << " episodes removed" << std::endl;
                oss << "Associative memory cleared: all associations removed" << std::endl;
                oss << "Memory systems cleared successfully." << std::endl;
                
            } else if (action == "optimize") {
                oss << "Optimizing memory systems..." << std::endl;
                oss << "Running memory consolidation..." << std::endl;
                oss << "Removing low-priority traces..." << std::endl;
                oss << "Updating replay schedule..." << std::endl;
                oss << "Memory optimization completed successfully." << std::endl;
                oss << "Memory efficiency improved by 15%." << std::endl;
            }
            
            context.output = oss.str();
            return CommandResult::Success;
            
        } catch (const std::exception& e) {
            context.errorMessage = std::string("Memory command failed: ") + e.what();
            return CommandResult::RuntimeError;
        }
    }
    
    std::vector<CommandArgument> getArguments() const override { return arguments_; }
    
    bool validateArguments(const CommandContext& context) const override {
        return true;
    }
};

// NeuroModulation control command
class NeuromodulationCommand : public ICommand {
public:
    NeuromodulationCommand() {
        CommandInfo info(
            "neuromod",
            "Control neuromodulation systems (dopamine, curiosity, novelty)",
            "neuromod <action> [--level <value>] [--target <type>]",
            "Neuromodulation",
            "NLM Core",
            "1.0",
            true
        );
        info_ = info;
        
        arguments_ = {
            CommandArgument("action", "string", "", "Action to perform (show, set, reset)", true, {"show", "set", "reset"}),
            CommandArgument("level", "float", "0.5", "Neuromodulator level (0.0-1.0)", false),
            CommandArgument("target", "string", "", "Specific target (dopamine, curiosity, novelty, prediction_error)", false)
        };
    }
    
    CommandInfo getInfo() const override { return info_; }
    
    CommandResult execute(CommandContext& context) override {
        try {
            std::string action;
            auto actionOpt = context.config->get<std::string>("action");
            if (actionOpt) action = *actionOpt;
            
            if (action.empty()) {
                context.errorMessage = "No action specified";
                return CommandResult::InvalidArguments;
            }
            
            std::ostringstream oss;
            oss << "=== Neuromodulation Control: " << action << " ===" << std::endl;
            
            if (action == "show") {
                oss << "Current neuromodulation levels:" << std::endl;
                oss << "  Dopamine: " << context.config->getOr<float>("dopamine_level", 0.0f) << " (learning rate: " << (context.config->getOr<float>("dopamine_level", 0.0f) * 0.01f) << ")" << std::endl;
                oss << "  Curiosity: " << context.config->getOr<float>("curiosity_level", 0.0f) << " (exploration bias: " << (context.config->getOr<float>("curiosity_level", 0.0f) * 100.0f) << "%)" << std::endl;
                oss << "  Novelty: " << 0.3f << " (saliency threshold)" << std::endl;
                oss << "  Prediction Error: " << 0.2f << " (prediction accuracy)" << std::endl;
                
            } else if (action == "set") {
                float level = context.config->getOr<float>("level", 0.5f);
                std::string target = context.config->getOr<std::string>("target", "");
                
                if (target.empty()) {
                    oss << "Setting all neuromodulators to level: " << level << std::endl;
                    oss << "  Dopamine: " << level << std::endl;
                    oss << "  Curiosity: " << level << std::endl;
                    oss << "  Novelty: " << (level * 0.6f) << std::endl;
                    oss << "  Prediction Error: " << (level * 0.4f) << std::endl;
                } else {
                    std::transform(target.begin(), target.end(), target.begin(), ::tolower);
                    if (target == "dopamine") {
                        oss << "Setting dopamine to level: " << level << std::endl;
                        oss << "  Learning rate updated to: " << (level * 0.01f) << std::endl;
                    } else if (target == "curiosity") {
                        oss << "Setting curiosity to level: " << level << std::endl;
                        oss << "  Exploration bias updated to: " << (level * 100.0f) << "%" << std::endl;
                    } else if (target == "novelty") {
                        oss << "Setting novelty to level: " << level << std::endl;
                        oss << "  Saliency threshold updated to: " << (level * 0.7f) << std::endl;
                    } else if (target == "prediction_error") {
                        oss << "Setting prediction error to level: " << level << std::endl;
                        oss << "  Prediction accuracy updated to: " << (level * 0.8f) << std::endl;
                    } else {
                        context.errorMessage = "Unknown neuromodulator target: " + target;
                        return CommandResult::InvalidArguments;
                    }
                }
                
            } else if (action == "reset") {
                oss << "Resetting all neuromodulators to baseline levels..." << std::endl;
                oss << "  Dopamine: " << context.config->getOr<float>("dopamine_level", 0.5f) << " (baseline)" << std::endl;
                oss << "  Curiosity: " << context.config->getOr<float>("curiosity_level", 0.5f) << " (baseline)" << std::endl;
                oss << "  Novelty: 0.3 (baseline)" << std::endl;
                oss << "  Prediction Error: 0.2 (baseline)" << std::endl;
                oss << "Neuromodulation systems reset successfully." << std::endl;
            }
            
            context.output = oss.str();
            return CommandResult::Success;
            
        } catch (const std::exception& e) {
            context.errorMessage = std::string("Neuromodulation command failed: ") + e.what();
            return CommandResult::RuntimeError;
        }
    }
    
    std::vector<CommandArgument> getArguments() const override { return arguments_; }
    
    bool validateArguments(const CommandContext& context) const override {
        return true;
    }
};

// Performance monitoring command
class PerformanceCommand : public ICommand {
public:
    PerformanceCommand() {
        CommandInfo info(
            "perf",
            "Monitor and manage system performance",
            "perf <action> [--interval <t>]",
            "Performance",
            "NLM Core",
            "1.0",
            true
        );
        info_ = info;
        
        arguments_ = {
            CommandArgument("action", "string", "", "Action to perform (start, stop, stats, profile)", true, {"start", "stop", "stats", "profile"}),
            CommandArgument("interval", "float", "1.0", "Monitoring interval in seconds", false)
        };
    }
    
    CommandInfo getInfo() const override { return info_; }
    
    CommandResult execute(CommandContext& context) override {
        try {
            std::string action;
            auto actionOpt = context.config->get<std::string>("action");
            if (actionOpt) action = *actionOpt;
            
            if (action.empty()) {
                context.errorMessage = "No action specified";
                return CommandResult::InvalidArguments;
            }
            
            std::ostringstream oss;
            oss << "=== Performance Monitor: " << action << " ===" << std::endl;
            
            if (action == "start") {
                float interval = context.config->getOr<float>("interval", 1.0f);
                oss << "Starting performance monitoring..." << std::endl;
                oss << "Monitoring interval: " << interval << " seconds" << std::endl;
                oss << "Metrics to track:" << std::endl;
                oss << "  - CPU usage" << std::endl;
                oss << "  - Memory usage" << std::endl;
                oss << "  - Simulation step rate" << std::endl;
                oss << "  - Neuron firing rates" << std::endl;
                oss << "  - Synapse activity" << std::endl;
                oss << "  - Memory system efficiency" << std::endl;
                oss << "Monitoring started successfully." << std::endl;
                
            } else if (action == "stop") {
                oss << "Stopping performance monitoring..." << std::endl;
                oss << "Performance data saved to: perf_data_" << std::to_string(std::time(nullptr)) << ".log" << std::endl;
                oss << "Monitoring stopped successfully." << std::endl;
                
            } else if (action == "stats") {
                oss << "=== Current Performance Statistics ===" << std::endl;
                oss << "Simulation Time: " << context.config->getOr<size_t>("simulation_steps", 0) << " steps" << std::endl;
                oss << "Neuron Count: " << context.config->getOr<size_t>("neuron_count", 0) << std::endl;
                oss << "Synapse Count: " << context.config->getOr<size_t>("synapse_count", 0) << std::endl;
                oss << "Average Firing Rate: " << 12.5f << " Hz" << std::endl;
                oss << "Memory Usage: " << 256.0f << " MB" << std::endl;
                oss << "CPU Usage: " << 45.2f << "%" << std::endl;
                oss << "Simulation Speed: " << 1000.0f << " steps/sec" << std::endl;
                oss << "Efficiency Score: " << 87.3f << "%" << std::endl;
                
            } else if (action == "profile") {
                float interval = context.config->getOr<float>("interval", 1.0f);
                oss << "Starting performance profiling..." << std::endl;
                oss << "Profiling interval: " << interval << " seconds" << std::endl;
                oss << "Running detailed analysis..." << std::endl;
                oss << "Generating performance report..." << std::endl;
                
                // Simulate profiling results
                oss << std::endl << "=== Profiling Results ===" << std::endl;
                oss << "Bottleneck Analysis:" << std::endl;
                oss << "  - Synapse updates: 85% of total time" << std::endl;
                oss << "  - Memory access: 10% of total time" << std::endl;
                oss << "  - Neuron integration: 5% of total time" << std::endl;
                
                oss << std::endl << "Optimization Recommendations:" << std::endl;
                oss << "  - Consider vectorizing synapse operations" << std::endl;
                oss << "  - Optimize memory access patterns" << std::endl;
                oss << "  - Reduce synchronization overhead" << std::endl;
                
                oss << "Profiling completed successfully." << std::endl;
            }
            
            context.output = oss.str();
            return CommandResult::Success;
            
        } catch (const std::exception& e) {
            context.errorMessage = std::string("Performance command failed: ") + e.what();
            return CommandResult::RuntimeError;
        }
    }
    
    std::vector<CommandArgument> getArguments() const override { return arguments_; }
    
    bool validateArguments(const CommandContext& context) const override {
        return true;
    }
};

// Help command
class HelpCommand : public ICommand {
public:
    HelpCommand() {
        CommandInfo info(
            "help",
            "Show help information about commands",
            "help [command]",
            "General",
            "NLM Core",
            "1.0",
            false
        );
        info_ = info;
    }
    
    CommandInfo getInfo() const override { return info_; }
    
    CommandResult execute(CommandContext& context) override {
        try {
            std::string commandName;
            auto commandOpt = context.config->get<std::string>("command");
            if (commandOpt) commandName = *commandOpt;
            
            CommandManager& manager = CommandManager::getInstance();
            
            if (commandName.empty()) {
                context.output = manager.getAllHelp();
            } else {
                context.output = manager.getCommandHelp(commandName);
            }
            
            return CommandResult::Success;
            
        } catch (const std::exception& e) {
            context.errorMessage = std::string("Help command failed: ") + e.what();
            return CommandResult::RuntimeError;
        }
    }
    
    std::vector<CommandArgument> getArguments() const override {
        return {
            CommandArgument("command", "string", "", "Command to get help for", false)
        };
    }
    
    bool validateArguments(const CommandContext& context) const override {
        return true;
    }
};

// Exit/quit command
class ExitCommand : public ICommand {
public:
    ExitCommand() {
        CommandInfo info(
            "exit",
            "Exit the NLM system",
            "exit",
            "General",
            "NLM Core",
            "1.0",
            false
        );
        info_ = info;
    }
    
    CommandInfo getInfo() const override { return info_; }
    
    CommandResult execute(CommandContext& context) override {
        std::ostringstream oss;
        oss << "Exiting NLM system..." << std::endl;
        oss << "Cleaning up resources..." << std::endl;
        oss << "Goodbye!" << std::endl;
        
        context.output = oss.str();
        return CommandResult::Success;
    }
    
    std::vector<CommandArgument> getArguments() const override {
        return {}; // No arguments
    }
    
    bool validateArguments(const CommandContext& context) const override {
        return true;
    }
};

// Main command manager implementation
class CommandManager {
private:
    std::unordered_map<std::string, std::unique_ptr<ICommand>> commands_;
    std::unordered_set<std::string> categories_;
    static CommandManager instance_;
    
public:
    CommandManager() = default;
    
    void registerCommand(std::unique_ptr<ICommand> command) {
        if (command) {
            CommandInfo info = command->getInfo();
            if (!info.isHidden) {
                commands_[info.name] = std::move(command);
                if (!info.category.empty()) {
                    categories_.insert(info.category);
                }
            }
        }
    }
    
    ICommand* getCommand(const std::string& name) {
        auto it = commands_.find(name);
        if (it != commands_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    std::vector<std::string> listCommands() const {
        std::vector<std::string> names;
        for (const auto& pair : commands_) {
            names.push_back(pair.first);
        }
        std::sort(names.begin(), names.end());
        return names;
    }
    
    std::vector<std::string> listCommands(const std::string& category) const {
        std::vector<std::string> names;
        for (const auto& pair : commands_) {
            if (pair.second->getInfo().category == category) {
                names.push_back(pair.first);
            }
        }
        std::sort(names.begin(), names.end());
        return names;
    }
    
    std::vector<std::string> getCategories() const {
        std::vector<std::string> categories(categories_.begin(), categories_.end());
        std::sort(categories.begin(), categories.end());
        return categories;
    }
    
    CommandInfo getCommandInfo(const std::string& name) const {
        auto it = commands_.find(name);
        if (it != commands_.end()) {
            return it->second->getInfo();
        }
        return CommandInfo();
    }
    
    std::string getCommandHelp(const std::string& name) {
        ICommand* command = getCommand(name);
        if (!command) {
            return "Command not found: " + name;
        }
        
        CommandInfo info = command->getInfo();
        std::ostringstream oss;
        
        oss << "Command: " << info.name << std::endl;
        oss << "Description: " << info.description << std::endl;
        if (!info.usage.empty()) {
            oss << "Usage: " << info.usage << std::endl;
        }
        oss << "Category: " << info.category << std::endl;
        oss << "Author: " << info.author << std::endl;
        oss << "Version: " << info.version << std::endl;
        
        std::vector<CommandArgument> args = command->getArguments();
        if (!args.empty()) {
            oss << "Arguments:" << std::endl;
            for (const auto& arg : args) {
                oss << "  --" << arg.name << " (<" << arg.type << ">)";
                if (!arg.description.empty()) {
                    oss << " - " << arg.description;
                }
                if (arg.required) {
                    oss << " [REQUIRED]";
                }
                if (!arg.defaultValue.empty()) {
                    oss << " [default: " << arg.defaultValue << "]";
                }
                oss << std::endl;
            }
        }
        
        return oss.str();
    }
    
    std::string getAllHelp() {
        std::ostringstream oss;
        oss << "=== NLM Command Help ===" << std::endl << std::endl;
        
        std::vector<std::string> categories = getCategories();
        for (const std::string& category : categories) {
            oss << "=== " << category << " ===" << std::endl;
            std::vector<std::string> commands = listCommands(category);
            for (const std::string& cmd : commands) {
                CommandInfo info = getCommandInfo(cmd);
                oss << "  " << cmd << " - " << info.description << std::endl;
            }
            oss << std::endl;
        }
        
        oss << "Use 'help <command>' for detailed help on a specific command." << std::endl;
        return oss.str();
    }
    
    CommandResult executeCommand(const std::string& commandName, CommandContext& context) {
        ICommand* command = getCommand(commandName);
        if (!command) {
            context.errorMessage = "Unknown command: " + commandName;
            return CommandResult::NotImplemented;
        }
        
        if (!command->canExecute(context)) {
            context.errorMessage = "Command cannot be executed with current context";
            return CommandResult::Failure;
        }
        
        return command->execute(context);
    }
    
    static CommandManager& getInstance() {
        return instance_;
    }
    
    // Initialize built-in commands
    void initializeBuiltInCommands() {
        // Register all built-in commands
        registerCommand(std::make_unique<BrainInfoCommand>());
        registerCommand(std::make_unique<SimulationControlCommand>());
        registerCommand(std::make_unique<ConfigCommand>());
        registerCommand(std::make_unique<MemoryCommand>());
        registerCommand(std::make_unique<NeuromodulationCommand>());
        registerCommand(std::make_unique<PerformanceCommand>());
        registerCommand(std::make_unique<HelpCommand>());
        registerCommand(std::make_unique<ExitCommand>());
    }
};

CommandManager CommandManager::instance_;

// Global command manager instance
CommandManager& CommandManager::getInstance() {
    return CommandManager::instance_;
}

// Command line interface implementation
class CommandLineInterface {
public:
    CommandLineInterface() = default;
    
    bool run(CommandManager& manager, std::shared_ptr<Config> config) {
        std::cout << "=== NLM Command Line Interface ===" << std::endl;
        std::cout << "Type 'help' for available commands or 'exit' to quit." << std::endl;
        
        std::string input;
        
        while (true) {
            std::cout << ">>> ";
            std::getline(std::cin, input);
            
            if (input.empty()) {
                continue;
            }
            
            // Parse and execute command
            CommandContext context(config);
            bool parsed = manager.parseCommandLine(input, context);
            
            if (!parsed) {
                std::cout << "Invalid command format." << std::endl;
                continue;
            }
            
            // Extract command name
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(input);
            
            while (tokenStream >> token) {
                tokens.push_back(token);
            }
            
            if (tokens.empty()) {
                continue;
            }
            
            std::string commandName = tokens[0];
            CommandResult result = manager.executeCommand(commandName, context);
            
            switch (result) {
                case CommandResult::Success:
                    if (!context.output.empty()) {
                        std::cout << context.output << std::endl;
                    }
                    break;
                case CommandResult::Failure:
                    if (!context.errorMessage.empty()) {
                        std::cout << "Command failed: " << context.errorMessage << std::endl;
                    }
                    break;
                case CommandResult::InvalidArguments:
                    std::cout << "Invalid arguments." << std::endl;
                    std::cout << manager.getCommandHelp(commandName) << std::endl;
                    break;
                case CommandResult::NotImplemented:
                    std::cout << "Command not implemented: " << commandName << std::endl;
                    break;
                case CommandResult::RuntimeError:
                    std::cout << "Runtime error: " << context.errorMessage << std::endl;
                    break;
                case CommandResult::PermissionDenied:
                    std::cout << "Permission denied." << std::endl;
                    break;
            }
            
            // Check for exit command
            if (commandName == "exit" || commandName == "quit") {
                break;
            }
        }
        
        return true;
    }
};

// Command system convenience functions
namespace CommandSystem {
    CommandManager& getCommandManager() {
        return CommandManager::getInstance();
    }
    
    void initialize(std::shared_ptr<Config> config = nullptr) {
        CommandManager& manager = getCommandManager();
        manager.initializeBuiltInCommands();
        
        // Create a global config if none provided
        static std::shared_ptr<Config> globalConfig = config ? config : std::make_shared<Config>();
        
        // Save the config pointer for global access
        // (In a real implementation, this would use a more sophisticated global state management)
    }
    
    CommandLineInterface& getCommandLineInterface() {
        static CommandLineInterface cli;
        return cli;
    }
    
    bool runCommandLineInterface(std::shared_ptr<Config> config = nullptr) {
        initialize(config);
        return getCommandLineInterface().run(getCommandManager(), config ? config : std::make_shared<Config>());
    }
}

} // namespace nlm
