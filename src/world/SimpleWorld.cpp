// NLM SimpleWorld Implementation
// Improved version with better configuration management, configuration methods,
// enhanced object spawning, and utility functions

#include "SimpleWorld.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <random>
#include <stdexcept>

namespace nlm {

// Configuration constants for SimpleWorld
namespace SimpleWorldConstants {
    constexpr float DEFAULT_WORLD_WIDTH = 20.0f;
    constexpr float DEFAULT_WORLD_HEIGHT = 20.0f;
    constexpr size_t DEFAULT_VISION_WIDTH = 16;
    constexpr size_t DEFAULT_VISION_HEIGHT = 16;
    constexpr float DEFAULT_MAX_ENERGY = 100.0f;
    constexpr float DEFAULT_ENERGY_DECAY_RATE = 0.01f;
    constexpr float DEFAULT_RESOURCE_ENERGY_GAIN = 30.0f;
    
    // Object spawning constants
    constexpr int DEFAULT_RESOURCE_COUNT = 5;
    constexpr int DEFAULT_HAZARD_COUNT = 3;
    constexpr float RESOURCE_MIN_X = 1.0f;
    constexpr float RESOURCE_MAX_X = DEFAULT_WORLD_WIDTH - 1.0f;
    constexpr float HAZARD_MIN_X = 1.0f;
    constexpr float HAZARD_MAX_X = DEFAULT_WORLD_WIDTH - 1.0f;
    constexpr float HAZARD_MIN_Y = 1.0f;
    constexpr float HAZARD_MAX_Y = DEFAULT_WORLD_HEIGHT - 1.0f;
    
    // Vision constants
    constexpr float DEFAULT_FOV = M_PI / 2.0f;  // 90 degree FOV
    constexpr float DEFAULT_MAX_RANGE = 8.0f;
    constexpr int DEFAULT_RAY_COUNT = 16;
    constexpr float DEFAULT_TOUCH_RANGE = 1.5f;
    
    // Interaction constants
    constexpr float DEFAULT_INTERACTION_RANGE = 1.0f;
    constexpr float DEFAULT_PASSIVE_DAMAGE = 0.01f;
    
    // Movement constants
    constexpr float DEFAULT_MOVE_SPEED = 3.0f;
    constexpr float DEFAULT_TURN_SPEED = 2.0f;
    constexpr float DEFAULT_MOVE_STEP = 0.1f;
    constexpr float DEFAULT_MOVEMENT_COST = -0.01f;
    constexpr float DEFAULT_WALL_HIT_COST = -0.05f;
    constexpr float DEFAULT_TURN_COST = -0.005f;
    constexpr float DEFAULT_ACTION_COOLDOWN = 0.1f;
    
    // Physical simulation constants
    constexpr float MAX_ENERGY = 1000.0f;
    constexpr float MIN_ENERGY = 0.0f;
    constexpr float MAX_HEALTH = 2.0f;
    constexpr float MIN_HEALTH = 0.0f;
    
