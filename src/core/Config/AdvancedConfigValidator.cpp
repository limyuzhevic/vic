// Advanced configuration validation implementation
#include "AdvancedConfigValidator.hpp"
#include "Logger.hpp"
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct ConfigValidator::Impl {
    std::vector<ConfigSchemaField> schema;
    std::vector<std::tuple<std::string, std::string, std::function<bool(const ConfigValue&, const ConfigValue&)>, std::string>> crossFieldValidations;
    std::unordered_map<std::string, std::string> fieldDescriptions;
    
    Impl() = default;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}
ConfigValidator::~ConfigValidator() = default;

void ConfigValidator::addField(const ConfigSchemaField& field) {
    pImpl->schema.push_back(field);
    pImpl->fieldDescriptions[field.name] = field.description;
}

void ConfigValidator::addCrossFieldValidation(const std::string& field1, const std::string& field2,
                                            std::function<bool(const ConfigValue&, const ConfigValue&)> validator,
                                            const std::string& errorMessage) {
    pImpl->crossFieldValidations.emplace_back(field1, field2, validator, errorMessage);
}

std::vector<ConfigValidationError> ConfigValidator::validate(const Config& config) const {
    std::vector<ConfigValidationError> errors;
    
    // Check schema requirements
    for (const auto& field : pImpl->schema) {
        if (field.required && !config.has(field.name)) {
            errors.emplace_back(ConfigValidationError::ErrorType::MissingRequiredField,
                              field.name, "Required field is missing",
                              "Add the " + field.name + " field with an appropriate value");
        } else if (config.has(field.name)) {
            auto value = config.get<ConfigValue>(field.name);
            if (value) {
                auto fieldErrors = validateValue(field, *value);
                errors.insert(errors.end(), fieldErrors.begin(), fieldErrors.end());
            }
        }
    }
    
    // Check cross-field validations
    for (const auto& [field1, field2, validator, errorMessage] : pImpl->crossFieldValidations) {
        if (config.has(field1) && config.has(field2)) {
            auto value1 = config.get<ConfigValue>(field1);
            auto value2 = config.get<ConfigValue>(field2);
            if (value1 && value2) {
                if (!validator(*value1, *value2)) {
                    errors.emplace_back(ConfigValidationError::ErrorType::InvalidValue,
                                      field1 + " or " + field2, errorMessage);
                }
            }
        }
    }
    
    return errors;
}

std::vector<ConfigValidationError> ConfigValidator::validateFile(const std::string& filepath) const {
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(filepath)) {
        return {ConfigValidationError(ConfigValidationError::ErrorType::SchemaMismatch,
                                     "file", "Failed to load configuration file: " + filepath)};
    }
    return validate(*config);
}

std::string ConfigValidator::getValidationSummary(const std::vector<ConfigValidationError>& errors) const {
    std::ostringstream oss;
    
    if (errors.empty()) {
        oss << "Configuration validation passed!" << std::endl;
        return oss.str();
    }
    
    oss << "Configuration validation failed with " << errors.size() << " error(s):" << std::endl;
    oss << std::endl;
    
    for (size_t i = 0; i < errors.size(); ++i) {
        const auto& error = errors[i];
        oss << "Error " << (i + 1) << ": " << error.field << std::endl;
        oss << "  Type: " << static_cast<int>(error.type) << std::endl;
        oss << "  Message: " << error.message << std::endl;
        if (!error.suggestion.empty()) {
            oss << "  Suggestion: " << error.suggestion << std::endl;
        }
        oss << std::endl;
    }
    
    return oss.str();
}

std::string ConfigValidator::getFieldHelp(const std::string& fieldName) const {
    auto it = pImpl->fieldDescriptions.find(fieldName);
    if (it != pImpl->fieldDescriptions.end()) {
        return it->second;
    }
    return "No description available for field: " + fieldName;
}

bool ConfigValidator::isValid(const Config& config) const {
    return validate(config).empty();
}

