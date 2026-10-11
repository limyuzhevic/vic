// AdvancedArgs.cpp - Advanced argument parsing implementation for NLM

#include "cli/AdvancedArgs.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace nlm {

AdvancedArgs::AdvancedArgs()
    : helpRequested(false),
      versionRequested(false),
      verbose(false),
      debug(false),
      debugLevel(0),
      batchMode(false),
      profilingEnabled(false),
      progressReporting(false),
      validationEnabled(false),
      errorInjectionEnabled(false),
      recoveryTestingEnabled(false),
      statisticsEnabled(false),
      outputFormat("text"),
      progressInterval(100),
      errorInjectionProbability(0.0f),
      neuronCount(500),
      regionCount(1),
      timestep(0.001),
      randomSeed(42),
      connectionProbability(0.15f),
      stdpLTPWeight(0.02f),
      stdpLTDWeight(0.015f),
      plasticityMode("mixed"),
      memoryCapacity(1000),
      neuromodulationEnabled(false),
      developmentStage(1),
      checkpointInterval(1000),
      autoSaveEnabled(false) {
}

AdvancedArgs::~AdvancedArgs() = default;

bool AdvancedArgs::parse(int argc, char** argv, Config& config) {
    // First parse basic arguments
    parseBasicArgs(argc, argv, config);
    
    // Then parse advanced arguments
    parseAdvancedArgs(argc, argv, config);
    
    // Apply config overrides
    applyConfigOverrides(config);
    
    // Validate arguments
    if (!validateArgs()) {
        return false;
    }
    
    return true;
}

void AdvancedArgs::initializeDefaultOptions() {
    NLM_LOG_INFO("Initializing default CLI options...");
    
    // Set default values
    outputFile = "";
    outputFormat = "text";
    progressInterval = 100;
    configFile = "configs/default.cfg";
    profilingFile = "profile_data.json";
    batchProfile = "standard";
    errorInjectionType = "none";
    errorInjectionProbability = 0.0f;
    
    // Default values for extended options
    neuronCount = 500;
    regionCount = 1;
    timestep = 0.001;
    randomSeed = 42;
    connectionProbability = 0.15f;
    stdpLTPWeight = 0.02f;
    stdpLTDWeight = 0.015f;
    plasticityMode = "mixed";
    memoryCapacity = 1000;
    neuromodulationEnabled = false;
    developmentStage = 1;
    checkpointInterval = 1000;
    autoSaveEnabled = false;
    
    NLM_LOG_INFO("Default CLI options initialized");
}

