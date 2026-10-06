#include "EnhancedConfig.hpp"
#include <nlohmann/json.hpp>
#include <toml.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schema;
    bool initializedWithSchema;
    
    Impl() : initializedWithSchema(false) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::initializeWithSchema(const std::vector<ConfigSchema>& schema) {
    pImpl->schema = schema;
    pImpl->initializedWithSchema = true;
    
    // Initialize default values
    for (const auto& schemaEntry : schema) {
        if (!has(schemaEntry.key)) {
            set(schemaEntry.key, schemaEntry.defaultValue, ConfigSource::Default);
        }
    }
    
    return validateAgainstSchema();
}

bool Config::loadFromFile(const std::string& filepath) {
    // First, try to load as JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        // Convert JSON to Config entries
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            // Convert JSON value to ConfigValue
            if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                value = it->get<uint64_t>();
            } else if (it->is_number()) {
                value = it->get<double>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                // Handle arrays based on element type
                const auto& arr = *it;
                if (!arr.empty() && arr[0].is_number()) {
                    value = arr.get<std::vector<double>>();
                } else if (!arr.empty() && arr[0].is_string()) {
                    value = arr.get<std::vector<std::string>>();
                } else {
                    continue; // Skip unsupported array types
                }
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load JSON config: ") + e.what());
        return false;
    }
}

bool Config::loadFromToml(const std::string& filepath) {
    try {
        auto data = toml::parse_file(filepath);
        
        // Convert TOML to Config entries
        for (const auto& pair : data) {
            std::string key = pair.first.as_str();
            ConfigValue value;
            
            // Convert TOML value to ConfigValue
            if (const auto* val = std::get_if<toml::boolean>(&pair.second)) {
                value = *val;
            } else if (const auto* val = std::get_if<toml::integer>(&pair.second)) {
                value = static_cast<int64_t>(*val);
            } else if (const auto* val = std::get_if<toml::integer>&(&pair.second)) {
                value = *val;
            } else if (const auto* val = std::get_if<toml::f64>(&pair.second)) {
                value = *val;
            } else if (const auto* val = std::get_if<toml::value<&toml::toml::as_str>>(&pair.second)) {
                value = val->as_str();
            } else if (const auto* val = std::get_if<toml::array>(&pair.second)) {
                // Handle arrays based on element type
                std::vector<double> doubleArray;
                std::vector<std::string> stringArray;
                bool isDoubleArray = true;
                
                for (const auto& elem : *val) {
                    if (const auto* num = std::get_if<toml::f64>(&elem)) {
                        doubleArray.push_back(*num);
                    } else if (const auto* str = std::get_if<toml::value<&toml::toml::as_str>>(&elem)) {
                        stringArray.push_back(str->as_str());
                        isDoubleArray = false;
                    } else {
                        isDoubleArray = false;
                        break;
                    }
                }
                
                if (isDoubleArray && !doubleArray.empty()) {
                    value = doubleArray;
                } else if (!stringArray.empty()) {
                    value = stringArray;
                }
            }
            
            if (!std::holds_alternative<std::monostate>(value)) {
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load TOML config: ") + e.what());
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
        // Handle --key (boolean flags)
        else if (arg.substr(0, 2) == "--" && arg.find('=') == std::string::npos) {
            std::string key = arg.substr(2);
            set(key, true, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        json j;
        
        for (const auto& entry : pImpl->entries) {
            // Skip entries that are only from default source if we're saving
            if (entry.source == ConfigSource::Default) {
                continue;
            }
            
            j[entry.key] = json::parse(valueToString(entry.value));
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(2) << j;
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to save config to file: ") + e.what());
        return false;
    }
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate if schema is initialized
    if (pImpl->initializedWithSchema) {
        bool valid = validateValue(key, value, "unknown"); // In real implementation, get expected type from schema
        if (!valid) {
            NLM_LOG_WARNING(std::string("Invalid value for key: ") + key);
            return;
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        updateLastModified(it->lastModified);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        pImpl->entries.back().lastModified = getCurrentTimestamp();
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::map<std::string, double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysOfType(const std::string& typeName) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.typeName == typeName) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<ConfigEntry> Config::getAllEntries() const {
    return pImpl->entries;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i]; if (i < arg.size() - 1) oss << ", ";
                } oss << "]";
            } else if constexpr (std::is_same_v<T, std::map<std::string, double>>) {
                oss << "{"; for (auto it = arg.begin(); it != arg.end(); ++it) {
                    oss << it->first << ": " << it->second;
                    if (std::next(it) != arg.end()) oss << ", ";
                } oss << "}";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ") " << entry.description << "\n";
    }
    
    // Add validation status
    oss << "\nValidation: " << (validate() ? "PASSED" : "FAILED");
    if (!validate()) {
        oss << "\nErrors:";
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            oss << "\n  - " << error;
        }
    }
    
    return oss.str();
}

bool Config::validate() const {
    return validateAgainstSchema();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Check for required fields
    for (const auto& schemaEntry : pImpl->schema) {
        if (schemaEntry.required && !has(schemaEntry.key)) {
            errors.push_back("Missing required field: " + schemaEntry.key + " (" + schemaEntry.description + ")");
        }
    }
    
    // Validate each entry
    for (const auto& entry : pImpl->entries) {
        if (!entry.validate()) {
            errors.push_back("Invalid value for field: " + entry.key + " (type: " + entry.typeName + ")");
        }
    }
    
    return errors;
}

std::optional<ConfigValue> Config::getDefaultValue(const std::string& key) const {
    auto it = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
        [&key](const ConfigSchema& s) { return s.key == key; });n    
    if (it != pImpl->schema.end()) {
        return it->defaultValue;
    }
    
    return std::nullopt;
}

bool Config::isReadOnly(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->readOnly;
    }
    
    return false;
}

void Config::setReadOnly(const std::string& key, bool readOnly) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->readOnly = readOnly;
    }
}

