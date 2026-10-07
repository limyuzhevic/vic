#include "EnhancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <openssl/sha.h>

namespace nlm {

struct EnhancedConfig::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigConstraint> constraints;
    std::vector<ConfigOption> defaultOptions;
    std::map<std::string, EnhancedConfig> templates;
    std::vector<Observer> observers;
    
    std::map<std::string, size_t> accessCount;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastModified;
    size_t loadCount;
    size_t saveCount;
    size_t validationCount;
    double averageValidationTime;
    
    std::string version;
    SecurityLevel securityLevel;
    std::vector<std::string> encryptedKeys;
    
    ConfigStats stats;
    
    Impl() : loadCount(0), saveCount(0), validationCount(0), averageValidationTime(0.0),
             securityLevel(SecurityLevel::Public), created(std::chrono::system_clock::now()),
             lastModified(created) {}
};

EnhancedConfig::EnhancedConfig() : pImpl(std::make_unique<Impl>()), logLevel(LogLevel::Info) {
    NLM_LOG_INFO("EnhancedConfig initialized");
}

EnhancedConfig::~EnhancedConfig() {
    NLM_LOG_INFO("EnhancedConfig destroyed");
}

EnhancedConfig::EnhancedConfig(EnhancedConfig&& other) noexcept : 
    pImpl(std::move(other.pImpl)), logger(std::move(other.logger)), logLevel(other.logLevel) {
}

EnhancedConfig& EnhancedConfig::operator=(EnhancedConfig&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        logger = std::move(other.logger);
        logLevel = other.logLevel;
    }
    return *this;
}

bool EnhancedConfig::loadFromFile(const std::string& filepath) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Loading configuration from file: " + filepath);
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
            return false;
        }
        
        // Try to parse as JSON first
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        if (!content.empty() && content[0] == '{') {
            // JSON format
            bool success = fromJSON(content);
            if (!success) {
                NLM_LOG_WARNING("Failed to parse JSON, trying legacy format");
                // Fall back to legacy format
                std::ifstream file2(filepath);
                if (!file2.is_open()) {
                    return false;
                }
                return loadFromLegacyFile(file2);
            }
        } else {
            // Legacy format
            std::ifstream file2(filepath);
            if (!file2.is_open()) {
                return false;
            }
            return loadFromLegacyFile(file2);
        }
        
        pImpl->lastModified = std::chrono::system_clock::now();
        pImpl->loadCount++;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        logMessage("Configuration loaded from file: " + filepath + " (" + std::to_string(duration.count()) + "ms)", LogLevel::Info);
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading configuration: ") + e.what());
        return false;
    }
}

bool EnhancedConfig::loadFromLegacyFile(std::ifstream& file) {
    std::string line;
    size_t lineNumber = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
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
        } else {
            NLM_LOG_WARNING("Invalid configuration line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    return success;
}

bool EnhancedConfig::loadFromEnvironment(const std::string& prefix) {
    NLM_LOG_INFO("Loading configuration from environment variables (prefix: " + prefix + ")");
    
    size_t envCount = 0;
    size_t validCount = 0;
    
    for (const auto& envPair : std::filesystem::current_path() == "/" ? 
         std::map<std::string, std::string>() : std::map<std::string, std::string>()) {
        // In a real implementation, we would iterate over actual environment variables
        // For now, this is a placeholder
    }
    
    // Simulate environment variable loading
    const std::map<std::string, std::string> mockEnvVars = {
        {"NLM_RANDOM_SEED", "42"},
        {"NLM_SIMULATION_TIMESTEP", "0.001"},
        {"NLM_NEURON_COUNT", "1000"},
        {"NLM_CONNECTION_PROBABILITY", "0.1"},
        {"NLM_ENABLE_DEVELOPMENT", "true"},
        {"NLM_CHECKPOINT_DIR", "./checkpoints"}
    };
    
    for (const auto& envPair : mockEnvVars) {
        envCount++;
        if (envPair.first.find(prefix) == 0) {
            std::string key = envPair.first.substr(prefix.length());
            std::string value = envPair.second;
            
            if (setFromEnvironmentVariable(key, value)) {
                validCount++;
                NLM_LOG_INFO("Loaded from environment: " + key + " = " + value);
            } else {
                NLM_LOG_WARNING("Failed to parse environment variable: " + key + " = " + value);
            }
        }
    }
    
    NLM_LOG_INFO("Environment loading complete: " + std::to_string(validCount) + "/" + std::to_string(envCount) + " variables loaded");
    
    return validCount > 0;
}

bool EnhancedConfig::setFromEnvironmentVariable(const std::string& key, const std::string& value) {
    try {
        ConfigValue configValue;
        if (stringToValue(value, configValue)) {
            set(key, configValue, ConfigSource::Environment);
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting environment variable " + key + ": " + e.what());
        return false;
    }
}

bool EnhancedConfig::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration from command line arguments");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: " + key + " = " + value);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            NLM_LOG_INFO("Command line: " + key + " = " + value);
        }
        // Handle boolean flags
        else if (arg == "--help" || arg == "--version" || arg == "--verbose") {
            set(arg.substr(2), true, ConfigSource::CommandLine);
            NLM_LOG_INFO("Command line flag: " + arg);
        }
    }
    
    return true;
}

