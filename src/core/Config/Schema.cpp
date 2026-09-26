#include "Schema.hpp"

namespace nlm {

// Global schema instance
ConfigurationSchema global_schema;

void ConfigurationSchema::addParameter(const ParameterMeta& param) {
    parameters[param.name] = param;
    
    // Update groups and categories
    if (!param.group.empty()) {
        groups[param.group].push_back(param.name);
    }
    if (!param.category.empty()) {
        categories[param.category].push_back(param.group);
    }
}

bool ConfigurationSchema::hasParameter(const std::string& name) const {
    return parameters.find(name) != parameters.end();
}

const ParameterMeta* ConfigurationSchema::getParameter(const std::string& name) const {
    auto it = parameters.find(name);
    if (it != parameters.end()) {
        return &it->second;
    }
    return nullptr;
}

void ConfigurationSchema::addGroup(const std::string& name, const std::vector<std::string>& parameter_names) {
    for (const auto& param_name : parameter_names) {
        auto it = parameters.find(param_name);
        if (it != parameters.end()) {
            it->second.group = name;
        }
    }
    groups[name] = parameter_names;
}

void ConfigurationSchema::addCategory(const std::string& name, const std::vector<std::string>& group_names) {
    for (const auto& group_name : group_names) {
        auto it = groups.find(group_name);
        if (it != groups.end()) {
            for (const auto& param_name : it->second) {
                auto param_it = parameters.find(param_name);
                if (param_it != parameters.end()) {
                    param_it->second.category = name;
                }
            }
        }
    }
    categories[name] = group_names;
}

ValidationResult ConfigurationSchema::validate(const std::string& key, const nlm::ConfigValue& value) const {
    auto it = parameters.find(key);
    if (it == parameters.end()) {
        return ValidationResult(false, {"Unknown parameter: " + key});
    }
    
    return ConfigurationValidator::validateParameter(key, value, it->second);
}

ValidationResult ConfigurationSchema::validateAll(const std::vector<std::pair<std::string, nlm::ConfigValue>>& key_values) const {
    ValidationResult result;
    
    for (const auto& kv : key_values) {
        ValidationResult param_result = validate(kv.first, kv.second);
        if (param_result.hasErrors()) {
            result.is_valid = false;
            result.errors.insert(result.errors.end(), param_result.errors.begin(), param_result.errors.end());
        }
        if (param_result.hasWarnings()) {
            result.warnings.insert(result.warnings.end(), param_result.warnings.begin(), param_result.warnings.end());
        }
    }
    
    return result;
}

std::vector<std::string> ConfigurationSchema::getAllKeys() const {
    std::vector<std::string> keys;
    keys.reserve(parameters.size());
    for (const auto& pair : parameters) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<std::string> ConfigurationSchema::getKeysByGroup(const std::string& group) const {
    std::vector<std::string> keys;
    auto it = groups.find(group);
    if (it != groups.end()) {
        keys = it->second;
    }
    return keys;
}

std::vector<std::string> ConfigurationSchema::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> keys;
    auto it = categories.find(category);
    if (it != categories.end()) {
        for (const auto& group : it->second) {
            auto group_it = groups.find(group);
            if (group_it != groups.end()) {
                keys.insert(keys.end(), group_it->second.begin(), group_it->second.end());
            }
        }
    }
    return keys;
}

std::vector<std::string> ConfigurationSchema::getGroups() const {
    std::vector<std::string> group_names;
    group_names.reserve(groups.size());
    for (const auto& pair : groups) {
        group_names.push_back(pair.first);
    }
    return group_names;
}

std::vector<std::string> ConfigurationSchema::getCategories() const {
    std::vector<std::string> category_names;
    category_names.reserve(categories.size());
    for (const auto& pair : categories) {
        category_names.push_back(pair.first);
    }
    return category_names;
}

std::string ConfigurationSchema::generateDocumentation() const {
    std::ostringstream oss;
    oss << "NLM Configuration Schema" << std::endl;
    oss << "=========================" << std::endl << std::endl;
    
    for (const auto& category : getCategories()) {
        oss << "Category: " << category << std::endl;
        oss << "---------" << std::endl;
        oss << generateCategoryDocumentation(category) << std::endl << std::endl;
    }
    
    return oss.str();
}

std::string ConfigurationSchema::generateCategoryDocumentation(const std::string& category) const {
    std::ostringstream oss;
    auto keys = getKeysByCategory(category);
    
    for (const auto& key : keys) {
        auto param = getParameter(key);
        if (!param) continue;
        
        oss << "Parameter: " << key << std::endl;
        oss << "  Type: " << typeToString(param->type) << std::endl;
        oss << "  Description: " << param->description << std::endl;
        oss << "  Category: " << param->category << std::endl;
        oss << "  Group: " << param->group << std::endl;
        
        // Default value
        if (param->type == ParameterType::INT && param->default_int.has_value()) {
            oss << "  Default: " << param->default_int.value() << std::endl;
        } else if (param->type == ParameterType::INT64 && param->default_int64.has_value()) {
            oss << "  Default: " << param->default_int64.value() << std::endl;
        } else if (param->type == ParameterType::DOUBLE && param->default_double.has_value()) {
            oss << "  Default: " << param->default_double.value() << std::endl;
        } else if (param->type == ParameterType::BOOL && param->default_bool.has_value()) {
            oss << "  Default: " << (param->default_bool.value() ? "true" : "false") << std::endl;
        } else if (param->type == ParameterType::STRING && param->default_string.has_value()) {
            oss << "  Default: \"" << param->default_string.value() << "\"" << std::endl;
        }
        
        // Constraints
        if (param->min_int.has_value() || param->max_int.has_value()) {
            oss << "  Range: [" << (param->min_int.has_value() ? std::to_string(param->min_int.value()) : "-inf")
                << ", " << (param->max_int.has_value() ? std::to_string(param->max_int.value()) : "inf") << "]" << std::endl;
        }
        if (param->min_double.has_value() || param->max_double.has_value()) {
            oss << "  Range: [" << (param->min_double.has_value() ? std::to_string(param->min_double.value()) : "-inf")
                << ", " << (param->max_double.has_value() ? std::to_string(param->max_double.value()) : "inf") << "]" << std::endl;
        }
        
        if (param->min_length.has_value() || param->max_length.has_value()) {
            oss << "  Length: [" << param->min_length.value_or(0) << ", " << param->max_length.value_or(0) << "]" << std::endl;
        }
        
        if (!param->allowed_values.empty()) {
            oss << "  Allowed values: ";
            for (size_t i = 0; i < param->allowed_values.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << param->allowed_values[i] << "\"";
            }
            oss << std::endl;
        }
        
        oss << std::endl;
    }
    
    return oss.str();
}

std::string ConfigurationSchema::exportSchema() const {
    // Convert schema to JSON format (simplified implementation)
    std::ostringstream oss;
    oss << "{\"schema\": [";
    
    bool first = true;
    for (const auto& pair : parameters) {
        if (!first) oss << ", ";
        first = false;
        
        oss << "{\"name\": \"" << pair.first << "\", \"type\": \"" << typeToString(pair.second.type) << "\", \"description\": \"" << pair.second.description << "\"}";
    }
    
    oss << "]}";
    return oss.str();
}

bool ConfigurationSchema::importSchema(const std::string& schema_json) {
    // Parse JSON and rebuild schema (simplified implementation)
    // This would use a JSON parser in a real implementation
    return false; // TODO: Implement proper JSON parsing
}

} // namespace nlm
