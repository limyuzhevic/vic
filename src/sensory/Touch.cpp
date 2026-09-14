#include "Touch.hpp"
#include <memory>
#include <vector>

namespace nlm {

struct Touch::Impl {
    std::vector<float> data_;
    size_t numSensors_;
};

Touch::Touch() : pImpl(std::make_unique<Impl>()) {
    pImpl->numSensors_ = 0;
}

Touch::Touch(size_t numSensors) : pImpl(std::make_unique<Impl>()) {
    pImpl->numSensors_ = numSensors;
    pImpl->data_.resize(numSensors, 0.0f);
}

void Touch::setData(const std::vector<float>& data) {
    if (pImpl) {
        pImpl->data_ = data;
        pImpl->numSensors_ = data.size();
    }
}

void Touch::setData(float* data, size_t size) {
    if (pImpl) {
        pImpl->data_.assign(data, data + size);
        pImpl->numSensors_ = size;
    }
}

size_t Touch::getNumSensors() const {
    return pImpl ? pImpl->numSensors_ : 0;
}

struct Proprioception::Impl {
    float x_ = 0.0f, y_ = 0.0f, orientation_ = 0.0f;
    float velocityX_ = 0.0f, velocityY_ = 0.0f, angularVelocity_ = 0.0f;
};

Proprioception::Proprioception() : pImpl(std::make_unique<Impl>()) {}

void Proprioception::setBodyState(float x, float y, float orientation,
                                 float velocityX, float velocityY, float angularVelocity) {
    if (pImpl) {
        pImpl->x_ = x;
        pImpl->y_ = y;
        pImpl->orientation_ = orientation;
        pImpl->velocityX_ = velocityX;
        pImpl->velocityY_ = velocityY;
        pImpl->angularVelocity_ = angularVelocity;
    }
}

void Proprioception::setBodyState(const std::vector<float>& state) {
    if (pImpl && state.size() >= 6) {
        pImpl->x_ = state[0];
        pImpl->y_ = state[1];
        pImpl->orientation_ = state[2];
        pImpl->velocityX_ = state[3];
        pImpl->velocityY_ = state[4];
        pImpl->angularVelocity_ = state[5];
    }
}

float Proprioception::getX() const {
    return pImpl ? pImpl->x_ : 0.0f;
}

float Proprioception::getY() const {
    return pImpl ? pImpl->y_ : 0.0f;
}

float Proprioception::getOrientation() const {
    return pImpl ? pImpl->orientation_ : 0.0f;
}

float Proprioception::getVelocityX() const {
    return pImpl ? pImpl->velocityX_ : 0.0f;
}

float Proprioception::getVelocityY() const {
    return pImpl ? pImpl->velocityY_ : 0.0f;
}

float Proprioception::getAngularVelocity() const {
    return pImpl ? pImpl->angularVelocity_ : 0.0f;
}

} // namespace nlm