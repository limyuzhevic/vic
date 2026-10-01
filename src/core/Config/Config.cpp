#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <cctype>

namespace nlm {

// ========== RANGE VALIDATOR IMPLEMENTATION ==========

bool RangeValidator::validate(const ConfigValue& value, std::string& errorMessage) const {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            double val = (std::is_same_v<T, double> ? arg : 
                         std::is_same_v<T, int64_t> ? static_cast<double>(arg) : 
                         static_cast<double>(arg));
            
            switch (type_) {
                case Type::MIN:
                    if (val < min_) {
                        errorMessage = "Value " + std::to_string(val) + " is less than minimum " + std::to_string(min_);
                    }
                    break;
                case Type::MAX:
                    if (val > max_) {
                        errorMessage = "Value " + std::to_string(val) + " is greater than maximum " + std::to_string(max_);
                    }
                    break;
                case Type::MIN_MAX:
                    if (val < min_ || val > max_) {
                        errorMessage = "Value " + std::to_string(val) + " is outside range [" + 
                                     std::to_string(min_) + ", " + std::to_string(max_) + "]";
                    }
                    break;
                case Type::EXCLUSIVE_MIN:
                    if (val <= min_) {
                        errorMessage = "Value " + std::to_string(val) + " is less than or equal to minimum " + std::to_string(min_);
                    }
                    break;
                case Type::EXCLUSIVE_MAX:
                    if (val >= max_) {
                        errorMessage = "Value " + std::to_string(val) + " is greater than or equal to maximum " + std::to_string(max_);
                    }
                    break;
            }
        }
    }, value);
    
    return errorMessage.empty();
}

// ========== TYPE VALIDATOR IMPLEMENTATION ==========

bool TypeValidator::validate(const ConfigValue& value, std::string& errorMessage) const {
    bool isValid = false;
    
    switch (expectedType_) {
        case ExpectedType::INT:
            isValid = std::holds_alternative<int>(value);
            if (!isValid) errorMessage = "Expected int, got different type";
            break;
        case ExpectedType::DOUBLE:
            isValid = std::holds_alternative<double>(value);
            if (!isValid) errorMessage = "Expected double, got different type";
            break;
        case ExpectedType::STRING:
            isValid = std::holds_alternative<std::string>(value);
            if (!isValid) errorMessage = "Expected string, got different type";
            break;
        case ExpectedType::BOOL:
            isValid = std::holds_alternative<bool>(value);
            if (!isValid) errorMessage = "Expected bool, got different type";
            break;
        case ExpectedType::VECTOR_INT:
            isValid = std::holds_alternative<std::vector<int>>(value);
            if (!isValid) errorMessage = "Expected vector<int>, got different type";
            break;
        case ExpectedType::VECTOR_DOUBLE:
            isValid = std::holds_alternative<std::vector<double>>(value);
            if (!isValid) errorMessage = "Expected vector<double>, got different type";
            break;
        case ExpectedType::VECTOR_STRING:
            isValid = std::holds_alternative<std::vector<std::string>>(value);
            if (!isValid) errorMessage = "Expected vector<string>, got different type";
            break;
    }
    
    return isValid;
}

