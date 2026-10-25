#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cctype>
#include <unordered_map>
#include <iomanip>
#include <numeric>
#include <ctime>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ValidationRule> validationRules;
    bool validationEnabled;
    
    Impl() : validationEnabled(true) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    SimpleJSONParser parser;
    auto jsonData = parser.parseFile(filepath);
    
    if (!parser.isValid()) {
        return false;
    }
    
    pImpl->entries.clear();
    
    for (const auto& pair : jsonData) {
        const std::string& key = pair.first;
        const JSONValue& jsonValue = pair.second;
        
        JSONError error;
        std::string errorMsg;
        ConfigValue configValue = jsonToConfigValue(jsonValue, error, errorMsg);
        
        if (error != JSONError::None) {
            return false;
        }
        
        set(key, configValue, ConfigSource::File);
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    SimpleJSONParser parser;
    auto jsonData = parser.parseString(jsonStr);
    
    if (!parser.isValid()) {
        return false;
    }
    
    pImpl->entries.clear();
    
    for (const auto& pair : jsonData) {
        const std::string& key = pair.first;
        const JSONValue& jsonValue = pair.second;
        
        JSONError error;
        std::string errorMsg;
        ConfigValue configValue = jsonToConfigValue(jsonValue, error, errorMsg);
        
        if (error != JSONError::None) {
            return false;
        }
        
        set(key, configValue, ConfigSource::Runtime);
    }
    
    return true;
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    bool first = true;
    
    for (const auto& entry : pImpl->entries) {
        if (!first) file << ",\n";
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        file << "\n";
    }
    
    file << "}\n";
    
    return true;
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
std::optional<T> Config::getWithValidation(const std::string& key, 
                                           const ValidationRule& rule) const {
    // First check if key exists
    auto value = get<T>(key);
    if (!value.has_value()) {
        if (rule.required) {
            return std::nullopt;
        }
        return std::nullopt;
    }
    
    // Validate type
    if (rule.type != "any") {
        bool typeMatches = false;
        if (rule.type == "int" && std::is_same_v<T, int>) typeMatches = true;
        else if (rule.type == "int64" && std::is_same_v<T, int64_t>) typeMatches = true;
        else if (rule.type == "double" && std::is_same_v<T, double>) typeMatches = true;
        else if (rule.type == "string" && std::is_same_v<T, std::string>) typeMatches = true;
        else if (rule.type == "bool" && std::is_same_v<T, bool>) typeMatches = true;
        else if (rule.type == "int_array" && std::is_same_v<T, std::vector<int64_t>>) typeMatches = true;
        else if (rule.type == "double_array" && std::is_same_v<T, std::vector<double>>) typeMatches = true;
        else if (rule.type == "string_array" && std::is_same_v<T, std::vector<std::string>>) typeMatches = true;
        
        if (!typeMatches) {
            return std::nullopt;
        }
    }
    
    // Validate value ranges
    if (rule.minValue.has_value() && value.value() < rule.minValue.value()) {
        return std::nullopt;
    }
    if (rule.maxValue.has_value() && value.value() > rule.maxValue.value()) {
        return std::nullopt;
    }
    
    return value;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
T Config::getOrWithValidation(const std::string& key, const T& defaultValue,
                             const ValidationRule& rule) const {
    auto val = getWithValidation<T>(key, rule);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

bool Config::setWithValidation(const std::string& key, const ConfigValue& value,
                               ConfigSource source, const ValidationRule& rule) {
    // Check type matching
    bool typeMatches = false;
    if (rule.type == "int" && std::holds_alternative<int>(value)) typeMatches = true;
    else if (rule.type == "int64" && std::holds_alternative<int64_t>(value)) typeMatches = true;
    else if (rule.type == "double" && std::holds_alternative<double>(value)) typeMatches = true;
    else if (rule.type == "string" && std::holds_alternative<std::string>(value)) typeMatches = true;
    else if (rule.type == "bool" && std::holds_alternative<bool>(value)) typeMatches = true;
    else if (rule.type == "int_array" && std::holds_alternative<std::vector<int64_t>>(value)) typeMatches = true;
    else if (rule.type == "double_array" && std::holds_alternative<std::vector<double>>(value)) typeMatches = true;
    else if (rule.type == "string_array" && std::holds_alternative<std::vector<std::string>>(value)) typeMatches = true;
    
    if (!typeMatches) {
        return false;
    }
    
    // Check range validation
    auto getNumberValue = [](const ConfigValue& val) -> std::optional<double> {
        if (std::holds_alternative<int>(val)) {
            return static_cast<double>(std::get<int>(val));
        } else if (std::holds_alternative<int64_t>(val)) {
            return static_cast<double>(std::get<int64_t>(val));
        } else if (std::holds_alternative<double>(val)) {
            return std::get<double>(val);
        }
        return std::nullopt;
    };
    
    auto numVal = getNumberValue(value);
    if (numVal.has_value()) {
        if (rule.minValue.has_value() && numVal.value() < rule.minValue.value()) {
            return false;
        }
        if (rule.maxValue.has_value() && numVal.value() > rule.maxValue.value()) {
            return false;
        }
    }
    
    set(key, value, source);
    return true;
}

void Config::set(const std::string& key, const std::string& value, 
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, 
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, 
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, 
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<int>& values, 
                 ConfigSource source) {
    set(key, ConfigValue(std::vector<int64_t>(values.begin(), values.end())), source);
}

void Config::set(const std::string& key, const std::vector<double>& values, 
                 ConfigSource source) {
    set(key, ConfigValue(values), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& values, 
                 ConfigSource source) {
    set(key, ConfigValue(values), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool Config::hasType(const std::string& key, const std::string& type) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    if (type == "int" && std::holds_alternative<int>(it->value)) return true;
    if (type == "int64" && std::holds_alternative<int64_t>(it->value)) return true;
    if (type == "double" && std::holds_alternative<double>(it->value)) return true;
    if (type == "string" && std::holds_alternative<std::string>(it->value)) return true;
    if (type == "bool" && std::holds_alternative<bool>(it->value)) return true;
    if (type == "int_array" && std::holds_alternative<std::vector<int64_t>>(it->value)) return true;
    if (type == "double_array" && std::holds_alternative<std::vector<double>>(it->value)) return true;
    if (type == "string_array" && std::holds_alternative<std::vector<std::string>>(it->value)) return true;
    
    return false;
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    // Also remove validation rule
    pImpl->validationRules.erase(
        std::remove_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
            [&key](const ValidationRule& r) { return r.key == key; }),
        pImpl->validationRules.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysByType(const std::string& type) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        bool matches = false;
        if (type == "int" && std::holds_alternative<int>(entry.value)) matches = true;
        else if (type == "int64" && std::holds_alternative<int64_t>(entry.value)) matches = true;
        else if (type == "double" && std::holds_alternative<double>(entry.value)) matches = true;
        else if (type == "string" && std::holds_alternative<std::string>(entry.value)) matches = true;
        else if (type == "bool" && std::holds_alternative<bool>(entry.value)) matches = true;
        else if (type == "int_array" && std::holds_alternative<std::vector<int64_t>>(entry.value)) matches = true;
        else if (type == "double_array" && std::holds_alternative<std::vector<double>>(entry.value)) matches = true;
        else if (type == "string_array" && std::holds_alternative<std::vector<std::string>>(entry.value)) matches = true;
        
        if (matches) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validationRules.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " 
        << pImpl->validationRules.size() << " validation rules):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    if (!pImpl->validationRules.empty()) {
        oss << "\nValidation Rules:\n";
        for (const auto& rule : pImpl->validationRules) {
            oss << "  " << rule.key << " (" << rule.type << ")";
            if (rule.required) oss << " [REQUIRED]";
            if (rule.minValue.has_value()) oss << " [min=" << rule.minValue.value() << "]";
            if (rule.maxValue.has_value()) oss << " [max=" << rule.maxValue.value() << "]";
            oss << "\n";
        }
    }
    
    return oss.str();
}

std::string Config::getStatistics() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    oss << "  Validation enabled: " << (pImpl->validationEnabled ? "yes" : "no") << "\n";
    oss << "  Validation rules: " << pImpl->validationRules.size() << "\n";
    
    // Count by type
    std::map<std::string, int> typeCounts;
    for (const auto& entry : pImpl->entries) {
        std::string type;
        if (std::holds_alternative<int>(entry.value)) type = "int";
        else if (std::holds_alternative<int64_t>(entry.value)) type = "int64";
        else if (std::holds_alternative<double>(entry.value)) type = "double";
        else if (std::holds_alternative<std::string>(entry.value)) type = "string";
        else if (std::holds_alternative<bool>(entry.value)) type = "bool";
        else if (std::holds_alternative<std::vector<int64_t>>(entry.value)) type = "int_array";
        else if (std::holds_alternative<std::vector<double>>(entry.value)) type = "double_array";
        else if (std::holds_alternative<std::vector<std::string>>(entry.value)) type = "string_array";
        
        typeCounts[type]++;
    }
    
    oss << "  By type:\n";
    for (const auto& pair : typeCounts) {
        oss << "    " << pair.first << ": " << pair.second << "\n";
    }
    
    return oss.str();
}

bool Config::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (!pImpl->validationEnabled) {
        return errors; // Skip validation if disabled
    }
    
    for (const auto& rule : pImpl->validationRules) {
        // Check if key exists
        if (!has(rule.key)) {
            if (rule.required) {
                errors.push_back("Missing required key: " + rule.key);
            }
            continue;
        }
        
        // Get value
        std::optional<ConfigValue> value;
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&rule](const ConfigEntry& e) { return e.key == rule.key; });n
        if (it != pImpl->entries.end()) {
            value = it->value;
        }
        
        if (!value.has_value()) {
            continue;
        }
        
        // Check type
        bool typeMatches = false;
        if (rule.type == "int" && std::holds_alternative<int>(*value)) typeMatches = true;
        else if (rule.type == "int64" && std::holds_alternative<int64_t>(*value)) typeMatches = true;
        else if (rule.type == "double" && std::holds_alternative<double>(*value)) typeMatches = true;
        else if (rule.type == "string" && std::holds_alternative<std::string>(*value)) typeMatches = true;
        else if (rule.type == "bool" && std::holds_alternative<bool>(*value)) typeMatches = true;
        else if (rule.type == "int_array" && std::holds_alternative<std::vector<int64_t>>(*value)) typeMatches = true;
        else if (rule.type == "double_array" && std::holds_alternative<std::vector<double>>(*value)) typeMatches = true;
        else if (rule.type == "string_array" && std::holds_alternative<std::vector<std::string>>(*value)) typeMatches = true;
        
        if (!typeMatches) {
            errors.push_back("Type mismatch for key '" + rule.key + "': expected " + rule.type + ", got " + configValueToString(*value));
            continue;
        }
        
        // Check range
        auto getNumberValue = [](const ConfigValue& val) -> std::optional<double> {
            if (std::holds_alternative<int>(val)) {
                return static_cast<double>(std::get<int>(val));
            } else if (std::holds_alternative<int64_t>(val)) {
                return static_cast<double>(std::get<int64_t>(val));
            } else if (std::holds_alternative<double>(val)) {
                return std::get<double>(val);
            }
            return std::nullopt;
        };
        
        auto numVal = getNumberValue(*value);
        if (numVal.has_value()) {
            if (rule.minValue.has_value() && numVal.value() < rule.minValue.value()) {
                errors.push_back("Value for key '" + rule.key + "' is below minimum: " + 
                               std::to_string(numVal.value()) + " < " + std::to_string(rule.minValue.value()));
            }
            if (rule.maxValue.has_value() && numVal.value() > rule.maxValue.value()) {
                errors.push_back("Value for key '" + rule.key + "' is above maximum: " + 
                               std::to_string(numVal.value()) + " > " + std::to_string(rule.maxValue.value()));
            }
        }
    }
    
    return errors;
}

