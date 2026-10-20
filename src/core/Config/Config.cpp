#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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

bool Config::validate(const std::string& key, std::string& errorMessage) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        errorMessage = "Key '" + key + "' does not exist in configuration";
        return false;
    }
    
    return validateConstraints(*it, schema.get());
}

std::vector<std::string> Config::validateAll() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        std::string error;
        if (!validateConstraints(entry, schema.get())) {
            error = "Validation failed for key '" + entry.key + "': ";
            // Generate specific error message
            error += generateConstraintError(entry, schema.get());
            errors.push_back(error);
        }
    }
    
    return errors;
}

std::vector<std::string> Config::getKeysInGroup(const std::string& groupName) const {
    std::vector<std::string> keys;
    
    if (schema) {
        auto groups = schema->getGroups();
        for (const auto& group : groups) {
            if (group.name == groupName) {
                for (const auto& key : group.keys) {
                    if (std::find(pImpl->entries.begin(), pImpl->entries.end(),
                        [&key](const ConfigEntry& e) { return e.key == key; }) != pImpl->entries.end()) {
                        keys.push_back(key);
                    }
                }
                break;
            }
        }
    }
    
    return keys;
}

void Config::resetToDefaults() {
    clear();
    if (schema) {
        // Reset to schema defaults
        auto defaults = schema->getDefaults();
        for (const auto& pair : defaults) {
            set(pair.first, pair.second, ConfigSource::Default);
        }
    }
}

bool Config::hasDefault(const std::string& key) const {
    return schema && schema->hasDefault(key);
}

std::string Config::generateConstraintError(const ConfigEntry& entry, const ConfigSchema* schema) const {
    if (!schema) return "No validation schema defined";
    
    std::string error = "";
    
    // Check type validation
    bool typeValid = validateType(entry.value, entry.key);
    if (!typeValid) {
        error += "Type mismatch for key '" + entry.key + "'. ";
    }
    
    // Check constraints
    auto constraints = schema->getConstraints(entry.key);
    for (const auto& constraint : constraints) {
        if (!validateConstraint(constraint, entry.value)) {
            if (!error.empty()) error += " ";
            error += constraint.errorMessage.empty() ? 
                ("Constraint '" + constraint.value + "' failed for key '" + entry.key + "'") : 
                constraint.errorMessage;
        }
    }
    
    // Check range constraints
    auto rangeConstraints = schema->getRangeConstraints(entry.key);
    for (const auto& rangeConstraint : rangeConstraints) {
        if (!validateRangeConstraint(rangeConstraint, entry.value)) {
            if (!error.empty()) error += " ";
            error += rangeConstraint.errorMessage.empty() ? 
                ("Range constraint failed for key '" + entry.key + "': value must be between " + 
                 std::to_string(rangeConstraint.min) + " and " + std::to_string(rangeConstraint.max)) : 
                rangeConstraint.errorMessage;
        }
    }
    
    if (error.empty()) {
        error = "Validation failed for key '" + entry.key + "'";
    }
    
    return error;
}

bool Config::validateType(const ConfigValue& value, const std::string& key) {
    // Simple type checking based on value
    bool isValid = false;
    std::visit([&isValid](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            isValid = true;
        } else if constexpr (std::is_same_v<T, double>) {
            isValid = true;
        } else if constexpr (std::is_same_v<T, bool>) {
            isValid = true;
        } else if constexpr (std::is_same_v<T, std::string>) {
            isValid = true;
        } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                             std::is_same_v<T, std::vector<double>> || 
                             std::is_same_v<T, std::vector<std::string>>) {
            isValid = true;
        }
    }, value);
    
    return isValid;
}

bool Config::validateConstraints(const ConfigEntry& entry, const ConfigSchema* schema) {
    if (!schema) return true; // No schema means no validation
    
    // Validate type
    if (!validateType(entry.value, entry.key)) {
        return false;
    }
    
    // Validate constraints
    auto constraints = schema->getConstraints(entry.key);
    for (const auto& constraint : constraints) {
        if (!validateConstraint(constraint, entry.value)) {
            return false;
        }
    }
    
    // Validate range constraints
    auto rangeConstraints = schema->getRangeConstraints(entry.key);
    for (const auto& rangeConstraint : rangeConstraints) {
        if (!validateRangeConstraint(rangeConstraint, entry.value)) {
            return false;
        }
    }
    
    return true;
}

