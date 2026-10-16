// **Configuration Validation**
// 
// Exception types for validation errors
// 
#include <string>

// Base validation exception
struct ValidationException : public std::exception {
    std::string message;
    
    explicit ValidationException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

// Specific validation exception types
struct RequiredFieldError : public ValidationException {
    std::string field;
    
    RequiredFieldError(const std::string& field) 
        : ValidationException("Required field \"" + field + "\" is missing"),
          field(field) {}
};

struct TypeMismatchError : public ValidationException {
    std::string field;
    std::string expectedType;
    std::string actualType;
    
    TypeMismatchError(const std::string& field, const std::string& expected, const std::string& actual)
        : ValidationException("Field \"" + field + "\" has type \"" + actual + "\", expected \"" + expected + "\""),
          field(field), expectedType(expected), actualType(actual) {}
};

struct RangeValidationError : public ValidationException {
    std::string field;
    std::string constraint;
    double value;
    
    RangeValidationError(const std::string& field, const std::string& constraint, double value)
        : ValidationException("Field \"" + field + "\" value " + std::to_string(value) + " violates constraint: \"" + constraint + "\""),
          field(field), constraint(constraint), value(value) {}
};

struct EnumValidationError : public ValidationException {
    std::string field;
    std::string value;
    std::string allowedValues;
    
    EnumValidationError(const std::string& field, const std::string& value, const std::string& allowed)
        : ValidationException("Field \"" + field + "\" value \"" + value + "\" is not in allowed values: \"" + allowed + "\""),
          field(field), value(value), allowedValues(allowed) {}
};

struct FormatValidationError : public ValidationException {
    std::string field;
    std::string format;
    std::string value;
    
    FormatValidationError(const std::string& field, const std::string& format, const std::string& value)
        : ValidationException("Field \"" + field + "\" value \"" + value + "\" does not match format \"" + format + "\""),
          field(field), format(format), value(value) {}
};

// Validation result aggregation
struct ValidationResult {
    bool isValid = true;
    std::vector<std::shared_ptr<ValidationException>> errors;
    std::vector<std::string> warnings;
    
    void addError(const std::shared_ptr<ValidationException>& error) {
        isValid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string toString() const {
        std::ostringstream oss;
        if (!isValid) {
            oss << "Validation failed with " << errors.size() << " error(s):\n";
            for (const auto& error : errors) {
                oss << "  - " << error->what() << "\n";
            }
        }
        if (!warnings.empty()) {
            oss << "Warnings: \n";
            for (const auto& warning : warnings) {
                oss << "  - " << warning << "\n";
            }
        }
        return oss.str();
    }
};

// Validation mode
enum class ValidationMode {
    Strict,   // Fail validation if any errors occur
    Warning,  // Warn on errors but allow invalid configuration
    Off       // No validation
};

// Configuration constraint types
enum class ConstraintType {
    None,
    Range,
    Enum,
    Required,
    Format,
    Custom
};

// Base constraint class
class Constraint {
public:
    virtual ~Constraint() = default;
    virtual ConstraintType getType() const = 0;
    virtual bool validate(const ConfigValue& value, ValidationResult& result) const = 0;
    virtual std::string getDescription() const = 0;
};

// Range constraint for numeric values
class RangeConstraint : public Constraint {
private:
    double minValue;
    double maxValue;
    std::string constraintDescription;
    
public:
    RangeConstraint(double min, double max, const std::string& desc = "") 
        : minValue(min), maxValue(max), constraintDescription(desc) {}
    
    ConstraintType getType() const override { return ConstraintType::Range; }
    
    bool validate(const ConfigValue& value, ValidationResult& result) const override {
        if (!value.has_value<double>()) return true;  // Not a numeric value, skip
        
        double val = value.value<double>();
        if (val < minValue || val > maxValue) {
            result.addError(std::make_shared<RangeValidationError>(
                "Unknown field", constraintDescription, val
            ));
            return false;
        }
        return true;
    }
    
    std::string getDescription() const override {
        return constraintDescription.empty() 
            ? ("Range: [" + std::to_string(minValue) + ", " + std::to_string(maxValue) + "]")
            : constraintDescription;
    }
};

// Enum constraint for string values
class EnumConstraint : public Constraint {
private:
    std::vector<std::string> allowedValues;
    std::string constraintDescription;
    
public:
    EnumConstraint(const std::vector<std::string>& values, const std::string& desc = "")
        : allowedValues(values), constraintDescription(desc) {}
    
    ConstraintType getType() const override { return ConstraintType::Enum; }
    
    bool validate(const ConfigValue& value, ValidationResult& result) const override {
        if (!value.has_value<std::string>()) return true;  // Not a string value, skip
        
        std::string val = value.value<std::string>();
        bool isValid = std::find(allowedValues.begin(), allowedValues.end(), val) != allowedValues.end();
        
        if (!isValid) {
            std::ostringstream oss;
            oss << "Allowed: ";
            for (size_t i = 0; i < allowedValues.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << allowedValues[i];
            }
            result.addError(std::make_shared<EnumValidationError>(
                "Unknown field", val, oss.str()
            ));
            return false;
        }
        return true;
    }
    
