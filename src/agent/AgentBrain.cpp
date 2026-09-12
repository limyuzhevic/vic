// NLM Agent Brain Implementation
// Improved version with better configuration management, bounds checking, and reduced magic numbers

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Configuration constants for improved code maintainability
namespace AgentBrainConstants {
    // Sensory scaling factors (configurable via Brain config)
    constexpr float DEFAULT_VISION_SCALE = 5.0f;
    constexpr float DEFAULT_TOUCH_SCALE = 8.0f;
    constexpr float DEFAULT_INTERNAL_SCALE = 5.0f;
    constexpr float DEFAULT_PROPRIOCEPTION_SCALE = 3.0f;
    
    // Activity thresholds
    constexpr float MIN_ACTIVITY_THRESHOLD = 0.01f;
    constexpr float MOTOR_ACTIVATTION_THRESHOLD = 0.5f;
    constexpr float MIN_MOTOR_ACTIVITY = 0.5f;
    
    // Neuromodulation parameters
    constexpr float REWARD_UPDATE_DECAY = 0.95f;
    constexpr float REWARD_UPDATE_GAIN = 0.05f;
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    constexpr float ELIGIBILITY_THRESHOLD = 0.001f;
    constexpr float ELIGIBILITY_DECAY = 0.1f;
    
    // Plasticity modulation
    constexpr float PLASTICITY_BASE_FACTOR = 0.5f;
    constexpr float PLASTICITY_DOPAMINE_GAIN = 0.5f;
    constexpr float PLASTICITY_CLAMP_MIN = 0.1f;
    constexpr float PLASTICITY_CLAMP_MAX = 2.0f;
    
    // Development stages
    constexpr double INITIAL_STAGE_AGE = 60.0;  // ~1 minute
    constexpr double CRITICAL_PERIOD_AGE = 300.0;  // ~5 minutes
    constexpr double MATURATION_AGE = 900.0;   // ~15 minutes
    
    // Synaptogenesis and pruning rates
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    constexpr float SYNAPTOGENESIS_GAIN = 2.0f;
    
    // Novelty and curiosity parameters
    constexpr float NOVELTY_DECAY_RATE = 0.99f;
    constexpr float CURIOUSITY_NOVELTY_WEIGHT = 2.0f;
    constexpr float CURIOUSITY_PREDICTION_ERROR_WEIGHT = 0.5f;
    constexpr float CURIOUSITY_LEVEL_CLAMP_MAX = 1.0f;
    constexpr float EXPLORATION_THRESHOLD = 0.5f;
    constexpr float EXPLORATION_MAX_CHANCE = 0.3f;
    
    // Motor command ranges
    constexpr int MOTOR_COMMAND_RANDOM_MIN = 0;
    constexpr int MOTOR_COMMAND_RANDOM_MAX = 7;
}

// Helper function to get scale factor from config or use default
static float getScaleFactor(const Config& config, const std::string& paramName, float defaultValue) {
    return config.hasValue(paramName) ? config.getOr<float>(paramName, defaultValue) : defaultValue;
}

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(AgentBrainConstants::NOVELTY_DECAY_RATE)
{
    // Validate brain pointer
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain initialized with null brain pointer");
        return;
    }
    
    // Initialize motor and sensory neuron groups
    initializeNeuronGroups();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Validate world
    if (!world.getWidth() || !world.getHeight()) {
        NLM_LOG_ERROR("AgentBrain initialized with invalid world dimensions");
        return;
    }
    
    // Initialize previous vision buffer
    size_t visionSize = world.getVisionWidth() * world.getVisionHeight();
    previousVision_.resize(visionSize, 0.0f);
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Log initialization statistics
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Get scale factors from brain config if available
    float visionScale = AgentBrainConstants::DEFAULT_VISION_SCALE;
    float touchScale = AgentBrainConstants::DEFAULT_TOUCH_SCALE;
    float internalScale = AgentBrainConstants::DEFAULT_INTERNAL_SCALE;
    float proprioceptionScale = AgentBrainConstants::DEFAULT_PROPRIOCEPTION_SCALE;
    
    if (brain_->getConfig()) {
        visionScale = getScaleFactor(*brain_->getConfig(), "agent_vision_scale", visionScale);
        touchScale = getScaleFactor(*brain_->getConfig(), "agent_touch_scale", touchScale);
        internalScale = getScaleFactor(*brain_->getConfig(), "agent_internal_scale", internalScale);
        proprioceptionScale = getScaleFactor(*brain_->getConfig(), "agent_proprioception_scale", proprioceptionScale);
    }
    
    // Process vision input (256 values -> sensoryVision_ neurons)
    processVisionInput(percept.getVision(), visionScale);
    
    // Process touch input (8 values -> sensoryTouch_ neurons)
    processTouchInput(percept.getTouch(), touchScale);
    
    // Process internal signals (4 values -> sensoryInternal_ neurons)
    processInternalInput(percept.getInternal(), internalScale);
    
    // Process proprioception (6 values -> sensoryProprioception_ neurons)
    processProprioceptionInput(percept.getProprioception(), proprioceptionScale);
    
    // Compute novelty (difference from previous vision)
    computeNovelty(percept.getVision());
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        updateCuriosity();
    }
}