std::vector<ConfigValidationError> ConfigValidator::validateValue(const ConfigSchemaField& field,
                                                                  const ConfigValue& value) const {
    std::vector<ConfigValidationError> errors;
    
    // Type validation
    std::string actualType = typeToString(value);
    if (actualType != field.type) {
        errors.emplace_back(ConfigValidationError::ErrorType::InvalidType,
                          field.name, "Expected type " + field.type + ", got " + actualType,
                          "Convert the value to type " + field.type);
    }
    
    // Range validation for numeric types
    if ((field.type == "int" || field.type == "double") && (field.minValue || field.maxValue)) {
        double val = 0.0;
        if (std::holds_alternative<int>(value)) {
            val = static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            val = std::get<double>(value);
        }
        
        if (field.minValue && val < *field.minValue) {
            errors.emplace_back(ConfigValidationError::ErrorType::OutOfRange,
                              field.name, "Value " + std::to_string(val) + " is less than minimum " + std::to_string(*field.minValue),
                              "Increase the value to at least " + std::to_string(*field.minValue));
        }
        
        if (field.maxValue && val > *field.maxValue) {
            errors.emplace_back(ConfigValidationError::ErrorType::OutOfRange,
                              field.name, "Value " + std::to_string(val) + " is greater than maximum " + std::to_string(*field.maxValue),
                              "Decrease the value to at most " + std::to_string(*field.maxValue));
        }
    }
    
    // Allowed values validation
    if (!field.allowedValues.empty()) {
        bool valueInAllowed = false;
        if (field.type == "string" && std::holds_alternative<std::string>(value)) {
            const std::string& val = std::get<std::string>(value);
            valueInAllowed = std::find(field.allowedValues.begin(), field.allowedValues.end(), val) != field.allowedValues.end();
        } else if (field.type == "int" && std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            valueInAllowed = std::any_of(field.allowedValues.begin(), field.allowedValues.end(),
                                        [val](const std::string& s) { return std::stoi(s) == val; });
        }
        
        if (!valueInAllowed) {
            std::string allowedList = "";
            for (size_t i = 0; i < field.allowedValues.size(); ++i) {
                if (i > 0) allowedList += ", ";
                allowedList += field.allowedValues[i];
            }
            errors.emplace_back(ConfigValidationError::ErrorType::InvalidValue,
                              field.name, "Value must be one of: " + allowedList,
                              "Choose one of the allowed values");
        }
    }
    
    // Custom validator
    if (field.validator && !field.validator(value)) {
        errors.emplace_back(ConfigValidationError::ErrorType::InvalidValue,
                          field.name, "Value does not pass custom validation",
                          "Adjust the value to pass validation");
    }
    
    return errors;
}

bool ConfigValidator::compareTypes(const std::string& type1, const std::string& type2) const {
    return type1 == type2;
}

std::string ConfigValidator::typeToString(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "array<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "array<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "array<string>";
    return "unknown";
}

// AdvancedCLIOptionParser implementation
struct AdvancedCLIOptionParser::Impl {
    struct Option {
        std::string longName;
        std::string shortName;
        std::string description;
        std::string type;
        std::string defaultValue;
        std::function<bool(const std::string&)> validator;
    };
    
    struct PositionalArgument {
        std::string name;
        std::string description;
        std::string type;
    };
    
    std::vector<Option> options;
    std::vector<PositionalArgument> positionalArgs;
    std::vector<std::string> errors;
    
    Impl() = default;
};

AdvancedCLIOptionParser::AdvancedCLIOptionParser() : pImpl(std::make_unique<Impl>()) {}
AdvancedCLIOptionParser::~AdvancedCLIOptionParser() = default;

void AdvancedCLIOptionParser::addOption(const std::string& longName, const std::string& shortName,
                                       const std::string& description, const std::string& type,
                                       const std::string& defaultValue,
                                       std::function<bool(const std::string&)> validator) {
    pImpl->options.emplace_back(Option{longName, shortName, description, type, defaultValue, validator});
}

