using std::to_string;
using std::stoi;
using std::stoll;
using std::stod;
using std::stof;
using std::stoll;
using std::isalpha;
using std::isalnum;
using std::toupper;
using std::tolower;
using std::tolower;
using std::isspace;
using std::isspace;
using std::isprint;
using std::isprint;
using std::to_string;

namespace nlm {

// Configuration validation exception hierarchy
class ConfigValidationException : public std::runtime_error {
public:
    enum class Severity {
        Low,
        Medium,
        High,
        Critical
    };

    ConfigValidationException(const std::string& message, Severity severity,
                               const std::string& context = "",
                               const std::string& suggestion = "")
        : std::runtime_error(message), severity(severity), context(context), suggestion(suggestion) {}

    Severity getSeverity() const { return severity; }
    const std::string& getContext() const { return context; }
    const std::string& getSuggestion() const { return suggestion; }

private:
    Severity severity;
    std::string context;
    std::string suggestion;
};

// Configuration validation error collector
class ValidationErrorCollector {
public:
    void addError(const std::string& message, ConfigValidationException::Severity severity = ConfigValidationException::Severity::High,
                  const std::string& context = "", const std::string& suggestion = "") {
        std::lock_guard<std::mutex> lock(mutex);
        errors.push_back({
            message,
            severity,
            context,
            suggestion,
            std::chrono::system_clock::now()
        });
    }

    bool hasErrors() const {
        std::lock_guard<std::mutex> lock(mutex);
        return !errors.empty();
    }

    std::vector<ValidationError> getErrors() const {
        std::lock_guard<std::mutex> lock(mutex);
        return errors;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        errors.clear();
    }

    std::string getReport() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::ostringstream oss;
        oss << "Configuration Validation Report\n";
        oss << "Generated: " << getCurrentTimestamp() << "\n";
        oss << "Total errors: " << errors.size() << "\n";

        for (const auto& error : errors) {
            oss << "\n[Severity: " << static_cast<int>(error.severity) << "] " << error.message;
            if (!error.context.empty()) {
                oss << " (Context: " << error.context << ")";
            }
            if (!error.suggestion.empty()) {
                oss << "\n   Suggestion: " << error.suggestion;
            }
            oss << "\n   Time: " << getTimestamp(error.timestamp);
        }

        return oss.str();
    }

private:
    struct ValidationError {
        std::string message;
        ConfigValidationException::Severity severity;
        std::string context;
        std::string suggestion;
        std::chrono::system_clock::time_point timestamp;
    };

    mutable std::mutex mutex;
    std::vector<ValidationError> errors;

    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        char buf[100];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_buf);
        return std::string(buf);
    }

    static std::string getTimestamp(const std::chrono::system_clock::time_point& tp) {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        char buf[100];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_buf);
        return std::string(buf);
    }
};

// Parameter validation rules
struct ValidationRule {
    std::string parameterName;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    std::string suggestion;
    ConfigValidationException::Severity severity;
};

// Parameter metadata for schema definition
struct ParameterMetadata {
    std::string name;
    std::string description;
    ConfigValue defaultValue;
    std::vector<std::string> categories;
    std::map<std::string, std::string> constraints;
    std::string type;
    bool required;
    std::string example;
};

// Configuration schema
class ConfigurationSchema {
public:
    ConfigurationSchema() {
        initializeBrainParameters();
        initializeMemoryParameters();
        initializeNeuromodulationParameters();
        initializeDevelopmentParameters();
        initializePerformanceParameters();
        initializeSystemLimitParameters();
        initializeValidationRules();
        initializeSecurityRules();
    }

    const std::vector<ParameterMetadata>& getParameters() const { return parameters; }
    const std::vector<ValidationRule>& getValidationRules() const { return validationRules; }
    const std::vector<ValidationRule>& getSecurityRules() const { return securityRules; }

    ParameterMetadata getParameter(const std::string& name) const {
        auto it = parameterMap.find(name);
        if (it != parameterMap.end()) {
            return it->second;
        }
        throw ConfigException("Unknown parameter: " + name, "getParameter");
    }

    bool hasParameter(const std::string& name) const {
        return parameterMap.find(name) != parameterMap.end();
    }

