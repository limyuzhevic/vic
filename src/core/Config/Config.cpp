#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <cmath>
#include <stdexcept>

namespace nlm {

// ============= EXCEPTION IMPLEMENTATIONS =============

// Exception constructors
ConfigError::ConfigError(const std::string& message) : NLMException("ConfigError: " + message) {}
ConfigValidationError::ConfigValidationError(const std::string& key, const std::string& message)
    : ConfigError("ConfigValidationError for key '" + key + "': " + message) {}
ConfigSchemaError::ConfigSchemaError(const std::string& message) : ConfigError("ConfigSchemaError: " + message) {}
ConfigMergeError::ConfigMergeError(const std::string& message) : ConfigError("ConfigMergeError: " + message) {}
IntegrationError::IntegrationError(const std::string& message) : NLMException("IntegrationError: " + message) {}
BrainInitializationError::BrainInitializationError(const std::string& message) : IntegrationError("BrainInitializationError: " + message) {}
MemorySystemError::MemorySystemError(const std::string& message) : IntegrationError("MemorySystemError: " + message) {}
MemoryError::MemoryError(const std::string& message) : NLMException("MemoryError: " + message) {}
PlasticityError::PlasticityError(const std::string& message) : MemoryError("PlasticityError: " + message) {}
SynapticError::SynapticError(const std::string& message) : MemoryError("SynapticError: " + message) {}

// ============= VALIDATION RESULT IMPLEMENTATIONS =============

std::string ValidationResult::getSummary() const {
    std::stringstream ss;
    ss << "Validation " << (isValid ? "PASSED" : "FAILED");
    if (errors.size() > 0) ss << ", " << errors.size() << " errors";
    if (warnings.size() > 0) ss << ", " << warnings.size() << " warnings";
    ss << " in " << validationTime.count() << " ms";
    return ss.str();
}

// ============= CONFIG SCHEMA IMPLEMENTATION =============

struct ConfigSchema::Impl {
    std::vector<ValidationRule> rules;
    std::map<std::string, ValidationRule> ruleMap;
};

ConfigSchema::ConfigSchema() : pImpl(std::make_unique<Impl>()) {}
ConfigSchema::~ConfigSchema() = default;

void ConfigSchema::addRule(const ValidationRule& rule) {
    validationRules.push_back(rule);
    pImpl->rules.push_back(rule);
    pImpl->ruleMap[rule.key] = rule;
}

void ConfigSchema::addRule(const std::string& key, const std::type_index& type, bool required,
                          const std::variant<int, double>& minValue,
                          const std::variant<int, double>& maxValue,
                          const std::string& defaultValue) {
    ValidationRule rule;
    rule.key = key;
    rule.type = type;
    rule.required = required;
    rule.minValue = minValue;
    rule.maxValue = maxValue;
    rule.defaultValue = defaultValue;
    
    validationRules.push_back(rule);
    pImpl->rules.push_back(rule);
    pImpl->ruleMap[key] = rule;
}

void ConfigSchema::addStringRule(const std::string& key, size_t minLength, size_t maxLength,
                                 const std::string& pattern, bool required) {
    ValidationRule rule;
    rule.key = key;
    rule.type = typeid(std::string);
    rule.validationPattern = pattern;
    rule.required = required;
    // For string rules, we'll store length constraints in validationPattern
    std::stringstream lenSpec;
    lenSpec << "len:" << minLength << "," << maxLength;
    if (!pattern.empty()) lenSpec << "," << pattern;
    rule.validationPattern = lenSpec.str();
    
    validationRules.push_back(rule);
    pImpl->rules.push_back(rule);
    pImpl->ruleMap[key] = rule;
}

void ConfigSchema::addNumericRule(const std::string& key, const std::type_index& type,
                                  const std::variant<int, double>& minValue,
                                  const std::variant<int, double>& maxValue,
                                  bool required) {
    ValidationRule rule;
    rule.key = key;
    rule.type = type;
    rule.required = required;
    rule.minValue = minValue;
    rule.maxValue = maxValue;
    
    validationRules.push_back(rule);
    pImpl->rules.push_back(rule);
    pImpl->ruleMap[key] = rule;
}

void ConfigSchema::addRangeRule(const std::string& key, double minValue, double maxValue,
                                bool required) {
    ValidationRule rule;
    rule.key = key;
    rule.type = typeid(double);
    rule.required = required;
    rule.minValue = minValue;
    rule.maxValue = maxValue;
    
    validationRules.push_back(rule);
    pImpl->rules.push_back(rule);
    pImpl->ruleMap[key] = rule;
}

void ConfigSchema::addListRule(const std::string& key, const std::type_index& elementType,
                               size_t minItems, size_t maxItems, bool required) {
    ValidationRule rule;
    rule.key = key;
    rule.type = elementType;
    rule.required = required;
    // Store list constraints in validationPattern
    std::stringstream listSpec;
    listSpec << "list:" << elementType.name() << "," << minItems << "," << maxItems;
    rule.validationPattern = listSpec.str();
    
    validationRules.push_back(rule);
    pImpl->rules.push_back(rule);
    pImpl->ruleMap[key] = rule;
}

ValidationResult ConfigSchema::validate(const Config& config) const {
    ValidationResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (const auto& rule : validationRules) {
        if (!config.has(rule.key)) {
            if (rule.required) {
                result.addError(rule.key, "Required configuration key is missing", ConfigSource::Default);
            }
            continue;
        }
        
        if (!validateValueAgainstRule(config.get<ConfigValue>(rule.key).value(), rule)) {
            result.addError(rule.key, "Value does not match schema validation rule", ConfigSource::Default);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    return result;
}

bool ConfigSchema::hasRule(const std::string& key) const {
    return pImpl->ruleMap.find(key) != pImpl->ruleMap.end();
}

void ConfigSchema::injectDefaults(Config& config) const {
    for (const auto& rule : validationRules) {
        if (rule.required && !config.has(rule.key) && !rule.defaultValue.empty()) {
            // Inject default value based on type
            if (rule.type == typeid(int)) {
                try {
                    int value = std::stoi(rule.defaultValue);
                    config.set(rule.key, value, ConfigSource::Default);
                } catch (...) {}
            } else if (rule.type == typeid(double)) {
                try {
                    double value = std::stod(rule.defaultValue);
                    config.set(rule.key, value, ConfigSource::Default);
                } catch (...) {}
            } else if (rule.type == typeid(bool)) {
                if (rule.defaultValue == "true" || rule.defaultValue == "1") {
                    config.set(rule.key, true, ConfigSource::Default);
                } else if (rule.defaultValue == "false" || rule.defaultValue == "0") {
                    config.set(rule.key, false, ConfigSource::Default);
                }
            } else if (rule.type == typeid(std::string)) {
                config.set(rule.key, rule.defaultValue, ConfigSource::Default);
            }
        }
    }
}

ConfigSchema ConfigSchema::getDefaultSchema() {
    ConfigSchema schema;
    
    // Add common configuration rules
    schema.addRangeRule("learning_rate", 0.0, 1.0, false);
    schema.addRangeRule("threshold", 0.0, 1.0, false);
    schema.addRangeRule("decay_rate", 0.0, 1.0, false);
    schema.addNumericRule("max_neurons", typeid(int), 1, 1000000, false);
    schema.addNumericRule("synaptic_strength", typeid(double), 0.0, 10.0, false);
    schema.addStringRule("model_name", 1, 100, "^[a-zA-Z0-9_-]+$", false);
    schema.addListRule("hidden_layers", typeid(int), 1, 10, false);
    
    return schema;
}

bool ConfigSchema::validateValueAgainstRule(const ConfigValue& value, const ValidationRule& rule) const {
    if (rule.type == typeid(void)) {
        return true; // No type validation
    }
    
    if (!rule.validationPattern.empty() && rule.validationPattern.substr(0, 4) == "len:") {
        // String length validation
        auto lenParts = split(rule.validationPattern.substr(4), ',');
        if (lenParts.size() >= 2) {
            size_t minLen = std::stoull(lenParts[0]);
            size_t maxLen = std::stoull(lenParts[1]);
            if (auto val = std::get_if<std::string>(&value)) {
                return val->length() >= minLen && val->length() <= maxLen;
            }
        }
    } else if (!rule.validationPattern.empty() && rule.validationPattern.substr(0, 5) == "list:") {
        // List validation
        auto listParts = split(rule.validationPattern.substr(5), ',');
        if (listParts.size() >= 3) {
            if (auto val = std::get_if<std::vector<int>>(&value)) {
                size_t minItems = std::stoull(listParts[2]);
                size_t maxItems = std::stoull(listParts[3]);
                return val->size() >= minItems && val->size() <= maxItems;
            } else if (auto val = std::get_if<std::vector<double>>(&value)) {
                size_t minItems = std::stoull(listParts[2]);
                size_t maxItems = std::stoull(listParts[3]);
                return val->size() >= minItems && val->size() <= maxItems;
            } else if (auto val = std::get_if<std::vector<std::string>>(&value)) {
                size_t minItems = std::stoull(listParts[2]);
                size_t maxItems = std::stoull(listParts[3]);
                return val->size() >= minItems && val->size() <= maxItems;
            }
        }
    }
    
    // Standard type and range validation
    if (rule.type == typeid(int) || rule.type == typeid(int64_t)) {
        if (auto val = std::get_if<int>(&value)) {
            if (rule.minValue.has_value()) {
                if (auto min = std::get_if<int>(&rule.minValue.value())) {
                    if (*val < *min) return false;
                }
            }
            if (rule.maxValue.has_value()) {
                if (auto max = std::get_if<int>(&rule.maxValue.value())) {
                    if (*val > *max) return false;
                }
            }
            return true;
        }
        return false;
    } else if (rule.type == typeid(double)) {
        if (auto val = std::get_if<double>(&value)) {
            if (rule.minValue.has_value()) {
                if (auto min = std::get_if<double>(&rule.minValue.value())) {
                    if (*val < *min) return false;
                }
            }
            if (rule.maxValue.has_value()) {
                if (auto max = std::get_if<double>(&rule.maxValue.value())) {
                    if (*val > *max) return false;
                }
            }
            return true;
        }
        return false;
    } else if (rule.type == typeid(bool)) {
        return std::get_if<bool>(&value) != nullptr;
    } else if (rule.type == typeid(std::string)) {
        if (auto val = std::get_if<std::string>(&value)) {
            if (!rule.validationPattern.empty() && rule.validationPattern.substr(0, 4) == "len:") {
                // Already handled above
                return true;
            }
            // Regex pattern validation
            if (std::regex_search(*val, std::regex(rule.validationPattern))) {
                return true;
            } else if (rule.validationPattern.empty()) {
                return true; // No pattern specified
            }
            return false;
        }
        return false;
    }
    
    // For complex types, basic existence check
    return !value.valueless_by_exception();
}

std::type_index ConfigSchema::getTypeIndexFromVariant(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) return typeid(int);
    if (std::holds_alternative<int64_t>(value)) return typeid(int64_t);
    if (std::holds_alternative<double>(value)) return typeid(double);
    if (std::holds_alternative<bool>(value)) return typeid(bool);
    if (std::holds_alternative<std::string>(value)) return typeid(std::string);
    if (std::holds_alternative<std::vector<int>>(value)) return typeid(std::vector<int>);
    if (std::holds_alternative<std::vector<double>>(value)) return typeid(std::vector<double>);
    if (std::holds_alternative<std::vector<std::string>>(value)) return typeid(std::vector<std::string>);
    return typeid(void);
}

// Helper function to split string
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// ============= CONFIG HEALTH MONITOR IMPLEMENTATION =============

struct ConfigHealthMonitor::Impl {
    std::vector<HealthMetric> metrics;
    std::vector<std::function<ValidationResult()>> healthChecks;
};

ConfigHealthMonitor::ConfigHealthMonitor() : pImpl(std::make_unique<Impl>()) {}
ConfigHealthMonitor::~ConfigHealthMonitor() = default;

void ConfigHealthMonitor::recordMetric(const std::string& name, double value, const std::string& unit, bool isHealthy) {
    HealthMetric metric;
    metric.metricName = name;
    metric.value = value;
    metric.unit = unit;
    metric.timestamp = std::chrono::system_clock::now();
    metric.isHealthy = isHealthy;
    
    metrics.push_back(metric);
    pImpl->metrics.push_back(metric);
}

ValidationResult ConfigHealthMonitor::checkHealth() const {
    ValidationResult result;
    
    for (const auto& check : pImpl->healthChecks) {
        auto checkResult = check();
        if (!checkResult.isValid) {
            for (const auto& error : checkResult.errors) {
                result.addError(error.key, error.errorMessage, error.source);
            }
        }
        for (const auto& warning : checkResult.warnings) {
            result.addWarning(warning);
        }
    }
    
    return result;
}

void ConfigHealthMonitor::addHealthCheck(std::function<ValidationResult()> check) {
    pImpl->healthChecks.push_back(check);
}

double ConfigHealthMonitor::getMetricAverage(const std::string& name) const {
    double sum = 0.0;
    size_t count = 0;
    
    for (const auto& metric : metrics) {
        if (metric.metricName == name) {
            sum += metric.value;
            count++;
        }
    }
    
    if (count == 0) return 0.0;
    return sum / count;
}

// ============= CONFIG IMPLEMENTATION - CONSTRUCTOR/DESCTRUCTOR =============

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSchema> schemas;
    std::unique_ptr<ConfigHealthMonitor> healthMonitor;
    std::map<std::string, std::chrono::milliseconds> performanceMetrics;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->healthMonitor = std::make_unique<ConfigHealthMonitor>();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

// ============= CONFIG IMPLEMENTATION - BASIC ACCESSORS/MODIFIERS =============

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
        T value = std::get<T>(it->value);
        return value;
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value) {
        return *value;
    }
    return defaultValue;
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
    set(key, value, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, value, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, value, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, value, source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                                      [&key](const ConfigEntry& e) { return e.key == key; }),
                       pImpl->entries.end());
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "Configuration Summary:";
    ss << "\nTotal entries: " << pImpl->entries.size();
    
    // Count by source
    std::map<std::string, size_t> sourceCount;
    for (const auto& entry : pImpl->entries) {
        std::string sourceStr;
        switch (entry.source) {
            case ConfigSource::Default: sourceStr = "Default"; break;
            case ConfigSource::File: sourceStr = "File"; break;
            case ConfigSource::CommandLine: sourceStr = "CommandLine"; break;
            case ConfigSource::Runtime: sourceStr = "Runtime"; break;
        }
        sourceCount[sourceStr]++;
    }
    
    ss << "\nBy source:";
    for (const auto& pair : sourceCount) {
        ss << "\n  " << pair.first << ": " << pair.second;
    }
    
    return ss.str();
}

