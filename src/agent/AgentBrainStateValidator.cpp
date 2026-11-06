#include "AgentBrainStateValidator.hpp"
#include <cmath>

namespace nlm {

bool AgentBrainStateValidator::validateBrainPointer(const std::shared_ptr<Brain>& brain) {
    return brain != nullptr;
}

bool AgentBrainStateValidator::validateNeuronGroups(const std::vector<Neuron*>& group) {
    // Check for null pointers in the group
    for (const Neuron* neuron : group) {
        if (neuron == nullptr) {
            NLM_LOG_ERROR("Found null neuron in group");
            return false;
        }
    }
    return true;
}

bool AgentBrainStateValidator::validateNeuromodulationLevels(float dopamine, float novelty, 
                                                            float curiosity, float predictionError) {
    // Check for valid ranges
    if (std::isnan(dopamine) || std::isnan(novelty) || std::isnan(curiosity) || 
        std::isnan(predictionError)) {
        NLM_LOG_ERROR("NaN value in neuromodulation levels");
        return false;
    }
    
    // Check reasonable ranges
    if (dopamine < DEFAULT_MIN_DOSE || dopamine > DEFAULT_MAX_DOSE) {
        NLM_LOG_WARNING("Dopamine level out of typical range: " + std::to_string(dopamine));
    }
    
    if (novelty < 0.0f || novelty > 1.0f) {
        NLM_LOG_WARNING("Novelty level out of typical range: " + std::to_string(novelty));
    }
    
    if (curiosity < 0.0f || curiosity > 1.0f) {
        NLM_LOG_WARNING("Curiosity level out of typical range: " + std::to_string(curiosity));
    }
    
    if (predictionError < -10.0f || predictionError > 10.0f) {
        NLM_LOG_WARNING("Prediction error out of typical range: " + std::to_string(predictionError));
    }
    
    return true;
}

bool AgentBrainStateValidator::validateDevelopmentalState(double age, float plasticity) {
    if (age < 0.0 || std::isnan(age)) {
        NLM_LOG_ERROR("Invalid developmental age: " + std::to_string(age));
        return false;
    }
    
    if (plasticity < 0.0f || plasticity > 2.0f) {
        NLM_LOG_WARNING("Plasticity modifier out of expected range: " + std::to_string(plasticity));
    }
    
    return true;
}

bool AgentBrainStateValidator::validateConfigurationFlags(bool reward, bool plasticity, 
                                                         bool development, bool curiosity) {
    // Configuration flags should be consistent
    return true;  // Currently no complex validation needed
}

bool AgentBrainStateValidator::validateCognitiveState(const std::vector<float>& vision,
                                                     const std::vector<float>& touch) {
    if (std::any_of(vision.begin(), vision.end(), [](float v) { return std::isnan(v); })) {
        NLM_LOG_ERROR("NaN value in vision data");
        return false;
    }
    
    if (std::any_of(touch.begin(), touch.end(), [](float t) { return std::isnan(t); })) {
        NLM_LOG_ERROR("NaN value in touch data");
        return false;
    }
    
    if (vision.size() != 256) {  // Standard vision size for SimpleWorld
        NLM_LOG_WARNING("Unexpected vision vector size: " + std::to_string(vision.size()));
    }
    
    if (touch.size() != 8) {  // Standard touch size for SimpleWorld
        NLM_LOG_WARNING("Unexpected touch vector size: " + std::to_string(touch.size()));
    }
    
    return true;
}

bool AgentBrainStateValidator::validateOverallState(const AgentBrainNeuronManager& neuronManager,
                                                   float dopamine, float novelty, float curiosity,
                                                   float predictionError, double developmentalAge,
                                                   float plasticityModifier,
                                                   bool rewardModulationEnabled,
                                                   bool structuralPlasticityEnabled,
                                                   bool developmentEnabled, bool curiosityEnabled,
                                                   const std::vector<float>& lastVision,
                                                   const std::vector<float>& lastTouch,
                                                   const std::string& lastText) {
    bool isValid = true;
    
    // Validate neuron manager
    if (!validateBrainPointer(neuronManager.getBrainPointer())) {
        NLM_LOG_ERROR("Invalid brain pointer in neuron manager");
        isValid = false;
    }
    
    // Validate neuromodulation levels
    if (!validateNeuromodulationLevels(dopamine, novelty, curiosity, predictionError)) {
        isValid = false;
    }
    
    // Validate developmental state
    if (!validateDevelopmentalState(developmentalAge, plasticityModifier)) {
        isValid = false;
    }
    
    // Validate cognitive state
    if (!validateCognitiveState(lastVision, lastTouch)) {
        isValid = false;
    }
    
    // Validate configuration flags
    if (!validateConfigurationFlags(rewardModulationEnabled, structuralPlasticityEnabled,
                                    developmentEnabled, curiosityEnabled)) {
        isValid = false;
    }
    
    return isValid;
}

} // namespace nlm