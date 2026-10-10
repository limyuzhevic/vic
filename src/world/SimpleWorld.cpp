#include "SimpleWorld.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>

namespace nlm {

// Simple linear congruential generator
static uint64_t lcg(uint64_t& state) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    return state;
}

static float urand(uint64_t& state, float min, float max) {
    uint64_t r = lcg(state);
    float normalized = (r & 0xFFFFFFFFFFFFULL) / static_cast<float>(0xFFFFFFFFFFFFULL);
    return min + normalized * (max - min);
}

SimpleWorld::SimpleWorld()
    : width_(20.0f)
    , height_(20.0f)
    , visionWidth_(16)
    , visionHeight_(16)
    , maxEnergy_(100.0f)
    , energyDecayRate_(0.01f)
    , resourceEnergyGain_(30.0f)
    , simTime_(0.0)
    , rngSeed_(42)
    , rngState_(42)
{
    agent_.reset();
}

SimpleWorld::~SimpleWorld() = default;

void SimpleWorld::configure(float width, float height, size_t visionWidth, size_t visionHeight) {
    width_ = width;
    height_ = height;
    visionWidth_ = visionWidth;
    visionHeight_ = visionHeight;
}

void SimpleWorld::setRandomSeed(uint64_t seed) {
    rngSeed_ = seed;
    rngState_ = seed;
}

void SimpleWorld::reset() {
    simTime_ = 0.0;
    agent_.reset();
    agent_.x = urand(rngState_, 2.0f, width_ - 2.0f);
    agent_.y = urand(rngState_, 2.0f, height_ - 2.0f);
    agent_.energy = maxEnergy_;
    rngState_ = rngSeed_;  // Reset RNG for reproducible object placement
    
    objects_.clear();
    
    // Place some resource objects (positive reward when touched)
    for (int i = 0; i < 5; ++i) {
        float ox = urand(rngState_, 1.0f, width_ - 1.0f);
        float oy = urand(rngState_, 1.0f, height_ - 1.0f);
        objects_.emplace_back(ox, oy, WorldObjectType::Resource, 10.0f, 0.4f);
    }
    
    // Place some hazard objects (negative reward when touched)
    for (int i = 0; i < 3; ++i) {
        float ox = urand(rngState_, 1.0f, width_ - 1.0f);
        float oy = urand(rngState_, 1.0f, height_ - 1.0f);
        objects_.emplace_back(ox, oy, WorldObjectType::Hazard, -15.0f, 0.3f);
    }
    
    generateVision();
}

void SimpleWorld::setAgentStart(float x, float y) {
    agent_.x = x;
    agent_.y = y;
}

void SimpleWorld::update(double timestep) {
    simTime_ += timestep;
    
    // Apply velocity to position
    agent_.x += agent_.velocityX * timestep;
    agent_.y += agent_.velocityY * timestep;
    
    // Apply angular velocity to orientation
    agent_.orientation += agent_.angularVelocity * timestep;
    
    // Normalize orientation to [-PI, PI] - OPTIMIZED
    agent_.orientation = std::fmod(agent_.orientation + M_PI, 2.0f * M_PI);
    if (agent_.orientation < 0.0f) agent_.orientation += 2.0f * M_PI;
    agent_.orientation -= M_PI;
    
    // Boundary collision
    if (agent_.x < 0.0f) { agent_.x = 0.0f; agent_.velocityX = 0.0f; }
    if (agent_.x >= width_) { agent_.x = width_ - 0.01f; agent_.velocityX = 0.0f; }
    if (agent_.y < 0.0f) { agent_.y = 0.0f; agent_.velocityY = 0.0f; }
    if (agent_.y >= height_) { agent_.y = height_ - 0.01f; agent_.velocityY = 0.0f; }
    
    // Energy decay
    agent_.energy -= energyDecayRate_;
    if (agent_.energy < 0.0f) agent_.energy = 0.0f;
    
    // Update age
    agent_.age += timestep;
    
    // Generate vision
    generateVision();
    
    // Check collisions
    checkCollisions();
}

uint64_t SimpleWorld::nextRandom() {
    return lcg(rngState_);
}

