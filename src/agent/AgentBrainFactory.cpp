#include "AgentBrainFactory.hpp"
#include "AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <random>

namespace nlm {

struct AgentBrainFactory::Impl {
    std::mt19937 randomGenerator;
    double lastUpdateTime = 0.0;
};

AgentBrainFactory::AgentBrainFactory() : pImpl(std::make_unique<Impl>()) {
    std::random_device rd;
    pImpl->randomGenerator.seed(rd());
}

AgentBrainFactory::~AgentBrainFactory() = default;

void AgentBrainFactory::initializeNeuronGroups(
    std::vector<Neuron*>& motorForward,
    std::vector<Neuron*>& motorBackward,
    std::vector<Neuron*>& motorTurnLeft,
    std::vector<Neuron*>& motorTurnRight,
    std::vector<Neuron*>& motorInteract,
    std::vector<Neuron*>& motorWait,
    std::vector<Neuron*>& sensoryVision,
    std::vector<Neuron*>& sensoryTouch,
    std::vector<Neuron*>& sensoryInternal,
    std::vector<Neuron*>& sensoryProprioception,
    const Brain& brain
) {
    // Clear existing groups
    motorForward.clear();
    motorBackward.clear();
    motorTurnLeft.clear();
    motorTurnRight.clear();
    motorInteract.clear();
    motorWait.clear();
    sensoryVision.clear();
    sensoryTouch.clear();
    sensoryInternal.clear();
    sensoryProprioception.clear();
    
    // Get distribution configuration
    NeuronDistribution distribution = getNeuronDistribution(brain);
    
    // Get brain populations
    auto regions = brain.getRegions();
    if (regions.empty()) {
        NLM_LOG_WARNING("No brain regions found for AgentBrain initialization");
        return;
    }
    
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto populations = region->getPopulations();
        for (const auto& population : populations) {
            if (!population) continue;
            
            NeuronType type = population->getNeuronType();
            auto neurons = population->getNeurons();
            
            if (neurons.empty()) continue;
            
            // Distribute neurons based on type and distribution
            distributeNeurons(neurons, type, 
                            sensoryVision, sensoryTouch, sensoryInternal, sensoryProprioception,
                            motorForward, motorBackward, motorTurnLeft, motorTurnRight, motorInteract, motorWait,
                            distribution);
        }
    }
    
    // Validate initialization
    if (!validateInitialization(sensoryVision, sensoryTouch, sensoryInternal, sensoryProprioception,
                                {motorForward, motorBackward, motorTurnLeft, motorTurnRight, motorInteract, motorWait})) {
        NLM_LOG_WARNING("AgentBrain initialization validation failed");
    }
}

NeuronDistribution AgentBrainFactory::getNeuronDistribution(const Brain& brain) {
    NeuronDistribution distribution = config_.distributionWeights;
    
    // Auto-calculate distribution based on brain characteristics if weights not specified
    if (distribution.isValid()) {
        return distribution;
    }
    
    // Default balanced distribution
    distribution = NeuronDistribution();
    distribution.normalize();
    
    return distribution;
}

std::vector<size_t> AgentBrainFactory::calculateOptimalAllocation(
    size_t totalNeurons,
    const std::vector<float>& weights
) {
    if (totalNeurons == 0 || weights.empty()) {
        return std::vector<size_t>(weights.size(), 0);
    }
    
    std::vector<size_t> allocation(weights.size(), 0);
    
    // Calculate initial allocation based on weights
    double weightSum = std::accumulate(weights.begin(), weights.end(), 0.0f);
    if (weightSum == 0.0) {
        // Equal distribution if weights are zero
        size_t base = totalNeurons / weights.size();
        size_t remainder = totalNeurons % weights.size();
        
        for (size_t i = 0; i < weights.size(); ++i) {
            allocation[i] = base + (i < remainder ? 1 : 0);
        }
    } else {
        // Allocate based on weight proportions
        for (size_t i = 0; i < weights.size(); ++i) {
            double proportion = weights[i] / weightSum;
            allocation[i] = static_cast<size_t>(proportion * totalNeurons);
        }
        
        // Ensure we don't exceed total neurons
        size_t allocated = std::accumulate(allocation.begin(), allocation.end(), 0);
        if (allocated != totalNeurons) {
            // Adjust remaining neurons
            for (size_t i = 0; i < (totalNeurons - allocated); ++i) {
                allocation[i % weights.size()]++;
            }
        }
    }
    
    return allocation;
}

