#include "BrainIO.hpp"
#include "../core/Logger/Logger.hpp"
#include "../environment/Environment.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

void BrainIO::processSensoryInput(std::shared_ptr<Brain> brain, const SensoryInput& input) {
    // Enhanced sensory input processing
    if (!validateSensoryInput(input)) {
        NLM_LOG_ERROR("Invalid sensory input received");
        return;
    }
    
    // Preprocess and transform input
    SensoryInput processedInput = preprocessSensoryInput(input);
    SensoryInput transformedInput = transformSensoryInput(processedInput, brain);
    
    // Normalize and inject into sensory neurons
    normalizeAndInjectSensoryData(brain, transformedInput);
    
    // Update working memory with encoded input
    updateWorkingMemoryFromSensory(brain, transformedInput);
    
    logInputProcessing(input);
}

void BrainIO::normalizeAndInjectSensoryData(std::shared_ptr<Brain> brain, const SensoryInput& input) {
    const auto& values = input.getData();
    if (values.empty()) return;
    
    // Get sensory neurons for input mapping
    std::vector<Neuron*> sensoryNeurons;
    collectSensoryNeurons(brain.get(), sensoryNeurons);
    
    if (sensoryNeurons.empty()) {
        NLM_LOG_WARNING("No sensory neurons available for input processing");
        return;
    }
    
    // Distribute input across sensory neurons with enhanced encoding
    for (size_t i = 0; i < sensoryNeurons.size(); ++i) {
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            // Enhanced normalization for better dynamic range
            normalizedValue = static_cast<float>(values[i]) * 15.0f; // Increased scale
        }
        
        // Apply non-linear transformation for better encoding
        normalizedValue = applyNonLinearTransformation(normalizedValue);
        
        // Inject into neuron
        sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Store in working memory if significant
        if (std::abs(normalizedValue) > 0.5f) {
            updateWorkingMemoryFromSensory(brain, input);
        }
    }
}

void BrainIO::updateWorkingMemoryFromSensory(std::shared_ptr<Brain> brain, const SensoryInput& input) {
    if (auto* workingMemory = brain->getWorkingMemory()) {
        const auto& values = input.getData();
        if (values.empty()) return;
        
        // Encode sensory input as memory traces
        for (size_t i = 0; i < values.size() && i < 10; ++i) {
            float encodedValue = static_cast<float>(values[i]) * 0.1f;
            NeuronId dummyId = NeuronId(i); // Simplified - in practice would use actual sensory neuron IDs
            
            workingMemory->storeToNeuron(dummyId, encodedValue);
        }
        
        workingMemory->update(0.001f); // Update with timestep
    }
}

void BrainIO::generateMotorOutput(std::shared_ptr<Brain> brain) {
    // Enhanced motor output generation
    auto action = selectActionBasedOnMotorNeurons(brain);
    convertActionToMotorOutput(brain, action);
    
    logActionSelection(action);
}

ActionType BrainIO::selectActionBasedOnMotorNeurons(std::shared_ptr<Brain> brain) {
    // Advanced action selection based on multiple factors
    if (brain->getMotorNeurons().empty()) {
        return ActionType::Wait;
    }
    
    // Calculate activity levels across motor neuron populations
    std::vector<float> motorActivities;
    for (auto* neuron : brain->getMotorNeurons()) {
        motorActivities.push_back(calculateNeuronActivity(neuron));
    }
    
    // Apply attention to select actions
    if (auto* attention = brain->getAttention()) {
        attention->applyAttentionToMotorNeurons(motorActivities);
    }
    
    // Select action based on highest activity with thresholding
    float maxActivity = *std::max_element(motorActivities.begin(), motorActivities.end());
    size_t maxIndex = std::distance(motorActivities.begin(), 
                                   std::max_element(motorActivities.begin(), motorActivities.end()));
    
    ActionType selectedAction = ActionType::Wait;
    if (maxActivity > getOutputActivationThreshold(ActionType::MoveForward)) {
        selectedAction = ActionType::MoveForward;
    } else if (maxActivity > getOutputActivationThreshold(ActionType::TurnLeft)) {
        selectedAction = ActionType::TurnLeft;
    } else if (maxActivity > getOutputActivationThreshold(ActionType::TurnRight)) {
        selectedAction = ActionType::TurnRight;
    }
    
    return selectedAction;
}