void AdvancedCLIOptionParser::addPositionalArgument(const std::string& name, const std::string& description,
                                                  const std::string& type) {
    pImpl->positionalArgs.emplace_back(PositionalArgument{name, description, type});
}

bool AdvancedCLIOptionParser::parse(int argc, char** argv, std::unordered_map<std::string, std::string>& parsedArgs) const {
    parsedArgs.clear();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Check for help option
        if (arg == "--help" || arg == "-h") {
            std::cout << getHelp("program_name");
            return false;
        }
        
        // Check for long options
        if (arg.find("--") == 0) {
            std::string optionName = arg.substr(2);
            bool found = false;
            
            for (const auto& option : pImpl->options) {
                if (option.longName == optionName) {
                    found = true;
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        // Option has a value
                        parsedArgs[optionName] = argv[++i];
                        
                        // Validate if validator is provided
                        if (option.validator && !option.validator(parsedArgs[optionName])) {
                            pImpl->errors.push_back("Invalid value for option " + option.longName + ": " + parsedArgs[optionName]);
                            return false;
                        }
                    } else {
                        // Option has no value (flag)
                        parsedArgs[optionName] = "true";
                    }
                    break;
                }
            }
            
            if (!found) {
                pImpl->errors.push_back("Unknown option: " + optionName);
                return false;
            }
        } else if (arg.find("-") == 0 && arg.length() == 2) {
            // Short option
            std::string optionName(1, arg[1]);
            bool found = false;
            
            for (const auto& option : pImpl->options) {
                if (option.shortName == optionName) {
                    found = true;
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        // Option has a value
                        parsedArgs[option.longName] = argv[++i];
                        
                        // Validate if validator is provided
                        if (option.validator && !option.validator(parsedArgs[option.longName])) {
                            pImpl->errors.push_back("Invalid value for option " + option.longName + ": " + parsedArgs[option.longName]);
                            return false;
                        }
                    } else {
                        // Option has no value (flag)
                        parsedArgs[option.longName] = "true";
                    }
                    break;
                }
            }
            
            if (!found) {
                pImpl->errors.push_back("Unknown option: -" + optionName);
                return false;
            }
        } else {
            // Positional argument
            if (pImpl->positionalArgs.empty()) {
                pImpl->errors.push_back("Unexpected argument: " + arg);
                return false;
            }
            
            // For simplicity, assign positional args in order
            static size_t posArgIndex = 0;
            if (posArgIndex < pImpl->positionalArgs.size()) {
                parsedArgs[pImpl->positionalArgs[posArgIndex].name] = arg;
                posArgIndex++;
            }
        }
    }
    
    return true;
}

std::string AdvancedCLIOptionParser::getHelp(const std::string& programName) const {
    std::ostringstream oss;
    
    oss << "Usage: " << programName << " [OPTIONS] [ARGUMENTS]" << std::endl;
    oss << std::endl;
    oss << "Options:" << std::endl;
    
    for (const auto& option : pImpl->options) {
        oss << "  --" << option.longName;
        if (!option.shortName.empty()) {
            oss << ", -" << option.shortName;
        }
        oss << std::endl;
        oss << "    " << option.description << std::endl;
        if (!option.defaultValue.empty()) {
            oss << "    Default: " << option.defaultValue << std::endl;
        }
        oss << std::endl;
    }
    
    if (!pImpl->positionalArgs.empty()) {
        oss << "Arguments:" << std::endl;
        for (const auto& arg : pImpl->positionalArgs) {
            oss << "  " << arg.name << std::endl;
            oss << "    " << arg.description << std::endl;
        }
        oss << std::endl;
    }
    
    oss << "  --help, -h    Show this help message" << std::endl;
    oss << std::endl;
    
    return oss.str();
}

std::string AdvancedCLIOptionParser::getErrorMessages() const {
    if (pImpl->errors.empty()) return "";
    
    std::ostringstream oss;
    oss << "Errors:" << std::endl;
    for (const auto& error : pImpl->errors) {
        oss << "  - " << error << std::endl;
    }
    return oss.str();
}