// ========== CONFIG IMPLEMENTATION ==========

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Validation system
    std::vector<ValidationRule> validationRules;
    std::vector<std::string> validationErrors;
    
    // Runtime configuration
    std::unordered_map<std::string, std::string> runtimeQueries;
    
    // Dynamic parameters
    std::unordered_map<std::string, std::pair<double, double>> dynamicRanges;
    std::unordered_map<std::string, double> dynamicSteps;
    
    // Schema information
    std::vector<std::string> requiredKeys;
    std::vector<std::string> derivedKeys;
    
    Impl() : validationErrors() {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO: Implement proper JSON/YAML parser
    // For now, keep backward compatibility with simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigurationError("Cannot open config file: " + filepath);
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

bool Config::loadFromString(const std::string& content) {
    // Simple parser for key=value format
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigurationError("Cannot create config file: " + filepath);
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by NLM Configuration System" << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << std::endl;
        file << entry.key << " = ";
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
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
        
        file << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
        file << std::endl;
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
    // Validate if validation rule exists
    std::string errorMessage;
    bool isValid = true;
    
    for (const auto& rule : pImpl->validationRules) {
        if (rule.key == key) {
            for (const auto& validator : rule.validators) {
                if (!validator(value, errorMessage)) {
                    isValid = false;
                    addValidationError(key, errorMessage);
                }
            }
        }
    }
    
    if (!isValid) {
        throw ConfigurationError("Configuration validation failed for key: " + key + ". Error: " + errorMessage);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        
        // Update metadata for dynamic parameters
        if (source == ConfigSource::Runtime && pImpl->dynamicRanges.count(key)) {
            auto range = pImpl->dynamicRanges[key];
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>) {
                    arg = std::max(range.first, std::min(range.second, arg));
                }
            }, it->value);
        }
    } else {
        ConfigEntry newEntry(key, value, source);
        
        // Check if key is required
        if (std::find(pImpl->requiredKeys.begin(), pImpl->requiredKeys.end(), key) != pImpl->requiredKeys.end()) {
            newEntry.required = true;
        }
        
        pImpl->entries.push_back(std::move(newEntry));
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

void Config::setDerived(const std::string& key, const ConfigValue& value) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = ConfigSource::Derived;
        it->derived = true;
        pImpl->derivedKeys.push_back(key);
    }
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
    
    // Remove from required/derived lists
    pImpl->requiredKeys.erase(
        std::remove_if(pImpl->requiredKeys.begin(), pImpl->requiredKeys.end(),
            [&key](const std::string& k) { return k == key; }),
        pImpl->requiredKeys.end()
    );
    
    pImpl->derivedKeys.erase(
        std::remove_if(pImpl->derivedKeys.begin(), pImpl->derivedKeys.end(),
            [&key](const std::string& k) { return k == key; }),
        pImpl->derivedKeys.end()
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
    pImpl->validationRules.clear();
    pImpl->validationErrors.clear();
    pImpl->runtimeQueries.clear();
    pImpl->dynamicRanges.clear();
    pImpl->dynamicSteps.clear();
    pImpl->requiredKeys.clear();
    pImpl->derivedKeys.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " 
        << pImpl->validationRules.size() << " validation rules):" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        
        if (entry.required) oss << " [REQUIRED]";
        if (entry.derived) oss << " [DERIVED]";
        
        if (!entry.description.empty()) oss << " - " << entry.description;
        
        oss << std::endl;
    }
    
    if (!pImpl->validationErrors.empty()) {
        oss << std::endl << "Validation Errors:" << std::endl;
        for (const auto& error : pImpl->validationErrors) {
            oss << "  " << error << std::endl;
        }
    }
    
    return oss.str();
}

void Config::addValidationRule(const ValidationRule& rule) {
    pImpl->validationRules.push_back(rule);
}

void Config::addRangeValidator(const std::string& key, double min, double max, const std::string& desc) {
    auto validator = [min, max](const ConfigValue& value, std::string& errorMessage) {
        return RangeValidator(RangeValidator::Type::MIN_MAX, min, max).validate(value, errorMessage);
    };
    
    ValidationRule rule(key, {validator}, desc);
    addValidationRule(rule);
}

void Config::addTypeValidator(const std::string& key, const TypeValidator::ExpectedType& type, const std::string& desc) {
    auto validator = [type](const ConfigValue& value, std::string& errorMessage) {
        return TypeValidator(type).validate(value, errorMessage);
    };
    
    ValidationRule rule(key, {validator}, desc);
    addValidationRule(rule);
}

bool Config::validate(const std::string& key, std::string& errorMessage) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        errorMessage = "Key not found: " + key;
        return false;
    }
    
    for (const auto& rule : pImpl->validationRules) {
        if (rule.key == key) {
            for (const auto& validator : rule.validators) {
                if (!validator(it->value, errorMessage)) {
                    addValidationError(key, errorMessage);
                    return false;
                }
            }
        }
    }
    
    return true;
}

bool Config::validateAll() const {
    pImpl->validationErrors.clear();
    
    bool allValid = true;
    for (const auto& entry : pImpl->entries) {
        std::string errorMessage;
        if (!validate(entry.key, errorMessage)) {
            allValid = false;
        }
    }
    
    return allValid;
}

