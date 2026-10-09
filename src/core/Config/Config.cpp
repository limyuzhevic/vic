#include "Config.hpp"
#include "../Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>

namespace nlm {

// Global configuration error tracking
class ConfigErrorTracker {
public:
    struct ErrorRecord {
        ConfigError error;
        std::string message;
        std::chrono::system_clock::time_point timestamp;
        std::string context;
    };
    
    void recordError(ConfigError error, const std::string& message, const std::string& context = "") {
        ErrorRecord record;
        record.error = error;
        record.message = message;
        record.timestamp = std::chrono::system_clock::now();
        record.context = context;
        errors_.push_back(record);
        
        // Log the error
        std::stringstream ss;
        ss << "Config Error [" << static_cast<int>(error) << "] " << message;
        if (!context.empty()) {
            ss << " (Context: " << context << ")";
        }
        NLM_LOG_ERROR(ss.str());
    }
    
    std::vector<ErrorRecord> getAllErrors() const {
        return errors_;
    }
    
    void clearErrors() {
        errors_.clear();
    }
    
    size_t getErrorCount() const {
        return errors_.size();
    }
    
    std::vector<ConfigError> getErrorTypes() const {
        std::vector<ConfigError> types;
        for (const auto& error : errors_) {
            if (std::find(types.begin(), types.end(), error.error) == types.end()) {
                types.push_back(error.error);
            }
        }
        return types;
    }
    
private:
    std::vector<ErrorRecord> errors_;
};

// Global error tracker instance
static ConfigErrorTracker& getErrorTracker() {
    static ConfigErrorTracker tracker;
    return tracker;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool validateOnSet;
    bool validateOnGet;
    
    Impl() : entries(), validateOnSet(true), validateOnGet(false) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Initializing Configuration System");
}

Config::~Config() {
    NLM_LOG_INFO("Destroying Configuration System (errors recorded: " + std::to_string(getErrorTracker().getErrorCount()) + ")");
}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Initialize logger for Config
static void initConfigLogger() {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    consoleLogger->setUseColors(true);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

bool Config::loadFromFile(const std::string& filepath) {
    NLM_LOG_INFO("Loading configuration from file: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        getErrorTracker().recordError(ConfigError::FileNotFound, 
            "Failed to open configuration file: " + filepath, "loadFromFile");
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    // Parse JSON/YAML format
    try {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Simple JSON parser (can be enhanced to use nlohmann/json)
        parseJsonConfig(content);
        NLM_LOG_INFO("Configuration loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        getErrorTracker().recordError(ConfigError::ParseError,
            std::string("Failed to parse configuration file: ") + e.what(), "loadFromFile");
        NLM_LOG_ERROR(std::string("Configuration parse error: ") + e.what());
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration from command line arguments");
    
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
                } else {
                    getErrorTracker().recordError(ConfigError::InvalidFormat,
                        "Invalid command line argument format: " + arg, "loadFromArgs");
                    NLM_LOG_WARNING("Invalid command line argument format: " + arg);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            } else {
                getErrorTracker().recordError(ConfigError::InvalidFormat,
                    "Invalid command line argument: " + arg, "loadFromArgs");
                NLM_LOG_WARNING("Invalid command line argument: " + arg);
            }
        }
        
        NLM_LOG_INFO("Command line arguments loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        getErrorTracker().recordError(ConfigError::ParseError,
            std::string("Failed to process command line arguments: ") + e.what(), "loadFromArgs");
        NLM_LOG_ERROR(std::string("Command line argument error: ") + e.what());
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    NLM_LOG_INFO("Saving configuration to file: " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        getErrorTracker().recordError(ConfigError::FileNotFound,
            "Failed to open configuration file for writing: " + filepath, "saveToFile");
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    try {
        // Write JSON format
        file << "{\n";
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) {
                file << ",\n";
            }
            first = false;
            
            file << "  \"" << escapeJsonString(entry.key) << "\": ";
            visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << escapeJsonString(arg) << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || 
                                   std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << std::setprecision(6) << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << std::setprecision(6) << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << escapeJsonString(arg[i]) << "\"";
                    }
                    file << "]";
                } else {
                    file << "null"; // Unknown type
                }
            }, entry.value);
            
            file << "  // Source: " << static_cast<int>(entry.source);
            if (!entry.description.empty()) {
                file << " // " << escapeJsonString(entry.description);
            }
            file << "\n";
        }
        
        file << "}\n";
        
        NLM_LOG_INFO("Configuration saved successfully");
        return true;
        
    } catch (const std::exception& e) {
        getErrorTracker().recordError(ConfigError::InvalidFormat,
            std::string("Failed to save configuration: ") + e.what(), "saveToFile");
        NLM_LOG_ERROR(std::string("Configuration save error: ") + e.what());
        return false;
    }
}

