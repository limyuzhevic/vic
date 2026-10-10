#include "Neuromodulator.hpp"
#include <algorithm>
#include <random>
#include <cmath>

namespace nlm {

struct Neuromodulator::Impl {
    std::unordered_map<NeuromodulatorType, NeuromodulatorEffect> modulatorEffects;
    std::unordered_map<NeuromodulatorType, float> currentLevels;
    std::unordered_map<NeuromodulatorType, std::vector<float>> levelHistory;
    
    float timeScale;
    float globalModulation;
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : timeScale(1.0f), globalModulation(1.0f), rng(std::random_device{}()), uniformDist(0.0f, 1.0f) {
        // Initialize default neuromodulator effects
        initializeDefaultEffects();
    }
    
    void initializeDefaultEffects() {
        // Dopamine
        NeuromodulatorEffect dopamineEffect = {
            NeuromodulatorType::DOPAMINE,
            0.1f,    // baseline
            2.0f,    // gain (high for reward prediction)
            500.0f,  // decay time constant (ms)
            0.05f,   // activation threshold
            2.0f,    // maximum effect
            true,    // enables STDP
            false,   // doesn't enhance NMDA directly
            true,    // strengthens tagging
            true     // promotes consolidation
        };
        modulatorEffects[NeuromodulatorType::DOPAMINE] = dopamineEffect;
        
        // Acetylcholine
        NeuromodulatorEffect achEffect = {
            NeuromodulatorType::ACETYLCHOLINE,
            0.2f,    // baseline
            1.5f,    // gain (moderate for attention)
            300.0f,  // decay time constant (ms)
            0.1f,    // activation threshold
            1.5f,    // maximum effect
            false,   // doesn't enable STDP
            true,    // enhances NMDA
            true,    // strengthens tagging
            false    // doesn't directly promote consolidation
        };
        modulatorEffects[NeuromodulatorType::ACETYLCHOLINE] = achEffect;
        
        // Serotonin
        NeuromodulatorEffect serotoninEffect = {
            NeuromodulatorType::SEROTONIN,
            0.15f,   // baseline
            1.2f,    // gain (moderate for mood)
            400.0f,  // decay time constant (ms)
            0.08f,   // activation threshold
            1.2f,    // maximum effect
            false,   // doesn't enable STDP
            false,   // doesn't enhance NMDA
            true,    // strengthens tagging
            true     // promotes consolidation
        };
        modulatorEffects[NeuromodulatorType::SEROTONIN] = serotoninEffect;
        
        // Norepinephrine
        NeuromodulatorEffect naEffect = {
            NeuromodulatorType::NORADRENALINE,
            0.05f,   // baseline
            1.8f,    // gain (high for arousal)
            200.0f,  // decay time constant (ms)
            0.03f,   // activation threshold
            1.8f,    // maximum effect
            true,    // enables STDP
            true,    // enhances NMDA
            true,    // strengthens tagging
            false    // doesn't directly promote consolidation
        };
        modulatorEffects[NeuromodulatorType::NORADRENALINE] = naEffect;
        
        // Endorphin
        NeuromodulatorEffect endorphinEffect = {
            NeuromodulatorType::ENDORPHIN,
            0.0f,    // baseline
            1.3f,    // gain (moderate for pain relief)
            600.0f,  // decay time constant (ms)
            0.02f,   // activation threshold
            1.3f,    // maximum effect
            false,   // doesn't enable STDP
            false,   // doesn't enhance NMDA
            true,    // strengthens tagging
            true     // promotes consolidation
        };
        modulatorEffects[NeuromodulatorType::ENDORPHIN] = endorphinEffect;
        
        // Initialize current levels
        for (const auto& pair : modulatorEffects) {
            currentLevels[pair.first] = pair.second.baseline;
            levelHistory[pair.first] = std::vector<float>(10, pair.second.baseline);
        }
    }
};

Neuromodulator::Neuromodulator() : pImpl(new Impl) {}

Neuromodulator::~Neuromodulator() {
    delete pImpl;
}

void Neuromodulator::addModulator(NeuromodulatorType type, const NeuromodulatorEffect& effect) {
    pImpl->modulatorEffects[type] = effect;
    pImpl->currentLevels[type] = effect.baseline;
    pImpl->levelHistory[type] = std::vector<float>(10, effect.baseline);
}

void Neuromodulator::removeModulator(NeuromodulatorType type) {
    pImpl->modulatorEffects.erase(type);
    pImpl->currentLevels.erase(type);
    pImpl->levelHistory.erase(type);
}

void Neuromodulator::update(float dt, const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            Timestamp currentTime) {
    // Calculate spike rates
    float preRate = preSpikes.empty() ? 0.0f : 
        static_cast<float>(preSpikes.size()) / (dt > 0 ? dt : 1.0f);
    float postRate = postSpikes.empty() ? 0.0f :
        static_cast<float>(postSpikes.size()) / (dt > 0 ? dt : 1.0f);
    
    float totalRate = preRate + postRate;
    
    // Update each neuromodulator level based on neural activity
    for (auto& pair : pImpl->modulatorEffects) {
        NeuromodulatorType type = pair.first;
        NeuromodulatorEffect& effect = pair.second;
        
        // Calculate modulation based on activity
        float activityModulation = std::min(1.0f, totalRate * effect.activationThreshold * 0.1f);
        
        // Apply decay
        float decayFactor = std::exp(-dt / effect.decayTimeConstant);
        
        // Update level with stochastic component
        float noise = (pImpl->uniformDist(pImpl->rng) - 0.5f) * 0.1f;  // ±10% noise
        
        pImpl->currentLevels[type] = 
            decayFactor * pImpl->currentLevels[type] +
            (1.0f - decayFactor) * effect.baseline * activityModulation +
            noise * effect.baseline;
        
        // Clamp to reasonable bounds
        pImpl->currentLevels[type] = std::max(0.0f, std::min(effect.maximumEffect, pImpl->currentLevels[type]));
        
        // Update history
        pImpl->levelHistory[type].push_back(pImpl->currentLevels[type]);
        if (pImpl->levelHistory[type].size() > 10) {
            pImpl->levelHistory[type].erase(pImpl->levelHistory[type].begin());
        }
    }
}

float Neuromodulator::getModulationLevel(NeuromodulatorType type) const {
    auto it = pImpl->currentLevels.find(type);
    if (it != pImpl->currentLevels.end()) {
        return it->second * pImpl->globalModulation;
    }
    return 0.0f;
}

float Neuromodulator::computeModulationGate(NeuromodulatorType type, const Timestamp& spikeTime) const {
    auto it = pImpl->currentLevels.find(type);
    if (it == pImpl->currentLevels.end()) return 0.0f;
    
    float level = it->second;
    
    // Apply temporal modulation based on spike timing
    float temporalMod = 1.0f;
    if (type == NeuromodulatorType::DOPAMINE) {
        // Dopamine: higher during reward prediction errors
        float rewardPrediction = std::sin(spikeTime * 0.001f) * 0.2f + 0.1f;
        temporalMod = 1.0f + rewardPrediction;
    } else if (type == NeuromodulatorType::ACETYLCHOLINE) {
        // Acetylcholine: higher during attention focus
        float attentionMod = 1.0f + 0.3f * std::cos(spikeTime * 0.002f);
        temporalMod = attentionMod;
    }
    
    return level * temporalMod;
}

float Neuromodulator::getEffectMultiplier(NeuromodulatorType type) const {
    auto it = pImpl->modulatorEffects.find(type);
    if (it == pImpl->modulatorEffects.end()) return 1.0f;
    
    float level = getModulationLevel(type);
    return 1.0f + (level - it->second.baseline) * it->second.gain;
}

bool Neuromodulator::enablesSTDP(NeuromodulatorType type) const {
    auto it = pImpl->modulatorEffects.find(type);
    return (it != pImpl->modulatorEffects.end()) && it->second.enablesSTDP;
}

bool Neuromodulator::enhancesNMDA(NeuromodulatorType type) const {
    auto it = pImpl->modulatorEffects.find(type);
    return (it != pImpl->modulatorEffects.end()) && it->second.enhancesNMDA;
}

bool Neuromodulator::strengthensTagging(NeuromodulatorType type) const {
    auto it = pImpl->modulatorEffects.find(type);
    return (it != pImpl->modulatorEffects.end()) && it->second.strengthensTagging;
}

bool Neuromodulator::promotesConsolidation(NeuromodulatorType type) const {
    auto it = pImpl->modulatorEffects.find(type);
    return (it != pImpl->modulatorEffects.end()) && it->second.promotesConsolidation;
}

void Neuromodulator::integrateWithHebbian(Hebbian* hebbian) {
    if (!hebbian) return;
    
    // Configure Hebbian based on neuromodulator effects
    for (const auto& pair : pImpl->modulatorEffects) {
        NeuromodulatorType type = pair.first;
        const NeuromodulatorEffect& effect = pair.second;
        
        float modulationLevel = getModulationLevel(type);
        
        // Set neuromodulation gate
        float gate = modulationLevel * effect.gain;
        
        // Configure Hebbian parameters based on neuromodulator
        if (enablesSTDP(type)) {
            hebbian->enableSTDHebbian(true);
            if (type == NeuromodulatorType::DOPAMINE || type == NeuromodulatorType::NORADRENALINE) {
                hebbian->setSTDHebbianParameters(
                    0.01f * gate,  // LTP weight
                    0.012f * gate, // LTD weight  
                    20.0f / gate   // Time constant (inversely proportional to gate)
                );
            }
        }
        
        if (enhancesNMDA(type)) {
            // Increase NMDA conductance modulation
            hebbian->setNMDAConductance(0.5f * gate);
        }
        
        if (strengthensTagging(type)) {
            // Enhance synaptic tagging
            hebbian->setTagDecayRate(0.001f / gate);
        }
        
        if (promotesConsolidation(type)) {
            // Enhance memory consolidation
            hebbian->setConsolidationThreshold(0.5f / gate);
        }
    }
}

void Neuromodulator::reset() {
    for (auto& pair : pImpl->currentLevels) {
        auto effectIt = pImpl->modulatorEffects.find(pair.first);
        if (effectIt != pImpl->modulatorEffects.end()) {
            pair.second = effectIt->second.baseline;
        }
    }
}

void Neuromodulator::setTimeScale(float timeScale) {
    pImpl->timeScale = std::max(0.1f, std::min(10.0f, timeScale));
}

} // namespace nlm
