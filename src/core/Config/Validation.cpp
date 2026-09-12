// NLM Configuration Validation and Schema System
// Validates configuration against schema and provides helpful error messages

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <any>

namespace nlm {

// Configuration schema field types
enum class ConfigFieldType {
    Integer,
    Float,
    String,
    Boolean,
    VectorInt,
    VectorFloat,
    VectorString,
    Enum,
    Object
};

// Configuration schema field definition
struct ConfigSchemaField {
    std::string name;
    ConfigFieldType type;
    std::any defaultValue;
    bool required;
    std::string description;
    std::string validationRegex;
    std::vector<std::string> allowedValues;
    
    ConfigSchemaField() : required(false) {}
    ConfigSchemaField(const std::string& name, ConfigFieldType type, 
                     const std::any& defaultVal, bool req, const std::string& desc)
        : name(name), type(type), defaultValue(defaultVal), required(req), description(desc) {}
};

// Complete configuration schema for NLM
class NLMConfigSchema {
public:
    NLMConfigSchema() {
        initializeDefaultSchema();
    }
    
    // Validate configuration against schema
    std::vector<std::string> validate(const Config& config) const {
        std::vector<std::string> errors;
        
        // Check required fields
        for (const auto& field : schema_) {
            if (field.required && !config.has(field.name)) {
                errors.push_back("Missing required field: " + field.name + ". " + field.description);
            }
        }
        
        // Validate field values
        for (const auto& entry : config.getKeys()) {
            auto valueOpt = config.get<std::string>(entry);
            if (!valueOpt) continue;
            
            auto fieldIt = getFieldByName(entry);
            if (fieldIt) {
                std::vector<std::string> fieldErrors = validateField(entry, *fieldIt, *valueOpt);
                errors.insert(errors.end(), fieldErrors.begin(), fieldErrors.end());
            }
        }
        
        return errors;
    }
    
    // Get all field names
    std::vector<std::string> getFieldNames() const {
        std::vector<std::string> names;
        for (const auto& field : schema_) {
            names.push_back(field.name);
        }
        return names;
    }
    
    // Get field by name
    const ConfigSchemaField* getFieldByName(const std::string& name) const {
        for (const auto& field : schema_) {
            if (field.name == name) {
                return &field;
            }
        }
        return nullptr;
    }
    
    // Get validation suggestions for errors
    std::string getValidationSuggestions(const std::string& error) const {
        if (error.find("Missing required field") != std::string::npos) {
            return "Ensure all required configuration parameters are specified.";
        } else if (error.find("invalid") != std::string::npos) {
            return "Check the value format and allowed values for the field.";
        }
        return "Review the field description and constraints.";
    }
    
    // Generate default configuration
    void applyDefaults(Config& config) const {
        for (const auto& field : schema_) {
            if (field.required && !config.has(field.name)) {
                applyDefaultValue(config, field);
            }
        }
    }
    
    // Get field type name as string
    std::string fieldTypeToString(ConfigFieldType type) const {
        switch (type) {
            case ConfigFieldType::Integer: return "integer";
            case ConfigFieldType::Float: return "float";
            case ConfigFieldType::String: return "string";
            case ConfigFieldType::Boolean: return "boolean";
            case ConfigFieldType::VectorInt: return "vector<int>";
            case ConfigFieldType::VectorFloat: return "vector<float>";
            case ConfigFieldType::VectorString: return "vector<string>";
            case ConfigFieldType::Enum: return "enum";
            case ConfigFieldType::Object: return "object";
            default: return "unknown";
        }
    }
    
    // Print schema documentation
    void printSchema() const {
        std::cout << "=== NLM Configuration Schema ===" << std::endl;
        std::cout << std::endl;
        
        for (const auto& field : schema_) {
            std::cout << field.name << " (" << fieldTypeToString(field.type) << ")";
            if (field.required) std::cout << " [required]";
            std::cout << std::endl;
            std::cout << "  Description: " << field.description << std::endl;
            
            if (!field.allowedValues.empty()) {
                std::cout << "  Allowed values: ";
                for (size_t i = 0; i < field.allowedValues.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << field.allowedValues[i];
                }
                std::cout << std::endl;
            }
            
            if (!field.validationRegex.empty()) {
                std::cout << "  Validation regex: " << field.validationRegex << std::endl;
            }
            
            std::cout << std::endl;
        }
    }

private:
    std::vector<ConfigSchemaField> schema_;
    
