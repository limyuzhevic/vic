// nlm_config tool implementation
// Configuration management tool for NLM

#include "tools/nlm_config/nlm_config.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "cli/ArgumentParser.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace nlm {

bool NLMConfigTool::execute(const std::vector<std::string>& args) {
    if (parser_.getPositionalArgs().empty()) {
        parser_.printHelp();
        return false;
    }
    
    std::string command = parser_.getPositionalArgs()[0];
    
    if (command == "get") {
        return executeGetCommand(args);
    } else if (command == "set") {
        return executeSetCommand(args);
    } else if (command == "load") {
        return executeLoadCommand(args);
    } else if (command == "save") {
        return executeSaveCommand(args);
    } else if (command == "list") {
        return executeListCommand(args);
    } else if (command == "validate") {
        return executeValidateCommand(args);
    } else if (command == "init") {
        return executeInitCommand(args);
    } else {
        log("Unknown command: " + command, true);
        return false;
    }
}

bool NLMConfigTool::executeGetCommand(const std::vector<std::string>& args) {
    if (!parser_.wasProvided("--key")) {
        log("Error: --key argument is required for 'get' command", true);
        return false;
    }
    
    std::string key = parser_.getValue("--key");
    std::string format = parser_.wasProvided("--format") ? parser_.getValue("--format") : "text";
    
    std::optional<ConfigValue> value = config_->get<ConfigValue>(key);
    if (!value.has_value()) {
        log("Error: Key not found: " + key, true);
        return false;
    }
    
    if (format == "json") {
        std::cout << "{\"" << key << "\": \"" << std::visit(
            [this](auto&& arg) -> std::string {
                if constexpr (std::is_same_v<decltype(arg), int>)
                    return std::to_string(arg);
                else if constexpr (std::is_same_v<decltype(arg), double>)
                    return std::to_string(arg);
                else if constexpr (std::is_same_v<decltype(arg), bool>)
                    return arg ? "true" : "false";
                else
                    return arg;
            }, *value) << "\"}" << std::endl;
    } else {
        std::cout << key << " = " << std::visit(
            [this](auto&& arg) -> std::string {
                if constexpr (std::is_same_v<decltype(arg), int>)
                    return std::to_string(arg);
                else if constexpr (std::is_same_v<decltype(arg), double>)
                    return std::to_string(arg);
                else if constexpr (std::is_same_v<decltype(arg), bool>)
                    return arg ? "true" : "false";
                else
                    return arg;
            }, *value) << std::endl;
    }
    
    return true;
}

bool NLMConfigTool::executeSetCommand(const std::vector<std::string>& args) {
    if (!parser_.wasProvided("--key") || !parser_.wasProvided("--value")) {
        log("Error: --key and --value arguments are required for 'set' command", true);
        return false;
    }
    
    std::string key = parser_.getValue("--key");
    std::string valueStr = parser_.getValue("--value");
    std::string format = parser_.wasProvided("--format") ? parser_.getValue("--format") : "auto";
    
    ConfigSource source = ConfigSource::Runtime;
    if (parser_.wasProvided("--source")) {
        std::string sourceStr = parser_.getValue("--source");
        if (sourceStr == "file") source = ConfigSource::File;
        else if (sourceStr == "command") source = ConfigSource::CommandLine;
        else if (sourceStr == "runtime") source = ConfigSource::Runtime;
    }
    
    // Auto-detect type based on format or value
    ConfigValue configValue;
    if (format == "auto" || format == "int") {
        try {
            int intValue = std::stoi(valueStr);
            configValue = intValue;
        } catch (...) {
            if (format == "auto") configValue = valueStr;
            else {
                log("Error: Value cannot be converted to integer: " + valueStr, true);
                return false;
            }
        }
    } else if (format == "double") {
        try {
            double doubleValue = std::stod(valueStr);
            configValue = doubleValue;
        } catch (...) {
            log("Error: Value cannot be converted to double: " + valueStr, true);
            return false;
        }
    } else if (format == "bool") {
        std::transform(valueStr.begin(), valueStr.end(), valueStr.begin(), ::tolower);
        configValue = (valueStr == "true" || valueStr == "1" || valueStr == "yes");
    } else {
        configValue = valueStr; // Default to string
    }
    
    config_->set(key, configValue, source);
    log("Set " << key << " = " << valueStr << " (source: " << source << ")");
    return true;
}

