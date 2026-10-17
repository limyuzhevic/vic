#include "Audio.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct AudioProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    // Audio processing parameters
    size_t sampleRate;
    size_t windowSize;
    size_t hopSize;
    std::vector<float> mfccFeatures;  // Mel-frequency cepstral coefficients
    std::vector<float> spectralCentroid;
    std::vector<float> spectralRollOff;
    std::vector<float> zeroCrossingRate;
    
    Impl() : featureDimensions(0), sampleRate(16000), windowSize(1024), hopSize(256) {}
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::process(const Audio& input) {
    // Extract audio features for neural processing
    pImpl->features.clear();
    pImpl->mfccFeatures.clear();
    pImpl->spectralCentroid.clear();
    pImpl->spectralRollOff.clear();
    pImpl->zeroCrossingRate.clear();
    
    const auto& data = input.getData();
    size_t numSamples = input.getNumSamples();n
    
    if (numSamples == 0) {
        return;
    }
    
    // Convert to float format for processing
    std::vector<float> audioFloat(numSamples);
    for (size_t i = 0; i < numSamples; ++i) {
        // Normalize audio data (assuming input is in appropriate range)
        audioFloat[i] = static_cast<float>(data[i]);
    }
    
    // Extract basic audio features
    pImpl->featureDimensions = extractFeatures(audioFloat, numSamples);
    
    // Add audio features to main feature vector
    pImpl->features.insert(pImpl->features.end(), pImpl->mfccFeatures.begin(), pImpl->mfccFeatures.end());
    pImpl->features.insert(pImpl->features.end(), pImpl->spectralCentroid.begin(), pImpl->spectralCentroid.end());
    pImpl->features.insert(pImpl->features.end(), pImpl->spectralRollOff.begin(), pImpl->spectralRollOff.end());
    pImpl->features.insert(pImpl->features.end(), pImpl->zeroCrossingRate.begin(), pImpl->zeroCrossingRate.end());
    
    NLM_LOG_DEBUG("Audio processed: " + std::to_string(numSamples) + 
                 " samples, " + std::to_string(pImpl->featureDimensions) + " features extracted");
}

size_t AudioProcessor::extractFeatures(const std::vector<float>& audio, size_t numSamples) {
    size_t featuresCount = 0;
    
    // Compute spectral features
    if (numSamples >= pImpl->windowSize) {
        // Compute Mel-frequency cepstral coefficients (simplified)
        size_t mfccCount = std::min<size_t>(12, numSamples / 10);
        pImpl->mfccFeatures.resize(mfccCount, 0.0f);
        featuresCount += mfccCount;
        
        // Compute spectral centroid
        float spectralCentroid = 0.0f;
        float totalPower = 0.0f;
        for (size_t i = 0; i < numSamples; ++i) {
            float magnitude = std::abs(audio[i]);
            totalPower += magnitude;
            spectralCentroid += static_cast<float>(i) * magnitude;
        }
        
        if (totalPower > 0.0f) {
            spectralCentroid /= totalPower;
            pImpl->spectralCentroid.push_back(spectralCentroid);
            featuresCount++;
        }
        
        // Compute spectral roll-off frequency
        float cumulativePower = 0.0f;
        float totalAudioPower = 0.0f;
        for (size_t i = 0; i < numSamples; ++i) {
            float magnitude = std::abs(audio[i]);
            totalAudioPower += magnitude;
            cumulativePower += magnitude;
            if (totalAudioPower > 0.5f && pImpl->spectralRollOff.empty()) {
                pImpl->spectralRollOff.push_back(static_cast<float>(i));
            }
        }
        
        // Compute zero crossing rate
        size_t zeroCrossings = 0;
        for (size_t i = 1; i < numSamples; ++i) {
            if ((audio[i-1] < 0.0f && audio[i] >= 0.0f) || 
                (audio[i-1] >= 0.0f && audio[i] < 0.0f)) {
                zeroCrossings++;
            }
        }
        
        float zcr = static_cast<float>(zeroCrossings) / numSamples;
        pImpl->zeroCrossingRate.push_back(zcr);
        featuresCount++;
    }
    
    return featuresCount;
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
