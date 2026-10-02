#include "Config.hpp"
#include "ConfigError.hpp"
#include "ConfigSchema.hpp"
#include "ConfigValidator.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cctype>
#include <cmath>
#include <limits>
#include <optional>
#include <regex>

namespace nlm {

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeDefaults();
    setLoggingEnabled(false);
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Config::initializeDefaults() {
    pImpl->isValid = true;
    pImpl->stats.totalEntries = 0;
    pImpl->stats.validationErrors = 0;
    for (size_t i = 0; i < 5; ++i) {
        pImpl->stats.entriesBySource[i] = 0;
    }
    pImpl->stats.lastValidation = std::chrono::system_clock::now();
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Fallback to simple format only (Phase 1 simplified implementation)
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileError("Cannot open config file", "Config.cpp", __LINE__, filepath);
        }
        
        std::string line;
        size_t lineNumber = 0;
        while (std::getline(file, line)) {
            ++lineNumber;
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
                
                if (key.empty()) {
                    throw ConfigError("Empty key at line " + std::to_string(lineNumber) + " in " + filepath,
                                     "Config.cpp", lineNumber, "File: " + filepath);
                }
                
                set(key, value, ConfigSource::File);
            } else {
                throw ConfigError("Invalid line " + std::to_string(lineNumber) + " in " + filepath + ": " + line,
                                 "Config.cpp", lineNumber, "File: " + filepath);
            }
        }
        
        pImpl->stats.entriesBySource[static_cast<int>(ConfigSource::File)] = pImpl->entries.size();
        return true;
        
    } catch (const ConfigError& e) {
        std::cerr << "Config Error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error loading config: " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        bool success = true;
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    if (key.empty()) {
                        throw ConfigError("Empty key in argument: " + arg, "Config.cpp", __LINE__);
                    }
                    
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    throw ConfigError("Invalid argument format: " + arg + " (missing =)", "Config.cpp", __LINE__);
                }
            } else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                if (key.empty()) {
                    throw ConfigError("Empty key in argument: " + arg, "Config.cpp", __LINE__);
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
        
        pImpl->stats.entriesBySource[static_cast<int>(ConfigSource::CommandLine)] = pImpl->entries.size();
        return success;
        
    } catch (const ConfigError& e) {
        std::cerr << "Config Error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error loading args: " << e.what() << std::endl;
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) {
    try {
        if (!isFileWritable(filepath)) {
            throw ConfigFileError("Cannot write config file", "Config.cpp", __LINE__, filepath);
        }
        
        createBackup(filepath);
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileError("Cannot open config file for writing", "Config.cpp", __LINE__, filepath);
        }
        
        file << "# Generated config file\n";
        file << "# Timestamp: " << getCurrentTimestamp() << "\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << valueToString(entry.value) << "\n\n";
        }
        
        file.close();
        
        if (isLoggingEnabled()) {
            std::cout << "Config saved to " << filepath << std::endl;
        }
        
        return true;
        
    } catch (const ConfigError& e) {
        std::cerr << "Config Error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error saving config: " << e.what() << std::endl;
        return false;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (key.empty()) {
        throw ConfigError("Config key cannot be empty", "Config.cpp", __LINE__);
    }
    
    ConfigValue oldValue;
    bool existed = false;
    
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            oldValue = entry.value;
            existed = true;
            break;
        }
    }
    
    ConfigEntry newEntry(key, value, source);
    
    if (existed) {
        newEntry.value = value;
        newEntry.source = source;
    }
    
    // Validate the new value
    if (!validateValue(key, value)) {
        throw ConfigValidationError("Validation failed for key: " + key, 
                                   "Config.cpp", __LINE__, key);
    }
    
    // Update the entry
    if (existed) {
        for (auto& entry : pImpl->entries) {
            if (entry.key == key) {
                entry.value = value;
                entry.source = source;
                entry.timestamp = std::chrono::system_clock::now();
                oldValue = value; // Update old value
                break;
            }
        }
    } else {
        pImpl->entries.push_back(newEntry);
    }
    
    // Track the change
    trackChange(key, oldValue, value, source, "Direct set operation");
    
    // Update stats
    pImpl->stats.totalEntries = pImpl->entries.size();
    pImpl->isValid = true;
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    ConfigValue oldValue;
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            oldValue = entry.value;
            break;
        }
    }
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    trackChange(key, oldValue, ConfigValue(), ConfigSource::Runtime, "Remove operation");
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Track changes before clearing
    for (const auto& entry : pImpl->entries) {
        trackChange(entry.key, entry.value, ConfigValue(), ConfigSource::Runtime, "Clear operation");
    }
    
    pImpl->entries.clear();
    pImpl->isValid = false;
}

