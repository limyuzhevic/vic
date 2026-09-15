// Brain CLI Main Implementation
// Contains the main CLI application implementation

#include "BrainCLI.hpp"
#include "Commands/BrainCommands.hpp"
#include "Config/CLIConfig.hpp"
#include "src/core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <signal.h>

namespace nlm {
namespace cli {

// Global variables for signal handling
volatile bool g_running = true;

// Signal handler
void signalHandler(int signum) {
    NLM_LOG_INFO("Signal " + std::to_string(signum) + " received, shutting down...");
    g_running = false;
}

CLIApplication::CLIApplication(const std::string& name, const std::string& version)
    : name_(name)
    , version_(version)
    , loggingEnabled_(true)
    , logLevel_(LogLevel::Info)
    , strictErrorHandling_(false)
    , maxMemoryUsage_(0)
{
    initDefaultConfig();
    Logger::setGlobal(std::make_shared<Logger>());
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel_);
    Logger::getGlobal()->addLogger(consoleLogger);
    
    NLM_LOG_INFO("CLI Application initialized: " + name_ + " v" + version_);
    
    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
}

CLIApplication::~CLIApplication() {
    for (auto* cmd : commands_) delete cmd;
    for (auto* subCmd : subCommands_) delete subCmd;
}

void CLIApplication::addCommand(Command* command) {
    commands_.push_back(command);
}

void CLIApplication::addSubCommand(SubCommand* subCommand) {
    subCommands_.push_back(subCommand);
}

bool CLIApplication::runInteractive() {
    displayWelcome();
    
    std::string input;
    while (g_running && std::getline(std::cin, input)) {
        if (input.empty() || input == "exit" || input == "quit") break;
        
        commandHistory_.push_back(input);
        
        if (!executeParsedCommand(input)) {
            NLM_LOG_ERROR("Command failed: " + input);
        }
    }
    
    NLM_LOG_INFO("Interactive mode exited");
    return true;
}

bool CLIApplication::runBatch(const std::string& script) {
    std::ifstream file(script);
    if (!file) {
        NLM_LOG_ERROR("Cannot open script file: " + script);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip comments and empty lines
        if (line.empty() || line.find('#') == 0) continue;
        
        if (!executeParsedCommand(line)) {
            NLM_LOG_ERROR("Batch command failed at line " + std::to_string(lineNum) + ": " + line);
            if (strictErrorHandling_) return false;
        }
    }
    
    file.close();
    NLM_LOG_INFO("Batch script completed: " + script);
    return true;
}

void CLIApplication::loadConfig(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file) {
        NLM_LOG_WARNING("Cannot open config file: " + configFile);
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Simple config parsing: key = value
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos && !line.empty() && line[0] != '#') {
            std::string key = line.substr(0, eqPos);
            std::string value = line.substr(eqPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            config_[key] = value;
        }
    }
    
    file.close();
    applyConfig();
    NLM_LOG_INFO("Configuration loaded from: " + configFile);
}

