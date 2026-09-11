// CommandLineParser.cpp - Implementation of command-line argument parsing

#include "CommandLineParser.hpp"
#include "core/Config/Config.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace nlm {

CommandLineParser::CommandLineParser(int argc, char** argv) : argc_(argc), argv_(argv) {
    // Initialize defaults
    showHelp_ = false;
    showVersion_ = false;
    experimentType_ = "phase2"; // Default
    configFile_ = "configs/default.cfg";
    config_ = std::make_shared<Config>();
}

bool CommandLineParser::parse() {
    parseArgs();
    parseConfigFile();
    applyParameterOverrides();
    return true;
}

void CommandLineParser::parseArgs() {
    for (int i = 1; i < argc_; ++i) {
        std::string arg(argv_[i]);
        
        // Check for help/version flags
        if (isFlag(arg, "--help") || isFlag(arg, "-h")) {
            showHelp_ = true;
        }
        
        if (isFlag(arg, "--version")) {
            showVersion_ = true;
        }
        
        // Check for experiment type
        if (isFlag(arg, "--phase2")) {
            experimentType_ = "phase2";
        } else if (isFlag(arg, "--phase6")) {
            experimentType_ = "phase6";
        }
        
        // Check for config file
        if (isConfigFlag(arg)) {
            configFile_ = arg.substr(2); // Remove --
        }
        
        // Check for parameter overrides
        if (isParameterOverride(arg)) {
            parameterOverrides_[arg.substr(2)] = "true"; // Will be parsed as bool for now
        }
    }
}

void CommandLineParser::parseConfigFile() {
    if (!config_->loadFromFile(configFile_)) {
        // If file doesn't exist, create default config
        initializeDefaultConfig();
    }
}

void CommandLineParser::applyParameterOverrides() {
    for (const auto& pair : parameterOverrides_) {
        std::string key = pair.first;
        std::string value = pair.second;
        
        // Try to determine type and set appropriately
        if (isBooleanString(value)) {
            config_->set(key, value == "true", ConfigSource::CommandLine);
        } else if (isIntegerString(value)) {
            try {
                config_->set(key, std::stoll(value), ConfigSource::CommandLine);
            } catch (...) {
                config_->set(key, value, ConfigSource::CommandLine);
            }
        } else if (isDoubleString(value)) {
            try {
                config_->set(key, std::stod(value), ConfigSource::CommandLine);
            } catch (...) {
                config_->set(key, value, ConfigSource::CommandLine);
            }
        } else {
            config_->set(key, value, ConfigSource::CommandLine);
        }
    }
}

void CommandLineParser::initializeDefaultConfig() {
    // Set default values for Phase 2
    config_->set("random_seed", 42, ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", 500, ConfigSource::Default);
    config_->set("region_count", 1, ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Plasticity defaults
    config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    config_->set("hebbian_learning_rate", 0.01f, ConfigSource::Default);
    config_->set("enable_stdp", true, ConfigSource::Default);
    config_->set("enable_hebbian", true, ConfigSource::Default);
    config_->set("enable_structural", true, ConfigSource::Default);
    
    // Structural plasticity
    config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

bool CommandLineParser::isFlag(const std::string& arg, const std::string& flag) {
    return arg == flag;
}

bool CommandLineParser::isConfigFlag(const std::string& arg) {
    return arg.size() >= 2 && arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos;
}

bool CommandLineParser::isParameterOverride(const std::string& arg) {
    return arg.size() >= 2 && arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos &&
           arg.find("=") < arg.size() - 1;
}

void CommandLineParser::printHelp() const {
    std::cout << HELP_MESSAGE << std::endl;
}

void CommandLineParser::printVersion() const {
    std::cout << VERSION_INFO << std::endl;
}

} // namespace nlm
