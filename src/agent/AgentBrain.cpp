#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

// Helper function to check if a neuron is valid and safe to use
static bool isNeuronValid(Neuron* n) {
    return n != nullptr && n->getState().membranePotential >= 0.0f;
}

// Helper function to get max value from a vector safely
static size_t getMaxFromVector(const std::vector<Neuron*>& neurons) {
    return neurons.empty() ? 0 : neurons.size();
}

// Helper function to calculate average activity safely
static float calcActivitySafe(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        if (!isNeuronValid(n)) continue;
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return neurons.empty() ? 0.0f : sum / neurons.size();
}

// Constructor implementation
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
    try {
        // Initialize motor and sensory neuron groups safely
        if (!brain_) {
            NLM_LOG_WARNING("AgentBrain initialized with null brain pointer");
            return;
        }
        
        for (const auto& region : brain_->getRegions()) {
            if (!region) continue;
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue;
                
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!isNeuronValid(n)) continue;
                        
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {  // 6 motor actions
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
                        if (!isNeuronValid(n)) continue;
                        
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {  // 4 sensory modalities
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
                     std::to_string(motorForward_.size() + motorBackward_.size() + 
                                   motorTurnLeft_.size() + motorTurnRight_.size() +
                                   motorInteract_.size() + motorWait_.size()) + " motor neurons, " +
                     std::to_string(sensoryVision_.size() + sensoryTouch_.size() +
                                   sensoryInternal_.size() + sensoryProprioception_.size()) + " sensory neurons");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error initializing AgentBrain: " + std::string(e.what()));
        // Continue with partial initialization
    } catch (...) {
        NLM_LOG_ERROR("Unknown error initializing AgentBrain");
        // Continue with partial initialization
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    try {
        // Validate world input
        if (world.getVisionWidth() == 0 || world.getVisionHeight() == 0) {
            NLM_LOG_ERROR("Invalid world dimensions for initialization");
            return;
        }
        
        size_t visionSize = world.getVisionWidth() * world.getVisionHeight();
        previousVision_.resize(visionSize, 0.0f);
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        NLM_LOG_INFO("AgentBrain initialized with " + 
                     std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                     std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                     std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
                     
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error initializing AgentBrain: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error initializing AgentBrain");
    }
}

// Constants for magic numbers
size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    // These values should be configurable but are fixed for this architecture
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    // There are 6 basic motor actions plus LookLeft/LookRight for sensory panning
    return 8;  // MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    try {
        if (!brain_) {
            NLM_LOG_WARNING("Cannot process sensory input: brain is null");
            return;
        }
        
        // Validate percept
        if (!percept.isValid()) {
            NLM_LOG_WARNING("Invalid sensory percept received");
            return;
        }
        
        // Vision input (256 values -> sensoryVision_ neurons)
        const auto& vision = percept.getVision();
        for (size_t i = 0; i < std::min(sensoryVision_.size(), vision.size()); ++i) {
            Neuron* neuron = sensoryVision_[i];
            if (isNeuronValid(neuron)) {
                // Inject current proportional to vision intensity
                float current = vision[i] * 5.0f;  // Scale factor for vision sensitivity
                neuron->injectCurrent(current);
            }
        }
        
        // Touch input (8 values -> sensoryTouch_ neurons)
        const auto& touch = percept.getTouch();
        for (size_t i = 0; i < std::min(sensoryTouch_.size(), touch.size()); ++i) {
            Neuron* neuron = sensoryTouch_[i];
            if (isNeuronValid(neuron)) {
                float current = touch[i] * 8.0f;  // Collision signal scale
                neuron->injectCurrent(current);
            }
        }
        
        // Internal signals (4 values -> sensoryInternal_ neurons)
        const auto& intern = percept.getInternal();
        for (size_t i = 0; i < std::min(sensoryInternal_.size(), intern.size()); ++i) {
            Neuron* neuron = sensoryInternal_[i];
            if (isNeuronValid(neuron)) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                neuron->injectCurrent(current);
            }
        }
        
        // Proprioception (6 values -> sensoryProprioception_ neurons)
        const auto& proprio = percept.getProprioception();
        for (size_t i = 0; i < std::min(sensoryProprioception_.size(), proprio.size()); ++i) {
            Neuron* neuron = sensoryProprioception_[i];
            if (isNeuronValid(neuron)) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                neuron->injectCurrent(current);
            }
        }
        
        // Compute novelty (difference from previous vision)
        if (!vision.empty()) {
            float totalDiff = 0.0f;
            for (size_t i = 0; i < std::min(vision.size(), previousVision_.size()); ++i) {
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
        NLM_LOG_ERROR("Error processing sensory input: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error processing sensory input");
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    try {
        if (!brain_) {
            NLM_LOG_WARNING("Cannot decode motor command: brain is null");
            return MotorCommand::Wait;
        }
        
        MotorCommand decoded = decodeFromMotorNeurons();
        
        // Apply curiosity-based exploration
        if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
            decoded = selectWithCuriosity(decoded);
        }
        
        return decoded;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error decoding motor command: " + std::string(e.what()));
        return MotorCommand::Wait;
    } catch (...) {
        NLM_LOG_ERROR("Unknown error decoding motor command");
        return MotorCommand::Wait;
    }
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    try {
        // Calculate average activity in each motor group
        float forwardAct = calcActivitySafe(motorForward_);
        float backwardAct = calcActivitySafe(motorBackward_);
        float leftAct = calcActivitySafe(motorTurnLeft_);
        float rightAct = calcActivitySafe(motorTurnRight_);
        float interactAct = calcActivitySafe(motorInteract_);
        float waitAct = calcActivitySafe(motorWait_);
        
        // Find maximum activity
        struct { MotorCommand cmd; float activity; } commands[] = {
            {MotorCommand::MoveForward, forwardAct},
            {MotorCommand::MoveBackward, backwardAct},
            {MotorCommand::TurnLeft, leftAct},
            {MotorCommand::TurnRight, rightAct},
            {MotorCommand::LookLeft, leftAct * 0.5f},  // Use turn left activity as proxy for look left
            {MotorCommand::LookRight, rightAct * 0.5f},  // Use turn right activity as proxy for look right
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
        NLM_LOG_ERROR("Error decoding from motor neurons: " + std::string(e.what()));
        return MotorCommand::Wait;
    } catch (...) {
        NLM_LOG_ERROR("Unknown error decoding from motor neurons");
        return MotorCommand::Wait;
    }
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    try {
        // Exploration: occasionally choose random action when curiosity is high
        if (curiosityLevel_ > 0.5f) {
            // Higher curiosity = more exploration
            float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
            
            if (!brain_ || !brain_->getRandomGenerator()) {
                NLM_LOG_WARNING("Cannot explore: random generator not available");
                return defaultCmd;
            }
            
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command from valid range [0, 7]
                int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
                switch (choice) {
                    case 0: return MotorCommand::MoveForward;
                    case 1: return MotorCommand::MoveBackward;
                    case 2: return MotorCommand::TurnLeft;
                    case 3: return MotorCommand::TurnRight;
                    case 4: return MotorCommand::LookLeft;
                    case 5: return MotorCommand::LookRight;
                    case 6: return MotorCommand::Interact;
                    default: return MotorCommand::Wait;  // Safety fallback
                }
            }
        }
        
        return defaultCmd;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error selecting with curiosity: " + std::string(e.what()));
        return defaultCmd;
    } catch (...) {
        NLM_LOG_ERROR("Unknown error selecting with curiosity");
        return defaultCmd;
    }
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    try {
        if (!brain_ || !rewardModulationEnabled_) return;
        
        // Clamp inputs to valid ranges
        reward = std::clamp(reward, -100.0f, 100.0f);
        predictedReward = std::clamp(predictedReward, -100.0f, 100.0f);
        
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
            if (!region) continue;
            
            for (auto* syn : region->getSynapses()) {
                if (!syn) continue;  // Safety check
                
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
        NLM_LOG_ERROR("Error applying reward modulation: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error applying reward modulation");
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    try {
        if (!brain_ || !developmentEnabled_) return;
        
        // Validate timestep
        if (timestep <= 0.0) {
            NLM_LOG_WARNING("Invalid timestep for development update");
            return;
        }
        
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
                sp->setSynaptogenesisRate(std::max(0.0f, synRate));
                sp->setPruningRate(std::max(0.0f, pruneRate));
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error updating development: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error updating development");
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    try {
        if (!brain_) return DevelopmentalStage::Initial;
        return brain_->getDevelopmentalStage();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting developmental stage: " + std::string(e.what()));
        return DevelopmentalStage::Initial;
    } catch (...) {
        NLM_LOG_ERROR("Unknown error getting developmental stage");
        return DevelopmentalStage::Initial;
    }
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
        
        NLM_LOG_INFO("AgentBrain reset successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error resetting AgentBrain: " + std::string(e.what()));
    } catch (...) {
        NLM_LOG_ERROR("Unknown error resetting AgentBrain");
    }
}

} // namespace nlm