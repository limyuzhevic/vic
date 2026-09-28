// ConfigSchema.cpp - Implementation of configuration schema validation
#include "ConfigSchema.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

bool ConfigSchema::validate(const std::unordered_map<std::string, ConfigValue>& config) const {
    std::vector<std::string> errors = getErrors(config);
    if (!errors.empty()) {
        for (const auto& error : errors) {
            NLM_LOG_ERROR("Configuration validation error: " + error);
        }
        return false;
    }
    return true;
}

std::vector<std::string> ConfigSchema::getErrors(const std::unordered_map<std::string, ConfigValue>& config) const {
    std::vector<std::string> errors;
    
    // Check for missing required fields
    for (const auto& field : fields) {
        if (field.required && config.find(field.name) == config.end()) {
            errors.push_back("Missing required field: " + field.name);
        }
    }
    
    // Validate field values
    for (const auto& pair : config) {
        const auto& fieldName = pair.first;
        const auto& value = pair.second;
        
        // Find corresponding field definition
        const ConfigField* field = getField(fieldName);
        if (!field) {
            errors.push_back("Unknown configuration field: " + fieldName);
            continue;
        }
        
        // Type validation
        if (!ConfigValidator::validateType(value, field->type)) {
            errors.push_back("Invalid type for field " + fieldName + ": expected " + field->type + ", got " + ConfigValidator::getValueTypeName(value));
        }
        
        // Range validation
        if (field->minValue != 0 || field->maxValue != 0) {
            if (!ConfigValidator::validateRange(value, field->minValue, field->maxValue)) {
                errors.push_back("Field " + fieldName + " value " + std::to_string(std::get<double>(value)) + 
                               " is outside valid range [" + std::to_string(field->minValue) + ", " + std::to_string(field->maxValue) + "]");
            }
        }
        
        // Custom validator
        if (field->validator && !field->validator(value)) {
            errors.push_back("Field " + fieldName + " validation failed");
        }
    }
    
    return errors;
}

const ConfigField* ConfigSchema::getField(const std::string& name) const {
    for (const auto& field : fields) {
        if (field.name == name) {
            return &field;
        }
    }
    return nullptr;
}

std::vector<ConfigField> ConfigSchema::getFieldsByCategory(const std::string& category) const {
    std::vector<ConfigField> categoryFields;
    for (const auto& field : fields) {
        if (field.category == category) {
            categoryFields.push_back(field);
        }
    }
    return categoryFields;
}

void ConfigSchema::addField(const ConfigField& field) {
    // Check if field already exists
    for (auto it = fields.begin(); it != fields.end(); ++it) {
        if (it->name == field.name) {
            *it = field;
            return;
        }
    }
    fields.push_back(field);
}

bool ConfigSchema::removeField(const std::string& name) {
    for (auto it = fields.begin(); it != fields.end(); ++it) {
        if (it->name == name) {
            fields.erase(it);
            return true;
        }
    }
    return false;
}

void ConfigSchema::clear() {
    fields.clear();
}

std::string ConfigSchema::toJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"version\": \"" << version << "\",\n";
    ss << "  \"description\": \"" << description << "\",\n";
    ss << "  \"fields\": [\n";
    
    for (size_t i = 0; i < fields.size(); ++i) {
        const auto& field = fields[i];
        ss << "    {\n";
        ss << "      \"name\": \"" << field.name << "\",\n";
        ss << "      \"type\": \"" << field.type << "\",\n";
        ss << "      \"required\": " << (field.required ? "true" : "false") << ",\n";
        ss << "      \"description\": \"" << field.description << "\",\n";
        ss << "      \"category\": \"" << field.category << "\"\n";
        if (i < fields.size() - 1) {
            ss << "    },\n";
        } else {
            ss << "    }\n";
        }
    }
    
    ss << "  ]\n";
    ss << "}\n";
    
    return ss.str();
}

