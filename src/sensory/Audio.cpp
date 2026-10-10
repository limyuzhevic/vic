#include "Audio.hpp"

namespace nlm {

struct Audio::Impl {
    std::vector<float> data;
    size_t sampleRate;
    size_t numSamples;
    
    Impl() : sampleRate(0), numSamples(0) {}
};

Audio::Audio() : pImpl(new Impl) {}

Audio::Audio(size_t sampleRate, size_t numSamples) : pImpl(new Impl) {
    pImpl->sampleRate = sampleRate;
    pImpl->numSamples = numSamples;
    pImpl->data.resize(numSamples, 0.0f);
}

Audio::~Audio() = default;

Audio::Audio(Audio&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Audio& Audio::operator=(Audio&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

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
    auto clone = std::make_unique<Audio>(*this);
    clone->setTimestamp(getTimestamp());
    return clone;
}

void Audio::setData(const std::vector<float>& data) {
    pImpl->data = data;
    pImpl->numSamples = data.size();
}

void Audio::setSampleRate(size_t rate) {
    pImpl->sampleRate = rate;
}

size_t Audio::getSampleRate() const {
    return pImpl->sampleRate;
}

size_t Audio::getNumSamples() const {
    return pImpl->numSamples;
}

} // namespace nlm
