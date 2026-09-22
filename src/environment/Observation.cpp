#include "Observation.hpp"
#include "SensoryInput.hpp"

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

// Additional utility methods for better integration
void Observation::update(const SensoryInput& input) {
    sensoryInput_ = input.clone();
}

const SensoryInput& Observation::getSensoryInput() {
    static SensoryInput emptyInput;
    return sensoryInput_ ? *sensoryInput_ : emptyInput;
}

const float* Observation::getVisionData() const {
    if (sensoryInput_) {
        return sensoryInput_->getVisionData();
    }
    return nullptr;
}

const float* Observation::getTouchData() const {
    if (sensoryInput_) {
        return sensoryInput_->getTouchData();
    }
    return nullptr;
}

const float* Observation::getInternalData() const {
    if (sensoryInput_) {
        return sensoryInput_->getInternalData();
    }
    return nullptr;
}

const float* Observation::getProprioceptionData() const {
    if (sensoryInput_) {
        return sensoryInput_->getProprioceptionData();
    }
    return nullptr;
}

int Observation::getVisionWidth() const {
    if (sensoryInput_) {
        return sensoryInput_->getVisionWidth();
    }
    return 0;
}

int Observation::getVisionHeight() const {
    if (sensoryInput_) {
        return sensoryInput_->getVisionHeight();
    }
    return 0;
}

int Observation::getTouchCount() const {
    if (sensoryInput_) {
        return sensoryInput_->getTouchCount();
    }
    return 0;
}

int Observation::getInternalCount() const {
    if (sensoryInput_) {
        return sensoryInput_->getInternalCount();
    }
    return 0;
}

int Observation::getProprioceptionCount() const {
    if (sensoryInput_) {
        return sensoryInput_->getProprioceptionCount();
    }
    return 0;
}

float Observation::getMaxEnergy() const {
    if (sensoryInput_) {
        return sensoryInput_->getMaxEnergy();
    }
    return 0.0f;
}

bool Observation::isHazard(int x, int y) const {
    if (sensoryInput_) {
        return sensoryInput_->isHazard(x, y);
    }
    return false;
}

float Observation::getEnergyAt(int x, int y) const {
    if (sensoryInput_) {
        return sensoryInput_->getEnergyAt(x, y);
    }
    return 0.0f;
}

} // namespace nlm