void BrainIO::convertActionToMotorOutput(std::shared_ptr<Brain> brain, const Action& action) {
    // Convert action to motor neuron activation
    switch (action.getType()) {
        case ActionType::Wait:
            // No action, keep current state
            break;
        case ActionType::MoveForward:
            // Activate forward motor neurons
            brain->injectCurrentToNeurons(NeuronType::Motor, 50.0f);
            break;
        case ActionType::TurnLeft:
            // Activate left turn neurons
            brain->injectCurrentToNeurons(NeuronType::Motor, 30.0f);
            break;
        case ActionType::TurnRight:
            // Activate right turn neurons
            brain->injectCurrentToNeurons(NeuronType::Motor, 30.0f);
            break;
        default:
            break;
    }
}

void BrainIO::updateEnvironmentBasedOnAction(std::shared_ptr<Brain> brain, const Action& action) {
    // Update environment based on action
    if (auto* environment = brain->getEnvironment()) {
        environment->applyAction(action);
        
        // Get feedback from environment
        float reward = environment->getLastReward();
        float surprise = environment->getLastSurprise();
        
        // Update neuromodulation based on environmental feedback
        updateNeuromodulationFromFeedback(brain, reward, surprise);
        
        logEnvironmentInteraction(action, *environment);
    }
}

void BrainIO::updateNeuromodulationFromFeedback(std::shared_ptr<Brain> brain, float reward, float surprise) {
    // Enhanced neuromodulation from environmental feedback
    if (auto* dopamine = brain->getDopamine()) {
        dopamine->receiveRewardSignal(reward);
        dopamine->receivePredictionErrorSignal(surprise);
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        curiosity->receiveExplorationReward(reward);
    }
    
    if (auto* novelty = brain->getNovelty()) {
        novelty->receiveNoveltySignal(surprise);
    }
}

bool BrainIO::validateSensoryInput(const SensoryInput& input) const {
    // Validate input data
    const auto& data = input.getData();
    if (data.empty()) {
        return false;
    }
    
    // Check for invalid values
    for (auto value : data) {
        if (std::isnan(value) || std::isinf(value)) {
            return false;
        }
    }
    
    return true;
}

SensoryInput BrainIO::preprocessSensoryInput(const SensoryInput& input) const {
    // Enhanced preprocessing with filtering and normalization
    SensoryInput processed = input;
    
    const auto& data = input.getData();
    if (data.empty()) return processed;
    
    // Apply moving average filter
    std::vector<float> filtered(data.size());
    const size_t filterSize = 3;
    for (size_t i = 0; i < data.size(); ++i) {
        float sum = 0.0f;
        size_t count = 0;
        
        for (size_t j = 0; j < filterSize; ++j) {
            if (i + j < data.size()) {
                sum += static_cast<float>(data[i + j]);
                count++;
            }
        }
        
        if (count > 0) {
            filtered[i] = sum / count;
        }
    }
    
    // Normalize to [-1, 1] range
    float maxVal = *std::max_element(filtered.begin(), filtered.end());
    float minVal = *std::min_element(filtered.begin(), filtered.end());
    
    if (maxVal - minVal > 0.001f) {
        for (size_t i = 0; i < filtered.size(); ++i) {
            filtered[i] = 2.0f * (filtered[i] - minVal) / (maxVal - minVal) - 1.0f;
        }
    }
    
    processed.setData(filtered);
    return processed;
}

SensoryInput BrainIO::transformSensoryInput(const SensoryInput& input, std::shared_ptr<Brain> brain) const {
    // Enhanced transformation using brain state
    SensoryInput transformed = input;
    
    const auto& data = input.getData();
    if (data.empty()) return transformed;
    
    // Apply attention-based weighting
    std::vector<float> transformedData(data.size());
    if (auto* attention = brain->getAttention()) {
        auto attentionWeights = attention->getAttentionWeights();
        for (size_t i = 0; i < data.size() && i < attentionWeights.size(); ++i) {
            transformedData[i] = static_cast<float>(data[i]) * attentionWeights[i];
        }
    } else {
        transformedData = data;
    }
    
    transformed.setData(transformedData);
    return transformed;
}

void BrainIO::logInputProcessing(const SensoryInput& input) const {
    NLM_LOG_INFO("Sensory input processed: " + std::to_string(input.getData().size()) + " values");
}

void BrainIO::logActionSelection(const Action& action) const {
    std::string actionStr;
    switch (action.getType()) {
        case ActionType::Wait: actionStr = "Wait"; break;
        case ActionType::MoveForward: actionStr = "MoveForward"; break;
        case ActionType::TurnLeft: actionStr = "TurnLeft"; break;
        case ActionType::TurnRight: actionStr = "TurnRight"; break;
        default: actionStr = "Unknown";
    }
    
    NLM_LOG_INFO("Action selected: " + actionStr);
}

