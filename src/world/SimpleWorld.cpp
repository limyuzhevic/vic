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
    try {
        // Validate world dimensions
        if (width_ <= 0.0f || height_ <= 0.0f) {
            Logger::getGlobal().error("SimpleWorld::reset: Invalid world dimensions: " + std::to_string(width_) + " x " + std::to_string(height_));
            return;
        }
        
        simTime_ = 0.0;
        agent_.reset();
        
        // Validate agent start position
        if (agent_.x < 0.0f || agent_.x >= width_ || agent_.y < 0.0f || agent_.y >= height_) {
            Logger::getGlobal().warning("SimpleWorld::reset: Agent start position out of bounds, placing in center");
            agent_.x = width_ * 0.5f;
            agent_.y = height_ * 0.5f;
        }
        
        // Place agent at valid position
        agent_.x = std::clamp(agent_.x, 2.0f, width_ - 2.0f);
        agent_.y = std::clamp(agent_.y, 2.0f, height_ - 2.0f);
        
        agent_.energy = maxEnergy_;
        
        // Reset RNG for reproducible object placement
        rngState_ = rngSeed_;
        
        objects_.clear();
        
        // Place some resource objects (positive reward when touched)
        for (int i = 0; i < 5; ++i) {
            float ox = urand(rngState_, 1.0f, width_ - 1.0f);
            float oy = urand(rngState_, 1.0f, height_ - 1.0f);
            
            // Validate object placement
            if (isValidPosition(ox, oy)) {
                objects_.emplace_back(ox, oy, WorldObjectType::Resource, 10.0f, 0.4f);
            } else {
                Logger::getGlobal().warning("SimpleWorld::reset: Invalid resource object position, skipping");
            }
        }
        
        // Place some hazard objects (negative reward when touched)
        for (int i = 0; i < 3; ++i) {
            float ox = urand(rngState_, 1.0f, width_ - 1.0f);
            float oy = urand(rngState_, 1.0f, height_ - 1.0f);
            
            // Validate object placement
            if (isValidPosition(ox, oy)) {
                objects_.emplace_back(ox, oy, WorldObjectType::Hazard, -15.0f, 0.3f);
            } else {
                Logger::getGlobal().warning("SimpleWorld::reset: Invalid hazard object position, skipping");
            }
        }
        
        generateVision();
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("SimpleWorld::reset: Exception during world reset: " + std::string(e.what()));
    }
}

void SimpleWorld::setAgentStart(float x, float y) {
    agent_.x = x;
    agent_.y = y;
}

void SimpleWorld::update(double timestep) {
    try {
        // Validate timestep
        if (timestep <= 0.0) {
            Logger::getGlobal().error("SimpleWorld::update: Invalid timestep: " + std::to_string(timestep));
            return;
        }
        
        // Validate agent state
        if (std::isnan(agent_.x) || std::isnan(agent_.y) || std::isnan(agent_.orientation)) {
            Logger::getGlobal().error("SimpleWorld::update: Agent state contains NaN values");
            return;
        }
        
        simTime_ += timestep;
        
        // Apply velocity to position
        agent_.x += agent_.velocityX * static_cast<float>(timestep);
        agent_.y += agent_.velocityY * static_cast<float>(timestep);
        
        // Apply angular velocity to orientation
        agent_.orientation += agent_.angularVelocity * static_cast<float>(timestep);
        
        // Normalize orientation to [-PI, PI] with bounds checking
        while (agent_.orientation > M_PI) agent_.orientation -= 2.0f * M_PI;
        while (agent_.orientation < -M_PI) agent_.orientation += 2.0f * M_PI;
        
        // Check for NaN or infinite values
        if (std::isnan(agent_.orientation) || std::isinf(agent_.orientation)) {
            Logger::getGlobal().warning("SimpleWorld::update: Invalid orientation, resetting to 0");
            agent_.orientation = 0.0f;
        }
        
        // Boundary collision with bounds checking
        if (agent_.x < 0.0f) {
            agent_.x = 0.0f;
            agent_.velocityX = 0.0f;
        } else if (agent_.x >= width_) {
            agent_.x = width_ - 0.01f;
            agent_.velocityX = 0.0f;
        }
        
        if (agent_.y < 0.0f) {
            agent_.y = 0.0f;
            agent_.velocityY = 0.0f;
        } else if (agent_.y >= height_) {
            agent_.y = height_ - 0.01f;
            agent_.velocityY = 0.0f;
        }
        
        // Validate agent position after collision
        agent_.x = std::clamp(agent_.x, 0.0f, width_);
        agent_.y = std::clamp(agent_.y, 0.0f, height_);
        
        // Energy decay with bounds checking
        agent_.energy -= energyDecayRate_;
        if (agent_.energy < 0.0f) {
            agent_.energy = 0.0f;
        } else if (agent_.energy > maxEnergy_) {
            agent_.energy = maxEnergy_;
        }
        
        // Update age
        agent_.age += timestep;
        
        // Validate energy
        if (agent_.energy < 0.0f || agent_.energy > maxEnergy_ * 2.0f) {
            Logger::getGlobal().warning("SimpleWorld::update: Energy out of reasonable range: " + std::to_string(agent_.energy));
        }
        
        generateVision();
        
        checkCollisions();
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("SimpleWorld::update: Exception during world update: " + std::string(e.what()));
    }
}

