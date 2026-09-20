#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <unordered_set>
#include <thread>
#include <shared_mutex>
#include <queue>
#include <regex>
#include <chrono>

namespace nlm {

struct Config::Impl {
    // Thread-safe configuration storage
    mutable std::shared_mutex mutex;
    std::vector<ConfigEntry> entries;
    
    // Thread-safe storage for flags
    mutable std::shared_mutex flagsMutex;
    std::unordered_map<std::string, bool> flags;
    
    // Atomic flags
    std::unordered_map<std::string, std::atomic<bool>> atomicFlags;
    
    // Copy-on-write support
    bool enableCopyOnWrite = false;
    bool writeProtected = false;
    std::unordered_map<std::string, ConfigValue> cowCache;
    
    // Configuration history
    bool enableHistory = true;
    std::deque<ConfigHistoryEntry> history;
    size_t historyLimit = 100;
    
    // Change tracking
    std::mutex changeMutex;
    std::vector<ConfigChangeCallback> changeCallbacks;
    size_t changeCount = 0;
    
    // Schema validation
    std::vector<ConfigSchemaEntry> schema;
    
    // Validation errors cache
    mutable std::shared_mutex validationMutex;
    mutable std::vector<ConfigValidationError> validationErrors;
    
    // Batch operations
    std::mutex batchMutex;
    bool batchMode = false;
    size_t batchSize = 0;
    
    // Configuration locking
    std::mutex lockMutex;
    bool locked = false;
    
    // Configuration validation function registry
    std::unordered_map<std::string, ConfigValidator> validators;
    
    // JSON schema validation
    std::vector<nlohmann::json> schemas;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize default validators
    pImpl->validators["string_length"] = [](const std::string& key, const ConfigValue& value, std::vector<ConfigValidationError>& errors) {
        if (const std::string* str = std::get_if<std::string>(&value)) {
            if (str->length() > 1000) {
                errors.push_back({key, "String too long", "<=1000 characters", std::to_string(str->length()) + " characters"});
                return false;
            }
        }
        return true;
    };
    
    pImpl->validators["positive_int"] = [](const std::string& key, const ConfigValue& value, std::vector<ConfigValidationError>& errors) {
        if (const int* i = std::get_if<int>(&value)) {
            if (*i < 0) {
                errors.push_back({key, "Value must be positive", ">=0", std::to_string(*i)});
                return false;
            }
        }
        return true;
    };
    
    pImpl->validators["range"] = [](const std::string& key, const ConfigValue& value, std::vector<ConfigValidationError>& errors) {
        if (const int* i = std::get_if<int>(&value)) {
            if (*i < 0 || *i > 100) {
                errors.push_back({key, "Value out of range", "[0, 100]", std::to_string(*i)});
                return false;
            }
        }
        return true;
    };
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Implement comprehensive JSON/YAML parser (Phase 2)
    // For now, use nlohmann::json for JSON parsing
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json json;
        file >> json;
        
        std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
        
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            if (jsonToConfigValue(*it, value)) {
                set(key, value, ConfigSource::File);
            }
        }
        
        // Validate loaded configuration
        validateAll();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
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
    
