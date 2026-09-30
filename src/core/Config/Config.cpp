#include "Config.hpp"
#include "ExceptionHelper/ErrorHandler.hpp"
#include "ExceptionHelper/ValidationHelper.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cstring>
#include <cctype>
#include <unordered_map>
#include <stdexcept>
#include <map>
#include <stdexcept>

namespace nlm {

namespace config {

// Forward declarations for validation helper
ValidationResult validateIntRange(const std::string& key, const ConfigValue& value);
ValidationResult validateDoubleRange(const std::string& key, const ConfigValue& value);
ValidationResult validateStringLength(const std::string& key, const ConfigValue& value);
ValidationResult validateBoolean(const std::string& key, const ConfigValue& value);
ValidationResult validateVectorSize(const std::string& key, const ConfigValue& value);

// Helper function to detect file format
bool isJson(const std::string& content) {
    if (content.empty()) return false;
    
    // Simple JSON detection
    size_t pos = content.find_first_not_of(" \t\n\r");
    if (pos == std::string::npos) return false;
    
    return content[pos] == '{';
}

bool isYaml(const std::string& content) {
    if (content.empty()) return false;
    
    // Simple YAML detection - look for key: value patterns
    size_t colonPos = content.find(':');
    if (colonPos == std::string::npos) return false;
    
    // Check if there's a key before the colon (no spaces before colon)
    size_t keyEnd = content.find_first_of(":\n\r", colonPos);
    if (keyEnd == std::string::npos) return false;
    
    // Extract potential key
    std::string key = content.substr(0, colonPos);
    // Remove trailing whitespace
    size_t keyEndTrim = key.find_last_not_of(" \t");
    if (keyEndTrim != std::string::npos) {
        key = key.substr(0, keyEndTrim + 1);
    }
    
    return !key.empty();
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool stringEndsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) return false;
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

std::string parseString(const std::string& line, size_t& pos) {
    std::string result;
    pos++; // Skip opening quote
    while (pos < line.length()) {
        if (line[pos] == '\\' && pos + 1 < line.length()) {
            pos++; // Skip escape character
            if (pos < line.length()) {
                result += line[pos];
                pos++;
            }
        } else if (line[pos] == '"') {
            pos++; // Skip closing quote
            break;
        } else {
            result += line[pos];
            pos++;
        }
    }
    return result;
}

double parseNumber(const std::string& line, size_t& pos) {
    std::string numStr;
    bool hasDecimal = false;
    bool hasExponent = false;
    
    while (pos < line.length() && (std::isdigit(line[pos]) || 
                                   line[pos] == '.' || 
                                   line[pos] == '-' || 
                                   line[pos] == '+' || 
                                   line[pos] == 'e' || 
                                   line[pos] == 'E')) {
        if (line[pos] == '.') {
            if (hasDecimal || hasExponent) break;
            hasDecimal = true;
        } else if (line[pos] == 'e' || line[pos] == 'E') {
            if (hasExponent) break;
            hasExponent = true;
        }
        numStr += line[pos];
        pos++;
    }
    
    try {
        return std::stod(numStr);
    } catch (...) {
        return 0.0;
    }
}

bool parseBool(const std::string& line, size_t& pos) {
    if (pos + 4 <= line.length() && toLower(line.substr(pos, 4)) == "true") {
        pos += 4;
        return true;
    } else if (pos + 5 <= line.length() && toLower(line.substr(pos, 5)) == "false") {
        pos += 5;
        return false;
    }
    return false;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(trim(item));
    }
    
