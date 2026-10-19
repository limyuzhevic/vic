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
    // Configurable parameters
    , moveSpeed_(3.0f)
    , turnSpeed_(2.0f)
    , actionCooldown_(0.1f)
    , movementStep_(0.1f)
    , backwardMovementFactor_(0.5f)
    , movementCost_(0.01f)
    , wallCollisionCost_(0.05f)
    , backwardMovementCost_(0.02f)
    , turnCost_(0.005f)
    , hazardDamage_(0.2f)
    , interactRange_(1.0f)
    , hazardDetectionRange_(1.5f)
    , touchDetectionRange_(0.3f)
    , visionMinIntensity_(0.4f)
    , sensoryNoveltyDecay_(0.99f)
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
    
    // Validate input parameters
    if (std::isnan(currentTime) || std::isinf(currentTime)) {
        return ActionResult(0.0f, false, "invalid_time");
    }
    
    // Only allow one action per step
    if (currentTime - agent_.lastActionTime < actionCooldown_) {
        return ActionResult(0.0f, false, "too soon");
    }
    
    agent_.lastActionTime = currentTime;
    
    switch (cmd) {
        case MotorCommand::MoveForward: {
            float dx = std::cos(agent_.orientation) * moveSpeed_;
            float dy = std::sin(agent_.orientation) * moveSpeed_;
            
            float newX = agent_.x + dx * movementStep_;
            float newY = agent_.y + dy * movementStep_;
            
            if (isValidPosition(newX, newY)) {
                agent_.x = newX;
                agent_.y = newY;
                agent_.isMoving = true;
                result.reward = -movementCost_;  // Small cost for movement
                result.success = true;
            } else {
                result.reward = -wallCollisionCost_;  // Cost for hitting wall
                result.success = false;
            }
            break;
        }
        
        case MotorCommand::MoveBackward: {
            float dx = -std::cos(agent_.orientation) * moveSpeed_ * backwardMovementFactor_;
            float dy = -std::sin(agent_.orientation) * moveSpeed_ * backwardMovementFactor_;
            
            float newX = agent_.x + dx * movementStep_;
            float newY = agent_.y + dy * movementStep_;
            
            if (isValidPosition(newX, newY)) {
                agent_.x = newX;
                agent_.y = newY;
                agent_.isMoving = true;
                result.reward = -backwardMovementCost_;
                result.success = true;
            } else {
                result.reward = -wallCollisionCost_;
                result.success = false;
            }
            break;
        }
        
        case MotorCommand::TurnLeft:
            agent_.angularVelocity = -turnSpeed_;
            agent_.isTurning = true;
            result.reward = -turnCost_;
            result.success = true;
            break;
            
        case MotorCommand::TurnRight:
            agent_.angularVelocity = turnSpeed_;
            agent_.isTurning = true;
            result.reward = -turnCost_;
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
    result.message = "no_object";
    
    // Validate agent state
    if (std::isnan(agent_.x) || std::isnan(agent_.y)) {
        return ActionResult(0.0f, false, "invalid_agent_position");
    }
    
    const float interactRange = 1.0f;
    
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - agent_.x;
        float dy = obj.y - agent_.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        // Avoid division by zero
        if (dist < 0.001f) continue;
        
        if (dist < interactRange + obj.radius) {
            obj.active = false;  // Consume the object
            
            if (obj.type == WorldObjectType::Resource) {
                agent_.energy = std::min(maxEnergy_, agent_.energy + resourceEnergyGain_);
                result.reward = obj.value;  // Positive reward
                result.success = true;
                result.message = "consumed_resource";
            } else if (obj.type == WorldObjectType::Hazard) {
                agent_.health -= hazardDamage_;
                result.reward = obj.value;  // Negative reward
                result.success = true;
                result.message = "hit_hazard";
            }
            
            // Only interact with one object at a time
            break;
        }
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
    
    std::vector<float> vision(visionRayCount_, 1.0f);  // Default: far/empty
    
    for (int i = 0; i < visionRayCount_; ++i) {
        // Angle for this ray
        float angle = agent_.orientation - M_PI / 4.0f + (M_PI / 2.0f * i / (visionRayCount_ - 1));
        
        // Cast ray
        float dx = std::cos(angle);
        float dy = std::sin(angle);
        
        float t = 0.0f;
        while (t < visionMaxRange_) {
            float px = agent_.x + dx * t;
            float py = agent_.y + dy * t;
            
            // Check wall collision
            if (!isValidPosition(px, py)) {
                vision[i] = t / visionMaxRange_;
                break;
            }
            
            // Check object collision
            WorldObject* obj = getObjectAt(px, py);
            if (obj != nullptr) {
                // Encode object type as intensity pattern
                // Resource = bright, Hazard = dark, different distances
                if (obj->type == WorldObjectType::Resource) {
                    vision[i] = (visionMaxRange_ - t) / visionMaxRange_ * visionMinIntensity_;  // Bright
                } else if (obj->type == WorldObjectType::Hazard) {
                    vision[i] = (visionMaxRange_ - t) / visionMaxRange_ * (visionMinIntensity_ * 0.5f);  // Dark
                } else {
                    vision[i] = 0.5f;
                }
                break;
            }
            
            t += visionRayStep_;
        }
    }
    
    sensory_.setVision(vision);
    sensory_.setTimestamp(simTime_);
    
    // Generate touch sensors (proximity to obstacles in 8 directions)
    std::vector<float> touch(touchRayCount_, 0.0f);
    for (int i = 0; i < touchRayCount_; ++i) {
        float angle = agent_.orientation + 2.0f * M_PI * i / touchRayCount_;
        float dx = std::cos(angle);
        float dy = std::sin(angle);
        
        float t = 0.0f;
        while (t < hazardDetectionRange_) {
            float px = agent_.x + dx * t;
            float py = agent_.y + dy * t;
            
            if (!isValidPosition(px, py) || getObjectAt(px, py) != nullptr) {
                touch[i] = 1.0f - t / hazardDetectionRange_;  // Closer = stronger signal
                break;
            }
            t += touchDetectionStep_;
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
    proprioception[3] = std::abs(agent_.velocityX) / internalSignalWeight_;  // Speed X
    proprioception[4] = std::abs(agent_.velocityY) / internalSignalWeight_;  // Speed Y
    proprioception[5] = std::abs(agent_.angularVelocity) / turnSpeed_;  // Turn rate
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
