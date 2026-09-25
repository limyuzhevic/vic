#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include "../environment/Observation.hpp"
#include "../sensory/Vision.hpp"
#include "../sensory/Audio.hpp"
#include "../sensory/InternalSignals.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    if (std::isnan(value)) {
        NLM_LOG_WARNING("Invalid reward value: NaN, using 0.0 instead");
        pImpl->currentValue = 0.0f;
    } else {
        pImpl->currentValue = value;
    }
}

void Reward::add(float delta) {
    if (std::isnan(delta)) {
        NLM_LOG_WARNING("Invalid reward delta: NaN, ignoring");
        return;
    }
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    if (!observation.isValid()) {
        NLM_LOG_WARNING("Cannot compute reward from invalid observation");
        return 0.0f;
    }

    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        NLM_LOG_WARNING("Observation has no valid sensory input");
        return 0.0f;
    }

    // Compute reward based on different sensory modalities
    float reward = 0.0f;
    
    // Vision-based reward (based on environmental features)
    if (std::string(sensoryInput->getType()) == "Vision") {
        const Vision* vision = dynamic_cast<const Vision*>(sensoryInput);
        if (vision) {
            const std::vector<float>& visionData = vision->getData();
            if (!visionData.empty()) {
                // Compute vision-based reward:
                // Positive reward for high contrast/brightness (exploration advantage)
                // Negative reward for uniform regions (lack of information)
                float meanIntensity = std::accumulate(visionData.begin(), visionData.end(), 0.0f) / visionData.size();
                
                // Compute contrast (variance) - high contrast is good for exploration
                float variance = 0.0f;
                for (float intensity : visionData) {
                    variance += (intensity - meanIntensity) * (intensity - meanIntensity);
                }
                variance /= visionData.size();
                
                // Reward for good contrast and reasonable brightness
                reward = variance * 0.1f + std::clamp(meanIntensity, 0.0f, 1.0f) * 0.05f;
            }
        }
    }n    
    // Audio-based reward (based on sound features)
    else if (std::string(sensoryInput->getType()) == "Audio") {
        const Audio* audio = dynamic_cast<const Audio*>(sensoryInput);
        if (audio) {
            const std::vector<float>& audioData = audio->getData();
            if (!audioData.empty()) {
                // Audio reward based on change and complexity
                float maxAmplitude = *std::max_element(audioData.begin(), audioData.end());
                float minAmplitude = *std::min_element(audioData.begin(), audioData.end());
                
                // Reward for dynamic sounds (both positive and negative)
                float amplitudeRange = std::abs(maxAmplitude - minAmplitude);
                reward = amplitudeRange * 0.2f;
                
                // Bonus for moderate amplitude (not too quiet)
                float avgAmplitude = (maxAmplitude + minAmplitude) * 0.5f;
                if (avgAmplitude > 0.1f) {
                    reward += 0.1f;
                }
            }
        }
    }
    
    // Internal signals-based reward (based on brain state)
    else if (std::string(sensoryInput->getType()) == "InternalSignals") {
        const InternalSignals* internal = dynamic_cast<const InternalSignals*>(sensoryInput);
        if (internal) {
            const std::vector<float>& internalData = internal->getData();
            if (!internalData.empty()) {
                // Internal reward based on activation levels and patterns
                float sum = std::accumulate(internalData.begin(), internalData.end(), 0.0f);
                float mean = sum / internalData.size();
                
                // Reward for moderate activation (not too high, not too low)
                reward = std::clamp(mean, 0.0f, 1.0f) * 0.3f;
                
                // Bonus for activity patterns (variance)
                float variance = 0.0f;
                for (float value : internalData) {
                    variance += (value - mean) * (value - mean);
                }
                variance /= internalData.size();
                reward += variance * 0.1f;
            }
        }
    }
    
    // Default reward computation for unknown sensory input types
    else {
        NLM_LOG_WARNING("Unknown sensory input type: " + std::string(sensoryInput->getType()) + 
                       ". Using default reward computation");
        // Simple default: based on overall data characteristics
        const std::vector<float>& data = sensoryInput->getData();
        if (!data.empty()) {
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            float mean = sum / data.size();
            reward = std::clamp(mean, 0.0f, 1.0f) * 0.1f;
        }
    }

    // Store in history
    pImpl->history.push_back(reward);
    
    // Limit history size
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin(), pImpl->history.begin() + 100);
    }

    // Update current value
    pImpl->currentValue = reward;

    NLM_LOG_DEBUG("Computed reward: " + std::to_string(reward) + 
                 " based on " + std::string(sensoryInput->getType()) + " input");
    
    return reward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
