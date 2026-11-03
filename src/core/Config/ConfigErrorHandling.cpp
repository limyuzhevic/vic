#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>

namespace nlm {

// Enhanced Config error handling methods

namespace {

// Validate key name
bool isValidKey(const std::string& key) {
    if (key.empty()) return false;
    if (key.length() > 256) return false;
    
    // Allow alphanumeric, underscores, dots, and hyphens
    static const std::regex validKeyPattern(R"^([a-zA-Z0-9_.-]+)$");
    return std::regex_match(key, validKeyPattern);
}

// Validate file path exists and is readable
bool isValidFilepath(const std::string& filepath) {
    if (filepath.empty()) return false;
    
    std::error_code ec;
    return std::filesystem::exists(filepath, ec) && 
           std::filesystem::is_regular_file(filepath, ec) &&
           std::filesystem::exists(filepath.parent_path(), ec);
}

// Validate configuration values based on key
bool isValidConfigValue(const std::string& key, const ConfigValue& value) {
    // Basic validation - could be expanded with type-specific validation
    if (std::holds_alternative<int>(value)) {
        auto val = std::get<int>(value);
        return val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max();
    }
    else if (std::holds_alternative<int64_t>(value)) {
        auto val = std::get<int64_t>(value);
        return val >= std::numeric_limits<int64_t>::min() && val <= std::numeric_limits<int64_t>::max();
    }
    else if (std::holds_alternative<double>(value)) {
        auto val = std::get<double>(value);
        return std::isfinite(val) && std::isnormal(val);
    }
    else if (std::holds_alternative<bool>(value)) {
        return true; // Always valid
    }
    else if (std::holds_alternative<std::string>(value)) {
        auto& str = std::get<std::string>(value);
        return str.length() <= 1024; // Reasonable string length
    }
    // Vector types have additional validation if needed
    
    return true;
}

} // namespace

// Config constructor with enhanced error handling
Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration
    try {
        // Add common configuration keys with defaults
        set("brain.neuron_count", int64_t(1000), ConfigSource::Default, "Total number of neurons");
        set("brain.synapse_density", 0.1, ConfigSource::Default, "Synapse connectivity density");
        set("brain.connection_probability", 0.05, ConfigSource::Default, "Probability of connection between neurons");
        set("simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep (seconds)");
        set("random_seed", int64_t(42), ConfigSource::Default, "Random seed for reproducibility");
        
        NLM_LOG_INFO("Configuration initialized with default values");
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("Failed to initialize default configuration: " + std::string(e.what()));
        throw;
    }
}

