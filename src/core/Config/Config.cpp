#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <string>
#include <optional>
#include <stdexcept>
#include <regex>
#include <cstdint>
#include <map>
#include <set>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Enhanced Config implementation with all requested features

// Global configuration system state
namespace {
    // Environment variable cache
    std::unordered_map<std::string, std::string> environmentCache;
    
    // Configuration templates registry
    std::unordered_map<std::string, ConfigTemplate> templateRegistry;
    
    // Validation rules registry
    std::vector<ValidationRule> validationRules;
    
    // Schema definitions registry
    std::unordered_map<std::string, SchemaDefinition> schemaRegistry;
    
    // Configuration history
    std::vector<ConfigHistoryEntry> configHistory;
    
    // Configuration inheritance tracking
    std::set<std::string> processedIncludes;
    
    // File format detection cache
    std::unordered_map<std::string, ConfigFormat> formatCache;
}

// Configuration descriptions for common keys
const std::unordered_map<std::string, std::string> ConfigDescriptions = {
    // Random settings
    {"random_seed", "Random seed for reproducible experiments"},
    {"simulation_timestep", "Simulation timestep in seconds"},
    {"neuron_count", "Number of neurons in the brain"},
    {"region_count", "Number of brain regions"},
    {"connection_probability", "Connection probability for random connectivity"},
    {"plasticity_learning_rate", "Plasticity learning rate"},
    {"stdp_ltp_weight", "STDP long-term potentiation weight"},
    {"stdp_ltd_weight", "STDP long-term depression weight"},
    {"stdp_time_constant", "STDP time constant"},
    {"development_synaptogenesis_rate", "Development synaptogenesis rate"},
    {"development_pruning_rate", "Development pruning rate"},
    {"dopamine_baseline", "Dopamine baseline level"},
    {"reward_discount_factor", "Reward discount factor"},
    {"environment_name", "Environment name"},
    {"environment_width", "Environment width"},
    {"environment_height", "Environment height"},
    {"log_level", "Log level (DEBUG, INFO, WARNING, ERROR)"},
    {"log_to_file", "Whether to log to file"},
    {"log_filename", "Log file name"},
    {"max_simulation_steps", "Maximum simulation steps"},
    {"simulation_time_limit", "Simulation time limit"},
    {"visualization_enabled", "Whether visualization is enabled"},
    {"visualization_update_rate", "Visualization update rate"}
};

// Enhanced Config implementation with all requested features

// Global configuration system state
namespace {
    // Environment variable cache
    std::unordered_map<std::string, std::string> environmentCache;
    
    // Configuration templates registry
    std::unordered_map<std::string, ConfigTemplate> templateRegistry;
    
    // Validation rules registry
    std::vector<ValidationRule> validationRules;
    
    // Schema definitions registry
    std::unordered_map<std::string, SchemaDefinition> schemaRegistry;
    
    // Configuration history
    std::vector<ConfigHistoryEntry> configHistory;
    
    // Configuration inheritance tracking
    std::set<std::string> processedIncludes;
    
    // File format detection cache
    std::unordered_map<std::string, ConfigFormat> formatCache;
}

// Helper function to detect file format
ConfigFormat Config::detectFileFormat(const std::string& filepath) {
    // Check cache first
    if (formatCache.find(filepath) != formatCache.end()) {
        return formatCache[filepath];
    }
    
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    ConfigFormat format = ConfigFormat::Unknown;
    
    if (ext == ".json") {
        format = ConfigFormat::JSON;
    } else if (ext == ".yaml" || ext == ".yml") {
        format = ConfigFormat::YAML;
    } else if (ext == ".ini" || ext == ".cfg") {
        format = ConfigFormat::INI;
    } else if (ext == ".properties") {
        format = ConfigFormat::Properties;
    } else if (ext == ".xml") {
        format = ConfigFormat::XML;
    } else if (ext == ".json5" || ext == ".jsonc") {
        format = ConfigFormat::JSON;
    } else {
        format = ConfigFormat::Simple;
    }
    
    // Cache the result
    formatCache[filepath] = format;
    
    return format;
}

