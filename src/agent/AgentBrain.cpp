#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
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
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , activityThreshold_(Config::ACTIVITY_THRESHOLD)
    , explorationWeight_(1.0f)
    , noveltyDecayFactor_(Config::NOVELTY_DECAY_FACTOR)
    , motorGroupCount_(Config::MAX_MOTOR_GROUPS)
    , sensoryGroupCount_(Config::MAX_SENSORY_GROUPS)
{
    // Initialize development thresholds for optimization
    precomputeActivityThresholds();
    
    // Initialize motor and sensory neuron groups
    distributeNeurons();
    
    NLM_LOG_INFO("AgentBrain initialized with improved memory management and error handling");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::precomputeActivityThresholds() {
    thresholds_.minActivityForAction = activityThreshold_;
    thresholds_.explorationChancePerUnit = Config::EXPLORATION_CHANCE_PER_CURIOSITY_UNIT;
}

void AgentBrain::distributeNeurons() {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: No brain available for neuron distribution");
        return;
    }
    
    // Clear existing groups
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
    
    // Count neurons by type for statistics
    size_t totalMotorNeurons = 0;
    size_t totalSensoryNeurons = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    auto weakPtr = std::shared_ptr<Neuron>(pop->getSharedNeuronPtr(n), n);
                    distributeMotorNeuron(weakPtr);
                    ++totalMotorNeurons;
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    auto weakPtr = std::shared_ptr<Neuron>(pop->getSharedNeuronPtr(n), n);
                    distributeSensoryNeuron(weakPtr);
                    ++totalSensoryNeurons;
                }
            }
        }
    }
    
    // Cache group counts for performance optimization
    motorGroupCount_ = Config::MAX_MOTOR_GROUPS;
    sensoryGroupCount_ = Config::MAX_SENSORY_GROUPS;
    
    NLM_LOG_INFO("AgentBrain neuron distribution complete:");
    NLM_LOG_INFO("  Motor neurons distributed into " + std::to_string(totalMotorNeurons) + " neurons across " + std::to_string(Config::MAX_MOTOR_GROUPS) + " groups");
    NLM_LOG_INFO("  Sensory neurons distributed into " + std::to_string(totalSensoryNeurons) + " neurons across " + std::to_string(Config::MAX_SENSORY_GROUPS) + " groups");
}

void AgentBrain::distributeMotorNeuron(const std::shared_ptr<Neuron>& neuronPtr) {
    if (!neuronPtr) return;
    
    // Convert to weak_ptr for memory safety
    std::weak_ptr<Neuron> weakPtr = neuronPtr;
    
    // Find first non-full group using round-robin approach
    static size_t groupIndex = 0;
    const size_t maxAttempts = 10;  // Prevent infinite loops
    
    for (size_t attempts = 0; attempts < maxAttempts; ++attempts) {
        switch (groupIndex % Config::MAX_MOTOR_GROUPS) {
            case 0: if (motorForward_.size() < Config::MAX_MOTOR_GROUPS) {
                       motorForward_.push_back(weakPtr); 
                       return; 
                   }
                   break;
            case 1: if (motorBackward_.size() < Config::MAX_MOTOR_GROUPS) {
                       motorBackward_.push_back(weakPtr); 
                       return; 
                   }
                   break;
            case 2: if (motorTurnLeft_.size() < Config::MAX_MOTOR_GROUPS) {
                       motorTurnLeft_.push_back(weakPtr); 
                       return; 
                   }
                   break;
            case 3: if (motorTurnRight_.size() < Config::MAX_MOTOR_GROUPS) {
                       motorTurnRight_.push_back(weakPtr); 
                       return; 
                   }
                   break;
            case 4: if (motorInteract_.size() < Config::MAX_MOTOR_GROUPS) {
                       motorInteract_.push_back(weakPtr); 
                       return; 
                   }
                   break;
            case 5: if (motorWait_.size() < Config::MAX_MOTOR_GROUPS) {
                       motorWait_.push_back(weakPtr); 
                       return; 
                   }
                   break;
        }
        groupIndex = (groupIndex + 1) % Config::MAX_MOTOR_GROUPS;
    }
}