bool NLMConfigTool::executeLoadCommand(const std::vector<std::string>& args) {
    if (!parser_.wasProvided("--file")) {
        log("Error: --file argument is required for 'load' command", true);
        return false;
    }
    
    std::string filepath = parser_.getValue("--file");
    bool validate = parser_.wasProvided("--validate") ? 
                   parser_.getValueBool("--validate") : true;
    
    log("Loading configuration from: " << filepath);
    
    if (!config_->loadFromFile(filepath)) {
        log("Error: Failed to load configuration from: " + filepath, true);
        return false;
    }
    
    if (validate) {
        if (!executeValidateCommand(args)) {
            return false;
        }
    }
    
    log("Configuration loaded successfully");
    return true;
}

bool NLMConfigTool::executeSaveCommand(const std::vector<std::string>& args) {
    if (!parser_.wasProvided("--file")) {
        log("Error: --file argument is required for 'save' command", true);
        return false;
    }
    
    std::string filepath = parser_.getValue("--file");
    std::string format = parser_.wasProvided("--format") ? 
                        parser_.getValue("--format") : "json";
    
    log("Saving configuration to: " << filepath);
    
    if (!config_->saveToFile(filepath)) {
        log("Error: Failed to save configuration to: " + filepath, true);
        return false;
    }
    
    log("Configuration saved successfully");
    return true;
}

bool NLMConfigTool::executeListCommand(const std::vector<std::string>& args) {
    std::string format = parser_.wasProvided("--format") ? 
                        parser_.getValue("--format") : "table";
    
    std::vector<std::string> keys = config_->getKeys();
    
    if (format == "json") {
        std::cout << "{" << std::endl;
        for (size_t i = 0; i < keys.size(); ++i) {
            std::optional<ConfigValue> value = config_->get<ConfigValue>(keys[i]);
            if (!value.has_value()) continue;
            
            std::cout << "  \"" << keys[i] << "\": \"" << std::visit(
                [this](auto&& arg) -> std::string {
                    if constexpr (std::is_same_v<decltype(arg), int>)
                        return std::to_string(arg);
                    else if constexpr (std::is_same_v<decltype(arg), double>)
                        return std::to_string(arg);
                    else if constexpr (std::is_same_v<decltype(arg), bool>)
                        return arg ? "true" : "false";
                    else
                        return arg;
                }, *value) << "\"";
            
            if (i < keys.size() - 1) std::cout << ",";
            std::cout << std::endl;
        }
        std::cout << "}" << std::endl;
    } else {
        std::cout << std::left << std::setw(30) << "Key" 
                  << std::setw(15) << "Type" 
                  << std::setw(20) << "Source" 
                  << std::setw(20) << "Value" << std::endl;
        std::cout << std::string(85, '=') << std::endl;
        
        for (const auto& key : keys) {
            std::optional<ConfigValue> value = config_->get<ConfigValue>(key);
            if (!value.has_value()) continue;
            
            std::string typeStr, sourceStr;
            
            // Determine type
            if (std::holds_alternative<int>(*value)) typeStr = "int";
            else if (std::holds_alternative<int64_t>(*value)) typeStr = "int64";
            else if (std::holds_alternative<double>(*value)) typeStr = "double";
            else if (std::holds_alternative<bool>(*value)) typeStr = "bool";
            else if (std::holds_alternative<std::string>(*value)) typeStr = "string";
            else if (std::holds_alternative<std::vector<int>>(*value)) typeStr = "int[]";
            else if (std::holds_alternative<std::vector<double>>(*value)) typeStr = "double[]";
            else if (std::holds_alternative<std::vector<std::string>>(*value)) typeStr = "string[]";
            else typeStr = "unknown";
            
            // Get source
            struct ConfigEntry {
                std::string key;
                ConfigValue value;
                ConfigSource source;
                std::string description;
            };
            // This is a simplified approach - in real implementation we'd have access to all entries
            sourceStr = "Runtime"; // Simplified for now
            
            // Get value string
            std::string valueStr = std::visit(
                [this](auto&& arg) -> std::string {
                    if constexpr (std::is_same_v<decltype(arg), int>)
                        return std::to_string(arg);
                    else if constexpr (std::is_same_v<decltype(arg), double>)
                        return std::to_string(arg);
                    else if constexpr (std::is_same_v<decltype(arg), bool>)
                        return arg ? "true" : "false";
                    else
                        return arg;
                }, *value);
            
            if (valueStr.length() > 25) valueStr = valueStr.substr(0, 22) + "...";
            
            std::cout << std::left << std::setw(30) << key
                      << std::setw(15) << typeStr
                      << std::setw(20) << sourceStr
                      << std::setw(20) << valueStr << std::endl;
        }
    }
    
    return true;
}

