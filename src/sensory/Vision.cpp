#include "Vision.hpp"

namespace nlm {

struct Vision::Impl {
    size_t width;
    size_t height;
    size_t channels;
    std::vector<float> data;
    
    Impl() : width(0), height(0), channels(0) {}
    Impl(size_t w, size_t h, size_t c) : width(w), height(h), channels(c), data(w * h * c, 0.0f) {}
};

Vision::Vision() : pImpl(new Impl) {}

Vision::Vision(size_t width, size_t height, size_t channels) : pImpl(new Impl(width, height, channels)) {}

Vision::~Vision() = default;

const char* Vision::getType() const {
    return "Vision";
}

const std::vector<float>& Vision::getData() const {
    return pImpl->data;
}

size_t Vision::getDimensions() const {
    return pImpl->data.size();
}

std::unique_ptr<SensoryInput> Vision::clone() const {
    auto clone = std::make_unique<Vision>(pImpl->width, pImpl->height, pImpl->channels);
    clone->pImpl->data = pImpl->data;
    return clone;
}

void Vision::setData(const std::vector<float>& data) {
    if (data.size() == pImpl->data.size()) {
        pImpl->data = data;
    }
}

void Vision::setData(float* data, size_t size) {
    pImpl->data.assign(data, data + std::min(size, pImpl->data.size()));
}

size_t Vision::getWidth() const {
    return pImpl->width;
}

size_t Vision::getHeight() const {
    return pImpl->height;
}

size_t Vision::getChannels() const {
    return pImpl->channels;
}

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    Impl() : featureDimensions(0) {}
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    // TODO PHASE 2: Implement real vision processing using NLM's neural machinery
    // PLACEHOLDER: Just pass through raw data
    pImpl->features = input.getData();
    pImpl->featureDimensions = input.getDimensions();
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