// Parse string value to ConfigValue
try {
    return parseStringValue(value, outValue);
} catch (const std::exception& e) {
    getErrorTracker().recordError(ConfigError::TypeMismatch,
        std::string("Failed to parse config value for key '") + key + "': " + e.what(), "setValueFromString");
    throw;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            getErrorTracker().recordError(ConfigError::KeyNotFound,
                "Configuration key not found: " + key, "get");
            NLM_LOG_DEBUG("Config key not found: " + key);
            return std::nullopt;
        }
        
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            getErrorTracker().recordError(ConfigError::TypeMismatch,
                std::string("Config value type mismatch for key '") + key + "': expected " + typeid(T).name(), "get");
            NLM_LOG_ERROR(std::string("Config value type mismatch for key '") + key + "': expected " + typeid(T).name());
            return std::nullopt;
        }
    } catch (const std::exception& e) {
        getErrorTracker().recordError(ConfigError::ParseError,
            std::string("Error getting config value for key '") + key + "': " + e.what(), "get");
        NLM_LOG_ERROR(std::string("Error getting config value for key '") + key + "': " + e.what());
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        NLM_LOG_DEBUG(std::string("Using default config value for key '") + key + "': " + getValueString(defaultValue));
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            NLM_LOG_DEBUG(std::string("Updating config key: ") + key + " (source: " + static_cast<int>(it->source) + " -> " + static_cast<int>(source) + ")");
            it->value = value;
            it->source = source;
        } else {
            NLM_LOG_DEBUG(std::string("Adding new config key: ") + key + " = " + getValueString(value) + " (source: " + static_cast<int>(source) + ")");
            pImpl->entries.emplace_back(key, value, source);
            
            // Validate if validation is enabled
            if (pImpl->validateOnSet) {
                auto errors = validateKey(key);
                if (!errors.empty()) {
                    NLM_LOG_WARNING(std::string("New config key '") + key + "' has validation issues");
                }
            }
        }
    } catch (const std::exception& e) {
        getErrorTracker().recordError(ConfigError::InvalidFormat,
            std::string("Error setting config key '") + key + "': " + e.what(), "set");
        NLM_LOG_ERROR(std::string("Error setting config key '") + key + "': " + e.what());
        throw;
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    setValueFromString(key, value, source);
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
    bool result = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (!result) {
        NLM_LOG_DEBUG("Config key not found: " + key);
    }
    
    return result;
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        NLM_LOG_DEBUG(std::string("Removing config key: ") + key);
        pImpl->entries.erase(it);
    } else {
        NLM_LOG_DEBUG(std::string("Attempted to remove non-existent config key: ") + key);
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
    NLM_LOG_DEBUG("Clearing all configuration entries");
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "Error count: " << getErrorTracker().getErrorCount() << "\n";
    
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
        if (!entry.description.empty()) {
            oss << "    Description: " << entry.description << "\n";
        }
    }
    
    oss << "\nValidation settings: set=" << (pImpl->validateOnSet ? "enabled" : "disabled") << 
        ", get=" << (pImpl->validateOnGet ? "enabled" : "disabled") << "\n";
    
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

