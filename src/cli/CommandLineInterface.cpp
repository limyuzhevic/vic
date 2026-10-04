#include "cli/CommandLineInterface.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <memory>
#include <functional>

namespace nlm::cli {

struct CommandLineInterface::Impl {
    std::string name;
    std::string version = "6.0.0";
    std::string description = "An Experimental Artificial Developmental Brain";
    
    struct Command {
        std::string name;
        std::string description;
        std::function<int(const std::vector<std::string>&)> handler;
        std::string usage;
    };
    
    std::vector<Command> commands;
    std::unique_ptr<Config> currentConfig;
    std::unique_ptr<Brain> currentBrain;
    
    Impl(const std::string& name) : name(name) {
        registerDefaultCommands();
    }
};

CommandLineInterface::CommandLineInterface(const std::string& name) 
    : pImpl(std::make_unique<Impl>(name)) {
}

CommandLineInterface::~CommandLineInterface() = default;

int CommandLineInterface::run(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    if (args.empty()) {
        printHelp();
        return 1;
    }
    
    std::string cmdName = args[0];
    std::vector<std::string> cmdArgs;
    
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--help" || args[i] == "-h") {
            printHelp();
            return 1;
        } else if (args[i] == "--version") {
            printVersion();
            return 1;
        } else if (args[i].substr(0, 2) == "--") {
            cmdArgs.push_back(args[i]);
        } else if (args[i][0] == '-') {
            cmdArgs.push_back(args[i]);
        } else {
            cmdArgs.push_back(args[i]);
        }
    }
    
    return executeCommand(cmdName, cmdArgs);
}

void CommandLineInterface::registerCommand(const std::string& name, 
                                         const std::string& description,
                                         std::function<int(const std::vector<std::string>&)> handler,
                                         const std::string& usage) {
    pImpl->commands.push_back({name, description, handler, usage});
}

void CommandLineInterface::printHelp() const {
    std::cout << "=== " << pImpl->name << " ===" << std::endl;
    std::cout << pImpl->description << std::endl;
    std::cout << std::endl;
    std::cout << "Version: " << pImpl->version << std::endl;
    std::cout << std::endl;
    std::cout << "Available commands:" << std::endl;
    
    for (const auto& cmd : pImpl->commands) {
        std::cout << "  " << std::left << std::setw(15) << cmd.name << " - " << cmd.description << std::endl;
        if (!cmd.usage.empty()) {
            std::cout << "    Usage: " << cmd.usage << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Use 'help <command>' for more details about a specific command." << std::endl;
    std::cout << "Use '--help' or '-h' with any command for detailed usage." << std::endl;
}

void CommandLineInterface::printVersion() const {
    std::cout << "=== " << pImpl->name << " ===" << std::endl;
    std::cout << "Version: " << pImpl->version << std::endl;
    std::cout << "Description: " << pImpl->description << std::endl;
}

std::string CommandLineInterface::getDefaultConfigPath() const {
    return "/tmp/nlm_config.txt";
}

void CommandLineInterface::saveConfigToFile(const std::string& filepath) const {
    if (pImpl->currentConfig) {
        std::ofstream out(filepath);
        if (out.is_open()) {
            out << pImpl->currentConfig->summary();
            out.close();
        }
    }
}

std::string CommandLineInterface::getConfigAsString() const {
    if (pImpl->currentConfig) {
        return pImpl->currentConfig->summary();
    }
    return "No configuration set";
}

std::vector<std::string> CommandLineInterface::split(const std::string& str) const {
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string word;
    
    while (iss >> word) {
        result.push_back(word);
    }
    
    return result;
}

} // namespace nlm::cli
