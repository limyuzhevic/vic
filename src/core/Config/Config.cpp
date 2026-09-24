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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Check file extension for proper handling
    bool isJsonFormat = false;
    if (filepath.size() >= 5) {
        std::string ext = filepath.substr(filepath.size() - 5);
        isJsonFormat = (ext == ".json" || ext == ".JSON");
    }
    
    if (isJsonFormat) {
        // TODO: Implement JSON parsing
        NLM_LOG_WARNING("JSON format not yet implemented, using key=value parser for: " + filepath);
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string valueStr = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Try to parse value as different types in order
            bool parsed = false;
            ConfigValue parsedValue;
            
            // Try boolean first
            if (valueStr == "true" || valueStr == "false") {
                bool val = (valueStr == "true");
                parsedValue = val;
                parsed = true;
            }
            // Try integer
            else {
                try {
                    size_t posEnd;
                    long long intVal = std::stoll(valueStr, &posEnd);
                    if (posEnd == valueStr.size()) {
                        parsedValue = intVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            if (!parsed) {
                // Try floating point
                try {
                    size_t posEnd;
                    double doubleVal = std::stod(valueStr, &posEnd);
                    if (posEnd == valueStr.size()) {
                        parsedValue = doubleVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            if (!parsed) {
                // Try string (handle quoted strings already stripped)
                parsedValue = valueStr;
                parsed = true;
            }
            
            if (parsed) {
                set(key, parsedValue, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Failed to parse value at " + filepath + ":" + std::to_string(lineNumber) + 
                              " for key " + key + ": " + valueStr);
            }
        } else {
            NLM_LOG_WARNING("Invalid format at " + filepath + ":" + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath);
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_ERROR("Invalid command line argument format: " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else if (arg[0] == '-') {
            NLM_LOG_ERROR("Missing value for command line argument: " + arg);
            success = false;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save config to file: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated from runtime configuration\n\n";
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Format value appropriately based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << arg << "\n";
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
                    file << arg[i];
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
                file << entry.key << " = " << "UNKNOWN_TYPE\n";
            }
        }, entry.value);
        file << "\n";
    }
    
    NLM_LOG_INFO("Saved configuration to: " + filepath);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot get value for empty key");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_WARNING("Config key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Config type mismatch for key: " + key + ". Expected " + typeid(T).name());
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        NLM_LOG_WARNING("Config key " + key + " not found, using default: " + std::to_string(defaultValue));
    }
    return val.value_or(defaultValue);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config with empty key");
        return;
    }
    
    // Trim whitespace from key
    std::string trimmedKey = trim(key);
    
    // Check for valid key format (alphanumeric and underscores)
    if (!std::all_of(trimmedKey.begin(), trimmedKey.end(), 
                    [](char c) { return std::isalnum(c) || c == '_'; })) {
        NLM_LOG_WARNING("Config key may contain non-alphanumeric characters: " + trimmedKey);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&trimmedKey](const ConfigEntry& e) { return e.key == trimmedKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(trimmedKey, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config with empty key");
        return;
    }
    
    std::string trimmedKey = trim(key);
    set(trimmedKey, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config with empty key");
        return;
    }
    
    std::string trimmedKey = trim(key);
    
    // Validate integer value (e.g., neuron_count should be positive)
    if (trimmedKey == "neuron_count" || trimmedKey == "region_count" || 
        trimmedKey == "max_simulation_steps") {
        if (value <= 0) {
            NLM_LOG_WARNING("Config " + trimmedKey + " should be positive, got: " + std::to_string(value));
        }
    }
    
    set(trimmedKey, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config with empty key");
        return;
    }
    
    std::string trimmedKey = trim(key);
    
    // Validate double value (e.g., simulation_timestep should be positive)
    if (trimmedKey == "simulation_timestep" || trimmedKey == "stdp_ltp_weight" || 
        trimmedKey == "stdp_ltd_weight" || trimmedKey == "stdp_time_constant" ||
        trimmedKey == "development_synaptogenesis_rate" || trimmedKey == "development_pruning_rate" ||
        trimmedKey == "reward_discount_factor" || trimmedKey == "visualization_update_rate") {
        if (value < 0.0) {
            NLM_LOG_WARNING("Config " + trimmedKey + " should not be negative, got: " + std::to_string(value));
        }
    }
    
    set(trimmedKey, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config with empty key");
        return;
    }
    
    std::string trimmedKey = trim(key);
    set(trimmedKey, ConfigValue(value), source);
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
    
    if (pImpl->entries.empty()) {
        oss << "  (empty)\n";
        return oss.str();
    }
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        try {
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
        } catch (const std::exception& e) {
            oss << "ERROR: " << e.what();
        }
        oss << "] (source: " << static_cast<int>(entry.source) << ")\n";
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
