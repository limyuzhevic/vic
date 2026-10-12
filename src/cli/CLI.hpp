#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <iostream>

namespace nlm {

// Command types
enum class CommandCategory {
    Agent,
    Configuration,
    Experiment,
    Visualization,
    System,
    Help
};

// Command structure for CLI
struct Command {
    std::string name;
    std::string description;
    std::string usage;
    CommandCategory category;
    bool requiresArgs;
    std::function<bool(const std::vector<std::string>&)> handler;
    std::vector<std::string> aliases;
    
    Command() : category(CommandCategory::Help), requiresArgs(false) {}
    
    Command(const std::string& name, const std::string& desc, const std::string& usage,
            CommandCategory cat, bool requiresArgs = false)
        : name(name), description(desc), usage(usage), category(cat), requiresArgs(requiresArgs) {}
};

// CLI help system
class CLIHelpSystem {
public:
    CLIHelpSystem() {
        initializeHelpTopics();
    }
    
    void initializeHelpTopics() {
        helpTopics_ = {
            {"agent", "Commands for controlling the agent"},
            {"config", "Commands for configuration management"},
            {"experiment", "Commands for running experiments"},
            {"visual", "Commands for visualization"},
            {"system", "System management commands"},
            {"general", "General commands"},
            {"agent start", "Start the agent simulation"},
            {"agent stop", "Stop the agent simulation"},
            {"agent status", "Get agent status and statistics"},
            {"agent reset", "Reset agent state"},
            {"agent capabilities", "View agent capabilities"},
            {"agent commands", "List available agent commands"},
            {"config load", "Load configuration from file"},
            {"config save", "Save configuration to file"},
            {"config show", "Show current configuration"},
            {"config set", "Set configuration value"},
            {"config validate", "Validate configuration"},
            {"experiment run", "Run an experiment"},
            {"experiment list", "List available experiments"},
            {"experiment status", "Get experiment status"},
            {"help", "Show help information"},
            {"help <topic>", "Show detailed help for a topic"},
            {"version", "Show version information"},
            {"exit", "Exit the application"},
            {"quit", "Quit the application"}
        };
    }
    
    void showHelp(const std::string& topic = "") const {
        if (topic.empty()) {
            showGeneralHelp();
        } else {
            showSpecificHelp(topic);
        }
    }
    
    void showGeneralHelp() const {
        std::cout << "=== NLM Command Line Interface ===" << std::endl;
        std::cout << "Available commands:" << std::endl;
        
        // Group commands by category
        std::unordered_map<CommandCategory, std::vector<const Command*>> groupedCommands;
        for (const auto& cmd : commands_) {
            groupedCommands[cmd->category].push_back(cmd.get());
        }
        
        // Display commands by category
        for (const auto& group : groupedCommands) {
            std::cout << "\n" << categoryToString(group.first) << ":" << std::endl;
            for (const auto* cmd : group.second) {
                std::cout << "  " << cmd->name << " - " << cmd->description << std::endl;
                if (!cmd->usage.empty()) {
                    std::cout << "      Usage: " << cmd->usage << std::endl;
                }
            }
        }
        
        std::cout << "\nFor more information on a specific command, use: help <command>" << std::endl;
        std::cout << "=== End of Help ===" << std::endl;
    }
    
    void showSpecificHelp(const std::string& topic) const {
        // Try exact match first
        auto it = helpTopics_.find(topic);
        if (it != helpTopics_.end()) {
            std::cout << "=== Help: " << topic << " ===" << std::endl;
            std::cout << it->second << std::endl;
            return;
        }
        
        // Try command-specific help
        for (const auto& cmd : commands_) {
            if (cmd->name == topic || 
                std::find(cmd->aliases.begin(), cmd->aliases.end(), topic) != cmd->aliases.end()) {
                std::cout << "=== Help: " << cmd->name << " ===" << std::endl;
                std::cout << cmd->description << std::endl;
                std::cout << "Usage: " << cmd->usage << std::endl;
                if (cmd->requiresArgs) {
                    std::cout << "Arguments: Required (see usage for details)" << std::endl;
                }
                return;
            }
        }
        
        // Try topic prefix (e.g., "agent" for "agent start", "agent status", etc.)
        for (const auto& pair : helpTopics_) {
            if (pair.first.find(topic) == 0) {
                std::cout << "=== Help: " << pair.first << " ===" << std::endl;
                std::cout << pair.second << std::endl;
            }
        }
        
        if (it == helpTopics_.end() && std::find(helpTopics_.begin(), helpTopics_.end(), topic) == helpTopics_.end()) {
            std::cout << "=== Unknown topic: " << topic << " ===" << std::endl;
            std::cout << "Type 'help' for a list of available topics." << std::endl;
        }
    }
    