void AgentBrain::distributeSensoryNeuron(const std::shared_ptr<Neuron>& neuronPtr) {
    if (!neuronPtr) return;
    
    // Convert to weak_ptr for memory safety
    std::weak_ptr<Neuron> weakPtr = neuronPtr;
    
    // Find first non-full group using round-robin approach
    static size_t groupIndex = 0;
    const size_t maxAttempts = 10;  // Prevent infinite loops
    
    for (size_t attempts = 0; attempts < maxAttempts; ++attempts) {
        switch (groupIndex % Config::MAX_SENSORY_GROUPS) {
            case 0: if (sensoryVision_.size() < Config::MAX_SENSORY_GROUPS) {
                       sensoryVision_.push_back(weakPtr); 
                       return; 
                   }
                   break;
            case 1: if (sensoryTouch_.size() < Config::MAX_SENSORY_GROUPS) {
                       sensoryTouch_.push_back(weakPtr); 
                       return; 
                   }
                   break;
            case 2: if (sensoryInternal_.size() < Config::MAX_SENSORY_GROUPS) {
                       sensoryInternal_.push_back(weakPtr); 
                       return; 
                   }
                   break;
            case 3: if (sensoryProprioception_.size() < Config::MAX_SENSORY_GROUPS) {
                       sensoryProprioception_.push_back(weakPtr); 
                       return; 
                   }
                   break;
        }
        groupIndex = (groupIndex + 1) % Config::MAX_SENSORY_GROUPS;
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(safeNeuronCount(sensoryVision_)) + " vision sensory neurons, " +
                 std::to_string(safeNeuronCount(sensoryTouch_)) + " touch sensory neurons, " +
                 std::to_string(safeNeuronCount(sensoryInternal_)) + " internal sensory neurons");
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::processSensoryInput: No brain available");
        return;
    }
    
    // Single-pass processing for better performance
    const auto& vision = percept.getVision();
    computeNovelty(vision);
    
    // Process all sensory modalities in parallel-friendly manner
    processVisionInput(vision, Config::VISION_SCALE_FACTOR);
    processTouchInput(percept.getTouch(), Config::TOUCH_SCALE_FACTOR);
    processInternalInput(percept.getInternal(), Config::INTERNAL_SCALE_FACTOR);
    processProprioceptionInput(percept.getProprioception(), Config::PROPRIOCEPTION_SCALE_FACTOR);
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

