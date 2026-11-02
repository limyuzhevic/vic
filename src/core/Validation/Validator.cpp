#include "Validation/Validator.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>

namespace nlm {

ConfigValidator::ConfigValidator() : verbose(false) {
}

ValidationResult ConfigValidator::validateConfigFile(const std::string& filepath) const {
    ValidationResult result;
    
    // Check if file exists
    std::ifstream file(filepath);
    if (!file.is_open()) {
        result.addError("Configuration file not found: " + filepath);
        return result;
    }
    
    // Try to parse as JSON or key=value format
    std::string line;
    int lineNumber = 0;
    bool isJson = false;
    bool isKeyValue = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
        std::string trimmed = trim(line);
        
        if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == '/') {
            continue;
        }
        
        // Check for JSON format
        if (trimmed.find('{') != std::string::npos || trimmed.find('[') != std::string::npos) {
            isJson = true;
        }
        
        // Check for key=value format
        size_t pos = trimmed.find('=');
        if (pos != std::string::npos) {
            isKeyValue = true;
            
            // Validate key
            std::string key = trim(trimmed.substr(0, pos));
            ValidationResult keyResult = validateKey(key);
            if (!keyResult.valid) {
                for (const auto& error : keyResult.errors) {
                    result.addError("Line " + std::to_string(lineNumber) + ": " + error);
                }
            }
            
            // Validate value
            std::string value = trim(trimmed.substr(pos + 1));
            ValidationResult valueResult = validateValue(key, value);
            if (!valueResult.valid) {
                for (const auto& error : valueResult.errors) {
                    result.addError("Line " + std::to_string(lineNumber) + ": " + error);
                }
            }
        } else if (trimmed.find('{') != std::string::npos || trimmed.find('}') != std::string::npos ||
                   trimmed.find(':') != std::string::npos || trimmed.find('[') != std::string::npos ||
                   trimmed.find(']') != std::string::npos) {
            // Could be JSON
            isJson = true;
        }
    }
    
    file.close();
    
    if (isJson) {
        if (verbose) {
            std::cout << "Detected JSON configuration format" << std::endl;
        }
    } else if (isKeyValue) {
        if (verbose) {
            std::cout << "Detected key=value configuration format" << std::endl;
        }
    } else {
        result.addError("Configuration file appears to be empty or in an unrecognized format");
    }
    
    return result;
}

