#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , motorGroups_(MAX_MOTOR_GROUPS)
    , sensoryGroups_(MAX_SENSORY_GROUPS)
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
    // Validate input
    if (!brain_) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    // Initialize groups
    initializeMotorGroups();
    initializeSensoryGroups();
    
    // Distribute neurons to groups
    distributeNeuronsToGroups();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeMotorGroups() {
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    // Reserve space for performance
    size_t neuronsPerGroup = MOTOR_GROUP_SIZE;  // Approximate distribution
    motorForward_.reserve(neuronsPerGroup);
    motorBackward_.reserve(neuronsPerGroup);
    motorTurnLeft_.reserve(neuronsPerGroup);
    motorTurnRight_.reserve(neuronsPerGroup);
    motorInteract_.reserve(neuronsPerGroup);
    motorWait_.reserve(neuronsPerGroup);
}

void AgentBrain::initializeSensoryGroups() {
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    // Reserve space for performance
    size_t neuronsPerGroup = SENSORY_GROUP_SIZE;  // Approximate distribution
    sensoryVision_.reserve(neuronsPerGroup);
    sensoryTouch_.reserve(neuronsPerGroup);
    sensoryInternal_.reserve(neuronsPerGroup);
    sensoryProprioception_.reserve(neuronsPerGroup);
}

void AgentBrain::distributeNeuronsToGroups() {
    // Clear existing groups first
    initializeMotorGroups();
    initializeSensoryGroups();
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot distribute neurons: brain pointer is null");
        return;
    }
    
    NLM_LOG_INFO("Distributing neurons to motor and sensory groups...");
    
    // Track distribution statistics
    size_t totalNeurons = 0;
    size_t motorNeurons = 0;
    size_t sensoryNeurons = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            totalNeurons += pop->getNeurons().size();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    // Distribute motor neurons using round-robin for even distribution
                    size_t idx = motorForward_.size() + motorBackward_.size() + 
                                motorTurnLeft_.size() + motorTurnRight_.size() + 
                                motorInteract_.size() + motorWait_.size();
                    
                    // Use deterministic distribution based on neuron ID for consistency
                    if (n->getId() != -1) {
                        idx = (n->getId() % MOTOR_GROUP_SIZE);
                    } else {
                        // Fallback to order-based distribution
                        idx = motorForward_.size() % MOTOR_GROUP_SIZE;
                    }
                    
                    switch (idx) {
                        case 0: motorForward_.push_back(n); break;
                        case 1: motorBackward_.push_back(n); break;
                        case 2: motorTurnLeft_.push_back(n); break;
                        case 3: motorTurnRight_.push_back(n); break;
                        case 4: motorInteract_.push_back(n); break;
                        case 5: motorWait_.push_back(n); break;
                    }
                    motorNeurons++;
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    // Distribute sensory neurons using round-robin
                    size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                sensoryInternal_.size() + sensoryProprioception_.size();
                    
                    if (n->getId() != -1) {
                        idx = (n->getId() % SENSORY_GROUP_SIZE);
                    } else {
                        idx = sensoryVision_.size() % SENSORY_GROUP_SIZE;
                    }
                    
                    switch (idx) {
                        case 0: sensoryVision_.push_back(n); break;
                        case 1: sensoryTouch_.push_back(n); break;
                        case 2: sensoryInternal_.push_back(n); break;
                        case 3: sensoryProprioception_.push_back(n); break;
                    }
                    sensoryNeurons++;
                }
            }
        }
    }
    
    totalNeurons = motorNeurons + sensoryNeurons;
    
    NLM_LOG_INFO("Neuron distribution complete:");
    NLM_LOG_INFO("  Total neurons processed: " + std::to_string(totalNeurons));
    NLM_LOG_INFO("  Motor neurons: " + std::to_string(motorNeurons));
    NLM_LOG_INFO("  Sensory neurons: " + std::to_string(sensoryNeurons));
    NLM_LOG_INFO("  Motor groups: " + std::to_string(motorGroups_));
    NLM_LOG_INFO("  Sensory groups: " + std::to_string(sensoryGroups_));
}

