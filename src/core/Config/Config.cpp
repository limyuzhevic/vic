#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unordered_set>
#include <limits>

namespace nlm {

// ConfigSchema implementation
void ConfigSchema::addRule(const ValidationRule& rule) {
    rules_.push_back(rule);
    if (!rule.keyPattern.empty()) {
        try {
            std::regex regex(rule.keyPattern);
            keyRuleMap_[rule.keyPattern] = rule;
        } catch (const std::regex_error&) {
            // Invalid regex, skip storing in map
        }
    }
}

ValidationResult ConfigSchema::validate(const std::string& key, const ConfigValue& value) const {
    // Check if key is valid
    if (!isValidKey(key)) {
        return ValidationResult::InvalidKey;
    }
    
    // Find matching rule
    bool ruleFound = false;
    for (const auto& rule : rules_) {
        if (rule.keyPattern.empty() || std::regex_match(key, std::regex(rule.keyPattern))) {
            ruleFound = true;
            
            // Check type
            bool typeValid = false;
            for (const auto& typeIndex : rule.allowedTypes) {
                if (typeIndex == typeid(int) && std::holds_alternative<int>(value)) {
                    typeValid = true;
                    break;
                } else if (typeIndex == typeid(int64_t) && std::holds_alternative<int64_t>(value)) {
                    typeValid = true;
                    break;
                } else if (typeIndex == typeid(double) && std::holds_alternative<double>(value)) {
                    typeValid = true;
                    break;
                } else if (typeIndex == typeid(bool) && std::holds_alternative<bool>(value)) {
                    typeValid = true;
                    break;
                } else if (typeIndex == typeid(std::string) && std::holds_alternative<std::string>(value)) {
                    typeValid = true;
                    break;
                } else if (typeIndex == typeid(std::vector<int>) && std::holds_alternative<std::vector<int>>(value)) {
                    typeValid = true;
                    break;
                } else if (typeIndex == typeid(std::vector<double>) && std::holds_alternative<std::vector<double>>(value)) {
                    typeValid = true;
                    break;
                } else if (typeIndex == typeid(std::vector<std::string>) && std::holds_alternative<std::vector<std::string>>(value)) {
                    typeValid = true;
                    break;
                }
            }
            
            if (!typeValid) {
                return ValidationResult::InvalidType;
            }
            
            // Validate value using custom validator
            if (rule.valueValidator && !rule.valueValidator(value)) {
                return ValidationResult::InvalidValue;
            }
            
            // Check range for numeric values
            if (std::holds_alternative<int>(value) && rule.valueValidator) {
                int intVal = std::get<int>(value);
                if (intVal < std::numeric_limits<int>::min() || intVal > std::numeric_limits<int>::max()) {
                    return ValidationResult::InvalidRange;
                }
            } else if (std::holds_alternative<double>(value) && rule.valueValidator) {
                double doubleVal = std::get<double>(value);
                if (std::isinf(doubleVal) || std::isnan(doubleVal)) {
                    return ValidationResult::InvalidRange;
                }
            }
            
            break;
        }
    }
    
    if (!ruleFound && !rules_.empty()) {
        return ValidationResult::InvalidKey;
    }
    
    return ValidationResult::Success;
}

bool ConfigSchema::isValidKey(const std::string& key) const {
    if (rules_.empty()) {
        return !key.empty();  // Basic validation if no schema
    }
    
    for (const auto& rule : rules_) {
        if (rule.keyPattern.empty() || std::regex_match(key, std::regex(rule.keyPattern))) {
            return true;
        }
    }
    
    return false;
}

std::string ConfigSchema::getErrorMessage(const std::string& key, const ConfigValue& value) const {
    ValidationResult result = validate(key, value);
    
    switch (result) {
        case ValidationResult::InvalidKey:
            return "Invalid key format: '" + key + "'";
        case ValidationResult::InvalidType:
            return "Value type mismatch for key: '" + key + "'";
        case ValidationResult::InvalidRange:
            return "Value out of range for key: '" + key + "'";
        case ValidationResult::InvalidValue:
            return "Value validation failed for key: '" + key + "'";
        case ValidationResult::ForbiddenOperation:
            return "Forbidden operation for key: '" + key + "'";
        case ValidationResult::Success:
        default:
            return "";
    }
}

void ConfigSchema::clear() {
    rules_.clear();
    keyRuleMap_.clear();
}

bool ConfigSchema::loadFromFile(const std::string& filepath) {
    // Validate file path for security
    std::filesystem::path path(filepath);
    std::filesystem::path normalized = std::filesystem::canonical(path);
    
    // Check for path traversal attempts
    std::filesystem::path basePath = ".";
    try {
        basePath = std::filesystem::current_path();
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
    
    // Ensure the path is within the current directory
    if (!normalized.is_relative() || !normalized.lexically_normal().string().empty()) {
        // Path traversal attempt detected
        return false;
    }
    
    // Check if file exists and is readable
    if (!std::filesystem::exists(normalized) || !std::filesystem::is_regular_file(normalized)) {
        return false;
    }
    
    std::ifstream file(normalized.string());
    if (!file.is_open()) {
        return false;
    }
    
    return loadFromString(std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));
}

bool ConfigSchema::loadFromString(const std::string& content) {
    clear();
    
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with enhanced security
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;  // Skip malformed lines
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string valueStr = trim(line.substr(pos + 1));
        
        // Validate key
        if (!isValidKey(key)) {
            continue;  // Skip invalid keys
        }
        
        // Parse value based on type detection
        ConfigValue value;
        bool parsed = false;
        
        // Try parsing as boolean
        if (valueStr == "true" || valueStr == "false") {
            value = (valueStr == "true");
            parsed = true;
        }
        // Try parsing as integer
        else if (isInteger(valueStr)) {
            value = std::stoll(valueStr);
            parsed = true;
        }
        // Try parsing as floating point
        else if (isDouble(valueStr)) {
            value = std::stod(valueStr);
            parsed = true;
        }
        // Try parsing as quoted string
        else if ((valueStr.front() == '"' && valueStr.back() == '"') ||
                (valueStr.front() == '\'' && valueStr.back() == '\'')) {
            value = valueStr.substr(1, valueStr.size() - 2);
            parsed = true;
        }
        // Treat as string
        else {
            value = valueStr;
            parsed = true;
        }
        
        if (parsed) {
            // Store the configuration entry
            // Note: This is a placeholder - actual implementation would need a container
            // for ConfigEntry objects
        }
    }
    
