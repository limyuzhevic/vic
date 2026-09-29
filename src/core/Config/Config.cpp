#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <unordered_set>
#include <queue>
#include <ctime>

namespace nlm {

// ConfigEntry implementation
bool ConfigEntry::isNumeric() const {
    return std::holds_alternative<int>(value) ||
           std::holds_alternative<int64_t>(value) ||
           std::holds_alternative<uint64_t>(value) ||
           std::holds_alternative<double>(value);
}

bool ConfigEntry::isBoolean() const {
    return std::holds_alternative<bool>(value);
}

bool ConfigEntry::isString() const {
    return std::holds_alternative<std::string>(value);
}

bool ConfigEntry::isArray() const {
    return std::holds_alternative<std::vector<int>>(value) ||
           std::holds_alternative<std::vector<int64_t>>(value) ||
           std::holds_alternative<std::vector<uint64_t>>(value) ||
           std::holds_alternative<std::vector<double>>(value) ||
           std::holds_alternative<std::vector<bool>>(value) ||
           std::holds_alternative<std::vector<std::string>>(value) ||
           std::holds_alternative<std::vector<std::vector<double>>>(value);
}

size_t ConfigEntry::getArraySize() const {
    if (std::holds_alternative<std::vector<int>>(value)) return std::get<std::vector<int>>(value).size();
    if (std::holds_alternative<std::vector<int64_t>>(value)) return std::get<std::vector<int64_t>>(value).size();
    if (std::holds_alternative<std::vector<uint64_t>>(value)) return std::get<std::vector<uint64_t>>(value).size();
    if (std::holds_alternative<std::vector<double>>(value)) return std::get<std::vector<double>>(value).size();
    if (std::holds_alternative<std::vector<bool>>(value)) return std::get<std::vector<bool>>(value).size();
    if (std::holds_alternative<std::vector<std::string>>(value)) return std::get<std::vector<std::string>>(value).size();
    if (std::holds_alternative<std::vector<std::vector<double>>>(value)) return std::get<std::vector<std::vector<double>>>(value).size();
    return 0;
}

template<typename T>
bool ConfigEntry::isType() const {
    return std::holds_alternative<T>(value);
}

bool ConfigEntry::validateValue(const ConfigValue& val) const {
    // Check minimum value if specified
    if (!minValue.empty()) {
        try {
            double minVal = std::stod(minValue);
            if (std::holds_alternative<double>(val)) {
                if (std::get<double>(val) < minVal) return false;
            } else if (std::holds_alternative<int>(val)) {
                if (static_cast<double>(std::get<int>(val)) < minVal) return false;
            } else if (std::holds_alternative<int64_t>(val)) {
                if (static_cast<double>(std::get<int64_t>(val)) < minVal) return false;
            } else if (std::holds_alternative<uint64_t>(val)) {
                if (static_cast<double>(std::get<uint64_t>(val)) < minVal) return false;
            }
        } catch (...) {
        }
    }
    
    // Check maximum value if specified
    if (!maxValue.empty()) {
        try {
            double maxVal = std::stod(maxValue);
            if (std::holds_alternative<double>(val)) {
                if (std::get<double>(val) > maxVal) return false;
            } else if (std::holds_alternative<int>(val)) {
                if (static_cast<double>(std::get<int>(val)) > maxVal) return false;
            } else if (std::holds_alternative<int64_t>(val)) {
                if (static_cast<double>(std::get<int64_t>(val)) > maxVal) return false;
            } else if (std::holds_alternative<uint64_t>(val)) {
                if (static_cast<double>(std::get<uint64_t>(val)) > maxVal) return false;
            }
        } catch (...) {
        }
    }
    
    return true;
}

std::string ConfigEntry::getValueTypeString() const {
    if (isNumeric()) return "numeric";
    if (isBoolean()) return "boolean";
    if (isString()) return "string";
    if (isArray()) return "array";
    return "unknown";
}

void ConfigEntry::updateAccess() const {
    ++accessCount;
    lastAccess = std::chrono::system_clock::now();
}

// ConfigSchema implementation
struct ConfigSchema::Impl {
    std::unordered_map<std::string, std::vector<ValidationRule>> rules;
};

ConfigSchema::ConfigSchema() : pImpl(std::make_unique<Impl>()) {}

ConfigSchema::~ConfigSchema() = default;

void ConfigSchema::addValidation(const std::string& key, RuleType type,
                                 const std::string& param1,
                                 const std::string& param2,
                                 const std::function<bool(const ConfigValue&)>& customValidator,
                                 const std::string& errorMessage) {
    ValidationRule rule;
    rule.type = type;
    rule.parameter1 = param1;
    rule.parameter2 = param2;
    rule.customValidator = customValidator;
    rule.errorMessage = errorMessage;
    
    pImpl->rules[key].push_back(rule);
}

std::vector<std::string> ConfigSchema::validateEntry(const ConfigEntry& entry) const {
    std::vector<std::string> errors;
    auto it = pImpl->rules.find(entry.key);
    if (it == pImpl->rules.end()) return errors;
    
    for (const auto& rule : it->second) {
        bool isValid = false;
        
        switch (rule.type) {
            case ValidationRule::RuleType::Range: {
                if (entry.isNumeric() && !rule.parameter1.empty() && !rule.parameter2.empty()) {
                    try {
                        double value = 0.0;
                        if (std::holds_alternative<double>(entry.value)) {
                            value = std::get<double>(entry.value);
                        } else if (std::holds_alternative<int>(entry.value)) {
                            value = static_cast<double>(std::get<int>(entry.value));
                        } else if (std::holds_alternative<int64_t>(entry.value)) {
                            value = static_cast<double>(std::get<int64_t>(entry.value));
                        } else if (std::holds_alternative<uint64_t>(entry.value)) {
                            value = static_cast<double>(std::get<uint64_t>(entry.value));
                        }
                        double min = std::stod(rule.parameter1);
                        double max = std::stod(rule.parameter2);
                        isValid = (value >= min && value <= max);
                        if (!isValid) {
                            if (!rule.errorMessage.empty()) {
                                errors.push_back(rule.errorMessage);
                            } else {
                                errors.push_back("Value " + std::to_string(value) + " is outside range [" + 
                                               rule.parameter1 + ", " + rule.parameter2 + "]");
                            }
                        }
                    } catch (...) {
                        if (!rule.errorMessage.empty()) {
                            errors.push_back(rule.errorMessage);
                        }
                    }
                }
                break;
            }
                
            case ValidationRule::RuleType::Regex: {
                if (entry.isString() && !rule.parameter1.empty()) {
                    try {
                        std::regex pattern(rule.parameter1);
                        isValid = std::regex_match(std::get<std::string>(entry.value), pattern);
                        if (!isValid) {
                            if (!rule.errorMessage.empty()) {
                                errors.push_back(rule.errorMessage);
                            } else {
                                errors.push_back("Value '" + std::get<std::string>(entry.value) + 
                                               "' does not match pattern '" + rule.parameter1 + "'");
                            }
                        }
                    } catch (const std::regex_error&) {
                    }
                }
                break;
            }
                
            case ValidationRule::RuleType::Length: {
                if (!rule.parameter1.empty()) {
                    try {
                        size_t requiredLength = std::stoul(rule.parameter1);
                        if (entry.isString()) {
                            isValid = std::get<std::string>(entry.value).size() == requiredLength;
                        } else if (entry.isArray()) {
                            isValid = entry.getArraySize() == requiredLength;
                        }
                    } catch (...) {}
                }
                break;
            }
                
            case ValidationRule::RuleType::Required:
                isValid = entry.required;
                break;
                
            case ValidationRule::RuleType::Custom:
                if (rule.customValidator) {
                    isValid = rule.customValidator(entry.value);
                }
                break;
        }
    }
    
    return errors;
}

std::vector<ConfigSchema::ValidationRule> ConfigSchema::getValidationRules(const std::string& key) const {
    auto it = pImpl->rules.find(key);
    if (it != pImpl->rules.end()) {
        return it->second;
    }
    return std::vector<ValidationRule>();
}

void ConfigSchema::clear() {
    pImpl->rules.clear();
}

bool ConfigSchema::hasRules(const std::string& key) const {
    return pImpl->rules.find(key) != pImpl->rules.end();
}

// Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::shared_ptr<ConfigSchema> schema;
    size_t memoryUsage;
    
