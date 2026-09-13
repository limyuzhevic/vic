#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Constants.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(Constants::INITIAL_PLASTICITY_MODIFIER)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(Constants::DEFAULT_NOVELTY_DECAY)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (const auto& neuron : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % Constants::NUM_MOTOR_ACTIONS) {
                            case 0: motorForward_.push_back(neuron); break;
                            case 1: motorBackward_.push_back(neuron); break;
                            case 2: motorTurnLeft_.push_back(neuron); break;
                            case 3: motorTurnRight_.push_back(neuron); break;
                            case 4: motorInteract_.push_back(neuron); break;
                            case 5: motorWait_.push_back(neuron); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (const auto& neuron : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(neuron); break;
                            case 1: sensoryTouch_.push_back(neuron); break;
                            case 2: sensoryInternal_.push_back(neuron); break;
                            case 3: sensoryProprioception_.push_back(neuron); break;
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(Constants::VISION_WIDTH * Constants::VISION_HEIGHT, 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = Constants::INITIAL_PLASTICITY_MODIFIER;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    return Constants::VISION_SIZE + Constants::SENSE_TOUCH_SIZE + 
           Constants::SENSE_INTERNAL_SIZE + Constants::SENSE_PROPRIOCEPTION_SIZE;
}

size_t AgentBrain::getMotorOutputSize() const {
    return Constants::NUM_MOTOR_ACTIONS;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < std::min(sensoryVision_.size(), vision.size()); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * Constants::kVisionInputScale;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < std::min(sensoryTouch_.size(), touch.size()); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * Constants::kTouchInputScale;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < std::min(sensoryInternal_.size(), intern.size()); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * Constants::kInternalInputScale;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < std::min(sensoryProprioception_.size(), proprio.size()); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * Constants::kProprioceptionInputScale;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty() && !previousVision_.empty()) {
        float totalDiff = 0.0f;
        size_t count = std::min(vision.size(), previousVision_.size());
        for (size_t i = 0; i < count; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize - Fixed division by zero risk
        noveltyLevel_ = totalDiff / static_cast<float>(count);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * Constants::kCuriosityFromNovelty + 
                         std::abs(predictionError_) * Constants::kCuriosityFromPredictionError;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > Constants::kHighCuriosityThreshold) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<std::shared_ptr<Neuron>>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (const auto& n : neurons) {
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
    if (bestActivity < Constants::kMinActivityThreshold) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > Constants::kExplorationCuriosityThreshold) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * Constants::kMaxExplorationChance;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command - FIXED: corrected range bug
            int choice = brain_->getRandomGenerator()->uniformInt(0, Constants::NUM_MOTOR_ACTIONS - 1);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::Interact;
                case 5: return MotorCommand::Wait;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
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
    dopamineLevel_ = std::clamp(dopamineLevel_, Constants::MIN_DOPAMINE_LEVEL, Constants::MAX_DOPAMINE_LEVEL);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > Constants::kMinEligibilityThreshold) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(Constants::kEligibilityDecayRate);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = Constants::kBasePlasticityFactor + Constants::kDopaminePlasticitySlope * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, Constants::kMinPlasticityFactor, Constants::kMaxPlasticityFactor);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(Constants::DEFAULT_LTP_WEIGHT * plasticityFactor);
        stdp->setLTDWeight(Constants::DEFAULT_LTD_WEIGHT * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < Constants::DEVELOPMENT_STAGE_DURATION_INITIAL) {  // ~1 minute
        plasticityModifier_ = Constants::INITIAL_PLASTICITY_MODIFIER;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < Constants::DEVELOPMENT_STAGE_DURATION_CRITICAL) {  // ~5 minutes
        plasticityModifier_ = Constants::CRITICAL_PERIOD_PLASTICITY_MODIFIER;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < Constants::DEVELOPMENT_STAGE_DURATION_MATURATION) {  // ~15 minutes
        plasticityModifier_ = Constants::MATURATION_PLASTICITY_MODIFIER;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = Constants::ADULT_PLASTICITY_MODIFIER;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = Constants::SYNAPTOGENESIS_RATE_INITIAL * plasticityModifier_;
            float pruneRate = Constants::PRUNING_RATE_INITIAL * (Constants::kPruningBaseFactor - plasticityModifier_);
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
    plasticityModifier_ = Constants::INITIAL_PLASTICITY_MODIFIER;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
