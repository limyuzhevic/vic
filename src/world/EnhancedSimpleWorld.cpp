#include "EnhancedSimpleWorld.hpp"
#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <sstream>

namespace nlm {

struct EnhancedSimpleWorld::Impl {
    // Configuration
    std::shared_ptr<WorldConfig> config_;
    WorldStats stats_;
    
    // World state
    AgentBody agent_;
    std::vector<WorldObject> objects_;
    SensoryPercept sensory_;
    double simTime_;
    
    // Random number generation
    uint64_t rngSeed_;
    uint64_t rngState_;
    
    // Advanced features
    std::string environmentType_;
    std::unordered_map<std::string, std::vector<std::pair<float, float>>> hazardPatterns_;
    std::unordered_map<std::string, std::vector<std::pair<float, float>>> resourcePatterns_;
    
    // Performance
    bool debugLoggingEnabled_;
    
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
    
    Impl() : config_(std::make_shared<WorldConfig>()), simTime_(0.0), rngSeed_(42), rngState_(42),
             debugLoggingEnabled_(false) {
        stats_.startTime_ = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    ~Impl() {
        objects_.clear();
    }
    
    void log(const std::string& message) const {
        if (debugLoggingEnabled_) {
            std::cout << "[EnhancedSimpleWorld] " << message << std::endl;
        }
    }
    
    void updateStats(double timestep) {
        stats_.totalSteps_++;
        stats_.lastStepTime_ = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
        
        // Update energy statistics
        if (agent_.energy < config_->maxEnergy_) {
            stats_.totalEnergyConsumed_ += config_->maxEnergy_ - agent_.energy;
        }
        
        // Update object statistics
        stats_.objectsDestroyed_ = 0;
        for (auto& obj : objects_) {
            if (!obj.active) {
                stats_.objectsDestroyed_++;
            }
        }
    }
    
    void generateVision() {
        // Simple raycasting-based vision
        // Cast rays in a cone in front of the agent
        
        std::vector<float> vision(config_->visionRayCount_, 1.0f); // Default: far/empty
        
        for (size_t i = 0; i < config_->visionRayCount_; ++i) {
            // Angle for this ray
            float angle = agent_.orientation - config_->visionFieldOfView_ / 2.0f + 
                         (config_->visionFieldOfView_ * i / (config_->visionRayCount_ - 1));
            
            // Cast ray
            float dx = std::cos(angle);
            float dy = std::sin(angle);
            
            float t = 0.0f;
            while (t < config_->visionMaxRange_) {
                float px = agent_.x + dx * t;
                float py = agent_.y + dy * t;
                
                // Check wall collision
                if (!isValidPosition(px, py)) {
                    vision[i] = t / config_->visionMaxRange_;
                    break;
                }
                
                // Check object collision
                WorldObject* obj = getObjectAt(px, py);
                if (obj != nullptr) {
                    // Encode object type as intensity pattern
                    // Resource = bright, Hazard = dark, different distances
                    if (obj->type == WorldObjectType::Resource) {
                        vision[i] = (config_->visionMaxRange_ - t) / config_->visionMaxRange_ * 0.8f; // Bright
                    } else if (obj->type == WorldObjectType::Hazard) {
                        vision[i] = (config_->visionMaxRange_ - t) / config_->visionMaxRange_ * 0.3f; // Dark
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
        for (size_t i = 0; i < 8; ++i) {
            float angle = agent_.orientation + 2.0f * M_PI * i / 8.0f;
            float dx = std::cos(angle);
            float dy = std::sin(angle);
            
            float t = 0.0f;
            while (t < config_->interactRange_) {
                float px = agent_.x + dx * t;
                float py = agent_.y + dy * t;
                
                if (!isValidPosition(px, py) || getObjectAt(px, py) != nullptr) {
                    touch[i] = 1.0f - t / config_->interactRange_; // Closer = stronger signal
                    break;
                }
                t += 0.1f;
            }
        }
        sensory_.setTouch(touch);
        
        // Generate internal signals (energy, health)
        std::vector<float> internal(4, 0.0f);
        internal[0] = agent_.energy / config_->maxEnergy_; // Energy level
        internal[1] = agent_.health;                       // Health
        internal[2] = agent_.isMoving ? 1.0f : 0.0f;     // Movement state
        internal[3] = agent_.isTurning ? 1.0f : 0.0f;   // Turning state
        sensory_.setInternal(internal);
        
        // Generate proprioception (body position/velocity)
        std::vector<float> proprioception(6, 0.0f);
        proprioception[0] = agent_.x / config_->width_;     // Normalized X
        proprioception[1] = agent_.y / config_->height_;    // Normalized Y
        proprioception[2] = (agent_.orientation + M_PI) / (2.0f * M_PI);  // Normalized orientation
        proprioception[3] = std::abs(agent_.velocityX) / 5.0f;  // Speed X
        proprioception[4] = std::abs(agent_.velocityY) / 5.0f;  // Speed Y
        proprioception[5] = std::abs(agent_.angularVelocity) / 3.0f;  // Turn rate
        sensory_.setProprioception(proprioception);
        
        // No audio for now
        sensory_.setAudio({});
        
        log("Vision generated: " + std::to_string(vision.size()) + " rays, " + 
            std::to_string(agent_.energy) + " energy, " + std::to_string(agent_.health) + " health");
    }
    
    void checkCollisions() {
        // Passive collision check (for damage when moving into objects)
        WorldObject* obj = getObjectAt(agent_.x, agent_.y);
        if (obj != nullptr && obj->type == WorldObjectType::Hazard) {
            agent_.health -= 0.01f;  // Passive damage
            stats_.collisionEvents_++;
            log("Collision with hazard at position (" + std::to_string(agent_.x) + ", " + 
                std::to_string(agent_.y) + ") - health: " + std::to_string(agent_.health));
        }
    }
    
    ActionResult interact() {
        ActionResult result;
        result.reward = 0.0f;
        result.success = false;
        
        // Check for objects in interaction range
        const float interactRange = config_->interactRange_;
        
        for (auto& obj : objects_) {
            if (!obj.active) continue;
            
            float dx = obj.x - agent_.x;
            float dy = obj.y - agent_.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist < interactRange + obj.radius) {
                obj.active = false;  // Consume the object
                
                if (obj.type == WorldObjectType::Resource) {
                    agent_.energy = std::min(config_->maxEnergy_, agent_.energy + config_->resourceEnergyGain_);
                    result.reward = obj.value;  // Positive reward
                    result.success = true;
                    result.message = "consumed_resource";
                    stats_.interactionEvents_++;
                    log("Agent consumed resource at (" + std::to_string(obj.x) + ", " + 
                        std::to_string(obj.y) + ") - energy: " + std::to_string(agent_.energy));
                } else if (obj.type == WorldObjectType::Hazard) {
                    agent_.health -= 0.2f;
                    result.reward = obj.value;  // Negative reward
                    result.success = true;
                    result.message = "hit_hazard";
                    stats_.interactionEvents_++;
                    log("Agent hit hazard at (" + std::to_string(obj.x) + ", " + 
                        std::to_string(obj.y) + ") - health: " + std::to_string(agent_.health));
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
    
    bool isValidPosition(float x, float y) const {
        // Check world boundaries with tolerance
        if (x < -config_->collisionTolerance_ || x >= config_->width_ - config_->collisionTolerance_ ||
            y < -config_->collisionTolerance_ || y >= config_->height_ - config_->collisionTolerance_) {
            return false;
        }
        return true;
    }
    
    WorldObject* getObjectAt(float x, float y) {
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
    
    void placeObjectsRandomly() {
        objects_.clear();
        
        // Place hazard objects based on patterns
        for (const auto& pattern : hazardPatterns_) {
            for (const auto& pos : pattern.second) {
                if (isValidPosition(pos.first, pos.second)) {
                    float value = config_->hazardDamage_ * (0.5f + urand(rngState_, 0.0f, 1.0f));
                    objects_.emplace_back(pos.first, pos.second, WorldObjectType::Hazard, value, 0.3f);
                }
            }
        }
        
        // Add default hazards if no patterns
        if (hazardPatterns_.empty()) {
            for (size_t i = 0; i < config_->initialHazards_; ++i) {
                float ox = urand(rngState_, 1.0f, config_->width_ - 1.0f);
                float oy = urand(rngState_, 1.0f, config_->height_ - 1.0f);
                objects_.emplace_back(ox, oy, WorldObjectType::Hazard, config_->hazardDamage_, 0.3f);
            }
        }
        
        // Place resource objects based on patterns
        for (const auto& pattern : resourcePatterns_) {
            for (const auto& pos : pattern.second) {
                if (isValidPosition(pos.first, pos.second)) {
                    float value = config_->resourceValue_ * (0.5f + urand(rngState_, 0.0f, 1.0f));
                    objects_.emplace_back(pos.first, pos.second, WorldObjectType::Resource, value, 0.4f);
                }
            }
        }
        
        // Add default resources if no patterns
        if (resourcePatterns_.empty()) {
            for (size_t i = 0; i < config_->initialResources_; ++i) {
                float ox = urand(rngState_, 1.0f, config_->width_ - 1.0f);
                float oy = urand(rngState_, 1.0f, config_->height_ - 1.0f);
                objects_.emplace_back(ox, oy, WorldObjectType::Resource, config_->resourceValue_, 0.4f);
            }
        }
        
        stats_.objectsCreated_ = objects_.size();
        log("World initialized with " + std::to_string(objects_.size()) + " objects");
    }
};

EnhancedSimpleWorld::EnhancedSimpleWorld(std::shared_ptr<WorldConfig> config)
    : pImpl_(new Impl()) {
    pImpl_->config_ = config;
    
    // Set up configuration defaults
    if (!pImpl_->config_) {
        pImpl_->config_ = std::make_shared<WorldConfig>();
    }
    
    pImpl_->config_->width_ = 20.0f;
    pImpl_->config_->height_ = 20.0f;
    pImpl_->config_->maxEnergy_ = 100.0f;
    pImpl_->config_->energyDecayRate_ = 0.01f;
    pImpl_->config_->resourceEnergyGain_ = 30.0f;
    pImpl_->config_->initialResources_ = 5;
    pImpl_->config_->initialHazards_ = 3;
    pImpl_->config_->resourceValue_ = 10.0f;
    pImpl_->config_->hazardDamage_ = -15.0f;
    
    pImpl_->log("EnhancedSimpleWorld created");
}

EnhancedSimpleWorld::~EnhancedSimpleWorld() {
    delete pImpl_;
}

EnhancedSimpleWorld::EnhancedSimpleWorld(EnhancedSimpleWorld&& other) noexcept : pImpl_(other.pImpl_) {
    other.pImpl_ = nullptr;
}

EnhancedSimpleWorld& EnhancedSimpleWorld::operator=(EnhancedSimpleWorld&& other) noexcept {
    if (this != &other) {
        delete pImpl_;
        pImpl_ = other.pImpl_;
        other.pImpl_ = nullptr;
    }
    return *this;
}

void EnhancedSimpleWorld::configure(float width, float height, size_t visionWidth, size_t visionHeight) {
    if (!pImpl_) return;
    
    pImpl_->config_->width_ = width;
    pImpl_->config_->height_ = height;
    pImpl_->sensory_.setVision(std::vector<float>(visionWidth * visionHeight, 0.0f));
    pImpl_->sensory_.getVisionWidth() = visionWidth;
    pImpl_->sensory_.getVisionHeight() = visionHeight;
    
    pImpl_->log("World configured: " + std::to_string(width) + "x" + std::to_string(height) + 
                " vision: " + std::to_string(visionWidth) + "x" + std::to_string(visionHeight));
}

void EnhancedSimpleWorld::setRandomSeed(uint64_t seed) {
    if (!pImpl_) return;
    
    pImpl_->config_->randomSeed_ = seed;
    pImpl_->rngSeed_ = seed;
    pImpl_->rngState_ = seed;
    
    pImpl_->log("Random seed set to: " + std::to_string(seed));
}

void EnhancedSimpleWorld::reset() {
    if (!pImpl_) return;
    
    pImpl_->simTime_ = 0.0;
    pImpl_->agent_.reset();
    pImpl_->agent_.x = pImpl_->urand(pImpl_->rngState_, 2.0f, pImpl_->config_->width_ - 2.0f);
    pImpl_->agent_.y = pImpl_->urand(pImpl_->rngState_, 2.0f, pImpl_->config_->height_ - 2.0f);
    pImpl_->agent_.energy = pImpl_->config_->maxEnergy_;
    pImpl_->rngState_ = pImpl_->rngSeed_;  // Reset RNG for reproducible object placement
    
    pImpl_->placeObjectsRandomly();
    pImpl_->generateVision();
    
    pImpl_->log("World reset complete");
}

void EnhancedSimpleWorld::setAgentStart(float x, float y) {
    if (!pImpl_) return;
    
    pImpl_->agent_.x = x;
    pImpl_->agent_.y = y;
}

void EnhancedSimpleWorld::update(double timestep) {
    if (!pImpl_) return;
    
    pImpl_->simTime_ += timestep;
    
    // Apply velocity to position
    pImpl_->agent_.x += pImpl_->agent_.velocityX * timestep;
    pImpl_->agent_.y += pImpl_->agent_.velocityY * timestep;
    
    // Apply angular velocity to orientation
    pImpl_->agent_.orientation += pImpl_->agent_.angularVelocity * timestep;
    
    // Normalize orientation to [-PI, PI]
    while (pImpl_->agent_.orientation > M_PI) pImpl_->agent_.orientation -= 2.0f * M_PI;
    while (pImpl_->agent_.orientation < -M_PI) pImpl_->agent_.orientation += 2.0f * M_PI;
    
    // Boundary collision with damping
    if (pImpl_->agent_.x < 0.0f) {
        pImpl_->agent_.x = 0.0f;
        pImpl_->agent_.velocityX = -pImpl_->agent_.velocityX * pImpl_->config_->velocityDamping_;
    } else if (pImpl_->agent_.x >= pImpl_->config_->width_) {
        pImpl_->agent_.x = pImpl_->config_->width_ - pImpl_->config_->collisionTolerance_;
        pImpl_->agent_.velocityX = -pImpl_->agent_.velocityX * pImpl_->config_->velocityDamping_;
    }
    
    if (pImpl_->agent_.y < 0.0f) {
        pImpl_->agent_.y = 0.0f;
        pImpl_->agent_.velocityY = -pImpl_->agent_.velocityY * pImpl_->config_->velocityDamping_;
    } else if (pImpl_->agent_.y >= pImpl_->config_->height_) {
        pImpl_->agent_.y = pImpl_->config_->height_ - pImpl_->config_->collisionTolerance_;
        pImpl_->agent_.velocityY = -pImpl_->agent_.velocityY * pImpl_->config_->velocityDamping_;
    }
    
    // Energy decay
    pImpl_->agent_.energy -= pImpl_->config_->energyDecayRate_ * timestep;
    if (pImpl_->agent_.energy < 0.0f) pImpl_->agent_.energy = 0.0f;
    
    // Update age
    pImpl_->agent_.age += timestep;
    
    // Generate vision
    pImpl_->generateVision();
    
    // Check collisions
    pImpl_->checkCollisions();
    
    pImpl_->updateStats(timestep);
}

ActionResult EnhancedSimpleWorld::applyMotorCommand(MotorCommand cmd, double currentTime) {
    if (!pImpl_) return ActionResult();
    
    ActionResult result;
    const float moveSpeed = pImpl_->config_->moveSpeed_;
    const float turnSpeed = pImpl_->config_->turnSpeed_;
    
    // Only allow one action per step (cooldown)
    if (currentTime - pImpl_->agent_.lastActionTime < pImpl_->config_->actionCooldown_) {
        return ActionResult(0.0f, false, "too soon");
    }
    
    pImpl_->agent_.lastActionTime = currentTime;
    pImpl_->stats_.totalActions_++;
    
    switch (cmd) {
        case MotorCommand::MoveForward: {
            float dx = std::cos(pImpl_->agent_.orientation) * moveSpeed;
            float dy = std::sin(pImpl_->agent_.orientation) * moveSpeed;
            
            float newX = pImpl_->agent_.x + dx * 0.1f;
            float newY = pImpl_->agent_.y + dy * 0.1f;
            
            if (pImpl_->isValidPosition(newX, newY)) {
                pImpl_->agent_.x = newX;
                pImpl_->agent_.y = newY;
                pImpl_->agent_.isMoving = true;
                result.reward = -0.01f;  // Small cost for movement
                result.success = true;
                pImpl_->log("Agent moved forward to (" + std::to_string(newX) + ", " + 
                            std::to_string(newY) + ")");
            } else {
                result.reward = -0.05f;  // Cost for hitting wall
                result.success = false;
                pImpl_->log("Agent hit wall at (" + std::to_string(newX) + ", " + 
                            std::to_string(newY) + ")");
            }
            break;
        }
        
        case MotorCommand::MoveBackward: {
            float dx = -std::cos(pImpl_->agent_.orientation) * moveSpeed * 0.5f;
            float dy = -std::sin(pImpl_->agent_.orientation) * moveSpeed * 0.5f;
            
            float newX = pImpl_->agent_.x + dx * 0.1f;
            float newY = pImpl_->agent_.y + dy * 0.1f;
            
            if (pImpl_->isValidPosition(newX, newY)) {
                pImpl_->agent_.x = newX;
                pImpl_->agent_.y = newY;
                pImpl_->agent_.isMoving = true;
                result.reward = -0.02f;
                result.success = true;
            } else {
                result.reward = -0.05f;
                result.success = false;
            }
            break;
        }
        
        case MotorCommand::TurnLeft:
            pImpl_->agent_.angularVelocity = -turnSpeed;
            pImpl_->agent_.isTurning = true;
            result.reward = -0.005f;
            result.success = true;
            break;
            
        case MotorCommand::TurnRight:
            pImpl_->agent_.angularVelocity = turnSpeed;
            pImpl_->agent_.isTurning = true;
            result.reward = -0.005f;
            result.success = true;
            break;
            
        case MotorCommand::LookLeft:
        case MotorCommand::LookRight:
            // Pan sensor (no movement)
            result.reward = 0.0f;
            result.success = true;
            break;
            
        case MotorCommand::Interact:
            result = pImpl_->interact();
            break;
            
        case MotorCommand::Wait:
        default:
            // Decay velocities
            pImpl_->agent_.velocityX *= pImpl_->config_->velocityDamping_;
            pImpl_->agent_.velocityY *= pImpl_->config_->velocityDamping_;
            pImpl_->agent_.angularVelocity *= pImpl_->config_->angularVelocityDamping_;
            pImpl_->agent_.isMoving = false;
            pImpl_->agent_.isTurning = false;
            result.reward = 0.0f;
            result.success = true;
            break;
    }
    
    if (result.success) {
        pImpl_->stats_.successfulActions_++;
    }
    
    return result;
}

void EnhancedSimpleWorld::addObject(const WorldObject& obj) {
    if (!pImpl_) return;
    
    if (pImpl_->objects_.size() >= pImpl_->config_->maxObjects_) {
        pImpl_->log("Warning: Maximum objects reached, removing oldest");
        pImpl_->objects_.erase(pImpl_->objects_.begin());
    }
    
    pImpl_->objects_.push_back(obj);
    pImpl_->stats_.objectsCreated_++;
}

void EnhancedSimpleWorld::removeObject(float x, float y) {
    if (!pImpl_) return;
    
    for (auto& obj : pImpl_->objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - x;
        float dy = obj.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj.radius) {
            obj.active = false;
            pImpl_->stats_.objectsDestroyed_++;
            pImpl_->log("Object removed at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
            return;
        }
    }
}

WorldObject* EnhancedSimpleWorld::getObjectAt(float x, float y) {
    if (!pImpl_) return nullptr;
    return pImpl_->getObjectAt(x, y);
}

bool EnhancedSimpleWorld::isValidPosition(float x, float y) const {
    if (!pImpl_) return false;
    return pImpl_->isValidPosition(x, y);
}

double EnhancedSimpleWorld::getUptime() const {
    if (!pImpl_) return 0.0;
    return pImpl_->stats_.startTime_ > 0.0 ? 
           (std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count() - pImpl_->stats_.startTime_) : 0.0;
}

std::string EnhancedSimpleWorld::getStatsString() const {
    if (!pImpl_) return "EnhancedSimpleWorld not initialized";
    
    std::ostringstream stream;
    stream << "=== EnhancedSimpleWorld Statistics ===\n";
    stream << "Simulation Time: " << pImpl_->simTime_ << "s\n";
    stream << "Agent Position: (" << pImpl_->agent_.x << ", " << pImpl_->agent_.y << ")\n";
    stream << "Agent Energy: " << pImpl_->agent_.energy << " / " << pImpl_->config_->maxEnergy_ << "\n";
    stream << "Agent Health: " << pImpl_->agent_.health << "\n";
    stream << "Total Steps: " << pImpl_->stats_.totalSteps_ << "\n";
    stream << "Total Actions: " << pImpl_->stats_.totalActions_ << "\n";
    stream << "Successful Actions: " << pImpl_->stats_.successfulActions_ << "\n";
    stream << "Collision Events: " << pImpl_->stats_.collisionEvents_ << "\n";
    stream << "Interaction Events: " << pImpl_->stats_.interactionEvents_ << "\n";
    stream << "Objects Created: " << pImpl_->stats_.objectsCreated_ << "\n";
    stream << "Objects Destroyed: " << pImpl_->stats_.objectsDestroyed_ << "\n";
    stream << "Uptime: " << getUptime() << "s\n";
    stream << "====================================\n";
    
    return stream.str();
}

bool EnhancedSimpleWorld::isValid() const {
    if (!pImpl_) return false;
    return pImpl_->config_ && pImpl_->agent_.energy >= 0.0f && pImpl_->agent_.health >= 0.0f;
}

std::string EnhancedSimpleWorld::getDebugInfo() const {
    if (!pImpl_) return "EnhancedSimpleWorld not initialized";
    
    std::ostringstream stream;
    stream << "=== EnhancedSimpleWorld Debug Info ===\n";
    stream << "World Valid: " << (isValid() ? "Yes" : "No") << "\n";
    stream << "Agent Valid: " << (pImpl_->agent_.health > 0.0f ? "Yes" : "No") << "\n";
    stream << "Configuration: " << pImpl_->config_->width_ << "x" << pImpl_->config_->height_ << "\n";
    stream << "Vision Rays: " << pImpl_->config_->visionRayCount_ << "\n";
    stream << "Max Energy: " << pImpl_->config_->maxEnergy_ << "\n";
    stream << "Energy Decay Rate: " << pImpl_->config_->energyDecayRate_ << "\n";
    stream << "Random Seed: " << pImpl_->config_->randomSeed_ << "\n";
    stream << "Debug Logging: " << (debugLoggingEnabled_ ? "Yes" : "No") << "\n";
    stream << "Environment Type: " << pImpl_->environmentType_ << "\n";
    stream << "Object Count: " << pImpl_->objects_.size() << "\n";
    stream << "Active Objects: " << std::count_if(pImpl_->objects_.begin(), pImpl_->objects_.end(), 
                                                 [](const WorldObject& obj) { return obj.active; }) << "\n";
    stream << "====================================\n";
    
    return stream.str();
}

void EnhancedSimpleWorld::addHazardPattern(const std::string& pattern) {
    if (!pImpl_) return;
    
    // Simple pattern parsing - format: "name:x1,y1;x2,y2;..."
    size_t colonPos = pattern.find(':');
    if (colonPos == std::string::npos) return;
    
    std::string name = pattern.substr(0, colonPos);
    std::string positions = pattern.substr(colonPos + 1);
    
    std::vector<std::pair<float, float>> posList;
    size_t start = 0;
    size_t end = positions.find(';');
    
    while (end != std::string::npos) {
        std::string pos = positions.substr(start, end - start);
        size_t commaPos = pos.find(',');
        if (commaPos != std::string::npos) {
            float x = std::stof(pos.substr(0, commaPos));
            float y = std::stof(pos.substr(commaPos + 1));
            posList.emplace_back(x, y);
        }
        start = end + 1;
        end = positions.find(';', start);
    }
    
    if (start < positions.length()) {
        std::string pos = positions.substr(start);
        size_t commaPos = pos.find(',');
        if (commaPos != std::string::npos) {
            float x = std::stof(pos.substr(0, commaPos));
            float y = std::stof(pos.substr(commaPos + 1));
            posList.emplace_back(x, y);
        }
    }
    
    pImpl_->hazardPatterns_[name] = posList;
    pImpl_->log("Added hazard pattern: " + name + " with " + std::to_string(posList.size()) + " positions");
}

void EnhancedSimpleWorld::addResourcePattern(const std::string& pattern) {
    if (!pImpl_) return;
    
    // Simple pattern parsing - format: "name:x1,y1;x2,y2;..."
    size_t colonPos = pattern.find(':');
    if (colonPos == std::string::npos) return;
    
    std::string name = pattern.substr(0, colonPos);
    std::string positions = pattern.substr(colonPos + 1);
    
    std::vector<std::pair<float, float>> posList;
    size_t start = 0;
    size_t end = positions.find(';');
    
    while (end != std::string::npos) {
        std::string pos = positions.substr(start, end - start);
        size_t commaPos = pos.find(',');
        if (commaPos != std::string::npos) {
            float x = std::stof(pos.substr(0, commaPos));
            float y = std::stof(pos.substr(commaPos + 1));
            posList.emplace_back(x, y);
        }
        start = end + 1;
        end = positions.find(';', start);
    }
    
    if (start < positions.length()) {
        std::string pos = positions.substr(start);
        size_t commaPos = pos.find(',');
        if (commaPos != std::string::npos) {
            float x = std::stof(pos.substr(0, commaPos));
            float y = std::stof(pos.substr(commaPos + 1));
            posList.emplace_back(x, y);
        }
    }
    
    pImpl_->resourcePatterns_[name] = posList;
    pImpl_->log("Added resource pattern: " + name + " with " + std::to_string(posList.size()) + " positions");
}

} // namespace nlm