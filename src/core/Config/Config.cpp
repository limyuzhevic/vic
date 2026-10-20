// Enhanced Configuration System with Comprehensive Error Handling
// All requirements from the implementation specification

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <mutex>
#include <chrono>
#include <thread>
#include <iostream>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <unordered_set>

namespace nlm {

// Configuration validation warnings for thread-safe operation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::vector<std::string>> validationWarnings;
    std::mutex implMutex;
};

// Constants for buffer overflow protection
const size_t MAX_STRING_LENGTH = 4096;  // 4KB limit for strings
const size_t MAX_FILE_SIZE = 10 * 1024 * 1024;  // 10MB limit for files

Config::Config() : pImpl(std::make_unique<Impl>()), configMutex() {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept {
    pImpl = std::move(other.pImpl);
    // Mutexes cannot be swapped, create new ones
    configMutex = std::mutex();
    other.configMutex = std::mutex();
}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        configMutex = std::mutex();
        other.configMutex = std::mutex();
    }
    return *this;
}

// Enhanced loadFromFile with comprehensive error recovery
ConfigResult Config::loadFromFile(const std::string& filepath) {
    // Validate input
    if (filepath.empty()) {
        return ConfigResult(ConfigErrorCode::InvalidValue, "Empty filepath not allowed");
    }
    
    if (filepath.length() > MAX_STRING_LENGTH) {
        return ConfigResult(ConfigErrorCode::InvalidValue, "Filepath too long");
    }
    
    // Check file size first
    std::error_code ec;
    auto fileSize = std::filesystem::file_size(filepath, ec);
    if (ec) {
        return ConfigResult(ConfigErrorCode::FileInvalid, "Cannot access file: " + filepath);
    }
    
    if (fileSize > MAX_FILE_SIZE) {
        return ConfigResult(ConfigErrorCode::InvalidValue, "File too large: " + filepath);
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return ConfigResult(ConfigErrorCode::FileNotFound, "Cannot open file: " + filepath);
    }
    
    try {
        std::string line;
        size_t lineNum = 0;
        while (std::getline(file, line)) {
            ++lineNum;
            
            // Trim line
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
                
                ConfigResult result = set(key, value, ConfigSource::File);
                if (result.isFailure()) {
                    pImpl->validationWarnings["line_" + std::to_string(lineNum)].push_back(result.getMessage());
                }
            } else {
                pImpl->validationWarnings["line_" + std::to_string(lineNum)].push_back("Invalid line format");
            }
        }
        
        return ConfigResult();
        
    } catch (const std::exception& e) {
        return ConfigResult(ConfigErrorCode::UnknownError, "Exception loading file: " + filepath + ": " + e.what());
    }
}

