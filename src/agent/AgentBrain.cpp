#include "AgentBrain.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// AgentBrain implementation - uses new subsystem architecture

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
{
    // Initialize with error checking
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain requires a valid brain pointer");
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Initialize sensory system with world parameters
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

// AgentSensorySystem implementation
void AgentSensorySystem::processSensoryInput(const std::vector<float>& vision,
                                           const std::vector<float>& touch,
                                           const std::vector<float>& internal,
                                           const std::vector<float>& proprioception) {
    // Vision processing
    for (size_t i = 0; i < visionNeurons.size() && i < vision.size(); ++i) {
        NLM_CHECK_BOUNDS(i, visionNeurons.size(), ErrorCode::ValidationOutOfRange, "Vision array access out of bounds");
        NLM_CHECK_NULL(visionNeurons[i], ErrorCode::ValidationNullPointer, "Vision neuron is null");
        float current = vision[i] * constants::NEURON_EXCITATORY_INPUT_SCALE;
        visionNeurons[i]->injectCurrent(current);
    }
    
    // Touch processing
    for (size_t i = 0; i < touchNeurons.size() && i < touch.size(); ++i) {
        NLM_CHECK_BOUNDS(i, touchNeurons.size(), ErrorCode::ValidationOutOfRange, "Touch array access out of bounds");
        NLM_CHECK_NULL(touchNeurons[i], ErrorCode::ValidationNullPointer, "Touch neuron is null");
        float current = touch[i] * constants::NEURON_TOUCH_INPUT_SCALE;
        touchNeurons[i]->injectCurrent(current);
    }
    
    // Internal signals
    for (size_t i = 0; i < internalNeurons.size() && i < internal.size(); ++i) {
        NLM_CHECK_BOUNDS(i, internalNeurons.size(), ErrorCode::ValidationOutOfRange, "Internal array access out of bounds");
        NLM_CHECK_NULL(internalNeurons[i], ErrorCode::ValidationNullPointer, "Internal neuron is null");
        float current = (internal[i] * 2.0f - 1.0f) * constants::NEURON_INTERNAL_INPUT_SCALE;
        internalNeurons[i]->injectCurrent(current);
    }
    
    // Proprioception
    for (size_t i = 0; i < proprioceptionNeurons.size() && i < proprioception.size(); ++i) {
        NLM_CHECK_BOUNDS(i, proprioceptionNeurons.size(), ErrorCode::ValidationOutOfRange, "Proprioception array access out of bounds");
        NLM_CHECK_NULL(proprioceptionNeurons[i], ErrorCode::ValidationNullPointer, "Proprioception neuron is null");
        float current = (proprioception[i] * 2.0f - 1.0f) * constants::NEURON_PROPRIOCEPTION_INPUT_SCALE;
        proprioceptionNeurons[i]->injectCurrent(current);
    }
}

float AgentSensorySystem::calculateNovelty(const std::vector<float>& currentVision) {
    float totalDiff = 0.0f;
    for (size_t i = 0; i < currentVision.size() && i < previousVision.size(); ++i) {
        float diff = std::abs(currentVision[i] - previousVision[i]);
        totalDiff += diff;
    }
    
    return totalDiff / std::max<size_t>(currentVision.size(), 1);
}

void AgentSensorySystem::updateNovelty() {
    noveltyLevel *= noveltyDecayRate;
}

std::vector<float> AgentSensorySystem::getSensoryValues() const {
    std::vector<float> values;
    // Add all sensory system values for output
    return values;
}

// AgentMotorSystem implementation
float AgentMotorSystem::calculateActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

MotorCommand AgentMotorSystem::selectAction(const std::map<MotorCommand, std::vector<Neuron*>>& motorGroups) {
    // Calculate activity for each motor group
    std::vector<std::pair<MotorCommand, float>> activities;
    
    for (const auto& pair : motorGroups) {
        float activity = calculateActivity(pair.second);
        activities.emplace_back(pair.first, activity);
    }
    
    // Find maximum activity
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = activityThreshold;
    
    for (const auto& pair : activities) {
        if (pair.second > bestActivity) {
            bestActivity = pair.second;
            best = pair.first;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentMotorSystem::applyExploration(MotorCommand defaultCommand, float curiosityLevel, RandomGenerator* rng) {
    if (curiosityLevel <= 0.5f) {
        return defaultCommand;
    }
    
    float exploreChance = curiosityLevel * constants::CURIOUSITY_EXPLORATION_MAX;
    float r = rng->uniformReal(0.0f, 1.0f);
    if (r < exploreChance) {
        // Random motor command
        int choice = rng->uniformInt(0, 7);
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::LookLeft;
            case 5: return MotorCommand::LookRight;
            case 6: return MotorCommand::Interact;
            default: return MotorCommand::Wait;
        }
    }
    
    return defaultCommand;
}

std::vector<MotorCommand> AgentMotorSystem::getAvailableCommands() const {
    return {
        MotorCommand::MoveForward,
        MotorCommand::MoveBackward,
        MotorCommand::TurnLeft,
        MotorCommand::TurnRight,
        MotorCommand::LookLeft,
        MotorCommand::LookRight,
        MotorCommand::Interact,
        MotorCommand::Wait
    };
}

// AgentLearningSystem implementation
void AgentLearningSystem::applyRewardModulation(float reward, float predictedReward) {
    if (!rewardModulationEnabled) return;
    
    predictionError = reward - predictedReward;
    updateExpectedReward(reward);
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel = predictionError;
    dopamineLevel = std::clamp(dopamineLevel, constants::DOPAMINE_MIN_LEVEL, constants::DOPAMINE_MAX_LEVEL);
}

void AgentLearningSystem::updateExpectedReward(float reward) {
    expectedReward = 0.95f * expectedReward + 0.05f * reward;
}

float AgentLearningSystem::calculatePredictionError(float reward, float predicted) {
    return reward - predicted;
}

void AgentLearningSystem::modulatePlasticity(float dopamine, STDP* stdp) {
    if (!stdp) return;
    
    float plasticityFactor = 0.5f + 0.5f * dopamine;
    plasticityFactor = std::clamp(plasticityFactor, constants::DOPAMINE_PLASTICITY_MIN, constants::DOPAMINE_PLASTICITY_MAX);
    
    stdp->setLTPWeight(constants::STDP_LTP_WEIGHT_DEFAULT * plasticityFactor);
    stdp->setLTDWeight(constants::STDP_LTD_WEIGHT_DEFAULT * plasticityFactor);
}

float AgentLearningSystem::getNeuromodulationLevel() const {
    return dopamineLevel;
}

// AgentDevelopmentSystem implementation
void AgentDevelopmentSystem::updateDevelopmentalStage(double timestep, Brain* brain) {
    if (!brain || !developmentEnabled) return;
    
    developmentalAge += timestep;
    
    // Update developmental stage based on age
    if (developmentalAge < constants::DEVELOPMENT_INITIAL_STAGE) {
        brain->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge < constants::DEVELOPMENT_CRITICAL_START) {
        brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge < constants::DEVELOPMENT_MATURATION_START) {
        brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        brain->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

DevelopmentalStage AgentDevelopmentSystem::getDevelopmentalStage() const {
    return developmentalStage;
}

bool AgentDevelopmentSystem::isCriticalPeriod() const {
    return developmentEnabled && 
           developmentalAge >= constants::DEVELOPMENT_CRITICAL_START &&
           developmentalAge < constants::DEVELOPMENT_MATURATION_START;
}

float AgentDevelopmentSystem::calculatePlasticityModifier() const {
    if (developmentalStage == DevelopmentalStage::Initial) {
        return constants::PLASTICITY_MODIFIER_INITIAL;
    } else if (developmentalStage == DevelopmentalStage::CriticalPeriod) {
        return constants::PLASTICITY_MODIFIER_CRITICAL;
    } else if (developmentalStage == DevelopmentalStage::Maturation) {
        return constants::PLASTICITY_MODIFIER_MATURATION;
    } else {
        return constants::PLASTICITY_MODIFIER_ADULT;
    }
}

// AgentBrain implementation
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain has no brain pointer - sensory processing skipped");
        return;
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    sensorySystem.processSensoryInput(vision, percept.getTouch(), 
                                      percept.getInternal(), percept.getProprioception());
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float novelty = sensorySystem.calculateNovelty(vision);
        sensorySystem.noveltyLevel = novelty;
        sensorySystem.updateNovelty();
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        learningSystem.dopamineLevel = novelty * constants::NEUROMODULATION_NOVELTY_SENSITIVITY + 
                                       std::abs(learningSystem.predictionError) * 0.5f;
        learningSystem.dopamineLevel = std::clamp(learningSystem.dopamineLevel, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain has no brain pointer - cannot decode motor command");
        return MotorCommand::Wait;
    }
    
    // Build motor groups from agent brain neurons
    std::map<MotorCommand, std::vector<Neuron*>> motorGroups;
    motorGroups[MotorCommand::MoveForward] = motorForward_;
    motorGroups[MotorCommand::MoveBackward] = motorBackward_;
    motorGroups[MotorCommand::TurnLeft] = motorTurnLeft_;
    motorGroups[MotorCommand::TurnRight] = motorTurnRight_;
    motorGroups[MotorCommand::Interact] = motorInteract_;
    motorGroups[MotorCommand::Wait] = motorWait_;
    
    MotorCommand decoded = motorSystem.selectAction(motorGroups);
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && learningSystem.dopamineLevel > constants::NEUROMODULATION_EXPLORATION_THRESHOLD) {
        decoded = motorSystem.applyExploration(decoded, learningSystem.dopamineLevel, brain_->getRandomGenerator());
    }
    
    return decoded;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    learningSystem.applyRewardModulation(reward, predictedReward);
    
    // Modulate plasticity based on dopamine
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        learningSystem.modulatePlasticity(learningSystem.dopamineLevel, stdp);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentSystem.updateDevelopmentalStage(timestep, brain_.get());
    
    // Update learning system plasticity modifier
    learningSystem.plasticityModifier = developmentSystem.calculatePlasticityModifier();
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = constants::SYNAPTOGENESIS_RATE_DEFAULT * developmentSystem.calculatePlasticityModifier();
            float pruneRate = constants::PRUNING_RATE_DEFAULT * (2.0f - developmentSystem.calculatePlasticityModifier());
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

void AgentBrain::reset() {
    learningSystem.dopamineLevel = 0.0f;
    learningSystem.noveltyLevel = 0.0f;
    learningSystem.curiosityLevel = 0.0f;
    learningSystem.predictionError = 0.0f;
    learningSystem.expectedReward = 0.0f;
    developmentSystem.developmentalAge = 0.0;
    learningSystem.plasticityModifier = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

void AgentBrain::logStatus() const {
    NLM_LOG_INFO("=== AgentBrain Status ===");
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(sensoryVision_.size() + sensoryTouch_.size() + 
                     sensoryInternal_.size() + sensoryProprioception_.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(motorForward_.size() + motorBackward_.size() + 
                     motorTurnLeft_.size() + motorTurnRight_.size() + motorInteract_.size() + motorWait_.size()));
    NLM_LOG_INFO("Neuromodulation level: " + std::to_string(learningSystem.dopamineLevel));
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(developmentSystem.developmentalStage)));
    NLM_LOG_INFO("====================================");
}

} // namespace nlm