// ============= CONFIG IMPLEMENTATION - ENHANCED VALIDATION METHODS =============

ValidationResult Config::validate() const {
    auto startTime = std::chrono::high_resolution_clock::now();
    ValidationResult result;
    
    // Basic validation rules
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            result.addError(entry.key, "Empty key", entry.source);
            continue;
        }
        
        // Validate value type based on key patterns
        if (entry.key.find("threshold") != std::string::npos) {
            if (auto val = get<double>(entry.key)) {
                if (*val < 0.0 || *val > 1.0) {
                    result.addError(entry.key, "Value must be between 0.0 and 1.0", entry.source);
                }
            }
        }
        
        if (entry.key.find("count") != std::string::npos || entry.key.find("max") != std::string::npos) {
            if (auto val = get<int>(entry.key)) {
                if (*val < 0) {
                    result.addError(entry.key, "Count/max must be non-negative", entry.source);
                }
            }
        }
        
        if (entry.key.find("rate") != std::string::npos || entry.key.find("level") != std::string::npos) {
            if (auto val = get<double>(entry.key)) {
                if (*val < 0.0) {
                    result.addError(entry.key, "Rate/level must be non-negative", entry.source);
                }
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Record performance metric
    recordValidationPerformance("basic_validation", result.validationTime.count());
    
    return result;
}

ValidationResult Config::validateAgainstSchema(const ConfigSchema& schema) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    ValidationResult result;
    
    // Use schema validation
    result = schema.validate(*this);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Record performance metric
    recordValidationPerformance("schema_validation", result.validationTime.count());
    
    return result;
}