void AgentBrain::processVisionInput(const std::vector<float>& vision, float scaleFactor) {
    // Use single pass with cached values
    for (size_t i = 0; i < std::min(vision.size(), sensoryVision_.size()); ++i) {
        auto neuron = safeLock(sensoryVision_[i]);
        if (neuron) {
            neuron->injectCurrent(vision[i] * scaleFactor);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch, float scaleFactor) {
    for (size_t i = 0; i < std::min(touch.size(), sensoryTouch_.size()); ++i) {
        auto neuron = safeLock(sensoryTouch_[i]);
        if (neuron) {
            neuron->injectCurrent(touch[i] * scaleFactor);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal, float scaleFactor) {
    for (size_t i = 0; i < std::min(internal.size(), sensoryInternal_.size()); ++i) {
        auto neuron = safeLock(sensoryInternal_[i]);
        if (neuron) {
            float current = (internal[i] * 2.0f - 1.0f) * scaleFactor;
            neuron->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio, float scaleFactor) {
    for (size_t i = 0; i < std::min(proprio.size(), sensoryProprioception_.size()); ++i) {
        auto neuron = safeLock(sensoryProprioception_[i]);
        if (neuron) {
            float current = (proprio[i] * 2.0f - 1.0f) * scaleFactor;
            neuron->injectCurrent(current);
        }
    }
}

void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        
        // Single pass with bounds checking
        size_t limit = std::min(vision.size(), previousVision_.size());
        for (size_t i = 0; i < limit; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize and apply decay
        noveltyLevel_ = (totalDiff / std::max<size_t>(vision.size(), 1)) * noveltyDecayFactor_;
        
        // Store for next comparison
        previousVision_ = vision;
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

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > activityThreshold_) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate activity for each motor group in a single pass
    auto groupActivations = calculateMotorGroupActivations();
    
    // Find maximum activity using single pass
    MotorCommand bestCommand = MotorCommand::Wait;
    float bestActivity = groupActivations.wait;
    
    if (groupActivations.forward > bestActivity) {
        bestActivity = groupActivations.forward;
        bestCommand = MotorCommand::MoveForward;
    }
    if (groupActivations.backward > bestActivity) {
        bestActivity = groupActivations.backward;
        bestCommand = MotorCommand::MoveBackward;
    }
    if (groupActivations.left > bestActivity) {
        bestActivity = groupActivations.left;
        bestCommand = MotorCommand::TurnLeft;
    }
    if (groupActivations.right > bestActivity) {
        bestActivity = groupActivations.right;
        bestCommand = MotorCommand::TurnRight;
    }
    if (groupActivations.interact > bestActivity) {
        bestActivity = groupActivations.interact;
        bestCommand = MotorCommand::Interact;
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < thresholds_.minActivityForAction) {
        return MotorCommand::Wait;
    }
    
    return bestCommand;
}

struct MotorGroupActivations {
    float forward = 0.0f;
    float backward = 0.0f;
    float left = 0.0f;
    float right = 0.0f;
    float interact = 0.0f;
    float wait = 0.0f;
};

MotorGroupActivations AgentBrain::calculateMotorGroupActivations() const {
    MotorGroupActivations activations;
    
    // Calculate average activity in each motor group
    auto calculateGroupActivity = [this](const std::vector<std::weak_ptr<Neuron>>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        float sum = 0.0f;
        size_t validNeurons = 0;
        
        for (const auto& weakPtr : neurons) {
            auto neuron = safeLock(weakPtr);
            if (neuron) {
                // Use membrane potential deviation from rest as activity measure
                float potential = neuron->getState().membranePotential;
                float resting = neuron->getState().restingPotential;
                sum += std::abs(potential - resting);
                ++validNeurons;
            }
        }
        
        return validNeurons > 0 ? sum / validNeurons : 0.0f;
    };
    
    activations.forward = calculateGroupActivity(motorForward_);
    activations.backward = calculateGroupActivity(motorBackward_);
    activations.left = calculateGroupActivity(motorTurnLeft_);
    activations.right = calculateGroupActivity(motorTurnRight_);
    activations.interact = calculateGroupActivity(motorInteract_);
    activations.wait = calculateGroupActivity(motorWait_);
    
    return activations;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > activityThreshold_ * explorationWeight_) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * thresholds_.explorationChancePerUnit;
        
        // Ensure reasonable exploration probability
        exploreChance = std::clamp(exploreChance, 0.0f, 1.0f);
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command from available actions
            std::vector<MotorCommand> possibleCommands = {
                MotorCommand::MoveForward,
                MotorCommand::MoveBackward,
                MotorCommand::TurnLeft,
                MotorCommand::TurnRight,
                MotorCommand::LookLeft,
                MotorCommand::LookRight,
                MotorCommand::Interact,
                MotorCommand::Wait
            };
            
            // Remove duplicate Wait command
            possibleCommands.erase(std::remove(possibleCommands.begin(), possibleCommands.end(), MotorCommand::Wait), possibleCommands.end());
            
            if (!possibleCommands.empty()) {
                size_t choice = brain_->getRandomGenerator()->uniformInt(0, possibleCommands.size() - 1);
                return possibleCommands[choice];
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error with bounds checking
    predictionError_ = std::clamp(reward - predictedReward, -1.0f, 1.0f);
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, Config::DOPAMINE_CLAMP_MIN, Config::DOPAMINE_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change with plasticity modifier
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
    plasticityFactor = std::clamp(plasticityFactor, Config::PLASTICITY_FACTOR_MIN, Config::PLASTICITY_FACTOR_MAX);
    
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
    
    // Update developmental stage based on age with configurable thresholds
    if (developmentalAge_ < Config::CRITICAL_PERIOD_START) {
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < Config::MATURATION_START) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < Config::ADULT_START) {
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
            // Synaptogenesis and pruning rates scale with development stage
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

size_t AgentBrain::safeNeuronCount(const std::vector<std::weak_ptr<Neuron>>& neurons) const {
    size_t count = 0;
    for (const auto& weakPtr : neurons) {
        if (!weakPtr.expired()) {
            ++count;
        }
    }
    return count;
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
