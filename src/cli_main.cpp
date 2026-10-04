#include "nlm_cli_exec.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// Simple command line parser
std::vector<std::string> parseCommandLine(const std::string& input) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream iss(input);
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        // Interactive mode
        std::cout << "=== NLM Phase 6 Integration CLI ===" << std::endl;
        std::cout << "Type 'help' for commands or 'exit' to quit." << std::endl;
        
        NLM_CLI_EXEC cli;
        
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) continue;
            
            std::vector<std::string> tokens = parseCommandLine(line);
            std::string cmd = tokens[0];
            std::vector<std::string> args(tokens.begin() + 1, tokens.end());
            
            if (!cli.executeCommand(cmd, args)) {
                std::cout << "Command failed." << std::endl;
            }
            
            std::cout << std::endl;
        }
        
        return 0;
    } else {
        // Batch mode
        NLM_CLI_EXEC cli;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--help" || arg == "-h") {
                cli.showHelp();
                return 0;
            } else if (arg == "--version" || arg == "-v") {
                cli.showVersion();
                return 0;
            }
        }
        
        // Parse command line arguments for batch execution
        std::string command;
        std::vector<std::string> args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "new" || arg == "run" || arg == "status" || arg == "save" || 
                arg == "load" || arg == "export") {
                command = arg;
                args.clear();
            } else if (command.empty()) {
                std::cerr << "Error: Command not specified" << std::endl;
                return 1;
            } else {
                args.push_back(arg);
            }
        }
        
        if (!command.empty()) {
            return cli.executeCommand(command, args) ? 0 : 1;
        }
        
        cli.showHelp();
        return 0;
    }
}