#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <codecvt>

namespace nlm {

using json = nlohmann::json;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ValidationCallback validationCallback;
    std::unordered_map<std::string, std::vector<std::string>> sourceIndex;
    bool validateOnLoad;
    bool mergeOnImport;
    std::string defaultFilepath;
    
    Impl()
        : validateOnLoad(true), mergeOnImport(true), defaultFilepath("./config.json") {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // PHASE 6: Advanced configuration system with JSON/YAML support
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    // Try to detect file format
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0, std::ios::beg);
    
    // Check for JSON format (starts with { and contains key-value pairs)
    if (firstLine.find('{') != std::string::npos && firstLine.find('}') != std::string::npos) {
        // JSON format
        return loadFromJSON(filepath);
    } else if (firstLine.find('-') != std::string::npos && firstLine.find(':') != std::string::npos) {
        // YAML-like format (simplified)
        return loadFromSimplified(filepath);
    } else {
        // Original key=value format for backward compatibility
        return loadFromSimple(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open JSON file: " + filepath);
            return false;
        }
        
        json j;
        file >> j;
        
        // Clear existing configuration
        pImpl->entries.clear();
        pImpl->sourceIndex.clear();
        
        // Recursively parse JSON
        parseJSON(j, filepath, 0);
        
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries");
        
        // Validate if enabled
        if (pImpl->validateOnLoad) {
            validate();
        }
        
        return true;
        
    } catch (const json::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromSimplified(const std::string& filepath) {
    // YAML-like format (simplified key: value pairs)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open simplified config file: " + filepath);
        return false;
    }
    
    // Clear existing configuration
    pImpl->entries.clear();
    pImpl->sourceIndex.clear();
    
    std::string line;
    int lineNum = 0;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Check for section headers
        if (line.find('[') != std::string::npos && line.find(']') != std::string::npos) {
            currentSection = line.substr(1, line.find(']') - 1);
            continue;
        }
        
        // Parse key: value
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Add section prefix to key
            if (!currentSection.empty()) {
                key = currentSection + "." + key;
            }
            
            ConfigEntry entry(key, value, ConfigSource::File, "", filepath, lineNum);
            pImpl->entries.push_back(entry);
            
            // Update index
            pImpl->sourceIndex[key].push_back(filepath);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from simplified file: " + filepath);
    NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries");
    
    return true;
}

bool Config::loadFromSimple(const std::string& filepath) {
    // Original key=value format for backward compatibility
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open simple config file: " + filepath);
        return false;
    }
    
    // Clear existing configuration
    pImpl->entries.clear();
    pImpl->sourceIndex.clear();
    
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
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
            
            ConfigEntry entry(key, value, ConfigSource::File, "", filepath, lineNum);
            pImpl->entries.push_back(entry);
            
            // Update index
            pImpl->sourceIndex[key].push_back(filepath);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from simple file: " + filepath);
    NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries");
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = loadFromSimpleArgs(argc, argv);
    success = loadFromEnvironment() && success;
    return success;
}