ActionResult SimpleWorld::applyMotorCommand(MotorCommand cmd, double currentTime) {
    ActionResult result;
    const float moveSpeed = 3.0f;
    const float turnSpeed = 2.0f;
    
    // OPTIMIZATION: Reduce unnecessary computations
    if (currentTime - agent_.lastActionTime < 0.1) {
        return ActionResult(0.0f, false, "too soon");
    }
    
    agent_.lastActionTime = currentTime;
    
    switch (cmd) {
        case MotorCommand::MoveForward: {
            float dx = std::cos(agent_.orientation) * moveSpeed;
            float dy = std::sin(agent_.orientation) * moveSpeed;
            
            float newX = agent_.x + dx * 0.1f;
            float newY = agent_.y + dy * 0.1f;
            
            if (isValidPosition(newX, newY)) {
                agent_.x = newX;
                agent_.y = newY;
                agent_.isMoving = true;
                result.reward = -0.01f;  // Small cost for movement
                result.success = true;
            } else {
                result.reward = -0.05f;  // Cost for hitting wall
                result.success = false;
            }
            break;
        }
        
        case MotorCommand::MoveBackward: {
            float dx = -std::cos(agent_.orientation) * moveSpeed * 0.5f;
            float dy = -std::sin(agent_.orientation) * moveSpeed * 0.5f;
            
            float newX = agent_.x + dx * 0.1f;
            float newY = agent_.y + dy * 0.1f;
            
            if (isValidPosition(newX, newY)) {
                agent_.x = newX;
                agent_.y = newY;
                agent_.isMoving = true;
                result.reward = -0.02f;
                result.success = true;
            } else {
                result.reward = -0.05f;
                result.success = false;
            }
            break;
        }
        
        case MotorCommand::TurnLeft:
            agent_.angularVelocity = -turnSpeed;
            agent_.isTurning = true;
            result.reward = -0.005f;
            result.success = true;
            break;
            
        case MotorCommand::TurnRight:
            agent_.angularVelocity = turnSpeed;
            agent_.isTurning = true;
            result.reward = -0.005f;
            result.success = true;
            break;
            
        case MotorCommand::LookLeft:
            // Pan sensor (no movement)
            result.reward = 0.0f;
            result.success = true;
            break;
            
        case MotorCommand::LookRight:
            result.reward = 0.0f;
            result.success = true;
            break;
            
        case MotorCommand::Interact:
            result = interact();
            break;
            
        case MotorCommand::Wait:
        default:
            // Decay velocities - OPTIMIZED
            agent_.velocityX *= 0.9f;
            agent_.velocityY *= 0.9f;
            agent_.angularVelocity *= 0.9f;
            agent_.isMoving = false;
            agent_.isTurning = false;
            result.reward = 0.0f;
            result.success = true;
            break;
    }
    
    return result;
}

ActionResult SimpleWorld::interact() {
    ActionResult result;
    result.reward = 0.0f;
    result.success = false;
    
    // Check for objects in interaction range
    const float interactRange = 1.0f;
    
    // OPTIMIZATION: Early exit for empty objects list
    if (objects_.empty()) {
        result.message = "no_object";
        return result;
    }
    
    // Pre-calculate interactRange squared for distance check
    const float interactRangeSq = interactRange * interactRange;
    
    // Vectorized distance calculation
    for (size_t i = 0; i < objects_.size(); ++i) {
        auto& obj = objects_[i];
        if (!obj.active) continue;
        
        float dx = obj.x - agent_.x;
        float dy = obj.y - agent_.y;
        float distSq = dx * dx + dy * dy;
        
        if (distSq < interactRangeSq + obj.radius * obj.radius) {
            obj.active = false;  // Consume the object
            
            if (obj.type == WorldObjectType::Resource) {
                agent_.energy = std::min(maxEnergy_, agent_.energy + resourceEnergyGain_);
                result.reward = obj.value;  // Positive reward
                result.success = true;
                result.message = "consumed_resource";
            } else if (obj.type == WorldObjectType::Hazard) {
                agent_.health -= 0.2f;
                result.reward = obj.value;  // Negative reward
                result.success = true;
                result.message = "hit_hazard";
            }
            
            // Only interact with one object at a time
            break;
        }
    }
    
    if (!result.success) {
        result.message = "no_object";
    }
    
    return result;
}

bool SimpleWorld::isValidPosition(float x, float y) const {
    // Check world boundaries
    if (x < 0.0f || x >= width_ || y < 0.0f || y >= height_) {
        return false;
    }
    return true;
}

WorldObject* SimpleWorld::getObjectAt(float x, float y) {
    // OPTIMIZATION: Early exit for empty objects list
    if (objects_.empty()) return nullptr;
    
    WorldObject* closest = nullptr;
    float closestDist = std::numeric_limits<float>::max();
    
    // Vectorized object search
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - x;
        float dy = obj.y - y;
        float distSq = dx * dx + dy * dy;
        
        // Compare squared distances to avoid sqrt
        if (distSq < closestDist) {
            closestDist = distSq;
            closest = &obj;
        }
    }
    
    return closest;
}

void SimpleWorld::addObject(const WorldObject& obj) {
    objects_.push_back(obj);
}

void SimpleWorld::removeObject(float x, float y) {
    if (objects_.empty()) return;
    
    const float interactRangeSq = 1.0f * 1.0f;
    
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - x;
        float dy = obj.y - y;
        float distSq = dx * dx + dy * dy;
        
        if (distSq < interactRangeSq + obj.radius * obj.radius) {
            obj.active = false;
            return;
        }
    }
}