void AgentBrain::updateCuriosity() {
    curiosityLevel_ = noveltyLevel_ * AgentBrainConstants::CURIOUSITY_NOVELTY_WEIGHT +
                     std::abs(predictionError_) * AgentBrainConstants::CURIOUSITY_PREDICTION_ERROR_WEIGHT;
    
    // Clamp curiosity level
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, AgentBrainConstants::CURIOUSITY_LEVEL_CLAMP_MAX);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > AgentBrainConstants::EXPLORATION_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (!n) continue;  // Safety check
            
            // Use membrane potential deviation from rest as activity measure
            float potential = n->getState().membranePotential;
            float resting = n->getState().restingPotential;
            sum += std::abs(potential - resting);
        }
        
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < AgentBrainConstants::MIN_MOTOR_ACTIVITY) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > AgentBrainConstants::EXPLORATION_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * AgentBrainConstants::EXPLORATION_MAX_CHANCE;
        
        // Generate random number safely
        if (brain_ && brain_->getRandomGenerator()) {
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command
                int choice = brain_->getRandomGenerator()->uniformInt(
                    AgentBrainConstants::MOTOR_COMMAND_RANDOM_MIN,
                    AgentBrainConstants::MOTOR_COMMAND_RANDOM_MAX);
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
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error with clipping
    predictionError_ = reward - predictedReward;
    predictionError_ = std::clamp(predictionError_, -1.0f, 1.0f);
    
    // Update expected reward (exponential moving average)
    expectedReward_ = AgentBrainConstants::REWARD_UPDATE_DECAY * expectedReward_ +
                      AgentBrainConstants::REWARD_UPDATE_GAIN * reward;
    
    // Clamp expected reward
    expectedReward_ = std::clamp(expectedReward_, 0.0f, 1.0f);
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, 
                                AgentBrainConstants::DOPAMINE_CLAMP_MIN,
                                AgentBrainConstants::DOPAMINE_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    applyRewardToSynapses(dopamineLevel_);
    
    // Modulate plasticity based on dopamine
    modulatePlasticityBasedOnDopamine(dopamineLevel_);
}

void AgentBrain::applyRewardToSynapses(float dopamineLevel) {
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;  // Safety check
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > AgentBrainConstants::ELIGIBILITY_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(AgentBrainConstants::ELIGIBILITY_DECAY);
            }
        }
    }
}

void AgentBrain::modulatePlasticityBasedOnDopamine(float dopamineLevel) {
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = AgentBrainConstants::PLASTICITY_BASE_FACTOR +
                             AgentBrainConstants::PLASTICITY_DOPAMINE_GAIN * dopamineLevel;
    
    // Clamp plasticity factor
    plasticityFactor = std::clamp(plasticityFactor,
                                 AgentBrainConstants::PLASTICITY_CLAMP_MIN,
                                 AgentBrainConstants::PLASTICITY_CLAMP_MAX);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        // Update STDP parameters based on plasticity factor
        float baseLTP = 0.01f;
        float baseLTD = 0.012f;
        
        stdp->setLTPWeight(baseLTP * plasticityFactor);
        stdp->setLTDWeight(baseLTD * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Update developmental stage based on age
    updateDevelopmentalStage(developmentalAge_);
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        updateStructuralPlasticity();
    }
}

void AgentBrain::updateDevelopmentalStage(double age) {
    if (age < AgentBrainConstants::INITIAL_STAGE_AGE) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (age < AgentBrainConstants::CRITICAL_PERIOD_AGE) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (age < AgentBrainConstants::MATURATION_AGE) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

void AgentBrain::updateStructuralPlasticity() {
    auto* sp = brain_->getStructuralPlasticity();
    if (!sp) return;
    
    // Higher synaptogenesis in early development
    float synRate = AgentBrainConstants::DEFAULT_SYNAPTOGENESIS_RATE * plasticityModifier_;
    float pruneRate = AgentBrainConstants::DEFAULT_PRUNING_RATE * 
                      (AgentBrainConstants::SYNAPTOGENESIS_GAIN - plasticityModifier_);
    
    // Clamp rates to reasonable values
    synRate = std::clamp(synRate, 0.0f, 0.01f);
    pruneRate = std::clamp(pruneRate, 0.0f, 0.001f);
    
    sp->setSynaptogenesisRate(synRate);
    sp->setPruningRate(pruneRate);
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
