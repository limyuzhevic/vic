#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <map>
#include <chrono>

// Optional includes for JSON/YAML support
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

// ConfigSource implementation
std::string ConfigSource::getSourceName() const {
    switch (type) {
        case Type::Default: return "default";
        case Type::File: return "file:" + filename;
        case Type::CommandLine: return "command line";
        case Type::Runtime: return "runtime";
        default: return "unknown";
    }
}

// Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigValidator> validators;
    size_t maxEntries;
    
    Impl() : maxEntries(10000) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try YAML first, then JSON, then legacy format
    if (loadFromYAML(filepath)) {
        return true;
    }
    if (loadFromJSON(filepath)) {
        return true;
    }
    return loadFromLegacyFile(filepath);
}

bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAVE_YAML_CPP
    try {
        YAML::Node root = YAML::LoadFile(filepath);
        bool success = importFromYAML(YAML::EmitToString(root), ConfigSource(ConfigSource::Type::File, filepath));
        if (success) {
            NLM_LOG_INFO("Loaded configuration from YAML file: " + filepath);
        }
        return success;
    } catch (const std::exception& e) {
        NLM_LOG_WARN("Failed to parse YAML file " + filepath + ": " + e.what());
    }
#endif
    return false;
}

bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAVE_NLOHMANN_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        nlohmann::json j;
        file >> j;
        
        std::string jsonContent;
        file.clear();
        file.seekg(0);
        std::getline(file, jsonContent, static_cast<char>(file.eof()));
        
        bool success = importFromJSON(jsonContent, ConfigSource(ConfigSource::Type::File, filepath));
        if (success) {
            NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        }
        return success;
    } catch (const std::exception& e) {
        NLM_LOG_WARN("Failed to parse JSON file " + filepath + ": " + e.what());
    }
#endif
    return false;
}

bool Config::loadFromYAMLString(const std::string& content) {
    return importFromYAML(content, ConfigSource(ConfigSource::Type::File, "string"));
}

bool Config::loadFromJSONString(const std::string& content) {
    return importFromJSON(content, ConfigSource(ConfigSource::Type::File, "string"));
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = loadFromLegacyArgs(argc, argv);
    if (!success) {
        NLM_LOG_ERROR("Failed to parse command line arguments");
    }
    return success;
}

bool Config::loadFromLegacyFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Legacy support for simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
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
            
            set(key, value, ConfigSource(ConfigSource::Type::File, filepath, lineNumber));
        }
    }
    
    if (!pImpl->entries.empty()) {
        NLM_LOG_INFO("Loaded legacy configuration from: " + filepath + " (" + 
                    std::to_string(pImpl->entries.size()) + " entries)");
    }
    
    return true;
}

bool Config::loadFromLegacyArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource(ConfigSource::Type::CommandLine, "", i));
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource(ConfigSource::Type::CommandLine, "", i));
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool pretty) const {
    // Determine format based on file extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "yaml" || ext == "yml") {
        return saveToYAML(filepath, pretty);
    } else if (ext == "json") {
        return saveToJSON(filepath, pretty);
    } else {
        // Legacy format
        return saveToLegacyFile(filepath);
    }
}

bool Config::saveToYAML(const std::string& filepath, bool pretty) const {
#ifdef HAVE_YAML_CPP
    try {
        YAML::Emitter out;
        if (pretty) {
            out.SetIndent(2);
        }
        
        out << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            if (pretty) {
                out << YAML::Comment(entry.description.c_str());
            }
            out << YAML::Key << entry.key;
            
            switch (entry.value.index()) {
                case 0: // int
                    out << std::get<int>(entry.value);
                    break;
                case 1: // int64_t
                    out << std::get<int64_t>(entry.value);
                    break;
                case 2: // double
                    out << std::get<double>(entry.value);
                    break;
                case 3: // bool
                    out << std::get<bool>(entry.value);
                    break;
                case 4: // string
                    out << std::get<std::string>(entry.value);
                    break;
                case 5: // vector<int>
                    out << ConvertToYAML(std::get<std::vector<int>>(entry.value));
                    break;
                case 6: // vector<double>
                    out << ConvertToYAML(std::get<std::vector<double>>(entry.value));
                    break;
                case 7: // vector<string>
                    out << ConvertToYAML(std::get<std::vector<std::string>>(entry.value));
                    break;
            }
        }
        
        out << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << out.c_str();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save YAML file " + filepath + ": " + e.what());
    }
