#include "SimpleWorld.hpp"
#include "../core/Exceptions.hpp"
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
    if (width <= 0.0f || height <= 0.0f) {
        NLM_LOG_ERROR("SimpleWorld::configure: invalid dimensions (width=" + std::to_string(width) + ", height=" + std::to_string(height) + ")");
        return;
    }
    if (visionWidth == 0 || visionHeight == 0) {
        NLM_LOG_ERROR("SimpleWorld::configure: invalid vision dimensions (width=" + std::to_string(visionWidth) + ", height=" + std::to_string(visionHeight) + ")");
        return;
    }
    
    width_ = width;
    height_ = height;
    visionWidth_ = visionWidth;
    visionHeight_ = visionHeight;
    
    NLM_LOG_INFO("SimpleWorld configured: " + std::to_string(width_) + "x" + std::to_string(height_) + 
                 " vision " + std::to_string(visionWidth_) + "x" + std::to_string(visionHeight_));
}

void SimpleWorld::setRandomSeed(uint64_t seed) {
    rngSeed_ = seed;
    rngState_ = seed;
}

void SimpleWorld::reset() {
    try {
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
        
        NLM_LOG_INFO("SimpleWorld reset: agent at (" + std::to_string(agent_.x) + ", " + std::to_string(agent_.y) + ")");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error resetting SimpleWorld: ") + e.what());
        throw;
    }
}

void SimpleWorld::setAgentStart(float x, float y) {
    if (!isValidPosition(x, y)) {
        NLM_LOG_ERROR("SimpleWorld::setAgentStart: invalid position (" + std::to_string(x) + ", " + std::to_string(y) + ")");
        return;
    }
    agent_.x = x;
    agent_.y = y;
    NLM_LOG_INFO("Agent positioned at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
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
    
    // Check for valid command
    if (cmd == MotorCommand::Wait) {
        // Decay velocities
        agent_.velocityX *= 0.9f;
        agent_.velocityY *= 0.9f;
        agent_.angularVelocity *= 0.9f;
        agent_.isMoving = false;
        agent_.isTurning = false;
        result.reward = 0.0f;
        result.success = true;
        result.message = "wait_command";
        return result;
    }
    
    // Only allow one action per step
    if (currentTime - agent_.lastActionTime < 0.1) {
        result.reward = -0.05f;  // Small penalty for trying to act too soon
        result.success = false;
        result.message = "too_soon";
        return result;
    }
    
    try {
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
                    agent_.velocityX = dx;
                    agent_.velocityY = dy;
                    agent_.isMoving = true;
                    result.reward = -0.01f;  // Small cost for movement
                    result.success = true;
                    result.message = "move_forward";
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
                    agent_.velocityX = dx;
                    agent_.velocityY = dy;
                    agent_.isMoving = true;
                    result.reward = -0.02f;
                    result.success = true;
                    result.message = "move_backward";
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
                result.message = "turn_left";
                break;
                
            case MotorCommand::TurnRight:
                agent_.angularVelocity = turnSpeed;
                agent_.isTurning = true;
                result.reward = -0.005f;
                result.success = true;
                result.message = "turn_right";
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
                
            default:
                result.reward = 0.0f;
                result.success = false;
                result.message = "unknown_command";
                break;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error applying motor command: ") + e.what());
        result.reward = -0.1f;  // Penalty for command failure
        result.success = false;
        result.message = "command_error";
    }
    
    return result;
}

ActionResult SimpleWorld::interact() {
    ActionResult result;
    result.reward = 0.0f;
    result.success = false;
    result.message = "no_interaction";
    
    try {
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
                    
                    NLM_LOG_INFO("Agent collected resource: +" + std::to_string(obj.value) + " energy");
                } else if (obj.type == WorldObjectType::Hazard) {
                    agent_.health -= 0.2f;
                    result.reward = obj.value;  // Negative reward
                    result.success = true;
                    result.message = "hit_hazard";
                    
                    NLM_LOG_WARNING("Agent hit hazard: -" + std::to_string(std::abs(obj.value)) + " health");
                }
                
                // Only interact with one object at a time
                break;
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error in interact(): ") + e.what());
        result.reward = -0.1f;
        result.success = false;
        result.message = "interact_error";
    }
    
    return result;
}

bool SimpleWorld::isValidPosition(float x, float y) const {
    // Check world boundaries with some tolerance for floating point precision
    if (x < -1.0f || x > width_ + 1.0f || y < -1.0f || y > height_ + 1.0f) {
        return false;
    }
    
    // Check for objects that occupy the space
    for (const auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - x;
        float dy = obj.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj.radius) {
            return false;
        }
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
