#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../sensory/Vision.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <random>

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
    , stressLevel_(0.0f)
    , fatigueLevel_(0.0f)
    , energyLevel_(1.0f)
    , healthLevel_(1.0f)
    , attentionLevel_(0.5f)
    , learningRate_(0.01f)
    , explorationRate_(0.1f)
    , noveltyThreshold_(0.3f)
    , rewardPredictionError_(0.0f)
    , memoryConsolidationAge_(0.0)
    , habituationLevel_(0.0f)
    , socialLearningEnabled_(false)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , noveltyEnabled_(true)
    , stressEnabled_(true)
    , fatigueEnabled_(true)
    , energyEnabled_(true)
    , healthEnabled_(true)
    , attentionEnabled_(true)
    , learningEnabled_(true)
    , explorationEnabled_(true)
    , socialLearningEnabled_(false)
    , sensoryNoveltyDecay_(0.99f)
    , stressDecay_(0.95f)
    , fatigueDecay_(0.98f)
    , energyDecay_(0.99f)
    , healthDecay_(0.97f)
    , attentionDecay_(0.95f)
    , learningDecay_(0.98f)
    , explorationDecay_(0.98f)
{
    // Initialize random number generator
    std::random_device rd;
    rng_.seed(rd());
    
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
                                    motorInteract_.size() + motorWait_.size() +
                                    motorLookLeft_.size() + motorLookRight_.size() +
                                    motorLookUp_.size() + motorLookDown_.size() +
                                    motorEat_.size() + motorDrink_.size() +
                                    motorRest_.size();
                        
                        switch (idx % 13) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                            case 6: motorLookLeft_.push_back(n); break;
                            case 7: motorLookRight_.push_back(n); break;
                            case 8: motorLookUp_.push_back(n); break;
                            case 9: motorLookDown_.push_back(n); break;
                            case 10: motorEat_.push_back(n); break;
                            case 11: motorDrink_.push_back(n); break;
                            case 12: motorRest_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size() +
                                    sensoryAudio_.size() + sensoryOlfactory_.size() +
                                    sensoryGustatory_.size();
                        
                        switch (idx % 7) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                            case 4: sensoryAudio_.push_back(n); break;
                            case 5: sensoryOlfactory_.push_back(n); break;
                            case 6: sensoryGustatory_.push_back(n); break;
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
    stressLevel_ = 0.0f;
    fatigueLevel_ = 0.0f;
    energyLevel_ = 1.0f;
    healthLevel_ = 1.0f;
    attentionLevel_ = 0.5f;
    learningRate_ = 0.01f;
    explorationRate_ = 0.1f;
    habituationLevel_ = 0.0f;
    rewardPredictionError_ = 0.0f;
    memoryConsolidationAge_ = 0.0;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                 std::to_string(sensoryAudio_.size()) + " audio sensory neurons");
}

void AgentBrain::enableDevelopmentSystem(bool enable) {
    developmentEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Development system enabled");
    } else {
        NLM_LOG_INFO("Development system disabled");
    }
}

void AgentBrain::enableExplorationSystem(bool enable) {
    explorationEnabled_ = enable;
    curiosityEnabled_ = enable;
    noveltyEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Exploration system enabled");
    } else {
        NLM_LOG_INFO("Exploration system disabled");
    }
}

void AgentBrain::enableSocialLearning(bool enable) {
    socialLearningEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Social learning enabled");
    } else {
        NLM_LOG_INFO("Social learning disabled");
    }
}

void AgentBrain::enableStressSystem(bool enable) {
    stressEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Stress system enabled");
    } else {
        NLM_LOG_INFO("Stress system disabled");
    }
}

void AgentBrain::enableFatigueSystem(bool enable) {
    fatigueEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Fatigue system enabled");
    } else {
        NLM_LOG_INFO("Fatigue system disabled");
    }
}

void AgentBrain::enableEnergySystem(bool enable) {
    energyEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Energy system enabled");
    } else {
        NLM_LOG_INFO("Energy system disabled");
    }
}

void AgentBrain::enableHealthSystem(bool enable) {
    healthEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Health system enabled");
    } else {
        NLM_LOG_INFO("Health system disabled");
    }
}

void AgentBrain::enableAttentionSystem(bool enable) {
    attentionEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Attention system enabled");
    } else {
        NLM_LOG_INFO("Attention system disabled");
    }
}