void CLIApplication::saveConfig(const std::string& configFile) {
    std::ofstream file(configFile);
    if (!file) {
        NLM_LOG_ERROR("Cannot open config file for writing: " + configFile);
        return;
    }
    
    for (const auto& pair : config_) {
        file << pair.first << " = " << pair.second << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Configuration saved to: " + configFile);
}

void CLIApplication::loadPlugin(const std::string& pluginPath) {
    // In a real implementation, this would load and initialize plugins
    loadedPlugins_.push_back(pluginPath);
    NLM_LOG_INFO("Plugin loaded: " + pluginPath);
}

void CLIApplication::listPlugins() const {
    NLM_LOG_INFO("Loaded plugins:");
    for (const auto& plugin : loadedPlugins_) {
        NLM_LOG_INFO("  - " + plugin);
    }
}

std::vector<std::string> CLIApplication::getCompletions(const std::string& input) const {
    std::vector<std::string> completions;
    std::string prefix;
    
    // Simple completion logic
    size_t spacePos = input.find(' ');
    if (spacePos != std::string::npos) {
        prefix = input.substr(0, spacePos + 1);
        std::string searchTerm = input.substr(spacePos + 1);
        
        // Complete subcommands
        for (const auto* subCmd : subCommands_) {
            if (subCmd->name.find(searchTerm) == 0) {
                completions.push_back(prefix + subCmd->name);
            }
        }
        
        // Complete commands
        for (const auto* cmd : commands_) {
            if (cmd->name.find(searchTerm) == 0) {
                completions.push_back(prefix + cmd->name);
            }
        }
    } else {
        // Complete top-level commands
        for (const auto* cmd : commands_) {
            if (cmd->name.find(input) == 0) {
                completions.push_back(cmd->name);
            }
        }
        for (const auto* subCmd : subCommands_) {
            if (subCmd->name.find(input) == 0) {
                completions.push_back(subCmd->name);
            }
        }
    }
    
    return completions;
}

void CLIApplication::showHelp() const {
    displayWelcome();
    
    NLM_LOG_INFO("\nCommands:");
    
    // Group commands by category
    std::map<std::string, std::vector<Command*>> categorized;
    
    for (const auto* cmd : commands_) {
        categorized[cmd->category].push_back(cmd);
    }
    
    for (const auto& pair : categorized) {
        NLM_LOG_INFO("  " + pair.first + ":");
        for (const auto* cmd : pair.second) {
            NLM_LOG_INFO("    " + cmd->name + " - " + cmd->description);
        }
    }
    
    NLM_LOG_INFO("\nType 'help <command>' for more information about a command.");
}

void CLIApplication::showCommandHelp(const std::string& commandName) const {
    // Find command or subcommand
    Command* cmd = nullptr;
    for (auto* c : commands_) {
        if (c->name == commandName) {
            cmd = c;
            break;
        }
    }
    
    if (!cmd) {
        for (auto* subCmd : subCommands_) {
            if (subCmd->name == commandName) {
                cmd = subCmd;
                break;
            }
        }
    }
    
    if (cmd) {
        cmd->displayHelp();
    } else {
        NLM_LOG_ERROR("Unknown command: " + commandName);
    }
}

void CLIApplication::showVersion() const {
    NLM_LOG_INFO(name_ + " version " + version_);
    NLM_LOG_INFO("NLM - Neural Learning Machine");
    NLM_LOG_INFO("Phase 2: Real Neural Computation");
}

void CLIApplication::saveBrainState(const std::string& filename, std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot save null brain");
        return;
    }
    
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Cannot open file for saving brain: " + filename);
        return;
    }
    
    // Simple serialization - in reality would use protobuf or other format
    try {
        // For now, just log that we're saving
        NLM_LOG_INFO("Saving brain state to: " + filename);
        
        // Create a config with brain statistics
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(brain->getTotalNeuronCount()), ConfigSource::User);
        config->set("brain.synapse_count", static_cast<int64_t>(brain->getTotalSynapseCount()), ConfigSource::User);
        config->set("brain.spike_count", static_cast<int64_t>(brain->getTotalSpikeCount()), ConfigSource::User);
        config->set("brain.firing_rate", brain->getAverageFiringRate(), ConfigSource::User);
        
        config->saveToFile(filename);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving brain state: " + std::string(e.what()));
    }
}

std::shared_ptr<Brain> CLIApplication::loadBrainState(const std::string& filename) {
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(filename)) {
        NLM_LOG_ERROR("Cannot load brain from file: " + filename);
        return nullptr;
    }
    
    // In a real implementation, this would reconstruct the brain from saved state
    NLM_LOG_INFO("Brain state loaded from: " + filename);
    
    // For now, return a new brain with loaded config
    auto brain = std::make_shared<Brain>(config);
    if (brain->initialize()) {
        return brain;
    }
    
    NLM_LOG_ERROR("Failed to initialize brain from loaded state");
    return nullptr;
}

