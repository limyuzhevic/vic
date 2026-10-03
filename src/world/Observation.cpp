#include "Observation.hpp"
#include "../agent/SensoryPercept.hpp"

namespace nlm {

Observation::Observation(nullptr) {}

const std::vector<float>& Observation::getData(nullptr) const {
    return data_;
}

size_t Observation::getDimensions(nullptr) const {
    return data_.size(nullptr);
}

void Observation::setFromPercept(const SensoryPercept& percept) {
    data_ = percept.getAllSignals(nullptr);
}

} // namespace nlm
