#include "ConfigEnhanced.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <ctime>

namespace nlm {

    // Template conversion functions
    template<typename T>
    T ConfigEnhanced::convert_to_type(const std::string& value_str) {
        std::istringstream iss(value_str);
        T result;
        if (iss >> result) {
            return result;
        }
        throw ConfigurationError("Cannot convert string to desired type: '" + value_str + "'");
    }

    template<typename T>
    std::string ConfigEnhanced::convert_to_string(const T& value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    // ParameterValidator implementation
    bool ConfigEnhanced::ParameterValidator::validateParameter(const ConfigParameter& param, const std::string& value_str) {
        std::string dummy;
        return validateParameter(param, value_str, dummy);
    }

    std::vector<std::string> ConfigEnhanced::ParameterValidator::validateParameter(const ConfigParameter& param, const std::string& value_str, std::string& converted_value) {
        std::vector<std::string> errors;
        converted_value.clear();

        // Check if value is provided
        if (value_str.empty() && param.required) {
            errors.push_back("Parameter '" + param.name + "' is required but not provided");
        }

        if (value_str.empty() && !param.required) {
            return errors; // Empty value for optional parameter is OK
        }

        // Convert based on type
        try {
            if (param.type == "int64_t") {
                int64_t int_val = convert_to_type<int64_t>(value_str);
                if (isInRange(param, static_cast<double>(int_val))) {
                    converted_value = convert_to_string(int_val);
                } else {
                    errors.push_back("Value '" + value_str + "' for parameter '" + param.name + "' is outside valid range [" + 
                                   std::to_string(param.range.first) + ", " + std::to_string(param.range.second) + "]");
                }
            } else if (param.type == "float") {
                float float_val = convert_to_type<float>(value_str);
                if (isInRange(param, static_cast<double>(float_val))) {
                    converted_value = convert_to_string(float_val);
                } else {
                    errors.push_back("Value '" + value_str + "' for parameter '" + param.name + "' is outside valid range [" + 
                                   std::to_string(param.range.first) + ", " + std::to_string(param.range.second) + "]");
                }
            } else if (param.type == "bool") {
                if (value_str == "true" || value_str == "1" || value_str == "yes") {
                    converted_value = "true";
                } else if (value_str == "false" || value_str == "0" || value_str == "no") {
                    converted_value = "false";
                } else {
                    errors.push_back("Invalid boolean value '" + value_str + "' for parameter '" + param.name + "' (expected true/false, 1/0, yes/no)");
                }
            } else if (param.type == "size_t") {
                uint64_t size_val = convert_to_type<uint64_t>(value_str);
                if (isInRange(param, static_cast<double>(size_val))) {
                    converted_value = convert_to_string(size_val);
                } else {
                    errors.push_back("Value '" + value_str + "' for parameter '" + param.name + "' is outside valid range [" + 
                                   std::to_string(param.range.first) + ", " + std::to_string(param.range.second) + "]");
                }
            } else {
                errors.push_back("Unknown parameter type '" + param.type + "' for parameter '" + param.name + "'");
            }

            // Check against valid values if specified
            if (errors.empty() && !param.valid_values.empty()) {
                bool value_valid = false;
                for (const auto& valid_val : param.valid_values) {
                    if (value_str == valid_val) {
                        value_valid = true;
                        break;
                    }
                }
                if (!value_valid) {
                    errors.push_back("Invalid value '" + value_str + "' for parameter '" + param.name + "'. Valid values: ");
                    for (size_t i = 0; i < param.valid_values.size(); ++i) {
                        if (i > 0) errors.back() += ", ";
                        errors.back() += param.valid_values[i];
                    }
                }
            }

        } catch (const std::exception& e) {
            errors.push_back("Error parsing value '" + value_str + "' for parameter '" + param.name + "': " + std::string(e.what()));
        }

        return errors;
    }

    bool ConfigEnhanced::ParameterValidator::isInRange(const ConfigParameter& param, double value) {
        if (param.range.first == -1e6 && param.range.second == 1e6) {
            return true; // No range checking
        }
        return value >= param.range.first && value <= param.range.second;
    }

    std::vector<std::string> ConfigEnhanced::ParameterValidator::getValidValues(const ConfigParameter& param) {
        return param.valid_values;
    }

    bool ConfigEnhanced::ParameterValidator::validateParameter(const ConfigParameter& param, const std::string& value_str, std::string& converted_value) {
        std::vector<std::string> errors = validateParameter(param, value_str, converted_value);
        return errors.empty();
    }

    // ConfigEnhanced implementation
    ConfigEnhanced::ConfigEnhanced() : config(std::make_shared<Config>()) {
        setSchema(getDefaultConfigSchema());
        NLM_LOG_INFO("ConfigEnhanced created with default schema");
    }

    ConfigEnhanced::ConfigEnhanced(const std::string& filename) : config(std::make_shared<Config>()) {
        setSchema(getDefaultConfigSchema());
        loadFromFile(filename);
        NLM_LOG_INFO("ConfigEnhanced loaded from " + filename);
    }

    bool ConfigEnhanced::setParameter(const std::string& key, const std::string& value) {
        // Find parameter in schema
        auto param_it = std::find_if(schema.begin(), schema.end(),
                                    [&key](const ConfigParameter& p) { return p.name == key; });
        if (param_it == schema.end()) {
            NLM_LOG_WARNING("Unknown parameter: " + key);
            return false;
        }

        // Validate and convert
        std::string converted_value;
        std::vector<std::string> errors = ParameterValidator::validateParameter(*param_it, value, converted_value);
        if (!errors.empty()) {
            for (const auto& error : errors) {
                NLM_LOG_ERROR("Validation error for parameter '" + key + "': " + error);
            }
            return false;
        }

        // Set the value in original config
        if (param_it->type == "int64_t") {
            config->set<int64_t>(key, convert_to_type<int64_t>(value));
        } else if (param_it->type == "float") {
            config->set<float>(key, convert_to_type<float>(value));
        } else if (param_it->type == "bool") {
            bool bool_val = (value == "true" || value == "1" || value == "yes");
            config->set<bool>(key, bool_val);
        } else if (param_it->type == "size_t") {
            config->set<size_t>(key, convert_to_type<size_t>(value));
        } else {
            config->set<std::string>(key, value);
        }

        NLM_LOG_INFO("Set parameter '" + key + "' = '" + value + "' (type: " + param_it->type + ")");
        return true;
    }

    bool ConfigEnhanced::setParameter(const std::string& key, double value) {
        // Find parameter in schema to check type compatibility
        auto param_it = std::find_if(schema.begin(), schema.end(),
                                    [&key](const ConfigParameter& p) { return p.name == key; });
        if (param_it == schema.end()) {
            NLM_LOG_WARNING("Unknown parameter: " + key);
            return false;
        }
        
        if (param_it->type == "float" || param_it->type == "int64_t" || param_it->type == "size_t") {
            return setParameter(key, convert_to_string(value));
        }
        return false;
    }

    bool ConfigEnhanced::setParameter(const std::string& key, int64_t value) {
        // Find parameter in schema to check type compatibility
        auto param_it = std::find_if(schema.begin(), schema.end(),
                                    [&key](const ConfigParameter& p) { return p.name == key; });
        if (param_it == schema.end()) {
            NLM_LOG_WARNING("Unknown parameter: " + key);
            return false;
        }
        
        if (param_it->type == "int64_t" || param_it->type == "size_t") {
            return setParameter(key, convert_to_string(value));
        }
        return false;
    }

    bool ConfigEnhanced::setParameter(const std::string& key, bool value) {
        return setParameter(key, value ? "true" : "false");
    }

    bool ConfigEnhanced::validate() {
        std::map<std::string, std::string> errors;
        validateWithSchema(schema, errors);
        return errors.empty();
    }

    bool ConfigEnhanced::isValid() const {
        return validate();
    }

    std::vector<std::string> ConfigEnhanced::getValidationErrors() const {
        std::map<std::string, std::string> validation_result;
        validateWithSchema(schema, validation_result);
        std::vector<std::string> errors;
        for (const auto& pair : validation_result) {
            if (!pair.second.empty()) {
                errors.push_back(pair.first + ": " + pair.second);
            }
        }
        return errors;
    }

    std::vector<std::string> ConfigEnhanced::getValidationWarnings() const {
        // For now, return empty warnings (could be enhanced later)
        return std::vector<std::string>();
    }

    std::map<std::string, std::string> ConfigEnhanced::validateWithSchema(const std::vector<ConfigParameter>& schema) {
        std::map<std::string, std::string> errors;

        for (const auto& param : schema) {
            std::string current_value_str;

            // Get current value as string
            if (config->has(param.name)) {
                if (param.type == "int64_t") {
                    current_value_str = convert_to_string(config->get<int64_t>(param.name));
                } else if (param.type == "float") {
                    current_value_str = convert_to_string(config->get<float>(param.name));
                } else if (param.type == "bool") {
                    current_value_str = config->get<bool>(param.name) ? "true" : "false";
                } else if (param.type == "size_t") {
                    current_value_str = convert_to_string(config->get<size_t>(param.name));
                } else {
                    current_value_str = config->get<std::string>(param.name);
                }
            }

            // Validate against schema
            std::string converted_value;
            std::vector<std::string> validation_errors = ParameterValidator::validateParameter(param, current_value_str, converted_value);

            if (!validation_errors.empty()) {
                for (const auto& error : validation_errors) {
                    errors[param.name] += (errors[param.name].empty() ? "" : "; ") + error;
                }
            } else if (param.required && current_value_str.empty()) {
                errors[param.name] = "Required parameter not provided";
            }
        }

        return errors;
    }

    void ConfigEnhanced::setSchema(const std::vector<ConfigParameter>& schema) {
        this->schema = schema;
        NLM_LOG_INFO("Config schema set with " + std::to_string(schema.size()) + " parameters");
    }

    std::vector<std::string> ConfigEnhanced::getGroups() const {
        std::vector<std::string> groups;
        std::set<std::string> unique_groups;

        for (const auto& param : schema) {
            if (unique_groups.find(param.group) == unique_groups.end()) {
                unique_groups.insert(param.group);
                groups.push_back(param.group);
            }
        }

        return groups;
    }

    std::vector<std::string> ConfigEnhanced::getParametersInGroup(const std::string& group) const {
        std::vector<std::string> parameters;

        for (const auto& param : schema) {
            if (param.group == group) {
                parameters.push_back(param.name);
            }
        }

        return parameters;
    }

    std::string ConfigEnhanced::getParameterDescription(const std::string& key) const {
        auto param_it = std::find_if(schema.begin(), schema.end(),
                                    [&key](const ConfigParameter& p) { return p.name == key; });
        if (param_it != schema.end()) {
            return param_it->description;
        }
        return "Unknown parameter";
    }

    bool ConfigEnhanced::hasParameter(const std::string& key) const {
        return std::any_of(schema.begin(), schema.end(),
                          [&key](const ConfigParameter& p) { return p.name == key; });
    }

    std::string ConfigEnhanced::getParameterType(const std::string& key) const {
        auto param_it = std::find_if(schema.begin(), schema.end(),
                                    [&key](const ConfigParameter& p) { return p.name == key; });
        if (param_it != schema.end()) {
            return param_it->type;
        }
        return "unknown";
    }

    void ConfigEnhanced::loadFromFile(const std::string& filepath) {
        config->loadFromFile(filepath);
        NLM_LOG_INFO("Configuration loaded from " + filepath);
    }

    void ConfigEnhanced::loadFromArgs(int argc, char** argv) {
        config->loadFromArgs(argc, argv);
        NLM_LOG_INFO("Configuration loaded from command line arguments");
    }

    void ConfigEnhanced::saveToFile(const std::string& filepath) const {
        config->saveToFile(filepath);
        NLM_LOG_INFO("Configuration saved to " + filepath);
    }

    bool ConfigEnhanced::has(const std::string& key) const {
        return config->has(key);
    }

    std::vector<std::string> ConfigEnhanced::getKeys() const {
        return config->getKeys();
    }

    void ConfigEnhanced::clear() {
        config->clear();
        NLM_LOG_INFO("Configuration cleared");
    }

    std::string ConfigEnhanced::summary() const {
        return config->summary();
    }

    std::string ConfigEnhanced::__repr__() const {
        return "<ConfigEnhanced: " + summary() + ">";
    }

    bool ConfigEnhanced::operator==(const ConfigEnhanced& other) const {
        return config->summary() == other.config->summary();
    }

    bool ConfigEnhanced::operator!=(const ConfigEnhanced& other) const {
        return !(*this == other);
    }

} // namespace nlm
