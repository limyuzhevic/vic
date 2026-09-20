#include "Vision.hpp"

namespace nlm {

Vision::Vision() : width(0), height(0) {}

Vision::Vision(size_t w, size_t h, size_t c) : width(w), height(h) {
    data.resize(w * h * c, 0.0f);
}

const char* Vision::getType() const {
    return "Vision";
}

const std::vector<float>& Vision::getData() const {
    return data;
}

size_t Vision::getDimensions() const {
    return data.size();
}

std::unique_ptr<SensoryInput> Vision::clone() const {
    auto clone = std::make_unique<Vision>(width, height, 3);
    clone->setData(this->data);
    return clone;
}

void Vision::setData(const std::vector<float>& inputData) {
    data = inputData;
}

void Vision::setData(float* inputData, size_t size) {
    data.assign(inputData, inputData + size);
}

size_t Vision::getWidth() const { return width; }
size_t Vision::getHeight() const { return height; }
size_t Vision::getChannels() const { return channels; }

} // namespace nlm