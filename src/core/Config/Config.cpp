#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// Helper function to check if a file exists and is readable
bool fileExistsAndReadable(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

// Helper function to validate configuration key format
bool isValidKey(const std::string& key) {
    if (key.empty()) return false;
    // Allow alphanumeric characters, underscores, and hyphens
    return std::all_of(key.begin(), key.end(),
        [](char c) { return std::isalnum(c) || c == '_' || c == '-'; });
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (!fileExistsAndReadable(filepath)) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Loading configuration from: " + filepath);
    
    // Try JSON format first
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return loadFromJSON(filepath);
    }
    // Try YAML format
    else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
        return loadFromYAML(filepath);
    }
    // Try YAML format with .yml extension
    else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 4) == ".yml") {
        return loadFromYAML(filepath);
    }
    // Fall back to simple key=value format
    else {
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open JSON configuration file: " + filepath);
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse JSON data
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            if (!isValidKey(key)) {
                NLM_LOG_WARNING("Invalid configuration key skipped: " + key);
                continue;
            }
            
            try {
                setFromJSONValue(key, *it, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error parsing JSON key '" + key + "': " + e.what());
            }
        }
        
        NLM_LOG_INFO("Successfully loaded " + std::to_string(pImpl->entries.size()) + 
                    " entries from JSON configuration file.");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON configuration file '" + filepath + 
                     "': " + e.what());
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse YAML data
        for (const auto& node : yamlData) {
            if (!node.first.IsScalar()) continue;
            
            std::string key = node.first.as<std::string>();
            if (!isValidKey(key)) {
                NLM_LOG_WARNING("Invalid configuration key skipped: " + key);
                continue;
            }
            
            try {
                setFromYAMLValue(key, node.second, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error parsing YAML key '" + key + "': " + e.what());
            }
        }
        
        NLM_LOG_INFO("Successfully loaded " + std::to_string(pImpl->entries.size()) + 
                    " entries from YAML configuration file.");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML configuration file '" + filepath + 
                     "': " + e.what());
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Cannot open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    int validEntries = 0;
    
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
            
            if (isValidKey(key)) {
                set(key, value, ConfigSource::File);
                validEntries++;
            } else {
                NLM_LOG_WARNING("Invalid configuration key at line " + std::to_string(lineNumber) + ": " + key);
            }
        } else {
            NLM_LOG_WARNING("Invalid format at line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Successfully loaded " + std::to_string(validEntries) + 
                " valid entries from " + std::to_string(lineNumber) + 
                " lines in simple configuration file.");
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv) {
        NLM_LOG_ERROR("Invalid command line arguments: argv is null");
        return false;
    }
    
    int validArgs = 0;
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            NLM_LOG_WARNING("Skipping null command line argument at position " + std::to_string(i));
            continue;
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (isValidKey(key)) {
                    set(key, value, ConfigSource::CommandLine);
                    validArgs++;
                } else {
                    NLM_LOG_ERROR("Invalid configuration key from command line: " + key);
                }
            } else {
                NLM_LOG_WARNING("Invalid command line argument format (missing '='): " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            if (i + 1 < argc && argv[i + 1]) {
                std::string value = argv[++i];
                
                if (isValidKey(key)) {
                    set(key, value, ConfigSource::CommandLine);
                    validArgs++;
                } else {
                    NLM_LOG_ERROR("Invalid configuration key from command line: " + key);
                }
            } else {
                NLM_LOG_WARNING("Missing value for command line argument: " + arg);
            }
        } else {
            NLM_LOG_WARNING("Ignoring unrecognized command line argument: " + arg);
        }
    }
    
    NLM_LOG_INFO("Processed " + std::to_string(validArgs) + " valid command line arguments.");
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (pImpl->entries.empty()) {
        NLM_LOG_WARNING("No configuration entries to save.");
        return false;
    }
    
    try {
        // Determine file format based on extension
        std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
        
        if (extension == "json" || extension == "JSON") {
            return saveToJSON(filepath);
        } else if (extension == "yaml" || extension == "yml" || extension == "YAML" || extension == "YML") {
            return saveToYAML(filepath);
        } else {
            // Default to simple format
            return saveToSimpleFormat(filepath);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save configuration to file '" + filepath + 
                     "': " + e.what());
        return false;
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = toJSONValue(entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot create JSON configuration file: " + filepath);
            return false;
        }
        
        file << jsonData.dump(2); // Pretty print with 2-space indentation
        NLM_LOG_INFO("Successfully saved " + std::to_string(pImpl->entries.size()) + 
                    " entries to JSON configuration file.");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to write JSON configuration to file '" + filepath + 
                     "': " + e.what());
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        emitter.SetStyle(YAML::EmitterStyle::Block); // Indented style
        
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            emitter << YAML::Value << toYAMLValue(entry.value);
        }
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot create YAML configuration file: " + filepath);
            return false;
        }
        
        file << emitter.c_str();
        NLM_LOG_INFO("Successfully saved " + std::to_string(pImpl->entries.size()) + 
                    " entries to YAML configuration file.");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to write YAML configuration to file '" + filepath + 
                     "': " + e.what());
        return false;
    }
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot create configuration file: " + filepath);
        return false;
    }
    
    int savedEntries = 0;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << toSimpleFormatValue(entry.value) << "\n\n";
        savedEntries++;
    }
    
    NLM_LOG_INFO("Successfully saved " + std::to_string(savedEntries) + 
                " entries to simple configuration file.");
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (!isValidKey(key)) {
        NLM_LOG_ERROR("Invalid configuration key requested: " + key);
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Configuration key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Type mismatch for configuration key: " + key + ". Expected " + 
                     typeid(T).name() + ", actual: " + variantTypeToString(it->value));
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        NLM_LOG_DEBUG("Configuration key not found, using default for: " + key);
        return defaultValue;
    }
    return val.value();
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!isValidKey(key)) {
        NLM_LOG_ERROR("Cannot set invalid configuration key: " + key);
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated configuration key: " + key + " (source: " + 
                     configSourceToString(source) + ")");
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added new configuration key: " + key + " (source: " + 
                     configSourceToString(source) + ")");
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
    if (!isValidKey(key)) {
        NLM_LOG_ERROR("Invalid configuration key checked: " + key);
        return false;
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    if (!isValidKey(key)) {
        NLM_LOG_ERROR("Cannot remove invalid configuration key: " + key);
        return;
    }
    
    size_t erased = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (erased > 0) {
        pImpl->entries.erase(pImpl->entries.end() - erased, pImpl->entries.end());
        NLM_LOG_DEBUG("Removed configuration key: " + key);
    } else {
        NLM_LOG_DEBUG("Configuration key not found for removal: " + key);
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
    if (pImpl->entries.empty()) {
        NLM_LOG_DEBUG("Configuration already empty, nothing to clear.");
        return;
    }
    
    NLM_LOG_INFO("Clearing " + std::to_string(pImpl->entries.size()) + " configuration entries.");
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << std::fixed << std::setprecision(3) << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
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

// Private helper methods
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

void Config::setFromJSONValue(const std::string& key, const nlohmann::json& value, ConfigSource source) {
    if (value.is_boolean()) {
        set(key, value.get<bool>(), source);
    } else if (value.is_number_integer()) {
        set(key, value.get<int64_t>(), source);
    } else if (value.is_number_float()) {
        set(key, value.get<double>(), source);
    } else if (value.is_string()) {
        set(key, value.get<std::string>(), source);
    } else if (value.is_array()) {
        // Determine array type
        if (!value.empty()) {
            if (std::all_of(value.begin(), value.end(), [](const auto& v) { return v.is_number_integer(); })) {
                std::vector<int64_t> intArray;
                for (const auto& v : value) {
                    intArray.push_back(v.get<int64_t>());
                }
                set(key, intArray, source);
            } else if (std::all_of(value.begin(), value.end(), [](const auto& v) { return v.is_number_float(); })) {
                std::vector<double> doubleArray;
                for (const auto& v : value) {
                    doubleArray.push_back(v.get<double>());
                }
                set(key, doubleArray, source);
            } else if (std::all_of(value.begin(), value.end(), [](const auto& v) { return v.is_string(); })) {
                std::vector<std::string> stringArray;
                for (const auto& v : value) {
                    stringArray.push_back(v.get<std::string>());
                }
                set(key, stringArray, source);
            } else {
                // Mixed type array, store as strings
                std::vector<std::string> stringArray;
                for (const auto& v : value) {
                    stringArray.push_back(v.dump());
                }
                set(key, stringArray, source);
            }
        } else {
            set(key, std::vector<int64_t>(), source);
        }
    } else {
        // Unknown type, store as string
        set(key, value.dump(), source);
    }
}

void Config::setFromYAMLValue(const std::string& key, const YAML::Node& value, ConfigSource source) {
    if (value.IsScalar()) {
        // Try different types in order of preference
        if (value.IsBoolean()) {
            set(key, value.as<bool>(), source);
        } else if (value.IsInteger()) {
            set(key, value.as<int64_t>(), source);
        } else if (value.IsDouble()) {
            set(key, value.as<double>(), source);
        } else {
            // String
            set(key, value.as<std::string>(), source);
        }
    } else if (value.IsSequence()) {
        // Array
        std::vector<int64_t> intArray;
        std::vector<double> doubleArray;
        std::vector<std::string> stringArray;
        
        bool allInt = true;
        bool allDouble = true;
        bool allString = true;
        
        for (const auto& item : value) {
            if (item.IsInteger()) {
                intArray.push_back(item.as<int64_t>());
                allString = false;
            } else if (item.IsDouble()) {
                doubleArray.push_back(item.as<double>());
                allInt = false;
                allString = false;
            } else if (item.IsScalar()) {
                stringArray.push_back(item.as<std::string>());
                allInt = false;
                allDouble = false;
            } else {
                allInt = false;
                allDouble = false;
                allString = false;
            }
        }
        
        if (allInt && !intArray.empty()) {
            set(key, intArray, source);
        } else if (allDouble && !doubleArray.empty()) {
            set(key, doubleArray, source);
        } else if (allString && !stringArray.empty()) {
            set(key, stringArray, source);
        } else if (!stringArray.empty()) {
            set(key, stringArray, source);
        } else {
            set(key, std::vector<int64_t>(), source);
        }
    } else {
        // Map or other complex type, store as string
        set(key, "YAML complex type", source);
    }
}

nlohmann::json Config::toJSONValue(const ConfigValue& value) {
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
            nlohmann::json jsonArray = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonArray.push_back(item);
            }
            return jsonArray;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json jsonArray = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonArray.push_back(item);
            }
            return jsonArray;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json jsonArray = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonArray.push_back(item);
            }
            return jsonArray;
        } else {
            // Unknown type, return empty string
            return "";
        }
    }, value);
}

