#include "AgentConfig.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

MotorDecoder::MotorDecoder(const AgentConfig& config) : config_(config) {}

float MotorDecoder::calculateActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        // Use membrane potential deviation from rest as activity measure
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

MotorCommand MotorDecoder::decodeFromMotorNeurons(
    const std::vector<Neuron*>& forward,
    const std::vector<Neuron*>& backward,
    const std::vector<Neuron*>& left,
    const std::vector<Neuron*>& right,
    const std::vector<Neuron*>& interact,
    const std::vector<Neuron*>& wait
) const {
    float forwardAct = calculateActivity(forward);
    float backwardAct = calculateActivity(backward);
    float leftAct = calculateActivity(left);
    float rightAct = calculateActivity(right);
    float interactAct = calculateActivity(interact);
    float waitAct = calculateActivity(wait);
    
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
    float bestActivity = waitAct; // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < config_.motorActivityThreshold) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand MotorDecoder::selectWithCuriosity(MotorCommand defaultCmd, float curiosityLevel, 
                                              const class RandomGenerator* rng) const {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel > config_.explorationThreshold) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel * config_.explorationChance; // Up to 30% random
        
        if (rng && exploreChance > 0.0f) {
            float r = rng->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command
                int choice = rng->uniformInt(0, 8);
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

SensoryProcessor::SensoryProcessor(const AgentConfig& config) : config_(config) {}

void SensoryProcessor::processSensoryInput(
    const SensoryPercept& percept,
    const std::vector<Neuron*>& visionNeurons,
    const std::vector<Neuron*>& touchNeurons,
    const std::vector<Neuron*>& internalNeurons,
    const std::vector<Neuron*>& proprioceptionNeurons
) {
    // Vision input (scaled by config)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < visionNeurons.size() && i < vision.size(); ++i) {
        if (visionNeurons[i]) {
            float current = vision[i] * config_.visionSensoryScale;
            visionNeurons[i]->injectCurrent(current);
        }
    }
    
    // Touch input (scaled by config)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < touchNeurons.size() && i < touch.size(); ++i) {
        if (touchNeurons[i]) {
            float current = touch[i] * config_.touchSensoryScale;
            touchNeurons[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (centered and scaled by config)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < internalNeurons.size() && i < intern.size(); ++i) {
        if (internalNeurons[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * config_.internalSensoryScale;
            internalNeurons[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (centered and scaled by config)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < proprioceptionNeurons.size() && i < proprio.size(); ++i) {
        if (proprioceptionNeurons[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * config_.proprioceptionSensoryScale;
            proprioceptionNeurons[i]->injectCurrent(current);
        }
    }
}

float SensoryProcessor::computeNovelty(const std::vector<float>& currentVision) const {
    if (currentVision.empty()) return 0.0f;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < currentVision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(currentVision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize
    float novelty = totalDiff / std::max<size_t>(currentVision.size(), 1);
    
    // Apply decay
    novelty *= config_.noveltyDecayRate;
    
    // Store for next time
    previousVision_ = currentVision;
    
    return novelty;
}

void SensoryProcessor::initialize(int visionWidth, int visionHeight) {
    previousVision_.resize(visionWidth * visionHeight, 0.0f);
    NLM_LOG_INFO("SensoryProcessor initialized for vision size: " + 
                 std::to_string(visionWidth) + "x" + std::to_string(visionHeight));
}

void SensoryProcessor::reset() {
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

NeuromodulationManager::NeuromodulationManager(const AgentConfig& config) : config_(config) {}

void NeuromodulationManager::update(float reward, float predictedReward,
                                    float& dopamineLevel,
                                    float& predictionError,
                                    float& expectedReward) const {
    // Compute prediction error
    predictionError = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward = config_.rewardDecayRate * expectedReward + 
                    (1.0f - config_.rewardDecayRate) * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel = predictionError;
    
    // Clamp to reasonable range
    dopamineLevel = std::clamp(dopamineLevel, config_.dopamineClampMin, config_.dopamineClampMax);
}

void NeuromodulationManager::applyPlasticityChanges(
    const std::vector<Neuron*>& neurons,
    float dopamineLevel,
    float plasticityModifier,
    const class STDP* stdp
) const {
    // Apply to all synapses with eligibility traces
    // This is a placeholder - actual implementation would need synapse access
    NLM_LOG_INFO("NeuromodulationManager: Applying plasticity changes with dopamine: " + 
                 std::to_string(dopamineLevel) + ", plasticity modifier: " + 
                 std::to_string(plasticityModifier));
}

DevelopmentManager::DevelopmentManager(const AgentConfig& config) : config_(config) {}

void DevelopmentManager::updateDevelopment(double timestep, double& developmentalAge,
                                          float& plasticityModifier,
                                          DevelopmentalStage& currentStage) const {
    developmentalAge += timestep;
    
    // Determine developmental stage based on age
    if (developmentalAge < config_.developmentalStage1End) {
        plasticityModifier = config_.initialPlasticity;
        currentStage = DevelopmentalStage::Initial;
    } else if (developmentalAge < config_.developmentalStage2End) {
        plasticityModifier = config_.criticalPeriodPlasticity;
        currentStage = DevelopmentalStage::CriticalPeriod;
    } else if (developmentalAge < config_.developmentalStage3End) {
        plasticityModifier = config_.maturationPlasticity;
        currentStage = DevelopmentalStage::Maturation;
    } else {
        plasticityModifier = config_.adultPlasticity;
        currentStage = DevelopmentalStage::Adult;
    }
}

void DevelopmentManager::updateStructuralPlasticity(
    float plasticityModifier,
    float& synaptogenesisRate,
    float& pruningRate
) const {
    // Higher synaptogenesis in early development
    synaptogenesisRate = config_.earlySynaptogenesisRate * plasticityModifier;
    pruningRate = config_.earlyPruningRate * (2.0f - plasticityModifier);
}

DevelopmentalStage DevelopmentManager::getDevelopmentalStage(double age) const {
    if (age < config_.developmentalStage1End) {
        return DevelopmentalStage::Initial;
    } else if (age < config_.developmentalStage2End) {
        return DevelopmentalStage::CriticalPeriod;
    } else if (age < config_.developmentalStage3End) {
        return DevelopmentalStage::Maturation;
    } else {
        return DevelopmentalStage::Adult;
    }
}

float DevelopmentManager::getPlasticityModifier(double age) const {
    if (age < config_.developmentalStage1End) {
        return config_.initialPlasticity;
    } else if (age < config_.developmentalStage2End) {
        return config_.criticalPeriodPlasticity;
    } else if (age < config_.developmentalStage3End) {
        return config_.maturationPlasticity;
    } else {
        return config_.adultPlasticity;
    }
}

CuriosityManager::CuriosityManager(const AgentConfig& config) : config_(config) {}

float CuriosityManager::updateCuriosity(float novelty, float predictionError) const {
    float curiosity = novelty * config_.curiosityUpdateWeight + 
                     std::abs(predictionError) * config_.predictionErrorWeight;
    return std::clamp(curiosity, 0.0f, 1.0f);
}

bool CuriosityManager::shouldExplore(float curiosityLevel) const {
    return curiosityLevel > config_.curiosityThreshold;
}

float CuriosityManager::getExplorationProbability(float curiosityLevel) const {
    return curiosityLevel * config_.explorationChance;
}

} // namespace nlm
