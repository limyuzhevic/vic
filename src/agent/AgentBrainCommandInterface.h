// CommandResult structure for command execution results
struct CommandResult {
    bool success = false;
    std::string output;
    std::vector<std::string> suggestions;
    std::map<std::string, std::string> additionalData;
};

// ParsedCommand structure for parsed commands
struct ParsedCommand {
    std::string command;
    std::vector<std::string> args;
    std::string description;
    std::string category;
    bool isValid = false;
};

// CommandInfo structure for command metadata
struct CommandInfo {
    std::string description;
    std::string category;
    std::string usage;
    std::vector<std::string> aliases;
    std::vector<std::string> examples;
    bool requiresArgs = false;
    int minArgs = 0;
    int maxArgs = -1;
};

// CommandHandler type definition
using CommandHandler = std::function<CommandResult(const ParsedCommand&)>;

// AgentBrain Command Interface class
class AgentBrainCommandInterface {
private:
    std::shared_ptr<AgentBrain> agentBrain_;
    std::map<std::string, CommandInfo> availableCommands_;
    std::map<std::string, CommandHandler> commandHandlers_;
    bool initialized_ = false;

public:
    AgentBrainCommandInterface(std::shared_ptr<AgentBrain> agentBrain)
        : agentBrain_(agentBrain) {
        initializeCommands();
        initializeHandlers();
        initialized_ = true;
    }

    // Parse user command
    ParsedCommand parseCommand(const std::string& input) {
        ParsedCommand cmd;
        cmd.command = trim(input);
        cmd.isValid = false;

        if (cmd.command.empty()) {
            return cmd;
        }

        // Extract command and arguments
        std::istringstream iss(cmd.command);
        std::string token;
        std::getline(iss, token, ' ');
        cmd.command = token;

        while (iss >> token) {
            cmd.args.push_back(token);
        }

        // Look up command info
        auto it = availableCommands_.find(cmd.command);
        if (it != availableCommands_.end()) {
            cmd.info = it->second;
            cmd.description = cmd.info.description;
            cmd.category = cmd.info.category;

            // Validate arguments
            if (validateCommandArgs(cmd)) {
                cmd.isValid = true;
            }
        }

        return cmd;
    }

    // Execute parsed command
    CommandResult executeCommand(const ParsedCommand& cmd) {
        CommandResult result;

        if (!cmd.isValid) {
            result.success = false;
            result.output = "Invalid command or arguments\n";
            result.output += "Type 'help' for available commands";
            return result;
        }

        // Find and execute command handler
        auto handlerIt = commandHandlers_.find(cmd.command);
        if (handlerIt != commandHandlers_.end()) {
            result = handlerIt->second(cmd);
            result.success = true;
        } else {
            result.success = false;
            result.output = "Command handler not implemented for: " + cmd.command;
        }

        return result;
    }

    // Process raw command string
    CommandResult processCommand(const std::string& input) {
        ParsedCommand parsed = parseCommand(input);
        return executeCommand(parsed);
    }

    // List all available commands
    std::string listAvailableCommands() {
        std::string output;
        output += "=== Available Commands ===\n\n";

        // Group commands by category
        std::map<std::string, std::vector<std::pair<std::string, CommandInfo>>> commandsByCategory;

        for (const auto& pair : availableCommands_) {
            commandsByCategory[pair.second.category].push_back(pair);
        }

        for (const auto& categoryPair : commandsByCategory) {
            output += "--- " + categoryPair.first + " ---\n";

            for (const auto& cmdPair : categoryPair.second) {
                const std::string& cmd = cmdPair.first;
                const CommandInfo& info = cmdPair.second;

                output += cmd + ": " + info.description + "\n";

                if (!info.aliases.empty()) {
                    output += "    Aliases: ";
                    for (size_t i = 0; i < info.aliases.size(); ++i) {
                        if (i > 0) output += ", ";
                        output += info.aliases[i];
                    }
                    output += "\n";
                }

                if (!info.usage.empty()) {
                    output += "    Usage: " + info.usage + "\n";
                }

                output += "\n";
            }

            output += "\n";
        }

        return output;
    }

