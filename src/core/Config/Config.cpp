#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <cmath>
#include <random>
#include <regex>
#include <iomanip>
#include <nlohmann/json.hpp>

namespace nlm {

// Define ConfigPresets static member
std::map<std::string, ConfigPresets::Preset> ConfigPresets::presets_;

// Config helper functions implementation
namespace config_helpers {
    std::string toString(const ConfigValue& value) {
        std::ostringstream oss;
        
        if (std::holds_alternative<int>(value)) {
            oss << std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            oss << std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            oss << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            oss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            oss << std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            const auto& vec = std::get<std::vector<int>>(value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& vec = std::get<std::vector<double>>(value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            const auto& vec = std::get<std::vector<std::string>>(value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << '"' << vec[i] << '"';
            }
            oss << "]";
        } else if (std::holds_alternative<std::map<std::string, std::string>>(value)) {
            const auto& map = std::get<std::map<std::string, std::string>>(value);
            oss << "{";
            for (auto it = map.begin(); it != map.end(); ++it) {
                if (it != map.begin()) oss << ", ";
                oss << '"' << it->first << "": '"' << it->second << '"';
            }
            oss << "}";
        }
        
        return oss.str();
    }
    
    ConfigValue fromString(const std::string& str, const std::string& key) {
        // Try to parse as boolean
        if (str == "true" || str == "false" || str == "1" || str == "0") {
            return str == "true" || str == "1";
        }
        
        // Try to parse as integer
        try {
            size_t pos;
            int intVal = std::stoi(str, &pos);
            if (pos == str.size()) {
                return intVal;
            }
        } catch (...) {}
        
        // Try to parse as int64_t
        try {
            size_t pos;
            int64_t int64Val = std::stoll(str, &pos);
            if (pos == str.size()) {
                return int64Val;
            }
        } catch (...) {}
        
        // Try to parse as double
        try {
            size_t pos;
            double doubleVal = std::stod(str, &pos);
            if (pos == str.size()) {
                return doubleVal;
            }
        } catch (...) {}
        
        // Check for vector patterns
        if (str.front() == '[' && str.back() == ']') {
            std::string content = str.substr(1, str.size() - 2);
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream iss(content);
            
            while (std::getline(iss, token, ',')) {
                token = trim(token);
                if (!token.empty()) {
                    tokens.push_back(token);
                }
            }
            
            if (!tokens.empty()) {
                // Try to determine vector type
                bool allInts = true;
                bool allFloats = true;
                bool allStrings = true;
                
                for (const auto& t : tokens) {
                    if (allInts) {
                        try {
                            std::stoi(t);
                        } catch (...) { allInts = false; }
                    }
                    if (allFloats) {
                        try {
                            std::stod(t);
                        } catch (...) { allFloats = false; }
                    }
                    if (allStrings) {
                        // Not a numeric string, treat as string
                        allStrings = true;
                    }
                }
                
                if (allInts) {
                    std::vector<int> intList;
                    intList.reserve(tokens.size());
                    for (const auto& t : tokens) {
                        intList.push_back(std::stoi(t));
                    }
                    return intList;
                } else if (allFloats) {
                    std::vector<double> floatList;
                    floatList.reserve(tokens.size());
                    for (const auto& t : tokens) {
                        floatList.push_back(std::stod(t));
                    }
                    return floatList;
                } else {
                    std::vector<std::string> stringList;
                    stringList.reserve(tokens.size());
                    for (const auto& t : tokens) {
                        stringList.push_back(trim(t));
                    }
                    return stringList;
                }
            }
        }
        
        // Check for map patterns
        if (str.front() == '{' && str.back() == '}') {
            std::string content = str.substr(1, str.size() - 2);
            std::map<std::string, std::string> map;
            std::string key;
            std::string value;
            std::istringstream iss(content);
            bool parsingKey = true;
            
            for (char c : content) {
                if (c == '"') {
                    if (parsingKey) {
                        key.clear();
                        parsingKey = false;
                    } else {
                        if (!key.empty() && !value.empty()) {
                            map[key] = value;
                            key.clear();
                            value.clear();
                        }
                        parsingKey = true;
                    }
                } else if (c == ':') {
                    parsingKey = false;
                } else if (c == ',') {
                    if (!key.empty() && !value.empty()) {
                        map[key] = value;
                        key.clear();
                        value.clear();
                    }
                } else {
                    if (parsingKey) {
                        key += c;
                    } else {
                        value += c;
                    }
                }
            }
            
            if (!key.empty() && !value.empty()) {
                map[key] = value;
            }
            
            if (!map.empty()) {
                return map;
            }
        }
        
        // Default to string
        return str;
    }
    
    bool isValidKey(const std::string& key) {
        if (key.empty()) return false;
        if (key.find(' ') != std::string::npos) return false;
        if (key.find('\t') != std::string::npos) return false;
        if (key.find('\n') != std::string::npos) return false;
        if (key.find('=') != std::string::npos) return false;
        return true;
    }
    
    const char* getTypeName(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64_t";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "int_list";
        if (std::holds_alternative<std::vector<double>>(value)) return "float_list";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "string_list";
        if (std::holds_alternative<std::map<std::string, std::string>>(value)) return "map";
        return "unknown";
    }
    
    std::string formatValue(const ConfigValue& value, size_t width) {
        std::string str = toString(value);
        if (width > 0 && str.size() > width) {
            return str.substr(0, width - 3) + "...";
        }
        return str;
    }
    
    void mergeConfig(Config& target, const Config& source) {
        for (const auto& sourceEntry : source.getKeys()) {
            auto sourceValue = source.get<std::string>(sourceEntry);
            if (sourceValue) {
                target.set(sourceEntry, *sourceValue);
            }
        }
    }
}

// Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ChangeRecord> changeHistory;
    std::map<std::string, ValidationRule> validationRules;
    bool autoSaveEnabled;
    std::string autoSavePath;
    int autoSaveInterval;
    std::chrono::steady_clock::time_point lastAutoSave;
    
    Impl() : autoSaveEnabled(false), autoSaveInterval(60), lastAutoSave(std::chrono::steady_clock::now()) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear(ConfigSource::File);
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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
            
            set(key, value, ConfigSource::File, "Loaded from file line " + std::to_string(lineNumber));
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool hasArgs = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
                hasArgs = true;
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            hasArgs = true;
        }
    }
    