#endif
    return false;
}

bool Config::saveToJSON(const std::string& filepath, bool pretty) const {
#ifdef HAVE_NLOHMANN_JSON
    try {
        nlohmann::json j;
        
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty()) {
                j[entry.key]["__description"] = entry.description;
            }
            
            switch (entry.value.index()) {
                case 0: // int
                    j[entry.key] = std::get<int>(entry.value);
                    break;
                case 1: // int64_t
                    j[entry.key] = std::get<int64_t>(entry.value);
                    break;
                case 2: // double
                    j[entry.key] = std::get<double>(entry.value);
                    break;
                case 3: // bool
                    j[entry.key] = std::get<bool>(entry.value);
                    break;
                case 4: // string
                    j[entry.key] = std::get<std::string>(entry.value);
                    break;
                case 5: // vector<int>
                    j[entry.key] = ConvertToJSON(std::get<std::vector<int>>(entry.value));
                    break;
                case 6: // vector<double>
                    j[entry.key] = ConvertToJSON(std::get<std::vector<double>>(entry.value));
                    break;
                case 7: // vector<string>
                    j[entry.key] = ConvertToJSON(std::get<std::vector<std::string>>(entry.value));
                    break;
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        if (pretty) {
            file << j.dump(2);
        } else {
            file << j.dump();
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save JSON file " + filepath + ": " + e.what());
    }
#endif
    return false;
}

std::string Config::saveToYAMLString(bool pretty) const {
#ifdef HAVE_YAML_CPP
    try {
        YAML::Emitter out;
        if (pretty) {
            out.SetIndent(2);
        }
        
        out << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            if (pretty) {
                out << YAML::Comment(entry.description.c_str());
            }
            out << YAML::Key << entry.key;
            
            switch (entry.value.index()) {
                case 0: // int
                    out << std::get<int>(entry.value);
                    break;
                case 1: // int64_t
                    out << std::get<int64_t>(entry.value);
                    break;
                case 2: // double
                    out << std::get<double>(entry.value);
                    break;
                case 3: // bool
                    out << std::get<bool>(entry.value);
                    break;
                case 4: // string
                    out << std::get<std::string>(entry.value);
                    break;
                case 5: // vector<int>
                    out << ConvertToYAML(std::get<std::vector<int>>(entry.value));
                    break;
                case 6: // vector<double>
                    out << ConvertToYAML(std::get<std::vector<double>>(entry.value));
                    break;
                case 7: // vector<string>
                    out << ConvertToYAML(std::get<std::vector<std::string>>(entry.value));
                    break;
            }
        }
        
        out << YAML::EndMap;
        return out.c_str();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to convert configuration to YAML: " + std::string(e.what()));
    }
#endif
    return "";
}

std::string Config::saveToJSONString(bool pretty) const {
#ifdef HAVE_NLOHMANN_JSON
    try {
        nlohmann::json j;
        
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty()) {
                j[entry.key]["__description"] = entry.description;
            }
            
            switch (entry.value.index()) {
                case 0: // int
                    j[entry.key] = std::get<int>(entry.value);
                    break;
                case 1: // int64_t
                    j[entry.key] = std::get<int64_t>(entry.value);
                    break;
                case 2: // double
                    j[entry.key] = std::get<double>(entry.value);
                    break;
                case 3: // bool
                    j[entry.key] = std::get<bool>(entry.value);
                    break;
                case 4: // string
                    j[entry.key] = std::get<std::string>(entry.value);
                    break;
                case 5: // vector<int>
                    j[entry.key] = ConvertToJSON(std::get<std::vector<int>>(entry.value));
                    break;
                case 6: // vector<double>
                    j[entry.key] = ConvertToJSON(std::get<std::vector<double>>(entry.value));
                    break;
                case 7: // vector<string>
                    j[entry.key] = ConvertToJSON(std::get<std::vector<std::string>>(entry.value));
                    break;
            }
        }
        
        if (pretty) {
            return j.dump(2);
        } else {
            return j.dump();
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to convert configuration to JSON: " + std::string(e.what()));
    }
#endif
    return "{}";
}

