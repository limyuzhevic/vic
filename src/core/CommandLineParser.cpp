#include "CommandLineParser.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cctype>

namespace nlm {

struct CommandLineParser::Impl {
    std::shared_ptr<CommandHandler> commandHandler;
    std::shared_ptr<Config> currentConfig;
    bool verbose;
    bool debug;
};

CommandLineParser::CommandLineParser() : pImpl(std::make_unique<Impl>()) {
    pImpl->verbose = false;
    pImpl->debug = false;
}

CommandLineParser::~CommandLineParser() = default;

CommandLineParser::CommandLineParser(CommandLineParser&&) noexcept = default;

CommandLineParser& CommandLineParser::operator=(CommandLineParser&&) noexcept = default;

void CommandLineParser::setCommandHandler(std::shared_ptr<CommandHandler> handler) {
    pImpl->commandHandler = handler;
}

CommandResult CommandLineParser::parseAndExecute(int argc, char** argv) {
    if (!pImpl->commandHandler) {
        return CommandResult(CommandStatus::Failed, "No command handler set", pImpl->currentConfig);
    }
    
    // Parse arguments
    auto options = parseArguments(argc, argv);
    
    // Check for special flags
    if (needsHelp(argc, argv)) {
        std::cout << getHelp() << std::endl;
        return CommandResult(CommandStatus::Success, "Help displayed");
    }
    
    if (needsVersion(argc, argv)) {
        std::cout << "NLM Version " << getVersion() << std::endl;
        return CommandResult(CommandStatus::Success, "Version displayed");
    }
    
    // Handle verbose and debug flags
    if (options.find("verbose") != options.end()) {
        pImpl->verbose = true;
        options.erase("verbose");
    }
    
    if (options.find("debug") != options.end()) {
        pImpl->debug = true;
        options.erase("debug");
    }
    
    // Find command name
    std::string commandName;
    if (options.find("--help") != options.end() || options.find("-h") != options.end()) {
        std::cout << getHelp() << std::endl;
        return CommandResult(CommandStatus::Success, "Help displayed");
    }
    
    // If no command found, show help
    if (commandName.empty()) {
        std::cout << getHelp() << std::endl;
        return CommandResult(CommandStatus::InvalidArguments, "No command specified");
    }
    
    // Get available commands
    auto commands = pImpl->commandHandler->getAvailableCommands();
    
    // Find the command
    std::shared_ptr<Command> selectedCommand = nullptr;
    for (auto& cmd : commands) {
        if (cmd->getName() == commandName) {
            selectedCommand = cmd;
            break;
        }
    }
    
    if (!selectedCommand) {
        std::cout << "Unknown command: " << commandName << std::endl;
        std::cout << getHelp() << std::endl;
        return CommandResult(CommandStatus::InvalidArguments, "Unknown command: " + commandName);
    }
    
    // Validate options
    auto cmdOptions = selectedCommand->getOptions();
    CommandResult validationResult = validateOptions(commandName, options, cmdOptions);
    if (validationResult.status != CommandStatus::Success) {
        return validationResult;
    }
    
    // Execute command
    CommandResult result = pImpl->commandHandler->execute(commandName, options, pImpl->currentConfig);
    
    if (pImpl->verbose && result.status == CommandStatus::Success) {
        NLM_LOG_INFO("Command executed successfully: " + commandName);
    } else if (pImpl->debug && result.status != CommandStatus::Success) {
        NLM_LOG_ERROR("Command failed: " + commandName + " - " + result.message);
    }
    
    return result;
}

std::string CommandLineParser::getHelp() const {
    std::stringstream ss;
    
    ss << "=== NLM Command Line Interface ===" << std::endl;
    ss << std::endl;
    ss << "Usage: nlm [OPTIONS] <command> [args]" << std::endl;
    ss << std::endl;
    
    // Global options
    ss << "Global Options:" << std::endl;
    ss << "  -h, --help              Show this help message" << std::endl;
    ss << "  --version               Show version information" << std::endl;
    ss << "  -v, --verbose           Enable verbose output" << std::endl;
    ss << "  --debug                 Enable debug mode" << std::endl;
    ss << std::endl;
    
    // Command categories
    ss << "Command Categories:" << std::endl;
    
    if (pImpl->commandHandler) {
        auto commands = pImpl->commandHandler->getAvailableCommands();
        
        for (CommandCategory category : {CommandCategory::Configuration,
                                         CommandCategory::BrainControl,
                                         CommandCategory::SimulationControl,
                                         CommandCategory::Visualization,
                                         CommandCategory::Advanced}) {
            ss << getCategoryName(category) << ":" << std::endl;
            
            for (auto& cmd : commands) {
                if (cmd->getCategory() == category) {
                    ss << "  " << cmd->getName() << ": " << cmd->getDescription() << std::endl;
                }
            }
            
            ss << std::endl;
        }
    }
    
    ss << "Examples:" << std::endl;
    ss << "  nlm --config config.json --steps 1000" << std::endl;
    ss << "  nlm save --filepath brain_state.json" << std::endl;
    ss << "  nlm --load brain_state.json --steps 5000" << std::endl;
    ss << std::endl;
    ss << "For more information about specific commands, use:" << std::endl;
    ss << "  nlm <command> --help" << std::endl;
    
    return ss.str();
}

std::string CommandLineParser::getVersion() {
    return "2.0.0 (Phase 2 - Real Neural Computation)";
}

std::string CommandLineParser::getUsageExamples() {
    return R"(
Basic Usage:
  nlm --help                                    Display help information
  nlm --version                                 Display version information
  nlm --config config.json --steps 1000         Run simulation with config file

Brain Control:
  nlm save --filepath brain_state.json          Save current brain state
  nlm load --filepath brain_state.json          Load brain state from file
  nlm checkpoint --filepath checkpoint.json     Create a checkpoint
  nlm reset                                      Reset brain to initial state

Configuration:
  nlm --set neuron_count=2000 --set region_count=4   Configure brain parameters

Simulation Control:
  nlm --steps 10000 --timestep 0.001             Run simulation for specific steps

Debugging:
  nlm debug --verbose --profile                  Enable verbose and profiling output

Batch Processing:
  nlm batch --file simulations.txt              Process multiple simulations from file

For detailed command help, use: nlm <command> --help
)";
}

