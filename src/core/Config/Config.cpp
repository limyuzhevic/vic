#include "Config.hpp"
#include "JsonParser.hpp"
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
    // Try JSON format first (files ending with .json)
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return loadFromJson(filepath);
    }
    
    // Try to parse as JSON by checking if it starts with {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    char firstChar;
    file >> firstChar;
    file.seekg(0); // Reset to beginning
    
    if (firstChar == '{') {
        return loadFromJson(filepath);
    }
    
    // Fall back to simple key=value format for backward compatibility
    return loadFromKeyValue(filepath);
}

bool Config::loadFromJson(const std::string& filepath) {
    std::map<std::string, JsonParser::JsonValue> jsonData;
    if (!JsonParser::parseFile(filepath, jsonData)) {
        return false;
    }
    
    clear(); // Clear existing entries
    
    for (const auto& pair : jsonData) {
        // Convert JsonValue to ConfigValue
        ConfigValue configValue = convertJsonToConfig(pair.second);
        set(pair.first, configValue, ConfigSource::File);
        // Note: JSON doesn't have descriptions, so we use empty string
    }
    
    return true;
}

bool Config::loadFromKeyValue(const std::string& filepath) {
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

ConfigValue Config::convertJsonToConfig(const JsonParser::JsonValue& jsonValue) {
    return std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return ConfigValue(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return ConfigValue(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return ConfigValue(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return ConfigValue(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return ConfigValue(arg);
        } else if constexpr (std::is_same_v<T, std::vector<JsonParser::JsonValue>>) {
            // Convert JSON array to vector
            std::vector<int> intVec;
            std::vector<double> doubleVec;
            std::vector<std::string> stringVec;
            
            for (const auto& item : arg) {
                std::visit([&](auto&& itemArg) {
                    using ItemT = std::decay_t<decltype(itemArg)>;
                    if constexpr (std::is_same_v<ItemT, int>) {
                        intVec.push_back(itemArg);
                    } else if constexpr (std::is_same_v<ItemT, int64_t>) {
                        intVec.push_back(static_cast<int>(itemArg));
                    } else if constexpr (std::is_same_v<ItemT, double>) {
                        doubleVec.push_back(itemArg);
                    } else if constexpr (std::is_same_v<ItemT, bool>) {
                        intVec.push_back(itemArg ? 1 : 0);
                    } else if constexpr (std::is_same_v<ItemT, std::string>) {
                        stringVec.push_back(itemArg);
                    }
                }, item.value);
            }
            
            // Prefer the most specific type
            if (!stringVec.empty()) {
                return ConfigValue(stringVec);
            } else if (!doubleVec.empty()) {
                return ConfigValue(doubleVec);
            } else {
                return ConfigValue(intVec);
            }
        }
        return ConfigValue(); // Fallback
    }, jsonValue.value);
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
        
        // Convert the variant value to string representation
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Quote string values
                valueStr = "\"" + arg + "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStr = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                // Serialize vector values
                valueStr = "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) valueStr += ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        valueStr += "\"" + std::to_string(item) + "\"";
                    } else {
                        valueStr += std::to_string(item);
                    }
                    first = false;
                }
                valueStr += "]";
            } else {
                // For basic types (int, int64_t, double, etc.)
                valueStr = std::to_string(arg);
            }
        }, entry.value);
        
        file << entry.key << " = " << valueStr << "\n\n";
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
