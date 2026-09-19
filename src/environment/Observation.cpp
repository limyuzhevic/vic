#include "Observation.hpp"

namespace nlm {

EnvironmentObservation::EnvironmentObservation() = default;

EnvironmentObservation::EnvironmentObservation(const SensoryInput& input) {
    sensoryInput_ = input.clone();
}

EnvironmentObservation::~EnvironmentObservation() = default;

const SensoryInput* EnvironmentObservation::getSensoryInput() const {
    return sensoryInput_.get();
}

bool EnvironmentObservation::isValid() const {
    return sensoryInput_ != nullptr;
}

std::unique_ptr<EnvironmentObservation> EnvironmentObservation::clone() const {
    if (!sensoryInput_) {
        return std::make_unique<EnvironmentObservation>();
    }
    auto obs = std::make_unique<EnvironmentObservation>(*sensoryInput_);
    return obs;
}

} // namespace nlm