bool EnhancedConfig::saveToFile(const std::string& filepath) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Saving configuration to file: " + filepath);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to create configuration file: " + filepath);
            return false;
        }
        
        // Write in JSON format
        file << toJSON();
        
        file.close();
        
        pImpl->saveCount++;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        logMessage("Configuration saved to file: " + filepath + " (" + std::to_string(duration.count()) + "ms)", LogLevel::Info);
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving configuration: ") + e.what());
        return false;
    }
}

template<typename T>
std::optional<T> EnhancedConfig::get(const std::string& key) const {
    auto lock = LockGuard(pImpl->configMutex);
    
    pImpl->accessCount[key]++;
    
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
T EnhancedConfig::getOr(const std::string& key, const T& defaultValue) const {
    auto lock = LockGuard(pImpl->configMutex);
    
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void EnhancedConfig::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto lock = LockGuard(pImpl->configMutex);
    
    std::string sanitizedKey = sanitizeKey(key);
    
    // Check if key is reserved
    if (isKeyReserved(sanitizedKey)) {
        NLM_LOG_WARNING("Attempted to set reserved key: " + sanitizedKey);
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&sanitizedKey](const ConfigEntry& e) { return e.key == sanitizedKey; });n
    
    ConfigValue oldValue;
    bool existed = false;
    
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
        existed = true;
        
        if (oldValue == value) {
            NLM_LOG_DEBUG("Setting unchanged value for key: " + sanitizedKey);
        }
        
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(sanitizedKey, value, source);
        NLM_LOG_DEBUG("Added new configuration entry: " + sanitizedKey);
    }
    
    pImpl->lastModified = std::chrono::system_clock::now();
    
    // Notify observers
    if (existed) {
        ConfigChange change(sanitizedKey, oldValue, value, source);
        notifyObservers(change);
    }
    
    // Validate if this key has constraints
    validateKey(sanitizedKey);
}

void EnhancedConfig::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void EnhancedConfig::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void EnhancedConfig::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void EnhancedConfig::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void EnhancedConfig::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

template<typename T>
bool EnhancedConfig::setValidated(const std::string& key, const T& value, ConfigSource source) {
    auto lock = LockGuard(pImpl->configMutex);
    
    std::string sanitizedKey = sanitizeKey(key);
    
    // Validate against constraints
    ValidationResult validationResult = validateKey(sanitizedKey);
    if (!validationResult.isValid) {
        NLM_LOG_ERROR("Validation failed for key " + sanitizedKey + ": " + validationResult.errorMessage);
        return false;
    }
    
    set(sanitizedKey, ConfigValue(value), source);
    return true;
}

bool EnhancedConfig::has(const std::string& key) const {
    auto lock = LockGuard(pImpl->configMutex);
    
    std::string sanitizedKey = sanitizeKey(key);
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&sanitizedKey](const ConfigEntry& e) { return e.key == sanitizedKey; });
}

