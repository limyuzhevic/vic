#pragma once

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

// Constants for AgentBrain
namespace AgentBrainConstants {
    // Curiosity thresholds
    constexpr float CURIOSITY_EXPLORATION_THRESHOLD = 0.3f;
    constexpr float CURIOSITY_EXPLORATION_MAX_LEVEL = 0.5f;
    constexpr float NOISE_THRESHOLD = 0.5f;
    
    // Scale factors for sensory processing
    constexpr float VISION_SCALE_FACTOR = 5.0f;
    constexpr float TOUCH_SCALE_FACTOR = 8.0f;
    constexpr float INTERNAL_SCALE_FACTOR = 5.0f;
    constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;
    
    // Reward modulation constants
    constexpr float REWARD_DECAY_FACTOR = 0.95f;
    constexpr float REWARD_UPDATE_FACTOR = 0.05f;
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    
    // Plasticity factors
    constexpr float PLASTICITY_BASE_FACTOR = 0.5f;
    constexpr float PLASTICITY_CLAMP_MIN = 0.1f;
    constexpr float PLASTICITY_CLAMP_MAX = 2.0f;
    constexpr float STDP_BASE_WEIGHT_LTP = 0.01f;
    constexpr float STDP_BASE_WEIGHT_LTD = 0.012f;
    
    // Eligibility trace
    constexpr float ELIGIBILITY_THRESHOLD = 0.001f;
    constexpr float ELIGIBILITY_DECAY = 0.1f;
    
    // Motor neuron distribution
    constexpr size_t MOTOR_NEIGHBOR_GROUPS = 6;
    constexpr size_t SENSORY_NEIGHBOR_GROUPS = 4;
    
    // Development stages
    constexpr double INITIAL_STAGE_DURATION = 60.0;      // ~1 minute
    constexpr double CRITICAL_PERIOD_DURATION = 240.0;   // ~4 minutes
    constexpr double MATURATION_DURATION = 600.0;        // ~10 minutes
    
    // Structural plasticity rates
    constexpr float SYNAPTOGENESIS_BASE_RATE = 0.0001f;
    constexpr float PRUNING_BASE_RATE = 0.00001f;
    
    // Random choice range for curiosity
    constexpr int RANDOM_CHOICE_MIN = 0;
    constexpr int RANDOM_CHOICE_MAX = 7;  // 8 choices: MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait
}

class MotorNeuronDistributionHelper {
public:
    static void DistributeMotorNeurons(
        const std::vector<Neuron*>& neurons,
        std::vector<Neuron*>& motorForward,
        std::vector<Neuron*>& motorBackward,
        std::vector<Neuron*>& motorTurnLeft,
        std::vector<Neuron*>& motorTurnRight,
        std::vector<Neuron*>& motorInteract,
        std::vector<Neuron*>& motorWait) {
        
        size_t totalDistributed = 0;
        for (Neuron* n : neurons) {
            // Distribute motor neurons to different action groups
            size_t idx = totalDistributed % AgentBrainConstants::MOTOR_NEIGHBOR_GROUPS;
            
            switch (idx) {
                case 0: motorForward.push_back(n); break;
                case 1: motorBackward.push_back(n); break;
                case 2: motorTurnLeft.push_back(n); break;
                case 3: motorTurnRight.push_back(n); break;
                case 4: motorInteract.push_back(n); break;
                case 5: motorWait.push_back(n); break;
            }
            
            ++totalDistributed;
        }
    }
    
    static void DistributeSensoryNeurons(
        const std::vector<Neuron*>& neurons,
        std::vector<Neuron*>& sensoryVision,
        std::vector<Neuron*>& sensoryTouch,
        std::vector<Neuron*>& sensoryInternal,
        std::vector<Neuron*>& sensoryProprioception) {
        
        size_t totalDistributed = 0;
        for (Neuron* n : neurons) {
            // Distribute sensory neurons
            size_t idx = totalDistributed % AgentBrainConstants::SENSORY_NEIGHBOR_GROUPS;
            
            switch (idx) {
                case 0: sensoryVision.push_back(n); break;
                case 1: sensoryTouch.push_back(n); break;
                case 2: sensoryInternal.push_back(n); break;
                case 3: sensoryProprioception.push_back(n); break;
            }
            
            ++totalDistributed;
        }
    }
};