bool Config::validateSchema() const {
    try {
        // Get all values as a map for schema validation
        std::unordered_map<std::string, std::variant<int, int64_t, double, bool, std::string>> values;
        for (const auto& entry : pImpl->entries) {
            // Convert ConfigValue to variant
            if (std::holds_alternative<int>(entry.value)) {
                values[entry.key] = std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                values[entry.key] = std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                values[entry.key] = std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                values[entry.key] = std::get<bool>(entry.value);
            } else if (std::holds_alternative<std::string>(entry.value)) {
                values[entry.key] = std::get<std::string>(entry.value);
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                // For arrays, skip schema validation
                continue;
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                continue;
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                continue;
            }
        }
        
        // Check against all registered schemas
        bool allValid = true;
        std::vector<std::string> allErrors;
        
        for (const auto& schemaName : ConfigSchemaRegistry::getInstance().getSchemaNames()) {
            auto errors = ConfigSchemaRegistry::getInstance().validate(schemaName, values);
            if (!errors.empty()) {
                allValid = false;
                allErrors.insert(allErrors.end(), errors.begin(), errors.end());
            }
        }
        
        pImpl->lastValidationErrors.clear();
        for (const auto& errorMsg : allErrors) {
            ErrorInfo errorInfo;
            errorInfo.code = ConfigErrorCode::SchemaValidationFailed;
            errorInfo.message = errorMsg;
            errorInfo.file = "Config.cpp";
            errorInfo.line = __LINE__;
            errorInfo.context = "Schema validation failed";
            errorInfo.timestamp = std::chrono::system_clock::now();
            pImpl->lastValidationErrors.push_back(errorInfo);
        }
        
        pImpl->stats.validationErrors = allErrors.size();
        pImpl->isValid = allValid;
        pImpl->stats.lastValidation = std::chrono::system_clock::now();
        
        if (pImpl->stats.validationErrors > 0 && isLoggingEnabled()) {
            std::cout << "Config validation failed with " << pImpl->stats.validationErrors << " errors" << std::endl;
            for (const auto& error : pImpl->lastValidationErrors) {
                std::cout << "  Error: " << error.message << std::endl;
            }
        }
        
        return allValid;
        
    } catch (const ConfigError& e) {
        std::cerr << "Config Error during schema validation: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error during schema validation: " << e.what() << std::endl;
        return false;
    }
}

std::vector<ErrorInfo> Config::getValidationErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->lastValidationErrors;
}

void Config::addSchema(const std::string& key, std::shared_ptr<ConfigSchema> schema) {
    if (!schema) {
        throw ConfigError("Schema cannot be null", "Config.cpp", __LINE__);
    }
    ConfigSchemaRegistry::getInstance().registerSchema(key, schema);
}

void Config::inheritFrom(const Config& other, const std::string& prefix) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& entry : other.pImpl->entries) {
        std::string newKey = prefix.empty() ? entry.key : prefix + "_" + entry.key;
        
        // Check if key already exists (if not, inherit)
        if (!has(newKey)) {
            ConfigEntry newEntry = entry;
            newEntry.source = ConfigSource::Inherited;
            pImpl->entries.push_back(newEntry);
            trackChange(newKey, ConfigValue(), entry.value, ConfigSource::Inherited, "Inheritance from parent config");
        }
    }
}