std::vector<ConfigValidationError> Config::getValidationErrors() const {
    std::vector<ConfigValidationError> errors;
    
    ValidationResult result = validate();
    errors = result.errors;
    
    return errors;
}

bool Config::fixCommonIssues() {
    bool fixed = false;
    
    // Fix empty keys
    for (auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            entry.key = "unnamed_" + std::to_string(std::rand());
            fixed = true;
        }
    }
    
    // Fix out-of-range values for known keys
    for (auto& entry : pImpl->entries) {
        if (entry.key.find("threshold") != std::string::npos) {
            if (auto val = std::get_if<double>(&entry.value)) {
                if (*val < 0.0) {
                    *val = 0.0;
                    fixed = true;
                } else if (*val > 1.0) {
                    *val = 1.0;
                    fixed = true;
                }
            }
        }
        
        if (entry.key.find("count") != std::string::npos || entry.key.find("max") != std::string::npos) {
            if (auto val = std::get_if<int>(&entry.value)) {
                if (*val < 0) {
                    *val = 0;
                    fixed = true;
                }
            }
        }
        
        if (entry.key.find("rate") != std::string::npos || entry.key.find("level") != std::string::npos) {
            if (auto val = std::get_if<double>(&entry.value)) {
                if (*val < 0.0) {
                    *val = 0.0;
                    fixed = true;
                }
            }
        }
    }
    
    return fixed;
}