class SensoryProcessor {
public:
    static void ProcessVisionInput(
        const std::vector<float>& vision,
        const std::vector<Neuron*>& sensoryVisionNeurons,
        float visionScale = AgentBrainConstants::VISION_SCALE_FACTOR) {
        
        for (size_t i = 0; i < sensoryVisionNeurons.size() && i < vision.size(); ++i) {
            if (sensoryVisionNeurons[i]) {
                // Inject current proportional to vision intensity
                float current = vision[i] * visionScale;
                sensoryVisionNeurons[i]->injectCurrent(current);
            }
        }
    }
    
    static void ProcessTouchInput(
        const std::vector<float>& touch,
        const std::vector<Neuron*>& sensoryTouchNeurons,
        float touchScale = AgentBrainConstants::TOUCH_SCALE_FACTOR) {
        
        for (size_t i = 0; i < sensoryTouchNeurons.size() && i < touch.size(); ++i) {
            if (sensoryTouchNeurons[i]) {
                float current = touch[i] * touchScale;
                sensoryTouchNeurons[i]->injectCurrent(current);
            }
        }
    }
    
    static void ProcessInternalInput(
        const std::vector<float>& internal,
        const std::vector<Neuron*>& sensoryInternalNeurons,
        float internalScale = AgentBrainConstants::INTERNAL_SCALE_FACTOR) {
        
        for (size_t i = 0; i < sensoryInternalNeurons.size() && i < internal.size(); ++i) {
            if (sensoryInternalNeurons[i]) {
                // Center and scale: input is in [-1, 1], output in [-10, 10] mV
                float current = (internal[i] * 2.0f - 1.0f) * internalScale;
                sensoryInternalNeurons[i]->injectCurrent(current);
            }
        }
    }
    
    static void ProcessProprioceptionInput(
        const std::vector<float>& proprioception,
        const std::vector<Neuron*>& sensoryProprioceptionNeurons,
        float proprioceptionScale = AgentBrainConstants::PROPRIOCEPTION_SCALE_FACTOR) {
        
        for (size_t i = 0; i < sensoryProprioceptionNeurons.size() && i < proprioception.size(); ++i) {
            if (sensoryProprioceptionNeurons[i]) {
                // Center and scale
                float current = (proprioception[i] * 2.0f - 1.0f) * proprioceptionScale;
                sensoryProprioceptionNeurons[i]->injectCurrent(current);
            }
        }
    }
    
    static float CalculateNovelty(
        const std::vector<float>& currentVision,
        const std::vector<float>& previousVision) {
        
        if (currentVision.empty()) return 0.0f;
        
        float totalDiff = 0.0f;
        for (size_t i = 0; i < currentVision.size() && i < previousVision.size(); ++i) {
            float diff = std::abs(currentVision[i] - previousVision[i]);
            totalDiff += diff;
        }
        
        // Normalize by number of vision values
        float novelty = totalDiff / static_cast<float>(std::max(currentVision.size(), 1));
        
        return novelty;
    }
};

