#include "ValidationUtilities.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Simple JSON library integration (assuming nlohmann/json is available)
// For YAML support, we assume yaml-cpp is available

ValidationConfig ValidationUtilities::parseValidationConfig(const std::string& filepath) {
    ValidationConfig config;
    
    // Check file extension to determine format
    if (filepath.substr(filepath.find_last_of(".") + 1) == "json") {
        // Parse JSON format
        std::ifstream file(filepath);
        if (file.is_open()) {
            nlohmann::json j;
            file >> j;
            
            if (j.contains("enable_cross_parameter_validation")) {
                config.enableCrossParameterValidation = j["enable_cross_parameter_validation"].get<bool>();
            }
            if (j.contains("enable_conditional_validation")) {
                config.enableConditionalValidation = j["enable_conditional_validation"].get<bool>();
            }
            if (j.contains("enable_range_validation")) {
                config.enableRangeValidation = j["enable_range_validation"].get<bool>();
            }
            if (j.contains("enable_dependency_validation")) {
                config.enableDependencyValidation = j["enable_dependency_validation"].get<bool>();
            }
            if (j.contains("enable_conflict_detection")) {
                config.enableConflictDetection = j["enable_conflict_detection"].get<bool>();
            }
            if (j.contains("max_memory_usage")) {
                config.maxMemoryUsage = j["max_memory_usage"].get<double>();
            }
            if (j.contains("max_validation_time")) {
                config.maxValidationTime = j["max_validation_time"].get<int>();
            }
            if (j.contains("report_all_warnings")) {
                config.reportAllWarnings = j["report_all_warnings"].get<bool>();
            }
            if (j.contains("validate_on_load")) {
                config.validateOnLoad = j["validate_on_load"].get<bool>();
            }
            if (j.contains("validate_on_set")) {
                config.validateOnSet = j["validate_on_set"].get<bool>();
            }
            if (j.contains("validate_on_change")) {
                config.validateOnChange = j["validate_on_change"].get<bool>();
            }
            if (j.contains("validation_file")) {
                config.validationFile = j["validation_file"].get<std::string>();
            }
        }
    } else if (filepath.substr(filepath.find_last_of(".") + 1) == "yaml" || 
               filepath.substr(filepath.find_last_of(".") + 1) == "yml") {
        // Parse YAML format
        YAML::Node doc = YAML::LoadFile(filepath);
        
        if (doc["enable_cross_parameter_validation"]) {
            config.enableCrossParameterValidation = doc["enable_cross_parameter_validation"].as<bool>();
        }
        if (doc["enable_conditional_validation"]) {
            config.enableConditionalValidation = doc["enable_conditional_validation"].as<bool>();
        }
        if (doc["enable_range_validation"]) {
            config.enableRangeValidation = doc["enable_range_validation"].as<bool>();
        }
        if (doc["enable_dependency_validation"]) {
            config.enableDependencyValidation = doc["enable_dependency_validation"].as<bool>();
        }
        if (doc["enable_conflict_detection"]) {
            config.enableConflictDetection = doc["enable_conflict_detection"].as<bool>();
        }
        if (doc["max_memory_usage"]) {
            config.maxMemoryUsage = doc["max_memory_usage"].as<double>();
        }
        if (doc["max_validation_time"]) {
            config.maxValidationTime = doc["max_validation_time"].as<int>();
        }
        if (doc["report_all_warnings"]) {
            config.reportAllWarnings = doc["report_all_warnings"].as<bool>();
        }
        if (doc["validate_on_load"]) {
            config.validateOnLoad = doc["validate_on_load"].as<bool>();
        }
        if (doc["validate_on_set"]) {
            config.validateOnSet = doc["validate_on_set"].as<bool>();
        }
        if (doc["validate_on_change"]) {
            config.validateOnChange = doc["validate_on_change"].as<bool>();
        }
        if (doc["validation_file"]) {
            config.validationFile = doc["validation_file"].as<std::string>();
        }
    } else {
        // Parse key=value format (INI-like)
        std::ifstream file(filepath);
        std::string line;
        std::string section;
        
        while (std::getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty() || line[0] == ';' || line[0] == '#') {
                continue;
            }
            
            // Check for section
            if (line[0] == '[' && line.back() == ']') {
                section = line.substr(1, line.size() - 2);
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                // Parse based on section
                if (section == "validation_options") {
                    if (key == "enable_cross_parameter_validation") {
                        config.enableCrossParameterValidation = (value == "true" || value == "1");
                    } else if (key == "enable_conditional_validation") {
                        config.enableConditionalValidation = (value == "true" || value == "1");
                    } else if (key == "enable_range_validation") {
                        config.enableRangeValidation = (value == "true" || value == "1");
                    } else if (key == "enable_dependency_validation") {
                        config.enableDependencyValidation = (value == "true" || value == "1");
                    } else if (key == "enable_conflict_detection") {
                        config.enableConflictDetection = (value == "true" || value == "1");
                    } else if (key == "max_memory_usage") {
                        config.maxMemoryUsage = std::stod(value);
                    } else if (key == "max_validation_time") {
                        config.maxValidationTime = std::stoi(value);
                    } else if (key == "report_all_warnings") {
                        config.reportAllWarnings = (value == "true" || value == "1");
                    } else if (key == "validate_on_load") {
                        config.validateOnLoad = (value == "true" || value == "1");
                    } else if (key == "validate_on_set") {
                        config.validateOnSet = (value == "true" || value == "1");
                    } else if (key == "validate_on_change") {
                        config.validateOnChange = (value == "true" || value == "1");
                    } else if (key == "validation_file") {
                        config.validationFile = value;
                    }
                }
            }
        }
    }
    
    return config;
}

