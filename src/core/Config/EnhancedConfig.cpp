#include "EnhancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <limits>
#include <iostream>

namespace nlm {

struct EnhancedConfig::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigMetadata> metadata;
    std::vector<std::shared_ptr<ConfigChangeCallback>> changeCallbacks;
    std::map<std::string, std::vector<TimeSeriesPoint>> timeSeries;
    std::chrono::system_clock::time_point lastCleanup;
    bool optimizedForPerformance;
    
    Impl() : optimizedForPerformance(false), lastCleanup(std::chrono::system_clock::now()) {}
};

EnhancedConfig::EnhancedConfig() : pImpl(std::make_unique<Impl>()) {}

EnhancedConfig::~EnhancedConfig() = default;

EnhancedConfig::EnhancedConfig(EnhancedConfig&&) noexcept = default;

EnhancedConfig& EnhancedConfig::operator=(EnhancedConfig&&) noexcept = default;

template<typename T>
bool EnhancedConfig::setValidated(const std::string& key, const T& value, ConfigSource source,
                                 const std::string& description, const ConfigMetadata& metadata) {
    ConfigValue configValue = value;
    ConfigMetadata actualMetadata = metadata;
    
    // Use existing metadata if available, otherwise use provided
    if (!hasMetadata(key)) {
        setMetadata(key, actualMetadata);
    } else {
        actualMetadata = getMetadata(key);
    }
    
    // Validate the value
    if (!validateValue(ConfigEntry(key, configValue, source, description, actualMetadata))) {
        return false;
    }
    
    // Get old value for callbacks
    ConfigValue oldValue;
    bool exists = false;
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            oldValue = entry.value;
            exists = true;
            break;
        }
    }
    
    // Set the new value
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        ConfigValue oldVal = it->value;
        it->value = configValue;
        it->source = source;
        it->lastModified = std::chrono::system_clock::now();
    } else {
        ConfigEntry newEntry(key, configValue, source, description, actualMetadata);
        pImpl->entries.push_back(newEntry);
    }
    
    // Notify callbacks if value changed
    if (exists && oldValue != configValue) {
        notifyChange(key, configValue, oldValue);
    }
    
    return true;
}

template<typename T>
bool EnhancedConfig::updateIfChanged(const std::string& key, const T& newValue, ConfigSource source) {
    ConfigValue newConfigValue = newValue;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false; // Key doesn't exist
    }
    
    if (it->value == newConfigValue) {
        return false; // No change
    }
    
    ConfigValue oldValue = it->value;
    it->value = newConfigValue;
    it->source = source;
    it->lastModified = std::chrono::system_clock::now();
    
    notifyChange(key, newConfigValue, oldValue);
    return true;
}

template<typename T>
std::optional<T> EnhancedConfig::getValidated(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return std::get_if<T>(&it->value);
}

template<typename T>
T EnhancedConfig::getOr(const std::string& key, const T& defaultValue) const {
    auto val = getValidated<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
std::vector<T> EnhancedConfig::getVectorOr(const std::string& key, const std::vector<T>& defaultValue) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return defaultValue;
    }
    
    const auto& value = it->value;
    return std::get_if<std::vector<T>>(&value);
}

template<typename T>
std::vector<std::vector<T>> EnhancedConfig::getMatrixOr(const std::string& key, const std::vector<std::vector<T>>& defaultValue) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return defaultValue;
    }
    
    const auto& value = it->value;
    return std::get_if<std::vector<std::vector<T>>>(&value);
}

bool EnhancedConfig::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool EnhancedConfig::remove(const std::string& key) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    size_t removedCount = std::distance(it, pImpl->entries.end());
    pImpl->entries.erase(it, pImpl->entries.end());
    
    if (removedCount > 0) {
        notifyRemoval(key);
    }
    
    return true;
}

void EnhancedConfig::setBatch(const std::map<std::string, ConfigValue>& batch, ConfigSource source) {
    for (const auto& pair : batch) {
        setValidated(pair.first, pair.second, source);
    }
}

bool EnhancedConfig::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> EnhancedConfig::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry)) {
            errors.push_back("Invalid value for key '" + entry.key + "': " + getValidationError(entry));
        }
    }
    
    return errors;
}

ConfigSource EnhancedConfig::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    
    return ConfigSource::Default;
}