    return true;
}

// Config implementation with enhanced security
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<ConfigSchema> schema;
    std::unordered_set<std::string> forbiddenKeys;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->schema = std::make_unique<ConfigSchema>();
    // Set up default security rules
    setupSecurityRules();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Validate file path for security
    std::filesystem::path path(filepath);
    
    // Check for path traversal attempts
    std::string normalized = path.lexically_normal().string();
    if (normalized.find("..") != std::string::npos || normalized.find("\\") != std::string::npos) {
        return false;  // Path traversal attempt detected
    }
    
    // Check if file exists and is readable
    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
        return false;
    }
    
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with enhanced security
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;  // Skip malformed lines
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string valueStr = trim(line.substr(pos + 1));
        
        // Security check: reject keys that could be injection attempts
        if (isForbiddenKey(key)) {
            continue;  // Skip potentially dangerous keys
        }
        
        // Parse value
        ConfigValue value;
        bool parsed = parseValue(valueStr, value);
        
        if (parsed) {
            // Validate against schema if present
            if (pImpl->schema && !pImpl->schema->isValidKey(key)) {
                continue;  // Skip keys not in schema
            }
            
            if (pImpl->schema) {
                ValidationResult validationResult = pImpl->schema->validate(key, value);
                if (validationResult != ValidationResult::Success) {
                    continue;  // Skip invalid values
                }
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
                
                // Security check
                if (!isForbiddenKey(key) && validateKey(key) && parseValue(value, ConfigValue{})) {
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Security check
            if (!isForbiddenKey(key) && validateKey(key) && parseValue(value, ConfigValue{})) {
                set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Validate file path for security
    std::filesystem::path path(filepath);
    std::string normalized = path.lexically_normal().string();
    
    // Check for path traversal attempts
    if (normalized.find("..") != std::string::npos) {
        return false;
    }
    
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << serializeValue(entry.value) << "\n";
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
    // Security validation
    if (!validateKey(key)) {
        return;  // Reject invalid keys
    }
    
    if (pImpl->schema) {
        ValidationResult validationResult = pImpl->schema->validate(key, value);
        if (validationResult != ValidationResult::Success) {
            return;  // Reject invalid values
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
    if (!validateKey(key)) {
        return;  // Don't allow removal of invalid keys
    }
    
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

void Config::setupSecurityRules() {
    // Set up default security rules
    ValidationRule keyRule;
    keyRule.keyPattern = "^[a-zA-Z][a-zA-Z0-9_]*$";  // Alphanumeric with underscore
    keyRule.required = false;
    keyRule.errorMessage = "Key must contain only alphanumeric characters and underscores, starting with a letter";
    pImpl->schema->addRule(keyRule);
    
    // Add forbidden key patterns
    pImpl->forbiddenKeys.insert("__");
    pImpl->forbiddenKeys.insert("config");
    pImpl->forbiddenKeys.insert("system");
    pImpl->forbiddenKeys.insert("admin");
}

bool Config::isForbiddenKey(const std::string& key) const {
    return pImpl->forbiddenKeys.find(key) != pImpl->forbiddenKeys.end();
}

bool Config::validateKey(const std::string& key) const {
    if (key.empty() || key.length() > 256) {
        return false;
    }
    
    // Check for suspicious patterns
    if (isForbiddenKey(key)) {
        return false;
    }
    
    // Basic pattern validation
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool Config::parseValue(const std::string& str, ConfigValue& value) {
    // Try parsing as boolean
    if (str == "true" || str == "false") {
        value = (str == "true");
        return true;
    }
    
    // Try parsing as integer
    if (isInteger(str)) {
        try {
            value = std::stoll(str);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Try parsing as floating point
    if (isDouble(str)) {
        try {
            value = std::stod(str);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Check for quoted strings
    if ((str.front() == '"' && str.back() == '"') ||
        (str.front() == '\'' && str.back() == '\'')) {
        value = str.substr(1, str.size() - 2);
        return true;
    }
    
    // Treat as string
    value = str;
    return true;
}

std::string Config::serializeValue(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::isInteger(const std::string& str) {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
    }
    
    for (size_t i = start; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

bool Config::isDouble(const std::string& str) {
    if (str.empty()) return false;
    
    bool hasDecimal = false;
    bool hasExponent = false;
    size_t start = 0;
    
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
    }
    
    for (size_t i = start; i < str.size(); ++i) {
        if (std::isdigit(str[i])) {
            continue;
        } else if (str[i] == '.' && !hasDecimal && !hasExponent) {
            hasDecimal = true;
        } else if ((str[i] == 'e' || str[i] == 'E') && !hasExponent) {
            hasExponent = true;
            // Next character must be sign or digit
            if (i + 1 < str.size() && (str[i + 1] == '+' || str[i + 1] == '-' || std::isdigit(str[i + 1]))) {
                continue;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
    
    return hasDecimal || hasExponent || (str.find_first_not_of("+-0123456789") == std::string::npos);
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

    