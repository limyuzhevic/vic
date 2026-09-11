#include "Touch.hpp"

namespace nlm {

struct Touch::Impl {
    std::vector<float> data;
    
    Impl() : data(8, 0.0f) {}
};

Touch::Touch() : pImpl(new Impl) {}

Touch::~Touch() = default;

const char* Touch::getType() const {
    return "Touch";
}

const std::vector<float>& Touch::getData() const {
    return pImpl->data;
}

size_t Touch::getDimensions() const {
    return pImpl->data.size();
}

std::unique_ptr<SensoryInput> Touch::clone() const {
    auto touch = std::make_unique<Touch>();
    touch->setData(pImpl->data);
    return touch;
}

void Touch::setData(const std::vector<float>& data) {
    if (data.size() == pImpl->data.size()) {
        pImpl->data = data;
    }
}

size_t Touch::getNumSensors() const {
    return pImpl->data.size();
}

} // namespace nlm