    std::vector<std::string> getParametersInCategory(const std::string& category) const {
        std::vector<std::string> result;
        for (const auto& param : parameters) {
            if (std::find(param.categories.begin(), param.categories.end(), category) != param.categories.end()) {
                result.push_back(param.name);
            }
        }
        return result;
    }

private:
    std::vector<ParameterMetadata> parameters;
    std::vector<ValidationRule> validationRules;
    std::vector<ValidationRule> securityRules;
    std::unordered_map<std::string, ParameterMetadata> parameterMap;

    void addParameter(const ParameterMetadata& param) {
        parameters.push_back(param);
        parameterMap[param.name] = param;
    }

    void addValidationRule(const ValidationRule& rule) {
        validationRules.push_back(rule);
    }

    void addSecurityRule(const ValidationRule& rule) {
        securityRules.push_back(rule);
    }

    void initializeBrainParameters() {
        // Neuron count parameters
        addParameter({
            "neuron_count",
            "Total number of neurons in the brain",
            500.0,  // Default
            {"brain", "neurons"},
            {
                {"min", "1"},
                {"max", "1000000"},
                {"type", "int64_t"}
            },
            "int64_t",
            true,
            "1000"
        });

        // Region count parameters
        addParameter({
            "region_count",
            "Number of brain regions",
            2.0,  // Default
            {"brain", "regions"},
            {
                {"min", "1"},
                {"max", "100"},
                {"type", "int64_t"}
            },
            "int64_t",
            true,
            "2"
        });

        // Connection probability parameters
        addParameter({
            "connection_probability",
            "Probability of connection between neurons (0.0 to 1.0)",
            0.12,  // Default
            {"brain", "connectivity"},
            {
                {"min", "0.0"},
                {"max", "1.0"},
                {"type", "double"}
            },
            "double",
            true,
            "0.12"
        });

        // Synapse density
        addParameter({
            "synapse_density",
            "Average number of synapses per neuron",
            0.08,  // Default
            {"brain", "connectivity"},
            {
                {"min", "0.001"},
                {"max", "1.0"},
                {"type", "double"}
            },
            "double",
            true,
            "0.08"
        });

        // Membrane potential thresholds
        addParameter({
            "v_thresh",
            "Neuron firing threshold (mV)",
            -45.0,  // Default
            {"brain", "neurons", "electrical"},
            {
                {"min", "-100.0"},
                {"max", "50.0"},
                {"type", "double"}
            },
            "double",
            true,
            "-45.0"
        });

        // Membrane resting potential
        addParameter({
            "v_rest",
            "Neuron resting membrane potential (mV)",
            -65.0,  // Default
            {"brain", "neurons", "electrical"},
            {
                {"min", "-90.0"},
                {"max", "-40.0"},
                {"type", "double"}
            },
            "double",
            true,
            "-65.0"
        });

        // Reset potential
        addParameter({
            "v_reset",
            "Neuron reset membrane potential after spike (mV)",
            -70.0,  // Default
            {"brain", "neurons", "electrical"},
            {
                {"min", "-80.0"},
                {"max", "-50.0"},
                {"type", "double"}
            },
            "double",
            true,
            "-70.0"
        });

        // Membrane time constant
        addParameter({
            "tau_mem",
            "Membrane time constant (ms)",
            25.0,  // Default
            {"brain", "neurons", "dynamics"},
            {
                {"min", "1.0"},
                {"max", "100.0"},
                {"type", "double"}
            },
            "double",
            true,
            "25.0"
        });

        // Refractory period
        addParameter({
            "tau_ref",
            "Neuron refractory period (ms)",
            3.0,  // Default
            {"brain", "neurons", "dynamics"},
            {
                {"min", "0.1"},
                {"max", "50.0"},
                {"type", "double"}
            },
            "double",
            true,
            "3.0"
        });

        // Initial weight parameters
        addParameter({
            "initial_weight_mean",
            "Mean of initial synaptic weight distribution",
            0.6,  // Default
            {"brain", "synapses", "initialization"},
            {
                {"min", "0.0"},
                {"max", "2.0"},
                {"type", "double"}
            },
            "double",
            true,
            "0.6"
        });

        addParameter({
            "initial_weight_std",
            "Standard deviation of initial synaptic weight distribution",
            0.15,  // Default
            {"brain", "synapses", "initialization"},
            {
                {"min", "0.0"},
                {"max", "1.0"},
                {"type", "double"}
            },
            "double",
            true,
            "0.15"
        });
    }

