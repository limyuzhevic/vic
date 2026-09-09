#include "AgentBody.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

AgentBody::AgentBody()
    : x_(0.0f)
    , y_(0.0f)
    , orientation_(0.0f)
    , energy_(100.0f)
    , visionWidth_(16)
    , visionHeight_(16)
    , maxSpeed_(1.0f)
{
    internalSignals_.resize(4, 0.0f);
}

AgentBody::~AgentBody() = default;

void AgentBody::setPosition(float x, float y) {
    x_ = x;
    y_ = y;
}

void AgentBody::setOrientation(float orientation) {
    orientation_ = orientation;
}

void AgentBody::move(float dx, float dy) {
    x_ += dx;
    y_ += dy;
}

void AgentBody::setInternalSignal(int index, float value) {
    if (index >= 0 && index < static_cast<int>(internalSignals_.size())) {
        internalSignals_[index] = value;
    }
}

float AgentBody::getInternalSignal(int index) const {
    if (index >= 0 && index < static_cast<int>(internalSignals_.size())) {
        return internalSignals_[index];
    }
    return 0.0f;
}

void AgentBody::reset() {
    x_ = 0.0f;
    y_ = 0.0f;
    orientation_ = 0.0f;
    energy_ = 100.0f;
    std::fill(internalSignals_.begin(), internalSignals_.end(), 0.0f);
}

} // namespace nlm