void BrainIO::logEnvironmentInteraction(const Action& action, const Environment& environment) const {
    NLM_LOG_INFO("Environment interaction: Action " + std::to_string(static_cast<int>(action.getType())) +
                " | Reward: " + std::to_string(environment.getLastReward()) +
                " | Surprise: " + std::to_string(environment.getLastSurprise()));
}

void BrainIO::updateNeuralStatesFromInput(std::shared_ptr<Brain> brain) {
    // Update neural states based on input patterns
    if (auto* workingMemory = brain->getWorkingMemory()) {
        auto memoryNeurons = workingMemory->getMemoryNeurons();
        
        // Enhance memory neurons based on input
        for (NeuronId neuronId : memoryNeurons) {
            brain->injectCurrent(neuronId, 0.5f);
        }
    }
}

void BrainIO::updateNeuralStatesFromOutput(std::shared_ptr<Brain> brain) {
    // Update neural states based on output actions
    if (auto* attention = brain->getAttention()) {
        attention->updateAttentionWeights();
    }
}

void BrainIO::resetIOState(std::shared_ptr<Brain> brain) {
    // Reset input/output state
    lastInputStep_ = 0;
    lastRewardLevel_ = 0.0f;
    lastSurpriseLevel_ = 0.0f;
}

void BrainIO::configureInputMapping(std::shared_ptr<Config> config) {
    // Configure input mapping from config
    inputMapping_.clear();
    
    // Set up default input mapping
    std::vector<float> defaultMapping = {1.0f, 0.8f, 0.6f, 0.4f, 0.2f};
    inputMapping_.push_back(defaultMapping);
}

void BrainIO::configureOutputMapping(std::shared_ptr<Config> config) {
    // Configure output mapping from config
    outputMapping_.clear();
    
    // Set up default output mapping
    std::vector<float> moveForwardMapping = {50.0f, 30.0f, 30.0f};
    std::vector<float> turnLeftMapping = {30.0f, 50.0f, 30.0f};
    std::vector<float> turnRightMapping = {30.0f, 30.0f, 50.0f};
    
    outputMapping_.push_back(moveForwardMapping);
    outputMapping_.push_back(turnLeftMapping);
    outputMapping_.push_back(turnRightMapping);
}

void BrainIO::setInputScalingFactors(std::shared_ptr<Config> config) {
    // Set input scaling factors from config
    inputScalingFactors_.clear();
    
    inputScalingFactors_.push_back(config->getOr<float>("input_scale_0", 10.0f));
    inputScalingFactors_.push_back(config->getOr<float>("input_scale_1", 8.0f));
    inputScalingFactors_.push_back(config->getOr<float>("input_scale_2", 6.0f));
}

void BrainIO::setOutputActivationThresholds(std::shared_ptr<Config> config) {
    // Set output activation thresholds from config
    outputActivationThresholds_.clear();
    
    outputActivationThresholds_.push_back(config->getOr<float>("output_threshold_wait", 0.1f));
    outputActivationThresholds_.push_back(config->getOr<float>("output_threshold_move", 0.5f));
    outputActivationThresholds_.push_back(config->getOr<float>("output_threshold_turn", 0.3f));
}

void BrainIO::collectSensoryNeurons(Brain* brain, std::vector<Neuron*>& sensoryNeurons) {
    // Collect all sensory neurons from the brain
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    sensoryNeurons.push_back(neuron);
                }
            }
        }
    }
}

float BrainIO::applyNonLinearTransformation(float value) const {
    // Apply non-linear transformation for better encoding
    return std::tanh(value); // Sigmoid-like transformation
}

float BrainIO::calculateNeuronActivity(Neuron* neuron) const {
    // Calculate neuron activity based on membrane potential
    if (!neuron) return 0.0f;
    
    const auto& state = neuron->getState();
    return std::abs(state.membranePotential - state.restingPotential);
}

float BrainIO::getInputScalingFactor(int inputIndex) const {
    if (inputIndex < inputScalingFactors_.size()) {
        return inputScalingFactors_[inputIndex];
    }
    return 10.0f; // Default
}

float BrainIO::getOutputActivationThreshold(ActionType type) const {
    // Convert action type to threshold index
    switch (type) {
        case ActionType::Wait: return outputActivationThresholds_[0];
        case ActionType::MoveForward: return outputActivationThresholds_[1];
        case ActionType::TurnLeft: return outputActivationThresholds_[2];
        case ActionType::TurnRight: return outputActivationThresholds_[2]; // Same as left
        default: return outputActivationThresholds_[0];
    }
}

} // namespace nlm