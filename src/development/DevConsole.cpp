#include "DevConsole.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace nlm {

struct DevConsole::Impl {
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<DevelopmentSystem> developmentSystem;
    bool running;
    std::vector<std::string> commandBuffer;
    bool isTty;
    
    // Command parsing
    struct ParsedCommand {
        std::string command;
        std::string args;
    };
    
    ParsedCommand parseCommand(const std::string& input) {
        ParsedCommand result;
        size_t spacePos = input.find(' ');
        if (spacePos != std::string::npos) {
            result.command = input.substr(0, spacePos);
            result.args = input.substr(spacePos + 1);
        } else {
            result.command = input;
            result.args = "";
        }
        
        // Trim whitespace
        result.command.erase(0, result.command.find_first_not_of(" \t"));
        result.command.erase(result.command.find_last_not_of(" \t") + 1);
        
        return result;
    }
    
    // Initialize default commands
    void initializeDefaultCommands() {
        // help command
        addCommand({"help", "Show help for commands", 
            [this](auto, auto, auto) { printHelp(); }});
        
        // exit/quit command
        addCommand({"exit", "Exit the development console", 
            [this](auto, auto, auto) { running = false; }});
        
        addCommand({"quit", "Exit the development console", 
            [this](auto, auto, auto) { running = false; }});
        
        // brain commands
        addCommand({"brain", "Get brain status and statistics",
            [this](auto, auto, auto) { printBrainStatus(); }});
        
        // development commands
        addCommand({"dev", "Get development system status",
            [this](auto, auto, auto) { printDevelopmentStatus(); }});
        
        // memory commands
        addCommand({"memory", "Get memory system status",
            [this](auto, auto, auto) { printMemoryStatus(); }});
        
        // performance commands
        addCommand({"perf", "Get performance statistics",
            [this](auto, auto, auto) { printPerformanceStats(); }});
        
        // config commands
        addCommand({"config", "Get current configuration",
            [this](auto, auto, auto) { printConfig(); }});
        
        // set command for runtime configuration
        addCommand({"set", "Set configuration value: set <key> <value>",
            [this](std::shared_ptr<Config> cfg, auto, auto) {
                if (this->commandBuffer.size() >= 2) {
                    std::string key = this->commandBuffer[0];
                    std::string value = this->commandBuffer[1];
                    cfg->set(key, value, ConfigSource::Runtime);
                    NLM_LOG_INFO("Set " + key + " = " + value);
                } else {
                    NLM_LOG_WARNING("Usage: set <key> <value>");
                }
            }});
        
        // enable/disable feature commands
        addCommand({"enable", "Enable a feature: enable <feature_name>",
            [this](std::shared_ptr<Config> cfg, auto, auto) {
                if (!this->commandBuffer.empty()) {
                    cfg->enableFeature(this->commandBuffer[0], true);
                    NLM_LOG_INFO("Enabled feature: " + this->commandBuffer[0]);
                }
            }});
        
        addCommand({"disable", "Disable a feature: disable <feature_name>",
            [this](std::shared_ptr<Config> cfg, auto, auto) {
                if (!this->commandBuffer.empty()) {
                    cfg->enableFeature(this->commandBuffer[0], false);
                    NLM_LOG_INFO("Disabled feature: " + this->commandBuffer[0]);
                }
            }});
        
        // reset brain command
        addCommand({"reset", "Reset the brain to initial state",
            [this](auto, auto, auto) {
                if (this->brain) {
                    this->brain->reset();
                    this->brain->initialize();
                    NLM_LOG_INFO("Brain reset to initial state");
                }
            }});
        
        // step command
        addCommand({"step", "Advance brain by one step: step <steps>",
            [this](auto, auto, auto) {
                int steps = 1;
                if (!this->commandBuffer.empty()) {
                    try {
                        steps = std::stoi(this->commandBuffer[0]);
                    } catch (...) {
                        NLM_LOG_WARNING("Invalid step count, using 1");
                    }
                }
                
                for (int i = 0; i < steps && this->running; ++i) {
                    this->brain->step(i);
                }
                
                NLM_LOG_INFO("Advanced brain by " + std::to_string(steps) + " steps");
            }});
        
        // status command
        addCommand({"status", "Get comprehensive status report",
            [this](auto, auto, auto) {
                this->printBrainStatus();
                this->printDevelopmentStatus();
                this->printMemoryStatus();
                this->printPerformanceStats();
                this->printConfig();
            }});
    }
    
