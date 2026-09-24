#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

// Configuration constants for agent behavior
namespace AgentBrainConfig {
    constexpr float DEFAULT_SENORY_NOVELTY_DECAY = 0.99f;
    constexpr float DEFAULT_SENSORY_VISION_SCALE = 5.0f;
    constexpr float DEFAULT_SENSORY_TOUCH_SCALE = 8.0f;
    constexpr float DEFAULT_SENSORY_INTERNAL_SCALE = 5.0f;
    constexpr float DEFAULT_SENSORY_PROPRIOCEPTION_SCALE = 3.0f;
    constexpr float DEFAULT_NOVELTY_NORMALIZATION_FACTOR = 1.0f;
    constexpr float DEFAULT_CURIOUSITY_WEIGHT_NOVELTY = 2.0f;
    constexpr float DEFAULT_CURIOUSITY_WEIGHT_PREDICTION_ERROR = 0.5f;
    constexpr float DEFAULT_MAX_CURIOUSITY_LEVEL = 1.0f;
    constexpr float DEFAULT_MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    constexpr float DEFAULT_EXPLORATION_CHANCE_FACTOR = 0.3f;
    constexpr float DEFAULT_MAX_EXPLORATION_CHANCE = 0.3f;
    constexpr float DEFAULT_DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DEFAULT_DOPAMINE_CLAMP_MAX = 1.0f;
    constexpr float DEFAULT_PLASTICITY_FACTOR_BASE = 0.5f;
    constexpr float DEFAULT_PLASTICITY_FACTOR_RANGE = 0.5f;
    constexpr float DEFAULT_PLASTICITY_MIN = 0.1f;
    constexpr float DEFAULT_PLASTICITY_MAX = 2.0f;
    constexpr float DEFAULT_ELIGIBILITY_THRESHOLD = 0.001f;
    constexpr float DEFAULT_ELIGIBILITY_DECAY_RATE = 0.1f;
    constexpr float DEFAULT_SYNAPTICENESIS_RATE_FACTOR = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE_FACTOR = 0.00001f;
    constexpr float DEFAULT_PRUNING_RATE_BASE = 2.0f;
    constexpr double INITIAL_DEVELOPMENT_AGE = 0.0;
    constexpr float INITIAL_PLASTICITY_MODIFIER = 1.0f;
}

// Validation helper functions
static bool validateInput(const SensoryPercept& percept) {
    if (!percept.getVision().empty() && 
        percept.getVision().size() > AgentBrain::getSensoryInputSize()) {
        NLM_LOG_WARNING("Vision percept size exceeds expected sensory input size");
        return false;
    }
    return true;
}

static float calculateScaleFactor(float input, float minInput, float maxInput, 
                                 float minOutput, float maxOutput) {
    if (maxInput <= minInput) return minOutput;
    float normalized = (input - minInput) / (maxInput - minInput);
    normalized = std::max(0.0f, std::min(1.0f, normalized));
    return minOutput + normalized * (maxOutput - minOutput);
}

static size_t calculateExpectedNeuronCount(const std::vector<Neuron*>& neurons) {
    size_t count = 0;
    for (const auto* neuron : neurons) {
        if (neuron) count++;
    }
    return count;
}

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(AgentBrainConfig::INITIAL_DEVELOPMENT_AGE)
    , plasticityModifier_(AgentBrainConfig::INITIAL_PLASTICITY_MODIFIER)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(AgentBrainConfig::DEFAULT_SENORY_NOVELTY_DECAY)
{
    // Validate brain pointer
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain initialized with null brain pointer");
        return;
    }
    
    // Initialize motor and sensory neuron groups
    try {
        for (const auto& region : brain_->getRegions()) {
            if (!region) {
                NLM_LOG_WARNING("Skipping null region in AgentBrain initialization");
                continue;
            }
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) {
                    NLM_LOG_WARNING("Skipping null population in AgentBrain initialization");
                    continue;
                }
                
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) {
                            NLM_LOG_WARNING("Skipping null neuron in motor population");
                            continue;
                        }
                        
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
                        if (!n) {
                            NLM_LOG_WARNING("Skipping null neuron in sensory population");
                            continue;
                        }
                        
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during AgentBrain neuron group initialization: " + std::string(e.what()));
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain: brain pointer is null");
        return;
    }
    
    if (world.getVisionWidth() == 0 || world.getVisionHeight() == 0) {
        NLM_LOG_WARNING("Initializing AgentBrain with invalid world dimensions");
    }
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = AgentBrainConfig::INITIAL_DEVELOPMENT_AGE;
    plasticityModifier_ = AgentBrainConfig::INITIAL_PLASTICITY_MODIFIER;
    
    // Log initialization details
    size_t visionNeurons = calculateExpectedNeuronCount(sensoryVision_);
    size_t touchNeurons = calculateExpectedNeuronCount(sensoryTouch_);
    size_t internalNeurons = calculateExpectedNeuronCount(sensoryInternal_);
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(visionNeurons) + " vision sensory neurons, " +
                 std::to_string(touchNeurons) + " touch sensory neurons, " +
                 std::to_string(internalNeurons) + " internal sensory neurons");
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
    if (!brain_) {
        NLM_LOG_ERROR("Cannot process sensory input: brain pointer is null");
        return;
    }
    
    if (!validateInput(percept)) {
        NLM_LOG_WARNING("Invalid sensory percept input, skipping processing");
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
    
    // Compute and update novelty
    updateNovelty(percept.getVision());
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        updateCuriosity();
    }
}

// Helper methods implementation
void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * AgentBrainConfig::DEFAULT_SENSORY_VISION_SCALE;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * AgentBrainConfig::DEFAULT_SENSORY_TOUCH_SCALE;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (internal[i] * 2.0f - 1.0f) * AgentBrainConfig::DEFAULT_SENSORY_INTERNAL_SCALE;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * AgentBrainConfig::DEFAULT_SENSORY_PROPRIOCEPTION_SCALE;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::updateNovelty(const std::vector<float>& vision) {
    if (vision.empty()) return;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize using config value
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    
    // Apply decay
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Store for next time
    previousVision_ = vision;
}

void AgentBrain::updateCuriosity() {
    curiosityLevel_ = noveltyLevel_ * AgentBrainConfig::DEFAULT_CURIOUSITY_WEIGHT_NOVELTY + 
                     std::abs(predictionError_) * AgentBrainConfig::DEFAULT_CURIOUSITY_WEIGHT_PREDICTION_ERROR;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, AgentBrainConfig::DEFAULT_MAX_CURIOUSITY_LEVEL);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot decode motor command: brain pointer is null");
        return MotorCommand::Wait;
    }
    
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
