#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyIndex;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file type based on extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return loadFromString(filepath); // Will need to read JSON content
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromString(filepath); // Will need to read YAML content
    } else {
        // Default to simple key=value format for backward compatibility
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
}

bool Config::loadFromString(const std::string& content) {
    // Try JSON first
    try {
        auto json = nlohmann::json::parse(content);
        return parseJsonConfig(json);
    } catch (const nlohmann::json::parse_error&) {
        // Try YAML
        try {
            auto yaml = YAML::Load(content);
            return parseYamlConfig(yaml);
        } catch (const YAML::ParserException&) {
            // Fall back to simple key=value format
            std::istringstream stream(content);
            std::string line;
            while (std::getline(stream, line)) {
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
            return true;
        }
    }
}

bool Config::parseJsonConfig(const nlohmann::json& json) {
    clear();
    
    for (const auto& [key, value] : json.items()) {
        ConfigValue configValue;
        if (parseValue(value, configValue)) {
            std::string description = "Configuration parameter: " + key;
            set(key, configValue, ConfigSource::File, description);
        }
    }
    return true;
}

bool Config::parseYamlConfig(const YAML::Node& yaml) {
    clear();
    
    for (const auto& node : yaml) {
        std::string key = node.first.as<std::string>();
        ConfigValue configValue;
        if (parseValue(node.second, configValue)) {
            std::string description = "Configuration parameter: " + key;
            set(key, configValue, ConfigSource::File, description);
        }
    }
    return true;
}

bool Config::parseValue(const nlohmann::json& json, ConfigValue& value) {
    if (json.is_number_integer()) {
        value = json.get<int>();
        return true;
    } else if (json.is_number()) {
        value = json.get<double>();
        return true;
    } else if (json.is_boolean()) {
        value = json.get<bool>();
        return true;
    } else if (json.is_string()) {
        value = json.get<std::string>();
        return true;
    } else if (json.is_array()) {
        // Try to determine array type
        if (!json.empty()) {
            auto firstType = json[0].type();
            if (firstType == nlohmann::json::value_t::number_integer) {
                std::vector<int> arr;
                for (const auto& item : json) {
                    arr.push_back(item.get<int>());
                }
                value = arr;
            } else if (firstType == nlohmann::json::value_t::number) {
                std::vector<double> arr;
                for (const auto& item : json) {
                    arr.push_back(item.get<double>());
                }
                value = arr;
            } else if (firstType == nlohmann::json::value_t::string) {
                std::vector<std::string> arr;
                for (const auto& item : json) {
                    arr.push_back(item.get<std::string>());
                }
                value = arr;
            } else {
                return false;
            }
            return true;
        }
    }
    return false;
}

bool Config::parseValue(const YAML::Node& yaml, ConfigValue& value) {
    if (yaml.IsScalar()) {
        if (yaml.IsInteger()) {
            value = yaml.as<int>();
            return true;
        } else if (yaml.IsFloat()) {
            value = yaml.as<double>();
            return true;
        } else if (yaml.IsBoolean()) {
            value = yaml.as<bool>();
            return true;
        } else if (yaml.IsDouble()) {
            value = yaml.as<double>();
            return true;
        } else {
            value = yaml.as<std::string>();
            return true;
        }
    } else if (yaml.IsSequence()) {
        if (yaml.size() == 0) return false;
        
        auto firstType = yaml[0].Type();
        if (firstType == YAML::NodeType::Scalar) {
            std::string firstStr = yaml[0].as<std::string>();
            bool isInt = std::all_of(firstStr.begin(), firstStr.end(), ::isdigit);
            
            if (isInt) {
                std::vector<int> arr;
                for (const auto& item : yaml) {
                    arr.push_back(item.as<int>());
                }
                value = arr;
            } else {
                std::vector<std::string> arr;
                for (const auto& item : yaml) {
                    arr.push_back(item.as<std::string>());
                }
                value = arr;
            }
            return true;
        }
    }
    return false;
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return arg;
        } else {
            return "";
        }
    }, value);
}

