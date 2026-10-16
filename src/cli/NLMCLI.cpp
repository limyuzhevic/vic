// NLMCLI Implementation

#include "NLMCLI.hpp"
#include <algorithm>
#include <chrono>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>

namespace nlm {

struct CommandRunner::Impl {
    std::map<std::string, std::unique_ptr<ICommand>> commands;
    std::vector<std::string> commandHistory;
    bool interactiveMode;
};

CommandRunner::CommandRunner() : pImpl(std::make_unique<Impl>()) {
    pImpl->interactiveMode = false;
}

CommandRunner::~CommandRunner() = default;

void CommandRunner::registerCommand(std::unique_ptr<ICommand> command) {
    if (command) {
        std::string name = command->getName();
        pImpl->commands[name] = std::move(command);
    }
}

CommandResult CommandRunner::executeCommand(const std::string& commandName,
                                           const std::vector<std::string>& args,
                                           const CLIConfig& config) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CommandResult result;
    
    auto it = pImpl->commands.find(commandName);
    if (it == pImpl->commands.end()) {
        result.success = false;
        result.message = "Unknown command: " + commandName;
        result.exitCode = 1;
        return result;
    }
    
    ICommand& command = *it->second;
    
    // Validate arguments
    if (!command.validateArguments(args)) {
        result.success = false;
        result.message = "Invalid arguments for command: " + commandName;
        result.exitCode = 2;
        return result;
    }
    
    try {
        result = command.execute(args, config);
    } catch (const std::exception& e) {
        result.success = false;
        result.message = "Command execution error: " + std::string(e.what());
        result.exitCode = 3;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    if (result.success) {
        pImpl->commandHistory.push_back(commandName + " (success)");
    } else {
        pImpl->commandHistory.push_back(commandName + " (failed)");
    }
    
    return result;
}

std::vector<std::string> CommandRunner::getCommandNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->commands.size());
    for (const auto& pair : pImpl->commands) {
        names.push_back(pair.first);
    }
    return names;
}

std::string CommandRunner::getCommandHelp(const std::string& commandName) const {
    auto it = pImpl->commands.find(commandName);
    if (it == pImpl->commands.end()) {
        return "Command not found: " + commandName;
    }
    
    std::ostringstream oss;
    oss << "Command: " << commandName << "\n";
    oss << "Description: " << it->second->getDescription() << "\n";
    oss << "Usage: " << it->second->getUsage() << "\n";
    return oss.str();
}

bool CommandRunner::hasCommand(const std::string& commandName) const {
    return pImpl->commands.find(commandName) != pImpl->commands.end();
}

std::map<std::string, std::string> CommandRunner::getCommandDescriptions() const {
    std::map<std::string, std::string> descriptions;
    for (const auto& pair : pImpl->commands) {
        descriptions[pair.first] = pair.second->getDescription();
    }
    return descriptions;
}

void CommandRunner::runInteractive(const CLIConfig& config) {
    pImpl->interactiveMode = true;
    
    std::cout << "=== NLM CLI Interactive Mode ===" << std::endl;
    std::cout << "Type 'help' for commands, 'exit' to quit" << std::endl;
    
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        
        if (line == "exit" || line == "quit") {
            break;
        } else if (line == "help") {
            std::cout << "Available commands:" << std::endl;
            auto descriptions = getCommandDescriptions();
            for (const auto& pair : descriptions) {
                std::cout << "  " << pair.first << ": " << pair.second << std::endl;
            }
        } else if (line == "history") {
            std::cout << "Command history:" << std::endl;
            for (const auto& entry : pImpl->commandHistory) {
                std::cout << "  " << entry << std::endl;
            }
        } else {
            // Parse command and arguments
            std::vector<std::string> parts;
            std::stringstream ss(line);
            std::string part;
            while (ss >> part) {
                parts.push_back(part);
            }
            
            if (parts.empty()) continue;
            
            std::string commandName = parts[0];
            std::vector<std::string> args(parts.begin() + 1, parts.end());
            
            std::cout << "Executing: " << commandName << std::endl;
            CommandResult result = executeCommand(commandName, args, config);
            
            if (!result.success) {
                std::cout << "Error: " << result.message << std::endl;
                if (!result.output.empty()) {
                    std::cout << "Details: " << result.output << std::endl;
                }
            } else if (!result.output.empty()) {
                std::cout << result.output << std::endl;
            }
            
            if (config.verbose) {
                std::cout << "Execution time: " << std::fixed << std::setprecision(3) 
                         << result.executionTime << "s" << std::endl;
            }
        }
        
        std::cout << ">>> ";
    }
    
    pImpl->interactiveMode = false;
    std::cout << "Exiting interactive mode." << std::endl;
}

