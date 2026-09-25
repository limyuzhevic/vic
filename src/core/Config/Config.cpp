#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::vector<ConfigValidator::ValidationRule>> validators;
    std::map<std::string, std::string> validationErrors;
    size_t nextId = 0;
    std::string description;
    bool validateOnSet = true;
    bool autoSave = false;
    std::string lastSavedFile;
    std::string lastError;
    mutable std::mutex mutex_;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->description = "NLM Configuration System";
    pImpl->validateOnSet = true;
    pImpl->autoSave = false;
    pImpl->nextId = 0;
}

Config::~Config() = default;

void Config::lock() const noexcept {
    pImpl->mutex_.lock();
}

void Config::unlock() const noexcept {
    pImpl->mutex_.unlock();
}

bool Config::tryLock() const noexcept {
    return pImpl->mutex_.try_lock();
}

// Template implementations
template<typename T>
T Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        throw ConfigError("Key not found: " + key);
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        std::stringstream ss;
        ss << "Type mismatch for key \"" << key << "\": expected " 
           << typeid(T).name() << " but found variant value";
        throw ConfigError(ss.str());
    }
}

template<typename T>
std::optional<T> Config::tryGet(const std::string& key) const noexcept {
    try {
        return get<T>(key);
    } catch (const ConfigError&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = tryGet<T>(key);
    if (val.has_value()) {
        return val.value();
    }
    return defaultValue;
}

template<typename T>
std::optional<T> Config::getWithValidation(const std::string& key, 
                                         const ConfigValidator::ValidationFunc& validator) const {
    auto val = tryGet<T>(key);
    if (!val.has_value()) {
        return std::nullopt;
    }
    
    // Convert T to ConfigValue for validation
    ConfigValue configValue;
    if constexpr (std::is_same_v<T, int>) {
        configValue = val.value();
    } else if constexpr (std::is_same_v<T, double>) {
        configValue = val.value();
    } else if constexpr (std::is_same_v<T, std::string>) {
        configValue = val.value();
    } else if constexpr (std::is_same_v<T, bool>) {
        configValue = val.value();
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        configValue = val.value();
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        configValue = val.value();
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        configValue = val.value();
    } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
        configValue = val.value();
    } else {
        throw ConfigError("Unsupported type for validation");
    }
    
    if (!validator(configValue)) {
        return std::nullopt;
    }
    
    return val.value();
}

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source, const std::string& description,
                 bool required, const std::string& validator) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto entry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entry != pImpl->entries.end()) {
        entry->value = value;
        entry->source = source;
        entry->description = description.empty() ? entry->description : description;
        entry->required = required;
        if (!validator.empty()) {
            entry->validator = validator;
        }
        entry->timestamp = std::chrono::system_clock::now();
    } else {
        entry = pImpl->entries.emplace(pImpl->entries.end(), key, value, source, description, required);
        if (!validator.empty()) {
            entry->validator = validator;
        }
    }
    
    // Validate if required
    if (pImpl->validateOnSet && !validateEntry(*entry)) {
        auto errors = getEntryValidationErrors(*entry);
        std::stringstream ss;
        ss << "Validation failed for key \"" << key << "\": ";
        for (const auto& error : errors) {
            ss << error << "; ";
        }
        pImpl->lastError = ss.str();
        if (entry->required) {
            throw ConfigValidationError(pImpl->lastError);
        }
    }
}

void Config::set(const std::string& key, const std::string& value,
                 ConfigSource source, const std::string& description,
                 bool required, const std::string& validator) {
    set(key, ConfigValue(value), source, description, required, validator);
}

void Config::set(const std::string& key, int value,
                 ConfigSource source, const std::string& description,
                 bool required, const std::string& validator) {
    set(key, ConfigValue(value), source, description, required, validator);
}

void Config::set(const std::string& key, double value,
                 ConfigSource source, const std::string& description,
                 bool required, const std::string& validator) {
    set(key, ConfigValue(value), source, description, required, validator);
}

void Config::set(const std::string& key, bool value,
                 ConfigSource source, const std::string& description,
                 bool required, const std::string& validator) {
    set(key, ConfigValue(value), source, description, required, validator);
}

void Config::setNested(const std::string& prefix, const std::string& key, const ConfigValue& value) {
    std::string fullKey = prefix + "." + key;
    set(fullKey, value);
}