bool ValidationUtilities::loadRulesFromFile(ValidationRulesEngine& engine, const std::string& filepath) {
    // Check file extension to determine format
    if (filepath.substr(filepath.find_last_of(".") + 1) == "json") {
        // Parse JSON format
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        if (j.contains("rules")) {
            for (const auto& ruleJson : j["rules"]) {
                ValidationRule rule;
                
                if (ruleJson.contains("id")) rule.id = ruleJson["id"].get<std::string>();
                if (ruleJson.contains("name")) rule.name = ruleJson["name"].get<std::string>();
                if (ruleJson.contains("description")) rule.description = ruleJson["description"].get<std::string>();
                if (ruleJson.contains("priority")) rule.priority = ruleJson["priority"].get<int>();
                
                // Parse level
                if (ruleJson.contains("level")) {
                    std::string levelStr = ruleJson["level"].get<std::string>();
                    if (levelStr == "Info") rule.level = ValidationLevel::Info;
                    else if (levelStr == "Warning") rule.level = ValidationLevel::Warning;
                    else if (levelStr == "Error") rule.level = ValidationLevel::Error;
                    else if (levelStr == "Critical") rule.level = ValidationLevel::Critical;
                }
                
                // Parse action
                if (ruleJson.contains("action")) {
                    std::string actionStr = ruleJson["action"].get<std::string>();
                    if (actionStr == "Allow") rule.defaultAction = ValidationAction::Allow;
                    else if (actionStr == "Warn") rule.defaultAction = ValidationAction::Warn;
                    else if (actionStr == "Error") rule.defaultAction = ValidationAction::Error;
                    else if (actionStr == "Skip") rule.defaultAction = ValidationAction::Skip;
                }
                
                engine.addRule(rule);
            }
        }
    } else {
        // Try to parse as config file format (INI-like)
        ValidationConfig config = parseValidationConfig(filepath);
        
        if (!config.validationFile.empty()) {
            // Load rules from specified validation file
            return loadRulesFromFile(engine, config.validationFile);
        }
    }
    
    return true;
}