void Config::loadEnvironmentVariables() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    const char* home = std::getenv("HOME");
    if (!home) {
        if (isLoggingEnabled()) {
            std::cout << "HOME environment variable not set" << std::endl;
        }
        return;
    }
    
    std::string envFile = std::string(home) + "/.nlm/env.cfg";
    loadFromFile(envFile);
    
    // Also load from NLM_CONFIG environment variable
    const char* nlmConfig = std::getenv("NLM_CONFIG");
    if (nlmConfig) {
        loadFromFile(std::string(nlmConfig));
    }
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    ConfigValidator validator;
    
    // Find the entry to get metadata
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return true; // New entry, nothing to validate against
    }
    
    const ConfigEntry& entry = *it;
    
    std::vector<std::string> errors;
    
    // Type validation
    std::string type = entry.type;
    if (type.empty()) type = "auto";
    
    std::vector<std::string> typeErrors = validator.validateType(key, value, type);
    errors.insert(errors.end(), typeErrors.begin(), typeErrors.end());
    
    // Range validation
    if (entry.minValue.has_value() && entry.maxValue.has_value()) {
        std::vector<std::string> rangeErrors = validator.validateRange(key, value, entry.minValue.value(), entry.maxValue.value());
        errors.insert(errors.end(), rangeErrors.begin(), rangeErrors.end());
    }
    
    // Pattern validation
    if (!entry.pattern.empty()) {
        std::vector<std::string> patternErrors = validator.validatePattern(key, value, entry.pattern);
        errors.insert(errors.end(), patternErrors.begin(), patternErrors.end());
    }
    
    // Cross-parameter validation
    std::vector<std::string> crossErrors = validateCrossParameters(key);
    errors.insert(errors.end(), crossErrors.begin(), crossErrors.end());
    
    if (!errors.empty()) {
        pImpl->isValid = false;
        for (const auto& error : errors) {
            ErrorInfo errorInfo;
            errorInfo.code = ConfigErrorCode::SchemaValidationFailed;
            errorInfo.message = error;
            errorInfo.file = "Config.cpp";
            errorInfo.line = __LINE__;
            errorInfo.context = "Value validation for key: " + key;
            errorInfo.timestamp = std::chrono::system_clock::now();
            pImpl->lastValidationErrors.push_back(errorInfo);
        }
    }
    
    return errors.empty();
}

std::vector<std::string> Config::getValidationIssues() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> issues;
    for (const auto& error : pImpl->lastValidationErrors) {
        issues.push_back(error.message);
    }
    return issues;
}

std::vector<std::string> Config::validateCrossParameters(const std::string& key) const {
    std::vector<std::string> errors;
    
    // Find entry to get dependencies
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) return errors;
    
    const ConfigEntry& entry = *it;
    
    // Check if any dependency fields are present
    for (const auto& depField : entry.requiredFields) {
        auto depIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&depField](const ConfigEntry& e) { return e.key == depField; });
        
        if (depIt != pImpl->entries.end()) {
            // Dependency exists, check cross-parameter validation
            // This is a simple example - could be more complex
            if (depField == "max_value" && key == "min_value") {
                // Ensure min_value < max_value
                double minVal = extractNumericValue(entry.value);
                double maxVal = extractNumericValue(depIt->value);
                
                if (minVal >= maxVal) {
                    errors.push_back("Cross-parameter validation failed: min_value (" + 
                                   std::to_string(minVal) + ") must be less than max_value (" + 
                                   std::to_string(maxVal) + ")");
                }
            }
        }
    }
    
    return errors;
}

void Config::trackChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue,
                         ConfigSource source, const std::string& reason) {
    ConfigChange change;
    change.key = key;
    change.oldValue = oldValue;
    change.newValue = newValue;
    change.source = source;
    change.timestamp = std::chrono::system_clock::now();
    change.reason = reason;
    
    pImpl->changeHistory.push_back(change);
}

void Config::clearHistory() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->changeHistory.clear();
}

std::vector<ConfigChange> Config::getChangeHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->changeHistory;
}

ConfigStats Config::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

std::string Config::getChangeSummary(size_t maxHistory) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    oss << "Configuration change summary (last " << std::min(maxHistory, pImpl->changeHistory.size()) << " changes):\n";
    
    size_t count = 0;
    for (auto it = pImpl->changeHistory.rbegin(); 
         it != pImpl->changeHistory.rend() && count < maxHistory; ++it, ++count) {
        const ConfigChange& change = *it;
        oss << "  " << getCurrentTimestamp() << ": " << change.key;
        
        if (!change.reason.empty()) {
            oss << " (" << change.reason << ")";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

std::string Config::analyzeConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    
    oss << "Configuration Analysis:\n";
    oss << "======================\n\n";
    
    oss << "Summary Statistics:\n";
    oss << "  Total entries: " << pImpl->stats.totalEntries << "\n";
    oss << "  Valid configuration: " << (pImpl->isValid ? "Yes" : "No") << "\n";
    oss << "  Validation errors: " << pImpl->stats.validationErrors << "\n";
    oss << "  Change history size: " << pImpl->changeHistory.size() << "\n\n";
    
    oss << "Entries by source:\n";
    for (size_t i = 0; i < 5; ++i) {
        ConfigSource source = static_cast<ConfigSource>(i);
        oss << "  " << static_cast<int>(source) << ": " << pImpl->stats.entriesBySource[i] << "\n";
    }
    
    oss << "\nEntries:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = " << valueToString(entry.value);
        oss << " (source: " << static_cast<int>(entry.source) << ", desc: " << entry.description << ")\n";
    }
    
    if (!pImpl->lastValidationErrors.empty()) {
        oss << "\nValidation Errors:\n";
        for (const auto& error : pImpl->lastValidationErrors) {
            oss << "  " << error.message << "\n";
        }
    }
    
    return oss.str();
}