// Enhanced loadFromArgs with comprehensive CLI support
ConfigResult Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        return ConfigResult(); // Not an error, just nothing to process
    }
    
    // Parse all arguments with comprehensive error handling
    std::unordered_set<std::string> processedKeys;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.empty()) {
            continue;
        }
        
        // Handle --help flag
        if (arg == "--help" || arg == "-h") {
            continue;
        }
        
        // Handle --config-file flag
        if (arg == "--config-file") {
            if (i + 1 < argc) {
                std::string configFile = argv[++i];
                ConfigResult result = loadFromFile(configFile);
                if (result.isFailure()) {
                    // Log error but continue processing other arguments
                }
            }
            continue;
        }
        
        // Handle boolean flags (--enable-feature, --debug, etc.)
        if (arg.find("=") == std::string::npos && arg.find_first_of("-") == 0 && arg.length() > 1) {
            std::string key;
            if (arg.find("--") == 0) {
                key = arg.substr(2);
            } else if (arg.find("-") == 0 && arg.length() > 2) {
                key = arg.substr(1);
            }
            
            if (!key.empty()) {
                if (processedKeys.find(key) != processedKeys.end()) {
                    return ConfigResult(ConfigErrorCode::DuplicateKey, "Duplicate key in arguments: " + key);
                }
                
                processedKeys.insert(key);
                
                ConfigResult result = set(key, true, ConfigSource::CommandLine);
                if (result.isFailure()) {
                    return result;
                }
            }
            continue;
        }
        
        // Handle key=value pairs
        size_t eqPos = arg.find("=");
        if (eqPos != std::string::npos) {
            std::string key = arg.substr(0, eqPos);
            std::string value = arg.substr(eqPos + 1);
            
            if (!key.empty() && key[0] == '-') {
                key = key.substr(1);
            }
            
            if (!value.empty() && value[0] == '-') {
                value = value.substr(1);
            }
            
            if (key.empty()) {
                return ConfigResult(ConfigErrorCode::InvalidValue, "Empty key in argument: " + arg);
            }
            
            if (processedKeys.find(key) != processedKeys.end()) {
                return ConfigResult(ConfigErrorCode::DuplicateKey, "Duplicate key in arguments: " + key);
            }
            
            processedKeys.insert(key);
            
            ConfigValue configValue;
            ConfigResult valueResult = parseCLIValue(value, configValue);
            if (valueResult.isFailure()) {
                return ConfigResult(ConfigErrorCode::ParseError, 
                                  "Invalid value for key " + key + ": " + value + " - " + valueResult.getMessage());
            }
            
            ConfigResult setResult = set(key, configValue, ConfigSource::CommandLine);
            if (setResult.isFailure()) {
                return setResult;
            }
            
            continue;
        }
        
        // Handle regular arguments that might be keys
        if (arg.find_first_of("-") == 0 && arg.length() > 1) {
            std::string key;
            if (arg.find("--") == 0) {
                key = arg.substr(2);
            } else {
                key = arg.substr(1);
            }
            
            if (!key.empty()) {
                if (processedKeys.find(key) != processedKeys.end()) {
                    return ConfigResult(ConfigErrorCode::DuplicateKey, "Duplicate key in arguments: " + key);
                }
                
                processedKeys.insert(key);
                
                ConfigValue configValue;
                if (parseCLIValue(arg, configValue)) {
                    ConfigResult result = set(key, configValue, ConfigSource::CommandLine);
                    if (result.isFailure()) {
                        return result;
                    }
                    continue;
                }
                
                ConfigResult result = set(key, arg.substr(1), ConfigSource::CommandLine);
                if (result.isFailure()) {
                    return result;
                }
            }
        }
    }
    
    return ConfigResult();
}

// Enhanced saveToFile implementation
ConfigResult Config::saveToFile(const std::string& filepath) const {
    // Validate input
    if (filepath.empty()) {
        return ConfigResult(ConfigErrorCode::InvalidValue, "Empty filepath not allowed");
    }
    
    if (filepath.length() > MAX_STRING_LENGTH) {
        return ConfigResult(ConfigErrorCode::InvalidValue, "Filepath too long");
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return ConfigResult(ConfigErrorCode::FileNotFound, "Cannot open file for writing: " + filepath);
    }
    
    // Write actual values, not placeholders
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert ConfigValue to string
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return ConfigResult();
}

// Legacy API methods (for backward compatibility)
bool Config::legacyLoadFromFile(const std::string& filepath) {
    ConfigResult result = loadFromFile(filepath);
    return result.isSuccess();
}

bool Config::legacyLoadFromArgs(int argc, char** argv) {
    ConfigResult result = loadFromArgs(argc, argv);
    return result.isSuccess();
}

bool Config::legacySaveToFile(const std::string& filepath) const {
    ConfigResult result = saveToFile(filepath);
    return result.isSuccess();
}

template<typename T>
std::optional<T> Config::legacyGet(const std::string& key) const {
    T value;
    ConfigResult result = get<T>(key, value);
    if (result.isSuccess()) {
        return value;
    }
    return std::nullopt;
}