    Impl() : memoryUsage(0) {
        schema = std::make_shared<ConfigSchema>();
        
        // Add common validation rules
        schema->addValidation("random_seed", ConfigSchema::ValidationRule::RuleType::Range, "0", "2147483647", "",
                             "Random seed must be in valid range [0, 2147483647]");
        schema->addValidation("neuron_count", ConfigSchema::ValidationRule::RuleType::Range, "1", "1000000", "",
                             "Neuron count must be positive and reasonable");
        schema->addValidation("connection_probability", ConfigSchema::ValidationRule::RuleType::Range, "0", "1", "",
                             "Connection probability must be between 0 and 1");
        schema->addValidation("simulation_timestep", ConfigSchema::ValidationRule::RuleType::Range, "0", "1", "",
                             "Simulation timestep must be positive and small");
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::string format;
    if (!detectFileFormat(filepath, format)) {
        return false;
    }
    
    if (format == "json") {
        return loadFromJSON(filepath);
    } else if (format == "yaml" || format == "yml") {
        return loadFromYAML(filepath);
    } else {
        return loadFromSimple(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '/' || line[0] == '#') {
            continue;
        }
        
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string valueStr = trim(line.substr(colonPos + 1));
            
            if (key.size() >= 2 && key.front() == '"' && key.back() == '"') {
                key = key.substr(1, key.size() - 2);
            }
            if (valueStr.size() >= 2 && valueStr.front() == '"' && valueStr.back() == '"') {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            if (valueStr.size() >= 2 && valueStr.front() == '[' && valueStr.back() == ']') {
                set(key, ConfigValue(std::vector<double>{}), ConfigSource::FileJSON);
            } else {
                set(key, valueStr, ConfigSource::FileJSON);
            }
        }
    }
    
    return true;
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentKey;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            currentKey = trim(line.substr(0, colonPos));
            std::string valueStr = trim(line.substr(colonPos + 1));
            
            if (valueStr.size() >= 2 && valueStr.front() == '"' && valueStr.back() == '"') {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            if (valueStr.find(':') != std::string::npos) {
                continue;
            }
            
            set(currentKey, valueStr, ConfigSource::FileYAML);
        }
    }
    
