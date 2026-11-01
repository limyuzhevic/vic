#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON/YAML parser with validation
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    // Try to parse as JSON first
    if (parseJSONFileWithValidation(filepath)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format
    file.clear();
    file.seekg(0);
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with better error handling
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Invalid configuration at line " + std::to_string(lineNumber) + 
                               " in " + filepath + ": " + std::string(e.what()));
            }
        } else {
            NLM_LOG_WARNING("Malformed configuration line " + std::to_string(lineNumber) + 
                           " in " + filepath + ": no '=' found");
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from text file: " + filepath);
    return true;
}

bool Config::parseJSONFileWithValidation(const std::string& filepath) {
    // Simple JSON parser implementation with validation
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Parse JSON using a simple recursive approach with validation
    size_t pos = 0;
    return parseJSONObjectWithValidation(content, pos);
}

bool Config::parseJSONObjectWithValidation(const std::string& json, size_t& pos) {
    // Parse JSON object with validation
    // Skip whitespace
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' || 
                                 json[pos] == '\t' || json[pos] == '\r')) {
        pos++;
    }
    
    if (pos >= json.size() || json[pos] != '{') {
        return false; // Not a JSON object
    }
    
    pos++; // Skip opening brace
    
    // Parse key-value pairs
    bool firstPair = true;
    while (pos < json.size() && json[pos] != '}') {
        if (!firstPair) {
            if (json[pos] == ',') {
                pos++;
            } else {
                NLM_LOG_ERROR("Expected comma between JSON key-value pairs");
                return false;
            }
        }
        
        // Parse key
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' || 
                                     json[pos] == '\t' || json[pos] == '\r')) {
            pos++;
        }
        
        if (pos >= json.size() || json[pos] != '"') {
            NLM_LOG_ERROR("Expected key string in JSON object");
            return false;
        }
        
        pos++; // Skip opening quote
        size_t keyStart = pos;
        while (pos < json.size() && json[pos] != '"') {
            if (json[pos] == '\\' && pos + 1 < json.size()) {
                pos++; // Skip escaped character
            }
            pos++;
        }
        
        if (pos >= json.size()) {
            NLM_LOG_ERROR("Unclosed string in JSON object");
            return false;
        }
        
        std::string key = json.substr(keyStart, pos - keyStart);
        pos++; // Skip closing quote
        
        // Skip whitespace after key
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' || 
                                     json[pos] == '\t' || json[pos] == '\r')) {
            pos++;
        }
        
        if (pos >= json.size() || json[pos] != ':') {
            NLM_LOG_ERROR("Expected colon after key in JSON object");
            return false;
        }
        
        pos++; // Skip colon
        
        // Skip whitespace after colon
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' || 
                                     json[pos] == '\t' || json[pos] == '\r')) {
            pos++;
        }
        
        // Parse value
        ConfigValue value;
        bool parsed = false;
        
        // Try different value types
        if (parseJSONValueWithValidation(json, pos, value)) {
            parsed = true;
        }
        
        if (!parsed) {
            NLM_LOG_ERROR("Failed to parse JSON value");
            return false;
        }
        
        // Store the key-value pair
        set(key, value, ConfigSource::File);
        
        // Skip whitespace before next comma or closing brace
        firstPair = false;
    }
    
    if (pos >= json.size() || json[pos] != '}') {
        NLM_LOG_ERROR("Expected closing brace in JSON object");
        return false;
    }
    
    pos++; // Skip closing brace
    return true;
}

