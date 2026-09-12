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
    NLM_LOG_INFO("SimpleWorld created with default dimensions: " + std::to_string(width_) + "x" + std::to_string(height_));
    
    // Validate configuration
    if (width_ <= 0.0f || height_ <= 0.0f) {
        NLM_LOG_ERROR("Invalid world dimensions: width=" + std::to_string(width_) + ", height=" + std::to_string(height_));
        width_ = std::max(width_, 1.0f);
        height_ = std::max(height_, 1.0f);
    }
    
    // Validate vision dimensions
    if (visionWidth_ == 0) {
        NLM_LOG_WARNING("Vision width is 0, setting to default 16");
        visionWidth_ = 16;
    }
    if (visionHeight_ == 0) {
        NLM_LOG_WARNING("Vision height is 0, setting to default 16");
        visionHeight_ = 16;
    }
    
    agent_.reset();
    NLM_LOG_DEBUG("SimpleWorld initialized");
}

SimpleWorld::~SimpleWorld() = default;

void SimpleWorld::configure(float width, float height, size_t visionWidth, size_t visionHeight) {
    // Validate input parameters
    if (width <= 0.0f) {
        NLM_LOG_ERROR("Invalid width: " + std::to_string(width) + ". Must be > 0");
        return;
    }
    if (height <= 0.0f) {
        NLM_LOG_ERROR("Invalid height: " + std::to_string(height) + ". Must be > 0");
        return;
    }
    if (visionWidth == 0) {
        NLM_LOG_ERROR("Invalid visionWidth: " + std::to_string(visionWidth) + ". Must be > 0");
        return;
    }
    if (visionHeight == 0) {
        NLM_LOG_ERROR("Invalid visionHeight: " + std::to_string(visionHeight) + ". Must be > 0");
        return;
    }
    
    width_ = width;
    height_ = height;
    visionWidth_ = visionWidth;
    visionHeight_ = visionHeight;
    
    NLM_LOG_INFO("World configured: " + std::to_string(width_) + "x" + std::to_string(height_) + 
                 ", vision: " + std::to_string(visionWidth_) + "x" + std::to_string(visionHeight_));
}

void SimpleWorld::setRandomSeed(uint64_t seed) {
    rngSeed_ = seed;
    rngState_ = seed;
    
    if (!seed) {
        NLM_LOG_WARNING("Random seed is 0 - using default seed 42");
        rngSeed_ = 42;
        rngState_ = 42;
    }
    
    NLM_LOG_DEBUG("Random seed set to: " + std::to_string(rngSeed_));
}

void SimpleWorld::reset() {
    simTime_ = 0.0;
    agent_.reset();
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
    
    NLM_LOG_INFO("World reset: " + std::to_string(objects_.size()) + " objects placed");
}

