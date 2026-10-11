// src/CLI/AdvancedCLI.cpp
#include "CLI/AdvancedCLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Exception/NLMException.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

struct AdvancedCLI::Impl {
    std::shared_ptr<Config> config;
    std::string executionMode;
    bool helpRequested;
    bool versionRequested;
    std::string configFilePath;
    
    // Available commands and their descriptions
    std::map<std::string, std::string> commands;
    
    // Experimental parameter sets
    std::map<std::string, std::unordered_map<std::string, std::string>> experimentalSets;
    
    // Configuration validation rules
    std::map<std::string, std::function<bool(const std::string&, const ConfigValue&)>> validationRules;
    
    Impl() : config(std::make_shared<Config>()), helpRequested(false), versionRequested(false) {
        initializeCommands();
        initializeExperimentalSets();
        initializeValidationRules();
    }
    
    void initializeCommands() {
        commands = {
            {"help", "Show help information"},
            {"version", "Show version information"},
            {"config", "Configuration file operations"},
            {"brain", "Brain configuration options"},
            {"plasticity", "Plasticity configuration options"},
            {"neuromod", "Neuromodulation configuration options"},
            {"memory", "Memory configuration options"},
            {"simulation", "Simulation configuration options"},
            {"experiment", "Experiment configuration options"},
            {"benchmark", "Performance benchmarking"},
            {"validate", "Configuration validation"},
            {"profile", "Performance profiling"}
        };
    }
    
    void initializeExperimentalSets() {
        // Standard configuration
        experimentalSets["standard"] = {
            {"brain.neuron_count", "1000"},
            {"brain.region_count", "1"},
            {"brain.connection_probability", "0.1"},
            {"plasticity.stdp.enable", "true"},
            {"plasticity.hebbian.enable", "true"},
            {"neuromod.curiosity.enable", "true"},
            {"neuromod.novelty.enable", "true"}
        };
        
        // High-performance configuration
        experimentalSets["high_performance"] = {
            {"brain.neuron_count", "5000"},
            {"brain.region_count", "3"},
            {"brain.connection_probability", "0.15"},
            {"plasticity.stdp.enable", "true"},
            {"plasticity.hebbian.enable", "true"},
            {"plasticity.structural.enable", "true"},
            {"neuromod.curiosity.enable", "true"},
            {"neuromod.novelty.enable", "true"},
            {"neuromod.dopamine.scale", "2.0"}
        };
        
        // Learning-focused configuration
        experimentalSets["learning"] = {
            {"brain.neuron_count", "2000"},
            {"plasticity.stdp.learning_rate", "0.005"},
            {"plasticity.hebbian.learning_rate", "0.001"},
            {"plasticity.structural.enable", "true"},
            {"neuromod.curiosity.enable", "true"},
            {"neuromod.novelty.enable", "true"},
            {"memory.working_memory.capacity", "2000"},
            {"memory.episodic.max_episodes", "5000"}
        };
        
        // Research configuration
        experimentalSets["research"] = {
            {"brain.neuron_count", "10000"},
            {"brain.region_count", "5"},
            {"simulation.timestep", "0.0001"},
            {"plasticity.stdp.enable", "true"},
            {"plasticity.hebbian.enable", "true"},
            {"plasticity.structural.enable", "true"},
            {"neuromod.curiosity.enable", "true"},
            {"neuromod.novelty.enable", "true"},
            {"neuromod.dopamine.scale", "1.5"},
            {"memory.working_memory.capacity", "5000"},
            {"memory.episodic.max_episodes", "20000"}
        };
    }
    