void EnhancedConfig::remove(const std::string& key) {
    auto lock = LockGuard(pImpl->configMutex);
    
    std::string sanitizedKey = sanitizeKey(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&sanitizedKey](const ConfigEntry& e) { return e.key == sanitizedKey; });n
    
    if (it != pImpl->entries.end()) {
        ConfigValue oldValue = it->value;
        pImpl->entries.erase(it);
        
        NLM_LOG_INFO("Removed configuration entry: " + sanitizedKey);
        
        // Notify observers
        ConfigChange change(sanitizedKey, oldValue, ConfigValue(nullptr), ConfigSource::Runtime);
        notifyObservers(change);
    }
}

std::vector<std::string> EnhancedConfig::getKeys() const {
    auto lock = LockGuard(pImpl->configMutex);
    
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> EnhancedConfig::getKeys(ConfigSource source) const {
    auto lock = LockGuard(pImpl->configMutex);
    
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::string EnhancedConfig::summary() const {
    auto lock = LockGuard(pImpl->configMutex);
    
    std::ostringstream oss;
    oss << "Enhanced Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "Version: " << pImpl->version << "\n";
    oss << "Created: " << formatTime(pImpl->created) << "\n";
    oss << "Last Modified: " << formatTime(pImpl->lastModified) << "\n";
    oss << "Security Level: " << static_cast<int>(pImpl->securityLevel) << "\n";
    oss << "Validation Errors: " << pImpl->stats.validationErrors << "\n";
    
    oss << "\nEntries by Source:\n";
    oss << "  Default: " << getSourceCount(ConfigSource::Default) << "\n";
    oss << "  File: " << getSourceCount(ConfigSource::File) << "\n";
    oss << "  Command Line: " << getSourceCount(ConfigSource::CommandLine) << "\n";
    oss << "  Runtime: " << getSourceCount(ConfigSource::Runtime) << "\n";
    oss << "  Environment: " << getSourceCount(ConfigSource::Environment) << "\n";
    
    oss << "\nAll Entries:\n";
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

std::string EnhancedConfig::help() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration Help\n";
    oss << "Version: " << pImpl->version << "\n";
    oss << "\nConfiguration Options:\n";
    
    // Show default options
    for (const auto& option : pImpl->defaultOptions) {
        oss << "  --" << option.key << " " << option.example << " (default: ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, option.defaultValue);
        oss << ") - " << option.description << "\n";
    }
    
    oss << "\nEnvironment Variables:\n";
    oss << "  NLM_<KEY> - Set configuration value from environment\n";
    oss << "\nFile Format:\n";
    oss << "  key = value\n";
    oss << "  # Comments start with # or //\n";
    oss << "  " << std::string(50, '=') << "\n";
    
    oss << "\nUsage Examples:\n";
    oss << "  ./program --neuron_count=1000 --simulation_timestep=0.001\n";
    oss << "  NLM_NEURON_COUNT=2000 ./program\n";
    oss << "  config_file.cfg contains configuration\n";
    
    return oss.str();
}

bool EnhancedConfig::validate() const {
    auto lock = LockGuard(pImpl->configMutex);
    
    size_t errorCount = 0;
    size_t warningCount = 0;
    
    // Validate each entry
    for (const auto& entry : pImpl->entries) {
        ValidationResult result = validateKey(entry.key);
        if (!result.isValid) {
            NLM_LOG_ERROR("Validation error for key " + entry.key + ": " + result.errorMessage);
            if (result.suggestion.empty()) {
                NLM_LOG_ERROR("  Suggestion: " + result.suggestion);
            }
            errorCount++;
        }
    }
    
    // Check for missing required keys
    for (const auto& option : pImpl->defaultOptions) {
        if (option.required && !has(option.key)) {
            NLM_LOG_ERROR("Missing required configuration: " + option.key);
            errorCount++;
        }
    }
    
    pImpl->stats.validationErrors = errorCount;
    pImpl->validationCount++;
    
    bool isValid = errorCount == 0;
    
    if (isValid) {
        NLM_LOG_INFO("Configuration validation passed");
    } else {
        NLM_LOG_ERROR("Configuration validation failed with " + std::to_string(errorCount) + " errors");
    }
    
    return isValid;
}

std::vector<std::string> EnhancedConfig::getValidationErrors() const {
    auto lock = LockGuard(pImpl->configMutex);
    
    std::vector<std::string> errors;
    
    // Validate each entry
    for (const auto& entry : pImpl->entries) {
        ValidationResult result = validateKey(entry.key);
        if (!result.isValid) {
            errors.push_back("Key '" + entry.key + "': " + result.errorMessage);
        }
    }
    
    return errors;
}

void EnhancedConfig::clear() {
    auto lock = LockGuard(pImpl->configMutex);
    
    NLM_LOG_INFO("Clearing configuration");
    
    pImpl->entries.clear();
    pImpl->constraints.clear();
    
    NLM_LOG_INFO("Configuration cleared");
}

void EnhancedConfig::merge(const EnhancedConfig& other) {
    auto lock = LockGuard(pImpl->configMutex);
    auto otherLock = LockGuard(other.pImpl->configMutex);
    
    NLM_LOG_INFO("Merging configuration");
    
    for (const auto& otherEntry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&otherEntry](const ConfigEntry& e) { return e.key == otherEntry.key; });
        
        if (it == pImpl->entries.end()) {
            // Add new entry
            pImpl->entries.push_back(otherEntry);
            NLM_LOG_INFO("Added configuration from merged source: " + otherEntry.key);
        } else {
            // Override existing entry (other takes precedence)
            it->value = otherEntry.value;
            it->source = otherEntry.source;
            NLM_LOG_INFO("Overrode configuration with merged source: " + otherEntry.key);
        }
    }
    
    pImpl->lastModified = std::chrono::system_clock::now();
}

