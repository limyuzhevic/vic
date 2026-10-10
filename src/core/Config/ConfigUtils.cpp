#include "ConfigUtils.hpp"
#include "Config.hpp"
#include "ConfigValidator.hpp"
#include "ConfigPresets.hpp"
#include "ConfigSchema.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {

// ConfigComparator implementation

bool ConfigComparator::configsEqual(const Config& config1, const Config& config2) {
    if (config1.getKeys().size() != config2.getKeys().size()) {
        return false;
    }
    
    for (const auto& key : config1.getKeys()) {
        if (!config2.has(key)) {
            return false;
        }
        
        auto val1 = config1.get(key);
        auto val2 = config2.get(key);
        
        if (!val1.has_value() || !val2.has_value()) {
            return false;
        }
        
        if (!variantsEqual(val1.value(), val2.value())) {
            return false;
        }
    }
    
    return true;
}

bool ConfigComparator::configsEqual(const Config& config1, const Config& config2, 
                                   double tolerance) {
    if (config1.getKeys().size() != config2.getKeys().size()) {
        return false;
    }
    
    for (const auto& key : config1.getKeys()) {
        if (!config2.has(key)) {
            return false;
        }
        
        auto val1 = config1.get(key);
        auto val2 = config2.get(key);
        
        if (!val1.has_value() || !val2.has_value()) {
            return false;
        }
        
        if (!variantsEqualWithTolerance(val1.value(), val2.value(), tolerance)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ConfigComparator::getAddedKeys(const Config& config1, const Config& config2) {
    std::vector<std::string> addedKeys;
    
    for (const auto& key : config2.getKeys()) {
        if (!config1.has(key)) {
            addedKeys.push_back(key);
        }
    }
    
    return addedKeys;
}

std::vector<std::string> ConfigComparator::getRemovedKeys(const Config& config1, const Config& config2) {
    std::vector<std::string> removedKeys;
    
    for (const auto& key : config1.getKeys()) {
        if (!config2.has(key)) {
            removedKeys.push_back(key);
        }
    }
    
    return removedKeys;
}

std::vector<std::string> ConfigComparator::getChangedKeys(const Config& config1, const Config& config2, 
                                                         double tolerance) {
    std::vector<std::string> changedKeys;
    
    for (const auto& key : config1.getKeys()) {
        if (config2.has(key)) {
            auto val1 = config1.get(key);
            auto val2 = config2.get(key);
            
            if (val1.has_value() && val2.has_value()) {
                if (!variantsEqualWithTolerance(val1.value(), val2.value(), tolerance)) {
                    changedKeys.push_back(key);
                }
            }
        }
    }
    
    return changedKeys;
}

ConfigComparator::DifferenceReport ConfigComparator::getDifferenceReport(const Config& config1, 
                                                                          const Config& config2, 
                                                                          double tolerance) {
    DifferenceReport report;
    report.addedKeys = getAddedKeys(config1, config2);
    report.removedKeys = getRemovedKeys(config1, config2);
    report.changedKeys = getChangedKeys(config1, config2, tolerance);
    
    return report;
}

std::string ConfigComparator::DifferenceReport::toString() const {
    std::ostringstream oss;
    oss << "=== Configuration Differences ===" << std::endl;
    
    if (!addedKeys.empty()) {
        oss << "Added keys:" << std::endl;
        for (const auto& key : addedKeys) {
            oss << "  + " << key << std::endl;
        }
    }
    
    if (!removedKeys.empty()) {
        oss << "Removed keys:" << std::endl;
        for (const auto& key : removedKeys) {
            oss << "  - " << key << std::endl;
        }
    }
    
    if (!changedKeys.empty()) {
        oss << "Changed keys:" << std::endl;
        for (const auto& key : changedKeys) {
            oss << "  ~ " << key << std::endl;
        }
    }
    
    return oss.str();
}

// ConfigMerger implementation

Config ConfigMerger::mergeConfigs(const Config& config1, const Config& config2) {
    Config result;
    
    // Copy config1
    for (const auto& key : config1.getKeys()) {
        auto value = config1.get(key);
        if (value.has_value()) {
            std::visit([&](auto&& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, int>) {
                    result.set(key, val, ConfigSource::Default);
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    result.set(key, val, ConfigSource::Default);
                } else if constexpr (std::is_same_v<T, double>) {
                    result.set(key, val, ConfigSource::Default);
                } else if constexpr (std::is_same_v<T, bool>) {
                    result.set(key, val, ConfigSource::Default);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    result.set(key, val, ConfigSource::Default);
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    result.set(key, val, ConfigSource::Default);
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    result.set(key, val, ConfigSource::Default);
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    result.set(key, val, ConfigSource::Default);
                }
            }, *value);
        }
    }
    
    // Override with config2
    for (const auto& key : config2.getKeys()) {
        auto value = config2.get(key);
        if (value.has_value()) {
            std::visit([&](auto&& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, int>) {
                    result.set(key, val, ConfigSource::File);
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    result.set(key, val, ConfigSource::File);
                } else if constexpr (std::is_same_v<T, double>) {
                    result.set(key, val, ConfigSource::File);
                } else if constexpr (std::is_same_v<T, bool>) {
                    result.set(key, val, ConfigSource::File);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    result.set(key, val, ConfigSource::File);
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    result.set(key, val, ConfigSource::File);
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    result.set(key, val, ConfigSource::File);
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    result.set(key, val, ConfigSource::File);
                }
            }, *value);
        }
    }
    
    return result;
}

Config ConfigMerger::mergeConfigs(const Config& config1, const Config& config2,
                                  ConfigSource preference) {
    if (preference == ConfigSource::File) {
        return mergeConfigs(config2, config1);
    }
    return mergeConfigs(config1, config2);
}

void ConfigMerger::applyConfig(Config& target, const Config& source,
                               ConfigSource sourceType) {
    for (const auto& key : source.getKeys()) {
        auto value = source.get(key);
        if (value.has_value()) {
            std::visit([&](auto&& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, int>) {
                    target.set(key, val, sourceType);
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    target.set(key, val, sourceType);
                } else if constexpr (std::is_same_v<T, double>) {
                    target.set(key, val, sourceType);
                } else if constexpr (std::is_same_v<T, bool>) {
                    target.set(key, val, sourceType);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    target.set(key, val, sourceType);
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    target.set(key, val, sourceType);
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    target.set(key, val, sourceType);
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    target.set(key, val, sourceType);
                }
            }, *value);
        }
    }
}

