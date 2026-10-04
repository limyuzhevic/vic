#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    // Try JSON first (common extensions)
    if (ext == "json" || ext == "JSON" || ext == "cfg" || ext == "conf") {
        return loadFromJSON(filepath);
    }
    // Try YAML next
    else if (ext == "yaml" || ext == "yml") {
        return loadFromYAML(filepath);
    }
    // Try key=value format as fallback
    else {
        return loadFromKeyValue(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON config file: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Recursively parse JSON and add to config
        parseJSONRecursive("", jsonData, ConfigSource::File);
        
        NLM_LOG_INFO("Successfully loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in file " + filepath + ": " + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading JSON config file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        // Clear existing entries
        clear();
        
        // Recursively parse YAML and add to config
        parseYAMLRecursive("", yamlData, ConfigSource::File);
        
        NLM_LOG_INFO("Successfully loaded YAML configuration from: " + filepath);
        return true;
        
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML parse error in file " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading YAML config file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromKeyValue(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
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
            
            set(key, value, ConfigSource::File);
            NLM_LOG_DEBUG("Loaded config key: " + key + " = " + value + " (line " + std::to_string(lineNumber) + ")");
        } else {
            NLM_LOG_WARNING("Ignoring malformed line in config file " + filepath + ": " + line + " (line " + std::to_string(lineNumber) + ")");
        }
    }
    
    NLM_LOG_INFO("Successfully loaded key-value configuration from: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

void Config::parseJSONRecursive(const std::string& prefix, const nlohmann::json& jsonData, ConfigSource source) {
    if (jsonData.is_object()) {
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = prefix.empty() ? it.key() : (prefix + "." + it.key());
            parseJSONRecursive(key, it.value(), source);
        }
    } else if (jsonData.is_array()) {
        for (size_t i = 0; i < jsonData.size(); ++i) {
            std::string key = prefix + "[" + std::to_string(i) + "]";
            parseJSONRecursive(key, jsonData[i], source);
        }
    } else {
        setConfigValueFromJSON(key, jsonData, source);
    }
}

void Config::parseYAMLRecursive(const std::string& prefix, const YAML::Node& yamlData, ConfigSource source) {
    if (yamlData.IsMap()) {
        for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
            std::string key = prefix.empty() ? it->first.as<std::string>() : (prefix + "." + it->first.as<std::string>());
            parseYAMLRecursive(key, it->second, source);
        }
    } else if (yamlData.IsSequence()) {
        for (size_t i = 0; i < yamlData.size(); ++i) {
            std::string key = prefix + "[" + std::to_string(i) + "]";
            parseYAMLRecursive(key, yamlData[i], source);
        }
    } else {
        setConfigValueFromYAML(prefix, yamlData, source);
    }
}

void Config::setConfigValueFromJSON(const std::string& key, const nlohmann::json& jsonValue, ConfigSource source) {
    std::string description = "Config value: " + key;
    
    if (jsonValue.is_string()) {
        set(key, jsonValue.get<std::string>(), source);
        setEntryDescription(key, description);
    } else if (jsonValue.is_number_integer()) {
        set(key, jsonValue.get<int64_t>(), source);
        setEntryDescription(key, description);
    } else if (jsonValue.is_number_unsigned()) {
        set(key, static_cast<int64_t>(jsonValue.get<uint64_t>()), source);
        setEntryDescription(key, description);
    } else if (jsonValue.is_number_float()) {
        set(key, jsonValue.get<double>(), source);
        setEntryDescription(key, description);
    } else if (jsonValue.is_boolean()) {
        set(key, jsonValue.get<bool>(), source);
        setEntryDescription(key, description);
    } else if (jsonValue.is_array()) {
        // For arrays, convert to string representation
        std::string arrayStr = "[";
        for (size_t i = 0; i < jsonValue.size(); ++i) {
            if (i > 0) arrayStr += ", ";
            arrayStr += jsonValue[i].dump();
        }
        arrayStr += "]";
        set(key, arrayStr, source);
        setEntryDescription(key, description);
    } else {
        set(key, jsonValue.dump(), source);
        setEntryDescription(key, description);
    }
}

void Config::setConfigValueFromYAML(const std::string& key, const YAML::Node& yamlValue, ConfigSource source) {
    std::string description = "Config value: " + key;
    
    if (yamlValue.IsScalar()) {
        std::string value = yamlValue.as<std::string>();
        
        // Try to infer type
        if (value == "true" || value == "false") {
            set(key, value == "true", source);
        } else {
            try {
                size_t pos;
                int intVal = std::stoi(value, &pos);
                if (pos == value.size()) {
                    set(key, intVal, source);
                } else {
                    double floatVal = std::stod(value, &pos);
                    if (pos == value.size()) {
                        set(key, floatVal, source);
                    } else {
                        set(key, value, source);
                    }
                }
            } catch (...) {
                set(key, value, source);
            }
        }
        setEntryDescription(key, description);
    } else if (yamlValue.IsSequence()) {
        // For sequences, convert to string representation
        std::string arrayStr = "[";
        for (size_t i = 0; i < yamlValue.size(); ++i) {
            if (i > 0) arrayStr += ", ";
            arrayStr += yamlValue[i].as<std::string>();
        }
        arrayStr += "]";
        set(key, arrayStr, source);
        setEntryDescription(key, description);
    } else {
        set(key, yamlValue.as<std::string>(), source);
        setEntryDescription(key, description);
    }
}