std::string ValidationUtilities::exportResultsToJSON(const std::vector<ValidationResultDetail>& results) {
    nlohmann::json j = results;
    
    // Custom serialization for ValidationResultDetail
    nlohmann::json jsonResults = nlohmann::json::array();
    for (const auto& result : results) {
        nlohmann::json jsonResult;
        
        jsonResult["key"] = result.key;
        jsonResult["subKey"] = result.subKey;
        jsonResult["level"] = static_cast<int>(result.level);
        jsonResult["action"] = static_cast<int>(result.action);
        jsonResult["message"] = result.message;
        jsonResult["ruleId"] = result.ruleId;
        jsonResult["priority"] = result.priority;
        jsonResult["context"] = result.context;
        jsonResult["isCrossParameter"] = result.isCrossParameter;
        jsonResult["isConditional"] = result.isConditional;
        jsonResult["executionTime"] = result.executionTime;
        
        jsonResults.push_back(jsonResult);
    }
    
    return jsonResults.dump(2);
}

std::string ValidationUtilities::exportResultsToYAML(const std::vector<ValidationResultDetail>& results) {
    // Convert to YAML node
    YAML::Emitter out;
    out << YAML::BeginSeq;
    
    for (const auto& result : results) {
        out << YAML::BeginMap;
        out << YAML::Key << "key" << YAML::Value << result.key;
        out << YAML::Key << "subKey" << YAML::Value << result.subKey;
        out << YAML::Key << "level" << YAML::Value << static_cast<int>(result.level);
        out << YAML::Key << "action" << YAML::Value << static_cast<int>(result.action);
        out << YAML::Key << "message" << YAML::Value << result.message;
        out << YAML::Key << "ruleId" << YAML::Value << result.ruleId;
        out << YAML::Key << "priority" << YAML::Value << result.priority;
        out << YAML::Key << "context" << YAML::Value << result.context;
        out << YAML::Key << "isCrossParameter" << YAML::Value << result.isCrossParameter;
        out << YAML::Key << "isConditional" << YAML::Value << result.isConditional;
        out << YAML::Key << "executionTime" << YAML::Value << result.executionTime;
        out << YAML::EndMap;
    }
    
    out << YAML::EndSeq;
    
    std::stringstream ss;
    ss << out.c_str();
    return ss.str();
}

std::string ValidationUtilities::exportResultsToCSV(const std::vector<ValidationResultDetail>& results) {
    std::ostringstream oss;
    
    // Write header
    oss << "Key,SubKey,Level,Action,Message,RuleId,Priority,Context,IsCrossParameter,IsConditional,ExecutionTime\n";
    
    for (const auto& result : results) {
        oss << "\"" << result.key << "\",";
        oss << "\"" << result.subKey << "\",";
        oss << static_cast<int>(result.level) << ",";
        oss << static_cast<int>(result.action) << ";\"" << result.message << "\",";
        oss << "\"" << result.ruleId << "\",";
        oss << result.priority << ",";
        oss << "\"" << result.context << "\",";
        oss << (result.isCrossParameter ? "true" : "false") << ",";
        oss << (result.isConditional ? "true" : "false") << ",";
        oss << result.executionTime << "\n";
    }
    
    return oss.str();
}

std::string ValidationUtilities::exportResultsToText(const std::vector<ValidationResultDetail>& results) {
    std::ostringstream oss;
    
    oss << "=== Validation Results ===\n\n";
    
    for (const auto& result : results) {
        oss << "[" << static_cast<int>(result.level) << "]";
        if (!result.ruleId.empty()) oss << "[" << result.ruleId << "]";
        oss << " Key: " << result.key;
        if (!result.subKey.empty()) oss << " SubKey: " << result.subKey;
        oss << " Priority: " << result.priority;
        oss << "\n  Message: " << result.message;
        if (!result.context.empty()) oss << "\n  Context: " << result.context;
        oss << "\n  Execution Time: " << result.executionTime << "ms\n\n";
    }
    
    return oss.str();
}

