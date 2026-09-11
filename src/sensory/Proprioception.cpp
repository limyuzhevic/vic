#include "Proprioception.hpp"

namespace nlm {

struct Proprioception::Impl {
    std::vector<float> data;
    
    Impl() : data(6, 0.0f) {}
};

Proprioception::Proprioception() : pImpl(new Impl) {}

Proprioception::~Proprioception() = default;

const char* Proprioception::getType() const {
    return "Proprioception";
}

const std::vector<float>& Proprioception::getData() const {
    return pImpl->data;
}

size_t Proprioception::getDimensions() const {
    return pImpl->data.size();
}

std::unique_ptr<SensoryInput> Proprioception::clone() const {
    auto proprio = std::make_unique<Proprioception>();
    proprio->setData(pImpl->data);
    return proprio;
}

void Proprioception::setData(const std::vector<float>& data) {
    if (data.size() == pImpl->data.size()) {
        pImpl->data = data;
    }
}

size_t Proprioception::getNumJointSignals() const {
    return pImpl->data.size();
}

} // namespace nlm