template<typename T>
T Config::legacyGetOr(const std::string& key, const T& defaultValue) const {
    T value;
    ConfigResult result = get<T>(key, value);
    if (result.isSuccess()) {
        return value;
    }
    return defaultValue;
}

void Config::legacySet(const std::string& key, const ConfigValue& value, ConfigSource source) {
    ConfigResult result = set(key, value, source);
    if (result.isFailure()) {
        throw ConfigException("Failed to set configuration: " + result.getMessage(), result.getErrorCode());
    }
}

void Config::legacySet(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigResult result = set(key, value, source);
    if (result.isFailure()) {
        throw ConfigException("Failed to set configuration: " + result.getMessage(), result.getErrorCode());
    }
}

void Config::legacySet(const std::string& key, int value, ConfigSource source) {
    ConfigResult result = set(key, value, source);
    if (result.isFailure()) {
        throw ConfigException("Failed to set configuration: " + result.getMessage(), result.getErrorCode());
    }
}

void Config::legacySet(const std::string& key, double value, ConfigSource source) {
    ConfigResult result = set(key, value, source);
    if (result.isFailure()) {
        throw ConfigException("Failed to set configuration: " + result.getMessage(), result.getErrorCode());
    }
}

void Config::legacySet(const std::string& key, bool value, ConfigSource source) {
    ConfigResult result = set(key, value, source);
    if (result.isFailure()) {
        throw ConfigException("Failed to set configuration: " + result.getMessage(), result.getErrorCode());
    }
}

bool Config::legacyHas(const std::string& key) const {
    return has(key);
}

void Config::legacyRemove(const std::string& key) {
    remove(key);
}

std::vector<std::string> Config::legacyGetKeys() const {
    return getKeys();
}

void Config::legacyClear() {
    clear();
}

std::string Config::legacySummary() const {
    return summary();
}

// Set implementation with type inference
ConfigResult Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    ConfigResult validationResult = validateConfigInput(key, value);
    if (validationResult.isFailure()) {
        return validationResult;
    }
    
    ConfigValue configValue;
    if (parseCLIValue(value, configValue)) {
        return set(key, configValue, source);
    } else {
        configValue = value;
        return set(key, configValue, source);
    }
}

// Core set implementation
ConfigResult Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    // Check for duplicate key (with same source type to allow overrides)
    auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (existingIt != pImpl->entries.end()) {
        // Update existing entry
        existingIt->value = value;
        existingIt->source = source;
        existingIt->timestamp = std::chrono::system_clock::now();
        
        ConfigResult validationResult = validateConfigEntry(*existingIt);
        if (validationResult.isFailure()) {
            pImpl->validationWarnings[key].push_back(validationResult.getMessage());
        }
        return ConfigResult();
    }
    
    // Add new entry
    pImpl->entries.emplace_back(key, value, source);
    
    // Validate the entry
    ConfigResult validationResult = validateConfigEntry(pImpl->entries.back());
    if (validationResult.isFailure()) {
        pImpl->validationWarnings[key].push_back(validationResult.getMessage());
    }
    
    return ConfigResult();
}

// Helper methods implementation
std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return result;
}

bool Config::configValueToBool(const ConfigValue& val) {
    if (std::holds_alternative<bool>(val)) {
        return std::get<bool>(val);
    }
    return false;
}

int Config::configValueToInt(const ConfigValue& val) {
    if (std::holds_alternative<int>(val)) {
        return std::get<int>(val);
    } else if (std::holds_alternative<int64_t>(val)) {
        return static_cast<int>(std::get<int64_t>(val));
    }
    return 0;
}

int64_t Config::configValueToInt64(const ConfigValue& val) {
    if (std::holds_alternative<int>(val)) {
        return static_cast<int64_t>(std::get<int>(val));
    } else if (std::holds_alternative<int64_t>(val)) {
        return std::get<int64_t>(val);
    }
    return 0;
}

