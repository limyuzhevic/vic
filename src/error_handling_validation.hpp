// Enhanced error handling and validation system for NLM

#pragma once

#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <regex>

namespace nlm {

// Error categories
enum class ErrorCategory {
    Configuration,      // Configuration and parameter errors
    Simulation,         // Simulation execution errors
    Memory,            // Memory system errors
    Neural,            // Neural system errors
    FileIO,            // File I/O errors
    Network,           // Network/world errors
    Validation,        // Validation errors
    System,            // System resource errors
    UserInput,         // User input errors
    Unknown            // Unknown or unexpected errors
};

// Enhanced error class with context
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, 
             ErrorCategory category = ErrorCategory::Unknown,
             const std::string& context = "")
        : std::runtime_error(combineMessage(message, context)),
          category_(category),
          context_(context),
          timestamp_(std::chrono::system_clock::now()) {}
    
    ErrorCategory getCategory() const { return category_; }
    const std::string& getContext() const { return context_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    
    // Convert error to user-friendly message
    std::string getUserFriendlyMessage() const {
        std::stringstream ss;
        ss << "NLM Error (" << getCategoryString(category_) << ")";
        if (!context_.empty()) {
            ss << " in context: " << context_;
        }
        ss << "\nDetails: " << what();
        ss << "\nTimestamp: " << getFormattedTimestamp();
        return ss.str();
    }
    
private:
    static std::string combineMessage(const std::string& message, const std::string& context) {
        if (context.empty()) return message;
        return message + " (Context: " + context + ")";
    }
    
    static std::string getCategoryString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::Configuration: return "Configuration";
            case ErrorCategory::Simulation: return "Simulation";
            case ErrorCategory::Memory: return "Memory";
            case ErrorCategory::Neural: return "Neural";
            case ErrorCategory::FileIO: return "File I/O";
            case ErrorCategory::Network: return "Network";
            case ErrorCategory::Validation: return "Validation";
            case ErrorCategory::System: return "System";
            case ErrorCategory::UserInput: return "User Input";
            case ErrorCategory::Unknown: return "Unknown";
            default: return "Unknown";
        }
    }
    
    std::string getFormattedTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    ErrorCategory category_;
    std::string context_;
    std::chrono::system_clock::time_point timestamp_;
};