bool ConfigSchema::fromJSON(const std::string& json) {
    // Simple JSON parsing - would use a proper JSON library in production
    // This is a placeholder implementation
    clear();
    
    // TODO: Implement proper JSON parsing
    NLM_LOG_WARNING("ConfigSchema::fromJSON not fully implemented - using simplified parsing");
    
    // Basic field parsing for common fields
    if (json.find("neuron_count") != std::string::npos) {
        ConfigField field;
        field.name = "neuron_count";
        field.type = "uint64";
        field.defaultValue = uint64_t(1000);
        field.required = false;
        field.description = "Number of neurons in the brain";
        field.category = "brain";
        field.minValue = 1;
        field.maxValue = 100000;
        addField(field);
    }
    
    if (json.find("region_count") != std::string::npos) {
        ConfigField field;
        field.name = "region_count";
        field.type = "uint64";
        field.defaultValue = uint64_t(1);
        field.required = false;
        field.description = "Number of brain regions";
        field.category = "brain";
        field.minValue = 1;
        field.maxValue = 1000;
        addField(field);
    }
    
    return true;
}

// ConfigSchemas implementation
ConfigSchema ConfigSchemas::getDefaultSchema() {
    ConfigSchema schema;
    schema.version = "1.0";
    schema.description = "Default NLM configuration schema";
    
    // Brain configuration fields
    ConfigField neuronCountField;
    neuronCountField.name = "neuron_count";
    neuronCountField.type = "uint64";
    neuronCountField.defaultValue = uint64_t(1000);
    neuronCountField.required = false;
    neuronCountField.description = "Number of neurons in the brain";
    neuronCountField.category = "brain";
    neuronCountField.minValue = 1;
    neuronCountField.maxValue = 100000;
    neuronCountField.displayName = "Neuron Count";
    schema.addField(neuronCountField);
    
    ConfigField regionCountField;
    regionCountField.name = "region_count";
    regionCountField.type = "uint64";
    regionCountField.defaultValue = uint64_t(1);
    regionCountField.required = false;
    regionCountField.description = "Number of brain regions";
    regionCountField.category = "brain";
    regionCountField.minValue = 1;
    regionCountField.maxValue = 1000;
    regionCountField.displayName = "Region Count";
    schema.addField(regionCountField);
    
    ConfigField connectionProbField;
    connectionProbField.name = "connection_probability";
    connectionProbField.type = "float";
    connectionProbField.defaultValue = 0.1f;
    connectionProbField.required = false;
    connectionProbField.description = "Probability of synaptic connections between neurons";
    connectionProbField.category = "brain";
    connectionProbField.minValue = 0;
    connectionProbField.maxValue = 1;
    connectionProbField.displayName = "Connection Probability";
    schema.addField(connectionProbField);
    
    // Development fields
    ConfigField devStageField;
    devStageField.name = "development_stage";
    devStageField.type = "string";
    devStageField.defaultValue = std::string("Adult");
    devStageField.required = false;
    devStageField.description = "Current developmental stage";
    devStageField.category = "development";
    devStageField.displayName = "Development Stage";
    schema.addField(devStageField);
    
    ConfigField plasticityField;
    plasticityField.name = "plasticity_enabled";
    plasticityField.type = "bool";
    plasticityField.defaultValue = true;
    plasticityField.required = false;
    plasticityField.description = "Enable synaptic plasticity";
    plasticityField.category = "development";
    plasticityField.displayName = "Plasticity Enabled";
    schema.addField(plasticityField);
    
    return schema;
}

ConfigSchema ConfigSchemas::getDevelopmentSchema() {
    ConfigSchema schema = getDefaultSchema();
    schema.version = "1.0";
    schema.description = "Development-focused NLM configuration schema";
    
    // Add development-specific fields
    ConfigField synaptogenesisField;
    synaptogenesisField.name = "synaptogenesis_rate";
    synaptogenesisField.type = "float";
    synaptogenesisField.defaultValue = 0.0001f;
    synaptogenesisField.required = false;
    synaptogenesisField.description = "Rate of new synapse formation";
    synaptogenesisField.category = "development";
    synaptogenesisField.minValue = 0;
    synaptogenesisField.maxValue = 1;
    schema.addField(synaptogenesisField);
    
    ConfigField pruningField;
    pruningField.name = "pruning_rate";
    pruningField.type = "float";
    pruningField.defaultValue = 0.00001f;
    pruningField.required = false;
    pruningField.description = "Rate of synapse elimination";
    pruningField.category = "development";
    pruningField.minValue = 0;
    pruningField.maxValue = 1;
    schema.addField(pruningField);
    
    return schema;
}

