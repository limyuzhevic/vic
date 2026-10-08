#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <stdexcept>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    FileFormat format = getFileFormat(filepath);
    if (format == FileFormat::Unknown) {
        throw ConfigError(ConfigError::ErrorCode::ParseError, "Unknown file format: " + filepath);
    }
    
    return loadFromString(readFile(filepath), format);
}

bool Config::loadFromString(const std::string& content, FileFormat format) {
    clear();
    
    switch (format) {
        case FileFormat::JSON:
            return loadFromJSON(content);
        case FileFormat::YAML:
            return loadFromYAML(content);
        case FileFormat::KeyValue:
            return loadFromKeyValueString(content);
        default:
            throw ConfigError(ConfigError::ErrorCode::ParseError, "Unsupported file format");
    }
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        auto json = nlohmann::json::parse(jsonStr);
        return jsonToConfigValue(json, pImpl->entries);
    } catch (const nlohmann::json::parse_error& e) {
        throw ConfigError(ConfigError::ErrorCode::ParseError, "JSON parse error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::ErrorCode::ParseError, "JSON parse error: " + std::string(e.what()));
    }
}

bool Config::loadFromYAML(const std::string& yamlStr) {
    try {
        auto node = YAML::Load(yamlStr);
        return yamlToConfigValue(node, pImpl->entries);
    } catch (const YAML::Exception& e) {
        throw ConfigError(ConfigError::ErrorCode::ParseError, "YAML parse error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::ErrorCode::ParseError, "YAML parse error: " + std::string(e.what()));
    }
}

bool Config::loadFromKeyValueString(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    int lineNum = 0;
    
    while (std::getline(stream, line)) {
        lineNum++;
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
        } else {
            throw ConfigError(ConfigError::ErrorCode::ParseError, 
                            "Invalid key=value format on line " + std::to_string(lineNum) + ": " + line);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        auto json = configValueToJSON();
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigError(ConfigError::ErrorCode::FileNotFound, "Cannot open file for writing: " + filepath);
        }
        
        file << json.dump(2);
        return true;
    } catch (const ConfigError&) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::ErrorCode::ParseError, "Failed to save to file: " + std::string(e.what()));
    }
}

bool Config::saveToKeyValueFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError(ConfigError::ErrorCode::FileNotFound, "Cannot open file for writing: " + filepath);
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << configValueToString(entry.value) << "\n";
    }
    
    return true;
}

std::string Config::configValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

nlohmann::json Config::configValueToJSON(const ConfigValue& value) const {
    nlohmann::json json;
    std::visit([&json](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                             std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<ConfigValue>>) {
            json = arg;
        } else {
            json = arg;
        }
    }, value);
    return json;
}

nlohmann::json Config::configValueToJSON() const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        std::vector<std::string> parts;
        splitKey(entry.key, parts);
        
        nlohmann::json* current = &json;
        for (size_t i = 0; i < parts.size(); ++i) {
            if (i == parts.size() - 1) {
                (*current)[parts[i]] = configValueToJSON(entry.value);
            } else {
                if (!current->contains(parts[i])) {
                    (*current)[parts[i]] = nlohmann::json::object();
                }
                current = &((*current)[parts[i]]);
            }
        }
    }
    
    return json;
}

