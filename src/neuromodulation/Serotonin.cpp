#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float mood;
    float impulsivity;
    float socialBehavior;
    float rewardSensitivity;
    float decayRate;
    SimulationStep lastUpdate;
    std::vector<NeuronId> rewardNeurons;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.02f)
        , mood(0.5f)
        , impulsivity(0.5f)
        , socialBehavior(0.5f)
        , rewardSensitivity(1.0f)
        , decayRate(0.03f)
        , lastUpdate(0) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity: stabilizes memories when mood is good
    float plasticity = 0.7f;  // Base stabilization
    
    // Mood affects learning: good mood enhances positive learning
    if (pImpl->mood > 0.6f) {
        plasticity *= 1.2f;  // Good mood enhances plasticity
    } else if (pImpl->mood < 0.3f) {
        plasticity *= 0.8f;  // Poor mood reduces plasticity
    }
    
    // Impulsivity reduces reflective learning
    if (pImpl->impulsivity > 0.7f) {
        plasticity *= 0.9f;
    }
    
    return std::clamp(plasticity, 0.0f, 1.0f);
}

void Serotonin::update(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Update mood and behavioral parameters
    pImpl->mood = 0.5f + 0.5f * pImpl->level;  // Mood scales with 5-HT level
    pImpl->impulsivity = 1.0f - pImpl->level;  // High 5-HT reduces impulsivity
    pImpl->socialBehavior = 0.5f + 0.5f * pImpl->level;  // Social behavior increases with 5-HT
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply behavioral effects
    applyBehavioralEffects(dt);
    
    // Update reward sensitivity
    updateRewardSensitivity(dt);
    
    // Log status periodically
    pImpl->lastUpdate++;
    if (pImpl->lastUpdate % 100 == 0) {
        NLM_LOG_INFO("Serotonin: Level=" + std::to_string(pImpl->level) +
                    " Mood=" + std::to_string(pImpl->mood) +
                    " Impulsivity=" + std::to_string(pImpl->impulsivity) +
                    " Social=" + std::to_string(pImpl->socialBehavior));
    }
}

void Serotonin::applyBehavioralEffects(TimestepDuration dt) {
    // Apply 5-HT effects on neural circuits
    if (pImpl->level > 0.3f) {
        // Enhance emotional regulation and stability
        float regulationStrength = (pImpl->level - 0.3f) * 2.0f;
        
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // 5-HT stabilizes neural activity
                    float current = neuron->getState().membranePotential;
                    float target = neuron->getState().restingPotential;
                    
                    // Dampen rapid fluctuations
                    if (std::abs(current - target) > 5.0f) {
                        float damp = regulationStrength * 0.5f;
                        neuron->setMembranePotential(current + (target - current) * damp);
                    }
                }
            }
        }
        
        // Enhance social behavior processing
        if (pImpl->brain->getSocialSystem()) {
            pImpl->brain->getSocialSystem()->updateSocialMetrics(pImpl->socialBehavior);
        }
    }
    
    // Reduce impulsivity by enhancing prefrontal control
    if (pImpl->impulsivity < 0.3f && pImpl->brain) {
        // Find prefrontal-like regions and enhance their control
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Internal) {
                    // Enhance top-down control
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(0.5f);
                    }
                }
            }
        }
    }
}

void Serotonin::updateRewardSensitivity(TimestepDuration dt) {
    // 5-HT modulates reward processing
    pImpl->rewardSensitivity = 0.5f + 0.5f * pImpl->level;
    
    // Enhance reward prediction error signaling
    if (pImpl->brain && pImpl->brain->getPredictionErrorSignal()) {
        float error = pImpl->brain->getPredictionErrorSignal()->getError();
        float magnitude = pImpl->brain->getPredictionErrorSignal()->getMagnitude();
        
        if (magnitude > 0.0f) {
            // 5-HT modulates reward prediction error processing
            float modulatedError = error * pImpl->rewardSensitivity;
            pImpl->brain->getPredictionErrorSignal()->computeError(
                pImpl->brain->getPredictionErrorSignal()->getError(),
                modulatedError
            );
        }
    }
    
    // Select reward-related neurons
    if (pImpl->level > 0.4f) {
        pImpl->rewardNeurons.clear();
        
        // Find motor and reward neurons
        if (pImpl->brain) {
            for (const auto& neuron : pImpl->brain->getMotorNeurons()) {
                pImpl->rewardNeurons.push_back(neuron);
            }
        }
    }
}

void Serotonin::signalReward(float rewardValue) {
    // Signal reward (e.g., food, social interaction)
    pImpl->level = std::min(1.0f, pImpl->level + rewardValue * 0.2f);
    
    // Enhance reward sensitivity
    pImpl->rewardSensitivity = std::min(1.0f, pImpl->rewardSensitivity + rewardValue * 0.1f);
    
    // Log reward signal
    NLM_LOG_INFO("Serotonin: Reward signal, level=" + std::to_string(pImpl->level) +
                " Reward=" + std::to_string(rewardValue));
}

void Serotonin::signalSocialInteraction(float socialQuality) {
    // Signal social interaction quality
    pImpl->level = std::min(1.0f, pImpl->level + socialQuality * 0.3f);
    pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + socialQuality * 0.2f);
    
    // Social interactions reduce stress and increase mood
    if (socialQuality > 0.5f) {
        // Positive social interaction
        pImpl->mood = std::min(1.0f, pImpl->mood + socialQuality * 0.3f);
    } else {
        // Negative social interaction
        pImpl->mood = std::max(0.0f, pImpl->mood - 0.2f);
    }
    
    NLM_LOG_INFO("Serotonin: Social interaction, mood=" + std::to_string(pImpl->mood) +
                " Social=" + std::to_string(pImpl->socialBehavior));
}

void Serotonin::signalStress(float stressLevel) {
    // Signal stress (social or environmental)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - stressLevel * 0.3f);
    
    // Stress reduces mood and social behavior
    pImpl->mood = std::max(0.0f, pImpl->mood - stressLevel * 0.2f);
    pImpl->socialBehavior = std::max(0.0f, pImpl->socialBehavior - stressLevel * 0.1f);
    
    // Prolonged stress increases impulsivity
    if (stressLevel > 0.7f) {
        pImpl->impulsivity = std::min(1.0f, pImpl->impulsivity + stressLevel * 0.3f);
    }
    
    NLM_LOG_INFO("Serotonin: Stress signal, mood=" + std::to_string(pImpl->mood) +
                " Stress level=" + std::to_string(stressLevel));
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

float Serotonin::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

bool Serotonin::isPositiveMood() const {
    return pImpl->mood > 0.6f;
}

} // namespace nlm
