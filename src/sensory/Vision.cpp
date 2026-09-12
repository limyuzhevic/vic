#include "SensoryInput.hpp"

namespace nlm {

// Vision implementation for prediction system
Vision::Vision(size_t width, size_t height, size_t channels) 
    : pImpl(new Impl(width, height, channels)) {}

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
    auto cloned = std::make_unique<Vision>(pImpl->width, pImpl->height, pImpl->channels);
    cloned->setData(pImpl->data);
    return cloned;
}

void Vision::setData(const std::vector<float>& data) {
    if (data.size() == pImpl->data.size()) {
        pImpl->data = data;
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