void Config::setEntryDescription(const std::string& key, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
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
    // Determine file format based on extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    try {
        if (ext == "json" || ext == "JSON" || ext == "cfg" || ext == "conf") {
            saveToJSON(file);
        } else if (ext == "yaml" || ext == "yml") {
            saveToYAML(file);
        } else {
            saveToKeyValue(file);
        }
        
        NLM_LOG_INFO("Successfully saved configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving config file " + filepath + ": " + e.what());
        return false;
    }
}

void Config::saveToJSON(std::ostream& stream) const {
    nlohmann::json jsonData;
    
    // Convert config entries to JSON
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        nlohmann::json& jsonValue = jsonData;
        
        // Handle nested keys (e.g., "section.subsection.key")
        size_t dotPos = key.find('.');
        while (dotPos != std::string::npos) {
            std::string section = key.substr(0, dotPos);
            key = key.substr(dotPos + 1);
            if (!jsonValue.contains(section)) {
                jsonValue[section] = nlohmann::json::object();
            }
            jsonValue = jsonValue[section];
            dotPos = key.find('.');
        }
        
        // Set the final value
        jsonValue[key] = getConfigValueAsJSON(entry.value);
    }
    
    // Write JSON with indentation
    stream << std::setw(2) << jsonData;
}

void Config::saveToYAML(std::ostream& stream) const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    // Convert config entries to YAML
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        YAML::Node current = emitter;
        
        // Handle nested keys (e.g., "section.subsection.key")
        size_t dotPos = key.find('.');
        while (dotPos != std::string::npos) {
            std::string section = key.substr(0, dotPos);
            key = key.substr(dotPos + 1);
            if (!current[section]) {
                current[section] = YAML::Node(YAML::NodeType::Map);
            }
            current = current[section];
            dotPos = key.find('.');
        }
        
        // Set the final value
        current[key] = getConfigValueAsYAML(entry.value);
    }
    
    emitter << YAML::EndMap;
    stream << emitter.c_str();
}

void Config::saveToKeyValue(std::ostream& stream) const {
    for (const auto& entry : pImpl->entries) {
        stream << "# " << entry.description << "\n";
        stream << entry.key << " = " << getConfigValueAsString(entry.value) << "\n\n";
    }
}

nlohmann::json Config::getConfigValueAsJSON(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
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
            return nlohmann::json(arg);
        }
    }, value);
}

YAML::Node Config::getConfigValueAsYAML(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> YAML::Node {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (const auto& item : arg) {
                node.push_back(item);
            }
            return node;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (const auto& item : arg) {
                node.push_back(item);
            }
            return node;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (const auto& item : arg) {
                node.push_back(item);
            }
            return node;
        } else {
            return YAML::Load(std::string(arg));
        }
    }, value);
}

std::string Config::getConfigValueAsString(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return '"' + arg + '"';
        } else {
            return std::to_string(arg);
        }
    }, value);
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    NLM_LOG_DEBUG("Getting config value for key: " + key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Config key not found: " + key);
        return std::nullopt;
    }
    
    try {
        T value = std::get<T>(it->value);
        NLM_LOG_DEBUG("Successfully retrieved config value: " + key + " = " + getConfigValueAsString(value));
        return value;
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Config value type mismatch for key: " + key + " - expected " + typeid(T).name() + "; actual: " + getValueTypeName(it->value));
        return std::nullopt;
    }
}

std::string Config::getValueTypeName(const ConfigValue& value) const {
    std::string typeName;
    std::visit([&typeName](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            typeName = "int";
        } else if constexpr (std::is_same_v<T, int64_t>) {
            typeName = "int64_t";
        } else if constexpr (std::is_same_v<T, double>) {
            typeName = "double";
        } else if constexpr (std::is_same_v<T, bool>) {
            typeName = "bool";
        } else if constexpr (std::is_same_v<T, std::string>) {
            typeName = "string";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            typeName = "vector<int>";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            typeName = "vector<double>";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            typeName = "vector<string>";
        } else {
            typeName = "unknown";
        }
    }, value);
    return typeName;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    NLM_LOG_DEBUG("Setting config value: " + key + " = " + getConfigValueAsString(value) + " (source: " + configSourceToString(source) + ")");
    
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Invalid config key: cannot be empty");
        return;
    }
    
    // Check for valid characters in key
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.' && c != '-' && c != '[' && c != ']') {
            NLM_LOG_WARNING("Config key contains unusual characters: " + key);
            break;
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated existing config entry: " + key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added new config entry: " + key);
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

bool Config::validate(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_ERROR("Validation failed: key cannot be empty");
        return false;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_ERROR("Validation failed: key not found: " + key);
        return false;
    }
    
    // Additional validation could be added here
    return true;
}

ConfigSource Config::configSourceToString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        default: return "Unknown";
    }
}

std::string Config::configSourceToString(ConfigSource source) const {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        default: return "Unknown";
    }
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