bool Config::parseJSONValueWithValidation(const std::string& json, size_t& pos, ConfigValue& value) {
    // Parse JSON value with validation
    // Skip whitespace
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' || 
                                 json[pos] == '\t' || json[pos] == '\r')) {
        pos++;
    }
    
    if (pos >= json.size()) {
        return false;
    }
    
    char c = json[pos];
    
    switch (c) {
        case '\"': {
            // String value
            pos++; // Skip opening quote
            size_t start = pos;
            while (pos < json.size() && json[pos] != '\"') {
                if (json[pos] == '\\' && pos + 1 < json.size()) {
                    pos++; // Skip escaped character
                }
                pos++;
            }
            
            if (pos >= json.size()) {
                NLM_LOG_ERROR("Unclosed string in JSON");
                return false;
            }
            
            value = json.substr(start, pos - start);
            pos++; // Skip closing quote
            return true;
        }
        
        case '{': {
            // Nested object - validate and store as string
            std::string objContent;
            int braceCount = 1;
            pos++;
            objContent += '{';
            
            while (pos < json.size() && braceCount > 0) {
                if (json[pos] == '{') {
                    braceCount++;
                } else if (json[pos] == '}') {
                    braceCount--;
                }
                objContent += json[pos];
                pos++;
            }
            
            // Validate object structure (basic check)
            if (braceCount != 0) {
                NLM_LOG_ERROR("Unbalanced braces in JSON object");
                return false;
            }
            
            value = objContent;
            return true;
        }
        
        case '[': {
            // Array - validate structure and store as string
            std::string arrayContent;
            int bracketCount = 1;
            pos++;
            arrayContent += '[';
            
            while (pos < json.size() && bracketCount > 0) {
                if (json[pos] == '[') {
                    bracketCount++;
                } else if (json[pos] == ']') {
                    bracketCount--;
                }
                arrayContent += json[pos];
                pos++;
            }
            
            // Validate array structure (basic check)
            if (bracketCount != 0) {
                NLM_LOG_ERROR("Unbalanced brackets in JSON array");
                return false;
            }
            
            value = arrayContent;
            return true;
        }
        
        case 't': {
            // true
            if (json.compare(pos, 4, "true") == 0) {
                value = true;
                pos += 4;
                return true;
            }
            break;
        }
        
        case 'f': {
            // false
            if (json.compare(pos, 5, "false") == 0) {
                value = false;
                pos += 5;
                return true;
            }
            break;
        }
        
        case 'n': {
            // null
            if (json.compare(pos, 4, "null") == 0) {
                value = std::string("null");
                pos += 4;
                return true;
            }
            break;
        }
        
        default: {
            // Number (integer or float)
            size_t start = pos;
            bool hasDecimal = false;
            bool hasExponent = false;
            
            while (pos < json.size() && (std::isdigit(json[pos]) || 
                                         json[pos] == '.' || json[pos] == '-' ||
                                         json[pos] == '+' || json[pos] == 'e' || json[pos] == 'E')) {
                if (json[pos] == '.') {
                    if (hasDecimal || hasExponent) {
                        break;
                    }
                    hasDecimal = true;
                } else if (json[pos] == 'e' || json[pos] == 'E') {
                    if (hasExponent) {
                        break;
                    }
                    hasExponent = true;
                }
                pos++;
            }
            
            if (pos == start) {
                NLM_LOG_ERROR("No valid number found in JSON");
                return false;
            }
            
            std::string numStr = json.substr(start, pos - start);
            
            // Try to parse as integer first
            try {
                size_t endPos;
                long long intVal = std::stoll(numStr, &endPos);
                if (endPos == numStr.size()) {
                    value = intVal;
                    return true;
                }
            } catch (...) {}
            
            // Try to parse as unsigned long long
            try {
                size_t endPos;
                unsigned long long uintVal = std::stoull(numStr, &endPos);
                if (endPos == numStr.size()) {
                    value = uintVal;
                    return true;
                }
            } catch (...) {}
            
            // Try to parse as double
            try {
                size_t endPos;
                double doubleVal = std::stod(numStr, &endPos);
                if (endPos == numStr.size()) {
                    value = doubleVal;
                    return true;
                }
            } catch (...) {}
            
            NLM_LOG_ERROR("Invalid number format in JSON: " + numStr);
            return false;
        }
    }
    
    NLM_LOG_ERROR("Unexpected character in JSON: " + std::string(1, c));
    return false;
}

// Validate configuration value based on key
void Config::validateConfigValue(const std::string& key, const ConfigValue& value) {
    // Validate known configuration keys
    std::string lowerKey = toLower(key);
    
    if (lowerKey == "random_seed") {
        if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
            NLM_LOG_WARNING("random_seed should be an integer value");
        }
        int64_t seed = std::visit([](auto&& v) { return v; }, value);
        if (seed < 0) {
            NLM_LOG_WARNING("random_seed should be non-negative");
        }
    } else if (lowerKey == "simulation_timestep" || lowerKey == "stdp_tau") {
        if (!std::holds_alternative<double>(value) && !std::holds_alternative<float>(value)) {
            NLM_LOG_WARNING("%s should be a floating-point value", key.c_str());
        }
        double timestep = std::visit([](auto&& v) { return static_cast<double>(v); }, value);
        if (timestep <= 0.0) {
            NLM_LOG_WARNING("%s should be positive", key.c_str());
        }
    } else if (lowerKey == "neuron_count" || lowerKey == "region_count") {
        if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
            NLM_LOG_WARNING("%s should be an integer value", key.c_str());
        }
        int64_t count = std::visit([](auto&& v) { return v; }, value);
        if (count <= 0) {
            NLM_LOG_WARNING("%s should be positive", key.c_str());
        }
    } else if (lowerKey == "connection_probability") {
        if (!std::holds_alternative<double>(value) && !std::holds_alternative<float>(value)) {
            NLM_LOG_WARNING("connection_probability should be a floating-point value");
        }
        double prob = std::visit([](auto&& v) { return static_cast<double>(v); }, value);
        if (prob < 0.0 || prob > 1.0) {
            NLM_LOG_WARNING("connection_probability should be between 0.0 and 1.0");
        }
    }
    
    // Additional validation for other keys...
}

// Get configuration description for better debugging
std::string Config::getConfigDescription(const std::string& key) const {
    std::string lowerKey = toLower(key);
    
    if (lowerKey == "random_seed") {
        return "Random seed for reproducible simulations";
    } else if (lowerKey == "simulation_timestep") {
        return "Simulation time step in seconds";
    } else if (lowerKey == "neuron_count") {
        return "Number of neurons in the simulation";
    } else if (lowerKey == "region_count") {
        return "Number of neural regions";
    } else if (lowerKey == "connection_probability") {
        return "Probability of connection between neurons";
    } else if (lowerKey == "stdp_ltp_weight") {
        return "STDP long-term potentiation weight";
    } else if (lowerKey == "stdp_ltd_weight") {
        return "STDP long-term depression weight";
    } else if (lowerKey == "stdp_tau") {
        return "STDP time constant";
    } else if (lowerKey == "synaptogenesis_rate") {
        return "Rate of new synapse formation";
    } else if (lowerKey == "pruning_rate") {
        return "Rate of synapse elimination";
    }
    
    return "Configuration parameter: " + key;
}

// Improved set method with validation
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Attempted to set empty configuration key");
        return;
    }
    
    // Validate value based on key (some keys have specific allowed ranges/types)
    validateConfigValue(key, value);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        it->value = value;
        it->source = source;
    } else {
        // Add new entry
        std::string description = getConfigDescription(key);
        pImpl->entries.emplace_back(key, value, source, description);
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
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

// Template functions for getOr
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    return value.has_value() ? value.value() : defaultValue;
}

}} // namespace nlm