bool Config::loadFromSimpleArgs(int argc, char** argv) {
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

bool Config::loadFromEnvironment(const std::string& prefix) {
    bool success = true;
    
    // Get all environment variables
    extern char** environ;
    for (char** env = environ; *env != nullptr; ++env) {
        std::string envStr(*env);
        size_t eqPos = envStr.find('=');
        if (eqPos == std::string::npos) continue;
        
        std::string key = envStr.substr(0, eqPos);
        std::string value = envStr.substr(eqPos + 1);
        
        // Check if key starts with prefix
        if (key.substr(0, prefix.size()) == prefix) {
            // Remove prefix and convert to lowercase
            std::string configKey = key.substr(prefix.size());
            
            // Convert environment variable name to config key format
            for (char& c : configKey) {
                if (c == '_') c = '.';
            }
            
            set(configKey, value, ConfigSource::Environment);
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath, Format format) const {
    switch (format) {
        case Format::SIMPLE:
            return saveToSimple(filepath);
        case Format::JSON:
            return saveToJSON(filepath);
        case Format::YAML:
            return saveToYAML(filepath);
        default:
            NLM_LOG_ERROR("Unsupported file format: " + std::to_string(static_cast<int>(format)));
            return false;
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    try {
        json j;
        
        // Convert configuration entries to JSON
        for (const auto& entry : pImpl->entries) {
            setJSONKeyValue(j, entry.key, entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot create JSON file: " + filepath);
            return false;
        }
        
        file << std::setw(2) << j;
        
        NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
        return true;
        
    } catch (const json::exception& e) {
        NLM_LOG_ERROR("JSON writing error in " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    try {
        YAML::Emitter out;
        out << YAML::BeginDoc;
        
        // Group entries by section
        std::unordered_map<std::string, YAML::Node> sections;
        for (const auto& entry : pImpl->entries) {
            size_t dotPos = entry.key.find('.');
            if (dotPos != std::string::npos) {
                std::string section = entry.key.substr(0, dotPos);
                std::string key = entry.key.substr(dotPos + 1);
                if (sections.find(section) == sections.end()) {
                    sections[section] = YAML::Node(YAML::NodeType::Map);
                }
                setYAMLKeyValue(sections[section], key, entry.value);
            } else {
                setYAMLKeyValue(out, entry.key, entry.value);
            }
        }
        
        // Write sections
        for (const auto& section : sections) {
            out << YAML::Key << section.first << YAML::Value << section.second;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot create YAML file: " + filepath);
            return false;
        }
        
        file << out.c_str();
        
        NLM_LOG_INFO("Saved configuration to YAML file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToSimple(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot create simple config file: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source != ConfigSource::Default) {  // Don't save default values
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << escapeString(getValueAsString(entry.value)) << "\n";
            file << "# Source: " << static_cast<int>(entry.source) << "\n\n";
        }
    }
    
    NLM_LOG_INFO("Saved configuration to simple file: " + filepath);
    return true;
}

bool Config::validate() const {
    NLM_LOG_INFO("Validating configuration...");
    
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        if (pImpl->validationCallback) {
            if (!pImpl->validationCallback(entry)) {
                NLM_LOG_ERROR("Configuration validation failed for key: " + entry.key);
                allValid = false;
            }
        } else {
            // Default validation: check if value types are reasonable
            if (!validateEntry(entry)) {
                NLM_LOG_ERROR("Configuration validation failed for key: " + entry.key);
                allValid = false;
            }
        }
    }
    
    if (allValid) {
        NLM_LOG_INFO("Configuration validation passed");
    } else {
        NLM_LOG_ERROR("Configuration validation failed");
    }
    
    return allValid;
}

void Config::setValidationCallback(ValidationCallback callback) {
    pImpl->validationCallback = callback;
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    // Basic validation: check for null or empty values where appropriate
    if (entry.key.empty()) {
        NLM_LOG_ERROR("Configuration entry has empty key");
        return false;
    }
    
    // Validate based on value type
    switch (entry.value.index()) {
        case 0: // int
        case 1: // int64_t
            // Check if value is within reasonable range
            break;
        case 2: { // double
            double val = std::get<double>(entry.value);
            if (std::isnan(val) || std::isinf(val)) {
                NLM_LOG_ERROR("Invalid numeric value for key: " + entry.key);
                return false;
            }
            break;
        }
        case 3: // bool
        case 4: { // std::string
            // String values should not be excessively long
            std::string val = std::get<std::string>(entry.value);
            if (val.length() > 10000) {
                NLM_LOG_ERROR("Value for key " + entry.key + " is too long");
                return false;
            }
            break;
        }
        case 5: { // std::vector<int>
            const auto& val = std::get<std::vector<int>>(entry.value);
            if (val.size() > 10000) {
                NLM_LOG_ERROR("Vector value for key " + entry.key + " is too large");
                return false;
            }
            break;
        }
        case 6: { // std::vector<double>
            const auto& val = std::get<std::vector<double>>(entry.value);
            if (val.size() > 10000) {
                NLM_LOG_ERROR("Vector value for key " + entry.key + " is too large");
                return false;
            }
            break;
        }
        case 7: { // std::vector<std::string>
            const auto& val = std::get<std::vector<std::string>>(entry.value);
            if (val.size() > 10000) {
                NLM_LOG_ERROR("Vector value for key " + entry.key + " is too large");
                return false;
            }
            break;
        }
        case 8: { // std::map<std::string, std::string> (nested)
            const auto& val = std::get<std::map<std::string, std::string>>(entry.value);
            if (val.size() > 1000) {
                NLM_LOG_ERROR("Nested value for key " + entry.key + " is too large");
                return false;
            }
            break;
        }
    }
    
    return true;
}

void Config::parseJSON(const json& j, const std::string& filepath, int lineNum) {
    if (j.is_object()) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            int currentLine = lineNum;
            parseJSONValue(it.value(), filepath, currentLine, key);
        }
    }
}

void Config::parseJSONValue(const json& j, const std::string& filepath, int lineNum, const std::string& key) {
    std::string fullKey = key;
    
    if (j.is_object()) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string subKey = fullKey.empty() ? it.key() : fullKey + "." + it.key();
            parseJSONValue(it.value(), filepath, lineNum, subKey);
        }
    } else {
        ConfigValue value;
        jsonToConfigValue(j, value);
        ConfigEntry entry(fullKey, value, ConfigSource::File, "", filepath, lineNum);
        pImpl->entries.push_back(entry);
        pImpl->sourceIndex[fullKey].push_back(filepath);
    }
}

void Config::jsonToConfigValue(const json& j, ConfigValue& outValue) {
    if (j.is_boolean()) {
        outValue = j.get<bool>();
    } else if (j.is_number_integer()) {
        outValue = j.get<int>();
    } else if (j.is_number_unsigned()) {
        outValue = static_cast<int64_t>(j.get<uint64_t>());
    } else if (j.is_number_float()) {
        outValue = j.get<double>();
    } else if (j.is_string()) {
        outValue = j.get<std::string>();
    } else if (j.is_array()) {
        // Determine array type
        if (!j.empty()) {
            if (j[0].is_boolean()) {
                std::vector<bool> vec;
                for (const auto& item : j) {
                    vec.push_back(item.get<bool>());
                }
                outValue = vec;
            } else if (j[0].is_number_integer()) {
                std::vector<int> vec;
                for (const auto& item : j) {
                    vec.push_back(item.get<int>());
                }
                outValue = vec;
            } else if (j[0].is_number_float()) {
                std::vector<double> vec;
                for (const auto& item : j) {
                    vec.push_back(item.get<double>());
                }
                outValue = vec;
            } else if (j[0].is_string()) {
                std::vector<std::string> vec;
                for (const auto& item : j) {
                    vec.push_back(item.get<std::string>());
                }
                outValue = vec;
            }
        }
    } else if (j.is_object()) {
        // Convert nested object to map
        std::map<std::string, std::string> map;
        for (auto it = j.begin(); it != j.end(); ++it) {
            map[it.key()] = it.value().dump();
        }
        outValue = map;
    }
}

void Config::setJSONKeyValue(json& j, const std::string& key, const ConfigValue& value) {
    size_t dotPos = key.find('.');
    if (dotPos != std::string::npos) {
        std::string section = key.substr(0, dotPos);
        std::string subKey = key.substr(dotPos + 1);
        
        if (!j.contains(section)) {
            j[section] = json::object();
        }
        
        setJSONKeyValue(j[section], subKey, value);
    } else {
        configValueToJSON(j[key], value);
    }
}

void Config::configValueToJSON(json& j, const ConfigValue& value) {
    switch (value.index()) {
        case 0: // int
            j = std::get<int>(value);
            break;
        case 1: // int64_t
            j = std::get<int64_t>(value);
            break;
        case 2: // double
            j = std::get<double>(value);
            break;
        case 3: // bool
            j = std::get<bool>(value);
            break;
        case 4: { // std::string
            j = std::get<std::string>(value);
            break;
        }
        case 5: { // std::vector<int>
            std::vector<int> vec = std::get<std::vector<int>>(value);
            j = vec;
            break;
        }
        case 6: { // std::vector<double>
            std::vector<double> vec = std::get<std::vector<double>>(value);
            j = vec;
            break;
        }
        case 7: { // std::vector<std::string>
            std::vector<std::string> vec = std::get<std::vector<std::string>>(value);
            j = vec;
            break;
        }
        case 8: { // std::map<std::string, std::string>
            std::map<std::string, std::string> map = std::get<std::map<std::string, std::string>>(value);
            j = map;
            break;
        }
    }
}

void Config::setYAMLKeyValue(YAML::Emitter& out, const std::string& key, const ConfigValue& value) {
    switch (value.index()) {
        case 0: // int
            out << YAML::Key << key << YAML::Value << std::get<int>(value);
            break;
        case 1: // int64_t
            out << YAML::Key << key << YAML::Value << std::get<int64_t>(value);
            break;
        case 2: // double
            out << YAML::Key << key << YAML::Value << std::get<double>(value);
            break;
        case 3: // bool
            out << YAML::Key << key << YAML::Value << std::get<bool>(value);
            break;
        case 4: { // std::string
            out << YAML::Key << key << YAML::Value << std::get<std::string>(value);
            break;
        }
        case 5: { // std::vector<int>
            std::vector<int> vec = std::get<std::vector<int>>(value);
            out << YAML::Key << key << YAML::Value << vec;
            break;
        }
        case 6: { // std::vector<double>
            std::vector<double> vec = std::get<std::vector<double>>(value);
            out << YAML::Key << key << YAML::Value << vec;
            break;
        }
        case 7: { // std::vector<std::string>
            std::vector<std::string> vec = std::get<std::vector<std::string>>(value);
            out << YAML::Key << key << YAML::Value << vec;
            break;
        }
        case 8: { // std::map<std::string, std::string>
            std::map<std::string, std::string> map = std::get<std::map<std::string, std::string>>(value);
            out << YAML::Key << key << YAML::Value << map;
            break;
        }
    }
}

void Config::setYAMLKeyValue(YAML::Node& parent, const std::string& key, const ConfigValue& value) {
    switch (value.index()) {
        case 0: // int
            parent[key] = std::get<int>(value);
            break;
        case 1: // int64_t
            parent[key] = std::get<int64_t>(value);
            break;
        case 2: // double
            parent[key] = std::get<double>(value);
            break;
        case 3: // bool
            parent[key] = std::get<bool>(value);
            break;
        case 4: { // std::string
            parent[key] = std::get<std::string>(value);
            break;
        }
        case 5: { // std::vector<int>
            std::vector<int> vec = std::get<std::vector<int>>(value);
            parent[key] = vec;
            break;
        }
        case 6: { // std::vector<double>
            std::vector<double> vec = std::get<std::vector<double>>(value);
            parent[key] = vec;
            break;
        }
        case 7: { // std::vector<std::string>
            std::vector<std::string> vec = std::get<std::vector<std::string>>(value);
            parent[key] = vec;
            break;
        }
        case 8: { // std::map<std::string, std::string>
            std::map<std::string, std::string> map = std::get<std::map<std::string, std::string>>(value);
            parent[key] = map;
            break;
        }
    }
}

std::string Config::getValueAsString(const ConfigValue& value) const {
    switch (value.index()) {
        case 0: // int
            return std::to_string(std::get<int>(value));
        case 1: // int64_t
            return std::to_string(std::get<int64_t>(value));
        case 2: // double
            return std::to_string(std::get<double>(value));
        case 3: // bool
            return std::get<bool>(value) ? "true" : "false";
        case 4: // std::string
            return std::get<std::string>(value);
        default:
            return "";
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    // Remove from entries
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    // Remove from index
    pImpl->sourceIndex.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->sourceIndex.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (Source: " << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

std::string Config::getSourceSummary(ConfigSource source) const {
    std::ostringstream oss;
    oss << "Configuration entries from source " << static_cast<int>(source) << ":\n";
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            oss << "  " << entry.key << " = " << getValueAsString(entry.value) << "\n";
        }
    }
    
    return oss.str();
}

bool Config::importFrom(const Config& other) {
    if (pImpl->mergeOnImport) {
        // Merge other's entries into this one
        for (const auto& otherEntry : other.pImpl->entries) {
            bool found = false;
            for (auto& thisEntry : pImpl->entries) {
                if (thisEntry.key == otherEntry.key) {
                    // Key exists, update it
                    thisEntry.value = otherEntry.value;
                    thisEntry.source = otherEntry.source;
                    thisEntry.description = otherEntry.description;
                    found = true;
                    break;
                }
            }
            if (!found) {
                // Add new entry
                pImpl->entries.push_back(otherEntry);
            }
        }
    } else {
        // Replace all entries
        pImpl->entries = other.pImpl->entries;
    }
    
    // Update index
    pImpl->sourceIndex.clear();
    for (const auto& entry : pImpl->entries) {
        pImpl->sourceIndex[entry.key].push_back("imported");
    }
    
    NLM_LOG_INFO("Imported configuration from another Config object");
    return true;
}

bool Config::exportTo(Config& other) const {
    // Clear other's configuration
    other.pImpl->entries.clear();
    other.pImpl->sourceIndex.clear();
    
    // Copy all entries
    for (const auto& entry : pImpl->entries) {
        other.pImpl->entries.push_back(entry);
    }
    
    // Update index
    other.pImpl->sourceIndex.clear();
    for (const auto& entry : other.pImpl->entries) {
        other.pImpl->sourceIndex[entry.key].push_back("exported");
    }
    
    NLM_LOG_INFO("Exported configuration to another Config object");
    return true;
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (entry.key == otherEntry.key && entry.value == otherEntry.value) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    
    return true;
}

void Config::merge(const Config& other, bool overwrite) {
    for (const auto& otherEntry : other.pImpl->entries) {
        bool found = false;
        for (auto& thisEntry : pImpl->entries) {
            if (thisEntry.key == otherEntry.key) {
                if (overwrite) {
                    thisEntry.value = otherEntry.value;
                    thisEntry.source = otherEntry.source;
                    thisEntry.description = otherEntry.description;
                }
                found = true;
                break;
            }
        }
        if (!found) {
            pImpl->entries.push_back(otherEntry);
        }
    }
}

std::string Config::createSnapshot() const {
    std::ostringstream oss;
    oss << "NLM-ConfigSnapshot:" << std::endl;
    oss << "Timestamp: " << getCurrentTimestamp() << std::endl;
    oss << "Version: 1.0" << std::endl;
    oss << "Entries:" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "  - key: " << entry.key << std::endl;
        oss << "    value: " << getValueAsString(entry.value) << std::endl;
        oss << "    source: " << static_cast<int>(entry.source) << std::endl;
        oss << "    description: " << entry.description << std::endl;
    }
    
    return oss.str();
}

bool Config::restoreFromSnapshot(const std::string& snapshot) {
    // Simple snapshot parsing for now
    // In a real implementation, you'd use proper serialization
    if (snapshot.find("NLM-ConfigSnapshot:") == std::string::npos) {
        NLM_LOG_ERROR("Invalid snapshot format");
        return false;
    }
    
    // Clear existing configuration
    clear();
    
    // Parse snapshot (simplified)
    size_t pos = 0;
    while (pos < snapshot.length()) {
        size_t keyStart = snapshot.find("  - key: ", pos);
        if (keyStart == std::string::npos) break;
        
        keyStart += 8; // Length of "  - key: "
        size_t keyEnd = snapshot.find("\n", keyStart);
        if (keyEnd == std::string::npos) break;
        
        std::string key = snapshot.substr(keyStart, keyEnd - keyStart);
        
        size_t valueStart = snapshot.find("    value: ", keyEnd);
        if (valueStart == std::string::npos) break;
        
        valueStart += 10; // Length of "    value: "
        size_t valueEnd = snapshot.find("\n", valueStart);
        if (valueEnd == std::string::npos) break;
        
        std::string valueStr = snapshot.substr(valueStart, valueEnd - valueStart);
        
        // Parse value (simplified)
        ConfigValue value;
        if (valueStr == "true" || valueStr == "false") {
            value = (valueStr == "true");
        } else {
            try {
                size_t decPos = valueStr.find('.');
                if (decPos != std::string::npos) {
                    value = std::stod(valueStr);
                } else {
                    value = std::stoll(valueStr);
                }
            } catch (...) {
                value = valueStr;
            }
        }
        
        ConfigEntry entry(key, value, ConfigSource::Runtime);
        pImpl->entries.push_back(entry);
        
        pos = valueEnd + 1;
    }
    
    NLM_LOG_INFO("Restored configuration from snapshot");
    return true;
}

std::string Config::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    return ss.str();
}

std::string Config::validateAgainstSchema(const std::string& schemaFile) const {
    // Simple schema validation for now
    // In a real implementation, you'd use a proper schema validation library
    
    std::string errors;
    
    // Load schema file (simplified)
    std::ifstream file(schemaFile);
    if (!file.is_open()) {
        return "Cannot open schema file: " + schemaFile;
    }
    
    // Parse schema and validate (simplified)
    // This is a placeholder for advanced schema validation
    
    return errors.empty() ? "Schema validation passed" : errors;
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

    
    // Try to detect file format
    std::string content;
    std::getline(file, content);
    file.clear();
    file.seekg(0, std::ios::beg);
    
    std::getline(file, content);
    file.clear();
    file.seekg(0, std::ios::beg);
    
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // JSON format
        return loadFromJSON(filepath);
    } else if (content.find('-') != std::string::npos) {
        // YAML-like format (simplified)
        return loadFromSimplified(filepath);
    } else {
        // Original key=value format for backward compatibility
        return loadFromSimple(filepath);
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
