#include "ConfigManager.hpp"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <random>
#include <stdexcept>
#include <cctype>
#include <cmath>

namespace nlm {

// ConfigValue implementation
template<typename T>
TypedConfigValue<T>::TypedConfigValue(const T& value) : value_(value) {}

// ConfigManager implementation
bool ConfigManager::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto entryIt = findEntry(key);
    if (entryIt != entries_.end()) {
        entryIt->value->fromString(value);
        notifyMonitoring(key, *entryIt->value);
        return true;
    }
    return false;
}

bool ConfigManager::set(const std::string& key, int value) {
    return set(key, std::to_string(value));
}

bool ConfigManager::set(const std::string& key, float value) {
    return set(key, std::to_string(value));
}

bool ConfigManager::set(const std::string& key, bool value) {
    return set(key, value ? "true" : "false");
}

bool ConfigManager::set(const std::string& key, const std::vector<int>& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto entryIt = findEntry(key);
    if (entryIt != entries_.end()) {
        auto typedValue = std::dynamic_pointer_cast<TypedConfigValue<std::vector<int>>>(entryIt->value);
        if (typedValue) {
            typedValue->set(value);
            notifyMonitoring(key, *entryIt->value);
            return true;
        }
    }
    return false;
}

bool ConfigManager::set(const std::string& key, const std::vector<float>& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto entryIt = findEntry(key);
    if (entryIt != entries_.end()) {
        auto typedValue = std::dynamic_pointer_cast<TypedConfigValue<std::vector<float>>>(entryIt->value);
        if (typedValue) {
            typedValue->set(value);
            notifyMonitoring(key, *entryIt->value);
            return true;
        }
    }
    return false;
}

std::string ConfigManager::getString(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto entryIt = findEntry(key);
    if (entryIt != entries_.end()) {
        return entryIt->value->toString();
    }
    throw std::runtime_error("Configuration key not found: " + key);
}

int ConfigManager::getInt(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto entryIt = findEntry(key);
    if (entryIt != entries_.end()) {
        auto typedValue = std::dynamic_pointer_cast<TypedConfigValue<int>>(entryIt->value);
        if (typedValue) {
            return typedValue->get();
        }
    }
    throw std::runtime_error("Configuration key not found or wrong type: " + key);
}

float ConfigManager::getFloat(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto entryIt = findEntry(key);
    if (entryIt != entries_.end()) {
        auto typedValue = std::dynamic_pointer_cast<TypedConfigValue<float>>(entryIt->value);
        if (typedValue) {
            return typedValue->get();
        }
    }
    throw std::runtime_error("Configuration key not found or wrong type: " + key);
}

bool ConfigManager::getBool(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto entryIt = findEntry(key);
    if (entryIt != entries_.end()) {
        auto typedValue = std::dynamic_pointer_cast<TypedConfigValue<bool>>(entryIt->value);
        if (typedValue) {
            return typedValue->get();
        }
    }
    throw std::runtime_error("Configuration key not found or wrong type: " + key);
}

std::vector<int> ConfigManager::getVectorInt(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto entryIt = findEntry(key);
    if (entryIt != entries_.end()) {
        auto typedValue = std::dynamic_pointer_cast<TypedConfigValue<std::vector<int>>>(entryIt->value);
        if (typedValue) {
            return typedValue->get();
        }
    }
    throw std::runtime_error("Configuration key not found or wrong type: " + key);
}

std::vector<float> ConfigManager::getVectorFloat(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto entryIt = findEntry(key);
    if (entryIt != entries_.end()) {
        auto typedValue = std::dynamic_pointer_cast<TypedConfigValue<std::vector<float>>>(entryIt->value);
        if (typedValue) {
            return typedValue->get();
        }
    }
    throw std::runtime_error("Configuration key not found or wrong type: " + key);
}

bool ConfigManager::hasKey(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return findEntry(key) != entries_.end();
}

std::vector<std::string> ConfigManager::getAllKeys() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> keys;
    for (const auto& entry : entries_) {
        keys.push_back(entry.name);
    }
    return keys;
}

std::string ConfigManager::getDescription(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto entryIt = findEntry(key);
    if (entryIt != entries_.end()) {
        return entryIt->description;
    }
    return "";
}

bool ConfigManager::isRequired(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto entryIt = findEntry(key);
    if (entryIt != entries_.end()) {
        return entryIt->required;
    }
    return false;
}