void CLIApplication::displayWelcome() const {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("╔═══════════════════════════════════════════════════════════════╗");
    NLM_LOG_INFO("║                                                               ║");
    NLM_LOG_INFO("║     NLM — 熙然                                                ║");
    NLM_LOG_INFO("║     Neural Learning Machine                                   ║");
    NLM_LOG_INFO("║                                                               ║");
    NLM_LOG_INFO("║     Advanced Brain Simulation Platform                       ║");
    NLM_LOG_INFO("║                                                               ║");
    NLM_LOG_INFO("╚═══════════════════════════════════════════════════════════════╝");
    NLM_LOG_INFO("");
}

void CLIApplication::displayPrompt() const {
    std::cout << ">>> ";
}

std::string CLIApplication::parseCommandLine(const std::string& input) const {
    return input; // Simple implementation
}

bool CLIApplication::executeParsedCommand(const std::string& commandLine) {
    // Parse command line (simple implementation)
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream iss(commandLine);
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.empty()) return true;
    
    std::string commandName = tokens[0];
    
    // Convert to lowercase for case-insensitive matching
    std::string lowerCmd = commandName;
    std::transform(lowerCmd.begin(), lowerCmd.end(), lowerCmd.begin(), ::tolower);
    
    // Find command
    Command* cmd = nullptr;
    for (auto* c : commands_) {
        if (c->name == lowerCmd) {
            cmd = c;
            break;
        }
    }
    
    if (!cmd) {
        for (auto* subCmd : subCommands_) {
            if (subCmd->name == lowerCmd) {
                cmd = subCmd;
                break;
            }
        }
    }
    
    if (!cmd) {
        NLM_LOG_ERROR("Unknown command: " + commandName);
        return false;
    }
    
    // Extract arguments (simplified)
    std::map<std::string, std::string> args;
    for (size_t i = 1; i < tokens.size(); ++i) {
        args["arg" + std::to_string(i - 1)] = tokens[i];
    }
    
    try {
        int result = cmd->executeCommand(args);
        return result == 0;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Command execution error: " + std::string(e.what()));
        return false;
    }
}

void CLIApplication::initDefaultConfig() {
    config_["general.help"] = "true";
    config_["general.version"] = "true";
    config_["general.quiet"] = "false";
    config_["general.batch"] = "false";
    config_["general.logging"] = "true";
    config_["general.log_level"] = "INFO";
    config_["performance.max_memory"] = "0"; // Unlimited
    config_["performance.cpu_affinity"] = "";
    config_["brain.default_brain"] = "brain.bin";
    config_["output.directory"] = "./output";
    config_["visualization.enabled"] = "false";
}

void CLIApplication::applyConfig() const {
    // Apply configuration to global settings
    if (config_.count("general.log_level")) {
        std::string levelStr = config_["general.log_level"];
        if (levelStr == "DEBUG") logLevel_ = LogLevel::Debug;
        else if (levelStr == "INFO") logLevel_ = LogLevel::Info;
        else if (levelStr == "WARNING") logLevel_ = LogLevel::Warning;
        else if (levelStr == "ERROR") logLevel_ = LogLevel::Error;
    }
    
    if (config_.count("performance.max_memory")) {
        maxMemoryUsage_ = std::stoull(config_["performance.max_memory"]);
    }
    
    // Update global logger with new level
    if (loggingEnabled_) {
        Logger::getGlobal()->setLevel(logLevel_);
    }
}

Command* CommandFactory::createBrainCommand(const std::string& operation) {
    // Factory method implementation would create specific brain commands
    // For now, return a generic command
    return new Command("brain", "Brain operations", "Brain");
}

Command* CommandFactory::createVisualizationCommand(const std::string& type) {
    return new Command("visualize", "Visualization commands", "Visualization");
}

Command* CommandFactory::createAnalysisCommand(const std::string& analysis) {
    return new Command("analyze", "Analysis commands", "Analysis");
}

Command* CommandFactory::createPluginCommand(const std::string& pluginOp) {
    return new Command("plugin", "Plugin commands", "Plugins");
}

} // namespace cli
} // namespace nlm