    return hasArgs;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << " (" << config_helpers::getTypeName(entry.value) << ")\n";
        file << entry.key << " = " << config_helpers::formatValue(entry.value) << "\n";
    }
    
    return true;
}

bool Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source,
                 const std::string& description,
                 const std::string& validationRegex,
                 const std::string& defaultValue,
                 bool required) {
    if (!config_helpers::isValidKey(key)) {
        return false;
    }
    
    // Check for validation rules
    if (!pImpl->validationRules.empty()) {
        auto it = pImpl->validationRules.find(key);
        if (it != pImpl->validationRules.end()) {
            if (!it->second.validator(value)) {
                return false;
            }
        }
    }
    
    // Record change in history
    ConfigValue oldValue;
    bool exists = false;
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            oldValue = entry.value;
            entry.value = value;
            entry.source = source;
            entry.description = description.empty() ? entry.description : description;
            entry.validationRegex = validationRegex.empty() ? entry.validationRegex : validationRegex;
            entry.defaultValue = defaultValue.empty() ? entry.defaultValue : defaultValue;
            entry.required = required;
            exists = true;
            
            // Add to change history
            ChangeRecord record;
            record.key = key;
            record.oldValue = oldValue;
            record.newValue = value;
            record.source = source;
            record.timestamp = std::chrono::system_clock::now();
            record.description = description.empty() ? "Updated configuration" : description;
            pImpl->changeHistory.push_back(record);
            break;
        }
    }
    
    if (!exists) {
        ConfigEntry newEntry(key, value, source, description, validationRegex, defaultValue, required);
        pImpl->entries.push_back(newEntry);
        
        // Add to change history
        ChangeRecord record;
        record.key = key;
        record.oldValue = ConfigValue(); // Empty value
        record.newValue = value;
        record.source = source;
        record.timestamp = std::chrono::system_clock::now();
        record.description = description.empty() ? "Added configuration" : description;
        pImpl->changeHistory.push_back(record);
    }
    
    return true;
}

bool Config::setInt(const std::string& key, int64_t value, ConfigSource source) {
    return set(key, value, source);
}

bool Config::setFloat(const std::string& key, float value, ConfigSource source) {
    return set(key, value, source);
}

bool Config::setBool(const std::string& key, bool value, ConfigSource source) {
    return set(key, value, source);
}

bool Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    return set(key, value, source);
}

bool Config::setList(const std::string& key, const std::vector<int64_t>& value, ConfigSource source) {
    return set(key, value, source);
}

bool Config::setList(const std::string& key, const std::vector<float>& value, ConfigSource source) {
    return set(key, value, source);
}