void EnhancedConfig::applyDefaults(const std::vector<ConfigOption>& defaults) {
    auto lock = LockGuard(pImpl->configMutex);
    
    NLM_LOG_INFO("Applying default configuration values");
    
    for (const auto& option : defaults) {
        if (!has(option.key)) {
            // Apply default value
            set(option.key, option.defaultValue, option.source);
            NLM_LOG_INFO("Applied default: " + option.key);
        } else {
            NLM_LOG_DEBUG("Configuration already has value for key: " + option.key);
        }
    }
    
    // Store defaults for help
    pImpl->defaultOptions = defaults;
}

void EnhancedConfig::addConstraint(const ConfigConstraint& constraint) {
    auto lock = LockGuard(pImpl->configMutex);
    
    NLM_LOG_INFO("Adding constraint for key: " + constraint.key);
    
    pImpl->constraints.push_back(constraint);
}

ConfigSource EnhancedConfig::getSource(const std::string& key) const {
    auto lock = LockGuard(pImpl->configMutex);
    
    std::string sanitizedKey = sanitizeKey(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&sanitizedKey](const ConfigEntry& e) { return e.key == sanitizedKey; });
    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    
    return ConfigSource::Default;
}

bool EnhancedConfig::fromEnvironment(const std::string& key) const {
    auto lock = LockGuard(pImpl->configMutex);
    
    std::string envVarName = environmentVariableName(key);
    
    // Check if this key comes from environment
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&envVarName](const ConfigEntry& e) { return e.key == key && e.source == ConfigSource::Environment; });n
    
    return it != pImpl->entries.end();
}

std::string EnhancedConfig::environmentVariableName(const std::string& key, const std::string& prefix) {
    std::string envVar = prefix + key;
    
    // Convert to uppercase
    std::transform(envVar.begin(), envVar.end(), envVar.begin(), ::toupper);
    
    // Replace dashes and underscores with underscores
    std::replace(envVar.begin(), envVar.end(), '-', '_');
    
    return envVar;
}