// Enhanced loadFromFile with better error handling
bool Config::loadFromFile(const std::string& filepath) {
    if (!isValidKey(filepath)) {
        NLM_LOG_ERROR("Invalid file path: " + filepath);
        return false;
    }
    
    if (!isValidFilepath(filepath)) {
        NLM_LOG_WARNING("Configuration file not found or not readable: " + filepath);
        return false;
    }
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
            return false;
        }
        
        // Try to parse as JSON first
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // For now, clear existing config and load
        clear();
        
        // Parse simple key=value format as fallback
        std::istringstream iss(content);
        std::string line;
        int lineNum = 0;
        
        while (std::getline(iss, line)) {
            lineNum++;
            // Skip empty lines and comments
            if (line.empty() || line.find('#') == 0 || line.find("//") == 0) {
                continue;
            }
            
            // Parse key=value format
            size_t equalsPos = line.find('=');
            if (equalsPos == std::string::npos) {
                NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": " + line);
                continue;
            }
            
            std::string key = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            if (key.empty()) {
                NLM_LOG_WARNING("Empty key in config line " + std::to_string(lineNum));
                continue;
            }
            
            if (!isValidKey(key)) {
                NLM_LOG_WARNING("Invalid key name in config line " + std::to_string(lineNum) + ": " + key);
                continue;
            }
            
            // Simple value parsing (could be enhanced for more types)
            if (value == "true" || value == "false") {
                set(key, value == "true", ConfigSource::File);
            } else if (value.find('.') != std::string::npos) {
                try {
                    double doubleVal = std::stod(value);
                    set(key, doubleVal, ConfigSource::File);
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Invalid float value in config line " + std::to_string(lineNum) + ": " + value);
                }
            } else {
                try {
                    int intVal = std::stoi(value);
                    set(key, intVal, ConfigSource::File);
                } catch (const std::exception& e) {
                    // Try as int64_t
                    try {
                        int64_t int64Val = std::stoll(value);
                        set(key, int64Val, ConfigSource::File);
                    } catch (const std::exception& e2) {
                        NLM_LOG_WARNING("Invalid integer value in config line " + std::to_string(lineNum) + ": " + value);
                    }
                }
            }
        }
        
        NLM_LOG_INFO("Configuration loaded from file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while loading configuration from file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Enhanced loadFromArgs with validation
bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv || argc <= 1) {
        return true; // No arguments to process
    }
    
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Check for --key=value or --key value format
            if (arg.find("--") == 0) {
                if (arg.find("=") != std::string::npos) {
                    // --key=value format
                    size_t equalsPos = arg.find("=");
                    std::string key = arg.substr(2, equalsPos - 2);
                    std::string value = arg.substr(equalsPos + 1);
                    
                    if (!isValidKey(key)) {
                        NLM_LOG_WARNING("Invalid command line key: " + key);
                        continue;
                    }
                    
                    // Parse value based on content
                    if (value == "true" || value == "false") {
                        set(key, value == "true", ConfigSource::CommandLine);
                    } else if (value.find('.') != std::string::npos) {
                        double doubleVal = std::stod(value);
                        set(key, doubleVal, ConfigSource::CommandLine);
                    } else {
                        try {
                            int intVal = std::stoi(value);
                            set(key, intVal, ConfigSource::CommandLine);
                        } catch (...) {
                            try {
                                int64_t int64Val = std::stoll(value);
                                set(key, int64Val, ConfigSource::CommandLine);
                            } catch (...) {
                                double doubleVal = std::stod(value);
                                set(key, doubleVal, ConfigSource::CommandLine);
                            }
                        }
                    }
                } else {
                    // --key value format (next argument)
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        std::string key = arg.substr(2);
                        std::string value = argv[i + 1];
                        
                        if (!isValidKey(key)) {
                            NLM_LOG_WARNING("Invalid command line key: " + key);
                            continue;
                        }
                        
                        // Try to parse value as different types
                        if (value == "true" || value == "false") {
                            set(key, value == "true", ConfigSource::CommandLine);
                        } else {
                            try {
                                int intVal = std::stoi(value);
                                set(key, intVal, ConfigSource::CommandLine);
                            } catch (...) {
                                try {
                                    int64_t int64Val = std::stoll(value);
                                    set(key, int64Val, ConfigSource::CommandLine);
                                } catch (...) {
                                    double doubleVal = std::stod(value);
                                    set(key, doubleVal, ConfigSource::CommandLine);
                                }
                            }
                        }
                        
                        i++; // Skip the value argument
                    }
                }
            }
        }
        
        NLM_LOG_INFO("Configuration loaded from command line arguments");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while loading configuration from command line: " + std::string(e.what()));
        return false;
    }
}