ConfigSchema ConfigSchemas::getReinforcementLearningSchema() {
    ConfigSchema schema = getDefaultSchema();
    schema.version = "1.0";
    schema.description = "Reinforcement learning focused NLM configuration schema";
    
    // Add neuromodulation fields
    ConfigField rewardModField;
    rewardModField.name = "reward_modulation_enabled";
    rewardModField.type = "bool";
    rewardModField.defaultValue = true;
    rewardModField.required = false;
    rewardModField.description = "Enable reward-based neuromodulation";
    rewardModField.category = "neuromodulation";
    rewardModField.displayName = "Reward Modulation Enabled";
    schema.addField(rewardModField);
    
    ConfigField curiosityField;
    curiosityField.name = "curiosity_enabled";
    curiosityField.type = "bool";
    curiosityField.defaultValue = true;
    curiosityField.required = false;
    curiosityField.description = "Enable curiosity-driven exploration";
    curiosityField.category = "neuromodulation";
    curiosityField.displayName = "Curiosity Enabled";
    schema.addField(curiosityField);
    
    return schema;
}

ConfigSchema ConfigSchemas::getPerformanceSchema() {
    ConfigSchema schema = getDefaultSchema();
    schema.version = "1.0";
    schema.description = "Performance-optimized NLM configuration schema";
    
    // Add performance fields
    ConfigField timestepField;
    timestepField.name = "simulation_timestep";
    timestepField.type = "double";
    timestepField.defaultValue = 0.001;
    timestepField.required = false;
    timestepField.description = "Simulation timestep duration";
    timestepField.category = "performance";
    timestepField.minValue = 0;
    timestepField.maxValue = 1.0;
    timestepField.displayName = "Timestep (seconds)";
    schema.addField(timestepField);
    
    ConfigField checkpointField;
    checkpointField.name = "checkpoint_interval";
    checkpointField.type = "uint64";
    checkpointField.defaultValue = uint64_t(1000);
    checkpointField.required = false;
    checkpointField.description = "Interval between checkpoints (in simulation steps)";
    checkpointField.category = "performance";
    checkpointField.minValue = 1;
    checkpointField.maxValue = 100000;
    schema.addField(checkpointField);
    
    return schema;
}

ConfigSchema ConfigSchemas::getMinimalSchema() {
    ConfigSchema schema;
    schema.version = "1.0";
    schema.description = "Minimal NLM configuration schema (fewest fields)";
    
    // Only essential fields
    ConfigField neuronCountField;
    neuronCountField.name = "neuron_count";
    neuronCountField.type = "uint64";
    neuronCountField.defaultValue = uint64_t(100);
    neuronCountField.required = false;
    neuronCountField.description = "Number of neurons";
    neuronCountField.category = "brain";
    schema.addField(neuronCountField);
    
    return schema;
}

bool ConfigSchemas::validateConfig(const std::unordered_map<std::string, ConfigValue>& config,
                                  const std::string& schemaName) {
    ConfigSchema schema;
    if (schemaName == "development") {
        schema = getDevelopmentSchema();
    } else if (schemaName == "reinforcement_learning") {
        schema = getReinforcementLearningSchema();
    } else if (schemaName == "performance") {
        schema = getPerformanceSchema();
    } else if (schemaName == "minimal") {
        schema = getMinimalSchema();
    } else {
        schema = getDefaultSchema();
    }
    
    return schema.validate(config);
}

std::vector<std::string> ConfigSchemas::getValidationErrors(
    const std::unordered_map<std::string, ConfigValue>& config,
    const std::string& schemaName) {
    ConfigSchema schema;
    if (schemaName == "development") {
        schema = getDevelopmentSchema();
    } else if (schemaName == "reinforcement_learning") {
        schema = getReinforcementLearningSchema();
    } else if (schemaName == "performance") {
        schema = getPerformanceSchema();
    } else if (schemaName == "minimal") {
        schema = getMinimalSchema();
    } else {
        schema = getDefaultSchema();
    }
    
    return schema.getErrors(config);
}