    void initializeDefaultSchema() {
        // Brain Configuration
        schema_.push_back(ConfigSchemaField(
            "brain.neuron_count", ConfigFieldType::Integer, 
            static_cast<std::any>(1000), true,
            "Total number of neurons in the brain"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "brain.synapse_density", ConfigFieldType::Float,
            static_cast<std::any>(0.1f), false,
            "Synapse connectivity density (0.0 to 1.0)"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "brain.connection_probability", ConfigFieldType::Float,
            static_cast<std::any>(0.05f), false,
            "Probability of connection between neurons (0.0 to 1.0)"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "brain.initial_weight_mean", ConfigFieldType::Float,
            static_cast<std::any>(0.5f), false,
            "Mean initial synaptic weight"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "brain.initial_weight_std", ConfigFieldType::Float,
            static_cast<std::any>(0.1f), false,
            "Standard deviation of initial weights"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "brain.v_thresh", ConfigFieldType::Float,
            static_cast<std::any>(-50.0f), false,
            "Neuron threshold potential (mV)"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "brain.v_rest", ConfigFieldType::Float,
            static_cast<std::any>(-70.0f), false,
            "Resting potential (mV)"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "brain.v_reset", ConfigFieldType::Float,
            static_cast<std::any>(-75.0f), false,
            "Reset potential after spike (mV)"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "brain.tau_mem", ConfigFieldType::Float,
            static_cast<std::any>(20.0f), false,
            "Membrane time constant (ms)"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "brain.tau_ref", ConfigFieldType::Float,
            static_cast<std::any>(2.0f), false,
            "Refractory period (ms)"
        ));
        
        // Plasticity Configuration
        schema_.push_back(ConfigSchemaField(
            "plasticity.stdp.enable", ConfigFieldType::Boolean,
            static_cast<std::any>(true), false,
            "Enable Spike-Timing-Dependent Plasticity"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "plasticity.stdp.learning_rate", ConfigFieldType::Float,
            static_cast<std::any>(0.001f), false,
            "STDP learning rate"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "plasticity.stdp.tau_plus", ConfigFieldType::Float,
            static_cast<std::any>(20.0f), false,
            "STDP time constant for potentiation (ms)"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "plasticity.stdp.tau_minus", ConfigFieldType::Float,
            static_cast<std::any>(20.0f), false,
            "STDP time constant for depression (ms)"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "plasticity.hebbian.enable", ConfigFieldType::Boolean,
            static_cast<std::any>(true), false,
            "Enable Hebbian learning"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "plasticity.structural.enable", ConfigFieldType::Boolean,
            static_cast<std::any>(true), false,
            "Enable structural plasticity"
        ));
        
        // Neuromodulation Configuration
        schema_.push_back(ConfigSchemaField(
            "neuromod.dopamine.scale", ConfigFieldType::Float,
            static_cast<std::any>(1.0f), false,
            "Dopamine modulation scale factor"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "neuromod.curiosity.enable", ConfigFieldType::Boolean,
            static_cast<std::any>(true), false,
            "Enable curiosity-driven exploration"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "neuromod.novelty.enable", ConfigFieldType::Boolean,
            static_cast<std::any>(true), false,
            "Enable novelty detection"
        ));
        
        // World Configuration
        schema_.push_back(ConfigSchemaField(
            "world.width", ConfigFieldType::Integer,
            static_cast<std::any>(20), false,
            "World width in cells"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "world.height", ConfigFieldType::Integer,
            static_cast<std::any>(20), false,
            "World height in cells"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "world.vision_width", ConfigFieldType::Integer,
            static_cast<std::any>(8), false,
            "Vision sensor width"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "world.vision_height", ConfigFieldType::Integer,
            static_cast<std::any>(8), false,
            "Vision sensor height"
        ));
        
        // Simulation Configuration
        schema_.push_back(ConfigSchemaField(
            "simulation.timestep", ConfigFieldType::Float,
            static_cast<std::any>(0.001f), false,
            "Simulation timestep (seconds)"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "simulation.max_steps", ConfigFieldType::Integer,
            static_cast<std::any>(10000), false,
            "Maximum simulation steps"
        ));
        
        // Development Configuration
        schema_.push_back(ConfigSchemaField(
            "development.initial_stage", ConfigFieldType::String,
            static_cast<std::any>(std::string("Initial")), false,
            "Initial developmental stage"
        ));
        
        schema_.push_back(ConfigSchemaField(
            "development.maturation_rate", ConfigFieldType::Float,
            static_cast<std::any>(0.01f), false,
            "Rate of developmental maturation"
        ));
    }
    
