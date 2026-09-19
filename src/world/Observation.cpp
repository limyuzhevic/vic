#include "Observation.hpp"
#include "../agent/SensoryPercept.hpp"

namespace nlm {

WorldObservation::WorldObservation() {}

const std::vector<float>& WorldObservation::getData() const {
    return data_;
}

size_t WorldObservation::getDimensions() const {
    return data_.size();
}

void WorldObservation::setFromPercept(const SensoryPercept& percept) {
    data_ = percept.getAllSignals();
}

} // namespace nlm