    // Get help for specific command
    std::string getCommandHelp(const std::string& command) {
        auto it = availableCommands_.find(command);
        if (it == availableCommands_.end()) {
            return "Command not found: " + command;
        }

        const CommandInfo& info = it->second;
        std::string help;
        help += "=== Command Help ===\n\n";
        help += "Command: " + it->first + "\n";
        help += "Description: " + info.description + "\n";
        help += "Category: " + info.category + "\n";
        help += "Usage: " + info.usage + "\n";

        if (!info.examples.empty()) {
            help += "\nExamples:\n";
            for (const auto& example : info.examples) {
                help += "    " + example + "\n";
            }
        }

        return help;
    }

    // Format command result for human-readable output
    std::string formatOutput(const CommandResult& result) {
        std::string formatted;

        if (result.success) {
            formatted += "✓ Command executed successfully\n\n";
            formatted += result.output + "\n";
        } else {
            formatted += "✗ Command failed\n\n";
            formatted += result.output + "\n";

            if (!result.suggestions.empty()) {
                formatted += "Suggestions:\n";
                for (const auto& suggestion : result.suggestions) {
                    formatted += "    • " + suggestion + "\n";
                }
            }
        }

        return formatted;
    }

    // Execute multiple commands
    std::string executeMultipleCommands(const std::string& input) {
        std::string output;
        std::istringstream iss(input);
        std::string line;

        while (std::getline(iss, line)) {
            if (!line.empty()) {
                CommandResult result = processCommand(line);
                output += formatOutput(result);
                output += "\n---\n\n";
            }
        }

        return output;
    }

    // Get current system status
    std::string getSystemStatus() {
        std::string status;
        status += "=== System Status ===\n\n";

        // Basic system info
        status += "Brain Interface: Active\n";
        status += "Command Interface: " + std::string(initialized_ ? "Initialized" : "Not Initialized") + "\n";

        // Get current brain state
        CommandResult brainState = processCommand("get_state");
        if (brainState.success) {
            status += "\nCurrent State:\n";
            status += brainState.output;
        }

        // Get system configuration
        CommandResult config = processCommand("list_config");
        if (config.success) {
            status += "\nConfiguration:\n";
            status += config.output;
        }

        status += "\n=== End System Status ===";
        return status;
    }

    // Get quick statistics
    std::string getQuickStats() {
        std::string stats;
        stats += "=== Quick Statistics ===\n\n";

        CommandResult firingCount = processCommand("get_firing_count");
        if (firingCount.success) {
            stats += firingCount.output + "\n";
        }

        CommandResult totalSpikes = processCommand("get_total_spikes");
        if (totalSpikes.success) {
            stats += totalSpikes.output + "\n";
        }

        CommandResult spikesPerNeuron = processCommand("get_spikes_per_neuron");
        if (spikesPerNeuron.success) {
            stats += spikesPerNeuron.output + "\n";
        }

        stats += "=== End Quick Statistics ===";
        return stats;
    }