bool Config::jsonToConfigValue(const nlohmann::json& json, std::vector<ConfigEntry>& entries) {
    if (json.is_object()) {
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            std::string description = "";
            
            if (json.contains("_description") && it.value().is_object()) {
                description = json["_description"].get<std::string>();
                for (auto desc_it = json.begin(); desc_it != json.end(); ++desc_it) {
                    if (desc_it.key() != "_description") {
                        std::vector<std::string> parts;
                        splitKey(desc_it.key(), parts);
                        std::vector<std::string> new_parts = parts;
                        new_parts.insert(new_parts.begin(), key);
                        std::string new_key = joinKeys(new_parts);
                        if (!desc_it.value().is_null()) {
                            ConfigValue value;
                            if (jsonToConfigValue(desc_it.value(), value)) {
                                entries.emplace_back(new_key, value, ConfigSource::File, "");
                            }
                        }
                    }
                }
                if (key != "_description") {
                    continue;
                }
            }
            
            if (key == "_description") continue;
            
            ConfigValue value;
            if (jsonToConfigValue(it.value(), value)) {
                entries.emplace_back(key, value, ConfigSource::File, description);
            }
        }
    } else if (json.is_array()) {
        std::vector<ConfigValue> arrayValue;
        for (const auto& item : json) {
            ConfigValue itemValue;
            if (jsonToConfigValue(item, itemValue)) {
                arrayValue.push_back(itemValue);
            }
        }
        if (!arrayValue.empty()) {
            entries.emplace_back("_array", arrayValue, ConfigSource::File, "Array values");
        }
    } else {
        ConfigValue value;
        if (jsonToConfigValue(json, value)) {
            entries.emplace_back("_single", value, ConfigSource::File, "Single value");
        }
    }
    
    return !entries.empty();
}

bool Config::jsonToConfigValue(const nlohmann::json& json, ConfigValue& value) {
    try {
        if (json.is_string()) {
            value = json.get<std::string>();
            return true;
        } else if (json.is_boolean()) {
            value = json.get<bool>();
            return true;
        } else if (json.is_number_integer()) {
            value = json.get<int>();
            return true;
        } else if (json.is_number()) {
            value = json.get<double>();
            return true;
        } else if (json.is_array()) {
            std::vector<ConfigValue> arrayValue;
            for (const auto& item : json) {
                ConfigValue itemValue;
                if (jsonToConfigValue(item, itemValue)) {
                    arrayValue.push_back(itemValue);
                }
            }
            if (!arrayValue.empty()) {
                value = arrayValue;
                return true;
            }
        } else if (json.is_object()) {
            value = json.dump();
            return true;
        } else if (json.is_null()) {
            value = std::string("null");
            return true;
        }
    } catch (const std::exception&) {
        return false;
    }
    
    return false;
}

bool Config::yamlToConfigValue(const YAML::Node& node, std::vector<ConfigEntry>& entries) {
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            std::string key = it->first.as<std::string>();
            std::string description = "";
            
            if (node["_description"] && node["_description"].IsScalar()) {
                description = node["_description"].as<std::string>();
                for (auto map_it = node.begin(); map_it != node.end(); ++map_it) {
                    if (map_it->first.as<std::string>() != "_description") {
                        std::vector<std::string> parts;
                        splitKey(map_it->first.as<std::string>(), parts);
                        std::vector<std::string> new_parts = parts;
                        new_parts.insert(new_parts.begin(), key);
                        std::string new_key = joinKeys(new_parts);
                        if (!map_it->second.IsNull()) {
                            ConfigValue value;
                            if (yamlToConfigValue(map_it->second, value)) {
                                entries.emplace_back(new_key, value, ConfigSource::File, "");
                            }
                        }
                    }
                }
                if (key == "_description") continue;
            }
            
            if (key == "_description") continue;
            
            ConfigValue value;
            if (yamlToConfigValue(it->second, value)) {
                entries.emplace_back(key, value, ConfigSource::File, description);
            }
        }
    } else if (node.IsSequence()) {
        std::vector<ConfigValue> arrayValue;
        for (const auto& item : node) {
            ConfigValue itemValue;
            if (yamlToConfigValue(item, itemValue)) {
                arrayValue.push_back(itemValue);
            }
        }
        if (!arrayValue.empty()) {
            entries.emplace_back("_array", arrayValue, ConfigSource::File, "Array values");
        }
    } else {
        ConfigValue value;
        if (yamlToConfigValue(node, value)) {
            entries.emplace_back("_single", value, ConfigSource::File, "Single value");
        }
    }
    
    return !entries.empty();
}

