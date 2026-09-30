#include "SimpleWorld.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

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
    
    // Apply velocity to position - cache local references for speed
    float& x = agent_.x;
    float& y = agent_.y;
    float vx = agent_.velocityX;
    float vy = agent_.velocityY;
    
    x += vx * timestep;
    y += vy * timestep;
    
    // Apply angular velocity to orientation
    float& orientation = agent_.orientation;
    float& angularVelocity = agent_.angularVelocity;
    orientation += angularVelocity * timestep;
    
    // Normalize orientation to [-PI, PI] - use modulo operation
    while (orientation > M_PI) orientation -= 2.0f * M_PI;
    while (orientation < -M_PI) orientation += 2.0f * M_PI;
    
    // Boundary collision - check if outside once
    bool isOutOfBounds = false;
    if (x < 0.0f) { x = 0.0f; vx = 0.0f; isOutOfBounds = true; }
    else if (x >= width_) { x = width_ - 0.01f; vx = 0.0f; isOutOfBounds = true; }
    if (y < 0.0f) { y = 0.0f; vy = 0.0f; isOutOfBounds = true; }
    else if (y >= height_) { y = height_ - 0.01f; vy = 0.0f; isOutOfBounds = true; }
    
    // Energy decay - cache decay rate
    const float decay = energyDecayRate_;
    float& energy = agent_.energy;
    energy -= decay;
    if (energy < 0.0f) energy = 0.0f;
    
    // Update age
    agent_.age += timestep;
    
    // Generate vision - essential for perception
    generateVision();
    
    // Check collisions only if agent moved
    if (isOutOfBounds || vx != 0.0f || vy != 0.0f) {
        checkCollisions();
    }
}

uint64_t SimpleWorld::nextRandom() {
    return lcg(rngState_);
}

ActionResult SimpleWorld::applyMotorCommand(MotorCommand cmd, double currentTime) {
    ActionResult result;
    const float moveSpeed = 3.0f;
    const float turnSpeed = 2.0f;
    
    // Only allow one action per step
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
            // Decay velocities
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
    
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - agent_.x;
        float dy = obj.y - agent_.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < interactRange + obj.radius) {
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
    // Fast path: check boundaries first (most common case)
    if (x < 0.0f || x >= width_ || y < 0.0f || y >= height_) {
        return nullptr;
    }
    
    // Pre-cache object list pointer and size for faster access
    auto& objects = objects_;
    const size_t objectCount = objects.size();
    
    // Use local variables for frequently accessed values
    const float x2 = x * x;
    const float y2 = y * y;
    
    // Process objects in a single pass with optimized distance calculation
    for (auto& obj : objects) {
        if (!obj.active) continue;
        
        // Compute distance squared to avoid sqrt for most cases
        float dx = obj.x - x;
        float dy = obj.y - y;
        float dist2 = dx * dx + dy * dy;
        
        // Compare with squared radius (more efficient than sqrt)
        float radius2 = obj.radius * obj.radius;
        
        if (dist2 < radius2) {
            return &obj;
        }
    }
    
    return nullptr;
}

void SimpleWorld::addObject(const WorldObject& obj) {
    objects_.push_back(obj);
}

void SimpleWorld::removeObject(float x, float y) {
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - x;
        float dy = obj.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj.radius) {
            obj.active = false;
            return;
        }
    }
}