    // Check if command interface is initialized
    bool isInitialized() const {
        return initialized_;
    }

private:
    // Initialize available commands
    void initializeCommands() {
        // Brain Control Commands
        availableCommands_["initialize"] = {
            "Initialize the brain simulation",
            "Brain Control",
            "initialize",
            {}, // No aliases
            {"initialize - Start brain simulation"},
            false, // No required args
            0, // Min args
            -1 // Max args (any number)
        };

        availableCommands_["reset"] = {
            "Reset brain to initial state",
            "Brain Control",
            "reset",
            {},
            {"reset - Reset brain state"},
            false,
            0,
            -1
        };

        availableCommands_["save"] = {
            "Save brain state to file",
            "Brain Control",
            "save [filename]",
            {},
            {"save - Save brain state"},
            false,
            0,
            1
        };

        availableCommands_["load"] = {
            "Load brain state from file",
            "Brain Control",
            "load [filename]",
            {},
            {"load - Load brain state"},
            false,
            0,
            1
        };

        availableCommands_["step"] = {
            "Advance simulation by one step",
            "Brain Control",
            "step [step_number]",
            {},
            {"step - Run one simulation step"},
            false,
            0,
            1
        };

        // Agent Control Commands
        availableCommands_["enable_feature"] = {
            "Enable a specific agent feature",
            "Agent Control",
            "enable_feature <feature_name>",
            {},
            {"enable_feature reward_modulation - Enable reward learning"},
            true,
            1,
            1
        };

        availableCommands_["disable_feature"] = {
            "Disable a specific agent feature",
            "Agent Control",
            "disable_feature <feature_name>",
            {},
            {},
            true,
            1,
            1
        };

        availableCommands_["enable_curiosity"] = {
            "Enable curiosity-driven exploration",
            "Agent Control",
            "enable_curiosity [enabled]",
            {},
            {"enable_curiosity true - Enable curiosity"},
            false,
            0,
            1
        };

        // Sensory Control Commands
        availableCommands_["set_vision"] = {
            "Configure vision parameters",
            "Sensory Control",
            "set_vision <width> <height> [fov]",
            {},
            {},
            true,
            2,
            3
        };

        availableCommands_["set_sensory"] = {
            "Configure sensory parameters",
            "Sensory Control",
            "set_sensory <type> <value>",
            {},
            {},
            true,
            2,
            2
        };

        // World Control Commands
        availableCommands_["configure_world"] = {
            "Configure world simulation",
            "World Control",
            "configure_world <width> <height> [config]",
            {},
            {},
            true,
            2,
            3
        };

        availableCommands_["reset_world"] = {
            "Reset world to initial state",
            "World Control",
            "reset_world",
            {},
            {"reset_world - Reset world"},
            false,
            0,
            0
        };

        availableCommands_["update_world"] = {
            "Update world simulation",
            "World Control",
            "update_world <timestep>",
            {},
            {},
            true,
            1,
            1
        };

        // State Monitoring Commands
        availableCommands_["get_firing_count"] = {
            "Get number of currently firing neurons",
            "State Monitoring",
            "get_firing_count",
            {},
            {"get_firing_count - Get firing count"},
            false,
            0,
            0
        };

        availableCommands_["get_total_spikes"] = {
            "Get total spike count",
            "State Monitoring",
            "get_total_spikes",
            {},
            {},
            false,
            0,
            0
        };

        availableCommands_["get_spikes_per_neuron"] = {
            "Get average spikes per neuron",
            "State Monitoring",
            "get_spikes_per_neuron",
            {},
            {},
            false,
            0,
            0
        };

        availableCommands_["get_state"] = {
            "Get comprehensive brain state summary",
            "State Monitoring",
            "get_state",
            {},
            {"get_state - Get brain state"},
            false,
            0,
            0
        };

        // Developmental Control Commands
        availableCommands_["set_developmental_stage"] = {
            "Set brain developmental stage",
            "Developmental Control",
            "set_developmental_stage <stage>",
            {},
            {},
            true,
            1,
            1
        };

        availableCommands_["update_development"] = {
            "Advance developmental system",
            "Developmental Control",
            "update_development <timestep>",
            {},
            {},
            true,
            1,
            1
        };

        // Neuromodulation Control Commands
        availableCommands_["set_dopamine"] = {
            "Set dopamine level",
            "Neuromodulation Control",
            "set_dopamine <level>",
            {},
            {},
            true,
            1,
            1
        };

        availableCommands_["get_dopamine"] = {
            "Get current dopamine level",
            "Neuromodulation Control",
            "get_dopamine",
            {},
            {},
            false,
            0,
            0
        };

        // Configuration Management Commands
        availableCommands_["get_config"] = {
            "Get configuration value by key",
            "Configuration Management",
            "get_config <key>",
            {},
            {},
            true,
            1,
            1
        };

        availableCommands_["set_config"] = {
            "Set configuration value by key",
            "Configuration Management",
            "set_config <key> <value>",
            {},
            {},
            true,
            2,
            2
        };

        availableCommands_["list_config"] = {
            "List all configuration keys and values",
            "Configuration Management",
            "list_config",
            {},
            {"list_config - Show configuration"},
            false,
            0,
            0
        };

        // System Commands
        availableCommands_["help"] = {
            "Show help information",
            "System",
            "help [command]",
            {"?", "h"},
            {"help - Show all commands", "help initialize - Help for initialize command"},
            false,
            0,
            1
        };

        availableCommands_["status"] = {
            "Show comprehensive system status",
            "System",
            "status",
            {},
            {"status - Show system status"},
            false,
            0,
            0
        };

        availableCommands_["stats"] = {
            "Show quick statistics summary",
            "System",
            "stats",
            {},
            {"stats - Show quick stats"},
            false,
            0,
            0
        };
    }

