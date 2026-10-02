#include "ConfigSchema.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {

ConfigSchema::ConfigSchema(const std::string& name) : name_(name) {}

ConfigSchema::~ConfigSchema() = default;

void ConfigSchema::addField(const std::string& key, const std::string& type,
                           const std::string& description,
                           const std::optional<double>& min,
                           const std::optional<double>& max,
                           const std::string& pattern,
                           bool required,
                           const std::vector<std::string>& dependsOn) {
    if (fields_.find(key) != fields_.end()) {
        throw ConfigSchemaError("Field already exists: " + key, "ConfigSchema.hpp", __LINE__);
    }
    
    FieldInfo field;
    field.type = type;
    field.description = description;
    field.min = min;
    field.max = max;
    field.pattern = pattern;
    field.required = required;
    field.dependsOn = dependsOn;
    field.hasDefault = false;
    
    fields_[key] = field;
}

void ConfigSchema::setDefault(const std::string& key,
                             const std::variant<int, int64_t, double, bool, std::string>& value) {
    if (fields_.find(key) == fields_.end()) {
        throw ConfigSchemaError("Unknown field: " + key, "ConfigSchema.hpp", __LINE__);
    }
    
    fields_[key].defaultValue = value;
    fields_[key].hasDefault = true;
}

void ConfigSchema::setDefaults(const std::unordered_map<std::string, std::variant<int, int64_t, double, bool, std::string>>& defaults) {
    for (const auto& pair : defaults) {
        if (fields_.find(pair.first) == fields_.end()) {
            throw ConfigSchemaError("Unknown field in defaults: " + pair.first, "ConfigSchema.hpp", __LINE__);
        }
        fields_[pair.first].defaultValue = pair.second;
        fields_[pair.first].hasDefault = true;
    }
}

void ConfigSchema::addValidationRule(const std::string& key,
                                    std::function<bool(const std::variant<int, int64_t, double, bool, std::string>&)> validator,
                                    const std::string& errorMessage) {
    if (fields_.find(key) == fields_.end()) {
        throw ConfigSchemaError("Unknown field: " + key, "ConfigSchema.hpp", __LINE__);
    }
    
    fields_[key].validators.push_back(validator);
}

void ConfigSchema::addCrossValidation(const std::string& key,
                                     std::function<bool(const std::unordered_map<std::string, std::variant<int, int64_t, double, bool, std::string>>&)> validator,
                                     const std::string& errorMessage) {
    crossValidators_.push_back(validator);
    crossValidatorMessages_.push_back(errorMessage);
}

void ConfigSchema::addFormatter(const std::string& key,
                               std::function<std::string(const std::variant<int, int64_t, double, bool, std::string>&)> formatter) {
    if (fields_.find(key) == fields_.end()) {
        throw ConfigSchemaError("Unknown field: " + key, "ConfigSchema.hpp", __LINE__);
    }
    
    formatters_[key] = formatter;
    fields_[key].formatter = formatter;
}

std::string ConfigSchema::getFieldType(const std::string& key) const {
    auto it = fields_.find(key);
    if (it != fields_.end()) {
        return it->second.type;
    }
    throw ConfigSchemaError("Unknown field: " + key, "ConfigSchema.hpp", __LINE__);
}

std::string ConfigSchema::getFieldDescription(const std::string& key) const {
    auto it = fields_.find(key);
    if (it != fields_.end()) {
        return it->second.description;
    }
    throw ConfigSchemaError("Unknown field: " + key, "ConfigSchema.hpp", __LINE__);
}

bool ConfigSchema::isFieldRequired(const std::string& key) const {
    auto it = fields_.find(key);
    if (it != fields_.end()) {
        return it->second.required;
    }
    throw ConfigSchemaError("Unknown field: " + key, "ConfigSchema.hpp", __LINE__);
}

std::optional<double> ConfigSchema::getFieldMin(const std::string& key) const {
    auto it = fields_.find(key);
    if (it != fields_.end()) {
        return it->second.min;
    }
    throw ConfigSchemaError("Unknown field: " + key, "ConfigSchema.hpp", __LINE__);
}

std::optional<double> ConfigSchema::getFieldMax(const std::string& key) const {
    auto it = fields_.find(key);
    if (it != fields_.end()) {
        return it->second.max;
    }
    throw ConfigSchemaError("Unknown field: " + key, "ConfigSchema.hpp", __LINE__);
}

std::string ConfigSchema::getFieldPattern(const std::string& key) const {
    auto it = fields_.find(key);
    if (it != fields_.end()) {
        return it->second.pattern;
    }
    throw ConfigSchemaError("Unknown field: " + key, "ConfigSchema.hpp", __LINE__);
}