bool Config::validateConstraint(const ConfigConstraint& constraint, const ConfigValue& value) {
    switch (constraint.op) {
        case ValidationOperator::Equal:
            return compareValues(value, constraint.value, constraint.op);
        case ValidationOperator::NotEqual:
            return !compareValues(value, constraint.value, ValidationOperator::Equal);
        case ValidationOperator::GreaterThan:
            return compareValues(value, constraint.value, constraint.op);
        case ValidationOperator::LessThan:
            return compareValues(value, constraint.value, constraint.op);
        case ValidationOperator::GreaterEqual:
            return compareValues(value, constraint.value, constraint.op);
        case ValidationOperator::LessEqual:
            return compareValues(value, constraint.value, constraint.op);
        case ValidationOperator::Range:
            return validateRangeConstraintImpl(value, constraint.value);
        case ValidationOperator::Callback:
            return constraint.callback ? constraint.callback(value) : false;
        default:
            return true;
    }
}

bool Config::validateRangeConstraint(const ConfigRangeConstraint& constraint, const ConfigValue& value) {
    return validateRangeConstraintImpl(value, constraint.min, constraint.max);
}

bool Config::validateRangeConstraintImpl(const ConfigValue& value, double min, double max) {
    std::optional<double> doubleVal = toDouble(value);
    if (!doubleVal.has_value()) {
        return false;
    }
    
    double val = doubleVal.value();
    if (min > max) std::swap(min, max);
    
    if (min > max) return false;
    
    if (std::isfinite(min)) {
        if (min == static_cast<int>(min)) {
            if (val < min) return false;
        } else {
            if (val < min) return false;
        }
    }
    
    if (std::isfinite(max)) {
        if (max == static_cast<int>(max)) {
            if (val > max) return false;
        } else {
            if (val > max) return false;
        }
    }
    
    return true;
}

std::optional<double> Config::toDouble(const ConfigValue& value) {
    std::optional<double> result;
    std::visit([&result](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            result = static_cast<double>(val);
        } else if constexpr (std::is_same_v<T, double>) {
            result = val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            try {
                size_t pos;
                double v = std::stod(val, pos);
                if (pos == val.size()) {
                    result = v;
                }
            } catch (...) {
                // Failed to convert
            }
        }
    }, value);
    return result;
}

bool Config::compareValues(const ConfigValue& value, const std::string& targetStr, ValidationOperator op) {
    std::optional<double> valDouble = toDouble(value);
    std::optional<double> targetDouble = toDouble(ConfigValue(targetStr));
    
    if (valDouble.has_value() && targetDouble.has_value()) {
        switch (op) {
            case ValidationOperator::Equal:
                return valDouble.value() == targetDouble.value();
            case ValidationOperator::NotEqual:
                return valDouble.value() != targetDouble.value();
            case ValidationOperator::GreaterThan:
                return valDouble.value() > targetDouble.value();
            case ValidationOperator::LessThan:
                return valDouble.value() < targetDouble.value();
            case ValidationOperator::GreaterEqual:
                return valDouble.value() >= targetDouble.value();
            case ValidationOperator::LessEqual:
                return valDouble.value() <= targetDouble.value();
            default:
                return false;
        }
    }
    
    // String comparison for non-numeric values
    if (std::holds_alternative<std::string>(value)) {
        std::string valStr = std::get<std::string>(value);
        switch (op) {
            case ValidationOperator::Equal:
                return valStr == targetStr;
            case ValidationOperator::NotEqual:
                return valStr != targetStr;
            default:
                return false;
        }
    }
    
    return false;
}

bool Config::validateRangeConstraintImpl(const ConfigValue& value, const std::string& targetStr) {
    // Parse range string like "min-max" or "min-max inclusive"
    size_t dashPos = targetStr.find('-');
    if (dashPos == std::string::npos) {
        return false;
    }
    
    std::string minStr = targetStr.substr(0, dashPos);
    std::string maxStr = targetStr.substr(dashPos + 1);
    
    std::optional<double> minVal = toDouble(ConfigValue(minStr));
    std::optional<double> maxVal = toDouble(ConfigValue(maxStr));
    
    if (!minVal.has_value() || !maxVal.has_value()) {
        return false;
    }
    
    return validateRangeConstraintImpl(value, minVal.value(), maxVal.value());
}