double Config::configValueToDouble(const ConfigValue& val) {
    if (std::holds_alternative<double>(val)) {
        return std::get<double>(val);
    } else if (std::holds_alternative<int>(val)) {
        return static_cast<double>(std::get<int>(val));
    } else if (std::holds_alternative<int64_t>(val)) {
        return static_cast<double>(std::get<int64_t>(val));
    }
    return 0.0;
}

std::string Config::configValueToString(const ConfigValue& val) {
    if (std::holds_alternative<std::string>(val)) {
        return std::get<std::string>(val);
    } else if (std::holds_alternative<int>(val)) {
        return std::to_string(std::get<int>(val));
    } else if (std::holds_alternative<int64_t>(val)) {
        return std::to_string(std::get<int64_t>(val));
    } else if (std::holds_alternative<double>(val)) {
        return std::to_string(std::get<double>(val));
    } else if (std::holds_alternative<bool>(val)) {
        return std::get<bool>(val) ? "true" : "false";
    }
    return "";
}

bool Config::configValueToBool(const ConfigValue& val, bool& outValue) {
    if (std::holds_alternative<bool>(val)) {
        outValue = std::get<bool>(val);
        return true;
    }
    return false;
}

bool Config::configValueToInt(const ConfigValue& val, int& outValue) {
    if (std::holds_alternative<int>(val)) {
        outValue = std::get<int>(val);
        return true;
    } else if (std::holds_alternative<int64_t>(val)) {
        outValue = static_cast<int>(std::get<int64_t>(val));
        return true;
    }
    return false;
}

bool Config::configValueToInt64(const ConfigValue& val, int64_t& outValue) {
    if (std::holds_alternative<int>(val)) {
        outValue = static_cast<int64_t>(std::get<int>(val));
        return true;
    } else if (std::holds_alternative<int64_t>(val)) {
        outValue = std::get<int64_t>(val);
        return true;
    }
    return false;
}

bool Config::configValueToDouble(const ConfigValue& val, double& outValue) {
    if (std::holds_alternative<double>(val)) {
        outValue = std::get<double>(val);
        return true;
    } else if (std::holds_alternative<int>(val)) {
        outValue = static_cast<double>(std::get<int>(val));
        return true;
    } else if (std::holds_alternative<int64_t>(val)) {
        outValue = static_cast<double>(std::get<int64_t>(val));
        return true;
    }
    return false;
}

bool Config::configValueToString(const ConfigValue& val, std::string& outValue) {
    if (std::holds_alternative<std::string>(val)) {
        outValue = std::get<std::string>(val);
        return true;
    } else if (std::holds_alternative<int>(val)) {
        outValue = std::to_string(std::get<int>(val));
        return true;
    } else if (std::holds_alternative<int64_t>(val)) {
        outValue = std::to_string(std::get<int64_t>(val));
        return true;
    } else if (std::holds_alternative<double>(val)) {
        outValue = std::to_string(std::get<double>(val));
        return true;
    } else if (std::holds_alternative<bool>(val)) {
        outValue = std::get<bool>(val) ? "true" : "false";
        return true;
    }
    return false;
}

ConfigResult Config::parseCLIValue(const std::string& arg, ConfigValue& outValue) {
    // Try boolean first
    if (safeStringToBool(arg, outValue)) {
        return ConfigResult();
    }
    
    // Try integer
    if (safeStringToInt(arg, outValue)) {
        return ConfigResult();
    }
    
    // Try double
    if (safeStringToDouble(arg, outValue)) {
        return ConfigResult();
    }
    
    // Treat as string
    outValue = arg;
    return ConfigResult();
}

