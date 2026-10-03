#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

// Configuration constants for better maintainability
namespace AgentConfig {
    constexpr float DEFAULT_ACTIVITY_THRESHOLD = 0.5f;
    constexpr float DEFAULT_CURIOUSITY_EXPLORATION = 0.3f;
    constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
    constexpr float DEFAULT_MINIMAL_CURRENT_SCALE = 5.0f;
    constexpr float DEFAULT_TOUCH_SENSITIVITY = 8.0f;
    constexpr float DEFAULT_VISUAL_SENSITIVITY = 2.0f;
    constexpr float DEFAULT_PROPRIOCEPTIVE_SENSITIVITY = 3.0f;
    constexpr size_t DEFAULT_VISION_WIDTH = 16;
    constexpr size_t DEFAULT_VISION_HEIGHT = 16;
    constexpr size_t DEFAULT_TOUCH_SENSORS = 8;
    constexpr size_t DEFAULT_INTERNAL_SENSORS = 4;
    constexpr size_t DEFAULT_PROPRIOCEPTORS = 6;
    constexpr size_t DEFAULT_MOTOR_ACTIONS = 6;
    constexpr float MIN_ACTIVITY_THRESHOLD = 0.0f;
    constexpr float MAX_ACTIVITY_THRESHOLD = 2.0f;
    constexpr float MIN_SENSITIVITY = 0.1f;
    constexpr float MAX_SENSITIVITY = 20.0f;
} // namespace AgentConfig

// Helper function to validate and clamp sensor sensitivity
float validateAndClampSensitivity(float value, float min, float max) {
    return std::clamp(value, min, max);
}

// Helper function to safely get vision size with zero-division protection
size_t getSafeVisionSize(size_t size) {
    return std::max(size, size_t(1));
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
    , sensoryNoveltyDecay_(AgentConfig::DEFAULT_NOVELTY_DECAY)
{
    if (!brain_) {
        throw std::invalid_argument("AgentBrain requires a valid brain pointer");
    }
    
    // Initialize motor and sensory neuron groups
    initializeNeuronGroups();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeNeuronGroups() {
    // Validate brain is available
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize neuron groups: brain is null");
        return;
    }
    
    // Initialize motor neuron groups
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    // Initialize sensory neuron groups  
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    // Distribute neurons based on type
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                distributeMotorNeurons(pop->getNeurons());
            } else if (type == NeuronType::Sensory) {
                distributeSensoryNeurons(pop->getNeurons());
            }
        }
    }
}

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    size_t idx = motorForward_.size() + motorBackward_.size() + 
                motorTurnLeft_.size() + motorTurnRight_.size() +
                motorInteract_.size() + motorWait_.size();
    
    for (Neuron* n : neurons) {
        switch (idx % 6) {
            case 0: motorForward_.push_back(n); break;
            case 1: motorBackward_.push_back(n); break;
            case 2: motorTurnLeft_.push_back(n); break;
            case 3: motorTurnRight_.push_back(n); break;
            case 4: motorInteract_.push_back(n); break;
            case 5: motorWait_.push_back(n); break;
        }
        idx++;
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                sensoryInternal_.size() + sensoryProprioception_.size();
    
    for (Neuron* n : neurons) {
        switch (idx % 4) {
            case 0: sensoryVision_.push_back(n); break;
            case 1: sensoryTouch_.push_back(n); break;
            case 2: sensoryInternal_.push_back(n); break;
            case 3: sensoryProprioception_.push_back(n); break;
        }
        idx++;
    }
}

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
    return AgentConfig::DEFAULT_VISION_WIDTH * AgentConfig::DEFAULT_VISION_HEIGHT +
           AgentConfig::DEFAULT_TOUCH_SENSORS +
           AgentConfig::DEFAULT_INTERNAL_SENSORS +
           AgentConfig::DEFAULT_PROPRIOCEPTORS;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return AgentConfig::DEFAULT_MOTOR_ACTIONS;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    processVisionInput(vision);
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    processTouchInput(touch);
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    processInternalInput(intern);
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    processProprioceptionInput(proprio);
    
    // Compute novelty (difference from previous vision)
    computeNovelty(vision);
    
    // Update curiosity based on novelty
    updateCuriosity();
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity with validation
            float current = vision[i] * AgentConfig::DEFAULT_MINIMAL_CURRENT_SCALE;
            current = std::clamp(current, 0.0f, 100.0f); // Safety clamp
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * validateAndClampSensitivity(
                AgentConfig::DEFAULT_TOUCH_SENSITIVITY,
                AgentConfig::MIN_SENSITIVITY,
                AgentConfig::MAX_SENSITIVITY
            );
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& intern) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * validateAndClampSensitivity(
                AgentConfig::DEFAULT_VISUAL_SENSITIVITY,
                AgentConfig::MIN_SENSITIVITY,
                AgentConfig::MAX_SENSITIVITY
            ) - 1.0f) * AgentConfig::DEFAULT_MINIMAL_CURRENT_SCALE;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * validateAndClampSensitivity(
                AgentConfig::DEFAULT_PROPRIOCEPTIVE_SENSITIVITY,
                AgentConfig::MIN_SENSITIVITY,
                AgentConfig::MAX_SENSITIVITY
            ) - 1.0f) * 3.0f; // Fixed scale factor
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (!vision.empty() && !previousVision_.empty()) {
        float totalDiff = 0.0f;
        size_t safeSize = getSafeVisionSize(vision.size());
        
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / safeSize;
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
}

void AgentBrain::updateCuriosity() {
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > AgentConfig::DEFAULT_CURIOUSITY_EXPLORATION) {
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
            if (n) {
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity using consistent threshold
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct; // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity (consistent threshold)
    if (bestActivity < AgentConfig::DEFAULT_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * AgentConfig::DEFAULT_CURIOUSNESS_EXPLORATION;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command - FIXED: 0 to 6 (inclusive)
            int choice = brain_->getRandomGenerator()->uniformInt(0, 6);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;  // Should never happen
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
            if (!syn) continue; // Safety check
            
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
