#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Novelty::Impl {
    float level;
    float baselineLevel;
    float sensorySurprise;
    float statisticalRarity;
    float predictionError;
    float changeDetection;
    float memoryAccessCost;
    std::vector<float> recentInputHistory;
    static constexpr size_t historySize = 100;
};

Novelty::Novelty() : pImpl(new Impl()) {
    pImpl->level = 0.0f;
    pImpl->baselineLevel = 0.01f;
    pImpl->sensorySurprise = 0.0f;
    pImpl->statisticalRarity = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->changeDetection = 0.0f;
    pImpl->memoryAccessCost = 0.1f;
    pImpl->recentInputHistory.reserve(pImpl->historySize);
}

Novelty::~Novelty() = default;

const char* Novelty::getName() const {
    return "Novelty Detection";
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Novelty affects sensory surprise
    pImpl->sensorySurprise = pImpl->level * 1.5f;
    if (pImpl->sensorySurprise > 1.0f) pImpl->sensorySurprise = 1.0f;
    
    // Novelty affects statistical rarity calculation
    pImpl->statisticalRarity = 1.0f - pImpl->level;
    
    // Novelty modulates prediction error sensitivity
    pImpl->predictionError = pImpl->level;
    
    NLM_LOG_DEBUG("Novelty level set to " + std::to_string(level) + 
                  ", surprise: " + std::to_string(pImpl->sensorySurprise) +
                  ", rarity: " + std::to_string(pImpl->statisticalRarity));
}

float Novelty::getPlasticityFactor() const {
    // Novelty enhances plasticity for learning new patterns
    return 1.5f + pImpl->level * 1.0f;  // Up to 2.5x plasticity
}

void Novelty::update(TimestepDuration dt) {
    // Novelty decays
    pImpl->level *= std::pow(0.9f, dt);
    
    // Maintain recent input history
    if (pImpl->recentInputHistory.size() >= pImpl->historySize) {
        pImpl->recentInputHistory.erase(pImpl->recentInputHistory.begin());
    }
}

void Novelty::detectNovelty(const std::vector<float>& input) {
    if (input.empty()) return;
    
    // Add to history
    pImpl->recentInputHistory.insert(pImpl->recentInputHistory.end(), 
                                     input.begin(), input.end());
    if (pImpl->recentInputHistory.size() > pImpl->historySize) {
        pImpl->recentInputHistory.erase(pImpl->recentInputHistory.begin(), 
                                       pImpl->recentInputHistory.begin() + 
                                       (pImpl->recentInputHistory.size() - pImpl->historySize));
    }
    
    // Calculate statistical deviation
    if (!pImpl->recentInputHistory.empty()) {
        float mean = std::accumulate(pImpl->recentInputHistory.begin(), 
                                     pImpl->recentInputHistory.end(), 0.0f) / 
                     pImpl->recentInputHistory.size();
        float variance = 0.0f;
        for (float val : pImpl->recentInputHistory) {
            variance += (val - mean) * (val - mean);
        }
        variance /= pImpl->recentInputHistory.size();
        float stdDev = std::sqrt(variance);
        
        // Calculate current deviation from mean
        float currentMean = std::accumulate(input.begin(), input.end(), 0.0f) / input.size();
        float deviation = std::abs(currentMean - mean);
        
        // Normalize by std dev (handle zero case)
        float normalizedNovelty = stdDev > 0.0f ? deviation / stdDev : 0.0f;
        normalizedNovelty = std::min(normalizedNovelty, 1.0f);
        
        pImpl->changeDetection = normalizedNovelty;
        setLevel(std::min(pImpl->level + normalizedNovelty * 0.3f, 1.0f));
        
        NLM_LOG_INFO("Novelty detected: deviation=" + std::to_string(deviation) + 
                     ", novelty_level=" + std::to_string(pImpl->level));
    }
}

void Novelty::signalSensorySurprise() {
    // Strong surprise triggers novelty system
    setLevel(std::min(pImpl->level + 0.2f, 1.0f));
    NLM_LOG_INFO("Sensory surprise triggers novelty system");
}

} // namespace nlm