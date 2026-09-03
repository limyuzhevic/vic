#include "Observation.hpp"
#include "../agent/SensoryPercept.hpp"

namespace nlm {

Observation::Observation() {}

const std::vector<float>& Observation::getData() const {
    return data_;
}

size_t Observation::getDimensions() const {
    return data_.size();
}

void Observation::setFromPercept(const SensoryPercept& percept) {
    data_ = percept.getAllSignals();
}

} // namespace nlm