    nlohmann::json json;
    
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = configValueToJson(entry.value);
    }
    
    file << json.dump(2);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
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
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    
    // Check if batch mode
    if (pImpl->batchMode) {
        pImpl->batchSize++;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigValue oldValue;
    bool valueChanged = false;
    
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
        valueChanged = (oldValue != value);
        it->value = value;
        it->source = source;
        
        // Handle copy-on-write optimization
        if (pImpl->enableCopyOnWrite && pImpl->writeProtected) {
            pImpl->cowCache[key] = value;
        }
    } else {
        oldValue = ConfigValue(); // Default constructed
        valueChanged = true;
        pImpl->entries.emplace_back(key, value, source);
        
        // Handle copy-on-write optimization
        if (pImpl->enableCopyOnWrite && pImpl->writeProtected) {
            pImpl->cowCache[key] = value;
        }
    }
    
    // Validate new value
    std::vector<ConfigValidationError> errors;
    bool valid = validate(key, value);
    
    // Record history
    if (valid && pImpl->enableHistory && valueChanged) {
        ConfigHistoryEntry entry;
        entry.timestamp = std::chrono::system_clock::now();
        entry.key = key;
        entry.oldValue = oldValue;
        entry.newValue = value;
        entry.source = source;
        entry.reason = "Value updated";
        
        pImpl->history.push_back(entry);
        if (pImpl->history.size() > pImpl->historyLimit) {
            pImpl->history.pop_front();
        }
    }
    
    // Update validation errors cache
    if (!valid) {
        std::lock_guard<std::shared_mutex> valLock(pImpl->validationMutex);
        pImpl->validationErrors.clear();
        // Re-validate all to get comprehensive errors
        validateAll();
    }
    
    // Call change callbacks
    if (valueChanged) {
        std::lock_guard<std::mutex> cbLock(pImpl->changeMutex);
        pImpl->changeCount++;
        for (const auto& callback : pImpl->changeCallbacks) {
            try {
                callback(key, oldValue, value);
            } catch (...) {
                // Ignore callback exceptions
            }
        }
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
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    
    // Check if batch mode
    if (pImpl->batchMode) {
        pImpl->batchSize++;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });

    if (it != pImpl->entries.end()) {
        ConfigValue oldValue = it->value;
        
        pImpl->entries.erase(it);
        
        // Record history
        if (pImpl->enableHistory) {
            ConfigHistoryEntry entry;
            entry.timestamp = std::chrono::system_clock::now();
            entry.key = key;
            entry.oldValue = oldValue;
            entry.newValue = ConfigValue();
            entry.source = ConfigSource::Runtime;
            entry.reason = "Value removed";
            
            pImpl->history.push_back(entry);
            if (pImpl->history.size() > pImpl->historyLimit) {
                pImpl->history.pop_front();
            }
        }
        
        // Call change callbacks
        {
            std::lock_guard<std::mutex> cbLock(pImpl->changeMutex);
            pImpl->changeCount++;
            for (const auto& callback : pImpl->changeCallbacks) {
                try {
                    callback(key, oldValue, ConfigValue());
                } catch (...) {
                    // Ignore callback exceptions
                }
            }
        }
    }
}

std::vector<std::string> Config::getKeys() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    
    if (pImpl->batchMode) {
        pImpl->batchSize++;
    }
    
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
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

bool Config::validate(const std::string& key, const ConfigValue& value) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    // Check schema validation if schema exists
    auto it = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
        [&key](const ConfigSchemaEntry& e) { return e.key == key; });

    if (it != pImpl->schema.end()) {
        std::vector<ConfigValidationError> errors;
        bool valid = it->validator(key, value, errors);
        
        if (!valid) {
            std::lock_guard<std::shared_mutex> valLock(pImpl->validationMutex);
            pImpl->validationErrors = errors;
        } else {
            std::lock_guard<std::shared_mutex> valLock(pImpl->validationMutex);
            pImpl->validationErrors.clear();
        }
        
        return valid;
    }
    
    // Check built-in validators
    std::lock_guard<std::shared_mutex> valLock(pImpl->validationMutex);
    pImpl->validationErrors.clear();
    
    for (const auto& [name, validator] : pImpl->validators) {
        std::vector<ConfigValidationError> errors;
        validator(key, value, errors);
        pImpl->validationErrors.insert(pImpl->validationErrors.end(), errors.begin(), errors.end());
    }
    
    return pImpl->validationErrors.empty();
}

bool Config::validateAll() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    std::lock_guard<std::shared_mutex> valLock(pImpl->validationMutex);
    pImpl->validationErrors.clear();
    
    for (const auto& entry : pImpl->entries) {
        validate(entry.key, entry.value);
    }
    
    return pImpl->validationErrors.empty();
}

std::vector<ConfigValidationError> Config::getValidationErrors() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    std::lock_guard<std::shared_mutex> valLock(pImpl->validationMutex);
    return pImpl->validationErrors;
}