std::string EnhancedConfig::toJSON() const {
    auto lock = LockGuard(pImpl->configMutex);
    
    nlohmann::json json;
    
    // Add version
    json["version"] = pImpl->version;
    
    // Add entries
    json["entries"] = nlohmann::json::array();
    for (const auto& entry : pImpl->entries) {
        nlohmann::json entryJson;
        entryJson["key"] = entry.key;
        
        // Convert ConfigValue to JSON
        entryJson["value"] = valueToJSON(entry.value);
        entryJson["source"] = static_cast<int>(entry.source);
        entryJson["description"] = entry.description;
        
        json["entries"].push_back(entryJson);
    }
    
    // Add metadata
    json["metadata"]["created"] = formatTime(pImpl->created);
    json["metadata"]["lastModified"] = formatTime(pImpl->lastModified);
    json["metadata"]["version"] = pImpl->version;
    
    return json.dump(2); // Pretty print with 2-space indentation
}

bool EnhancedConfig::fromJSON(const std::string& jsonStr) {
    try {
        nlohmann::json json = nlohmann::json::parse(jsonStr);
        
        // Clear existing entries
        clear();
        
        // Load version
        if (json.contains("version")) {
            pImpl->version = json["version"];
        }
        
        // Load entries
        if (json.contains("entries")) {
            for (const auto& entryJson : json["entries"]) {
                std::string key = entryJson["key"];
                ConfigValue value = valueFromJSON(entryJson["value"]);
                ConfigSource source = static_cast<ConfigSource>(entryJson["source"].get<int>());
                std::string description = entryJson.value("description", "");
                
                set(key, value, source);
                
                // Note: description is not currently stored in ConfigEntry
                // This could be added to the ConfigEntry struct
            }
        }
        
        pImpl->lastModified = std::chrono::system_clock::now();
        
        NLM_LOG_INFO("Configuration loaded from JSON");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception parsing JSON: ") + e.what());
        return false;
    }
}

std::string EnhancedConfig::formatTime(const std::chrono::system_clock::time_point& time) {
    auto inTimeT = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&inTimeT), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string EnhancedConfig::valueToJSON(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> ||
                      std::is_same_v<T, double> || std::is_same_v<T, bool>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
            return result;
        } else {
            return "null";
        }
    }, value);
}

ConfigValue EnhancedConfig::valueFromJSON(const nlohmann::json& json) const {
    if (json.is_null()) {
        return ConfigValue();
    } else if (json.is_number_integer()) {
        return json.get<int64_t>();
    } else if (json.is_number_float()) {
        return json.get<double>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_array()) {
        // Determine array type based on elements
        if (json.empty()) {
            return std::vector<int>();
        }
        
        // Check if all elements are integers
        bool allIntegers = true;
        bool allDoubles = true;
        bool allStrings = true;
        
        for (const auto& element : json) {
            if (!element.is_number_integer()) allIntegers = false;
            if (!element.is_number_float()) allDoubles = false;
            if (!element.is_string()) allStrings = false;
        }
        
        if (allIntegers) {
            std::vector<int64_t> result;
            for (const auto& element : json) {
                result.push_back(element.get<int64_t>());
            }
            return result;
        } else if (allDoubles) {
            std::vector<double> result;
            for (const auto& element : json) {
                result.push_back(element.get<double>());
            }
            return result;
        } else if (allStrings) {
            std::vector<std::string> result;
            for (const auto& element : json) {
                result.push_back(element.get<std::string>());
            }
            return result;
        }
        
        // Default to integers if types are mixed
        std::vector<int64_t> result;
        for (const auto& element : json) {
            if (element.is_number_integer()) {
                result.push_back(element.get<int64_t>());
            } else if (element.is_number_float()) {
                result.push_back(static_cast<int64_t>(element.get<double>()));
            } else {
                result.push_back(0); // Default for unknown types
            }
        }
        return result;
    }
    
    return ConfigValue();
}

ValidationResult EnhancedConfig::validateKey(const std::string& key) const {
    // Validate against constraints
    for (const auto& constraint : pImpl->constraints) {
        if (constraint.key == key) {
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            
            if (it != pImpl->entries.end()) {
                ValidationResult result = constraint.validator(it->value);
                if (!result.isValid) {
                    return result;
                }
            } else {
                // Key doesn't exist, check if it's required
                if (constraint.source == ConfigSource::File || constraint.source == ConfigSource::CommandLine) {
                    return ValidationResult(false, "Required key not found: " + key);
                }
            }
        }
    }
    
    // Validate against type-specific rules
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entryIt != pImpl->entries.end()) {
        ValidationResult typeResult = validateTypeConstraints(key, entryIt->value);
        if (!typeResult.isValid) {
            return typeResult;
        }
    }
    
    return ValidationResult(true);
}