bool AgentBrain::validateBrainStructure() const {
    if (!brain_) {
        NLM_LOG_ERROR("Brain validation failed: brain pointer is null");
        return false;
    }
    
    // Check if brain has regions and neurons
    if (brain_->getRegionCount() == 0) {
        NLM_LOG_WARNING("Brain validation: No regions found");
    }
    
    // Verify we have expected neuron types
    bool hasMotor = false;
    bool hasSensory = false;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Motor) hasMotor = true;
            else if (type == NeuronType::Sensory) hasSensory = true;
        }
    }
    
    if (!hasMotor) {
        NLM_LOG_WARNING("Brain validation: No motor neurons found");
    }
    if (!hasSensory) {
        NLM_LOG_WARNING("Brain validation: No sensory neurons found");
    }
    
    // Log group composition
    NLM_LOG_INFO("Brain structure validation:");
    NLM_LOG_INFO("  Motor groups: " + std::to_string(motorGroups_));
    NLM_LOG_INFO("  Sensory groups: " + std::to_string(sensoryGroups_));
    NLM_LOG_INFO("  Forward neurons: " + std::to_string(motorForward_.size()));
    NLM_LOG_INFO("  Backward neurons: " + std::to_string(motorBackward_.size()));
    NLM_LOG_INFO("  Left turn neurons: " + std::to_string(motorTurnLeft_.size()));
    NLM_LOG_INFO("  Right turn neurons: " + std::to_string(motorTurnRight_.size()));
    NLM_LOG_INFO("  Interact neurons: " + std::to_string(motorInteract_.size()));
    NLM_LOG_INFO("  Wait neurons: " + std::to_string(motorWait_.size()));
    NLM_LOG_INFO("  Vision neurons: " + std::to_string(sensoryVision_.size()));
    NLM_LOG_INFO("  Touch neurons: " + std::to_string(sensoryTouch_.size()));
    NLM_LOG_INFO("  Internal neurons: " + std::to_string(sensoryInternal_.size()));
    NLM_LOG_INFO("  Proprioception neurons: " + std::to_string(sensoryProprioception_.size()));
    
    return true;
}

void AgentBrain::configureNeuronGroups(size_t motorGroups, size_t sensoryGroups) {
    motorGroups_ = std::min(motorGroups, MAX_MOTOR_GROUPS);
    sensoryGroups_ = std::min(sensoryGroups, MAX_SENSORY_GROUPS);
    
    NLM_LOG_INFO("Neuron groups reconfigured:");
    NLM_LOG_INFO("  Motor groups: " + std::to_string(motorGroups_));
    NLM_LOG_INFO("  Sensory groups: " + std::to_string(sensoryGroups_));
    
    // Redistribute neurons with new configuration
    if (brain_) {
        distributeNeuronsToGroups();
    }
}

float AgentBrain::calculateNeuronActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    for (const Neuron* n : neurons) {
        if (n) {
            totalActivity += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
    }
    
    return totalActivity / neurons.size();
}

size_t AgentBrain::countActiveNeurons(const std::vector<Neuron*>& neurons) const {
    size_t activeCount = 0;
    for (const Neuron* n : neurons) {
        if (n && n->isFiring()) {
            activeCount++;
        }
    }
    return activeCount;
}

void AgentBrain::initialize(const SimpleWorld& world) {
    // Use validation to log brain structure
    validateBrainStructure();
    
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
    // Calculate average activity in each motor group using helper method
    float forwardAct = calculateNeuronActivity(motorForward_);
    float backwardAct = calculateNeuronActivity(motorBackward_);
    float leftAct = calculateNeuronActivity(motorTurnLeft_);
    float rightAct = calculateNeuronActivity(motorTurnRight_);
    float interactAct = calculateNeuronActivity(motorInteract_);
    float waitAct = calculateNeuronActivity(motorWait_);
    
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
