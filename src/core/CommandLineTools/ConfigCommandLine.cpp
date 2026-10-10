#include "CommandLineTools.hpp"
#include "Config/ConfigHelper.hpp"
#include "ErrorHandling/ErrorHandling.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <filesystem>

namespace nlm {

ConfigCommandLine::ConfigCommandLine() = default;

ConfigCommandLine::~ConfigCommandLine() = default;

bool ConfigCommandLine::parseArguments(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle --help/-h
            if (arg == "--help" || arg == "-h") {
                showHelp();
                return false;
            }
            
            // Handle --version
            if (arg == "--version") {
                showVersion();
                return false;
            }
            
            // Handle --config
            if (arg == "--config") {
                if (i + 1 < argc) {
                    configFile_ = argv[++i];
                } else {
                    NLM_THROW_ERROR(NLMErrors::ConfigInvalidValue, "Missing file path after --config");
                }
            }
            
            // Handle --output
            if (arg == "--output") {
                if (i + 1 < argc) {
                    outputFile_ = argv[++i];
                } else {
                    NLM_THROW_ERROR(NLMErrors::ConfigInvalidValue, "Missing file path after --output");
                }
            }
            
            // Handle --verbose
            if (arg == "--verbose" || arg == "-v") {
                verbose_ = true;
            }
            
            // Handle --quiet
            if (arg == "--quiet" || arg == "-q") {
                quiet_ = true;
            }
            
            // Handle --validate
            if (arg == "--validate") {
                validateOnly_ = true;
            }
            
            // Handle --format
            if (arg == "--format") {
                if (i + 1 < argc) {
                    format_ = argv[++i];
                } else {
                    NLM_THROW_ERROR(NLMErrors::ConfigInvalidValue, "Missing format after --format");
                }
            }
            
            // Handle --create-default
            if (arg == "--create-default") {
                createDefaultConfig_ = true;
            }
            
            // Handle --list
            if (arg == "--list") {
                listKeys_ = true;
            }
            
            // Handle --get
            if (arg == "--get") {
                if (i + 1 < argc) {
                    getKey_ = argv[++i];
                } else {
                    NLM_THROW_ERROR(NLMErrors::ConfigInvalidValue, "Missing key after --get");
                }
            }
            
            // Handle --set
            if (arg == "--set") {
                if (i + 2 < argc) {
                    setKey_ = argv[++i];
                    setValue_ = argv[++i];
                } else {
                    NLM_THROW_ERROR(NLMErrors::ConfigInvalidValue, "Missing key=value after --set");
                }
            }
            
            // Handle --merge
            if (arg == "--merge") {
                if (i + 1 < argc) {
                    mergeFile_ = argv[++i];
                } else {
                    NLM_THROW_ERROR(NLMErrors::ConfigInvalidValue, "Missing file path after --merge");
                }
            }
            
            // Handle --schema
            if (arg == "--schema") {
                if (i + 1 < argc) {
                    schemaFile_ = argv[++i];
                } else {
                    NLM_THROW_ERROR(NLMErrors::ConfigInvalidValue, "Missing schema file path after --schema");
                }
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigCommandLine::loadConfiguration() {
    try {
        ConfigHelper config;
        
        // Load main configuration file
        if (!configFile_.empty()) {
            if (!config.loadFromFile(configFile_)) {
                NLM_THROW_ERROR(NLMErrors::ConfigFileNotFound, "Cannot load configuration file: " + configFile_);
            }
        } else {
            // Load default configuration
            config = ConfigHelper::createDefaultConfig();
        }
        
        // Store configuration for later use
        config_ = std::move(config);
        
        // Merge with other configuration if specified
        if (!mergeFile_.empty()) {
            ConfigHelper mergeConfig;
            if (mergeConfig.loadFromFile(mergeFile_)) {
                config_.merge(mergeConfig);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigCommandLine::processCommands() {
    try {
        // List configuration keys
        if (listKeys_) {
            listKeys();
        }
        
        // Get specific value
        if (!getKey_.empty()) {
            getValue(getKey_);
        }
        
        // Set specific value
        if (!setKey_.empty()) {
            setValue(setKey_, setValue_);
        }
        
        // Create default configuration
        if (createDefaultConfig_) {
            createDefaultConfig();
        }
        
        // Validate configuration
        if (validateOnly_) {
            validateConfiguration();
        }
        
        // Save configuration
        if (!outputFile_.empty()) {
            saveConfiguration();
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

void ConfigCommandLine::listKeys() {
    std::cout << "Configuration Keys:" << std::endl;
    std::cout << "===================" << std::endl;
    
    auto keys = config_.getKeys();
    std::sort(keys.begin(), keys.end());
    
    for (const auto& key : keys) {
        std::cout << "  " << key;
        
        auto value = getConfigValue(key);
        if (value) {
            std::cout << " = " << value;
        }
        
        std::cout << std::endl;
    }
}

void ConfigCommandLine::getValue(const std::string& key) {
    std::cout << "Key: " << key << std::endl;
    
    auto value = getConfigValue(key);
    if (value) {
        std::cout << "Value: " << value << std::endl;
    } else {
        std::cout << "Value: (not set)" << std::endl;
    }
}

void ConfigCommandLine::setValue(const std::string& key, const std::string& valueStr) {
    try {
        // Try to parse value to appropriate type
        config_.set(key, parseValue(valueStr));
        
        std::cout << "Set " << key << " = " << valueStr << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void ConfigCommandLine::saveConfiguration() {
    try {
        std::string jsonContent = config_.toJSON(true); // Pretty print
        
        std::ofstream file(outputFile_);
        if (!file.is_open()) {
            NLM_THROW_ERROR(NLMErrors::ConfigFileNotFound, "Cannot open output file: " + outputFile_);
        }
        
        file << jsonContent;
        file.close();
        
        std::cout << "Configuration saved to: " << outputFile_ << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void ConfigCommandLine::createDefaultConfig() {
    config_ = ConfigHelper::createDefaultConfig();
    
    std::cout << "Created default configuration" << std::endl;
    std::cout << "Save to file using --output option" << std::endl;
}

void ConfigCommandLine::validateConfiguration() {
    std::cout << "Validating configuration..." << std::endl;
    
    // Convert to JSON for validation
    std::string jsonContent = config_.toJSON(true);
    
    // Basic validation - check if it's valid JSON
    try {
        ConfigHelper testConfig;
        if (testConfig.loadFromString(jsonContent)) {
            std::cout << "Configuration is valid JSON" << std::endl;
        } else {
            std::cout << "Configuration is NOT valid JSON" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Validation error: " << e.what() << std::endl;
    }
}

void ConfigCommandLine::showHelp() {
    std::cout << "NLM Configuration Command Line Tool" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Usage: nlm-config [options]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help                    Show this help message" << std::endl;
    std::cout << "  --version                     Show version information" << std::endl;
    std::cout << "  -v, --verbose                 Enable verbose output" << std::endl;
    std::cout << "  -q, --quiet                   Disable output" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --config <file>               Load configuration from file" << std::endl;
    std::cout << "  --output <file>               Save configuration to file" << std::endl;
    std::cout << "  --format <type>               Output format (json, keyvalue)" << std::endl;
    std::cout << "  --create-default              Create default configuration" << std::endl;
    std::cout << "  --validate                    Validate configuration only" << std::endl;
    std::cout << "  --schema <file>               Validate using schema file" << std::endl;
    std::cout << "  --merge <file>                Merge with configuration file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Query Options:" << std::endl;
    std::cout << "  --list                        List all configuration keys" << std::endl;
    std::cout << "  --get <key>                   Get value for specific key" << std::endl;
    std::cout << "  --set <key> <value>           Set value for specific key" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm-config --config config.json --list" << std::endl;
    std::cout << "  nlm-config --config config.json --get brain.neuron.count" << std::endl;
    std::cout << "  nlm-config --create-default --output config.json" << std::endl;
    std::cout << "  nlm-config --config config.json --set debug.enabled true" << std::endl;
    std::cout << std::endl;
}

void ConfigCommandLine::showVersion() {
    std::cout << "NLM Configuration Tool v6.0.0 (Phase 6)" << std::endl;
    std::cout << "Built with JSON/YAML parser support" << std::endl;
    std::cout << "Enhanced error handling and configuration management" << std::endl;
}

ConfigValue ConfigCommandLine::parseValue(const std::string& str) {
    // Try to parse string to appropriate type
    if (str == "true" || str == "false") {
        bool boolVal = (str == "true");
        return ConfigValue(boolVal);
    }
    
    try {
        // Try integer
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            return ConfigValue(intVal);
        }
        
        // Try double
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return ConfigValue(doubleVal);
        }
        
        // Fall back to string
        return ConfigValue(str);
        
    } catch (const std::exception&) {
        return ConfigValue(str);
    }
}

std::string ConfigCommandLine::getConfigValue(const std::string& key) {
    auto val = config_.get<std::string>(key);
    if (val) {
        return *val;
    }
    
    // Try other types
    auto intVal = config_.get<int>(key);
    if (intVal) {
        return std::to_string(*intVal);
    }
    
    auto doubleVal = config_.get<double>(key);
    if (doubleVal) {
        return std::to_string(*doubleVal);
    }
    
    auto boolVal = config_.get<bool>(key);
    if (boolVal) {
        return (*boolVal ? "true" : "false");
    }
    
    return "";
}

} // namespace nlm
