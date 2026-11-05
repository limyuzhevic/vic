// Enhanced NLM Configuration Implementation
// Phase 6: Improved Configuration Management with Validation

#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <ctime>

namespace nlm {

Config::Config() : data_() {
    initializeDefaults();
}

void Config::initializeDefaults() {
    // Set all default values
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    
    // STDP parameters
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Memory parameters
    set("working_memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    set("max_episodes", static_cast<int64_t>(1000), ConfigSource::Default);
    
    // Development
    set("development_enabled", true, ConfigSource::Default);
    set("plasticity_modifier", 1.0f, ConfigSource::Default);
    
    // Simulation
    set("max_steps", static_cast<int64_t>(10000), ConfigSource::Default);
    set("checkpoint_interval", static_cast<int64_t>(100), ConfigSource::Default);
    
    // Logging
    set("log_level", static_cast<int64_t>(1), ConfigSource::Default); // 0=Error, 1=Info, 2=Debug
}

bool Config::loadFromFile(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Config file not found: " + filename + ", using defaults");
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        int successfulLines = 0;
        int malformedLines = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }
            
            // Parse key-value pairs
            size_t delimiterPos = line.find('=');
            if (delimiterPos == std::string::npos) {
                malformedLines++;
                continue;
            }
            
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            
            // Trim whitespace
            key = trim(key);
            value = trim(value);
            
            if (key.empty() || value.empty()) {
                malformedLines++;
                continue;
            }
            
            if (parseLine(key, value)) {
                successfulLines++;
            } else {
                malformedLines++;
            }
        }
        
        file.close();
        
        if (successfulLines > 0) {
            NLM_LOG_INFO("Loaded " + std::to_string(successfulLines) + " configuration parameters from: " + filename);
            if (malformedLines > 0) {
                NLM_LOG_WARNING("Skipped " + std::to_string(malformedLines) + " malformed lines in config file");
            }
            return true;
        } else if (malformedLines > 0) {
            NLM_LOG_ERROR("No valid configuration parameters found in: " + filename);
            return false;
        } else {
            NLM_LOG_WARNING("Empty config file: " + filename + ", using defaults");
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while loading config file " + filename + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char* argv[]) {
    bool anyArgs = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--config" && i + 1 < argc) {
            // Already handled by loadFromFile
            continue;
        } else if (arg.find("--") == 0) {
            // Parse --key=value format
            std::string key = arg.substr(2);
            size_t equalPos = key.find('=');
            if (equalPos != std::string::npos) {
                std::string keyName = key.substr(0, equalPos);
                std::string keyValue = key.substr(equalPos + 1);
                
                if (parseLine(keyName, keyValue)) {
                    anyArgs = true;
                }
            }
        } else if (arg == "--help" || arg == "-h") {
            // Help handled elsewhere
            continue;
        } else if (arg == "save" || arg == "load" || arg == "run" || arg == "build" || arg == "test") {
            // Command handled elsewhere
            continue;
        } else {
            // Unknown argument, could be a value
            std::cerr << "Unknown argument: " << arg << std::endl;
        }
    }
    
    return anyArgs;
}

