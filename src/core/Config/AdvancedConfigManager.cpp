#include "AdvancedConfigManager.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <filesystem>

// JSON support is optional - provide fallback if not available
try_include(nlohmann/json.hpp)
    #include <nlohmann/json.hpp>
#else
// Simple JSON fallback for basic parsing
namespace nlohmann {
    class json {};
}
#endif

namespace nlm {

namespace {
    // Helper function to trim whitespace
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
    
    // Helper function to split string by delimiter
    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(trim(token));
        }
        return tokens;
    }
}

bool AdvancedConfigManager::loadFromFile(const std::string& filepath, bool validate) {
    // Determine format from file extension
    std::string format = getFormatFromFile(filepath);
    
    NLM_LOG_INFO("Loading configuration from " + filepath + " (format: " + format + ")");
    
    bool success = false;
    if (format == "cfg") {
        success = loadCfgFormat(filepath, config_);
    } else if (format == "json") {
        success = loadJsonFormat(filepath, config_);
    } else {
        NLM_LOG_ERROR("Unknown configuration format for file: " + filepath);
        return false;
    }
    
    if (!success) {
        NLM_LOG_ERROR("Failed to load configuration from: " + filepath);
        return false;
    }
    
    if (validate) {
        auto errors = validate();
        if (!errors.empty()) {
            NLM_LOG_ERROR("Configuration validation failed:");
            for (const auto& error : errors) {
                NLM_LOG_ERROR("  - " + error);
            }
            return false;
        }
        NLM_LOG_INFO("Configuration validation passed");
    }
    
    NLM_LOG_INFO("Configuration loaded successfully from: " + filepath);
    return true;
}

bool AdvancedConfigManager::saveToFile(const std::string& filepath, const std::string& format) const {
    std::string actualFormat = format;
    if (actualFormat == "auto") {
        actualFormat = getFormatFromFile(filepath);
    }
    
    NLM_LOG_INFO("Saving configuration to " + filepath + " (format: " + actualFormat + ")");
    
    bool success = false;
    if (actualFormat == "cfg") {
        success = saveCfgFormat(filepath, config_);
    } else if (actualFormat == "json") {
        success = saveJsonFormat(filepath, config_);
    } else {
        NLM_LOG_ERROR("Unknown configuration format: " + actualFormat);
        return false;
    }
    
    if (!success) {
        NLM_LOG_ERROR("Failed to save configuration to: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Configuration saved successfully to: " + filepath);
    return true;
}

void AdvancedConfigManager::applyCommandLineArgs(int argc, char* argv[], const std::string& prefix) {
    NLM_LOG_INFO("Applying command-line arguments...");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Skip help/version flags
        if (arg == "--help" || arg == "-h" || arg == "--version" || arg == "-v") {
            continue;
        }
        
        // Parse argument
        parseCommandLineArg(arg, prefix);
    }
    
    NLM_LOG_INFO("Command-line arguments applied");
}

std::map<std::string, std::string> AdvancedConfigManager::exportConfig(const std::string& configName) const {
    std::map<std::string, std::string> exported;
    
    for (const auto& key : config_->getKeys()) {
        // Filter by configName if specified
        if (!configName.empty() && key.find(configName + "::") != 0) {
            continue;
        }
        
        try {
            // Try to get appropriate type based on value
            std::string valueStr;
            
            if (config_->getOr<int64_t>(key, 0) == config_->getOr<int64_t>(key, 0)) {
                valueStr = std::to_string(config_->getOr<int64_t>(key, 0));
            } else if (config_->getOr<double>(key, 0.0) == config_->getOr<double>(key, 0.0)) {
                valueStr = std::to_string(config_->getOr<double>(key, 0.0));
            } else if (config_->getOr<float>(key, 0.0f) == config_->getOr<float>(key, 0.0f)) {
                valueStr = std::to_string(config_->getOr<float>(key, 0.0f));
            } else {
                valueStr = config_->getOr<std::string>(key, "");
            }
            
            exported[key] = valueStr;
        } catch (...) {
            exported[key] = "";
        }
    }
    
    return exported;
}

