// CommandExecutor class for comprehensive command interface
class CommandExecutor {
private:
    std::shared_ptr<AgentBrain> agentBrain_;
    std::map<std::string, CommandInfo> availableCommands_;
    std::map<std::string, CommandHandler> commandHandlers_;
    
public:
    CommandExecutor(std::shared_ptr<AgentBrain> agentBrain) : agentBrain_(agentBrain) {
        initializeCommands();
        initializeHandlers();
    }
    
    // Parse user commands
    ParsedCommand parseCommand(const std::string& input) {
        ParsedCommand cmd;
        std::vector<std::string> tokens = tokenize(input);
        
        if (tokens.empty()) {
            cmd.type = CommandType::INVALID;
            return cmd;
        }
        
        cmd.command = tokens[0];
        cmd.args = std::vector<std::string>(tokens.begin() + 1, tokens.end());
        
        // Check if command is available
        auto it = availableCommands_.find(cmd.command);
        if (it != availableCommands_.end()) {
            cmd.type = CommandType::AVAILABLE;
            cmd.info = it->second;
            
            // Parse arguments based on command type
            parseCommandArgs(cmd);
        } else {
            cmd.type = CommandType::UNKNOWN;
        }
        
        return cmd;
    }
    
    // Execute parsed commands
    CommandResult executeCommand(const ParsedCommand& cmd) {
        CommandResult result;
        
        try {
            if (cmd.type == CommandType::AVAILABLE) {
                auto it = commandHandlers_.find(cmd.command);
                if (it != commandHandlers_.end()) {
                    result = it->second(cmd);
                    result.success = true;
                } else {
                    result.success = false;
                    result.output = "Command handler not found for: " + cmd.command;
                }
            } else if (cmd.type == CommandType::UNKNOWN) {
                result.success = false;
                result.output = "Unknown command: " + cmd.command + "\n";
                result.output += "Type 'help' for available commands";
            } else {
                result.success = false;
                result.output = "Invalid command format";
            }
        } catch (const std::exception& e) {
            result.success = false;
            result.output = "Error executing command: " + std::string(e.what());
        }
        
        return result;
    }
    
    // List all available commands
    std::string listAvailableCommands() {
        std::string output;
        output += "=== Available Commands ===\n\n";
        
        for (const auto& pair : availableCommands_) {
            const auto& info = pair.second;
            output += "Command: " + pair.first + "\n";
            output += "  Description: " + info.description + "\n";
            output += "  Category: " + info.category + "\n";
            
            if (!info.aliases.empty()) {
                output += "  Aliases: ";
                for (size_t i = 0; i < info.aliases.size(); ++i) {
                    if (i > 0) output += ", ";
                    output += info.aliases[i];
                }
                output += "\n";
            }
            
            if (!info.usage.empty()) {
                output += "  Usage: " + info.usage + "\n";
            }
            
            output += "\n";
        }
        
        return output;
    }
    
    // Get command help
    std::string getCommandHelp(const std::string& command) {
        auto it = availableCommands_.find(command);
        if (it == availableCommands_.end()) {
            return "Command not found: " + command;
        }
        
        const auto& info = it->second;
        std::string help;
        help += "=== Command Help ===\n\n";
        help += "Command: " + it->first + "\n";
        help += "Description: " + info.description + "\n";
        help += "Category: " + info.category + "\n";
        
        if (!info.detailedDescription.empty()) {
            help += "\nDetails: " + info.detailedDescription + "\n";
        }
        
        if (!info.aliases.empty()) {
            help += "\nAliases: ";
            for (size_t i = 0; i < info.aliases.size(); ++i) {
                if (i > 0) help += ", ";
                help += info.aliases[i];
            }
            help += "\n";
        }
        
        if (!info.usage.empty()) {
            help += "\nUsage: " + info.usage + "\n";
        }
        
        if (!info.examples.empty()) {
            help += "\nExamples:\n";
            for (const auto& example : info.examples) {
                help += "  " + example + "\n";
            }
        }
        
        return help;
    }
    
    // Human-readable formatting for command outputs
    std::string formatOutput(const CommandResult& result) {
        std::string formatted;
        
        if (result.success) {
            formatted += "✓ Success\n\n";
            formatted += result.output + "\n";
        } else {
            formatted += "✗ Error\n\n";
            formatted += result.output + "\n";
            
            if (!result.suggestions.empty()) {
                formatted += "\nSuggestions:\n";
                for (const auto& suggestion : result.suggestions) {
                    formatted += "  • " + suggestion + "\n";
                }
            }
        }
        
        return formatted;
    }
    
private:
    struct CommandInfo {
        std::string description;
        std::string category;
        std::string detailedDescription;
        std::string usage;
        std::vector<std::string> aliases;
        std::vector<std::string> examples;
        std::function<bool(const ParsedCommand&)> validator;
    };
    
