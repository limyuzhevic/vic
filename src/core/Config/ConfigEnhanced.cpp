#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <set>
#include <numeric>
#include <chrono>
#include <random>
#include <cryptography>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> descriptions;
    std::vector<std::string> errorMessages;
    bool isValid;
    std::map<std::string, ConfigValue> defaults;
    std::string currentSnapshot;
    std::vector<std::string> backupFiles;
    bool hasBackup;
    std::string backupPath;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->isValid = true;
    pImpl->hasBackup = false;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        // Log error but don't crash - allow fallback to defaults
        pImpl->errorMessages.push_back("Cannot open config file: " + filepath);
        pImpl->isValid = false;
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        std::string trimmedLine = trim(line);
        if (trimmedLine.empty() || 
            trimmedLine[0] == '#' || 
            trimmedLine[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = trimmedLine.find('=');
        if (pos == std::string::npos) {
            pImpl->errorMessages.push_back("Invalid config format at line " + std::to_string(lineNumber) + ": missing '='");
            hasErrors = true;
            continue;
        }
        
        std::string key = trim(trimmedLine.substr(0, pos));
        std::string value = trim(trimmedLine.substr(pos + 1));
        
        if (key.empty()) {
            pImpl->errorMessages.push_back("Empty key at line " + std::to_string(lineNumber));
            hasErrors = true;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            setConfigValue(key, value);
        } catch (const std::exception& e) {
            pImpl->errorMessages.push_back("Error parsing config key '" + key + "' at line " + 
                                         std::to_string(lineNumber) + ": " + e.what());
            hasErrors = true;
        }
    }
    
    pImpl->isValid = !hasErrors;
    
    if (hasErrors) {
        // Log error but continue
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool hasErrors = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Convert value based on key name (basic type inference)
                try {
                    setConfigValue(key, value);
                } catch (const std::exception& e) {
                    pImpl->errorMessages.push_back("Error parsing CLI argument --" + key + "=" + value + ": " + e.what());
                    hasErrors = true;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            try {
                setConfigValue(key, value);
            } catch (const std::exception& e) {
                pImpl->errorMessages.push_back("Error parsing CLI argument -" + key + " " + value + ": " + e.what());
                hasErrors = true;
            }
        }
    }
    
    pImpl->isValid = !hasErrors;
    return !hasErrors;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        pImpl->errorMessages.push_back("Cannot save config to file: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->descriptions.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->descriptions.clear();
    pImpl->errorMessages.clear();
    pImpl->isValid = true;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

bool Config::validate() const {
    bool isValid = true;
    
    // Validate numeric ranges for known configuration keys
    if (has("neuron_count")) {
        int64_t neurons = getOr<int64_t>("neuron_count", -1);
        if (neurons <= 0) {
            pImpl->errorMessages.push_back("Invalid neuron_count: must be positive, got " + std::to_string(neurons));
            isValid = false;
        } else if (neurons > 1000000) {
            // Warning, not error
        }
    }
    
    if (has("region_count")) {
        int64_t regions = getOr<int64_t>("region_count", -1);
        if (regions <= 0) {
            pImpl->errorMessages.push_back("Invalid region_count: must be positive, got " + std::to_string(regions));
            isValid = false;
        }
    }
    
    if (has("connection_probability")) {
        float prob = getOr<float>("connection_probability", -1.0f);
        if (prob < 0.0f || prob > 1.0f) {
            pImpl->errorMessages.push_back("Invalid connection_probability: must be between 0.0 and 1.0, got " + std::to_string(prob));
            isValid = false;
        }
    }
    
    if (has("simulation_timestep")) {
        double timestep = getOr<double>("simulation_timestep", -1.0);
        if (timestep <= 0.0) {
            pImpl->errorMessages.push_back("Invalid simulation_timestep: must be positive, got " + std::to_string(timestep));
            isValid = false;
        } else if (timestep > 1.0) {
            // Warning, not error
        }
    }
    
    if (has("random_seed")) {
        int64_t seed = getOr<int64_t>("random_seed", -1);
        if (seed < 0) {
            pImpl->errorMessages.push_back("Invalid random_seed: must be non-negative, got " + std::to_string(seed));
            isValid = false;
        }
    }
    
    // Validate STDP parameters
    if (has("stdp_ltp_weight")) {
        float ltp = getOr<float>("stdp_ltp_weight", -1.0f);
        if (ltp < 0.0f) {
            pImpl->errorMessages.push_back("Invalid stdp_ltp_weight: must be non-negative, got " + std::to_string(ltp));
            isValid = false;
        }
    }
    
    if (has("stdp_ltd_weight")) {
        float ltd = getOr<float>("stdp_ltd_weight", -1.0f);
        if (ltd < 0.0f) {
            pImpl->errorMessages.push_back("Invalid stdp_ltd_weight: must be non-negative, got " + std::to_string(ltd));
            isValid = false;
        }
    }
    
    if (has("stdp_tau")) {
        float tau = getOr<float>("stdp_tau", -1.0f);
        if (tau <= 0.0f) {
            pImpl->errorMessages.push_back("Invalid stdp_tau: must be positive, got " + std::to_string(tau));
            isValid = false;
        }
    }
    
    return isValid;
}

std::string Config::summaryWithValidation() const {
    std::ostringstream oss;
    oss << "Configuration Validation Summary:" << std::endl;
    
    if (validate()) {
        oss << "✓ Configuration is valid" << std::endl;
    } else {
        oss << "✗ Configuration has validation errors" << std::endl;
        oss << "Error messages:" << std::endl;
        for (const auto& error : pImpl->errorMessages) {
            oss << "  - " << error << std::endl;
        }
    }
    
    oss << "\nDetailed Configuration (" << pImpl->entries.size() << " entries):" << std::endl;
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

// Implement the remaining methods...

// Helper functions
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void Config::setConfigValue(const std::string& key, const std::string& value) {
    // Convert value based on key name (basic type inference)
    if (key == "neuron-count" || key == "random-seed") {
        try {
            int64_t intVal = std::stoll(value);
            set(key, intVal, ConfigSource::CommandLine);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid integer value for " + key + ": " + value);
        }
    } else if (key == "simulation-timestep" || key == "stdp-tau" || 
               key == "synaptogenesis-rate" || key == "pruning-rate" ||
               key == "connection-prob" || key == "stdp-ltp-weight" ||
               key == "stdp-ltd-weight") {
        try {
            float floatVal = std::stof(value);
            set(key, floatVal, ConfigSource::CommandLine);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid float value for " + key + ": " + value);
        }
    } else {
        // Try as string first
        set(key, value, ConfigSource::CommandLine);
    }
}

// Template instantiations (already present)

} // namespace nlm