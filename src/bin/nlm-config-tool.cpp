#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "src/core/Config/EnhancedConfig.hpp"

void printUsage(const std::string& programName) {
    std::cout << "=== NLM Enhanced Configuration Tool ===" << std::endl;
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << "  --config-file FILE        Load configuration from file (JSON or YAML)" << std::endl;
    std::cout << "  --json STRING             Load configuration from JSON string" << std::endl;
    std::cout << "  --yaml STRING             Load configuration from YAML string" << std::endl;
    std::cout << "  --set KEY=VALUE           Set a configuration value" << std::endl;
    std::cout << "  --get KEY                 Get a configuration value" << std::endl;
    std::cout << "  --list                    List all configuration keys" << std::endl;
    std::cout << "  --export-format FORMAT    Export configuration in specified format (json, yaml)" << std::endl;
    std::cout << "  --merge FILE              Merge configuration from file" << std::endl;
    std::cout << "  --validate SCHEMA         Validate configuration against schema" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --config-file config.json" << std::endl;
    std::cout << "  " << programName << " --json \"{\"neuron_count\": 100, \"simulation_timestep\": 0.001}\" --get neuron_count" << std::endl;
    std::cout << "  " << programName << " --merge base_config.json --set simulation.timestep=0.0005" << std::endl;
}

void printVersion() {
    std::cout << "NLM Configuration Tool v1.0.0" << std::endl;
    std::cout << "Phase 6: Enhanced Configuration Management" << std::endl;
    std::cout << "Features: JSON/YAML support, pattern matching, validation, merging" << std::endl;
}

void processFile(EnhancedConfig& config, const std::string& filepath) {
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        if (!config.loadFromJsonFile(filepath)) {
            std::cerr << "Error: Failed to load JSON file: " << filepath << std::endl;
        }
    } else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml" ||
               filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml") {
        if (!config.loadFromYamlFile(filepath)) {
            std::cerr << "Error: Failed to load YAML file: " << filepath << std::endl;
        }
    } else {
        std::cerr << "Error: Unknown file format for " << filepath << ". Expected .json, .yaml, or .yml" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    EnhancedConfig config;
    std::string jsonStr;
    std::string yamlStr;
    std::string configFile;
    std::string schemaFile;
    std::string exportFormat;
    bool mergeFlag = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "--config-file" && i + 1 < argc) {
            configFile = argv[++i];
            processFile(config, configFile);
        } else if (arg == "--json" && i + 1 < argc) {
            jsonStr = argv[++i];
            config.loadFromJson(jsonStr);
        } else if (arg == "--yaml" && i + 1 < argc) {
            yamlStr = argv[++i];
            config.loadFromYaml(yamlStr);
        } else if (arg == "--set" && i + 1 < argc) {
            std::string keyValue = argv[++i];
            size_t pos = keyValue.find('=');
            if (pos != std::string::npos && pos > 0) {
                std::string key = keyValue.substr(0, pos);
                std::string value = keyValue.substr(pos + 1);
                
                // Try to parse as different types
                bool boolVal;
                int intVal;
                double doubleVal;
                
                if (value == "true" || value == "false") {
                    boolVal = (value == "true");
                    config.set(key, boolVal, ConfigSource::CommandLine);
                } else if (std::all_of(value.begin(), value.end(), ::isdigit) || 
                          (value[0] == '-' && std::all_of(value.begin() + 1, value.end(), ::isdigit))) {
                    intVal = std::stoi(value);
                    config.set(key, intVal, ConfigSource::CommandLine);
                } else {
                    // Try as float
                    try {
                        doubleVal = std::stod(value);
                        config.set(key, doubleVal, ConfigSource::CommandLine);
                    } catch (...) {
                        // Fall back to string
                        config.set(key, value, ConfigSource::CommandLine);
                    }
                }
            }
        } else if (arg == "--get" && i + 1 < argc) {
            std::string key = argv[++i];
            if (config.has(key)) {
                std::visit([](auto&& val) {
                    std::cout << val << std::endl;
                }, config.get<const ConfigValue&>(key).value());
            } else {
                std::cout << "Key not found: " << key << std::endl;
            }
        } else if (arg == "--list") {
            std::cout << "Configuration keys:" << std::endl;
            for (const auto& entry : config.getKeys()) {
                std::cout << "  " << entry << std::endl;
            }
        } else if (arg == "--export-format" && i + 1 < argc) {
            exportFormat = argv[++i];
            if (exportFormat == "json" || exportFormat == "yaml") {
                std::cout << EnhancedConfig::toJson(config) << std::endl;
            } else {
                std::cerr << "Error: Unsupported export format: " << exportFormat << std::endl;
            }
        } else if (arg == "--merge" && i + 1 < argc) {
            mergeFlag = true;
            EnhancedConfig mergeConfig;
            processFile(mergeConfig, argv[++i]);
            config.merge(mergeConfig);
        } else if (arg == "--validate" && i + 1 < argc) {
            schemaFile = argv[++i];
            std::ifstream schemaStream(schemaFile);
            if (schemaStream.is_open()) {
                std::stringstream schemaBuffer;
                schemaBuffer << schemaStream.rdbuf();
                if (config.validate(schemaBuffer.str())) {
                    std::cout << "Configuration is valid!" << std::endl;
                } else {
                    std::cout << "Configuration validation errors:" << std::endl;
                    for (const auto& error : config.getValidationErrors()) {
                        std::cout << "  - " << error << std::endl;
                    }
                }
            } else {
                std::cerr << "Error: Cannot open schema file: " << schemaFile << std::endl;
            }
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // If no arguments, show current config summary
    if (argc == 1) {
        std::cout << config.summary() << std::endl;
    }
    
    return 0;
}