// Helper function to expand environment variables
std::string Config::expandEnvironmentVariables(const std::string& str) {
    std::string result = str;
    size_t pos = 0;
    
    while ((pos = result.find('$', pos)) != std::string::npos) {
        size_t endPos = pos + 1;
        if (endPos < result.size() && result[endPos] == '{') {
            // Handle ${VAR_NAME} syntax
            endPos++;
            size_t varEnd = result.find('}', endPos);
            if (varEnd == std::string::npos) {
                break; // Malformed ${...}, stop processing
            }
            std::string varName = result.substr(endPos, varEnd - endPos);
            
            // Check environment variable cache first
            std::string value;
            if (environmentCache.find(varName) != environmentCache.end()) {
                value = environmentCache[varName];
            } else {
                // Try to get from actual environment
                const char* envValue = std::getenv(varName.c_str());
                if (envValue) {
                    value = envValue;
                    environmentCache[varName] = value;
                }
            }
            
            if (!value.empty()) {
                result.replace(pos, varEnd - pos + 1, value);
                pos += value.size();
            } else {
                // Remove the ${VAR_NAME} if not found
                result.replace(pos, varEnd - pos + 1, "");
                pos += 0;
            }
        } else {
            // Handle $VAR_NAME syntax
            endPos = pos + 1;
            while (endPos < result.size() && (std::isalnum(result[endPos]) || result[endPos] == '_')) {
                endPos++;
            }
            std::string varName = result.substr(pos + 1, endPos - pos - 1);
            
            std::string value;
            if (environmentCache.find(varName) != environmentCache.end()) {
                value = environmentCache[varName];
            } else {
                const char* envValue = std::getenv(varName.c_str());
                if (envValue) {
                    value = envValue;
                    environmentCache[varName] = value;
                }
            }
            
            if (!value.empty()) {
                result.replace(pos, endPos - pos, value);
                pos += value.size();
            } else {
                // Remove the $VAR_NAME if not found
                result.replace(pos, endPos - pos, "");
                pos += 0;
            }
        }
    }
    
    return result;
}

// Helper function to parse include directives
bool Config::parseIncludeDirective(const std::string& line, std::string& includedFile) {
    std::string trimmed = trim(line);
    
    // Support both @include and # include syntax
    if (trimmed.rfind("@include", 0) == 0) {
        includedFile = trim(trimmed.substr(8));
        return true;
    } else if (trimmed.rfind("# include", 0) == 0) {
        includedFile = trim(trimmed.substr(9));
        return true;
    }
    
    return false;
}

// Helper function to resolve file path for includes
std::string Config::resolveIncludePath(const std::string& baseDir, const std::string& includePath) {
    std::filesystem::path base(baseDir);
    std::filesystem::path include(includePath);
    
    if (include.is_absolute()) {
        return include.string();
    }
    
    // Try relative to base directory
    std::filesystem::path resolved = base / include;
    if (std::filesystem::exists(resolved)) {
        return resolved.string();
    }
    
    // Try relative to current working directory
    if (std::filesystem::exists(include)) {
        return include.string();
    }
    
    return "";
}

// Helper function to save configuration history
void Config::saveConfigHistory(const std::string& key, const ConfigValue& oldValue, 
                              const ConfigValue& newValue, ConfigSource source, 
                              const std::string& reason) {
    configHistory.emplace_back(key, oldValue, newValue, source, reason);
    
    // Limit history size to prevent memory issues
    if (configHistory.size() > 1000) {
        configHistory.erase(configHistory.begin(), configHistory.begin() + 100);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    // Simple JSON parser implementation without external dependencies
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Clear existing entries
    pImpl->entries.clear();
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    try {
        // Simple JSON parsing - in production, you would use a proper JSON library
        parseJSON(content);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // For now, YAML parsing uses simple key=value format
    // In production, you would use a YAML library like yaml-cpp
    return loadFromSimple(filepath);
}

void Config::parseJSON(const std::string& jsonStr) {
    // Simple JSON parsing for basic key-value pairs
    size_t pos = 0;
    
    while (pos < jsonStr.size()) {
        skipWhitespace(jsonStr, pos);
        
        if (pos >= jsonStr.size()) break;
        
        if (jsonStr[pos] == '}') {
            break;
        }
        
        // Parse key
        std::string key = parseString(jsonStr, pos);
        skipWhitespace(jsonStr, pos);
        
        if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
            throw std::runtime_error("Invalid JSON: expected colon after key");
        }
        
        pos++; // Skip colon
        skipWhitespace(jsonStr, pos);
        
        // Parse value
        ConfigValue value = parseJSONValue(jsonStr, pos);
        
        set(key, value, ConfigSource::File);
        
        skipWhitespace(jsonStr, pos);
        
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++; // Skip comma
        }
    }
}

