#include "SensoryInput.hpp"
#include <algorithm>

namespace nlm {

struct Audio::Impl {
    size_t sampleRate;
    size_t numSamples;
    std::vector<float> data;
    
    Impl(size_t rate, size_t samples) : sampleRate(rate), numSamples(samples), data(samples, 0.0f) {}
};

Audio::Audio() : pImpl(new Impl(0, 0)) {}

Audio::Audio(size_t sampleRate, size_t numSamples) 
    : pImpl(new Impl(sampleRate, numSamples)) {}

Audio::~Audio() = default;

const char* Audio::getType() const {
    return "Audio";
}

const std::vector<float>& Audio::getData() const {
    return pImpl->data;
}

size_t Audio::getDimensions() const {
    return pImpl->numSamples;
}

std::unique_ptr<SensoryInput> Audio::clone() const {
    auto audio = std::make_unique<Audio>(pImpl->sampleRate, pImpl->numSamples);
    audio->setData(pImpl->data);
    return audio;
}

void Audio::setData(const std::vector<float>& data) {
    if (data.size() == pImpl->data.size()) {
        pImpl->data = data;
    }
}

void Audio::setSampleRate(size_t rate) {
    pImpl->sampleRate = rate;
}

size_t Audio::getSampleRate() const { return pImpl->sampleRate; }
size_t Audio::getNumSamples() const { return pImpl->numSamples; }

} // namespace nlm