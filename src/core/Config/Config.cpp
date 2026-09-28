#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cmath>
#include <unordered_map>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ValidationError> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema_(nullptr) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::setSchema(const std::shared_ptr<ConfigSchema>& schema) {
    schema_ = schema;
}

std::shared_ptr<ConfigSchema> Config::getSchema() const {
    return schema_;
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    if (!schema_) {
        // If no schema, check for basic types
        for (const auto& entry : pImpl->entries) {
            if (!validateType(entry.value, typeid(std::string))) {
                pImpl->validationErrors.emplace_back(
                    "Invalid type for key: " + entry.key, entry.key);
            }
        }
        return pImpl->validationErrors.empty();
    }
    
    // Validate against schema
    for (const auto& entry : pImpl->entries) {
        if (schema_->hasKey(entry.key)) {
            const auto& rules = schema_->getRules(entry.key);
            
            // Check required rule
            bool isRequired = std::any_of(rules.begin(), rules.end(),
                [](const ValidationRule& rule) { return rule.required; });
            
            if (isRequired && !has(entry.key)) {
                pImpl->validationErrors.emplace_back(
                    "Required key is missing: " + entry.key, entry.key);
                continue;
            }
            
            // Apply all validation rules
            for (const auto& rule : rules) {
                if (!rule.validator(entry.value)) {
                    pImpl->validationErrors.emplace_back(rule.errorMessage, entry.key);
                    break; // Stop at first validation error for this key
                }
            }
        }
    }
    
    return pImpl->validationErrors.empty();
}

std::vector<ValidationError> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check file extension to determine format
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    try {
        if (extension == "json" || extension == "JSON") {
            return loadFromJSON(filepath);
        } else if (extension == "yaml" || extension == "yml" || extension == "YAML" || extension == "YML") {
            return loadFromYAML(filepath);
        } else {
            // Fall back to simple key=value format
            return loadFromLegacyFile(filepath);
        }
    } catch (const std::exception& e) {
        // Log error and return false
        return false;
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        // Clear existing config
        clear();
        
        // Parse JSON object
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                
                // Convert JSON value to ConfigValue
                ConfigValue value;
                if (it.value().is_boolean()) {
                    value = it.value().get<bool>();
                } else if (it.value().is_number_integer()) {
                    value = it.value().get<int64_t>();
                } else if (it.value().is_number_unsigned()) {
                    value = it.value().get<uint64_t>();
                } else if (it.value().is_number()) {
                    value = it.value().get<double>();
                } else if (it.value().is_string()) {
                    value = it.value().get<std::string>();
                } else if (it.value().is_array()) {
                    // Convert JSON array to vector
                    const auto& arr = it.value();
                    if (!arr.empty()) {
                        if (arr[0].is_number_integer()) {
                            std::vector<int> vec(arr.size());
                            for (size_t i = 0; i < arr.size(); ++i) {
                                vec[i] = arr[i].get<int>();
                            }
                            value = vec;
                        } else if (arr[0].is_number()) {
                            std::vector<double> vec(arr.size());
                            for (size_t i = 0; i < arr.size(); ++i) {
                                vec[i] = arr[i].get<double>();
                            }
                            value = vec;
                        } else if (arr[0].is_string()) {
                            std::vector<std::string> vec(arr.size());
                            for (size_t i = 0; i < arr.size(); ++i) {
                                vec[i] = arr[i].get<std::string>();
                            }
                            value = vec;
                        } else {
                            // Fallback to string representation
                            value = arr.dump();
                        }
                    }
                } else {
                    // Unknown type, convert to string
                    value = it.value().dump();
                }
                
                // Set the value with description
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // For now, YAML parsing uses the same implementation as JSON
    // TODO: Implement proper YAML parsing when YAML library is available
    return loadFromJSON(filepath);
}

bool Config::loadFromLegacyFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Parse simple key=value pairs (legacy format)
    clear();
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
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
        }
    }
    
    return true;
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
    
    // Get current time for generation timestamp
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S") << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << '"' << arg << '"';
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << arg;
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
                    file << '"' << arg[i] << '"';
                }
                file << "]";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
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
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate before setting if schema exists
    if (schema_ && schema_->hasKey(key)) {
        const auto& rules = schema_->getRules(key);
        for (const auto& rule : rules) {
            if (!rule.validator(value)) {
                throw ValidationError(rule.errorMessage, key);
            }
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
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
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
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

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validationErrors.clear();
}

std::string Config::summary() const {
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
    if (!pImpl->validationErrors.empty()) {
        oss << "\nValidation Errors:\n";
        for (const auto& error : pImpl->validationErrors) {
            oss << "  " << error.getKey() << ": " << error.what() << "\n";
        }
    }
    return oss.str();
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

bool Config::convertStringToInt(const std::string& str, ConfigValue& out) {
    try {
        size_t pos;
        int64_t value = std::stoll(str, &pos);
        if (pos == str.size()) {
            out = static_cast<int>(value); // Convert to int
            return true;
        }
    } catch (const std::exception&) {
        // Fall through to other conversions
    }
    return false;
}

bool Config::convertStringToDouble(const std::string& str, ConfigValue& out) {
    try {
        size_t pos;
        double value = std::stod(str, &pos);
        if (pos == str.size()) {
            out = value;
            return true;
        }
    } catch (const std::exception&) {
        // Fall through to other conversions
    }
    return false;
}

bool Config::convertStringToBool(const std::string& str, ConfigValue& out) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        out = true;
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        out = false;
        return true;
    }
    return false;
}

bool Config::convertStringToString(const std::string& str, ConfigValue& out) {
    out = str;
    return true;
}

