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
    
    // Apply velocity to position
    agent_.x += agent_.velocityX * timestep;
    agent_.y += agent_.velocityY * timestep;
    
    // Apply angular velocity to orientation
    agent_.orientation += agent_.angularVelocity * timestep;
    
    // Normalize orientation to [-PI, PI]
    while (agent_.orientation > M_PI) agent_.orientation -= 2.0f * M_PI;
    while (agent_.orientation < -M_PI) agent_.orientation += 2.0f * M_PI;
    
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
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - x;
        float dy = obj.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj.radius) {
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
    // Simple raycasting-based vision
    // Cast rays in a cone in front of the agent
    
    const int rayCount = 16;
    const float fov = M_PI / 2.0f;  // 90 degree FOV
    const float maxRange = 8.0f;
    
    // Validate parameters
    if (rayCount <= 0 || maxRange <= 0.0f || fov <= 0.0f) {
        // Reset to default vision and return
        std::vector<float> vision(visionWidth_, 1.0f);
        sensory_.setVision(vision);
        return;
    }
    
    // Avoid division by zero for rayCount = 1
    float angleStep = (rayCount > 1) ? (fov / (rayCount - 1)) : 0.0f;
    
    std::vector<float> vision(rayCount, 1.0f);  // Default: far/empty
    
    for (int i = 0; i < rayCount; ++i) {
        // Angle for this ray
        float angle = agent_.orientation - fov / 2.0f + angleStep * i;
        
        // Validate trigonometric input
        if (std::isnan(angle) || std::isinf(angle)) {
            continue; // Skip invalid ray
        }
        
        // Cast ray with floating point precision handling
        float dx = std::cos(angle);
        float dy = std::sin(angle);
        
        float t = 0.0f;
        // Use epsilon for floating point comparison to avoid infinite loops
        const float epsilon = 1e-6f;
        while (t < maxRange - epsilon) {
            float px = agent_.x + dx * t;
            float py = agent_.y + dy * t;
            
            // Validate position calculations
            if (std::isnan(px) || std::isnan(py) || std::isinf(px) || std::isinf(py)) {
                // Invalid position due to overflow, break ray
                vision[i] = 1.0f;
                break;
            }
            
            // Check wall collision with bounds checking
            if (!isValidPosition(px, py)) {
                // Clamp t to valid range
                vision[i] = std::max(0.0f, std::min(t / maxRange, 1.0f));
                break;
            }
            
            // Check object collision with null pointer validation
            WorldObject* obj = getObjectAt(px, py);
            if (obj != nullptr) {
                // Encode object type as intensity pattern
                // Resource = bright, Hazard = dark, different distances
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
            
            // Additional safety check to prevent infinite loops
            if (t > maxRange * 2.0f) {
                break;
            }
        }
    }
    
    sensory_.setVision(vision);
    sensory_.setTimestamp(simTime_);
    
    // Generate touch sensors (proximity to obstacles in 8 directions)
    std::vector<float> touch(8, 0.0f);
    const float touchMaxRange = 1.5f;
    
    for (int i = 0; i < 8; ++i) {
        float angle = agent_.orientation + 2.0f * M_PI * i / 8.0f;
        
        // Validate trigonometric input
        if (std::isnan(angle) || std::isinf(angle)) {
            continue; // Skip invalid direction
        }
        
        float dx = std::cos(angle);
        float dy = std::sin(angle);
        
        float t = 0.0f;
        const float epsilon = 1e-6f;
        
        while (t < touchMaxRange - epsilon) {
            float px = agent_.x + dx * t;
            float py = agent_.y + dy * t;
            
            // Validate position calculations
            if (std::isnan(px) || std::isnan(py) || std::isinf(px) || std::isinf(py)) {
                // Invalid position due to overflow, break touch ray
                touch[i] = 1.0f;
                break;
            }
            
            // Check for wall or object collision with bounds checking
            if (!isValidPosition(px, py) || getObjectAt(px, py) != nullptr) {
                // Clamp t to valid range to avoid division by zero or out-of-range values
                touch[i] = std::max(0.0f, std::min(1.0f - t / touchMaxRange, 1.0f));
                break;
            }
            
            t += 0.1f;
            
            // Safety check to prevent infinite loops
            if (t > touchMaxRange * 2.0f) {
                break;
            }
        }
    }
    
    // Validate and set touch sensors
    for (float& touchValue : touch) {
        // Clamp values to [0.0, 1.0] range
        touchValue = std::max(0.0f, std::min(touchValue, 1.0f));
    }
    
    sensory_.setTouch(touch);
    
    // Generate internal signals (energy, health) with validation
    std::vector<float> internal(4, 0.0f);
    
    // Energy level: clamp between 0 and 1
    if (maxEnergy_ > 0.0f) {
        internal[0] = std::max(0.0f, std::min(agent_.energy / maxEnergy_, 1.0f));
    }
    
    // Health: clamp to reasonable range
    internal[1] = std::max(0.0f, std::min(agent_.health, 1.0f));
    
    // Movement and turning states
    internal[2] = agent_.isMoving ? 1.0f : 0.0f;
    internal[3] = agent_.isTurning ? 1.0f : 0.0f;
    
    sensory_.setInternal(internal);
    
    // Generate proprioception (body position/velocity) with validation
    std::vector<float> proprioception(6, 0.0f);
    
    // Normalize position coordinates with division by zero protection
    if (width_ > 0.0f) {
        proprioception[0] = std::max(0.0f, std::min(agent_.x / width_, 1.0f));
    }
    if (height_ > 0.0f) {
        proprioception[1] = std::max(0.0f, std::min(agent_.y / height_, 1.0f));
    }
    
    // Normalize orientation to [0, 1] range
    float normalizedOrientation = (agent_.orientation + M_PI) / (2.0f * M_PI);
    proprioception[2] = std::max(0.0f, std::min(normalizedOrientation, 1.0f));
    
    // Normalize velocities with speed limits
    proprioception[3] = std::max(0.0f, std::min(std::abs(agent_.velocityX) / 5.0f, 1.0f));
    proprioception[4] = std::max(0.0f, std::min(std::abs(agent_.velocityY) / 5.0f, 1.0f));
    proprioception[5] = std::max(0.0f, std::min(std::abs(agent_.angularVelocity) / 3.0f, 1.0f));
    
    sensory_.setProprioception(proprioception);
    
    // No audio for now
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
