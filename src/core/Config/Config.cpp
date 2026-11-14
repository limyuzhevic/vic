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

#include "nlohmann/json.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool useJsonFormat = false;  // Whether to use JSON format for save/load
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect file format based on extension
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json" || extension == ".cfg.json") {
        pImpl->useJsonFormat = true;
        return loadFromJsonFile(filepath);
    } else if (extension == ".yaml" || extension == ".yml" || extension == ".cfg.yaml") {
        // TODO: Add YAML support when yaml-cpp is available
        return false;  // YAML not yet implemented
    } else {
        // Try JSON format first (for backward compatibility)
        if (loadFromJsonFile(filepath)) {
            pImpl->useJsonFormat = true;
            return true;
        }
        // Fall back to key=value format
        file.clear();
        file.seekg(0, std::ios::beg);
        return loadFromKeyValueFile(filepath);
    }
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse JSON array or object
        if (jsonData.is_array()) {
            // Handle JSON array format
            for (const auto& item : jsonData) {
                if (item.is_object() && item.contains("key") && item.contains("value")) {
                    std::string key = item["key"];
                    ConfigValue value = parseJsonValue(item["value"]);
                    std::string description = item.value("description", "");
                    ConfigSource source = ConfigSource::File;
                    
                    pImpl->entries.emplace_back(key, value, source, description);
                }
            }
        } else if (jsonData.is_object()) {
            // Handle JSON object format (key-value pairs)
            for (const auto& [key, value] : jsonData.items()) {
                ConfigValue configValue = parseJsonValue(value);
                pImpl->entries.emplace_back(key, configValue, ConfigSource::File, "");
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse JSON config file: ") + e.what());
        return false;
    }
}

bool Config::loadFromKeyValueFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Clear existing entries
    pImpl->entries.clear();
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
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
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING(std::string("Warning: Failed to parse key='") + key + std::string("' on line ") + 
                              std::to_string(lineNumber) + std::string(": ") + e.what());
            }
        } else {
            NLM_LOG_WARNING(std::string("Warning: Invalid line ") + std::to_string(lineNumber) + 
                          std::string(": ") + line);
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
    
    if (pImpl->useJsonFormat) {
        return saveToJsonFile(filepath);
    } else {
        return saveToKeyValueFile(filepath);
    }
}

bool Config::saveToJsonFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        
        if (pImpl->useJsonFormat) {
            // Create JSON object format
            nlohmann::json configObject = nlohmann::json::object();
            for (const auto& entry : pImpl->entries) {
                configObject[entry.key] = serializeConfigValue(entry.value);
            }
            file << configObject.dump(4);  // Pretty print with 4-space indentation
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to save JSON config file: ") + e.what());
        return false;
    }
}

bool Config::saveToKeyValueFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << serializeConfigValue(entry.value) << "\n\n";
    }
    
    return true;
}

ConfigValue Config::parseJsonValue(const nlohmann::json& jsonValue) {
    if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_number_integer()) {
        return jsonValue.get<int64_t>();
    } else if (jsonValue.is_number_float()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_array()) {
        if (!jsonValue.empty()) {
            // Determine array element type
            if (jsonValue[0].is_string()) {
                std::vector<std::string> vec;
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<std::string>());
                }
                return vec;
            } else if (jsonValue[0].is_number_integer()) {
                std::vector<int> vec;
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<int>());
                }
                return vec;
            } else if (jsonValue[0].is_number_float()) {
                std::vector<double> vec;
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<double>());
                }
                return vec;
            }
        }
    }
    
    // Default to empty string for unsupported types
    return std::string("");
}

nlohmann::json Config::serializeConfigValue(const ConfigValue& value) {
    return std::visit([](auto&& v) -> nlohmann::json {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            return static_cast<int64_t>(v);
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            return static_cast<double>(v);
        } else if constexpr (std::is_same_v<T, bool>) {
            return v;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return nlohmann::json(v);
        } else {
            return nlohmann::json();
        }
    }, value);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, "");
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    try {
        // Check if it's a boolean
        if (value == "true" || value == "false") {
            set(key, value == "true", source);
            return;
        }
        
        // Check if it's an integer
        size_t pos;
        int intValue;
        try {
            intValue = std::stoi(value, &pos);
            if (pos == value.size()) {
                set(key, intValue, source);
                return;
            }
        } catch (...) {}
        
        // Check if it's a floating point number
        int64_t int64Value;
        try {
            int64Value = std::stoll(value, &pos);
            if (pos == value.size()) {
                set(key, int64Value, source);
                return;
            }
        } catch (...) {}
        
        double doubleValue;
        try {
            doubleValue = std::stod(value, &pos);
            if (pos == value.size()) {
                set(key, doubleValue, source);
                return;
            }
        } catch (...) {}
        
        // Check if it's a comma-separated list
        if (value.find(',') != std::string::npos) {
            std::vector<std::string> list;
            std::stringstream ss(value);
            std::string item;
            while (std::getline(ss, item, ',')) {
                trim(item);
                if (!item.empty()) {
                    list.push_back(item);
                }
            }
            set(key, list, source);
            return;
        }
        
        // Default to string
        set(key, ConfigValue(value), source);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to set config key '") + key + std::string("': ") + e.what());
        throw std::runtime_error(std::string("Failed to set config key '") + key + std::string("': ") + e.what());
    }
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, static_cast<int64_t>(value), source);
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

// Template implementations
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
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