Config ConfigMerger::mergeConfigs(const std::vector<Config>& configs) {
    if (configs.empty()) {
        return Config();
    }
    
    Config result = configs[0];
    for (size_t i = 1; i < configs.size(); ++i) {
        result = mergeConfigs(result, configs[i]);
    }
    
    return result;
}

Config ConfigMerger::createWithDefaults(const Config& defaults, const Config& overrides) {
    Config result = defaults;
    applyConfig(result, overrides, ConfigSource::CommandLine);
    return result;
}

Config ConfigMerger::createWithSchemaDefaults(const ConfigSchema& schema, const Config& overrides) {
    Config result;
    std::map<std::string, ConfigValue> configMap = ConfigSchema::createConfigFromSchema(schema);
    
    // Convert map to config
    for (const auto& pair : configMap) {
        std::visit([&](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                result.set(pair.first, val, ConfigSource::Default);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                result.set(pair.first, val, ConfigSource::Default);
            } else if constexpr (std::is_same_v<T, double>) {
                result.set(pair.first, val, ConfigSource::Default);
            } else if constexpr (std::is_same_v<T, bool>) {
                result.set(pair.first, val, ConfigSource::Default);
            } else if constexpr (std::is_same_v<T, std::string>) {
                result.set(pair.first, val, ConfigSource::Default);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                result.set(pair.first, val, ConfigSource::Default);
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                result.set(pair.first, val, ConfigSource::Default);
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                result.set(pair.first, val, ConfigSource::Default);
            }
        }, pair.second);
    }
    
    // Apply overrides
    applyConfig(result, overrides, ConfigSource::CommandLine);
    
    return result;
}

// ConfigDiffer implementation

std::string ConfigDiffer::generateDiff(const Config& config1, const Config& config2) {
    ConfigComparator::DifferenceReport report = ConfigComparator::getDifferenceReport(config1, config2);
    return report.toString();
}