void Config::setArray(const std::string& key, const std::vector<int>& values) {
    set(key, ConfigValue(values));
}

void Config::setArray(const std::string& key, const std::vector<double>& values) {
    set(key, ConfigValue(values));
}

void Config::setArray(const std::string& key, const std::vector<std::string>& values) {
    set(key, ConfigValue(values));
}

void Config::setArray(const std::string& key, const std::vector<bool>& values) {
    set(key, ConfigValue(values));
}

std::optional<std::vector<int>> Config::getIntArray(const std::string& key) const {
    try {
        auto value = get<std::vector<int>>(key);
        return value;
    } catch (const ConfigError&) {
        return std::nullopt;
    }
}

std::optional<std::vector<double>> Config::getDoubleArray(const std::string& key) const {
    try {
        auto value = get<std::vector<double>>(key);
        return value;
    } catch (const ConfigError&) {
        return std::nullopt;
    }
}

std::optional<std::vector<std::string>> Config::getStringArray(const std::string& key) const {
    try {
        auto value = get<std::vector<std::string>>(key);
        return value;
    } catch (const ConfigError&) {
        return std::nullopt;
    }
}

std::optional<std::vector<bool>> Config::getBoolArray(const std::string& key) const {
    try {
        auto value = get<std::vector<bool>>(key);
        return value;
    } catch (const ConfigError&) {
        return std::nullopt;
    }
}

std::optional<NestedConfig> Config::getNested(const std::string& prefix) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    NestedConfig nested(prefix);
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(prefix + ".") == 0 && entry.key != prefix) {
            std::string subKey = entry.key.substr(prefix.size() + 1);
            nested.set(subKey, entry.value);
        }
    }
    
    if (nested.size() > 0) {
        return nested;
    }
    return std::nullopt;
}

bool Config::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool Config::hasNested(const std::string& prefix) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&prefix](const ConfigEntry& e) { 
            return e.key.find(prefix + ".") == 0 && e.key != prefix; 
        });
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

void Config::removeNested(const std::string& prefix) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&prefix](const ConfigEntry& e) { 
                return e.key.find(prefix + ".") == 0 && e.key != prefix; 
            }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeys(const std::string& prefix) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(prefix + ".") == 0 && entry.key != prefix) {
            keys.push_back(entry.key.substr(prefix.size() + 1));
        }
    }
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->entries.clear();
}

