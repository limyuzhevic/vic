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

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // For JSON/TOML file support
    json jsonCache; // Cached JSON representation for saving
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with empty JSON cache
    pImpl->jsonCache = json::object();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::fileExists(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

std::string Config::getFileExtension(const std::string& filename) {
    size_t pos = filename.rfind('.');
    if (pos != std::string::npos) {
        return filename.substr(pos + 1);
    }
    return "";
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    if (!fileExists(filepath)) {
        NLM_LOG_WARNING("JSON config file not found: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON config file: " + filepath);
        return false;
    }
    
    try {
        // Clear existing entries
        pImpl->entries.clear();
        pImpl->jsonCache = json::parse(file);
        
        // Parse JSON into Config entries
        if (pImpl->jsonCache.is_object()) {
            for (const auto& [key, value] : pImpl->jsonCache.items()) {
                ConfigValue configValue = jsonToConfigValue(value);
                // Try to infer description from key
                std::string description = "Configuration parameter: " + key;
                set(key, configValue, ConfigSource::File);
            }
        } else {
            NLM_LOG_ERROR("Invalid JSON format in config file: " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in config file " + filepath + ": " + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading JSON config file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromTomlFile(const std::string& filepath) {
    // Simplified TOML parsing - in production, use a proper TOML library
    if (!fileExists(filepath)) {
        NLM_LOG_WARNING("TOML config file not found: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open TOML config file: " + filepath);
        return false;
    }
    
    // For now, fall back to simple text parsing
    // TODO: Implement proper TOML parsing using toml11 or similar
    return loadFromFile(filepath);
}

bool Config::saveToJsonFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON config file for writing: " + filepath);
        return false;
    }
    
    try {
        // Build JSON from current entries
        json j = json::object();
        for (const auto& entry : pImpl->entries) {
            j[entry.key] = configValueToJson(entry.value);
        }
        
        // Write with pretty formatting
        file << j.dump(2);
        
        NLM_LOG_INFO("Saved JSON configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving JSON config file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::saveToTextFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open text config file for writing: " + filepath);
        return false;
    }
    
    try {
        file << "# NLM Configuration File\n";
        file << "# Generated: " << __DATE__ << " " << __TIME__ << "\n";
        file << "\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            // Format value based on type
            std::visit([&file](auto&& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    // Escape quotes in string values
                    std::string escaped = val;
                    size_t pos = 0;
                    while ((pos = escaped.find('"', pos)) != std::string::npos) {
                        escaped.replace(pos, 1, "\\\"");
                        pos += 2;
                    }
                    file << entry.key << " = \"" << escaped << "\"\n";
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                     std::is_same_v<T, std::vector<double>> ||
                                     std::is_same_v<T, std::vector<std::string>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) file << ", ";
                        if constexpr (std::is_same_v<T, std::vector<std::string>) {
                            // Escape quotes in string elements
                            std::string elem = val[i];
                            size_t pos = 0;
                            while ((pos = elem.find('"', pos)) != std::string::npos) {
                                elem.replace(pos, 1, "\\\"");
                                pos += 2;
                            }
                            file << "\"" << elem << "\"";
                        } else {
                            file << val[i];
                        }
                    }
                    file << "]\n";
                } else {
                    // For scalar types
                    if constexpr (std::is_same_v<T, bool>) {
                        file << entry.key << " = " << (val ? "true" : "false") << "\n";
                    } else {
                        file << entry.key << " = " << val << "\n";
                    }
                }
            }, entry.value);
            file << "\n";
        }
        
        NLM_LOG_INFO("Saved text configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving text config file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromJsonString(const std::string& jsonString) {
    try {
        // Clear existing entries
        pImpl->entries.clear();
        pImpl->jsonCache = json::parse(jsonString);
        
        // Parse JSON into Config entries
        if (pImpl->jsonCache.is_object()) {
            for (const auto& [key, value] : pImpl->jsonCache.items()) {
                ConfigValue configValue = jsonToConfigValue(value);
                set(key, configValue, ConfigSource::File);
            }
        } else {
            NLM_LOG_ERROR("Invalid JSON string format");
            return false;
        }
        
        return true;
        
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading JSON string: " + std::string(e.what()));
        return false;
    }
}

std::string Config::toJsonString() const {
    try {
        json j = json::object();
        for (const auto& entry : pImpl->entries) {
            j[entry.key] = configValueToJson(entry.value);
        }
        return j.dump(2);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error converting to JSON string: " + std::string(e.what()));
        return "{}";
    }
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Don't overwrite existing entries unless the other has higher priority
        bool exists = has(entry.key);
        if (!exists) {
            pImpl->entries.push_back(entry);
        } else {
            // Check if other entry has higher source priority
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                 [&entry](const ConfigEntry& e) { return e.key == entry.key; });n            if (it != pImpl->entries.end() && entry.source > it->source) {
                it->value = entry.value;
                it->source = entry.source;
            }
        }
    }
}

std::string Config::getStatistics() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    
    // Count entries by source
    std::map<ConfigSource, int> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    oss << "  By source:\n";
    for (const auto& [source, count] : sourceCount) {
        oss << "    " << static_cast<int>(source) << ": " << count << "\n";
    }
    
    oss << "  Memory usage: ~" << pImpl->entries.size() * 64 << " bytes\n";
    
    return oss.str();
}

bool Config::loadFromFile(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    if (ext == "json") {
        return loadFromJsonFile(filepath);
    } else if (ext == "toml") {
        return loadFromTomlFile(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Config file not found (using default): " + filepath);
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from: " + filepath);
        return true;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.find("--config") == 0) {
            continue;  // Handled in main.cpp
        }
        
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
    std::string ext = getFileExtension(filepath);
    if (ext == "json") {
        return saveToJsonFile(filepath);
    } else if (ext == "toml") {
        NLM_LOG_ERROR("TOML save not yet implemented - use .txt or .json extension");
        return false;
    } else {
        return saveToTextFile(filepath);
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        // Find description based on key (common configuration keys)
        std::string description = "Configuration parameter: " + key;
        
        // Add common descriptions for known keys
        if (key == "neuron_count") {
            description = "Number of neurons in the brain";
        } else if (key == "simulation_timestep") {
            description = "Time step for simulation in seconds";
        } else if (key == "connection_probability") {
            description = "Probability of connection between neurons";
        } else if (key == "stdp_ltp_weight") {
            description = "STDP Long-Term Potentiation weight";
        } else if (key == "stdp_ltd_weight") {
            description = "STDP Long-Term Depression weight";
        } else if (key == "stdp_tau") {
            description = "STDP time constant in milliseconds";
        }
        
        pImpl->entries.emplace_back(key, value, source, description);
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
    pImpl->jsonCache = json::object();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "JSON format available: " << (pImpl->jsonCache.is_null() ? "No" : "Yes") << "\n";
    oss << "\nKey entries:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << val[i];
                }
                oss << "]";
            } else {
                oss << val;
            }
        }, entry.value);
        oss << " (Source: " << static_cast<int>(entry.source) << ")\n";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