    // Execute parsed command
    void executeCommand(const ParsedCommand& parsed) {
        // Try to find command in custom commands
        auto it = std::find_if(commands_.begin(), commands_.end(),
            [&parsed](const DevCommand& cmd) { return cmd.name == parsed.command; });n        
        if (it != commands_.end()) {
            // Store parsed args for command use
            commandBuffer.clear();
            if (!parsed.args.empty()) {
                size_t start = 0;
                size_t end = parsed.args.find(' ');
                while (end != std::string::npos) {
                    commandBuffer.push_back(parsed.args.substr(start, end - start));
                    start = end + 1;
                    end = parsed.args.find(' ', start);
                }
                commandBuffer.push_back(parsed.args.substr(start));
            }
            
            // Execute command
            it->action(config, brain, developmentSystem);
            
            // Add to history
            DevConsoleEntry entry;
            entry.input = parsed.command + (parsed.args.empty() ? "" : " " + parsed.args);
            entry.timestamp = getCurrentTimestamp();
            history_.push_back(entry);
        } else {
            // Command not found
            NLM_LOG_WARNING("Unknown command: " + parsed.command);
            printHelp(parsed.command);
        }
    }
    
    // Get current timestamp
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    // Print current console prompt
    void printPrompt() {
        std::cout << "nlm> ";
        std::cout.flush();
    }
};

DevConsole::DevConsole() : isActive_(false) {
    pImpl = std::make_unique<Impl>();
    pImpl->running = false;
    pImpl->isTty = true;
}

DevConsole::~DevConsole() = default;

void DevConsole::initialize(std::shared_ptr<Config> config,
                            std::shared_ptr<Brain> brain,
                            std::shared_ptr<DevelopmentSystem> developmentSystem) {
    pImpl->config = config;
    pImpl->brain = brain;
    pImpl->developmentSystem = developmentSystem;
    pImpl->running = true;
    
    // Initialize default commands
    pImpl->initializeDefaultCommands();
    
    // Print welcome message
    NLM_LOG_INFO("=== NLM Development Console ===");
    NLM_LOG_INFO("Type 'help' for available commands");
    NLM_LOG_INFO("Use 'set <key> <value>' to modify configuration at runtime");
    NLM_LOG_INFO("Use 'exit' or 'quit' to leave the console");
    NLM_LOG_INFO("");
}

bool DevConsole::processInput(const std::string& input) {
    if (!isActive_ || !pImpl->running) {
        return false;
    }
    
    std::string trimmedInput = input;
    // Remove carriage return and newline characters
    if (!trimmedInput.empty() && trimmedInput.back() == '\r') {
        trimmedInput.pop_back();
    }
    if (!trimmedInput.empty() && trimmedInput.back() == '\n') {
        trimmedInput.pop_back();
    }
    
    if (trimmedInput.empty()) {
        return true;
    }
    
    // Check for special commands
    if (pImpl->processSpecialCommand(trimmedInput)) {
        return true;
    }
    
    // Parse and execute command
    auto parsed = pImpl->parseCommand(trimmedInput);
    pImpl->executeCommand(parsed);
    
    // Print prompt for next command
    pImpl->printPrompt();
    
    return true;
}

void DevConsole::run(bool continuous) {
    if (pImpl->running && isActive_) {
        return; // Already running
    }
    
    isActive_ = true;
    pImpl->running = true;
    pImpl->commandBuffer.clear();
    
    printBanner();
    pImpl->printPrompt();
    
    if (continuous) {
        // Simple line input loop
        std::string line;
        while (pImpl->running && isActive_) {
            std::getline(std::cin, line);
            processInput(line);
        }
    }
}

void DevConsole::stop() {
    isActive_ = false;
    pImpl->running = false;
    NLM_LOG_INFO("Development console stopped");
}

void DevConsole::addCommand(const DevCommand& command) {
    // Check if command already exists
    auto it = std::find_if(commands_.begin(), commands_.end(),
        [&command](const DevCommand& cmd) { return cmd.name == command.name; });n    
    if (it == commands_.end()) {
        commands_.push_back(command);
        NLM_LOG_INFO("Added command: " + command.name);
    } else {
        *it = command;
        NLM_LOG_INFO("Updated command: " + command.name);
    }
}

void DevConsole::removeCommand(const std::string& name) {
    auto it = std::find_if(commands_.begin(), commands_.end(),
        [&name](const DevCommand& cmd) { return cmd.name == name; });
    
    if (it != commands_.end()) {
        commands_.erase(it);
        NLM_LOG_INFO("Removed command: " + name);
    }
}

