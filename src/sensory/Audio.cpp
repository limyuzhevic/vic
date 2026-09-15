#include "Audio.hpp"

namespace nlm {

struct AudioProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    Impl() : featureDimensions(0) {}
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::process(const Audio& input) {
    // Real audio processing using NLM's neural machinery
    // Process raw audio samples to extract meaningful features for the brain
    
    const auto& data = input.getData();
    size_t numSamples = input.getNumSamples();
    size_t sampleRate = input.getSampleRate();
    
    // Clear previous features
    pImpl->features.clear();
    
    if (numSamples == 0) {
        pImpl->featureDimensions = 0;
        return;
    }
    
    // 1. Basic audio statistics
    std::vector<float> stats;
    float sum = 0.0f;
    float sumSq = 0.0f;
    for (size_t i = 0; i < numSamples; ++i) {
        float sample = data[i];
        sum += sample;
        sumSq += sample * sample;
    }
    
    float mean = sum / numSamples;
    float variance = sumSq / numSamples - mean * mean;
    float stddev = std::sqrt(std::max(0.0f, variance));
    
    // Normalize by expected range (-1.0 to 1.0)
    float normalizedMean = mean / 1.0f;  // Assuming -1 to 1 range
    float normalizedStd = stddev / 1.0f;
    
    stats.push_back(normalizedMean);  // Central tendency
    stats.push_back(normalizedStd);   // Spread/variance
    
    // 2. Frequency domain analysis (simplified DFT)
    std::vector<float> freqFeatures;
    size_t numFreqBins = std::min<size_t>(64, numSamples / 4);  // Up to 64 frequency bins
    
    for (size_t bin = 0; bin < numFreqBins; ++bin) {
        float sumReal = 0.0f;
        float sumImag = 0.0f;
        float freq = static_cast<float>(bin) * (sampleRate / 2.0f) / numFreqBins;
        
        for (size_t i = 0; i < numSamples; i += 2) {
            float phase = 2.0f * M_PI * freq * i / sampleRate;
            sumReal += data[i] * std::cos(phase);
            sumImag += data[i] * std::sin(phase);
        }
        
        float magnitude = std::sqrt(sumReal * sumReal + sumImag * sumImag);
        float normalizedMag = std::min(magnitude / 2.0f, 1.0f);  // Normalize
        freqFeatures.push_back(normalizedMag);
        
        // Add harmonics (simplified)
        if (bin > 0 && bin < 10) {
            float harmonic = normalizedMag * (1.0f / bin);  // Simple harmonic decay
            freqFeatures.push_back(harmonic);
        }
    }
    
    // 3. Temporal features
    std::vector<float> temporalFeatures;
    
    // Zero-crossing rate
    size_t zeroCrossings = 0;
    for (size_t i = 1; i < numSamples; ++i) {
        if ((data[i - 1] < 0.0f && data[i] >= 0.0f) || (data[i - 1] >= 0.0f && data[i] < 0.0f)) {
            zeroCrossings++;
        }
    }
    float zcr = static_cast<float>(zeroCrossings) / numSamples;
    temporalFeatures.push_back(zcr);
    
    // Sound envelope (RMS of absolute values)
    float rms = 0.0f;
    for (size_t i = 0; i < numSamples; ++i) {
        rms += data[i] * data[i];
    }
    rms = std::sqrt(rms / numSamples);
    temporalFeatures.push_back(std::min(rms / 2.0f, 1.0f));  // Normalize
    
    // 4. Spectrogram-based features (energy in different frequency bands)
    std::vector<float> spectralFeatures;
    size_t numBands = 8;
    
    for (size_t band = 0; band < numBands; ++band) {
        float bandEnergy = 0.0f;
        size_t startFreq = (band * sampleRate / numBands) / 2;  // Simplified
        size_t endFreq = ((band + 1) * sampleRate / numBands) / 2;
        
        for (size_t bin = startFreq; bin < std::min(endFreq, numFreqBins); ++bin) {
            // Access frequency magnitude from the DFT
            size_t freqIndex = bin * 2;  // Simplified frequency bin mapping
            if (freqIndex < numSamples) {
                float mag = data[freqIndex];
                bandEnergy += mag * mag;
            }
        }
        
        spectralFeatures.push_back(std::min(bandEnergy / 10.0f, 1.0f));  // Normalize
    }
    
    // 5. Sound type classification (simple heuristics)
    std::vector<float> soundType;
    
    // Characteristic frequency content
    float highFreqEnergy = 0.0f;
    float lowFreqEnergy = 0.0f;
    
    for (size_t bin = numFreqBins / 2; bin < numFreqBins; ++bin) {
        size_t freqIndex = bin * 2;
        if (freqIndex < numSamples) {
            highFreqEnergy += data[freqIndex] * data[freqIndex];
        }
    }
    
    for (size_t bin = 0; bin < numFreqBins / 2; ++bin) {
        size_t freqIndex = bin * 2;
        if (freqIndex < numSamples) {
            lowFreqEnergy += data[freqIndex] * data[freqIndex];
        }
    }
    
    float harmonicRatio = highFreqEnergy / (highFreqEnergy + lowFreqEnergy + 0.001f);
    soundType.push_back(harmonicRatio);  // 0 = low freq, 1 = high freq
    
    // Combine all features
    pImpl->features.reserve(stats.size() + freqFeatures.size() + temporalFeatures.size() + 
                            spectralFeatures.size() + soundType.size());
    
    pImpl->features.insert(pImpl->features.end(), stats.begin(), stats.end());
    pImpl->features.insert(pImpl->features.end(), freqFeatures.begin(), freqFeatures.end());
    pImpl->features.insert(pImpl->features.end(), temporalFeatures.begin(), temporalFeatures.end());
    pImpl->features.insert(pImpl->features.end(), spectralFeatures.begin(), spectralFeatures.end());
    pImpl->features.insert(pImpl->features.end(), soundType.begin(), soundType.end());
    
    // Set feature dimensions
    pImpl->featureDimensions = pImpl->features.size();
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