std::unordered_map<std::string, ConfigValue> ConfigSchemas::getRecommendedConfig(const std::string& useCase) {
    std::unordered_map<std::string, ConfigValue> config;
    
    if (useCase == "development") {
        config["neuron_count"] = uint64_t(2000);
        config["region_count"] = uint64_t(2);
        config["connection_probability"] = 0.15f;
        config["development_stage"] = std::string("CriticalPeriod");
        config["plasticity_enabled"] = true;
        config["synaptogenesis_rate"] = 0.0001f;
        config["pruning_rate"] = 0.00001f;
    } else if (useCase == "reinforcement_learning") {
        config["neuron_count"] = uint64_t(1500);
        config["region_count"] = uint64_t(2);
        config["connection_probability"] = 0.2f;
        config["reward_modulation_enabled"] = true;
        config["curiosity_enabled"] = true;
        config["development_stage"] = std::string("Adult");
    } else if (useCase == "performance") {
        config["neuron_count"] = uint64_t(500);
        config["region_count"] = uint64_t(1);
        config["connection_probability"] = 0.05f;
        config["simulation_timestep"] = 0.001;
        config["checkpoint_interval"] = uint64_t(1000);
    } else {
        config["neuron_count"] = uint64_t(1000);
        config["region_count"] = uint64_t(1);
        config["connection_probability"] = 0.1f;
    }
    
    return config;
}

// ConfigTemplate implementation
void ConfigTemplate::setTemplateVariable(const std::string& name, const ConfigValue& value) {
    templateVariables_[name] = value;
}

ConfigValue ConfigTemplate::getTemplateVariable(const std::string& name) const {
    auto it = templateVariables_.find(name);
    if (it != templateVariables_.end()) {
        return it->second;
    }
    return std::monostate();
}

void ConfigTemplate::addSubstitution(const std::string& pattern, const std::string& replacement) {
    substitutions_.push_back({pattern, replacement});
}

std::string ConfigTemplate::applySubstitutions(const std::string& input) const {
    std::string result = input;
    for (const auto& sub : substitutions_) {
        size_t pos = 0;
        while ((pos = result.find(sub.first, pos)) != std::string::npos) {
            result.replace(pos, sub.first.length(), sub.second);
            pos += sub.second.length();
        }
    }
    return result;
}

std::unordered_map<std::string, ConfigValue> ConfigTemplate::generateConfig() const {
    // Apply template variables and substitutions
    std::unordered_map<std::string, ConfigValue> config;
    
    // Add template variables
    for (const auto& pair : templateVariables_) {
        config[pair.first] = pair.second;
    }
    
    return config;
}

void ConfigTemplate::clear() {
    templateVariables_.clear();
    substitutions_.clear();
}

bool ConfigTemplate::isEmpty() const {
    return templateVariables_.empty() && substitutions_.empty();
}

std::string ConfigTemplate::toJSON() const {
    // Placeholder - would implement proper JSON serialization
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"variables\": {\n";
    for (size_t i = 0; i < templateVariables_.size(); ++i) {
        const auto& pair = *std::next(templateVariables_.begin(), i);
        ss << "    \"" << pair.first << "\": \"" << "value_placeholder\"";
        if (i < templateVariables_.size() - 1) {
            ss << ",\n";
        } else {
            ss << "\n";
        }
    }
    ss << "  },\n";
    ss << "  \"substitutions\": []\n";
    ss << "}\n";
    return ss.str();
}

bool ConfigTemplate::fromJSON(const std::string& json) {
    // Placeholder - would implement proper JSON parsing
    clear();
    NLM_LOG_WARNING("ConfigTemplate::fromJSON not fully implemented");
    return true;
}

// ConfigValidator implementation
bool ConfigValidator::validate(const std::unordered_map<std::string, ConfigValue>& config,
                              const ConfigSchema& schema) {
    return schema.validate(config);
}

std::vector<std::string> ConfigValidator::getAllValidationErrors(
    const std::unordered_map<std::string, ConfigValue>& config,
    const ConfigSchema& schema) {
    return schema.getErrors(config);
}