// Advanced configuration methods
void Config::enableValidation(bool validateOnSet, bool validateOnGet) {
    pImpl->validateOnSet = validateOnSet;
    pImpl->validateOnGet = validateOnGet;
    NLM_LOG_INFO(std::string("Configuration validation enabled: set=") + (validateOnSet ? "yes" : "no") + 
        ", get=" + (validateOnGet ? "yes" : "no"));
}

std::vector<std::string> Config::validateKey(const std::string& key) const {
    std::vector<std::string> errors;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        errors.push_back("Key does not exist");
        return errors;
    }
    
    // Basic validation based on value type
    const auto& value = it->value;
    
    try {
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (val < -1000000 || val > 1000000) {
                errors.push_back("Integer value out of expected range [-1000000, 1000000]");
            }
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < -1000.0 || val > 1000.0) {
                errors.push_back("Double value out of expected range [-1000.0, 1000.0]");
            }
        } else if (std::holds_alternative<std::string>(value)) {
            const std::string& val = std::get<std::string>(value);
            if (val.empty()) {
                errors.push_back("String value is empty");
            } else if (val.length() > 1000) {
                errors.push_back("String value too long (max 1000 characters)");
            }
        }
        
    } catch (const std::exception& e) {
        errors.push_back(std::string("Validation error: ") + e.what());
    }
    
    return errors;
}

bool Config::validateAll() const {
    std::vector<std::string> allErrors;
    
    for (const auto& entry : pImpl->entries) {
        auto errors = validateKey(entry.key);
        if (!errors.empty()) {
            allErrors.push_back("Key '" + entry.key + "': " + 
                errors[0] + (errors.size() > 1 ? " (and " + std::to_string(errors.size() - 1) + " more)" : ""));
        }
    }
    
    if (!allErrors.empty()) {
        NLM_LOG_WARNING(std::to_string(allErrors.size()) + " configuration validation errors found");
        for (const auto& error : allErrors) {
            NLM_LOG_WARNING("  " + error);
        }
        return false;
    }
    
    NLM_LOG_INFO("All configuration values passed validation");
    return true;
}

