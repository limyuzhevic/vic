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
    // Initialize motor and sensory neuron groups with enhanced distribution
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Enhanced motor neuron distribution with role-based assignment
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                     motorTurnLeft_.size() + motorTurnRight_.size() +
                                     motorInteract_.size() + motorWait_.size();
                        
                        // Smart distribution based on motor function
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;     // Navigate forward
                            case 1: motorBackward_.push_back(n); break;    // Navigate backward  
                            case 2: motorTurnLeft_.push_back(n); break;     // Rotate left
                            case 3: motorTurnRight_.push_back(n); break;   // Rotate right
                            case 4: motorInteract_.push_back(n); break;     // Interact with environment
                            case 5: motorWait_.push_back(n); break;         // Default - wait/idle
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Enhanced sensory neuron distribution with modality-specific grouping
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                     sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        // Sensory modality assignment
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;          // Visual processing
                            case 1: sensoryTouch_.push_back(n); break;          // Tactile/force sensing
                            case 2: sensoryInternal_.push_back(n); break;       // Internal state (homeostasis)
                            case 3: sensoryProprioception_.push_back(n); break; // Body position/velocity
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
            // Enhanced current injection with adaptive scaling
            float baseCurrent = vision[i] * 5.0f;  // Base scaling factor
            
            // Apply novelty-based amplification
            if (curiosityEnabled_) {
                float noveltyBoost = 1.0f + (noveltyLevel_ * 0.5f);  // Up to 50% boost
                baseCurrent *= noveltyBoost;
            }
            
            // Apply attention modulation based on prediction error
            if (predictionError_ != 0.0f) {
                float attentionGain = std::abs(predictionError_) * 0.3f + 1.0f;
                baseCurrent *= attentionGain;
            }
            
            sensoryVision_[i]->injectCurrent(baseCurrent);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            
            // Apply reward prediction error modulation
            if (predictionError_ > 0.0f) {
                current *= (1.0f + predictionError_ * 0.2f);  // Reward increases sensitivity
            } else if (predictionError_ < 0.0f) {
                current *= (1.0f - std::abs(predictionError_) * 0.1f);  // Punishment decreases sensitivity
            }
            
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            
            // Apply homeostatic regulation
            current *= plasticityModifier_;
            
            // Apply neuromodulation
            if (dopamineLevel_ > 0.0f) {
                current *= (1.0f + dopamineLevel_ * 0.2f);
            }
            
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            
            // Apply developmental modulation
            if (developmentalAge_ < 60.0) {
                // Early development - higher sensitivity
                current *= 1.5f;
            } else if (developmentalAge_ > 600.0) {
                // Adult - lower sensitivity, more stable
                current *= 0.7f;
            }
            
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute comprehensive novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        size_t validComparisons = 0;
        
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            
            // Apply attention-weighted novelty detection
            float attentionWeight = 1.0f;
            if (curiosityEnabled_) {
                attentionWeight += noveltyLevel_ * 0.5f;
            }
            if (predictionError_ != 0.0f) {
                attentionWeight += std::abs(predictionError_) * 0.3f;
            }
            
            totalDiff += diff * attentionWeight;
            validComparisons++;
        }
        
        // Normalize with enhanced calculation
        if (validComparisons > 0) {
            noveltyLevel_ = totalDiff / validComparisons;
        }
        
        // Multi-stage decay for realistic novelty attenuation
        noveltyLevel_ *= sensoryNoveltyDecay_;
        if (curiosityEnabled_ && developmentalAge_ > 120.0) {
            // Adult brains decay novelty faster
            noveltyLevel_ *= 0.95f;
        }
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity with enhanced integration
    if (curiosityEnabled_) {
        // Combine multiple sources of curiosity drive
        float noveltyCuriosity = noveltyLevel_ * 2.0f;
        float predictionCuriosity = std::abs(predictionError_) * 0.5f;
        
        // Add developmental modulation to curiosity
        float developmentalCuriosity = 1.0f;
        if (developmentalAge_ < 60.0) {
            developmentalCuriosity = 1.5f;  // High curiosity in early development
        } else if (developmentalAge_ > 600.0) {
            developmentalCuriosity = 0.7f;  // Lower curiosity in adult
        }
        
        curiosityLevel_ = (noveltyCuriosity + predictionCuriosity) * 0.5f * developmentalCuriosity;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        
        // Add exploration pressure for stable adult brains
        if (developmentalAge_ > 300.0 && curiosityLevel_ < 0.1f) {
            curiosityLevel_ += 0.05f;  // Minimum exploration drive
        }
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

    // Enhanced motor decoding with adaptive thresholds and multi-level decision making
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Enhanced activity measure with multiple factors
            float potential = n->getState().membranePotential;
            float rest = n->getState().restingPotential;
            
            // Calculate normalized activity level
            float activity = std::abs(potential - rest) / 50.0f;  // Normalize to 0-1 range
            
            // Apply refractory period modulation
            if (n->isRefractory()) {
                activity *= 0.3f;
            }
            
            // Apply neuromodulation influence
            if (n->getState().dopamineLevel > 0.0f) {
                activity *= (1.0f + n->getState().dopamineLevel * 0.2f);
            }
            
            sum += activity;
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Enhanced decision making with confidence weighting and temporal dynamics
    struct EnhancedCommand {
        MotorCommand cmd;
        float activity;
        float confidence;
        float timestamp;
        float explorationWeight;
    };
    
    EnhancedCommand commands[] = {
        {MotorCommand::MoveForward, forwardAct, forwardAct * 0.8f, 0.0, 1.0f},
        {MotorCommand::MoveBackward, backwardAct, backwardAct * 0.8f, 1.0f, 1.0f},
        {MotorCommand::TurnLeft, leftAct, leftAct * 0.8f, 2.0f, 1.0f},
        {MotorCommand::TurnRight, rightAct, rightAct * 0.8f, 3.0f, 1.0f},
        {MotorCommand::Interact, interactAct, interactAct * 0.9f, 4.0f, 1.2f},
        {MotorCommand::Wait, waitAct, waitAct * 0.7f, 5.0f, 0.5f}
    };
    
    // Apply curiosity-based exploration and temporal discounting
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        for (auto& cmd : commands) {
            // Increase exploration weight for commands with curiosity
            cmd.explorationWeight = 1.0f + curiosityLevel_ * 0.4f;
            
            // Apply temporal discounting for older commands
            cmd.confidence *= std::pow(0.95f, cmd.timestamp);
        }
    }
    
    // Find maximum activity with enhanced criteria
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    float bestConfidence = 0.0f;
    
    for (const auto& c : commands) {
        // Enhanced selection with confidence weighting
        float selectionScore = c.activity * c.confidence * c.explorationWeight;
        
        // Apply threshold gating
        if (selectionScore < 0.1f) continue;  // Ignore weak signals
        
        // Apply curiosity-based exploration bias
        if (curiosityEnabled_ && curiosityLevel_ > 0.5f && c.cmd != MotorCommand::Wait) {
            selectionScore *= (1.0f + curiosityLevel_ * 0.3f);
        }
        
        if (selectionScore > bestActivity) {
            bestActivity = selectionScore;
            best = c.cmd;
            bestConfidence = c.confidence;
        }
    }
    
    // Apply adaptive action threshold
    float adaptiveThreshold = 0.3f;
    if (developmentalAge_ < 60.0) {
        adaptiveThreshold = 0.5f;  // More exploratory in development
    } else if (developmentalAge_ > 300.0) {
        adaptiveThreshold = 0.2f;  // More conservative in adult
    }
    
    // Only act if there's meaningful activity above threshold
    if (bestActivity < adaptiveThreshold) {
        return MotorCommand::Wait;
    }
    
    return best;
}

    // Enhanced curiosity-based exploration with adaptive decision making
    MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
        // Exploration: occasionally choose random action when curiosity is high
        if (curiosityEnabled_ && curiosityLevel_ > 0.5f) {
            // Higher curiosity = more exploration
            float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
            
            // Enhanced randomization with neuromodulation influence
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Enhanced random motor command selection
                // Add options based on developmental stage and neuromodulation
                int choice = brain_->getRandomGenerator()->uniformInt(0, 9);  // Increased options
                switch (choice) {
                    case 0: return MotorCommand::MoveForward;
                    case 1: return MotorCommand::MoveBackward;
                    case 2: return MotorCommand::TurnLeft;
                    case 3: return MotorCommand::TurnRight;
                    case 4: return MotorCommand::LookLeft;
                    case 5: return MotorCommand::LookRight;
                    case 6: return MotorCommand::Interact;
                    case 7: return MotorCommand::Wait;
                    case 8: return MotorCommand::MoveForward;  // Repeat for higher probability
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
    
    // Update expected reward with adaptive learning rate
    float learningRate = 0.05f;
    if (developmentalAge_ < 60.0) {
        learningRate = 0.1f;  // Faster learning in early development
    } else if (developmentalAge_ > 300.0) {
        learningRate = 0.02f;  // Slower learning in adult
    }
    
    expectedReward_ = (1.0f - learningRate) * expectedReward_ + learningRate * reward;
    
    // Dopamine-like signal with enhanced dynamics
    dopamineLevel_ = predictionError_;
    
    // Apply neuromodulation dynamics
    if (dopamineLevel_ > 0.5f) {
        // Strong prediction error - induce plasticity
        dopamineLevel_ = 1.0f;
    } else if (dopamineLevel_ < -0.5f) {
        // Strong negative prediction error - suppress plasticity
        dopamineLevel_ = -1.0f;
    }
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with enhanced eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Enhanced reward-modulated weight change with plasticity factor
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                
                // Apply synaptic limits
                float newWeight = syn->getWeight() + delta;
                newWeight = std::clamp(newWeight, 0.0f, 1.0f);
                
                syn->addToWeight(newWeight - syn->getWeight());
                
                // Enhanced eligibility trace decay with neuromodulation
                float decayRate = 0.1f;
                if (dopamineLevel_ > 0.0f) {
                    decayRate *= (1.0f + dopamineLevel_ * 0.5f);  // Positive PE enhances learning
                }
                syn->decayEligibilityTrace(decayRate);
            }
        }
    }
    
    // Modulate plasticity based on dopamine with enhanced dynamics
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP with enhanced parameters
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        // Enhanced STDP parameter modulation
        float baseLTP = 0.01f;
        float baseLTD = 0.012f;
        
        float ltpWeight = baseLTP * plasticityFactor;
        float ltdWeight = baseLTD * plasticityFactor * 1.1f;  // LTD slightly stronger
        
        stdp->setLTPWeight(ltpWeight);
        stdp->setLTDWeight(ltdWeight);
        
        // Apply to other plasticity rules
        auto* hebbian = brain_->getHebbian();
        if (hebbian) {
            float hebbianFactor = plasticityFactor * 0.8f;  // Hebbian slightly less sensitive
            hebbian->setLearningRate(baseLTP * hebbianFactor);
        }
    }
    
    // Apply neuromodulation to structural plasticity
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Structural plasticity is more sensitive to neuromodulation
            float structuralFactor = 1.5f * plasticityFactor;
            
            // Higher synaptogenesis with positive dopamine
            float synRate = 0.0001f * structuralFactor;
            if (dopamineLevel_ > 0.0f) {
                synRate *= (1.0f + dopamineLevel_ * 0.5f);
            }
            
            // Lower pruning with positive dopamine
            float pruneRate = 0.00001f * (2.0f - plasticityFactor);
            if (dopamineLevel_ > 0.0f) {
                pruneRate *= 0.5f;  // Reduce pruning with positive PE
            }
            
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
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
