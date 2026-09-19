#include "Vision.hpp"
#include "VisionProcessor.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Vision::Impl {
    std::vector<float> data;
    size_t width;
    size_t height;
    size_t channels;
    
    Impl() : width(0), height(0), channels(0) {}
    Impl(size_t w, size_t h, size_t c) : width(w), height(h), channels(c) {
        data.resize(w * h * c, 0.0f);
    }
};

Vision::Vision() : pImpl(std::make_unique<Impl>()) {}

Vision::Vision(size_t width, size_t height, size_t channels) : pImpl(std::make_unique<Impl>(width, height, channels)) {}

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
    auto cloned = std::make_unique<Vision>(pImpl->width, pImpl->height, pImpl->channels);
    cloned->pImpl->data = pImpl->data;
    return cloned;
}

void Vision::setData(const std::vector<float>& inputData) {
    pImpl->data = inputData;
    // Calculate dimensions based on data size
    if (!inputData.empty()) {
        pImpl->channels = 3; // Assume RGB
        // Calculate square dimensions
        double sqrtValue = std::sqrt(static_cast<double>(inputData.size()) / pImpl->channels);
        pImpl->width = static_cast<size_t>(sqrtValue);
        pImpl->height = pImpl->width;
        
        // If we have non-square aspect ratio, adjust height
        size_t expectedSize = pImpl->width * pImpl->height * pImpl->channels;
        if (expectedSize != inputData.size()) {
            // Try to find the closest valid dimensions
            for (size_t h = 1; h <= inputData.size() / pImpl->channels; ++h) {
                size_t w = inputData.size() / (pImpl->channels * h);
                if (w > 0 && w * h * pImpl->channels == inputData.size()) {
                    pImpl->width = w;
                    pImpl->height = h;
                    break;
                }
            }
        }
    }
}

void Vision::setData(float* data, size_t size) {
    pImpl->data.assign(data, data + size);
    // Calculate dimensions similarly
    if (size > 0) {
        pImpl->channels = 3;
        // Calculate square dimensions
        double sqrtValue = std::sqrt(static_cast<double>(size) / pImpl->channels);
        pImpl->width = static_cast<size_t>(sqrtValue);
        pImpl->height = pImpl->width;
        
        // If we have non-square aspect ratio, adjust height
        size_t expectedSize = pImpl->width * pImpl->height * pImpl->channels;
        if (expectedSize != size) {
            // Try to find the closest valid dimensions
            for (size_t h = 1; h <= size / pImpl->channels; ++h) {
                size_t w = size / (pImpl->channels * h);
                if (w > 0 && w * h * pImpl->channels == size) {
                    pImpl->width = w;
                    pImpl->height = h;
                    break;
                }
            }
        }
    }
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

} // namespace nlm