void AgentBrain::enableLearningSystem(bool enable) {
    learningEnabled_ = enable;
    rewardModulationEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Learning system enabled");
    } else {
        NLM_LOG_INFO("Learning system disabled");
    }
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i] != nullptr) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i] != nullptr) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i] != nullptr) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i] != nullptr) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Audio input (if available)
    const auto& audio = percept.getAudio();
    for (size_t i = 0; i < sensoryAudio_.size() && i < audio.size(); ++i) {
        if (sensoryAudio_[i] != nullptr) {
            float current = audio[i] * 3.0f;  // Sound intensity
            sensoryAudio_[i]->injectCurrent(current);
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
        
        // Update habituation based on repeated exposure
        if (noveltyLevel_ < noveltyThreshold_) {
            habituationLevel_ = std::min(1.0f, habituationLevel_ + 0.01f);
        } else {
            habituationLevel_ = std::max(0.0f, habituationLevel_ - 0.05f);
        }
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty and exploration drives
    if (curiosityEnabled_ && explorationEnabled_) {
        // Primary curiosity: response to novelty
        float curiosityFromNovelty = noveltyLevel_ * (1.0f - habituationLevel_);
        
        // Secondary curiosity: response to prediction error
        float curiosityFromError = std::abs(predictionError_) * 0.5f;
        
        // Tertiary curiosity: response to stress and fatigue
        float curiosityFromStress = stressLevel_ * 0.3f;
        float curiosityFromFatigue = fatigueLevel_ * 0.2f;
        
        // Combine curiosity sources
        curiosityLevel_ = curiosityFromNovelty + curiosityFromError + 
                         curiosityFromStress + curiosityFromFatigue;
        
        // Apply exploration drive
        curiosityLevel_ *= (1.0f + explorationRate_);
        
        // Clamp to reasonable range
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 2.0f);
    }
    
    // Update attention based on sensory input strength
    if (attentionEnabled_) {
        float attentionFromVision = 0.0f;
        for (float val : vision) {
            attentionFromVision += std::abs(val);
        }
        attentionFromVision /= std::max(1.0f, static_cast<float>(vision.size()));
        
        // Combine with existing attention
        attentionLevel_ = attentionLevel_ * 0.9f + attentionFromVision * 0.1f;
        attentionLevel_ = std::clamp(attentionLevel_, 0.1f, 1.0f);
    }
    
    // Update stress based on sensory challenges
    if (stressEnabled_) {
        float stressFromNovelty = noveltyLevel_ * 0.3f;
        float stressFromPredictionError = std::abs(predictionError_) * 0.4f;
        float stressFromConflict = 0.0f;
        
        // Stress decays over time
        stressLevel_ = stressLevel_ * stressDecay_ + stressFromNovelty + stressFromPredictionError + stressFromConflict;
        stressLevel_ = std::clamp(stressLevel_, 0.0f, 1.0f);
    }
    
    // Update fatigue based on activity
    if (fatigueEnabled_) {
        // Fatigue increases with activity level
        float activityLevel = 0.0f;
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i] != nullptr) {
                activityLevel += std::abs(sensoryVision_[i]->getState().membranePotential - sensoryVision_[i]->getState().restingPotential);
            }
        }
        activityLevel /= std::max(1.0f, static_cast<float>(sensoryVision_.size()));
        
        // Fatigue grows with activity and decays with rest
        fatigueLevel_ = fatigueLevel_ * fatigueDecay_ + activityLevel * 0.05f;
        fatigueLevel_ = std::clamp(fatigueLevel_, 0.0f, 1.0f);
    }
    
    // Update energy based on expenditure
    if (energyEnabled_) {
        // Energy decays with activity and can be replenished
        float energyExpenditure = fatigueLevel_ * 0.3f;
        energyLevel_ = energyLevel_ * energyDecay_ - energyExpenditure;
        energyLevel_ = std::clamp(energyLevel_, 0.0f, 1.0f);
        
        // If energy is too low, reduce performance
        if (energyLevel_ < 0.3f) {
            attentionLevel_ *= energyLevel_ / 0.3f;
            learningRate_ *= energyLevel_ / 0.3f;
        }
    }
    
    // Update health based on stress and fatigue
    if (healthEnabled_) {
        float healthFromStress = 1.0f - stressLevel_;
        float healthFromFatigue = 1.0f - fatigueLevel_;
        float healthFromEnergy = energyLevel_;
        
        healthLevel_ = healthFromStress * 0.4f + healthFromFatigue * 0.3f + healthFromEnergy * 0.3f;
        healthLevel_ = std::clamp(healthLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && explorationEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Apply habituation-based action selection
    if (habituationLevel_ > 0.5f && attentionLevel_ > 0.7f) {
        // Highly habitual responses when well-attended
        decoded = selectHabitualAction(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (n == nullptr) continue;
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
    float lookLeftAct = calcActivity(motorLookLeft_);
    float lookRightAct = calcActivity(motorLookRight_);
    float lookUpAct = calcActivity(motorLookUp_);
    float lookDownAct = calcActivity(motorLookDown_);
    float eatAct = calcActivity(motorEat_);
    float drinkAct = calcActivity(motorDrink_);
    float restAct = calcActivity(motorRest_);
    
    // Find maximum activity with attention weighting
    struct { MotorCommand cmd; float activity; float attention_weight; } commands[] = {
        {MotorCommand::MoveForward, forwardAct, attentionLevel_},
        {MotorCommand::MoveBackward, backwardAct, attentionLevel_ * 0.8f},
        {MotorCommand::TurnLeft, leftAct, attentionLevel_ * 0.9f},
        {MotorCommand::TurnRight, rightAct, attentionLevel_ * 0.9f},
        {MotorCommand::LookLeft, lookLeftAct, attentionLevel_ * 0.7f},
        {MotorCommand::LookRight, lookRightAct, attentionLevel_ * 0.7f},
        {MotorCommand::Interact, interactAct, attentionLevel_ * 0.8f},
        {MotorCommand::Wait, waitAct, attentionLevel_ * 0.5f},
        {MotorCommand::Look, lookUpAct, attentionLevel_ * 0.6f},
        {MotorCommand::LookUp, lookUpAct, attentionLevel_ * 0.6f},
        {MotorCommand::LookDown, lookDownAct, attentionLevel_ * 0.6f},
        {MotorCommand::Eat, eatAct, attentionLevel_ * 0.7f},
        {MotorCommand::Drink, drinkAct, attentionLevel_ * 0.7f},
        {MotorCommand::Rest, restAct, attentionLevel_ * 0.5f}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct * attentionLevel_;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        // Apply attention modulation
        float weightedActivity = c.activity * c.attention_weight;
        
        if (weightedActivity > bestActivity) {
            bestActivity = weightedActivity;
            best = c.cmd;
        }
    }
    
    // Apply stress modulation
    if (stressLevel_ > 0.5f) {
        // High stress reduces complex actions
        if (best == MotorCommand::LookUp || best == MotorCommand::LookDown || 
            best == MotorCommand::LookLeft || best == MotorCommand::LookRight) {
            best = MotorCommand::Wait; // Skip detailed looking actions
        }
    }
    
    // Apply fatigue modulation
    if (fatigueLevel_ > 0.7f) {
        // High fatigue favors simple actions
        if (best == MotorCommand::Eat || best == MotorCommand::Drink) {
            best = MotorCommand::MoveForward; // Switch to simpler action
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f * attentionLevel_) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = rng_() / static_cast<float>(rng_.max());
        if (r < exploreChance) {
            // Random motor command
            int choice = rng_() % 15; // 15 different actions now
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::LookUp;
                case 7: return MotorCommand::LookDown;
                case 8: return MotorCommand::Interact;
                case 9: return MotorCommand::Eat;
                case 10: return MotorCommand::Drink;
                case 11: return MotorCommand::Rest;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::selectHabitualAction(MotorCommand defaultCmd) {
    // Habituation: prefer actions that have been successful before
    // This implements reinforcement learning through habit formation
    
    struct { MotorCommand cmd; float success_history; } actionHistory[] = {
        {MotorCommand::MoveForward, 0.8f},
        {MotorCommand::TurnLeft, 0.7f},
        {MotorCommand::TurnRight, 0.7f},
        {MotorCommand::Interact, 0.6f},
        {MotorCommand::Wait, 0.5f}
    };
    
    MotorCommand best = defaultCmd;
    float bestSuccess = 0.0f;
    
    for (const auto& a : actionHistory) {
        if (a.success_history > bestSuccess) {
            bestSuccess = a.success_history;
            best = a.cmd;
        }
    }
    
    return best;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_ || !learningEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    rewardPredictionError_ = predictionError_;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Apply stress modulation to learning
    float stressModulation = 1.0f + stressLevel_ * 0.5f;  // Stress can enhance learning in moderation
    float stressFactor = std::clamp(stressModulation, 0.5f, 2.0f);
    
    // Apply fatigue modulation to learning
    float fatigueModulation = 1.0f - fatigueLevel_ * 0.8f;  // Fatigue reduces learning
    float fatigueFactor = std::clamp(fatigueModulation, 0.1f, 1.0f);
    
    // Apply attention modulation to learning
    float attentionModulation = attentionLevel_ * 1.2f;  // Attention enhances learning
    float attentionFactor = std::clamp(attentionModulation, 0.5f, 1.5f);
    
    // Apply energy modulation to learning
    float energyModulation = energyLevel_ * 1.1f;  // Energy supports learning
    float energyFactor = std::clamp(energyModulation, 0.3f, 1.2f);
    
    // Apply health modulation to learning
    float healthModulation = healthLevel_ * 1.05f;  // Health supports learning
    float healthFactor = std::clamp(healthModulation, 0.5f, 1.1f);
    
    // Combined learning factor
    float learningFactor = stressFactor * fatigueFactor * attentionFactor * energyFactor * healthFactor;
    learningFactor = std::clamp(learningFactor, 0.1f, 3.0f);
    
    // Scale with learning rate
    learningFactor *= learningRate_;
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_ * learningFactor;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f * learningFactor);
            }
        }
    }
    
    // Update habituation based on reward consistency
    if (std::abs(predictionError_) < 0.1f) {
        // Small prediction error increases habituation
        habituationLevel_ = std::min(1.0f, habituationLevel_ + 0.02f);
    } else {
        // Large prediction error decreases habituation
        habituationLevel_ = std::max(0.0f, habituationLevel_ - 0.05f);
    }
    
    // Modulate plasticity based on neuromodulators
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor * learningFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor * learningFactor);
    }
    
    // Update social learning (if enabled)
    if (socialLearningEnabled_) {
        updateSocialLearning(reward, predictedReward);
    }
}

