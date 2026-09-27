// Configuration command implementation
#pragma once

#include "BaseCommand.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {
namespace cli {

class ConfigCommand : public BaseCommand {
public:
    ConfigCommand() : BaseCommand("config", "Configuration management commands") {}
    
    void addArguments() override {
        // Main config command arguments (handled by subcommand selection)
    }
    
    CommandResult execute() override {
        return CommandResult(0, "Config command requires subcommand: show, set, get, validate, reset, or save\n");
    }
};

class ConfigShowCommand : public BaseCommand {
public:
    ConfigShowCommand() : BaseCommand("show", "Display current configuration") {
        addArgument(Argument("all", "Show all configuration values", "bool", "false"));
        addArgument(Argument("source", "Show configuration source", "string", ""));
    }
    
    void addArguments() override {
        addArgument(Argument("all", "Show all configuration values", "bool", "false"));
        addArgument(Argument("source", "Show configuration source", "string", ""));
    }
    
    CommandResult execute() override {
        std::stringstream output;
        output << "Current Configuration:" << std::endl;
        
        if (!config) {
            output << "  No configuration loaded" << std::endl;
            return CommandResult(0, output.str());
        }
        
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            std::string valueStr;
            
            if (config->has(key)) {
                if (config->get<std::string>(key)) {
                    valueStr = *config->get<std::string>(key);
                } else if (config->get<int>(key)) {
                    valueStr = std::to_string(*config->get<int>(key));
                } else if (config->get<double>(key)) {
                    valueStr = std::to_string(*config->get<double>(key));
                } else if (config->get<bool>(key)) {
                    valueStr = *config->get<bool>(key) ? "true" : "false";
                } else if (config->get<std::vector<std::string>>(key)) {
                    const auto& vec = *config->get<std::vector<std::string>>(key);
                    std::stringstream vecStr;
                    vecStr << "[";
                    for (size_t i = 0; i < vec.size(); ++i) {
                        if (i > 0) vecStr << ", ";
                        vecStr << vec[i];
                    }
                    vecStr << "]";
                    valueStr = vecStr.str();
                }
            }
            
            output << "  " << std::left << std::setw(30) << key << " = " << valueStr << std::endl;
        }
        
        output << std::endl;
        output << "Total keys: " << keys.size() << std::endl;
        
        return CommandResult(0, output.str());
    }
};

class ConfigSetCommand : public BaseCommand {
public:
    ConfigSetCommand() : BaseCommand("set", "Set configuration value") {
        addArgument(Argument("key", "Configuration key", "string", "", true));
        addArgument(Argument("value", "Configuration value", "string", "", true));
    }
    
    void addArguments() override {
        addArgument(Argument("key", "Configuration key", "string", "", true));
        addArgument(Argument("value", "Configuration value", "string", "", true));
    }
    
    CommandResult execute() override {
        if (!config) {
            return CommandResult(1, "No configuration available");
        }
        
        std::string key = getConfigValue("key");
        std::string valueStr = getConfigValue("value");
        
        // Try to infer type from value
        if (valueStr.empty()) {
            return CommandResult(1, "Value cannot be empty");
        }
        
        bool isNumeric = true;
        for (char c : valueStr) {
            if (!std::isdigit(c) && c != '-' && c != '.') {
                isNumeric = false;
                break;
            }
        }
        
        bool isBool = (valueStr == "true" || valueStr == "false");
        
        if (isBool) {
            config->set(key, valueStr == "true", ConfigSource::Runtime);
        } else if (isNumeric) {
            // Try to parse as double first
            try {
                size_t pos;
                double doubleVal = std::stod(valueStr, &pos);
                if (pos == valueStr.size()) {
                    config->set(key, doubleVal, ConfigSource::Runtime);
                } else {
                    config->set(key, valueStr, ConfigSource::Runtime);
                }
            } catch (...) {
                config->set(key, valueStr, ConfigSource::Runtime);
            }
        } else {
            config->set(key, valueStr, ConfigSource::Runtime);
        }
        
        std::stringstream output;
        output << "Set configuration key '" << key << "' to value '" << valueStr << "'" << std::endl;
        
        return CommandResult(0, output.str());
    }
};

class ConfigGetCommand : public BaseCommand {
public:
    ConfigGetCommand() : BaseCommand("get", "Get configuration value") {
        addArgument(Argument("key", "Configuration key", "string", "", true));
    }
    
    void addArguments() override {
        addArgument(Argument("key", "Configuration key", "string", "", true));
    }
    