    void addHelpTopic(const std::string& topic, const std::string& description) {
        helpTopics_[topic] = description;
    }
    
    bool hasHelpTopic(const std::string& topic) const {
        return helpTopics_.find(topic) != helpTopics_.end();
    }
    
    void addCommand(std::unique_ptr<Command> cmd) {
        commands_.push_back(std::move(cmd));
        
        // Add to help system
        std::string topicName = cmd->name;
        if (!cmd->aliases.empty()) {
            topicName = cmd->aliases[0]; // Use first alias as topic name
        }
        addHelpTopic(topicName, cmd->description);
    }
    
    Command* getCommand(const std::string& name) {
        for (auto& cmd : commands_) {
            if (cmd->name == name) {
                return cmd.get();
            }
        }
        return nullptr;
    }
    
    std::vector<std::string> getCommandsByCategory(CommandCategory category) const {
        std::vector<std::string> cmdNames;
        for (const auto& cmd : commands_) {
            if (cmd->category == category) {
                cmdNames.push_back(cmd->name);
            }
        }
        return cmdNames;
    }
    
private:
    std::vector<std::unique_ptr<Command>> commands_;
    std::unordered_map<std::string, std::string> helpTopics_;
    
    std::string categoryToString(CommandCategory category) const {
        switch (category) {
            case CommandCategory::Agent: return "Agent Commands";
            case CommandCategory::Configuration: return "Configuration Commands";
            case CommandCategory::Experiment: return "Experiment Commands";
            case CommandCategory::Visualization: return "Visualization Commands";
            case CommandCategory::System: return "System Commands";
            case CommandCategory::Help: return "Help Commands";
            default: return "Unknown Commands";
        }
    }
};

// Main CLI interface
class CLI {
public:
    CLI() : helpSystem_(), running_(false) {
        initializeCommands();
    }
    
    void run(int argc, char** argv) {
        if (argc <= 1) {
            showHelp();
            return;
        }
        
        std::string command = argv[1];
        std::vector<std::string> args;
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        executeCommand(command, args);
    }
    
    void showHelp() const {
        helpSystem_.showHelp();
    }
    
    void start() {
        running_ = true;
        std::cout << "=== NLM CLI Started ===" << std::endl;
        std::cout << "Type 'help' for available commands." << std::endl;
        
        std::string input;
        while (running_) {
            std::cout << "nlm> ";
            std::getline(std::cin, input);
            
            if (input.empty()) {
                continue;
            }
            
            processInput(input);
        }
    }
    
    void stop() {
        running_ = false;
        std::cout << "=== NLM CLI Stopped ===" << std::endl;
    }
    
    void addCommand(std::unique_ptr<Command> cmd) {
        helpSystem_.addCommand(std::move(cmd));
    }
    
private:
    CLIHelpSystem helpSystem_;
    bool running_;
    std::unordered_map<std::string, std::function<bool(const std::vector<std::string>&)>> commandHandlers_;
    