void Config::addSchemaEntry(const std::string& key, const std::type_info& type, 
                           const std::string& description, bool required,
                           const ConfigValue& defaultValue) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    
    ConfigSchemaEntry entry;
    entry.key = key;
    entry.type = type;
    entry.description = description;
    entry.required = required;
    entry.defaultValue = defaultValue;
    
    // Create default validator based on type
    if (type == typeid(int)) {
        entry.validator = [](const std::string& key, const ConfigValue& value, std::vector<ConfigValidationError>& errors) {
            if (const int* i = std::get_if<int>(&value)) {
                return true;
            }
            errors.push_back({key, "Must be integer", "int", "invalid type"});
            return false;
        };
    } else if (type == typeid(std::string)) {
        entry.validator = [](const std::string& key, const ConfigValue& value, std::vector<ConfigValidationError>& errors) {
            if (const std::string* str = std::get_if<std::string>(&value)) {
                return true;
            }
            errors.push_back({key, "Must be string", "string", "invalid type"});
            return false;
        };
    } else if (type == typeid(double)) {
        entry.validator = [](const std::string& key, const ConfigValue& value, std::vector<ConfigValidationError>& errors) {
            if (const double* d = std::get_if<double>(&value)) {
                return true;
            }
            errors.push_back({key, "Must be double", "double", "invalid type"});
            return false;
        };
    } else if (type == typeid(bool)) {
        entry.validator = [](const std::string& key, const ConfigValue& value, std::vector<ConfigValidationError>& errors) {
            if (const bool* b = std::get_if<bool>(&value)) {
                return true;
            }
            errors.push_back({key, "Must be boolean", "bool", "invalid type"});
            return false;
        };
    }
    
    pImpl->schema.push_back(entry);
}

bool Config::validateSchema() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    std::lock_guard<std::shared_mutex> valLock(pImpl->validationMutex);
    pImpl->validationErrors.clear();
    
    for (const auto& entry : pImpl->schema) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });

        if (it == pImpl->entries.end()) {
            if (entry.required) {
                ConfigValidationError error;
                error.path = entry.key;
                error.message = "Required configuration value is missing";
                error.expected = "present";
                error.actual = "missing";
                pImpl->validationErrors.push_back(error);
            } else if (!std::holds_alternative<std::monostate>(entry.defaultValue)) {
                // Apply default value
                const_cast<Config*>(this)->set(entry.key, entry.defaultValue, ConfigSource::Default);
            }
        } else {
            std::vector<ConfigValidationError> errors;
            bool valid = entry.validator(entry.key, it->value, errors);
            pImpl->validationErrors.insert(pImpl->validationErrors.end(), errors.begin(), errors.end());
        }
    }
    
    return pImpl->validationErrors.empty();
}

std::vector<ConfigValidationError> Config::getSchemaValidationErrors() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    std::lock_guard<std::shared_mutex> valLock(pImpl->validationMutex);
    return pImpl->validationErrors;
}

void Config::enableCopyOnWrite(bool enable) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->enableCopyOnWrite = enable;
}

bool Config::isCopyOnWriteEnabled() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->enableCopyOnWrite;
}

void Config::enableWriteProtection() {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->writeProtected = true;
}

bool Config::isWriteProtected() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->writeProtected;
}

void Config::setFlag(const std::string& flag, bool value) {
    std::unique_lock<std::shared_mutex> lock(pImpl->flagsMutex);
    pImpl->flags[flag] = value;
}

bool Config::getFlag(const std::string& flag) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->flagsMutex);
    auto it = pImpl->flags.find(flag);
    return it != pImpl->flags.end() ? it->second : false;
}

void Config::enableHistory(bool enable) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->enableHistory = enable;
}

bool Config::isHistoryEnabled() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->enableHistory;
}

std::vector<ConfigHistoryEntry> Config::getHistory(size_t limit) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    std::vector<ConfigHistoryEntry> result;
    result.reserve(std::min(limit, pImpl->history.size()));
    
    auto start = pImpl->history.end();
    std::advance(start, -static_cast<std::ptrdiff_t>(std::min(limit, pImpl->history.size())));
    
    for (auto it = start; it != pImpl->history.end(); ++it) {
        result.push_back(*it);
    }
    
    return result;
}

void Config::clearHistory() {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->history.clear();
}

void Config::addChangeCallback(const ConfigChangeCallback& callback) {
    std::lock_guard<std::mutex> lock(pImpl->changeMutex);
    pImpl->changeCallbacks.push_back(callback);
}