ValidationResult EnhancedConfig::validateTypeConstraints(const std::string& key, const ConfigValue& value) const {
    // Find corresponding default option for type information
    for (const auto& option : pImpl->defaultOptions) {
        if (option.key == key) {
            // Check type
            bool typeMatches = false;
            
            if (std::holds_alternative<int>(value) && option.typeName == "int") typeMatches = true;
            else if (std::holds_alternative<int64_t>(value) && option.typeName == "int64_t") typeMatches = true;
            else if (std::holds_alternative<double>(value) && option.typeName == "double") typeMatches = true;
            else if (std::holds_alternative<bool>(value) && option.typeName == "bool") typeMatches = true;
            else if (std::holds_alternative<std::string>(value) && option.typeName == "string") typeMatches = true;
            else if (std::holds_alternative<std::vector<int>>(value) && option.typeName == "std::vector<int>") typeMatches = true;
            else if (std::holds_alternative<std::vector<double>>(value) && option.typeName == "std::vector<double>") typeMatches = true;
            else if (std::holds_alternative<std::vector<std::string>>(value) && option.typeName == "std::vector<std::string>") typeMatches = true;
            
            if (!typeMatches) {
                return ValidationResult(false, "Type mismatch for key " + key + ". Expected " + option.typeName + ".");
            }
            
            // Check range constraints
            if (option.minValue > 0 || option.maxValue > 0) {
                double numericValue = 0.0;
                if (std::holds_alternative<int>(value)) numericValue = static_cast<double>(std::get<int>(value));
                else if (std::holds_alternative<int64_t>(value)) numericValue = static_cast<double>(std::get<int64_t>(value));
                else if (std::holds_alternative<double>(value)) numericValue = std::get<double>(value);
                
                if (option.minValue > 0 && numericValue < option.minValue) {
                    return ValidationResult(false, "Value too small for key " + key + ". Minimum: " + std::to_string(option.minValue));
                }
                
                if (option.maxValue > 0 && numericValue > option.maxValue) {
                    return ValidationResult(false, "Value too large for key " + key + ". Maximum: " + std::to_string(option.maxValue));
                }
            }
            
            // Check valid values for string/enum types
            if (!option.validValues.empty() && std::holds_alternative<std::string>(value)) {
                std::string strValue = std::get<std::string>(value);
                bool valid = false;
                for (const auto& validValue : option.validValues) {
                    if (strValue == validValue) {
                        valid = true;
                        break;
                    }
                }
                
                if (!valid) {
                    return ValidationResult(false, "Invalid value for key " + key + ". Valid values: " + 
                                                formatStringList(option.validValues));
                }
            }
            
            break;
        }
    }
    
    return ValidationResult(true);
}

void EnhancedConfig::notifyObservers(const ConfigChange& change) {
    for (const auto& observer : pImpl->observers) {
        try {
            observer(change);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Observer error: ") + e.what());
        }
    }
}

void EnhancedConfig::logMessage(const std::string& message, LogLevel level) {
    if (logger && level >= logLevel) {
        logger->log(message, level);
    }
}

std::string EnhancedConfig::formatStringList(const std::vector<std::string>& items) const {
    if (items.empty()) return "";
    
    std::string result = items[0];
    for (size_t i = 1; i < items.size(); ++i) {
        result += ", " + items[i];
    }
    
    return result;
}

// Helper functions implementation

std::string EnhancedConfig::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string EnhancedConfig::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string EnhancedConfig::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string EnhancedConfig::convertValueToString(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
            return result;
        } else {
            return "";
        }
    }, value);
}

