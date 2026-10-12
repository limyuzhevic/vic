#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

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

    // Enhanced action selection with temporal context and novelty weighting
    float timeWeightedForwardAct = forwardAct * (1.0f + noveltyLevel_ * 0.3f);
    float timeWeightedBackwardAct = backwardAct * (1.0f + noveltyLevel_ * 0.3f);
    float timeWeightedLeftAct = leftAct * (1.0f + curiosityLevel_ * 0.2f);
    float timeWeightedRightAct = rightAct * (1.0f + curiosityLevel_ * 0.2f);
    float timeWeightedInteractAct = interactAct * (1.0f + dopamineLevel_ * 0.5f);
    float timeWeightedWaitAct = waitAct;
    
    // Apply action-specific modulations based on neuromodulators
    // Dopamine enhances goal-directed actions
    float dopamineMod = 1.0f + std::max(0.0f, dopamineLevel_) * 0.3f;
    timeWeightedForwardAct *= dopamineMod;
    timeWeightedInteractAct *= dopamineMod;
    
    // Novelty enhances exploration and new experiences
    float noveltyMod = 1.0f + noveltyLevel_ * 0.5f;
    timeWeightedLeftAct *= noveltyMod;
    timeWeightedRightAct *= noveltyMod;
    
    // Curiosity drives exploration when baseline action is weak
    float curiosityMod = 1.0f + curiosityLevel_ * 0.4f;
    timeWeightedWaitAct *= curiosityMod;
    
    // Find maximum activity with enhanced weighting
    struct { MotorCommand cmd; float activity; float baseActivity; float neuromodMod; } commands[] = {
        {MotorCommand::MoveForward, timeWeightedForwardAct, forwardAct, dopamineMod},
        {MotorCommand::MoveBackward, timeWeightedBackwardAct, backwardAct, dopamineMod},
        {MotorCommand::TurnLeft, timeWeightedLeftAct, leftAct, noveltyMod},
        {MotorCommand::TurnRight, timeWeightedRightAct, rightAct, noveltyMod},
        {MotorCommand::Interact, timeWeightedInteractAct, interactAct, dopamineMod},
        {MotorCommand::Wait, timeWeightedWaitAct, waitAct, curiosityMod}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    float bestNeuromod = curiosityMod;
    
    // Enhanced selection logic: prefer actions with high neuromodulation and base activity
    for (const auto& c : commands) {
        // Composite score considering activity, neuromodulation, and action type
        float score = c.activity * 0.6f + c.neuromod * 0.3f + c.baseActivity * 0.1f;
        
        // Override for strong neuromodulation (prioritize curiosity-driven exploration)
        if (c.neuromod > 1.3f && c.baseActivity < 0.3f) {
            score *= 1.5f;  // Boost exploration actions when neuromod is high
        }
        
        if (score > bestActivity || 
            (std::abs(score - bestActivity) < 0.01f && c.neuromod > bestNeuromod)) {
            bestActivity = score;
            best = c.cmd;
            bestNeuromod = c.neuromod;
        }
    }
    
    // Enhanced action threshold with adaptive modulation
    float adaptiveThreshold = 0.5f;
    
    // Lower threshold when neuromodulation is high (more exploratory)
    if (neuromodulationLevel > 0.3f) {
        adaptiveThreshold = 0.3f;
    }
    
    // Even lower threshold when curiosity is very high (always explore)
    if (curiosityLevel_ > 0.7f) {
        adaptiveThreshold = 0.2f;
    }
    
    // Enhanced action selection logic
    if (bestActivity < adaptiveThreshold) {
        // Low activity: prefer curiosity-driven exploration or maintain status quo
        if (curiosityLevel_ > 0.6f) {
            // High curiosity: random exploration
            return selectWithCuriosity(MotorCommand::Wait);
        } else if (dopamineLevel_ < -0.3f) {
            // Negative dopamine: increase exploration tendency
            if (std::rand() % 100 < 30) {  // 30% chance to explore
                return selectWithCuriosity(MotorCommand::Wait);
            }
        }
        
        return MotorCommand::Wait;
    }
    
    // Action-specific feedback modulation
    switch (best) {
        case MotorCommand::MoveForward:
            // Record successful forward movement for future reference
            lastAction_ = best;
            actionHistory_.push_back({best, bestActivity, time(0)});
            break;
        case MotorCommand::Interact:
            // Successful interaction can increase confidence in future interactions
            interactionSuccessCount_++;
            lastAction_ = best;
            actionHistory_.push_back({best, bestActivity, time(0)});
            break;
        default:
            lastAction_ = best;
            actionHistory_.push_back({best, bestActivity, time(0)});
            break;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced exploration with temporal weighting and action value learning
    if (curiosityLevel_ > 0.5f) {
        // Calculate action preference based on history and neuromodulation
        std::vector<std::pair<MotorCommand, float>> actionPreferences;
        
        // Get action history for preference calculation
        float totalActionValue = 0.0f;
        for (const auto& actionData : actionHistory_) {
            totalActionValue += actionData.value;
        }
        
        // Calculate preferences based on action value and novelty
        for (int i = 0; i < 8; ++i) {  // All possible motor commands
            MotorCommand cmd = static_cast<MotorCommand>(i);
            float baseValue = 0.33f;  // Base exploration probability
            
            // Find last occurrence of this action
            float lastOccurrence = 1000000.0f; // Large number for "never"
            for (const auto& actionData : actionHistory_) {
                if (actionData.cmd == cmd) {
                    lastOccurrence = std::min(lastOccurrence, actionData.timestamp);
                }
            }
            
            // Lower recent occurrence = higher preference for exploration
            float recencyBonus = std::max(0.0f, 1.0f - (lastOccurrence / 100.0f));
            
            // Action value from history
            float actionValue = 0.0f;
            for (const auto& actionData : actionHistory_) {
                if (actionData.cmd == cmd) {
                    actionValue += actionData.value;
                }
            }
            
            if (actionHistory_.size() > 0) {
                actionValue /= actionHistory_.size();
            }
            
            // Enhanced preference calculation
            float preference = baseValue * (1.0f + curiosityLevel_ * 0.5f) * 
                              (1.0f + recencyBonus * 0.3f) * 
                              (1.0f + actionValue * 0.2f);
            
            // Ensure reasonable bounds
            preference = std::clamp(preference, 0.0f, 1.0f);
            
            actionPreferences.emplace_back(cmd, preference);
            totalActionValue += preference;
        }
        
        // Normalize preferences
        if (totalActionValue > 0.0f) {
            for (auto& pref : actionPreferences) {
                pref.second /= totalActionValue;
            }
        }
        
        // Select action based on preferences (roulette wheel selection)
        float randValue = static_cast<float>(std::rand()) / RAND_MAX;
        float cumulative = 0.0f;
        
        for (const auto& pref : actionPreferences) {
            cumulative += pref.second;
            if (randValue <= cumulative) {
                return pref.first;
            }
        }
        
        // Fallback to last choice
        return lastAction_;
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
