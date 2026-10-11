#include "Audio.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <complex>

namespace nlm {

struct AudioProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    float sampleRate;
    
    Impl() : featureDimensions(0), sampleRate(44100.0f) {}
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::process(const Audio& input) {
    // Implement real audio processing using NLM's neural machinery
    // Simple feature extraction for Phase 2
    
    const auto& data = input.getData();
    
    // Audio feature extraction
    size_t numSamples = input.getNumSamples();
    size_t sampleRate = input.getSampleRate();
    
    // 1. Basic statistics features
    float minVal = std::numeric_limits<float>::max();
    float maxVal = std::numeric_limits<float>::lowest();
    float sum = 0.0f;
    float sumSq = 0.0f;
    
    for (float val : data) {
        minVal = std::min(minVal, val);
        maxVal = std::max(maxVal, val);
        sum += val;
        sumSq += val * val;
    }
    
    float mean = numSamples > 0 ? sum / numSamples : 0.0f;
    float variance = numSamples > 0 ? (sumSq / numSamples) - (mean * mean) : 0.0f;
    float stdDev = std::sqrt(variance);
    
    // 2. Spectral features (simplified FFT)
    // For a full implementation, use proper FFT library
    std::vector<float> spectrum;
    if (numSamples >= 64) {
        // Simple periodogram approximation
        float binWidth = sampleRate / static_cast<float>(numSamples / 2);
        for (size_t bin = 0; bin < numSamples / 2; bin += 8) {
            float power = 0.0f;
            size_t windowSize = std::min(static_cast<size_t>(64), numSamples - bin);
            
            for (size_t i = 0; i < windowSize; ++i) {
                size_t idx = bin + i;
                if (idx < data.size()) {
                    power += data[idx] * data[idx];
                }
            }
            
            spectrum.push_back(power / windowSize);
        }
    }
    
    // 3. Zero-crossing rate
    size_t zeroCrossings = 0;
    for (size_t i = 1; i < numSamples; ++i) {
        if ((data[i-1] >= 0.0f && data[i] < 0.0f) || 
            (data[i-1] < 0.0f && data[i] >= 0.0f)) {
            zeroCrossings++;
        }
    }
    float zeroCrossingRate = numSamples > 0 ? static_cast<float>(zeroCrossings) / (2.0f * numSamples) : 0.0f;
    
    // 4. RMS (Root Mean Square) energy
    float rms = std::sqrt(variance);
    
    // 5. Crest factor (peak to RMS ratio)
    float crestFactor = rms > 0.0f ? (maxVal - minVal) / (2.0f * rms) : 0.0f;
    
    // 6. Harmonic content
    float fundamental = 0.0f;
    if (numSamples >= 1024) {
        // Simple pitch detection using autocorrelation
        float maxCorr = 0.0f;
        for (size_t lag = 100; lag < std::min(static_cast<size_t>(500), numSamples / 2); lag += 10) {
            float corr = 0.0f;
            for (size_t i = 0; i < numSamples - lag; ++i) {
                corr += data[i] * data[i + lag];
            }
            if (corr > maxCorr) {
                maxCorr = corr;
                fundamental = static_cast<float>(sampleRate) / lag;
            }
        }
    }
    
    // Build feature vector
    pImpl->features.clear();
    
    // Add statistical features
    pImpl->features.push_back(mean);
    pImpl->features.push_back(stdDev);
    pImpl->features.push_back(variance);
    pImpl->features.push_back(maxVal - minVal); // peak-to-peak
    pImpl->features.push_back(rms);
    pImpl->features.push_back(crestFactor);
    pImpl->features.push_back(zeroCrossingRate);
    
    // Add spectral features (top frequencies)
    if (!spectrum.empty()) {
        // Sort spectrum by power
        std::vector<size_t> indices(spectrum.size());
        for (size_t i = 0; i < spectrum.size(); ++i) indices[i] = i;
        
        std::sort(indices.begin(), indices.end(), 
                  [&spectrum](size_t a, size_t b) { return spectrum[a] > spectrum[b]; });
        
        // Add top 3 spectral peaks
        size_t numPeaks = std::min<size_t>(3, spectrum.size());
        for (size_t i = 0; i < numPeaks; ++i) {
            pImpl->features.push_back(spectrum[indices[i]]);
        }
        
        // Add spectral centroid
        float spectralCentroid = 0.0f;
        float totalPower = 0.0f;
        for (size_t i = 0; i < spectrum.size(); ++i) {
            spectralCentroid += static_cast<float>(i) * spectrum[i];
            totalPower += spectrum[i];
        }
        if (totalPower > 0.0f) {
            spectralCentroid /= totalPower;
            pImpl->features.push_back(spectralCentroid);
        } else {
            pImpl->features.push_back(0.0f);
        }
    } else {
        pImpl->features.push_back(0.0f); // placeholder for spectral features
        pImpl->features.push_back(0.0f);
    }
    
    // Add harmonic features
    pImpl->features.push_back(fundamental);
    pImpl->features.push_back(0.0f); // placeholder for harmonic complexity
    
    pImpl->featureDimensions = pImpl->features.size();
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
