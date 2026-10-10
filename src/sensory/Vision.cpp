#include "Vision.hpp"
#include "core/Random/Random.hpp"
#include <algorithm>

namespace nlm {

struct Vision::Impl {
    std::vector<float> data;
    size_t width;
    size_t height;
    size_t channels;
    
    Impl() : width(0), height(0), channels(0) {}
};

Vision::Vision() : pImpl(new Impl) {}

Vision::Vision(size_t width, size_t height, size_t channels) : pImpl(new Impl) {
    pImpl->width = width;
    pImpl->height = height;
    pImpl->channels = channels;
    pImpl->data.resize(width * height * channels, 0.0f);
}

Vision::~Vision() = default;

Vision::Vision(Vision&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Vision& Vision::operator=(Vision&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

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
    auto clone = std::make_unique<Vision>(*this);
    clone->setTimestamp(getTimestamp());
    return clone;
}

void Vision::setData(const std::vector<float>& data) {
    pImpl->data = data;
    // Calculate dimensions
    pImpl->width = 0;
    pImpl->height = 0;
    pImpl->channels = 0;
    
    if (!pImpl->data.empty()) {
        // Simple heuristic: assume square image if possible
        if (pImpl->channels > 0) {
            size_t total_pixels = pImpl->data.size() / pImpl->channels;
            pImpl->width = static_cast<size_t>(std::sqrt(total_pixels));
            pImpl->height = total_pixels / pImpl->width;
        }
    }
}

void Vision::setData(float* data, size_t size) {
    pImpl->data.assign(data, data + size);
    // Calculate dimensions (simplified)
    pImpl->width = 0;
    pImpl->height = 0;
    pImpl->channels = 3; // Default to RGB
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
