#include "Audio.hpp"
#include "AudioProcessor.hpp"
#include <vector>
#include <algorithm>

namespace nlm {

struct Audio::Impl {
    std::vector<float> data;
    size_t numSamples;
    
    Impl() : numSamples(0) {}
    Impl(size_t samples) : numSamples(samples) {
        data.resize(samples, 0.0f);
    }
};

Audio::Audio() : pImpl(std::make_unique<Impl>()) {}

Audio::Audio(size_t numSamples) : pImpl(std::make_unique<Impl>(numSamples)) {}

Audio::~Audio() = default;

const char* Audio::getType() const {
    return "Audio";
}

const std::vector<float>& Audio::getData() const {
    return pImpl->data;
}

size_t Audio::getDimensions() const {
    return pImpl->data.size();
}

std::unique_ptr<SensoryInput> Audio::clone() const {
    auto cloned = std::make_unique<Audio>(pImpl->numSamples);
    cloned->pImpl->data = pImpl->data;
    return cloned;
}

void Audio::setData(const std::vector<float>& inputData) {
    pImpl->data = inputData;
    pImpl->numSamples = inputData.size();
}

void Audio::setData(float* data, size_t size) {
    pImpl->data.assign(data, data + size);
    pImpl->numSamples = size;
}

size_t Audio::getNumSamples() const {
    return pImpl->numSamples;
}

} // namespace nlm