bool Config::parseLine(const std::string& key, const std::string& value) {
    try {
        // Check if key exists (try all types)
        ConfigSource source = ConfigSource::CommandLine;
        
        // Try to parse based on value content
        if (value.find('.') != std::string::npos) {
            // Try float
            try {
                float floatVal = std::stof(value);
                set(key, floatVal, source);
                return true;
            } catch (...) {}
            
            // Try double
            try {
                double doubleVal = std::stod(value);
                set(key, doubleVal, source);
                return true;
            } catch (...) {}
        } else if (value == "true" || value == "false") {
            // Boolean
            bool boolVal = (value == "true");
            set(key, boolVal, source);
            return true;
        } else {
            // Try integer
            try {
                int64_t intVal = std::stoll(value);
                set(key, intVal, source);
                return true;
            } catch (...) {}
            
            // Fall back to string
            set(key, value, source);
            return true;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing config line '" + key + " = " + value + "': " + std::string(e.what()));
    }
    
    return false;
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

ConfigSource Config::detectConfigSource(const std::string& key) const {
    auto it = sourceMap_.find(key);
    if (it != sourceMap_.end()) {
        return it->second;
    }
    return ConfigSource::Default;
}

bool Config::validate() const {
    bool isValid = true;
    std::vector<std::string> errors;
    
    // Validate neuron count
    auto neuronCountOpt = get<int64_t>("neuron_count");
    if (neuronCountOpt) {
        int64_t count = *neuronCountOpt;
        if (count < 10 || count > 1000000) {
            errors.push_back("neuron_count must be between 10 and 1,000,000");
            isValid = false;
        }
    }
    
    // Validate region count
    auto regionCountOpt = get<int64_t>("region_count");
    if (regionCountOpt) {
        int64_t count = *regionCountOpt;
        if (count < 1 || count > 1000) {
            errors.push_back("region_count must be between 1 and 1000");
            isValid = false;
        }
    }
    
    // Validate connection probability
    auto connProbOpt = get<float>("connection_probability");
    if (connProbOpt) {
        float prob = *connProbOpt;
        if (prob < 0.0f || prob > 1.0f) {
            errors.push_back("connection_probability must be between 0.0 and 1.0");
            isValid = false;
        }
    }
    
    // Validate simulation timestep
    auto timestepOpt = get<double>("simulation_timestep");
    if (timestepOpt) {
        double timestep = *timestepOpt;
        if (timestep <= 0.0 || timestep > 1.0) {
            errors.push_back("simulation_timestep must be between 0.0 and 1.0");
            isValid = false;
        }
    }
    
    // Validate STDP parameters
    auto ltpOpt = get<float>("stdp_ltp_weight");
    if (ltpOpt) {
        float ltp = *ltpOpt;
        if (ltp < 0.0f) {
            errors.push_back("stdp_ltp_weight must be positive");
            isValid = false;
        }
    }
    
    auto ltdOpt = get<float>("stdp_ltd_weight");
    if (ltdOpt) {
        float ltd = *ltdOpt;
        if (ltd < 0.0f) {
            errors.push_back("stdp_ltd_weight must be positive");
            isValid = false;
        }
    }
    
    // Validate structural plasticity
    auto synRateOpt = get<float>("synaptogenesis_rate");
    if (synRateOpt) {
        float rate = *synRateOpt;
        if (rate < 0.0f || rate > 0.01f) {
            errors.push_back("synaptogenesis_rate should be between 0.0 and 0.01");
        }
    }
    
    // Report validation errors
    if (!isValid) {
        NLM_LOG_ERROR("Configuration validation failed:");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  - " + error);
        }
    }
    
    return isValid;
}

void Config::logConfiguration() const {
    NLM_LOG_INFO("=== NLM Configuration ===");
    
    auto logValue = [this](const std::string& key) {
        auto neuronCountOpt = get<int64_t>("neuron_count");
        if (neuronCountOpt) {
            NLM_LOG_INFO("  " + key + ": " + std::to_string(*neuronCountOpt));
        }
    };
    
    logValue("neuron_count");
    logValue("region_count");
    
    auto logFloatValue = [this](const std::string& key) {
        auto connProbOpt = get<float>("connection_probability");
        if (connProbOpt) {
            NLM_LOG_INFO(std::fixed, std::setprecision(4));
            NLM_LOG_INFO("  " + key + ": " + std::to_string(*connProbOpt));
        }
    };
    
    logFloatValue("connection_probability");
    
    NLM_LOG_INFO("========================");
}

bool Config::saveToFile(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to create config file: " + filename);
            return false;
        }
        
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated by NLM Configuration System" << std::endl;
        file << "# Generated at: " << getCurrentTimestamp() << std::endl;
        file << std::endl;
        
        // Write all parameters
        saveToStream(file);
        
        file.close();
        NLM_LOG_INFO("Configuration saved to: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to file: " + std::string(e.what()));
        return false;
    }
}

void Config::saveToStream(std::ostream& stream) const {
    stream << "# Basic simulation parameters" << std::endl;
    auto neuronCountOpt = get<int64_t>("neuron_count");
    if (neuronCountOpt) {
        stream << "neuron_count = " << *neuronCountOpt << std::endl;
    }
    
    auto regionCountOpt = get<int64_t>("region_count");
    if (regionCountOpt) {
        stream << "region_count = " << *regionCountOpt << std::endl;
    }
    
    auto connProbOpt = get<float>("connection_probability");
    if (connProbOpt) {
        stream << "connection_probability = " << *connProbOpt << std::endl;
    }
    
    stream << std::endl;
    stream << "# Synaptic plasticity parameters" << std::endl;
    auto ltpOpt = get<float>("stdp_ltp_weight");
    if (ltpOpt) {
        stream << "stdp_ltp_weight = " << *ltpOpt << std::endl;
    }
    
    auto ltdOpt = get<float>("stdp_ltd_weight");
    if (ltdOpt) {
        stream << "stdp_ltd_weight = " << *ltdOpt << std::endl;
    }
    
    stream << std::endl;
    stream << "# Structural plasticity" << std::endl;
    auto synRateOpt = get<float>("synaptogenesis_rate");
    if (synRateOpt) {
        stream << "synaptogenesis_rate = " << *synRateOpt << std::endl;
    }
    
    auto pruneRateOpt = get<float>("pruning_rate");
    if (pruneRateOpt) {
        stream << "pruning_rate = " << *pruneRateOpt << std::endl;
    }
}

std::string Config::getCurrentTimestamp() const {
    std::time_t now = std::time(nullptr);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

} // namespace nlm