class NeuromodulationProcessor {
public:
    static void ApplyRewardModulation(
        std::vector<Neuron*>& sensoryVisionNeurons,
        std::vector<Neuron*>& sensoryTouchNeurons,
        std::vector<Neuron*>& sensoryInternalNeurons,
        std::vector<Neuron*>& sensoryProprioceptionNeurons,
        float reward,
        float predictedReward,
        float& predictionError,
        float& expectedReward,
        float plasticityModifier,
        std::shared_ptr<Brain> brain,
        bool rewardModulationEnabled) {
        
        if (!brain || !rewardModulationEnabled) return;
        
        // Compute prediction error
        predictionError = reward - predictedReward;
        
        // Update expected reward (exponential moving average)
        expectedReward = AgentBrainConstants::REWARD_DECAY_FACTOR * expectedReward +
                         AgentBrainConstants::REWARD_UPDATE_FACTOR * reward;
        
        // Dopamine-like signal (based on prediction error)
        float dopamineLevel = predictionError;
        
        // Clamp to reasonable range
        dopamineLevel = std::clamp(dopamineLevel,
                                   AgentBrainConstants::DOPAMINE_CLAMP_MIN,
                                   AgentBrainConstants::DOPAMINE_CLAMP_MAX);
        
        // Apply to all synapses with eligibility traces
        for (const auto& region : brain->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                float eligibility = syn->getEligibilityTrace();
                
                if (std::abs(eligibility) > AgentBrainConstants::ELIGIBILITY_THRESHOLD) {
                    // Apply reward-modulated weight change
                    float delta = eligibility * dopamineLevel * plasticityModifier;
                    syn->addToWeight(delta);
                    
                    // Decay eligibility trace
                    syn->decayEligibilityTrace(AgentBrainConstants::ELIGIBILITY_DECAY);
                }
            }
        }
        
        // Modulate plasticity based on dopamine
        // Positive dopamine increases plasticity, negative decreases
        float plasticityFactor = AgentBrainConstants::PLASTICITY_BASE_FACTOR +
                                 AgentBrainConstants::PLASTICITY_BASE_FACTOR * dopamineLevel;
        plasticityFactor = std::clamp(plasticityFactor,
                                      AgentBrainConstants::PLASTICITY_CLAMP_MIN,
                                      AgentBrainConstants::PLASTICITY_CLAMP_MAX);
        
        // Apply to STDP
        auto* stdp = brain->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(AgentBrainConstants::STDP_BASE_WEIGHT_LTP * plasticityFactor);
            stdp->setLTDWeight(AgentBrainConstants::STDP_BASE_WEIGHT_LTD * plasticityFactor);
        }
    }
};

class DevelopmentalProcessor {
public:
    static void UpdateDevelopmentalStage(
        double& developmentalAge,
        float& plasticityModifier,
        DevelopmentalStage developmentalStage,
        std::shared_ptr<Brain> brain,
        bool developmentEnabled,
        float structuralPlasticityEnabled,
        double timestep) {
        
        if (!brain || !developmentEnabled) return;
        
        developmentalAge += timestep;
        
        // Update developmental stage based on age
        DevelopmentalStage newStage;
        float newPlasticityModifier;
        
        if (developmentalAge < AgentBrainConstants::INITIAL_STAGE_DURATION) {
            // ~1 minute
            newStage = DevelopmentalStage::Initial;
            newPlasticityModifier = 1.0f;  // High plasticity
        } else if (developmentalAge < AgentBrainConstants::CRITICAL_PERIOD_DURATION) {
            // ~4 minutes  
            newStage = DevelopmentalStage::CriticalPeriod;
            newPlasticityModifier = 0.8f;
        } else if (developmentalAge < AgentBrainConstants::MATURATION_DURATION) {
            // ~10 minutes
            newStage = DevelopmentalStage::Maturation;
            newPlasticityModifier = 0.5f;
        } else {
            newStage = DevelopmentalStage::Adult;
            newPlasticityModifier = 0.2f;  // Adult - more stable
        }
        
        plasticityModifier = newPlasticityModifier;
        brain->setDevelopmentalStage(newStage);
        
        // Structural plasticity changes with development
        if (structuralPlasticityEnabled) {
            auto* sp = brain->getStructuralPlasticity();
            if (sp) {
                // Higher synaptogenesis in early development
                float synRate = AgentBrainConstants::SYNAPTOGENESIS_BASE_RATE * plasticityModifier;
                float pruneRate = AgentBrainConstants::PRUNING_BASE_RATE * (2.0f - plasticityModifier);
                sp->setSynaptogenesisRate(synRate);
                sp->setPruningRate(pruneRate);
            }
        }
    }
};