// ConfigComparator implementation
ConfigComparator::ConfigComparator() = default;
ConfigComparator::~ConfigComparator() = default;

std::vector<ConfigComparator::ConfigDiff> ConfigComparator::compare(const Config& config1, const Config& config2) const {
    std::vector<ConfigDiff> diffs;
    
    // Get keys from both configurations
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    // Check for added/removed keys
    std::unordered_set<std::string> keys1Set(keys1.begin(), keys1.end());
    std::unordered_set<std::string> keys2Set(keys2.begin(), keys2.end());
    
    // Keys in config1 but not in config2
    for (const auto& key : keys1Set) {
        if (keys2Set.find(key) == keys2Set.end()) {
            auto value = config1.get<ConfigValue>(key);
            std::string valueStr = value ? (std::holds_alternative<std::string>(*value) ? std::get<std::string>(*value) : std::to_string(*value)) : "null";
            diffs.emplace_back(key, DiffType::REMOVED, valueStr, "", "Field removed in new configuration");
        }
    }
    
    // Keys in config2 but not in config1
    for (const auto& key : keys2Set) {
        if (keys1Set.find(key) == keys1Set.end()) {
            auto value = config2.get<ConfigValue>(key);
            std::string valueStr = value ? (std::holds_alternative<std::string>(*value) ? std::get<std::string>(*value) : std::to_string(*value)) : "null";
            diffs.emplace_back(key, DiffType::ADDED, "", valueStr, "Field added in new configuration");
        }
    }
    
    // Check for modified keys
    for (const auto& key : keys1Set) {
        if (keys2Set.find(key) != keys2Set.end()) {
            auto value1 = config1.get<ConfigValue>(key);
            auto value2 = config2.get<ConfigValue>(key);
            if (value1 && value2 && value1 != value2) {
                std::string oldStr = std::holds_alternative<std::string>(*value1) ? std::get<std::string>(*value1) : std::to_string(*value1);
                std::string newStr = std::holds_alternative<std::string>(*value2) ? std::get<std::string>(*value2) : std::to_string(*value2);
                diffs.emplace_back(key, DiffType::MODIFIED, oldStr, newStr, "Field value changed");
            }
        }
    }
    
    return diffs;
}

std::string ConfigComparator::getHumanReadableDiff(const std::vector<ConfigDiff>& diffs) const {
    std::ostringstream oss;
    
    if (diffs.empty()) {
        oss << "Configurations are identical." << std::endl;
        return oss.str();
    }
    
    oss << "Differences between configurations:" << std::endl;
    oss << std::endl;
    
    for (const auto& diff : diffs) {
        switch (diff.type) {
            case ConfigDiff::DiffType::ADDED:
                oss << "+ Added " << diff.fieldName << " = " << diff.newValue << std::endl;
                break;
            case ConfigComparator::ConfigDiff::DiffType::REMOVED:
                oss << "- Removed " << diff.fieldName << " (was: " << diff.oldValue << ")" << std::endl;
                break;
            case ConfigComparator::ConfigDiff::DiffType::MODIFIED:
                oss << "* Modified " << diff.fieldName << std::endl;
                oss << "  - Old: " << diff.oldValue << std::endl;
                oss << "  + New: " << diff.newValue << std::endl;
                break;
            case ConfigComparator::ConfigDiff::DiffType::UNCHANGED:
                oss << "  Unchanged " << diff.fieldName << std::endl;
                break;
        }
    }
    
    return oss.str();
}

