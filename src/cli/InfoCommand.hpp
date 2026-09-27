// Information command implementation
#pragma once

#include "BaseCommand.hpp"
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cstring>

namespace nlm {
namespace cli {

class InfoCommand : public BaseCommand {
public:
    InfoCommand() : BaseCommand("info", "System information commands") {}
    
    CommandResult execute() override {
        return CommandResult(0, "Info command requires subcommand: brain, config, system, or licenses\n");
    }
};

class InfoBrainCommand : public BaseCommand {
public:
    InfoBrainCommand() : BaseCommand("brain", "Show brain statistics") {}
    
    CommandResult execute() override {
        std::stringstream output;
        output << "Brain Statistics:" << std::endl;
        output << "  (Brain statistics would be displayed here)" << std::endl;
        output << "  Note: Full brain statistics implementation requires access to brain objects" << std::endl;
        
        return CommandResult(0, output.str());
    }
};

class InfoConfigCommand : public BaseCommand {
public:
    InfoConfigCommand() : BaseCommand("config", "Show configuration") {}
    
    CommandResult execute() override {
        std::stringstream output;
        output << "Configuration Information:" << std::endl;
        
        if (!config) {
            output << "  No configuration loaded" << std::endl;
            return CommandResult(0, output.str());
        }
        
        output << "  Configuration keys: " << config->getKeys().size() << std::endl;
        output << "  Configuration loaded: " << (config->getKeys().empty() ? "No" : "Yes") << std::endl;
        output << "  Config file support: Yes" << std::endl;
        
        return CommandResult(0, output.str());
    }
};

class InfoSystemCommand : public BaseCommand {
public:
    InfoSystemCommand() : BaseCommand("system", "Show system information") {}
    
    CommandResult execute() override {
        std::stringstream output;
        output << "System Information:" << std::endl;
        
        // Get system information
        auto cpus = std::thread::hardware_concurrency();
        output << "  CPU cores: " << cpus << std::endl;
        
        // Get memory info (platform-specific)
        #ifdef _WIN32
        output << "  Platform: Windows" << std::endl;
        #elif __linux__
        output << "  Platform: Linux" << std::endl;
        #elif __APPLE__
        output << "  Platform: macOS" << std::endl;
        #else
        output << "  Platform: Unknown" << std::endl;
        #endif
        
        output << "  NLM Version: 1.0.0" << std::endl;
        output << "  Build type: Release" << std::endl;
        output << "  Compiler: " << __VERSION__ << std::endl;
        
        return CommandResult(0, output.str());
    }
};

class InfoLicensesCommand : public BaseCommand {
public:
    InfoLicensesCommand() : BaseCommand("licenses", "Show license information") {}
    
    CommandResult execute() override {
        std::stringstream output;
        output << "License Information:" << std::endl;
        output << "  NLM (Neural Learning Machine)" << std::endl;
        output << "  Copyright (c) 2026" << std::endl;
        output << "  All rights reserved." << std::endl;
        output << std::endl;
        output << "  This software is part of the NLM project." << std::endl;
        output << "  See LICENSE file for detailed license terms." << std::endl;
        
        return CommandResult(0, output.str());
    }
};

// Command factory function
inline void registerInfoCommands(CommandProcessor& processor) {
    processor.registerCommand(new InfoCommand());
    processor.registerCommand(new InfoBrainCommand());
    processor.registerCommand(new InfoConfigCommand());
    processor.registerCommand(new InfoSystemCommand());
    processor.registerCommand(new InfoLicensesCommand());
}

} // namespace cli
} // namespace nlm
