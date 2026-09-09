#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cassert>

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
    , sensoryNoveltyDecay_(SENSORY_NOVELTY_DECAY)
{
    assert(brain_ && "AgentBrain requires a valid brain pointer");
    
    // Initialize components using delegation
    initializeMotorGroups();
    initializeSensoryGroups();
    initializeDevelopmentStage();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!validateInputRanges()) {
        safeLogInitialization();
        return;
    }
    
    // Initialize state
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Log initialization details
    safeLogInitialization();
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return MOTOR_GROUP_COUNT;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!hasBrain()) return;
    
    // Process each sensory modality using helper methods
    const auto& vision = percept.getVision();
    processVisionInput(vision);
    
    const auto& touch = percept.getTouch();
    processTouchInput(touch);
    
    const auto& intern = percept.getInternal();
    processInternalInput(intern);
    
    const auto& proprio = percept.getProprioception();
    processProprioceptionInput(proprio);
    
    // Compute novelty
    if (!vision.empty()) {
        computeSensoryNovelty(vision);
    }
    
    // Update curiosity based on novelty
    updateCuriosityLevel();
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!hasBrain()) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > CURIOSITY_LEVEL_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!hasBrain() || !rewardModulationEnabled_) return;
    
    applyPredictionError(reward, predictedReward);
    updateDopamineSignal();
    
    // Apply plasticity effects
    applyBrainPlasticityEffects();
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!hasBrain() || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    updateDevelopmentStages();
    updateStructuralPlasticity();
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!hasBrain()) return DevelopmentalStage::Initial;
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
    resetState();
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

// ==========================================
// Neuron Distribution Methods
// ==========================================

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    for (size_t i = 0; i < neurons.size(); ++i) {
        Neuron* n = neurons[i];
        if (!n) continue;
        
        // Distribute to motor groups based on index modulo MOTOR_GROUP_COUNT
        size_t groupIndex = i % MOTOR_GROUP_COUNT;
        switch (groupIndex) {
            case 0: motorForward_.push_back(n); break;
            case 1: motorBackward_.push_back(n); break;
            case 2: motorTurnLeft_.push_back(n); break;
            case 3: motorTurnRight_.push_back(n); break;
            case 4: motorInteract_.push_back(n); break;
            case 5: motorWait_.push_back(n); break;
        }
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    for (size_t i = 0; i < neurons.size(); ++i) {
        Neuron* n = neurons[i];
        if (!n) continue;
        
        // Distribute to sensory groups based on index modulo SENSORY_GROUP_COUNT
        size_t groupIndex = i % SENSORY_GROUP_COUNT;
        switch (groupIndex) {
            case 0: sensoryVision_.push_back(n); break;
            case 1: sensoryTouch_.push_back(n); break;
            case 2: sensoryInternal_.push_back(n); break;
            case 3: sensoryProprioception_.push_back(n); break;
        }
    }
}

MotorCommand AgentBrain::getMotorCommandFromIndex(size_t index) const {
    switch (index) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::Interact;
        case 5: return MotorCommand::Wait;
        default: return MotorCommand::Wait;
    }
}

// ==========================================
// Initialization Methods
// ==========================================

void AgentBrain::initializeMotorGroups() {
    if (!hasBrain()) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Motor) {
                std::vector<Neuron*> neurons = pop->getNeurons();
                distributeMotorNeurons(neurons);
            }
        }
    }
}

void AgentBrain::initializeSensoryGroups() {
    if (!hasBrain()) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Sensory) {
                std::vector<Neuron*> neurons = pop->getNeurons();
                distributeSensoryNeurons(neurons);
            }
        }
    }
}

void AgentBrain::initializeDevelopmentStage() {
    developmentalAge_ = 0.0;
    plasticityModifier_ = ACTIVITY_THRESHOLD_DEVELOPMENT_INITIAL;
}

void AgentBrain::resetState() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = ACTIVITY_THRESHOLD_DEVELOPMENT_INITIAL;
}

// ==========================================
// Validation Methods
// ==========================================

bool AgentBrain::validateInputRanges() const {
    if (!hasBrain()) return false;
    
    const size_t visionNeurons = sensoryVision_.size();
    if (visionNeurons > 256) {
        NLM_LOG_WARNING("Vision neuron count exceeds expected range: " + 
                       std::to_string(visionNeurons));
        return false;
    }
    
    return true;
}

void AgentBrain::safeLogInitialization() const {
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

// ==========================================
// Motor Decoding
// ==========================================

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (!n) continue;
            
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
    if (bestActivity < ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * EXPLORATION_CHANCE_FACTOR;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
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

// ==========================================
// Neuromodulation and State Management
// ==========================================

void AgentBrain::updateDopamineSignal() {
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
}

void AgentBrain::updateCuriosityLevel() {
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * NOVELTY_WEIGHT_FACTOR + 
                         std::abs(predictionError_) * PREDICTION_ERROR_WEIGHT;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

void AgentBrain::applyPredictionError(float reward, float predictedReward) {
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = EXPECTED_REWARD_ALPHA * expectedReward_ + 
                     EXPECTED_REWARD_BETA * reward;
}

void AgentBrain::applyBrainPlasticityEffects() {
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > ELIGIBILITY_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(ELIGIBILITY_DECAY_RATE);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = PLASTICITY_BASE + PLASMODICITY_ENHANCEMENT * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(STDP_LTP_BASE * plasticityFactor);
        stdp->setLTDWeight(STDP_LTD_BASE * plasticityFactor);
    }
}

// ==========================================
// Development and Plasticity Management
// ==========================================

void AgentBrain::updateDevelopmentStages() {
    if (developmentalAge_ < DEVELOPMENT_STAGE_1) {
        plasticityModifier_ = ACTIVITY_THRESHOLD_DEVELOPMENT_INITIAL;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < DEVELOPMENT_STAGE_2) {
        plasticityModifier_ = ACTIVITY_THRESHOLD_DEVELOPMENT_CRITICAL;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < DEVELOPMENT_STAGE_3) {
        plasticityModifier_ = ACTIVITY_THRESHOLD_DEVELOPMENT_MATURATION;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = ACTIVITY_THRESHOLD_DEVELOPMENT_ADULT;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

void AgentBrain::updateStructuralPlasticity() {
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = SYNAPTOGENESIS_BASE_RATE * plasticityModifier_;
            float pruneRate = PRUNING_BASE_RATE * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

// ==========================================
// Sensory Processing Helpers
// ==========================================

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * ACTIVITY_SCALE_FACTOR_VISION;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * ACTIVITY_SCALE_FACTOR_TOUCH;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& intern) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - INTERNAL_CENTER_OFFSET) * ACTIVITY_SCALE_FACTOR_INTERNAL;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - PROPRIO_CENTER_OFFSET) * ACTIVITY_SCALE_FACTOR_PROPRIO;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::computeSensoryNovelty(const std::vector<float>& vision) {
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    
    // Decay and update
    noveltyLevel_ *= SENSORY_NOVELTY_DECAY;
    
    // Store for next time
    previousVision_ = vision;
}

} // namespace nlm