std::vector<std::string> CommandRunner::generateCompletions(const std::string& partialCommand) const {
    std::vector<std::string> completions;
    
    if (partialCommand.empty()) {
        for (const auto& pair : pImpl->commands) {
            completions.push_back(pair.first);
        }
    } else {
        for (const auto& pair : pImpl->commands) {
            if (pair.first.find(partialCommand) == 0) {
                completions.push_back(pair.first);
            }
        }
    }
    
    std::sort(completions.begin(), completions.end());
    return completions;
}

CommandRunner& getCommandRunner() {
    static CommandRunner runner;
    return runner;
}

// CLIArgumentParser implementation
CLIConfig CLIArgumentParser::parseGlobalArguments(int argc, char** argv) {
    CLIConfig config;
    config.verbose = false;
    config.quiet = false;
    config.help = false;
    config.version = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            config.help = true;
        } else if (arg == "--version") {
            config.version = true;
        } else if (arg == "--verbose" || arg == "-v") {
            config.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            config.quiet = true;
        } else if (arg.substr(0, 8) == "--config=") {
            config.configFile = arg.substr(8);
        } else if (arg == "--config" && i + 1 < argc) {
            config.configFile = argv[++i];
        } else if (arg.substr(0, 9) == "--output=") {
            config.outputFormat = arg.substr(9);
        } else if (arg == "--output" && i + 1 < argc) {
            config.outputFormat = argv[++i];
        } else if (arg.substr(0, 12) == "--log-level=") {
            config.logLevel = arg.substr(12);
        } else if (arg == "--log-level" && i + 1 < argc) {
            config.logLevel = argv[++i];
        } else if (arg.find('=') != std::string::npos && arg.find('=') > 0) {
            std::string key = arg.substr(0, arg.find('='));
            std::string value = arg.substr(arg.find('=') + 1);
            config.envOverrides[key] = value;
        }
    }
    
    return config;
}

std::vector<std::string> CLIArgumentParser::parseCommandArguments(int argc, char** argv, std::string& commandName) {
    std::vector<std::string> args;
    
    if (argc < 2) {
        commandName = "help";
        return args;
    }
    
    commandName = argv[1];
    
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.push_back("--help");
        } else if (arg == "--version") {
            args.push_back("--version");
        } else if (arg == "--verbose" || arg == "-v") {
            args.push_back("--verbose");
        } else if (arg == "--quiet" || arg == "-q") {
            args.push_back("--quiet");
        } else if (arg.find('=') != std::string::npos) {
            args.push_back(arg);
        } else {
            args.push_back(arg);
        }
    }
    
    return args;
}

void CLIArgumentParser::printHelp(const CLIConfig& config, const CommandRunner& runner) {
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                               NLM Command Line Interface                         ║" << std::endl;
    std::cout << "║                                                                               ║" << std::endl;
    std::cout << "║  Usage: nlm-cli [global-options] <command> [command-options]                 ║" << std::endl;
    std::cout << "║                                                                               ║" << std::endl;
    std::cout << "║  Global Options:                                                             ║" << std::endl;
    
    std::cout << "║    --config <file>                    Configuration file                  ║" << std::endl;
    std::cout << "║    --output <format>                  Output format (json, csv, text)     ║" << std::endl;
    std::cout << "║    --log-level <level>                Logging level                      ║" << std::endl;
    std::cout << "║    --verbose, -v                      Verbose output                     ║" << std::endl;
    std::cout << "║    --quiet, -q                        Quiet output                       ║" << std::endl;
    std::cout << "║    --help                             Show help                          ║" << std::endl;
    std::cout << "║    --version                          Show version                       ║" << std::endl;
    std::cout << "║                                                                               ║" << std::endl;
    
    std::cout << "║  Commands:                                                                    ║" << std::endl;
    
    auto commandDescriptions = runner.getCommandDescriptions();
    int maxNameLength = 0;
    for (const auto& pair : commandDescriptions) {
        maxNameLength = std::max(maxNameLength, static_cast<int>(pair.first.length()));
    }
    
    int row = 0;
    for (const auto& pair : commandDescriptions) {
        if (row % 3 == 0 && row > 0) {
            std::cout << "║                                                                               ║" << std::endl;
        }
        
        std::cout << "║    " << std::left << std::setw(20) << pair.first << " " 
                  << std::setw(maxNameLength + 5) << "" << pair.second;
        
        if (row % 3 != 2) {
            std::cout << " " << std::setw(maxNameLength + 5 - pair.first.length()) << "";
        }
        
        row++;
    }
    
    if (row % 3 != 0) {
        std::cout << std::endl;
        for (int i = row % 3; i < 3; ++i) {
            std::cout << "║                                                                               ║" << std::endl;
        }
    }
    
    std::cout << "║                                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << std::endl << "Examples:" << std::endl;
    std::cout << "  nlm-cli run --scenario=movie_test --neuron_count=1000" << std::endl;
    std::cout << "  nlm-cli config --list" << std::endl;
    std::cout << "  nlm-cli experiment --name=Phase3Demo --steps=5000" << std::endl;
    std::cout << "  nlm-cli visualize --output=graph.dot --format=png" << std::endl;
    std::cout << "  nlm-cli benchmark --neurons=2000 --steps=1000" << std::endl;
}