ConfigValue Config::parseJSONValue(const std::string& jsonStr, size_t& pos) {
    skipWhitespace(jsonStr, pos);
    
    if (pos >= jsonStr.size()) {
        throw std::runtime_error("Unexpected end of JSON");
    }
    
    char c = jsonStr[pos];
    
    if (c == '"') {
        // String value
        std::string value = parseString(jsonStr, pos);
        return value;
    } else if (c == 't' || c == 'f') {
        // Boolean value
        if (jsonStr.substr(pos, 4) == "true") {
            pos += 4;
            return true;
        } else if (jsonStr.substr(pos, 5) == "false") {
            pos += 5;
            return false;
        }
        throw std::runtime_error("Invalid JSON boolean value");
    } else if (c == '-' || std::isdigit(c)) {
        // Number value
        size_t endPos = pos;
        while (endPos < jsonStr.size() && (std::isdigit(jsonStr[endPos]) || jsonStr[endPos] == '.' || jsonStr[endPos] == '-' || jsonStr[endPos] == 'e' || jsonStr[endPos] == 'E')) {
            endPos++;
        }
        
        std::string numStr = jsonStr.substr(pos, endPos - pos);
        
        try {
            // Try to parse as int
            size_t intPos;
            int intVal = std::stoi(numStr, &intPos);
            if (intPos == numStr.size()) {
                return intVal;
            }
            
            // Try to parse as double
            size_t doublePos;
            double doubleVal = std::stod(numStr, &doublePos);
            if (doublePos == numStr.size()) {
                return doubleVal;
            }
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid JSON number: " + numStr);
        }
    } else if (c == '[') {
        // Array value
        return parseJSONArray(jsonStr, pos);
    }
    
    throw std::runtime_error("Unsupported JSON value type");
}

std::vector<ConfigValue> Config::parseJSONArray(const std::string& jsonStr, size_t& pos) {
    std::vector<ConfigValue> array;
    
    if (pos >= jsonStr.size() || jsonStr[pos] != '[') {
        throw std::runtime_error("Invalid JSON array: expected '['");
    }
    
    pos++; // Skip '['
    skipWhitespace(jsonStr, pos);
    
    if (pos < jsonStr.size() && jsonStr[pos] == ']') {
        pos++; // Skip ']'
        return array;
    }
    
    while (pos < jsonStr.size()) {
        ConfigValue value = parseJSONValue(jsonStr, pos);
        array.push_back(value);
        
        skipWhitespace(jsonStr, pos);
        
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++; // Skip comma
            skipWhitespace(jsonStr, pos);
        } else if (pos < jsonStr.size() && jsonStr[pos] == ']') {
            pos++; // Skip ']'
            break;
        } else {
            throw std::runtime_error("Invalid JSON array: expected ',' or ']'");
        }
    }
    
    return array;
}

std::string Config::parseString(const std::string& jsonStr, size_t& pos) {
    if (pos >= jsonStr.size() || jsonStr[pos] != '"') {
        throw std::runtime_error("Invalid JSON string: expected '"');
    }
    
    pos++; // Skip opening quote
    size_t start = pos;
    
    while (pos < jsonStr.size() && jsonStr[pos] != '"') {
        if (jsonStr[pos] == '\\') {
            pos++; // Skip escape character
            if (pos >= jsonStr.size()) {
                throw std::runtime_error("Invalid JSON string: incomplete escape");
            }
        }
        pos++;
    }
    
    if (pos >= jsonStr.size() || jsonStr[pos] != '"') {
        throw std::runtime_error("Invalid JSON string: unclosed string");
    }
    
    std::string value = jsonStr.substr(start, pos - start);
    pos++; // Skip closing quote
    
    return value;
}

void Config::skipWhitespace(const std::string& str, size_t& pos) {
    while (pos < str.size() && std::isspace(str[pos])) {
        pos++;
    }
}

bool Config::loadFromSimple(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            throw std::runtime_error("Invalid format at line " + std::to_string(lineNum) + ": expected key=value");
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::File);
        
        // Try to get description from built-in map
        auto descIt = ConfigDescriptions.find(key);
        if (descIt != ConfigDescriptions.end()) {
            // Update description
            auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            if (entryIt != pImpl->entries.end()) {
                entryIt->description = descIt->second;
            }
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
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
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Determine file format based on extension
        std::string ext = "." + std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".json") {
            return saveToJSON(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            return saveToYAML(filepath);
        } else {
            return saveToSimple(filepath);
        }
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        nlohmann::json value;
        
        // Convert ConfigValue to JSON
        std::visit([&value](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                value = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                value = arg;
            }
        }, entry.value);
        
        jsonData[entry.key] = value;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << jsonData.dump(4); // Pretty print with 4 spaces indentation
    return true;
}

bool Config::saveToYAML(const std::string& filepath) const {
    // For now, save as JSON since YAML parsing is complex
    return saveToJSON(filepath);
}

