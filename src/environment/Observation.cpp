#include "Observation.hpp"

namespace nlm {

Observation::Observation() = default;

Observation::Observation(const SensoryInput& input) {
    sensoryInput_ = input.clone();
}

Observation::~Observation() = default;

const SensoryInput* Observation::getSensoryInput() const {
    return sensoryInput_.get();
}

bool Observation::isValid() const {
    return sensoryInput_ != nullptr;
}

std::unique_ptr<Observation> Observation::clone() const {
    if (!sensoryInput_) {
        return std::make_unique<Observation>();
    }
    auto obs = std::make_unique<Observation>(*sensoryInput_);
    return obs;
}

} // namespace nlm
