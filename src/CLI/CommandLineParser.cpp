// CommandLineParser.cpp
#include "CLI/CommandLineParser.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>

struct CommandLineParser::Impl {
    struct Command {
        std::string name;
        std::string description;
        std::vector<CommandLineOption> options;
    };
    
    std::vector<Command> commands;
    std::vector<CommandLineOption> globalOptions;
    
    // Default options
    CommandLineOption helpOpt = {"help", 'h', "Show this help message", false, "", nullptr};
    CommandLineOption versionOpt = {"version", 'v', "Show version information", false, "", nullptr};
};

CommandLineParser::CommandLineParser() : pImpl(std::make_unique<Impl>()) {
    // Add default global options
    pImpl->globalOptions.push_back(pImpl->helpOpt);
    pImpl->globalOptions.push_back(pImpl->versionOpt);
}

void CommandLineParser::addCommand(const std::string& name, const std::string& description,
                                   const std::vector<CommandLineOption>& options) {
    pImpl->commands.push_back({name, description, options});
}

void CommandLineParser::addOption(const CommandLineOption& option) {
    pImpl->globalOptions.push_back(option);
}

ParsedCommand CommandLineParser::parse(int argc, char** argv) {
    ParsedCommand result;
    result.helpRequested = false;
    result.versionRequested = false;
    
    if (argc < 2) {
        result.command = "help";
        return result;
    }
    
    // First argument is the command
    result.command = argv[1];
    
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Check if it's an option (starts with -)
        if (arg[0] == '-') {
            size_t equalsPos = arg.find('=');
            std::string key;
            std::string value;
            
            if (equalsPos != std::string::npos) {
                key = arg.substr(1, equalsPos - 1);
                value = arg.substr(equalsPos + 1);
            } else {
                key = arg.substr(1);
                
                // Check if next arg is a value (not another option)
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    value = argv[++i];
                } else {
                    value = "true"; // Flag option
                }
            }
            
            result.options[key] = value;
        } else {
            // Positional argument
            result.args.push_back(arg);
        }
    }
    
    // Check for help/version flags
    if (result.options.find("help") != result.options.end() || 
        result.options.find("h") != result.options.end()) {
        result.helpRequested = true;
    }
    
    if (result.options.find("version") != result.options.end() ||
        result.options.find("v") != result.options.end()) {
        result.versionRequested = true;
    }
    
    return result;
}

void CommandLineParser::showHelp() const {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                     NLM - Neural Learning Machine                 ║\n";
    std::cout << "║                     Version 0.1.0                               ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "USAGE: nlm [OPTIONS] COMMAND [ARGS]...\n\n";
    
    std::cout << "GLOBAL OPTIONS:\n";
    for (const auto& opt : pImpl->globalOptions) {
        std::string shortDesc = "-" + std::string(1, opt.shortOpt) + ", --" + opt.name;
        std::cout << "  " << std::left << std::setw(25) << shortDesc 
                  << "  " << opt.description << "\n";
    }
    
    std::cout << "\nCOMMANDS:\n";
    for (const auto& cmd : pImpl->commands) {
        std::cout << "  " << std::left << std::setw(20) << cmd.name 
                  << "  " << cmd.description << "\n";
    }
    
    std::cout << "\nUse 'nlm help COMMAND' to see more details about a specific command.\n";
}

void CommandLineParser::showVersion() const {
    std::cout << "NLM - Neural Learning Machine v0.1.0\n";
    std::cout << "Copyright (c) 2024 NLM Project\n";
    std::cout << "An experimental artificial developmental brain simulation.\n";
}