void Config::enableRuntimeQuery(const std::string& key, const std::string& queryExpr) {
    pImpl->runtimeQueries[key] = queryExpr;
}

ConfigValue Config::evaluateQuery(const std::string& queryExpr) const {
    // Simple query evaluation - just parse arithmetic expressions
    // In a real implementation, you'd use a proper expression parser
    try {
        // Handle simple arithmetic: a + b, a - b, a * b, a / b
        size_t plusPos = queryExpr.find('+');
        size_t minusPos = queryExpr.find('-');
        size_t mulPos = queryExpr.find('*');
        size_t divPos = queryExpr.find('/');
        
        // Find the operator position
        size_t opPos = std::string::npos;
        char op = 0;
        
        if (plusPos != std::string::npos && (opPos == std::string::npos || plusPos < opPos)) {
            opPos = plusPos; op = '+';
        }
        if (minusPos != std::string::npos && (opPos == std::string::npos || minusPos < opPos)) {
            opPos = minusPos; op = '-';
        }
        if (mulPos != std::string::npos && (opPos == std::string::npos || mulPos < opPos)) {
            opPos = mulPos; op = '*';
        }
        if (divPos != std::string::npos && (opPos == std::string::npos || divPos < opPos)) {
            opPos = divPos; op = '/';
        }
        
        if (opPos != std::string::npos) {
            std::string left = queryExpr.substr(0, opPos);
            std::string right = queryExpr.substr(opPos + 1);
            
            // Trim whitespace
            left = trim(left);
            right = trim(right);
            
            // Try to get values from config
            double leftVal = 0.0, rightVal = 0.0;
            
            auto leftOpt = get<double>(left);
            if (leftOpt.has_value()) {
                leftVal = leftOpt.value();
            } else {
                // Try to parse as literal
                try { leftVal = std::stod(left); } catch (...) {}
            }
            
            auto rightOpt = get<double>(right);
            if (rightOpt.has_value()) {
                rightVal = rightOpt.value();
            } else {
                // Try to parse as literal
                try { rightVal = std::stod(right); } catch (...) {}
            }
            
            switch (op) {
                case '+': return leftVal + rightVal;
                case '-': return leftVal - rightVal;
                case '*': return leftVal * rightVal;
                case '/': 
                    if (rightVal != 0.0) return leftVal / rightVal;
                    else throw ConfigurationError("Division by zero in query: " + queryExpr);
            }
        }
        
        // If no operator found, try to get the value directly
        auto valOpt = get<double>(queryExpr);
        if (valOpt.has_value()) {
            return valOpt.value();
        }
        
        // Return 0 if query cannot be evaluated
        return 0.0;
        
    } catch (const std::exception& e) {
        throw ConfigurationError("Query evaluation error: " + std::string(e.what()) + " in query: " + queryExpr);
    }
}

std::vector<std::string> Config::getRequiredKeys() const {
    return pImpl->requiredKeys;
}

std::vector<std::string> Config::getDerivedKeys() const {
    return pImpl->derivedKeys;
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

void Config::enableDynamicParameter(const std::string& key, double minValue, double maxValue, double step) {
    pImpl->dynamicRanges[key] = std::make_pair(minValue, maxValue);
    pImpl->dynamicSteps[key] = step;
    
    // Validate existing value against new range
    if (has(key)) {
        auto valOpt = get<double>(key);
        if (valOpt.has_value()) {
            double val = valOpt.value();
            val = std::max(minValue, std::min(maxValue, val));
            set(key, val, ConfigSource::Runtime);
        }
    }
}

ConfigValue Config::getDynamicValue(const std::string& key) const {
    if (pImpl->dynamicRanges.count(key)) {
        auto valOpt = get<double>(key);
        if (valOpt.has_value()) {
            double val = valOpt.value();
            double step = pImpl->dynamicSteps.count(key) ? pImpl->dynamicSteps[key] : 0.1;
            
            // Round to nearest step
            val = std::round(val / step) * step;
            return val;
        }
    }
    
    throw ConfigurationError("Dynamic parameter not found: " + key);
}

void Config::addValidationError(const std::string& key, const std::string& error) {
    pImpl->validationErrors.push_back(key + ": " + error);
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