bool CommandLineParser::needsHelp(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h" || arg == "help") {
            return true;
        }
    }
    return false;
}

bool CommandLineParser::needsVersion(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--version" || arg == "version") {
            return true;
        }
    }
    return false;
}

std::map<std::string, std::string> CommandLineParser::parseArguments(int argc, char** argv) {
    std::map<std::string, std::string> options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        size_t pos = arg.find('=');
        if (pos != std::string::npos && arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            options[key] = value;
        }
        // Handle --key value format
        else if (arg.substr(0, 2) == "--" && i + 1 < argc) {
            std::string nextArg(argv[i + 1]);
            bool isNextFlag = (nextArg.substr(0, 2) == "--" || 
                              (nextArg.size() == 1 && nextArg[0] == '-')) ||
                             nextArg.find('=') != std::string::npos;
            
            if (!isNextFlag) {
                std::string key = arg.substr(2);
                std::string value = nextArg;
                options[key] = value;
                i++; // Skip the value
            } else {
                options[arg.substr(2)] = "true";
            }
        }
        // Handle -key value format (short options)
        else if (arg.size() == 2 && arg[0] == '-' && i + 1 < argc) {
            std::string nextArg(argv[i + 1]);
            bool isNextFlag = (nextArg.substr(0, 2) == "--" || 
                              (nextArg.size() == 1 && nextArg[0] == '-')) ||
                             nextArg.find('=') != std::string::npos;
            
            if (!isNextFlag) {
                std::string key = arg.substr(1);
                std::string value = nextArg;
                options[key] = value;
                i++; // Skip the value
            } else {
                options[arg.substr(1)] = "true";
            }
        }
        // Handle standalone flags (without values)
        else if ((arg.size() == 2 && arg[0] == '-') || arg.substr(0, 2) == "--") {
            std::string key = arg;
            if (key.substr(0, 2) == "--") {
                key = key.substr(2);
            } else {
                key = key.substr(1);
            }
            options[key] = "true";
        }
    }
    
    return options;
}

CommandResult CommandLineParser::validateOptions(const std::string& commandName,
                                               const std::map<std::string, std::string>& options,
                                               const std::vector<CommandLineOption>& commandOptions) {
    // Implementation would validate options against command requirements
    // For now, just return success
    return CommandResult(CommandStatus::Success, "Options validated");
}

std::string CommandLineParser::getCategoryName(CommandCategory category) const {
    switch (category) {
        case CommandCategory::Configuration:
            return "Configuration Commands:";
        case CommandCategory::BrainControl:
            return "Brain Control Commands:";
        case CommandCategory::SimulationControl:
            return "Simulation Control Commands:";
        case CommandCategory::Visualization:
            return "Visualization Commands:";
        case CommandCategory::Advanced:
            return "Advanced Commands:";
        default:
            return "Unknown Commands:";
    }
}

} // namespace nlm