std::string ConfigDiffer::generateHumanReadableDiff(const Config& config1, const Config& config2) {
    std::ostringstream oss;
    oss << "=== Configuration Comparison ===" << std::endl;
    
    auto report = ConfigComparator::getDifferenceReport(config1, config2);
    
    if (report.addedKeys.empty() && report.removedKeys.empty() && report.changedKeys.empty()) {
        oss << "Configurations are identical." << std::endl;
        return oss.str();
    }
    
    oss << "Keys in config2 but not config1:" << std::endl;
    for (const auto& key : report.addedKeys) {
        auto value = config2.get(key);
        if (value.has_value()) {
            oss << "  + " << key << " = " << valueToString(*value) << std::endl;
        }
    }
    
    oss << "Keys in config1 but not config2:" << std::endl;
    for (const auto& key : report.removedKeys) {
        auto value = config1.get(key);
        if (value.has_value()) {
            oss << "  - " << key << " = " << valueToString(*value) << std::endl;
        }
    }
    
    oss << "Keys with different values:" << std::endl;
    for (const auto& key : report.changedKeys) {
        auto val1 = config1.get(key);
        auto val2 = config2.get(key);
        if (val1.has_value() && val2.has_value()) {
            oss << "  ~ " << key << ": " << valueToString(*val1) 
                << " -> " << valueToString(*val2) << std::endl;
        }
    }
    
    return oss.str();
}

std::string ConfigDiffer::generateJsonDiff(const Config& config1, const Config& config2) {
    // Simple JSON diff implementation
    std::ostringstream oss;
    oss << "{\n  \"added\": [";
    bool first = true;
    for (const auto& key : ConfigComparator::getAddedKeys(config1, config2)) {
        if (!first) oss << ", ";
        oss << "\"" << key << "\"";
        first = false;
    }
    oss << "],\n  \"removed\": [";
    first = true;
    for (const auto& key : ConfigComparator::getRemovedKeys(config1, config2)) {
        if (!first) oss << ", ";
        oss << "\"" << key << "\"";
        first = false;
    }
    oss << "],\n  \"changed\": [";
    first = true;
    for (const auto& key : ConfigComparator::getChangedKeys(config1, config2)) {
        if (!first) oss << ", ";
        oss << "\"" << key << "\"";
        first = false;
    }
    oss << "]\n}";
    
    return oss.str();
}

std::string ConfigDiffer::generateUnifiedDiff(const Config& config1, const Config& config2) {
    std::ostringstream oss;
    oss << "--- config1\n+++ config2\n";
    
    auto report = ConfigComparator::getDifferenceReport(config1, config2);
    
    // Get all unique keys
    std::set<std::string> allKeys;
    for (const auto& key : config1.getKeys()) allKeys.insert(key);
    for (const auto& key : config2.getKeys()) allKeys.insert(key);
    
    bool inHeader = true;
    for (const auto& key : allKeys) {
        if (report.addedKeys.end() == std::find(report.addedKeys.begin(), 
                                               report.addedKeys.end(), key)) {
            if (report.removedKeys.end() != std::find(report.removedKeys.begin(), 
                                                     report.removedKeys.end(), key)) {
                continue; // key was removed, won't appear in config2
            }
            
            if (report.changedKeys.end() != std::find(report.changedKeys.begin(), 
                                                     report.changedKeys.end(), key)) {
                oss << "@@ -" << getKeyLineNumber(config1, key) << ",+" 
                    << getKeyLineNumber(config2, key) << " @@\n";
                
                auto val1 = config1.get(key);
                auto val2 = config2.get(key);
                if (val1.has_value()) {
                    oss << "- " << key << " = " << valueToString(*val1) << "\n";
                }
                if (val2.has_value()) {
                    oss << "+ " << key << " = " << valueToString(*val2) << "\n";
                }
            } else {
                auto val1 = config1.get(key);
                if (val1.has_value()) {
                    oss << " " << key << " = " << valueToString(*val1) << "\n";
                }
            }
        }
    }
    
    return oss.str();
}

bool ConfigDiffer::areCompatible(const Config& config1, const Config& config2) {
    return getCompatibilityIssues(config1, config2).empty();
}

std::vector<std::string> ConfigDiffer::getCompatibilityIssues(const Config& config1, const Config& config2) {
    std::vector<std::string> issues;
    
    // Check for type mismatches
    for (const auto& key : config1.getKeys()) {
        if (config2.has(key)) {
            auto val1 = config1.get(key);
            auto val2 = config2.get(key);
            
            if (val1.has_value() && val2.has_value()) {
                ConfigType type1 = getValueType(*val1);
                ConfigType type2 = getValueType(*val2);
                
                if (type1 != type2) {
                    issues.push_back("Type mismatch for key '" + key + "': " +
                                   typeToString(type1) + " != " + typeToString(type2));
                }
            }
        }
    }
    
    return issues;
}

