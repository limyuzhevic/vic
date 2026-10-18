// Enhanced error handling and validation functions
namespace nlm {
namespace validation {

// Validate configuration against required parameters
bool validateConfiguration(const Config& config) {
    std::cout << "=== Configuration Validation ===" << std::endl;
    
    bool isValid = true;
    
    // Check for required configuration parameters
    const std::vector<std::pair<std::string, std::string>> requiredParams = {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"}
    };
    
    std::cout << "Checking required parameters:" << std::endl;
    for (const auto& param : requiredParams) {
        if (!config.has(param.first)) {
            std::cout << "  ✗ Missing required parameter: " << param.first << std::endl;
            std::cout << "    Description: " << param.second << std::endl;
            isValid = false;
        } else {
            std::cout << "  ✓ Found parameter: " << param.first << std::endl;
        }
    }
    
    // Validate specific parameter ranges and types
    std::cout << std::endl;
    std::cout << "Validating parameter ranges:" << std::endl;
    
    // Validate random_seed
    auto randomSeedOpt = config.get<int64_t>("random_seed");
    if (randomSeedOpt.has_value()) {
        int64_t seed = *randomSeedOpt;
        if (seed < 0) {
            std::cout << "  ✗ Invalid random_seed: " << seed << " (must be >= 0)" << std::endl;
            isValid = false;
        } else {
            std::cout << "  ✓ Valid random_seed: " << seed << std::endl;
        }
    }
    
    // Validate simulation_timestep
    auto timestepOpt = config.get<double>("simulation_timestep");
    if (timestepOpt.has_value()) {
        double timestep = *timestepOpt;
        if (timestep <= 0.0 || timestep > 1.0) {
            std::cout << "  ✗ Invalid simulation_timestep: " << timestep << " (must be > 0 and <= 1.0)" << std::endl;
            isValid = false;
        } else {
            std::cout << "  ✓ Valid simulation_timestep: " << timestep << "s" << std::endl;
        }
    }
    
    // Validate neuron_count
    auto neuronCountOpt = config.get<int64_t>("neuron_count");
    if (neuronCountOpt.has_value()) {
        int64_t neuronCount = *neuronCountOpt;
        if (neuronCount <= 0 || neuronCount > 1000000) {
            std::cout << "  ✗ Invalid neuron_count: " << neuronCount << " (must be > 0 and <= 1,000,000)" << std::endl;
            isValid = false;
        } else {
            std::cout << "  ✓ Valid neuron_count: " << neuronCount << std::endl;
        }
    }
    
    // Validate region_count
    auto regionCountOpt = config.get<int64_t>("region_count");
    if (regionCountOpt.has_value()) {
        int64_t regionCount = *regionCountOpt;
        if (regionCount <= 0 || regionCount > 100) {
            std::cout << "  ✗ Invalid region_count: " << regionCount << " (must be > 0 and <= 100)" << std::endl;
            isValid = false;
        } else {
            std::cout << "  ✓ Valid region_count: " << regionCount << std::endl;
        }
    }
    
    // Validate connection_probability
    auto connProbOpt = config.get<double>("connection_probability");
    if (connProbOpt.has_value()) {
        double connProb = *connProbOpt;
        if (connProb < 0.0 || connProb > 1.0) {
            std::cout << "  ✗ Invalid connection_probability: " << connProb << " (must be >= 0.0 and <= 1.0)" << std::endl;
            isValid = false;
        } else {
            std::cout << "  ✓ Valid connection_probability: " << connProb << std::endl;
        }
    }
    
    // Check for configuration consistency
    std::cout << std::endl;
    std::cout << "Checking configuration consistency:" << std::endl;
    
    auto neuronCount = config.getOr<int64_t>("neuron_count", 1000);
    auto regionCount = config.getOr<int64_t>("region_count", 2);
    
    if (neuronCount < regionCount * 10) {
        std::cout << "  ⚠ Warning: neuron_count (" << neuronCount << ") is small relative to region_count (" << regionCount << ")" << std::endl;
    } else {
        std::cout << "  ✓ neuron_count is reasonable for region_count" << std::endl;
    }
    
    // Check for deprecated or problematic configurations
    std::cout << std::endl;
    std::cout << "Checking for deprecated configurations:" << std::endl;
    
    std::vector<std::string> deprecatedKeys = {
        "deprecated_option_1",
        "problematic_setting"
    };
    
    for (const auto& key : deprecatedKeys) {
        if (config.has(key)) {
            std::cout << "  ✗ Deprecated or problematic configuration found: " << key << std::endl;
            isValid = false;
        }
    }
    
    if (isValid) {
        std::cout << std::endl;
        std::cout << "✓ Configuration validation passed" << std::endl;
    } else {
        std::cout << std::endl;
        std::cout << "✗ Configuration validation failed" << std::endl;
        std::cout << "Please fix the configuration issues above." << std::endl;
    }
    
    return isValid;
}

// Error handler for simulation operations
class SimulationErrorHandler {
public:
    enum class ErrorCode {
        Success = 0,
        BrainInitializationFailed = 1,
        ConfigurationError = 2,
        MemoryError = 3,
        InvalidParameter = 4,
        Timeout = 5,
        SimulationStepFailed = 6
    };
    
    struct ErrorInfo {
        ErrorCode code;
        std::string message;
        std::string context;
        std::string suggestion;
    };
    
    SimulationErrorHandler() : lastError(ErrorCode::Success) {}
    
    void setError(ErrorCode code, const std::string& message, const std::string& context = "", const std::string& suggestion = "") {
        lastError = code;
        lastErrorInfo.code = code;
        lastErrorInfo.message = message;
        lastErrorInfo.context = context;
        lastErrorInfo.suggestion = suggestion;
        
        std::cerr << "ERROR: " << message << std::endl;
        if (!context.empty()) {
            std::cerr << "  Context: " << context << std::endl;
        }
        if (!suggestion.empty()) {
            std::cerr << "  Suggestion: " << suggestion << std::endl;
        }
    }
    
    bool isSuccess() const {
        return lastError == ErrorCode::Success;
    }
    
    ErrorCode getLastError() const {
        return lastError;
    }
    
    const ErrorInfo& getLastErrorInfo() const {
        return lastErrorInfo;
    }
    
    void clearError() {
        lastError = ErrorCode::Success;
        lastErrorInfo = {};
    }
    
    std::string getFormattedError() const {
        if (lastError == ErrorCode::Success) {
            return "No error";
        }
        
        std::stringstream ss;
        ss << "Error Code: " << static_cast<int>(lastError) << std::endl;
        ss << "Message: " << lastErrorInfo.message << std::endl;
        ss << "Context: " << lastErrorInfo.context << std::endl;
        ss << "Suggestion: " << lastErrorInfo.suggestion << std::endl;
        
        return ss.str();
    }
    
private:
    ErrorCode lastError;
    ErrorInfo lastErrorInfo;
};

}} // namespace nlm::validation