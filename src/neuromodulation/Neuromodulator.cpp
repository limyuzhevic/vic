#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

/**
 * Helper function for safe string copying
 */
void safeStrCopy(char* dest, const char* src, size_t destSize) {
    if (dest && src) {
        strncpy_s(dest, destSize, src, _TRUNCATE);
        dest[destSize - 1] = '\0';
    }
}

/**
 * Validate input parameters for neuromodulators
 * @param level Level value to validate (should be between 0.0 and 1.0)
 * @return true if valid, false otherwise
 */
bool validateLevel(float level) {
    return !std::isnan(level) && !std::isinf(level) && level >= 0.0f && level <= 1.0f;
}

/**
 * Safely update level with bounds checking
 * @param impl Pointer to implementation
 * @param newLevel New level value
 * @return true if update successful, false otherwise
 */
bool safeUpdateLevel(Dopamine::Impl* impl, float newLevel) {
    if (!impl) {
        NLM_LOG_ERROR("Attempt to update null Dopamine implementation");
        return false;
    }
    
    if (!validateLevel(newLevel)) {
        NLM_LOG_ERROR("Invalid dopamine level: " + std::to_string(newLevel));
        return false;
    }
    
    impl->level = newLevel;
    return true;
}

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

/**
 * Safe implementation constructor with validation
 */
Dopamine::Dopamine() : pImpl(new Impl) {
    if (!pImpl) {
        NLM_LOG_ERROR("Failed to allocate Dopamine implementation");
        throw std::runtime_error("Memory allocation failed for Dopamine");
    }
}

/**
 * Safe implementation destructor with cleanup
 */
Dopamine::~Dopamine() {
    delete pImpl;
    pImpl = nullptr;
}

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
