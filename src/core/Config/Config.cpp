#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <regex>
#include <unordered_map>
#include <chrono>
#include <limits>

namespace nlm {

// Helper functions implementation
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

bool Config::isQuoted(const std::string& str) {
    return (str.size() >= 2 && 
            ((str.front() == '"' && str.back() == '"') ||
             (str.front() == '\'' && str.back() == '\'')));
}

std::string Config::unquote(const std::string& str) {
    if (isQuoted(str)) {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

std::string Config::quote(const std::string& str) {
    return "\"" + str + "\"";
}

bool Config::isNumber(const std::string& str) {
    std::regex numberPattern("^-?\d+(\.\d+)?([eE][+-]?\d+)?$");
    return std::regex_match(str, numberPattern);
}

// Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigChangeCallback> changeCallbacks;
    std::unordered_map<std::string, ConfigValidator> validators;
    std::vector<ConfigSchema> schema;
    std::unordered_map<std::string, time_t> fileTimestamps;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// File loading with format detection
bool Config::loadFromFile(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        lastError_ = "Configuration file does not exist: " + filepath;
        return false;
    }
    
    auto now = std::filesystem::last_write_time(std::filesystem::path(filepath));
    auto fileTime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        now.time_since_epoch()
    );
    time_t currentTime = std::chrono::system_clock::to_time_t(fileTime);
    
    if (pImpl->fileTimestamps.count(filepath) && pImpl->fileTimestamps[filepath] == currentTime) {
        return true;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError_ = "Failed to open configuration file: " + filepath;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    bool success = false;
    
    if (isJSON(content)) {
        success = parseJSON(content, filepath);
    } else if (isYAML(content)) {
        success = parseYAML(content, filepath);
    } else {
        success = parseSimpleFormat(content, filepath);
    }
    
    if (success) {
        pImpl->fileTimestamps[filepath] = currentTime;
        lastError_.clear();
    }
    
    return success;
}

// Simple JSON parsing
bool Config::parseJSON(const std::string& content, const std::string& filepath) {
    try {
        size_t pos = 0;
        while (pos < content.size()) {
            size_t braceStart = content.find('{', pos);
            if (braceStart == std::string::npos) break;
            
            size_t braceEnd = content.find('}', braceStart);
            if (braceEnd == std::string::npos) break;
            
            std::string jsonObj = content.substr(braceStart + 1, braceEnd - braceStart - 1);
            
            size_t keyPos = 0;
            while (keyPos < jsonObj.size()) {
                size_t colonPos = jsonObj.find(':', keyPos);
                if (colonPos == std::string::npos) break;
                
                size_t commaPos = jsonObj.find(',', colonPos);
                if (commaPos == std::string::npos) {
                    commaPos = jsonObj.size();
                }
                
                std::string key = trim(jsonObj.substr(keyPos, colonPos - keyPos));
                std::string valueStr = trim(jsonObj.substr(colonPos + 1, commaPos - colonPos - 1));
                
                if (isQuoted(key)) key = unquote(key);
                if (isQuoted(valueStr)) valueStr = unquote(valueStr);
                
                ConfigValue value;
                if (isNumber(valueStr)) {
                    if (valueStr.find('.') != std::string::npos || 
                        valueStr.find('e') != std::string::npos || 
                        valueStr.find('E') != std::string::npos) {
                        value = std::stod(valueStr);
                    } else {
                        value = std::stoll(valueStr);
                    }
                } else if (valueStr == "true" || valueStr == "false") {
                    value = (valueStr == "true");
                } else {
                    value = valueStr;
                }
                
                bool existed = has(key);
                ConfigValue oldValue = ConfigValue();
                
                if (existed) {
                    auto& entry = getEntry(key);
                    oldValue = entry.value;
                    entry.value = value;
                    entry.source = ConfigSource::File;
                    
                    notifyChange(key, oldValue, value);
                } else {
                    pImpl->entries.emplace_back(key, value, ConfigSource::File);
                }
                
                keyPos = commaPos + 1;
                while (keyPos < jsonObj.size() && std::isspace(jsonObj[keyPos])) {
                    keyPos++;
                }
            }
            
            pos = braceEnd + 1;
            while (pos < content.size() && std::isspace(content[pos])) {
                pos++;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        lastError_ = "JSON parsing error: " + std::string(e.what());
        return false;
    }
}

// Simple YAML parsing
bool Config::parseYAML(const std::string& content, const std::string& filepath) {
    try {
        size_t pos = 0;
        while (pos < content.size()) {
            size_t colonPos = content.find(':', pos);
            if (colonPos == std::string::npos) break;
            
            size_t keyStart = pos;
            while (keyStart > 0 && std::isspace(content[keyStart - 1])) {
                keyStart--;
            }
            
            size_t keyEnd = keyStart;
            while (keyEnd < colonPos && !std::isspace(content[keyEnd])) {
                keyEnd++;
            }
            
            std::string key = trim(content.substr(keyStart, keyEnd - keyStart));
            
            size_t valueStart = colonPos + 1;
            while (valueStart < content.size() && std::isspace(content[valueStart])) {
                valueStart++;
            }
            
            size_t valueEnd = valueStart;
            while (valueEnd < content.size() && content[valueEnd] != '\n' && 
                   content[valueEnd] != '\r') {
                valueEnd++;
            }
            
            std::string valueStr = trim(content.substr(valueStart, valueEnd - valueStart));
            
            if (valueStr.find('"') == 0 && valueStr.back() == '"') {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            } else if (valueStr.find("'") == 0 && valueStr.back() == "'") {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            ConfigValue value;
            if (isNumber(valueStr)) {
                if (valueStr.find('.') != std::string::npos || 
                    valueStr.find('e') != std::string::npos || 
                    valueStr.find('E') != std::string::npos) {
                    value = std::stod(valueStr);
                } else {
                    value = std::stoll(valueStr);
                }
            } else if (valueStr == "true" || valueStr == "false") {
                value = (valueStr == "true");
            } else {
                value = valueStr;
            }
            
            bool existed = has(key);
            ConfigValue oldValue = ConfigValue();
            
            if (existed) {
                auto& entry = getEntry(key);
                oldValue = entry.value;
                entry.value = value;
                entry.source = ConfigSource::File;
                
                notifyChange(key, oldValue, value);
            } else {
                pImpl->entries.emplace_back(key, value, ConfigSource::File);
            }
            
            pos = valueEnd + 1;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        lastError_ = "YAML parsing error: " + std::string(e.what());
        return false;
    }
}

// Simple format parsing
bool Config::parseSimpleFormat(const std::string& content, const std::string& filepath) {
    try {
        size_t pos = 0;
        while (pos < content.size()) {
            size_t equalPos = content.find('=', pos);
            if (equalPos == std::string::npos) break;
            
            size_t keyStart = pos;
            while (keyStart > 0 && std::isspace(content[keyStart - 1])) {
                keyStart--;
            }
            
            size_t keyEnd = keyStart;
            while (keyEnd < equalPos && !std::isspace(content[keyEnd])) {
                keyEnd++;
            }
            
            std::string key = trim(content.substr(keyStart, keyEnd - keyStart));
            
            size_t valueStart = equalPos + 1;
            while (valueStart < content.size() && std::isspace(content[valueStart])) {
                valueStart++;
            }
            
            size_t valueEnd = valueStart;
            while (valueEnd < content.size() && content[valueEnd] != '\n' && 
                   content[valueEnd] != '\r' && content[valueEnd] != '#') {
                valueEnd++;
            }
            
            std::string valueStr = trim(content.substr(valueStart, valueEnd - valueStart));
            
            if (!valueStr.empty() && valueStr[0] == '#') {
                pos = valueEnd + 1;
                continue;
            }
            
            if (valueStr.empty()) {
                pos = valueEnd + 1;
                continue;
            }
            
            if (isQuoted(valueStr)) {
                valueStr = unquote(valueStr);
            }
            
            ConfigValue value;
            if (isNumber(valueStr)) {
                if (valueStr.find('.') != std::string::npos || 
                    valueStr.find('e') != std::string::npos || 
                    valueStr.find('E') != std::string::npos) {
                    value = std::stod(valueStr);
                } else {
                    value = std::stoll(valueStr);
                }
            } else if (valueStr == "true" || valueStr == "false") {
                value = (valueStr == "true");
            } else {
                value = valueStr;
            }
            
            bool existed = has(key);
            ConfigValue oldValue = ConfigValue();
            
            if (existed) {
                auto& entry = getEntry(key);
                oldValue = entry.value;
                entry.value = value;
                entry.source = ConfigSource::File;
                
                notifyChange(key, oldValue, value);
            } else {
                pImpl->entries.emplace_back(key, value, ConfigSource::File);
            }
            
            pos = valueEnd + 1;
            while (pos < content.size() && std::isspace(content[pos])) {
                pos++;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        lastError_ = "Simple format parsing error: " + std::string(e.what());
        return false;
    }
}

// Get helper methods
ConfigEntry& Config::getEntry(const std::string& key) {
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            return entry;
        }
    }
    static ConfigEntry dummyEntry;
    return dummyEntry;
}

const ConfigEntry* Config::getEntry(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            return &entry;
        }
    }
    return nullptr;
}

std::optional<int> Config::getInt(const std::string& key) const {
    auto entry = getEntry(key);
    if (entry.key.empty()) return std::nullopt;
    
    if (entry.value.isInt()) {
        return entry.value.asInt();
    }
    return std::nullopt;
}

std::optional<int64_t> Config::getInt64(const std::string& key) const {
    auto entry = getEntry(key);
    if (entry.key.empty()) return std::nullopt;
    
    if (entry.value.isInt64()) {
        return entry.value.asInt64();
    }
    return std::nullopt;
}

std::optional<double> Config::getDouble(const std::string& key) const {
    auto entry = getEntry(key);
    if (entry.key.empty()) return std::nullopt;
    
    if (entry.value.isDouble()) {
        return entry.value.asDouble();
    }
    return std::nullopt;
}

std::optional<bool> Config::getBool(const std::string& key) const {
    auto entry = getEntry(key);
    if (entry.key.empty()) return std::nullopt;
    
    if (entry.value.isBool()) {
        return entry.value.asBool();
    }
    return std::nullopt;
}

std::optional<std::string> Config::getString(const std::string& key) const {
    auto entry = getEntry(key);
    if (entry.key.empty()) return std::nullopt;
    
    if (entry.value.isString()) {
        return entry.value.asString();
    }
    return std::nullopt;
}

std::optional<std::vector<int>> Config::getIntArray(const std::string& key) const {
    auto entry = getEntry(key);
    if (entry.key.empty()) return std::nullopt;
    
    if (entry.value.isIntArray()) {
        return entry.value.asIntArray();
    }
    return std::nullopt;
}

std::optional<std::vector<double>> Config::getDoubleArray(const std::string& key) const {
    auto entry = getEntry(key);
    if (entry.key.empty()) return std::nullopt;
    
    if (entry.value.isDoubleArray()) {
        return entry.value.asDoubleArray();
    }
    return std::nullopt;
}

std::optional<std::vector<std::string>> Config::getStringArray(const std::string& key) const {
    auto entry = getEntry(key);
    if (entry.key.empty()) return std::nullopt;
    
    if (entry.value.isStringArray()) {
        return entry.value.asStringArray();
    }
    return std::nullopt;
}

// Enhanced set methods
void Config::setInt(const std::string& key, int value, ConfigSource source,
                   const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::setDouble(const std::string& key, double value, ConfigSource source,
                       const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::setBool(const std::string& key, bool value, ConfigSource source,
                     const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::setString(const std::string& key, const std::string& value, ConfigSource source,
                       const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

// Callbacks and validation
void Config::addChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.push_back(callback);
}

void Config::removeChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.erase(
        std::remove(pImpl->changeCallbacks.begin(), pImpl->changeCallbacks.end(), callback),
        pImpl->changeCallbacks.end()
    );
}

void Config::addValidator(const std::string& key, const ConfigValidator& validator) {
    pImpl->validators[key] = validator;
}

void Config::removeValidator(const std::string& key) {
    pImpl->validators.erase(key);
}

void Config::setSchema(const std::vector<ConfigSchema>& schema) {
    pImpl->schema = schema;
}

const std::vector<ConfigSchema>& Config::getSchema() const {
    return pImpl->schema;
}

void Config::batchSet(const std::map<std::string, ConfigValue>& values) {
    std::vector<std::pair<std::string, ConfigValue>> changes;
    
    for (const auto& [key, value] : values) {
        bool existed = has(key);
        ConfigValue oldValue;
        
        if (existed) {
            oldValue = getEntry(key).value;
        }
        
        set(key, value, ConfigSource::Runtime);
        
        if (existed && oldValue != value) {
            changes.emplace_back(key, value);
        }
    }
    
    for (const auto& [key, value] : changes) {
        notifyChange(key, getEntry(key).value, value);
    }
}

void Config::notifyChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
    for (const auto& callback : pImpl->changeCallbacks) {
        try {
            callback(key, oldValue, newValue);
        } catch (const std::exception& e) {
            lastError_ = "Callback error for key '" + key + "': " + std::string(e.what());
        }
    }
}

bool Config::validateValue(const std::string& key, const ConfigValue& value, std::string& errorMessage) const {
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end()) {
        return validatorIt->second(value, errorMessage);
    }
    return true;
}

bool Config::validateSchema(const ConfigSchema& schema, const ConfigEntry& entry) const {
    if (schema.expectedType == ConfigValue::Type::Unknown) {
        return true;
    }
    
    ConfigValue::Type actualType = entry.value.type();
    if (actualType != schema.expectedType) {
        return false;
    }
    
    return true;
}

// Statistics
size_t Config::entryCount(ConfigSource source) const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            count++;
        }
    }
    return count;
}

// Template implementations
#include <typeinfo>

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto entry = getEntry(key);
    if (entry.key.empty()) return std::nullopt;
    
