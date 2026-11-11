// DemoCLI.cpp - Implementation of common CLI utilities for NLM demos

#include "DemoCLI.hpp"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace cli {

DemoOptions parseCommonArgs(int argc, char* argv[]) {
    DemoOptions options;
    
    // Process command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg.substr(0, 2) == "--") {
            std::string flag = arg.substr(2);
            std::string value = "";
            
            // Check if there's a value after the flag
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                value = argv[++i];
            }
            
            // Parse different flags
            if (flag == "num-episodes") {
                try {
                    options.numEpisodes = std::stoi(value);
                } catch (const std::exception& e) {
                    printDemoError("Invalid number for --num-episodes: " + value);
                }
            } else if (flag == "steps-per-episode") {
                try {
                    options.stepsPerEpisode = std::stoi(value);
                } catch (const std::exception& e) {
                    printDemoError("Invalid number for --steps-per-episode: " + value);
                }
            } else if (flag == "sandbox-steps") {
                try {
                    options.sandboxSteps = std::stoi(value);
                } catch (const std::exception& e) {
                    printDemoError("Invalid number for --sandbox-steps: " + value);
                }
            } else if (flag == "config") {
                options.configFile = value;
            } else if (flag == "verbose") {
                options.verbose = true;
                options.logLevel = "debug";
            } else if (flag == "quiet") {
                options.quiet = true;
                options.logLevel = "error";
            } else if (flag == "log-level") {
                options.logLevel = value;
            } else if (flag == "no-pattern-probe") {
                options.enablePatternProbe = false;
            } else if (flag == "no-sandbox") {
                options.enableSandbox = false;
            } else if (flag == "no-checkpoint") {
                options.enableCheckpoint = false;
            } else if (flag == "no-development") {
                options.enableDevelopment = false;
            } else {
                printDemoError("Unknown flag: " + flag);
            }
        } else if (arg[0] == '-') {
            std::string flag = arg.substr(1);
            if (flag == "h") {
                printUsage(argv[0]);
                exit(0);
            } else if (flag == "v") {
                printVersion();
                exit(0);
            } else {
                printDemoError("Unknown flag: -" + flag);
            }
        } else {
            printDemoError("Unexpected argument: " + arg);
        }
    }
    
    return options;
}

std::shared_ptr<Config> createDemoConfig(const DemoOptions& options) {
    auto config = std::make_shared<Config>();
    
    // Set default configuration values
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Plasticity settings
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Load from file if specified and exists
    if (!options.configFile.empty() && std::filesystem::exists(options.configFile)) {
        if (config->loadFromFile(options.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
        } else {
            NLM_LOG_WARNING("Failed to load configuration from: " + options.configFile);
            NLM_LOG_WARNING("Using default configuration instead.");
        }
    }
    
    return config;
}

void initializeDemoLogger(const DemoOptions& options) {
    auto logger = std::make_shared<Logger>();
    
    LogLevel logLevel = LogLevel::Info;
    if (options.logLevel == "debug") {
        logLevel = LogLevel::Debug;
    } else if (options.logLevel == "warn") {
        logLevel = LogLevel::Warning;
    } else if (options.logLevel == "error") {
        logLevel = LogLevel::Error;
    }
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Demo logger initialized (" + options.logLevel + " level)");
}

void cleanupDemo() {
    NLM_LOG_INFO("Cleaning up demo resources...");
    Logger::setGlobal(nullptr);
}

void printDemoError(const std::string& message, bool exit) {
    std::cerr << "Demo Error: " << message << std::endl;
    
    if (exit) {
        std::exit(1);
    }
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "NLM Phase Demo executable with common CLI options" << std::endl << std::endl;
    
    std::cout << "Common options:" << std::endl;
    std::cout << "  --num-episodes <n>       Number of episodes (default: 5)" << std::endl;
    std::cout << "  --steps-per-episode <n>  Steps per episode (default: аще; default: 200)" << std::endl;
    std::cout << "  --sandbox-steps <n>      Steps for sandbox demo (default: 100)" << std::endl;
    std::cout << "  --config <file>          Configuration file path (default: configs/default.cfg)" << std::endl;
    std::cout << "  --verbose                 Enable verbose logging" << std::endl;
    std::cout << "  --quiet                  Disable informational logging" << std::endl;
    std::cout << "  --log-level <level>      Set log level (debug, info, warn, error)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Demo-specific options:" << std::endl;
    std::cout << "  --no-pattern-probe       Disable pattern probe test" << std::endl;
    std::cout << "  --no-sandbox             Disable sandbox test" << std::endl;
    std::cout << "  --no-checkpoint          Disable checkpoint test" << std::endl;
    std::cout << "  --no-development         Disable development system" << std::endl;
    
    std::cout << "Example:" << std::endl;
    std::cout << "  " << programName << " --num-episodes 3 --steps-per-episode 100 --verbose" << std::endl;
}

void printVersion() {
    std::cout << "NLM Demo Executable v0.1.0" << std::endl;
    std::cout << "Neural Learning Machine Phase Demos" << std::endl;
    std::cout << "Built with common CLI interface" << std::endl;
}

} // namespace cli
} // namespace nlm