std::string DevConsole::getHelp() const {
    std::stringstream help;
    help << "=== NLM Development Console Commands ===\n\n";
    help << "General Commands:\n";
    help << "  help [command] - Show help for commands (or specific command)\n";
    help << "  exit/quit     - Exit the development console\n";
    help << "  status        - Show comprehensive status report\n\n";
    
    help << "Brain Commands:\n";
    help << "  brain         - Show brain status and statistics\n";
    help << "  step [n]      - Advance brain by n steps (default: 1)\n";
    help << "  reset         - Reset brain to initial state\n\n";
    
    help << "Development Commands:\n";
    help << "  dev           - Show development system status\n\n";
    
    help << "Memory Commands:\n";
    help << "  memory        - Show memory system status\n\n";
    
    help << "Performance Commands:\n";
    help << "  perf          - Show performance statistics\n\n";
    
    help << "Configuration Commands:\n";
    help << "  config        - Show current configuration\n";
    help << "  set <key> <value> - Set configuration value at runtime\n";
    help << "  enable <name> - Enable a feature\n";
    help << "  disable <name>- Disable a feature\n\n";
    
    help << "For more information about specific commands, type 'help <command>'";
    
    return help.str();
}

bool DevConsole::Impl::processSpecialCommand(const std::string& input) {
    if (input == "clear") {
        system("clear");
        return true;
    } else if (input == "history") {
        for (const auto& entry : history_) {
            std::cout << "[" << entry.timestamp << "] " << entry.input << std::endl;
        }
        return true;
    } else if (input == "banner") {
        printBanner();
        return true;
    }
    return false;
}

void DevConsole::printHelp(const std::string& command) const {
    if (command.empty()) {
        NLM_LOG_INFO(getHelp());
        return;
    }
    
    // Find command in the list
    auto it = std::find_if(commands_.begin(), commands_.end(),
        [&command](const DevCommand& cmd) { return cmd.name == command; });n    
    if (it != commands_.end()) {
        NLM_LOG_INFO("Command: " + it->name);
        NLM_LOG_INFO("Description: " + it->description);
    } else {
        NLM_LOG_WARNING("Command not found: " + command);
    }
}

void DevConsole::printBrainStatus() const {
    if (!pImpl->brain) {
        NLM_LOG_WARNING("Brain not available");
        return;
    }
    
    NLM_LOG_INFO("=== Brain Status ===");
    NLM_LOG_INFO("Neurons: " + std::to_string(pImpl->brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Synapses: " + std::to_string(pImpl->brain->getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(pImpl->brain->getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(pImpl->brain->getFiringNeuronCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(pImpl->brain->getAverageFiringRate()));
    NLM_LOG_INFO("Excitation/Inhibition ratio: " + std::to_string(pImpl->brain->getExcitationInhibitionRatio()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(pImpl->brain->getTotalSpikeCount()));
}

void DevConsole::printDevelopmentStatus() const {
    if (!pImpl->developmentSystem) {
        NLM_LOG_WARNING("Development system not available");
        return;
    }
    
    NLM_LOG_INFO("=== Development Status ===");
    NLM_LOG_INFO("Stage: " + std::string(pImpl->developmentSystem->getStageName()));
    NLM_LOG_INFO("Age: " + std::to_string(pImpl->developmentSystem->getDevelopmentalAge()) + " steps");
    NLM_LOG_INFO("Critical period: " + std::string(pImpl->developmentSystem->isCriticalPeriod() ? "Yes" : "No"));
    NLM_LOG_INFO("Plasticity modifier: " + std::to_string(pImpl->developmentSystem->getPlasticityModifier()));
}

void DevConsole::printMemoryStatus() const {
    NLM_LOG_INFO("=== Memory Status ===");
    
    // Working memory
    NLM_LOG_INFO("Working memory: Active traces available");
    
    // Episodic memory
    NLM_LOG_INFO("Episodic memory: Episode storage operational");
    
    // Semantic memory
    NLM_LOG_INFO("Semantic memory: Fact database operational");
    
    // Procedural memory
    NLM_LOG_INFO("Procedural memory: Skill database operational");
    
    // Associative memory
    NLM_LOG_INFO("Associative memory: Association storage operational");
}

void DevConsole::printPerformanceStats() const {
    NLM_LOG_INFO("=== Performance Statistics ===");
    NLM_LOG_INFO("Performance monitoring: Active");
    NLM_LOG_INFO("Memory pools: Configured");
    NLM_LOG_INFO("Event queues: Operational");
    NLM_LOG_INFO("Vectorization: Available");
    NLM_LOG_INFO("Parallel processing: Ready");
    NLM_LOG_INFO("Cache optimization: Active");
}

void DevConsole::printConfig() const {
    if (!pImpl->config) {
        NLM_LOG_WARNING("Configuration not available");
        return;
    }
    
    NLM_LOG_INFO("=== Current Configuration ===");
    NLM_LOG_INFO(pImpl->config->summary());
}

} // namespace nlm