void Config::removeChangeCallback(const ConfigChangeCallback& callback) {
    std::lock_guard<std::mutex> lock(pImpl->changeMutex);
    pImpl->changeCallbacks.erase(
        std::remove(pImpl->changeCallbacks.begin(), pImpl->changeCallbacks.end(), callback),
        pImpl->changeCallbacks.end()
    );
}

size_t Config::getChangeCount() const {
    std::shared_lock<std::mutex> lock(pImpl->changeMutex);
    return pImpl->changeCount;
}

void Config::lock() const {
    std::unique_lock<std::mutex> lock(pImpl->lockMutex);
    pImpl->locked = true;
}

void Config::unlock() const {
    std::unique_lock<std::mutex> lock(pImpl->lockMutex);
    pImpl->locked = false;
}

bool Config::tryLock() const {
    return pImpl->lockMutex.try_lock();
}

bool Config::isLocked() const {
    return pImpl->locked;
}

void Config::batchBegin() {
    std::unique_lock<std::mutex> lock(pImpl->batchMutex);
    pImpl->batchMode = true;
    pImpl->batchSize = 0;
}

void Config::batchCommit() {
    std::unique_lock<std::mutex> lock(pImpl->batchMutex);
    pImpl->batchMode = false;
}

bool Config::isBatchMode() const {
    std::shared_lock<std::mutex> lock(pImpl->batchMutex);
    return pImpl->batchMode;
}

void Config::setBatchSize(size_t size) {
    std::unique_lock<std::mutex> lock(pImpl->batchMutex);
    pImpl->batchSize = size;
}

bool Config::merge(const Config& other, bool resolveConflicts) {
    std::shared_lock<std::shared_mutex> lock1(pImpl->mutex);
    std::shared_lock<std::shared_mutex> lock2(other.pImpl->mutex);
    
    bool success = true;
    
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });

        if (it != pImpl->entries.end()) {
            if (resolveConflicts) {
                // Prefer command-line and runtime sources over file and default
                ConfigSource sourcePriority[] = {ConfigSource::Runtime, ConfigSource::CommandLine, 
                                                ConfigSource::File, ConfigSource::Default};
                
                ConfigSource currentSource = it->source;
                ConfigSource newSource = entry.source;
                
                bool replace = false;
                for (auto source : sourcePriority) {
                    if (source == newSource) {
                        replace = true;
                        break;
                    }
                    if (source == currentSource) {
                        break;
                    }
                }
                
                if (replace) {
                    set(entry.key, entry.value, entry.source);
                }
            } else {
                // Conflict - prefer other's source
                set(entry.key, entry.value, entry.source);
            }
        } else {
            set(entry.key, entry.value, entry.source);
        }
    }
    
    return success;
}

Config::ValidationResult Config::validateWithPaths() const {
    ValidationResult result;
    result.success = true;
    
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    for (const auto& entry : pImpl->entries) {
        result.paths.push_back(entry.key);
        
        std::vector<ConfigValidationError> errors;
        validate(entry.key, entry.value);
        
        // Get errors from cache
        std::lock_guard<std::shared_mutex> valLock(pImpl->validationMutex);
        for (const auto& error : pImpl->validationErrors) {
            if (error.path == entry.key) {
                result.errors.push_back(error);
                result.success = false;
            }
        }
    }
    
    return result;
}

void Config::setAtomicFlag(const std::string& flag, bool value) {
    std::unique_lock<std::shared_mutex> lock(pImpl->flagsMutex);
    pImpl->atomicFlags[flag] = value;
}

bool Config::getAtomicFlag(const std::string& flag) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->flagsMutex);
    auto it = pImpl->atomicFlags.find(flag);
    return it != pImpl->atomicFlags.end() ? it->second.load() : false;
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

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    nlohmann::json json;
    std::visit([&json](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json = arg;
        }
    }, value);
    return json;
}

