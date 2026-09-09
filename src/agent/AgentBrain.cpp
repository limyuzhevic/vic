#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cassert>
#include <limits>
#include <random>

namespace nlm {

// Constants for better code maintainability
namespace AgentBrainConstants {
    const size_t DEFAULT_MOTOR_GROUPS = 6;  // MoveForward, MoveBackward, TurnLeft, TurnRight, Interact, Wait
    const size_t DEFAULT_SENSORY_GROUPS = 4;  // Vision, Touch, Internal, Proprioception
    const float DEFAULT_NOVELTY_DECAY = 0.99f;
    const float DEFAULT_PLATSCITY_MODIFIER = 1.0f;
    const float DEFAULT_DEVELOPMENTAL_AGE_FACTOR = 0.5f;
    const float MAX_DOPAMINE_LEVEL = 1.0f;
    const float MIN_DOPAMINE_LEVEL = -1.0f;
    const float DEFAULT_CURIOUSITY_THRESHOLD = 0.3f;
    const float DEFAULT_EXPLORATION_CHANCE = 0.3f;
    const size_t MAX_RANDOM_CHOICE = 7;
    const float DEFAULT_REWARD_EXPONENT = 0.95f;
    const float DEFAULT_NEW_REWARD_WEIGHT = 0.05f;
    const size_t SENSORY_GROUP_MULTIPLIER_VISION = 256;
    const size_t SENSORY_GROUP_MULTIPLIER_TOUCH = 8;
    const size_t SENSORY_GROUP_MULTIPLIER_INTERNAL = 4;
    const size_t SENSORY_GROUP_MULTIPLIER_PROPRIOCEPTION = 6;
    const float VISION_SCALE_FACTOR = 5.0f;
    const float TOUCH_SCALE_FACTOR = 8.0f;
    const float INTERNAL_SCALE_FACTOR = 2.0f;
    const float INTERNAL_AMPLITUDE_FACTOR = 5.0f;
    const float PROPRIOCEPTION_SCALE_FACTOR = 2.0f;
    const float PROPRIOCEPTION_AMPLITUDE_FACTOR = 3.0f;
    const float MINIMUM_ACTIVITY_THRESHOLD = 0.5f;
    const float HIGH_CURIOSITY_THRESHOLD = 0.5f;
    const float ELIGIBILITY_THRESHOLD = 0.001f;
    const float ELIGIBILITY_DECAY_RATE = 0.1f;
    const float BASE_PLASTICITY_FACTOR = 0.5f;
    const float DOPAMINE_PLASTICITY_SENSITIVITY = 0.5f;
    const float MIN_PLASTICITY_FACTOR = 0.1f;
    const float MAX_PLASTICITY_FACTOR = 2.0f;
    const float STDP_LTP_BASE_WEIGHT = 0.01f;
    const float STDP_LTD_BASE_WEIGHT = 0.012f;
    const float SYNAPTOGENESIS_BASE_RATE = 0.0001f;
    const float PRUNING_BASE_RATE = 0.00001f;
    const float PRUNING_SENSITIVITY = 2.0f;
    const double INITIAL_DEVELOPMENT_END = 60.0;
    const double CRITICAL_PERIOD_END = 300.0;
    const double MATURATION_END = 900.0;
    const float HIGH_PLASTICITY_FACTOR = 1.0f;
    const float CRITICAL_PERIOD_PLASTICITY_FACTOR = 0.8f;
    const float MATURATION_PLASTICITY_FACTOR = 0.5f;
    const float ADULT_PLASTICITY_FACTOR = 0.2f;
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
    , sensoryNoveltyDecay_(AgentBrainConstants::DEFAULT_NOVELTY_DECAY)
    , currentWorld_(nullptr)
{
    assert(brain_ != nullptr && "Brain pointer cannot be null");
    
    if (!isValid()) {
        throw std::invalid_argument("Invalid brain pointer provided to AgentBrain constructor");
    }
    
    // Initialize neural groups by distributing neurons from brain
    initializeNeuralGroups();
}

AgentBrain::~AgentBrain() = default;

AgentBrain& AgentBrain::operator=(AgentBrain&& other) noexcept {
    if (this != &other) {
        // Clear current state
        clearNeuralGroups();
        resetNeuromodulationState();
        resetDevelopmentState();
        clearPreviousVision();
        
        // Move resources
        brain_ = std::move(other.brain_);
        currentWorld_ = other.currentWorld_;
        
        // Move neural groups
        motorForward_ = std::move(other.motorForward_);
        motorBackward_ = std::move(other.motorBackward_);
        motorTurnLeft_ = std::move(other.motorTurnLeft_);
        motorTurnRight_ = std::move(other.motorTurnRight_);
        motorInteract_ = std::move(other.motorInteract_);
        motorWait_ = std::move(other.motorWait_);
        
        sensoryVision_ = std::move(other.sensoryVision_);
        sensoryTouch_ = std::move(other.sensoryTouch_);
        sensoryInternal_ = std::move(other.sensoryInternal_);
        sensoryProprioception_ = std::move(other.sensoryProprioception_);
        
        // Move state variables
        dopamineLevel_ = other.dopamineLevel_;
        noveltyLevel_ = other.noveltyLevel_;
        curiosityLevel_ = other.curiosityLevel_;
        predictionError_ = other.predictionError_;
        expectedReward_ = other.expectedReward_;
        developmentalAge_ = other.developmentalAge_;
        plasticityModifier_ = other.plasticityModifier_;
        
        rewardModulationEnabled_ = other.rewardModulationEnabled_;
        structuralPlasticityEnabled_ = other.structuralPlasticityEnabled_;
        developmentEnabled_ = other.developmentEnabled_;
        curiosityEnabled_ = other.curiosityEnabled_;
        
        previousVision_ = std::move(other.previousVision_);
        sensoryNoveltyDecay_ = other.sensoryNoveltyDecay_;
        
        // Clear source state
        other.currentWorld_ = nullptr;
        other.resetNeuromodulationState();
        other.resetDevelopmentState();
    }
    return *this;
}

void AgentBrain::initialize(const SimpleWorld& world) {
    assert(isValid() && "Brain must be valid for initialization");
    
    if (!isValid()) {
        handleInvalidState("initialize");
        return; // Silently handle invalid state to prevent crashes
    }
    
    // Store reference to world for future use
    currentWorld_ = &world;
    
    // Resize previous vision vector with bounds checking
    size_t visionSize = world.getVisionWidth() * world.getVisionHeight();
    previousVision_.resize(visionSize, 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = AgentBrainConstants::DEFAULT_PLATSCITY_MODIFIER;
    
    // Re-initialize neural groups with current world context
    initializeNeuralGroups();
    
    logInitializationStatus();
}

void AgentBrain::initializeNeuralGroups() {
    assert(isValid() && "Brain must be valid for neural group initialization");
    
    // Clear existing groups
    clearNeuralGroups();
    
    if (!brain_) {
        handleInvalidState("initializeNeuralGroups");
        return;
    }
    
    // Initialize neural groups by distributing neurons from brain
    distributeBrainNeurons(*this, *currentWorld_);
}

void AgentBrain::clearNeuralGroups() {
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    // Optimize memory usage
    optimizeMemoryUsage();
}

void AgentBrain::distributeBrainNeurons(AgentBrain& agent, const SimpleWorld& world) {
    if (!agent.isValid()) {
        return;
    }
    
    std::shared_ptr<Brain> brain = agent.brain_;
    if (!brain) {
        return;
    }
    
    for (const auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                agent.distributeMotorNeurons(agent.motorForward_, pop->getNeurons());
                agent.distributeMotorNeurons(agent.motorBackward_, pop->getNeurons());
                agent.distributeMotorNeurons(agent.motorTurnLeft_, pop->getNeurons());
                agent.distributeMotorNeurons(agent.motorTurnRight_, pop->getNeurons());
                agent.distributeMotorNeurons(agent.motorInteract_, pop->getNeurons());
                agent.distributeMotorNeurons(agent.motorWait_, pop->getNeurons());
            } else if (type == NeuronType::Sensory) {
                agent.distributeSensoryNeurons(agent.sensoryVision_, pop->getNeurons());
                agent.distributeSensoryNeurons(agent.sensoryTouch_, pop->getNeurons());
                agent.distributeSensoryNeurons(agent.sensoryInternal_, pop->getNeurons());
                agent.distributeSensoryNeurons(agent.sensoryProprioception_, pop->getNeurons());
            }
        }
    }
}

void AgentBrain::distributeMotorNeurons(std::vector<Neuron*>& target,
                                       const std::vector<Neuron*>& source) {
    if (source.empty()) {
        return;
    }
    
    // Reserve space to avoid frequent reallocations
    target.reserve(target.size() + source.size());
    
    for (Neuron* neuron : source) {
        if (neuron) {
            target.push_back(neuron);
        }
    }
}

void AgentBrain::distributeSensoryNeurons(std::vector<Neuron*>& target,
                                         const std::vector<Neuron*>& source) {
    if (source.empty()) {
        return;
    }
    
    // Reserve space to avoid frequent reallocations
    target.reserve(target.size() + source.size());
    
    for (Neuron* neuron : source) {
        if (neuron) {
            target.push_back(neuron);
        }
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    // Return computed size based on configured sensory groups
    return sensoryVision_.size() * AgentBrainConstants::SENSORY_GROUP_MULTIPLIER_VISION +
           sensoryTouch_.size() * AgentBrainConstants::SENSORY_GROUP_MULTIPLIER_TOUCH +
           sensoryInternal_.size() * AgentBrainConstants::SENSORY_GROUP_MULTIPLIER_INTERNAL +
           sensoryProprioception_.size() * AgentBrainConstants::SENSORY_GROUP_MULTIPLIER_PROPRIOCEPTION;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Return computed size based on configured motor groups
    return AgentBrainConstants::DEFAULT_MOTOR_GROUPS;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!isValid()) {
        handleInvalidState("processSensoryInput");
        return;
    }
    
    // Vision input processing
    processVisionInput(percept.getVision());
    
    // Touch input processing
    processTouchInput(percept.getTouch());
    
    // Internal signals processing
    processInternalInput(percept.getInternal());
    
    // Proprioception processing
    processProprioceptionInput(percept.getProprioception());
    
    // Compute novelty (difference from previous vision)
    computeNovelty(percept.getVision());
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    // Vision input (256 values -> sensoryVision_ neurons)
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        Neuron* neuron = sensoryVision_[i];
        if (neuron && safeVectorAccess(sensoryVision_, i)) {
            // Inject current proportional to vision intensity
            float current = vision[i] * AgentBrainConstants::VISION_SCALE_FACTOR;
            neuron->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    // Touch input (8 values -> sensoryTouch_ neurons)
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        Neuron* neuron = sensoryTouch_[i];
        if (neuron && safeVectorAccess(sensoryTouch_, i)) {
            float current = touch[i] * AgentBrainConstants::TOUCH_SCALE_FACTOR;
            neuron->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    // Internal signals (4 values -> sensoryInternal_ neurons)
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        Neuron* neuron = sensoryInternal_[i];
        if (neuron && safeVectorAccess(sensoryInternal_, i)) {
            // Center and scale the input
            float normalized = (internal[i] * AgentBrainConstants::INTERNAL_SCALE_FACTOR - 1.0f) * AgentBrainConstants::INTERNAL_AMPLITUDE_FACTOR;
            neuron->injectCurrent(normalized);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        Neuron* neuron = sensoryProprioception_[i];
        if (neuron && safeVectorAccess(sensoryProprioception_, i)) {
            // Center and scale the input
            float normalized = (proprio[i] * AgentBrainConstants::PROPRIOCEPTION_SCALE_FACTOR - 1.0f) * AgentBrainConstants::PROPRIOCEPTION_AMPLITUDE_FACTOR;
            neuron->injectCurrent(normalized);
        }
    }
}

void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    // Compute novelty (difference from previous vision)
    if (vision.empty()) {
        return;
    }
    
    float totalDiff = 0.0f;
    
    // Use efficient loop with bounds checking
    size_t limit = std::min(vision.size(), previousVision_.size());
    for (size_t i = 0; i < limit; ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize
    noveltyLevel_ = totalDiff / static_cast<float>(std::max<size_t>(vision.size(), 1));
    
    // Apply decay and update
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Store for next time
    previousVision_ = vision;
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!isValid()) {
        handleInvalidState("decodeMotorCommand");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > AgentBrainConstants::DEFAULT_CURIOUSITY_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() const {
    // Calculate average activity in each motor group using optimized approach
    float forwardAct = calculateNeuronActivity(motorForward_);
    float backwardAct = calculateNeuronActivity(motorBackward_);
    float leftAct = calculateNeuronActivity(motorTurnLeft_);
    float rightAct = calculateNeuronActivity(motorTurnRight_);
    float interactAct = calculateNeuronActivity(motorInteract_);
    float waitAct = calculateNeuronActivity(motorWait_);
    
    // Create command array for efficient comparison
    struct MotorCommandInfo {
        MotorCommand cmd;
        float activity;
    };
    
    MotorCommandInfo commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    // Find maximum activity using linear search
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < AgentBrainConstants::MINIMUM_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) const {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > AgentBrainConstants::HIGH_CURIOSITY_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * AgentBrainConstants::DEFAULT_EXPLORATION_CHANCE;
        
        // Generate random number safely
        if (brain_ && brain_->getRandomGenerator()) {
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command from valid range
                int choice = brain_->getRandomGenerator()->uniformInt(0, AgentBrainConstants::MAX_RANDOM_CHOICE - 1);
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
    if (!isValid() || !rewardModulationEnabled_) {
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = AgentBrainConstants::DEFAULT_REWARD_EXPONENT * expectedReward_ + 
                     AgentBrainConstants::DEFAULT_NEW_REWARD_WEIGHT * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, AgentBrainConstants::MIN_DOPAMINE_LEVEL, 
                               AgentBrainConstants::MAX_DOPAMINE_LEVEL);
    
    // Apply to all synapses with eligibility traces
    applyPlasticityModifications();
    
    // Modulate plasticity based on dopamine
    modulatePlasticityBasedOnDopamine();
}

void AgentBrain::applyPlasticityModifications() {
    if (!brain_) {
        return;
    }
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) {
                continue;
            }
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > AgentBrainConstants::ELIGIBILITY_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(AgentBrainConstants::ELIGIBILITY_DECAY_RATE);
            }
        }
    }
}

void AgentBrain::modulatePlasticityBasedOnDopamine() {
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = AgentBrainConstants::BASE_PLASTICITY_FACTOR + 
                             AgentBrainConstants::DOPAMINE_PLASTICITY_SENSITIVITY * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, AgentBrainConstants::MIN_PLASTICITY_FACTOR, 
                                 AgentBrainConstants::MAX_PLASTICITY_FACTOR);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(AgentBrainConstants::STDP_LTP_BASE_WEIGHT * plasticityFactor);
        stdp->setLTDWeight(AgentBrainConstants::STDP_LTD_BASE_WEIGHT * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!isValid() || !developmentEnabled_) {
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < AgentBrainConstants::INITIAL_DEVELOPMENT_END) {  // ~1 minute
        plasticityModifier_ = AgentBrainConstants::HIGH_PLASTICITY_FACTOR;
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        }
    } else if (developmentalAge_ < AgentBrainConstants::CRITICAL_PERIOD_END) {  // ~5 minutes
        plasticityModifier_ = AgentBrainConstants::CRITICAL_PERIOD_PLASTICITY_FACTOR;
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        }
    } else if (developmentalAge_ < AgentBrainConstants::MATURATION_END) {  // ~15 minutes
        plasticityModifier_ = AgentBrainConstants::MATURATION_PLASTICITY_FACTOR;
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        }
    } else {
        plasticityModifier_ = AgentBrainConstants::ADULT_PLASTICITY_FACTOR;  // Adult - more stable
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = AgentBrainConstants::SYNAPTOGENESIS_BASE_RATE * plasticityModifier_;
            float pruneRate = AgentBrainConstants::PRUNING_BASE_RATE * (AgentBrainConstants::PRUNING_SENSITIVITY - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!isValid()) {
        return DevelopmentalStage::Initial;
    }
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
    resetNeuromodulationState();
    resetDevelopmentState();
    clearPreviousVision();
}