bool Config::setList(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    return set(key, value, source);
}

std::optional<int64_t> Config::getInt(const std::string& key) const {
    auto value = get<int64_t>(key);
    return value;
}

std::optional<float> Config::getFloat(const std::string& key) const {
    auto value = get<float>(key);
    return value;
}

std::optional<bool> Config::getBool(const std::string& key) const {
    auto value = get<bool>(key);
    return value;
}

std::optional<std::string> Config::getString(const std::string& key) const {
    auto value = get<std::string>(key);
    return value;
}

std::optional<std::vector<int64_t>> Config::getIntList(const std::string& key) const {
    auto value = get<std::vector<int64_t>>(key);
    return value;
}

std::optional<std::vector<float>> Config::getFloatList(const std::string& key) const {
    auto value = get<std::vector<float>>(key);
    return value;
}

std::optional<std::vector<std::string>> Config::getStringList(const std::string& key) const {
    auto value = get<std::vector<std::string>>(key);
    return value;
}

ConfigValidationResult Config::validate() const {
    ConfigValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.isValid()) {
            result.addError("Invalid configuration for key: " + entry.key);
        }
        
        // Check if required entry is missing
        if (entry.required) {
            bool found = false;
            for (const auto& otherEntry : pImpl->entries) {
                if (otherEntry.key == entry.key) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                result.addError("Required configuration missing: " + entry.key);
            }
        }
    }
    
    return result;
}

bool Config::has(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end();
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigSource::Default;
    }
    
    return it->source;
}

void Config::remove(const std::string& key) {
    remove(key, ConfigSource::Runtime);
}

void Config::remove(const std::string& key, ConfigSource source) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key, source](const ConfigEntry& e) {
            return e.key == key && (source == ConfigSource::Default || e.source == source);
        });
    
    pImpl->entries.erase(it, pImpl->entries.end());
}

std::vector<std::string> Config::getKeys() const {
    return getKeys(ConfigSource::Default);
}

std::vector<std::string> Config::getKeys(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (source == ConfigSource::Default || entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    clear(ConfigSource::Runtime);
}

void Config::clear(ConfigSource source) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [source](const ConfigEntry& e) {
            return source == ConfigSource::Default || e.source == source;
        });
    
    pImpl->entries.erase(it, pImpl->entries.end());
}

std::string Config::summary() const {
    return summary(ConfigSource::Default);
}

std::string Config::summary(ConfigSource source) const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << getKeys(source).size() << " entries)";
    
    if (!pImpl->entries.empty()) {
        oss << "\n\nKeys:";
        for (const auto& entry : pImpl->entries) {
            if (source == ConfigSource::Default || entry.source == source) {
                oss << "\n  " << entry.key << " (" << config_helpers::getTypeName(entry.value) << ") - Source: " << static_cast<int>(entry.source);
            }
        }
    }
    
    return oss.str();
}

std::string Config::getDetailedSummary() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration ===\n";
    oss << "Total Entries: " << pImpl->entries.size() << "\n";
    
    std::map<ConfigSource, size_t> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[entry.source]++;
    }
    
    oss << "\nEntries by Source:";
    for (const auto& pair : sourceCounts) {
        oss << "\n  " << static_cast<int>(pair.first) << ": " << pair.second;
    }
    
    auto validationResult = validate();
    oss << "\n\nValidation: " << (validationResult.valid ? "PASS" : "FAIL");
    if (!validationResult.errors.empty()) {
        oss << "\nErrors:";
        for (const auto& error : validationResult.errors) {
            oss << "\n  - " << error;
        }
    }
    
    if (!validationResult.warnings.empty()) {
        oss << "\nWarnings:";
        for (const auto& warning : validationResult.warnings) {
            oss << "\n  - " << warning;
        }
    }
    
    oss << "\n";
    return oss.str();
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.getKeys()) {
        auto value = other.get<std::string>(entry);
        if (value) {
            set(entry, *value, source);
        }
    }
}

std::string Config::exportToJSON() const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        std::string strValue = config_helpers::toString(entry.value);
        json[entry.key] = strValue;
    }
    
    return json.dump(2);
}

bool Config::importFromJSON(const std::string& json) {
    try {
        auto jsonObj = nlohmann::json::parse(json);
        clear(ConfigSource::File);
        
        for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
            std::string key = it.key();
            std::string strValue = it.value().get<std::string>();
            
            ConfigValue value = config_helpers::fromString(strValue, key);
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const nlohmann::json::parse_error&) {
        return false;
    }
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCount(ConfigSource source) const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (source == ConfigSource::Default || entry.source == source) {
            count++;
        }
    }
    return count;
}