    void initializeCommands() {
        // Agent commands
        addCommand(std::make_unique<Command>(
            "agent", "Manage agent operations", "agent <command> [args]",
            CommandCategory::Agent, true
        ));
        
        addCommand(std::make_unique<Command>(
            "start", "Start the agent simulation", "start [--config file] [--duration time]",
            CommandCategory::Agent, false
        ));
        
        addCommand(std::make_unique<Command>(
            "stop", "Stop the agent simulation", "stop",
            CommandCategory::Agent, false
        ));
        
        addCommand(std::make_unique<Command>(
            "status", "Get agent status and statistics", "status [--verbose]",
            CommandCategory::Agent, false
        ));
        
        addCommand(std::make_unique<Command>(
            "reset", "Reset agent state", "reset [--hard]",
            CommandCategory::Agent, false
        ));
        
        addCommand(std::make_unique<Command>(
            "capabilities", "View agent capabilities", "capabilities [--format json|table]",
            CommandCategory::Agent, false
        ));
        
        addCommand(std::make_unique<Command>(
            "commands", "List available agent commands", "commands [--category category]",
            CommandCategory::Agent, false
        ));
        
        // Configuration commands
        addCommand(std::make_unique<Command>(
            "config", "Manage configuration", "config <command> [args]",
            CommandCategory::Configuration, true
        ));
        
        addCommand(std::make_unique<Command>(
            "load", "Load configuration from file", "load <file> [--format json|yaml]",
            CommandCategory::Configuration, false
        ));
        
        addCommand(std::make_unique<Command>(
            "save", "Save configuration to file", "save <file> [--format json|yaml]",
            CommandCategory::Configuration, false
        ));
        
        addCommand(std::make_unique<Command>(
            "show", "Show current configuration", "show [--section section]",
            CommandCategory::Configuration, false
        ));
        
        addCommand(std::make_unique<Command>(
            "set", "Set configuration value", "set <key> <value> [--source source]",
            CommandCategory::Configuration, false
        ));
        
        addCommand(std::make_unique<Command>(
            "validate", "Validate configuration", "validate",
            CommandCategory::Configuration, false
        ));
        
        // Experiment commands
        addCommand(std::make_unique<Command>(
            "experiment", "Manage experiments", "experiment <command> [args]",
            CommandCategory::Experiment, true
        ));
        
        addCommand(std::make_unique<Command>(
            "run", "Run an experiment", "run <experiment> [--config file] [--steps steps]",
            CommandCategory::Experiment, false
        ));
        
        addCommand(std::make_unique<Command>(
            "list", "List available experiments", "list [--type type]",
            CommandCategory::Experiment, false
        ));
        
        addCommand(std::make_unique<Command>(
            "status", "Get experiment status", "status [--id id]",
            CommandCategory::Experiment, false
        ));
        
        // Visualization commands
        addCommand(std::make_unique<Command>(
            "visual", "Manage visualization", "visual <command> [args]",
            CommandCategory::Visualization, true
        ));
        
        addCommand(std::make_unique<Command>(
            "start", "Start visualization", "start [--port port] [--host host]",
            CommandCategory::Visualization, false
        ));
        
        addCommand(std::make_unique<Command>(
            "stop", "Stop visualization", "stop",
            CommandCategory::Visualization, false
        ));
        
        // System commands
        addCommand(std::make_unique<Command>(
            "system", "System management", "system <command> [args]",
            CommandCategory::System, true
        ));
        
        addCommand(std::make_unique<Command>(
            "info", "Show system information", "info",
            CommandCategory::System, false
        ));
        
        addCommand(std::make_unique<Command>(
            "logs", "View system logs", "logs [--level level] [--tail lines]",
            CommandCategory::System, false
        ));
        
        addCommand(std::make_unique<Command>(
            "version", "Show version information", "version",
            CommandCategory::System, false
        ));
        
        addCommand(std::make_unique<Command>(
            "exit", "Exit the application", "exit",
            CommandCategory::System, false
        ));
        
        addCommand(std::make_unique<Command>(
            "quit", "Quit the application", "quit",
            CommandCategory::System, false
        ));
        
        addCommand(std::make_unique<Command>(
            "help", "Show help information", "help [topic]",
            CommandCategory::Help, false
        ));
    }
    
    void executeCommand(const std::string& commandName, const std::vector<std::string>& args) {
        Command* cmd = helpSystem_.getCommand(commandName);
        if (!cmd) {
            std::cout << "=== Error: Unknown command: " << commandName << " ===" << std::endl;
            showHelp();
            return;
        }
        
        if (cmd->requiresArgs && args.empty()) {
            std::cout << "=== Error: Command requires arguments ===" << std::endl;
            std::cout << "Usage: " << cmd->usage << std::endl;
            return;
        }
        
        // Execute command
        if (cmd->handler) {
            if (cmd->handler(args)) {
                std::cout << "=== Command executed successfully ===" << std::endl;
            } else {
                std::cout << "=== Error: Command execution failed ===" << std::endl;
            }
        } else {
            std::cout << "=== Error: Command handler not implemented ===" << std::endl;
        }
    }
    
    void processInput(const std::string& input) {
        // Parse input into command and arguments
        std::istringstream iss(input);
        std::string command;
        iss >> command;
        
        std::vector<std::string> args;
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }
        
        executeCommand(command, args);
    }
};

}} // namespace nlm