YAML::Node Config::toYAMLValue(const ConfigValue& value) {
    YAML::Emitter emitter;
    emitter.SetStyle(YAML::EmitterStyle::Block); // Indented style
    
    emitter << YAML::BeginMap;
    
    std::visit([&emitter](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            emitter << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            emitter << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            emitter << arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            emitter << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            emitter << YAML::Flow;
            emitter << YAML::BeginSeq;
            for (const auto& item : arg) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            emitter << YAML::Flow;
            emitter << YAML::BeginSeq;
            for (const auto& item : arg) {
                emitter << std::fixed << std::setprecision(6) << item;
            }
            emitter << YAML::EndSeq;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            emitter << YAML::Flow;
            emitter << YAML::BeginSeq;
            for (const auto& item : arg) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else {
            // Unknown type
            emitter << "unknown";
        }
    }, value);
    
    emitter << YAML::EndMap;
    
    // Convert emitter content to YAML::Node
    std::stringstream stream;
    stream << emitter.c_str();
    
    YAML::Node node = YAML::Load(stream.str());
    return node;
}

std::string Config::toSimpleFormatValue(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
            return result;
        } else {
            // Primitive types
            std::ostringstream oss;
            oss << arg;
            return oss.str();
        }
    }, value);
}

std::string Config::variantTypeToString(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
        else return "unknown";
    }, value);
}

std::string Config::configSourceToString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        default: return "Unknown";
    }
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

// Note: nlohmann/json and yaml-cpp headers need to be added to CMakeLists.txt
// For now, this code is provided as an example of what could be implemented
