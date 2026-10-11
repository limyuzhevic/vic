#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <sstream>

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
    if (!brain_) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    try {
        // Initialize motor and sensory neuron groups with error checking
        initializeNeuronGroups();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize AgentBrain neuron groups: " + std::string(e.what()));
        throw;
    }
}

void AgentBrain::initializeNeuronGroups() {
    if (!brain_) return;
    
    try {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) {
                            NLM_LOG_WARNING("Null neuron encountered in motor population");
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
                            NLM_LOG_WARNING("Null neuron encountered in sensory population");
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
                     std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in initializeNeuronGroups: " + std::string(e.what()));
        throw;
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    try {
        previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        std::stringstream ss;
        ss << "AgentBrain initialized with " 
           << sensoryVision_.size() << " vision sensory neurons, "
           << sensoryTouch_.size() << " touch sensory neurons, "
           << sensoryInternal_.size() << " internal sensory neurons";
        NLM_LOG_INFO(ss.str());
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize AgentBrain: " + std::string(e.what()));
        throw;
    }
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot process sensory input: Brain not initialized");
        return;
    }
    
    try {
        // Validate input data
        const auto& vision = percept.getVision();
        const auto& touch = percept.getTouch();
        const auto& intern = percept.getInternal();
        const auto& proprio = percept.getProprioception();
        
        // Vision input (256 values -> sensoryVision_ neurons)
        processVisionInput(vision);
        
        // Touch input (8 values -> sensoryTouch_ neurons)
        processTouchInput(touch);
        
        // Internal signals (4 values -> sensoryInternal_ neurons)
        processInternalInput(intern);
        
        // Proprioception (6 values -> sensoryProprioception_ neurons)
        processProprioceptionInput(proprio);
        
        // Compute novelty (difference from previous vision)
        if (!vision.empty()) {
            float totalDiff = computeNovelty(vision);
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
        NLM_LOG_ERROR("Exception in processSensoryInput: " + std::string(e.what()));
        // Continue with default behavior rather than crashing
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
    }
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            try {
                // Inject current proportional to vision intensity
                float current = vision[i] * 5.0f;  // Scale factor
                sensoryVision_[i]->injectCurrent(current);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error injecting vision current into neuron: " + std::string(e.what()));
            }
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            try {
                float current = touch[i] * 8.0f;  // Collision signal
                sensoryTouch_[i]->injectCurrent(current);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error injecting touch current into neuron: " + std::string(e.what()));
            }
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& intern) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            try {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                sensoryInternal_[i]->injectCurrent(current);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error injecting internal current into neuron: " + std::string(e.what()));
            }
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            try {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                sensoryProprioception_[i]->injectCurrent(current);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error injecting proprioception current into neuron: " + std::string(e.what()));
            }
        }
    }
}

float AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (vision.empty() || previousVision_.empty()) {
        return 0.0f;
    }
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    return totalDiff / std::max<size_t>(vision.size(), 1);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot decode motor command: Brain not initialized");
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
        NLM_LOG_ERROR("Exception in decodeMotorCommand: " + std::string(e.what()));
        return MotorCommand::Wait;
    }
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    try {
        // Calculate average activity in each motor group
        auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
            if (neurons.empty()) return 0.0f;
            float sum = 0.0f;
            for (Neuron* n : neurons) {
                if (!n) {
                    continue;
                }
                // Use membrane potential deviation from rest as activity measure
                try {
                    sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error accessing neuron state: " + std::string(e.what()));
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
        if (bestActivity < 0.5f) {
            return MotorCommand::Wait;
        }
        
        return best;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in decodeFromMotorNeurons: " + std::string(e.what()));
        return MotorCommand::Wait;
    }
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    try {
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
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in selectWithCuriosity: " + std::string(e.what()));
        return defaultCmd;
    }
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    try {
        if (!brain_ || !rewardModulationEnabled_) {
            return;
        }
        
        // Validate input parameters
        if (std::isnan(reward) || std::isinf(reward)) {
            NLM_LOG_WARNING("Invalid reward value: " + std::to_string(reward));
            return;
        }
        
        if (std::isnan(predictedReward) || std::isinf(predictedReward)) {
            NLM_LOG_WARNING("Invalid predicted reward value: " + std::to_string(predictedReward));
            predictedReward = 0.0f;
        }
        
        // Compute prediction error
        predictionError_ = reward - predictedReward;
        
        // Update expected reward (exponential moving average)
        expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
        
        // Dopamine-like signal (based on prediction error)
        dopamineLevel_ = predictionError_;
        
        // Clamp to reasonable range
        dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
        
        // Apply to all synapses with eligibility traces
        applyRewardToSynapses();
        
        // Modulate plasticity based on dopamine
        modulatePlasticityWithDopamine();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in applyRewardModulation: " + std::string(e.what()));
        // Continue with default values rather than crashing
        predictionError_ = 0.0f;
        dopamineLevel_ = 0.0f;
    }
}

void AgentBrain::applyRewardToSynapses() {
    try {
        if (!brain_) return;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                if (!syn) {
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
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in applyRewardToSynapses: " + std::string(e.what()));
        throw;
    }
}

void AgentBrain::modulatePlasticityWithDopamine() {
    try {
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
        NLM_LOG_ERROR("Exception in modulatePlasticityWithDopamine: " + std::string(e.what()));
        throw;
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    try {
        if (!brain_ || !developmentEnabled_) {
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
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in updateDevelopment: " + std::string(e.what()));
        // Continue with default values rather than crashing
        plasticityModifier_ = 1.0f;
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        }
    }
}

void AgentBrain::reset() {
    try {
        previousVision_.clear();
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        // Reset neuromodulation levels
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        
        NLM_LOG_INFO("AgentBrain reset successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in AgentBrain::reset: " + std::string(e.what()));
        throw;
    }
}

} // namespace nlm