std::string Config::summary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
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

bool Config::isFileReadable(const std::string& filepath) const {
    return std::filesystem::exists(filepath) && std::filesystem::is_regular_file(filepath) &&
           std::filesystem::status(filepath).permissions() & std::filesystem::perms::owner_read;
}

bool Config::isFileWritable(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    std::filesystem::path parent = path.parent_path();
    
    if (!parent.empty() && !std::filesystem::exists(parent)) {
        std::filesystem::create_directories(parent);
    }
    
    return !std::filesystem::exists(path) ||
           (std::filesystem::status(path).permissions() & std::filesystem::perms::owner_write);
}

std::string Config::getBackupPath(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    return path.parent_path().string() + "/." + path.filename().string() + ".bak";
}

bool Config::createBackup(const std::string& filepath) {
    if (!isFileReadable(filepath)) {
        return false;
    }
    
    std::ifstream src(filepath, std::ios::binary);
    if (!src) {
        return false;
    }
    
    std::string backupPath = getBackupPath(filepath);
    std::ofstream dst(backupPath, std::ios::binary);
    if (!dst) {
        return false;
    }
    
    dst << src.rdbuf();
    return true;
}

bool Config::restoreFromBackup(const std::string& filepath) {
    std::string backupPath = getBackupPath(filepath);
    if (!std::filesystem::exists(backupPath)) {
        return false;
    }
    
    std::filesystem::remove(filepath);
    std::filesystem::rename(backupPath, filepath);
    return true;
}

std::string Config::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << " UTC";
    return oss.str();
}

std::optional<ConfigValue> Config::parseValue(const std::string& valueStr) {
    if (isBoolean(valueStr)) {
        return valueStr == "true" || valueStr == "1";
    }
    
    if (isNumeric(valueStr)) {
        size_t pos;
        double doubleVal = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            // Check if it's an integer
            try {
                int intVal = std::stoi(valueStr);
                return intVal;
            } catch (...) {
                // Could be a large integer
                try {
                    int64_t int64Val = std::stoll(valueStr);
                    return int64Val;
                } catch (...) {
                    return doubleVal;
                }
            }
        }
        return doubleVal;
    }
    
    return valueStr;
}

std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    if (std::holds_alternative<int>(value)) {
        oss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        oss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        oss << std::fixed << std::setprecision(6) << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        oss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        oss << std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        oss << "[";
        const auto& vec = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        oss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << std::fixed << std::setprecision(6) << vec[i];
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        oss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
    }
    return oss.str();
}

bool Config::compareValues(const ConfigValue& a, const ConfigValue& b) {
    if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) {
        return std::get<int>(a) == std::get<int>(b);
    } else if (std::holds_alternative<int64_t>(a) && std::holds_alternative<int64_t>(b)) {
        return std::get<int64_t>(a) == std::get<int64_t>(b);
    } else if (std::holds_alternative<double>(a) && std::holds_alternative<double>(b)) {
        return std::abs(std::get<double>(a) - std::get<double>(b)) < 1e-9;
    } else if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
        return std::get<bool>(a) == std::get<bool>(b);
    } else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)) {
        return std::get<std::string>(a) == std::get<std::string>(b);
    }
    return false;
}

ConfigErrorCode Config::detectErrorCode(const std::string& message) {
    if (message.find("FileNotFound") != std::string::npos) return ConfigErrorCode::FileNotFound;
    if (message.find("InvalidFileFormat") != std::string::npos) return ConfigErrorCode::InvalidFileFormat;
    if (message.find("MissingRequiredField") != std::string::npos) return ConfigErrorCode::MissingRequiredField;
    if (message.find("InvalidType") != std::string::npos) return ConfigErrorCode::InvalidType;
    if (message.find("OutOfRange") != std::string::npos) return ConfigErrorCode::OutOfRange;
    if (message.find("PermissionDenied") != std::string::npos) return ConfigErrorCode::PermissionDenied;
    if (message.find("FileWriteError") != std::string::npos) return ConfigErrorCode::FileWriteError;
    if (message.find("SchemaValidationFailed") != std::string::npos) return ConfigErrorCode::SchemaValidationFailed;
    if (message.find("MissingEnvironmentVariable") != std::string::npos) return ConfigErrorCode::MissingEnvironmentVariable;
    if (message.find("InvalidEnvironmentValue") != std::string::npos) return ConfigErrorCode::InvalidEnvironmentValue;
    return ConfigErrorCode::UnknownError;
}

