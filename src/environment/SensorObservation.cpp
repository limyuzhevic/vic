#include "SensoryInput.hpp"

namespace nlm {

SensorObservation::SensorObservation() = default;

SensorObservation::SensorObservation(const SensoryInput& input) {
    sensoryInput_ = input.clone();
}

SensorObservation::~SensorObservation() = default;

const SensoryInput* SensorObservation::getSensoryInput() const {
    return sensoryInput_.get();
}

bool SensorObservation::isValid() const {
    return sensoryInput_ != nullptr;
}

std::unique_ptr<SensorObservation> SensorObservation::clone() const {
    if (!sensoryInput_) {
        return std::make_unique<SensorObservation>();
    }
    auto obs = std::make_unique<SensorObservation>(*sensoryInput_);
    return obs;
}

} // namespace nlm
