// nlm-cli profiling launcher
//
// This file provides the main CLI entry point for profiling tools.
// It integrates with the existing CLI system to provide profiling commands.

#pragma once

#include "src/cli/CLIEngine.hpp"
#include "src/cli/HelpSystem.hpp"
#include "profiling/ProfileCPU.hpp"
#include <memory>
#include <iostream>

namespace nlm {

class ProfilingLauncher {
public:
    ProfilingLauncher() {
        // Register profiling tools with CLI engine
        registerProfilingTools();
    }
    
    void registerProfilingTools() {
        // Create CLI engine
        auto engine = std::make_shared<CLIEngine>();
        
        // Register profiling tools
        engine->registerCommand(std::make_unique<ProfileCPU>());
        
        // Additional profiling tools would be registered here:
        // engine->registerCommand(std::make_unique<ProfileMemory>());
        // engine->registerCommand(std::make_unique<ProfileIO>());
        // engine->registerCommand(std::make_unique<ProfileTime>());
        // engine->registerCommand(std::make_unique<ProfileFlameGraph>());
        // engine->registerCommand(std::make_unique<ProfileCallGraph>());
        // engine->registerCommand(std::make_unique<ProfileThread>());
        // engine->registerCommand(std::make_unique<ProfileContextSwitch>());
        // engine->registerCommand(std::make_unique<ProfileCache>());
        // engine->registerCommand(std::make_unique<ProfileBranch>());
        
        // Store engine for later use
        // In real implementation, would be stored as member variable
        std::cout << "Profiling tools registered successfully" << std::endl;
    }
    
    bool executeProfilingCommand(const std::string& commandName,
                                const std::vector<std::string>& args,
                                Config& config) {
        // Execute profiling command through CLI engine
        // This would delegate to the appropriate profiling tool
        std::cout << "Executing profiling command: " << commandName << std::endl;
        
        // For demonstration, just show help
        auto engine = std::make_shared<CLIEngine>();
        engine->displayHelp(commandName);
        
        return true;
    }
};

} // namespace nlm