bool Config::saveToLegacyFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by NLM Configuration System" << std::endl;
    file << "# Format: key = value" << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            file << "# " << entry.description << std::endl;
        }
        file << entry.key << " = ";
        
        switch (entry.value.index()) {
            case 0: // int
                file << std::get<int>(entry.value);
                break;
            case 1: // int64_t
                file << std::get<int64_t>(entry.value);
                break;
            case 2: // double
                file << std::get<double>(entry.value);
                break;
            case 3: // bool
                file << (std::get<bool>(entry.value) ? "true" : "false");
                break;
            case 4: // string
                file << "\"" << std::get<std::string>(entry.value) << "\"";
                break;
            case 5: // vector<int>
                file << ConvertToLegacy(std::get<std::vector<int>>(entry.value));
                break;
            case 6: // vector<double>
                file << ConvertToLegacy(std::get<std::vector<double>>(entry.value));
                break;
            case 7: // vector<string>
                file << ConvertToLegacy(std::get<std::vector<std::string>>(entry.value));
                break;
        }
        
        file << std::endl << std::endl;
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
        if (pImpl->entries.size() >= pImpl->maxEntries) {
            // Remove oldest entry (simple LRU)
            pImpl->entries.erase(pImpl->entries.begin());
        }
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Run validators if any
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end()) {
        ConfigEntry tempEntry(key, value, source);
        if (!validatorIt->second(tempEntry)) {
            NLM_LOG_WARN("Configuration validation failed for key: " + key);
        }
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setValidated(const std::string& key, const ConfigValue& value, 
                         const std::string& regex, bool required,
                         ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->validationRegex = regex;
        it->required = required;
    } else {
        pImpl->entries.emplace_back(key, value, source, "", regex, required);
    }
    
    // Run validators if any
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end()) {
        ConfigEntry tempEntry(key, value, source, "", regex, required);
        if (!validatorIt->second(tempEntry)) {
            NLM_LOG_WARN("Configuration validation failed for key: " + key);
        }
    }
}

void Config::addValidator(const std::string& key, ConfigValidator validator) {
    pImpl->validators[key] = validator;
}

bool Config::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        if (!entry.isValid()) {
            NLM_LOG_ERROR("Invalid configuration entry: " + entry.key + 
                         " (source: " + entry.source.getSourceName() + ")");
            return false;
        }
        
        auto validatorIt = pImpl->validators.find(entry.key);
        if (validatorIt != pImpl->validators.end()) {
            if (!validatorIt->second(entry)) {
                NLM_LOG_ERROR("Validator failed for configuration entry: " + entry.key);
                return false;
            }
        }
    }
    return true;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

std::string Config::getType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "not found";
    }
    
    return it->getTypeName();
}