void AdvancedArgs::parseBasicArgs(int argc, char** argv, Config& config) {
    NLM_LOG_INFO("Parsing basic command-line arguments...");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Help and version flags
        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
        } else if (arg == "--version" || arg == "-v") {
            versionRequested = true;
        } else if (arg.substr(0, 2) == "--") {
            // Handle --key=value format
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Convert to lowercase for easier comparison
                std::string lowerKey = toLower(key);
                
                // Map keys to internal variables
                if (lowerKey == "verbose") {
                    verbose = (value == "true" || value == "1" || value == "yes");
                } else if (lowerKey == "debug") {
                    debug = (value == "true" || value == "1" || value == "yes");
                    debugLevel = std::stoi(value);
                } else if (lowerKey == "batch") {
                    batchMode = (value == "true" || value == "1" || value == "yes");
                } else if (lowerKey == "profile") {
                    profilingEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (lowerKey == "progress") {
                    progressReporting = (value == "true" || value == "1" || value == "yes");
                } else if (lowerKey == "validate") {
                    validationEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (lowerKey == "error_inject") {
                    errorInjectionEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (lowerKey == "recovery_test") {
                    recoveryTestingEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (lowerKey == "statistics") {
                    statisticsEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (lowerKey == "output") {
                    outputFile = value;
                } else if (lowerKey == "format") {
                    outputFormat = value;
                } else if (lowerKey == "progress_interval") {
                    progressInterval = std::stoul(value);
                } else if (lowerKey == "config") {
                    configFile = value;
                } else if (lowerKey == "profile_file") {
                    profilingFile = value;
                } else if (lowerKey == "batch_profile") {
                    batchProfile = value;
                } else if (lowerKey == "error_injection_type") {
                    errorInjectionType = value;
                } else if (lowerKey == "error_injection_probability") {
                    errorInjectionProbability = std::stof(value);
                } else if (lowerKey == "neuron_count") {
                    neuronCount = std::stoll(value);
                } else if (lowerKey == "region_count") {
                    regionCount = std::stoll(value);
                } else if (lowerKey == "timestep") {
                    timestep = std::stod(value);
                } else if (lowerKey == "seed") {
                    randomSeed = std::stoll(value);
                } else if (lowerKey == "connection_probability") {
                    connectionProbability = std::stof(value);
                } else if (lowerKey == "stdp_ltp_weight") {
                    stdpLTPWeight = std::stof(value);
                } else if (lowerKey == "stdp_ltd_weight") {
                    stdpLTDWeight = std::stof(value);
                } else if (lowerKey == "plasticity_mode") {
                    plasticityMode = value;
                } else if (lowerKey == "memory_capacity") {
                    memoryCapacity = std::stoll(value);
                } else if (lowerKey == "neuromodulation") {
                    neuromodulationEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (lowerKey == "development_stage") {
                    developmentStage = std::stoll(value);
                } else if (lowerKey == "checkpoint_interval") {
                    checkpointInterval = std::stoll(value);
                } else if (lowerKey == "auto_save") {
                    autoSaveEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (lowerKey == "batch_config") {
                    batchConfigFiles.push_back(value);
                }
            }
        }
    }
}

void AdvancedArgs::parseAdvancedArgs(int argc, char** argv, Config& config) {
    NLM_LOG_INFO("Parsing advanced command-line arguments...");
    
    // Handle batch config files (multiple --batch-config options)
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --batch-config=value or --batch-config value
        if (arg.substr(0, 12) == "--batch-config") {
            std::string value;
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                value = arg.substr(pos + 1);
            } else if (i + 1 < argc) {
                value = argv[++i];
            }
            
            if (!value.empty()) {
                batchConfigFiles.push_back(value);
            }
        }
    }
}

void AdvancedArgs::applyConfigOverrides(Config& config) const {
    NLM_LOG_INFO("Applying configuration overrides from command line...");
    
    // Apply neuron count
    if (neuronCount > 0) {
        config.set("neuron_count", neuronCount, ConfigSource::CommandLine);
    }
    
    // Apply region count
    if (regionCount > 0) {
        config.set("region_count", regionCount, ConfigSource::CommandLine);
    }
    
    // Apply timestep
    if (timestep > 0.0) {
        config.set("simulation_timestep", timestep, ConfigSource::CommandLine);
    }
    
    // Apply random seed
    if (randomSeed > 0) {
        config.set("random_seed", randomSeed, ConfigSource::CommandLine);
    }
    
    // Apply connection probability
    if (connectionProbability >= 0.0f && connectionProbability <= 1.0f) {
        config.set("connection_probability", connectionProbability, ConfigSource::CommandLine);
    }
    
    // Apply STDP weights
    config.set("stdp_ltp_weight", stdpLTPWeight, ConfigSource::CommandLine);
    config.set("stdp_ltd_weight", stdpLTDWeight, ConfigSource::CommandLine);
    
    // Apply extended plasticity parameters
    config.set("plasticity_mode", plasticityMode, ConfigSource::CommandLine);
    
    // Apply memory system configuration
    config.set("memory_capacity", memoryCapacity, ConfigSource::CommandLine);
    
    // Apply neuromodulation settings
    config.set("neuromodulation_enabled", neuromodulationEnabled, ConfigSource::CommandLine);
    
    // Apply development stage configuration
    config.set("development_stage", developmentStage, ConfigSource::CommandLine);
    
    // Apply checkpoint and persistence options
    config.set("checkpoint_interval", checkpointInterval, ConfigSource::CommandLine);
    config.set("auto_save", autoSaveEnabled, ConfigSource::CommandLine);
}

bool AdvancedArgs::validateArgs() const {
    NLM_LOG_INFO("Validating command-line arguments...");
    
    // Validate required arguments
    if (helpRequested || versionRequested) {
        NLM_LOG_INFO("Help or version requested - validation skipped");
        return true;
    }
    
    // Validate configuration file
    if (!configFile.empty()) {
        std::ifstream file(configFile);
        if (!file.good()) {
            NLM_LOG_WARNING("Configuration file not found: " + configFile);
        }
    }
    
    // Validate batch config files
    for (const auto& batchConfig : batchConfigFiles) {
        std::ifstream file(batchConfig);
        if (!file.good()) {
            NLM_LOG_WARNING("Batch configuration file not found: " + batchConfig);
        }
    }
    
    // Validate output format
    std::string validFormats[] = {"text", "json", "csv"};
    bool formatValid = false;
    for (const auto& validFormat : validFormats) {
        if (outputFormat == validFormat) {
            formatValid = true;
            break;
        }
    }
    if (!formatValid) {
        NLM_LOG_WARNING("Invalid output format: " + outputFormat + ". Using default 'text'");
        outputFormat = "text";
    }
    
    // Validate debug level
    if (debugLevel < 0 || debugLevel > 3) {
        NLM_LOG_WARNING("Invalid debug level: " + std::to_string(debugLevel) + ". Using 0 (none)");
        debugLevel = 0;
    }
    
    // Validate error injection probability
    if (errorInjectionProbability < 0.0f || errorInjectionProbability > 1.0f) {
        NLM_LOG_WARNING("Invalid error injection probability: " + std::to_string(errorInjectionProbability) + ". Using 0.0");
        errorInjectionProbability = 0.0f;
    }
    
    // Validate plasticity mode
    std::string validModes[] = {"stdp", "hebbian", "structural", "mixed"};
    bool modeValid = false;
    for (const auto& validMode : validModes) {
        if (plasticityMode == validMode) {
            modeValid = true;
            break;
        }
    }
    if (!modeValid) {
        NLM_LOG_WARNING("Invalid plasticity mode: " + plasticityMode + ". Using 'mixed'");
        plasticityMode = "mixed";
    }
    
    // Validate development stage
    if (developmentStage < 1 || developmentStage > 6) {
        NLM_LOG_WARNING("Invalid development stage: " + std::to_string(developmentStage) + ". Using 1");
        developmentStage = 1;
    }
    
    // Validate checkpoint interval
    if (checkpointInterval < 0) {
        NLM_LOG_WARNING("Invalid checkpoint interval: " + std::to_string(checkpointInterval) + ". Using 1000");
        checkpointInterval = 1000;
    }
    
    NLM_LOG_INFO("Command-line argument validation completed");
    return true;
}

} // namespace nlm