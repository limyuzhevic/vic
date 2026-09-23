#include "CLI.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

CLI::CLI() : config_(std::make_shared<Config>()), version_("NLM 0.1.0 Phase 6") {
    // Initialize default help text
    helpText_ = R"(
NLM (熙然) - Neural Learning Machine - Command Line Interface

Usage: nlm [OPTIONS] [ARGS...]

Options:
  --help                    Show this help message
  --version                 Show version information
  --config FILE             Load configuration from FILE (default: configs/default.cfg)
  --log-level LEVEL         Set logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
  --verbose                 Enable verbose logging
  --quiet                   Disable all logging output

Arguments (for specific commands):
  run                      Run the simulation
  config                    Configure runtime parameters
  checkpoint                Checkpoint management
  status                   Display system status

Common Examples:
  nlm --help
  nlm --version
  nlm --config experiment.cfg
  nlm --log-level DEBUG --verbose

For more information, visit: https://github.com/nlm-project
)";
}

CLI::~CLI() = default;

bool CLI::parse(int argc, char** argv) {
    // Store command line arguments
    args_.clear();
    for (int i = 1; i < argc; ++i) {
        args_.push_back(argv[i]);
    }
    
    // Parse configuration from arguments
    bool configSuccess = parseConfigFromArgs(argc, argv);
    
    // If no config file specified, use default
    if (config_.get() && !config_->has("random_seed")) {
        config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config_->set("simulation_timestep", 0.001, ConfigSource::Default);
        config_->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    }
    
    return configSuccess;
}

std::string CLI::getHelp() const {
    return helpText_;
}

std::string CLI::getVersion() const {
    return version_;
}

void CLI::registerCommand(const std::string& name,
                         const std::string& description,
                         std::function<bool(const std::vector<std::string>&)> handler) {
    commands_[name] = std::make_pair(description, handler);
}

bool CLI::executeCommand(const std::string& name, const std::vector<std::string>& args) {
    auto it = commands_.find(name);
    if (it == commands_.end()) {
        NLM_LOG_ERROR("Unknown command: " + name);
        return false;
    }
    
    return it->second.second(args);
}

void CLI::printUsage(const std::string& programName) const {
    std::cout << "Usage: " << programName << " [OPTIONS] [ARGS...]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << "  --config FILE             Load configuration from FILE" << std::endl;
    std::cout << "  --log-level LEVEL         Set logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)" << std::endl;
    std::cout << "  --verbose                 Enable verbose logging" << std::endl;
    std::cout << "  --quiet                   Disable all logging output" << std::endl;
}

bool CLI::parseConfigFromArgs(int argc, char** argv) {
    std::string configFile = "configs/default.cfg";
    LogLevel logLevel = LogLevel::Info;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --config=FILE or --config FILE
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
        // Handle --log-level=LEVEL
        else if (arg.find("--log-level=") != std::string::npos) {
            std::string level = arg.substr(arg.find("=") + 1);
            // Convert to lowercase
            std::transform(level.begin(), level.end(), level.begin(),
                          [](unsigned char c) { return std::tolower(c); });
            if (level == "debug") logLevel = LogLevel::Debug;
            else if (level == "info") logLevel = LogLevel::Info;
            else if (level == "warning") logLevel = LogLevel::Warning;
            else if (level == "error") logLevel = LogLevel::Error;
            else if (level == "critical") logLevel = LogLevel::Critical;
        }
        // Handle --verbose
        else if (arg == "--verbose") {
            logLevel = LogLevel::Debug;
        }
        // Handle --quiet
        else if (arg == "--quiet") {
            logLevel = LogLevel::Error;
        }
    }
    
    // Load configuration from file
    if (config_->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_WARNING("Could not load configuration from: " + configFile);
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Set default log level if not specified
    if (!config_->has("log_level")) {
        config_->set("log_level", static_cast<int64_t>(static_cast<int>(logLevel)), ConfigSource::Default);
    }
    
    return true;
}

} // namespace nlm