std::string ConfigComparator::getUnifiedDiff(const std::vector<ConfigDiff>& diffs) const {
    std::ostringstream oss;
    
    if (diffs.empty()) {
        return "--- /dev/null\t2026-01-01 00:00:00.000000000 +0000\n+++ /dev/null\t2026-01-01 00:00:00.000000000 +0000\n@@ -0,0 +0,0 @@\nNo differences\n";
    }
    
    oss << "--- config_original\t2026-01-01 00:00:00.000000000 +0000" << std::endl;
    oss << "+++ config_new\t2026-01-01 00:00:00.000000000 +0000" << std::endl;
    
    for (const auto& diff : diffs) {
        if (diff.type == ConfigDiff::DiffType::ADDED) {
            oss << "+ " << diff.fieldName << " = " << diff.newValue << std::endl;
        } else if (diff.type == ConfigComparator::ConfigDiff::DiffType::REMOVED) {
            oss << "- " << diff.fieldName << " = " << diff.oldValue << std::endl;
        } else if (diff.type == ConfigComparator::ConfigDiff::DiffType::MODIFIED) {
            oss << "- " << diff.fieldName << " = " << diff.oldValue << std::endl;
            oss << "+ " << diff.fieldName << " = " << diff.newValue << std::endl;
        }
    }
    
    return oss.str();
}

// ConfigMigrator implementation
struct ConfigMigrator::Impl {
    struct Migration {
        std::string fromVersion;
        std::string toVersion;
        std::function<Config(Config&)> migrationFunction;
        std::string description;
    };
    
    std::vector<Migration> migrations;
    std::vector<std::string> history;
    
    Impl() = default;
};

ConfigMigrator::ConfigMigrator() : pImpl(std::make_unique<Impl>()) {}
ConfigMigrator::~ConfigMigrator() = default;

void ConfigMigrator::addMigration(const std::string& fromVersion, const std::string& toVersion,
                                  std::function<Config(Config&)> migrationFunction,
                                  const std::string& description) {
    pImpl->migrations.emplace_back(Migration{fromVersion, toVersion, migrationFunction, description});
}

Config ConfigMigrator::applyMigrations(Config config, const std::string& fromVersion, const std::string& toVersion) const {
    std::string currentVersion = fromVersion;
    
    for (const auto& migration : pImpl->migrations) {
        if (migration.fromVersion == currentVersion && migration.toVersion == toVersion) {
            NLM_LOG_INFO("Applying migration from " + currentVersion + " to " + toVersion + ": " + migration.description);
            config = migration.migrationFunction(config);
            currentVersion = toVersion;
            pImpl->history.push_back("Migrated from " + migration.fromVersion + " to " + migration.toVersion);
            break;
        }
    }
    
    return config;
}

std::vector<std::string> ConfigMigrator::getMigrationHistory() const {
    return pImpl->history;
}

// ConfigExporter implementation
ConfigExporter::ConfigExporter() = default;
ConfigExporter::~ConfigExporter() = default;

std::string ConfigExporter::exportToJSON(const Config& config) const {
    json j;
    
    for (const auto& key : config.getKeys()) {
        auto value = config.get<ConfigValue>(key);
        if (value) {
            // Handle different types
            if (std::holds_alternative<int>(*value)) {
                j[key] = std::get<int>(*value);
            } else if (std::holds_alternative<int64_t>(*value)) {
                j[key] = std::get<int64_t>(*value);
            } else if (std::holds_alternative<double>(*value)) {
                j[key] = std::get<double>(*value);
            } else if (std::holds_alternative<bool>(*value)) {
                j[key] = std::get<bool>(*value);
            } else if (std::holds_alternative<std::string>(*value)) {
                j[key] = std::get<std::string>(*value);
            } else {
                j[key] = "unhandled_type";
            }
        }
    }
    
    return j.dump(2);
}

