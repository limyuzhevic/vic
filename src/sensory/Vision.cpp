// Vision implementation for NLM - Vision.cpp
// This implements the Vision class defined in SensoryInput.hpp

#include "Vision.hpp"

namespace nlm {

struct Vision::Impl {
    std::vector<float> data_;
    size_t width_;
    size_t height_;
    size_t channels_;
    
    Impl() : width_(0), height_(0), channels_(0) {}
};

Vision::Vision() : pImpl(new Impl) {}

Vision::Vision(size_t width, size_t height, size_t channels)
    : pImpl(new Impl) {
    pImpl->width_ = width;
    pImpl->height_ = height;
    pImpl->channels_ = channels;
    pImpl->data_.resize(width * height * channels, 0.0f);
}

Vision::~Vision() = default;

const char* Vision::getType() const {
    return "Vision";
}

const std::vector<float>& Vision::getData() const {
    return pImpl->data_;
}

size_t Vision::getDimensions() const {
    return pImpl->width_ * pImpl->height_ * pImpl->channels_;
}

std::unique_ptr<SensoryInput> Vision::clone() const {
    auto clone = std::make_unique<Vision>(pImpl->width_, pImpl->height_, pImpl->channels_);
    clone->pImpl->data_ = pImpl->data_;
    clone->setTimestamp(timestamp_);
    return clone;
}

void Vision::setData(const std::vector<float>& data) {
    if (data.size() != pImpl->data_.size()) {
        pImpl->data_.resize(data.size());
    }
    std::copy(data.begin(), data.end(), pImpl->data_.begin());
}

void Vision::setData(float* data, size_t size) {
    if (size != pImpl->data_.size()) {
        pImpl->data_.resize(size);
    }
    std::copy(data, data + size, pImpl->data_.begin());
}

size_t Vision::getWidth() const {
    return pImpl->width_;
}

size_t Vision::getHeight() const {
    return pImpl->height_;
}

size_t Vision::getChannels() const {
    return pImpl->channels_;
}

} // namespace nlm