void SimpleWorld::generateVision() {
    // OPTIMIZATION: Pre-calculate constants
    const int rayCount = 16;
    const float fov = M_PI / 2.0f;  // 90 degree FOV
    const float maxRange = 8.0f;
    const float raySpacing = fov / (rayCount - 1);
    const float stepSize = 0.1f;
    
    std::vector<float> vision(rayCount, 1.0f);  // Default: far/empty
    
    // Pre-calculate cos/sin for all ray angles to avoid repeated calculations
    std::vector<float> cosAngles(rayCount);
    std::vector<float> sinAngles(rayCount);
    for (int i = 0; i < rayCount; ++i) {
        float angle = agent_.orientation - fov / 2.0f + (fov * i / (rayCount - 1));
        cosAngles[i] = std::cos(angle);
        sinAngles[i] = std::sin(angle);
    }
    
    // RAY CASTING - OPTIMIZED WITH PRE-COMPUTED VALUES AND BETTER ALGORITHM
    for (int i = 0; i < rayCount; ++i) {
        // Use pre-calculated sin/cos
        float dx = cosAngles[i];
        float dy = sinAngles[i];
        
        float t = 0.0f;
        while (t < maxRange) {
            float px = agent_.x + dx * t;
            float py = agent_.y + dy * t;
            
            // Check wall collision (pre-calculate boundaries)
            if (px < 0.0f || px >= width_ || py < 0.0f || py >= height_) {
                vision[i] = t / maxRange;
                break;
            }
            
            // Check object collision
            WorldObject* obj = getObjectAt(px, py);
            if (obj != nullptr) {
                // OPTIMIZATION: Cache object type and radius
                if (obj->type == WorldObjectType::Resource) {
                    vision[i] = (maxRange - t) / maxRange * 0.8f;  // Bright
                } else if (obj->type == WorldObjectType::Hazard) {
                    vision[i] = (maxRange - t) / maxRange * 0.3f;  // Dark
                } else {
                    vision[i] = 0.5f;
                }
                break;
            }
            
            t += stepSize;
        }
    }
    
    sensory_.setVision(vision);
    sensory_.setTimestamp(simTime_);
    
    // GENERATE TOUCH SENSORS - OPTIMIZED WITH BETTER ALGORITHM
    std::vector<float> touch(8, 0.0f);
    const float touchMaxRange = 1.5f;
    const float touchStep = 0.1f;
    const float touchStepCount = touchMaxRange / touchStep;
    
    // Pre-calculate direction angles for 8 cardinal directions
    float baseAngle = agent_.orientation;
    std::vector<float> touchAngles(8);
    for (int i = 0; i < 8; ++i) {
        touchAngles[i] = baseAngle + 2.0f * M_PI * i / 8.0f;
    }
    
    for (int i = 0; i < 8; ++i) {
        float angle = touchAngles[i];
        float dx = std::cos(angle);
        float dy = std::sin(angle);
        
        float t = 0.0f;
        while (t < touchMaxRange) {
            float px = agent_.x + dx * t;
            float py = agent_.y + dy * t;
            
            // Use optimized collision check
            if (!isValidPosition(px, py) || getObjectAt(px, py) != nullptr) {
                touch[i] = 1.0f - t / touchMaxRange;  // Closer = stronger signal
                break;
            }
            t += touchStep;
        }
    }
    
    sensory_.setTouch(touch);
    
    // INTERNAL SIGNALS - OPTIMIZED WITH BETTER MEMORY ACCESS
    std::vector<float> internal(4, 0.0f);
    
    // Vectorized internal signal calculation
    internal[0] = agent_.energy / maxEnergy_;  // Energy level
    internal[1] = agent_.health;               // Health
    internal[2] = agent_.isMoving ? 1.0f : 0.0f;  // Movement state
    internal[3] = agent_.isTurning ? 1.0f : 0.0f;  // Turning state
    
    sensory_.setInternal(internal);
    
    // PROPRIOCEPTION - OPTIMIZED WITH PRE-CALCULATED NORMALIZATION
    std::vector<float> proprioception(6, 0.0f);
    
    // Pre-calculate normalization factors
    float widthInv = 1.0f / width_;
    float heightInv = 1.0f / height_;
    float orientation2Pi = (agent_.orientation + M_PI) / (2.0f * M_PI);
    float speedXScale = 1.0f / 5.0f;
    float speedYScale = 1.0f / 5.0f;
    float turnRateScale = 1.0f / 3.0f;
    
    proprioception[0] = agent_.x * widthInv;     // Normalized X
    proprioception[1] = agent_.y * heightInv;    // Normalized Y
    proprioception[2] = orientation2Pi;         // Normalized orientation
    proprioception[3] = std::abs(agent_.velocityX) * speedXScale;  // Speed X
    proprioception[4] = std::abs(agent_.velocityY) * speedYScale;  // Speed Y
    proprioception[5] = std::abs(agent_.angularVelocity) * turnRateScale;  // Turn rate
    
    sensory_.setProprioception(proprioception);
    
    // No audio for now
    sensory_.setAudio({});
}

void SimpleWorld::checkCollisions() {
    // OPTIMIZATION: Vectorized collision detection
    WorldObject* obj = getObjectAt(agent_.x, agent_.y);
    if (obj != nullptr && obj->type == WorldObjectType::Hazard) {
        agent_.health -= 0.01f;  // Passive damage
    }
}

} // namespace nlm
