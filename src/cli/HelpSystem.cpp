// HelpSystem implementation
//
// Help and documentation system for CLI commands

#include "HelpSystem.hpp"
#include <iostream>
#include <algorithm>

namespace nlm {

HelpSystem::HelpSystem() {
    loadDefaultHelp();
}

HelpSystem::~HelpSystem() = default;

void HelpSystem::loadCommandHelp() {
    // Commands are loaded via addCommandHelp
    loadDefaultHelp();
}

int HelpSystem::showCommandHelp(const std::string& commandName) const {
    auto it = commandHelp.find(commandName);
    if (it == commandHelp.end()) {
        std::cout << "Command: " << commandName << std::endl;
        std::cout << "Description: Command not found or help not available" << std::endl;
        std::cout << "Usage: " << commandName << " <arguments>" << std::endl;
        return 1;
    }
    
    const auto& help = it->second;
    
    std::cout << "Command: " << commandName << std::endl;
    std::cout << "Description: " << help.description << std::endl;
    std::cout << "Usage: " << help.usage << std::endl;
    
    if (!help.examples.empty()) {
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        for (const auto& example : help.examples) {
            std::cout << "  " << example << std::endl;
        }
    }
    
    return 0;
}

void HelpSystem::showGeneralHelp() const {
    std::cout << "NLM CLI - Neural Learning Machine" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm [OPTIONS] <COMMAND> [ARGS]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Available commands:" << std::endl;
    std::cout << std::endl;
    
    // Show brain commands
    std::cout << "Brain commands:" << std::endl;
    std::cout << "  brain create <name> [--neurons N] [--regions R] [--config FILE]" << std::endl;
    std::cout << "  brain initialize <name> [--current] [--reset]" << std::endl;
    std::cout << "  brain step <name> <steps> [--verbose] [--stats]" << std::endl;
    std::cout << "  brain save <name> <file> [--overwrite]" << std::endl;
    std::cout << "  brain load <name> <file> [--merge]" << std::endl;
    std::cout << "  brain status <name> [--full] [--stats] [--connections]" << std::endl;
    std::cout << "  brain reset <name>" << std::endl;
    std::cout << std::endl;
    
    // Show agent commands
    std::cout << "Agent commands:" << std::endl;
    std::cout << "  agent enable [--curiosity] [--reward-modulation] [--neuromodulation]" << std::endl;
    std::cout << "  agent disable [--all]" << std::endl;
    std::cout << "  agent status [--agent-name NAME]" << std::endl;
    std::cout << std::endl;
    
    // Show config commands
    std::cout << "Config commands:" << std::endl;
    std::cout << "  config show [--global] [--brain NAME] [--all]" << std::endl;
    std::cout << "  config set [--global] [--brain NAME] <key> <value>" << std::endl;
    std::cout << "  config validate [--global] [--brain NAME] [--fix-duplicates]" << std::endl;
    std::cout << std::endl;
    
    // Show experiment commands
    std::cout << "Experiment commands:" << std::endl;
    std::cout << "  experiment run [--brain NAME] <phase> <steps> [--continue]" << std::endl;
    std::cout << "  experiment status [--brain NAME] [--all]" << std::endl;
    std::cout << std::endl;
    
    // Show monitor commands
    std::cout << "Monitor commands:" << std::endl;
    std::cout << "  monitor brain <name> [--stats] [--neuromodulation] [--spikes] [--activity]" << std::endl;
    std::cout << "  monitor agent [--curiosity] [--reward-modulation] [--status]" << std::endl;
    std::cout << std::endl;
    
    // Show system commands
    std::cout << "System commands:" << std::endl;
    std::cout << "  help [command] - Show help for specific command or all commands" << std::endl;
    std::cout << "  version - Show version and build info" << std::endl;
    std::cout << "  exit/quit - Exit CLI" << std::endl;
    std::cout << "  reset - Reset all brains to default state" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Global options:" << std::endl;
    std::cout << "  --help, -h     Show this help message" << std::endl;
    std::cout << "  --version, -v  Show version information" << std::endl;
}

std::string HelpSystem::getCommandHelp(const std::string& commandName) const {
    auto it = commandHelp.find(commandName);
    if (it != commandHelp.end()) {
        const auto& help = it->second;
        return "Command: " + commandName + "\nDescription: " + help.description + "\nUsage: " + help.usage;
    }
    return "";
}

void HelpSystem::addCommandHelp(const std::string& commandName,
                               const std::string& description,
                               const std::string& usage,
                               const std::vector<std::string>& examples) {
    CommandHelp help;
    help.description = description;
    help.usage = usage;
    help.examples = examples;
    help.loaded = true;
    
    commandHelp[commandName] = help;
}

void HelpSystem::loadDefaultHelp() const {
    // Add help for built-in system commands
    addCommandHelp("help", "Show help information", "help [command]");
    addCommandHelp("version", "Show version and build info", "version");
    addCommandHelp("exit", "Exit CLI", "exit");
    addCommandHelp("quit", "Exit CLI", "quit");
    addCommandHelp("reset", "Reset all brains to default state", "reset");
    
    // Note: Command-specific help will be added by individual command implementations
}

} // namespace nlm