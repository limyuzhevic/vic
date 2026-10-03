#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Validation schema
    std::map<std::string, TypeValidator> typeValidators;
    std::map<std::string, RangeValidator> rangeValidators;
    std::map<std::string, EnumValidator> enumValidators;
    
    // Type inference cache
    std::map<std::string, std::string> inferredTypes;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Properly serialize the config value
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << arg;
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        // Invalidate inferred type
        pImpl->inferredTypes.erase(key);
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

void Config::set(const std::string& key, const ConfigValue& value, const std::string& type,
                 const std::string& validationRules, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->type = type;
        it->validationRules = validationRules;
        pImpl->inferredTypes.erase(key);
    } else {
        pImpl->entries.emplace_back(key, value, source, "", type, validationRules);
    }
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult entryResult = validate(entry.key);
        if (entryResult.hasErrors()) {
            result.isValid = false;
            for (const auto& error : entryResult.errors) {
                result.addError("[" + entry.key + "] " + error);
            }
        }
        if (entryResult.hasWarnings()) {
            for (const auto& warning : entryResult.warnings) {
                result.addWarning("[" + entry.key + "] " + warning);
            }
        }
    }
    
    return result;
}

ValidationResult Config::validate(const std::string& key) const {
    ValidationResult result;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        result.addError("Key does not exist");
        return result;
    }
    
    const auto& entry = *it;
    
    // Type validation
    std::string type = entry.type.empty() ? inferType(entry.value) : entry.type;
    
    if (!typeValidators.empty() && typeValidators.find(type) != typeValidators.end()) {
        if (!typeValidators.at(type)(entry.value)) {
            result.addError("Type validation failed for key: " + key);
        }
    }
    
    // Range validation
    double doubleValue = 0.0;
    bool isNumeric = false;
    
    if constexpr (std::is_same_v<int, std::decay_t<decltype(std::get<int>(entry.value))>>) {
        doubleValue = static_cast<double>(std::get<int>(entry.value));
        isNumeric = true;
    } else if constexpr (std::is_same_v<double, std::decay_t<decltype(std::get<double>(entry.value))>>) {
        doubleValue = std::get<double>(entry.value);
        isNumeric = true;
    }
    
    if (isNumeric) {
        for (const auto& pair : rangeValidators) {
            if (pair.second(doubleValue)) {
                if (!pair.second(doubleValue)) {
                    result.addError("Range validation failed for key: " + key);
                }
            }
        }
    }
    
    // Enum validation
    if constexpr (std::is_same_v<std::string, std::decay_t<decltype(std::get<std::string>(entry.value))>>) {
        if (!enumValidators.empty() && enumValidators.find(type) != enumValidators.end()) {
            if (!enumValidators.at(type)(std::get<std::string>(entry.value))) {
                result.addError("Enum validation failed for key: " + key);
            }
        }
    }
    
    return result;
}

bool Config::validateType(const std::string& key, const std::string& expectedType) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    std::string actualType = it->type.empty() ? inferType(it->value) : it->type;
    return actualType == expectedType;
}

bool Config::validateRange(const std::string& key, double min, double max) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    double value = 0.0;
    bool isNumeric = false;
    
    if constexpr (std::is_same_v<int, std::decay_t<decltype(std::get<int>(it->value))>>) {
        value = static_cast<double>(std::get<int>(it->value));
        isNumeric = true;
    } else if constexpr (std::is_same_v<double, std::decay_t<decltype(std::get<double>(it->value))>>) {
        value = std::get<double>(it->value);
        isNumeric = true;
    }
    
    return isNumeric && value >= min && value <= max;
}

bool Config::validateEnum(const std::string& key, const std::vector<std::string>& allowedValues) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    if constexpr (std::is_same_v<std::string, std::decay_t<decltype(std::get<std::string>(it->value))>>) {
        const std::string& value = std::get<std::string>(it->value);
        return std::find(allowedValues.begin(), allowedValues.end(), value) != allowedValues.end();
    }
    
    return false;
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
    pImpl->inferredTypes.erase(key);
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
    pImpl->inferredTypes.clear();
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
    return oss.str();
}

std::string Config::getValidationErrors() const {
    ValidationResult validation = validate();
    std::ostringstream oss;
    for (const auto& error : validation.errors) {
        oss << error << "\n";
    }
    return oss.str();
}

std::string Config::getValidationWarnings() const {
    ValidationResult validation = validate();
    std::ostringstream oss;
    for (const auto& warning : validation.warnings) {
        oss << warning << "\n";
    }
    return oss.str();
}

void Config::addTypeValidator(const std::string& typeName, TypeValidator validator) {
    pImpl->typeValidators[typeName] = validator;
}

void Config::addRangeValidator(const std::string& typeName, RangeValidator validator) {
    pImpl->rangeValidators[typeName] = validator;
}

void Config::addEnumValidator(const std::string& typeName, EnumValidator validator) {
    pImpl->enumValidators[typeName] = validator;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, entry.type, entry.validationRules, source);
    }
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntriesBySource(ConfigSource source) const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            count++;
        }
    }
    return count;
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

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool Config::isInteger(const std::string& str, int64_t& value) {
    try {
        size_t pos;
        value = std::stoll(str, &pos);
        return pos == str.size();
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::isDouble(const std::string& str, double& value) {
    try {
        size_t pos;
        value = std::stod(str, &pos);
        return pos == str.size();
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::isBoolean(const std::string& str, bool& value) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "1" || lower == "yes") {
        value = true;
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no") {
        value = false;
        return true;
    }
    return false;
}

std::string Config::serializeValue(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::deserializeValue(const std::string& str, ConfigValue& value) {
    // Try integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            value = intVal;
            return true;
        }
    } catch (const std::exception&) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            value = doubleVal;
            return true;
        }
    } catch (const std::exception&) {}
    
    // Try boolean
    bool boolVal;
    if (isBoolean(str, boolVal)) {
        value = boolVal;
        return true;
    }
    
    // Assume string
    value = str;
    return true;
}

std::string Config::inferType(const ConfigValue& value) {
    if (pImpl->inferredTypes.count("unknown")) {
        return "unknown";
    }
    
    std::string type;
    if constexpr (std::is_same_v<int, std::decay_t<decltype(std::get<int>(value))>>) {
        type = "int";
    } else if constexpr (std::is_same_v<double, std::decay_t<decltype(std::get<double>(value))>>) {
        type = "double";
    } else if constexpr (std::is_same_v<bool, std::decay_t<decltype(std::get<bool>(value))>>) {
        type = "bool";
    } else if constexpr (std::is_same_v<std::string, std::decay_t<decltype(std::get<std::string>(value))>>) {
        type = "string";
    } else if constexpr (std::is_same_v<std::vector<int>, std::decay_t<decltype(std::get<std::vector<int>>(value))>>) {
        type = "int[]";
    } else if constexpr (std::is_same_v<std::vector<double>, std::decay_t<decltype(std::get<std::vector<double>>(value))>>) {
        type = "double[]";
    } else if constexpr (std::is_same_v<std::vector<std::string>, std::decay_t<decltype(std::get<std::vector<std::string>>(value))>>) {
        type = "string[]";
    }
    
    return type;
}

} // namespace nlm