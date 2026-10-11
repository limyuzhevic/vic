#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iostream>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
    , state_()
    , config_()
{
    if (!brain_) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    // Initialize neuron groups
    neuronGroups_ = {
        {}, // vision (will be populated in initialize)
        {}, // touch
        {}, // internal
        {}, // proprioception
        {}, // forward
        {}, // backward
        {}, // turnLeft
        {}, // turnRight
        {}, // interact
        {}  // wait
    };
    
    NLM_LOG_INFO("AgentBrain created with brain dependency injection");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain: brain is null");
        return;
    }
    
    // Populate neuron groups from brain regions
    populateNeuronGroups();
    
    // Initialize state
    state_.previousVision.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    state_.developmentalAge = 0.0;
    state_.plasticityModifier = 1.0f;
    
    // Reset performance counters
    resetPerformanceCounters();
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(neuronGroups_.vision.size()) + " vision neurons, " +
                 std::to_string(neuronGroups_.touch.size()) + " touch neurons, " +
                 std::to_string(neuronGroups_.internal.size()) + " internal neurons");
}

void AgentBrain::populateNeuronGroups() {
    if (!brain_) return;
    
    // Clear existing groups
    neuronGroups_ = {
        {}, {}, {}, {}, {}, {}, {}, {}, {}
    };
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& population : region->getPopulations()) {
            NeuronType type = population->getNeuronType();
            auto& neurons = population->getNeurons();
            
            // Distribute neurons to appropriate groups
            if (type == NeuronType::Motor) {
                distributeMotorNeurons(neurons);
            } else if (type == NeuronType::Sensory) {
                distributeSensoryNeurons(neurons);
            }
        }
    }
}

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    // Round-robin distribution among motor groups
    size_t groupSizes[6] = {
        neuronGroups_.forward.size(),
        neuronGroups_.backward.size(),
        neuronGroups_.turnLeft.size(),
        neuronGroups_.turnRight.size(),
        neuronGroups_.interact.size(),
        neuronGroups_.wait.size()
    };
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        size_t groupIdx = 0;
        
        // Find group with minimum neurons for balanced distribution
        for (size_t j = 1; j < 6; ++j) {
            if (groupSizes[j] < groupSizes[groupIdx]) {
                groupIdx = j;
            }
        }
        
        // Add to the selected group
        switch (groupIdx) {
            case 0: neuronGroups_.forward.push_back(neurons[i]); break;
            case 1: neuronGroups_.backward.push_back(neurons[i]); break;
            case 2: neuronGroups_.turnLeft.push_back(neurons[i]); break;
            case 3: neuronGroups_.turnRight.push_back(neurons[i]); break;
            case 4: neuronGroups_.interact.push_back(neurons[i]); break;
            case 5: neuronGroups_.wait.push_back(neurons[i]); break;
        }
        groupSizes[groupIdx]++;
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    // Distribute sensory neurons to specialized groups
    size_t totalVisionCapacity = VISION_NEURON_GROUP_SIZE;
    size_t totalTouchCapacity = TOUCH_NEURON_GROUP_SIZE;
    size_t totalInternalCapacity = INTERNAL_NEURON_GROUP_SIZE;
    size_t totalProprioceptionCapacity = PROPRIOCEPTION_NEURON_GROUP_SIZE;
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        // Assign based on neuron characteristics (would need neuron type info)
        // For now, distribute evenly
        size_t distribution = i % 4; // 4 sensory modalities
        switch (distribution) {
            case 0: 
                if (neuronGroups_.vision.size() < totalVisionCapacity) {
                    neuronGroups_.vision.push_back(neurons[i]);
                }
                break;
            case 1: 
                if (neuronGroups_.touch.size() < totalTouchCapacity) {
                    neuronGroups_.touch.push_back(neurons[i]);
                }
                break;
            case 2: 
                if (neuronGroups_.internal.size() < totalInternalCapacity) {
                    neuronGroups_.internal.push_back(neurons[i]);
                }
                break;
            case 3: 
                if (neuronGroups_.proprioception.size() < totalProprioceptionCapacity) {
                    neuronGroups_.proprioception.push_back(neurons[i]);
                }
                break;
        }
    }
}

void AgentBrain::enableRewardModulation(bool enable) {
    config_.rewardModulationEnabled = enable;
    state_.dopamineLevel = enable ? state_.dopamineLevel : 0.0f;
}

void AgentBrain::enableStructuralPlasticity(bool enable) {
    config_.structuralPlasticityEnabled = enable;
}

void AgentBrain::enableDevelopment(bool enable) {
    config_.developmentEnabled = enable;
    if (!enable) {
        state_.developmentalAge = 0.0;
        state_.plasticityModifier = 1.0f;
    }
}