bool Config::isValid(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return it->isValid();
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

void Config::remove(const std::vector<std::string>& keys) {
    for (const auto& key : keys) {
        remove(key);
    }
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validators.clear();
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists - if so, update it
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            *it = entry; // Copy the entry
        } else {
            pImpl->entries.push_back(entry);
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

std::vector<ConfigEntry> Config::getEntries() const {
    return pImpl->entries;
}

std::vector<ConfigEntry> Config::getUserDefinedEntries() const {
    std::vector<ConfigEntry> userEntries;
    for (const auto& entry : pImpl->entries) {
        if (entry.source.isUserDefined()) {
            userEntries.push_back(entry);
        }
    }
    return userEntries;
}

std::vector<ConfigEntry> Config::getFileEntries() const {
    std::vector<ConfigEntry> fileEntries;
    for (const auto& entry : pImpl->entries) {
        if (entry.source.type == ConfigSource::Type::File) {
            fileEntries.push_back(entry);
        }
    }
    return fileEntries;
}

std::string Config::summary() const {
    return summary(false);
}

std::string Config::summary(bool includeUserOnly) const {
    std::ostringstream oss;
    oss << "Configuration Statistics:" << std::endl;
    oss << "  Total entries: " << pImpl->entries.size() << std::endl;
    oss << "  User-defined entries: " << getUserDefinedCount() << std::endl;
    oss << "  File entries: " << getFileCount() << std::endl;
    oss << "  Runtime entries: " << getRuntimeCount() << std::endl;
    
    if (includeUserOnly) {
        oss << std::endl << "User-defined entries:" << std::endl;
        for (const auto& entry : getUserDefinedEntries()) {
            oss << "  " << entry.key << " (" << entry.getTypeName() << ") - " 
                << entry.source.getSourceName() << std::endl;
            if (!entry.description.empty()) {
                oss << "    Description: " << entry.description << std::endl;
            }
            oss << "    Valid: " << (entry.isValid() ? "Yes" : "No") << std::endl;
        }
    } else {
        oss << std::endl << "All entries:" << std::endl;
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
            oss << "] (" << entry.source.getSourceName() << ")";
            if (!entry.description.empty()) {
                oss << " - " << entry.description;
            }
            oss << " - Valid: " << (entry.isValid() ? "Yes" : "No") << std::endl;
        }
    }
    
    Statistics stats = getStatistics();
    oss << std::endl << "Validation Score: " << (stats.validationScore * 100.0) << "%" << std::endl;
    
    return oss.str();
}

bool Config::importFromYAML(const std::string& yamlContent, ConfigSource source) {
#ifdef HAVE_YAML_CPP
    try {
        YAML::Node root = YAML::Load(yamlContent);
        for (const auto& it : root) {
            std::string key = it.first.as<std::string>();
            
            // Check for metadata
            if (it.second.IsMap() && it.second["__description"].IsDefined()) {
                std::string description = it.second["__description"].as<std::string>();
                ConfigValue value;
                
                // Convert based on type
                if (it.second.IsScalar()) {
                    if (it.second.IsInt()) {
                        value = it.second.as<int>();
                    } else if (it.second.IsDouble()) {
                        value = it.second.as<double>();
                    } else if (it.second.IsBool()) {
                        value = it.second.as<bool>();
                    } else {
                        value = it.second.as<std::string>();
                    }
                } else {
                    // Complex type (array)
                    std::vector<std::string> strArray;
                    for (const auto& item : it.second) {
                        strArray.push_back(item.as<std::string>());
                    }
                    value = strArray;
                }
                
                setValidated(key, value, "", false, source);
            } else {
                // Simple key-value
                ConfigValue value;
                if (it.second.IsInt()) {
                    value = it.second.as<int>();
                } else if (it.second.IsDouble()) {
                    value = it.second.as<double>();
                } else if (it.second.IsBool()) {
                    value = it.second.as<bool>();
                } else if (it.second.IsScalar()) {
                    value = it.second.as<std::string>();
                } else {
                    // Convert sequence to vector
                    std::vector<double> vec;
                    for (const auto& item : it.second) {
                        vec.push_back(item.as<double>());
                    }
                    value = vec;
                }
                
                set(key, value, source);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML content: " + std::string(e.what()));
    }
#endif
    return false;
}

bool Config::importFromJSON(const std::string& jsonContent, ConfigSource source) {
#ifdef HAVE_NLOHMANN_JSON
    try {
        nlohmann::json j = nlohmann::json::parse(jsonContent);
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Check for metadata
            if (it->is_object() && it->contains("__description")) {
                std::string description = (*it)["__description"].get<std::string>();
                ConfigValue value;
                
                // Convert based on type
                if (it->is_number_integer()) {
                    value = it->get<int>();
                } else if (it->is_number_float()) {
                    value = it->get<double>();
                } else if (it->is_boolean()) {
                    value = it->get<bool>();
                } else if (it->is_string()) {
                    value = it->get<std::string>();
                } else if (it->is_array()) {
                    // Convert array to vector
                    std::vector<double> vec = it->get<std::vector<double>>();
                    value = vec;
                }
                
                setValidated(key, value, "", false, source);
            } else {
                // Simple key-value
                ConfigValue value;
                if (it->is_number_integer()) {
                    value = it->get<int>();
                } else if (it->is_number_float()) {
                    value = it->get<double>();
                } else if (it->is_boolean()) {
                    value = it->get<bool>();
                } else if (it->is_string()) {
                    value = it->get<std::string>();
                } else if (it->is_array()) {
                    std::vector<double> vec = it->get<std::vector<double>>();
                    value = vec;
                }
                
                set(key, value, source);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON content: " + std::string(e.what()));
    }
#endif
    return false;
}

std::vector<std::string> Config::getSources() const {
    std::vector<std::string> sources;
    for (const auto& entry : pImpl->entries) {
        std::string sourceName = entry.source.getSourceName();
        if (std::find(sources.begin(), sources.end(), sourceName) == sources.end()) {
            sources.push_back(sourceName);
        }
    }
    return sources;
}

Config::Statistics Config::getStatistics() const {
    Statistics stats;
    stats.totalEntries = pImpl->entries.size();
    
    std::map<std::string, size_t> typeCounts;
    
    for (const auto& entry : pImpl->entries) {
        std::string typeName = entry.getTypeName();
        typeCounts[typeName]++;
        
        if (entry.source.type == ConfigSource::Type::Runtime) {
            stats.runtimeEntries++;
        } else if (entry.source.type == ConfigSource::Type::File) {
            stats.fileEntries++;
        } else if (entry.source.type == ConfigSource::Type::CommandLine) {
            stats.userEntries++;
        }
    }
    
    stats.typeCounts = typeCounts;
    
    // Calculate validation score
    size_t validCount = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.isValid()) {
            validCount++;
        }
    }
    
    stats.validationScore = pImpl->entries.empty() ? 1.0 : (static_cast<double>(validCount) / pImpl->entries.size());
    
    return stats;
}

bool Config::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        if (!entry.isValid()) {
            NLM_LOG_ERROR("Invalid configuration entry: " + entry.key + 
                         " (source: " + entry.source.getSourceName() + ")");
            return false;
        }
        
        auto validatorIt = pImpl->validators.find(entry.key);
        if (validatorIt != pImpl->validators.end()) {
            if (!validatorIt->second(entry)) {
                NLM_LOG_ERROR("Validator failed for configuration entry: " + entry.key);
                return false;
            }
        }
    }
    return true;
}