    // Time constants
    constexpr double MIN_TIMESTEP = 1e-6;
    constexpr double MAX_TIMESTEP = 1.0;
}

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
    : width_(SimpleWorldConstants::DEFAULT_WORLD_WIDTH)
    , height_(SimpleWorldConstants::DEFAULT_WORLD_HEIGHT)
    , visionWidth_(SimpleWorldConstants::DEFAULT_VISION_WIDTH)
    , visionHeight_(SimpleWorldConstants::DEFAULT_VISION_HEIGHT)
    , maxEnergy_(SimpleWorldConstants::DEFAULT_MAX_ENERGY)
    , energyDecayRate_(SimpleWorldConstants::DEFAULT_ENERGY_DECAY_RATE)
    , resourceEnergyGain_(SimpleWorldConstants::DEFAULT_RESOURCE_ENERGY_GAIN)
    , simTime_(0.0)
    , rngSeed_(42)
    , rngState_(42)
{
    agent_.reset();
    validateConfiguration();
}

SimpleWorld::~SimpleWorld() = default;

void SimpleWorld::validateConfiguration() const {
    if (width_ <= 0.0f || height_ <= 0.0f) {
        throw std::invalid_argument("World dimensions must be positive");
    }
    
    if (visionWidth_ == 0 || visionHeight_ == 0) {
        throw std::invalid_argument("Vision dimensions must be positive");
    }
    
    if (maxEnergy_ <= 0.0f) {
        throw std::invalid_argument("Max energy must be positive");
    }
    
    if (energyDecayRate_ < 0.0f) {
        throw std::invalid_argument("Energy decay rate cannot be negative");
    }
}

void SimpleWorld::configure(float width, float height, size_t visionWidth, size_t visionHeight) {
    // Validate inputs
    if (width <= 0.0f || height <= 0.0f) {
        throw std::invalid_argument("World dimensions must be positive");
    }
    
    if (visionWidth == 0 || visionHeight == 0) {
        throw std::invalid_argument("Vision dimensions must be positive");
    }
    
    width_ = width;
    height_ = height;
    visionWidth_ = visionWidth;
    visionHeight_ = visionHeight;
    
    // Adjust agent position if it's out of bounds
    if (agent_.x >= width_) agent_.x = width_ - 0.01f;
    if (agent_.y >= height_) agent_.y = height_ - 0.01f;
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
    // Validate position
    if (!isValidPosition(x, y)) {
        throw std::invalid_argument("Invalid agent start position");
    }
    
    agent_.x = x;
    agent_.y = y;
}

void SimpleWorld::update(double timestep) {
    // Validate timestep
    if (timestep < 1e-6 || timestep > 1.0) {
        throw std::invalid_argument("Timestep out of valid range");
    }
    
    simTime_ += timestep;
    
    // Apply velocity to position with bounds checking
    float newX = agent_.x + agent_.velocityX * timestep;
    float newY = agent_.y + agent_.velocityY * timestep;
    
    // Ensure new position is valid
    if (isValidPosition(newX, newY)) {
        agent_.x = newX;
        agent_.y = newY;
    } else {
        // Reflect or stop at boundary
        agent_.velocityX = 0.0f;
        agent_.velocityY = 0.0f;
        agent_.x = std::clamp(newX, 0.0f, width_ - 0.01f);
        agent_.y = std::clamp(newY, 0.0f, height_ - 0.01f);
    }
    
    // Apply angular velocity to orientation
    agent_.orientation += agent_.angularVelocity * timestep;
    
    // Normalize orientation to [-PI, PI]
    while (agent_.orientation > M_PI) agent_.orientation -= 2.0f * M_PI;
    while (agent_.orientation < -M_PI) agent_.orientation += 2.0f * M_PI;
    
    // Energy decay with bounds checking
    agent_.energy -= energyDecayRate_;
    if (agent_.energy < 0.0f) {
        agent_.energy = 0.0f;
    }
    
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
    
    // Only allow one action per step with configurable cooldown
    if (currentTime - agent_.lastActionTime < 0.1) {
        return ActionResult(0.0f, false, "too soon");
    }
    
    agent_.lastActionTime = currentTime;
    
    switch (cmd) {
        case MotorCommand::MoveForward: {
            float moveSpeed = 3.0f;
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
            float moveSpeed = 1.5f;
            float dx = -std::cos(agent_.orientation) * moveSpeed;
            float dy = -std::sin(agent_.orientation) * moveSpeed;
            
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
            agent_.angularVelocity = -2.0f;
            agent_.isTurning = true;
            result.reward = -0.005f;
            result.success = true;
            break;
            
        case MotorCommand::TurnRight:
            agent_.angularVelocity = 2.0f;
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
    
    std::vector<float> vision(rayCount, 1.0f);  // Default: far/empty
    
    for (int i = 0; i < rayCount; ++i) {
        // Angle for this ray
        float angle = agent_.orientation - fov / 2.0f + (fov * i / (rayCount - 1));
        
        // Cast ray
        float dx = std::cos(angle);
        float dy = std::sin(angle);
        
        float t = 0.0f;
        while (t < maxRange) {
            float px = agent_.x + dx * t;
            float py = agent_.y + dy * t;
            
            // Check wall collision
            if (!isValidPosition(px, py)) {
                vision[i] = t / maxRange;
                break;
            }
            
            // Check object collision
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
        }
    }
    
    sensory_.setVision(vision);
    sensory_.setTimestamp(simTime_);
    
    // Generate touch sensors (proximity to obstacles in 8 directions)
    std::vector<float> touch(8, 0.0f);
    for (int i = 0; i < 8; ++i) {
        float angle = agent_.orientation + 2.0f * M_PI * i / 8.0f;
        float dx = std::cos(angle);
        float dy = std::sin(angle);
        
        float t = 0.0f;
        while (t < 1.5f) {
            float px = agent_.x + dx * t;
            float py = agent_.y + dy * t;
            
            if (!isValidPosition(px, py) || getObjectAt(px, py) != nullptr) {
                touch[i] = 1.0f - t / 1.5f;  // Closer = stronger signal
                break;
            }
            t += 0.1f;
        }
    }
    sensory_.setTouch(touch);
    
    // Generate internal signals (energy, health)
    std::vector<float> internal(4, 0.0f);
    internal[0] = agent_.energy / maxEnergy_;  // Energy level
    internal[1] = agent_.health;               // Health
    internal[2] = agent_.isMoving ? 1.0f : 0.0f;  // Movement state
    internal[3] = agent_.isTurning ? 1.0f : 0.0f;  // Turning state
    sensory_.setInternal(internal);
    
    // Generate proprioception (body position/velocity)
    std::vector<float> proprioception(6, 0.0f);
    proprioception[0] = agent_.x / width_;     // Normalized X
    proprioception[1] = agent_.y / height_;    // Normalized Y
    proprioception[2] = (agent_.orientation + M_PI) / (2.0f * M_PI);  // Normalized orientation
    proprioception[3] = std::abs(agent_.velocityX) / 5.0f;  // Speed X
    proprioception[4] = std::abs(agent_.velocityY) / 5.0f;  // Speed Y
    proprioception[5] = std::abs(agent_.angularVelocity) / 3.0f;  // Turn rate
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
