#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    nlohmann::json schema;
    std::vector<std::string> validationErrors;
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
    
    // Try to parse as JSON first
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    // Trim whitespace
    content.erase(0, content.find_first_not_of(" \t\n\r"));
    content.erase(content.find_last_not_of(" \t\n\r") + 1);
    
    if (!content.empty() && content.front() == '{') {
        return loadFromJSON(content);
    } else if (!content.empty() && (content.find("---") == 0 || content.find("yaml:") == 0)) {
        return loadFromYAML(content);
    }
    
    // Fallback to simple key=value format
    std::ifstream file2(filepath);
    if (!file2.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file2, line)) {
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
    file2.close();
    
    return true;
}

bool Config::loadFromJSON(const std::string& jsonString) {
    try {
        auto json = nlohmann::json::parse(jsonString);
        parseJSONToConfig(json, pImpl->entries);
        substituteEnvironmentVariables();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlString) {
    try {
        auto yaml = YAML::Load(yamlString);
        parseYAMLToConfig(yaml, pImpl->entries);
        substituteEnvironmentVariables();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "YAML parsing error: " << e.what() << std::endl;
        return false;
    }
}

void Config::parseJSONToConfig(const nlohmann::json& json, std::vector<ConfigEntry>& entries) {
    entries.clear();
    
    if (!json.is_object()) {
        return;
    }
    
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = normalizeKey(it.key());
        ConfigValue value;
        std::string description = "";
        
        // Extract value based on JSON type
        if (it->is_string()) {
            value = it->get<std::string>();
        } else if (it->is_number_integer()) {
            value = it->get<int>();
        } else if (it->is_number_unsigned()) {
            value = static_cast<int64_t>(it->get<uint64_t>());
        } else if (it->is_number_float()) {
            value = it->get<double>();
        } else if (it->is_boolean()) {
            value = it->get<bool>();
        } else if (it->is_array()) {
            // Handle arrays
            if (!it->empty()) {
                if (it->front().is_string()) {
                    std::vector<std::string> strArray;
                    for (const auto& item : *it) {
                        strArray.push_back(item.get<std::string>());
                    }
                    value = strArray;
                } else if (it->front().is_number_integer()) {
                    std::vector<int> intArray;
                    for (const auto& item : *it) {
                        intArray.push_back(item.get<int>());
                    }
                    value = intArray;
                } else if (it->front().is_number_float()) {
                    std::vector<double> doubleArray;
                    for (const auto& item : *it) {
                        doubleArray.push_back(item.get<double>());
                    }
                    value = doubleArray;
                }
            }
        }
        
        // Add to entries
        entries.emplace_back(key, value, ConfigSource::File, description);
    }
}

nlohmann::json Config::configToJSON(const std::vector<ConfigEntry>& entries) {
    nlohmann::json json;
    
    for (const auto& entry : entries) {
        std::string key = entry.key;
        json[key] = configValueToString(entry.value);
    }
    
    return json;
}

void Config::parseYAMLToConfig(const YAML::Node& yaml, std::vector<ConfigEntry>& entries) {
    entries.clear();
    
    if (!yaml.IsMap()) {
        return;
    }
    
    for (auto it = yaml.begin(); it != yaml.end(); ++it) {
        std::string key = normalizeKey(it->first.as<std::string>());
        ConfigValue value;
        std::string description = "";
        
        // Extract value based on YAML type
        if (it->second.IsScalar()) {
            std::string strValue = it->second.as<std::string>();
            value = stringToConfigValue(strValue);
        } else if (it->second.IsSequence()) {
            // Handle sequences
            if (!it->second.empty()) {
                YAML::Node first = it->second[0];
                if (first.IsScalar()) {
                    std::vector<std::string> strArray;
                    for (const auto& item : it->second) {
                        strArray.push_back(item.as<std::string>());
                    }
                    value = strArray;
                }
            }
        }
        
        // Add to entries
        entries.emplace_back(key, value, ConfigSource::File, description);
    }
}

YAML::Node Config::configToYAML(const std::vector<ConfigEntry>& entries) {
    YAML::Node yaml;
    
    for (const auto& entry : entries) {
        std::string key = entry.key;
        yaml[key] = configValueToString(entry.value);
    }
    
    return yaml;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Save in JSON format (modern format)
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    auto json = configToJSON(pImpl->entries);
    file << json.dump(2); // Pretty print with 2-space indentation
    return true;
}

