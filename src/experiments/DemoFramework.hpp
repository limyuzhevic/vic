#pragma once

/**
 * Common demo framework utilities
 * Provides shared functionality for NLM demo executables
 */

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

/**
 * Common demo utility class providing shared functionality
 */
class DemoFramework {
public:
    DemoFramework() : verbose_(false), quiet_(false), profiling_(false) {}
    virtual ~DemoFramework() {}

    /**
     * Parse command line arguments
     */
    struct Arguments {
        bool showHelp = false;
        bool showVersion = false;
        bool verbose = false;
        bool quiet = false;
        bool profiling = false;
        std::string configFile = "configs/default.cfg";
        std::vector<std::string> extraArgs;
    };

    static Arguments parseArguments(int argc, char* argv[]);

    /**
     * Initialize logging with configurable level
     */
    void initializeLogging(LogLevel level = LogLevel::Info) {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(level);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        if (verbose_) {
            NLM_LOG_INFO("Verbose logging enabled");
        }
        if (quiet_) {
            NLM_LOG_INFO("Quiet mode - minimal output");
        }
        if (profiling_) {
            NLM_LOG_INFO("Profiling mode enabled");
        }
    }

    /**
     * Load configuration from file with fallback
     */
    static std::shared_ptr<Config> loadConfig(const std::string& configFile) {
        auto config = std::make_shared<Config>();
        
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_WARNING("Could not load config from " + configFile + ", using defaults");
        }
        
        return config;
    }

    /**
     * Apply command-line overrides to config
     */
    static void applyConfigOverrides(std::shared_ptr<Config> config, int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    // Override config from command line
                    std::string value = arg.substr(arg.find('=') + 1);
                    // Basic config override - in real implementation would parse properly
                    NLM_LOG_INFO("Config override from command line: " + arg);
                }
            }
        }
    }

    /**
     * Print help information
     */
    static void printHelp(const std::string& programName) {
        std::cout << "NLM Demo Programs\n";
        std::cout << "==================\n\n";
        std::cout << "Usage: " << programName << " [options] [demo_args]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --help, -h              Show this help message\n";
        std::cout << "  --version               Show version information\n";
        std::cout << "  --verbose, -v           Enable verbose output\n";
        std::cout << "  --quiet, -q             Suppress non-essential output\n";
        std::cout << "  --profile               Enable profiling mode\n";
        std::cout << "  --config <file>         Load configuration from file\n\n";
        std::cout << "Demo-specific arguments are passed to the individual demo.\n";
    }

    /**
     * Print version information
     */
    static void printVersion() {
        std::cout << "NLM Demo v0.1.0 (Phase 6 Integration)\n";
        std::cout << "Neural Learning Machine - Experimental artificial developmental brain\n";
    }

    /**
     * Start timing for profiling
     */
    void startProfile() {
        if (profiling_) {
            profileStart_ = std::chrono::high_resolution_clock::now();
        }
    }

    /**
     * End timing for profiling
     */
    void endProfile() {
        if (profiling_ && profileStart_.time_since_epoch().count() > 0) {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(end - profileStart_).count();
            NLM_LOG_INFO("Profile duration: " << duration << " seconds");
            profileStart_ = {};
        }
    }

    /**
     * Set verbose output mode
     */
    void setVerbose(bool verbose) { verbose_ = verbose; }

    /**
     * Set quiet mode
     */
    void setQuiet(bool quiet) { quiet_ = quiet; }

    /**
     * Set profiling mode
     */
    void setProfiling(bool profiling) { profiling_ = profiling; }

    /**
     * Check if verbose output is enabled
     */
    bool isVerbose() const { return verbose_; }

    /**
     * Check if quiet mode is enabled
     */
    bool isQuiet() const { return quiet_; }

    /**
     * Check if profiling mode is enabled
     */
    bool isProfiling() const { return profiling_; }

    /**
     * Get the configuration file path
     */
    const std::string& getConfigFile() const { return arguments_.configFile; }

    /**
     * Get extra command-line arguments
     */
    const std::vector<std::string>& getExtraArgs() const { return arguments_.extraArgs; }

protected:
    Arguments arguments_;
    bool verbose_;
    bool quiet_;
    bool profiling_;
    std::chrono::high_resolution_clock::time_point profileStart_;
};

} // namespace nlm