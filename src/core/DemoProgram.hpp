#pragma once

/**
 * Base class for all NLM demo programs
 * Provides common functionality and interfaces for CLI integration
 */

#include <string>
#include <iostream>
#include <memory>
#include <chrono>
#include <iomanip>
#include "core/Config/Config.hpp"
#include "core/CLI/CLIFramework.hpp"

namespace nlm {

class DemoProgram {
public:
    CLIParser parser;
    ProgressReporter reporter;
    std::shared_ptr<Config> config;
    
    DemoProgram(const std::string& programName)
        : parser(programName), reporter(false, false, true) {
        
        // Add common CLI arguments
        parser.addArgument("help", "Show help message", false);
        parser.addArgument("version", "Show version information", false);
        parser.addArgument("config", "Load configuration file", false);
        parser.addArgument("verbose", "Enable verbose output", false);
        parser.addArgument("quiet", "Suppress all output", false);
        parser.addArgument("progress", "Show progress indicators", false);
    }
    
    virtual ~DemoProgram() = default;
    
    virtual int run(int argc, char** argv) {
        if (!parser.parse(argc, argv)) {
            return 0; // Help/version was displayed
        }
        
        // Update reporter settings from CLI
        reporter.verbose = parser.getBoolValue("verbose", false);
        reporter.quiet = parser.getBoolValue("quiet", false);
        reporter.showProgress = parser.getBoolValue("progress", true);
        
        // Load configuration
        std::string configFile = parser.getValue("config", "configs/default.cfg");
        config = ConfigManager::createConfig(argc, argv, configFile);
        
        // Set default values for Phase 2
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Print configuration summary if verbose
        if (reporter.verbose) {
            ConfigManager::printConfigSummary(*config);
        }
        
        // Run the demo
        return execute();
    }
    
    virtual int execute() = 0;
    
protected:
    virtual void setupConfiguration(std::shared_ptr<Config> config) {
        // Override in derived classes for demo-specific configuration
    }
    
    virtual void validateConfiguration() const {
        // Validate configuration in derived classes
    }
    
    std::string formatTime(double seconds) const {
        std::stringstream ss;
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds - hours * 3600) / 60);
        double secs = seconds - hours * 3600 - minutes * 60;
        
        if (hours > 0) {
            ss << hours << "h ";
        }
        ss << minutes << "m " << std::fixed << std::setprecision(1) << secs << "s";
        return ss.str();
    }
};

} // namespace nlm