    std::string getDescription() const override {
        if (!constraintDescription.empty()) return constraintDescription;
        
        std::ostringstream oss;
        oss << "Enum: ";
        for (size_t i = 0; i < allowedValues.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << allowedValues[i];
        }
        return oss.str();
    }
};

// Format constraint for string values
class FormatConstraint : public Constraint {
private:
    std::string pattern;
    std::string constraintDescription;
    
public:
    FormatConstraint(const std::string& regex, const std::string& desc = "")
        : pattern(regex), constraintDescription(desc) {}
    
    ConstraintType getType() const override { return ConstraintType::Format; }
    
    bool validate(const ConfigValue& value, ValidationResult& result) const override {
        if (!value.has_value<std::string>()) return true;  // Not a string value, skip
        
        std::string val = value.value<std::string>();
        std::regex regex(pattern);
        if (!std::regex_match(val, regex)) {
            result.addError(std::make_shared<FormatValidationError>(
                "Unknown field", pattern, val
            ));
            return false;
        }
        return true;
    }
    
    std::string getDescription() const override {
        return constraintDescription.empty() ? ("Pattern: " + pattern) : constraintDescription;
    }
};

// Required constraint
class RequiredConstraint : public Constraint {
private:
    bool optional;
    
public:
    explicit RequiredConstraint(bool opt = false) : optional(opt) {}
    
    ConstraintType getType() const override { return ConstraintType::Required; }
    
    bool validate(const ConfigValue& value, ValidationResult& result) const override {
        if (optional) return true;  // Optional field, no validation
        if (!hasValue(value)) {
            result.addError(std::make_shared<RequiredFieldError>("Unknown field"));
            return false;
        }
        return true;
    }
    
    std::string getDescription() const override {
        return optional ? "Optional" : "Required";
    }
    
private:
    bool hasValue(const ConfigValue& value) const {
        return std::visit([](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::monostate>) return false;
            if constexpr (std::is_same_v<T, std::string>) return !arg.empty();
            if constexpr (std::is_same_v<T, int64_t>) return arg != 0;
            if constexpr (std::is_same_v<T, double>) return arg != 0.0;
            if constexpr (std::is_same_v<T, bool>) return arg;
            return true;
        }, value);
    }
};

// Configuration field schema
struct ConfigFieldSchema {
    std::string key;
    std::string description;
    std::vector<std::shared_ptr<Constraint>> constraints;
    std::string defaultValue;
    bool optional;
    
    ConfigFieldSchema(const std::string& k, const std::string& desc = "", bool opt = false)
        : key(k), description(desc), optional(opt) {}
    
    void addConstraint(const std::shared_ptr<Constraint>& constraint) {
        constraints.push_back(constraint);
    }
    
    std::string getConstraintsDescription() const {
        std::ostringstream oss;
        for (size_t i = 0; i < constraints.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << constraints[i]->getDescription();
        }
        return oss.str();
    }
};

// Configuration schema
class ConfigSchema {
private:
    std::vector<ConfigFieldSchema> fields;
    std::unordered_map<std::string, size_t> fieldIndex;
    
public:
    void addField(const ConfigFieldSchema& field) {
        fields.push_back(field);
        fieldIndex[field.key] = fields.size() - 1;
    }
    
    bool hasField(const std::string& key) const {
        return fieldIndex.find(key) != fieldIndex.end();
    }
    
    const ConfigFieldSchema* getField(const std::string& key) const {
        auto it = fieldIndex.find(key);
        if (it != fieldIndex.end()) {
            return &fields[it->second];
        }
        return nullptr;
    }
    
    const std::vector<ConfigFieldSchema>& getAllFields() const {
        return fields;
    }
    
    std::string getSchemaDescription() const {
        std::ostringstream oss;
        oss << "Configuration Schema with " << fields.size() << " field(s):\n";
        for (const auto& field : fields) {
            oss << "  " << field.key << ": " << field.description;
            if (!field.constraints.empty()) {
                oss << " (" << field.getConstraintsDescription() << ")";
            }
            if (field.optional) oss << " [optional]";
            oss << "\n";
        }
        return oss.str();
    }
};

// Forward declaration
class ConfigValidator;

// Configuration validator interface
class ConfigValidator {
private:
    ValidationMode mode;
    bool strictMode;
    
public:
    explicit ConfigValidator(ValidationMode validationMode = ValidationMode::Strict)
        : mode(validationMode), strictMode(validationMode == ValidationMode::Strict) {}
    
    ValidationMode getMode() const { return mode; }
    void setMode(ValidationMode newMode) {
        mode = newMode;
        strictMode = (newMode == ValidationMode::Strict);
    }
    