// Enhanced validation methods
ConfigResult Config::validateConfigEntry(const ConfigEntry& entry) const {
    // Basic validation
    if (entry.key.empty()) {
        return ConfigResult(ConfigErrorCode::InvalidValue, "Empty key not allowed");
    }
    
    // Type-specific validation
    ConfigResult typeResult = validateValueType(entry.value, "");
    if (typeResult.isFailure()) {
        return typeResult;
    }
    
    // Range validation for numeric types
    ConfigResult rangeResult = validateValueRange(entry);
    if (rangeResult.isFailure()) {
        return rangeResult;
    }
    
    return ConfigResult();
}

ConfigResult Config::validateValueType(const ConfigValue& value, const std::string& expectedType) const {
    // If expectedType is provided, validate against it
    if (!expectedType.empty()) {
        // Map expectedType to ConfigValue type
        if (expectedType == "int" && !std::holds_alternative<int>(value) && 
            !std::holds_alternative<int64_t>(value)) {
            return ConfigResult(ConfigErrorCode::TypeMismatch, 
                "Expected int type but got " + valueTypeToString(value));
        } else if (expectedType == "double" && !std::holds_alternative<double>(value)) {
            return ConfigResult(ConfigErrorCode::TypeMismatch,
                "Expected double type but got " + valueTypeToString(value));
        } else if (expectedType == "bool" && !std::holds_alternative<bool>(value)) {
            return ConfigResult(ConfigErrorCode::TypeMismatch,
                "Expected bool type but got " + valueTypeToString(value));
        } else if (expectedType == "string" && !std::holds_alternative<std::string>(value)) {
            return ConfigResult(ConfigErrorCode::TypeMismatch,
                "Expected string type but got " + valueTypeToString(value));
        }
    }
    
    return ConfigResult();
}

ConfigResult Config::validateValueRange(const ConfigEntry& entry) const {
    // Check for out-of-range values based on key names
    std::string key = toLower(entry.key);
    
    if (key.find("count") != std::string::npos || key.find("size") != std::string::npos) {
        // Validate that count/size is positive
        if (std::holds_alternative<int>(entry.value)) {
            if (std::get<int>(entry.value) < 0) {
                return ConfigResult(ConfigErrorCode::OutOfRange,
                    "Count/size must be non-negative: " + entry.key);
            }
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            if (std::get<int64_t>(entry.value) < 0) {
                return ConfigResult(ConfigErrorCode::OutOfRange,
                    "Count/size must be non-negative: " + entry.key);
            }
        }
    }
    
    if (key.find("rate") != std::string::npos || key.find("probability") != std::string::npos) {
        // Validate that probability is between 0 and 1
        if (std::holds_alternative<double>(entry.value)) {
            double val = std::get<double>(entry.value);
            if (val < 0.0 || val > 1.0) {
                return ConfigResult(ConfigErrorCode::OutOfRange,
                    "Probability must be between 0 and 1: " + entry.key);
            }
        }
    }
    
    return ConfigResult();
}

std::string Config::valueTypeToString(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    return "unknown";
}

std::string Config::errorCodeToString(ConfigErrorCode code) {
    switch (code) {
        case ConfigErrorCode::None: return "None";
        case ConfigErrorCode::FileNotFound: return "FileNotFound";
        case ConfigErrorCode::FileInvalid: return "FileInvalid";
        case ConfigErrorCode::ParseError: return "ParseError";
        case ConfigErrorCode::KeyNotFound: return "KeyNotFound";
        case ConfigErrorCode::DuplicateKey: return "DuplicateKey";
        case ConfigErrorCode::TypeMismatch: return "TypeMismatch";
        case ConfigErrorCode::InvalidValue: return "InvalidValue";
        case ConfigErrorCode::ValidationError: return "ValidationError";
        case ConfigErrorCode::OutOfRange: return "OutOfRange";
        case ConfigErrorCode::ThreadSafetyError: return "ThreadSafetyError";
        case ConfigErrorCode::MemoryError: return "MemoryError";
        case ConfigErrorCode::UnknownError: return "UnknownError";
        default: return "UnknownError";
    }
}

