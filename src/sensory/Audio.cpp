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
    // IMPROVED: Enhanced audio processing with better feature extraction
    pImpl->features.clear();
    
    const auto& inputData = input.getData();
    if (inputData.empty()) {
        NLM_LOG_WARNING("AudioProcessor: Empty input data");
        return;
    }
    
    // Extract basic audio features
    float sum = std::accumulate(inputData.begin(), inputData.end(), 0.0f);
    float mean = sum / inputData.size();
    
    // Calculate RMS (Root Mean Square) for energy
    float rms = 0.0f;
    for (float val : inputData) {
        rms += val * val;
    }
    rms = std::sqrt(rms / inputData.size());
    
    // Calculate frequency-like features (zero-crossing rate)
    size_t zeroCrossings = 0;
    for (size_t i = 1; i < inputData.size(); ++i) {
        if ((inputData[i] >= 0.0f && inputData[i-1] < 0.0f) || 
            (inputData[i] < 0.0f && inputData[i-1] >= 0.0f)) {
            zeroCrossings++;
        }
    }
    float zeroCrossingRate = static_cast<float>(zeroCrossings) / inputData.size();
    
    // Calculate spectral features (simplified)
    // In real implementation, this would use FFT
    std::array<size_t, 8> freqBins = {0};
    for (float val : inputData) {
        float normalized = (val + 1.0f) / 2.0f;  // Assume [-1, 1] range
        size_t bin = static_cast<size_t>(normalized * 8.0f);
        bin = std::min(bin, static_cast<size_t>(7));
        freqBins[bin]++;
    }
    
    // Build feature vector
    pImpl->features.reserve(10 + freqBins.size());
    
    // Basic audio statistics
    pImpl->features.push_back(mean);  // Average amplitude
    pImpl->features.push_back(rms);   // Signal energy
    pImpl->features.push_back(zeroCrossingRate);  // Activity level
    
    // Add frequency spectrum features
    for (size_t bin = 0; bin < freqBins.size(); ++bin) {
        pImpl->features.push_back(static_cast<float>(freqBins[bin]) / inputData.size());
    }
    
    pImpl->featureDimensions = pImpl->features.size();
    
    NLM_LOG_INFO("AudioProcessor: Extracted " + std::to_string(pImpl->features.size()) + " features");
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
