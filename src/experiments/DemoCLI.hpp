// Common CLI utility for NLM demos
#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace nlm {
namespace cli {

// Common CLI options for NLM demos
struct DemoOptions {
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    int sandboxSteps = 100;
    std::string configFile = "configs/default.cfg";
    bool verbose = false;
    bool quiet = false;
    std::string logLevel = "info";
    
    // Pattern probe options
    bool enablePatternProbe = true;
    
    // Sandbox options  
    bool enableSandbox = true;
    bool enableCheckpoint = true;
    
    // Development options
    bool enableDevelopment = true;
};

// Common CLI argument parser
DemoOptions parseCommonArgs(int argc, char* argv[]);

// Common configuration setup
std::shared_ptr<Config> createDemoConfig(const DemoOptions& options);

// Common demo initialization helper
void initializeDemoLogger(const DemoOptions& options);

// Common cleanup function
void cleanupDemo();

// Error handling utilities
void printDemoError(const std::string& message, bool exit = true);

} // namespace cli
} // namespace nlm