    return true;
}

bool Config::loadFromSimple(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        std::string key, value;
        if (parseSimpleFormat(line, key, value)) {
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    std::string format;
    if (!detectFileFormat(filepath, format)) {
        return false;
    }
    
    if (format == "json") {
        return saveToJSON(filepath, prettyPrint);
    } else if (format == "yaml" || format == "yml") {
        return saveToYAML(filepath);
    } else {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty()) {
                file << "# " << entry.description << "\n";
            }
            file << entry.key << " = " << serializeValue(entry.value) << "\n\n";
        }
        
        return true;
    }
}

bool Config::saveToJSON(const std::string& filepath, bool prettyPrint) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (prettyPrint) {
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": " << serializeValue(entry.value);
            if (i < pImpl->entries.size() - 1) file << ",\n";
            else file << "\n";
        }
        file << "}\n";
    } else {
        file << "{";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "\"" << entry.key << "\":" << serializeValue(entry.value);
            if (i < pImpl->entries.size() - 1) file << ",";
        }
        file << "}";
    }
    
    return true;
}

bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        file << entry.key << ": " << serializeValue(entry.value) << "\n";
    }
    
    return true;
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    return false;
}

bool Config::loadFromArgs(int argc, char** argv) {
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
}

void Config::setProgrammatic(const std::string& key, const ConfigValue& value,
                             const std::string& description,
                             const std::string& category) {
    set(key, value, ConfigSource::Programmatic, description, category);
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
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

std::string Config::getAsString(const std::string& key) const {
    auto val = get<std::string>(key);
    if (val.has_value()) {
        return val.value();
    }
    
    auto intVal = get<int>(key);
    if (intVal.has_value()) return std::to_string(intVal.value());
    
    auto doubleVal = get<double>(key);
    if (doubleVal.has_value()) return std::to_string(doubleVal.value());
    
    auto boolVal = get<bool>(key);
    if (boolVal.has_value()) return boolVal.value() ? "true" : "false";
    
    return "";
}

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source,
                 const std::string& description,
                 const std::string& category,
                 bool validate) {
    ConfigEntry entry(key, value, source, description, category);
    
    if (validate && pImpl->schema) {
        auto errors = pImpl->schema->validateEntry(entry);
        if (!errors.empty()) {
            throw std::runtime_error("Configuration validation failed for key \"" + 
                                   key + "\": " + errors[0]);
        }
    }
    
    entry.defaultValue = serializeValue(value);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        *it = entry;
    } else {
        pImpl->entries.push_back(entry);
    }
}

void Config::set(const std::string& key, const std::string& value,
                 ConfigSource source,
                 const std::string& description,
                 const std::string& category,
                 bool validate) {
    set(key, ConfigValue(value), source, description, category, validate);
}

void Config::set(const std::string& key, int value,
                 ConfigSource source,
                 const std::string& description,
                 const std::string& category,
                 bool validate) {
    set(key, ConfigValue(value), source, description, category, validate);
}