bool Config::isValid(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return it->isValid();
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getUserDefinedCount() const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source.isUserDefined()) {
            count++;
        }
    }
    return count;
}

size_t Config::getFileCount() const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source.type == ConfigSource::Type::File) {
            count++;
        }
    }
    return count;
}

size_t Config::getRuntimeCount() const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source.type == ConfigSource::Type::Runtime) {
            count++;
        }
    }
    return count;
}

// Helper functions
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

bool Config::validateString(const std::string& value, const std::string& regex) {
    if (regex.empty()) return true;
    std::regex pattern(regex);
    return std::regex_match(value, pattern);
}

bool Config::parseLine(const std::string& line, std::string& key, std::string& value) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) return false;
    
    key = trim(line.substr(0, pos));
    value = trim(line.substr(pos + 1));
    
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    return !key.empty();
}

std::string Config::escapeYAMLString(const std::string& str) {
    std::string result = str;
    // Escape YAML special characters
    size_t pos = 0;
    while ((pos = result.find_first_of("\\`\"'!@#$%^&*()-_=+[]{}|;<>,.?/~", pos)) != std::string::npos) {
        result.insert(pos, "\\");
        pos += 2;
    }
    return result;
}

std::string Config::escapeJSONString(const std::string& str) {
    // Use nlohmann::json for proper escaping when available
#ifdef HAVE_NLOHMANN_JSON
    return nlohmann::json(str).dump();
#else
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if ('\x00' <= c && c <= '\x1f') {
                    result += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
        }
    }
    return result;
#endif
    }
}

// YAML conversion helpers
YAML::Node Config::ConvertToYAML(const std::vector<int>& vec) {
    YAML::Node node;
    for (int val : vec) {
        node.push_back(val);
    }
    return node;
}

YAML::Node Config::ConvertToYAML(const std::vector<double>& vec) {
    YAML::Node node;
    for (double val : vec) {
        node.push_back(val);
    }
    return node;
}

YAML::Node Config::ConvertToYAML(const std::vector<std::string>& vec) {
    YAML::Node node;
    for (const std::string& val : vec) {
        node.push_back(val);
    }
    return node;
}

// JSON conversion helpers
nlohmann::json Config::ConvertToJSON(const std::vector<int>& vec) {
    return nlohmann::json(vec);
}

nlohmann::json Config::ConvertToJSON(const std::vector<double>& vec) {
    return nlohmann::json(vec);
}

nlohmann::json Config::ConvertToJSON(const std::vector<std::string>& vec) {
    return nlohmann::json(vec);
}

// Legacy conversion helpers
std::string Config::ConvertToLegacy(const std::vector<int>& vec) {
    std::ostringstream oss;
    oss << "["; 
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << vec[i];
    }
    oss << "]";
    return oss.str();
}

std::string Config::ConvertToLegacy(const std::vector<double>& vec) {
    std::ostringstream oss;
    oss << "["; 
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << vec[i];
    }
    oss << "]";
    return oss.str();
}

std::string Config::ConvertToLegacy(const std::vector<std::string>& vec) {
    std::ostringstream oss;
    oss << "["; 
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << vec[i] << "\"";
    }
    oss << "]";
    return oss.str();
}

// Explicit template instantiations
#include "Config.hpp" // Include header for template definitions

// This ensures all template instantiations are properly exported
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