bool ConfigManager::addEntry(const std::string& key, const std::string& value,
                             const std::string& description,
                             bool required,
                             const std::string& validationPattern,
                             std::function<bool(const ConfigValue&)> validator) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if key already exists
    if (findEntry(key) != entries_.end()) {
        return false;
    }
    
    // Create value from string
    auto configValue = createConfigValueFromString(value);
    if (!configValue) {
        return false;
    }
    
    ConfigEntry entry(key, configValue, description, required, validationPattern);
    entry.validator = validator;
    
    if (!validateEntry(entry)) {
        return false;
    }
    
    entries_.push_back(entry);
    notifyMonitoring(key, *entry.value);
    return true;
}

bool ConfigManager::addIntEntry(const std::string& key, int value,
                                const std::string& description,
                                bool required,
                                int minValue,
                                int maxValue) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (findEntry(key) != entries_.end()) {
        return false;
    }
    
    if (value < minValue || value > maxValue) {
        return false;
    }
    
    auto configValue = createConfigValue(value);
    
    auto validator = [minValue, maxValue](const ConfigValue& val) {
        auto typedValue = std::dynamic_pointer_cast<TypedConfigValue<int>>(const_cast<ConfigValue&>(val).clone());
        return typedValue && typedValue->get() >= minValue && typedValue->get() <= maxValue;
    };
    
    ConfigEntry entry(key, configValue, description, required);
    entry.validator = validator;
    
    if (!validateEntry(entry)) {
        return false;
    }
    
    entries_.push_back(entry);
    notifyMonitoring(key, *entry.value);
    return true;
}

bool ConfigManager::addFloatEntry(const std::string& key, float value,
                                  const std::string& description,
                                  bool required,
                                  float minValue,
                                  float maxValue) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (findEntry(key) != entries_.end()) {
        return false;
    }
    
    if (value < minValue || value > maxValue) {
        return false;
    }
    
    auto configValue = createConfigValue(value);
    
    auto validator = [minValue, maxValue](const ConfigValue& val) {
        auto typedValue = std::dynamic_pointer_cast<TypedConfigValue<float>>(const_cast<ConfigValue&>(val).clone());
        return typedValue && typedValue->get() >= minValue && typedValue->get() <= maxValue;
    };
    
    ConfigEntry entry(key, configValue, description, required);
    entry.validator = validator;
    
    if (!validateEntry(entry)) {
        return false;
    }
    
    entries_.push_back(entry);
    notifyMonitoring(key, *entry.value);
    return true;
}

bool ConfigManager::addBoolEntry(const std::string& key, bool value,
                                 const std::string& description,
                                 bool required) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (findEntry(key) != entries_.end()) {
        return false;
    }
    
    auto configValue = createConfigValue(value);
    
    ConfigEntry entry(key, configValue, description, required);
    entries_.push_back(entry);
    notifyMonitoring(key, *entry.value);
    return true;
}

bool ConfigManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    file.close();
    
    return loadFromString(content);
}

bool ConfigManager::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content = saveToString();
    file << content;
    
    file.close();
    
    saveCount_++;
    return true;
}

bool ConfigManager::loadFromString(const std::string& configString) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Clear existing entries
    entries_.clear();
    groups_.clear();
    
    // Simple parsing - key=value format, one per line
    std::istringstream stream(configString);
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse key=value
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            continue; // Skip invalid lines
        }
        
        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        // Skip if key already exists
        if (findEntry(key) != entries_.end()) {
            continue;
        }
        
        // Create value from string
        auto configValue = createConfigValueFromString(value);
        if (configValue) {
            ConfigEntry entry(key, configValue, "Loaded from string");
            entries_.push_back(entry);
        }
    }
    
    loadCount_++;
    return true;
}

std::string ConfigManager::saveToString() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream stream;
    
    for (const auto& entry : entries_) {
        stream << entry.name << "=" << entry.value->toString() << "\n";
    }
    
    return stream.str();
}

bool ConfigManager::loadFromCommandLine(int argc, char* argv[]) {
    // Simple command line parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            size_t equalsPos = key.find('=');
            
            if (equalsPos != std::string::npos) {
                std::string value = key.substr(equalsPos + 1);
                key = key.substr(0, equalsPos);
                set(key, value);
            } else {
                // Boolean flag
                set(key, true);
            }
        }
    }
    
    return true;
}

