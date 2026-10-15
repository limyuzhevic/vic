// Audio implementation for NLM - Audio.cpp
// This implements the Audio class defined in SensoryInput.hpp

#include "Audio.hpp"

namespace nlm {

struct Audio::Impl {
    std::vector<float> data_;
    size_t sampleRate_;
    size_t numSamples_;
    
    Impl() : sampleRate_(0), numSamples_(0) {}
};

Audio::Audio() : pImpl(new Impl) {}

Audio::Audio(size_t sampleRate, size_t numSamples)
    : pImpl(new Impl) {
    pImpl->sampleRate_ = sampleRate;
    pImpl->numSamples_ = numSamples;
    pImpl->data_.resize(numSamples, 0.0f);
}

Audio::~Audio() = default;

const char* Audio::getType() const {
    return "Audio";
}

const std::vector<float>& Audio::getData() const {
    return pImpl->data_;
}

size_t Audio::getDimensions() const {
    return pImpl->numSamples_;
}

std::unique_ptr<SensoryInput> Audio::clone() const {
    auto clone = std::make_unique<Audio>(pImpl->sampleRate_, pImpl->numSamples_);
    clone->pImpl->data_ = pImpl->data_;
    clone->setTimestamp(timestamp_);
    return clone;
}

void Audio::setData(const std::vector<float>& data) {
    if (data.size() != pImpl->data_.size()) {
        pImpl->data_.resize(data.size());
    }
    std::copy(data.begin(), data.end(), pImpl->data_.begin());
}

void Audio::setSampleRate(size_t rate) {
    pImpl->sampleRate_ = rate;
}

size_t Audio::getSampleRate() const {
    return pImpl->sampleRate_;
}

size_t Audio::getNumSamples() const {
    return pImpl->numSamples_;
}

} // namespace nlm