// JSON parsing helper methods
void Config::parseJsonConfig(const std::string& jsonContent) {
    // Simple JSON parser - in production, use nlohmann/json
    size_t pos = 0;
    while (pos < jsonContent.size()) {
        pos = jsonContent.find('"', pos);
        if (pos == std::string::npos) break;
        
        size_t keyEnd = jsonContent.find('"', pos + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = jsonContent.substr(pos + 1, keyEnd - pos - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace
        while (pos < jsonContent.size() && std::isspace(jsonContent[pos])) pos++;
        
        if (pos >= jsonContent.size() || jsonContent[pos] != ':') break;
        pos++;
        
        // Skip whitespace
        while (pos < jsonContent.size() && std::isspace(jsonContent[pos])) pos++;
        
        if (pos >= jsonContent.size()) break;
        
        // Parse value based on first character
        if (jsonContent[pos] == '"') {
            // String value
            size_t valueEnd = jsonContent.find('"', pos + 1);
            if (valueEnd == std::string::npos) break;
            std::string value = jsonContent.substr(pos + 1, valueEnd - pos - 1);
            set(key, value, ConfigSource::File);
            pos = valueEnd + 1;
        } else if (jsonContent[pos] == 't' || jsonContent[pos] == 'f') {
            // Boolean value
            bool value = (jsonContent[pos] == 't');
            set(key, value, ConfigSource::File);
            // Skip "rue" or "alse"
            size_t valueLen = (value ? 4 : 5);
            pos += valueLen;
        } else if (jsonContent[pos] >= '0' && jsonContent[pos] <= '9' || jsonContent[pos] == '-') {
            // Number value
            size_t valueEnd = jsonContent.find_first_of(",}", pos);
            if (valueEnd == std::string::npos) valueEnd = jsonContent.size();
            std::string valueStr = jsonContent.substr(pos, valueEnd - pos);
            
            try {
                if (valueStr.find('.') != std::string::npos || valueStr.find('e') != std::string::npos || valueStr.find('E') != std::string::npos) {
                    double value = std::stod(valueStr);
                    set(key, value, ConfigSource::File);
                } else {
                    int value = std::stoi(valueStr);
                    set(key, value, ConfigSource::File);
                }
            } catch (...) {
                // Skip invalid number
            }
            
            pos = valueEnd;
        } else if (jsonContent[pos] == '[') {
            // Array value
            size_t arrayEnd = jsonContent.find(']', pos);
            if (arrayEnd == std::string::npos) break;
            
            std::string arrayContent = jsonContent.substr(pos + 1, arrayEnd - pos - 1);
            std::vector<double> arrayValues;
            
            size_t arrayPos = 0;
            while (arrayPos < arrayContent.size()) {
                size_t nextComma = arrayContent.find(',', arrayPos);
                if (nextComma == std::string::npos) {
                    nextComma = arrayContent.size();
                }
                
                std::string item = trim(arrayContent.substr(arrayPos, nextComma - arrayPos));
                if (!item.empty()) {
                    try {
                        double value = std::stod(item);
                        arrayValues.push_back(value);
                    } catch (...) {
                        // Skip invalid array item
                    }
                }
                
                arrayPos = nextComma + 1;
                while (arrayPos < arrayContent.size() && std::isspace(arrayContent[arrayPos])) arrayPos++;
            }
            
            set(key, arrayValues, ConfigSource::File);
            pos = arrayEnd + 1;
        }
        
        // Skip comma or whitespace
        while (pos < jsonContent.size() && (jsonContent[pos] == ',' || std::isspace(jsonContent[pos]))) pos++;
    }
}

std::string Config::escapeJsonString(const std::string& str) {
    std::string result;
    result.reserve(str.size() + 10);
    
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c == 127) {
                    result += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

// Helper functions
std::string Config::getValueString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::setValueFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    try {
        ConfigValue outValue;
        bool parsed = parseStringValue(valueStr, outValue);
        if (parsed) {
            set(key, outValue, source);
            return true;
        } else {
            getErrorTracker().recordError(ConfigError::ParseError,
                std::string("Failed to parse string value '") + valueStr + "' for key '") + key + "'", "setValueFromString");
            return false;
        }
    } catch (const std::exception& e) {
        getErrorTracker().recordError(ConfigError::TypeMismatch,
            std::string("Failed to set config value for key '") + key + "' with value '") + valueStr + "': " + e.what(), "setValueFromString");
        return false;
    }
}

bool Config::parseStringValue(const std::string& valueStr, ConfigValue& outValue) {
    // Try to parse as boolean
    std::string lower = toLower(valueStr);
    if (lower == "true" || lower == "false") {
        outValue = (lower == "true");
        return true;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intValue = std::stoi(valueStr, &pos);
        if (pos == valueStr.size()) {
            outValue = intValue;
            return true;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleValue = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            outValue = doubleValue;
            return true;
        }
    } catch (...) {}
    
    // Try to parse as vector (e.g., "[1,2,3]" or "1 2 3")
    if (valueStr.find('[') != std::string::npos) {
        // Parse as JSON-like array
        std::string content = valueStr;
        if (content.front() == '[' && content.back() == ']') {
            content = content.substr(1, content.size() - 2);
            std::vector<double> vectorValue;
            std::stringstream ss(content);
            std::string item;
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                try {
                    double val = std::stod(item);
                    vectorValue.push_back(val);
                } catch (...) {
                    return false;
                }
            }
            if (!vectorValue.empty()) {
                outValue = vectorValue;
                return true;
            }
        }
    }
    
    // Default to string
    outValue = valueStr;
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