// Configuration validation
class ConfigurationValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::unordered_map<std::string, std::string> suggestedValues;
    };
    
    ConfigurationValidator() = default;
    ~ConfigurationValidator() = default;
    
    /**
     * Validate configuration for brain simulation
     */
    static ValidationResult validateBrainConfig(const Config& config) {
        ValidationResult result;
        result.isValid = true;
        
        // Validate required parameters
        validateNeuronCount(config, result);
        validateRegionCount(config, result);
        validateConnectionProbability(config, result);
        validateTimestep(config, result);
        validatePlasticityParameters(config, result);
        validateDevelopmentParameters(config, result);
        validateNeuromodulationParameters(config, result);
        
        return result;
    }
    
    /**
     * Validate configuration for world simulation
     */
    static ValidationResult validateWorldConfig(const Config& config) {
        ValidationResult result;
        result.isValid = true;
        
        validateWorldSize(config, result);
        validateVisionParameters(config, result);
        validateEnergyParameters(config, result);
        
        return result;
    }
    
    /**
     * Validate configuration for experiment setup
     */
    static ValidationResult validateExperimentConfig(const Config& config) {
        ValidationResult result;
        result.isValid = true;
        
        validateExperimentParameters(config, result);
        validateRandomSeed(config, result);
        
        return result;
    }
    
    /**
     * Validate configuration file
     */
    static ValidationResult validateConfigFile(const std::string& filepath) {
        ValidationResult result;
        
        if (filepath.empty()) {
            result.isValid = false;
            result.errors.push_back("Configuration file path cannot be empty");
            return result;
        }
        
        // Check if file exists
        std::ifstream file(filepath);
        if (!file) {
            result.isValid = false;
            result.errors.push_back("Configuration file does not exist: " + filepath);
            return result;
        }
        
        // Try to parse the file
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            result.isValid = false;
            result.errors.push_back("Failed to parse configuration file: " + filepath);
            return result;
        }
        
        // Validate parsed configuration
        auto brainValidation = validateBrainConfig(*config);
        auto worldValidation = validateWorldConfig(*config);
        auto experimentValidation = validateExperimentConfig(*config);
        
        result.isValid = brainValidation.isValid && worldValidation.isValid && experimentValidation.isValid;
        result.errors.insert(result.errors.end(), brainValidation.errors.begin(), brainValidation.errors.end());
        result.errors.insert(result.errors.end(), worldValidation.errors.begin(), worldValidation.errors.end());
        result.errors.insert(result.errors.end(), experimentValidation.errors.begin(), experimentValidation.errors.end());
        
        result.warnings.insert(result.warnings.end(), brainValidation.warnings.begin(), brainValidation.warnings.end());
        result.warnings.insert(result.warnings.end(), worldValidation.warnings.begin(), worldValidation.warnings.end());
        result.warnings.insert(result.warnings.end(), experimentValidation.warnings.begin(), experimentValidation.warnings.end());
        
        result.suggestedValues.insert(brainValidation.suggestedValues.begin(), brainValidation.suggestedValues.end());
        result.suggestedValues.insert(worldValidation.suggestedValues.begin(), worldValidation.suggestedValues.end());
        result.suggestedValues.insert(experimentValidation.suggestedValues.begin(), experimentValidation.suggestedValues.end());
        
        return result;
    }
    
    /**
     * Validate input parameters for simulation
     */
    static ValidationResult validateSimulationParameters(uint64_t neurons,
                                                         uint64_t steps,
                                                         double timestep) {
        ValidationResult result;
        result.isValid = true;
        
        if (neurons == 0) {
            result.isValid = false;
            result.errors.push_back("Number of neurons must be greater than 0");
        } else if (neurons > 1000000) { // 1 million neurons maximum
            result.isValid = false;
            result.warnings.push_back("High neuron count may lead to performance issues");
        }
        
        if (steps == 0) {
            result.isValid = false;
            result.errors.push_back("Number of steps must be greater than 0");
        }
        
        if (timestep <= 0.0 || timestep > 1.0) {
            result.isValid = false;
            result.errors.push_back("Time step must be between 0 and 1 second");
        }
        
        return result;
    }
    
    /**
     * Validate input parameters for world configuration
     */
    static ValidationResult validateWorldParameters(size_t width,
                                                     size_t height,
                                                     size_t visionWidth,
                                                     size_t visionHeight) {
        ValidationResult result;
        result.isValid = true;
        
        if (width == 0 || height == 0) {
            result.isValid = false;
            result.errors.push_back("World dimensions must be greater than 0");
        }
        
        if (visionWidth > width || visionHeight > height) {
            result.isValid = false;
            result.errors.push_back("Vision dimensions cannot be larger than world dimensions");
        }
        
        if (visionWidth == 0 || visionHeight == 0) {
            result.isValid = false;
            result.errors.push_back("Vision dimensions must be greater than 0");
        }
        
        return result;
    }
    