uint64_t SimpleWorld::nextRandom() {
    return lcg(rngState_);
}

ActionResult SimpleWorld::applyMotorCommand(MotorCommand cmd, double currentTime) {
    try {
        // Validate input parameters
        if (currentTime < 0.0) {
            Logger::getGlobal().error("SimpleWorld::applyMotorCommand: Invalid current time: " + std::to_string(currentTime));
            return ActionResult(0.0f, false, "invalid_time");
        }
        
        if (std::isnan(static_cast<float>(currentTime))) {
            Logger::getGlobal().error("SimpleWorld::applyMotorCommand: NaN current time");
            return ActionResult(0.0f, false, "nan_time");
        }
        
        ActionResult result;
        const float moveSpeed = 3.0f;
        const float turnSpeed = 2.0f;
        
        // Only allow one action per step with timing validation
        if (currentTime - agent_.lastActionTime < 0.1) {
            return ActionResult(0.0f, false, "too_soon");
        }
        
        agent_.lastActionTime = currentTime;
        
        switch (cmd) {
            case MotorCommand::MoveForward:
            case MotorCommand::MoveBackward:
            case MotorCommand::TurnLeft:
            case MotorCommand::TurnRight:
            case MotorCommand::LookLeft:
            case MotorCommand::LookRight:
            case MotorCommand::Interact:
            case MotorCommand::Wait:
                // Valid motor command
                break;
            default:
                Logger::getGlobal().warning("SimpleWorld::applyMotorCommand: Invalid motor command");
                return ActionResult(0.0f, false, "invalid_command");
        }
        
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
        
        // Validate result
        if (std::isnan(result.reward)) {
            result.reward = 0.0f;
            Logger::getGlobal().warning("SimpleWorld::applyMotorCommand: NaN reward, setting to 0");
        }
        
        return result;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("SimpleWorld::applyMotorCommand: Exception applying motor command: " + std::string(e.what()));
        return ActionResult(0.0f, false, "exception");
    }
}