std::vector<std::string> EnhancedConfig::getKeys(ConfigSource source) const {
    std::vector<std::string> keys;
    
    for (const auto& entry : pImpl->entries) {
        if (source == ConfigSource::Any || entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

void EnhancedConfig::setMetadata(const std::string& key, const ConfigMetadata& metadata) {
    pImpl->metadata[key] = metadata;
}

ConfigMetadata EnhancedConfig::getMetadata(const std::string& key) const {
    auto it = pImpl->metadata.find(key);
    if (it != pImpl->metadata.end()) {
        return it->second;
    }
    
    return ConfigMetadata();
}

bool EnhancedConfig::hasMetadata(const std::string& key) const {
    return pImpl->metadata.find(key) != pImpl->metadata.end();
}

std::string EnhancedConfig::exportToString() const {
    std::ostringstream oss;
    oss << "# Enhanced NLM Configuration\n";
    oss << "# Generated: " << std::chrono::system_clock::now() << "\n";
    oss << "# Total entries: " << pImpl->entries.size() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.description << "\n";
        oss << "# Category: " << entry.metadata.category << "\n";
        oss << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        // Format value based on type
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                               std::is_same_v<T, std::vector<int64_t>> ||
                               std::is_same_v<T, std::vector<double>> ||
                               std::is_same_v<T, std::vector<float>> ||
                               std::is_same_v<T, std::vector<bool>> ||
                               std::is_same_v<T, std::vector<std::string>>) {
                oss << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::vector<double>>>) {
                oss << entry.key << " = [[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << "; ";
                    oss << "[";
                    for (size_t j = 0; j < arg[i].size(); ++j) {
                        if (j > 0) oss << ", ";
                        oss << arg[i][j];
                    }
                    oss << "]";
                }
                oss << "]]\n";
            } else {
                oss << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        oss << "\n";
    }
    
    return oss.str();
}

bool EnhancedConfig::importFromString(const std::string& configString, ConfigSource source) {
    std::istringstream iss(configString);
    std::string line;
    std::string currentDescription;
    std::string currentCategory;
    bool inSection = false;
    
    while (std::getline(iss, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#') {
            // Comment or empty line
            if (line.find("Category:") != std::string::npos) {
                currentCategory = line.substr(line.find("Category:") + 9);
                currentCategory.erase(0, currentCategory.find_first_not_of(" \t"));
            }
            continue;
        }
        
        if (line.find("Description:") != std::string::npos) {
            currentDescription = line.substr(line.find("Description:") + 12);
            currentDescription.erase(0, currentDescription.find_first_not_of(" \t"));
            inSection = true;
            continue;
        }
        
        if (line.find("Key:") != std::string::npos) {
            // Parse section header
            inSection = false;
            currentDescription.clear();
            continue;
        }
        
        if (inSection) {
            // Try to parse key=value pair
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(key);
                std::string value = line.substr(pos + 1);
                value.erase(0, value.find_first_not_of(" \t\"'"));
                
                // Parse value based on format
                ConfigValue parsedValue;
                if (value.front() == '[' && value.back() == ']') {
                    // Vector or matrix
                    std::string inner = value.substr(1, value.size() - 2);
                    // Check if it's a matrix (contains semicolon-separated rows)
                    if (inner.find(';') != std::string::npos) {
                        std::vector<std::vector<double>> matrix;
                        // Parse matrix...
                        // Simplified for now
                    } else {
                        std::vector<double> vector;
                        // Parse vector...
                        // Simplified for now
                    }
                } else {
                    // Try to convert to appropriate type
                    parsedValue = parseValue(value);
                }
                
                // Set with current metadata
                ConfigMetadata metadata;
                metadata.description = currentDescription;
                metadata.category = currentCategory;
                
                setValidated(key, parsedValue, source, currentDescription, metadata);
            }
        }
    }
    
    return true;
}

std::string EnhancedConfig::summary() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration Summary:\n";
    oss << "======================\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Sources:\n";
    
    std::map<ConfigSource, int> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    for (const auto& pair : sourceCount) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    
    oss << "\nCategories:\n";
    std::map<std::string, int> categoryCount;
    for (const auto& entry : pImpl->entries) {
        categoryCount[entry.metadata.category]++;
    }
    
    for (const auto& pair : categoryCount) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    oss << "\nValidation status: " << (validate() ? "PASS" : "FAIL") << "\n";
    
    if (!validate()) {
        auto errors = getValidationErrors();
        oss << "\nValidation errors:\n";
        for (const auto& error : errors) {
            oss << "  - " << error << "\n";
        }
    }
    
    return oss.str();
}

size_t EnhancedConfig::getEntryCount() const {
    return pImpl->entries.size();
}