class MotorCommandProcessor {
public:
    static MotorCommand DecodeFromMotorNeurons(
        const std::vector<Neuron*>& motorForward,
        const std::vector<Neuron*>& motorBackward,
        const std::vector<Neuron*>& motorTurnLeft,
        const std::vector<Neuron*>& motorTurnRight,
        const std::vector<Neuron*>& motorInteract,
        const std::vector<Neuron*>& motorWait) {
        
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
        
        float forwardAct = calcActivity(motorForward);
        float backwardAct = calcActivity(motorBackward);
        float leftAct = calcActivity(motorTurnLeft);
        float rightAct = calcActivity(motorTurnRight);
        float interactAct = calcActivity(motorInteract);
        float waitAct = calcActivity(motorWait);
        
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
        if (bestActivity < AgentBrainConstants::NOISE_THRESHOLD) {
            return MotorCommand::Wait;
        }
        
        return best;
    }
    
    static MotorCommand SelectWithCuriosity(
        MotorCommand defaultCmd,
        float curiosityLevel,
        float explorationChance,
        std::shared_ptr<Brain> brain) {
        
        if (curiosityLevel <= AgentBrainConstants::CURIOSITY_EXPLORATION_MAX_LEVEL) {
            return defaultCmd;
        }
        
        // Higher curiosity = more exploration
        float actualExploreChance = curiosityLevel * explorationChance;  // Up to 30% random
        
        float r = brain->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < actualExploreChance) {
            // Random motor command
            int choice = brain->getRandomGenerator()->uniformInt(
                AgentBrainConstants::RANDOM_CHOICE_MIN,
                AgentBrainConstants::RANDOM_CHOICE_MAX);
                
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
        
        return defaultCmd;
    }
};

// AgentBrain implementation
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
                    MotorNeuronDistributionHelper::DistributeMotorNeurons(
                        pop->getNeurons(),
                        motorForward_,
                        motorBackward_,
                        motorTurnLeft_,
                        motorTurnRight_,
                        motorInteract_,
                        motorWait_);
                } else if (type == NeuronType::Sensory) {
                    MotorNeuronDistributionHelper::DistributeSensoryNeurons(
                        pop->getNeurons(),
                        sensoryVision_,
                        sensoryTouch_,
                        sensoryInternal_,
                        sensoryProprioception_);
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
    
    // Process each sensory modality using the helper class
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& intern = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Process sensory inputs
    SensoryProcessor::ProcessVisionInput(vision, sensoryVision_);
    SensoryProcessor::ProcessTouchInput(touch, sensoryTouch_);
    SensoryProcessor::ProcessInternalInput(intern, sensoryInternal_);
    SensoryProcessor::ProcessProprioceptionInput(proprio, sensoryProprioception_);
    
    // Compute novelty using helper class
    if (!vision.empty()) {
        noveltyLevel_ = SensoryProcessor::CalculateNovelty(vision, previousVision_);
        
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
    
    MotorCommand decoded = MotorCommandProcessor::DecodeFromMotorNeurons(
        motorForward_, motorBackward_, motorTurnLeft_, motorTurnRight_,
        motorInteract_, motorWait_);
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > AgentBrainConstants::CURIOSITY_EXPLORATION_THRESHOLD) {
        decoded = MotorCommandProcessor::SelectWithCuriosity(
            decoded, curiosityLevel_, 0.3f, brain_);
    }
    
    return decoded;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    NeuromodulationProcessor::ApplyRewardModulation(
        sensoryVision_, sensoryTouch_, sensoryInternal_, sensoryProprioception_,
        reward, predictedReward, predictionError_, expectedReward_,
        plasticityModifier_, brain_, rewardModulationEnabled_);
}

void AgentBrain::updateDevelopment(double timestep) {
    DevelopmentalProcessor::UpdateDevelopmentalStage(
        developmentalAge_, plasticityModifier_, getDevelopmentalStage(),
        brain_, developmentEnabled_, structuralPlasticityEnabled_, timestep);
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