void ConfigManager::printUsage(const std::string& programName) const {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration options:" << std::endl;
    
    for (const auto& entry : entries_) {
        std::cout << "  --" << entry.name;
        if (entry.value->getType() == ConfigValueType::Bool) {
            std::cout << "[=true|false]";
        } else if (entry.value->getType() == ConfigValueType::String) {
            std::cout << "=<string>";
        } else if (entry.value->getType() == ConfigValueType::Int) {
            std::cout << "=<int>";
        } else if (entry.value->getType() == ConfigValueType::Float) {
            std::cout << "=<float>";
        }
        std::cout << " - " << entry.description << std::endl;
    }
}

bool ConfigManager::validate() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& entry : entries_) {
        if (!validateEntry(entry)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ConfigManager::getValidationErrors() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> errors;
    
    for (const auto& entry : entries_) {
        if (!validateEntry(entry)) {
            errors.push_back("Invalid configuration: " + entry.name);
        }
    }
    
    return errors;
}

bool ConfigManager::createGroup(const std::string& groupName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (groups_.find(groupName) != groups_.end()) {
        return false; // Group already exists
    }
    
    groups_[groupName] = std::vector<std::string>();
    return true;
}

bool ConfigManager::addToGroup(const std::string& key, const std::string& groupName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto groupIt = groups_.find(groupName);
    if (groupIt == groups_.end()) {
        return false;
    }
    
    if (findEntry(key) == entries_.end()) {
        return false;
    }
    
    groupIt->second.push_back(key);
    return true;
}

std::vector<std::string> ConfigManager::getKeysInGroup(const std::string& groupName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto groupIt = groups_.find(groupName);
    if (groupIt == groups_.end()) {
        return std::vector<std::string>();
    }
    
    return groupIt->second;
}

std::vector<std::string> ConfigManager::getAllGroups() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> groups;
    for (const auto& pair : groups_) {
        groups.push_back(pair.first);
    }
    return groups;
}

std::string ConfigManager::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream stream;
    stream << "=== Configuration Statistics ===\n";
    stream << "Name: " << name_ << "\n";
    stream << "Version: " << version_ << "\n";
    stream << "Load Count: " << loadCount_ << "\n";
    stream << "Save Count: " << saveCount_ << "\n";
    stream << "Uptime: " << getUptime() << " seconds\n";
    stream << "Total Entries: " << entries_.size() << "\n";
    stream << "Total Groups: " << groups_.size() << "\n";
    stream << "Thread Safe: " << (threadSafe_ ? "Yes" : "No") << "\n";
    stream << "Monitoring Enabled: " << (monitoringEnabled_ ? "Yes" : "No") << "\n";
    stream << "Encryption Enabled: " << (encryptionEnabled_ ? "Yes" : "No") << "\n";
    stream << "Compression Enabled: " << (compressionEnabled_ ? "Yes" : "No") << "\n";
    
    return stream.str();
}

double ConfigManager::getUptime() const {
    return std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count() - startTime_;
}

bool ConfigManager::createBackup(const std::string& backupPath) {
    std::string path = backupPath;
    if (path.empty()) {
        path = "config_backup_" + std::to_string(std::time(nullptr)) + ".cfg";
    }
    
    return saveToFile(path);
}

bool ConfigManager::restoreFromBackup(const std::string& backupPath) {
    return loadFromFile(backupPath);
}

std::string ConfigManager::getBackupList() const {
    // This would typically scan a directory for backup files
    // For now, return empty string
    return "";
}

bool ConfigManager::equals(const ConfigManager& other) const {
    std::lock_guard<std::mutex> lock1(mutex_);
    std::lock_guard<std::mutex> lock2(other.mutex_);
    
    if (entries_.size() != other.entries_.size()) {
        return false;
    }
    
    for (const auto& entry : entries_) {
        const auto& otherEntry = *other.findEntry(entry.name);
        if (entry.value->toString() != otherEntry.value->toString()) {
            return false;
        }
    }
    
    return true;
}

