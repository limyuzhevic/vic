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
    if (!input.getData().empty()) {
        pImpl->features = input.getData();
        pImpl->featureDimensions = input.getDimensions();
    } else {
        // Reset features if input is empty
        pImpl->features.clear();
        pImpl->featureDimensions = 0;
    }
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