// Enhanced saveToFile with error handling
bool Config::saveToFile(const std::string& filepath) const {
    if (!isValidKey(filepath)) {
        NLM_LOG_ERROR("Invalid file path for save: " + filepath);
        return false;
    }
    
    try {
        // Ensure directory exists
        std::filesystem::path path(filepath);
        if (!path.parent_path().empty()) {
            std::error_code ec;
            if (!std::filesystem::exists(path.parent_path(), ec)) {
                if (!std::filesystem::create_directories(path.parent_path(), ec)) {
                    NLM_LOG_ERROR("Cannot create directory for config file: " + filepath);
                    return false;
                }
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
            return false;
        }
        
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated by NLM v0.1.0" << std::endl;
        file << "# Source: " << (pImpl ? "unknown" : "runtime") << std::endl;
        file << std::endl;
        
        // Write all configuration entries
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << std::endl;
            file << entry.key << " = ";
            
            // Write value based on type
            if (std::holds_alternative<int>(entry.value)) {
                file << std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                file << std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                file << std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                file << (std::get<bool>(entry.value) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(entry.value)) {
                file << "\"" << std::get<std::string>(entry.value) << "\"";
            } else {
                file << "UNKNOWN_TYPE";
            }
            
            file << std::endl << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Configuration saved to file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while saving configuration to file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Enhanced set method with validation
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!isValidKey(key)) {
        throw ConfigValidationError("Invalid configuration key: " + key);
    }
    
    if (!isValidConfigValue(key, value)) {
        throw ConfigValidationError("Invalid configuration value for key: " + key);
    }
    
    // Check for critical configuration consistency
    if (key == "brain.neuron_count") {
        auto val = std::get<int64_t>(value);
        if (val <= 0 || val > 1000000) { // Reasonable bounds
            NLM_LOG_WARNING("Unusual neuron count: " + std::to_string(val));
        }
    }
    
    if (key == "brain.synapse_density") {
        auto val = std::get<double>(value);
        if (val < 0.0 || val > 1.0) {
            NLM_LOG_WARNING("Synapse density should be between 0.0 and 1.0: " + std::to_string(val));
        }
    }
    
    if (key == "simulation_timestep") {
        auto val = std::get<double>(value);
        if (val <= 0.0 || val > 0.1) { // Should be small for stability
            NLM_LOG_WARNING("Simulation timestep should be small (<= 0.1s): " + std::to_string(val));
        }
    }
    
    // Set the value
    pImpl->entries.push_back(ConfigEntry(key, value, source, ""));
    
    NLM_LOG_DEBUG("Configuration set: " + key + " = " + (std::holds_alternative<std::string>(value) ? 
        "\"" + std::get<std::string>(value) + "\"" : std::to_string(std::get<double>(value))));
}

// Enhanced get method with validation
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (!isValidKey(key)) {
        NLM_LOG_ERROR("Invalid key requested: " + key);
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& entry) { return entry.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Configuration key not found: " + key);
        return std::nullopt;
    }
    
    try {
        if constexpr (std::is_same_v<T, int>) {
            if (std::holds_alternative<int>(it->value)) {
                return std::get<int>(it->value);
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int64_t>(it->value)) {
                return std::get<int64_t>(it->value);
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(it->value)) {
                return std::get<double>(it->value);
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(it->value)) {
                return std::get<bool>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(it->value)) {
                return std::get<std::string>(it->value);
            }
        }
        
        NLM_LOG_WARNING("Type mismatch for key " + key + ". Requested " + 
                       typeid(T).name() + ", got " + typeid(it->value).name());
        return std::nullopt;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while getting config value for key " + key + ": " + std::string(e.what()));
        return std::nullopt;
    }
    
    return std::nullopt;
}

// Enhanced summary method with better formatting
std::string Config::summary() const {
    std::stringstream ss;
    ss << "NLM Configuration Summary:" << std::endl;
    ss << "=======================" << std::endl;
    ss << "Total configuration entries: " << pImpl->entries.size() << std::endl;
    ss << std::endl;
    
    // Group entries by source
    int defaultCount = 0, fileCount = 0, commandLineCount = 0, runtimeCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default: defaultCount++; break;
            case ConfigSource::File: fileCount++; break;
            case ConfigSource::CommandLine: commandLineCount++; break;
            case ConfigSource::Runtime: runtimeCount++; break;
        }
    }
    
    ss << "Sources:" << std::endl;
    ss << "  Default:     " << defaultCount << std::endl;
    ss << "  File:        " << fileCount << std::endl;
    ss << "  Command Line:" << commandLineCount << std::endl;
    ss << "  Runtime:     " << runtimeCount << std::endl;
    ss << std::endl;
    
    // Show some key values
    ss << "Key Configuration Values:" << std::endl;
    
    const std::vector<std::string> importantKeys = {
        "brain.neuron_count",
        "brain.synapse_density", 
        "simulation_timestep",
        "random_seed"
    };
    
    for (const auto& key : importantKeys) {
        auto val = get<double>(key);
        if (val.has_value()) {
            ss << "  " << key << ": " << val.value() << std::endl;
        }
    }
    
    return ss.str();
}

} // namespace nlm