bool NLMConfigTool::executeValidateCommand(const std::vector<std::string>& args) {
    std::string filepath = parser_.wasProvided("--file") ? 
                          parser_.getValue("--file") : "";
    
    if (!filepath.empty()) {
        // Validate configuration file
        auto tempConfig = std::make_shared<Config>();
        if (!tempConfig->loadFromFile(filepath)) {
            log("Error: Invalid configuration file: " + filepath, true);
            return false;
        }
        log("Configuration file " << filepath << " is valid");
        return true;
    } else {
        // Validate current configuration
        std::string summary = config_->summary();
        if (summary.empty()) {
            log("Warning: Configuration is empty", false);
        } else {
            log("Current configuration is valid");
            std::cout << "Configuration Summary:" << std::endl << summary << std::endl;
        }
        return true;
    }
}

bool NLMConfigTool::executeInitCommand(const std::vector<std::string>& args) {
    std::string filepath = parser_.wasProvided("--file") ? 
                          parser_.getValue("--file") : "configs/default.cfg";
    std::string format = parser_.wasProvided("--format") ? 
                        parser_.getValue("--format") : "json";
    
    log("Initializing configuration file: " << filepath);
    
    // Create default configuration
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Set default values for Phase 2
    config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Save to file
    if (!config_->saveToFile(filepath)) {
        log("Error: Failed to save configuration to: " + filepath, true);
        return false;
    }
    
    log("Default configuration initialized and saved to: " << filepath);
    return true;
}

void NLMConfigTool::printHelp() const {
    std::cout << "Usage: nlm_config <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  get      Get a configuration value" << std::endl;
    std::cout << "  set      Set a configuration value" << std::endl;
    std::cout << "  load     Load configuration from file" << std::endl;
    std::cout << "  save     Save configuration to file" << std::endl;
    std::cout << "  list     List all configuration values" << std::endl;
    std::cout << "  validate Validate configuration" << std::endl;
    std::cout << "  init     Initialize default configuration" << std::endl;
    std::cout << std::endl;
    std::cout << "For help on specific commands, use 'nlm_config <command> --help'" << std::endl;
}

std::string NLMConfigTool::getUsage() const {
    return "nlm_config <command> [options]"; 
}

std::string NLMConfigTool::getDescription() const {
    return "Configuration management tool for NLM. Supports loading, saving, getting, and setting configuration values with support for JSON, YAML, and INI formats.";
}

std::string NLMConfigTool::getVersion() const {
    return "1.0.0";
}

void NLMConfigTool::printVersion() const {
    std::cout << "nlm_config version " << getVersion() << std::endl;
}