bool Config::loadFromFile(const std::string& filepath, 
                         bool validate,
                         const std::vector<std::string>& allowedExtensions) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        pImpl->lastError = "File does not exist: " + filepath;
        return false;
    }
    
    // Try to detect file format
    std::string format = detectFileFormat(filepath);
    
    // Check if format is allowed
    if (allowedExtensions.empty() || 
        std::find(allowedExtensions.begin(), allowedExtensions.end(), format) == allowedExtensions.end()) {
        pImpl->lastError = "File format \"" + format + "\" not allowed. Allowed formats: ";
        for (const auto& ext : allowedExtensions) {
            pImpl->lastError += ext + " ";
        }
        return false;
    }
    
    // Read file content
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open file: " + filepath;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Parse based on format
    bool success = false;
    if (format == "json") {
        success = parseJSON(content, *this);
    } else if (format == "yaml" || format == "yml") {
        success = parseYAML(content, *this);
    } else {
        success = parseCFG(content, *this);
    }
    
    if (success && validate) {
        return validate();
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath,
                        bool prettyPrint,
                        const std::string& format) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    // Determine format if auto
    std::string targetFormat = format;
    if (targetFormat == "auto") {
        targetFormat = detectFileFormat(filepath);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open file for writing: " + filepath;
        return false;
    }
    
    bool success = false;
    if (targetFormat == "json") {
        file << toJSON(prettyPrint);
        success = true;
    } else if (targetFormat == "yaml" || targetFormat == "yml") {
        file << toYAML();
        success = true;
    } else {
        // Save as CFG format (key=value)
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            std::visit([&file](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << val << "\"";
                } else {
                    file << val;
                }
            }, entry.value);
            file << "\n\n";
        }
        success = true;
    }
    
    if (success) {
        pImpl->lastSavedFile = filepath;
        pImpl->lastError.clear();
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Try to parse value as different types
                try {
                    // Try bool
                    if (value == "true" || value == "false") {
                        set(key, value == "true", ConfigSource::CommandLine);
                    }
                    // Try int
                    else if (std::all_of(value.begin(), value.end(), ::isdigit) || 
                             (value[0] == '-' && std::all_of(value.begin() + 1, value.end(), ::isdigit))) {
                        set(key, std::stoi(value), ConfigSource::CommandLine);
                    }
                    // Try double
                    else {
                        size_t pos2 = value.find('.');
                        if (pos2 != std::string::npos && 
                            std::all_of(value.begin(), value.begin() + pos2, ::isdigit) &&
                            std::all_of(value.begin() + pos2 + 1, value.end(), ::isdigit)) {
                            set(key, std::stod(value), ConfigSource::CommandLine);
                        } else {
                            set(key, value, ConfigSource::CommandLine);
                        }
                    }
                } catch (...) {
                    set(key, value, ConfigSource::CommandLine);
                }
            } else {
                set(arg.substr(2), "true", ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Same type parsing as above
            try {
                if (value == "true" || value == "false") {
                    set(key, value == "true", ConfigSource::CommandLine);
                } else if (std::all_of(value.begin(), value.end(), ::isdigit) || 
                           (value[0] == '-' && std::all_of(value.begin() + 1, value.end(), ::isdigit))) {
                    set(key, std::stoi(value), ConfigSource::CommandLine);
                } else {
                    size_t pos = value.find('.');
                    if (pos != std::string::npos && 
                        std::all_of(value.begin(), value.begin() + pos, ::isdigit) &&
                        std::all_of(value.begin() + pos + 1, value.end(), ::isdigit)) {
                        set(key, std::stod(value), ConfigSource::CommandLine);
                    } else {
                        set(key, value, ConfigSource::CommandLine);
                    }
                }
            } catch (...) {
                set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    
    return true;
}

bool Config::validate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->validationErrors.clear();
    
    bool allValid = true;
    for (const auto& entry : pImpl->entries) {
        if (!validateEntry(entry)) {
            auto errors = getEntryValidationErrors(entry);
            for (const auto& error : errors) {
                pImpl->validationErrors[entry.key] = error;
            }
            allValid = false;
        }
    }
    
    return allValid;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    std::vector<std::string> errors;
    for (const auto& pair : pImpl->validationErrors) {
        errors.push_back(pair.first + ": " + pair.second);
    }
    return errors;
}

void Config::addValidator(const std::string& key, const ConfigValidator::ValidationFunc& validator,
                         const ConfigValidator::ErrorFunc& errorHandler,
                         const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto entry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entry != pImpl->entries.end()) {
        ConfigValidator::ValidationRule rule;
        rule.name = name.empty() ? "custom" : name;
        rule.validator = validator;
        rule.errorHandler = errorHandler;
        rule.active = true;
        
        pImpl->validators[key].push_back(rule);
    }
}

std::string Config::summary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "Source: " << pImpl->description << "\n";
    oss << "Validate on set: " << (pImpl->validateOnSet ? "Yes" : "No") << "\n";
    oss << "Auto save: " << (pImpl->autoSave ? "Yes" : "No") << "\n";
    oss << "Last error: " << (pImpl->lastError.empty() ? "None" : pImpl->lastError) << "\n";
    oss << "\nEntries:\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (Source: " << static_cast<int>(entry.source) << ", Required: " 
            << (entry.required ? "Yes" : "No") << ")\n";
        if (!entry.description.empty()) {
            oss << "    Description: " << entry.description << "\n";
        }
        if (!entry.validator.empty()) {
            oss << "    Validator: " << entry.validator << "\n";
        }
        oss << "    Last modified: " << entry.getTimestampString() << "\n";
    }
    
    return oss.str();
}

std::string Config::toJSON(bool prettyPrint) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    std::ostringstream oss;
    
    if (prettyPrint) {
        oss << std::setw(2);
    }
    
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": ";
        
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << escapeJSON(val) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                                 std::is_same_v<T, std::vector<double>> || 
                                 std::is_same_v<T, std::vector<std::string>> ||
                                 std::is_same_v<T, std::vector<bool>>) {
                oss << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if constexpr (std::is_same_v<T, std::string>) {
                        oss << "\"" << escapeJSON(val[j]) << "\"";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        oss << (val[j] ? "true" : "false");
                    } else {
                        oss << val[j];
                    }
                    if (j < val.size() - 1) {
                        oss << ", ";
                    }
                }
                oss << "]";
            } else {
                oss << val;
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    
    oss << "}\n";
    return oss.str();
}