bool ValidationUtilities::parseConfigFromJSON(const std::string& content, Config& config) {
    try {
        nlohmann::json j = nlohmann::json::parse(content);
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            if (it->is_boolean()) {
                config.set(key, it->get<bool>());
            } else if (it->is_number_integer()) {
                config.set(key, it->get<int64_t>());
            } else if (it->is_number_float()) {
                config.set(key, it->get<double>());
            } else if (it->is_string()) {
                config.set(key, it->get<std::string>());
            } else if (it->is_array()) {
                if (it->empty()) {
                    continue;
                }
                
                // Check array type
                if ((*it)[0].is_boolean()) {
                    std::vector<bool> vec;
                    for (const auto& item : *it) {
                        vec.push_back(item.get<bool>());
                    }
                    config.set(key, vec);
                } else if ((*it)[0].is_number_integer()) {
                    std::vector<int64_t> vec;
                    for (const auto& item : *it) {
                        vec.push_back(item.get<int64_t>());
                    }
                    config.set(key, vec);
                } else if ((*it)[0].is_number_float()) {
                    std::vector<double> vec;
                    for (const auto& item : *it) {
                        vec.push_back(item.get<double>());
                    }
                    config.set(key, vec);
                } else if ((*it)[0].is_string()) {
                    std::vector<std::string> vec;
                    for (const auto& item : *it) {
                        vec.push_back(item.get<std::string>());
                    }
                    config.set(key, vec);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ValidationUtilities::parseConfigFromYAML(const std::string& content, Config& config) {
    try {
        YAML::Node doc = YAML::Load(content);
        
        for (const auto& node : doc) {
            std::string key = node.first.as<std::string>();
            YAML::Node value = node.second;
            
            if (value.IsScalar()) {
                if (value.IsBool()) {
                    config.set(key, value.as<bool>());
                } else if (value.IsInt()) {
                    config.set(key, value.as<int64_t>());
                } else if (value.IsDouble()) {
                    config.set(key, value.as<double>());
                } else if (value.IsString()) {
                    config.set(key, value.as<std::string>());
                }
            } else if (value.IsSequence()) {
                if (value.empty()) {
                    continue;
                }
                
                // Check sequence type
                if (value[0].IsBool()) {
                    std::vector<bool> vec;
                    for (const auto& item : value) {
                        vec.push_back(item.as<bool>());
                    }
                    config.set(key, vec);
                } else if (value[0].IsInt()) {
                    std::vector<int64_t> vec;
                    for (const auto& item : value) {
                        vec.push_back(item.as<int64_t>());
                    }
                    config.set(key, vec);
                } else if (value[0].IsDouble()) {
                    std::vector<double> vec;
                    for (const auto& item : value) {
                        vec.push_back(item.as<double>());
                    }
                    config.set(key, vec);
                } else if (value[0].IsString()) {
                    std::vector<std::string> vec;
                    for (const auto& item : value) {
                        vec.push_back(item.as<std::string>());
                    }
                    config.set(key, vec);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ValidationUtilities::parseConfigFromKeyValue(const std::string& content, Config& config) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        // Check for section
        if (line.find('=') == std::string::npos) {
            // Not a key=value pair, skip
            continue;
        }
        
        size_t pos = line.find('=');
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        // Try to parse value
        if (value == "true" || value == "false") {
            config.set(key, value == "true");
        } else {
            try {
                // Try to parse as number
                if (value.find('.') != std::string::npos || value.find('e') != std::string::npos || value.find('E') != std::string::npos) {
                    // Float or scientific notation
                    double doubleValue = std::stod(value);
                    config.set(key, doubleValue);
                } else {
                    // Integer
                    int64_t intValue = std::stoll(value);
                    config.set(key, intValue);
                }
            } catch (...) {
                // String value
                config.set(key, value);
            }
        }
    }
    
    return true;
}

std::vector<ValidationRule> ValidationUtilities::generateRulesFromSchema(const ConfigSchema& schema) {
    std::vector<ValidationRule> rules;
    
    auto parameters = schema.getAllParameters();
    for (const auto& param : parameters) {
        // Generate validation rules based on parameter constraints
        ValidationRule rule;
        
        // Create rule ID based on parameter
        rule.id = "VALIDATE_" + param.key + "_001";
        rule.name = "Validation for " + param.key;
        rule.description = "Validation rule generated from schema for parameter: " + param.key;
        rule.priority = 50;  // Medium priority
        
        // Set level based on whether parameter is required
        if (param.required) {
            rule.level = ValidationLevel::Error;
        } else {
            rule.level = ValidationLevel::Warning;
        }
        
        rule.defaultAction = ValidationAction::Error;
        
        // Generate validator based on parameter type and constraints
        if (param.hasConstraints()) {
            // Add range validator
            rule.setRangeValidator(
                [param](const Config& config, const std::vector<ConfigEntry>& entries,
                        const ConfigParameter& p, const ConfigEntry& entry) {
                    std::vector<ValidationResultDetail> results;
                    
                    if (p.minValue.has_value() && entry.value.if_type<double>()) {
                        double value = *entry.value.if_type<double>();
                        if (value < p.minValue.value()) {
                            results.emplace_back(p.key, "", ValidationLevel::Error,
                                               "Value " + std::to_string(value) + " is less than minimum " + 
                                               std::to_string(p.minValue.value()),
                                               "SCHEMA_BASED", 80);
                        }
                    }
                    
                    if (p.maxValue.has_value() && entry.value.if_type<double>()) {
                        double value = *entry.value.if_type<double>();
                        if (value > p.maxValue.value()) {
                            results.emplace_back(p.key, "", ValidationLevel::Error,
                                               "Value " + std::to_string(value) + " exceeds maximum " + 
                                               std::to_string(p.maxValue.value()),
                                               "SCHEMA_BASED", 80);
                        }
                    }
                    
                    if (!p.allowedValues.empty()) {
                        std::string strValue;
                        if (entry.value.if_type<std::string>()) {
                            strValue = *entry.value.if_type<std::string>();
                        } else {
                            // Try to convert to string
                            strValue = std::to_string(std::visit([](auto&& arg) {
                                using T = std::decay_t<decltype(arg)>;
                                if constexpr (std::is_same_v<T, std::string>) return arg;
                                else if constexpr (std::is_same_v<T, bool>) return std::to_string(arg);
                                else return std::to_string(arg);
                            }, entry.value));
                        }
                        
                        bool isAllowed = false;
                        for (const auto& allowed : p.allowedValues) {
                            if (strValue == allowed) {
                                isAllowed = true;
                                break;
                            }
                        }
                        
                        if (!isAllowed) {
                            results.emplace_back(p.key, "", ValidationLevel::Error,
                                               "Value '" + strValue + "' is not in allowed values",
                                               "SCHEMA_BASED", 80);
                        }
                    }
                    
                    return results;
                }
            );
        }
        
        rules.push_back(rule);
    }
    
    return rules;
}

std::string ValidationUtilities::getValidationStatistics(const ValidationFramework& framework) {
    std::ostringstream oss;
    
    oss << "=== Validation Framework Statistics ===\n\n";
    oss << "  Framework Information:" << std::endl;
    oss << "    - Total validations: " << framework.getTotalValidations() << std::endl;
    oss << "    - Successful validations: " << framework.getSuccessfulValidations() << std::endl;
    oss << "    - Error validations: " << framework.getErrorValidations() << std::endl;
    oss << "\n  Engine Statistics:" << std::endl;
    oss << "    " << framework.getEngine().getStatistics() << std::endl;
    
    return oss.str();
}

bool ValidationUtilities::isValid(const std::vector<ValidationResultDetail>& results, ValidationLevel minLevel) {
    for (const auto& result : results) {
        if (result.level >= minLevel) {
            return false;
        }
    }
    return true;
}

std::string ValidationUtilities::getValidationSummary(const std::vector<ValidationResultDetail>& results) {
    std::ostringstream oss;
    
    int total = results.size();
    int info = 0, warning = 0, error = 0, critical = 0;
    
    for (const auto& result : results) {
        switch (result.level) {
            case ValidationLevel::Info: info++; break;
            case ValidationLevel::Warning: warning++; break;
            case ValidationLevel::Error: error++; break;
            case ValidationLevel::Critical: critical++; break;
        }
    }
    
    oss << "=== Validation Summary ===\n";
    oss << "Total results: " << total << "\n";
    oss << "Info: " << info << "\n";
    oss << "Warnings: " << warning << "\n";
    oss << "Errors: " << error << "\n";
    oss << "Critical: " << critical << "\n";
    
    if (total > 0) {
        oss << "\nValidity: " << (isValid(results) ? "VALID" : "INVALID") << "\n";
    }
    
    return oss.str();
}

std::vector<ValidationResultDetail> ValidationUtilities::filterByRuleId(const std::vector<ValidationResultDetail>& results, const std::string& ruleId) {
    std::vector<ValidationResultDetail> filtered;
    for (const auto& result : results) {
        if (result.ruleId == ruleId) {
            filtered.push_back(result);
        }
    }
    return filtered;
}

std::map<ValidationLevel, std::vector<ValidationResultDetail>> ValidationUtilities::groupBySeverity(const std::vector<ValidationResultDetail>& results) {
    std::map<ValidationLevel, std::vector<ValidationResultDetail>> grouped;
    
    for (const auto& result : results) {
        grouped[result.level].push_back(result);
    }
    
    return grouped;
}

std::map<ValidationLevel, int> ValidationUtilities::countByLevel(const std::vector<ValidationResultDetail>& results) {
    std::map<ValidationLevel, int> counts;
    
    for (const auto& result : results) {
        counts[result.level]++;
    }
    
    return counts;
}

std::set<std::string> ValidationUtilities::getInvalidKeys(const std::vector<ValidationResultDetail>& results) {
    std::set<std::string> invalidKeys;
    for (const auto& result : results) {
        invalidKeys.insert(result.key);
    }
    return invalidKeys;
}

std::vector<ValidationResultDetail> ValidationUtilities::mergeResults(const std::vector<ValidationResultDetail>& results1,
                                                                     const std::vector<ValidationResultDetail>& results2) {
    std::vector<ValidationResultDetail> merged = results1;
    merged.insert(merged.end(), results2.begin(), results2.end());
    return merged;
}

std::vector<ValidationResultDetail> ValidationUtilities::sortByPriority(const std::vector<ValidationResultDetail>& results) {
    std::vector<ValidationResultDetail> sorted = results;
    std::sort(sorted.begin(), sorted.end(),
              [](const ValidationResultDetail& a, const ValidationResultDetail& b) {
                  return a.priority > b.priority;  // Higher priority first
              });
    return sorted;
}

void ValidationUtilities::addMetadata(std::vector<ValidationResultDetail>& results, const std::string& frameworkVersion,
                                     const std::string& timestamp, const std::string& frameworkName) {
    for (auto& result : results) {
        if (result.context.empty()) {
            result.context = "Framework: " + frameworkName + ", Version: " + frameworkVersion + 
                           ", Timestamp: " + timestamp;
        } else {
            result.context += " | Framework: " + frameworkName + ", Version: " + frameworkVersion + 
                            ", Timestamp: " + timestamp;
        }
    }
}

std::vector<ValidationResultDetail> ValidationUtilities::validateConfigWithSchema(const Config& config, const ConfigSchema& schema) {
    std::vector<ValidationResultDetail> results;
    
    // Get schema validation results
    auto schemaResults = schema.validateAll(getConfigEntriesFromConfig(config));
    
    // Convert schema validation results to ValidationResultDetail
    for (const auto& schemaResult : schemaResults) {
        ValidationResultDetail detail;
        detail.key = schemaResult.key;
        detail.subKey = "";
        detail.level = schemaResult.valid ? ValidationLevel::Info : ValidationLevel::Error;
        detail.action = schemaResult.valid ? ValidationAction::Allow : ValidationAction::Error;
        detail.message = schemaResult.errorMessage;
        detail.ruleId = "SCHEMA_VALIDATION";
        detail.priority = 100;
        detail.isCrossParameter = false;
        detail.isConditional = false;
        detail.executionTime = 0.0;
        
        results.push_back(detail);
    }
    
    return results;
}

std::vector<ValidationResultDetail> ValidationUtilities::validateConfigWithFramework(const Config& config, const ConfigSchema& schema,
                                                                                    ValidationFramework& framework) {
    return framework.validate(config, schema);
}

bool ValidationUtilities::saveValidationResults(const std::string& filepath, const std::vector<ValidationResultDetail>& results,
                                               const std::string& format) {
    if (format == "text") {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        file << exportResultsToText(results);
        file.close();
        return true;
    } else if (format == "json") {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        file << exportResultsToJSON(results);
        file.close();
        return true;
    } else if (format == "yaml") {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        file << exportResultsToYAML(results);
        file.close();
        return true;
    } else if (format == "csv") {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        file << exportResultsToCSV(results);
        file.close();
        return true;
    }
    
    return false;
}

bool ValidationUtilities::loadValidationResults(const std::string& filepath, std::vector<ValidationResultDetail>& results,
                                               const std::string& format) {
    // This is a simplified implementation
    // In practice, you would implement proper parsing for each format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, just read as text and parse
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Parse content (simplified)
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.find("Key:") != std::string::npos) {
            // Parse line and add to results
            ValidationResultDetail result;
            // Simple parsing - in practice, you would need more robust parsing
            result.message = line;
            results.push_back(result);
        }
    }
    
    return true;
}

ValidationConfig ValidationUtilities::createConfigFromCommandLine(int argc, char** argv) {
    ValidationConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--validate-only") {
            config.validateOnLoad = true;
            config.validateOnSet = false;
            config.validateOnChange = false;
        } else if (arg == "--skip-validation") {
            config.validateOnLoad = false;
            config.validateOnSet = false;
            config.validateOnChange = false;
        } else if (arg.find("--validation-level=") == 0) {
            std::string level = arg.substr(20);
            if (level == "Info") {
                // All validations at Info level
            } else if (level == "Warning") {
                // Only Warning and Error
            } else if (level == "Error") {
                // Only Error and Critical
            } else if (level == "Critical") {
                // Only Critical
            }
        } else if (arg.find("--validation-file=") == 0) {
            config.validationFile = arg.substr(20);
        } else if (arg.find("--validation-report=") == 0) {
            // This would be handled separately
        } else if (arg == "--validation-stats") {
            // This would be handled separately
        } else if (arg.find("--validation-timeout=") == 0) {
            config.maxValidationTime = std::stoi(arg.substr(22));
        } else if (arg.find("--memory-limit=") == 0) {
            config.maxMemoryUsage = std::stod(arg.substr(17));
        }
    }
    
    return config;
}

bool ValidationUtilities::parseValidationOptions(int argc, char** argv, ValidationConfig& config,
                                                 std::string& validationFile, std::string& reportFile) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--validate-only") {
            config.validateOnLoad = true;
            config.validateOnSet = false;
            config.validateOnChange = false;
        } else if (arg == "--skip-validation") {
            config.validateOnLoad = false;
            config.validateOnSet = false;
            config.validateOnChange = false;
        } else if (arg.find("--validation-file=") == 0) {
            validationFile = arg.substr(20);
        } else if (arg.find("--validation-report=") == 0) {
            reportFile = arg.substr(22);
        } else if (arg == "--validation-stats") {
            // This would be handled by the caller
        } else if (arg.find("--validation-timeout=") == 0) {
            config.maxValidationTime = std::stoi(arg.substr(22));
        } else if (arg.find("--memory-limit=") == 0) {
            config.maxMemoryUsage = std::stod(arg.substr(17));
        }
    }
    
    return true;
}