bool Config::mergeConfigs(const Config& other, ConfigSource source) {
    bool merged = false;
    
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it == pImpl->entries.end() || static_cast<int>(it->source) < static_cast<int>(entry.source)) {
            pImpl->entries.push_back(entry);
            merged = true;
        }
    }
    
    return merged;
}

std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::map<std::string, std::string>>> 
Config::toPythonDict() const {
    std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::map<std::string, std::string>>> result;
    
    for (const auto& entry : pImpl->entries) {
        try {
            // Try to get the appropriate type
            if (auto val = get<int>(entry.key)) {
                result[entry.key] = *val;
            } else if (auto val = get<int64_t>(entry.key)) {
                result[entry.key] = static_cast<int>(*val);
            } else if (auto val = get<double>(entry.key)) {
                result[entry.key] = *val;
            } else if (auto val = get<bool>(entry.key)) {
                result[entry.key] = *val;
            } else if (auto val = get<std::string>(entry.key)) {
                result[entry.key] = *val;
            } else {
                // For complex types, create a string representation
                std::string str;
                std::visit([&str](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::vector<int>>) {
                        str = "[";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            if (i > 0) str += ", ";
                            str += std::to_string(arg[i]);
                        }
                        str += "]";
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        str = "[";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            if (i > 0) str += ", ";
                            str += std::to_string(arg[i]);
                        }
                        str += "]";
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        str = "[";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            if (i > 0) str += ", ";
                            str += "\"" + arg[i] + "\"";
                        }
                        str += "]";
                    }
                }, entry.value);
                result[entry.key] = str;
            }
        } catch (...) {
            // Skip problematic entries
            continue;
        }
    }
    
    return result;
}