YAML::Node Config::configValueToYaml(const ConfigValue& value) {
    YAML::Node node;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        }
    }, value);
    return node;
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
    
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) file << ",\n";
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            }
        }, entry.value);
    }
    
    file << "\n}\n";
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const ConfigEntry& entry = pImpl->entries[index];
    
    try {
        return std::get<T>(entry.value);
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
    if (!validateKey(key)) {
        throw std::invalid_argument("Invalid configuration key: " + key);
    }
    
    size_t index = 0;
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        index = it->second;
        pImpl->entries[index].value = value;
        pImpl->entries[index].source = source;
    } else {
        index = pImpl->entries.size();
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keyIndex[key] = index;
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
    return pImpl->keyIndex.find(key) != pImpl->keyIndex.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        size_t index = it->second;
        
        pImpl->entries.erase(pImpl->entries.begin() + index);
        
        pImpl->keyIndex.clear();
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            pImpl->keyIndex[pImpl->entries[i].key] = i;
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
    pImpl->keyIndex.clear();
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
        oss << "] (" << static_cast<int>(entry.source) << ") " << entry.description << "\n";
    }
    return oss.str();
}

template<typename T>
bool Config::getValidated(const std::string& key, T& value) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        value = val.value();
        return true;
    }
    return false;
}

template<typename T>
std::optional<T> Config::getNested(const std::string& path) const {
    std::vector<std::string> keys;
    std::string current;
    
    for (char c : path) {
        if (c == '.') {
            if (!current.empty()) {
                keys.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        keys.push_back(current);
    }
    
    if (keys.empty()) {
        return std::nullopt;
    }
    
    auto currentMap = get<std::unordered_map<std::string, ConfigValue>>(keys[0]);
    if (!currentMap.has_value()) {
        return std::nullopt;
    }
    
    for (size_t i = 1; i < keys.size(); ++i) {
        const auto& innerMap = currentMap.value();
        auto nextMap = innerMap.get<std::unordered_map<std::string, ConfigValue>>(keys[i]);
        if (!nextMap.has_value()) {
            return std::nullopt;
        }
        currentMap = nextMap;
    }
    
    return currentMap.get<T>("") ? currentMap.value()[""] : std::nullopt;
}

template<typename T>
void Config::setNested(const std::string& path, const T& value, ConfigSource source) {
    std::vector<std::string> keys;
    std::string current;
    
    for (char c : path) {
        if (c == '.') {
            if (!current.empty()) {
                keys.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        keys.push_back(current);
    }
    
    if (keys.empty()) {
        throw std::invalid_argument("Invalid nested path: " + path);
    }
    
    std::unordered_map<std::string, ConfigValue> currentMap;
    
    for (size_t i = 0; i < keys.size() - 1; ++i) {
        auto nestedMap = get<std::unordered_map<std::string, ConfigValue>>(keys[i]);
        if (!nestedMap.has_value()) {
            currentMap[keys[i]] = std::unordered_map<std::string, ConfigValue>();
        } else {
            currentMap = nestedMap.value();
        }
        set(keys[i], currentMap, source);
    }
    
    currentMap[keys.back()] = value;
    set(keys.back(), currentMap, source);
}

bool Config::validateKey(const std::string& key) const {
    static const std::regex keyPattern("^[a-zA-Z_][a-zA-Z0-9_]*$");
    return std::regex_match(key, keyPattern);
}

bool Config::validateValueType(const ConfigValue& value, const std::type_info& expectedType) const {
    if (std::holds_alternative<int>(value)) return expectedType == typeid(int);
    if (std::holds_alternative<int64_t>(value)) return expectedType == typeid(int64_t);
    if (std::holds_alternative<double>(value)) return expectedType == typeid(double);
    if (std::holds_alternative<bool>(value)) return expectedType == typeid(bool);
    if (std::holds_alternative<std::string>(value)) return expectedType == typeid(std::string);
    if (std::holds_alternative<std::vector<int>>(value)) return expectedType == typeid(std::vector<int>);
    if (std::holds_alternative<std::vector<double>>(value)) return expectedType == typeid(std::vector<double>);
    if (std::holds_alternative<std::vector<std::string>>(value)) return expectedType == typeid(std::vector<std::string>);
    return false;
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool Config::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
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