std::vector<std::string> ConfigSchema::getFieldDependencies(const std::string& key) const {
    auto it = fields_.find(key);
    if (it != fields_.end()) {
        return it->second.dependsOn;
    }
    throw ConfigSchemaError("Unknown field: " + key, "ConfigSchema.hpp", __LINE__);
}

std::variant<int, int64_t, double, bool, std::string> ConfigSchema::getDefault(const std::string& key) const {
    auto it = fields_.find(key);
    if (it != fields_.end() && it->second.hasDefault) {
        return it->second.defaultValue;
    }
    throw ConfigSchemaError("No default value for field: " + key, "ConfigSchema.hpp", __LINE__);
}

bool ConfigSchema::hasDefault(const std::string& key) const {
    auto it = fields_.find(key);
    return it != fields_.end() && it->second.hasDefault;
}

std::vector<std::function<bool(const std::variant<int, int64_t, double, bool, std::string>&)>>
ConfigSchema::getValidators(const std::string& key) const {
    auto it = fields_.find(key);
    if (it != fields_.end()) {
        return it->second.validators;
    }
    throw ConfigSchemaError("Unknown field: " + key, "ConfigSchema.hpp", __LINE__);
}

std::vector<std::function<bool(const std::unordered_map<std::string, std::variant<int, int64_t, double, bool, std::string>>&)>>
ConfigSchema::getCrossValidators() const {
    return crossValidators_;
}

std::function<std::string(const std::variant<int, int64_t, double, bool, std::string>&)>
ConfigSchema::getFormatter(const std::string& key) const {
    auto it = fields_.find(key);
    if (it != fields_.end()) {
        return it->second.formatter;
    }
    throw ConfigSchemaError("Unknown field: " + key, "ConfigSchema.hpp", __LINE__);
}

std::vector<std::string> ConfigSchema::validate(const std::unordered_map<std::string, std::variant<int, int64_t, double, bool, std::string>>& values) const {
    std::vector<std::string> errors;
    
    // Validate each field
    for (const auto& pair : fields_) {
        const std::string& key = pair.first;
        const FieldInfo& field = pair.second;
        
        // Check if field is required and present in values
        if (field.required && values.find(key) == values.end()) {
            errors.push_back("Required field missing: " + key);
        }
        
        // Validate if present
        if (values.find(key) != values.end()) {
            auto fieldErrors = validateField(key, values.at(key));
            errors.insert(errors.end(), fieldErrors.begin(), fieldErrors.end());
        }
    }
    
    // Run cross-validators
    for (size_t i = 0; i < crossValidators_.size(); ++i) {
        if (!crossValidators_[i](values)) {
            errors.push_back(crossValidatorMessages_[i]);
        }
    }
    
    return errors;
}

std::vector<std::string> ConfigSchema::validateField(const std::string& key,
                                                    const std::variant<int, int64_t, double, bool, std::string>& value) const {
    std::vector<std::string> errors;
    
    auto it = fields_.find(key);
    if (it == fields_.end()) {
        errors.push_back("Unknown field: " + key);
        return errors;
    }
    
    const FieldInfo& field = it->second;
    
    // Validate type (basic type checking)
    std::string expectedType = field.type;
    if (expectedType != "auto" && expectedType != "any") {
        bool typeValid = false;
        if (expectedType == "int" && std::holds_alternative<int>(value)) typeValid = true;
        else if (expectedType == "int64" && std::holds_alternative<int64_t>(value)) typeValid = true;
        else if (expectedType == "double" && (std::holds_alternative<double>(value) || std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value))) typeValid = true;
        else if (expectedType == "bool" && std::holds_alternative<bool>(value)) typeValid = true;
        else if (expectedType == "string" && std::holds_alternative<std::string>(value)) typeValid = true;
        else if (expectedType == "int_array" && std::holds_alternative<std::vector<int>>(value)) typeValid = true;
        else if (expectedType == "double_array" && std::holds_alternative<std::vector<double>>(value)) typeValid = true;
        else if (expectedType == "string_array" && std::holds_alternative<std::vector<std::string>>(value)) typeValid = true;
        
        if (!typeValid) {
            errors.push_back("Field '" + key + "' has wrong type. Expected: " + expectedType);
        }
    }
    
    // Validate range
    if (field.min.has_value() || field.max.has_value()) {
        double numValue = 0.0;
        bool isNumeric = false;
        
        if (std::holds_alternative<int>(value)) {
            numValue = static_cast<double>(std::get<int>(value));
            isNumeric = true;
        } else if (std::holds_alternative<int64_t>(value)) {
            numValue = static_cast<double>(std::get<int64_t>(value));
            isNumeric = true;
        } else if (std::holds_alternative<double>(value)) {
            numValue = std::get<double>(value);
            isNumeric = true;
        }
        
        if (isNumeric) {
            if (field.min.has_value() && numValue < field.min.value()) {
                errors.push_back("Field '" + key + "' value " + std::to_string(numValue) + " is less than minimum " + std::to_string(field.min.value()));
            }
            if (field.max.has_value() && numValue > field.max.value()) {
                errors.push_back("Field '" + key + "' value " + std::to_string(numValue) + " exceeds maximum " + std::to_string(field.max.value()));
            }
        }
    }
    
    // Validate pattern
    if (!field.pattern.empty() && std::holds_alternative<std::string>(value)) {
        std::string strValue = std::get<std::string>(value);
        if (!matchesPattern(strValue, field.pattern)) {
            errors.push_back("Field '" + key + "' value '" + strValue + "' does not match pattern '" + field.pattern + "'");
        }
    }
    
    // Run field-specific validators
    for (const auto& validator : field.validators) {
        if (!validator(value)) {
            errors.push_back("Field '" + key + "' validation failed");
        }
    }
    
    return errors;
}