bool Config::fromPythonDict(const std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::map<std::string, std::string>>>& dict) {
    bool loaded = false;
    
    for (const auto& pair : dict) {
        const std::string& key = pair.first;
        const auto& value = pair.second;
        
        try {
            if (std::holds_alternative<int>(value)) {
                set(key, std::get<int>(value), ConfigSource::Runtime);
            } else if (std::holds_alternative<double>(value)) {
                set(key, std::get<double>(value), ConfigSource::Runtime);
            } else if (std::holds_alternative<bool>(value)) {
                set(key, std::get<bool>(value), ConfigSource::Runtime);
            } else if (std::holds_alternative<std::string>(value)) {
                set(key, std::get<std::string>(value), ConfigSource::Runtime);
            } else if (std::holds_alternative<std::vector<int>>(value)) {
                set(key, std::get<std::vector<int>>(value), ConfigSource::Runtime);
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                set(key, std::get<std::vector<double>>(value), ConfigSource::Runtime);
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                set(key, std::get<std::vector<std::string>>(value), ConfigSource::Runtime);
            }
            loaded = true;
        } catch (...) {
            // Skip problematic entries
            continue;
        }
    }
    
    return loaded;
}

// ============= CONFIG IMPLEMENTATION - ADDITIONAL VALIDATION METHODS =============