void Config::addValidationRule(const ValidationRule& rule) {
    // Check if rule already exists
    auto it = std::find_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
        [&rule](const ValidationRule& r) { return r.key == rule.key; });n
    if (it != pImpl->validationRules.end()) {
        *it = rule; // Update existing rule
    } else {
        pImpl->validationRules.push_back(rule);
    }
}

void Config::removeValidationRule(const std::string& key) {
    pImpl->validationRules.erase(
        std::remove_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
            [&key](const ValidationRule& r) { return r.key == key; }),
        pImpl->validationRules.end()
    );
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& otherEntry : other.pImpl->entries) {
        bool exists = false;
        for (auto& entry : pImpl->entries) {
            if (entry.key == otherEntry.key) {
                entry.value = otherEntry.value;
                entry.source = source; // Override with merge source
                exists = true;
                break;
            }
        }
        if (!exists) {
            pImpl->entries.push_back(otherEntry);
        }
    }
    
    // Merge validation rules
    for (const auto& otherRule : other.pImpl->validationRules) {
        bool exists = false;
        for (auto& rule : pImpl->validationRules) {
            if (rule.key == otherRule.key) {
                rule = otherRule; // Override with merge rule
                exists = true;
                break;
            }
        }
        if (!exists) {
            pImpl->validationRules.push_back(otherRule);
        }
    }
}