    return result;
}

std::string getEnvironmentVariable(const std::string& name) {
    const char* value = std::getenv(name.c_str());
    return value ? std::string(value) : "";
}

} // namespace config

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool errorLoggingEnabled;
    std::string lastError;
    
    Impl() : errorLoggingEnabled(false), lastError("") {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Register default validation rules
    ValidationHelper::registerValidation("learning_rate", validateDoubleRange);
    ValidationHelper::registerValidation("temperature", validateDoubleRange);
    ValidationHelper::registerValidation("threshold", validateDoubleRange);
    ValidationHelper::registerValidation("weight", validateDoubleRange);
    ValidationHelper::registerValidation("enabled", validateBoolean);
    ValidationHelper::registerValidation("parameters", validateVectorSize);
    ValidationHelper::registerValidation("sizes", validateVectorSize);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

ValidationResult Config::validate() const {
    std::unordered_map<std::string, ConfigValue> values;
    
    for (const auto& entry : pImpl->entries) {
        values[entry.key] = entry.value;
    }
    
    return ValidationHelper::validateAll(values);
}

std::string Config::getValidationError() const {
    ValidationResult result = validate();
    if (result.isValid) {
        return "";
    }
    return "Validation error: " + result.errorMessage + " (code: " + result.errorCode + ")";
}

bool Config::isValid() const {
    return validate().isValid;
}

ValidationResult Config::validateSchema(const std::string& schemaFile) const {
    try {
        // Simple schema validation
        std::ifstream file(schemaFile);
        if (!file.is_open()) {
            return ValidationResult::error("Cannot open schema file: " + schemaFile, "SCHEMA_FILE_ERROR");
        }
        
        // Parse schema (simplified implementation)
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // Basic schema validation
        if (content.find("schema") == std::string::npos) {
            return ValidationResult::error("Invalid schema format", "INVALID_SCHEMA");
        }
        
        // Additional schema validation would go here
        return ValidationResult::success();
        
    } catch (const std::exception& e) {
        return ValidationResult::error("Schema validation failed: " + std::string(e.what()), "SCHEMA_VALIDATION_ERROR");
    }
}

ValidationResult Config::validateRange(const std::string& key, const std::string& type, double min, double max) const {
    try {
        auto value = get<double>(key);
        if (!value.has_value()) {
            return ValidationResult::error("Key not found: " + key, "KEY_NOT_FOUND");
        }
        
        double val = value.value();
        if (val < min || val > max) {
            return ValidationResult::error(
                "Value for key '" + key + "' is out of range [" + 
                std::to_string(min) + ", " + std::to_string(max) + "]",
                "RANGE_ERROR"
            );
        }
        
        return ValidationResult::success();
        
    } catch (const std::exception& e) {
        return ValidationResult::error("Range validation failed: " + std::string(e.what()), "RANGE_VALIDATION_ERROR");
    }
}

void Config::setDescription(const std::string& key, const std::string& description) {
    VALIDATE_PARAMETER(!key.empty(), "Key cannot be empty");
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->description = description;
    } else {
        // Create new entry with description
        ConfigValue value = 0.0; // Default value
        pImpl->entries.emplace_back(key, value, ConfigSource::Runtime, description);
    }
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->description;
    }
    return "";
}

std::vector<std::string> Config::getKeysWithSource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::string Config::getDetailedError() const {
    return pImpl->lastError;
}

void Config::enableErrorLogging(bool enable) {
    pImpl->errorLoggingEnabled = enable;
}

void Config::logError(const std::string& error) const {
    if (pImpl->errorLoggingEnabled) {
        pImpl->lastError = error;
        // In a real implementation, you would log to a file or logger
        // For now, we just store it internally
    }
}

void Config::throwException(const std::string& error) const {
    logError(error);
    throw ConfigException(error);
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        ErrorContext context = ErrorHandler::createContext("Config", "loadFromFile");
        context.setFile(__FILE__).setLine(__LINE__);
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        bool isJsonFormat = config::isJson(content);
        bool isYamlFormat = config::isYaml(content) && !isJsonFormat;
        
        if (isJsonFormat) {
            parseJsonConfig(content);
        } else if (isYamlFormat) {
            parseYamlConfig(content);
        } else {
            // Fallback to simple key=value format
            parseKeyValueConfig(content);
        }
        
        return true;
        
    } catch (const ConfigException& e) {
        throw;
    } catch (const std::exception& e) {
        throwException("Failed to load config file '" + filepath + "': " + std::string(e.what()));
        return false;
    }
}