void Config::set(const std::string& key, double value,
                 ConfigSource source,
                 const std::string& description,
                 const std::string& category,
                 bool validate) {
    set(key, ConfigValue(value), source, description, category, validate);
}

void Config::set(const std::string& key, bool value,
                 ConfigSource source,
                 const std::string& description,
                 const std::string& category,
                 bool validate) {
    set(key, ConfigValue(value), source, description, category, validate);
}

bool Config::has(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end();
}

bool Config::isNumeric(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) return false;
    return it->isNumeric();
}

bool Config::isString(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) return false;
    return it->isString();
}

bool Config::isBoolean(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) return false;
    return it->isBoolean();
}

bool Config::isArray(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) return false;
    return it->isArray();
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->description;
    }
    return "";
}

std::string Config::getCategory(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->category;
    }
    return "general";
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    return ConfigSource::Default;
}

uint32_t Config::getAccessCount(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->getAccessCount();
    }
    return 0;
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

void Config::resetToDefaults() {
    for (auto& entry : pImpl->entries) {
        try {
            ConfigValue originalValue = deserializeValue(entry.defaultValue);
            entry.value = originalValue;
            entry.source = ConfigSource::Default;
        } catch (...) {}
    }
}

std::vector<std::string> Config::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<ConfigEntry> Config::getEntriesBySource(ConfigSource source) const {
    std::vector<ConfigEntry> entries;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            entries.push_back(entry);
        }
    }
    return entries;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [" << formatValue(entry.value) << "] (source=" 
            << static_cast<int>(entry.source) << ", category=" << entry.category << ")\n";
    }
    return oss.str();
}

std::string Config::summaryByCategory() const {
    std::ostringstream oss;
    oss << "Configuration by Category:\n";
    
    std::unordered_map<std::string, std::vector<ConfigEntry>> entriesByCategory;
    for (const auto& entry : pImpl->entries) {
        entriesByCategory[entry.category].push_back(entry);
    }
    
    for (const auto& pair : entriesByCategory) {
        oss << "  Category: " << pair.first << " (" << pair.second.size() << " entries)\n";
        for (const auto& entry : pair.second) {
            oss << "    " << entry.key << " = " << formatValue(entry.value) << "\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string Config::getUsageStatistics() const {
    std::ostringstream oss;
    oss << "Configuration Usage Statistics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    
    uint32_t totalAccesses = 0;
    for (const auto& entry : pImpl->entries) {
        totalAccesses += entry.accessCount;
    }
    oss << "  Total accesses: " << totalAccesses << "\n";
    
    if (!pImpl->entries.empty()) {
        oss << "  Average accesses per entry: " << (totalAccesses / pImpl->entries.size()) << "\n";
    }
    
    if (totalAccesses > 0) {
        std::string mostAccessedKey = "";
        uint32_t maxAccesses = 0;
        for (const auto& entry : pImpl->entries) {
            if (entry.accessCount > maxAccesses) {
                maxAccesses = entry.accessCount;
                mostAccessedKey = entry.key;
            }
        }
        if (!mostAccessedKey.empty()) {
            oss << "  Most accessed key: \"" << mostAccessedKey << "\" (" << maxAccesses << " times)\n";
        }
    }
    
    return oss.str();
}

std::string Config::diff(const Config& other) const {
    std::ostringstream oss;
    oss << "Configuration Differences:\n";
    
    std::unordered_set<std::string> thisKeys, otherKeys;
    for (const auto& entry : pImpl->entries) {
        thisKeys.insert(entry.key);
    }
    for (const auto& entry : other.pImpl->entries) {
        otherKeys.insert(entry.key);
    }
    
    std::vector<std::string> onlyInThis, onlyInOther, inBoth;
    for (const auto& key : thisKeys) {
        if (otherKeys.find(key) != otherKeys.end()) {
            inBoth.push_back(key);
        } else {
            onlyInThis.push_back(key);
        }
    }
    for (const auto& key : otherKeys) {
        if (thisKeys.find(key) == thisKeys.end()) {
            onlyInOther.push_back(key);
        }
    }
    
    if (!onlyInThis.empty()) {
        oss << "  Only in this config:\n";
        for (const auto& key : onlyInThis) {
            oss << "    - \"" << key << "\"\n";
        }
    }
    
    if (!onlyInOther.empty()) {
        oss << "  Only in other config:\n";
        for (const auto& key : onlyInOther) {
            oss << "    + \"" << key << "\"\n";
        }
    }
    
    if (!inBoth.empty()) {
        oss << "  Differences:\n";
        for (const auto& key : inBoth) {
            auto thisEntry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                         [&key](const ConfigEntry& e) { return e.key == key; });
            auto otherEntry = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                                          [&key](const ConfigEntry& e) { return e.key == key; });
            
            if (thisEntry != pImpl->entries.end() && otherEntry != other.pImpl->entries.end() &&
                thisEntry->value != otherEntry->value) {
                oss << "    \"" << key << "\":\"" << formatValue(thisEntry->value) << "\" vs \"" 
                    << formatValue(otherEntry->value) << "\"\n";
            }
        }
    }
    
    return oss.str();
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherEntry = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                                      [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (otherEntry == other.pImpl->entries.end() || otherEntry->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

void Config::setSchema(std::shared_ptr<ConfigSchema> schema) {
    pImpl->schema = schema;
}

std::shared_ptr<ConfigSchema> Config::getSchema() const {
    return pImpl->schema;
}

std::vector<std::string> Config::validateAll() const {
    std::vector<std::string> allErrors;
    if (!pImpl->schema) return allErrors;
    
    for (const auto& entry : pImpl->entries) {
        auto errors = pImpl->schema->validateEntry(entry);
        allErrors.insert(allErrors.end(), errors.begin(), errors.end());
    }
    
    return allErrors;
}

void Config::setBatch(const std::vector<std::pair<std::string, ConfigValue>>& values,
                      ConfigSource source) {
    for (const auto& pair : values) {
        set(pair.first, pair.second, source);
    }
}

bool Config::importFromString(const std::string& data, const std::string& format) {
    if (format == "json" || format == "auto") {
        clear();
        std::istringstream stream(data);
        std::string line;
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '{' || line[0] == '}') {
                continue;
            }
            std::string key, value;
            if (parseSimpleFormat(line, key, value)) {
                set(key, value, ConfigSource::Runtime);
            }
        }
        return true;
    } else if (format == "yaml" || format == "yml") {
        clear();
        std::istringstream stream(data);
        std::string line;
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '%') {
                continue;
            }
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(0, colonPos));
                std::string value = trim(line.substr(colonPos + 1));
                if (!value.empty() && value[0] == ' ') {
                    value = value.substr(1);
                }
                set(key, value, ConfigSource::Runtime);
            }
        }
        return true;
    }
    return false;
}