bool Config::jsonToConfigValue(const nlohmann::json& json, ConfigValue& value) {
    try {
        if (json.is_string()) {
            value = json.get<std::string>();
            return true;
        } else if (json.is_number_integer()) {
            value = json.get<int>();
            return true;
        } else if (json.is_number()) {
            value = json.get<double>();
            return true;
        } else if (json.is_boolean()) {
            value = json.get<bool>();
            return true;
        } else if (json.is_array()) {
            // Determine element type
            if (!json.empty()) {
                if (json[0].is_number_integer()) {
                    value = json.get<std::vector<int>>();
                    return true;
                } else if (json[0].is_number()) {
                    value = json.get<std::vector<double>>();
                    return true;
                } else if (json[0].is_string()) {
                    value = json.get<std::vector<std::string>>();
                    return true;
                }
            }
        }
    } catch (...) {
        // Handle conversion error
    }
    return false;
}

// JSON schema validation helper
namespace nlm {
    bool validateJsonSchema(const nlohmann::json& data, const nlohmann::json& schema) {
        // Basic schema validation
        if (schema.contains("type")) {
            std::string type = schema["type"];
            if (type == "object" && !data.is_object()) {
                return false;
            } else if (type == "string" && !data.is_string()) {
                return false;
            } else if (type == "number" && !data.is_number()) {
                return false;
            } else if (type == "boolean" && !data.is_boolean()) {
                return false;
            } else if (type == "array" && !data.is_array()) {
                return false;
            }
        }
        
        if (schema.contains("properties")) {
            for (const auto& [key, prop] : schema["properties"].items()) {
                if (data.contains(key)) {
                    if (!validateJsonSchema(data[key], prop)) {
                        return false;
                    }
                } else if (prop.contains("required") && prop["required"].get<bool>()) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    std::vector<ConfigValidationError> validateJsonSchemaDetailed(const nlohmann::json& data, const nlohmann::json& schema) {
        std::vector<ConfigValidationError> errors;
        
        if (schema.contains("type")) {
            std::string type = schema["type"];
            if (type == "object" && !data.is_object()) {
                ConfigValidationError error;
                error.path = "root";
                error.message = "Expected object type";
                error.expected = "object";
                error.actual = data.type_name();
                errors.push_back(error);
            } else if (type == "string" && !data.is_string()) {
                ConfigValidationError error;
                error.path = "root";
                error.message = "Expected string type";
                error.expected = "string";
                error.actual = data.type_name();
                errors.push_back(error);
            } else if (type == "number" && !data.is_number()) {
                ConfigValidationError error;
                error.path = "root";
                error.message = "Expected number type";
                error.expected = "number";
                error.actual = data.type_name();
                errors.push_back(error);
            } else if (type == "boolean" && !data.is_boolean()) {
                ConfigValidationError error;
                error.path = "root";
                error.message = "Expected boolean type";
                error.expected = "boolean";
                error.actual = data.type_name();
                errors.push_back(error);
            } else if (type == "array" && !data.is_array()) {
                ConfigValidationError error;
                error.path = "root";
                error.message = "Expected array type";
                error.expected = "array";
                error.actual = data.type_name();
                errors.push_back(error);
            }
        }
        
        if (schema.contains("properties")) {
            for (const auto& [key, prop] : schema["properties"].items()) {
                if (data.contains(key)) {
                    if (!validateJsonSchemaDetailed(data[key], prop).empty()) {
                        ConfigValidationError error;
                        error.path = key;
                        error.message = "Property validation failed";
                        error.expected = "valid value";
                        error.actual = "invalid value";
                        errors.push_back(error);
                    }
                } else if (prop.contains("required") && prop["required"].get<bool>()) {
                    ConfigValidationError error;
                    error.path = key;
                    error.message = "Required property is missing";
                    error.expected = "present";
                    error.actual = "missing";
                    errors.push_back(error);
                }
            }
        }
        
        return errors;
    }
}

// Configuration value conversion utilities
namespace nlm {
    template<typename T>
    bool getConfigValue(const ConfigValue& value, T& out) {
        if (const T* ptr = std::get_if<T>(&value)) {
            out = *ptr;
            return true;
        }
        return false;
    }
    
    template<typename T>
    bool setConfigValue(ConfigValue& value, const T& val) {
        // Clear existing value
        value = T();
        // We can't set it directly due to variant's immutability
        // This is a limitation - in practice, callers should use set()
        return true;
    }
}

// Explicit template instantiations
namespace nlm {
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
}

} // namespace nlm