size_t EnhancedConfig::getEntryCount(ConfigSource source) const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [source](const ConfigEntry& e) { return e.source == source; });
}

void EnhancedConfig::addChangeCallback(std::shared_ptr<ConfigChangeCallback> callback) {
    pImpl->changeCallbacks.push_back(callback);
}

void EnhancedConfig::removeChangeCallback(const std::shared_ptr<ConfigChangeCallback>& callback) {
    pImpl->changeCallbacks.erase(
        std::remove(pImpl->changeCallbacks.begin(), pImpl->changeCallbacks.end(), callback),
        pImpl->changeCallbacks.end()
    );
}

void EnhancedConfig::recordTimeSeries(const std::string& key, const ConfigValue& value, const std::string& context) {
    TimeSeriesPoint point;
    point.timestamp = std::chrono::system_clock::now();
    point.value = value;
    point.context = context;
    
    pImpl->timeSeries[key].push_back(point);
    
    // Cleanup old time series data periodically
    auto now = std::chrono::system_clock::now();
    if (std::chrono::duration_cast<std::chrono::hours>(now - pImpl->lastCleanup).count() > 1) {
        cleanupTimeSeries();
    }
}

std::vector<EnhancedConfig::TimeSeriesPoint> EnhancedConfig::getTimeSeries(const std::string& key) const {
    auto it = pImpl->timeSeries.find(key);
    if (it != pImpl->timeSeries.end()) {
        return it->second;
    }
    
    return std::vector<TimeSeriesPoint>();
}

void EnhancedConfig::clearTimeSeries(const std::string& key) {
    auto it = pImpl->timeSeries.find(key);
    if (it != pImpl->timeSeries.end()) {
        pImpl->timeSeries.erase(it);
    }
}

bool EnhancedConfig::equals(const EnhancedConfig& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (otherIt == other.pImpl->entries.end() || otherIt->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> EnhancedConfig::getDifferences(const EnhancedConfig& other) const {
    std::vector<std::string> differences;
    
    // Check for differences in this config vs other
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (otherIt == other.pImpl->entries.end() || otherIt->value != entry.value) {
            differences.push_back("Key '" + entry.key + "' differs or missing in other config");
        }
    }
    
    // Check for keys that exist in other but not in this
    for (const auto& otherEntry : other.pImpl->entries) {
        auto thisIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&otherEntry](const ConfigEntry& e) { return e.key == otherEntry.key; });
        
        if (thisIt == pImpl->entries.end()) {
            differences.push_back("Key '" + otherEntry.key + "' exists only in other config");
        }
    }
    
    return differences;
}

void EnhancedConfig::optimizeForPerformance() {
    // Sort entries by access frequency (if we had that data)
    std::sort(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& a, const ConfigEntry& b) {
            return a.key < b.key;
        });
    
    // Pre-compile validation rules
    for (auto& entry : pImpl->entries) {
        // Pre-validate known patterns
        validateValue(entry);
    }
    
    pImpl->optimizedForPerformance = true;
}

void EnhancedConfig::notifyChange(const std::string& key, const ConfigValue& newValue, const ConfigValue& oldValue) {
    for (const auto& callback : pImpl->changeCallbacks) {
        callback->onConfigChanged(key, newValue, oldValue);
    }
}

void EnhancedConfig::notifyAddition(const std::string& key, const ConfigValue& value) {
    for (const auto& callback : pImpl->changeCallbacks) {
        callback->onConfigAdded(key, value);
    }
}

void EnhancedConfig::notifyRemoval(const std::string& key) {
    for (const auto& callback : pImpl->changeCallbacks) {
        callback->onConfigRemoved(key);
    }
}

bool EnhancedConfig::validateValue(const ConfigEntry& entry) const {
    // Basic validation - could be extended
    return !entry.key.empty();
}

std::string EnhancedConfig::getValidationError(const ConfigEntry& entry) const {
    // Return validation error message
    return "Validation failed for key '" + entry.key + "'";
}

void EnhancedConfig::cleanupTimeSeries() {
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - std::chrono::hours(24); // Keep 24 hours of data
    
    for (auto& pair : pImpl->timeSeries) {
        auto& series = pair.second;
        series.erase(
            std::remove_if(series.begin(), series.end(),
                [cutoff](const TimeSeriesPoint& point) {
                    return point.timestamp < cutoff;
                }),
            series.end()
        );
    }
    
    pImpl->lastCleanup = now;
}

// Template instantiations
// This would be expanded in the full implementation

} // namespace nlm