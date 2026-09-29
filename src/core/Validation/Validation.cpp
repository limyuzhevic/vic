#include "Validation.hpp"
#include "core/Config/Config.hpp"
#include <numeric>

namespace nlm {

namespace validation {

// Static member definitions
std::vector<ValidationResult> ConfigValidator::validationResults_;
std::vector<ValidationResult> ValidationManager::validationResults_;
bool ValidationManager::validationEnabled_ = true;

// ConfigValidator implementation
ValidationResult ConfigValidator::validateConfig(const Config& config) {
    ValidationResult result = validateSimulationParameters(config);
    if (!result.valid) {
        return result;
    }
    
    result = validatePlasticityParameters(config);
    if (!result.valid) {
        return result;
    }
    
    result = validateNeuromodulationParameters(config);
    if (!result.valid) {
        return result;
    }
    
    result = validateBrainParameters(config);
    if (!result.valid) {
        return result;
    }
    
    result = validateWorldParameters(config);
    if (!result.valid) {
        return result;
    }
    
    return ValidationResult::success();
}

ValidationResult ConfigValidator::validateSimulationParameters(const Config& config) {
    // Validate simulation timestep
    if (config.has("simulation_timestep")) {
        double timestep = config.getOr<double>("simulation_timestep", 0.001);
        if (!isValidSimulationTimestep(timestep)) {
            return ValidationResult::error(
                "Invalid simulation timestep: " + std::to_string(timestep) +
                ". Must be between 0.0001s and 0.1s",
                "simulation_parameters"
            );
        }
    }
    
    // Validate neuron count
    if (config.has("neuron_count")) {
        size_t neuronCount = config.getOr<size_t>("neuron_count", 1000);
        if (!isValidNeuronCount(neuronCount)) {
            return ValidationResult::error(
                "Invalid neuron count: " + std::to_string(neuronCount) +
                ". Must be between 1 and 1,000,000",
                "simulation_parameters"
            );
        }
    }
    
    // Validate region count
    if (config.has("region_count")) {
        size_t regionCount = config.getOr<size_t>("region_count", 1);
        if (!isValidSize(regionCount, 1, 100)) {
            return ValidationResult::error(
                "Invalid region count: " + std::to_string(regionCount) +
                ". Must be between 1 and 100",
                "simulation_parameters"
            );
        }
    }
    
    // Validate connection probability
    if (config.has("connection_probability")) {
        float connProb = config.getOr<float>("connection_probability", 0.1f);
        if (!isValidConnectionProbability(connProb)) {
            return ValidationResult::error(
                "Invalid connection probability: " + std::to_string(connProb) +
                ". Must be between 0.0 and 1.0",
                "simulation_parameters"
            );
        }
    }
    
    return ValidationResult::success();
}

ValidationResult ConfigValidator::validatePlasticityParameters(const Config& config) {
    // Validate STDP parameters
    if (config.has("stdp_ltp_weight")) {
        float ltpWeight = config.getOr<float>("stdp_ltp_weight", 0.01f);
        if (!isValidSynapticWeight(ltpWeight, 0.0f, 1.0f)) {
            return ValidationResult::error(
                "Invalid STDP LTP weight: " + std::to_string(ltpWeight),
                "plasticity_parameters"
            );
        }
    }
    
    if (config.has("stdp_ltd_weight")) {
        float ltdWeight = config.getOr<float>("stdp_ltd_weight", 0.012f);
        if (!isValidSynapticWeight(ltdWeight, 0.0f, 1.0f)) {
            return ValidationResult::error(
                "Invalid STDP LTD weight: " + std::to_string(ltdWeight),
                "plasticity_parameters"
            );
        }
    }
    
    if (config.has("stdp_tau")) {
        float tau = config.getOr<float>("stdp_tau", 20.0f);
        if (!isValidFloat(tau, 1.0f, 100.0f)) {
            return ValidationResult::error(
                "Invalid STDP time constant: " + std::to_string(tau) +
                ". Must be between 1.0 and 100.0ms",
                "plasticity_parameters"
            );
        }
    }
    
    // Validate Hebbian learning rate
    if (config.has("plasticity_hebbian_learning_rate")) {
        float hebbRate = config.getOr<float>("plasticity_hebbian_learning_rate", 0.001f);
        if (!isValidFloat(hebbRate, 0.0f, 1.0f)) {
            return ValidationResult::error(
                "Invalid Hebbian learning rate: " + std::to_string(hebbRate),
                "plasticity_parameters"
            );
        }
    }
    
    return ValidationResult::success();
}

ValidationResult ConfigValidator::validateNeuromodulationParameters(const Config& config) {
    // Validate dopamine scale
    if (config.has("neuromod_dopamine_scale")) {
        float dopamineScale = config.getOr<float>("neuromod_dopamine_scale", 1.0f);
        if (!isValidFloat(dopamineScale, 0.0f, 10.0f)) {
            return ValidationResult::error(
                "Invalid dopamine scale: " + std::to_string(dopamineScale) +
                ". Must be between 0.0 and 10.0",
                "neuromodulation_parameters"
            );
        }
    }
    
    return ValidationResult::success();
}

ValidationResult ConfigValidator::validateBrainParameters(const Config& config) {
    // Validate brain-specific parameters
    if (config.has("brain.v_thresh")) {
        float v_thresh = config.getOr<float>("brain.v_thresh", -50.0f);
        if (!isValidFloat(v_thresh, -100.0f, 0.0f)) {
            return ValidationResult::error(
                "Invalid neuron threshold: " + std::to_string(v_thresh),
                "brain_parameters"
            );
        }
    }
    
    if (config.has("brain.v_rest")) {
        float v_rest = config.getOr<float>("brain.v_rest", -70.0f);
        if (!isValidFloat(v_rest, -100.0f, 0.0f)) {
            return ValidationResult::error(
                "Invalid resting potential: " + std::to_string(v_rest),
                "brain_parameters"
            );
        }
    }
    
    if (config.has("brain.v_reset")) {
        float v_reset = config.getOr<float>("brain.v_reset", -75.0f);
        if (!isValidFloat(v_reset, -100.0f, 0.0f)) {
            return ValidationResult::error(
                "Invalid reset potential: " + std::to_string(v_reset),
                "brain_parameters"
            );
        }
    }
    
    return ValidationResult::success();
}

ValidationResult ConfigValidator::validateWorldParameters(const Config& config) {
    // Validate world-specific parameters
    if (config.has("world.width")) {
        size_t worldWidth = config.getOr<size_t>("world.width", 20);
        if (!isValidSize(worldWidth, 10, 1000)) {
            return ValidationResult::error(
                "Invalid world width: " + std::to_string(worldWidth) +
                ". Must be between 10 and 1000",
                "world_parameters"
            );
        }
    }
    
    if (config.has("world.height")) {
        size_t worldHeight = config.getOr<size_t>("world.height", 20);
        if (!isValidSize(worldHeight, 10, 1000)) {
            return ValidationResult::error(
                "Invalid world height: " + std::to_string(worldHeight) +
                ". Must be between 10 and 1000",
                "world_parameters"
            );
        }
    }
    
    return ValidationResult::success();
}

std::string ConfigValidator::collectValidationErrors(const Config& config) {
    reset();
    
    validateConfig(config);
    
    std::ostringstream oss;
    bool firstError = true;
    
    for (const auto& result : validationResults_) {
        if (!result.valid) {
            if (!firstError) {
                oss << "\n";
            }
            oss << "  - " << result.errorMessage;
            firstError = false;
        }
    }
    
    if (firstError) {
        oss << "All configuration parameters are valid.";
    }
    
    return oss.str();
}

// NeuralComputationValidator implementation
ValidationResult NeuralComputationValidator::validateNeuronState(const NeuronState& state) {
    if (!isValidNeuronState(state)) {
        return ValidationResult::error(
            "Invalid neuron state: V=" + std::to_string(state.membranePotential) +
            ", V_rest=" + std::to_string(state.restingPotential) +
            ", V_thresh=" + std::to_string(state.threshold) +
            ", refractory=" + std::to_string(state.refractoryPeriod),
            "neuron_state_validation"
        );
    }
    return ValidationResult::success();
}

ValidationResult NeuralComputationValidator::validateSynapticWeight(float weight, const std::string& context) {
    if (!isValidSynapticWeight(weight)) {
        return ValidationResult::error(
            "Invalid synaptic weight: " + std::to_string(weight) +
            ". Must be between 0.0 and 10.0",
            context
        );
    }
    return ValidationResult::success();
}

ValidationResult NeuralComputationValidator::validateCurrent(float current, const std::string& context) {
    if (!isValidFloat(current, -1000.0f, 1000.0f)) {
        return ValidationResult::error(
            "Invalid current: " + std::to_string(current) +
            ". Must be between -1000.0 and 1000.0",
            context
        );
    }
    return ValidationResult::success();
}

ValidationResult NeuralComputationValidator::validateTime(double time, const std::string& context) {
    if (!isValidDouble(time, 0.0, 1000000.0)) {
        return ValidationResult::error(
            "Invalid time: " + std::to_string(time) +
            ". Must be between 0.0 and 1,000,000s",
            context
        );
    }
    return ValidationResult::success();
}

ValidationResult NeuralComputationValidator::validateIntegrationResult(float result, float expectedMin, float expectedMax) {
    if (!isValidFloat(result, expectedMin, expectedMax)) {
        return ValidationResult::error(
            "Integration result " + std::to_string(result) +
            " is outside expected range [" + std::to_string(expectedMin) + ", " + std::to_string(expectedMax) + "]",
            "integration_result_validation"
        );
    }
    return ValidationResult::success();
}

ValidationResult NeuralComputationValidator::validateNormalization(float value, float min, float max) {
    if (!isValidFloat(value, min, max)) {
        return ValidationResult::error(
            "Normalization value " + std::to_string(value) +
            " is outside expected range [" + std::to_string(min) + ", " + std::to_string(max) + "]",
            "normalization_validation"
        );
    }
    
    float normalized = (value - min) / (max - min);
    if (!isValidFloat(normalized, 0.0f, 1.0f)) {
        return ValidationResult::error(
            "Normalized value " + std::to_string(normalized) +
            " is outside [0.0, 1.0] range",
            "normalization_validation"
        );
    }
    
    return ValidationResult::success();
}

ValidationResult NeuralComputationValidator::validateSTDPPotentiation(float weightChange, float maxLTP) {
    if (weightChange > maxLTP) {
        return ValidationResult::error(
            "STDP potentiation " + std::to_string(weightChange) +
            " exceeds maximum LTP of " + std::to_string(maxLTP),
            "stdp_potentiation_validation"
        );
    }
    return ValidationResult::success();
}

ValidationResult NeuralComputationValidator::validateSTDPDepression(float weightChange, float maxLTD) {
    if (weightChange < -maxLTD) {
        return ValidationResult::error(
            "STDP depression " + std::to_string(weightChange) +
            " exceeds maximum LTD of " + std::to_string(maxLTD),
            "stdp_depression_validation"
        );
    }
    return ValidationResult::success();
}

// ValidationManager implementation
ValidationResult ValidationManager::validateAll(const Config& config) {
    if (!validationEnabled_) {
        return ValidationResult::success();
    }
    
    reset();
    validationResults_.push_back(ConfigValidator::validateConfig(config));
    
    for (const auto& result : validationResults_) {
        if (!result.valid) {
            return result;
        }
    }
    
    return ValidationResult::success();
}

std::string ValidationManager::getAllValidationErrors() {
    std::ostringstream oss;
    bool firstError = true;
    
    for (const auto& result : validationResults_) {
        if (!result.valid) {
            if (!firstError) {
                oss << "\n";
            }
            oss << "  - " << result.errorMessage;
            firstError = false;
        }
    }
    
    if (firstError) {
        oss << "All validations passed.";
    }
    
    return oss.str();
}

} // namespace validation

} // namespace nlm