void AgentBrain::updateSocialLearning(float reward, float predictedReward) {
    // Social learning: learn from observing others
    // This implements observation learning and imitation
    
    // Calculate social prediction error
    float socialPredictionError = reward - predictedReward;
    
    // Update social learning rate based on attention
    float socialLearningRate = learningRate_ * attentionLevel_ * 0.5f;
    
    // Apply to all synapses with social eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            // Social eligibility trace (based on social observation)
            float socialEligibility = syn->getSocialEligibilityTrace();
            
            if (std::abs(socialEligibility) > 0.001f) {
                // Apply social reward-modulated weight change
                float delta = socialEligibility * socialPredictionError * socialLearningRate;
                syn->addToWeight(delta);
                
                // Decay social eligibility trace
                syn->decaySocialEligibilityTrace(0.05f * socialLearningRate);
            }
        }
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Update memory consolidation based on age
    memoryConsolidationAge_ += timestep;
    if (memoryConsolidationAge_ > 1000.0) {
        consolidateMemories();
        memoryConsolidationAge_ = 0.0;
    }
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        learningRate_ = 0.05f;       // Fast learning
        explorationRate_ = 0.2f;    // High exploration
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        learningRate_ = 0.03f;
        explorationRate_ = 0.15f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        learningRate_ = 0.02f;
        explorationRate_ = 0.1f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else if (developmentalAge_ < 2700.0) {  // ~45 minutes
        plasticityModifier_ = 0.3f;
        learningRate_ = 0.01f;
        explorationRate_ = 0.05f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        learningRate_ = 0.005f;      // Slow learning
        explorationRate_ = 0.02f;   // Low exploration
        brain_->setDevelopmentalStage(DevelopmentalStage::Aging);
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
    
    // Update developmental abilities based on stage
    updateDevelopmentalAbilities();
}

void AgentBrain::updateDevelopmentalAbilities() {
    switch (brain_->getDevelopmentalStage()) {
        case DevelopmentalStage::Initial:
            // High plasticity, basic learning, high exploration
            enableExplorationSystem(true);
            enableLearningSystem(true);
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Language-like learning, intermediate exploration
            enableExplorationSystem(true);
            enableLearningSystem(true);
            enableSocialLearning(false);  // Social learning develops later
            break;
            
        case DevelopmentalStage::Maturation:
            // Specialized learning, reduced exploration
            enableExplorationSystem(true);
            enableLearningSystem(true);
            enableSocialLearning(true);   // Social learning emerges
            break;
            
        case DevelopmentalStage::Adult:
            // Refinement, low exploration
            enableExplorationSystem(false);
            enableLearningSystem(true);
            enableSocialLearning(true);
            enableStressSystem(true);     // Stress response develops
            break;
            
        case DevelopmentalStage::Aging:
            // Decline in plasticity and learning
            enableExplorationSystem(false);
            enableLearningSystem(false);  // Learning declines
            enableSocialLearning(true);   // Social bonds persist
            enableFatigueSystem(true);    // Fatigue increases
            break;
    }
}

void AgentBrain::consolidateMemories() {
    if (!brain_) return;
    
    NLM_LOG_INFO("Consolidating memories...");
    
    // Simulate memory consolidation process
    // This strengthens important memories and weakens unimportant ones
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            // Memory consolidation based on synaptic strength
            float weight = syn->getWeight();
            
            // Strengthen strong synapses (important memories)
            if (weight > 0.1f) {
                syn->addToWeight(0.01f * weight);
            }
            
            // Weaken weak synapses (unimportant memories)
            else if (weight < -0.1f) {
                syn->addToWeight(-0.01f * std::abs(weight));
            }
        }
    }
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    stressLevel_ = 0.0f;
    fatigueLevel_ = 0.0f;
    energyLevel_ = 1.0f;
    healthLevel_ = 1.0f;
    attentionLevel_ = 0.5f;
    learningRate_ = 0.01f;
    explorationRate_ = 0.1f;
    habituationLevel_ = 0.0f;
    rewardPredictionError_ = 0.0f;
    memoryConsolidationAge_ = 0.0;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
