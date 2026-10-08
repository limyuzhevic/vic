#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>
#include <stack>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    if (loadFromJSONFile(filepath)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Fallback to simple key=value format
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

bool Config::loadFromJSONFile(const std::string& filepath) {
    // Simple JSON parser for configuration files
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Parse JSON
    return parseJSON(content);
}

bool Config::parseJSON(const std::string& jsonContent) {
    // Skip whitespace
    auto it = jsonContent.begin();
    skipWhitespace(it, jsonContent.end());
    
    if (it == jsonContent.end()) {
        return false; // Empty JSON
    }
    
    if (*it != '{') {
        return false; // Expected object at root
    }
    
    ++it; // Skip {
    skipWhitespace(it, jsonContent.end());
    
    while (it != jsonContent.end() && *it != '}') {
        // Parse key
        std::string key;
        if (!parseJSONString(it, jsonContent.end(), key)) {
            return false;
        }
        
        skipWhitespace(it, jsonContent.end());
        
        if (it == jsonContent.end() || *it != ':') {
            return false;
        }
        
        ++it; // Skip :
        skipWhitespace(it, jsonContent.end());
        
        // Parse value
        std::string valueStr;
        if (!parseJSONValue(it, jsonContent.end(), valueStr)) {
            return false;
        }
        
        // Convert value string to appropriate type
        convertAndSetValue(key, valueStr);
        
        skipWhitespace(it, jsonContent.end());
        
        if (it != jsonContent.end() && *it == ',') {
            ++it; // Skip ,
            skipWhitespace(it, jsonContent.end());
        }
    }
    
    return true;
}

bool Config::parseJSONString(std::string::iterator& it, const std::string::iterator& end, std::string& out) {
    if (it == end || *it != '\"') {
        return false;
    }
    
    ++it; // Skip opening quote
    out.clear();
    
    while (it != end) {
        if (*it == '\\') {
            ++it; // Skip escape character
            if (it == end) return false;
            out += *it;
        } else if (*it == '\"') {
            break; // Closing quote
        } else {
            out += *it;
        }
        ++it;
    }
    
    if (it == end || *it != '\"') {
        return false;
    }
    
    ++it; // Skip closing quote
    return true;
}

bool Config::parseJSONValue(std::string::iterator& it, const std::string::iterator& end, std::string& out, const std::string& jsonContent) {
    skipWhitespace(it, end);
    
    if (it == end) {
        return false;
    }
    
    switch (*it) {
        case '\"':
            return parseJSONString(it, end, out);
        case '{':
        case '}':
        case '[':
        case ']':
            // For now, skip complex structures
            return false;
        default:
            // Parse number or boolean or null
            size_t start = std::distance(jsonContent.begin(), it);
            while (it != end && *it != ',' && *it != '}' && *it != '\"') {
                ++it;
            }
            out = jsonContent.substr(start, std::distance(jsonContent.begin(), it) - start);
            return true;
    }
}

void Config::convertAndSetValue(const std::string& key, const std::string& valueStr) {
    // Try to convert to different types
    try {
        if (valueStr == "true" || valueStr == "false") {
            bool boolVal = (valueStr == "true");
            set(key, boolVal, ConfigSource::File);
            return;
        }
        
        if (valueStr == "null" || valueStr.empty()) {
            set(key, std::string(""), ConfigSource::File);
            return;
        }
        
        // Try to parse as integer
        size_t pos;
        int intVal = std::stoi(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, intVal, ConfigSource::File);
            return;
        }
        
        // Try to parse as double
        double doubleVal = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, doubleVal, ConfigSource::File);
            return;
        }
        
        // As string
        set(key, valueStr, ConfigSource::File);
    } catch (...) {
        // If all conversions fail, store as string
        set(key, valueStr, ConfigSource::File);
    }
}

void Config::skipWhitespace(std::string::iterator& it, const std::string::iterator& end) {
    while (it != end && std::isspace(*it)) {
        ++it;
    }
}
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
        
        // Write value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                                 std::is_same_v<T, std::vector<double>> || 
                                 std::is_same_v<T, std::vector<std::string>>) {
                // For arrays, write in a readable format
                file << entry.key << " = [";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << item << "\"";
                    } else {
                        file << item;
                    }
                    first = false;
                }
                file << "]\n";
            } else {
                // For basic types (int, int64_t, double)
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
    }
    
    // Write source information
    file << "#\n# Configuration source indicators:\n# 0=Default, 1=File, 2=CommandLine, 3=Runtime\n";
    file << "# source = " << static_cast<int>(entry.source) << "\n";
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
}

// Validation and error handling implementation

// Static error handling
std::string Config::lastError = "";

void Config::setLastError(const std::string& error) {
    lastError = error;
    NLM_LOG_ERROR("Config error: " + error);
}

std::string Config::getLastError() {
    return lastError;
}

void Config::clearLastError() {
    lastError = "";
}

bool Config::validate() const {
    std::string errors = validateAndGetErrors();
    return errors.empty();
}

std::string Config::validateAndGetErrors() const {
    std::string errors;
    
    for (const auto& entry : pImpl->entries) {
        std::string error = getValidationError(entry.key);
        if (!error.empty()) {
            if (!errors.empty()) errors += "\n";
            errors += "Key \"" + entry.key + "\": " + error;
        }
    }
    
    return errors;
}

bool Config::hasRequiredKeys(const std::vector<std::string>& requiredKeys) const {
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            setLastError("Missing required key: " + key);
            return false;
        }
    }
    return true;
}

std::string Config::getValidationError(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "Key not found in configuration";
    }
    
    // Check for empty strings
    if (std::holds_alternative<std::string>(it->value) && 
        std::get<std::string>(it->value).empty()) {
        return "String value cannot be empty";
    }
    
    // Check numeric ranges for specific keys
    if (key == "random_seed") {
        if (auto val = get<int64_t>(key)) {
            if (*val < 0 || *val > 2147483647) {
                return "random_seed must be between 0 and 2147483647";
            }
        }
    }
    
    if (key == "neuron_count") {
        if (auto val = get<size_t>(key)) {
            if (*val == 0) {
                return "neuron_count must be greater than 0";
            }
            if (*val > 1000000) {
                return "neuron_count cannot exceed 1000000";
            }
        }
    }
    
    if (key == "simulation_timestep") {
        if (auto val = get<double>(key)) {
            if (*val <= 0.0) {
                return "simulation_timestep must be positive";
            }
            if (*val > 1.0) {
                return "simulation_timestep cannot exceed 1.0 seconds";
            }
        }
    }
    
    // Check connection probability range
    if (key == "connection_probability") {
        if (auto val = get<float>(key)) {
            if (*val < 0.0f || *val > 1.0f) {
                return "connection_probability must be between 0.0 and 1.0";
            }
        }
    }
    
    return "";
}

void Config::addValidationRule(const std::string& key, 
                              std::function<bool(const ConfigValue&)> validator,
                              const std::string& errorMessage) {
    // Store validation rules - for now just log them
    // In a full implementation, this would store in pImpl
    NLM_LOG_INFO("Validation rule added for key \"" + key + "\": " + errorMessage);
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Basic validation for value types
    if (std::holds_alternative<int>(value) && std::get<int>(value) > 100000) {
        return false;
    }
    
    if (std::holds_alternative<double>(value) && std::get<double>(value) < -1000.0) {
        return false;
    }
    
    if (std::holds_alternative<std::string>(value) && 
        std::get<std::string>(value).find("password") != std::string::npos) {
        setLastError("Potential security issue: key name contains 'password'");
        return false;
    }
    
    return true;
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
