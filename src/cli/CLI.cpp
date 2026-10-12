// CLI implementation
#include "CLI.hpp"
#include <iostream>
#include <sstream>

namespace nlm {

CLI::CLI() : helpSystem_(), running_(false) {
    initializeCommands();
}

void CLI::run(int argc, char** argv) {
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

void CLI::showHelp() const {
    helpSystem_.showHelp();
}

void CLI::start() {
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

void CLI::stop() {
    running_ = false;
    std::cout << "=== NLM CLI Stopped ===" << std::endl;
}

void CLI::addCommand(std::unique_ptr<Command> cmd) {
    helpSystem_.addCommand(std::move(cmd));
}

void CLI::executeCommand(const std::string& commandName, const std::vector<std::string>& args) {
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

void CLI::processInput(const std::string& input) {
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

}} // namespace nlm
