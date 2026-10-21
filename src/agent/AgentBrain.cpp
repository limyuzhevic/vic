#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

// Constants for agent brain parameters
namespace AgentBrainConstants {
    // Sensory input sizes
    constexpr size_t VISION_SIZE = 256;          // 16x16 vision grid
    constexpr size_t TOUCH_SIZE = 8;             // Touch sensor inputs
    constexpr size_t INTERNAL_SIZE = 4;          // Internal state sensors
    constexpr size_t PROPRIOCEPTION_SIZE = 6;    // Proprioception sensors
    
    // Curiosity thresholds
    constexpr float MIN_CURIOUSNESS_THRESHOLD = 0.3f;
    constexpr float MAX_CURIOUSNESS_THRESHOLD = 0.5f;
    
    // Activity thresholds
    constexpr float MIN_MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    
    // Novelty decay rate
    constexpr float NOVELTY_DECAY_RATE = 0.99f;
    
    // Scaling factors
    constexpr float VISION_SCALE_FACTOR = 5.0f;
    constexpr float TOUCH_COLLISION_FACTOR = 8.0f;
    constexpr float INTERNAL_SIGNAL_SCALE_FACTOR = 5.0f;
    constexpr float INTERNAL_SIGNAL_CENTER = 2.0f;
    constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;
    constexpr float PROPRIOCEPTION_CENTER = 2.0f;
    
    // Random generation ranges
    constexpr int MIN_RANDOM_INDEX = 0;
    constexpr int MAX_RANDOM_INDEX = 7;  // For MotorCommand enum (0-7 inclusive)
    
    // Motor command count
    constexpr size_t MOTOR_COMMAND_COUNT = 8;  // MotorCommand enum has 8 values (0-7)
    
    // Reward modulation parameters
    constexpr float EXPECTED_REWARD_DECAY = 0.95f;
    constexpr float EXPECTED_REWARD_UPDATE_RATE = 0.05f;
    
    // Eligibility trace parameters
    constexpr float ELIGIBILITY_THRESHOLD = 0.001f;
    constexpr float ELIGIBILITY_DECAY = 0.1f;
    
    // Plasticity factors
    constexpr float PLASTICITY_BASE_FACTOR = 0.5f;
    constexpr float PLASTICITY_CLAMP_MIN = 0.1f;
    constexpr float PLASTICITY_CLAMP_MAX = 2.0f;
    
    // STDP weights
    constexpr float LTP_WEIGHT_BASE = 0.01f;
    constexpr float LTD_WEIGHT_BASE = 0.012f;
    
    // Structural plasticity rates
    constexpr float SYNAPTOGENESIS_BASE_RATE = 0.0001f;
    constexpr float PRUNING_BASE_RATE = 0.00001f;
    
    // Developmental stages
    constexpr double INITIAL_STAGE_AGE = 60.0;
    constexpr double CRITICAL_PERIOD_AGE = 300.0;
    constexpr double MATURATION_STAGE_AGE = 900.0;
    
    // Plasticity modifiers for development stages
    constexpr float HIGH_PLASTICITY = 1.0f;
    constexpr float MODERATE_PLASTICITY = 0.8f;
    constexpr float LOW_PLASTICITY = 0.5f;
    constexpr float ADULT_PLASTICITY = 0.2f;
}

