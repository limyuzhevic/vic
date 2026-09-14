#include "Audio.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <map>
#include <unordered_set>

namespace nlm {

struct AudioProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    Impl() : featureDimensions(0) {}
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::process(const Audio& input) {
    const std::vector<float>& audioData = input.getData();
    size_t sampleRate = input.getSampleRate();
    size_t numSamples = input.getNumSamples();n
    
    if (audioData.empty() || numSamples == 0) {
        pImpl->features.clear();
        pImpl->featureDimensions = 0;
        return;
    }
    
    pImpl->features.clear();
    
    // 1. Statistical features
    float sum = std::accumulate(audioData.begin(), audioData.end(), 0.0f);
    float mean = sum / numSamples;
    
    float varianceSum = 0.0f;
    for (float sample : audioData) {
        varianceSum += (sample - mean) * (sample - mean);
    }
    float variance = varianceSum / numSamples;
    
    pImpl->features.push_back(mean);
    pImpl->features.push_back(variance);
    
    // 2. Frequency analysis (simplified via zero-crossings)
    size_t zeroCrossings = 0;
    for (size_t i = 1; i < numSamples; ++i) {
        if ((audioData[i] >= 0.0f && audioData[i-1] < 0.0f) ||
            (audioData[i] < 0.0f && audioData[i-1] >= 0.0f)) {
            zeroCrossings++;
        }
    }
    float zeroCrossingRate = static_cast<float>(zeroCrossings) / (numSamples - 1);
    pImpl->features.push_back(zeroCrossingRate);
    
    // 3. Energy and RMS
    float energySum = 0.0f;
    for (float sample : audioData) {
        energySum += sample * sample;
    }
    float energy = energySum / numSamples;
    float rms = std::sqrt(energy);
    pImpl->features.push_back(energy);
    pImpl->features.push_back(rms);
    
    // 4. Temporal features - dynamic range
    float minSample = *std::min_element(audioData.begin(), audioData.end());
    float maxSample = *std::max_element(audioData.begin(), audioData.end());
    float dynamicRange = maxSample - minSample;
    pImpl->features.push_back(dynamicRange);
    
    // 5. Spectrogram-like features (simplified via FFT bins approximation)
    // Create a simple frequency representation by dividing into segments
    size_t numSegments = std::min<size_t>(8, numSamples / 4); // Use up to 8 frequency bands
    if (numSegments > 0) {
        size_t segmentSize = numSamples / numSegments;
        std::vector<float> segmentEnergies(numSegments, 0.0f);
        
        for (size_t seg = 0; seg < numSegments; ++seg) {
            size_t start = seg * segmentSize;
            size_t end = (seg == numSegments - 1) ? numSamples : start + segmentSize;
            
            float segEnergy = 0.0f;
            for (size_t i = start; i < end; ++i) {
                segEnergy += audioData[i] * audioData[i];
            }
            segmentEnergies[seg] = segEnergy;
        }
        
        // Add frequency band statistics
        float minBandEnergy = *std::min_element(segmentEnergies.begin(), segmentEnergies.end());
        float maxBandEnergy = *std::max_element(segmentEnergies.begin(), segmentEnergies.end());
        float avgBandEnergy = sum / numSegments;
        
        pImpl->features.push_back(minBandEnergy);
        pImpl->features.push_back(maxBandEnergy);
        pImpl->features.push_back(avgBandEnergy);
    }
    
    // 6. Pitch estimation (simplified)
    // Find the most common sample magnitude as a rough pitch indicator
    if (!audioData.empty()) {
        std::map<float, size_t> magnitudeCounts;
        for (float sample : audioData) {
            float magnitude = std::abs(sample);
            magnitudeCounts[magnitude]++;
        }
        
        if (!magnitudeCounts.empty()) {
            float peakMagnitude = magnitudeCounts.rbegin()->first;
            pImpl->features.push_back(peakMagnitude);
        }
    }
    
    pImpl->featureDimensions = pImpl->features.size();
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