std::string ValidationUtilities::generateValidationReport(const std::vector<ValidationResultDetail>& results,
                                                         const ValidationConfig& config) {
    std::ostringstream oss;
    
    oss << "=== Configuration Validation Report ===\n";
    oss << "Generated: " << getCurrentTimestamp() << "\n";
    oss << "Configuration Validation Options:" << std::endl;
    oss << "  - Cross-parameter validation: " << (config.enableCrossParameterValidation ? "Enabled" : "Disabled") << std::endl;
    oss << "  - Conditional validation: " << (config.enableConditionalValidation ? "Enabled" : "Disabled") << std::endl;
    oss << "  - Range validation: " << (config.enableRangeValidation ? "Enabled" : "Disabled") << std::endl;
    oss << "  - Dependency validation: " << (config.enableDependencyValidation ? "Enabled" : "Disabled") << std::endl;
    oss << "  - Conflict detection: " << (config.enableConflictDetection ? "Enabled" : "Disabled") << std::endl;
    oss << "  - Maximum memory usage: " << config.maxMemoryUsage << " MB\n";
    oss << "  - Maximum validation time: " << config.maxValidationTime << " seconds\n";
    oss << "  - Report all warnings: " << (config.reportAllWarnings ? "Yes" : "No") << std::endl;
    oss << "\n";
    
    oss << getValidationSummary(results) << "\n";
    
    oss << "\n=== Detailed Results ===\n";
    for (const auto& result : results) {
        oss << result.toString() << "\n";
    }
    
    return oss.str();
}