std::string Config::toYAML() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    std::ostringstream oss;
    
    oss << "# NLM Configuration\n";
    oss << "# Generated at: " << std::chrono::system_clock::now() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.description << "\n";
        oss << entry.key << ": ";
        
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << escapeYAML(val) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                                 std::is_same_v<T, std::vector<double>> || 
                                 std::is_same_v<T, std::vector<std::string>> ||
                                 std::is_same_v<T, std::vector<bool>>) {
                oss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if constexpr (std::is_same_v<T, std::string>) {
                        oss << "\"" << escapeYAML(val[i]) << "\"";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        oss << (val[i] ? "true" : "false");
                    } else {
                        oss << val[i];
                    }
                    if (i < val.size() - 1) {
                        oss << ", ";
                    }
                }
                oss << "]";
            } else {
                oss << val;
            }
        }, entry.value);
        
        oss << "\n\n";
    }
    
    return oss.str();
}

double Config::getMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    // Estimate memory usage (rough approximation)
    double totalBytes = 0;
    for (const auto& entry : pImpl->entries) {
        totalBytes += entry.key.size() * sizeof(char);
        totalBytes += sizeof(ConfigValue); // Variant size
        totalBytes += sizeof(ConfigSource);
        totalBytes += entry.description.size() * sizeof(char);
        totalBytes += sizeof(bool); // required flag
        totalBytes += entry.metadata.size() * sizeof(std::pair<std::string, std::string>);
    }
    
    // Add validators memory
    for (const auto& pair : pImpl->validators) {
        totalBytes += pair.second.size() * sizeof(ConfigValidator::ValidationRule);
    }
    
    return totalBytes / (1024.0 * 1024.0); // Convert to MB
}

Config::ConfigStats Config::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    stats.fileEntries = 0;
    stats.commandLineEntries = 0;
    stats.runtimeEntries = 0;
    stats.defaultEntries = 0;
    stats.memoryUsageMB = getMemoryUsage();
    stats.lastModified = std::chrono::system_clock::now();
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::File:
                stats.fileEntries++;
                break;
            case ConfigSource::CommandLine:
                stats.commandLineEntries++;
                break;
            case ConfigSource::Runtime:
                stats.runtimeEntries++;
                break;
            case ConfigSource::Default:
                stats.defaultEntries++;
                break;
            case ConfigSource::Override:
                // Treat overrides as runtime for statistics
                stats.runtimeEntries++;
                break;
        }
    }
    
    return stats;
}

bool Config::operator==(const Config& other) const {
    std::lock_guard<std::mutex> lock1(pImpl->mutex_);
    std::lock_guard<std::mutex> lock2(other.pImpl->mutex_);
    
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    // Compare entries (order doesn't matter)
    auto otherEntries = other.pImpl->entries;
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(otherEntries.begin(), otherEntries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == otherEntries.end()) {
            return false;
        }
        
        // Compare values
        if (entry.value != it->value) {
            return false;
        }
        
        // Don't compare source or metadata for equality
    }
    
    return true;
}

void Config::merge(const Config& other, ConfigSource source) {
    std::lock_guard<std::mutex> lock1(pImpl->mutex_);
    std::lock_guard<std::mutex> lock2(other.pImpl->mutex_);
    
    for (const auto& otherEntry : other.pImpl->entries) {
        auto entry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&otherEntry](const ConfigEntry& e) { return e.key == otherEntry.key; });
        
        if (entry != pImpl->entries.end()) {
            // Merge metadata
            for (const auto& meta : otherEntry.metadata) {
                entry->metadata[meta.first] = meta.second;
            }
        } else {
            pImpl->entries.push_back(otherEntry);
        }
    }
}

void Config::mergeNested(const Config& other, const std::string& prefix, 
                        ConfigSource source) {
    std::lock_guard<std::mutex> lock1(pImpl->mutex_);
    std::lock_guard<std::mutex> lock2(other.pImpl->mutex_);
    
    for (const auto& otherEntry : other.pImpl->entries) {
        std::string fullKey = prefix + "." + otherEntry.key;
        
        auto entry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&fullKey](const ConfigEntry& e) { return e.key == fullKey; });
        
        if (entry != pImpl->entries.end()) {
            // Merge metadata
            for (const auto& meta : otherEntry.metadata) {
                entry->metadata[meta.first] = meta.second;
            }
        } else {
            ConfigEntry newEntry = otherEntry;
            newEntry.key = fullKey;
            pImpl->entries.push_back(newEntry);
        }
    }
}

