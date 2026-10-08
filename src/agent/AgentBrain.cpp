#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

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
    // Validate input
    if (!brain) {
        NLM_LOG_ERROR("AgentBrain constructor: brain pointer is null");
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    try {
        // Initialize motor and sensory neuron groups
        if (brain_) {
            for (const auto& region : brain_->getRegions()) {
                if (!region) {
                    NLM_LOG_WARNING("AgentBrain: Encountered null region in brain");
                    continue;
                }
                
                for (auto& pop : region->getPopulations()) {
                    if (!pop) {
                        NLM_LOG_WARNING("AgentBrain: Encountered null population in region");
                        continue;
                    }
                    
                    NeuronType type = pop->getNeuronType();
                    
                    if (type == NeuronType::Motor) {
                        auto neurons = pop->getNeurons();
                        for (Neuron* n : neurons) {
                            if (!n) {
                                NLM_LOG_WARNING("AgentBrain: Encountered null neuron in motor population");
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
                        auto neurons = pop->getNeurons();
                        for (Neuron* n : neurons) {
                            if (!n) {
                                NLM_LOG_WARNING("AgentBrain: Encountered null neuron in sensory population");
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
            
            NLM_LOG_INFO("AgentBrain initialized with " +
                         std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                         std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                         std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                         std::to_string(sensoryProprioception_.size()) + " proprioception sensory neurons");
        } else {
            NLM_LOG_WARNING("AgentBrain: Brain pointer became null after initialization");
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain constructor: Exception during initialization: " + std::string(e.what()));
        throw;
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!world.getWidth() || !world.getHeight()) {
        NLM_LOG_ERROR("AgentBrain::initialize: Invalid world dimensions");
        throw std::invalid_argument("World must have valid dimensions");
    }
    
    try {
        previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        NLM_LOG_INFO("AgentBrain initialized with " +
                     std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                     std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                     std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                     std::to_string(sensoryProprioception_.size()) + " proprioception sensory neurons");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain::initialize: Exception during initialization: " + std::string(e.what()));
        throw;
    }
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
        NLM_LOG_WARNING("AgentBrain::processSensoryInput: Brain pointer is null");
        return;
    }
    
    try {
        // Validate percept data
        if (!percept.getVision().empty() && percept.getVision().size() != 256) {
            NLM_LOG_WARNING("AgentBrain::processSensoryInput: Vision data size mismatch. Expected 256, got " + std::to_string(percept.getVision().size()));
        }
        
        if (!percept.getTouch().empty() && percept.getTouch().size() != 8) {
            NLM_LOG_WARNING("AgentBrain::processSensoryInput: Touch data size mismatch. Expected 8, got " + std::to_string(percept.getTouch().size()));
        }
        
        if (!percept.getInternal().empty() && percept.getInternal().size() != 4) {
            NLM_LOG_WARNING("AgentBrain::processSensoryInput: Internal data size mismatch. Expected 4, got " + std::to_string(percept.getInternal().size()));
        }
        
        if (!percept.getProprioception().empty() && percept.getProprioception().size() != 6) {
            NLM_LOG_WARNING("AgentBrain::processSensoryInput: Proprioception data size mismatch. Expected 6, got " + std::to_string(percept.getProprioception().size()));
        }
        
        // Vision input (256 values -> sensoryVision_ neurons)
        const auto& vision = percept.getVision();
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                // Inject current proportional to vision intensity
                float current = vision[i] * 5.0f;  // Scale factor
                sensoryVision_[i]->injectCurrent(current);
            } else {
                NLM_LOG_WARNING("AgentBrain::processSensoryInput: Null vision neuron at index " + std::to_string(i));
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain::processSensoryInput: Exception during processing: " + std::string(e.what()));
        throw;
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain::decodeMotorCommand: Brain pointer is null");
        return MotorCommand::Wait;
    }
    
    try {
        MotorCommand decoded = decodeFromMotorNeurons();
        
        // Apply curiosity-based exploration
        if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
            decoded = selectWithCuriosity(decoded);
        }
        
        return decoded;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain::decodeMotorCommand: Exception during decoding: " + std::string(e.what()));
        return MotorCommand::Wait;
    }
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    try {
        // Validate that we have motor neurons
        if (motorForward_.empty() && motorBackward_.empty() && motorTurnLeft_.empty() && 
            motorTurnRight_.empty() && motorInteract_.empty() && motorWait_.empty()) {
            NLM_LOG_WARNING("AgentBrain::decodeFromMotorNeurons: No motor neurons available");
            return MotorCommand::Wait;
        }
        
        // Calculate average activity in each motor group
        auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
            if (neurons.empty()) return 0.0f;
            
            float sum = 0.0f;
            for (Neuron* n : neurons) {
                if (!n) {
                    continue; // Skip null neurons
                }
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain::decodeFromMotorNeurons: Exception during decoding: " + std::string(e.what()));
        return MotorCommand::Wait;
    }
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    try {
        // Exploration: occasionally choose random action when curiosity is high
        if (curiosityEnabled_ && curiosityLevel_ > 0.5f) {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain::selectWithCuriosity: Exception during selection: " + std::string(e.what()));
        return defaultCmd;
    }
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        if (!brain_) {
            NLM_LOG_WARNING("AgentBrain::applyRewardModulation: Brain pointer is null");
        }
        return;
    }
    
    try {
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
                if (!syn) {
                    NLM_LOG_WARNING("AgentBrain::applyRewardModulation: Encountered null synapse");
                    continue;
                }
                
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain::applyRewardModulation: Exception during modulation: " + std::string(e.what()));
        throw;
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        if (!brain_) {
            NLM_LOG_WARNING("AgentBrain::updateDevelopment: Brain pointer is null");
        }
        return;
    }
    
    try {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain::updateDevelopment: Exception during update: " + std::string(e.what()));
        throw;
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain::getDevelopmentalStage: Brain pointer is null");
        return DevelopmentalStage::Initial;
    }
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
    try {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        // Clear previous vision
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
        
        // Clear neuron groups
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
        
        NLM_LOG_INFO("AgentBrain reset successfully");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain::reset: Exception during reset: " + std::string(e.what()));
        throw;
    }
}

} // namespace nlm