bool ConfigValidator::validateType(const ConfigValue& value, const std::string& typeName) {
    switch (value.index()) {
        case 0: // std::monostate (null)
            return typeName == "null" || typeName == "string" || typeName == "bool" || 
                   typeName == "int" || typeName == "uint" || typeName == "float" || 
                   typeName == "double" || typeName == "bool_array" || typeName == "int_array" ||
                   typeName == "uint_array" || typeName == "float_array" || typeName == "string_array";
                   
        case 1: // bool
            return typeName == "bool";
            
        case 2: // int64_t
            return typeName == "int" || typeName == "uint" || typeName == "float" || typeName == "double";
            
        case 3: // uint64_t
            return typeName == "uint" || typeName == "float" || typeName == "double";
            
        case 4: // double
            return typeName == "float" || typeName == "double";
            
        case 5: // std::string
            return typeName == "string" || typeName == "bool_array" || typeName == "int_array" ||
                   typeName == "uint_array" || typeName == "float_array" || typeName == "string_array";
            
        case 6: // std::vector<bool>
            return typeName == "bool_array";
            
        case 7: // std::vector<int64_t>
            return typeName == "int_array" || typeName == "uint_array" || typeName == "float_array";
            
        case 8: // std::vector<uint64_t>
            return typeName == "uint_array" || typeName == "float_array";
            
        case 9: // std::vector<double>
            return typeName == "float_array" || typeName == "double";
            
        case 10: // std::vector<std::string>
            return typeName == "string_array";
            
        default:
            return false;
    }
}

std::string ConfigValidator::getValueTypeName(const ConfigValue& value) {
    switch (value.index()) {
        case 0: return "null";
        case 1: return "bool";
        case 2: return "int";
        case 3: return "uint";
        case 4: return "float";
        case 5: return "string";
        case 6: return "bool_array";
        case 7: return "int_array";
        case 8: return "uint_array";
        case 9: return "float_array";
        case 10: return "string_array";
        default: return "unknown";
    }
}

bool ConfigValidator::validateRange(const ConfigValue& value, int min, int max) {
    if (value.index() == 2) { // int64_t
        int64_t val = std::get<int64_t>(value);
        return val >= min && val <= max;
    } else if (value.index() == 3) { // uint64_t
        uint64_t val = std::get<uint64_t>(value);
        return val >= static_cast<uint64_t>(min) && val <= static_cast<uint64_t>(max);
    } else if (value.index() == 4) { // double
        double val = std::get<double>(value);
        return val >= min && val <= max;
    }
    return false;
}

bool ConfigValidator::validateRange(const ConfigValue& value, double min, double max) {
    if (value.index() == 2) { // int64_t
        double val = static_cast<double>(std::get<int64_t>(value));
        return val >= min && val <= max;
    } else if (value.index() == 3) { // uint64_t
        double val = static_cast<double>(std::get<uint64_t>(value));
        return val >= min && val <= max;
    } else if (value.index() == 4) { // double
        double val = std::get<double>(value);
        return val >= min && val <= max;
    }
    return false;
}

bool ConfigValidator::validateArraySize(const ConfigValue& value, size_t expectedSize) {
    if (value.index() == 6) { // std::vector<bool>
        return std::get<std::vector<bool>>(value).size() == expectedSize;
    } else if (value.index() == 7) { // std::vector<int64_t>
        return std::get<std::vector<int64_t>>(value).size() == expectedSize;
    } else if (value.index() == 8) { // std::vector<uint64_t>
        return std::get<std::vector<uint64_t>>(value).size() == expectedSize;
    } else if (value.index() == 9) { // std::vector<double>
        return std::get<std::vector<double>>(value).size() == expectedSize;
    } else if (value.index() == 10) { // std::vector<std::string>
        return std::get<std::vector<std::string>>(value).size() == expectedSize;
    }
    return false;
}

bool ConfigValidator::validateArrayType(const ConfigValue& value, const std::string& elementType) {
    if (elementType == "bool" && value.index() == 6) return true;
    if (elementType == "int" && value.index() == 7) return true;
    if (elementType == "uint" && value.index() == 8) return true;
    if (elementType == "float" && value.index() == 9) return true;
    if (elementType == "string" && value.index() == 10) return true;
    return false;
}
}