bool Config::convertStringToVector(const std::string& str, ConfigValue& out) {
    // Simple vector parsing for now
    // This can be enhanced later to handle different vector types
    if (str.empty() || str[0] != '[' || str.back() != ']') {
        return false;
    }
    
    std::string content = str.substr(1, str.size() - 2);
    if (content.empty()) {
        out = std::vector<int>{}; // Empty int vector
        return true;
    }
    
    std::stringstream ss(content);
    std::string item;
    std::vector<int> vec;
    
    while (std::getline(ss, item, ',')) {
        item = trim(item);
        try {
            int value = std::stoi(item);
            vec.push_back(value);
        } catch (const std::exception&) {
            return false;
        }
    }
    
    out = vec;
    return true;
}

std::string Config::getTypeName(const ConfigValue& value) {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return std::string("int");
        else if constexpr (std::is_same_v<T, int64_t>) return std::string("int64_t");
        else if constexpr (std::is_same_v<T, double>) return std::string("double");
        else if constexpr (std::is_same_v<T, bool>) return std::string("bool");
        else if constexpr (std::is_same_v<T, std::string>) return std::string("string");
        else if constexpr (std::is_same_v<T, std::vector<int>>) return std::string("vector<int>");
        else if constexpr (std::is_same_v<T, std::vector<double>>) return std::string("vector<double>");
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return std::string("vector<string>");
    }, value);
    return std::string("unknown");
}

std::string Config::getValueDescription(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << '"' << arg << '"';
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || 
                             std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::fixed << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << '"' << arg[i] << '"';
            }
            oss << "]";
        }
    }, value);
    return oss.str();
}

bool Config::parseNumber(const std::string& str, double& out) {
    try {
        size_t pos;
        out = std::stod(str, &pos);
        return pos == str.size();
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::parseBool(const std::string& str, bool& out) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        out = true;
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        out = false;
        return true;
    }
    return false;
}

// ConfigSchema methods
void ConfigSchema::addRule(const std::string& key, ValidationFunc validator, 
                          const std::string& errorMessage, bool required) {
    rules_[key].push_back({validator, errorMessage, required});
}

void ConfigSchema::addTypeValidation(const std::string& key, const std::type_info& type) {
    typeInfo_[key] = &type;
}

void ConfigSchema::addRangeValidation(const std::string& key, double min, double max, bool inclusive) {
    if (!hasKey(key)) {
        throw ValidationError("Cannot add range validation for unknown key: " + key, key);
    }
    
    auto validator = [min, max, inclusive](const ConfigValue& value) {
        try {
            double val = 0.0;
            if (std::holds_alternative<int>(value)) {
                val = static_cast<double>(std::get<int>(value));
            } else if (std::holds_alternative<int64_t>(value)) {
                val = static_cast<double>(std::get<int64_t>(value));
            } else if (std::holds_alternative<double>(value)) {
                val = std::get<double>(value);
            } else {
                return false; // Not a numeric type
            }
            
            if (inclusive) {
                return val >= min && val <= max;
            } else {
                return val > min && val < max;
            }
        } catch (...) {
            return false;
        }
    };
    
    std::string errorMsg = "Value must be between " + std::to_string(min) + 
                          " and " + std::to_string(max);
    addRule(key, validator, errorMsg, false);
}

template<typename T>
void ConfigSchema::addAllowedValues(const std::string& key, const std::vector<T>& allowedValues) {
    if (!hasKey(key)) {
        throw ValidationError("Cannot add allowed values for unknown key: " + key, key);
    }
    
    auto validator = [allowedValues](const ConfigValue& value) {
        try {
            T val;
            if constexpr (std::is_same_v<T, int>) {
                if (std::holds_alternative<int>(value)) {
                    val = std::get<int>(value);
                } else {
                    return false;
                }
            } else if constexpr (std::is_same_v<T, double>) {
                if (std::holds_alternative<double>(value)) {
                    val = std::get<double>(value);
                } else {
                    return false;
                }
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (std::holds_alternative<std::string>(value)) {
                    val = std::get<std::string>(value);
                } else {
                    return false;
                }
            } else {
                return false;
            }
            
            return std::find(allowedValues.begin(), allowedValues.end(), val) != allowedValues.end();
        } catch (...) {
            return false;
        }
    };
    
    std::string errorMsg = "Value must be one of: ";
    for (size_t i = 0; i < allowedValues.size(); ++i) {
        if (i > 0) errorMsg += ", ";
        if constexpr (std::is_same_v<T, std::string>) {
            errorMsg += '"' + allowedValues[i] + '"';
        } else {
            errorMsg += std::to_string(allowedValues[i]);
        }
    }
    
    addRule(key, validator, errorMsg, false);
}

bool ConfigSchema::hasKey(const std::string& key) const {
    return rules_.find(key) != rules_.end();
}

const std::vector<ValidationRule>& ConfigSchema::getRules(const std::string& key) const {
    static const std::vector<ValidationRule> emptyRules;
    auto it = rules_.find(key);
    return it != rules_.end() ? it->second : emptyRules;
}

std::vector<std::string> ConfigSchema::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(rules_.size());
    for (const auto& pair : rules_) {
        keys.push_back(pair.first);
    }
    return keys;
}

void ConfigSchema::clear() {
    rules_.clear();
    typeInfo_.clear();
}

// Explicit template instantiations for ConfigSchema
namespace nlm {
    template void ConfigSchema::addAllowedValues<int>(const std::string&, const std::vector<int>&);
    template void ConfigSchema::addAllowedValues<double>(const std::string&, const std::vector<double>&);
    template void ConfigSchema::addAllowedValues<std::string>(const std::string&, const std::vector<std::string>&);
}

// Explicit template instantiations
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

} // namespace nlm