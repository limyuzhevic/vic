#include "ConfigManager.hpp"
#include "Logger.hpp"
#include "ConfigSchema.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {
    
    std::shared_ptr<Config> ConfigManager::setupConfig(const CLI::Options& cliOptions) {
        auto config = std::make_shared<Config>();
        
        // Initialize schema
        ConfigSchema::initializeSchema();
        
        // Log configuration start
        if (cliOptions.verbose) {
            NLM_LOG_INFO("=== Configuration Setup ===");
            NLM_LOG_INFO("Loading configuration from: " + cliOptions.configFile);
        }
        
        // Load from file
        if (!cliOptions.configFile.empty()) {
            if (config->loadFromFile(cliOptions.configFile)) {
                if (cliOptions.verbose) {
                    NLM_LOG_INFO("Loaded configuration from: " + cliOptions.configFile);
                }
            } else {
                NLM_LOG_WARNING("Failed to load configuration from: " + cliOptions.configFile);
                NLM_LOG_WARNING("Using default configuration.");
            }
        } else {
            NLM_LOG_INFO("Using default configuration (no config file specified).");
        }
        
        // Apply centralized defaults from schema
        ConfigSchema::applyDefaults(*config);
        
        // Log configuration summary with validation
        logConfigurationSummary(*config, cliOptions.verbose);
        
        return config;
    }
    
    void ConfigManager::logConfigurationSummary(const Config& config, bool verbose) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration Summary:");
        
        // Get all schema parameters
        auto allParams = ConfigSchema::getAllParameterNames();
        
        for (const auto& paramName : allParams) {
            try {
                ConfigParameter param = ConfigSchema::getParameter(paramName);
                
                if (config.has(paramName)) {
                    std::string valueStr;
                    
                    if (param.typeName == "int") {
                        valueStr = std::to_string(config.getOr<int64_t>(paramName, 
                                                                     std::get<int64_t>(param.defaultValue)));
                    } else if (param.typeName == "int64_t") {
                        valueStr = std::to_string(config.getOr<int64_t>(paramName, 
                                                                     std::get<int64_t>(param.defaultValue)));
                    } else if (param.typeName == "double") {
                        valueStr = std::to_string(config.getOr<double>(paramName, 
                                                                     std::get<double>(param.defaultValue)));
                    } else if (param.typeName == "bool") {
                        valueStr = config.getOr<bool>(paramName, std::get<bool>(param.defaultValue)) ? "true" : "false";
                    } else if (param.typeName == "string") {
                        valueStr = config.getOr<std::string>(paramName, std::get<std::string>(param.defaultValue));
                    }
                    
                    if (verbose) {
                        NLM_LOG_INFO("  " + paramName + ": " + valueStr + " (from " + 
                                   getSourceString(config.getSource(paramName)) + ")");
                    } else {
                        NLM_LOG_INFO("  " + paramName + ": " + valueStr);
                    }
                } else {
                    if (verbose) {
                        NLM_LOG_INFO("  " + paramName + ": " + param.getDefaultString() + " (default)");
                    }
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error logging configuration parameter '" + paramName + "': " + e.what());
            }
        }
        
        NLM_LOG_INFO("");
    }
    
    std::string ConfigManager::getSourceString(ConfigSource source) {
        switch (source) {
            case ConfigSource::Default: return "default";
            case ConfigSource::File: return "config file";
            case ConfigSource::CommandLine: return "command line";
            case ConfigSource::Runtime: return "runtime";
            default: return "unknown";
        }
    }
    
} // namespace nlm