void AdvancedConfigManager::importConfig(const std::map<std::string, std::string>& configData, const std::string& configName) {
    NLM_LOG_INFO("Importing configuration...");
    
    for (const auto& [key, value] : configData) {
        // Filter by configName if specified
        if (!configName.empty() && key.find(configName + "::") != 0) {
            continue;
        }
        
        // Try to convert value to appropriate type
        try {
            // Try integer
            try {
                int64_t intVal = std::stoll(value);
                config_->set(key, intVal);
                continue;
            } catch (...) {}
            
            // Try floating point
            try {
                double floatVal = std::stod(value);
                config_->set(key, floatVal);
                continue;
            } catch (...) {}
            
            // Try boolean
            if (value == "true" || value == "1" || value == "yes") {
                config_->set(key, true);
                continue;
            } else if (value == "false" || value == "0" || value == "no") {
                config_->set(key, false);
                continue;
            }
            
            // Default to string
            config_->set(key, value);
        } catch (...) {
            NLM_LOG_WARN("Failed to import configuration value: " + key + " = " + value);
        }
    }
    
    NLM_LOG_INFO("Configuration imported successfully");
}

std::vector<std::string> AdvancedConfigManager::validate() const {
    std::vector<std::string> errors;
    
    // Basic validation rules
    if (!config_->has("simulation_timestep") || config_->getOr<double>("simulation_timestep", 0.0) <= 0.0) {
        errors.push_back("simulation_timestep must be positive");
    }
    
    if (!config_->has("neuron_count") || config_->getOr<size_t>("neuron_count", 0) == 0) {
        errors.push_back("neuron_count must be positive");
    }
    
    if (config_->has("connection_probability")) {
        float prob = config_->getOr<float>("connection_probability", 0.0f);
        if (prob < 0.0f || prob > 1.0f) {
            errors.push_back("connection_probability must be between 0.0 and 1.0");
        }
    }
    
    return errors;
}

bool AdvancedConfigManager::createBackup(const std::string& backupPath) const {
    std::string timestamp = std::to_string(std::time(nullptr));
    std::string fullPath = backupPath + ".backup_" + timestamp;
    
    NLM_LOG_INFO("Creating backup at: " + fullPath);
    return saveToFile(fullPath, "cfg");
}

bool AdvancedConfigManager::restoreFromBackup(const std::string& backupPath) {
    NLM_LOG_INFO("Restoring from backup: " + backupPath);
    
    // First clear current config
    config_->clear();
    
    // Load backup
    return loadFromFile(backupPath, true);
}

std::map<std::string, size_t> AdvancedConfigManager::getStats() const {
    std::map<std::string, size_t> stats;
    
    stats["total_keys"] = config_->getKeys().size();
    
    size_t intCount = 0, floatCount = 0, stringCount = 0, boolCount = 0;
    for (const auto& key : config_->getKeys()) {
        try {
            config_->getOr<int64_t>(key, 0);
            intCount++;
        } catch (...) {
            try {
                config_->getOr<double>(key, 0.0);
                floatCount++;
            } catch (...) {
                try {
                    config_->getOr<bool>(key, false);
                    boolCount++;
                } catch (...) {
                    stringCount++;
                }
            }
        }
    }
    
    stats["integer_values"] = intCount;
    stats["float_values"] = floatCount;
    stats["boolean_values"] = boolCount;
    stats["string_values"] = stringCount;
    
    return stats;
}

std::string AdvancedConfigManager::getFormatFromFile(const std::string& filepath) {
    // Check file extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "cfg" || ext == "config") {
        return "cfg";
    } else if (ext == "json") {
        return "json";
    }
    
    return "unknown";
}

bool AdvancedConfigManager::loadCfgFormat(const std::string& filepath, const std::shared_ptr<Config>& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse key = value format
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            NLM_LOG_WARN("Invalid configuration line " + std::to_string(lineNum) + ": " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, equalsPos));
        std::string value = trim(line.substr(equalsPos + 1));
        
        if (key.empty() || value.empty()) {
            NLM_LOG_WARN("Invalid configuration line " + std::to_string(lineNum) + ": " + line);
            continue;
        }
        
        // Try to determine type and set value
        bool set = false;
        
        // Try integer
        try {
            size_t pos;
            long long intVal = std::stoll(value, &pos);
            if (pos == value.length()) {
                config->set(key, intVal);
                set = true;
            }
        } catch (...) {}
        
        // Try floating point
        if (!set) {
            try {
                size_t pos;
                double floatVal = std::stod(value, &pos);
                if (pos == value.length()) {
                    config->set(key, floatVal);
                    set = true;
                }
            } catch (...) {}
        }
        
        // Try boolean
        if (!set) {
            std::string lowerValue = value;
            std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
            if (lowerValue == "true" || lowerValue == "yes" || lowerValue == "1") {
                config->set(key, true);
                set = true;
            } else if (lowerValue == "false" || lowerValue == "no" || lowerValue == "0") {
                config->set(key, false);
                set = true;
            }
        }
        
        // Default to string
        if (!set) {
            config->set(key, value);
        }
    }
    
    return true;
}

