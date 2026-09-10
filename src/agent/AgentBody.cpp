#include "AgentBody.hpp"

namespace nlm {

void AgentBody::reset() {
    x = 0.0f;
    y = 0.0f;
    orientation = 0.0f;
    velocityX = 0.0f;
    velocityY = 0.0f;
    angularVelocity = 0.0f;
    energy = 100.0f;
    health = 100.0f;
    age = 0.0f;
    isMoving = false;
    isTurning = false;
    lastActionTime = 0.0f;
}

} // namespace nlm