void CLIArgumentParser::printVersion() {
    std::cout << "NLM CLI v1.0.0" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Built with enhanced command-line interface for neural simulation control." << std::endl;
}

bool CLIArgumentParser::isHelpArgument(const std::string& arg) {
    return arg == "--help" || arg == "-h";
}

bool CLIArgumentParser::isVersionArgument(const std::string& arg) {
    return arg == "--version";
}

// OutputFormatter implementation
std::string OutputFormatter::format(const std::string& content, const std::string& format) {
    if (format == "json") {
        // Simple JSON formatting
        return "{\"content\": \"" + content + "\"}";
    } else if (format == "csv") {
        // Simple CSV formatting
        return '"' + content + '"';
    } else {
        // Default to plain text
        return content;
    }
}

std::string OutputFormatter::formatConfig(const std::shared_ptr<Config>& config) {
    if (!config) {
        return "null";
    }
    return config->summary();
}

std::string OutputFormatter::formatResults(const std::vector<CommandResult>& results) {
    std::ostringstream oss;
    oss << "Command Results:" << std::endl;
    oss << "=================" << std::endl;
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        oss << "Result " << (i + 1) << ": " << (result.success ? "SUCCESS" : "FAILURE") << std::endl;
        oss << "  Message: " << result.message << std::endl;
        if (!result.output.empty()) {
            oss << "  Output: " << result.output << std::endl;
        }
        oss << "  Exit Code: " << result.exitCode << std::endl;
        oss << "  Execution Time: " << std::fixed << std::setprecision(3) << result.executionTime << "s" << std::endl;
        oss << std::endl;
    }
    
    return oss.str();
}

std::string OutputFormatter::formatProgress(const std::string& operation, double progress, const std::string& details) {
    std::ostringstream oss;
    oss << "Progress: " << operation << std::endl;
    oss << "  " << std::fixed << std::setprecision(1) << (progress * 100) << "%";
    if (!details.empty()) {
        oss << " (" << details << ")";
    }
    oss << std::endl;
    return oss.str();
}

std::string OutputFormatter::formatTable(const std::vector<std::vector<std::string>>& rows, const std::vector<std::string>& headers) {
    if (rows.empty()) {
        return "No data to display.";
    }
    
    // Determine column widths
    std::vector<size_t> colWidths;
    if (!headers.empty()) {
        colWidths.resize(headers.size());
        for (size_t i = 0; i < headers.size(); ++i) {
            colWidths[i] = headers[i].length();
        }
    }
    
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size() && i < colWidths.size(); ++i) {
            colWidths[i] = std::max(colWidths[i], row[i].length());
        }
    }
    
    // Format table
    std::ostringstream oss;
    
    // Header
    if (!headers.empty()) {
        for (size_t i = 0; i < headers.size(); ++i) {
            oss << std::left << std::setw(colWidths[i] + 2) << headers[i];
        }
        oss << std::endl;
        
        // Separator
        for (size_t i = 0; i < headers.size(); ++i) {
            oss << std::string(colWidths[i] + 2, '-');
        }
        oss << std::endl;
    }
    
    // Rows
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            oss << std::left << std::setw(colWidths[i] + 2) << row[i];
        }
        oss << std::endl;
    }
    
    return oss.str();
}

std::string OutputFormatter::formatError(const std::string& error, const std::string& context) {
    std::ostringstream oss;
    oss << "Error: " << error;
    if (!context.empty()) {
        oss << " (Context: " << context << ")";
    }
    oss << std::endl;
    return oss.str();
}

std::string OutputFormatter::formatSuccess(const std::string& message, const std::string& details) {
    std::ostringstream oss;
    oss << "Success: " << message;
    if (!details.empty()) {
        oss << " (Details: " << details << ")";
    }
    oss << std::endl;
    return oss.str();
}

} // namespace nlm