// Novelty implementation
#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    float level;
    float plasticityFactor;
    float timeConstant;
    float surpriseBuffer;
    float lastInputHash;
    
    Impl() : level(0.0f), plasticityFactor(1.0f), timeConstant(1.0f), surpriseBuffer(0.0f), lastInputHash(0.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}
Novelty::~Novelty() = default;

const char* Novelty::getName() const {
    return "Novelty";
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->plasticityFactor = 1.0f + pImpl->level * 0.8f;  // Range: 1.0 to 1.8
}

float Novelty::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty level over time
    pImpl->level *= std::exp(-dt / pImpl->timeConstant);
    
    // Process surprise signals
    if (pImpl->surpriseBuffer > 0.001f) {
        pImpl->level += pImpl->surpriseBuffer * 0.2f;
        pImpl->surpriseBuffer = 0.0f;
    }
    
    // Clamp level
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Novelty::analyzeInputChange(const std::vector<float>& newInput) {
    // Compute hash of current input for change detection
    float inputHash = 0.0f;
    for (float val : newInput) {
        inputHash += std::abs(val);
    }
    
    // Detect novelty
    if (pImpl->lastInputHash > 0.0f) {
        float change = std::abs(inputHash - pImpl->lastInputHash);
        if (change > 1.0f) {
            // Significant change - increase surprise
            pImpl->surpriseBuffer += change * 0.1f;
            NLM_LOG_DEBUG(std::string("Novelty detected: change = ") + std::to_string(change));
        }
    }
    
    pImpl->lastInputHash = inputHash;
}

float Novelty::computeNovelty(const std::vector<float>& input1, const std::vector<float>& input2) {
    if (input1.size() != input2.size()) return 1.0f;  // Max novelty for size mismatch
    
    float totalDifference = 0.0f;
    for (size_t i = 0; i < input1.size(); ++i) {
        totalDifference += std::abs(input1[i] - input2[i]);
    }
    
    float averageDifference = totalDifference / input1.size();
    return std::min(1.0f, averageDifference / 10.0f);  // Normalize to [0, 1]
}

} // namespace nlm