void Config::parseJsonConfig(const std::string& content) {
    try {
        ErrorContext context = ErrorHandler::createContext("Config", "parseJsonConfig");
        context.setFile(__FILE__).setLine(__LINE__);
        
        size_t pos = 0;
        while (pos < content.length()) {
            pos = content.find_first_not_of(" \t\n\r", pos);
            if (pos == std::string::npos) break;
            
            if (content[pos] == '}') break; // End of object
            
            if (content[pos] != '"') {
                throw ConfigParseException("Invalid JSON: expected string key");
            }
            
            std::string key = config::parseString(content, pos);
            if (pos >= content.length() || content[pos] != ':') {
                throw ConfigParseException("Invalid JSON: missing colon after key");
            }
            pos++; // Skip colon
            
            pos = content.find_first_not_of(" \t\n\r", pos);
            if (pos == std::string::npos) break;
            
            ConfigValue value;
            
            if (content[pos] == '"') {
                std::string strValue = config::parseString(content, pos);
                value = strValue;
            } else if (content[pos] == 't' || content[pos] == 'f') {
                bool boolValue = config::parseBool(content, pos);
                value = boolValue;
            } else if (std::isdigit(content[pos]) || content[pos] == '-' || content[pos] == '+') {
                double numValue = config::parseNumber(content, pos);
                value = numValue;
            } else if (content[pos] == '[') {
                pos++; // Skip '['
                std::vector<std::string> arrayValues;
                size_t bracketDepth = 1;
                size_t startPos = pos;
                
                while (pos < content.length() && bracketDepth > 0) {
                    if (content[pos] == '[') bracketDepth++;
                    else if (content[pos] == ']') bracketDepth--;
                    else if (content[pos] == '"') {
                        size_t quoteEnd = content.find('"', pos + 1);
                        if (quoteEnd != std::string::npos) {
                            arrayValues.push_back(content.substr(pos + 1, quoteEnd - pos - 1));
                            pos = quoteEnd + 1;
                            continue;
                        }
                    }
                    pos++;
                }
                
                if (!arrayValues.empty()) {
                    bool allInts = std::all_of(arrayValues.begin(), arrayValues.end(),
                        [](const std::string& s) {
                            try { std::stoi(s); return true; } catch (...) { return false; }
                        });
                    bool allDoubles = std::all_of(arrayValues.begin(), arrayValues.end(),
                        [](const std::string& s) {
                            try { std::stod(s); return true; } catch (...) { return false; }
                        });
                    
                    if (allInts) {
                        std::vector<int> intArray;
                        for (const auto& str : arrayValues) {
                            try { intArray.push_back(std::stoi(str)); } catch (...) {}
                        }
                        value = intArray;
                    } else if (allDoubles) {
                        std::vector<double> doubleArray;
                        for (const auto& str : arrayValues) {
                            try { doubleArray.push_back(std::stod(str)); } catch (...) {}
                        }
                        value = doubleArray;
                    } else {
                        std::vector<std::string> stringArray = arrayValues;
                        value = stringArray;
                    }
                } else {
                    value = std::vector<double>();
                }
            } else {
                throw ConfigParseException(std::string("Invalid JSON: unexpected character '" + content[pos] + "'"));
            }
            
            pos = content.find_first_not_of(" \t\n\r", pos);
            
            set(key, value, ConfigSource::File);
            
            if (pos < content.length() && content[pos] == ',') {
                pos++;
            }
        }
        
    } catch (const ConfigParseException& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigParseException(std::string("JSON parsing failed: ") + e.what());
    }
}