bool Config::yamlToConfigValue(const YAML::Node& node, ConfigValue& value) {
    try {
        if (node.IsScalar()) {
            std::string str = node.as<std::string>();
            if (str == "true" || str == "false") {
                value = str == "true";
            } else if (std::regex_match(str, std::regex("^-?\d+$"))) {
                value = std::stoi(str);
            } else if (std::regex_match(str, std::regex("^-?\d+\.\d*$") || 
                                         std::regex_match(str, std::regex("^-?\d*\.\d+$")))) {
                value = std::stod(str);
            } else {
                value = str;
            }
            return true;
        } else if (node.IsMap()) {
            value = "{\"unknown\": true}";
            return true;
        } else if (node.IsSequence()) {
            std::vector<ConfigValue> arrayValue;
            for (const auto& item : node) {
                ConfigValue itemValue;
                if (yamlToConfigValue(item, itemValue)) {
                    arrayValue.push_back(itemValue);
                }
            }
            if (!arrayValue.empty()) {
                value = arrayValue;
                return true;
            }
        } else if (node.IsNull()) {
            value = std::string("null");
            return true;
        }
    } catch (const std::exception&) {
        return false;
    }
    
    return false;
}

bool Config::stringToConfigValue(const std::string& str, ConfigValue& value) {
    if (str.empty()) return false;
    
    if (str == "true" || str == "false") {
        value = (str == "true");
        return true;
    }
    
    try {
        if (str.find('.') == std::string::npos) {
            value = std::stoi(str);
            return true;
        }
    } catch (...) {}
    
    try {
        value = std::stod(str);
        return true;
    } catch (...) {}
    
    value = str;
    return true;
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

void Config::splitKey(const std::string& key, std::vector<std::string>& parts) {
    parts.clear();
    std::string current;
    
    for (char c : key) {
        if (c == '.' || c == '/' || c == '\\') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
        } else {
            current.push_back(c);
        }
    }
    
    if (!current.empty()) {
        parts.push_back(current);
    }
}

std::string Config::joinKeys(const std::vector<std::string>& parts) {
    if (parts.empty()) return "";
    
    std::string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result += "." + parts[i];
    }
    return result;
}

bool Config::isFileJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string firstLine;
    std::getline(file, firstLine);
    firstLine = trim(firstLine);
    
    return (firstLine.empty() || firstLine[0] == '{' || firstLine[0] == '[' || 
            firstLine.find("{") != std::string::npos || firstLine.find("[") != std::string::npos);
}

bool Config::isFileYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string firstLine;
    std::getline(file, firstLine);
    firstLine = trim(firstLine);
    
    return (!firstLine.empty() && 
            (firstLine.find(":") != std::string::npos || 
             firstLine.find("-") != std::string::npos ||
             (firstLine[0] == ' ' && firstLine[0] != ' ')));
}

FileFormat Config::getFileFormat(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError(ConfigError::ErrorCode::FileNotFound, "Cannot open file: " + filepath);
    }
    
    std::string firstLine;
    std::getline(file, firstLine);
    firstLine = trim(firstLine);
    
    if (firstLine.empty()) {
        return FileFormat::KeyValue;
    }
    
    if (firstLine[0] == '{' || firstLine[0] == '[') {
        return FileFormat::JSON;
    }
    
    if (firstLine.find("{") != std::string::npos || firstLine.find("[") != std::string::npos) {
        return FileFormat::JSON;
    }
    
    if (firstLine.find(":") != std::string::npos || 
        (firstLine[0] == '-' && firstLine.size() > 1)) {
        return FileFormat::YAML;
    }
    
    return FileFormat::KeyValue;
}

std::string Config::readFile(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError(ConfigError::ErrorCode::FileNotFound, "Cannot open file: " + filepath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Template instantiations

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

ValidationResult Config::validate() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                auto intVal = std::get<T>(entry.value);
                if (intVal < INT_MIN / 10 || intVal > INT_MAX / 10) {
                    result.valid = false;
                    result.errorMessage = "Integer value out of range for key: " + entry.key;
                }
            } else if constexpr (std::is_same_v<T, double>) {
                auto doubleVal = std::get<T>(entry.value);
                if (std::isinf(doubleVal) || std::isnan(doubleVal)) {
                    result.valid = false;
                    result.errorMessage = "Invalid numeric value (NaN or infinity) for key: " + entry.key;
                }
            }
        }, entry.value);
    }
    
    return result;
}

// Explicit template instantiations

// Integer types
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// Default values
template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