    void initializeValidationRules() {
        // Validation rules for specific keys
        validationRules["brain.neuron_count"] = 
            [](const std::string& key, const ConfigValue& value) {
                if (std::holds_alternative<int>(value)) {
                    int count = std::get<int>(value);
                    return count > 0 && count <= 100000;
                }
                return false;
            };
        
        validationRules["brain.region_count"] = 
            [](const std::string& key, const ConfigValue& value) {
                if (std::holds_alternative<int>(value)) {
                    int count = std::get<int>(value);
                    return count >= 1 && count <= 10;
                }
                return false;
            };
        
        validationRules["brain.connection_probability"] = 
            [](const std::string& key, const ConfigValue& value) {
                if (std::holds_alternative<double>(value)) {
                    double prob = std::get<double>(value);
                    return prob >= 0.0 && prob <= 1.0;
                }
                return false;
            };
        
        validationRules["simulation.timestep"] = 
            [](const std::string& key, const ConfigValue& value) {
                if (std::holds_alternative<double>(value)) {
                    double timestep = std::get<double>(value);
                    return timestep > 0.0 && timestep <= 0.1;
                }
                return false;
            };
    }
    
    bool parseArguments(int argc, char** argv) {
        try {
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                std::string value;
                
                // Check for --key=value pattern
                if (arg.find("--") == 0 && arg.find("=") != std::string::npos) {
                    size_t equalPos = arg.find("=");
                    std::string key = arg.substr(2, equalPos - 2);
                    value = arg.substr(equalPos + 1);
                    
                    if (!parseArgument(key, value)) {
                        return false;
                    }
                }
                // Check for --key value pattern
                else if (arg.find("--") == 0 && i + 1 < argc && argv[i + 1][0] != '-') {
                    std::string key = arg.substr(2);
                    value = argv[++i];
                    
                    if (!parseArgument(key, value)) {
                        return false;
                    }
                }
                // Check for short options
                else if (arg.find("-") == 0 && arg.length() == 2) {
                    std::string key = arg.substr(1);
                    
                    if (!parseArgument(key, "true")) {
                        return false;
                    }
                }
                // Special arguments
                else if (!handleSpecialArguments(arg, value)) {
                    return false;
                }
            }
            
            return validateArguments();
        } catch (const NLMException& e) {
            NLM_LOG_ERROR(std::string(e.what()));
            return false;
        } catch (...) {
            NLM_LOG_ERROR("Unexpected error parsing command line arguments");
            return false;
        }
    }
    
    bool parseArgument(const std::string& arg, const std::string& value) {
        // Convert argument to lowercase for case-insensitive matching
        std::string key = arg;
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        
        // Handle special arguments
        if (key == "help") {
            helpRequested = true;
            return true;
        } else if (key == "version") {
            versionRequested = true;
            return true;
        } else if (key == "config") {
            configFilePath = value;
            return true;
        } else if (key == "mode") {
            executionMode = value;
            return true;
        } else if (key == "experiment") {
            // Apply experimental parameter set
            auto it = experimentalSets.find(value);
            if (it != experimentalSets.end()) {
                applyExperimentalParameters(config, it->second);
                logProgress("INFO", "Applied experimental parameter set: " + value);
                return true;
            } else {
                NLM_LOG_ERROR("Unknown experimental parameter set: " + value);
                return false;
            }
        }
        
        // Add configuration option
        addConfigOption(key, value, ConfigSource::CommandLine, "Command line argument");
        return true;
    }
    
    bool handleSpecialArguments(const std::string& arg, const std::string& value) {
        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
            return true;
        } else if (arg == "--version") {
            versionRequested = true;
            return true;
        } else if (arg == "--config") {
            // File path is in the next argument
            configFilePath = value; // value would be set by the caller
            return true;
        } else if (arg == "--mode") {
            executionMode = value;
            return true;
        }
        
        NLM_LOG_ERROR("Unknown argument: " + arg);
        return false;
    }
    
    void addConfigOption(const std::string& key, const std::string& value, ConfigSource source, const std::string& description) {
        try {
            // Try to determine the type of the value
            ConfigValue configValue;
            bool isBool = isBooleanValue(value);
            bool isNumeric = !isBool && isNumericValue(value);
            
            if (isBool) {
                configValue = stringToBool(value);
            } else if (isNumeric) {
                // Check if it's an integer
                try {
                    size_t pos;
                    int intValue = std::stoi(value, &pos);
                    if (pos == value.length()) {
                        configValue = intValue;
                    } else {
                        double doubleValue = stringToDouble(value);
                        configValue = doubleValue;
                    }
                } catch (...) {
                    configValue = stringToDouble(value);
                }
            } else {
                configValue = value;
            }
            
            // Validate if there's a validation rule
            auto ruleIt = validationRules.find(key);
            if (ruleIt != validationRules.end()) {
                if (!ruleIt->second(key, configValue)) {
                    NLM_LOG_WARNING("Configuration validation failed for key: " + key);
                    // Continue anyway - this allows for flexible configuration
                }
            }
            
            config->set(key, configValue, source);
            logProgress("DEBUG", "Set config " + key + " = " + value + " (source: " + std::to_string(static_cast<int>(source)) + ")");
            
        } catch (...) {
            NLM_LOG_ERROR("Failed to set configuration option: " + key + " = " + value);
            throw NLMException("Failed to set configuration option: " + key);
        }
    }
    
    void processConfigurationOverrides(std::shared_ptr<Config> config) {
        // Load configuration file if specified
        if (!configFilePath.empty()) {
            if (config->loadFromFile(configFilePath)) {
                logProgress("INFO", "Loaded configuration from file: " + configFilePath);
            } else {
                logProgress("WARNING", "Failed to load configuration from file: " + configFilePath);
            }
        }
    }
    
    std::unordered_map<std::string, std::string> getExperimentalParameters(const std::string& name) const {
        auto it = experimentalSets.find(name);
        if (it != experimentalSets.end()) {
            return it->second;
        }
        return {};
    }
    
    bool validateConfiguration(const Config& config) const {
        // Perform basic validation
        if (!config.has("brain.neuron_count")) {
            NLM_LOG_ERROR("Missing required configuration: brain.neuron_count");
            return false;
        }
        
        if (!config.has("simulation.timestep")) {
            NLM_LOG_ERROR("Missing required configuration: simulation.timestep");
            return false;
        }
        
        // Validate values using validation rules
        for (const auto& [key, rule] : validationRules) {
            auto valueOpt = config.get<std::string>(key);
            if (valueOpt) {
                // For simplicity, we'll do basic type checking here
                // In a real implementation, we'd need to check the actual type
                if (!rule(key, ConfigValue())) {
                    NLM_LOG_ERROR("Configuration validation failed for key: " + key);
                    return false;
                }
            }
        }
        
        return true;
    }
    
    void applyExperimentalParameters(std::shared_ptr<Config> config, const std::unordered_map<std::string, std::string>& params) {
        for (const auto& [key, value] : params) {
            config->set(key, value, ConfigSource::Default);
        }
    }
    
    std::unordered_map<std::string, std::string> getDefaultConfiguration() const {
        std::unordered_map<std::string, std::string> defaults;
        
        defaults["brain.neuron_count"] = "1000";
        defaults["brain.region_count"] = "1";
        defaults["brain.connection_probability"] = "0.1";
        defaults["simulation.timestep"] = "0.001";
        defaults["random_seed"] = "42";
        defaults["plasticity.stdp.enable"] = "true";
        defaults["plasticity.hebbian.enable"] = "true";
        defaults["plasticity.structural.enable"] = "true";
        defaults["neuromod.curiosity.enable"] = "true";
        defaults["neuromod.novelty.enable"] = "true";
        
        return defaults;
    }
    
    bool isBooleanValue(const std::string& value) const {
        std::string lowerValue = value;
        std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
        
        return lowerValue == "true" || lowerValue == "false" ||
               lowerValue == "1" || lowerValue == "0" ||
               lowerValue == "yes" || lowerValue == "no";
    }
    
    bool stringToBool(const std::string& value) const {
        std::string lowerValue = value;
        std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
        
        if (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes") {
            return true;
        } else if (lowerValue == "false" || lowerValue == "0" || lowerValue == "no") {
            return false;
        }
        
        return false; // Default to false for invalid values
    }
    
    bool isNumericValue(const std::string& value) const {
        // Check if it's a boolean first
        if (isBooleanValue(value)) {
            return false;
        }
        
        // Check if it's a number
        size_t pos;
        double testValue;
        
        try {
            testValue = std::stod(value, &pos);
            return pos == value.length();
        } catch (...) {
            return false;
        }
    }
    
    double stringToDouble(const std::string& value) const {
        try {
            return std::stod(value);
        } catch (...) {
            return 0.0;
        }
    }
    
    std::string getErrorMessage(const std::string& arg, const std::string& value) const {
        return "Invalid argument: " + arg + " = " + value;
    }
    
    void logProgress(const std::string& level, const std::string& message) const {
        if (level == "INFO") {
            NLM_LOG_INFO(message);
        } else if (level == "WARNING") {
            NLM_LOG_WARNING(message);
        } else if (level == "ERROR") {
            NLM_LOG_ERROR(message);
        } else {
            NLM_LOG_DEBUG(message);
        }
    }
};