// Helper function to clamp values
inline float clamp(float value, float min, float max) {
    return std::max(min, std::min(value, max));
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
    , plasticityModifier_(AgentBrainConstants::HIGH_PLASTICITY)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(AgentBrainConstants::NOVELTY_DECAY_RATE)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            if (!region) continue;
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue;
                
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    const auto& neurons = pop->getNeurons();
                    if (!neurons.empty()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                     motorTurnLeft_.size() + motorTurnRight_.size() +
                                     motorInteract_.size() + motorWait_.size();
                        
                        // Use modulo to distribute evenly across 6 motor groups
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(neurons[0]); break;
                            case 1: motorBackward_.push_back(neurons[0]); break;
                            case 2: motorTurnLeft_.push_back(neurons[0]); break;
                            case 3: motorTurnRight_.push_back(neurons[0]); break;
                            case 4: motorInteract_.push_back(neurons[0]); break;
                            case 5: motorWait_.push_back(neurons[0]); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    const auto& neurons = pop->getNeurons();
                    if (!neurons.empty()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                     sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        // Use modulo to distribute evenly across 4 sensory groups
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(neurons[0]); break;
                            case 1: sensoryTouch_.push_back(neurons[0]); break;
                            case 2: sensoryInternal_.push_back(neurons[0]); break;
                            case 3: sensoryProprioception_.push_back(neurons[0]); break;
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
    plasticityModifier_ = AgentBrainConstants::HIGH_PLASTICITY;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    // Updated from hardcoded values to named constants for better maintainability
    return AgentBrainConstants::VISION_SIZE + AgentBrainConstants::TOUCH_SIZE + 
           AgentBrainConstants::INTERNAL_SIZE + AgentBrainConstants::PROPRIOCEPTION_SIZE;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    // Updated to match MotorCommand enum which has 8 values (0-7)
    return AgentBrainConstants::MOTOR_COMMAND_COUNT;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * AgentBrainConstants::VISION_SCALE_FACTOR;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * AgentBrainConstants::TOUCH_COLLISION_FACTOR;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            // Center at 0 and scale the signal: (x * 2 - 1) * 5
            float current = (intern[i] * AgentBrainConstants::INTERNAL_SIGNAL_CENTER - 1.0f) * 
                           AgentBrainConstants::INTERNAL_SIGNAL_SCALE_FACTOR;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            // Center at 0 and scale the signal: (x * 2 - 1) * 3
            float current = (proprio[i] * AgentBrainConstants::PROPRIOCEPTION_CENTER - 1.0f) * 
                           AgentBrainConstants::PROPRIOCEPTION_SCALE_FACTOR;
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
        // Curiously exploration: novelty * 2.0 + prediction error * 0.5
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > AgentBrainConstants::MIN_CURIOUSNESS_THRESHOLD) {
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
            if (!n) continue;  // Null pointer check
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
    if (bestActivity < AgentBrainConstants::MIN_MOTOR_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > AgentBrainConstants::MAX_CURIOUSNESS_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(
                AgentBrainConstants::MIN_RANDOM_INDEX, 
                AgentBrainConstants::MAX_RANDOM_INDEX
            );
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                case 7: return MotorCommand::Wait;
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
    expectedReward_ = AgentBrainConstants::EXPECTED_REWARD_DECAY * expectedReward_ + 
                     AgentBrainConstants::EXPECTED_REWARD_UPDATE_RATE * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        if (!region) continue;
        
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > AgentBrainConstants::ELIGIBILITY_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(AgentBrainConstants::ELIGIBILITY_DECAY);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = AgentBrainConstants::PLASTICITY_BASE_FACTOR + 
                             AgentBrainConstants::PLASTICITY_BASE_FACTOR * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 
                                  AgentBrainConstants::PLASTICITY_CLAMP_MIN,
                                  AgentBrainConstants::PLASTICITY_CLAMP_MAX);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(AgentBrainConstants::LTP_WEIGHT_BASE * plasticityFactor);
        stdp->setLTDWeight(AgentBrainConstants::LTD_WEIGHT_BASE * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < AgentBrainConstants::INITIAL_STAGE_AGE) {  // ~1 minute
        plasticityModifier_ = AgentBrainConstants::HIGH_PLASTICITY;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < AgentBrainConstants::CRITICAL_PERIOD_AGE) {  // ~5 minutes
        plasticityModifier_ = AgentBrainConstants::MODERATE_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < AgentBrainConstants::MATURATION_STAGE_AGE) {  // ~15 minutes
        plasticityModifier_ = AgentBrainConstants::LOW_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = AgentBrainConstants::ADULT_PLASTICITY;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = AgentBrainConstants::SYNAPTOGENESIS_BASE_RATE * plasticityModifier_;
            float pruneRate = AgentBrainConstants::PRUNING_BASE_RATE * (2.0f - plasticityModifier_);
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
    plasticityModifier_ = AgentBrainConstants::HIGH_PLASTICITY;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
