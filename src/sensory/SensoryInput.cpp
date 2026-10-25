#include "SensoryInput.hpp"

namespace nlm {

SensoryInput::SensoryInput() : timestamp_(0.0) {}

double SensoryInput::getTimestamp() const {
    return timestamp_;
}

void SensoryInput::setTimestamp(double timestamp) {
    timestamp_ = timestamp;
}

struct Vision::Impl {
    size_t width;
    size_t height;
    size_t channels;
    std::vector<float> data;
    
    Impl(size_t w, size_t h, size_t c) : width(w), height(h), channels(c), data(w * h * c, 0.0f) {}
};

Vision::Vision() : pImpl(new Impl(0, 0, 0)) {}

Vision::Vision(size_t width, size_t height, size_t channels) 
    : pImpl(new Impl(width, height, channels)) {}

Vision::~Vision() = default;

InputType Vision::getInputType() const {
    return InputType::Vision;
}

const char* Vision::getType() const {
    return "Vision";
}

const std::vector<float>& Vision::getData() const {
    return pImpl->data;
}

size_t Vision::getDimensions() const {
    return pImpl->width * pImpl->height * pImpl->channels;
}

std::unique_ptr<SensoryInput> Vision::clone() const {
    auto vision = std::make_unique<Vision>(pImpl->width, pImpl->height, pImpl->channels);
    vision->setData(pImpl->data);
    return vision;
}

void Vision::setData(const std::vector<float>& data) {
    if (data.size() == pImpl->data.size()) {
        pImpl->data = data;
    }
}

void Vision::setData(float* data, size_t size) {
    if (size == pImpl->data.size()) {
        pImpl->data.assign(data, data + size);
    }
}

float Vision::getIntensity() const {
    if (pImpl->data.empty()) return 0.0f;
    
    float maxAbs = 0.0f;
    for (float value : pImpl->data) {
        maxAbs = std::max(maxAbs, std::abs(value));
    }
    return maxAbs;
}

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

InputType Audio::getInputType() const {
    return InputType::Audio;
}

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

float Audio::getIntensity() const {
    if (pImpl->data.empty()) return 0.0f;
    
    float maxAbs = 0.0f;
    for (float value : pImpl->data) {
        maxAbs = std::max(maxAbs, std::abs(value));
    }
    return maxAbs;
}

struct InternalSignals::Impl {
    std::vector<float> signals;
    
    Impl() = default;
};

InternalSignals::InternalSignals() : pImpl(new Impl) {}

InternalSignals::~InternalSignals() = default;

InputType InternalSignals::getInputType() const {
    return InputType::InternalSignals;
}

const char* InternalSignals::getType() const {
    return "InternalSignals";
}

const std::vector<float>& InternalSignals::getData() const {
    return pImpl->signals;
}

size_t InternalSignals::getDimensions() const {
    return pImpl->signals.size();
}

std::unique_ptr<SensoryInput> InternalSignals::clone() const {
    auto clone = std::make_unique<InternalSignals>();
    for (float sig : pImpl->signals) {
        clone->addSignal(sig);
    }
    return clone;
}

void InternalSignals::addSignal(float value) {
    pImpl->signals.push_back(value);
}

float InternalSignals::getIntensity() const {
    if (pImpl->signals.empty()) return 0.0f;
    
    float maxAbs = 0.0f;
    for (float value : pImpl->signals) {
        maxAbs = std::max(maxAbs, std::abs(value));
    }
    return maxAbs;
}

} // namespace nlm