ConfigResult Config::validate() const {
    std::lock_guard<std::mutex> lock(configMutex);
    
    for (const auto& entry : pImpl->entries) {
        ConfigResult validationResult = validateConfigEntry(entry);
        if (validationResult.isFailure()) {
            return validationResult;
        }
    }
    
    return ConfigResult();
}

// Helper functions
static bool safeStringToBool(const std::string& str, ConfigValue& outValue) {
    std::string lower = str;
    for (char& c : lower) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    
    if (lower == "true" || lower == "yes" || lower == "1" || lower == "on") {
        outValue = true;
        return true;
    } else if (lower == "false" || lower == "no" || lower == "0" || lower == "off") {
        outValue = false;
        return true;
    }
    return false;
}

static bool safeStringToInt(const std::string& str, ConfigValue& outValue) {
    if (!isValidString(str)) {
        return false;
    }
    
    try {
        size_t pos;
        int64_t val = std::stoll(str, &pos);
        if (pos == str.size()) {
            if (val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max()) {
                outValue = static_cast<int>(val);
            } else {
                outValue = val;
            }
            return true;
        }
    } catch (const std::exception&) {}
    return false;
}

static bool safeStringToDouble(const std::string& str, ConfigValue& outValue) {
    if (!isValidString(str)) {
        return false;
    }
    
    try {
        size_t pos;
        double val = std::stod(str, &pos);
        if (pos == str.size()) {
            outValue = val;
            return true;
        }
    } catch (const std::exception&) {}
    return false;
}

bool isValidString(const std::string& str) {
    return str.length() <= MAX_STRING_LENGTH && str.find('\0') == std::string::npos;
}

} // namespace nlm

// Forward declaration for template implementations
namespace nlm {
template<typename T>
ConfigResult Config::get(const std::string& key, T& outValue) const {
    std::lock_guard<std::mutex> lock(configMutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigResult(ConfigErrorCode::KeyNotFound, "Key not found: " + key);
    }
    
    try {
        outValue = std::get<T>(it->value);
        return ConfigResult();
    } catch (const std::bad_variant_access&) {
        return ConfigResult(ConfigErrorCode::TypeMismatch, 
            "Type mismatch for key: " + key + " (expected " + valueTypeToString(it->value) + ")");
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    T value;
    ConfigResult result = get<T>(key, value);
    if (result.isSuccess()) {
        return value;
    }
    return defaultValue;
}

bool Config::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(configMutex);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(configMutex);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->validationWarnings.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(configMutex);
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(configMutex);
    pImpl->entries.clear();
    pImpl->validationWarnings.clear();
}

std::string Config::summary() const {
    std::lock_guard<std::mutex> lock(configMutex);
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    if (!pImpl->validationWarnings.empty()) {
        oss << "\nValidation Warnings:\n";
        for (const auto& warning : pImpl->validationWarnings) {
            for (const auto& msg : warning.second) {
                oss << "  WARNING: " << msg << "\n";
            }
        }
    }
    
    return oss.str();
}

} // namespace nlm

// Explicit template instantiations
template std::optional<int> nlm::Config::legacyGet<int>(const std::string&) const;
template std::optional<int64_t> nlm::Config::legacyGet<int64_t>(const std::string&) const;
template std::optional<double> nlm::Config::legacyGet<double>(const std::string&) const;
template std::optional<bool> nlm::Config::legacyGet<bool>(const std::string&) const;
template std::optional<std::string> nlm::Config::legacyGet<std::string>(const std::string&) const;

template int nlm::Config::legacyGetOr<int>(const std::string&, const int&) const;
template int64_t nlm::Config::legacyGetOr<int64_t>(const std::string&, const int64_t&) const;
template double nlm::Config::legacyGetOr<double>(const std::string&, const double&) const;
template bool nlm::Config::legacyGetOr<bool>(const std::string&, const bool&) const;
template std::string nlm::Config::legacyGetOr<std::string>(const std::string&, const std::string&) const;