ValidationConfig ValidationUtilities::loadValidationConfig(const std::string& filepath) {
    return ValidationUtilities::parseValidationConfig(filepath);
}

bool ValidationUtilities::saveValidationConfig(const std::string& filepath, const ValidationConfig& config) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# Validation Configuration\n";
    file << "# Enable cross-parameter validation\n";
    file << (config.enableCrossParameterValidation ? "enable_cross_parameter_validation = true" : "enable_cross_parameter_validation = false") << "\n";
    file << "\n";
    file << "# Enable conditional validation\n";
    file << (config.enableConditionalValidation ? "enable_conditional_validation = true" : "enable_conditional_validation = false") << "\n";
    file << "\n";
    file << "# Enable range validation\n";
    file << (config.enableRangeValidation ? "enable_range_validation = true" : "enable_range_validation = false") << "\n";
    file << "\n";
    file << "# Enable dependency validation\n";
    file << (config.enableDependencyValidation ? "enable_dependency_validation = true" : "enable_dependency_validation = false") << "\n";
    file << "\n";
    file << "# Enable conflict detection\n";
    file << (config.enableConflictDetection ? "enable_conflict_detection = true" : "enable_conflict_detection = false") << "\n";
    file << "\n";
    file << "# Maximum memory usage (MB)\n";
    file << "max_memory_usage = " << config.maxMemoryUsage << "\n";
    file << "\n";
    file << "# Maximum validation time (seconds)\n";
    file << "max_validation_time = " << config.maxValidationTime << "\n";
    file << "\n";
    file << "# Report all warnings\n";
    file << (config.reportAllWarnings ? "report_all_warnings = true" : "report_all_warnings = false") << "\n";
    file << "\n";
    file << "# Validate on load\n";
    file << (config.validateOnLoad ? "validate_on_load = true" : "validate_on_load = false") << "\n";
    file << "\n";
    file << "# Validate on set\n";
    file << (config.validateOnSet ? "validate_on_set = true" : "validate_on_set = false") << "\n";
    file << "\n";
    file << "# Validate on change\n";
    file << (config.validateOnChange ? "validate_on_change = true" : "validate_on_change = false") << "\n";
    file << "\n";
    file << "# Validation rules file\n";
    file << "validation_file = " << config.validationFile << "\n";
    
    file.close();
    return true;
}

// Helper function to get config entries from a Config object
std::vector<ConfigEntry> ValidationUtilities::getConfigEntriesFromConfig(const Config& config) {
    std::vector<ConfigEntry> entries;
    
    // This is a simplified version - in practice, we would need access to internal config data
    // For now, we'll create a mock implementation
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        // Try to get the value based on type
        if (auto val = config.get<int>(key); val.has_value()) {
            entries.emplace_back(key, *val);
        } else if (auto val = config.get<double>(key); val.has_value()) {
            entries.emplace_back(key, *val);
        } else if (auto val = config.get<bool>(key); val.has_value()) {
            entries.emplace_back(key, *val);
        } else if (auto val = config.get<std::string>(key); val.has_value()) {
            entries.emplace_back(key, *val);
        }
    }
    
    return entries;
}

} // namespace nlm