void AgentBrain::resetNeuromodulationState() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
}

void AgentBrain::resetDevelopmentState() {
    developmentalAge_ = 0.0;
    plasticityModifier_ = AgentBrainConstants::DEFAULT_PLATSCITY_MODIFIER;
}

void AgentBrain::clearPreviousVision() {
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

bool AgentBrain::hasNeuralGroups() const {
    return !motorForward_.empty() || !motorBackward_.empty() || 
           !motorTurnLeft_.empty() || !motorTurnRight_.empty() ||
           !motorInteract_.empty() || !motorWait_.empty() ||
           !sensoryVision_.empty() || !sensoryTouch_.empty() ||
           !sensoryInternal_.empty() || !sensoryProprioception_.empty();
}

bool AgentBrain::safeVectorAccess(const std::vector<Neuron*>& vec, size_t index) const {
    return index < vec.size() && !vec.empty();
}

float AgentBrain::calculateNeuronActivity(const std::vector<Neuron*>& neurons) {
    return NeuronActivityCalculator::calculateAverageActivity(neurons);
}

bool AgentBrain::isValid() const {
    return brain_ != nullptr;
}

// Additional method implementations
void AgentBrain::setDevelopmentEnabled(bool enabled, double duration) {
    developmentEnabled_ = enabled;
    if (enabled) {
        developmentalAge_ = 0.0;
        // Update developmental stage boundaries
        AgentBrainConstants::INITIAL_DEVELOPMENT_END = duration;
        AgentBrainConstants::CRITICAL_PERIOD_END = duration * 5.0;
        AgentBrainConstants::MATURATION_END = duration * 15.0;
    }
}

void AgentBrain::logNeuralGroupStats() const {
    NLM_LOG_INFO("Neural group statistics:");
    NLM_LOG_INFO("  Motor groups: Forward=" + std::to_string(motorForward_.size()) +
                 " Backward=" + std::to_string(motorBackward_.size()) +
                 " Left=" + std::to_string(motorTurnLeft_.size()) +
                 " Right=" + std::to_string(motorTurnRight_.size()) +
                 " Interact=" + std::to_string(motorInteract_.size()) +
                 " Wait=" + std::to_string(motorWait_.size()));
    NLM_LOG_INFO("  Sensory groups: Vision=" + std::to_string(sensoryVision_.size()) +
                 " Touch=" + std::to_string(sensoryTouch_.size()) +
                 " Internal=" + std::to_string(sensoryInternal_.size()) +
                 " Proprioception=" + std::to_string(sensoryProprioception_.size()));
}

std::string AgentBrain::getNeuralGroupSummary() const {
    std::string summary = "Neural Groups: Motor(" + std::to_string(motorForward_.size() + motorBackward_.size() +
                                         motorTurnLeft_.size() + motorTurnRight_.size() +
                                         motorInteract_.size() + motorWait_.size()) +
                         ") Sensory(" + std::to_string(sensoryVision_.size() + sensoryTouch_.size() +
                                                sensoryInternal_.size() + sensoryProprioception_.size()) +
                         ") Age(" + std::to_string(static_cast<size_t>(developmentalAge_)) + ")";
    return summary;
}

void AgentBrain::optimizeMemoryUsage() {
    // Shrink vectors if they contain excessive unused capacity
    size_t shrinkThreshold = 16;  // Minimum size before considering shrink
    
    auto shrinkVector = [shrinkThreshold](std::vector<Neuron*>& vec) {
        if (vec.size() < shrinkThreshold && vec.capacity() > vec.size() * 2) {
            std::vector<Neuron*> temp;
            temp.swap(vec);
            vec = std::move(temp);
        }
    };
    
    shrinkVector(motorForward_);
    shrinkVector(motorBackward_);
    shrinkVector(motorTurnLeft_);
    shrinkVector(motorTurnRight_);
    shrinkVector(motorInteract_);
    shrinkVector(motorWait_);
    shrinkVector(sensoryVision_);
    shrinkVector(sensoryTouch_);
    shrinkVector(sensoryInternal_);
    shrinkVector(sensoryProprioception_);
}

void AgentBrain::reserveMemoryForGrowth() {
    // Pre-allocate memory for expected growth
    size_t estimatedGrowthFactor = 2;
    
    motorForward_.reserve(motorForward_.size() * estimatedGrowthFactor);
    motorBackward_.reserve(motorBackward_.size() * estimatedGrowthFactor);
    motorTurnLeft_.reserve(motorTurnLeft_.size() * estimatedGrowthFactor);
    motorTurnRight_.reserve(motorTurnRight_.size() * estimatedGrowthFactor);
    motorInteract_.reserve(motorInteract_.size() * estimatedGrowthFactor);
    motorWait_.reserve(motorWait_.size() * estimatedGrowthFactor);
    
    sensoryVision_.reserve(sensoryVision_.size() * estimatedGrowthFactor);
    sensoryTouch_.reserve(sensoryTouch_.size() * estimatedGrowthFactor);
    sensoryInternal_.reserve(sensoryInternal_.size() * estimatedGrowthFactor);
    sensoryProprioception_.reserve(sensoryProprioception_.size() * estimatedGrowthFactor);
}

void AgentBrain::handleInvalidState(const std::string& operation) const {
    // Log warning for invalid state operations
    NLM_LOG_WARNING("AgentBrain::" + operation + " called with invalid state (brain_ is null)");
}

void AgentBrain::logInitializationStatus() const {
    if (isValid()) {
        NLM_LOG_INFO("AgentBrain initialization completed successfully");
        logNeuralGroupStats();
    } else {
        NLM_LOG_ERROR("AgentBrain initialization failed - invalid brain state");
    }
}

// NeuronActivityCalculator static helper class
namespace {
class NeuronActivityCalculator {
public:
    static float calculateAverageActivity(const std::vector<Neuron*>& neurons) {
        if (neurons.empty()) {
            return 0.0f;
        }
        
        float sum = 0.0f;
        size_t validNeurons = 0;
        
        for (const Neuron* neuron : neurons) {
            if (neuron) {
                // Use membrane potential deviation from rest as activity measure
                sum += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                validNeurons++;
            }
        }
        
        return validNeurons > 0 ? sum / static_cast<float>(validNeurons) : 0.0f;
    }
};
} // namespace

} // namespace nlm