ValidationResult ConfigValidator::validateConfig(const std::string& key, const std::string& value) const {
    ValidationResult result;
    
    // Basic key validation
    ValidationResult keyResult = validateKey(key);
    if (!keyResult.valid) {
        for (const auto& error : keyResult.errors) {
            result.addError(error);
        }
        return result;
    }
    
    // Key-specific validation
    ValidationResult specificResult = validateKeySpecific(key, value);
    if (!specificResult.valid) {
        for (const auto& error : specificResult.errors) {
            result.addError(error);
        }
        for (const auto& warning : specificResult.warnings) {
            result.addWarning(warning);
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateKey(const std::string& key) const {
    ValidationResult result;
    
    if (key.empty()) {
        result.addError("Key cannot be empty");
    }
    
    if (!isValidKey(key)) {
        result.addError("Invalid key format: " + key + ". Keys should be alphanumeric with underscores");
    }
    
    return result;
}

ValidationResult ConfigValidator::validateKeySpecific(const std::string& key, const std::string& value) const {
    ValidationResult result;
    
    // Dispatch to key-specific validator
    if (key == "random_seed") {
        return validateRandomSeed(key, value);
    } else if (key == "simulation_timestep") {
        return validateSimulationTimestep(key, value);
    } else if (key == "neuron_count") {
        return validateNeuronCount(key, value);
    } else if (key == "region_count") {
        return validateRegionCount(key, value);
    } else if (key == "connection_probability") {
        return validateConnectionProbability(key, value);
    } else if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || key == "stdp_tau") {
        return validateStdpParameters({{key, value}});
    } else if (key == "synaptogenesis_rate" || key == "pruning_rate") {
        return validatePlasticityParameters({{key, value}});
    } else if (key == "environment_width" || key == "environment_height" ||
               key == "vision_width" || key == "vision_height") {
        return validateEnvironmentParameter(key, value);
    } else if (key == "log_level") {
        std::set<std::string> validLogLevels = {"DEBUG", "INFO", "WARNING", "ERROR"};
        return validateEnum(key, value, validLogLevels);
    } else if (key == "log_to_file") {
        return validateBoolean(key, value);
    } else {
        // Generic validation
        result.addWarning("Unknown configuration key: " + key);
    }
    
    return result;
}

ValidationResult ConfigValidator::validateRandomSeed(const std::string& key, const std::string& value) const {
    ValidationResult result;
    
    if (!isInteger(value)) {
        result.addError("random_seed must be an integer");
    } else {
        int64_t seed = std::stoll(value);
        if (seed < 0) {
            result.addError("random_seed must be non-negative");
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateSimulationTimestep(const std::string& key, const std::string& value) const {
    ValidationResult result;
    
    if (!isFloat(value)) {
        result.addError("simulation_timestep must be a floating-point number");
    } else {
        double timestep = std::stod(value);
        if (timestep <= 0.0) {
            result.addError("simulation_timestep must be positive");
        } else if (timestep > 1.0) {
            result.addWarning("simulation_timestep is very large, may cause numerical instability");
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateNeuronCount(const std::string& key, const std::string& value) const {
    ValidationResult result;
    
    if (!isInteger(value)) {
        result.addError("neuron_count must be an integer");
    } else {
        int64_t count = std::stoll(value);
        if (count <= 0) {
            result.addError("neuron_count must be positive");
        } else if (count > 100000) {
            result.addWarning("neuron_count is very large, may require significant computational resources");
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateRegionCount(const std::string& key, const std::string& value) const {
    ValidationResult result;
    
    if (!isInteger(value)) {
        result.addError("region_count must be an integer");
    } else {
        int64_t count = std::stoll(value);
        if (count <= 0) {
            result.addError("region_count must be positive");
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateConnectionProbability(const std::string& key, const std::string& value) const {
    ValidationResult result;
    
    if (!isFloat(value)) {
        result.addError("connection_probability must be a floating-point number");
    } else {
        double prob = std::stod(value);
        if (prob < 0.0 || prob > 1.0) {
            result.addError("connection_probability must be between 0.0 and 1.0");
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateStdpParameters(const std::map<std::string, std::string>& config) const {
    ValidationResult result;
    
    // Extract STDP parameters
    std::map<std::string, std::string> stdpParams;
    for (const auto& pair : config) {
        if (pair.first.find("stdp_") == 0) {
            stdpParams[pair.first] = pair.second;
        }
    }
    
    // Validate each parameter
    for (const auto& pair : stdpParams) {
        if (!isFloat(pair.second)) {
            result.addError(pair.first + " must be a floating-point number");
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validatePlasticityParameters(const std::map<std::string, std::string>& config) const {
    ValidationResult result;
    
    // Extract plasticity parameters
    std::map<std::string, std::string> plasticityParams;
    for (const auto& pair : config) {
        if (pair.first.find("synaptogenesis_") == 0 || pair.first.find("pruning_") == 0) {
            plasticityParams[pair.first] = pair.second;
        }
    }
    
    // Validate each parameter
    for (const auto& pair : plasticityParams) {
        if (!isFloat(pair.second)) {
            result.addError(pair.first + " must be a floating-point number");
        } else {
            double param = std::stod(pair.second);
            if (param < 0.0) {
                result.addError(pair.first + " must be non-negative");
            }
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateEnvironmentParameter(const std::string& key, const std::string& value) const {
    ValidationResult result;
    
    if (!isInteger(value)) {
        result.addError(key + " must be an integer");
    } else {
        int param = std::stoi(value);
        if (param <= 0) {
            result.addError(key + " must be positive");
        }
    }
    
    return result;
}

bool ConfigValidator::isValidKey(const std::string& key) const {
    return !key.empty() && std::all_of(key.begin(), key.end(), [](char c) {
        return std::isalnum(c) || c == '_';
    });
}

std::string ConfigValidator::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// InputValidator implementations
InputValidator::InputValidator() : verbose(false) {
}

ValidationResult InputValidator::validateNeuralActivity(double activity, const std::string& context) const {
    ValidationResult result;
    
    return validateRange("neural_activity", activity, -1.0, 100.0, context);
}

ValidationResult InputValidator::validateSynapticWeight(float weight, const std::string& context) const {
    ValidationResult result;
    
    if (weight < -1.0f || weight > 1.0f) {
        result.addError("Synaptic weight " + std::to_string(weight) + " is outside valid range [-1.0, 1.0]");
    }
    
    return result;
}

ValidationResult InputValidator::validateMembranePotential(double voltage, const std::string& neuronType) const {
    ValidationResult result;
    
    // Different neuron types have different valid ranges
    double minVoltage = -100.0, maxVoltage = 50.0; // Default ranges
    
    if (neuronType == "excitatory" || neuronType.empty()) {
        minVoltage = -70.0;
        maxVoltage = 40.0;
    } else if (neuronType == "inhibitory") {
        minVoltage = -80.0;
        maxVoltage = -40.0;
    } else if (neuronType == "sensory") {
        minVoltage = -60.0;
        maxVoltage = 30.0;
    } else if (neuronType == "motor") {
        minVoltage = -50.0;
        maxVoltage = 50.0;
    }
    
    return validateRange("membrane_potential", voltage, minVoltage, maxVoltage, neuronType);
}

ValidationResult InputValidator::validateCurrentInjection(float current, const std::string& neuronType) const {
    ValidationResult result;
    
    // Different neuron types have different valid ranges
    double minCurrent = -50.0, maxCurrent = 50.0; // Default ranges
    
    if (neuronType == "excitatory") {
        minCurrent = -30.0;
        maxCurrent = 30.0;
    } else if (neuronType == "inhibitory") {
        minCurrent = -20.0;
        maxCurrent = 20.0;
    }
    
    return validateRange("current_injection", current, minCurrent, maxCurrent, neuronType);
}

ValidationResult InputValidator::validateSpikeTime(double spikeTime, double previousSpikeTime,
                                                 double minInterval) const {
    ValidationResult result;
    
    if (spikeTime < previousSpikeTime - 0.001) { // Allow small tolerance
        result.addError("Spike time " + std::to_string(spikeTime) + " is before previous spike time " + std::to_string(previousSpikeTime));
    }
    
    if (spikeTime - previousSpikeTime < minInterval) {
        result.addWarning("Spike interval is very short: " + std::to_string(spikeTime - previousSpikeTime) + "s");
    }
    
    return result;
}

ValidationResult InputValidator::validateNeuronCount(size_t count, const std::string& context) const {
    ValidationResult result;
    
    if (count <= 0) {
        result.addError("Neuron count must be positive: " + std::to_string(count));
    }
    
    if (context == "brain" && count > 100000) {
        result.addWarning("Very large neuron count in brain: " + std::to_string(count));
    }
    
    return result;
}

ValidationResult InputValidator::validateSynapseCount(size_t count, const std::string& context) const {
    ValidationResult result;
    
    if (count <= 0) {
        result.addError("Synapse count must be positive: " + std::to_string(count));
    }
    
    return result;
}

ValidationResult InputValidator::validateFiringRate(double rate, const std::string& neuronType) const {
    ValidationResult result;
    
    // Different neuron types have different valid firing rates
    double minRate = 0.0, maxRate = 1000.0; // Default ranges
    
    if (neuronType == "excitatory") {
        minRate = 0.0;
        maxRate = 500.0;
    } else if (neuronType == "inhibitory") {
        minRate = 0.0;
        maxRate = 300.0;
    } else if (neuronType == "sensory") {
        minRate = 0.0;
        maxRate = 100.0;
    } else if (neuronType == "motor") {
        minRate = 0.0;
        maxRate = 200.0;
    }
    
    return validateRange("firing_rate", rate, minRate, maxRate, neuronType);
}

ValidationResult InputValidator::validateEnergyLevel(double energy, const std::string& context) const {
    ValidationResult result;
    
    if (energy < 0.0) {
        result.addError("Energy level cannot be negative: " + std::to_string(energy));
    }
    
    if (context == "agent" && energy > 100.0) {
        result.addWarning("Very high energy level in agent: " + std::to_string(energy));
    }
    
    return result;
}

ValidationResult InputValidator::validateReward(float reward, const std::string& context) const {
    ValidationResult result;
    
    // Rewards can be negative (punishment) or positive (reinforcement)
    if (std::abs(reward) > 1000.0) {
        result.addWarning("Reward magnitude is very large: " + std::to_string(reward));
    }
    
    return result;
}

ValidationResult InputValidator::validateActionCommand(int action, const std::string& neuronType) const {
    ValidationResult result;
    
    // Validate action command
    if (action < 0) {
        result.addError("Action command cannot be negative: " + std::to_string(action));
    }
    
    // Common action ranges
    if (neuronType == "motor") {
        if (action > 10) {
            result.addWarning("Action command value is high: " + std::to_string(action));
        }
    }
    
    return result;
}

ValidationResult InputValidator::validateAllInputs(const std::map<std::string, double>& inputs) const {
    ValidationResult result;
    
    // Validate each input
    for (const auto& pair : inputs) {
        ValidationResult inputResult = validateInput(pair.first, pair.second);
        if (!inputResult.valid) {
            for (const auto& error : inputResult.errors) {
                result.addError("Input '" + pair.first + "': " + error);
            }
        }
        for (const auto& warning : inputResult.warnings) {
            result.addWarning("Input '" + pair.first + "': " + warning);
        }
    }
    
    return result;
}

// ValidationUtils implementations
std::string ValidationUtils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ValidationUtils::isNumber(const std::string& str) {
    return isInteger(str) || isFloat(str);
}

bool ValidationUtils::isInteger(const std::string& str) {
    if (str.empty() || (str[0] == '-' && str.size() == 1)) return false;
    
    for (char c : str) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}

bool ValidationUtils::isFloat(const std::string& str) {
    std::istringstream iss(str);
    float f;
    iss >> f;
    return !iss.fail() && iss.eof();
}

bool ValidationUtils::isPositive(const std::string& str) {
    if (!isNumber(str)) return false;
    
    std::istringstream iss(str);
    double d;
    iss >> d;
    return d > 0;
}

bool ValidationUtils::isNonNegative(const std::string& str) {
    if (!isNumber(str)) return false;
    
    std::istringstream iss(str);
    double d;
    iss >> d;
    return d >= 0;
}

bool ValidationUtils::isValidFile(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

std::string ValidationUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool ValidationUtils::stringContains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

std::vector<std::string> ValidationUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    
    return result;
}

std::string ValidationUtils::join(const std::vector<std::string>& parts, const std::string& delimiter) {
    std::string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) result += delimiter;
        result += parts[i];
    }
    return result;
}

ValidationResult ConfigValidator::validateRange(const std::string& key, double value,
                                               double min, double max,
                                               const std::string& description) const {
    ValidationResult result;
    
    if (value < min) {
        std::string desc = description.empty() ? key : description;
        result.addError(desc + " value " + std::to_string(value) + " is below minimum " + std::to_string(min));
    }
    
    if (value > max) {
        std::string desc = description.empty() ? key : description;
        result.addError(desc + " value " + std::to_string(value) + " is above maximum " + std::to_string(max));
    }
    
    return result;
}

ValidationResult ConfigValidator::validateEnum(const std::string& key, const std::string& value,
                                               const std::set<std::string>& validValues,
                                               const std::string& description) const {
    ValidationResult result;
    
    if (validValues.find(value) == validValues.end()) {
        std::string desc = description.empty() ? key : description;
        result.addError(desc + " value " + value + " is not in valid list");
        
        std::string validList = "Valid values: ";
        bool first = true;
        for (const auto& valid : validValues) {
            if (!first) validList += ", ";
            validList += valid;
            first = false;
        }
        result.addError(validList);
    }
    
    return result;
}

ValidationResult ConfigValidator::validateString(const std::string& key, const std::string& value,
                                                 size_t minLength, size_t maxLength,
                                                 const std::string& pattern,
                                                 const std::string& description) const {
    ValidationResult result;
    
    std::string desc = description.empty() ? key : description;
    
    if (value.size() < minLength) {
        result.addError(desc + " value is too short (minimum " + std::to_string(minLength) + " characters)");
    }
    
    if (value.size() > maxLength) {
        result.addError(desc + " value is too long (maximum " + std::to_string(maxLength) + " characters)");
    }
    
    if (!pattern.empty()) {
        // Simple pattern matching (could be enhanced)
        if (pattern == "alphanumeric" && !std::all_of(value.begin(), value.end(), [](char c) {
            return std::isalnum(c) || c == '_';
        })) {
            result.addError(desc + " value must be alphanumeric");
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateFile(const std::string& key, const std::string& filepath,
                                               bool required, const std::string& description) const {
    ValidationResult result;
    
    std::string desc = description.empty() ? key : description;
    
    if (required && filepath.empty()) {
        result.addError(desc + " is required");
    } else if (!filepath.empty() && !isValidFile(filepath)) {
        result.addError(desc + " file not found: " + filepath);
    }
    
    return result;
}

ValidationResult InputValidator::validatePositive(const std::string& key, double value,
                                                 double min, const std::string& description) const {
    ValidationResult result;
    
    if (value < min) {
        std::string desc = description.empty() ? key : description;
        result.addError(desc + " value " + std::to_string(value) + " is below minimum " + std::to_string(min));
    }
    
    return result;
}

ValidationResult InputValidator::validateRange(const std::string& key, double value,
                                               double min, double max,
                                               const std::string& description) const {
    ValidationResult result;
    
    if (value < min) {
        std::string desc = description.empty() ? key : description;
        result.addError(desc + " value " + std::to_string(value) + " is below minimum " + std::to_string(min));
    }
    
    if (value > max) {
        std::string desc = description.empty() ? key : description;
        result.addError(desc + " value " + std::to_string(value) + " is above maximum " + std::to_string(max));
    }
    
    return result;
}

ValidationResult InputValidator::validateInput(const std::string& key, double value) const {
    // Dispatch to key-specific validator
    if (key == "neural_activity") {
        return validateNeuralActivity(value, key);
    } else if (key == "synaptic_weight") {
        return validateSynapticWeight(static_cast<float>(value), key);
    } else if (key == "membrane_potential") {
        return validateMembranePotential(value, key);
    } else if (key == "current_injection") {
        return validateCurrentInjection(static_cast<float>(value), key);
    } else if (key == "firing_rate") {
        return validateFiringRate(value, key);
    } else if (key == "energy_level") {
        return validateEnergyLevel(value, key);
    } else if (key == "reward") {
        return validateReward(static_cast<float>(value), key);
    } else {
        // Generic validation
        return validatePositive(key, value);
    }
}

} // namespace nlm