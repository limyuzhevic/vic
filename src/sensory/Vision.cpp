#include "Vision.hpp"
#include <algorithm>

namespace nlm {

Vision::Vision() : width_(0), height_(0), channels_(0), timestamp_(0.0) {}

Vision::Vision(size_t width, size_t height, size_t channels) 
    : width_(width), height_(height), channels_(channels), timestamp_(0.0) {
    size_t totalPixels = width * height * channels;
    data_.resize(totalPixels);
}

NeuronType Vision::getType() const {
    return NeuronType::Sensory;
}

const std::vector<float>& Vision::getData() const {
    return data_;
}

size_t Vision::getDimensions() const {
    return data_.size();
}

Timestamp Vision::getTimestamp() const {
    return timestamp_;
}

void Vision::setTimestamp(Timestamp timestamp) {
    timestamp_ = timestamp;
}

void Vision::reset() {
    std::fill(data_.begin(), data_.end(), 0.0f);
    timestamp_ = 0.0;
}

size_t Vision::getWidth() const {
    return width_;
}

size_t Vision::getHeight() const {
    return height_;
}

size_t Vision::getChannels() const {
    return channels_;
}

void Vision::setData(const std::vector<float>& data) {
    data_ = data;
}

} // namespace nlm