    // Validate configuration entry
    ValidationResult validate(const ConfigEntry& entry) const {
        ValidationResult result;
        
        // Check if field is defined in schema
        if (!isKnownField(entry.key)) {
            if (strictMode) {
                result.addError(std::make_shared<ValidationException>(
                    "Unknown configuration field: " + entry.key
                ));
            } else {
                result.addWarning("Unknown configuration field: " + entry.key);
            }
            return result;
        }
        
        // Validate against constraints
        validateConstraints(entry, result);
        
        return result;
    }
    
    // Validate all entries
    ValidationResult validateAll(const std::vector<ConfigEntry>& entries) const {
        ValidationResult result;
        
        for (const auto& entry : entries) {
            ValidationResult entryResult = validate(entry);
            // Merge results
            if (!entryResult.isValid) {
                result.isValid = false;
                result.errors.insert(result.errors.end(), 
                                   entryResult.errors.begin(), entryResult.errors.end());
            }
            result.warnings.insert(result.warnings.end(),
                                  entryResult.warnings.begin(), entryResult.warnings.end());
        }
        
        return result;
    }
    
    // Get default configuration schema
    virtual ConfigSchema getDefaultSchema() const = 0;
    
protected:
    // Check if field is known
    virtual bool isKnownField(const std::string& key) const {
        return schema.hasField(key);
    }
    
    // Validate against schema constraints
    virtual void validateConstraints(const ConfigEntry& entry, ValidationResult& result) const {
        const ConfigFieldSchema* field = schema.getField(entry.key);
        if (!field) return;
        
        for (const auto& constraint : field->constraints) {
            constraint->validate(entry.value, result);
        }
    }
    
    ConfigSchema schema;
};

// NLM-specific configuration validator
class NLMConfigValidator : public ConfigValidator {
public:
    NLMConfigValidator(ValidationMode mode = ValidationMode::Strict) 
        : ConfigValidator(mode) {
        initializeDefaultSchema();
    }
    
    ConfigSchema getDefaultSchema() const override {
        return schema;
    }
    
private:
    void initializeDefaultSchema() {
        // Neuron count
        ConfigFieldSchema neuronCount("neuron_count", "Number of neurons in the network");
        neuronCount.addConstraint(std::make_shared<RangeConstraint>(1, 1000000, "Must be between 1 and 1,000,000"));
        neuronCount.addConstraint(std::make_shared<RequiredConstraint>());
        schema.addField(neuronCount);
        
        // Simulation timestep
        ConfigFieldSchema timestep("simulation_timestep", "Time step for simulation in seconds");
        timestep.addConstraint(std::make_shared<RangeConstraint>(1e-6, 1.0, "Must be between 1µs and 1.0s"));
        timestep.addConstraint(std::make_shared<RequiredConstraint>());
        schema.addField(timestep);
        
        // Region count
        ConfigFieldSchema regionCount("region_count", "Number of neural regions");
        regionCount.addConstraint(std::make_shared<RangeConstraint>(1, 100, "Must be between 1 and 100"));
        regionCount.addConstraint(std::make_shared<RequiredConstraint>());
        schema.addField(regionCount);
        
        // Connection probability
        ConfigFieldSchema connProb("connection_probability", "Probability of connection between neurons");
        connProb.addConstraint(std::make_shared<RangeConstraint>(0.0, 1.0, "Must be between 0.0 and 1.0"));
        connProb.addConstraint(std::make_shared<RequiredConstraint>());
        schema.addField(connProb);
        
        // Random seed
        ConfigFieldSchema randomSeed("random_seed", "Random seed for reproducibility");
        randomSeed.addConstraint(std::make_shared<RangeConstraint>(0, std::numeric_limits<uint64_t>::max(), "Must be non-negative"));
        schema.addField(randomSeed);
        
        // STDP parameters
        ConfigFieldSchema stdpLtp("stdp_ltp_weight", "STDP LTP weight");
        stdpLtp.addConstraint(std::make_shared<RangeConstraint>(0.0, 1.0, "Must be between 0.0 and 1.0"));
        schema.addField(stdpLtp);
        
        ConfigFieldSchema stdpLtd("stdp_ltd_weight", "STDP LTD weight");
        stdpLtd.addConstraint(std::make_shared<RangeConstraint>(0.0, 1.0, "Must be between 0.0 and 1.0"));
        schema.addField(stdpLtd);
        
        ConfigFieldSchema stdpTau("stdp_tau", "STDP time constant in simulation steps");
        stdpTau.addConstraint(std::make_shared<RangeConstraint>(1, 10000, "Must be between 1 and 10,000"));
        schema.addField(stdpTau);
        
        // Structural plasticity parameters
        ConfigFieldSchema synaptogenesisRate("synaptogenesis_rate", "Rate of new synapse formation");
        synaptogenesisRate.addConstraint(std::make_shared<RangeConstraint>(0.0, 1.0, "Must be between 0.0 and 1.0"));
        schema.addField(synaptogenesisRate);
        
        ConfigFieldSchema pruningRate("pruning_rate", "Rate of synapse elimination");
        pruningRate.addConstraint(std::make_shared<RangeConstraint>(0.0, 1.0, "Must be between 0.0 and 1.0"));
        schema.addField(pruningRate);
    }
};