void AgentBrainFactory::createSensoryGroups(
    std::vector<Neuron*>& visionGroup,
    std::vector<Neuron*>& touchGroup,
    std::vector<Neuron*>& internalGroup,
    std::vector<Neuron*>& proprioceptionGroup,
    const Brain& brain
) {
    // Clear existing groups
    visionGroup.clear();
    touchGroup.clear();
    internalGroup.clear();
    proprioceptionGroup.clear();
    
    auto regions = brain.getRegions();
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto populations = region->getPopulations();
        for (const auto& population : populations) {
            if (!population) continue;
            
            NeuronType type = population->getNeuronType();
            auto neurons = population->getNeurons();
            
            switch (type) {
                case NeuronType::SensoryVision:
                    visionGroup.insert(visionGroup.end(), neurons.begin(), neurons.end());
                    break;
                case NeuronType::SensoryTouch:
                    touchGroup.insert(touchGroup.end(), neurons.begin(), neurons.end());
                    break;
                case NeuronType::SensoryInternal:
                    internalGroup.insert(internalGroup.end(), neurons.begin(), neurons.end());
                    break;
                case NeuronType::SensoryProprioception:
                    proprioceptionGroup.insert(proprioceptionGroup.end(), neurons.begin(), neurons.end());
                    break;
                default:
                    // Motor neurons and others are handled separately
                    break;
            }
        }
    }
}

void AgentBrainFactory::createMotorGroups(
    std::vector<Neuron*>& forwardGroup,
    std::vector<Neuron*>& backwardGroup,
    std::vector<Neuron*>& leftGroup,
    std::vector<Neuron*>& rightGroup,
    std::vector<Neuron*>& interactGroup,
    std::vector<Neuron*>& waitGroup,
    const Brain& brain
) {
    // Clear existing groups
    forwardGroup.clear();
    backwardGroup.clear();
    leftGroup.clear();
    rightGroup.clear();
    interactGroup.clear();
    waitGroup.clear();
    
    auto regions = brain.getRegions();
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto populations = region->getPopulations();
        for (const auto& population : populations) {
            if (!population) continue;
            
            NeuronType type = population->getNeuronType();
            auto neurons = population->getNeurons();
            
            switch (type) {
                case NeuronType::MotorForward:
                    forwardGroup.insert(forwardGroup.end(), neurons.begin(), neurons.end());
                    break;
                case NeuronType::MotorBackward:
                    backwardGroup.insert(backwardGroup.end(), neurons.begin(), neurons.end());
                    break;
                case NeuronType::MotorTurnLeft:
                    leftGroup.insert(leftGroup.end(), neurons.begin(), neurons.end());
                    break;
                case NeuronType::MotorTurnRight:
                    rightGroup.insert(rightGroup.end(), neurons.begin(), neurons.end());
                    break;
                case NeuronType::MotorInteract:
                    interactGroup.insert(interactGroup.end(), neurons.begin(), neurons.end());
                    break;
                case NeuronType::MotorWait:
                    waitGroup.insert(waitGroup.end(), neurons.begin(), neurons.end());
                    break;
                default:
                    // Skip non-motor neurons
                    break;
            }
        }
    }
}

bool AgentBrainFactory::validateInitialization(
    const std::vector<Neuron*>& visionGroup,
    const std::vector<Neuron*>& touchGroup,
    const std::vector<Neuron*>& internalGroup,
    const std::vector<Neuron*>& proprioceptionGroup,
    const std::vector<std::vector<Neuron*>>& motorGroups
) {
    bool valid = true;
    
    // Check that at least one sensory group has neurons
    if (visionGroup.empty() && touchGroup.empty() && internalGroup.empty() && proprioceptionGroup.empty()) {
        NLM_LOG_WARNING("No sensory neurons found in AgentBrain initialization");
        valid = false;
    }
    
    // Check that at least one motor group has neurons
    bool hasMotorNeurons = false;
    for (const auto& group : motorGroups) {
        if (!group.empty()) {
            hasMotorNeurons = true;
            break;
        }
    }
    
    if (!hasMotorNeurons) {
        NLM_LOG_WARNING("No motor neurons found in AgentBrain initialization");
        valid = false;
    }
    
    // Check that all neuron pointers are not null
    for (const auto& group : {visionGroup, touchGroup, internalGroup, proprioceptionGroup}) {
        for (const auto& neuron : group) {
            if (neuron == nullptr) {
                NLM_LOG_ERROR("Null neuron pointer in AgentBrain group");
                valid = false;
            }
        }
    }
    
    for (const auto& group : motorGroups) {
        for (const auto& neuron : group) {
            if (neuron == nullptr) {
                NLM_LOG_ERROR("Null neuron pointer in AgentBrain motor group");
                valid = false;
            }
        }
    }
    
    return valid;
}