bool Config::validateRuntime() const {
    ValidationResult result = validate();
    return result.isValid;
}

ValidationResult Config::validateHealth() const {
    ValidationResult result;
    
    // Check basic validation
    ValidationResult basicResult = validate();
    if (!basicResult.isValid) {
        result.isValid = false;
        for (const auto& error : basicResult.errors) {
            result.addError(error.key, error.errorMessage, error.source);
        }
    }
    
    // Check schema validation if schema exists
    if (!pImpl->schemas.empty()) {
        ValidationResult schemaResult;
        for (const auto& schemaPair : pImpl->schemas) {
            ValidationResult schemaValidation = schemaPair.second.validate(*this);
            if (!schemaValidation.isValid) {
                result.isValid = false;
                for (const auto& error : schemaValidation.errors) {
                    result.addError(error.key, error.errorMessage, error.source);
                }
            }
        }
    }
    
    // Record performance metric
    recordValidationPerformance("health_validation", result.validationTime.count());
    
    return result;
}

bool Config::repairConfiguration() {
    bool repaired = false;
    
    // Fix common issues
    if (fixCommonIssues()) {
        repaired = true;
    }
    
    // Inject default values from schema
    for (const auto& schemaPair : pImpl->schemas) {
        schemaPair.second.injectDefaults(*this);
        repaired = true;
    }
    
    return repaired;
}

// ============= CONFIG IMPLEMENTATION - CROSS-VALIDATION =============

