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
            
        case MotorCommand::LookUp:
            // Tilt sensor up
            agent_.headPitch = std::min(agent_.headPitch + 0.5f, 1.0f);
            generateVision();
            result.reward = 0.0f;
            result.success = true;
            break;
            
        case MotorCommand::LookDown:
            // Tilt sensor down
            agent_.headPitch = std::max(agent_.headPitch - 0.5f, -1.0f);
            generateVision();
            result.reward = 0.0f;
            result.success = true;
            break;
            
        case MotorCommand::Jump:
            // Jump action with physics
            if (agent_.onGround) {
                agent_.velocityY = 5.0f;  // Jump velocity
                agent_.onGround = false;
                agent_.isMoving = true;
                result.reward = -0.5f;  // Jump cost
                result.success = true;
                result.message = "jumped";
            } else {
                result.reward = -0.1f;
                result.success = false;
                result.message = "already_airborne";
            }
            break;
            
        case MotorCommand::Climb:
            // Climb action
            if (agent_.climbing) {
                // Continue climbing
                agent_.climbProgress += 0.1f;
                agent_.y += 0.1f;
                agent_.energy -= 0.05f;
                result.reward = -0.1f;
                result.success = true;
                result.message = "climbing";
            } else {
                // Start climbing if near climbable surface
                float climbRange = 1.0f;
                WorldObject* climbable = getObjectAt(agent_.x, agent_.y - climbRange);
                if (climbable && climbable->type == WorldObjectType::Wall) {
                    agent_.climbing = true;
                    agent_.climbProgress = 0.0f;
                    result.reward = -0.2f;
                    result.success = true;
                    result.message = "started_climbing";
                } else {
                    result.reward = -0.1f;
                    result.success = false;
                    result.message = "no_climbable_surface";
                }
            }
            break;
            
        case MotorCommand::Fall:
            // Fall action (allow falling)
            if (agent_.onGround) {
                // Start falling process
                agent_.falling = true;
                agent_.fallTimer = 0.0f;
                result.reward = 0.0f;
                result.success = true;
                result.message = "started_falling";
            } else {
                result.reward = -0.1f;
                result.success = false;
                result.message = "already_falling";
            }
            break;
            
        case MotorCommand::Roll:
            // Roll action (slide/kick)
            if (agent_.onGround) {
                float rollSpeed = 3.0f;
                agent_.angularVelocity = rollSpeed * 2.0f;  // Roll in place
                agent_.isTurning = true;
                agent_.energy -= 0.1f;
                result.reward = -0.2f;
                result.success = true;
                result.message = "rolled";
            } else {
                result.reward = -0.1f;
                result.success = false;
                result.message = "need_ground_to_roll";
            }
            break;
            
        case MotorCommand::Grab:
            // Grab action (pick up object)
            if (agent_.grabbing) {
                result.reward = -0.05f;
                result.success = false;
                result.message = "already_grabbing";
            } else {
                float grabRange = 1.0f;
                WorldObject* object = getObjectAt(agent_.x, agent_.y);
                if (object && object->active && object->type == WorldObjectType::Resource) {
                    // Pick up resource
                    object->active = false;
                    agent_.grabbedObject = object;
                    agent_.grabbing = true;
                    agent_.grabbedValue = object->value;
                    agent_.energy -= 0.1f;
                    result.reward = -0.1f;
                    result.success = true;
                    result.message = "grabbed_object";
                } else if (object && object->active) {
                    // Can't grab hazard
                    result.reward = -0.2f;
                    result.success = false;
                    result.message = "cannot_grab_hazard";
                } else {
                    result.reward = -0.1f;
                    result.success = false;
                    result.message = "no_object_to_grab";
                }
            }
            break;
            
        case MotorCommand::Release:
            // Release action (drop object)
            if (agent_.grabbing && agent_.grabbedObject) {
                // Drop the object at current position
                WorldObject dropped;
                dropped.x = agent_.x;
                dropped.y = agent_.y;
                dropped.radius = 0.5f;
                dropped.type = WorldObjectType::Resource;
                dropped.value = agent_.grabbedValue;
                dropped.active = true;
                
                objects_.push_back(dropped);
                
                // Gain energy from released resource
                float energyGained = std::min(maxEnergy_ - agent_.energy, agent_.grabbedValue * 0.1f);
                agent_.energy += energyGained;
                
                agent_.grabbing = false;
                agent_.grabbedObject = nullptr;
                agent_.grabbedValue = 0.0f;
                
                result.reward = energyGained * 0.5f;
                result.success = true;
                result.message = "released_object";
            } else {
                result.reward = -0.05f;
                result.success = false;
                result.message = "not_grabbing_anything";
            }
            break;
            
        case MotorCommand::Push:
            // Push action (push nearby object)
            if (agent_.grabbing && agent_.grabbedObject) {
                // Push the grabbed object
                agent_.grabbedObject->x += std::cos(agent_.orientation) * 0.5f;
                agent_.grabbedObject->y += std::sin(agent_.orientation) * 0.5f;
                agent_.energy -= 0.05f;
                result.reward = -0.1f;
                result.success = true;
                result.message = "pushed_object";
            } else {
                result.reward = -0.1f;
                result.success = false;
                result.message = "nothing_to_push";
            }
            break;
            
        case MotorCommand::Pull:
            // Pull action (pull nearby object)
            if (agent_.grabbing && agent_.grabbedObject) {
                // Pull the grabbed object toward agent
                float dx = agent_.x - agent_.grabbedObject->x;
                float dy = agent_.y - agent_.grabbedObject->y;
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist > 0.1f) {
                    float force = 0.3f;
                    agent_.grabbedObject->x += dx / dist * force;
                    agent_.grabbedObject->y += dy / dist * force;
                }
                agent_.energy -= 0.05f;
                result.reward = -0.1f;
                result.success = true;
                result.message = "pulled_object";
            } else {
                result.reward = -0.1f;
                result.success = false;
                result.message = "nothing_to_pull";
            }
            break;
            
        case MotorCommand::Lift:
            // Lift action (lift object overhead)
            if (agent_.grabbing && agent_.grabbedObject) {
                // Lift object to a higher position
                if (agent_.grabbedObject->y < height_ - 1.0f) {
                    agent_.grabbedObject->y += 0.2f;
                    agent_.energy -= 0.1f;
                    result.reward = -0.1f;
                    result.success = true;
                    result.message = "lifted_object";
                } else {
                    result.reward = -0.1f;
                    result.success = false;
                    result.message = "too_high_to_lift";
                }
            } else {
                result.reward = -0.1f;
                result.success = false;
                result.message = "nothing_to_lift";
            }
            break;
            
        case MotorCommand::Drop:
            // Drop action (release grabbed object)
            if (agent_.grabbing && agent_.grabbedObject) {
                // Drop object at current position
                WorldObject dropped;
                dropped.x = agent_.x;
                dropped.y = agent_.y;
                dropped.radius = agent_.grabbedObject->radius;
                dropped.type = agent_.grabbedObject->type;
                dropped.value = agent_.grabbedObject->value;
                dropped.active = true;
                
                objects_.push_back(dropped);
                
                agent_.grabbing = false;
                agent_.grabbedObject = nullptr;
                agent_.grabbedValue = 0.0f;
                
                // Different reward for different object types
                if (dropped.type == WorldObjectType::Resource) {
                    float energyGained = std::min(maxEnergy_ - agent_.energy, dropped.value * 0.1f);
                    agent_.energy += energyGained;
                    result.reward = energyGained * 0.5f + 0.2f;  // Base reward for dropping
                }
                
                result.success = true;
                result.message = "dropped_object";
            } else {
                result.reward = -0.1f;
                result.success = false;
                result.message = "nothing_to_drop";
            }
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