bool AdvancedConfigManager::saveCfgFormat(const std::string& filepath, const std::shared_ptr<Config>& config) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << std::time(nullptr) << "\n\n";
    
    for (const auto& key : config->getKeys()) {
        file << key << " = ";
        
        try {
            // Try to get and write appropriate value
            int64_t intVal = config->getOr<int64_t>(key, 0);
            file << intVal;
        } catch (...) {
            try {
                double floatVal = config->getOr<double>(key, 0.0);
                file << floatVal;
            } catch (...) {
                try {
                    bool boolVal = config->getOr<bool>(key, false);
                    file << (boolVal ? "true" : "false");
                } catch (...) {
                    std::string strVal = config->getOr<std::string>(key, "");
                    file << "\"" << strVal << "\"";
                }
            }
        }
        
        file << "\n";
    }
    
    return true;
}

bool AdvancedConfigManager::loadJsonFormat(const std::string& filepath, const std::shared_ptr<Config>& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Recursively set config values from JSON
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            const auto& value = it.value();
            
            // Handle different JSON types
            if (value.is_string()) {
                config->set(key, value.get<std::string>());
            } else if (value.is_number_integer()) {
                config->set(key, value.get<int64_t>());
            } else if (value.is_number_float()) {
                config->set(key, value.get<double>());
            } else if (value.is_boolean()) {
                config->set(key, value.get<bool>());
            } else if (value.is_array()) {
                // For arrays, convert to string representation
                std::string arrayStr = "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) arrayStr += ", ";
                    arrayStr += value[i].dump();
                }
                arrayStr += "]";
                config->set(key, arrayStr);
            } else if (value.is_object()) {
                // For objects, convert to string representation
                config->set(key, value.dump());
            } else {
                config->set(key, "null");
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON configuration: " + std::string(e.what()));
        return false;
    }
    
    return true;
}

bool AdvancedConfigManager::saveJsonFormat(const std::string& filepath, const std::shared_ptr<Config>& config) const {
    nlohmann::json jsonData;
    
    for (const auto& key : config->getKeys()) {
        // Try to get appropriate JSON value
        try {
            int64_t intVal = config->getOr<int64_t>(key, 0);
            jsonData[key] = intVal;
        } catch (...) {
            try {
                double floatVal = config->getOr<double>(key, 0.0);
                jsonData[key] = floatVal;
            } catch (...) {
                try {
                    bool boolVal = config->getOr<bool>(key, false);
                    jsonData[key] = boolVal;
                } catch (...) {
                    std::string strVal = config->getOr<std::string>(key, "");
                    jsonData[key] = strVal;
                }
            }
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << jsonData.dump(2); // Pretty print with 2-space indentation
    return true;
}

void AdvancedConfigManager::parseCommandLineArg(const std::string& arg, const std::string& prefix) {
    if (arg.substr(0, 2) == "--") {
        // Full argument format: --key=value or --key value
        size_t equalsPos = arg.find('=');
        std::string key, value;
        
        if (equalsPos != std::string::npos) {
            key = arg.substr(2, equalsPos - 2);
            value = arg.substr(equalsPos + 1);
        } else {
            // Check if next argument is value (not starting with --)
            // This would require access to next argument, so we'll handle it differently
            key = arg.substr(2);
            value = "true"; // Default boolean flag
        }
        
        if (!prefix.empty()) {
            key = prefix + "::" + key;
        }
        
        try {
            // Try to determine type
            size_t pos;
            try {
                int64_t intVal = std::stoll(value, &pos);
                if (pos == value.length()) {
                    config_->set(key, intVal);
                    return;
                }
            } catch (...) {}
            
            try {
                double floatVal = std::stod(value, &pos);
                if (pos == value.length()) {
                    config_->set(key, floatVal);
                    return;
                }
            } catch (...) {}
            
            // Boolean
            std::string lowerValue = value;
            std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
            if (lowerValue == "true" || lowerValue == "yes" || lowerValue == "1") {
                config_->set(key, true);
                return;
            } else if (lowerValue == "false" || lowerValue == "no" || lowerValue == "0") {
                config_->set(key, false);
                return;
            }
            
            // String
            config_->set(key, value);
        } catch (...) {
            NLM_LOG_WARN("Failed to parse command line argument: " + arg);
        }
    } else if (arg.substr(0, 1) == "-") {
        // Short argument format: -f
        std::string key = arg.substr(1);
        
        if (!prefix.empty()) {
            key = prefix + "::" + key;
        }
        
        // Default boolean flag for short arguments
        config_->set(key, true);
    }
}

} // namespace nlm
