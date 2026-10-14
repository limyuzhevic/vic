// Configuration validation system
class ConfigValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ValidationResult() : isValid(true) {}
        
        void addError(const std::string& error) {
            isValid = false;
            errors.push_back(error);
        }
        
        void addWarning(const std::string& warning) {
            warnings.push_back(warning);
        }
        
        std::string getSummary() const {
            std::stringstream ss;
            if (!errors.empty()) {
                ss << "Configuration validation failed:\n";
                for (const auto& error : errors) {
                    ss << "  ERROR: " << error << "\n";
                }
            }
            if (!warnings.empty()) {
                if (!errors.empty()) ss << "\n";
                ss << "Configuration warnings:\n";
                for (const auto& warning : warnings) {
                    ss << "  WARNING: " << warning << "\n";
                }
            }
            if (isValid && errors.empty() && warnings.empty()) {
                ss << "Configuration is valid.";
            }
            return ss.str();
        }
    };
    
    static ValidationResult validate(const Config& config) {
        ValidationResult result;
        
        // Validate required keys
        validateRequiredKeys(config, result);
        
        // Validate value ranges
        validateValueRanges(config, result);
        
        // Validate consistency
        validateConsistency(config, result);
        
        return result;
    }
    
private:
    static void validateRequiredKeys(const Config& config, ValidationResult& result) {
        // These keys are required for basic operation
        const std::vector<std::string> requiredKeys = {
            "random_seed",
            "simulation_timestep",
            "neuron_count",
            "region_count"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                result.addWarning("Missing optional key: " + key);
            }
        }
    }
    
    static void validateValueRanges(const Config& config, ValidationResult& result) {
        // Validate neuron_count (should be positive)
        if (auto neuronCount = config.getOr<int64_t>("neuron_count", 0); neuronCount <= 0) {
            result.addError("neuron_count must be positive, got " + std::to_string(neuronCount));
        }
        
        // Validate region_count (should be positive)
        if (auto regionCount = config.getOr<int64_t>("region_count", 0); regionCount <= 0) {
            result.addError("region_count must be positive, got " + std::to_string(regionCount));
        }
        
        // Validate simulation_timestep (should be positive and reasonable)
        if (auto timestep = config.getOr<double>("simulation_timestep", 0.0); timestep <= 0.0 || timestep > 1.0) {
            result.addWarning("simulation_timestep is " + std::to_string(timestep) + ". May be too large for stable simulation.");
        }
        
        // Validate connection_probability (should be between 0 and 1)
        if (auto prob = config.getOr<float>("connection_probability", -1.0f); prob < 0.0f || prob > 1.0f) {
            result.addWarning("connection_probability is " + std::to_string(prob) + ". Should be between 0.0 and 1.0.");
        }
        
        // Validate plasticity parameters
        if (auto ltp = config.getOr<float>("stdp_ltp_weight", -1.0f); ltp < 0.0f) {
            result.addWarning("stdp_ltp_weight should be positive for potentiation.");
        }
        
        if (auto ltd = config.getOr<float>("stdp_ltd_weight", -1.0f); ltd < 0.0f) {
            result.addWarning("stdp_ltd_weight should be positive for depression.");
        }
        
        if (auto tau = config.getOr<float>("stdp_tau", -1.0f); tau <= 0.0f) {
            result.addWarning("stdp_tau should be positive.");
        }
        
        // Validate structural plasticity rates
        if (auto synaptogenesis = config.getOr<float>("synaptogenesis_rate", -1.0f); synaptogenesis < 0.0f || synaptogenesis > 1.0f) {
            result.addWarning("synaptogenesis_rate should be between 0.0 and 1.0.");
        }
        
        if (auto pruning = config.getOr<float>("pruning_rate", -1.0f); pruning < 0.0f || pruning > 1.0f) {
            result.addWarning("pruning_rate should be between 0.0 and 1.0.");
        }
    }
    
    static void validateConsistency(const Config& config, ValidationResult& result) {
        // Check that neuron_count is reasonable
        auto neuronCount = config.getOr<int64_t>("neuron_count", 1000);
        if (neuronCount > 10000) {
            result.addWarning("neuron_count is very large (" + std::to_string(neuronCount) + "). This may impact performance.");
        }
        
        // Check region_count vs neuron_count
        auto regionCount = config.getOr<int64_t>("region_count", 1);
        if (neuronCount > 0 && regionCount > neuronCount) {
            result.addWarning("region_count (" + std::to_string(regionCount) + ") is greater than neuron_count (" + std::to_string(neuronCount) + "). May cause uneven distribution.");
        }
        
        // Validate that debugging options are not enabled in production-like settings
        if (auto logLevel = config.getOr<std::string>("log_level", "INFO"); logLevel == "DEBUG") {
            result.addWarning("DEBUG log level enabled. This will impact performance and may produce excessive output.");
        }
        
        // Check for potential numerical stability issues
        auto timestep = config.getOr<double>("simulation_timestep", 0.001);
        if (timestep < 0.0001) {
            result.addWarning("simulation_timestep is very small (" + std::to_string(timestep) + "). This may lead to long simulation times.");
        }
    }
};

// Configuration validation function
std::string validateConfig(const Config& config) {
    auto result = ConfigValidator::validate(config);
    return result.getSummary();
}