ActionResult SimpleWorld::interact() {
    ActionResult result;
    result.reward = 0.0f;
    result.success = false;
    
    // Validate agent position
    if (std::isnan(agent_.x) || std::isnan(agent_.y)) {
        Logger::getGlobal().error("SimpleWorld::interact: Agent position contains NaN values");
        return result;
    }
    
    // Check for objects in interaction range
    const float interactRange = 1.0f;
    
    try {
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
                } else {
                    Logger::getGlobal().warning("SimpleWorld::interact: Unknown object type encountered");
                }
                
                // Only interact with one object at a time
                break;
            }
        }
    } catch (const std::exception& e) {
        Logger::getGlobal().error("SimpleWorld::interact: Exception during interaction: " + std::string(e.what()));
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
    try {
        // Validate agent state before generating vision
        if (std::isnan(agent_.x) || std::isnan(agent_.y) || std::isnan(agent_.orientation)) {
            Logger::getGlobal().error("SimpleWorld::generateVision: Agent state contains NaN values");
            return;
        }
        
        // Validate world dimensions
        if (width_ <= 0.0f || height_ <= 0.0f) {
            Logger::getGlobal().error("SimpleWorld::generateVision: Invalid world dimensions");
            return;
        }
        
        // Simple raycasting-based vision
        // Cast rays in a cone in front of the agent
        
        const int rayCount = 16;
        const float fov = M_PI / 2.0f;  // 90 degree FOV
        const float maxRange = 8.0f;
        
        std::vector<float> vision(rayCount, 1.0f);  // Default: far/empty
        
        try {
            for (int i = 0; i < rayCount; ++i) {
                // Validate ray count
                if (i < 0) {
                    Logger::getGlobal().error("SimpleWorld::generateVision: Invalid ray index");
                    continue;
                }
                
                // Angle for this ray
                float angle = agent_.orientation - fov / 2.0f + (fov * i / (rayCount - 1));
                
                // Check for NaN in angle
                if (std::isnan(angle)) {
                    Logger::getGlobal().warning("SimpleWorld::generateVision: NaN angle, setting to 0");
                    angle = 0.0f;
                }
                
                // Cast ray
                float dx = std::cos(angle);
                float dy = std::sin(angle);
                
                float t = 0.0f;
                while (t < maxRange) {
                    float px = agent_.x + dx * t;
                    float py = agent_.y + dy * t;
                    
                    // Validate ray casting calculations
                    if (std::isnan(px) || std::isnan(py)) {
                        Logger::getGlobal().warning("SimpleWorld::generateVision: NaN in ray casting, breaking");
                        break;
                    }
                    
                    // Check wall collision
                    if (!isValidPosition(px, py)) {
                        vision[i] = t / maxRange;
                        break;
                    }
                    
                    // Check object collision
                    WorldObject* obj = getObjectAt(px, py);
                    if (obj != nullptr) {
                        // Validate object pointer
                        if (!obj->active) {
                            continue;
                        }
                        
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
                    
                    // Validate t
                    if (std::isnan(t) || std::isinf(t)) {
                        Logger::getGlobal().warning("SimpleWorld::generateVision: Invalid t value, breaking");
                        break;
                    }
                }
                
                // Validate vision value
                if (std::isnan(vision[i]) || std::isinf(vision[i])) {
                    vision[i] = 1.0f;
                    Logger::getGlobal().warning("SimpleWorld::generateVision: NaN/inf vision value, resetting to 1.0");
                }
            }
        } catch (const std::exception& e) {
            Logger::getGlobal().error("SimpleWorld::generateVision: Exception during ray casting: " + std::string(e.what()));
            // Fallback to default vision
            vision.assign(rayCount, 1.0f);
        }
        
        try {
            sensory_.setVision(vision);
        } catch (const std::exception& e) {
            Logger::getGlobal().error("SimpleWorld::generateVision: Exception setting sensory vision: " + std::string(e.what()));
        }
        
        try {
            sensory_.setTimestamp(simTime_);
        } catch (const std::exception& e) {
            Logger::getGlobal().error("SimpleWorld::generateVision: Exception setting timestamp: " + std::string(e.what()));
        }
        
        // Generate touch sensors (proximity to obstacles in 8 directions)
        std::vector<float> touch(8, 0.0f);
        try {
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
        } catch (const std::exception& e) {
            Logger::getGlobal().error("SimpleWorld::generateVision: Exception generating touch sensors: " + std::string(e.what()));
        }
        
        // Generate internal signals (energy, health)
        std::vector<float> internal(4, 0.0f);
        try {
            internal[0] = agent_.energy / maxEnergy_;  // Energy level
            internal[1] = agent_.health;               // Health
            internal[2] = agent_.isMoving ? 1.0f : 0.0f;  // Movement state
            internal[3] = agent_.isTurning ? 1.0f : 0.0f;  // Turning state
            
            sensory_.setInternal(internal);
        } catch (const std::exception& e) {
            Logger::getGlobal().error("SimpleWorld::generateVision: Exception generating internal signals: " + std::string(e.what()));
        }
        
        // Generate proprioception (body position/velocity)
        std::vector<float> proprioception(6, 0.0f);
        try {
            proprioception[0] = agent_.x / width_;     // Normalized X
            proprioception[1] = agent_.y / height_;    // Normalized Y
            proprioception[2] = (agent_.orientation + M_PI) / (2.0f * M_PI);  // Normalized orientation
            proprioception[3] = std::abs(agent_.velocityX) / 5.0f;  // Speed X
            proprioception[4] = std::abs(agent_.velocityY) / 5.0f;  // Speed Y
            proprioception[5] = std::abs(agent_.angularVelocity) / 3.0f;  // Turn rate
            
            sensory_.setProprioception(proprioception);
        } catch (const std::exception& e) {
            Logger::getGlobal().error("SimpleWorld::generateVision: Exception generating proprioception: " + std::string(e.what()));
        }
        
        // No audio for now
        sensory_.setAudio({});
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("SimpleWorld::generateVision: Unexpected exception: " + std::string(e.what()));
    }
}

void SimpleWorld::checkCollisions() {
    try {
        // Validate agent position
        if (std::isnan(agent_.x) || std::isnan(agent_.y)) {
            Logger::getGlobal().error("SimpleWorld::checkCollisions: Agent position contains NaN values");
            return;
        }
        
        // Passive collision check (for damage when moving into objects)
        WorldObject* obj = getObjectAt(agent_.x, agent_.y);
        if (obj != nullptr && obj->type == WorldObjectType::Hazard) {
            // Validate hazard damage
            if (std::isnan(obj->value) || obj->value >= 0.0f) {
                Logger::getGlobal().warning("SimpleWorld::checkCollisions: Invalid hazard value: " + std::to_string(obj->value));
                return;
            }
            
            agent_.health -= 0.01f;  // Passive damage
            
            // Validate health bounds
            if (agent_.health < 0.0f) {
                agent_.health = 0.0f;
                Logger::getGlobal().warning("SimpleWorld::checkCollisions: Health depleted to 0");
            }
            
            if (agent_.health <= 0.0f) {
                // Agent died - should handle this in game logic
                Logger::getGlobal().info("SimpleWorld::checkCollisions: Agent died from hazard contact");
            }
        }
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("SimpleWorld::checkCollisions: Exception during collision check: " + std::string(e.what()));
    }
}

} // namespace nlm