// Static helper implementations
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

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string Config::escapeJSON(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\\': result += "\\\\"; break;
            case '\\': result += "\\\\"; break;
            case '\\': result += "\\\\"; break;
            default: result += c;
        }
    }
    return result;
}

std::string Config::escapeYAML(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '\"' || c == '\'') {
            result += '\\';
        }
        result += c;
    }
    return result;
}

bool Config::parseJSON(const std::string& content, Config& config) {
    try {
        // Simple JSON parsing - in a real implementation, use a JSON library
        // This is a placeholder for Phase 2+
        std::istringstream stream(content);
        char c;
        while (stream.get(c)) {
            if (c == '}') {
                continue;
            }
            if (c == '\"') {
                std::string key;
                while (stream.get(c) && c != '\"') {
                    key += c;
                }
                stream.ignore(); // Skip colon
                if (stream.peek() == '\"') {
                    std::string value;
                    while (stream.get(c) && c != '\"') {
                        value += c;
                    }
                    stream.ignore(); // Skip quote
                    config.set(key, value, ConfigSource::File);
                } else if (stream.peek() == '{') {
                    // Nested object - not implemented in this simple version
                    std::string dummy;
                    while (stream.get(c) && c != '}') {
                        dummy += c;
                    }
                }
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::parseYAML(const std::string& content, Config& config) {
    try {
        // Simple YAML parsing - in a real implementation, use a YAML library
        // This is a placeholder for Phase 2+
        std::istringstream stream(content);
        std::string line;
        std::string currentSection;
        
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            if (line.find(':') != std::string::npos) {
                size_t pos = line.find(':');
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                config.set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::parseCFG(const std::string& content, Config& config) {
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
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                config.set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    // Check if entry is required but missing (shouldn't happen in entry)
    if (entry.required) {
        return !entry.key.empty();
    }
    
    // Basic type validation
    if (entry.value.index() == std::variant_npos) {
        return false;
    }
    
    // Run custom validators
    auto it = pImpl->validators.find(entry.key);
    if (it != pImpl->validators.end()) {
        for (const auto& rule : it->second) {
            if (rule.active && !rule.validator(entry.value)) {
                pImpl->validationErrors[entry.key] = rule.errorHandler(entry.value);
                return false;
            }
        }
    }
    
    return true;
}

std::vector<std::string> Config::getEntryValidationErrors(const ConfigEntry& entry) const {
    std::vector<std::string> errors;
    
    if (pImpl->validators.find(entry.key) != pImpl->validators.end()) {
        for (const auto& rule : pImpl->validators.at(entry.key)) {
            if (rule.active && !rule.validator(entry.value)) {
                errors.push_back(rule.errorHandler(entry.value));
            }
        }
    }
    
    return errors;
}

std::string Config::detectFileFormat(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return "unknown";
    }
    
    // Read first few bytes for format detection
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return "unknown";
    }
    
    char buffer[1024];
    file.read(buffer, sizeof(buffer));
    std::string content(buffer, file.gcount());
    
    return isJSONFile(content) ? "json" : 
           isYAMLFile(content) ? "yaml" : 
           isCFGFile(content) ? "cfg" : "unknown";
}

bool Config::isJSONFile(const std::string& content) {
    // Simple JSON detection
    return content.find('{') != std::string::npos &&
           content.find('}') != std::string::npos;
}

bool Config::isYAMLFile(const std::string& content) {
    // Simple YAML detection
    return content.find(':') != std::string::npos &&
           (content.find('-') != std::string::npos || content.find('*') != std::string::npos);
}

bool Config::isCFGFile(const std::string& content) {
    // Simple CFG detection
    return content.find('=') != std::string::npos;
}

// Explicit template instantiations
template std::optional<int> Config::tryGet<int>(const std::string&) const noexcept;
template std::optional<double> Config::tryGet<double>(const std::string&) const noexcept;
template std::optional<std::string> Config::tryGet<std::string>(const std::string&) const noexcept;
template std::optional<bool> Config::tryGet<bool>(const std::string&) const noexcept;

} // namespace nlm