std::string ConfigManager::diff(const ConfigManager& other) const {
    std::lock_guard<std::mutex> lock1(mutex_);
    std::lock_guard<std::mutex> lock2(other.mutex_);
    
    std::ostringstream stream;
    
    for (const auto& entry : entries_) {
        const auto& otherEntry = *other.findEntry(entry.name);
        if (entry.value->toString() != otherEntry.value->toString()) {
            stream << "Difference in " << entry.name << ": "
                   << entry.value->toString() << " vs "
                   << otherEntry.value->toString() << "\n";
        }
    }
    
    return stream.str();
}

bool ConfigManager::merge(const ConfigManager& other, bool overrideExisting) {
    std::lock_guard<std::mutex> lock1(mutex_);
    std::lock_guard<std::mutex> lock2(other.mutex_);
    
    for (const auto& otherEntry : other.entries_) {
        auto entryIt = findEntry(otherEntry.name);
        if (entryIt != entries_.end()) {
            if (overrideExisting) {
                entryIt->value = otherEntry.value->clone();
                entryIt->description = otherEntry.description;
                entryIt->required = otherEntry.required;
            }
        } else {
            entries_.push_back(otherEntry);
        }
    }
    
    return true;
}

void ConfigManager::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    entries_.clear();
    groups_.clear();
}

void ConfigManager::resetGroup(const std::string& groupName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto groupIt = groups_.find(groupName);
    if (groupIt != groups_.end()) {
        for (const auto& key : groupIt->second) {
            auto entryIt = findEntry(key);
            if (entryIt != entries_.end()) {
                entries_.erase(entryIt);
            }
        }
        groupIt->second.clear();
    }
}

bool ConfigManager::saveTemplate(const std::string& templateName) {
    return saveToFile(templateName + ".template");
}

bool ConfigManager::loadTemplate(const std::string& templateName) {
    return loadFromFile(templateName + ".template");
}

std::string ConfigManager::getTemplateList() const {
    // This would typically scan a directory for template files
    // For now, return empty string
    return "";
}

bool ConfigManager::enableEncryption(const std::string& keyFile) {
    // Basic encryption implementation (not production-ready)
    encryptionEnabled_ = true;
    return true;
}

bool ConfigManager::disableEncryption() {
    encryptionEnabled_ = false;
    return true;
}

ConfigManager::ConfigEntry* ConfigManager::findEntry(const std::string& key) {
    for (auto& entry : entries_) {
        if (entry.name == key) {
            return &entry;
        }
    }
    return nullptr;
}

const ConfigManager::ConfigEntry* ConfigManager::findEntry(const std::string& key) const {
    for (const auto& entry : entries_) {
        if (entry.name == key) {
            return &entry;
        }
    }
    return nullptr;
}

template<typename T>
std::shared_ptr<ConfigValue> ConfigManager::createConfigValue(const T& value) {
    return std::make_shared<TypedConfigValue<T>>(value);
}

std::shared_ptr<ConfigValue> ConfigManager::createConfigValueFromString(const std::string& str) {
    // Try to parse as different types
    if (str.empty()) {
        return createConfigValue(std::string(""));
    }
    
    // Try int
    try {
        size_t pos;
        int intValue = std::stoi(str, &pos);
        if (pos == str.length()) {
            return createConfigValue(intValue);
        }
    } catch (...) {}
    
    // Try float
    try {
        size_t pos;
        float floatValue = std::stof(str, &pos);
        if (pos == str.length()) {
            return createConfigValue(floatValue);
        }
    } catch (...) {}
    
    // Try bool
    if (str == "true" || str == "false" || str == "1" || str == "0") {
        bool boolValue = (str == "true" || str == "1");
        return createConfigValue(boolValue);
    }
    
    // Default to string
    return createConfigValue(str);
}

std::string ConfigManager::serializeValue(const ConfigValue& value) const {
    return value.toString();
}

bool ConfigManager::deserializeValue(const std::string& str, ConfigValue& value) {
    try {
        value.fromString(str);
        return true;
    } catch (...) {
        return false;
    }
}

bool ConfigManager::validateEntry(const ConfigEntry& entry) const {
    if (entry.required && (entry.value == nullptr || entry.value->toString().empty())) {
        return false;
    }
    
    if (entry.validator && !entry.validator(*entry.value)) {
        return false;
    }
    
    return true;
}

void ConfigManager::notifyMonitoring(const std::string& key, const ConfigValue& value) const {
    if (monitoringEnabled_ && monitoringCallback_) {
        monitoringCallback_(key, value);
    }
}

} // namespace nlm