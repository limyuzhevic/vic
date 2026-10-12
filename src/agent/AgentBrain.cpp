#include "AgentBrain.hpp"
#include "AgentBrainConstants.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
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
    , plasticityModifier_(AgentBrainConstants::DEFAULT_PLASTICITY_MODIFIER)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(AgentBrainConstants::NOVELTY_DECAY_RATE)
{
    // Initialize neuron groups using balanced distribution
    initializeNeuronGroups();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeNeuronGroups() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize neuron groups: brain pointer is null");
        return;
    }
    
    try {
        // Create mapping from action group indices to their descriptions
        // This makes the distribution more predictable and maintainable
        std::vector<std::reference_wrapper<std::vector<Neuron*>>> motorGroups = {
            std::ref(motorForward_),
            std::ref(motorBackward_),
            std::ref(motorTurnLeft_),
            std::ref(motorTurnRight_),
            std::ref(motorInteract_),
            std::ref(motorWait_)
        };
        
        std::vector<std::reference_wrapper<std::vector<Neuron*>>> sensoryGroups = {
            std::ref(sensoryVision_),
            std::ref(sensoryTouch_),
            std::ref(sensoryInternal_),
            std::ref(sensoryProprioception_)
        };
        
        // Distribute motor neurons using round-robin allocation for better balance
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() != NeuronType::Motor && 
                    pop->getNeuronType() != NeuronType::Sensory) {
                    continue;  // Skip non-motor/sensory neurons
                }
                
                const bool isMotor = (pop->getNeuronType() == NeuronType::Motor);
                auto& groups = isMotor ? motorGroups : sensoryGroups;
                
                for (Neuron* n : pop->getNeurons()) {
                    if (!n) {
                        NLM_LOG_WARNING("Skipping null neuron in population");
                        continue;
                    }
                    
                    // Find the group with the fewest neurons for balanced distribution
                    size_t minGroupIndex = 0;
                    size_t minSize = groups[0].get().size();
                    
                    for (size_t i = 1; i < groups.size(); ++i) {
                        if (groups[i].get().size() < minSize) {
                            minSize = groups[i].get().size();
                            minGroupIndex = i;
                        }
                    }
                    
                    // Add neuron to the least populated group
                    groups[minGroupIndex].get().push_back(n);
                    
                    // Log assignment for debugging
                    NLM_LOG_DEBUG("Assigned neuron to group " + std::to_string(minGroupIndex) + 
                               " (" + (isMotor ? "motor" : "sensory") + ")");
                }
            }
        }
        
        NLM_LOG_INFO("Neuron distribution completed:");
        NLM_LOG_INFO("  Motor groups: Forward=" + std::to_string(motorForward_.size()) +
                     " Backward=" + std::to_string(motorBackward_.size()) +
                     " Left=" + std::to_string(motorTurnLeft_.size()) +
                     " Right=" + std::to_string(motorTurnRight_.size()) +
                     " Interact=" + std::to_string(motorInteract_.size()) +
                     " Wait=" + std::to_string(motorWait_.size()));
        NLM_LOG_INFO("  Sensory groups: Vision=" + std::to_string(sensoryVision_.size()) +
                     " Touch=" + std::to_string(sensoryTouch_.size()) +
                     " Internal=" + std::to_string(sensoryInternal_.size()) +
                     " Proprioception=" + std::to_string(sensoryProprioception_.size()));
                     
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in initializeNeuronGroups: " + std::string(e.what()));
        throw;
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!world) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain: world pointer is null");
        return;
    }
    
    try {
        previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
        developmentalAge_ = 0.0;
        plasticityModifier_ = AgentBrainConstants::DEFAULT_PLASTICITY_MODIFIER;
        
        NLM_LOG_INFO("AgentBrain initialized with " + 
                     std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                     std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                     std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
                     
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in initialize: " + std::string(e.what()));
        throw;
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return AgentBrainConstants::EXPECTED_SENSORY_INPUT_SIZE;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return AgentBrainConstants::EXPECTED_MOTOR_OUTPUT_SIZE;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot process sensory input: brain pointer is null");
        return;
    }
    
    try {
        if (!&percept) {
            NLM_LOG_ERROR("Cannot process null sensory percept");
            return;
        }
        
        // Vision input (256 values -> sensoryVision_ neurons)
        const auto& vision = percept.getVision();
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                // Inject current proportional to vision intensity
                float current = vision[i] * AgentBrainConstants::VISION_CURRENT_SCALE;  // Scale factor
                sensoryVision_[i]->injectCurrent(current);
            } else {
                NLM_LOG_DEBUG("Skipping null vision neuron at index " + std::to_string(i));
            }
        }
        
        // Touch input (8 values -> sensoryTouch_ neurons)
        const auto& touch = percept.getTouch();
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * AgentBrainConstants::TOUCH_CURRENT_SCALE;  // Collision signal
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
        
        // Internal signals (4 values -> sensoryInternal_ neurons)
        const auto& intern = percept.getInternal();
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * AgentBrainConstants::INTERNAL_CURRENT_SCALE;  // Center and scale
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
        
        // Proprioception (6 values -> sensoryProprioception_ neurons)
        const auto& proprio = percept.getProprioception();
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * AgentBrainConstants::PROPRIOCEPTIVE_CURRENT_SCALE;  // Center and scale
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
            curiosityLevel_ = std::clamp(curiosityLevel_, AgentBrainConstants::MIN_CURIOSITY_LEVEL, AgentBrainConstants::MAX_CURIOSITY_LEVEL);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in processSensoryInput: " + std::string(e.what()));
        throw;
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot decode motor command: brain pointer is null");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > AgentBrainConstants::CURIOSITY_THRESHOLD) {
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
            if (!n) continue;
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return neurons.empty() ? 0.0f : sum / neurons.size();
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
        float exploreChance = curiosityLevel_ * AgentBrainConstants::EXPLORATION_MAX_CHANCE;  // Up to 30% random
        
        if (brain_) {
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command
                int choice = brain_->getRandomGenerator()->uniformInt(0, AgentBrainConstants::MAX_RANDOM_EXPLORATION_CHOICES - 1);
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
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        if (!brain_) {
            NLM_LOG_WARNING("Cannot apply reward modulation: brain pointer is null");
        }
        return;
    }
    
    try {
        // Compute prediction error
        predictionError_ = reward - predictedReward;
        
        // Update expected reward (exponential moving average)
        expectedReward_ = AgentBrainConstants::REWARD_EXPECTATION_DECAY * expectedReward_ + 
                         AgentBrainConstants::REWARD_EXPECTATION_UPDATE * reward;
        
        // Dopamine-like signal (based on prediction error)
        dopamineLevel_ = predictionError_;
        
        // Clamp to reasonable range
        dopamineLevel_ = std::clamp(dopamineLevel_, AgentBrainConstants::MIN_NEUROMODULATION_LEVEL, 
                                   AgentBrainConstants::MAX_NEUROMODULATION_LEVEL);
        
        // Apply to all synapses with eligibility traces
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                if (!syn) continue;
                
                float eligibility = syn->getEligibilityTrace();
                
                if (std::abs(eligibility) > AgentBrainConstants::ELIGIBILITY_TRACE_THRESHOLD) {
                    // Apply reward-modulated weight change
                    float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                    syn->addToWeight(delta);
                    
                    // Decay eligibility trace
                    syn->decayEligibilityTrace(AgentBrainConstants::ELIGIBILITY_TRACE_DECAY);
                }
            }
        }
        
        // Modulate plasticity based on dopamine
        // Positive dopamine increases plasticity, negative decreases
        float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
        plasticityFactor = std::clamp(plasticityFactor, AgentBrainConstants::MIN_PLASTICITY_FACTOR, 
                                     AgentBrainConstants::MAX_PLASTICITY_FACTOR);
        
        // Apply to STDP
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(AgentBrainConstants::STDP_LTP_WEIGHT_BASE * plasticityFactor);
            stdp->setLTDWeight(AgentBrainConstants::STDP_LTD_WEIGHT_BASE * plasticityFactor);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in applyRewardModulation: " + std::string(e.what()));
        throw;
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        if (!brain_) {
            NLM_LOG_WARNING("Cannot update development: brain pointer is null");
        }
        return;
    }
    
    try {
        developmentalAge_ += timestep;
        
        // Simple developmental stages based on age
        // This is a biologically inspired approximation
        if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_INITIAL_DURATION) {  // ~1 minute
            plasticityModifier_ = 1.0f;  // High plasticity
            brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        } else if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_CRITICAL_PERIOD_DURATION) {  // ~5 minutes
            plasticityModifier_ = 0.8f;
            brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_MATURATION_DURATION) {  // ~15 minutes
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
                float synRate = AgentBrainConstants::SYNAPTOGENESIS_BASE_RATE * plasticityModifier_;
                float pruneRate = AgentBrainConstants::PRUNING_BASE_RATE * (2.0f - plasticityModifier_);
                sp->setSynaptogenesisRate(synRate);
                sp->setPruningRate(pruneRate);
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in updateDevelopment: " + std::string(e.what()));
        throw;
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
    plasticityModifier_ = AgentBrainConstants::DEFAULT_PLASTICITY_MODIFIER;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
