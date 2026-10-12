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
    // TODO PHASE 2: Implement real audio processing using NLM's neural machinery
    // PLACEHOLDER: Just pass through raw data with bounds checking
    
    // Check for null input
    if (!input.getData()) {
        NLM_LOG_ERROR("AudioProcessor::process: Input data is null");
        return;
    }
    
    const auto& values = input.getData();
    size_t dimensions = input.getDimensions();
    
    // Bounds checking for audio data
    if (values.empty()) {
        NLM_LOG_ERROR("AudioProcessor::process: Input data is empty");
        return;
    }
    
    if (dimensions == 0) {
        NLM_LOG_ERROR("AudioProcessor::process: Invalid dimensions (0)");
        return;
    }
    
    // Check if the actual data size matches the dimensions
    if (values.size() != dimensions) {
        NLM_LOG_WARNING("AudioProcessor::process: Data size (" + std::to_string(values.size()) + ") does not match dimensions (" + std::to_string(dimensions) + ")");
    }
    
    // Store features with bounds checking
    pImpl->features.clear();
    pImpl->features.reserve(dimensions);
    
    for (size_t i = 0; i < dimensions; ++i) {
        // Bounds checking for array access
        if (i < values.size()) {
            pImpl->features.push_back(values[i]);
        } else {
            NLM_LOG_ERROR("AudioProcessor::process: Array bounds exceeded when accessing input data");
            break;
        }
    }
    
    pImpl->featureDimensions = dimensions;
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