    CommandResult execute() override {
        if (!config) {
            return CommandResult(1, "No configuration available");
        }
        
        std::string key = getConfigValue("key");
        
        if (!config->has(key)) {
            return CommandResult(1, "Configuration key not found: " + key);
        }
        
        std::string valueStr;
        
        if (config->get<std::string>(key)) {
            valueStr = *config->get<std::string>(key);
        } else if (config->get<int>(key)) {
            valueStr = std::to_string(*config->get<int>(key));
        } else if (config->get<double>(key)) {
            valueStr = std::to_string(*config->get<double>(key));
        } else if (config->get<bool>(key)) {
            valueStr = *config->get<bool>(key) ? "true" : "false";
        } else if (config->get<std::vector<std::string>>(key)) {
            const auto& vec = *config->get<std::vector<std::string>>(key);
            std::stringstream vecStr;
            vecStr << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) vecStr << ", ";
                vecStr << vec[i];
            }
            vecStr << "]";
            valueStr = vecStr.str();
        }
        
        std::stringstream output;
        output << valueStr << std::endl;
        
        return CommandResult(0, output.str());
    }
};

class ConfigValidateCommand : public BaseCommand {
public:
    ConfigValidateCommand() : BaseCommand("validate", "Validate configuration") {}
    
    CommandResult execute() override {
        std::stringstream output;
        output << "Configuration Validation:" << std::endl;
        
        if (!config) {
            output << "  No configuration to validate" << std::endl;
            return CommandResult(1, output.str());
        }
        
        auto keys = config->getKeys();
        output << "  Found " << keys.size() << " configuration keys" << std::endl;
        
        bool hasErrors = false;
        for (const auto& key : keys) {
            // Basic validation - could be expanded with more sophisticated checks
            if (key.empty()) {
                output << "  ERROR: Empty configuration key" << std::endl;
                hasErrors = true;
            } else if (key.find(' ') != std::string::npos) {
                output << "  WARNING: Key contains spaces: '" << key << "'" << std::endl;
            }
        }
        
        if (!hasErrors) {
            output << "  ✓ Configuration appears valid" << std::endl;
            return CommandResult(0, output.str());
        } else {
            output << "  ✗ Configuration validation failed" << std::endl;
            return CommandResult(1, output.str());
        }
    }
};

class ConfigResetCommand : public BaseCommand {
public:
    ConfigResetCommand() : BaseCommand("reset", "Reset configuration to defaults") {}
    
    CommandResult execute() override {
        if (!config) {
            return CommandResult(1, "No configuration available to reset");
        }
        
        config->clear();
        
        // Load default configuration
        auto defaultConfig = std::make_shared<Config>();
        defaultConfig->loadFromFile("configs/default.cfg");
        
        // Copy defaults back
        auto keys = defaultConfig->getKeys();
        for (const auto& key : keys) {
            config->set(key, defaultConfig->getOr<int64_t>(key, 0), ConfigSource::Default);
        }
        
        std::stringstream output;
        output << "Configuration reset to defaults" << std::endl;
        output << "Loaded " << keys.size() << " default configuration keys" << std::endl;
        
        return CommandResult(0, output.str());
    }
};

class ConfigSaveCommand : public BaseCommand {
public:
    ConfigSaveCommand() : BaseCommand("save", "Save configuration to file") {
        addArgument(Argument("file", "Output configuration file path", "string", "", true));
    }
    
    void addArguments() override {
        addArgument(Argument("file", "Output configuration file path", "string", "", true));
    }
    
    CommandResult execute() override {
        if (!config) {
            return CommandResult(1, "No configuration available to save");
        }
        
        std::string filepath = getConfigValue("file");
        
        if (filepath.empty()) {
            return CommandResult(1, "File path cannot be empty");
        }
        
        if (config->saveToFile(filepath)) {
            std::stringstream output;
            output << "Configuration saved to: " << filepath << std::endl;
            return CommandResult(0, output.str());
        } else {
            return CommandResult(1, "Failed to save configuration to: " + filepath);
        }
    }
};

// Command factory function
inline void registerConfigCommands(CommandProcessor& processor) {
    processor.registerCommand(new ConfigCommand());
    processor.registerCommand(new ConfigShowCommand());
    processor.registerCommand(new ConfigSetCommand());
    processor.registerCommand(new ConfigGetCommand());
    processor.registerCommand(new ConfigValidateCommand());
    processor.registerCommand(new ConfigResetCommand());
    processor.registerCommand(new ConfigSaveCommand());
}

} // namespace cli
} // namespace nlm
