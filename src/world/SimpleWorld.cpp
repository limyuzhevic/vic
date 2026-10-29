// SimpleWorld.cpp - Implementation of the SimpleWorld class

#include "SimpleWorld.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

SimpleWorld::SimpleWorld()
    : width_(10.0f), height_(10.0f)
    , visionWidth_(8), visionHeight_(8)
    , maxEnergy_(100.0f), energyDecayRate_(0.01f), energyGainRate_(0.05f)
    , simTime_(0.0), rngSeed_(42), rngState_(0) {
    agent_.reset();
    sensory_ = SensoryPercept();
}

SimpleWorld::~SimpleWorld() = default;

void SimpleWorld::configure(float width, float height, size_t visionWidth, size_t visionHeight) {
    width_ = width;
    height_ = height;
    visionWidth_ = visionWidth;
    visionHeight_ = visionHeight;
    
    // Generate vision grid
    generateVision();
}

void SimpleWorld::reset() {
    agent_.reset();
    simTime_ = 0.0;
    
    // Clear all objects except walls
    std::vector<WorldObject> validObjects;
    for (const auto& obj : objects_) {
        if (obj.type == WorldObjectType::Wall) {
            validObjects.push_back(obj);
        }
    }
    objects_ = validObjects;
    
    // Add some default walls
    if (objects_.empty()) {
        // Add boundary walls
        for (float x = 0.0f; x <= width_; x += 0.5f) {
            objects_.push_back(WorldObject(x, 0.0f, WorldObjectType::Wall, 0.0f, 0.2f));
            objects_.push_back(WorldObject(x, height_, WorldObjectType::Wall, 0.0f, 0.2f));
        }
        for (float y = 1.0f; y < height_; y += 0.5f) {
            objects_.push_back(WorldObject(0.0f, y, WorldObjectType::Wall, 0.0f, 0.2f));
            objects_.push_back(WorldObject(width_, y, WorldObjectType::Wall, 0.0f, 0.2f));
        }
    }
    
    // Generate initial vision
    generateVision();
}

void SimpleWorld::setAgentStart(float x, float y) {
    if (isValidPosition(x, y)) {
        agent_.x = x;
        agent_.y = y;
        generateVision();
    }
}

void SimpleWorld::update(double timestep) {
    simTime_ += timestep;
    
    // Update agent physics
    float vx = agent_.velocityX;
    float vy = agent_.velocityY;
    
    // Apply movement
    agent_.x += vx * timestep;
    agent_.y += vy * timestep;
    
    // Handle boundary wrapping
    if (agent_.x < 0.0f) agent_.x += width_;
    else if (agent_.x > width_) agent_.x -= width_;
    if (agent_.y < 0.0f) agent_.y += height_;
    else if (agent_.y > height_) agent_.y -= height_;
    
    // Decay energy
    agent_.energy = std::max(0.0f, agent_.energy - energyDecayRate_ * timestep);
    
    // Generate vision for next step
    generateVision();
}

ActionResult SimpleWorld::applyMotorCommand(MotorCommand cmd, double currentTime) {
    // Store current position for collision detection
    float oldX = agent_.x;
    float oldY = agent_.y;
    
    // Update velocity based on command
    switch (cmd) {
        case MotorCommand::MoveForward:
            agent_.velocityX += std::cos(agent_.orientation) * 2.0f;
            agent_.velocityY += std::sin(agent_.orientation) * 2.0f;
            break;
        case MotorCommand::MoveBackward:
            agent_.velocityX -= std::cos(agent_.orientation) * 2.0f;
            agent_.velocityY -= std::sin(agent_.orientation) * 2.0f;
            break;
        case MotorCommand::TurnLeft:
            agent_.orientation -= 0.1f;
            break;
        case MotorCommand::TurnRight:
            agent_.orientation += 0.1f;
            break;
        case MotorCommand::LookLeft:
            agent_.orientation -= 0.05f;
            break;
        case MotorCommand::LookRight:
            agent_.orientation += 0.05f;
            break;
        case MotorCommand::Interact:
            return interact();
        case MotorCommand::Wait:
            break;
    }
    
    // Apply velocity damping
    agent_.velocityX *= 0.9f;
    agent_.velocityY *= 0.9f;
    agent_.angularVelocity *= 0.8f;
    
    // Check for collisions with objects
    checkCollisions();
    
    // Check if position is valid
    bool positionValid = isValidPosition(agent_.x, agent_.y);
    bool objectCollision = getObjectAt(agent_.x, agent_.y) != nullptr;
    
    agent_.isMoving = (std::abs(agent_.velocityX) > 0.01f || std::abs(agent_.velocityY) > 0.01f);
    agent_.isTurning = (std::abs(agent_.orientation) > 0.01f);
    
    if (!positionValid || objectCollision) {
        // Collision occurred - revert movement
        agent_.x = oldX;
        agent_.y = oldY;
        return ActionResult(-0.1f, false, "Collision detected");
    }
    
    // Successful movement
    agent_.lastActionTime = static_cast<float>(currentTime);
    return ActionResult(0.0f, true, "Moved successfully");
}

