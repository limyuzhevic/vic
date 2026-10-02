// Enhanced neuromodulation system for NLM
// Provides advanced neuromodulation behaviors with sophisticated learning mechanisms

#include "AdvancedNeuromodulation.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// AdvancedNeuromodulator implementation

void AdvancedNeuromodulator::applyDopamineEffects(float level, float effect) {
    // Dopamine affects both plasticity and excitability
    if (brain_) {
        // Update STDP parameters
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            float plasticityFactor = getPlasticityFactor();
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
        }
        
        // Apply to neurons (excitability modulation)
        float excitabilityFactor = getExcitabilityFactor();
        if (excitabilityFactor > 1.0f) {
            // Apply additional current to neurons
            for (auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        float additionalCurrent = (excitabilityFactor - 1.0f) * 0.5f;
                        neuron->injectCurrent(additionalCurrent);
                    }
                }
            }
        }
    }
}

void AdvancedNeuromodulator::applySerotoninEffects(float level, float effect) {
    // Serotonin affects anxiety and learning rate
    if (brain_) {
        // Modulate plasticity rate
        float plasticityFactor = getPlasticityFactor();
        
        // Update learning rates
        auto* hebbian = brain_->getHebbian();
        if (hebbian) {
            hebbian->setLearningRate(0.01f * plasticityFactor);
        }
        
        // Apply to cognitive systems
        auto* planner = brain_->getPlanner();
        if (planner) {
            // Serotonin affects decision making and exploration
            planner->setExplorationFactor(std::clamp(level, 0.0f, 1.0f));
        }
    }
}

void AdvancedNeuromodulator::applyAcetylcholineEffects(float level, float effect) {
    // Acetylcholine affects attention and memory
    if (brain_) {
        // Enhance attention mechanisms
        auto* attention = brain_->getAttention();
        if (attention) {
            attention->setInhibitionStrength(0.5f - level * 0.3f);  // Reduced inhibition
            attention->setExcitationStrength(0.5f + level * 0.3f);  // Enhanced excitation
        }
        
        // Boost memory encoding
        if (level > 0.0f) {
            auto* workingMemory = brain_->getWorkingMemory();
            if (workingMemory) {
                workingMemory->enhanceEncoding(std::abs(level));
            }
        }
    }
}

void AdvancedNeuromodulator::applyNorepinephrineEffects(float level, float effect) {
    // Norepinephrine affects arousal and vigilance
    if (brain_) {
        // Increase arousal
        if (level > 0.0f) {
            // Apply to sensory systems
            for (auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == NeuronType::Sensory) {
                        for (auto* neuron : pop->getNeurons()) {
                            neuron->injectCurrent(level * 0.3f);
                        }
                    }
                }
            }
        }
        
        // Enhance alertness in cognitive systems
        auto* planner = brain_->getPlanner();
        if (planner) {
            planner->setAlertnessLevel(std::clamp(level, 0.0f, 1.0f));
        }
    }
}

void AdvancedNeuromodulator::applyAdrenalineEffects(float level, float effect) {
    // Adrenaline affects stress response and rapid reactions
    if (brain_) {
        // Emergency response - prepare for quick action
        if (level > 0.5f) {
            // Prepare motor systems for rapid response
            for (auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == NeuronType::Motor) {
                        for (auto* neuron : pop->getNeurons()) {
                            neuron->setRefractoryPeriod(static_cast<uint32_t>(std::max(1.0f, 5.0f - level * 4.0f)));
                        }
                    }
                }
            }
        }
        
        // Increase overall excitability
        float overallExcitability = 1.0f + level * 0.8f;
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent((overallExcitability - 1.0f) * 0.2f);
                }
            }
        }
    }
}

void AdvancedNeuromodulator::applyCustomEffects(float level, float effect) {
    // Custom neuromodulation effects
    if (brain_ && effect_.effectFunction) {
        float customEffect = effect_.effectFunction(level);
        
        // Apply custom effect to all neurons
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(customEffect * 0.1f);
                }
            }
        }
    }
}

// NeuromodulationUtils implementation

NeuromodulationEffect NeuromodulationUtils::createSophisticatedEffect(NeuromodulationType type,
                                                                   float baseStrength,
                                                                   float inputGain,
                                                                   float adaptationTime,
                                                                   float saturationLevel,
                                                                   float inhibitionThreshold) {
    NeuromodulationEffect effect;
    effect.type = type;
    effect.baseStrength = baseStrength;
    effect.inputGain = inputGain;
    effect.adaptationTime = adaptationTime;
    effect.saturationLevel = saturationLevel;
    effect.inhibitionThreshold = inhibitionThreshold;
    
    // Custom effect functions based on neuromodulator type
    switch (type) {
        case NeuromodulationType::Dopamine:
            effect.effectFunction = [](float level) {
                // Dopamine: inverted U-shaped response (optimal at moderate levels)
                float normalized = std::abs(level);
                return 1.0f - std::abs(2.0f * normalized - 1.0f);
            };
            break;
        case NeuromodulationType::Serotonin:
            effect.effectFunction = [](float level) {
                // Serotonin: linear response
                return level;
            };
            break;
        case NeuromodulationType::Acetylcholine:
            effect.effectFunction = [](float level) {
                // Acetylcholine: thresholded response
                return (level > 0.0f) ? level : 0.0f;
            };
            break;
        case NeuromodulationType::Norepinephrine:
            effect.effectFunction = [](float level) {
                // Norepinephrine: exponential response
                return std::tanh(level * 2.0f);
            };
            break;
        case NeuromodulationType::Adrenaline:
            effect.effectFunction = [](float level) {
                // Adrenaline: super-linear response
                return std::pow(std::tanh(level), 2.0f);
            };
            break;
        default:
            effect.effectFunction = [](float level) {
                // Default: sigmoidal response
                return 2.0f / (1.0f + std::exp(-10.0f * (level - 0.5f))) - 1.0f;
            };
            break;
    }
    
    return effect;
}

// Explicit template instantiations

// AdvancedNeuromodulationManager

// CreateNeuromodulationSchedule implementations (stubbed for now)

} // namespace nlm