void AgentBrainFactory::distributeNeurons(
    const std::vector<Neuron*>& neurons,
    NeuronType type,
    std::vector<Neuron*>& visionGroup,
    std::vector<Neuron*>& touchGroup,
    std::vector<Neuron*>& internalGroup,
    std::vector<Neuron*>& proprioceptionGroup,
    std::vector<Neuron*>& motorForward,
    std::vector<Neuron*>& motorBackward,
    std::vector<Neuron*>& motorTurnLeft,
    std::vector<Neuron*>& motorTurnRight,
    std::vector<Neuron*>& motorInteract,
    std::vector<Neuron*>& motorWait,
    const NeuronDistribution& distribution
) {
    if (neurons.empty()) return;
    
    switch (type) {
        case NeuronType::SensoryVision:
            if (config_.enableVisionNeurons) {
                visionGroup.insert(visionGroup.end(), neurons.begin(), neurons.end());
            }
            break;
        case NeuronType::SensoryTouch:
            if (config_.enableTouchNeurons) {
                touchGroup.insert(touchGroup.end(), neurons.begin(), neurons.end());
            }
            break;
        case NeuronType::SensoryInternal:
            if (config_.enableInternalNeurons) {
                internalGroup.insert(internalGroup.end(), neurons.begin(), neurons.end());
            }
            break;
        case NeuronType::SensoryProprioception:
            if (config_.enableProprioceptionNeurons) {
                proprioceptionGroup.insert(proprioceptionGroup.end(), neurons.begin(), neurons.end());
            }
            break;
        case NeuronType::MotorForward:
            if (config_.enableAllMotorNeurons) {
                motorForward.insert(motorForward.end(), neurons.begin(), neurons.end());
            }
            break;
        case NeuronType::MotorBackward:
            if (config_.enableAllMotorNeurons) {
                motorBackward.insert(motorBackward.end(), neurons.begin(), neurons.end());
            }
            break;
        case NeuronType::MotorTurnLeft:
            if (config_.enableAllMotorNeurons) {
                motorTurnLeft.insert(motorTurnLeft.end(), neurons.begin(), neurons.end());
            }
            break;
        case NeuronType::MotorTurnRight:
            if (config_.enableAllMotorNeurons) {
                motorTurnRight.insert(motorTurnRight.end(), neurons.begin(), neurons.end());
            }
            break;
        case NeuronType::MotorInteract:
            if (config_.enableAllMotorNeurons) {
                motorInteract.insert(motorInteract.end(), neurons.begin(), neurons.end());
            }
            break;
        case NeuronType::MotorWait:
            if (config_.enableAllMotorNeurons) {
                motorWait.insert(motorWait.end(), neurons.begin(), neurons.end());
            }
            break;
        default:
            // Unknown neuron type - ignore
            break;
    }
}

void NeuronDistribution::normalize() {
    float totalWeight = visionWeight + touchWeight + internalWeight + proprioceptionWeight +
                       motorForwardWeight + motorBackwardWeight + motorTurnLeftWeight + motorTurnRightWeight +
                       motorInteractWeight + motorWaitWeight;
    
    if (totalWeight > 0.0f) {
        visionWeight /= totalWeight;
        touchWeight /= totalWeight;
        internalWeight /= totalWeight;
        proprioceptionWeight /= totalWeight;
        motorForwardWeight /= totalWeight;
        motorBackwardWeight /= totalWeight;
        motorTurnLeftWeight /= totalWeight;
        motorTurnRightWeight /= totalWeight;
        motorInteractWeight /= totalWeight;
        motorWaitWeight /= totalWeight;
    }
}

bool NeuronDistribution::isValid() const {
    float totalWeight = visionWeight + touchWeight + internalWeight + proprioceptionWeight +
                       motorForwardWeight + motorBackwardWeight + motorTurnLeftWeight + motorTurnRightWeight +
                       motorInteractWeight + motorWaitWeight;
    
    return std::abs(totalWeight - 1.0f) < 0.01f &&
           visionWeight >= 0.0f && touchWeight >= 0.0f && internalWeight >= 0.0f &&
           proprioceptionWeight >= 0.0f && motorForwardWeight >= 0.0f &&
           motorBackwardWeight >= 0.0f && motorTurnLeftWeight >= 0.0f &&
           motorTurnRightWeight >= 0.0f && motorInteractWeight >= 0.0f &&
           motorWaitWeight >= 0.0f;
}

void AgentBrainConfig::updateFromBrain(const Brain& brain) {
    size_t totalNeurons = brain.getTotalNeuronCount();
    scaleForBrainSize(totalNeurons);
}

void AgentBrainConfig::scaleForBrainSize(size_t neuronCount) {
    float scale = static_cast<float>(neuronCount) / 1000.0f; // Reference to 1000 neurons
    
    scalingFactor = std::max(0.1f, std::min(scale, 10.0f)); // Clamp between 0.1 and 10
    
    if (scalingFactor != 1.0f) {
        maxVisionNeurons = static_cast<size_t>(maxVisionNeurons * scalingFactor);
        maxTouchNeurons = static_cast<size_t>(maxTouchNeurons * scalingFactor);
        maxInternalNeurons = static_cast<size_t>(maxInternalNeurons * scalingFactor);
        maxProprioceptionNeurons = static_cast<size_t>(maxProprioceptionNeurons * scalingFactor);
        
        maxMotorForwardNeurons = static_cast<size_t>(maxMotorForwardNeurons * scalingFactor);
        maxMotorBackwardNeurons = static_cast<size_t>(maxMotorBackwardNeurons * scalingFactor);
        maxMotorTurnLeftNeurons = static_cast<size_t>(maxMotorTurnLeftNeurons * scalingFactor);
        maxMotorTurnRightNeurons = static_cast<size_t>(maxMotorTurnRightNeurons * scalingFactor);
        maxMotorInteractNeurons = static_cast<size_t>(maxMotorInteractNeurons * scalingFactor);
        maxMotorWaitNeurons = static_cast<size_t>(maxMotorWaitNeurons * scalingFactor);
    }
}

AgentBrainConfig AgentBrainFactory::config_;

} // namespace nlm