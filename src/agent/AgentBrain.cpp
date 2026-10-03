#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Constants for AgentBrain behavior
namespace {
    constexpr float DEFAULT_SENSOR_NOVELTY_DECAY = 0.99f;
    constexpr float DEFAULT_VISION_SCALE = 5.0f;
    constexpr float DEFAULT_TOUCH_SCALE = 8.0f;
    constexpr float DEFAULT_INTERNAL_SCALE = 5.0f;
    constexpr float DEFAULT_PROPRIOCEPTION_SCALE = 3.0f;
    constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.01f;
    constexpr float DEFAULT_CURIOUS_THRESHOLD = 0.3f;
    constexpr float DEFAULT_EXPLORATION_THRESHOLD = 0.5f;
    constexpr float DEFAULT_ACTIVITY_THRESHOLD = 0.5f;
    constexpr float DEFAULT_EXPLORATION_RATE = 0.3f;
    constexpr float DEFAULT_PREDICTION_ERROR_LERP = 0.05f;
    constexpr float DEFAULT_DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DEFAULT_DOPAMINE_CLAMP_MAX = 1.0f;
    constexpr float DEFAULT_PLASTICITY_FACTOR_BASE = 0.5f;
    constexpr float DEFAULT_PLASTICITY_FACTOR_SCALE = 0.5f;
    constexpr float DEFAULT_PLASTICITY_FACTOR_MIN = 0.1f;
    constexpr float DEFAULT_PLASTICITY_FACTOR_MAX = 2.0f;
    constexpr float DEFAULT_STDP_LTP_BASE = 0.01f;
    constexpr float DEFAULT_STDP_LTD_BASE = 0.012f;
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE_BASE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE_BASE = 0.00001f;
    constexpr float DEFAULT_ELIGIBILITY_DECAY = 0.1f;
    constexpr float DEFAULT_ELIGIBILITY_THRESHOLD = 0.001f;
}

// Helper functions for AgentBrain
namespace {
    template<typename T>
    std::vector<std::shared_ptr<T>> convertNeuronsToShared(const std::vector<T*>& neurons) {
        std::vector<std::shared_ptr<T>> result;
        result.reserve(neurons.size());
        for (T* n : neurons) {
            if (n) {
                result.push_back(std::shared_ptr<T>(n));
            }
        }
        return result;
    }
    
    float calculateNeuronActivity(const std::vector<std::shared_ptr<Neuron>>& neurons) {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (const auto& neuron : neurons) {
            if (neuron) {
                // Use membrane potential deviation from rest as activity measure
                sum += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            }
        }
        return sum / neurons.size();
    }
}

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
    , sensoryNoveltyDecay_(DEFAULT_SENSOR_NOVELTY_DECAY)
{
    // Validate input
    if (!brain_) {
        NLM_LOG_WARN("AgentBrain created with null brain pointer");
        return;
    }
    
    // Initialize motor and sensory neuron groups
    initializeNeuronGroups();
}

void AgentBrain::initializeNeuronGroups() {
    if (!brain_) return;
    
    // Clear existing groups
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
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    // Distribute motor neurons to different action groups
                    distributeMotorNeuron(n);
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    // Distribute sensory neurons
                    distributeSensoryNeuron(n);
                }
            }
        }
    }
}

void AgentBrain::distributeMotorNeuron(Neuron* neuron) {
    if (!neuron) return;
    
    size_t idx = motorForward_.size() + motorBackward_.size() + 
                 motorTurnLeft_.size() + motorTurnRight_.size() +
                 motorInteract_.size() + motorWait_.size();
    
    switch (idx % 6) {
        case 0: motorForward_.push_back(std::shared_ptr<Neuron>(neuron)); break;
        case 1: motorBackward_.push_back(std::shared_ptr<Neuron>(neuron)); break;
        case 2: motorTurnLeft_.push_back(std::shared_ptr<Neuron>(neuron)); break;
        case 3: motorTurnRight_.push_back(std::shared_ptr<Neuron>(neuron)); break;
        case 4: motorInteract_.push_back(std::shared_ptr<Neuron>(neuron)); break;
        case 5: motorWait_.push_back(std::shared_ptr<Neuron>(neuron)); break;
    }
}

void AgentBrain::distributeSensoryNeuron(Neuron* neuron) {
    if (!neuron) return;
    
    size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                 sensoryInternal_.size() + sensoryProprioception_.size();
    
    switch (idx % 4) {
        case 0: sensoryVision_.push_back(std::shared_ptr<Neuron>(neuron)); break;
        case 1: sensoryTouch_.push_back(std::shared_ptr<Neuron>(neuron)); break;
        case 2: sensoryInternal_.push_back(std::shared_ptr<Neuron>(neuron)); break;
        case 3: sensoryProprioception_.push_back(std::shared_ptr<Neuron>(neuron)); break;
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!world) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain with null world");
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

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return VISION_PIXELS + TOUCH_SENSORS + INTERNAL_SENSORS + PROPRIOCEPTION_SENSORS;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return MOTOR_GROUP_COUNT;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Validate percept
    if (!percept.getVision().empty() && percept.getVision().size() != getVisionDimension()) {
        NLM_LOG_WARN("Vision size mismatch in processSensoryInput");
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * DEFAULT_VISION_SCALE;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * DEFAULT_TOUCH_SCALE;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * DEFAULT_INTERNAL_SCALE;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * DEFAULT_PROPRIOCEPTION_SCALE;  // Center and scale
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
    if (curiosityEnabled_ && curiosityLevel_ > DEFAULT_CURIOUS_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
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
    if (bestActivity < DEFAULT_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > DEFAULT_EXPLORATION_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * DEFAULT_EXPLORATION_RATE;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, MOTOR_COMMAND_OPTIONS);
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
    
    // Validate inputs
    if (std::isnan(reward) || std::isnan(predictedReward)) {
        NLM_LOG_WARN("Invalid reward values in applyRewardModulation");
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = (1.0f - DEFAULT_PREDICTION_ERROR_LERP) * expectedReward_ + 
                      DEFAULT_PREDICTION_ERROR_LERP * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, DEFAULT_DOPAMINE_CLAMP_MIN, DEFAULT_DOPAMINE_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > DEFAULT_ELIGIBILITY_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(DEFAULT_ELIGIBILITY_DECAY);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = DEFAULT_PLASTICITY_FACTOR_BASE + 
                             DEFAULT_PLASTICITY_FACTOR_SCALE * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, DEFAULT_PLASTICITY_FACTOR_MIN, DEFAULT_PLASTICITY_FACTOR_MAX);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(DEFAULT_STDP_LTP_BASE * plasticityFactor);
        stdp->setLTDWeight(DEFAULT_STDP_LTD_BASE * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    if (std::isnan(timestep) || timestep < 0.0) {
        NLM_LOG_WARN("Invalid timestep in updateDevelopment");
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < CRITICAL_STAGE_AGE) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < CRITICAL_PERIOD_AGE) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < MATURATION_AGE) {  // ~15 minutes
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
            float synRate = DEFAULT_SYNAPTOGENESIS_RATE_BASE * plasticityModifier_;
            float pruneRate = DEFAULT_PRUNING_RATE_BASE * (2.0f - plasticityModifier_);
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