std::string ConfigSchema::formatValue(const std::string& key,
                                     const std::variant<int, int64_t, double, bool, std::string>& value) const {
    auto it = fields_.find(key);
    if (it == fields_.end()) {
        throw ConfigSchemaError("Unknown field: " + key, "ConfigSchema.hpp", __LINE__);
    }
    
    const FieldInfo& field = it->second;
    
    if (field.formatter) {
        return field.formatter(value);
    }
    
    // Default formatting
    std::ostringstream oss;
    if (std::holds_alternative<int>(value)) {
        oss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        oss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        oss << std::fixed << std::setprecision(6) << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        oss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        oss << std::get<std::string>(value);
    }
    
    return oss.str();
}

std::vector<std::string> ConfigSchema::getFieldKeys() const {
    std::vector<std::string> keys;
    keys.reserve(fields_.size());
    for (const auto& pair : fields_) {
        keys.push_back(pair.first);
    }
    return keys;
}

size_t ConfigSchema::getValidatorCount() const {
    size_t count = 0;
    for (const auto& pair : fields_) {
        count += pair.second.validators.size();
    }
    return count;
}

size_t ConfigSchema::getCrossValidatorCount() const {
    return crossValidators_.size();
}

std::string ConfigSchema::getInfo() const {
    std::ostringstream oss;
    oss << "Schema: " << name_ << "\n";
    oss << "Fields: " << fields_.size() << "\n";
    oss << "Validators: " << getValidatorCount() << "\n";
    oss << "Cross-validators: " << getCrossValidatorCount() << "\n";
    
    if (!fields_.empty()) {
        oss << "\nField details:\n";
        for (const auto& pair : fields_) {
            oss << "  " << pair.first << " (" << pair.second.type << ")";
            if (pair.second.required) oss << " [required]";
            if (pair.second.hasDefault) {
                oss << " [default: " << formatValue(pair.first, pair.second.defaultValue) << "]";
            }
            if (!pair.second.description.empty()) {
                oss << " - " << pair.second.description;
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

// ConfigSchemaRegistry implementation
ConfigSchemaRegistry& ConfigSchemaRegistry::getInstance() {
    static ConfigSchemaRegistry instance;
    return instance;
}

void ConfigSchemaRegistry::registerSchema(const std::string& name, std::shared_ptr<ConfigSchema> schema) {
    if (!schema) {
        throw ConfigSchemaError("Schema cannot be null", "ConfigSchema.hpp", __LINE__);
    }
    schemas_[name] = schema;
}

std::shared_ptr<ConfigSchema> ConfigSchemaRegistry::getSchema(const std::string& name) const {
    auto it = schemas_.find(name);
    if (it != schemas_.end()) {
        return it->second;
    }
    return nullptr;
}

bool ConfigSchemaRegistry::hasSchema(const std::string& name) const {
    return schemas_.find(name) != schemas_.end();
}

void ConfigSchemaRegistry::removeSchema(const std::string& name) {
    schemas_.erase(name);
}

std::vector<std::string> ConfigSchemaRegistry::getSchemaNames() const {
    std::vector<std::string> names;
    names.reserve(schemas_.size());
    for (const auto& pair : schemas_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> ConfigSchemaRegistry::validate(const std::string& schemaName,
                                                      const std::unordered_map<std::string, std::variant<int, int64_t, double, bool, std::string>>& values) const {
    auto schema = getSchema(schemaName);
    if (!schema) {
        throw ConfigSchemaError("Unknown schema: " + schemaName, "ConfigSchema.hpp", __LINE__);
    }
    return schema->validate(values);
}

void ConfigSchemaRegistry::clear() {
    schemas_.clear();
}

} // namespace nlm