private:
    static void validateNeuronCount(const Config& config, ValidationResult& result) {
        if (!config.has("neuron_count")) {
            result.isValid = false;
            result.errors.push_back("Missing required parameter: neuron_count");
            result.suggestedValues["neuron_count"] = "100";
            return;
        }
        
        int64_t neuronCount = config.getOr<int64_t>("neuron_count", 0);
        if (neuronCount <= 0) {
            result.isValid = false;
            result.errors.push_back("neuron_count must be a positive integer");
        } else if (neuronCount > 1000000) {
            result.isValid = false;
            result.errors.push_back("neuron_count is too large (maximum: 1,000,000)");
            result.suggestedValues["neuron_count"] = "10000";
        }
    }
    
    static void validateRegionCount(const Config& config, ValidationResult& result) {
        if (!config.has("region_count")) {
            result.isValid = false;
            result.errors.push_back("Missing required parameter: region_count");
            result.suggestedValues["region_count"] = "1";
            return;
        }
        
        int64_t regionCount = config.getOr<int64_t>("region_count", 0);
        if (regionCount <= 0) {
            result.isValid = false;
            result.errors.push_back("region_count must be a positive integer");
        }
    }
    
    static void validateConnectionProbability(const Config& config, ValidationResult& result) {
        if (config.has("connection_probability")) {
            float prob = config.getOr<float>("connection_probability", 0.0f);
            if (prob < 0.0f || prob > 1.0f) {
                result.isValid = false;
                result.errors.push_back("connection_probability must be between 0.0 and 1.0");
                result.suggestedValues["connection_probability"] = "0.1";
            }
        }
    }
    
    static void validateTimestep(const Config& config, ValidationResult& result) {
        if (config.has("simulation_timestep")) {
            double timestep = config.getOr<double>("simulation_timestep", 0.0);
            if (timestep <= 0.0 || timestep > 1.0) {
                result.isValid = false;
                result.errors.push_back("simulation_timestep must be between 0 and 1 second");
                result.suggestedValues["simulation_timestep"] = "0.001";
            }
        }
    }
    
    static void validatePlasticityParameters(const Config& config, ValidationResult& result) {
        // STDP parameters
        if (config.has("stdp_ltp_weight")) {
            float weight = config.getOr<float>("stdp_ltp_weight", 0.0f);
            if (weight < 0.0f) {
                result.warnings.push_back("STDP LTP weight should be positive");
            }
        }
        
        if (config.has("stdp_ltd_weight")) {
            float weight = config.getOr<float>("stdp_ltd_weight", 0.0f);
            if (weight < 0.0f) {
                result.warnings.push_back("STDP LTD weight should be positive");
            }
        }
        
        // Structural plasticity parameters
        if (config.has("synaptogenesis_rate")) {
            float rate = config.getOr<float>("synaptogenesis_rate", 0.0f);
            if (rate < 0.0f || rate > 1.0f) {
                result.warnings.push_back("synaptogenesis_rate should be between 0.0 and 1.0");
            }
        }
        
        if (config.has("pruning_rate")) {
            float rate = config.getOr<float>("pruning_rate", 0.0f);
            if (rate < 0.0f || rate > 1.0f) {
                result.warnings.push_back("pruning_rate should be between 0.0 and 1.0");
            }
        }
    }
    
    static void validateDevelopmentParameters(const Config& config, ValidationResult& result) {
        if (config.has("developmental_maturation_rate")) {
            float rate = config.getOr<float>("developmental_maturation_rate", 0.0f);
            if (rate < 0.0f || rate > 1.0f) {
                result.warnings.push_back("developmental_maturation_rate should be between 0.0 and 1.0");
            }
        }
        
        if (config.has("critical_period_duration")) {
            float duration = config.getOr<float>("critical_period_duration", 0.0f);
            if (duration < 0.0f) {
                result.warnings.push_back("critical_period_duration should be positive");
            }
        }
    }
    
    static void validateNeuromodulationParameters(const Config& config, ValidationResult& result) {
        if (config.has("dopamine_sensitivity")) {
            float sensitivity = config.getOr<float>("dopamine_sensitivity", 0.0f);
            if (sensitivity < 0.0f) {
                result.warnings.push_back("dopamine_sensitivity should be positive");
            }
        }
        
        if (config.has("curiosity_rate")) {
            float rate = config.getOr<float>("curiosity_rate", 0.0f);
            if (rate < 0.0f || rate > 1.0f) {
                result.warnings.push_back("curiosity_rate should be between 0.0 and 1.0");
            }
        }
    }
    
    static void validateWorldSize(const Config& config, ValidationResult& result) {
        if (config.has("world_width") && config.has("world_height")) {
            float width = config.getOr<float>("world_width", 0.0f);
            float height = config.getOr<float>("world_height", 0.0f);
            
            if (width <= 0.0f || height <= 0.0f) {
                result.isValid = false;
                result.errors.push_back("World dimensions must be positive");
            }
        }
    }
    
    static void validateVisionParameters(const Config& config, ValidationResult& result) {
        if (config.has("vision_width") && config.has("vision_height")) {
            float width = config.getOr<float>("vision_width", 0.0f);
            float height = config.getOr<float>("vision_height", 0.0f);
            
            if (width <= 0.0f || height <= 0.0f) {
                result.isValid = false;
                result.errors.push_back("Vision dimensions must be positive");
            }
        }
    }
    
    static void validateEnergyParameters(const Config& config, ValidationResult& result) {
        if (config.has("max_energy")) {
            float energy = config.getOr<float>("max_energy", 0.0f);
            if (energy <= 0.0f) {
                result.isValid = false;
                result.errors.push_back("max_energy must be positive");
            }
        }
    }
    
    static void validateExperimentParameters(const Config& config, ValidationResult& result) {
        if (config.has("experiment_duration")) {
            float duration = config.getOr<float>("experiment_duration", 0.0f);
            if (duration <= 0.0f) {
                result.isValid = false;
                result.errors.push_back("experiment_duration must be positive");
            }
        }
    }
    
    static void validateRandomSeed(const Config& config, ValidationResult& result) {
        if (config.has("random_seed")) {
            int64_t seed = config.getOr<int64_t>("random_seed", 0);
            if (seed < 0) {
                result.warnings.push_back("random_seed should be positive");
            }
        }
    }
};