void SimpleWorld::generateVision() {
    // Pre-compute frequently used values for vision generation
    const float agentX = agent_.x;
    const float agentY = agent_.y;
    const float orientation = agent_.orientation;
    const float fov = M_PI / 2.0f;  // 90 degree FOV
    const float maxRange = 8.0f;
    
    // Pre-compute trigonometric values for all ray angles
    std::vector<float> cosines(16);
    std::vector<float> sines(16);
    for (int i = 0; i < 16; ++i) {
        float angle = orientation - fov / 2.0f + (fov * i / 15.0f);
        cosines[i] = std::cos(angle);
        sines[i] = std::sin(angle);
    }
    
    std::vector<float> vision(16, 1.0f);  // Default: far/empty
    
    // Process rays in batches for better cache locality
    for (int i = 0; i < 16; ++i) {
        const float dx = cosines[i];
        const float dy = sines[i];
        
        float t = 0.0f;
        while (t < maxRange) {
            float px = agentX + dx * t;
            float py = agentY + dy * t;
            
            // Check wall collision
            if (!isValidPosition(px, py)) {
                vision[i] = t / maxRange;
                break;
            }
            
            // Check object collision
            WorldObject* obj = getObjectAt(px, py);
            if (obj != nullptr) {
                // Encode object type as intensity pattern
                if (obj->type == WorldObjectType::Resource) {
                    vision[i] = (maxRange - t) / maxRange * 0.8f;  // Bright
                } else if (obj->type == WorldObjectType::Hazard) {
                    vision[i] = (maxRange - t) / maxRange * 0.3f;  // Dark
                } else {
                    vision[i] = 0.5f;
                }
                break;
            }
            
            t += 0.1f;
        }
    }
    
    // Generate touch sensors (proximity to obstacles)
    std::vector<float> touch(8, 0.0f);
    const float interactRange = 1.5f;
    
    // Pre-compute touch ray angles
    std::vector<float> touchCosines(8);
    std::vector<float> touchSines(8);
    for (int i = 0; i < 8; ++i) {
        float angle = orientation + 2.0f * M_PI * i / 8.0f;
        touchCosines[i] = std::cos(angle);
        touchSines[i] = std::sin(angle);
    }
    
    for (int i = 0; i < 8; ++i) {
        const float dx = touchCosines[i];
        const float dy = touchSines[i];
        
        float t = 0.0f;
        while (t < interactRange) {
            float px = agentX + dx * t;
            float py = agentY + dy * t;
            
            if (!isValidPosition(px, py) || getObjectAt(px, py) != nullptr) {
                touch[i] = 1.0f - t / interactRange;  // Closer = stronger signal
                break;
            }
            t += 0.1f;
        }
    }
    
    // Generate internal signals
    std::vector<float> internal(4, 0.0f);
    internal[0] = agent_.energy / maxEnergy_;  // Energy level
    internal[1] = agent_.health;               // Health
    internal[2] = agent_.isMoving ? 1.0f : 0.0f;  // Movement state
    internal[3] = agent_.isTurning ? 1.0f : 0.0f;  // Turning state
    
    // Generate proprioception (6 values)
    std::vector<float> proprioception(6, 0.0f);
    const float invWidth = 1.0f / width_;
    const float invHeight = 1.0f / height_;
    const float twoPI = 2.0f * M_PI;
    
    proprioception[0] = agent_.x * invWidth;     // Normalized X
    proprioception[1] = agent_.y * invHeight;    // Normalized Y
    proprioception[2] = (agent_.orientation + M_PI) * (1.0f / twoPI);  // Normalized orientation
    proprioception[3] = std::abs(agent_.velocityX) * (1.0f / 5.0f);  // Speed X
    proprioception[4] = std::abs(agent_.velocityY) * (1.0f / 5.0f);  // Speed Y
    proprioception[5] = std::abs(agent_.angularVelocity) * (1.0f / 3.0f);  // Turn rate
    
    // Assign all signals
    sensory_.setVision(vision);
    sensory_.setTimestamp(simTime_);
    sensory_.setTouch(touch);
    sensory_.setInternal(internal);
    sensory_.setProprioception(proprioception);
    sensory_.setAudio({});
}

void SimpleWorld::checkCollisions() {
    // Passive collision check (for damage when moving into objects)
    WorldObject* obj = getObjectAt(agent_.x, agent_.y);
    if (obj != nullptr && obj->type == WorldObjectType::Hazard) {
        agent_.health -= 0.01f;  // Passive damage
    }
}

} // namespace nlm