std::string Config::toJSONString(const ConfigValue& value) const {
    std::string result;
    std::visit([&result](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            result = "\"" + escapeString(val) + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            result = val ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                             std::is_same_v<T, std::vector<double>> || 
                             std::is_same_v<T, std::vector<std::string>>) {
            result = "[";
            bool first = true;
            std::visit([&result, &first](auto&& vec) {
                for (const auto& item : vec) {
                    if (!first) result += ", ";
                    first = false;
                    // Recursive call would be needed for nested arrays
                    // For now, assume simple elements
                    using ItemType = std::decay_t<decltype(item)>;
                    if constexpr (std::is_same_v<ItemType, std::string>) {
                        result += "\"" + escapeString(item) + "\"";
                    } else {
                        result += std::to_string(item);
                    }
                }
            }, val);
            result += "]";
        } else {
            result = std::to_string(val);
        }
    }, value);
    return result;
}

std::string Config::toYAMLString(const ConfigValue& value) const {
    std::string result;
    std::visit([&result](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            // YAML string
            if (val.find('\n') != std::string::npos || val.find(':') != std::string::npos) {
                result = "\"" + escapeString(val) + "\"";
            } else {
                result = val;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            result = val ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                             std::is_same_v<T, std::vector<double>> || 
                             std::is_same_v<T, std::vector<std::string>>) {
            result = "[";
            bool first = true;
            std::visit([&result, &first](auto&& vec) {
                for (const auto& item : vec) {
                    if (!first) result += ", ";
                    first = false;
                    using ItemType = std::decay_t<decltype(item)>;
                    if constexpr (std::is_same_v<ItemType, std::string>) {
                        result += "\"" + escapeString(item) + "\"";
                    } else {
                        result += std::to_string(item);
                    }
                }
            }, val);
            result += "]";
        } else {
            result = std::to_string(val);
        }
    }, value);
    return result;
}

std::string Config::escapeString(const std::string& str) const {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c == 127) {
                    // Escape control characters
                    result += "\\x" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

std::string Config::unescapeString(const std::string& str) const {
    std::string result;
    size_t i = 0;
    while (i < str.size()) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '\\': result += '\\'; i += 2; break;
                case '\"': result += '\"'; i += 2; break;
                case 'n': result += '\n'; i += 2; break;
                case 'r': result += '\r'; i += 2; break;
                case 't': result += '\t'; i += 2; break;
                case 'x':
                    if (i + 3 < str.size() && isxdigit(str[i + 2]) && isxdigit(str[i + 3])) {
                        char hex[3] = {str[i + 2], str[i + 3], '\0'};
                        result += static_cast<char>(std::strtol(hex, nullptr, 16));
                        i += 4;
                    } else {
                        result += str[i]; i++;
                    }
                    break;
                default: result += str[i]; i++;
            }
        } else {
            result += str[i];
            i++;
        }
    }
    return result;
}

std::string Config::typeToString(const ConfigValue& value) const {
    std::string typeStr;
    std::visit([&typeStr](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) typeStr = "int";
        else if constexpr (std::is_same_v<T, int64_t>) typeStr = "int64_t";
        else if constexpr (std::is_same_v<T, double>) typeStr = "double";
        else if constexpr (std::is_same_v<T, bool>) typeStr = "bool";
        else if constexpr (std::is_same_v<T, std::string>) typeStr = "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) typeStr = "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) typeStr = "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) typeStr = "vector<string>";
    }, value);
    return typeStr;
}

// Implementation of ConfigSchema methods
ConfigSchema& ConfigSchema::addConstraint(const ConfigConstraint& constraint) {
    constraints.push_back(constraint);
    return *this;
}

ConfigSchema& ConfigSchema::addRangeConstraint(const std::string& key, double min, double max, bool inclusiveMin, bool inclusiveMax, const std::string& errorMsg) {
    ConfigRangeConstraint constraint;
    constraint.key = key;
    constraint.min = min;
    constraint.max = max;
    constraint.inclusiveMin = inclusiveMin;
    constraint.inclusiveMax = inclusiveMax;
    constraint.errorMessage = errorMsg;
    rangeConstraints.push_back(constraint);
    return *this;
}

ConfigSchema& ConfigSchema::addCallbackConstraint(const std::string& key, std::function<bool(const ConfigValue&)> callback, const std::string& errorMsg) {
    ConfigConstraint constraint;
    constraint.key = key;
    constraint.op = ValidationOperator::Callback;
    constraint.callback = callback;
    constraint.errorMessage = errorMsg;
    constraints.push_back(constraint);
    return *this;
}

ConfigSchema& ConfigSchema::addGroup(const ConfigGroup& group) {
    groups.push_back(group);
    return *this;
}