    std::vector<std::string> validateField(const std::string& key, 
                                          const ConfigSchemaField& field, 
                                          const std::string& value) const {
        std::vector<std::string> errors;
        
        // Basic type checking
        switch (field.type) {
            case ConfigFieldType::Integer: {
                try {
                    size_t pos;
                    std::stoi(value, pos);
                    if (pos != value.size()) errors.push_back("Field " + field.name + 
                        " must be an integer");
                } catch (...) {
                    errors.push_back("Field " + field.name + " must be an integer");
                }
                break;
            }
            case ConfigFieldType::Float: {
                try {
                    size_t pos;
                    std::stof(value, pos);
                    if (pos != value.size()) errors.push_back("Field " + field.name + 
                        " must be a float");
                } catch (...) {
                    errors.push_back("Field " + field.name + " must be a float");
                }
                break;
            }
            case ConfigFieldType::Boolean: {
                if (value != "true" && value != "false" && value != "0" && value != "1") {
                    errors.push_back("Field " + field.name + " must be a boolean (true/false)");
                }
                break;
            }
            case ConfigFieldType::String: {
                // Basic validation
                break;
            }
            case ConfigFieldType::Enum: {
                if (std::find(field.allowedValues.begin(), field.allowedValues.end(), value) == 
                    field.allowedValues.end()) {
                    errors.push_back("Field " + field.name + " must be one of: ");
                    for (size_t i = 0; i < field.allowedValues.size(); ++i) {
                        if (i > 0) errors.back() += ", ";
                        errors.back() += field.allowedValues[i];
                    }
                }
                break;
            }
        }
        
        // Range validation for numeric types
        if (errors.empty()) {
            try {
                if (field.type == ConfigFieldType::Float) {
                    float val = std::stof(value);
                    if (val < 0.0f && field.name.find("rate") != std::string::npos) {
                        errors.push_back("Field " + field.name + " must be non-negative");
                    }
                } else if (field.type == ConfigFieldType::Integer) {
                    int val = std::stoi(value);
                    if (val < 0 && (field.name.find("count") != std::string::npos || 
                                   field.name.find("steps") != std::string::npos)) {
                        errors.push_back("Field " + field.name + " must be non-negative");
                    }
                }
            } catch (...) {
                // Ignore if value is not numeric
            }
        }
        
        return errors;
    }
    
    void applyDefaultValue(Config& config, const ConfigSchemaField& field) const {
        try {
            if (field.type == ConfigFieldType::Integer) {
                int val = std::any_cast<int>(field.defaultValue);
                config.set(field.name, val);
            } else if (field.type == ConfigFieldType::Float) {
                float val = std::any_cast<float>(field.defaultValue);
                config.set(field.name, val);
            } else if (field.type == ConfigFieldType::Boolean) {
                bool val = std::any_cast<bool>(field.defaultValue);
                config.set(field.name, val);
            } else if (field.type == ConfigFieldType::String) {
                std::string val = std::any_cast<std::string>(field.defaultValue);
                config.set(field.name, val);
            }
        } catch (...) {
            // If default value application fails, skip silently
        }
    }
};

// Configuration validation utilities
namespace config_validation {

    // Validate configuration keys
    std::vector<std::string> validateKeys(const std::vector<std::string>& keys) {
        std::vector<std::string> invalidKeys;
        NLMConfigSchema schema;
        
        for (const auto& key : keys) {
            if (schema.getFieldByName(key) == nullptr) {
                invalidKeys.push_back(key);
            }
        }
        
        return invalidKeys;
    }
    
    // Get validation report
    std::string generateValidationReport(const std::vector<std::string>& errors, 
                                         const std::vector<std::string>& warnings) {
        std::ostringstream report;
        report << "=== Configuration Validation Report ===" << std::endl << std::endl;
        
        if (errors.empty() && warnings.empty()) {
            report << "✓ Configuration is valid!" << std::endl;
            return report.str();
        }
        
        if (!errors.empty()) {
            report << "Errors:" << std::endl;
            for (const auto& error : errors) {
                report << "  ✗ " << error << std::endl;
                report << "    -> " << error.substr(0, error.find('.') + 1) 
                       << " " << schema.getValidationSuggestions(error) << std::endl;
            }
            report << std::endl;
        }
        
        if (!warnings.empty()) {
            report << "Warnings:" << std::endl;
            for (const auto& warning : warnings) {
                report << "  ⚠ " << warning << std::endl;
            }
            report << std::endl;
        }
        
        report << "Use 'config help' for more information about valid fields.";
        return report.str();
    }
    
    // Get NLM schema instance
    inline NLMConfigSchema schema;
}

} // namespace nlm