// Simulation error handler
class SimulationErrorHandler {
public:
    SimulationErrorHandler() = default;
    ~SimulationErrorHandler() = default;
    
    /**
     * Handle simulation errors with recovery suggestions
     */
    static bool handleError(const std::exception& error, const std::string& context = "") {
        try {
            // Convert to NLMError if possible
            throw; // Re-throw to be caught by calling code
        } catch (const NLMError& nlmError) {
            // Log the error with full context
            logError(nlmError, context);
            
            // Try recovery
            if (canRecoverFromError(nlmError)) {
                return attemptRecovery(nlmError, context);
            } else {
                // Suggest alternatives
                suggestAlternatives(nlmError);
            }
        } catch (const std::exception& e) {
            // Handle other standard exceptions
            NLMError nlmError(
                std::string("Standard exception: ") + e.what(),
                ErrorCategory::Simulation,
                context
            );
            logError(nlmError, context);
        }
        
        return false; // Error not recovered
    }
    
    /**
     * Validate simulation state before execution
     */
    static bool validateSimulationState(const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw NLMError("Brain pointer is null", ErrorCategory::Validation, "Simulation validation");
        }
        
        if (!brain->isInitialized()) {
            throw NLMError("Brain is not initialized", ErrorCategory::Simulation, "Simulation validation");
        }
        
        // Check neural connectivity
        size_t totalNeurons = brain->getTotalNeuronCount();
        if (totalNeurons == 0) {
            throw NLMError("Brain has no neurons", ErrorCategory::Neural, "Simulation validation");
        }
        
        // Check memory systems
        if (brain->getWorkingMemory() == nullptr) {
            // Warning but not an error - working memory is optional
            NLM_LOG_WARNING("Working memory is not available");
        }
        
        return true;
    }
    
    /**
     * Validate simulation step parameters
     */
    static bool validateStepParameters(SimulationStep step, Timestamp time) {
        if (step < 0) {
            throw NLMError("Step must be non-negative", ErrorCategory::Validation, "Step validation");
        }
        
        if (time < 0.0) {
            throw NLMError("Time must be non-negative", ErrorCategory::Validation, "Time validation");
        }
        
        if (time > 1000000.0) { // 1000 seconds maximum
            throw NLMError("Time is too large (maximum: 1000 seconds)", ErrorCategory::Validation, "Time validation");
        }
        
        return true;
    }
    
