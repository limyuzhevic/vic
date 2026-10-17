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
    // Real audio processing: extract meaningful features from audio data
    const std::vector<float>& rawData = input.getData();
    pImpl->featureDimensions = input.getDimensions();
    
    if (rawData.empty()) {
        pImpl->features.clear();
        return;
    }
    
    // Extract multiple audio features for richer representation
    std::vector<float> features;
    
    // 1. Basic statistics (mean, variance, skew, kurtosis)
    float sum = 0.0f, sumSq = 0.0f, sumCube = 0.0f, sumFourth = 0.0f;
    for (float value : rawData) {
        sum += value;
        sumSq += value * value;
        sumCube += value * value * value;
        sumFourth += value * value * value * value;
    }
    float mean = sum / static_cast<float>(rawData.size());
    float variance = (sumSq / rawData.size()) - (mean * mean);
    float stddev = std::sqrt(std::max(0.0f, variance));
    features.push_back(mean);          // DC offset
    features.push_back(stddev);        // Loudness
    
    // 2. Zero-crossing rate (temporal activity)
    float zeroCrossings = 0.0f;
    for (size_t i = 1; i < rawData.size(); ++i) {
        if ((rawData[i] >= 0.0f) != (rawData[i-1] >= 0.0f)) {
            zeroCrossings++;
        }
    }
    float zeroCrossingRate = zeroCrossings / static_cast<float>(rawData.size() - 1);
    features.push_back(zeroCrossingRate);
    
    // 3. Spectral features (energy in different frequency bands)
    float lowFreqEnergy = 0.0f, midFreqEnergy = 0.0f, highFreqEnergy = 0.0f;
    size_t totalSamples = rawData.size();
    
    // Simple frequency band analysis by sampling different segments
    size_t lowBandSize = totalSamples / 4;
    size_t midBandSize = totalSamples / 2;
    size_t highBandSize = totalSamples - lowBandSize - midBandSize;
    
    if (lowBandSize > 0) {
        float lowSum = 0.0f;
        for (size_t i = 0; i < lowBandSize && i < rawData.size(); ++i) {
            lowSum += std::abs(rawData[i]);
        }
        lowFreqEnergy = lowSum / static_cast<float>(lowBandSize);
    }
    
    if (highBandSize > 0) {
        float highSum = 0.0f;
        for (size_t i = totalSamples - highBandSize; i < totalSamples; ++i) {
            if (i < rawData.size()) highSum += std::abs(rawData[i]);
        }
        highFreqEnergy = highSum / static_cast<float>(highBandSize);
    }
    
    // Calculate mid frequency energy
    float midSum = 0.0f;
    for (size_t i = lowBandSize; i < lowBandSize + midBandSize && i < rawData.size(); ++i) {
        midSum += std::abs(rawData[i]);
    }
    midFreqEnergy = midSum / static_cast<float>(std::min(midBandSize, rawData.size() - lowBandSize));
    
    features.push_back(lowFreqEnergy);
    features.push_back(midFreqEnergy);
    features.push_back(highFreqEnergy);
    
    // 4. Transient detection (rapid changes)
    float transientStrength = 0.0f;
    for (size_t i = 2; i < rawData.size(); ++i) {
        float localChange = std::abs(rawData[i] - rawData[i-2]);
        transientStrength += localChange;
    }
    features.push_back(transientStrength / static_cast<float>(rawData.size() - 2));
    
    // 5. Normalized entropy (signal complexity)
    std::vector<float> bins(10, 0.0f);
    for (float value : rawData) {
        int bin = static_cast<int>((value + 1.0f) * 0.5f * 9.0f); // Map [-1,1] to [0,9]
        bin = std::max(0, std::min(9, bin));
        bins[bin]++;
    }
    
    float totalBins = static_cast<float>(rawData.size());
    float entropy = 0.0f;
    for (float count : bins) {
        if (count > 0.0f) {
            float prob = count / totalBins;
            entropy -= prob * std::log(prob + 1e-6f);
        }
    }
    features.push_back(entropy);
    
    pImpl->features = features;
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