std::string Config::exportToString(const std::string& format, bool prettyPrint) const {
    if (format == "json") {
        std::ostringstream oss;
        oss << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            oss << "  \"" << entry.key << "\": " << formatValue(entry.value);
            if (i < pImpl->entries.size() - 1) oss << ",\n";
            else oss << "\n";
        }
        oss << "}\n";
        return oss.str();
    } else if (format == "yaml") {
        std::ostringstream oss;
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty()) {
                oss << "# " << entry.description << "\n";
            }
            oss << entry.key << ": " << formatValue(entry.value) << "\n";
        }
        return oss.str();
    } else if (format == "simple") {
        std::ostringstream oss;
        for (const auto& entry : pImpl->entries) {
            oss << entry.key << " = " << formatValue(entry.value) << "\n";
        }
        return oss.str();
    }
    return "";
}

void Config::optimizeMemoryLayout() {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [](const ConfigEntry& entry) {
                return entry.accessCount == 0;
            }),
        pImpl->entries.end()
    );
    
    pImpl->entries.shrink_to_fit();
}

size_t Config::getMemoryUsage() const {
    size_t total = sizeof(*pImpl);
    for (const auto& entry : pImpl->entries) {
        total += entry.key.capacity();
        total += entry.description.capacity();
        total += entry.category.capacity();
        total += entry.validationRegex.capacity();
        total += entry.minValue.capacity();
        total += entry.maxValue.capacity();
        total += entry.defaultValue.capacity();
    }
    return total;
}

// Explicit template instantiations
namespace {
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<int64_t>> Config::get<std::vector<int64_t>>(const std::string&) const;
template std::optional<std::vector<uint64_t>> Config::get<std::vector<uint64_t>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<bool>> Config::get<std::vector<bool>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
template std::optional<std::vector<std::vector<double>>> Config::get<std::vector<std::vector<double>>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}

// ConfigGetters implementation
namespace ConfigGetters {
template<typename T>
T ConfigGetters::getTyped(const Config& config, const std::string& key, const T& defaultValue) {
    auto val = config.get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}
} // namespace ConfigGetters

} // namespace nlm
