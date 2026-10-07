#include "Config.hpp"
#include "Utils/JSONParser.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <memory>
#include <limits>
#include <numeric>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first (new format)
    auto jsonValue = JSONParser::parseFile(filepath);
    if (jsonValue.has_value()) {
        return parseJSON(*jsonValue);
    }
    
    // Fall back to simple key=value format (legacy support)
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

bool Config::parseJSON(const JSONValue& jsonValue) {
    // Clear existing entries
    clear();
    
    // Check if it's an object
    if (!std::holds_alternative<std::map<std::string, JSONValue>>(jsonValue)) {
        return false;
    }
    
    const auto& obj = std::get<std::map<std::string, JSONValue>>(jsonValue);
    
    for (const auto& pair : obj) {
        const std::string& key = pair.first;
        const JSONValue& value = pair.second;
        
        // Convert JSON value to ConfigValue
        ConfigValue configValue;
        if (std::holds_alternative<std::monostate>(value)) {
            // null -> string "null"
            configValue = "null";
        } else if (std::holds_alternative<bool>(value)) {
            configValue = std::get<bool>(value);
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            // Check if it's an integer
            if (val == static_cast<int64_t>(val) && val >= INT64_MIN && val <= INT64_MAX) {
                configValue = static_cast<int64_t>(val);
            } else if (val >= std::numeric_limits<float>::min() && val <= std::numeric_limits<float>::max()) {
                configValue = static_cast<float>(val);
            } else {
                configValue = val;
            }
        } else if (std::holds_alternative<int64_t>(value)) {
            configValue = std::get<int64_t>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            configValue = std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<JSONValue>>(value)) {
            // Arrays are not directly supported, convert to string
            configValue = "[array]";
        } else if (std::holds_alternative<std::map<std::string, JSONValue>>(value)) {
            // Objects are not directly supported, convert to string
            configValue = "[object]";
        }
        
        set(key, configValue, ConfigSource::File);
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Create a JSON object for saving
    std::map<std::string, JSONValue> jsonObj;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to JSONValue
        JSONValue jsonValue;
        if (std::holds_alternative<int>(entry.value)) {
            jsonValue = std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            jsonValue = std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            jsonValue = std::get<double>(entry.value);
        } else if (std::holds_alternative<float>(entry.value)) {
            jsonValue = static_cast<double>(std::get<float>(entry.value));
        } else if (std::holds_alternative<bool>(entry.value)) {
            jsonValue = std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            jsonValue = std::get<std::string>(entry.value);
        } else {
            // For unsupported types, use string representation
            jsonValue = "[" + std::to_string(static_cast<int>(entry.value.index())) + "]";
        }
        
        jsonObj[entry.key] = std::move(jsonValue);
    }
    
    std::string jsonString = JSONParser::serialize(jsonObj, 0);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << jsonString;
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
// JSON parsing support
bool Config::parseJSON(const JSONValue& jsonValue) {
    // Clear existing entries
    clear();
    
    // Check if it's an object
    if (!std::holds_alternative<std::map<std::string, JSONValue>>(jsonValue)) {
        return false;
    }
    
    const auto& obj = std::get<std::map<std::string, JSONValue>>(jsonValue);
    
    for (const auto& pair : obj) {
        const std::string& key = pair.first;
        const JSONValue& value = pair.second;
        
        // Convert JSON value to ConfigValue
        ConfigValue configValue;
        if (std::holds_alternative<std::monostate>(value)) {
            // null -> string "null"
            configValue = "null";
        } else if (std::holds_alternative<bool>(value)) {
            configValue = std::get<bool>(value);
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            // Check if it's an integer
            if (val == static_cast<int64_t>(val) && val >= INT64_MIN && val <= INT64_MAX) {
                configValue = static_cast<int64_t>(val);
            } else if (val >= std::numeric_limits<float>::min() && val <= std::numeric_limits<float>::max()) {
                configValue = static_cast<float>(val);
            } else {
                configValue = val;
            }
        } else if (std::holds_alternative<int64_t>(value)) {
            configValue = std::get<int64_t>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            configValue = std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<JSONValue>>(value)) {
            // Arrays are not directly supported, convert to string
            configValue = "[array]";
        } else if (std::holds_alternative<std::map<std::string, JSONValue>>(value)) {
            // Objects are not directly supported, convert to string
            configValue = "[object]";
        }
        
        set(key, configValue, ConfigSource::File);
    }
    
    return true;
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
