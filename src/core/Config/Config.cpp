#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <optional>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Fast lookup table for common access patterns
    mutable std::unordered_map<std::string, size_t> keyToIndex;
    bool dirty;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->dirty = false;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config: Failed to open file: " + filepath);
        return false;
    }
    
    // Clear existing config to avoid duplicates
    clear();
    
    // Try to detect file format based on extension
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    bool success = false;
    
    if (extension == ".json") {
        success = loadFromJSON(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        success = loadFromYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        success = loadFromSimpleFormat(filepath);
    }
    
    if (success) {
        NLM_LOG_INFO("Config: Loaded configuration from " + filepath);
    } else {
        NLM_LOG_ERROR("Config: Failed to parse configuration file: " + filepath);
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        return true; // No arguments provided
    }
    
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
        NLM_LOG_ERROR("Config: Failed to save file: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated at " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "# Source: " << static_cast<int>(entry.source) << "\n\n";
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
        NLM_LOG_WARNING("Config: Type mismatch for key '" + key + "', expected " + typeid(T).name());
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        // Validate default value for critical parameters
        validateDefaultValue(key, defaultValue);
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key format
    if (key.empty() || !isValidKey(key)) {
        NLM_LOG_ERROR("Config: Invalid key name: '" + key + "'");
        return;
    }
    
    // Mark as dirty for tracking
    pImpl->dirty = true;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        ConfigEntry newEntry(key, value, source, "Automatically set configuration");
        pImpl->entries.emplace_back(std::move(newEntry));
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Validate numeric ranges for critical parameters
    validateNumericRange(key, value);
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
    pImpl->dirty = true;
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    // Clear cache
    pImpl->keyToIndex.clear();
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
    pImpl->keyToIndex.clear();
    pImpl->dirty = false;
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
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
                    oss << std::fixed << std::setprecision(6) << arg[i];
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
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

bool Config::isValidKey(const std::string& key) const {
    // Validate key format
    if (key.empty() || key.find(' ') != std::string::npos) {
        return false;
    }
    
    // Check for allowed characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
            return false;
        }
    }
    
    return true;
}

std::string Config::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Config::validateDefaultValue(const std::string& key, const ConfigValue& value) const {
    // Validate critical configuration parameters
    if (key == "neuron_count" || key == "simulation_timestep") {
        double num = 0.0;
        if (std::holds_alternative<double>(value)) {
            num = std::get<double>(value);
        } else if (std::holds_alternative<int>(value)) {
            num = static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            num = static_cast<double>(std::get<int64_t>(value));
        }
        
        if (num <= 0.0) {
            NLM_LOG_ERROR("Config: Invalid default value for key '" + key + "': " + std::to_string(num) + ". Must be positive.");
        }
    }
}

void Config::validateNumericRange(const std::string& key, double value) const {
    // Validate numeric ranges for critical parameters
    if (key == "connection_probability" && (value < 0.0 || value > 1.0)) {
        NLM_LOG_WARNING("Config: Connection probability out of range [0,1]: " + std::to_string(value));
    }
    
    if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || key == "stdp_tau") {
        if (value < 0.0) {
            NLM_LOG_WARNING("Config: " + key + " should be positive: " + std::to_string(value));
        }
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with better error handling
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Config: Invalid line in " + filepath + " (line " + std::to_string(lineNumber) + "): " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Parse value based on type
        if (value.empty()) {
            continue; // Skip empty values
        }
        
        // Try to parse as boolean
        if (value == "true" || value == "false") {
            set(key, value == "true", ConfigSource::File);
            continue;
        }
        
        // Try to parse as integer
        try {
            if (value.find('.') == std::string::npos) {
                size_t posEnd;
                long intVal = std::stol(value, &posEnd);
                if (posEnd == value.size()) {
                    set(key, intVal, ConfigSource::File);
                    continue;
                }
            }
            
            // Try to parse as double
            size_t posEnd;
            double doubleVal = std::stod(value, &posEnd);
            if (posEnd == value.size()) {
                set(key, doubleVal, ConfigSource::File);
                continue;
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Config: Failed to parse value for key '" + key + "' (line " + std::to_string(lineNumber) + "): " + e.what());
        }
        
        // Default to string
        set(key, value, ConfigSource::File);
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& filepath) {
    // TODO: Implement proper JSON parser
    NLM_LOG_ERROR("Config: JSON format not yet implemented - falling back to simple format");
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromYAML(const std::string& filepath) {
    // TODO: Implement proper YAML parser
    NLM_LOG_ERROR("Config: YAML format not yet implemented - falling back to simple format");
    return loadFromSimpleFormat(filepath);
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm