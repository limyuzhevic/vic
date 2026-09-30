#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , serotoninLevel_(0.0f)
    , norepinephrineLevel_(0.0f)
    , acetylcholineLevel_(0.0f)
    , gabaLevel_(0.0f)
    , glutamateLevel_(0.0f)
    , endorphinsLevel_(0.0f)
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

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // ===== PHASE 6 ENHANCED SENSORY PROCESSING =====
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Enhanced injection with neuromodulation
            float current = vision[i] * 5.0f;  // Scale factor
            
            // Apply neuromodulation to sensory processing
            float neuromodulation = 1.0f;
            if (rewardModulationEnabled_) {
                neuromodulation += dopamineLevel_ * 0.3f;
            }
            if (curiosityEnabled_ && noveltyLevel_ > 0.3f) {
                neuromodulation += 0.5f;  // Enhance sensitivity to novel stimuli
            }
            
            // Store in working memory with enhanced encoding
            current *= neuromodulation;
            
            // Enhanced neural encoding with memory integration
            if (brain_->getWorkingMemory()) {
                // Store with sensory novelty modulation
                float noveltyWeight = (noveltyLevel_ + 0.1f) * 0.5f;  // 0.05-0.5 scale
                brain_->getWorkingMemory()->storeToNeuron(sensoryVision_[i]->getId(), 
                    std::min(1.0f, vision[i] * 3.0f * (1.0f + noveltyWeight)));
            }
            
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            
            // Enhanced with prediction error modulation
            float predictionErrorMod = 1.0f + std::abs(predictionError_) * 0.5f;
            current *= predictionErrorMod;
            
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            
            // Enhanced with curiosity-based exploration
            if (curiosityEnabled_ && curiosityLevel_ > 0.5f) {
                current *= 1.5f;  // Enhance internal exploration
            }
            
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            
            // Enhanced with reward modulation
            if (rewardModulationEnabled_) {
                float rewardMod = dopamineLevel_ * 0.2f + 1.0f;
                current *= rewardMod;
            }
            
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // ===== NOVELTY AND PREDICTION INTEGRATION =====
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Enhanced novelty calculation with prediction error
        float predictionErrorWeighted = std::abs(predictionError_) * 5.0f;
        totalDiff += predictionErrorWeighted; // Prediction error adds to novelty
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update with curiosity modulation
        noveltyLevel_ *= sensoryNoveltyDecay_ * (1.0f - curiosityLevel_ * 0.1f);
        
        // Store for next time
        previousVision_ = vision;
        
        // Feed novelty back to sensory processing
        if (noveltyLevel_ > 0.5f) {
            // High novelty enhances all sensory currents
            float enhancement = (noveltyLevel_ - 0.5f) * 2.0f;  // 0-1 scale
            for (auto& neuron : sensoryVision_) {
                if (neuron) {
                    neuron->injectCurrent(enhancement);
                }
            }
        }
    }
    
    // ===== PREDICTION AND LEARNING INTEGRATION =====
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        // Enhanced curiosity formula: novelty * prediction_error * exploration_drive
        float curiosityFromNovelty = noveltyLevel_ * 0.7f;
        float curiosityFromError = std::abs(predictionError_) * 0.3f;
        float explorationDrive = developmentalAge_ > 2 ? 0.5f : 1.0f;  // Reduced exploration in adult
        
        curiosityLevel_ = (curiosityFromNovelty + curiosityFromError) * explorationDrive;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // ===== DEVELOPMENTAL MODULATION =====
    
    // Development affects sensory processing (reduced sensitivity in adult)
    if (developmentEnabled_ && developmentalAge_ > 300.0) {
        // Reduced sensory gain in adult stage
        float sensoryGainReduction = 0.8f - (plasticityModifier_ * 0.3f);
        
        for (auto& neuron : sensoryVision_) {
            if (neuron) {
                // Reset to baseline for developmental modulation
                neuron->setMembranePotential(neuron->getState().restingPotential);
            }
        }
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // ===== PHASE 6 ENHANCED MOTOR DECODING =====
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Apply developmental modulation to motor selection
    if (developmentEnabled_ && developmentalAge_ > 300.0) {
        // In adult stage, bias toward more stable behaviors
        if (developmentalStage_ > 2) {
            float stabilityBias = 0.7f;
            if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) > stabilityBias) {
                // Maintain current behavior with higher probability
                return decoded;
            }
        }
    }
    
    // Apply neuromodulation-based motor modulation
    if (rewardModulationEnabled_ && std::abs(dopamineLevel_) > 0.2f) {
        // High dopamine -> increased exploration
        if (dopamineLevel_ > 0.5f) {
            // Strong positive dopamine encourages novel actions
            if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < 0.3f) {
                // Occasionally choose random action
                return selectWithCuriosity(MotorCommand::Wait);
            }
        } else if (dopamineLevel_ < -0.5f) {
            // Strong negative dopamine encourages cautious behavior
            return MotorCommand::Wait;  // Avoid risky actions
        }
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
    
    // ===== PHASE 6 ENHANCED REWARD MODULATION =====
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average with decay)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // ===== ENHANCED DOPAMINE SIGNALLING =====
    
    // Dopamine-like signal based on prediction error
    dopamineLevel_ = predictionError_;
    
    // Decay prediction error over time
    predictionError_ *= 0.9f;
    
    // ===== INTEGRATED PLASTICITY MODULATION =====
    
    // Apply to all synapses with eligibility traces (enhanced)
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Enhanced reward-modulated weight change
                // Integrate neuromodulation factors
                float neuromodulation = 1.0f;
                
                if (rewardModulationEnabled_) {
                    neuromodulation += dopamineLevel_ * 0.5f;
                }
                if (structuralPlasticityEnabled_) {
                    neuromodulation *= (1.0f + developmentalAge_ * 0.001f);
                }
                
                float delta = eligibility * dopamineLevel_ * plasticityModifier_ * neuromodulation;
                syn->addToWeight(delta);
                
                // Enhanced eligibility trace decay
                float decayRate = 0.1f + curiosityLevel_ * 0.1f;  // Faster decay with high curiosity
                syn->decayEligibilityTrace(decayRate);
            }
        }
    }
    
    // ===== NEUROMODULATION FEEDBACK TO AGENT STATE =====
    
    // Update neuromodulation state
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Update development based on reward history
    float rewardHistory = std::abs(expectedReward_);
    if (developmentEnabled_ && developmentalAge_ < 600.0) {
        // Reward accelerates development
        developmentalAge_ += reward * 0.5f;
        
        // Update developmental stage
        if (developmentalAge_ < 60.0) {  // ~1 minute
            developmentalStage_ = DevelopmentalStage::Initial;
        } else if (developmentalAge_ < 300.0) {  // ~5 minutes
            developmentalStage_ = DevelopmentalStage::CriticalPeriod;
        } else if (developmentalAge_ < 900.0) {  // ~15 minutes
            developmentalStage_ = DevelopmentalStage::Maturation;
        } else {
            developmentalStage_ = DevelopmentalStage::Adult;
        }
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

// Serotonin level
float getSerotoninLevel() const {
    return serotoninLevel_;
}

// Norepinephrine level
float getNorepinephrineLevel() const {
    return norepinephrineLevel_;
}

// Acetylcholine level
float getAcetylcholineLevel() const {
    return acetylcholineLevel_;
}

// GABA level
float getGABALevel() const {
    return gabaLevel_;
}

// Glutamate level
float getGlutamateLevel() const {
    return glutamateLevel_;
}

// Endorphins level
float getEndorphinsLevel() const {
    return endorphinsLevel_;
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
    // Reset all neuromodulator levels
    dopamineLevel_ = 0.0f;
    serotoninLevel_ = 0.0f;
    norepinephrineLevel_ = 0.0f;
    acetylcholineLevel_ = 0.0f;
    gabaLevel_ = 0.0f;
    glutamateLevel_ = 0.0f;
    endorphinsLevel_ = 0.0f;
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