    void initializeMemoryParameters() {
        // Working memory capacity
        addParameter({
            "working_memory_capacity",
            "Capacity of working memory (number of items)",
            150,  // Default
            {"memory", "working"},
            {
                {"min", "10"},
                {"max", "10000"},
                {"type", "int64_t"}
            },
            "int64_t",
            true,
            "150"
        });

        // Episodic memory limits
        addParameter({
            "episodic_memory_max_episodes",
            "Maximum number of episodic memory episodes",
            2000,  // Default
            {"memory", "episodic"},
            {
                {"min", "100"},
                {"max", "1000000"},
                {"type", "int64_t"}
            },
            "int64_t",
            true,
            "2000"
        });

        // Episodic memory decay rate
        addParameter({
            "episodic_memory_decay_rate",
            "Decay rate for episodic memory (per timestep)",
            0.001,  // Default
            {"memory", "episodic"},
            {
                {"min", "0.0"},
                {"max", "1.0"},
                {"type", "double"}
            },
            "double",
            true,
            "0.001"
        });

        // Associative memory capacity
        addParameter({
            "associative_memory_capacity",
            "Capacity of associative memory (number of patterns)",
            500,  // Default
            {"memory", "associative"},
            {
                {"min", "10"},
                {"max", "100000"},
                {"type", "int64_t"}
            },
            "int64_t",
            true,
            "500"
        });

        // Working memory decay rate
        addParameter({
            "working_memory_decay_rate",
            "Decay rate for working memory (per timestep)",
            0.0005,  // Default
            {"memory", "working"},
            {
                {"min", "0.0"},
                {"max", "1.0"},
                {"type", "double"}
            },
            "double",
            true,
            "0.0005"
        });
    }

    void initializeNeuromodulationParameters() {
        // Dopamine sensitivity
        addParameter({
            "dopamine_sensitivity",
            "Sensitivity to dopamine (higher = stronger effect)",
            1.2,  // Default
            {"neuromodulation", "dopamine"},
            {
                {"min", "0.0"},
                {"max", "10.0"},
                {"type", "double"}
            },
            "double",
            true,
            "1.2"
        });

        // Curiosity threshold
        addParameter({
            "curiosity_threshold",
            "Threshold for curiosity-driven exploration",
            0.3,  // Default
            {"neuromodulation", "curiosity"},
            {
                {"min", "0.0"},
                {"max", "1.0"},
                {"type", "double"}
            },
            "double",
            true,
            "0.3"
        });

        // Curiosity strength
        addParameter({
            "curiosity_strength",
            "Strength of curiosity modulation",
            0.8,  // Default
            {"neuromodulation", "curiosity"},
            {
                {"min", "0.0"},
                {"max", "2.0"},
                {"type", "double"}
            },
            "double",
            true,
            "0.8"
        });

        // Novelty threshold
        addParameter({
            "novelty_threshold",
            "Threshold for novelty detection",
            0.3,  // Default
            {"neuromodulation", "novelty"},
            {
                {"min", "0.0"},
                {"max", "1.0"},
                {"type", "double"}
            },
            "double",
            true,
            "0.3"
        });

        // Acetylcholine sensitivity
        addParameter({
            "acetylcholine_sensitivity",
            "Sensitivity to acetylcholine",
            1.0,  // Default
            {"neuromodulation", "acetylcholine"},
            {
                {"min", "0.0"},
                {"max", "5.0"},
                {"type", "double"}
            },
            "double",
            true,
            "1.0"
        });

        // Norepinephrine sensitivity
        addParameter({
            "norepinephrine_sensitivity",
            "Sensitivity to norepinephrine",
            1.0,  // Default
            {"neuromodulation", "norepinephrine"},
            {
                {"min", "0.0"},
                {"max", "5.0"},
                {"type", "double"}
            },
            "double",
            true,
            "1.0"
        });

        // Serotonin sensitivity
        addParameter({
            "serotonin_sensitivity",
            "Sensitivity to serotonin",
            1.0,  // Default
            {"neuromodulation", "serotonin"},
            {
                {"min", "0.0"},
                {"max", "5.0"},
                {"type", "double"}
            },
            "double",
            true,
            "1.0"
        });
    }