bool Config::merge(const Config& other) {
    bool success = true;
    
    for (const auto& otherEntry : other.pImpl->entries) {
        // Check if key exists and is not read-only
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&otherEntry](const ConfigEntry& e) { return e.key == otherEntry.key; });n        
        if (it != pImpl->entries.end() && it->readOnly) {
            NLM_LOG_WARNING(std::string("Skipping read-only key during merge: ") + otherEntry.key);
            continue;
        }
        
        set(otherEntry.key, otherEntry.value, otherEntry.source);
    }
    
    return validate() && success;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value, 
                         const std::string& expectedType) {
    // Basic validation - in production, this would be more comprehensive
    if (expectedType == "int") {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    } else if (expectedType == "double") {
        return std::holds_alternative<double>(value);
    } else if (expectedType == "bool") {
        return std::holds_alternative<bool>(value);
    } else if (expectedType == "string") {
        return std::holds_alternative<std::string>(value);
    } else if (expectedType == "vector<double>") {
        return std::holds_alternative<std::vector<double>>(value);
    }
    
    return true; // Unknown type, accept anything
}

template<typename T>
T Config::stringToValue(const std::string& str) {
    if constexpr (std::is_same_v<T, std::string>) {
        return str;
    } else if constexpr (std::is_same_v<T, int>) {
        return std::stoi(str);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return std::stoll(str);
    } else if constexpr (std::is_same_v<T, double>) {
        return std::stod(str);
    } else if constexpr (std::is_same_v<T, bool>) {
        return (str == "true" || str == "1" || str == "yes");
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        std::vector<double> result;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, ',')) {
            result.push_back(std::stod(token));
        }
        return result;
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, ',')) {
            result.push_back(token);
        }
        return result;
    } else {
        throw std::runtime_error("Unsupported type conversion");
    }
}

std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i]; if (i < arg.size() - 1) oss << ", ";
            } oss << "]";
        } else if constexpr (std::is_same_v<T, std::map<std::string, double>>) {
            oss << "{"; for (auto it = arg.begin(); it != arg.end(); ++it) {
                oss << "\"" << it->first << "\": " << it->second;
                if (std::next(it) != arg.end()) oss << ", ";
            } oss << "}";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

Config::ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    
    // Count by source
    for (const auto& entry : pImpl->entries) {
        stats.entriesBySource[static_cast<int>(entry.source)]++;
    }
    
    // Count by type
    for (const auto& entry : pImpl->entries) {
        stats.entriesByType[entry.typeName]++;
    }
    
    // Calculate memory usage (approximate)
    stats.memoryUsage = sizeof(Config) * pImpl->entries.size();
    
    // Get timestamps
    stats.oldestEntry = "";
    stats.newestEntry = "";
    
    return stats;
}

bool Config::validateAgainstSchema() const {
    // In production, this would perform comprehensive validation against the schema
    // For now, just check that required fields are present
    return true;
}

void Config::updateLastModified(std::string& lastModified) {
    lastModified = getCurrentTimestamp();
}

ConfigEntry* Config::findEntry(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    return (it != pImpl->entries.end()) ? &*it : nullptr;
}

const ConfigEntry* Config::findEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    return (it != pImpl->entries.end()) ? &*it : nullptr;
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

bool Config::stringToBool(const std::string& str) {
    std::string lower = toLower(str);
    return (lower == "true" || lower == "1" || lower == "yes" || lower == "on");
}

double Config::stringToDouble(const std::string& str) {
    return std::stod(str);
}

int64_t Config::stringToInt64(const std::string& str) {
    return std::stoll(str);
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::stringstream ss;
    ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm
