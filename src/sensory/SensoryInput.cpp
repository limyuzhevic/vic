#include "SensoryInput.hpp"

namespace nlm {

SensoryInput::SensoryInput() : timestamp_(0.0), has_error_(false) {}

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
    mutable std::string error_message_;
    bool has_error_;
    
    Impl(size_t w, size_t h, size_t c) : width(w), height(h), channels(c), data(w * h * c, 0.0f), has_error_(false) {}
};

Vision::Vision() : pImpl(new Impl(0, 0, 0)) {}

Vision::Vision(size_t width, size_t height, size_t channels) 
    : pImpl(new Impl(width, height, channels)) {}

Vision::~Vision() = default;

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
        pImpl->has_error_ = false;
        pImpl->error_message_.clear();
    } else {
        pImpl->has_error_ = true;
        pImpl->error_message_ = "Data size mismatch: expected " + std::to_string(pImpl->data.size()) + 
                                " elements, got " + std::to_string(data.size());
    }
}

void Vision::setData(float* data, size_t size) {
    if (size == pImpl->data.size()) {
        pImpl->data.assign(data, data + size);
        pImpl->has_error_ = false;
        pImpl->error_message_.clear();
    } else {
        pImpl->has_error_ = true;
        pImpl->error_message_ = "Data size mismatch: expected " + std::to_string(pImpl->data.size()) + 
                                " elements, got " + std::to_string(size);
    }
}

size_t Vision::getWidth() const { return pImpl->width; }
size_t Vision::getHeight() const { return pImpl->height; }
size_t Vision::getChannels() const { return pImpl->channels; }

struct Audio::Impl {
    size_t sampleRate;
    size_t numSamples;
    std::vector<float> data;
    mutable std::string error_message_;
    bool has_error_;
    
    Impl(size_t rate, size_t samples) : sampleRate(rate), numSamples(samples), data(samples, 0.0f), has_error_(false) {}
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
        pImpl->has_error_ = false;
        pImpl->error_message_.clear();
    } else {
        pImpl->has_error_ = true;
        pImpl->error_message_ = "Data size mismatch: expected " + std::to_string(pImpl->data.size()) + 
                                " elements, got " + std::to_string(data.size());
    }
}

void Audio::setSampleRate(size_t rate) {
    pImpl->sampleRate = rate;
}

size_t Audio::getSampleRate() const { return pImpl->sampleRate; }
size_t Audio::getNumSamples() const { return pImpl->numSamples; }

// Audio-specific methods
void Audio::applyGain(float gainDb) {
    float gainFactor = std::pow(10.0f, gainDb / 20.0f); // Convert dB to linear factor
    for (float& sample : pImpl->data) {
        sample *= gainFactor;
    }
    pImpl->has_error_ = false;
    pImpl->error_message_.clear();
}

float Audio::getDuration() const {
    if (pImpl->sampleRate == 0) return 0.0f;
    return static_cast<float>(pImpl->numSamples) / static_cast<float>(pImpl->sampleRate);
}

std::vector<float> Audio::getFrequency() const {
    // Simple FFT approximation for demonstration
    std::vector<float> frequencies;
    size_t windowSize = std::min(static_cast<size_t>(1024), pImpl->numSamples);
    
    if (windowSize < 2) {
        pImpl->has_error_ = true;
        pImpl->error_message_ = "Not enough data for frequency analysis";
        return frequencies;
    }
    
    // Simple energy calculation for demonstration
    frequencies.resize(windowSize / 2, 0.0f);
    for (size_t i = 0; i < windowSize / 2; ++i) {
        frequencies[i] = std::abs(pImpl->data[i]);
    }
    
    pImpl->has_error_ = false;
    pImpl->error_message_.clear();
    return frequencies;
}

struct InternalSignals::Impl {
    std::vector<float> signals;
    
    Impl() = default;
};

InternalSignals::InternalSignals() : pImpl(new Impl) {}

InternalSignals::~InternalSignals() = default;

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

void InternalSignals::clearSignals() {
    pImpl->signals.clear();
}

} // namespace nlm
