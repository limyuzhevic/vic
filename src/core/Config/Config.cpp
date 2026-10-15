#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cassert>
#include <unordered_map>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Cache for faster lookups
    mutable std::unordered_map<std::string, size_t> keyToIndex;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Cannot open config file: " + filepath);
        return false;
    }
    
    try {
        // Parse JSON
        json configJson;
        file >> configJson;
        
        pImpl->entries.clear();
        pImpl->keyToIndex.clear();
        
        // Process entries from JSON
        if (configJson.contains("config_entries")) {
            for (const auto& entry : configJson["config_entries"]) {
                std::string key = entry.value("key", "");
                if (key.empty()) continue;
                
                // Parse value based on type
                ConfigValue value;
                if (entry.contains("int_value")) {
                    value = entry["int_value"].get<int>();
                } else if (entry.contains("int64_value")) {
                    value = entry["int64_value"].get<int64_t>();
                } else if (entry.contains("double_value")) {
                    value = entry["double_value"].get<double>();
                } else if (entry.contains("bool_value")) {
                    value = entry["bool_value"].get<bool>();
                } else if (entry.contains("string_value")) {
                    value = entry["string_value"].get<std::string>();
                } else if (entry.contains("vector_int_value")) {
                    value = entry["vector_int_value"].get<std::vector<int>>();
                } else if (entry.contains("vector_double_value")) {
                    value = entry["vector_double_value"].get<std::vector<double>>();
                } else if (entry.contains("vector_string_value")) {
                    value = entry["vector_string_value"].get<std::vector<std::string>>();
                } else if (entry.contains("string")) {
                    // Backward compatibility with simple string values
                    value = entry["string"].get<std::string>();
                }
                
                std::string description = entry.value("description", "");
                ConfigSource source = ConfigSource::File;
                
                set(key, value, source, description);
            }
        } else {
            // Fallback to simple key=value format for backward compatibility
            NLM_LOG_INFO("Using backward-compatible config format for: " + filepath);
            return Config::loadFromFileSimple(filepath);
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath + " (" + 
                    std::to_string(pImpl->entries.size()) + " entries)");
        return true;
        
    } catch (const json::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in config file " + filepath + ": " + std::string(e.what()));
        // Fallback to simple format
        return Config::loadFromFileSimple(filepath);
    }
}

bool Config::loadFromFileSimple(const std::string& filepath) {
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
            std::string valueStr = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            set(key, valueStr, ConfigSource::File, "");
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
        NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
        return false;
    }
    
    // Write JSON with metadata
    json configJson;
    configJson["version"] = "2.0";
    configJson["format"] = "json";
    
    // Create entries array
    json entriesJson = json::array();
    for (const auto& entry : pImpl->entries) {
        json entryJson;
        entryJson["key"] = entry.key;
        entryJson["source"] = static_cast<int>(entry.source);
        entryJson["description"] = entry.description;
        
        // Store value based on type
        std::visit([&entryJson](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                entryJson["int_value"] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                entryJson["int64_value"] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                entryJson["double_value"] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                entryJson["bool_value"] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                entryJson["string_value"] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                entryJson["vector_int_value"] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                entryJson["vector_double_value"] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                entryJson["vector_string_value"] = arg;
            }
        }, entry.value);
        
        entriesJson.push_back(entryJson);
    }
    
    configJson["config_entries"] = entriesJson;
    
    // Write pretty JSON
    file << configJson.dump(2);
    
    NLM_LOG_INFO("Saved configuration to JSON file: " + filepath + " (" + 
                std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Check cache first
    auto itCache = pImpl->keyToIndex.find(key);
    if (itCache != pImpl->keyToIndex.end()) {
        size_t index = itCache->second;
        if (index < pImpl->entries.size()) {
            const auto& entry = pImpl->entries[index];
            try {
                return std::get<T>(entry.value);
            } catch (const std::bad_variant_access&) {
                return std::nullopt;
            }
        }
    }
    
    // Linear search as fallback
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
    set(key, value, source, "");
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    // Update cache
    auto itCache = pImpl->keyToIndex.find(key);
    if (itCache != pImpl->keyToIndex.end()) {
        size_t index = itCache->second;
        if (index < pImpl->entries.size()) {
            pImpl->entries[index] = ConfigEntry(key, value, source, description);
            return;
        }
    }
    
    // Add new entry
    pImpl->entries.emplace_back(key, value, source, description);
    pImpl->keyToIndex[key] = pImpl->entries.size() - 1;
}

bool Config::has(const std::string& key) const {
    return pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
}

void Config::remove(const std::string& key) {
    auto itCache = pImpl->keyToIndex.find(key);
    if (itCache == pImpl->keyToIndex.end()) {
        return;
    }
    
    size_t index = itCache->second;
    pImpl->entries.erase(pImpl->entries.begin() + index);
    pImpl->keyToIndex.erase(itCache);
    
    // Update cache indices for remaining entries
    for (auto& pair : pImpl->keyToIndex) {
        if (pair.second > index) {
            pair.second--;
        }
    }
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
        oss << "] (" << static_cast<int>(entry.source) << ") - " << entry.description << "\n";
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
// (These would need to be included in the header file for proper compilation)

} // namespace nlm