void Config::parseYamlConfig(const std::string& content) {
    try {
        ErrorContext context = ErrorHandler::createContext("Config", "parseYamlConfig");
        context.setFile(__FILE__).setLine(__LINE__);
        
        std::vector<std::string> lines = config::split(content, '\n');
        
        for (const auto& line : lines) {
            std::string trimmedLine = config::trim(line);
            if (trimmedLine.empty() || trimmedLine.find('#') == 0) {
                continue;
            }
            
            size_t colonPos = trimmedLine.find(':');
            if (colonPos == std::string::npos) {
                continue;
            }
            
            std::string key = config::trim(trimmedLine.substr(0, colonPos));
            std::string value = config::trim(trimmedLine.substr(colonPos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            ConfigValue configValue;
            
            std::string lowerValue = config::toLower(value);
            if (lowerValue == "true") {
                configValue = true;
            } else if (lowerValue == "false") {
                configValue = false;
            } else {
                try {
                    size_t pos;
                    double numValue = std::stod(value, &pos);
                    if (pos == value.length()) {
                        configValue = numValue;
                    } else {
                        if (value.find('[') != std::string::npos) {
                            std::vector<double> arrayValues;
                            size_t start = value.find('[') + 1;
                            size_t end = value.find(']', start);
                            if (end != std::string::npos) {
                                std::string arrayContent = value.substr(start, end - start);
                                std::vector<std::string> arrayItems = config::split(arrayContent, ',');
                                for (const auto& item : arrayItems) {
                                    std::string trimmedItem = config::trim(item);
                                    try { arrayValues.push_back(std::stod(trimmedItem)); } catch (...) {}
                                }
                            }
                            configValue = arrayValues;
                        } else {
                            configValue = value;
                        }
                    }
                } catch (...) {
                    configValue = value;
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
    } catch (const std::exception& e) {
        throw ConfigParseException(std::string("YAML parsing failed: ") + e.what());
    }
}

void Config::parseKeyValueConfig(const std::string& content) {
    try {
        ErrorContext context = ErrorHandler::createContext("Config", "parseKeyValueConfig");
        context.setFile(__FILE__).setLine(__LINE__);
        
        std::vector<std::string> lines = config::split(content, '\n');
        
        for (const auto& line : lines) {
            std::string trimmedLine = config::trim(line);
            if (trimmedLine.empty() || trimmedLine.find('#') == 0) {
                continue;
            }
            
            size_t pos = trimmedLine.find('=');
            if (pos == std::string::npos) {
                continue;
            }
            
            std::string key = config::trim(trimmedLine.substr(0, pos));
            std::string value = config::trim(trimmedLine.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
        
    } catch (const std::exception& e) {
        throw ConfigParseException(std::string("Key-value parsing failed: ") + e.what());
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        ErrorContext context = ErrorHandler::createContext("Config", "loadFromArgs");
        context.setFile(__FILE__).setLine(__LINE__);
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                }
            } else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        throwException(std::string("Failed to load from command line arguments: ") + e.what());
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        ErrorContext context = ErrorHandler::createContext("Config", "saveToFile");
        context.setFile(__FILE__).setLine(__LINE__);
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "{\n";
        
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || 
                                    std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << arg[j];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << arg[j];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << "\"" << arg[j] << "\"";
                    }
                    file << "]";
                } else {
                    file << "null";
                }
            }, entry.value);
            
            if (i < pImpl->entries.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        
        file << "}\n";
        
        return true;
        
    } catch (const std::exception& e) {
        throwException(std::string("Failed to save config file '" + filepath + "': ") + e.what());
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    VALIDATE_PARAMETER(!key.empty(), "Key cannot be empty");
    
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
    VALIDATE_PARAMETER(!key.empty(), "Key cannot be empty");
    
    ValidationResult validation = validateValue(key, value);
    if (!validation.isValid) {
        throw ConfigValidationException(validation.errorMessage);
    }
    
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
    VALIDATE_PARAMETER(!key.empty(), "Key cannot be empty");
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    VALIDATE_PARAMETER(!key.empty(), "Key cannot be empty");
    
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

bool Config::isValidKey(const std::string& key) {
    return !key.empty() && std::all_of(key.begin(), key.end(),
        [](char c) { return std::isalnum(c) || c == '_' || c == '-'; });
}

ValidationResult Config::validateValue(const std::string& key, const ConfigValue& value) {
    if (!isValidKey(key)) {
        return ValidationResult::error("Invalid key format: " + key, "INVALID_KEY_FORMAT");
    }
    
    // Check for type-specific validation
    if (std::holds_alternative<int>(value)) {
        int val = std::get<int>(value);
        if (val < -1000 || val > 1000) {
            return ValidationResult::error("Integer value out of range [-1000, 1000]", "INT_RANGE");
        }
    } else if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (val < -10000.0 || val > 10000.0) {
            return ValidationResult::error("Double value out of range [-10000.0, 10000.0]", "DOUBLE_RANGE");
        }
    } else if (std::holds_alternative<std::string>(value)) {
        const std::string& val = std::get<std::string>(value);
        if (val.length() > 1024) {
            return ValidationResult::error("String value exceeds maximum length of 1024", "STRING_LENGTH");
        }
    } else if (std::holds_alternative<bool>(value)) {
        // Booleans are always valid
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& val = std::get<std::vector<int>>(value);
        if (val.size() > 100) {
            return ValidationResult::error("Integer vector exceeds maximum size of 100", "VECTOR_SIZE");
        }
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& val = std::get<std::vector<double>>(value);
        if (val.size() > 100) {
            return ValidationResult::error("Double vector exceeds maximum size of 100", "VECTOR_SIZE");
        }
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& val = std::get<std::vector<std::string>>(value);
        if (val.size() > 100) {
            return ValidationResult::error("String vector exceeds maximum size of 100", "VECTOR_SIZE");
        }
    }
    
    return ValidationResult::success();
}

std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
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
                oss << arg[i];
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
            oss << "null";
        }
    }, value);
    return oss.str();
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