    void initializeDevelopmentParameters() {
        // Critical period length
        addParameter({
            "critical_period_length",
            "Length of critical period for development (in timesteps)",
            120,  // Default
            {"development", "plasticity"},
            {
                {"min", "10"},
                {"max", "10000"},
                {"type", "int64_t"}
            },
            "int64_t",
            true,
            "120"
        });

        // Plasticity decay rate
        addParameter({
            "plasticity_decay_rate",
            "Rate at which plasticity decreases during development (per timestep)",
            0.002,  // Default
            {"development", "plasticity"},
            {
                {"min", "0.0"},
                {"max", "0.1"},
                {"type", "double"}
            },
            "double",
            true,
            "0.002"
        });

        // Synaptogenesis rate
        addParameter({
            "synaptogenesis_rate",
            "Rate of new synapse formation during development (per timestep)",
            0.00015,  // Default
            {"development", "plasticity"},
            {
                {"min", "0.0"},
                {"max", "0.01"},
                {"type", "double"}
            },
            "double",
            true,
            "0.00015"
        });

        // Pruning rate
        addParameter({
            "pruning_rate",
            "Rate of synapse pruning during development (per timestep)",
            0.00002,  // Default
            {"development", "plasticity"},
            {
                {"min", "0.0"},
                {"max", "0.001"},
                {"type", "double"}
            },
            "double",
            true,
            "0.00002"
        });

        // Plasticity increase factor
        addParameter({
            "plasticity_increase_factor",
            "Factor by which plasticity increases during critical period",
            2.0,  // Default
            {"development", "plasticity"},
            {
                {"min", "1.0"},
                {"max", "10.0"},
                {"type", "double"}
            },
            "double",
            true,
            "2.0"
        });
    }

    void initializePerformanceParameters() {
        // Simulation timestep
        addParameter({
            "timestep",
            "Simulation timestep (in seconds)",
            0.005,  // Default
            {"performance", "simulation"},
            {
                {"min", "0.0001"},
                {"max", "1.0"},
                {"type", "double"}
            },
            "double",
            true,
            "0.005"
        });

        // Maximum simulation steps
        addParameter({
            "max_steps",
            "Maximum number of simulation steps",
            5000,  // Default
            {"performance", "simulation"},
            {
                {"min", "100"},
                {"max", "1000000"},
                {"type", "int64_t"}
            },
            "int64_t",
            true,
            "5000"
        });

        // Random seed
        addParameter({
            "random_seed",
            "Random seed for reproducible simulations",
            42,  // Default
            {"performance", "simulation"},
            {
                {"min", "0"},
                {"max", "2147483647"},
                {"type", "int64_t"}
            },
            "int64_t",
            true,
            "42"
        });

        // Checkpoint interval
        addParameter({
            "checkpoint_interval",
            "Interval between checkpoints (in steps)",
            500,  // Default
            {"performance", "checkpointing"},
            {
                {"min", "1"},
                {"max", "1000000"},
                {"type", "int64_t"}
            },
            "int64_t",
            true,
            "500"
        });

        // Visualization update rate
        addParameter({
            "visualization_update_rate",
            "Rate of visualization updates (in Hz)",
            30.0,  // Default
            {"performance", "visualization"},
            {
                {"min", "1.0"},
                {"max", "1000.0"},
                {"type", "double"}
            },
            "double",
            true,
            "30.0"
        });

        // Optimization threshold
        addParameter({
            "optimization_threshold",
            "Threshold for optimization algorithms",
            0.01,  // Default
            {"performance", "optimization"},
            {
                {"min", "0.0"},
                {"max", "1.0"},
                {"type", "double"}
            },
            "double",
            true,
            "0.01"
        });

        // Learning rate
        addParameter({
            "learning_rate",
            "Global learning rate for plasticity",
            0.01,  // Default
            {"performance", "learning"},
            {
                {"min", "0.0"},
                {"max", "1.0"},
                {"type", "double"}
            },
            "double",
            true,
            "0.01"
        });
    }