std::string Config::saveToJSON() const {
    auto json = configToJSON(pImpl->entries);
    return json.dump(2);
}

std::string Config::saveToYAML() const {
    auto yaml = configToYAML(pImpl->entries);
    std::stringstream stream;
    stream << yaml;
    return stream.str();
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

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    // Basic validation rules
    if (!has("brain.neuron_count")) {
        pImpl->validationErrors.push_back("Missing required key: brain.neuron_count");
    } else {
        auto neuronCountOpt = get<int>("brain.neuron_count");
        if (neuronCountOpt && neuronCountOpt.value() <= 0) {
            pImpl->validationErrors.push_back("brain.neuron_count must be positive");
        } else if (neuronCountOpt && neuronCountOpt.value() > 1000000) {
            pImpl->validationErrors.push_back("brain.neuron_count is very large, may impact performance");
        }
    }
    
    if (!has("brain.connection_probability")) {
        pImpl->validationErrors.push_back("Missing recommended key: brain.connection_probability");
    } else {
        auto connProbOpt = get<double>("brain.connection_probability");
        if (connProbOpt) {
            double prob = connProbOpt.value();
            if (prob < 0.0 || prob > 1.0) {
                pImpl->validationErrors.push_back("brain.connection_probability must be between 0.0 and 1.0");
            }
        }
    }
    
    return pImpl->validationErrors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

void Config::merge(const Config& other) {
    auto otherKeys = other.getKeys();
    for (const auto& key : otherKeys) {
        auto valueOpt = other.get<std::string>(key);
        if (valueOpt) {
            set(key, *valueOpt);
        }
    }
}

void Config::substituteEnvironmentVariables() {
    for (auto& entry : pImpl->entries) {
        if (std::holds_alternative<std::string>(entry.value)) {
            std::string& value = std::get<std::string>(entry.value);
            // Replace ${VAR} or $VAR patterns
            size_t pos = 0;
            while ((pos = value.find('$', pos)) != std::string::npos) {
                size_t endPos = value.find('}', pos);
                if (endPos == std::string::npos) {
                    endPos = value.find(' ', pos);
                }
                if (endPos == std::string::npos) endPos = value.size();
                
                std::string varName = value.substr(pos + 1, endPos - pos - 1);
                const char* envValue = std::getenv(varName.c_str());
                if (envValue) {
                    value.replace(pos, endPos - pos + 1, envValue);
                    pos += strlen(envValue);
                } else {
                    pos = endPos + 1;
                }
            }
        }
    }
}

void Config::setSchema(const nlohmann::json& schema) {
    pImpl->schema = schema;
}

bool Config::validateAgainstSchema() const {
    if (!pImpl->schema.empty()) {
        auto json = configToJSON(pImpl->entries);
        // Simple schema validation - check required fields
        if (pImpl->schema.contains("required")) {
            auto required = pImpl->schema["required"];
            if (required.is_array()) {
                for (const auto& item : required) {
                    if (!has(item.get<std::string>())) {
                        pImpl->validationErrors.push_back("Missing required schema field: " + item.get<std::string>());
                    }
                }
            }
        }
    }
    return validate();
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

std::string Config::normalizeKey(const std::string& key) {
    std::string result = toLower(trim(key));
    // Replace spaces and special characters with underscores
    for (char& c : result) {
        if (!std::isalnum(c) && c != '_') {
            c = '_';
        }
    }
    return result;
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::stringstream ss;
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            ss << "["; 
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << arg[i] << "\"";
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else {
            ss << arg;
        }
    }, value);
    return ss.str();
}

ConfigValue Config::stringToConfigValue(const std::string& str) {
    // Try to determine type from string
    if (str.empty()) return std::string();
    
    // Check for boolean
    if (str == "true") return true;
    if (str == "false") return false;
    
    // Check for number
    try {
        if (str.find('.') != std::string::npos) {
            return std::stod(str);
        } else {
            return std::stoi(str);
        }
    } catch (...) {
        // Not a number, return as string
        return str;
    }
}

} // namespace nlm