private:
    static void logError(const NLMError& error, const std::string& context) {
        // Log error with full details
        std::cerr << "ERROR: " << error.getUserFriendlyMessage() << std::endl;
        
        // In a real implementation, this would log to a proper logging system
        // with different levels and outputs
    }
    
    static bool canRecoverFromError(const NLMError& error) {
        // Determine if error can be recovered from
        switch (error.getCategory()) {
            case ErrorCategory::Configuration:
                return false; // Configuration errors usually require manual fix
            case ErrorCategory::System:
                return false; // System errors usually require restart
            case ErrorCategory::FileIO:
                return true; // File I/O errors might be recoverable
            case ErrorCategory::Memory:
                return false; // Memory errors usually fatal
            case ErrorCategory::Neural:
                return true; // Some neural errors might be recoverable
            default:
                return false;
        }
    }
    
    static bool attemptRecovery(const NLMError& error, const std::string& context) {
        // Attempt to recover from error
        switch (error.getCategory()) {
            case ErrorCategory::FileIO:
                // Try to recover from file I/O error
                return attemptFileIORecovery(error);
            case ErrorCategory::Neural:
                // Try to recover from neural error
                return attemptNeuralRecovery(error);
            default:
                return false;
        }
    }
    
    static bool attemptFileIORecovery(const NLMError& error) {
        // Implementation for file I/O error recovery
        // Could involve retrying operations, using backup files, etc.
        return false;
    }
    
    static bool attemptNeuralRecovery(const NLMError& error) {
        // Implementation for neural error recovery
        // Could involve resetting neural states, applying plasticity, etc.
        return false;
    }
    
    static void suggestAlternatives(const NLMError& error) {
        // Suggest alternatives based on error type
        switch (error.getCategory()) {
            case ErrorCategory::Configuration:
                std::cerr << "SUGGESTION: Check configuration file or command line parameters" << std::endl;
                break;
            case ErrorCategory::FileIO:
                std::cerr << "SUGGESTION: Check file permissions and disk space" << std::endl;
                break;
            case ErrorCategory::Memory:
                std::cerr << "SUGGESTION: Reduce simulation parameters or increase system resources" << std::endl;
                break;
            default:
                std::cerr << "SUGGESTION: Try running with --help for more information" << std::endl;
                break;
        }
    }
};

// Input validation utilities
class InputValidator {
public:
    InputValidator() = default;
    ~InputValidator() = default;
    
    /**
     * Validate file path
     */
    static bool validateFilePath(const std::string& path, bool mustExist = false) {
        if (path.empty()) {
            return false;
        }
        
        // Check for invalid characters
        static const std::regex invalidChars("[\\/:*?\"<>|]");
        if (std::regex_search(path, invalidChars)) {
            return false;
        }
        
        if (mustExist) {
            std::ifstream file(path);
            return file.good();
        }
        
        return true;
    }
    
    /**
     * Validate directory path
     */
    static bool validateDirectoryPath(const std::string& path, bool mustExist = false) {
        if (path.empty()) {
            return false;
        }
        
        // Check for invalid characters
        static const std::regex invalidChars("[\\/:*?\"<>|]");
        if (std::regex_search(path, invalidChars)) {
            return false;
        }
        
        if (mustExist) {
            std::error_code ec;
            bool exists = std::filesystem::exists(path, ec);
            bool isDirectory = std::filesystem::is_directory(path, ec);
            return exists && isDirectory && !ec;
        }
        
        return true;
    }
    
    /**
     * Validate string contains only allowed characters
     */
    static bool validateString(const std::string& str, 
                               const std::string& allowedChars = "a-zA-Z0-9_.-") {
        static const std::regex allowedPattern("^[" + allowedChars + "]*$", std::regex::ECMAScript);
        return std::regex_match(str, allowedPattern);
    }
    
    /**
     * Validate parameter ranges
     */
    template<typename T>
    static bool validateRange(const T& value, const T& min, const T& max) {
        return value >= min && value <= max;
    }
    
    /**
     * Validate that a number is within reasonable bounds
     */
    static bool validateReasonableValue(double value, double min, double max, 
                                      const std::string& paramName) {
        if (value < min || value > max) {
            std::cerr << "WARNING: " << paramName << " value (" << value 
                     << ") is outside recommended range (" << min << " - " << max << ")" << std::endl;
            return false;
        }
        return true;
    }
};

} // namespace nlm