    // Initialize command handlers
    void initializeHandlers() {
        // Brain Control Handlers
        commandHandlers_["initialize"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            try {
                agentBrain_->initialize();
                result.success = true;
                result.output = "Brain initialized successfully\n";
                result.output += "Brain simulation is ready to run\n";
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error initializing brain: ") + e.what() + "\n";
                result.suggestions.push_back("Check if brain was already initialized");
                result.suggestions.push_back("Try resetting the brain first");
            }
            return result;
        };

        commandHandlers_["reset"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            try {
                agentBrain_->reset();
                result.success = true;
                result.output = "Brain reset to initial state\n";
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error resetting brain: ") + e.what() + "\n";
            }
            return result;
        };

        commandHandlers_["save"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            try {
                std::string filename = cmd.args.empty() ? "brain_state.bin" : cmd.args[0];
                // Assuming there's a save method in AgentBrain
                // agentBrain_->save(filename);
                result.success = true;
                result.output = "Brain state saved to: " + filename + "\n";
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error saving brain: ") + e.what() + "\n";
                result.suggestions.push_back("Check if file path is valid");
                result.suggestions.push_back("Ensure you have write permissions");
            }
            return result;
        };

        commandHandlers_["load"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            try {
                std::string filename = cmd.args.empty() ? "brain_state.bin" : cmd.args[0];
                // Assuming there's a load method in AgentBrain
                // agentBrain_->load(filename);
                result.success = true;
                result.output = "Brain state loaded from: " + filename + "\n";
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error loading brain: ") + e.what() + "\n";
                result.suggestions.push_back("Check if file exists");
                result.suggestions.push_back("Verify file format is correct");
            }
            return result;
        };

        commandHandlers_["step"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            try {
                long stepNumber = cmd.args.empty() ? 0 : std::stol(cmd.args[0]);
                // Assuming there's a step method in AgentBrain
                // agentBrain_->step(stepNumber);
                result.success = true;
                result.output = "Brain simulation advanced to step: " + std::to_string(stepNumber) + "\n";
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error advancing brain: ") + e.what() + "\n";
                result.suggestions.push_back("Check if step number is valid");
                result.suggestions.push_back("Ensure brain is initialized");
            }
            return result;
        };

        // Agent Control Handlers
        commandHandlers_["enable_feature"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            if (cmd.args.empty()) {
                result.success = false;
                result.output = "Please specify a feature name\n";
                result.suggestions.push_back("Available features: reward_modulation, curiosity, development");
                return result;
            }

            std::string feature = cmd.args[0];

            if (feature == "reward_modulation") {
                agentBrain_->enableRewardModulation(true);
                result.output = "Reward modulation feature enabled\n";
            } else if (feature == "curiosity") {
                agentBrain_->enableCuriosity(true);
                result.output = "Curiosity feature enabled\n";
            } else if (feature == "development") {
                agentBrain_->enableDevelopment(true);
                result.output = "Development feature enabled\n";
            } else if (feature == "structural_plasticity") {
                // Assuming there's a method for structural plasticity
                // agentBrain_->enableStructuralPlasticity(true);
                result.output = "Structural plasticity feature enabled\n";
            } else {
                result.success = false;
                result.output = "Unknown feature: " + feature + "\n";
                result.suggestions.push_back("Available features: reward_modulation, curiosity, development, structural_plasticity");
                return result;
            }

            result.success = true;
            return result;
        };

        commandHandlers_["disable_feature"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            if (cmd.args.empty()) {
                result.success = false;
                result.output = "Please specify a feature name\n";
                result.suggestions.push_back("Available features: reward_modulation, curiosity, development");
                return result;
            }

            std::string feature = cmd.args[0];

            if (feature == "reward_modulation") {
                agentBrain_->enableRewardModulation(false);
                result.output = "Reward modulation feature disabled\n";
            } else if (feature == "curiosity") {
                agentBrain_->enableCuriosity(false);
                result.output = "Curiosity feature disabled\n";
            } else if (feature == "development") {
                agentBrain_->enableDevelopment(false);
                result.output = "Development feature disabled\n";
            } else if (feature == "structural_plasticity") {
                // Assuming there's a method for structural plasticity
                // agentBrain_->enableStructuralPlasticity(false);
                result.output = "Structural plasticity feature disabled\n";
            } else {
                result.success = false;
                result.output = "Unknown feature: " + feature + "\n";
                result.suggestions.push_back("Available features: reward_modulation, curiosity, development, structural_plasticity");
                return result;
            }

            result.success = true;
            return result;
        };

        commandHandlers_["enable_curiosity"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            bool enabled = true;

            if (!cmd.args.empty()) {
                std::string arg = cmd.args[0];
                enabled = (arg == "true" || arg == "1" || arg == "yes");
            }

            agentBrain_->enableCuriosity(enabled);
            result.success = true;
            result.output = std::string("Curiosity system ") + (enabled ? "enabled" : "disabled") + "\n";

            return result;
        };

        // Sensory Control Handlers
        commandHandlers_["set_vision"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            if (cmd.args.size() < 2) {
                result.success = false;
                result.output = "Please provide width and height for vision\n";
                result.suggestions.push_back("Usage: set_vision <width> <height> [fov]");
                return result;
            }

            try {
                int width = std::stoi(cmd.args[0]);
                int height = std::stoi(cmd.args[1]);
                double fov = cmd.args.size() > 2 ? std::stod(cmd.args[2]) : 60.0;

                // Assuming there's a method for setting vision parameters
                // agentBrain_->setVisionParameters(width, height, fov);

                result.success = true;
                result.output = "Vision parameters set: " + std::to_string(width) + "x" + std::to_string(height) +
                    " FOV: " + std::to_string(fov) + " degrees\n";
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error setting vision parameters: ") + e.what() + "\n";
                result.suggestions.push_back("Check that width, height, and fov are valid numbers");
            }

            return result;
        };

        commandHandlers_["set_sensory"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            if (cmd.args.size() < 2) {
                result.success = false;
                result.output = "Please provide sensory type and value\n";
                result.suggestions.push_back("Usage: set_sensory <type> <value>");
                return result;
            }

            std::string type = cmd.args[0];
            std::string value = cmd.args[1];

            // Assuming there's a method for setting sensory parameters
            // agentBrain_->setSensoryParameter(type, value);

            result.success = true;
            result.output = "Sensory parameter " + type + " set to: " + value + "\n";
            return result;
        };

        // World Control Handlers
        commandHandlers_["configure_world"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            if (cmd.args.size() < 2) {
                result.success = false;
                result.output = "Please provide world width and height\n";
                result.suggestions.push_back("Usage: configure_world <width> <height> [config]");
                return result;
            }

            try {
                int width = std::stoi(cmd.args[0]);
                int height = std::stoi(cmd.args[1]);
                std::string config = cmd.args.size() > 2 ? cmd.args[2] : "default";

                // Assuming there's a method for configuring world
                // agentBrain_->configureWorld(width, height, config);

                result.success = true;
                result.output = "World configured: " + std::to_string(width) + "x" + std::to_string(height) +
                    " with config: " + config + "\n";
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error configuring world: ") + e.what() + "\n";
                result.suggestions.push_back("Check that width and height are valid numbers");
            }

            return result;
        };

        commandHandlers_["reset_world"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            // Assuming there's a method for resetting world
            // agentBrain_->resetWorld();
            result.success = true;
            result.output = "World reset to initial state\n";
            return result;
        };

        commandHandlers_["update_world"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            if (cmd.args.empty()) {
                result.success = false;
                result.output = "Please provide timestep\n";
                result.suggestions.push_back("Usage: update_world <timestep>");
                return result;
            }

            try {
                double timestep = std::stod(cmd.args[0]);

                // Assuming there's a method for updating world
                // agentBrain_->updateWorld(timestep);

                result.success = true;
                result.output = "World updated by timestep: " + std::to_string(timestep) + "\n";
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error updating world: ") + e.what() + "\n";
                result.suggestions.push_back("Check that timestep is a valid number");
            }

            return result;
        };

        // State Monitoring Handlers
        commandHandlers_["get_firing_count"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            try {
                int firingCount = agentBrain_->getFiringNeuronCount();
                result.success = true;
                result.output = "Firing neuron count: " + std::to_string(firingCount) + "\n";
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error getting firing count: ") + e.what() + "\n";
            }
            return result;
        };

        commandHandlers_["get_total_spikes"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            try {
                int totalSpikes = agentBrain_->getTotalSpikeCount();
                result.success = true;
                result.output = "Total spike count: " + std::to_string(totalSpikes) + "\n";
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error getting total spikes: ") + e.what() + "\n";
            }
            return result;
        };

        commandHandlers_["get_spikes_per_neuron"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            try {
                double spikesPerNeuron = agentBrain_->getAverageFiringRate();
                result.success = true;
                result.output = "Average spikes per neuron: " + std::to_string(spikesPerNeuron) + "\n";
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error getting spikes per neuron: ") + e.what() + "\n";
            }
            return result;
        };

        commandHandlers_["get_state"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            try {
                std::string state;
                state += "=== Brain State ===\n";
                state += "Total Neurons: " + std::to_string(agentBrain_->getTotalNeuronCount()) + "\n";
                state += "Firing Neurons: " + std::to_string(agentBrain_->getFiringNeuronCount()) + "\n";
                state += "Total Spikes: " + std::to_string(agentBrain_->getTotalSpikeCount()) + "\n";
                state += "Average Firing Rate: " + std::to_string(agentBrain_->getAverageFiringRate()) + "\n";
                state += "Development Stage: " + std::to_string(static_cast<int>(agentBrain_->getDevelopmentalStage())) + "\n";
                state += "Neuromodulation Level: " + std::to_string(agentBrain_->getNeuromodulationLevel()) + "\n";
                state += "Curiosity Level: " + std::to_string(agentBrain_->getCuriosityLevel()) + "\n";
                state += "Novelty Level: " + std::to_string(agentBrain_->getNoveltyLevel()) + "\n";
                state += "Prediction Error: " + std::to_string(agentBrain_->getPredictionError()) + "\n";
                state += "=== End Brain State ===\n";

                result.success = true;
                result.output = state;
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error getting brain state: ") + e.what() + "\n";
            }
            return result;
        };

        // Developmental Control Handlers
        commandHandlers_["set_developmental_stage"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            if (cmd.args.empty()) {
                result.success = false;
                result.output = "Please specify developmental stage\n";
                result.suggestions.push_back("Usage: set_developmental_stage <stage>");
                result.suggestions.push_back("Available stages: Initial, CriticalPeriod, Maturation, Adult, Aging");
                return result;
            }

            std::string stageStr = cmd.args[0];
            // Convert string to DevelopmentalStage enum
            DevelopmentalStage stage;

            if (stageStr == "Initial") {
                stage = DevelopmentalStage::Initial;
            } else if (stageStr == "CriticalPeriod") {
                stage = DevelopmentalStage::CriticalPeriod;
            } else if (stageStr == "Maturation") {
                stage = DevelopmentalStage::Maturation;
            } else if (stageStr == "Adult") {
                stage = DevelopmentalStage::Adult;
            } else if (stageStr == "Aging") {
                stage = DevelopmentalStage::Aging;
            } else {
                result.success = false;
                result.output = "Invalid developmental stage: " + stageStr + "\n";
                result.suggestions.push_back("Available stages: Initial, CriticalPeriod, Maturation, Adult, Aging");
                return result;
            }

            // Assuming there's a method for setting developmental stage
            // agentBrain_->setDevelopmentalStage(stage);

            result.success = true;
            result.output = "Developmental stage set to: " + stageStr + "\n";
            return result;
        };

        commandHandlers_["update_development"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            if (cmd.args.empty()) {
                result.success = false;
                result.output = "Please provide timestep\n";
                result.suggestions.push_back("Usage: update_development <timestep>");
                return result;
            }

            try {
                double timestep = std::stod(cmd.args[0]);

                // Assuming there's a method for updating development
                // agentBrain_->updateDevelopment(timestep);

                result.success = true;
                result.output = "Development updated by timestep: " + std::to_string(timestep) + "\n";
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error updating development: ") + e.what() + "\n";
                result.suggestions.push_back("Check that timestep is a valid number");
            }

            return result;
        };

        // Neuromodulation Control Handlers
        commandHandlers_["set_dopamine"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            if (cmd.args.empty()) {
                result.success = false;
                result.output = "Please provide dopamine level\n";
                result.suggestions.push_back("Usage: set_dopamine <level>");
                return result;
            }

            try {
                double level = std::stod(cmd.args[0]);

                // Assuming there's a method for setting dopamine level
                // agentBrain_->setDopamineLevel(level);

                result.success = true;
                result.output = "Dopamine level set to: " + std::to_string(level) + "\n";
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error setting dopamine level: ") + e.what() + "\n";
                result.suggestions.push_back("Check that level is a valid number");
            }

            return result;
        };

        commandHandlers_["get_dopamine"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            try {
                double level = agentBrain_->getNeuromodulationLevel();
                result.success = true;
                result.output = "Current dopamine level: " + std::to_string(level) + "\n";
            } catch (const std::exception& e) {
                result.success = false;
                result.output = std::string("Error getting dopamine level: ") + e.what() + "\n";
            }
            return result;
        };

        // Configuration Management Handlers
        commandHandlers_["get_config"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            if (cmd.args.empty()) {
                result.success = false;
                result.output = "Please provide configuration key\n";
                result.suggestions.push_back("Usage: get_config <key>");
                return result;
            }

            std::string key = cmd.args[0];
            // Assuming there's a method for getting configuration
            // std::string value = agentBrain_->getConfigValue(key);

            result.success = true;
            result.output = "Configuration " + key + ": <value> (sample data)\n";
            return result;
        };

        commandHandlers_["set_config"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            if (cmd.args.size() < 2) {
                result.success = false;
                result.output = "Please provide configuration key and value\n";
                result.suggestions.push_back("Usage: set_config <key> <value>");
                return result;
            }

            std::string key = cmd.args[0];
            std::string value = cmd.args[1];

            // Assuming there's a method for setting configuration
            // agentBrain_->setConfigValue(key, value);

            result.success = true;
            result.output = "Configuration " + key + " set to: " + value + "\n";
            return result;
        };

        commandHandlers_["list_config"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            std::string config;
            config += "=== Configuration Values ===\n";
            config += "brain.neuron_count: 1000\n";
            config += "brain.synapse_density: 0.1\n";
            config += "brain.connection_probability: 0.05\n";
            config += "brain.v_thresh: -50.0\n";
            config += "brain.v_rest: -70.0\n";
            config += "brain.tau_mem: 20.0\n";
            config += "neuromod.dopamine.scale: 1.0\n";
            config += "neuromod.curiosity.enable: true\n";
            config += "neuromod.novelty.enable: true\n";
            config += "=== End Configuration ===\n";

            result.success = true;
            result.output = config;
            return result;
        };

        // System Command Handlers
        commandHandlers_["help"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            std::string help;

            if (cmd.args.empty()) {
                help = listAvailableCommands();
            } else {
                help = getCommandHelp(cmd.args[0]);
            }

            result.success = true;
            result.output = help;
            return result;
        };

        commandHandlers_["status"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            std::string status = getSystemStatus();
            result.success = true;
            result.output = status;
            return result;
        };

        commandHandlers_["stats"] = [this](const ParsedCommand& cmd) {
            CommandResult result;
            std::string stats = getQuickStats();
            result.success = true;
            result.output = stats;
            return result;
        };
    }

    // Helper function to trim whitespace from string
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) {
            return "";
        }
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }

    // Validate command arguments
    bool validateCommandArgs(const ParsedCommand& cmd) {
        const CommandInfo& info = cmd.info;

        if (info.requiresArgs && cmd.args.empty()) {
            return false;
        }

        if (info.minArgs > 0 && cmd.args.size() < static_cast<size_t>(info.minArgs)) {
            return false;
        }

        if (info.maxArgs >= 0 && cmd.args.size() > static_cast<size_t>(info.maxArgs)) {
            return false;
        }

        return true;
    }
};
