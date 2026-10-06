// SchemaValidator implementation
#include "SchemaValidator.hpp"
#include <algorithm>

namespace nlm {

ValidationResult SchemaValidator::validate(const Config& config, const nlohmann::json& schema) const {
    ValidationResult result(true);

    try {
        // Validate schema first
        ValidationResult schemaResult = validateSchema(schema);
        if (!schemaResult.valid) {
            return schemaResult;
        }

        // Validate against schema
        result = validateRequiredFields(schema, config);
        if (!result.valid) {
            return result;
        }

        result = validateTypes(schema, config);
        if (!result.valid) {
            return result;
        }

        result = validateConstraints(schema, config);
        if (!result.valid) {
            return result;
        }

        result = validateNestedSchemas(schema, config);

    } catch (const std::exception& e) {
        result.valid = false;
        result.errors.push_back(std::string("Schema validation error: ") + e.what());
    }

    return result;
}

ValidationResult SchemaValidator::validateSchema(const nlohmann::json& schema) const {
    ValidationResult result(true);

    if (!isSchemaValid(schema)) {
        result.valid = false;
        result.errors.push_back("Invalid schema structure");
        return result;
    }

    // Check for required schema elements
    if (schema.find("type") != schema.end()) {
        std::string type = schema["type"].get<std::string>();
        if (!isValidType(type)) {
            result.valid = false;
            result.errors.push_back("Invalid type specification: " + type);
        }
    }

    return result;
}

bool SchemaValidator::validateRequiredFields(const nlohmann::json& schema, const Config& config) const {
    ValidationResult result(true);

    if (schema.find("required") != schema.end()) {
        nlohmann::json required = schema["required"];
        if (required.is_array()) {
            for (const auto& item : required) {
                if (!item.is_string()) {
                    continue;
                }

                std::string key = item.get<std::string>();
                if (!config.has(key)) {
                    result.valid = false;
                    result.errors.push_back("Required key missing: " + key);
                }
            }
        }
    }

    return result.valid;
}

bool SchemaValidator::validateTypes(const nlohmann::json& schema, const Config& config) const {
    ValidationResult result(true);

    if (schema.find("properties") != schema.end()) {
        nlohmann::json properties = schema["properties"];
        if (properties.is_object()) {
            for (auto it = properties.begin(); it != properties.end(); ++it) {
                std::string key = it.key();
                nlohmann::json propertySchema = it.value();

                if (!validateTypeConstraint(key, propertySchema)) {
                    result.valid = false;
                    result.errors.push_back("Type validation failed for key: " + key);
                }
            }
        }
    }

    return result.valid;
}

bool SchemaValidator::validateConstraints(const nlohmann::json& schema, const Config& config) const {
    ValidationResult result(true);

    if (schema.find("properties") != schema.end()) {
        nlohmann::json properties = schema["properties"];
        if (properties.is_object()) {
            for (auto it = properties.begin(); it != properties.end(); ++it) {
                std::string key = it.key();
                nlohmann::json propertySchema = it.value();

                if (!validateRangeConstraint(key, propertySchema)) {
                    result.valid = false;
                    result.errors.push_back("Range validation failed for key: " + key);
                }

                if (!validateEnumConstraint(key, propertySchema)) {
                    result.valid = false;
                    result.errors.push_back("Enum validation failed for key: " + key);
                }

                if (!validatePatternConstraint(key, propertySchema)) {
                    result.valid = false;
                    result.errors.push_back("Pattern validation failed for key: " + key);
                }

                if (!validateFormatConstraint(key, propertySchema)) {
                    result.valid = false;
                    result.errors.push_back("Format validation failed for key: " + key);
                }
            }
        }
    }

    return result.valid;
}

bool SchemaValidator::validateNestedSchemas(const nlohmann::json& schema, const Config& config) const {
    ValidationResult result(true);

    if (schema.find("properties") != schema.end()) {
        nlohmann::json properties = schema["properties"];
        if (properties.is_object()) {
            for (auto it = properties.begin(); it != properties.end(); ++it) {
                std::string key = it.key();
                nlohmann::json propertySchema = it.value();

                if (propertySchema.find("properties") != propertySchema.end()) {
                    // This is a nested object - check if the config value is an object
                    if (config.has(key)) {
                        try {
                            auto value = config.get<std::string>(key);
                            // For now, we skip nested object validation in this implementation
                            // Would need to recursively parse the string as JSON/YAML
                        } catch (...) {}
                    }
                }
            }
        }
    }

    return result.valid;
}

bool SchemaValidator::validateTypeConstraint(const std::string& key, const nlohmann::json& schema) const {
    if (!schema.contains("type")) {
        return true; // No type constraint
    }

    std::string type = schema["type"].get<std::string>();
    return isValidType(type);
}

bool SchemaValidator::validateRangeConstraint(const std::string& key, const nlohmann::json& schema) const {
    if (!schema.contains("minimum") && !schema.contains("maximum")) {
        return true; // No range constraint
    }

    // Get the current value
    const Config* configPtr = reinterpret_cast<const Config*>(&config_);
    if (!configPtr || !configPtr->has(key)) {
        return false;
    }

    double value = getNumericValue(*configPtr, key);
    if (value == std::numeric_limits<double>::lowest()) {
        return false; // Failed to get value
    }

    if (schema.contains("minimum")) {
        double min = schema["minimum"].get<double>();
        if (value < min) {
            return false;
        }
    }

    if (schema.contains("maximum")) {
        double max = schema["maximum"].get<double>();
        if (value > max) {
            return false;
        }
    }

    return true;
}

bool SchemaValidator::validateEnumConstraint(const std::string& key, const nlohmann::json& schema) const {
    if (!schema.contains("enum")) {
        return true; // No enum constraint
    }

    nlohmann::json enumValues = schema["enum"];
    if (!enumValues.is_array()) {
        return false;
    }

    // Get the current value
    const Config* configPtr = reinterpret_cast<const Config*>(&config_);
    if (!configPtr || !configPtr->has(key)) {
        return false;
    }

    std::string valueStr;
    try {
        auto value = configPtr->get<std::string>(key);
        if (value) {
            valueStr = *value;
        } else {
            return false;
        }
    } catch (...) {
        return false;
    }

    // Check if value is in enum
    for (const auto& enumItem : enumValues) {
        if (enumItem.is_string() && enumItem.get<std::string>() == valueStr) {
            return true;
        }
    }

    return false;
}

bool SchemaValidator::validatePatternConstraint(const std::string& key, const nlohmann::json& schema) const {
    if (!schema.contains("pattern")) {
        return true; // No pattern constraint
    }

    std::string pattern = schema["pattern"].get<std::string>();
    std::string regexError;

    // Get the current value
    const Config* configPtr = reinterpret_cast<const Config*>(&config_);
    if (!configPtr || !configPtr->has(key)) {
        return false;
    }

    std::string valueStr;
    try {
        auto value = configPtr->get<std::string>(key);
        if (value) {
            valueStr = *value;
        } else {
            return false;
        }
    } catch (...) {
        return false;
    }

    // Simple pattern matching for demonstration
    // Would use proper regex library in production
    if (pattern == "^[a-zA-Z0-9_]+$") {
        return std::all_of(valueStr.begin(), valueStr.end(),
                          [](char c) { return std::isalnum(c) || c == '_'; });
    }

    return true;
}

bool SchemaValidator::validateFormatConstraint(const std::string& key, const nlohmann::json& schema) const {
    if (!schema.contains("format")) {
        return true; // No format constraint
    }

    std::string format = schema["format"].get<std::string>();

    // Get the current value
    const Config* configPtr = reinterpret_cast<const Config*>(&config_);
    if (!configPtr || !configPtr->has(key)) {
        return false;
    }

    std::string valueStr;
    try {
        auto value = configPtr->get<std::string>(key);
        if (value) {
            valueStr = *value;
        } else {
            return false;
        }
    } catch (...) {
        return false;
    }

    if (format == "email") {
        return valueStr.find("@") != std::string::npos;
    } else if (format == "uri") {
        return valueStr.find("://") != std::string::npos;
    } else if (format == "ipv4") {
        // Simple IPv4 format check
        size_t octets = std::count(valueStr.begin(), valueStr.end(), '.');
        return octets == 3 && valueStr.find_first_not_of("0123456789.") == std::string::npos;
    }

    return true;
}

std::string SchemaValidator::getSchemaTypeDescription(const nlohmann::json& schema) {
    if (!schema.contains("type")) {
        return "any";
    }

    std::string type = schema["type"].get<std::string>();

    if (type == "object") {
        return "object";
    } else if (type == "array") {
        if (schema.contains("items")) {
            return "array of " + getSchemaTypeDescription(schema["items"]);
        }
        return "array";
    } else {
        return type;
    }
}

bool SchemaValidator::isSchemaValid(const nlohmann::json& schema) {
    if (!schema.is_object()) {
        return false;
    }

    // Basic schema validation
    std::vector<std::string> allowedTypes = {"string", "number", "integer", "boolean", "object", "array"};

    if (schema.contains("type")) {
        std::string type = schema["type"].get<std::string>();
        if (std::find(allowedTypes.begin(), allowedTypes.end(), type) == allowedTypes.end()) {
            return false;
        }
    }

    // Check additional constraints
    if (schema.contains("properties") && !schema["properties"].is_object()) {
        return false;
    }

    if (schema.contains("items") && !schema["items"].is_object()) {
        return false;
    }

    return true;
}

nlohmann::json SchemaValidator::createDefaultSchema() {
    nlohmann::json schema;
    schema["type"] = "object";
    schema["additionalProperties"] = false;
    return schema;
}

} // namespace nlm