ValidationResult Config::crossValidate() const {
    ValidationResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Check for conflicting values
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        for (size_t j = i + 1; j < pImpl->entries.size(); ++j) {
            const auto& entry1 = pImpl->entries[i];
            const auto& entry2 = pImpl->entries[j];
            
            // Check for common naming patterns that shouldn't conflict
            if (entry1.key.find("rate") != std::string::npos && entry2.key.find("rate") != std::string::npos && entry1.key != entry2.key) {
                // Multiple rate parameters - check if they're reasonable
                double val1, val2;
                bool isDouble1 = false, isDouble2 = false;
                
                if (auto v1 = get<double>(entry1.key)) val1 = *v1, isDouble1 = true;
                if (auto v2 = get<double>(entry2.key)) val2 = *v2, isDouble2 = true;
                
                if (isDouble1 && isDouble2 && std::abs(val1 - val2) < 0.001) {
                    result.addWarning("Multiple rate parameters have similar values: " + entry1.key + " and " + entry2.key);
                }
            }
        }
    }
    
    // Check for dependent key relationships
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find("threshold") != std::string::npos) {
            if (auto threshold = get<double>(entry.key)) {
                // Check if there are related parameters
                bool foundRelated = false;
                for (const auto& other : pImpl->entries) {
                    if (other.key.find("rate") != std::string::npos && other.key != entry.key) {
                        foundRelated = true;
                        break;
                    }
                }
                if (!foundRelated) {
                    result.addWarning("Threshold parameter set without related rate parameters");
                }
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Record performance metric
    recordValidationPerformance("cross_validation", result.validationTime.count());
    
    return result;
}

ValidationResult Config::validateIntegration() const {
    ValidationResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Integration validation checks
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find("neurons") != std::string::npos) {
            if (auto neurons = get<int>(entry.key)) {
                if (*neurons < 10) {
                    result.addWarning("Low neuron count may affect system stability");
                }
            }
        }
        
        if (entry.key.find("plasticity") != std::string::npos) {
            if (auto plasticity = get<double>(entry.key)) {
                if (*plasticity > 1.0) {
                    result.addWarning("High plasticity may cause unstable learning");
                }
            }
        }
    }
    
    // Check for required configuration completeness
    std::set<std::string> essentialKeys = {"learning_rate", "threshold", "neurons"};
    bool allEssentialPresent = true;
    for (const auto& key : essentialKeys) {
        if (!has(key)) {
            allEssentialPresent = false;
            result.addError(key, "Essential configuration key is missing for system integration", ConfigSource::Default);
        }
    }
    
    if (!allEssentialPresent) {
        result.addWarning("System integration may be compromised due to missing essential parameters");
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Record performance metric
    recordValidationPerformance("integration_validation", result.validationTime.count());
    
    return result;
}

void Config::enableHealthMonitoring() {
    if (!pImpl->healthMonitor) {
        pImpl->healthMonitor = std::make_unique<ConfigHealthMonitor>();
    }
}

void Config::disableHealthMonitoring() {
    pImpl->healthMonitor.reset();
}

bool Config::isHealthMonitoringEnabled() const {
    return pImpl->healthMonitor != nullptr && !pImpl->healthMonitor->getMetrics().empty();
}