void SimpleWorld::setAgentStart(float x, float y) {
    if (!isValidPosition(x, y)) {
        NLM_LOG_ERROR("Invalid agent start position: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
        return;
    }
    
    agent_.x = x;
    agent_.y = y;
    NLM_LOG_INFO("Agent start position set to: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
}

void SimpleWorld::update(double timestep) {
    if (timestep <= 0.0) {
        NLM_LOG_ERROR("Invalid timestep: " + std::to_string(timestep) + ". Must be > 0");
        return;
    }
    
    simTime_ += timestep;
    
    // Apply velocity to position
    agent_.x += agent_.velocityX * timestep;
    agent_.y += agent_.velocityY * timestep;
    
    // Apply angular velocity to orientation
    agent_.orientation += agent_.angularVelocity * timestep;
    
    // Normalize orientation to [-PI, PI]
    while (agent_.orientation > M_PI) agent_.orientation -= 2.0f * M_PI;
    while (agent_.orientation < -M_PI) agent_.orientation += 2.0f * M_PI;
    
    // Boundary collision with validation
    if (agent_.x < 0.0f) { agent_.x = 0.0f; agent_.velocityX = 0.0f; }
    if (agent_.x >= width_) { agent_.x = width_ - 0.01f; agent_.velocityX = 0.0f; }
    if (agent_.y < 0.0f) { agent_.y = 0.0f; agent_.velocityY = 0.0f; }
    if (agent_.y >= height_) { agent_.y = height_ - 0.01f; agent_.velocityY = 0.0f; }
    
    // Energy decay
    agent_.energy -= energyDecayRate_;
    if (agent_.energy < 0.0f) {
        agent_.energy = 0.0f;
        NLM_LOG_WARNING("Agent energy depleted");
    }
    
    // Update age
    agent_.age += timestep;
    
    // Generate vision
    generateVision();
    
    // Check collisions
    checkCollisions();
    
    NLM_LOG_DEBUG("World updated at time " + std::to_string(simTime_) + ": " +
                  "pos=(" + std::to_string(agent_.x) + ", " + std::to_string(agent_.y) + ")");
}

uint64_t SimpleWorld::nextRandom() {
    return lcg(rngState_);
}

ActionResult SimpleWorld::applyMotorCommand(MotorCommand cmd, double currentTime) {
    ActionResult result;
    const float moveSpeed = 3.0f;
    const float turnSpeed = 2.0f;
    
    // Validate current time
    if (currentTime < 0.0) {
        NLM_LOG_ERROR("Invalid currentTime: " + std::to_string(currentTime) + ". Must be >= 0");
        result.success = false;
        result.message = "invalid_time";
        return result;
    }
    
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
                result.message = "wall_collision";
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
                result.message = "wall_collision";
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
            result.message = "look_left";
            break;
            
        case MotorCommand::LookRight:
            result.reward = 0.0f;
            result.success = true;
            result.message = "look_right";
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
            result.message = "wait";
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
                NLM_LOG_INFO("Agent consumed resource at (" + std::to_string(obj.x) + ", " + 
                           std::to_string(obj.y) + ")");
            } else if (obj.type == WorldObjectType::Hazard) {
                agent_.health -= 0.2f;
                result.reward = obj.value;  // Negative reward
                result.success = true;
                result.message = "hit_hazard";
                NLM_LOG_WARNING("Agent hit hazard at (" + std::to_string(obj.x) + ", " + 
                               std::to_string(obj.y) + ")");
            }
            
            // Validate health
            if (agent_.health < 0.0f) {
                agent_.health = 0.0f;
                NLM_LOG_WARNING("Agent health depleted");
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
    // Check world boundaries with validation
    if (x < 0.0f || x >= width_ || y < 0.0f || y >= height_) {
        return false;
    }
    
    // Additional safety margin for agent size (agent is ~1 unit diameter)
    if (x < 0.5f || x > width_ - 0.5f || y < 0.5f || y > height_ - 0.5f) {
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
    // Validate object before adding
    if (!obj.active) {
        NLM_LOG_WARNING("Attempting to add inactive object");
    }
    
    if (!isValidPosition(obj.x, obj.y)) {
        NLM_LOG_ERROR("Invalid object position: (" + std::to_string(obj.x) + ", " + 
                      std::to_string(obj.y) + ")");
        return;
    }
    
    objects_.push_back(obj);
    NLM_LOG_DEBUG("Object added at (" + std::to_string(obj.x) + ", " + 
                  std::to_string(obj.y) + ") with type " + std::to_string(static_cast<int>(obj.type)));
}

void SimpleWorld::removeObject(float x, float y) {
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - x;
        float dy = obj.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj.radius) {
            obj.active = false;
            NLM_LOG_DEBUG("Object removed at (" + std::to_string(obj.x) + ", " + 
                          std::to_string(obj.y) + ")");
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
    
    // Validate and clamp internal signals
    for (size_t i = 0; i < internal.size(); ++i) {
        if (std::isfinite(internal[i])) {
            internal[i] = std::clamp(internal[i], 0.0f, 1.0f);
        } else {
            NLM_LOG_WARNING("Invalid internal signal at index " + std::to_string(i) + ". Using 0.");
            internal[i] = 0.0f;
        }
    }
    
    sensory_.setInternal(internal);
    
    // Generate proprioception (body position/velocity)
    std::vector<float> proprioception(6, 0.0f);
    proprioception[0] = agent_.x / width_;     // Normalized X
    proprioception[1] = agent_.y / height_;    // Normalized Y
    proprioception[2] = (agent_.orientation + M_PI) / (2.0f * M_PI);  // Normalized orientation
    proprioception[3] = std::abs(agent_.velocityX) / 5.0f;  // Speed X
    proprioception[4] = std::abs(agent_.velocityY) / 5.0f;  // Speed Y
    proprioception[5] = std::abs(agent_.angularVelocity) / 3.0f;  // Turn rate
    
    // Validate proprioception values
    for (size_t i = 0; i < proprioception.size(); ++i) {
        if (!std::isfinite(proprioception[i])) {
            NLM_LOG_WARNING("Invalid proprioception value at index " + std::to_string(i) + ". Clamping.");
            proprioception[i] = std::clamp(proprioception[i], 0.0f, 1.0f);
        }
    }
    
    sensory_.setProprioception(proprioception);
    
    // No audio for now
    sensory_.setAudio({});
}

void SimpleWorld::checkCollisions() {
    // Passive collision check (for damage when moving into objects)
    WorldObject* obj = getObjectAt(agent_.x, agent_.y);
    if (obj != nullptr && obj->type == WorldObjectType::Hazard) {
        agent_.health -= 0.01f;  // Passive damage
        
        // Validate health
        if (agent_.health <= 0.0f) {
            NLM_LOG_ERROR("Agent health depleted due to hazard collision");
            agent_.health = 0.0f;
        } else {
            NLM_LOG_WARNING("Agent took passive damage from hazard collision");
        }
    }
}
}

} // namespace nlm