    try {
        if constexpr (std::is_same_v<T, int>) {
            if (entry.value.isInt()) return entry.value.asInt();
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (entry.value.isInt64()) return entry.value.asInt64();
        } else if constexpr (std::is_same_v<T, double>) {
            if (entry.value.isDouble()) return entry.value.asDouble();
        } else if constexpr (std::is_same_v<T, bool>) {
            if (entry.value.isBool()) return entry.value.asBool();
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (entry.value.isString()) return entry.value.asString();
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (entry.value.isIntArray()) return entry.value.asIntArray();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (entry.value.isDoubleArray()) return entry.value.asDoubleArray();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (entry.value.isStringArray()) return entry.value.asStringArray();
        }
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
void Config::set(const std::string& key, const T& value, ConfigSource source,
                 const std::string& description) {
    ConfigValue configValue;
    
    if constexpr (std::is_same_v<T, int>) {
        configValue = ConfigValue(value);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        configValue = ConfigValue(value);
    } else if constexpr (std::is_same_v<T, double>) {
        configValue = ConfigValue(value);
    } else if constexpr (std::is_same_v<T, bool>) {
        configValue = ConfigValue(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        configValue = ConfigValue(value);
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        configValue = ConfigValue(value);
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        configValue = ConfigValue(value);
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        configValue = ConfigValue(value);
    } else {
        lastError_ = "Unsupported type for configuration key: " + key;
        return;
    }
    
    std::string errorMsg;
    if (!validateValue(key, configValue, errorMsg)) {
        lastError_ = "Validation failed for key '" + key + "': " + errorMsg;
        return;
    }
    
    bool schemaValid = true;
    for (const auto& schema : pImpl->schema) {
        if (schema.key == key && !validateSchema(schema, ConfigEntry(key, configValue, source, description))) {
            schemaValid = false;
            break;
        }
    }
    
    if (!schemaValid) {
        lastError_ = "Schema validation failed for key: " + key;
        return;
    }
    
    bool existed = has(key);
    ConfigValue oldValue;
    
    if (existed) {
        oldValue = getEntry(key).value;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                           [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        ConfigValue storedOldValue = it->value;
        it->value = configValue;
        it->source = source;
        it->description = description;
        
        if (storedOldValue != configValue) {
            notifyChange(key, storedOldValue, configValue);
        }
    } else {
        pImpl->entries.emplace_back(key, configValue, source, description);
    }
}

// Explicit template instantiations
namespace nlm {
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
    template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
    template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
    
    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
    template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
    template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
    template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;
    
    template void Config::set<int>(const std::string&, const int&, ConfigSource, const std::string&);
    template void Config::set<int64_t>(const std::string&, const int64_t&, ConfigSource, const std::string&);
    template void Config::set<double>(const std::string&, const double&, ConfigSource, const std::string&);
    template void Config::set<bool>(const std::string&, const bool&, ConfigSource, const std::string&);
    template void Config::set<std::string>(const std::string&, const std::string&, ConfigSource, const std::string&);
    template void Config::set<std::vector<int>>(const std::string&, const std::vector<int>&, ConfigSource, const std::string&);
    template void Config::set<std::vector<double>>(const std::string&, const std::vector<double>&, ConfigSource, const std::string&);
    template void Config::set<std::vector<std::string>>(const std::string&, const std::vector<std::string>&, ConfigSource, const std::string&);
}

// ConfigHelpers namespace implementation
namespace ConfigHelpers {
    std::vector<ConfigSchema> getDefaultSchema() {
        return std::vector<ConfigSchema>{
            ConfigSchema("random_seed", ConfigValue::Type::Int, true, ConfigValue(42),
                        "Random seed for reproducible experiments"),
            ConfigSchema("simulation_timestep", ConfigValue::Type::Double, true, ConfigValue(0.001),
                        "Simulation timestep in seconds"),
            ConfigSchema("neuron_count", ConfigValue::Type::Int, true, ConfigValue(1000),
                        "Number of neurons in the brain"),
            ConfigSchema("region_count", ConfigValue::Type::Int, true, ConfigValue(2),
                        "Number of brain regions"),
            ConfigSchema("connection_probability", ConfigValue::Type::Double, true, ConfigValue(0.1),
                        "Connection probability for random connectivity"),
            ConfigSchema("stdp_ltp_weight", ConfigValue::Type::Double, false, ConfigValue(0.01),
                        "STDP LTP weight"),
            ConfigSchema("stdp_ltd_weight", ConfigValue::Type::Double, false, ConfigValue(0.012),
                        "STDP LTD weight"),
            ConfigSchema("stdp_time_constant", ConfigValue::Type::Double, false, ConfigValue(20.0),
                        "STDP time constant"),
            ConfigSchema("verbose", ConfigValue::Type::Bool, false, ConfigValue(false),
                        "Verbose output"),
            ConfigSchema("quiet", ConfigValue::Type::Bool, false, ConfigValue(false),
                        "Quiet mode (minimal output)"),
            ConfigSchema("progress", ConfigValue::Type::Bool, false, ConfigValue(true),
                        "Show progress updates"),
        };
    }
    
    bool validatePositive(const ConfigValue& value, std::string& errorMessage) {
        if (value.isInt() && value.asInt() < 0) {
            errorMessage = "Value must be positive";
            return false;
        }
        if (value.isDouble() && value.asDouble() < 0.0) {
            errorMessage = "Value must be positive";
            return false;
        }
        return true;
    }
    
    bool validateNonNegative(const ConfigValue& value, std::string& errorMessage) {
        if (value.isInt() && value.asInt() < 0) {
            errorMessage = "Value must be non-negative";
            return false;
        }
        if (value.isDouble() && value.asDouble() < 0.0) {
            errorMessage = "Value must be non-negative";
            return false;
        }
        return true;
    }
    
    bool validateRange(const ConfigValue& value, double min, double max, std::string& errorMessage) {
        if (value.isInt()) {
            int val = value.asInt();
            if (val < min || val > max) {
                errorMessage = "Value must be between " + std::to_string(min) + " and " + std::to_string(max);
                return false;
            }
        } else if (value.isDouble()) {
            double val = value.asDouble();
            if (val < min || val > max) {
                errorMessage = "Value must be between " + std::to_string(min) + " and " + std::to_string(max);
                return false;
            }
        }
        return true;
    }
}

} // namespace nlm