ValidationResult Config::validateHealth() const {
    ValidationResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Check health monitor if available
    if (pImpl->healthMonitor) {
        ValidationResult healthCheck = pImpl->healthMonitor->checkHealth();
        if (!healthCheck.isValid) {
            for (const auto& error : healthCheck.errors) {
                result.addError(error.key, error.errorMessage, error.source);
            }
        }
        for (const auto& warning : healthCheck.warnings) {
            result.addWarning(warning);
        }
    }
    
    // Basic health checks
    if (pImpl->entries.empty()) {
        result.addError("config", "Configuration has no entries");
    }
    
    // Check for suspicious values
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find("threshold") != std::string::npos) {
            if (auto val = get<double>(entry.key)) {
                if (*val < 0.0 || *val > 2.0) {
                    result.addWarning("Threshold value " + std::to_string(*val) + " may be outside normal range");
                }
            }
        }
        
        if (entry.key.find("rate") != std::string::npos) {
            if (auto val = get<double>(entry.key)) {
                if (*val > 10.0) {
                    result.addWarning("High rate value " + std::to_string(*val) + " may cause instability");
                }
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Record performance metric
    recordValidationPerformance("health_check", result.validationTime.count());
    
    return result;
}

ValidationResult Config::validateAgainstSchema(const ConfigSchema& schema) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    ValidationResult result;
    
    // Validate against provided schema
    result = schema.validate(*this);
    
    // If schema validation fails but we have a default schema, try that as well
    if (!result.isValid) {
        ValidationResult defaultSchemaValidation = validateAgainstSchema(ConfigSchema::getDefaultSchema());
        if (!defaultSchemaValidation.isValid) {
            for (const auto& error : defaultSchemaValidation.errors) {
                result.addError(error.key, error.errorMessage, error.source);
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Record performance metric
    recordValidationPerformance("schema_validation", result.validationTime.count());
    
    return result;
}

bool Config::enableHealthMonitoring() {
    if (!pImpl->healthMonitor) {
        pImpl->healthMonitor = std::make_unique<ConfigHealthMonitor>();
    }
    return pImpl->healthMonitor != nullptr;
}

void Config::disableHealthMonitoring() {
    pImpl->healthMonitor.reset();
}

bool Config::isHealthMonitoringEnabled() const {
    return pImpl->healthMonitor != nullptr && !pImpl->healthMonitor->getMetrics().empty();
}

// ============= CONFIG IMPLEMENTATION - REMAINING ORIGINAL METHODS =============

std::vector<ConfigValidationError> Config::validateConfig() const {
    std::vector<ConfigValidationError> errors;
    
    // Basic validation rules
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            errors.emplace_back(entry.key, "Empty key", entry.source);
            continue;
        }
        
        // Validate value type based on key patterns
        if (entry.key.find("threshold") != std::string::npos) {
            if (auto val = get<double>(entry.key)) {
                if (*val < 0.0 || *val > 1.0) {
                    errors.emplace_back(entry.key, "Value must be between 0.0 and 1.0", entry.source);
                }
            }
        }
        
        if (entry.key.find("count") != std::string::npos || entry.key.find("max") != std::string::npos) {
            if (auto val = get<int>(entry.key)) {
                if (*val < 0) {
                    errors.emplace_back(entry.key, "Count/max must be non-negative", entry.source);
                }
            }
        }
        
        if (entry.key.find("rate") != std::string::npos || entry.key.find("level") != std::string::npos) {
            if (auto val = get<double>(entry.key)) {
                if (*val < 0.0) {
                    errors.emplace_back(entry.key, "Rate/level must be non-negative", entry.source);
                }
            }
        }
    }
    
    return errors;
}

void Config::resetConfig() {
    // Clear all entries
    pImpl->entries.clear();
    
    // TODO: Restore default values from built-in defaults
    // For now, just clear everything
    // In a real implementation, this would restore default values
    // from a built-in configuration schema
}

template<typename T>
std::vector<std::pair<std::string, T>> Config::getAll(const std::string& keyPattern) const {
    std::vector<std::pair<std::string, T>> result;
    
    for (const auto& entry : pImpl->entries) {
        if (keyPattern == "*" || keyMatchesPattern(entry.key, keyPattern)) {
            if (auto val = get<T>(entry.key)) {
                result.emplace_back(entry.key, *val);
            }
        }
    }
    
    return result;
}

bool Config::isComplete(const std::set<std::string>& requiredKeys) const {
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            return false;
        }
    }
    return true;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Only merge entries from lower priority sources or same source
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it == pImpl->entries.end() || static_cast<int>(it->source) < static_cast<int>(entry.source)) {
            pImpl->entries.push_back(entry);
        }
    }
}

std::map<std::string, size_t> Config::getStatistics() const {
    std::map<std::string, size_t> stats;
    
    for (const auto& entry : pImpl->entries) {
        std::string sourceStr;
        switch (entry.source) {
            case ConfigSource::Default: sourceStr = "Default"; break;
            case ConfigSource::File: sourceStr = "File"; break;
            case ConfigSource::CommandLine: sourceStr = "CommandLine"; break;
            case ConfigSource::Runtime: sourceStr = "Runtime"; break;
        }
        stats[sourceStr]++;
    }
    
    return stats;
}

// ============= CONFIG IMPLEMENTATION - INTERNAL HELPERS =============

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

bool Config::keyMatchesPattern(const std::string& key, const std::string& pattern) {
    if (pattern == "*") return true;
    
    // Simple wildcard matching for now
    if (pattern.find('*') != std::string::npos) {
        std::regex regex(pattern);
        return std::regex_match(key, regex);
    }
    
    return key == pattern;
}

} // namespace nlm