std::string ConfigExporter::exportToHumanReadable(const Config& config) const {
    std::ostringstream oss;
    
    oss << "Configuration Export" << std::endl;
    oss << "===================" << std::endl;
    oss << std::endl;
    
    for (const auto& key : config.getKeys()) {
        auto value = config.get<ConfigValue>(key);
        if (value) {
            oss << key << " = ";
            
            // Format value based on type
            if (std::holds_alternative<int>(*value)) {
                oss << std::get<int>(*value);
            } else if (std::holds_alternative<int64_t>(*value)) {
                oss << std::get<int64_t>(*value);
            } else if (std::holds_alternative<double>(*value)) {
                oss << std::get<double>(*value);
            } else if (std::holds_alternative<bool>(*value)) {
                oss << (std::get<bool>(*value) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(*value)) {
                oss << "\"" << std::get<std::string>(*value) << "\"";
            } else {
                oss << "unhandled_type";
            }
            
            oss << std::endl;
        }
    }
    
    return oss.str();
}

bool ConfigExporter::importFromJSON(const std::string& json, Config& config) const {
    try {
        auto j = json::parse(json);
        config.clear();
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Handle different JSON types
            if (it->is_number_integer()) {
                config.set(key, it->get<int>());
            } else if (it->is_number_float()) {
                config.set(key, it->get<double>());
            } else if (it->is_boolean()) {
                config.set(key, it->get<bool>());
            } else if (it->is_string()) {
                config.set(key, it->get<std::string>());
            } else {
                config.set(key, std::string("unhandled_type"));
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to import from JSON: " + std::string(e.what()));
        return false;
    }
}

bool ConfigExporter::importFromHumanReadable(const std::string& text, Config& config) const {
    std::istringstream iss(text);
    std::string line;
    
    config.clear();
    
    while (std::getline(iss, line)) {
        // Skip empty lines and comments
        line.erase(0, line.find_first_not_of(" \t"));
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse key=value format
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            config.set(key, value);
        }
    }
    
    return true;
}

// ConfigStats implementation
ConfigStats::ConfigStats() = default;
ConfigStats::~ConfigStats() = default;

ConfigStats::ConfigStatistics ConfigStats::computeStats(const Config& config) const {
    ConfigStatistics stats;
    
    stats.totalFields = config.getKeys().size();
    
    double totalValueLength = 0.0;
    size_t totalNumericFields = 0;
    
    for (const auto& key : config.getKeys()) {
        auto value = config.get<ConfigValue>(key);
        if (value) {
            // Count field types
            if (std::holds_alternative<std::string>(*value)) {
                stats.stringFields++;
            } else if (std::holds_alternative<int>(*value) || 
                      std::holds_alternative<int64_t>(*value) ||
                      std::holds_alternative<double>(*value)) {
                totalNumericFields++;
            } else if (std::holds_alternative<bool>(*value)) {
                // Boolean field
            } else {
                // Array field or other
            }
            
            // Calculate value length
            if (std::holds_alternative<std::string>(*value)) {
                totalValueLength += std::get<std::string>(*value).size();
            } else if (std::holds_alternative<int>(*value)) {
                totalValueLength += std::to_string(std::get<int>(*value)).size();
            } else if (std::holds_alternative<int64_t>(*value)) {
                totalValueLength += std::to_string(std::get<int64_t>(*value)).size();
            } else if (std::holds_alternative<double>(*value)) {
                totalValueLength += std::to_string(std::get<double>(*value)).size();
            }
        }
    }
    
    stats.workingMemory = totalNumericFields;
    stats.episodicMemory = totalValueLength;
    stats.associativeMemory = totalValueLength;
    
    if (!stats.totalFields) {
        stats.averageFiringRate = 0.0f;
    } else {
        stats.averageFiringRate = static_cast<float>(totalValueLength) / stats.totalFields;
    }
    
    return stats;
}

std::unordered_map<std::string, size_t> ConfigStats::getFieldTypeDistribution(const Config& config) const {
    std::unordered_map<std::string, size_t> distribution;
    
    for (const auto& key : config.getKeys()) {
        auto value = config.get<ConfigValue>(key);
        if (value) {
            std::string type = typeToString(*value);
            distribution[type]++;
        }
    }
    
    return distribution;
}

std::unordered_map<std::string, size_t> ConfigStats::getSourceDistribution(const Config& config) const {
    // Note: This is a simplified implementation since Config doesn't have source tracking
    // In a real implementation, you would need to access the internal source information
    std::unordered_map<std::string, size_t> distribution;
    distribution["unknown"] = config.getKeys().size();
    return distribution;
}

} // namespace nlm