    void initializeSystemLimitParameters() {
        // Maximum neurons
        addParameter({
            "max_neurons",
            "Maximum number of neurons allowed in the brain",
            1000000,  // Default
            {"system_limits", "resources"},
            {
                {"min", "100"},
                {"max", "10000000"},
                {"type", "int64_t"}
            },
            "int64_t",
            true,
            "1000000"
        });

        // Maximum memory usage
        addParameter({
            "max_memory_usage_mb",
            "Maximum memory usage in megabytes",
            512,  // Default
            {"system_limits", "resources"},
            {
                {"min", "64"},
                {"max", "4096"},
                {"type", "int64_t"}
            },
            "int64_t",
            true,
            "512"
        });

        // Maximum file descriptors
        addParameter({
            "max_file_descriptors",
            "Maximum number of file descriptors",
            1024,  // Default
            {"system_limits", "resources"},
            {
                {"min", "256"},
                {"max", "65536"},
                {"type", "int64_t"}
            },
            "int64_t",
            true,
            "1024"
        });

        // Maximum connections per neuron
        addParameter({
            "max_connections_per_neuron",
            "Maximum number of connections a single neuron can have",
            1000,  // Default
            {"system_limits", "connectivity"},
            {
                {"min", "10"},
                {"max", "10000"},
                {"type", "int64_t"}
            },
            "int64_t",
            true,
            "1000"
        });

        // Maximum simulation time
        addParameter({
            "max_simulation_time_ms",
            "Maximum simulation time in milliseconds",
            3600000,  // Default (1 hour)
            {"system_limits", "simulation"},
            {
                {"min", "1000"},
                {"max", "86400000"},
                {"type", "int64_t"}
            },
            "int64_t",
            true,
            "3600000"
        });
    }

    void initializeValidationRules() {
        // Range validation rule
        addValidationRule({
            "range_validation",
            [this](const ConfigValue& value) {
                return validateRange(value);
            },
            "Parameter value out of valid range",
            "Check parameter constraints in configuration schema",
            ConfigValidationException::Severity::High
        });

        // Type validation rule
        addValidationRule({
            "type_validation",
            [this](const ConfigValue& value) {
                return validateType(value);
            },
            "Parameter type mismatch",
            "Ensure parameter type matches schema definition",
            ConfigValidationException::Severity::High
        });

        // Dependency validation rule
        addValidationRule({
            "dependency_validation",
            [this](const ConfigValue& value) {
                return validateDependencies();
            },
            "Parameter dependency violation",
            "Check that dependent parameters are consistent",
            ConfigValidationException::Severity::Medium
        });

        // Constraint validation rule
        addValidationRule({
            "constraint_validation",
            [this](const ConfigValue& value) {
                return validateConstraints();
            },
            "Parameter constraint violation",
            "Check that parameters meet system constraints",
            ConfigValidationException::Severity::High
        });

        // Consistency validation rule
        addValidationRule({
            "consistency_validation",
            [this](const ConfigValue& value) {
                return validateConsistency();
            },
            "Parameter inconsistency detected",
            "Check that parameter combinations are consistent",
            ConfigValidationException::Severity::Medium
        });
    }

    void initializeSecurityRules() {
        // Input sanitization rule
        addSecurityRule({
            "input_sanitization",
            [this](const ConfigValue& value) {
                return sanitizeInput(value);
            },
            "Potentially unsafe configuration value",
            "Remove or escape special characters from configuration values",
            ConfigValidationException::Severity::Critical
        });

        // File path validation rule
        addSecurityRule({
            "file_path_validation",
            [this](const ConfigValue& value) {
                return validateFilePath(value);
            },
            "Potentially unsafe file path",
            "Use absolute paths and validate file permissions",
            ConfigValidationException::Severity::High
        });

        // Access control rule
        addSecurityRule({
            "access_control",
            [this](const ConfigValue& value) {
                return checkAccessControl(value);
            },
            "Access control violation",
            "Check file system permissions for configuration files",
            ConfigValidationException::Severity::High
        });
    }

    // Validation helper methods
    bool validateRange(const ConfigValue& value) const {
        // Implementation would check if value is within defined ranges
        return true;
    }

    bool validateType(const ConfigValue& value) const {
        // Implementation would check if value type matches schema
        return true;
    }

    bool validateDependencies() const {
        // Implementation would check parameter dependencies
        return true;
    }

    bool validateConstraints() const {
        // Implementation would check system constraints
        return true;
    }

    bool validateConsistency() const {
        // Implementation would check parameter consistency
        return true;
    }

    bool sanitizeInput(const ConfigValue& value) const {
        // Implementation would sanitize input to prevent injection
        return true;
    }

    bool validateFilePath(const ConfigValue& value) const {
        // Implementation would validate file paths
        return true;
    }

    bool checkAccessControl(const ConfigValue& value) const {
        // Implementation would check file system permissions
        return true;
    }
};