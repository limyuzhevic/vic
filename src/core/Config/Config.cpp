#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::parseJSON(const std::string& content) {
    try {
        // Simple JSON parser implementation
        // For now, implement a basic parser that handles JSON objects
        size_t pos = 0;
        bool result = parseJSONObject(content, pos);
        return result && pos == content.size();
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool Config::parseYAML(const std::string& content) {
    try {
        // Simple YAML parser implementation
        // For now, implement a basic parser that handles YAML mappings
        size_t pos = 0;
        bool result = parseYAMLMapping(content, pos);
        return result && pos == content.size();
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool Config::parseSimpleFormat(const std::string& content) {
    try {
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
    } catch (const std::exception& e) {
        return false;
    }
}

// Helper parsing methods
bool Config::parseJSONObject(const std::string& content, size_t& pos) {
    if (pos >= content.size() || content[pos] != '{') {
        return false;
    }
    pos++;
    
    bool first = true;
    while (pos < content.size()) {
        // Skip whitespace
        while (pos < content.size() && std::isspace(content[pos])) {
            pos++;
        }
        
        if (content[pos] == '}') {
            pos++;
            return true;
        }
        
        if (!first) {
            if (pos >= content.size() || content[pos] != ',') {
                return false;
            }
            pos++;
        }
        first = false;
        
        // Parse key
        size_t keyStart = pos;
        while (pos < content.size() && content[pos] != ':' && !std::isspace(content[pos])) {
            pos++;
        }
        if (pos >= content.size() || content[pos] != ':') {
            return false;
        }
        
        std::string key = trim(content.substr(keyStart, pos - keyStart));
        pos++; // Skip colon
        
        // Skip whitespace after colon
        while (pos < content.size() && std::isspace(content[pos])) {
            pos++;
        }
        
        // Parse value
        std::string valueStr;
        if (content[pos] == '"') {
            // String value
            pos++; // Skip opening quote
            size_t valueStart = pos;
            while (pos < content.size() && content[pos] != '"') {
                if (content[pos] == '\\' && pos + 1 < content.size()) {
                    pos += 2; // Skip escaped character
                    continue;
                }
                pos++;
            }
            valueStr = content.substr(valueStart, pos - valueStart);
            pos++; // Skip closing quote
        } else if (content[pos] == '[') {
            // Array value - parse array
            std::string arrayStr = "[";
            size_t arrayPos = pos;
            size_t bracketCount = 1;
            pos++;
            while (pos < content.size() && bracketCount > 0) {
                if (content[pos] == '[') {
                    bracketCount++;
                } else if (content[pos] == ']') {
                    bracketCount--;
                }
                if (bracketCount > 0) {
                    arrayStr += content[pos];
                }
                pos++;
            }
            // Parse array as string (simplified approach)
            valueStr = arrayStr;
        } else {
            // Number or boolean value
            size_t valueStart = pos;
            while (pos < content.size() && !std::isspace(content[pos]) && content[pos] != ',' && content[pos] != '}') {
                pos++;
            }
            valueStr = content.substr(valueStart, pos - valueStart);
        }
        
        // Set value
        try {
            ConfigValue configValue = parseValue(valueStr);
            set(key, configValue, ConfigSource::File);
        } catch (...) {
            return false;
        }
        
        // Skip whitespace
        while (pos < content.size() && std::isspace(content[pos])) {
            pos++;
        }
    }
    
    return false;
}

bool Config::parseYAMLMapping(const std::string& content, size_t& pos) {
    if (pos >= content.size() || content[pos] != '{') {
        return false;
    }
    pos++;
    
    bool first = true;
    while (pos < content.size()) {
        // Skip whitespace
        while (pos < content.size() && std::isspace(content[pos])) {
            pos++;
        }
        
        if (content[pos] == '}') {
            pos++;
            return true;
        }
        
        if (!first) {
            if (pos >= content.size() || content[pos] != ',') {
                return false;
            }
            pos++;
        }
        first = false;
        
        // Parse key
        size_t keyStart = pos;
        while (pos < content.size() && content[pos] != ':' && !std::isspace(content[pos])) {
            pos++;
        }
        if (pos >= content.size() || content[pos] != ':') {
            return false;
        }
        
        std::string key = trim(content.substr(keyStart, pos - keyStart));
        pos++; // Skip colon
        
        // Skip whitespace after colon
        while (pos < content.size() && std::isspace(content[pos])) {
            pos++;
        }
        
        // Parse value
        std::string valueStr;
        if (content[pos] == '"') {
            // String value
            pos++; // Skip opening quote
            size_t valueStart = pos;
            while (pos < content.size() && content[pos] != '"') {
                if (content[pos] == '\\' && pos + 1 < content.size()) {
                    pos += 2; // Skip escaped character
                    continue;
                }
                pos++;
            }
            valueStr = content.substr(valueStart, pos - valueStart);
            pos++; // Skip closing quote
        } else if (content[pos] == '[') {
            // Array value - parse array
            std::string arrayStr = "[";
            size_t arrayPos = pos;
            size_t bracketCount = 1;
            pos++;
            while (pos < content.size() && bracketCount > 0) {
                if (content[pos] == '[') {
                    bracketCount++;
                } else if (content[pos] == ']') {
                    bracketCount--;
                }
                if (bracketCount > 0) {
                    arrayStr += content[pos];
                }
                pos++;
            }
            valueStr = arrayStr;
        } else {
            // Number or boolean value
            size_t valueStart = pos;
            while (pos < content.size() && !std::isspace(content[pos]) && content[pos] != ',' && content[pos] != '}') {
                pos++;
            }
            valueStr = content.substr(valueStart, pos - valueStart);
        }
        
        // Set value
        try {
            ConfigValue configValue = parseValue(valueStr);
            set(key, configValue, ConfigSource::File);
        } catch (...) {
            return false;
        }
        
        // Skip whitespace
        while (pos < content.size() && std::isspace(content[pos])) {
            pos++;
        }
    }
    
    return false;
}

ConfigValue Config::parseValue(const std::string& str) {
    // Trim whitespace
    std::string value = trim(str);
    
    if (value.empty()) {
        return std::string();
    }
    
    // Check for boolean
    if (value == "true") return true;
    if (value == "false") return false;
    
    // Check for number
    try {
        if (value.find('.') != std::string::npos) {
            return std::stod(value);
        } else {
            return std::stoll(value);
        }
    } catch (...) {}
    
    // Check for array (starts with [)
    if (value.front() == '[' && value.back() == ']') {
        // Parse array content
        std::string arrayContent = value.substr(1, value.size() - 2);
        
        if (arrayContent.empty()) {
            return std::vector<int>{};
        }
        
        // Try to parse as int array first
        try {
            std::vector<int> intArray;
            size_t start = 0;
            size_t end = 0;
            while ((end = arrayContent.find(',', start)) != std::string::npos) {
                std::string element = trim(arrayContent.substr(start, end - start));
                if (!element.empty()) {
                    intArray.push_back(std::stoi(element));
                }
                start = end + 1;
            }
            if (start < arrayContent.size()) {
                std::string element = trim(arrayContent.substr(start));
                if (!element.empty()) {
                    intArray.push_back(std::stoi(element));
                }
            }
            if (!intArray.empty()) {
                return intArray;
            }
        } catch (...) {}
        
        // Try to parse as double array
        try {
            std::vector<double> doubleArray;
            size_t start = 0;
            size_t end = 0;
            while ((end = arrayContent.find(',', start)) != std::string::npos) {
                std::string element = trim(arrayContent.substr(start, end - start));
                if (!element.empty()) {
                    doubleArray.push_back(std::stod(element));
                }
                start = end + 1;
            }
            if (start < arrayContent.size()) {
                std::string element = trim(arrayContent.substr(start));
                if (!element.empty()) {
                    doubleArray.push_back(std::stod(element));
                }
            }
            if (!doubleArray.empty()) {
                return doubleArray;
            }
        } catch (...) {}
        
        // Try to parse as string array
        try {
            std::vector<std::string> stringArray;
            size_t start = 0;
            size_t end = 0;
            while ((end = arrayContent.find(',', start)) != std::string::npos) {
                std::string element = trim(arrayContent.substr(start, end - start));
                if (!element.empty()) {
                    // Remove quotes if present
                    if (element.size() >= 2 && 
                        ((element.front() == '"' && element.back() == '"') ||
                         (element.front() == '\'' && element.back() == '\''))) {
                        element = element.substr(1, element.size() - 2);
                    }
                    stringArray.push_back(element);
                }
                start = end + 1;
            }
            if (start < arrayContent.size()) {
                std::string element = trim(arrayContent.substr(start));
                if (!element.empty()) {
                    if (element.size() >= 2 && 
                        ((element.front() == '"' && element.back() == '"') ||
                         (element.front() == '\'' && element.back() == '\''))) {
                        element = element.substr(1, element.size() - 2);
                    }
                    stringArray.push_back(element);
                }
            }
            if (!stringArray.empty()) {
                return stringArray;
            }
        } catch (...) {}
    }
    
    // Default to string
    return value;
}

std::string Config::serializeValue(const ConfigValue& value) {
    try {
        std::ostringstream oss;
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Quote strings and escape quotes inside
                oss << "\"";
                for (char c : val) {
                    if (c == '\\' || c == '\"') oss << '\\';
                    oss << c;
                }
                oss << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << val[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << val[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) oss << ", \"";
                    for (char c : val[i]) {
                        if (c == '\\' || c == '\"') oss << '\\';
                        oss << c;
                    }
                    oss << "\"";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, int>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << val;
            }
        }, value);
        return oss.str();
    } catch (...) {
        return "\"ERROR\"";
    }
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

bool Config::validate() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> errors;
    
    // Check schema validation
    for (const auto& entry : pImpl->entries) {
        std::string error = validateEntry(entry);
        if (!error.empty()) {
            errors.push_back("Key \"" + entry.key + "\": " + error);
        }
    }
    
    // Check for required schema entries
    for (const auto& schema : schema_) {
        if (schema.required && !has(schema.name)) {
            errors.push_back("Required key \"" + schema.name + "\" is missing");
        }
    }
    
    // Store validation errors in a thread-safe way
    // (In a real implementation, you might want to use a shared pointer or reference to errors)
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> errors;
    
    // Check schema validation
    for (const auto& entry : pImpl->entries) {
        std::string error = validateEntry(entry);
        if (!error.empty()) {
            errors.push_back("Key \"" + entry.key + "\": " + error);
        }
    }
    
    // Check for required schema entries
    for (const auto& schema : schema_) {
        if (schema.required && !has(schema.name)) {
            errors.push_back("Required key \"" + schema.name + "\" is missing");
        }
    }
    
    return errors;
}

std::string Config::validateEntry(const ConfigEntry& entry) const {
    // Basic validation
    if (entry.key.empty()) {
        return "Key cannot be empty";
    }
    
    // Validate based on entry type
    std::string error;
    
    std::visit([&](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        
        if constexpr (std::is_same_v<T, int>) {
            if (value < -1000000 || value > 1000000) {
                error = "Value out of range for integer";
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(value) || std::isinf(value)) {
                error = "Invalid double value (NaN or infinity)";
            } else if (value < -1e9 || value > 1e9) {
                error = "Value out of range for double";
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (value.size() > 10000) {
                error = "String value too long (max 10000 characters)";
            }
            // Add regex validation if schema is present
            for (const auto& schema : schema_) {
                if (schema.name == entry.key && !schema.validation.empty()) {
                    // Basic regex implementation (simplified)
                    // In a real implementation, use a proper regex library
                    if (!validateStringWithRegex(value, schema.validation)) {
                        error = "String validation failed";
                    }
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (value.size() > 1000) {
                error = "Integer array too large (max 1000 elements)";
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (value.size() > 1000) {
                error = "Double array too large (max 1000 elements)";
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (value.size() > 1000) {
                error = "String array too large (max 1000 elements)";
            }
            for (const auto& str : value) {
                if (str.size() > 1000) {
                    error = "String array element too long (max 1000 characters)";
                    break;
                }
            }
        }
    }, entry.value);
    
    return error;
}

bool Config::validateStringWithRegex(const std::string& str, const std::string& pattern) {
    // Simple regex validation implementation
    // For patterns like "^[a-zA-Z0-9_]+$", implement basic validation
    
    if (pattern == "^[a-zA-Z0-9_]+$") {
        for (char c : str) {
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
                  (c >= '0' && c <= '9') || c == '_')) {
                return false;
            }
        }
        return true;
    }
    
    // If pattern is empty or unknown, accept any string
    if (pattern.empty() || pattern == "any") {
        return true;
    }
    
    return true; // Simplified implementation
}

void Config::addSchema(const std::string& name, const std::string& type, 
                      const std::optional<std::string>& defaultValue,
                      bool required, const std::string& validation) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ConfigSchema schema;
    schema.name = name;
    schema.type = type;
    schema.defaultValue = defaultValue;
    schema.required = required;
    schema.validation = validation;
    
    schema_.push_back(schema);
    
    // Set default value if provided and key doesn't exist
    if (defaultValue && !has(name)) {
        if (type == "int") {
            try {
                set(name, std::stoi(*defaultValue), ConfigSource::Default);
            } catch (...) {}
        } else if (type == "double") {
            try {
                set(name, std::stod(*defaultValue), ConfigSource::Default);
            } catch (...) {}
        } else if (type == "string") {
            set(name, *defaultValue, ConfigSource::Default);
        } else if (type == "bool") {
            if (*defaultValue == "true" || *defaultValue == "1") {
                set(name, true, ConfigSource::Default);
            } else if (*defaultValue == "false" || *defaultValue == "0") {
                set(name, false, ConfigSource::Default);
            }
        }
    }
}
