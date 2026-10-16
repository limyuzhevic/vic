#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

// Constants for AgentBrain operations
namespace AgentConstants {
    constexpr float EPSILON = 1e-6f;
    constexpr float MIN_VISION_VALUE = -1.0f;
    constexpr float MAX_VISION_VALUE = 1.0f;
    constexpr float MIN_TOUCH_VALUE = -1.0f;
    constexpr float MAX_TOUCH_VALUE = 1.0f;
    constexpr float MIN_INTERNAL_VALUE = -1.0f;
    constexpr float MAX_INTERNAL_VALUE = 1.0f;
    constexpr float MIN_PROPRIOCEPTIVE_VALUE = -1.0f;
    constexpr float MAX_PROPRIOCEPTIVE_VALUE = 1.0f;
    constexpr float MIN_ACTIVITY_THRESHOLD = 0.5f;
    constexpr float MIN_NOVELTY_THRESHOLD = 0.1f;
    constexpr float MIN_CURIOUSNESS_THRESHOLD = 0.3f;
    constexpr float MAX_EXPLORATION_CHANCE = 0.3f;
    constexpr float MIN_PREDICTION_ERROR_RANGE = -10.0f;
    constexpr float MAX_PREDICTION_ERROR_RANGE = 10.0f;
    constexpr float MIN_DOPAMINE_LEVEL = -1.0f;
    constexpr float MAX_DOPAMINE_LEVEL = 1.0f;
    constexpr float MIN_PLASTICITY_FACTOR = 0.1f;
    constexpr float MAX_PLASTICITY_FACTOR = 2.0f;
    constexpr float MIN_DEV_STAGE_INIT = 0.0;
    constexpr float MAX_DEV_STAGE_INIT = 60.0;
    constexpr float MIN_DEV_STAGE_CRITICAL = 60.0;
    constexpr float MAX_DEV_STAGE_CRITICAL = 300.0;
    constexpr float MIN_DEV_STAGE_MATURATION = 300.0;
    constexpr float MAX_DEV_STAGE_MATURATION = 900.0;
    constexpr float MIN_DEV_STAGE_ADULT = 900.0;
    constexpr float MIN_DEV_STAGE_AGING = 900.0;
    constexpr size_t MIN_VISION_SIZE = 256;
    constexpr size_t MIN_TOUCH_SIZE = 8;
    constexpr size_t MIN_INTERNAL_SIZE = 4;
    constexpr size_t MIN_PROPRIOCEPTIVE_SIZE = 6;
    constexpr size_t MIN_MOTOR_GROUP_SIZE = 1;
}

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
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
    , sensoryNoveltyDecay_(0.99f)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
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
    // Enhanced null safety and parameter validation
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain: brain_ is null, skipping sensory processing");
        return;
    }
    
    if (!percept) {
        NLM_LOG_ERROR("AgentBrain: percept is null, aborting sensory processing");
        return;
    }
    
    // Validate that neuron groups have been properly initialized
    if (sensoryVision_.empty() && sensoryTouch_.empty() && 
        sensoryInternal_.empty() && sensoryProprioception_.empty()) {
        NLM_LOG_WARNING("AgentBrain: No sensory neurons initialized, skipping sensory processing");
        return;
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    
    // Validate vision data
    if (vision.empty()) {
        NLM_LOG_WARNING("AgentBrain: Vision data is empty, skipping vision processing");
    } else {
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                // Validate and clamp vision values to reasonable range
                float current = vision[i] * 5.0f;  // Scale factor
                
                // Apply bounds checking to prevent extreme current values
                if (current < -50.0f) current = -50.0f;
                if (current > 50.0f) current = 50.0f;
                
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    
    // Validate touch data
    if (touch.empty()) {
        NLM_LOG_WARNING("AgentBrain: Touch data is empty, skipping touch processing");
    } else {
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * 8.0f;  // Collision signal
                
                // Apply bounds checking
                if (current < -40.0f) current = -40.0f;
                if (current > 40.0f) current = 40.0f;
                
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    
    // Validate internal data
    if (intern.empty()) {
        NLM_LOG_WARNING("AgentBrain: Internal data is empty, skipping internal signal processing");
    } else {
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (sensoryInternal_[i]) {
                // Apply bounds checking and clamping for internal signals
                float value = intern[i];
                value = std::max(AgentConstants::MIN_INTERNAL_VALUE, std::min(value, AgentConstants::MAX_INTERNAL_VALUE));
                
                float current = (value * 2.0f - 1.0f) * 5.0f;  // Center and scale
                
                // Apply bounds checking
                if (current < -30.0f) current = -30.0f;
                if (current > 30.0f) current = 30.0f;
                
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    
    // Validate proprioceptive data
    if (proprio.empty()) {
        NLM_LOG_WARNING("AgentBrain: Proprioceptive data is empty, skipping proprioception processing");
    } else {
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception_[i]) {
                // Apply bounds checking and clamping for proprioception
                float value = proprio[i];
                value = std::max(AgentConstants::MIN_PROPRIOCEPTIVE_VALUE, std::min(value, AgentConstants::MAX_PROPRIOCEPTIVE_VALUE));
                
                float current = (value * 2.0f - 1.0f) * 3.0f;  // Center and scale
                
                // Apply bounds checking
                if (current < -25.0f) current = -25.0f;
                if (current > 25.0f) current = 25.0f;
                
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
    }
    
    // Compute novelty (difference from previous vision) - enhanced with validation
    if (!vision.empty() && !previousVision_.empty()) {
        // Validate that previous vision has the same size as current vision
        if (previousVision_.size() == vision.size()) {
            float totalDiff = 0.0f;
            
            for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
                // Validate individual vision values
                float currentValue = std::max(AgentConstants::MIN_VISION_VALUE, std::min(vision[i], AgentConstants::MAX_VISION_VALUE));
                float previousValue = std::max(AgentConstants::MIN_VISION_VALUE, std::min(previousVision_[i], AgentConstants::MAX_VISION_VALUE));
                
                float diff = std::abs(currentValue - previousValue);
                
                // Apply bounds checking to difference calculation
                if (diff > 2.0f) diff = 2.0f;  // Cap maximum difference
                
                totalDiff += diff;
            }
            
            // Normalize with safety check
            size_t numSamples = std::max<size_t>(vision.size(), 1);
            noveltyLevel_ = totalDiff / numSamples;
            
            // Apply novelty decay
            noveltyLevel_ *= sensoryNoveltyDecay_;
            
            // Clamp novelty level to valid range
            noveltyLevel_ = std::max(0.0f, std::min(noveltyLevel_, 1.0f));
            
            // Store for next time
            previousVision_ = vision;
        } else {
            NLM_LOG_WARNING("AgentBrain: Vision size mismatch, cannot compute novelty correctly");
        }
    } else if (!vision.empty() && previousVision_.empty()) {
        // Initialize previous vision with current vision for first iteration
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty - enhanced with validation
    if (curiosityEnabled_) {
        // Validate novelty level calculation
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        
        // Apply bounds checking
        curiosityLevel_ = std::max(0.0f, std::min(curiosityLevel_, 1.0f));
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    // Enhanced motor command decoding with comprehensive safety checks and validation
    
    // Validate brain pointer
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain: brain_ is null, cannot decode motor command");
        return MotorCommand::Wait;
    }
    
    // Validate neuron group sizes
    if (motorForward_.empty() && motorBackward_.empty() && 
        motorTurnLeft_.empty() && motorTurnRight_.empty() &&
        motorInteract_.empty() && motorWait_.empty()) {
        NLM_LOG_WARNING("AgentBrain: No motor neurons initialized, returning Wait");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > AgentConstants::MIN_CURIOUSNESS_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Enhanced motor neuron decoding with comprehensive safety checks and validation
    
    // Validate brain pointer
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain: brain_ is null, cannot decode motor commands from neurons");
        return MotorCommand::Wait;
    }
    
    // Helper function to calculate activity with validation
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        // Validate neuron pointers
        for (const Neuron* n : neurons) {
            if (!n) {
                // Log warning but continue - this shouldn't happen in normal operation
                continue;
            }
        }
        
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (!n) {
                continue;  // Skip null neurons
            }
            
            // Get neuron state with safety check
            const NeuronState& state = n->getState();
            
            // Validate neuron state values
            if (std::isnan(state.membranePotential) || std::isinf(state.membranePotential)) {
                continue;  // Skip invalid neurons
            }
            
            if (std::isnan(state.restingPotential) || std::isinf(state.restingPotential)) {
                continue;  // Skip invalid neurons
            }
            
            // Use membrane potential deviation from rest as activity measure
            float potential = state.membranePotential - state.restingPotential;
            
            // Apply bounds checking
            potential = std::max(-50.0f, std::min(potential, 50.0f));
            
            sum += std::abs(potential);
        }
        
        // Validate sum before division
        if (neurons.size() == 0) {
            return 0.0f;
        }
        
        return sum / neurons.size();
    };
    
    // Calculate activity for each motor group
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity with validation
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
    
    // Enhanced action selection with confidence check
    if (bestActivity < AgentConstants::MIN_ACTIVITY_THRESHOLD) {
        NLM_LOG_DEBUG("AgentBrain: Low motor activity (" + std::to_string(bestActivity) + "), selecting Wait");
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced curiosity-based exploration with safety checks
    
    // Validate input parameters
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain: brain_ is null, cannot perform curiosity-based selection");
        return defaultCmd;
    }
    
    if (curiosityLevel_ < 0.0f || curiosityLevel_ > 1.0f) {
        NLM_LOG_WARNING("AgentBrain: Invalid curiosity level " + std::to_string(curiosityLevel_) + ", clamping to [0,1]");
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * AgentConstants::MAX_EXPLORATION_CHANCE;  // Up to 30% random
        
        // Validate exploration chance
        exploreChance = std::max(AgentConstants::MIN_EXPLORATION_CHANCE, std::min(exploreChance, AgentConstants::MAX_EXPLORATION_CHANCE));
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int maxChoice = 7;  // Cases 0-6 for motor commands
            int choice = brain_->getRandomGenerator()->uniformInt(0, maxChoice);
            
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
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    // Enhanced reward modulation with comprehensive safety checks and validation
    
    // Validate input parameters
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain: brain_ is null, cannot apply reward modulation");
        return;
    }
    
    if (!rewardModulationEnabled_) {
        NLM_LOG_DEBUG("AgentBrain: Reward modulation is disabled, skipping");
        return;
    }
    
    // Validate reward values
    if (std::isnan(reward) || std::isinf(reward)) {
        NLM_LOG_ERROR("AgentBrain: Invalid reward value " + std::to_string(reward) + ", skipping");
        return;
    }
    
    if (std::isnan(predictedReward) || std::isinf(predictedReward)) {
        NLM_LOG_ERROR("AgentBrain: Invalid predicted reward value " + std::to_string(predictedReward) + ", skipping");
        return;
    }
    
    // Clamp reward values to reasonable range
    reward = std::max(AgentConstants::MIN_PREDICTION_ERROR_RANGE, std::min(reward, AgentConstants::MAX_PREDICTION_ERROR_RANGE));
    predictedReward = std::max(AgentConstants::MIN_PREDICTION_ERROR_RANGE, std::min(predictedReward, AgentConstants::MAX_PREDICTION_ERROR_RANGE));
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Apply bounds checking
    dopamineLevel_ = std::max(AgentConstants::MIN_DOPAMINE_LEVEL, std::min(dopamineLevel_, AgentConstants::MAX_DOPAMINE_LEVEL));
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        if (!region) {
            NLM_LOG_WARNING("AgentBrain: Null region encountered, skipping");
            continue;
        }
        
        for (auto* syn : region->getSynapses()) {
            if (!syn) {
                NLM_LOG_WARNING("AgentBrain: Null synapse encountered, skipping");
                continue;
            }
            
            float eligibility = syn->getEligibilityTrace();
            
            // Validate eligibility trace
            if (std::isnan(eligibility) || std::isinf(eligibility)) {
                NLM_LOG_WARNING("AgentBrain: Invalid eligibility trace " + std::to_string(eligibility) + ", skipping");
                continue;
            }
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                
                // Validate delta
                if (std::isnan(delta) || std::isinf(delta)) {
                    NLM_LOG_WARNING("AgentBrain: Invalid weight delta " + std::to_string(delta) + ", skipping");
                    continue;
                }
                
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    
    // Apply bounds checking
    plasticityFactor = std::max(AgentConstants::MIN_PLASTICITY_FACTOR, std::min(plasticityFactor, AgentConstants::MAX_PLASTICITY_FACTOR));
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    // Enhanced development system update with comprehensive safety checks and validation
    
    // Validate input parameters
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain: brain_ is null, cannot update development");
        return;
    }
    
    if (!developmentEnabled_) {
        NLM_LOG_DEBUG("AgentBrain: Development system is disabled, skipping update");
        return;
    }
    
    // Validate timestep
    if (std::isnan(timestep) || std::isinf(timestep) || timestep < 0.0) {
        NLM_LOG_ERROR("AgentBrain: Invalid timestep " + std::to_string(timestep) + ", skipping development update");
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Enhanced developmental stages with validation
    if (developmentalAge_ < AgentConstants::MAX_DEV_STAGE_INIT) {
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < AgentConstants::MAX_DEV_STAGE_CRITICAL) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < AgentConstants::MAX_DEV_STAGE_MATURATION) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else if (developmentalAge_ < AgentConstants::MAX_DEV_STAGE_ADULT) {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    } else {
        // Aging phase
        plasticityModifier_ = 0.1f;  // Reduced plasticity in old age
        brain_->setDevelopmentalStage(DevelopmentalStage::Aging);
    }
    
    // Validate plasticity modifier
    plasticityModifier_ = std::max(0.0f, std::min(plasticityModifier_, 2.0f));
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            
            // Apply bounds checking
            synRate = std::max(0.0f, std::min(synRate, 0.001f));
            pruneRate = std::max(0.0f, std::min(pruneRate, 0.0001f));
            
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        } else {
            NLM_LOG_WARNING("AgentBrain: Structural plasticity system not available");
        }
    }
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
    // Enhanced reset function with comprehensive safety checks and validation
    
    // Reset all neuromodulation levels with validation
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Validate and reset previous vision data
    if (!previousVision_.empty()) {
        // Check for valid vision data before reset
        bool hasValidData = false;
        for (float value : previousVision_) {
            if (!std::isnan(value) && !std::isinf(value)) {
                hasValidData = true;
                break;
            }
        }
        
        if (!hasValidData) {
            // Clear previous vision safely
            std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
            NLM_LOG_DEBUG("AgentBrain: Previous vision data was invalid, reset to zeros");
        } else {
            // Clear previous vision with original data
            std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
        }
    }
    
    // Log reset completion
    NLM_LOG_INFO("AgentBrain reset completed - all neuromodulation levels and state cleared");
}

} // namespace nlm