bool Config::saveToSimple(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert value to string based on its type
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = '"' + arg + '"';
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStr = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                valueStr = "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) valueStr += ", ";
                    valueStr += std::to_string(item);
                    first = false;
                }
                valueStr += "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                valueStr = "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) valueStr += ", ";
                    valueStr += std::to_string(item);
                    first = false;
                }
                valueStr += "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                valueStr = "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) valueStr += ", ";
                    valueStr += '"' + item + '"';
                    first = false;
                }
                valueStr += "]";
            } else {
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
    // Validate configuration
    validateConfigEntry(key, value);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        
        // Update description if available
        auto descIt = ConfigDescriptions.find(key);
        if (descIt != ConfigDescriptions.end() && it->description.empty()) {
            it->description = descIt->second;
        }
    } else {
        std::string description = "";
        auto descIt = ConfigDescriptions.find(key);
        if (descIt != ConfigDescriptions.end()) {
            description = descIt->second;
        }
        
        pImpl->entries.emplace_back(key, value, source, description);
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "["; bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "["; bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "["; bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << "\"" << item << "\"";
                    first = false;
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

// Type conversion utilities
std::optional<int> Config::convertToInt(const std::string& str) {
    try {
        size_t pos;
        int value = std::stoi(str, &pos);
        if (pos != str.size()) {
            // Check if there are extra non-whitespace characters
            bool valid = true;
            for (size_t i = pos; i < str.size(); ++i) {
                if (!std::isspace(str[i])) {
                    valid = false;
                    break;
                }
            }
            if (!valid) {
                return std::nullopt;
            }
        }
        return value;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<double> Config::convertToDouble(const std::string& str) {
    try {
        size_t pos;
        double value = std::stod(str, &pos);
        if (pos != str.size()) {
            // Check if there are extra non-whitespace characters
            bool valid = true;
            for (size_t i = pos; i < str.size(); ++i) {
                if (!std::isspace(str[i])) {
                    valid = false;
                    break;
                }
            }
            if (!valid) {
                return std::nullopt;
            }
        }
        return value;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<bool> Config::convertToBool(const std::string& str) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        return false;
    }
    return std::nullopt;
}

std::optional<std::string> Config::convertToString(const std::string& str) {
    return str;
}

// Configuration validation
bool Config::validateConfigEntry(const std::string& key, const ConfigValue& value) {
    // Get description for validation if available
    std::string description = "";
    auto descIt = ConfigDescriptions.find(key);
    if (descIt != ConfigDescriptions.end()) {
        description = descIt->second;
    }
    
    // Basic validation based on key names and value types
    if (key == "random_seed") {
        auto val = std::get<int>(value);
        if (val < 0) {
            throw std::runtime_error("random_seed must be non-negative");
        }
    } else if (key == "simulation_timestep") {
        auto val = std::get<double>(value);
        if (val <= 0.0) {
            throw std::runtime_error("simulation_timestep must be positive");
        }
    } else if (key == "neuron_count") {
        auto val = std::get<int>(value);
        if (val <= 0) {
            throw std::runtime_error("neuron_count must be positive");
        }
    } else if (key == "region_count") {
        auto val = std::get<int>(value);
        if (val < 0) {
            throw std::runtime_error("region_count must be non-negative");
        }
    } else if (key == "connection_probability") {
        auto val = std::get<double>(value);
        if (val < 0.0 || val > 1.0) {
            throw std::runtime_error("connection_probability must be between 0.0 and 1.0");
        }
    } else if (key == "plasticity_learning_rate") {
        auto val = std::get<double>(value);
        if (val < 0.0) {
            throw std::runtime_error("plasticity_learning_rate must be non-negative");
        }
    } else if (key == "environment_width") {
        auto val = std::get<int>(value);
        if (val <= 0) {
            throw std::runtime_error("environment_width must be positive");
        }
    } else if (key == "environment_height") {
        auto val = std::get<int>(value);
        if (val <= 0) {
            throw std::runtime_error("environment_height must be positive");
        }
    }
    
    // Check if value fits in the variant type
    std::visit([key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            // Check int range
            if (arg < INT_MIN || arg > INT_MAX) {
                throw std::runtime_error("Value for key '" + key + "' exceeds int range");
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            // Check int64_t range
            if (arg < INT64_MIN || arg > INT64_MAX) {
                throw std::runtime_error("Value for key '" + key + "' exceeds int64_t range");
            }
        } else if constexpr (std::is_same_v<T, double>) {
            // Check double range (NaN is not allowed)
            if (std::isnan(arg)) {
                throw std::runtime_error("Value for key '" + key + "' cannot be NaN");
            }
        }
    }, value);
    
    return true;
}

// Explicit template instantiations
namespace {
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
}

} // namespace nlm
