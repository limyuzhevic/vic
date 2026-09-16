#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
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
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

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

    bool valid = true;
    std::ostringstream oss;
    
    oss << "Configuration Validation Report\n";
    oss << "==============================\n\n";
    
    // Check for duplicate keys (should not happen, but checking for safety)
    std::unordered_map<std::string, int> keyCount;
    for (const auto& entry : pImpl->entries) {
        keyCount[entry.key]++;
        if (keyCount[entry.key] > 1) {
            oss << "WARNING: Duplicate key \"" << entry.key << "\" found\n";
        }
    }
    
    // Validate value types based on key patterns
    for (const auto& entry : pImpl->entries) {
        bool typeOk = true;
        std::string typeInfo;
        
        // Basic type validation based on key patterns
        if (entry.key.find("simulation.") == 0) {
            if (entry.key == "simulation.dt") {
                if (!std::holds_alternative<double>(entry.value) && 
                    !std::holds_alternative<int>(entry.value)) {
                    typeOk = false;
                    typeInfo = "Expected numeric type for simulation.dt";
                }
            } else if (entry.key == "simulation.maxTime") {
                if (!std::holds_alternative<double>(entry.value)) {
                    typeOk = false;
                    typeInfo = "Expected double type for simulation.maxTime";
                }
            } else if (entry.key == "simulation.vision.enabled" || 
                       entry.key == "simulation.audio.enabled" ||
                       entry.key == "simulation.internal.enabled") {
                if (!std::holds_alternative<bool>(entry.value)) {
                    typeOk = false;
                    typeInfo = "Expected boolean type for simulation.* enabled flags";
                }
            }
        } else if (entry.key.find("brain.neurons.") == 0) {
            if (!std::holds_alternative<int>(entry.value)) {
                typeOk = false;
                typeInfo = "Expected integer type for brain.neurons.* counts";
            }
        } else if (entry.key.find("brain.connections.") == 0) {
            if (!std::holds_alternative<bool>(entry.value)) {
                typeOk = false;
                typeInfo = "Expected boolean type for brain.connections.* enabled flags";
            }
        } else if (entry.key.find("brain.") == 0) {
            if (!std::holds_alternative<bool>(entry.value)) {
                typeOk = false;
                typeInfo = "Expected boolean type for brain.* enabled flags";
            }
        }
        
        if (!typeOk) {
            valid = false;
            oss << "ERROR: Invalid type for key \"" << entry.key << "\": " << typeInfo << "\n";
        }
    }
    
    oss << "Validation " << (valid ? "PASSED" : "FAILED") << "\n";
    return oss.str();
}

// Get the type of a configuration parameter as string
std::string Config::getParameterType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "not_found";
    }
    
    if (std::holds_alternative<int>(it->value)) {
        return "int";
    } else if (std::holds_alternative<int64_t>(it->value)) {
        return "int64_t";
    } else if (std::holds_alternative<double>(it->value)) {
        return "double";
    } else if (std::holds_alternative<bool>(it->value)) {
        return "bool";
    } else if (std::holds_alternative<std::string>(it->value)) {
        return "string";
    } else if (std::holds_alternative<std::vector<int>>(it->value)) {
        return "std::vector<int>";
    } else if (std::holds_alternative<std::vector<double>>(it->value)) {
        return "std::vector<double>";
    } else if (std::holds_alternative<std::vector<std::string>>(it->value)) {
        return "std::vector<std::string>";
    }
    
    return "unknown";
}

// Set a configuration parameter with type checking
template<typename T>
void Config::setParameterWithType(const std::string& key, const T& value, ConfigSource source) {
    // Validate that the key is not empty
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    // Set the value with type checking
    set(key, ConfigValue(value), source);
}

// Set a configuration parameter with validation
template<typename T>
void Config::setParameter(const std::string& key, const T& value, ConfigSource source) {
    // Validate the key and value
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    // For specific parameter validation
    if (key == "simulation.dt" && value < 0.0) {
        throw std::invalid_argument("Simulation time step (dt) cannot be negative");
    }
    if (key == "simulation.maxTime" && value < 0.0) {
        throw std::invalid_argument("Maximum simulation time cannot be negative");
    }
    if (key == "brain.neurons.excitatory" && value < 0) {
        throw std::invalid_argument("Number of excitatory neurons cannot be negative");
    }
    if (key == "brain.neurons.inhibitory" && value < 0) {
        throw std::invalid_argument("Number of inhibitory neurons cannot be negative");
    }
    
    set(key, ConfigValue(value), source);
}