void AgentBrain::enableCuriosity(bool enable) {
    config_.curiosityEnabled = enable;
}

bool AgentBrain::isRewardModulationEnabled() const { return config_.rewardModulationEnabled; }
bool AgentBrain::isStructuralPlasticityEnabled() const { return config_.structuralPlasticityEnabled; }
bool AgentBrain::isDevelopmentEnabled() const { return config_.developmentEnabled; }
bool AgentBrain::isCuriosityEnabled() const { return config_.curiosityEnabled; }

size_t AgentBrain::getSensoryInputSize() const {
    return neuronGroups_.vision.size() + 
           neuronGroups_.touch.size() + 
           neuronGroups_.internal.size() + 
           neuronGroups_.proprioception.size();
}

size_t AgentBrain::getMotorOutputSize() const {
    return neuronGroups_.forward.size() + 
           neuronGroups_.backward.size() + 
           neuronGroups_.turnLeft.size() + 
           neuronGroups_.turnRight.size() + 
           neuronGroups_.interact.size() + 
           neuronGroups_.wait.size();
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    state_.stepsProcessed++;
    state_.sensoryInputEvents++;
    
    if (!brain_) return;
    
    // Process each sensory modality
    processVisionInput(percept.getVision());
    processTouchInput(percept.getTouch());
    processInternalInput(percept.getInternal());
    processProprioceptionInput(percept.getProprioception());
    
    // Update neuromodulation state
    updateNeuromodulationState();
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < neuronGroups_.vision.size() && i < vision.size(); ++i) {
        if (neuronGroups_.vision[i]) {
            float current = vision[i] * 5.0f;  // Scale factor
            neuronGroups_.vision[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < neuronGroups_.touch.size() && i < touch.size(); ++i) {
        if (neuronGroups_.touch[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            neuronGroups_.touch[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    for (size_t i = 0; i < neuronGroups_.internal.size() && i < internal.size(); ++i) {
        if (neuronGroups_.internal[i]) {
            float current = (internal[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            neuronGroups_.internal[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprioception) {
    for (size_t i = 0; i < neuronGroups_.proprioception.size() && i < proprioception.size(); ++i) {
        if (neuronGroups_.proprioception[i]) {
            float current = (proprioception[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            neuronGroups_.proprioception[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::updateNeuromodulationState() {
    // Update novelty detection
    float novelty = calculateNovelty(neuronGroups_.vision);
    state_.noveltyLevel = novelty * config_.curiosityThreshold;
    
    // Update curiosity based on novelty and prediction error
    updateCuriosityBasedOnNovelty();
    
    // Apply dopamine effects
    if (config_.rewardModulationEnabled) {
        applyDopamineEffects();
    }
}

float AgentBrain::calculateNovelty(const std::vector<Neuron*>& visionNeurons) const {
    // Simplified novelty calculation based on vision input
    // In a real implementation, this would use actual visual data
    float totalActivity = 0.0f;
    for (Neuron* neuron : visionNeurons) {
        if (neuron) {
            totalActivity += std::abs(neuron->getState().membranePotential);
        }
    }
    
    return totalActivity / std::max(static_cast<float>(visionNeurons.size()), 1.0f);
}

void AgentBrain::updateCuriosityBasedOnNovelty() {
    if (!config_.curiosityEnabled) return;
    
    state_.curiosityLevel = state_.noveltyLevel * 2.0f + 
                           std::abs(state_.predictionError) * 0.5f;
    state_.curiosityLevel = std::clamp(state_.curiosityLevel, 0.0f, 1.0f);
}

void AgentBrain::applyDopamineEffects() {
    // Dopamine effects are applied at the synapse level in Brain.cpp
    // Here we update local dopamine level
    state_.dopamineLevel = state_.predictionError;
    state_.dopamineLevel = std::clamp(state_.dopamineLevel, -1.0f, 1.0f);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    state_.motorCommandsProduced++;
    
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (config_.curiosityEnabled && state_.curiosityLevel > config_.curiosityThreshold) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using optimized approach
    auto calcActivity = [this](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        // Use single pass for better cache locality
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (n) {
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(neuronGroups_.forward);
    float backwardAct = calcActivity(neuronGroups_.backward);
    float leftAct = calcActivity(neuronGroups_.turnLeft);
    float rightAct = calcActivity(neuronGroups_.turnRight);
    float interactAct = calcActivity(neuronGroups_.interact);
    float waitAct = calcActivity(neuronGroups_.wait);
    
    // Use std::max_element for cleaner code
    struct CommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandActivity commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    // Find maximum activity using std::max_element
    auto maxIt = std::max_element(std::begin(commands), std::end(commands),
        [](const CommandActivity& a, const CommandActivity& b) {
            return a.activity < b.activity;
        });
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct; // Default to wait if nothing stronger
    
    if (maxIt != std::end(commands) && maxIt->activity > bestActivity) {
        best = maxIt->cmd;
        bestActivity = maxIt->activity;
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (state_.curiosityLevel > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = state_.curiosityLevel * 0.3f;  // Up to 30% random
        
        if (brain_ && brain_->getRandomGenerator()) {
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command selection
                MotorCommand randomCmd = selectRandomMotorCommand();
                if (randomCmd != MotorCommand::Wait) {
                    return randomCmd;
                }
            }
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::selectRandomMotorCommand() {
    // Random motor command selection
    if (brain_ && brain_->getRandomGenerator()) {
        int choice = brain_->getRandomGenerator()->uniformInt(0, 6);
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::Interact;
            case 5: return MotorCommand::Wait;
            default: return MotorCommand::Wait;
        }
    }
    return MotorCommand::Wait;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !config_.rewardModulationEnabled) return;
    
    // Compute prediction error
    state_.predictionError = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    state_.expectedReward = 0.95f * state_.expectedReward + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    state_.dopamineLevel = state_.predictionError;
    
    // Clamp to reasonable range
    state_.dopamineLevel = std::clamp(state_.dopamineLevel, -1.0f, 1.0f);
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !config_.developmentEnabled) return;
    
    state_.developmentalAge += timestep;
    
    // Simple developmental stages based on age
    if (state_.developmentalAge < 60.0) {  // ~1 minute
        state_.plasticityModifier = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (state_.developmentalAge < 300.0) {  // ~5 minutes
        state_.plasticityModifier = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (state_.developmentalAge < 900.0) {  // ~15 minutes
        state_.plasticityModifier = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        state_.plasticityModifier = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const { return state_.dopamineLevel; }
float AgentBrain::getCuriosityLevel() const { return state_.curiosityLevel; }
float AgentBrain::getNoveltyLevel() const { return state_.noveltyLevel; }
float AgentBrain::getPredictionError() const { return state_.predictionError; }

void AgentBrain::configureForGoalOrientedBehavior() {
    config_.curiosityEnabled = true;
    config_.rewardModulationEnabled = true;
    config_.structuralPlasticityEnabled = true;
    config_.developmentEnabled = true;
    
    config_.curiosityThreshold = 0.2f;
    config_.explorationProbability = 0.1f;
}

void AgentBrain::configureForExploration() {
    config_.curiosityEnabled = true;
    config_.rewardModulationEnabled = true;
    config_.structuralPlasticityEnabled = true;
    config_.developmentEnabled = true;
    
    config_.curiosityThreshold = 0.5f;
    config_.explorationProbability = 0.4f;
}

void AgentBrain::configureForExploitation() {
    config_.curiosityEnabled = false;
    config_.rewardModulationEnabled = true;
    config_.structuralPlasticityEnabled = false;
    config_.developmentEnabled = false;
    
    config_.curiosityThreshold = 0.0f;
    config_.explorationProbability = 0.0f;
}

void AgentBrain::optimizeForSpeed() {
    config_.optimizeForSpeed = true;
    config_.optimizeForAccuracy = false;
    
    // Enable fast-path optimizations
    NLM_LOG_INFO("AgentBrain optimized for speed");
}

void AgentBrain::optimizeForAccuracy() {
    config_.optimizeForSpeed = false;
    config_.optimizeForAccuracy = true;
    
    // Enable accuracy-preserving operations
    NLM_LOG_INFO("AgentBrain optimized for accuracy");
}

float AgentBrain::getAverageSensoryResponse() const {
    float totalResponse = 0.0f;
    size_t neuronCount = 0;
    
    // Calculate average response across all sensory neurons
    auto addNeuronResponse = [&](const std::vector<Neuron*>& neurons) {
        for (Neuron* neuron : neurons) {
            if (neuron) {
                totalResponse += std::abs(neuron->getState().membranePotential);
                neuronCount++;
            }
        }
    };
    
    addNeuronResponse(neuronGroups_.vision);
    addNeuronResponse(neuronGroups_.touch);
    addNeuronResponse(neuronGroups_.internal);
    addNeuronResponse(neuronGroups_.proprioception);
    
    return neuronCount > 0 ? totalResponse / neuronCount : 0.0f;
}

float AgentBrain::getMotorConfidence(MotorCommand cmd) const {
    // Calculate confidence based on motor neuron activity
    const std::vector<Neuron*>* motorGroup = nullptr;
    
    switch (cmd) {
        case MotorCommand::MoveForward: motorGroup = &neuronGroups_.forward; break;
        case MotorCommand::MoveBackward: motorGroup = &neuronGroups_.backward; break;
        case MotorCommand::TurnLeft: motorGroup = &neuronGroups_.turnLeft; break;
        case MotorCommand::TurnRight: motorGroup = &neuronGroups_.turnRight; break;
        case MotorCommand::Interact: motorGroup = &neuronGroups_.interact; break;
        case MotorCommand::Wait: motorGroup = &neuronGroups_.wait; break;
    }
    
    if (!motorGroup || motorGroup->empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    for (Neuron* neuron : *motorGroup) {
        if (neuron) {
            totalActivity += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
        }
    }
    
    return totalActivity / motorGroup->size();
}

void AgentBrain::reset() {
    // Reset neuromodulation state
    state_.dopamineLevel = 0.0f;
    state_.noveltyLevel = 0.0f;
    state_.curiosityLevel = 0.0f;
    state_.predictionError = 0.0f;
    state_.expectedReward = 0.0f;
    
    // Reset development state
    state_.developmentalAge = 0.0;
    state_.plasticityModifier = 1.0f;
    
    // Reset performance counters
    resetPerformanceCounters();
    
    // Clear previous vision
    state_.previousVision.clear();
    
    // Log reset
    NLM_LOG_INFO("AgentBrain reset for new episode");
}

size_t AgentBrain::getStepCount() const {
    return state_.stepsProcessed;
}

size_t AgentBrain::getInputCount() const {
    return state_.sensoryInputEvents;
}

size_t AgentBrain::getOutputCount() const {
    return state_.motorCommandsProduced;
}

void AgentBrain::getPerformanceMetrics(std::map<std::string, float>& metrics) const {
    metrics.clear();
    
    // Basic statistics
    metrics["steps_processed"] = static_cast<float>(state_.stepsProcessed);
    metrics["sensory_inputs"] = static_cast<float>(state_.sensoryInputEvents);
    metrics["motor_commands"] = static_cast<float>(state_.motorCommandsProduced);
    
    // Neuromodulation levels
    metrics["dopamine_level"] = state_.dopamineLevel;
    metrics["novelty_level"] = state_.noveltyLevel;
    metrics["curiosity_level"] = state_.curiosityLevel;
    metrics["prediction_error"] = state_.predictionError;
    metrics["expected_reward"] = state_.expectedReward;
    
    // Development state
    metrics["developmental_age"] = static_cast<float>(state_.developmentalAge);
    metrics["plasticity_modifier"] = state_.plasticityModifier;
    
    // Configuration status
    metrics["reward_modulation_enabled"] = config_.rewardModulationEnabled ? 1.0f : 0.0f;
    metrics["structural_plasticity_enabled"] = config_.structuralPlasticityEnabled ? 1.0f : 0.0f;
    metrics["development_enabled"] = config_.developmentEnabled ? 1.0f : 0.0f;
    metrics["curiosity_enabled"] = config_.curiosityEnabled ? 1.0f : 0.0f;
    
    // Performance optimization flags
    metrics["optimize_for_speed"] = config_.optimizeForSpeed ? 1.0f : 0.0f;
    metrics["optimize_for_accuracy"] = config_.optimizeForAccuracy ? 1.0f : 0.0f;
    
    // Calculate derived metrics
    float totalSensoryNeurons = static_cast<float>(
        neuronGroups_.vision.size() + neuronGroups_.touch.size() + 
        neuronGroups_.internal.size() + neuronGroups_.proprioception.size()
    );
    
    float avgSensoryResponse = getAverageSensoryResponse();
    metrics["average_sensory_response"] = avgSensoryResponse;
    
    // Calculate motor command confidence distribution
    float totalMotorCommands = getMotorOutputSize();
    if (totalMotorCommands > 0) {
        float forwardConfidence = getMotorConfidence(MotorCommand::MoveForward);
        float backwardConfidence = getMotorConfidence(MotorCommand::MoveBackward);
        float leftConfidence = getMotorConfidence(MotorCommand::TurnLeft);
        float rightConfidence = getMotorConfidence(MotorCommand::TurnRight);
        float interactConfidence = getMotorConfidence(MotorCommand::Interact);
        float waitConfidence = getMotorConfidence(MotorCommand::Wait);
        
        metrics["motor_confidence_forward"] = forwardConfidence;
        metrics["motor_confidence_backward"] = backwardConfidence;
        metrics["motor_confidence_left"] = leftConfidence;
        metrics["motor_confidence_right"] = rightConfidence;
        metrics["motor_confidence_interact"] = interactConfidence;
        metrics["motor_confidence_wait"] = waitConfidence;
    }
    
    // Development stage (as float for convenience)
    metrics["developmental_stage"] = static_cast<float>(getDevelopmentalStage());
}

void AgentBrain::resetPerformanceCounters() {
    state_.stepsProcessed = 0;
    state_.sensoryInputEvents = 0;
    state_.motorCommandsProduced = 0;
}

} // namespace nlm