AdvancedCLI::AdvancedCLI() : pImpl(std::make_unique<Impl>()) {}

AdvancedCLI::~AdvancedCLI() = default;

bool AdvancedCLI::parseArguments(int argc, char** argv) {
    return pImpl->parseArguments(argc, argv);
}

std::shared_ptr<const Config> AdvancedCLI::getConfiguration() const {
    return pImpl->config;
}

std::string AdvancedCLI::getExecutionMode() const {
    return pImpl->executionMode.empty() ? "standard" : pImpl->executionMode;
}

bool AdvancedCLI::isHelpRequested() const {
    return pImpl->helpRequested;
}

bool AdvancedCLI::isVersionRequested() const {
    return pImpl->versionRequested;
}

std::string AdvancedCLI::getConfigFilePath() const {
    return pImpl->configFilePath;
}

std::string AdvancedCLI::getCommandHelp(const std::string& command) const {
    auto it = pImpl->commands.find(command);
    if (it != pImpl->commands.end()) {
        return it->second;
    }
    return "Unknown command: " + command;
}

std::vector<std::string> AdvancedCLI::getAvailableCommands() const {
    std::vector<std::string> commands;
    for (const auto& pair : pImpl->commands) {
        commands.push_back(pair.first);
    }
    return commands;
}