std::string Config::createSnapshot() const {
    std::ostringstream oss;
    oss << "NLM_CONFIG_SNAPSHOT\n";
    oss << "Version: 1.0\n";
    oss << "Entries: " << pImpl->entries.size() << "\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << entry.key << "=" << configValueToString(entry.value) << ";";
    }
    
    oss << "\nValidationEnabled: " << (pImpl->validationEnabled ? "true" : "false");
    
    time_t now = std::time(nullptr);
    oss << "\nCreated: " << std::ctime(&now);
    
    return oss.str();
}

bool Config::restoreFromSnapshot(const std::string& snapshot) {
    std::istringstream iss(snapshot);
    std::string line;
    
    if (!std::getline(iss, line) || line != "NLM_CONFIG_SNAPSHOT") {
        return false;
    }
    
    clear();
    
    while (std::getline(iss, line)) {
        if (line.empty() || line.find("=") == std::string::npos) continue;
        
        size_t pos = line.find("=");
        std::string key = line.substr(0, pos);
        std::string valueStr = line.substr(pos + 1);
        
        // Remove trailing semicolon
        if (!valueStr.empty() && valueStr.back() == ';') {
            valueStr.pop_back();
        }
        
        // Try to parse value
        try {
            // Try int
            size_t endpos;
            long long intValue = std::stoll(valueStr);
            set(key, intValue, ConfigSource::Runtime);
        } catch (...) {
            try {
                // Try double
                double doubleValue = std::stod(valueStr);
                set(key, doubleValue, ConfigSource::Runtime);
            } catch (...) {
                try {
                    // Try bool
                    if (valueStr == "true") {
                        set(key, true, ConfigSource::Runtime);
                    } else if (valueStr == "false") {
                        set(key, false, ConfigSource::Runtime);
                    } else {
                        // Try string
                        set(key, valueStr, ConfigSource::Runtime);
                    }
                } catch (...) {
                    // Leave as string if all else fails
                    set(key, valueStr, ConfigSource::Runtime);
                }
            }
        }
    }
    
    // Restore validation enabled
    std::getline(iss, line);
    if (line.find("ValidationEnabled: true") != std::string::npos) {
        pImpl->validationEnabled = true;
    } else {
        pImpl->validationEnabled = false;
    }
    
    return true;
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto val = get<std::string>(key);
    return val.has_value() ? val.value() : defaultValue;
}