template<typename T>
ConfigSchema& ConfigSchema::setDefault(const std::string& key, const T& value, ConfigSource source, const std::string& description) {
    defaults[key] = ConfigValue(value);
    descriptions[key] = description;
    defaultSources[key] = source;
    return *this;
}

std::vector<ConfigConstraint> ConfigSchema::getConstraints(const std::string& key) const {
    std::vector<ConfigConstraint> keyConstraints;
    for (const auto& constraint : constraints) {
        if (constraint.key == key) {
            keyConstraints.push_back(constraint);
        }
    }
    return keyConstraints;
}

std::vector<ConfigRangeConstraint> ConfigSchema::getRangeConstraints(const std::string& key) const {
    std::vector<ConfigRangeConstraint> keyRangeConstraints;
    for (const auto& constraint : rangeConstraints) {
        if (constraint.key == key) {
            keyRangeConstraints.push_back(constraint);
        }
    }
    return keyRangeConstraints;
}

template<typename T>
std::optional<T> ConfigSchema::getDefault(const std::string& key) const {
    auto it = defaults.find(key);
    if (it == defaults.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->second);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::vector<std::string> ConfigSchema::validate(const std::map<std::string, ConfigEntry>& entries) const {
    std::vector<std::string> errors;
    
    for (const auto& pair : entries) {
        const std::string& key = pair.first;
        const ConfigEntry& entry = pair.second;
        
        // Validate constraints
        auto constraints = getConstraints(key);
        for (const auto& constraint : constraints) {
            if (!validateConstraint(constraint, entry.value)) {
                std::string error = constraint.errorMessage.empty() ? 
                    ("Constraint '" + constraint.value + "' failed for key '" + key + "'") : 
                    constraint.errorMessage;
                errors.push_back(error);
            }
        }
        
        // Validate range constraints
        auto rangeConstraints = getRangeConstraints(key);
        for (const auto& rangeConstraint : rangeConstraints) {
            if (!validateRangeConstraint(rangeConstraint, entry.value)) {
                std::string error = rangeConstraint.errorMessage.empty() ? 
                    ("Range constraint failed for key '" + key + "': value must be between " + 
                     std::to_string(rangeConstraint.min) + " and " + std::to_string(rangeConstraint.max)) : 
                    rangeConstraint.errorMessage;
                errors.push_back(error);
            }
        }
    }
    
    return errors;
}

std::string ConfigSchema::generateDocumentation() const {
    std::string doc;
    doc += "# Configuration Schema\n\n";
    
    // Documentation for groups
    doc += "## Configuration Groups\n\n";
    for (const auto& group : groups) {
        doc += "### " + group.name + "\n";
        doc += group.description + "\n\n";
        doc += "Keys: ";
        for (size_t i = 0; i < group.keys.size(); ++i) {
            if (i > 0) doc += ", ";
            doc += "`" + group.keys[i] + "`";
        }
        doc += "\n\n";
    }
    
    // Documentation for defaults
    doc += "## Default Values\n\n";
    for (const auto& pair : defaults) {
        doc += "- `" + pair.first + "`: " + typeToString(pair.second) + " (source: " + 
               std::to_string(static_cast<int>(defaultSources.at(pair.first))) + ")\n";
        if (!descriptions.count(pair.first)) doc += "  (no description)\n";
    }
    
    // Documentation for constraints
    doc += "\n## Constraints\n\n";
    for (const auto& constraint : constraints) {
        doc += "- `" + constraint.key + "`: ";
        switch (constraint.op) {
            case ValidationOperator::Equal: doc += "== " + constraint.value; break;
            case ValidationOperator::NotEqual: doc += "!= " + constraint.value; break;
            case ValidationOperator::GreaterThan: doc += "> " + constraint.value; break;
            case ValidationOperator::LessThan: doc += "< " + constraint.value; break;
            case ValidationOperator::GreaterEqual: doc += ">= " + constraint.value; break;
            case ValidationOperator::LessEqual: doc += "<= " + constraint.value; break;
            case ValidationOperator::Range: doc += "in range " + constraint.value; break;
            case ValidationOperator::Callback: doc += "custom callback"; break;
        }
        if (!constraint.errorMessage.empty()) {
            doc += " (error: " + constraint.errorMessage + ")";
        }
        doc += "\n";
    }
    
    for (const auto& rangeConstraint : rangeConstraints) {
        doc += "- `" + rangeConstraint.key + "`: range [" + std::to_string(rangeConstraint.min) + ", " + 
               std::to_string(rangeConstraint.max) + "]";
        if (!rangeConstraint.errorMessage.empty()) {
            doc += " (error: " + rangeConstraint.errorMessage + ")";
        }
        doc += "\n";
    }
    
    return doc;
}

bool ConfigSchema::validateConstraint(const ConfigConstraint& constraint, const ConfigValue& value) {
    if (constraint.op == ValidationOperator::Callback && constraint.callback) {
        return constraint.callback(value);
    }
    
    return compareValues(value, constraint.value, constraint.op);
}

bool ConfigSchema::validateRangeConstraint(const ConfigRangeConstraint& constraint, const ConfigValue& value) {
    return validateRangeConstraintImpl(value, constraint.min, constraint.max);
}

std::optional<double> ConfigSchema::toDouble(const ConfigValue& value) {
    std::optional<double> result;
    std::visit([&result](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            result = static_cast<double>(val);
        } else if constexpr (std::is_same_v<T, double>) {
            result = val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            try {
                size_t pos;
                double v = std::stod(val, pos);
                if (pos == val.size()) {
                    result = v;
                }
            } catch (...) {
                // Failed to convert
            }
        }
    }, value);
    return result;
}