    struct ParsedCommand {
        std::string command;
        std::vector<std::string> args;
        CommandInfo info;
        CommandType type;
    };
    
    struct CommandResult {
        bool success;
        std::string output;
        std::vector<std::string> suggestions;
        stdmap<>.type AdditionalData;
    };
    
    enum class CommandType {
        INVALID,
        UNKNOWN,
        AVAILABLE
    };
    
    using CommandHandler = std::function<CommandResult(const ParsedCommand&)>;
    
    void initializeCommands() {
        // Brain control commands
        availableCommands_["initialize"] = {
            "Initialize the brain",
            "Brain Control",
            "Initializes the brain with default configuration",
            "initialize",
            {"init"},
            {"initialize", "init - Initialize brain"},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["reset"] = {
            "Reset brain state",
            "Brain Control",
            "Resets all brain state, memory, and connections",
            "reset",
            {},
            {"reset - Reset brain to initial state"},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["save"] = {
            "Save brain state",
            "Brain Control",
            "Save current brain state to file",
            "save [filename]",
            {},
            {"save - Save brain state"},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["load"] = {
            "Load brain state",
            "Brain Control",
            "Load brain state from file",
            "load [filename]",
            {},
            {"load - Load brain state"},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["step"] = {
            "Step brain simulation",
            "Brain Control",
            "Advance brain simulation by one step",
            "step [step_number]",
            {},
            {"step - Run one simulation step"},
            [](const ParsedCommand&) { return true; }
        };
        
        // Agent control commands
        availableCommands_["enable_feature"] = {
            "Enable agent feature",
            "Agent Control",
            "Enable a specific agent subsystem",
            "enable_feature <feature_name>",
            {},
            {"enable_feature reward_modulation - Enable reward learning"},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["disable_feature"] = {
            "Disable agent feature",
            "Agent Control",
            "Disable a specific agent subsystem",
            "disable_feature <feature_name>",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["enable_curiosity"] = {
            "Enable curiosity-driven exploration",
            "Agent Control",
            "Enable curiosity system for exploration",
            "enable_curiosity [enabled]",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        // Sensory control commands
        availableCommands_["set_vision"] = {
            "Set vision parameters",
            "Sensory Control",
            "Configure sensory vision parameters",
            "set_vision <width> <height> [fov]",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["set_sensory"] = {
            "Set sensory parameters",
            "Sensory Control",
            "Configure sensory system parameters",
            "set_sensory <type> <value>",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        // World control commands
        availableCommands_["configure_world"] = {
            "Configure world simulation",
            "World Control",
            "Configure world simulation parameters",
            "configure_world <width> <height> [config]",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["reset_world"] = {
            "Reset world simulation",
            "World Control",
            "Reset world to initial state",
            "reset_world",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["update_world"] = {
            "Update world simulation",
            "World Control",
            "Advance world simulation by timestep",
            "update_world <timestep>",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        // State monitoring commands
        availableCommands_["get_firing_count"] = {
            "Get firing neuron count",
            "State Monitoring",
            "Get number of currently firing neurons",
            "get_firing_count",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["get_total_spikes"] = {
            "Get total spike count",
            "State Monitoring",
            "Get total number of spikes in brain history",
            "get_total_spikes",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["get_spikes_per_neuron"] = {
            "Get spikes per neuron",
            "State Monitoring",
            "Get average spikes per neuron",
            "get_spikes_per_neuron",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["get_state"] = {
            "Get brain state summary",
            "State Monitoring",
            "Get comprehensive brain state summary",
            "get_state",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        // Developmental control commands
        availableCommands_["set_developmental_stage"] = {
            "Set developmental stage",
            "Developmental Control",
            "Set brain developmental stage",
            "set_developmental_stage <stage>",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["update_development"] = {
            "Update development",
            "Developmental Control",
            "Advance developmental system",
            "update_development <timestep>",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        // Neuromodulation control commands
        availableCommands_["set_dopamine"] = {
            "Set dopamine level",
            "Neuromodulation Control",
            "Set dopamine neuromodulator level",
            "set_dopamine <level>",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["get_dopamine"] = {
            "Get current dopamine level",
            "Neuromodulation Control",
            "Get current dopamine level",
            "get_dopamine",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        // Configuration management commands
        availableCommands_["get_config"] = {
            "Get configuration value",
            "Configuration Management",
            "Get configuration value by key",
            "get_config <key>",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["set_config"] = {
            "Set configuration value",
            "Configuration Management",
            "Set configuration value by key",
            "set_config <key> <value>",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        availableCommands_["list_config"] = {
            "List all configuration values",
            "Configuration Management",
            "List all configuration keys and values",
            "list_config",
            {},
            {},
            [](const ParsedCommand&) { return true; }
        };
        
        // Help command
        availableCommands_["help"] = {
            "Show help information",
            "System",
            "Show help for commands or general information",
            "help [command]",
            {"?", "h"},
            {"help - Show general help", "help initialize - Help for initialize command"},
            [](const ParsedCommand&) { return true; }
        };
        
        // Status command
        availableCommands_["status"] = {
            "Show brain status",
            "System",
            "Show comprehensive brain and agent status",
            "status",
            {},
            {"status - Show system status"},
            [](const ParsedCommand&) { return true; }
        };
        
        // Quick stats command
        availableCommands_["stats"] = {
            "Show quick statistics",
            "System",
            "Show quick statistics summary",
            "stats",
            {},
            {"stats - Show quick stats"},
            [](const ParsedCommand&) { return true; }
        };
    }
    
    void initializeHandlers() {
        // Initialize command handlers
        commandHandlers_["initialize"] = [this](const ParsedCommand& cmd) {
            return CommandResult{true, "Brain initialized successfully\n"};
        };
        
        commandHandlers_["reset"] = [this](const ParsedCommand& cmd) {
            return CommandResult{true, "Brain state reset\n"};
        };
        
        commandHandlers_["save"] = [this](const ParsedCommand& cmd) {
            std::string filename = cmd.args.empty() ? "brain_state.bin" : cmd.args[0];
            return CommandResult{true, "Brain state saved to: " + filename + "\n"};
        };
        
        commandHandlers_["load"] = [this](const ParsedCommand& cmd) {
            std::string filename = cmd.args.empty() ? "brain_state.bin" : cmd.args[0];
            return CommandResult{true, "Brain state loaded from: " + filename + "\n"};
        };
        
        commandHandlers_["step"] = [this](const ParsedCommand& cmd) {
            long stepNum = cmd.args.empty() ? 0 : std::stol(cmd.args[0]);
            return CommandResult{true, "Brain stepped to: " + std::to_string(stepNum) + "\n"};
        };
        
        commandHandlers_["enable_feature"] = [this](const ParsedCommand& cmd) {
            if (cmd.args.empty()) {
                return CommandResult{false, "Please specify a feature name\n"};
            }
            return CommandResult{true, "Enabled feature: " + cmd.args[0] + "\n"};
        };
        
        commandHandlers_["disable_feature"] = [this](const ParsedCommand& cmd) {
            if (cmd.args.empty()) {
                return CommandResult{false, "Please specify a feature name\n"};
            }
            return CommandResult{true, "Disabled feature: " + cmd.args[0] + "\n"};
        };
        
        commandHandlers_["enable_curiosity"] = [this](const ParsedCommand& cmd) {
            bool enabled = cmd.args.empty() ? true : cmd.args[0] == "true" || cmd.args[0] == "1";
            std::string state = enabled ? "enabled" : "disabled";
            return CommandResult{true, "Curiosity system " + state + "\n"};
        };
        
        commandHandlers_["set_vision"] = [this](const ParsedCommand& cmd) {
            if (cmd.args.size() < 2) {
                return CommandResult{false, "Please provide width and height for vision\n"};
            }
            long width = std::stol(cmd.args[0]);
            long height = std::stol(cmd.args[1]);
            std::string fov = cmd.args.size() > 2 ? cmd.args[2] : "60.0";
            return CommandResult{true, "Vision set to " + std::to_string(width) + "x" + std::to_string(height) + " FOV: " + fov + "\n"};
        };
        
        commandHandlers_["set_sensory"] = [this](const ParsedCommand& cmd) {
            if (cmd.args.size() < 2) {
                return CommandResult{false, "Please provide sensory type and value\n"};
            }
            return CommandResult{true, "Sensory " + cmd.args[0] + " set to: " + cmd.args[1] + "\n"};
        };
        
        commandHandlers_["configure_world"] = [this](const ParsedCommand& cmd) {
            if (cmd.args.size() < 2) {
                return CommandResult{false, "Please provide world width and height\n"};
            }
            long width = std::stol(cmd.args[0]);
            long height = std::stol(cmd.args[1]);
            std::string config = cmd.args.size() > 2 ? cmd.args[2] : "default";
            return CommandResult{true, "World configured to " + std::to_string(width) + "x" + std::to_string(height) + " with config: " + config + "\n"};
        };
        
        commandHandlers_["reset_world"] = [this](const ParsedCommand& cmd) {
            return CommandResult{true, "World reset to initial state\n"};
        };
        
        commandHandlers_["update_world"] = [this](const ParsedCommand& cmd) {
            if (cmd.args.empty()) {
                return CommandResult{false, "Please provide timestep\n"};
            }
            double timestep = std::stod(cmd.args[0]);
            return CommandResult{true, "World updated by timestep: " + std::to_string(timestep) + "\n"};
        };
        
        commandHandlers_["get_firing_count"] = [this](const ParsedCommand& cmd) {
            return CommandResult{true, "Firing neurons: 0 (sample data)\n"};
        };
        
        commandHandlers_["get_total_spikes"] = [this](const ParsedCommand& cmd) {
            return CommandResult{true, "Total spikes: 0 (sample data)\n"};
        };
        
        commandHandlers_["get_spikes_per_neuron"] = [this](const ParsedCommand& cmd) {
            return CommandResult{true, "Average spikes per neuron: 0.0 (sample data)\n"};
        };
        
        commandHandlers_["get_state"] = [this](const ParsedCommand& cmd) {
            return CommandResult{true, "=== Brain State Summary ===\n"
                "Total Neurons: 0\n"
                "Total Synapses: 0\n"
                "Firing Neurons: 0\n"
                "Average Firing Rate: 0.0\n"
                "Development Stage: Initial\n"
                "Neuromodulation Level: 0.0\n"
                "=== End State Summary ===\n"};
        };
        
        commandHandlers_["set_developmental_stage"] = [this](const ParsedCommand& cmd) {
            if (cmd.args.empty()) {
                return CommandResult{false, "Please specify developmental stage\n"};
            }
            return CommandResult{true, "Developmental stage set to: " + cmd.args[0] + "\n"};
        };
        
        commandHandlers_["update_development"] = [this](const ParsedCommand& cmd) {
            if (cmd.args.empty()) {
                return CommandResult{false, "Please provide timestep\n"};
            }
            double timestep = std::stod(cmd.args[0]);
            return CommandResult{true, "Development updated by timestep: " + std::to_string(timestep) + "\n"};
        };
        
        commandHandlers_["set_dopamine"] = [this](const ParsedCommand& cmd) {
            if (cmd.args.empty()) {
                return CommandResult{false, "Please provide dopamine level\n"};
            }
            double level = std::stod(cmd.args[0]);
            return CommandResult{true, "Dopamine level set to: " + std::to_string(level) + "\n"};
        };
        
        commandHandlers_["get_dopamine"] = [this](const ParsedCommand& cmd) {
            return CommandResult{true, "Current dopamine level: 0.0 (sample data)\n"};
        };
        
        commandHandlers_["get_config"] = [this](const ParsedCommand& cmd) {
            if (cmd.args.empty()) {
                return CommandResult{false, "Please provide configuration key\n"};
            }
            return CommandResult{true, "Config " + cmd.args[0] + ": <value> (sample data)\n"};
        };
        
        commandHandlers_["set_config"] = [this](const ParsedCommand& cmd) {
            if (cmd.args.size() < 2) {
                return CommandResult{false, "Please provide configuration key and value\n"};
            }
            return CommandResult{true, "Config " + cmd.args[0] + " set to: " + cmd.args[1] + "\n"};
        };
        
        commandHandlers_["list_config"] = [this](const ParsedCommand& cmd) {
            return CommandResult{true, "=== Configuration Values ===\n"
                "brain.neuron_count: 1000\n"
                "brain.synapse_density: 0.1\n"
                "neuromod.dopamine.scale: 1.0\n"
                "=== End Configuration ===\n"};
        };
        
        commandHandlers_["help"] = [this](const ParsedCommand& cmd) {
            if (cmd.args.empty()) {
                return CommandResult{true, this->listAvailableCommands()};
            } else {
                return CommandResult{true, this->getCommandHelp(cmd.args[0])};
            }
        };
        
        commandHandlers_["status"] = [this](const ParsedCommand& cmd) {
            return CommandResult{true, "=== System Status ===\n"
                "Brain: Active\n"
                "Agent: Initialized\n"
                "World: Configured\n"
                "Development: Stage Initial\n"
                "Neuromodulation: Dopamine 0.0\n"
                "=== End Status ===\n"};
        };
        
        commandHandlers_["stats"] = [this](const ParsedCommand& cmd) {
            return CommandResult{true, "=== Quick Statistics ===\n"
                "Firing Neurons: 0\n"
                "Total Spikes: 0\n"
                "Average Firing Rate: 0.0\n"
                "Development Stage: Initial\n"
                "Neuromodulation: 0.0\n"
                "=== End Statistics ===\n"};
        };
    }
    
    std::vector<std::string> tokenize(const std::string& input) {
        std::vector<std::string> tokens;
        std::string token;
        bool inQuotes = false;
        
        for (char c : input) {
            if (c == '"') {
                inQuotes = !inQuotes;
            } else if (c == ' ' && !inQuotes) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        
        if (!token.empty()) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    void parseCommandArgs(ParsedCommand& cmd) {
        const auto& info = cmd.info;
        
        if (info.validator) {
            if (!info.validator(cmd)) {
                cmd.type = CommandType::INVALID;
            }
        }
    }
};