float Config::getValidationScore() const {
    auto validationResult = validate();
    if (validationResult.errors.empty()) {
        return 1.0f;
    }
    
    // Simple score based on ratio of valid entries
    return 1.0f - (validationResult.errors.size() / static_cast<float>(pImpl->entries.size()));
}

bool Config::applyPreset(const std::string& presetName) {
    try {
        auto presetConfig = ConfigPresets::applyPreset(presetName);
        merge(*presetConfig, ConfigSource::Preset);
        return true;
    } catch (const std::runtime_error&) {
        return false;
    }
}

std::vector<std::string> Config::getAvailablePresets() const {
    return ConfigPresets::getPresetNames();
}

std::string Config::getPresetDescription(const std::string& presetName) const {
    return ConfigPresets::getPresetDescription(presetName);
}

bool Config::equals(const Config& other) const {
    return getKeys() == other.getKeys();
}

bool Config::differs(const Config& other) const {
    return !equals(other);
}

void Config::enableAutoSave(const std::string& filepath, int intervalSeconds) {
    pImpl->autoSaveEnabled = true;
    pImpl->autoSavePath = filepath;
    pImpl->autoSaveInterval = intervalSeconds;
    pImpl->lastAutoSave = std::chrono::steady_clock::now();
}

void Config::disableAutoSave() {
    pImpl->autoSaveEnabled = false;
    pImpl->autoSavePath.clear();
}

bool Config::isAutoSaveEnabled() const {
    return pImpl->autoSaveEnabled;
}

std::shared_ptr<Config> Config::clone() const {
    auto clone = std::make_shared<Config>();
    
    // Copy all entries
    for (const auto& entry : pImpl->entries) {
        clone->set(entry.key, entry.value, entry.source, entry.description,
                   entry.validationRegex, entry.defaultValue, entry.required);
    }
    
    return clone;
}

std::vector<Config::ChangeRecord> Config::getChangeHistory() const {
    return pImpl->changeHistory;
}

void Config::clearChangeHistory() {
    pImpl->changeHistory.clear();
}

void Config::addValidationRule(const ValidationRule& rule) {
    pImpl->validationRules[rule.key] = rule;
}

void Config::clearValidationRules() {
    pImpl->validationRules.clear();
}

std::vector<std::string> Config::findKeys(const std::string& pattern) const {
    std::vector<std::string> matches;
    std::regex regex(pattern);
    
    for (const auto& entry : pImpl->entries) {
        if (std::regex_search(entry.key, regex)) {
            matches.push_back(entry.key);
        }
    }
    
    return matches;
}

std::vector<std::string> Config::findKeys(const std::regex& pattern) const {
    std::vector<std::string> matches;
    
    for (const auto& entry : pImpl->entries) {
        if (std::regex_search(entry.key, pattern)) {
            matches.push_back(entry.key);
        }
    }
    
    return matches;
}

void Config::setMultiple(const std::map<std::string, ConfigValue>& values, ConfigSource source) {
    for (const auto& pair : values) {
        set(pair.first, pair.second, source);
    }
}

std::map<std::string, ConfigValue> Config::getMultiple(const std::vector<std::string>& keys) const {
    std::map<std::string, ConfigValue> result;
    
    for (const auto& key : keys) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });n        
        if (it != pImpl->entries.end()) {
            result[key] = it->value;
        }
    }
    
    return result;
}

// Helper function implementations
std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool Config::isValidKeyFormat(const std::string& key) {
    if (key.empty()) return false;
    if (key.find(' ') != std::string::npos) return false;
    if (key.find('\t') != std::string::npos) return false;
    if (key.find('\n') != std::string::npos) return false;
    if (key.find('=') != std::string::npos) return false;
    return true;
}

ConfigValue Config::convertToVariant(const std::string& str) {
    return config_helpers::fromString(str);
}

std::string Config::variantToString(const ConfigValue& value) {
    return config_helpers::toString(value);
}

bool Config::validateAgainstRegex(const std::string& value, const std::string& regex) {
    if (regex.empty()) return true;
    try {
        std::regex pattern(regex);
        return std::regex_match(value, pattern);
    } catch (...) {
        return false;
    }
}

bool Config::validateValueRange(const ConfigValue& value, const std::string& key) {
    // Implementation for range validation
    return true;
}

} // namespace nlm