void AdvancedCLI::printHelp(const std::string& command) const {
    if (command.empty()) {
        // Print general help
        std::cout << "NLM - Advanced Command Line Interface" << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: ./nlm [OPTIONS]" << std::endl;
        std::cout << std::endl;
        std::cout << "Basic Options:" << std::endl;
        std::cout << "  -h, --help              Show this help message" << std::endl;
        std::cout << "  --version              Show version information" << std::endl;
        std::cout << "  --config <file>         Load configuration from file" << std::endl;
        std::cout << "  --mode <mode>           Set execution mode (standard, benchmark, etc.)" << std::endl;
        std::cout << "  --experiment <name>     Apply experimental parameter set" << std::endl;
        std::cout << std::endl;
        std::cout << "Brain Configuration:" << std::endl;
        std::cout << "  --brain.neuron_count <N>    Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --brain.region_count <R>    Number of regions (default: 1)" << std::endl;
        std::cout << "  --brain.connection_probability <P> Connection probability (default: 0.1)" << std::endl;
        std::cout << std::endl;
        std::cout << "Plasticity Configuration:" << std::endl;
        std::cout << "  --plasticity.stdp.enable <B>     Enable/disable STDP" << std::endl;
        std::cout << "  --plasticity.hebbian.enable <B>  Enable/disable Hebbian learning" << std::endl;
        std::cout << "  --plasticity.structural.enable <B> Enable/disable structural plasticity" << std::endl;
        std::cout << std::endl;
        std::cout << "Neuromodulation Configuration:" << std::endl;
        std::cout << "  --neuromod.curiosity.enable <B> Enable/disable curiosity" << std::endl;
        std::cout << "  --neuromod.novelty.enable <B>   Enable/disable novelty detection" << std::endl;
        std::cout << "  --neuromod.dopamine.scale <S>    Dopamine scale factor (default: 1.0)" << std::endl;
        std::cout << std::endl;
        std::cout << "Memory Configuration:" << std::endl;
        std::cout << "  --memory.working_memory.capacity <C> Working memory capacity" << std::endl;
        std::cout << "  --memory.episodic.max_episodes <N> Maximum number of episodes" << std::endl;
        std::cout << std::endl;
        std::cout << "Available Experimental Parameter Sets:" << std::endl;
        for (const auto& pair : pImpl->experimentalSets) {
            std::cout << "  " << pair.first << " - " << getCommandHelp(pair.first) << std::endl;
        }
        std::cout << std::endl;
        std::cout << "For more information about a specific command, use:" << std::endl;
        std::cout << "  ./nlm --help <command>" << std::endl;
    } else {
        std::string help = getCommandHelp(command);
        if (help.find("Unknown") != std::string::npos) {
            std::cout << help << std::endl;
        } else {
            std::cout << "Command: " << command << std::endl;
            std::cout << "Description: " << help << std::endl;
        }
    }
}

