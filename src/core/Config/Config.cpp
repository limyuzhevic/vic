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

// Config method implementations

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension for format detection
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    if (ext == "json" || ext == "JSON") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml" || ext == "YAML" || ext == "YML") {
        return loadFromYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    
    try {
        // Simple JSON parsing (for production, use a proper JSON library like nlohmann/json)
        // This is a basic implementation that handles objects with string keys
        
        // Remove whitespace and comments (simple JSON)
        std::string cleaned;
        for (char c : jsonContent) {
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                continue; // Skip whitespace
            }
            if (c == '/' && !cleaned.empty() && cleaned.back() == '/') {
                // Simple comment handling
                continue;
            }
            cleaned += c;
        }
        
        if (cleaned.empty() || cleaned[0] != '{' || cleaned.back() != '}') {
            return false;
        }
        
        // Parse JSON object
        std::string content = cleaned.substr(1, cleaned.size() - 2); // Remove { and }
        size_t pos = 0;
        
        while (pos < content.size()) {
            // Find key
            size_t keyStart = content.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Skip colon
            size_t colonPos = content.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            // Find value
            size_t valueStart = content.find_first_not_of(' ', colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            ConfigValue value;
            if (content[valueStart] == '"') {
                // String value
                size_t valueEnd = content.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) break;
                std::string strValue = content.substr(valueStart + 1, valueEnd - valueStart - 1);
                value = strValue;
            } else if (content[valueStart] == 't' || content[valueStart] == 'f') {
                // Boolean value
                if (content.substr(valueStart, 4) == "true") {
                    value = true;
                } else if (content.substr(valueStart, 5) == "false") {
                    value = false;
                } else {
                    break;
                }
            } else if (content[valueStart] == '-' || std::isdigit(content[valueStart])) {
                // Number (integer or float)
                size_t numEnd = valueStart;
                while (numEnd < content.size() && (std::isdigit(content[numEnd]) || content[numEnd] == '.' || content[numEnd] == '-')) {
                    numEnd++;
                }
                std::string numStr = content.substr(valueStart, numEnd - valueStart);
                
                // Try to parse as int, then double
                try {
                    size_t pos2;
                    int intVal = std::stoi(numStr, &pos2);
                    if (pos2 == numStr.size()) {
                        value = intVal;
                    } else {
                        double doubleVal = std::stod(numStr, &pos2);
                        if (pos2 == numStr.size()) {
                            value = doubleVal;
                        } else {
                            break;
                        }
                    }
                } catch (...) {
                    break;
                }
            } else if (content[valueStart] == '[') {
                // Array - parse as list of values
                size_t arrayEnd = content.find(']', valueStart);
                if (arrayEnd == std::string::npos) break;
                
                std::string arrayContent = content.substr(valueStart + 1, arrayEnd - valueStart - 1);
                value = parseJSONArray(arrayContent);
            } else {
                break;
            }
            
            set(key, value, ConfigSource::File);
            
            // Find next comma
            size_t commaPos = content.find(',', valueStart);
            if (commaPos == std::string::npos) break;
            pos = commaPos + 1;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string yamlContent = buffer.str();
    
    try {
        // Simple YAML parsing (for production, use a proper YAML library like YAML-C++)
        // This is a basic implementation that handles key-value pairs
        
        // Remove comments and handle basic YAML structure
        std::string cleaned;
        for (size_t i = 0; i < yamlContent.size(); ++i) {
            // Handle line comments
            if (i + 1 < yamlContent.size() && yamlContent[i] == '#' && yamlContent[i + 1] == ' ') {
                // Skip to end of line
                while (i < yamlContent.size() && yamlContent[i] != '\n') {
                    i++;
                }
                if (i < yamlContent.size() && yamlContent[i] == '\n') {
                    cleaned += '\n';
                }
                continue;
            }
            
            // Handle block comments
            if (i + 1 < yamlContent.size() && yamlContent[i] == '/' && yamlContent[i + 1] == '/') {
                while (i < yamlContent.size() && yamlContent[i] != '\n') {
                    i++;
                }
                if (i < yamlContent.size() && yamlContent[i] == '\n') {
                    cleaned += '\n';
                }
                continue;
            }
            
            if (yamlContent[i] != ' ' && yamlContent[i] != '\t' && yamlContent[i] != '\n' && yamlContent[i] != '\r') {
                cleaned += yamlContent[i];
            }
        }
        
        // Parse key: value pairs
        size_t pos = 0;
        while (pos < cleaned.size()) {
            // Find key (until first colon)
            size_t colonPos = cleaned.find(':', pos);
            if (colonPos == std::string::npos) break;
            
            std::string key = trim(cleaned.substr(pos, colonPos - pos));
            
            // Find value (after colon)
            size_t valueStart = cleaned.find_first_not_of(' ', colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            // Determine value type
            ConfigValue value;
            
            if (valueStart < cleaned.size() && cleaned[valueStart] == '"') {
                // String value
                size_t valueEnd = cleaned.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) break;
                std::string strValue = cleaned.substr(valueStart + 1, valueEnd - valueStart - 1);
                value = strValue;
            } else if (valueStart < cleaned.size() && (cleaned[valueStart] == 't' || cleaned[valueStart] == 'f')) {
                // Boolean value
                if (cleaned.substr(valueStart, 4) == "true") {
                    value = true;
                } else if (cleaned.substr(valueStart, 5) == "false") {
                    value = false;
                } else {
                    break;
                }
            } else if (valueStart < cleaned.size() && (cleaned[valueStart] == '-' || std::isdigit(cleaned[valueStart]))) {
                // Number
                size_t numEnd = valueStart;
                while (numEnd < cleaned.size() && (std::isdigit(cleaned[numEnd]) || cleaned[numEnd] == '.' || cleaned[numEnd] == '-')) {
                    numEnd++;
                }
                std::string numStr = cleaned.substr(valueStart, numEnd - valueStart);
                
                try {
                    size_t pos2;
                    int intVal = std::stoi(numStr, &pos2);
                    if (pos2 == numStr.size()) {
                        value = intVal;
                    } else {
                        double doubleVal = std::stod(numStr, &pos2);
                        if (pos2 == numStr.size()) {
                            value = doubleVal;
                        } else {
                            break;
                        }
                    }
                } catch (...) {
                    break;
                }
            } else {
                break;
            }
            
            set(key, value, ConfigSource::File);
            
            // Find next line
            size_t newlinePos = cleaned.find('\n', valueStart);
            if (newlinePos == std::string::npos) break;
            pos = newlinePos + 1;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

ConfigValue Config::parseJSONArray(const std::string& arrayContent) {
    ConfigValue result;
    std::vector<std::string> elements;
    
    size_t pos = 0;
    while (pos < arrayContent.size()) {
        size_t elementStart = arrayContent.find_first_not_of(' ', pos);
        if (elementStart == std::string::npos) break;
        
        char firstChar = arrayContent[elementStart];
        
        if (firstChar == '"') {
            // String
            size_t elementEnd = arrayContent.find('"', elementStart + 1);
            if (elementEnd == std::string::npos) break;
            elements.push_back(arrayContent.substr(elementStart + 1, elementEnd - elementStart - 1));
            pos = elementEnd + 1;
        } else if (firstChar == 't' || firstChar == 'f') {
            // Boolean
            if (arrayContent.substr(elementStart, 4) == "true") {
                elements.push_back("1");
                pos = elementStart + 4;
            } else if (arrayContent.substr(elementStart, 5) == "false") {
                elements.push_back("0");
                pos = elementStart + 5;
            } else {
                break;
            }
        } else if (firstChar == '-' || std::isdigit(firstChar)) {
            // Number
            size_t elementEnd = elementStart;
            while (elementEnd < arrayContent.size() && (std::isdigit(arrayContent[elementEnd]) || arrayContent[elementEnd] == '.' || arrayContent[elementEnd] == '-')) {
                elementEnd++;
            }
            elements.push_back(arrayContent.substr(elementStart, elementEnd - elementStart));
            pos = elementEnd;
        } else if (firstChar == '[') {
            // Nested array
            size_t nestedEnd = findMatchingBracket(arrayContent, elementStart, '[');
            if (nestedEnd == std::string::npos) break;
            elements.push_back(arrayContent.substr(elementStart, nestedEnd - elementStart + 1));
            pos = nestedEnd + 1;
        } else {
            break;
        }
        
        // Skip comma
        size_t commaPos = arrayContent.find(',', pos);
        if (commaPos != std::string::npos) {
            pos = commaPos + 1;
        } else {
            break;
        }
    }
    
    if (!elements.empty()) {
        // Determine array type based on first element
        if (elements[0].find('.') != std::string::npos) {
            std::vector<double> doubleArray;
            for (const auto& elem : elements) {
                try {
                    doubleArray.push_back(std::stod(elem));
                } catch (...) {
                    doubleArray.push_back(0.0);
                }
            }
            result = doubleArray;
        } else {
            try {
                std::vector<int> intArray;
                for (const auto& elem : elements) {
                    intArray.push_back(std::stoi(elem));
                }
                result = intArray;
            } catch (...) {
                std::vector<std::string> stringArray;
                for (const auto& elem : elements) {
                    if (elem == "1" || elem == "0") {
                        bool val = (elem == "1");
                        stringArray.push_back(val ? "true" : "false");
                    } else {
                        stringArray.push_back(elem);
                    }
                }
                result = stringArray;
            }
        }
    }
    
    return result;
}

size_t Config::findMatchingBracket(const std::string& str, size_t start, char bracket) {
    if (str[start] != bracket) return std::string::npos;
    
    int depth = 1;
    size_t pos = start + 1;
    
    while (pos < str.size() && depth > 0) {
        if (str[pos] == bracket) {
            depth++;
        } else if (str[pos] == getClosingBracket(bracket)) {
            depth--;
        }
        pos++;
    }
    
    return (depth == 0) ? pos - 1 : std::string::npos;
}

char Config::getClosingBracket(char bracket) {
    switch (bracket) {
        case '(': return ')';
        case '[': return ']';
        case '{': return '}';
        default: return bracket;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    return loadFromFile(filepath);  // Recursive call for backward compatibility
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.size() >= 2 && arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (!arg.empty() && arg[0] == '-' && i + 1 < argc) {
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
        file << entry.key << " = ";
        
        // Write actual value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n";
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
        // Type mismatch - log error or handle appropriately
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

// Config validation and error handling methods

bool Config::validate() const {
    std::string errors = validateAndGetErrors();
    return errors.empty();
}

std::string Config::validateAndGetErrors() const {
    std::stringstream errors;
    
    for (const auto& entry : pImpl->entries) {
        // Check if key has schema
        bool keyValid = true;
        std::string keyError;
        
        // Validate key format (basic check)
        if (entry.key.empty()) {
            keyError = "Key cannot be empty";
            keyValid = false;
        }
        
        // Check for duplicate keys
        int duplicateCount = 0;
        for (const auto& other : pImpl->entries) {
            if (other.key == entry.key && &other != &entry) {
                duplicateCount++;
            }
        }
        
        if (duplicateCount > 0) {
            keyError += "Duplicate key: '" + entry.key + "' (" + std::to_string(duplicateCount + 1) + " occurrences)";
            keyValid = false;
        }
        
        // Validate key format (allowed characters)
        bool validChars = true;
        for (char c : entry.key) {
            if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
                validChars = false;
                break;
            }
        }
        
        if (!validChars) {
            keyError += "Invalid character in key: '" + entry.key + "' (only alphanumeric, _, -, . allowed)";
            keyValid = false;
        }
        
        if (!keyValid) {
            errors << "Key validation error for '" << entry.key << "': " << keyError << "\n";
        }
        
        // Validate value based on type
        std::string valueError = validateValue(entry.key, entry.value);
        if (!valueError.empty()) {
            errors << "Value validation error for key '" << entry.key << "': " << valueError << "\n";
        }
        
        // Check constraints
        if (!checkConstraints(entry.key, entry.value)) {
            errors << "Constraint validation failed for key '" << entry.key << "'\n";
        }
    }
    
    return errors.str();
}

bool Config::validateKey(const std::string& key) const {
    if (key.empty()) return false;
    
    // Check for duplicate keys
    int duplicateCount = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            duplicateCount++;
        }
    }
    
    if (duplicateCount > 1) return false;  // Allow the current key itself
    
    // Validate key format (allowed characters)
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
            return false;
        }
    }
    
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // For now, basic validation - could be extended with schema-based validation
    std::visit([this, key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            // String validation
            if (arg.empty()) {
                // Empty strings are allowed
            }
            if (arg.length() > 1024) {
                // Too long - could log warning
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            // Int array validation
            for (const auto& item : arg) {
                if (item < -1000000 || item > 1000000) {
                    // Out of range - could log warning
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            // Double array validation
            for (const auto& item : arg) {
                if (std::isnan(item) || std::isinf(item)) {
                    // Invalid numbers - could log warning
                }
            }
        }
        // Other types have no specific validation
    }, value);
    
    return true;  // Validation passed (or warnings logged)
}

bool Config::hasSchema() const {
    // Check if schema is implemented
    return false;  // Placeholder - schema could be added later
}

void Config::setSchema(const std::vector<ConfigEntry>& schema) {
    // Schema implementation would go here
    // For now, just ignore the schema
}

std::vector<ConfigEntry> Config::getSchema() const {
    return std::vector<ConfigEntry>();  // Placeholder - no schema defined
}

void Config::addConstraint(const std::string& key, std::function<bool(const ConfigValue&)> constraint) {
    // Constraint implementation would go here
    // For now, just ignore the constraint
}

bool Config::checkConstraints(const std::string& key, const ConfigValue& value) const {
    // Constraint checking implementation would go here
    // For now, always return true
    return true;
}

bool Config::supportsFormat(const std::string& ext) {
    std::string lowerExt = toLower(ext);
    return lowerExt == "json" || lowerExt == "yaml" || lowerExt == "yml" || lowerExt == "cfg" || lowerExt == "ini";
}

std::vector<std::string> Config::getSupportedFormats() {
    return {"json", "yaml", "yml", "cfg", "ini", "properties"};
}

} // namespace nlm
