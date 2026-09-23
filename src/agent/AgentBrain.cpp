#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

const float VISION_SCALE_FACTOR = 5.0f;          // Scale factor for vision input
    const float TOUCH_SCALE_FACTOR = 8.0f;           // Scale factor for touch input
    const float INTERNAL_SCALE_FACTOR = 5.0f;       // Scale factor for internal signals
    const float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;  // Scale factor for proprioception
    const float NOVELTY_SENSITIVITY = 1.0f;          // Novelty sensitivity factor
    const float CURIOSITY_NOVELTY_FACTOR = 2.0f;     // Novelty contribution to curiosity
    const float CURIOSITY_ERROR_FACTOR = 0.5f;       // Prediction error contribution to curiosity
    const float CURIOSITY_THRESHOLD = 0.5f;          // Minimum curiosity for exploration
    const float EXPLORATION_PROBABILITY_FACTOR = 0.3f; // Max exploration probability
    const float MOTOR_ACTIVITY_THRESHOLD = 0.5f;     // Threshold for meaningful activity
    const float DOPAMINE_CLAMP_MIN = -1.0f;          // Minimum dopamine value
    const float DOPAMINE_CLAMP_MAX = 1.0f;          // Maximum dopamine value
    const float PLASTICITY_BASE = 0.5f;              // Base plasticity factor
    const float PLASTICITY_RANGE = 0.5f;             // Plasticity adjustment range
    const float REWARD_DECAY = 0.95f;               // Reward decay factor
    const float REWARD_LEARNING_RATE = 0.05f;       // Reward learning rate
    const float ELIGIBILITY_DECAY = 0.1f;           // Eligibility trace decay
    const float ELIGIBILITY_THRESHOLD = 0.001f;     // Minimum eligibility for plasticity
    const float DEVELOPMENT_PLASTICITY_MULTIPLIER = 1.0f; // Development stage 1
    const float DEVELOPMENT_CRITICAL_PLASTICITY = 0.8f;  // Development stage 2
    const float DEVELOPMENT_MATURATION_PLASTICITY = 0.5f;  // Development stage 3
    const float DEVELOPMENT_ADULT_PLASTICITY = 0.2f;      // Development stage 4
    const float SYNAPTONGENESIS_BASE_RATE = 0.0001f;      // Base synaptogenesis rate
    const float PRUNING_BASE_RATE = 0.00001f;            // Base pruning rate
    const float PRUNING_PLASTICITY_FACTOR = 2.0f;        // Plasticity factor for pruning
    const float NOVELTY_DECAY_FACTOR = 0.99f;           // Novelty decay per timestep

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain: no brain provided");
        return;
    }
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

bool AgentBrain::initializeSafely(const SimpleWorld& world) {
    try {
        if (!brain_) {
            NLM_LOG_ERROR("Cannot initialize AgentBrain: no brain provided");
            return false;
        }
        
        // Check if brain has required regions and populations
        if (brain_->getRegions().empty()) {
            NLM_LOG_ERROR("Brain has no regions initialized");
            return false;
        }
        
        // Check if required neuron types are available
        bool hasMotor = false;
        bool hasSensory = false;
        
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                if (type == NeuronType::Motor) hasMotor = true;
                if (type == NeuronType::Sensory) hasSensory = true;
            }
        }
        
        if (!hasMotor) {
            NLM_LOG_ERROR("No motor neurons found in brain");
            return false;
        }
        
        if (!hasSensory) {
            NLM_LOG_ERROR("No sensory neurons found in brain");
            return false;
        }
        
        // Perform safe initialization
        initialize(world);
        initialized_ = true;
        
        NLM_LOG_INFO("AgentBrain safely initialized with validation checks passed");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during AgentBrain initialization: ") + e.what());
        return false;
    }
}

bool AgentBrain::isFullyInitialized() const {
    return initialized_ && brain_ && 
           !sensoryVision_.empty() && !motorForward_.empty();
}

AgentBrain::AgentStatus AgentBrain::getStatus() const {
    AgentStatus status;
    status.developmentalAge = developmentalAge_;
    status.plasticityModifier = plasticityModifier_;
    status.curiosityLevel = curiosityLevel_;
    status.noveltyLevel = noveltyLevel_;
    status.predictionError = predictionError_;
    status.dopamineLevel = dopamineLevel_;
    status.developmentalStage = getDevelopmentalStage();
    
    // Count active neurons
    status.activeMotorNeurons = 0;
    for (const auto& neurons : {motorForward_, motorBackward_, motorTurnLeft_, 
                                motorTurnRight_, motorInteract_, motorWait_}) {
        for (const auto* n : neurons) {
            if (n && n->getState().membranePotential > -20.0f) { // Threshold for active
                status.activeMotorNeurons++;
            }
        }
    }
    
    status.activeSensoryNeurons = 0;
    for (const auto& neurons : {sensoryVision_, sensoryTouch_, sensoryInternal_, 
                                sensoryProprioception_}) {
        for (const auto* n : neurons) {
            if (n && n->getState().membranePotential > -20.0f) { // Threshold for active
                status.activeSensoryNeurons++;
            }
        }
    }
    
    // Determine if learning is occurring based on various factors
    status.isLearning = (curiosityLevel_ > 0.3f) || (std::abs(predictionError_) > 0.1f) ||
                       (plasticityModifier_ > 0.5f);
    
    return status;
}

std::vector<MotorCommand> AgentBrain::getAvailableActions() const {
    std::vector<MotorCommand> actions;
    
    // Determine which motor actions are available based on neuron groups
    if (!motorForward_.empty()) actions.push_back(MotorCommand::MoveForward);
    if (!motorBackward_.empty()) actions.push_back(MotorCommand::MoveBackward);
    if (!motorTurnLeft_.empty()) actions.push_back(MotorCommand::TurnLeft);
    if (!motorTurnRight_.empty()) actions.push_back(MotorCommand::TurnRight);
    if (!motorInteract_.empty()) actions.push_back(MotorCommand::Interact);
    if (!motorWait_.empty()) actions.push_back(MotorCommand::Wait);
    
    // LookLeft and LookRight might be available depending on implementation
    // For now, add them if there's any motor neurons available
    if (!motorTurnLeft_.empty() || !motorTurnRight_.empty()) {
        actions.push_back(MotorCommand::LookLeft);
        actions.push_back(MotorCommand::LookRight);
    }
    
    return actions;
}

void AgentBrain::processSensoryInputs(const std::vector<SensoryPercept>& percepts) {
    for (const auto& percept : percepts) {
        processSensoryInput(percept);
        stats_.incrementSensoryProcessing();
    }
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, DOPAMINE_CLAMP_MIN, DOPAMINE_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > ELIGIBILITY_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(ELIGIBILITY_DECAY);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = PLASTICITY_BASE + PLASTICITY_RANGE * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
    
    stats_.incrementRewardModulation();
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = DEVELOPMENT_PLASTICITY_MULTIPLIER;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = DEVELOPMENT_CRITICAL_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = DEVELOPMENT_MATURATION_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = DEVELOPMENT_ADULT_PLASTICITY;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = SYNAPTONGENESIS_BASE_RATE * plasticityModifier_;
            float pruneRate = PRUNING_BASE_RATE * (PRUNING_PLASTICITY_FACTOR - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
    
    stats_.incrementDevelopmentUpdate();
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
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
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
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
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
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r > exploreChance) {  // Changed from < to > for proper exploration
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;  // Added missing MotorCommand:: prefix
                case 5: return MotorCommand::LookRight;  // Added missing MotorCommand:: prefix
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
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