void SimpleWorld::generateVision() {
    // Create vision grid for agent's current position and orientation
    std::vector<float> vision(visionWidth_ * visionHeight_, 0.0f);
    
    float maxRange = 8.0f;
    
    for (int i = 0; i < visionWidth_ * visionHeight_; ++i) {
        int x = i % visionWidth_;
        int y = i / visionWidth_;
        
        // Convert to angle and distance from agent
        float angle = agent_.orientation - M_PI / 2.0f + 
                     static_cast<float>(x) / (visionWidth_ - 1) * M_PI / 2.0f;
        float dist = static_cast<float>(y) / (visionHeight_ - 1) * maxRange;
        
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
}

void SimpleWorld::checkCollisions() {
    // Check for collisions with world boundaries and objects
    float margin = 0.2f;
    
    if (agent_.x < margin || agent_.x > width_ - margin ||
        agent_.y < margin || agent_.y > height_ - margin) {
        // Push back to valid position
        if (agent_.x < margin) agent_.x = margin;
        else if (agent_.x > width_ - margin) agent_.x = width_ - margin;
        if (agent_.y < margin) agent_.y = margin;
        else if (agent_.y > height_ - margin) agent_.y = height_ - margin;
    }
}

ActionResult SimpleWorld::interact() {
    // Check for objects within reach
    float reach = 0.5f;
    WorldObject* obj = getObjectAt(agent_.x, agent_.y);
    
    if (obj == nullptr) {
        // Check nearby positions
        for (const auto& testObj : objects_) {
            float dx = agent_.x - testObj.x;
            float dy = agent_.y - testObj.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist <= reach) {
                obj = const_cast<WorldObject*>(&testObj);
                break;
            }
        }
    }
    
    if (obj != nullptr) {
        // Interact with object based on type
        if (obj->type == WorldObjectType::Resource) {
            agent_.energy = std::min(maxEnergy_, agent_.energy + 20.0f);
            return ActionResult(1.0f, true, "Collected resource");
        } else if (obj->type == WorldObjectType::Hazard) {
            agent_.health = std::max(0.0f, agent_.health - 0.2f);
            return ActionResult(-0.5f, false, "Hazard damage");
        } else if (obj->type == WorldObjectType::Marker) {
            return ActionResult(0.5f, true, "Found marker");
        }
    }
    
    return ActionResult(-0.1f, false, "Nothing to interact with");
}

void SimpleWorld::addObject(const WorldObject& obj) {
    objects_.push_back(obj);
}

void SimpleWorld::removeObject(float x, float y) {
    auto it = std::remove_if(objects_.begin(), objects_.end(),
        [x, y](const WorldObject& obj) {
            return (std::abs(obj.x - x) < 0.1f && std::abs(obj.y - y) < 0.1f);
        });
    objects_.erase(it, objects_.end());
}

bool SimpleWorld::isValidPosition(float x, float y) const {
    // Check if position is within bounds
    if (x < 0.0f || x > width_ || y < 0.0f || y > height_) {
        return false;
    }
    
    // Check if position is inside a wall (wall is 0.2f radius at corners)
    for (const auto& obj : objects_) {
        if (obj.type == WorldObjectType::Wall) {
            float dx = x - obj.x;
            float dy = y - obj.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist < obj.radius) {
                return false;
            }
        }
    }
    
    return true;
}

WorldObject* SimpleWorld::getObjectAt(float x, float y) {
    for (auto& obj : objects_) {
        float dx = x - obj.x;
        float dy = y - obj.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj.radius) {
            return &obj;
        }
    }
    return nullptr;
}

void SimpleWorld::addBrainRegion(std::unique_ptr<NeuralRegion> region) {
    brainRegions_.push_back(std::move(region));
}

ActionResult SimpleWorld::applyAction(const NeuralRegion* brainRegion, MotorCommand cmd) {
    // Simple implementation: just apply the motor command
    ActionResult result = applyMotorCommand(cmd, simTime_);
    
    // If brain region exists, try to influence the action
    if (brainRegion) {
        // For now, just return the standard result
        // In a real implementation, this could use brain region activity to modify behavior
        return result;
    }
    
    return result;
}

float SimpleWorld::computeReward(const NeuralRegion* brainRegion) {
    // Simple reward based on agent's internal state
    float reward = 0.0f;
    
    // Energy-based reward
    if (agent_.energy > 0.5f) {
        reward += 0.1f;
    } else if (agent_.energy < 0.2f) {
        reward -= 0.2f;
    }
    
    // Health-based reward
    if (agent_.health > 0.5f) {
        reward += 0.05f;
    } else if (agent_.health < 0.3f) {
        reward -= 0.1f;
    }
    
    return reward;
}

SensoryPercept SimpleWorld::observe(const NeuralRegion* brainRegion) {
    // Return current sensory percept
    // In a real implementation, this could filter/analyze based on brain region
    return sensory_;
}

void SimpleWorld::setRandomSeed(uint64_t seed) {
    rngSeed_ = seed;
    rngState_ = seed;
}

uint64_t SimpleWorld::nextRandom() {
    // Simple LCG random number generator
    rngState_ = rngState_ * 1664525 + 1013904223;
    return rngState_;
}

} // namespace nlm