bool Config::validateValueType(const std::string& key, const ConfigValue& value) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return true; // New entry, nothing to validate against
    }
    
    const ConfigEntry& entry = *it;
    
    // Convert field type to variant type name
    std::string fieldType = entry.type;
    if (fieldType.empty()) fieldType = "auto";
    
    // Simple type checking
    if (fieldType == "int" && !std::holds_alternative<int>(value) && 
        !std::holds_alternative<int64_t>(value) && 
        !(std::holds_alternative<double>(value) && std::floor(std::get<double>(value)) == std::get<double>(value))) {
        return false;
    }
    
    if (fieldType == "double" && !std::holds_alternative<int>(value) && 
        !std::holds_alternative<int64_t>(value) && 
        !std::holds_alternative<double>(value)) {
        return false;
    }
    
    if (fieldType == "bool" && !std::holds_alternative<bool>(value)) {
        return false;
    }
    
    if (fieldType == "string" && !std::holds_alternative<std::string>(value)) {
        return false;
    }
    
    return true;
}

bool Config::validateValueRange(const std::string& key, const ConfigValue& value) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end() || !entry->minValue.has_value() || !entry->maxValue.has_value()) {
        return true;
    }
    
    const ConfigEntry& entry = *it;
    
    double numValue = extractNumericValue(value);
    if (numValue == -std::numeric_limits<double>::max()) {
        return false;
    }
    
    return numValue >= entry.minValue.value() && numValue <= entry.maxValue.value();
}

bool Config::validateValuePattern(const std::string& key, const ConfigValue& value) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end() || entry->pattern.empty() || !std::holds_alternative<std::string>(value)) {
        return true;
    }
    
    const ConfigEntry& entry = *it;
    std::string strValue = std::get<std::string>(value);
    
    try {
        std::regex regex(entry.pattern);
        return std::regex_match(strValue, regex);
    } catch (...) {
        return false;
    }
}

double Config::extractNumericValue(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return static_cast<double>(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    }
    return -std::numeric_limits<double>::max();
}

bool Config::isLoggingEnabled() const {
    return pImpl->isLoggingEnabled;
}

void Config::setLoggingEnabled(bool enabled) {
    pImpl->isLoggingEnabled = enabled;
}

size_t Config::getEstimatedMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    size_t total = 0;
    for (const auto& entry : pImpl->entries) {
        total += sizeof(ConfigEntry) + entry.key.capacity() + entry.description.capacity();
        // Estimate ConfigValue size
        std::visit([&total](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            total += sizeof(T);
            if constexpr (std::is_same_v<T, std::string>) {
                total += arg.capacity();
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                total += arg.capacity() * sizeof(int);
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                total += arg.capacity() * sizeof(double);
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                for (const auto& str : arg) {
                    total += str.capacity();
                }
            }
        }, entry.value);
    }
    return total;
}

double Config::getValidationTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->stats.lastValidation).count();
    return static_cast<double>(duration);
}

bool Config::equals(const Config& other) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (otherIt == other.pImpl->entries.end()) {
            return false;
        }
        
        if (!compareValues(entry.value, otherIt->value)) {
            return false;
        }
    }
    
    return true;
}

void Config::logChanges(std::ostream& output) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->changeHistory.empty()) {
        output << "No changes recorded\n";
        return;
    }
    
    output << "Recent changes (last " << std::min(10, static_cast<int>(pImpl->changeHistory.size())) << "):\n";
    
    size_t count = 0;
    for (auto it = pImpl->changeHistory.rbegin(); 
         it != pImpl->changeHistory.rend() && count < 10; ++it, ++count) {
        const ConfigChange& change = *it;
        output << "  " << getCurrentTimestamp() << ": " << change.key;
        
        if (!change.reason.empty()) {
            output << " (" << change.reason << ")";
        }
        
        output << "\n";
    }
}

} // namespace nlm