// ConfigValidatorUtils implementation

bool ConfigValidatorUtils::validateAgainstSchema(const Config& config, const ConfigSchema& schema) {
    return schema.validate(config);
}

std::vector<std::string> ConfigValidatorUtils::getValidationErrors(const Config& config, 
                                                                   const ConfigSchema& schema) {
    std::vector<std::string> errors;
    
    // Check required keys
    auto schemaEntries = schema.getAllEntries();
    for (const auto& entry : schemaEntries) {
        if (entry.required && !config.has(entry.name)) {
            errors.push_back("Missing required configuration key: " + entry.name);
        }
    }
    
    // Check schema validation
    std::map<std::string, ConfigValue> configMap;
    for (const auto& key : config.getKeys()) {
        auto value = config.get(key);
        if (value.has_value()) {
            configMap[key] = *value;
        }
    }
    
    if (!schema.validate(configMap)) {
        errors.push_back("Configuration validation failed against schema");
    }
    
    return errors;
}

std::vector<std::string> ConfigValidatorUtils::getValidationWarnings(const Config& config, 
                                                                     const ConfigSchema& schema) {
    std::vector<std::string> warnings;
    
    // Check for potential issues
    for (const auto& key : config.getKeys()) {
        auto value = config.get(key);
        if (!value.has_value()) continue;
        
        // Check for extreme values
        if (std::holds_alternative<double>(*value)) {
            double val = std::get<double>(*value);
            if (std::abs(val) > 1e6) {
                warnings.push_back("Extremely large value for key '" + key + "': " + 
                                 std::to_string(val));
            }
        }
        
        if (std::holds_alternative<int64_t>(*value)) {
            int64_t val = std::get<int64_t>(*value);
            if (std::abs(val) > 1e6) {
                warnings.push_back("Extremely large value for key '" + key + "': " + 
                                 std::to_string(val));
            }
        }
    }
    
    return warnings;
}

Config ConfigValidatorUtils::sanitizeConfig(const Config& config, const ConfigSchema& schema) {
    Config sanitized = config;
    
    // Apply sanitization logic
    for (const auto& key : sanitized.getKeys()) {
        auto value = sanitized.get(key);
        if (!value.has_value()) continue;
        
        // Sanitize based on schema
        if (schema.hasEntry(key)) {
            const auto& entry = schema.getEntry(key);
            sanitized.set(key, sanitizeValue(*value, entry), ConfigSource::Runtime);
        }
    }
    
    return sanitized;
}

double ConfigValidatorUtils::getConfigHealthScore(const Config& config, const ConfigSchema& schema) {
    double score = 0.0;
    double maxScore = 0.0;
    
    // Schema compliance (0-1)
    std::map<std::string, ConfigValue> configMap;
    for (const auto& key : config.getKeys()) {
        auto value = config.get(key);
        if (value.has_value()) {
            configMap[key] = *value;
        }
    }
    
    if (schema.validate(configMap)) {
        score += 1.0;
    }
    maxScore += 1.0;
    
    // Completeness (0-1)
    auto schemaEntries = schema.getAllEntries();
    int providedCount = 0;
    for (const auto& entry : schemaEntries) {
        if (config.has(entry.name)) providedCount++;
    }
    
    if (!schemaEntries.empty()) {
        score += static_cast<double>(providedCount) / schemaEntries.size();
        maxScore += 1.0;
    }
    
    return score / maxScore;
}

ConfigValidatorUtils::ConfigIssues ConfigValidatorUtils::getConfigIssues(const Config& config, 
                                                                           const ConfigSchema& schema) {
    ConfigIssues issues;
    
    // Check for missing required keys
    auto schemaEntries = schema.getAllEntries();
    for (const auto& entry : schemaEntries) {
        if (entry.required && !config.has(entry.name)) {
            issues.missingRequired.push_back("Missing required configuration key: " + entry.name);
        }
    }
    
    // Check for invalid values
    std::map<std::string, ConfigValue> configMap;
    for (const auto& key : config.getKeys()) {
        auto value = config.get(key);
        if (value.has_value()) {
            configMap[key] = *value;
            
            if (!schema.hasEntry(key) || !schema.validate(key, *value)) {
                issues.invalidValues.push_back("Invalid value for key '" + key + "': " + 
                                             valueToString(*value));
            }
        }
    }
    
    return issues;
}