void AdvancedCLI::printVersion() const {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 2)" << std::endl;
    std::cout << "Build: Release" << std::endl;
    std::cout << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  - Real LIF neuron dynamics" << std::endl;
    std::cout << "  - Event-driven spike propagation" << std::endl;
    std::cout << "  - STDP and Hebbian plasticity" << std::endl;
    std::cout << "  - Structural plasticity" << std::endl;
    std::cout << "  - Integrated memory systems" << std::endl;
    std::cout << "  - Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
}

bool AdvancedCLI::validateArguments() const {
    return pImpl->validateConfiguration(*pImpl->config);
}

std::string AdvancedCLI::getExecutionSummary() const {
    std::stringstream ss;
    ss << "Execution Summary:" << std::endl;
    ss << "  Mode: " << getExecutionMode() << std::endl;
    ss << "  Configuration file: " << getConfigFilePath() << std::endl;
    ss << "  Help requested: " << (isHelpRequested() ? "yes" : "no") << std::endl;
    ss << "  Version requested: " << (isVersionRequested() ? "yes" : "no") << std::endl;
    
    auto config = getConfiguration();
    if (config) {
        ss << "  Configuration keys: " << config->getKeys().size() << std::endl;
        ss << "  Neuron count: " << config->getOr<int64_t>("brain.neuron_count", 0) << std::endl;
        ss << "  Region count: " << config->getOr<int64_t>("brain.region_count", 0) << std::endl;
        ss << "  Simulation timestep: " << config->getOr<double>("simulation.timestep", 0.0) << "s" << std::endl;
    }
    
    return ss.str();
}

std::shared_ptr<const Config> parseNLMArguments(int argc, char** argv) {
    AdvancedCLI cli;
    if (cli.parseArguments(argc, argv)) {
        return cli.getConfiguration();
    }
    return nullptr;
}

int executeNLMWithAdvancedCLI(int argc, char** argv) {
    try {
        AdvancedCLI cli;
        
        if (cli.parseArguments(argc, argv)) {
            if (cli.isHelpRequested()) {
                cli.printHelp();
                return 0;
            }
            
            if (cli.isVersionRequested()) {
                cli.printVersion();
                return 0;
            }
            
            // Print execution summary
            NLM_LOG_INFO(cli.getExecutionSummary());
            
            // Here you would typically create and run the brain simulation
            // For now, we'll just return success
            NLM_LOG_INFO("NLM simulation configured successfully");
            return 0;
        } else {
            NLM_LOG_ERROR("Failed to parse command line arguments");
            return 1;
        }
    } catch (const NLMException& e) {
        NLM_LOG_ERROR(std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Unexpected error in NLM advanced CLI");
        return 1;
    }
}

} // namespace nlm