bool EnhancedConfig::stringToValue(const std::string& str, ConfigValue& value) {
    if (str.empty()) {
        value = std::string();
        return true;
    }
    
    // Try to parse as boolean
    if (str == "true" || str == "false") {
        value = (str == "true");
        return true;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intValue = std::stoi(str, &pos);
        if (pos == str.size()) {
            value = intValue;
            return true;
        }
    } catch (...) {}
    
    // Try to parse as int64_t
    try {
        size_t pos;
        int64_t int64Value = std::stoll(str, &pos);
        if (pos == str.size()) {
            value = int64Value;
            return true;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleValue = std::stod(str, &pos);
        if (pos == str.size()) {
            value = doubleValue;
            return true;
        }
    } catch (...) {}
    
    // Default to string
    value = str;
    return true;
}

std::string EnhancedConfig::sanitizeKey(const std::string& key) {
    std::string result = key;
    
    // Remove leading/trailing whitespace
    result = trim(result);
    
    // Convert to lowercase for consistency
    result = toLower(result);
    
    return result;
}

bool EnhancedConfig::isKeyReserved(const std::string& key) {
    static const std::vector<std::string> reservedKeys = {
        "help", "version", "verbose", "debug", "test", "benchmark",
        "config", "file", "environment", "load", "save", "validate",
        "list", "clear", "merge", "template", "preset", "encrypt"
    };
    
    return std::find(reservedKeys.begin(), reservedKeys.end(), key) != reservedKeys.end();
}

// Validation helper functions
ValidationResult EnhancedConfig::validateKey(const std::string& key) const {
    return validateTypeConstraints(key, getValue(key));
}

ConfigValue EnhancedConfig::getValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->value;
    }
    
    return ConfigValue();
}

// Logging setup
void EnhancedConfig::setLogLevel(LogLevel level) {
    logLevel = level;
}

EnhancedConfig::LogLevel EnhancedConfig::getLogLevel() const {
    return logLevel;
}

void EnhancedConfig::setLogger(std::unique_ptr<Logger> logger) {
    this->logger = std::move(logger);
}

// Security setup
void EnhancedConfig::setSecurityLevel(SecurityLevel level) {
    pImpl->securityLevel = level;
}

EnhancedConfig::SecurityLevel EnhancedConfig::getSecurityLevel() const {
    return pImpl->securityLevel;
}

void EnhancedConfig::setEncryptedKeys(const std::vector<std::string>& keys) {
    pImpl->encryptedKeys = keys;
}

bool EnhancedConfig::isEncrypted(const std::string& key) const {
    return std::find(pImpl->encryptedKeys.begin(), pImpl->encryptedKeys.end(), key) != pImpl->encryptedKeys.end();
}

// Configuration history
void EnhancedConfig::recordChange(const std::string& key, const ConfigValue& oldValue, 
                                 const ConfigValue& newValue, ConfigSource source, 
                                 const std::string& description) {
    ConfigChange change(key, oldValue, newValue, source, description);
    // Note: History tracking would be added to pImpl if needed
}

// Observers
void EnhancedConfig::addObserver(const Observer& observer) {
    pImpl->observers.push_back(observer);
}

void EnhancedConfig::removeObserver(const Observer& observer) {
    auto it = std::find(pImpl->observers.begin(), pImpl->observers.end(), observer);
    if (it != pImpl->observers.end()) {
        pImpl->observers.erase(it);
    }
}

// Configuration metrics
EnhancedConfig::ConfigStats EnhancedConfig::getStats() const {
    auto lock = LockGuard(pImpl->configMutex);
    
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    stats.fromFile = getSourceCount(ConfigSource::File);
    stats.fromCommandLine = getSourceCount(ConfigSource::CommandLine);
    stats.fromRuntime = getSourceCount(ConfigSource::Runtime);
    stats.fromEnvironment = getSourceCount(ConfigSource::Environment);
    stats.validationErrors = pImpl->stats.validationErrors;
    stats.lastModifiedTimestamp = pImpl->lastModified.time_since_epoch().count();
    
    return stats;
}

size_t EnhancedConfig::getSourceCount(ConfigSource source) const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [source](const ConfigEntry& e) { return e.source == source; });
}

} // namespace nlm