// Config implementation details
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::vector<ConfigConstraint>> constraints;
    std::map<std::string, std::vector<ConfigRangeConstraint>> rangeConstraints;
};

Config::Config(const std::shared_ptr<ConfigSchema>& schema) : schema(schema), version(1, 0, 0) {
    pImpl = std::make_unique<Impl>();
    
    // Apply schema defaults if schema is provided
    if (schema) {
        auto defaults = schema->getDefaults();
        for (const auto& pair : defaults) {
            set(pair.first, pair.second, ConfigSource::Default);
        }
    }
}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::validateConstraints(const ConfigEntry& entry, const ConfigSchema* schema) {
    if (!schema) return true;
    
    // Validate type
    if (!validateType(entry.value, entry.key)) {
        return false;
    }
    
    // Validate constraints
    auto constraints = schema->getConstraints(entry.key);
    for (const auto& constraint : constraints) {
        if (!validateConstraint(constraint, entry.value)) {
            return false;
        }
    }
    
    // Validate range constraints
    auto rangeConstraints = schema->getRangeConstraints(entry.key);
    for (const auto& rangeConstraint : rangeConstraints) {
        if (!validateRangeConstraint(rangeConstraint, entry.value)) {
            return false;
        }
    }
    
    return true;
}

// Explicit template instantiations for ConfigSchema
template ConfigSchema& ConfigSchema::setDefault<int>(const std::string&, const int&, ConfigSource, const std::string&);
template ConfigSchema& ConfigSchema::setDefault<int64_t>(const std::string&, const int64_t&, ConfigSource, const std::string&);
template ConfigSchema& ConfigSchema::setDefault<double>(const std::string&, const double&, ConfigSource, const std::string&);
template ConfigSchema& ConfigSchema::setDefault<bool>(const std::string&, const bool&, ConfigSource, const std::string&);
template ConfigSchema& ConfigSchema::setDefault<std::string>(const std::string&, const std::string&, ConfigSource, const std::string&);

template std::optional<int> ConfigSchema::getDefault<int>(const std::string&) const;
template std::optional<int64_t> ConfigSchema::getDefault<int64_t>(const std::string&) const;
template std::optional<double> ConfigSchema::getDefault<double>(const std::string&) const;
template std::optional<bool> ConfigSchema::getDefault<bool>(const std::string&) const;
template std::optional<std::string> ConfigSchema::getDefault<std::string>(const std::string&) const;

// Static schema initialization
std::shared_ptr<ConfigSchema> Config::getDefaultSchema() {
    static std::shared_ptr<ConfigSchema> defaultSchema = []() {
        auto schema = std::make_shared<ConfigSchema>();
        
        // Add common configuration groups
        schema->addGroup(ConfigGroup("Neural Network", "Neural network configuration"))
              .addGroup(ConfigGroup("Simulation", "Simulation parameters"))
              .addGroup(ConfigGroup("Learning", "Learning algorithm settings"))
              .addGroup(ConfigGroup("Memory", "Memory management settings"))
              .addGroup(ConfigGroup("Performance", "Performance optimization settings"));
        
        return schema;
    }();
    
    return defaultSchema;
}

bool Config::hasDefault(const std::string& key) const {
    return schema && schema->hasDefault(key);
}

template<typename T>
std::optional<T> Config::getDefault(const std::string& key) const {
    if (schema) {
        return schema->getDefault<T>(key);
    }
    return std::nullopt;
}