int Config::getInt(const std::string& key, int defaultValue) const {
    auto val = get<int>(key);
    return val.has_value() ? val.value() : defaultValue;
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    auto val = get<double>(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    auto val = get<bool>(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::vector<std::string> Config::getStringArray(const std::string& key) const {
    auto val = get<std::vector<std::string>>(key);
    return val.has_value() ? val.value() : std::vector<std::string>();
}

std::vector<int> Config::getIntArray(const std::string& key) const {
    auto val = get<std::vector<int64_t>>(key);
    if (val.has_value()) {
        std::vector<int> result;
        for (int64_t v : val.value()) {
            result.push_back(static_cast<int>(v));
        }
        return result;
    }
    return std::vector<int>();
}

std::vector<double> Config::getDoubleArray(const std::string& key) const {
    auto val = get<std::vector<double>>(key);
    return val.has_value() ? val.value() : std::vector<double>();
}

Config Config::createDefaultConfig(int neuronCount, int regionCount,
                                   double timestep) {
    Config config;
    
    config.set("neuron_count", static_cast<int64_t>(neuronCount));
    config.set("region_count", static_cast<int64_t>(regionCount));
    config.set("simulation_timestep", timestep);
    config.set("random_seed", static_cast<int64_t>(42));
    config.set("connection_probability", 0.1);
    
    // Add validation rules
    config.addValidationRule(ValidationRule("neuron_count", "int", "Number of neurons", 1, 100000, true));
    config.addValidationRule(ValidationRule("region_count", "int", "Number of brain regions", 1, 100, true));
    config.addValidationRule(ValidationRule("simulation_timestep", "double", "Simulation timestep", 0.0001, 1.0, true));
    
    return config;
}

Config Config::createMinimalConfig() {
    Config config;
    config.set("neuron_count", static_cast<int64_t>(100));
    config.set("region_count", static_cast<int64_t>(1));
    config.set("simulation_timestep", 0.001);
    return config;
}

Config Config::createResearchConfig() {
    Config config = createDefaultConfig(10000, 4, 0.0001);
    config.set("stdp_ltp_weight", 0.01);
    config.set("stdp_ltd_weight", 0.012);
    config.set("stdp_tau", 20.0);
    config.set("synaptogenesis_rate", 0.0001);
    config.set("pruning_rate", 0.00001);
    config.set("development_enabled", true);
    config.set("reward_modulation_enabled", true);
    return config;
}

Config Config::createHighPerformanceConfig() {
    Config config = createDefaultConfig(50000, 8, 0.0005);
    config.set("stdp_ltp_weight", 0.005);
    config.set("stdp_ltd_weight", 0.006);
    config.set("stdp_tau", 15.0);
    config.set("synaptogenesis_rate", 0.00001);
    config.set("pruning_rate", 0.000001);
    config.set("development_enabled", false);
    config.set("reward_modulation_enabled", false);
    return config;
}

std::map<std::string, std::string> Config::toPythonDict() const {
    std::map<std::string, std::string> result;
    
    for (const auto& entry : pImpl->entries) {
        std::string valueStr;
        
        if (std::holds_alternative<int>(entry.value)) {
            valueStr = std::to_string(std::get<int>(entry.value));
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            valueStr = std::to_string(std::get<int64_t>(entry.value));
        } else if (std::holds_alternative<double>(entry.value)) {
            valueStr = std::to_string(std::get<double>(entry.value));
        } else if (std::holds_alternative<std::string>(entry.value)) {
            valueStr = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            valueStr = std::get<bool>(entry.value) ? "true" : "false";
        } else if (std::holds_alternative<std::vector<int64_t>>(entry.value)) {
            const auto& arr = std::get<std::vector<int64_t>>(entry.value);
            valueStr = "[";
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) valueStr += ", ";
                valueStr += std::to_string(arr[i]);
            }
            valueStr += "]";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            const auto& arr = std::get<std::vector<double>>(entry.value);
            valueStr = "[";
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) valueStr += ", ";
                valueStr += std::to_string(arr[i]);
            }
            valueStr += "]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            const auto& arr = std::get<std::vector<std::string>>(entry.value);
            valueStr = "[";
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) valueStr += ", ";
                valueStr += "\"" + arr[i] + "\"";
            }
            valueStr += "]";
        }
        
        result[entry.key] = valueStr;
    }
    
    return result;
}

