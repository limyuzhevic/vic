#include "InternalSignals.hpp"

namespace nlm {

struct InternalSignalsProcessor::Impl {
    std::vector<float> processedSignals;
    std::vector<float> homeostaticSignals;
    
    Impl() = default;
};

InternalSignalsProcessor::InternalSignalsProcessor() : pImpl(new Impl) {}

InternalSignalsProcessor::~InternalSignalsProcessor() = default;

void InternalSignalsProcessor::process(const InternalSignals& signals) {
    // Real internal signal processing using NLM's neural machinery
    // Process internal brain states like arousal, attention, homeostasis
    
    // Get all internal signals
    const auto& signalValues = signals.getData();
    size_t signalCount = signalValues.size();
    
    // Clear previous features
    pImpl->processedSignals.clear();
    pImpl->homeostaticSignals.clear();
    
    if (signalCount == 0) {
        return;
    }
    
    // Define signal types for indexing
    enum InternalSignalType {
        Arousal = 0,
        Attention = 1,
        Homeostasis = 2,
        MemoryLoad = 3,
        Stress = 4,
        Reward = 5,
        Punishment = 6,
        Fatigue = 7,
        Count = 8
    };
    
    // Extract and normalize individual signals
    std::array<float, InternalSignalType::Count> normalizedSignals = {0.0f};
    
    // Normalize each signal to range [0, 1] based on expected ranges
    for (size_t i = 0; i < signalCount && i < InternalSignalType::Count; ++i) {
        float value = signalValues[i];
        
        switch (static_cast<InternalSignalType>(i)) {
            case InternalSignalType::Arousal:
                // Arousal typically 0-1 (0 = calm, 1 = highly aroused)
                normalizedSignals[i] = std::clamp(value, 0.0f, 1.0f);
                break;
            case InternalSignalType::Attention:
                // Attention typically 0-1 (0 = no attention, 1 = full focus)
                normalizedSignals[i] = std::clamp(value, 0.0f, 1.0f);
                break;
            case InternalSignalType::Homeostasis:
                // Homeostasis typically 0-1 (0 = satisfied, 1 = drive present)
                normalizedSignals[i] = std::clamp(value, 0.0f, 1.0f);
                break;
            case InternalSignalType::MemoryLoad:
                // Memory load typically 0-1 (0 = light, 1 = heavy)
                normalizedSignals[i] = std::clamp(value, 0.0f, 1.0f);
                break;
            case InternalSignalType::Stress:
                // Stress typically 0-1 (0 = relaxed, 1 = stressed)
                normalizedSignals[i] = std::clamp(value, 0.0f, 1.0f);
                break;
            case InternalSignalType::Reward:
                // Reward typically -1 to 1 (negative = bad, positive = good)
                normalizedSignals[i] = (value + 1.0f) * 0.5f;  // Normalize to 0-1
                break;
            case InternalSignalType::Punishment:
                // Punishment typically -1 to 1
                normalizedSignals[i] = (value + 1.0f) * 0.5f;
                break;
            case InternalSignalType::Fatigue:
                // Fatigue typically 0-1 (0 = fresh, 1 = exhausted)
                normalizedSignals[i] = std::clamp(value, 0.0f, 1.0f);
                break;
        }
    }
    
    // Store processed signals
    pImpl->processedSignals = std::vector<float>(normalizedSignals.begin(), normalizedSignals.end());
    
    // Generate homeostatic signals as combinations of internal signals
    // Allostatic load: combined drive from multiple needs
    float allostaticLoad = 0.0f;
    float homeostaticWeightSum = 0.0f;
    
    // Hunger/satiety (Homeostasis)
    float hungerNeed = normalizedSignals[InternalSignalType::Homeostasis] * 0.4f;  // 40% weight
    allostaticLoad += hungerNeed;
    homeostaticWeightSum += 0.4f;
    
    // Thirst/saturation (would be separate signal, using homeostasis as proxy)
    float thirstNeed = normalizedSignals[InternalSignalType::Homeostasis] * 0.3f;  // 30% weight
    allostaticLoad += thirstNeed;
    homeostaticWeightSum += 0.3f;
    
    // Fatigue/rest need
    float fatigueNeed = normalizedSignals[InternalSignalType::Fatigue] * 0.2f;  // 20% weight
    allostaticLoad += fatigueNeed;
    homeostaticWeightSum += 0.2f;
    
    // Energy regulation (simplified from homeostatic signals)
    float energyNeed = (1.0f - normalizedSignals[InternalSignalType::Fatigue]) * 0.1f;
    allostaticLoad += energyNeed;
    homeostaticWeightSum += 0.1f;
    
    // Normalize allostatic load
    if (homeostaticWeightSum > 0.0f) {
        allostaticLoad /= homeostaticWeightSum;
    }
    
    pImpl->homeostaticSignals.push_back(allostaticLoad);  // Allostatic load
    pImpl->homeostaticSignals.push_back(normalizedSignals[InternalSignalType::Fatigue]);  // Fatigue level
    pImpl->homeostaticSignals.push_back(1.0f - normalizedSignals[InternalSignalType::Attention]);  // Attention deficit
    
    // Add arousal as combination of arousal and stress
    float totalArousal = normalizedSignals[InternalSignalType::Arousal] + 
                        normalizedSignals[InternalSignalType::Stress] * 0.5f;
    pImpl->homeostaticSignals.push_back(std::min(totalArousal, 1.0f));  // Capped arousal
    
    // Add reward prediction error (simplified)
    float rewardPrediction = normalizedSignals[InternalSignalType::Reward];
    float punishmentPrediction = normalizedSignals[InternalSignalType::Punishment];
    float rewardError = rewardPrediction - punishmentPrediction;
    pImpl->homeostaticSignals.push_back((rewardError + 1.0f) * 0.5f);  // Normalized to 0-1

const std::vector<float>& InternalSignalsProcessor::getProcessedSignals() const {
    return pImpl->processedSignals;
}

const std::vector<float>& InternalSignalsProcessor::getHomeostaticSignals() const {
    return pImpl->homeostaticSignals;
}

} // namespace nlm