std::string ConfigValidatorUtils::ConfigIssues::toString() const {
    std::ostringstream oss;
    
    if (!missingRequired.empty()) {
        oss << "Missing required keys:" << std::endl;
        for (const auto& issue : missingRequired) {
            oss << "  - " << issue << std::endl;
        }
    }
    
    if (!invalidValues.empty()) {
        oss << "Invalid values:" << std::endl;
        for (const auto& issue : invalidValues) {
            oss << "  - " << issue << std::endl;
        }
    }
    
    if (!potentialIssues.empty()) {
        oss << "Potential issues:" << std::endl;
        for (const auto& issue : potentialIssues) {
            oss << "  - " << issue << std::endl;
        }
    }
    
    return oss.str();
}

// Helper functions implementation

std::string ConfigDiffer::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else {
            oss << val;
        }
    }, value);
    return oss.str();
}

ConfigType ConfigDiffer::getValueType(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return ConfigType::Integer;
    if (std::holds_alternative<int64_t>(value)) return ConfigType::Integer64;
    if (std::holds_alternative<double>(value)) return ConfigType::Float;
    if (std::holds_alternative<bool>(value)) return ConfigType::Boolean;
    if (std::holds_alternative<std::string>(value)) return ConfigType::String;
    if (std::holds_alternative<std::vector<int>>(value)) return ConfigType::IntVector;
    if (std::holds_alternative<std::vector<double>>(value)) return ConfigType::FloatVector;
    if (std::holds_alternative<std::vector<std::string>>(value)) return ConfigType::StringVector;
    return ConfigType::Unknown;
}

std::string ConfigDiffer::typeToString(ConfigType type) {
    switch (type) {
        case ConfigType::Integer: return "int";
        case ConfigType::Integer64: return "int64_t";
        case ConfigType::Float: return "float/double";
        case ConfigType::Boolean: return "bool";
        case ConfigType::String: return "string";
        case ConfigType::IntVector: return "vector<int>";
        case ConfigType::FloatVector: return "vector<double>";
        case ConfigType::StringVector: return "vector<string>";
        default: return "unknown";
    }
}

bool ConfigDiffer::variantsEqual(const ConfigValue& val1, const ConfigValue& val2) {
    if (val1.index() != val2.index()) {
        return false;
    }
    
    return std::visit([](auto&& v1, auto&& v2) {
        using T = std::decay_t<decltype(v1)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return v1 == v2;
        } else if constexpr (std::is_same_v<T, bool>) {
            return v1 == v2;
        } else if constexpr (std::is_same_v<T, double>) {
            return std::abs(v1 - v2) < 1e-9;
        } else {
            return v1 == v2;
        }
    }, val1, val2);
}

bool ConfigDiffer::variantsEqualWithTolerance(const ConfigValue& val1, const ConfigValue& val2, double tolerance) {
    if (val1.index() != val2.index()) {
        return false;
    }
    
    return std::visit([tolerance](auto&& v1, auto&& v2) {
        using T = std::decay_t<decltype(v1)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return v1 == v2;
        } else if constexpr (std::is_same_v<T, bool>) {
            return v1 == v2;
        } else if constexpr (std::is_same_v<T, double>) {
            return std::abs(v1 - v2) <= tolerance;
        } else {
            return v1 == v2;
        }
    }, val1, val2);
}

ConfigValue ConfigValidatorUtils::sanitizeValue(const ConfigValue& value, const ConfigSchemaEntry& entry) {
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, double>) {
            if (entry.range.first != 0.0 || entry.range.second != 0.0) {
                if (val < entry.range.first) val = entry.range.first;
                if (val > entry.range.second) val = entry.range.second;
            }
        }
    }, value);
    
    return value;
}

int ConfigDiffer::getKeyLineNumber(const Config& config, const std::string& key) {
    // Simple implementation - return key's hash as line number
    std::hash<std::string> hasher;
    return static_cast<int>(hasher(key) % 1000) + 1;
}

} // namespace nlm