// Get parameter value with type validation
template<typename T>
std::optional<T> Config::getValidated(const std::string& key) const {
    auto value = get<T>(key);
    if (!value.has_value()) {
        return std::nullopt;
    }
    
    // Additional validation based on key
    if (key == "simulation.dt" && *value <= 0.0) {
        throw std::out_of_range("Invalid simulation time step value: " + std::to_string(*value));
    }
    if (key == "simulation.maxTime" && *value < 0.0) {
        throw std::out_of_range("Invalid maximum simulation time value: " + std::to_string(*value));
    }
    
    return value;
}

// Check if a configuration parameter is valid for a type
bool Config::isValidParameterType(const std::string& key, const std::type_info& typeInfo) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false; // Key not found
    }
    
    if (typeInfo == typeid(int) && std::holds_alternative<int>(it->value)) {
        return true;
    } else if (typeInfo == typeid(int64_t) && std::holds_alternative<int64_t>(it->value)) {
        return true;
    } else if (typeInfo == typeid(double) && std::holds_alternative<double>(it->value)) {
        return true;
    } else if (typeInfo == typeid(bool) && std::holds_alternative<bool>(it->value)) {
        return true;
    } else if (typeInfo == typeid(std::string) && std::holds_alternative<std::string>(it->value)) {
        return true;
    } else if (typeInfo == typeid(std::vector<int>) && std::holds_alternative<std::vector<int>>(it->value)) {
        return true;
    } else if (typeInfo == typeid(std::vector<double>) && std::holds_alternative<std::vector<double>>(it->value)) {
        return true;
    } else if (typeInfo == typeid(std::vector<std::string>) && std::holds_alternative<std::vector<std::string>>(it->value)) {
        return true;
    }
    
    return false;
}

// Get all configuration entries with detailed information
std::vector<ConfigEntry> Config::getAllEntries() const {
    return pImpl->entries;
}

// Import configuration from another config (merge)
void Config::importFrom(const Config& other) {
    for (const auto& entry : other.getAllEntries()) {
        // If key already exists, update it with the new source (overwrites)
        set(entry.key, entry.value, entry.source);
    }
}

// Get configuration statistics
ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    
    // Count by source
    for (const auto& entry : pImpl->entries) {
        int sourceIndex = static_cast<int>(entry.source);
        if (sourceIndex >= 0 && sourceIndex < 4) {
            stats.entriesBySource[sourceIndex]++;
        }
    }
    
    // Count by type
    for (const auto& entry : pImpl->entries) {
        std::string type;
        if (std::holds_alternative<int>(entry.value)) {
            type = "int";
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            type = "int64_t";
        } else if (std::holds_alternative<double>(entry.value)) {
            type = "double";
        } else if (std::holds_alternative<bool>(entry.value)) {
            type = "bool";
        } else if (std::holds_alternative<std::string>(entry.value)) {
            type = "string";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            type = "std::vector<int>";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            type = "std::vector<double>";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            type = "std::vector<std::string>";
        } else {
            type = "unknown";
        }
        stats.typeCounts[type]++;
    }
    
    return stats;
}

// Generate configuration report
std::string Config::generateReport() const {
    std::ostringstream oss;
    oss << "Configuration Report\n";
    oss << "===================\n\n";
    
    oss << "Total entries: " << pImpl->entries.size() << "\n\n";
    
    // Summary by source
    std::unordered_map<ConfigSource, size_t> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[entry.source]++;
    }
    
    oss << "Entries by source:\n";
    for (const auto& pair : sourceCounts) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << " entries\n";
    }
    oss << "\n";
    
    // Entries sorted by key
    std::vector<ConfigEntry> sortedEntries = pImpl->entries;
    std::sort(sortedEntries.begin(), sortedEntries.end(),
              [](const ConfigEntry& a, const ConfigEntry& b) { return a.key < b.key; });
    
    oss << "Configuration entries:\n";
    for (const auto& entry : sortedEntries) {
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

} // namespace nlm