Config Config::fromPythonDict(const std::map<std::string, std::string>& configDict) {
    Config config;
    
    for (const auto& pair : configDict) {
        const std::string& key = pair.first;
        const std::string& valueStr = pair.second;
        
        // Try to parse value string
        try {
            // Try int
            size_t endpos;
            long long intValue = std::stoll(valueStr);
            config.set(key, intValue, ConfigSource::Runtime);
        } catch (...) {
            try {
                // Try double
                double doubleValue = std::stod(valueStr);
                config.set(key, doubleValue, ConfigSource::Runtime);
            } catch (...) {
                try {
                    // Try bool
                    if (valueStr == "true") {
                        config.set(key, true, ConfigSource::Runtime);
                    } else if (valueStr == "false") {
                        config.set(key, false, ConfigSource::Runtime);
                    } else {
                        // Try array
                        if (valueStr.front() == '[' && valueStr.back() == ']') {
                            std::string inner = valueStr.substr(1, valueStr.size() - 2);
                            std::vector<std::string> strings;
                            size_t pos = 0;
                            size_t commaPos;
                            while ((commaPos = inner.find(",", pos)) != std::string::npos) {
                                std::string item = inner.substr(pos, commaPos - pos);
                                // Remove quotes if present
                                if (item.size() >= 2 && item.front() == '\"' && item.back() == '\"') {
                                    item = item.substr(1, item.size() - 2);
                                }
                                strings.push_back(item);
                                pos = commaPos + 1;
                            }
                            if (pos < inner.size()) {
                                std::string item = inner.substr(pos);
                                if (item.size() >= 2 && item.front() == '\"' && item.back() == '\"') {
                                    item = item.substr(1, item.size() - 2);
                                }
                                strings.push_back(item);
                            }
                            config.set(key, strings, ConfigSource::Runtime);
                        } else {
                            // String
                            config.set(key, valueStr, ConfigSource::Runtime);
                        }
                    }
                } catch (...) {
                    // String fallback
                    config.set(key, valueStr, ConfigSource::Runtime);
                }
            }
        }
    }
    
    return config;
}

template<typename T>
std::string Config::configValueToString(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::vector<int64_t>>(value)) {
        const auto& arr = std::get<std::vector<int64_t>>(value);
        std::string result = "[";
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) result += ", ";
            result += std::to_string(arr[i]);
        }
        result += "]";
        return result;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& arr = std::get<std::vector<double>>(value);
        std::string result = "[";
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) result += ", ";
            result += std::to_string(arr[i]);
        }
        result += "]";
        return result;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& arr = std::get<std::vector<std::string>>(value);
        std::string result = "[\"";
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) result += "\", \";
            result += arr[i];
        }
        result += "\"]";
        return result;
    }
    
    return "unknown";
}

// Explicit template instantiations
namespace {
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
    
    template std::optional<int> Config::getWithValidation<int>(const std::string&, const ValidationRule&) const;
    template int Config::getOrWithValidation<int>(const std::string&, const int&, const ValidationRule&) const;
}

} // namespace nlm
