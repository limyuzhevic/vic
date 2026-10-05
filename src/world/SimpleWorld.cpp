#include "SimpleWorld.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <numeric>

namespace nlm {

// VisionRayCaster implementation
VisionRayCaster::VisionRayCaster(float worldWidth, float worldHeight)
    : worldWidth_(worldWidth), worldHeight_(worldHeight), fov_(WorldConfig::fovAngle),
      maxRange_(WorldConfig::maxVisionRange), rayCount_(WorldConfig::rayCount),
      anglesDirty_(true) {
    updatePrecomputedAngles();
}

float VisionRayCaster::castSingleRay(float startX, float startY, float angle,
                                   const std::vector<WorldObject>& objects) const {
    float dx = std::cos(angle);
    float dy = std::sin(angle);
    
    float t = 0.0f;
    while (t < maxRange_) {
        float px = startX + dx * t;
        float py = startY + dy * t;
        
        // Check world boundaries
        if (px < 0.0f || px >= worldWidth_ || py < 0.0f || py >= worldHeight_) {
            return t / maxRange_;
        }
        
        // Check object collision
        for (const auto& obj : objects) {
            if (!obj.active) continue;
            
            float dist = std::sqrt((px - obj.x) * (px - obj.x) + (py - obj.y) * (py - obj.y));
            if (dist < obj.radius) {
                // Encode object type as intensity pattern
                if (obj.type == WorldObjectType::Resource) {
                    return (maxRange_ - t) / maxRange_ * 0.8f;  // Bright
                } else if (obj.type == WorldObjectType::Hazard) {
                    return (maxRange_ - t) / maxRange_ * 0.3f;  // Dark
                } else {
                    return 0.5f;
                }
            }
        }
        
        t += WorldConfig::raySpacing;
    }
    
    return 1.0f; // No obstacle found
}

std::vector<float> VisionRayCaster::castRays(float agentX, float agentY, float orientation) const {
    std::vector<float> vision(rayCount_, 1.0f);
    
    for (size_t i = 0; i < rayCount_; ++i) {
        float angle = orientation - fov_ / 2.0f + (fov_ * static_cast<float>(i) / (rayCount_ - 1));
        vision[i] = castSingleRay(agentX, agentY, angle, {}); // Objects will be passed from outside
    }
    
    return vision;
}

void VisionRayCaster::updatePrecomputedAngles() {
    precomputedAngles_.clear();
    precomputedAngles_.reserve(rayCount_);
    
    for (size_t i = 0; i < rayCount_; ++i) {
        float angle = -fov_ / 2.0f + (fov_ * static_cast<float>(i) / (rayCount_ - 1));
        precomputedAngles_.push_back(angle);
    }
    
    anglesDirty_ = false;
}

// TouchSensorSystem implementation
TouchSensorSystem::TouchSensorSystem(float worldWidth, float worldHeight)
    : worldWidth_(worldWidth), worldHeight_(worldHeight),
      sensorCount_(WorldConfig::touchSensorCount), maxRange_(1.5f) {
}

float TouchSensorSystem::computeSingleSensor(float sensorAngle, float agentX, float agentY,
                                           const std::vector<WorldObject>& objects) const {
    float dx = std::cos(sensorAngle);
    float dy = std::sin(sensorAngle);
    
    float t = 0.0f;
    while (t < maxRange_) {
        float px = agentX + dx * t;
        float py = agentY + dy * t;
        
        // Check world boundaries
        if (px < 0.0f || px >= worldWidth_ || py < 0.0f || py >= worldHeight_) {
            return 1.0f - t / maxRange_;
        }
        
        // Check object collision
        for (const auto& obj : objects) {
            if (!obj.active) continue;
            
            float dist = std::sqrt((px - obj.x) * (px - obj.x) + (py - obj.y) * (py - obj.y));
            if (dist < obj.radius) {
                return 1.0f - t / maxRange_;  // Closer = stronger signal
            }
        }
        
        t += 0.1f;
    }
    
    return 0.0f; // No obstacle found within range
}

std::vector<float> TouchSensorSystem::computeTouchSensors(float agentX, float agentY, float orientation,
                                                         const std::vector<WorldObject>& objects) const {
    std::vector<float> touch(sensorCount_, 0.0f);
    
    for (size_t i = 0; i < sensorCount_; ++i) {
        float angle = orientation + 2.0f * M_PI * static_cast<float>(i) / sensorCount_;
        touch[i] = computeSingleSensor(angle, agentX, agentY, objects);
    }
    
    return touch;
}

// ObjectManager implementation
void ObjectManager::addObject(const WorldObject& obj) {
    objects_.push_back(obj);
    if (WorldConfig::enableObjectCaching) {
        rebuildSpatialGrid();
    }
}

void ObjectManager::removeObject(size_t index) {
    if (index < objects_.size()) {
        objects_[index].active = false;
        if (WorldConfig::enableObjectCaching) {
            rebuildSpatialGrid();
        }
    }
}

void ObjectManager::removeObjectAt(float x, float y) {
    for (auto& obj : objects_) {
        if (obj.active) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < obj.radius) {
                obj.active = false;
                break;
            }
        }
    }
    
    if (WorldConfig::enableObjectCaching) {
        rebuildSpatialGrid();
    }
}

WorldObject* ObjectManager::getObjectAt(float x, float y) {
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

const WorldObject* ObjectManager::getObjectAt(float x, float y) const {
    for (const auto& obj : objects_) {
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

std::vector<size_t> ObjectManager::getObjectsInRange(float x, float y, float range) const {
    std::vector<size_t> result;
    
    for (size_t i = 0; i < objects_.size(); ++i) {
        if (!objects_[i].active) continue;
        
        float dx = objects_[i].x - x;
        float dy = objects_[i].y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < range) {
            result.push_back(i);
        }
    }
    
    return result;
}

void ObjectManager::updateObject(size_t index, float x, float y) {
    if (index < objects_.size()) {
        objects_[index].x = x;
        objects_[index].y = y;
        if (WorldConfig::enableObjectCaching) {
            rebuildSpatialGrid();
        }
    }
}

void ObjectManager::activateObject(size_t index) {
    if (index < objects_.size()) {
        objects_[index].active = true;
    }
}

void ObjectManager::deactivateObject(size_t index) {
    if (index < objects_.size()) {
        objects_[index].active = false;
    }
}

int ObjectManager::getResourceCount() const {
    int count = 0;
    for (const auto& obj : objects_) {
        if (obj.active && obj.type == WorldObjectType::Resource) {
            count++;
        }
    }
    return count;
}

int ObjectManager::getHazardCount() const {
    int count = 0;
    for (const auto& obj : objects_) {
        if (obj.active && obj.type == WorldObjectType::Hazard) {
            count++;
        }
    }
    return count;
}

int ObjectManager::getWallCount() const {
    int count = 0;
    for (const auto& obj : objects_) {
        if (obj.active && obj.type == WorldObjectType::Wall) {
            count++;
        }
    }
    return count;
}

void ObjectManager::clear() {
    objects_.clear();
    if (WorldConfig::enableObjectCaching) {
        rebuildSpatialGrid();
    }
}

void ObjectManager::resetActiveFlags() {
    for (auto& obj : objects_) {
        obj.active = true;
    }
}

void ObjectManager::rebuildSpatialGrid() {
    if (!WorldConfig::enableObjectCaching) {
        spatialGrid_.clear();
        return;
    }
    
    // Simple implementation - in practice, you'd use a more sophisticated spatial indexing
    spatialGrid_.clear();
    // Placeholder for spatial grid implementation
}

// SimpleWorld implementation
SimpleWorld::SimpleWorld()
    : width_(WorldConfig::defaultWidth), height_(WorldConfig::defaultHeight),
      visionWidth_(WorldConfig::defaultVisionWidth), visionHeight_(WorldConfig::defaultVisionHeight),
      maxEnergy_(WorldConfig::defaultMaxEnergy), energyDecayRate_(WorldConfig::energyDecayRate),
      resourceEnergyGain_(WorldConfig::resourceEnergyGain), simTime_(0.0),
      rngSeed_(WorldConfig::defaultRandomSeed), rngState_(WorldConfig::defaultRandomSeed),
      visionCaster_(width_, height_), touchSystem_(width_, height_),
      enablePerformanceMetrics_(WorldConfig::enablePerformanceMetrics),
      enableDebugLogging_(WorldConfig::enableDebugLogging) {
    
    visionCaster_.setFOV(WorldConfig::fovAngle);
    visionCaster_.setMaxRange(WorldConfig::maxVisionRange);
    visionCaster_.setRayCount(WorldConfig::rayCount);
    
    touchSystem_.setSensorCount(WorldConfig::touchSensorCount);
    touchSystem_.setMaxRange(WorldConfig::interactionRange);
    
    agent_.reset();
}

SimpleWorld::~SimpleWorld() = default;

void SimpleWorld::configure(float width, float height, size_t visionWidth, size_t visionHeight) {
    width_ = width;
    height_ = height;
    visionWidth_ = visionWidth;
    visionHeight_ = visionHeight;
    
    // Update dependent systems
    visionCaster_ = VisionRayCaster(width_, height_);
    touchSystem_ = TouchSensorSystem(width_, height_);
    
    visionCaster_.setFOV(WorldConfig::fovAngle);
    visionCaster_.setMaxRange(WorldConfig::maxVisionRange);
    visionCaster_.setRayCount(WorldConfig::rayCount);
    
    touchSystem_.setSensorCount(WorldConfig::touchSensorCount);
    touchSystem_.setMaxRange(WorldConfig::interactionRange);
}

void SimpleWorld::setRandomSeed(uint64_t seed) {
    rngSeed_ = seed;
    rngState_ = seed;
}

void SimpleWorld::reset() {
    simTime_ = 0.0;
    agent_.reset();
    
    // Set random seed for reproducible placement
    rngState_ = rngSeed_;
    
    objectManager_.clear();
    
    // Place some resource objects (positive reward when touched)
    for (int i = 0; i < WorldConfig::defaultResourceCount; ++i) {
        float ox = urand(rngState_, WorldConfig::boundaryBuffer, width_ - WorldConfig::boundaryBuffer);
        float oy = urand(rngState_, WorldConfig::boundaryBuffer, height_ - WorldConfig::boundaryBuffer);
        objectManager_.addObject(WorldObject(ox, oy, WorldObjectType::Resource, 
                                            WorldConfig::resourceEnergyGain, WorldConfig::objectRadius));
    }
    
    // Place some hazard objects (negative reward when touched)
    for (int i = 0; i < WorldConfig::defaultHazardCount; ++i) {
        float ox = urand(rngState_, WorldConfig::boundaryBuffer, width_ - WorldConfig::boundaryBuffer);
        float oy = urand(rngState_, WorldConfig::boundaryBuffer, height_ - WorldConfig::boundaryBuffer);
        objectManager_.addObject(WorldObject(ox, oy, WorldObjectType::Hazard, 
                                            -WorldConfig::hazardHealthLoss, WorldConfig::objectRadius));
    }
    
    generateVision();
}

void SimpleWorld::setAgentStart(float x, float y) {
    agent_.x = x;
    agent_.y = y;
}

void SimpleWorld::update(double timestep) {
    double updateStartTime = simTime_;
    
    if (enablePerformanceMetrics_) {
        metrics_.incrementFrameCount();
    }
    
    // Update physics
    updatePhysics(timestep);
    
    // Generate sensory input
    generateVision();
    generateTouchSensors();
    generateInternalSignals();
    generateProprioception();
    
    // Check for collisions
    checkCollisions();
    
    // Update world objects
    updateObjects(timestep);
    
    // Record performance metrics
    if (enablePerformanceMetrics_) {
        metrics_.recordUpdateTime(simTime_ - updateStartTime);
    }
    
    simTime_ += timestep;
}

void SimpleWorld::updatePhysics(double timestep) {
    // Apply velocity to position
    agent_.x += agent_.velocityX * timestep;
    agent_.y += agent_.velocityY * timestep;
    
    // Apply angular velocity to orientation
    agent_.orientation += agent_.angularVelocity * timestep;
    
    // Normalize orientation to [-PI, PI]
    while (agent_.orientation > M_PI) agent_.orientation -= 2.0f * M_PI;
    while (agent_.orientation < -M_PI) agent_.orientation += 2.0f * M_PI;
    
    // Boundary collision
    handleBoundaryCollisions();
    
    // Energy decay
    agent_.energy -= energyDecayRate_;
    if (agent_.energy < 0.0f) agent_.energy = 0.0f;
    
    // Update age
    agent_.age += timestep;
}

void SimpleWorld::handleBoundaryCollisions() {
    if (!WorldConfig::enableCollisionDetection) return;
    
    // Boundary collision with walls
    if (agent_.x < 0.0f) { agent_.x = 0.0f; agent_.velocityX = 0.0f; }
    if (agent_.x >= width_) { agent_.x = width_ - WorldConfig::wallThickness; agent_.velocityX = 0.0f; }
    if (agent_.y < 0.0f) { agent_.y = 0.0f; agent_.velocityY = 0.0f; }
    if (agent_.y >= height_) { agent_.y = height_ - WorldConfig::wallThickness; agent_.velocityY = 0.0f; }
}

void SimpleWorld::checkCollisions() {
    // Passive collision check (for damage when moving into objects)
    WorldObject* obj = getObjectAt(agent_.x, agent_.y);
    if (obj != nullptr && obj->type == WorldObjectType::Hazard) {
        agent_.health -= WorldConfig::passiveHazardDamage;
    }
}

ActionResult SimpleWorld::applyMotorCommand(MotorCommand cmd, double currentTime) {
    // Performance tracking
    double actionStartTime = simTime_;
    
    ActionResult result;
    result.success = false;
    result.message = "unknown_error";
    
    // Only allow one action per step
    if (currentTime - agent_.lastActionTime < WorldConfig::actionCooldown) {
        result.reward = 0.0f;
        result.success = false;
        result.message = "too_soon";
        
        if (enablePerformanceMetrics_) {
            metrics_.recordInteractionTime(simTime_ - actionStartTime);
        }
        
        return result;
    }
    
    agent_.lastActionTime = currentTime;
    
    switch (cmd) {
        case MotorCommand::MoveForward: {
            float dx = std::cos(agent_.orientation) * WorldConfig::moveSpeed;
            float dy = std::sin(agent_.orientation) * WorldConfig::moveSpeed;
            
            float newX = agent_.x + dx * WorldConfig::defaultTimeStep;
            float newY = agent_.y + dy * WorldConfig::defaultTimeStep;
            
            if (isValidPosition(newX, newY)) {
                agent_.x = newX;
                agent_.y = newY;
                agent_.isMoving = true;
                result.reward = -WorldConfig::moveSpeed * 0.01f;  // Small cost for movement
                result.success = true;
                result.message = "moved_forward";
            } else {
                result.reward = -0.05f;  // Cost for hitting wall
                result.success = false;
                result.message = "wall_collision";
            }
            break;
        }
        
        case MotorCommand::MoveBackward: {
            float dx = -std::cos(agent_.orientation) * WorldConfig::backwardMoveSpeed;
            float dy = -std::sin(agent_.orientation) * WorldConfig::backwardMoveSpeed;
            
            float newX = agent_.x + dx * WorldConfig::defaultTimeStep;
            float newY = agent_.y + dy * WorldConfig::defaultTimeStep;
            
            if (isValidPosition(newX, newY)) {
                agent_.x = newX;
                agent_.y = newY;
                agent_.isMoving = true;
                result.reward = -WorldConfig::backwardMoveSpeed * 0.01f;
                result.success = true;
                result.message = "moved_backward";
            } else {
                result.reward = -0.05f;
                result.success = false;
                result.message = "wall_collision";
            }
            break;
        }
        
        case MotorCommand::TurnLeft:
            agent_.angularVelocity = -WorldConfig::turnSpeed;
            agent_.isTurning = true;
            result.reward = -WorldConfig::turnSpeed * 0.005f;
            result.success = true;
            result.message = "turned_left";
            break;
            
        case MotorCommand::TurnRight:
            agent_.angularVelocity = WorldConfig::turnSpeed;
            agent_.isTurning = true;
            result.reward = -WorldConfig::turnSpeed * 0.005f;
            result.success = true;
            result.message = "turned_right";
            break;
            
        case MotorCommand::LookLeft:
            // Pan sensor (no movement)
            result.reward = 0.0f;
            result.success = true;
            result.message = "looked_left";
            break;
            
        case MotorCommand::LookRight:
            result.reward = 0.0f;
            result.success = true;
            result.message = "looked_right";
            break;
            
        case MotorCommand::Interact:
            result = interact();
            break;
            
        case MotorCommand::Wait:
        default:
            // Decay velocities
            agent_.velocityX *= WorldConfig::velocityDecay;
            agent_.velocityY *= WorldConfig::velocityDecay;
            agent_.angularVelocity *= WorldConfig::velocityDecay;
            agent_.isMoving = false;
            agent_.isTurning = false;
            result.reward = 0.0f;
            result.success = true;
            result.message = "waited";
            break;
    }
    
    // Notify event listeners
    notifyEventListeners("action_applied", std::to_string(result.reward) + "," + std::to_string(result.success));
    
    // Record performance metrics
    if (enablePerformanceMetrics_) {
        metrics_.recordInteractionTime(simTime_ - actionStartTime);
    }
    
    return result;
}

ActionResult SimpleWorld::interact() {
    ActionResult result;
    result.reward = 0.0f;
    result.success = false;
    result.message = "no_object";
    
    // Check for objects in interaction range
    const float interactRange = WorldConfig::interactionRange;
    
    for (auto& obj : objectManager_.getAllObjects()) {
        if (!obj.active) continue;
        
        float dx = obj.x - agent_.x;
        float dy = obj.y - agent_.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < interactRange + obj.radius) {
            obj.active = false;  // Consume the object
            
            // Record event
            notifyEventListeners("object_interacted", 
                               std::to_string(obj.x) + "," + std::to_string(obj.y) + "," +
                               std::to_string(static_cast<int>(obj.type)));
            
            if (obj.type == WorldObjectType::Resource) {
                agent_.energy = std::min(maxEnergy_, agent_.energy + WorldConfig::resourceEnergyGain);
                result.reward = obj.value;  // Positive reward
                result.success = true;
                result.message = "consumed_resource";
            } else if (obj.type == WorldObjectType::Hazard) {
                agent_.health -= WorldConfig::hazardHealthLoss;
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

void SimpleWorld::updateObjects(double timestep) {
    // Update any dynamic objects (future extension)
    for (auto& obj : objectManager_.getAllObjects()) {
        if (!obj.active) continue;
        
        // Simple bouncing for future expansion
        if (obj.type == WorldObjectType::Marker) {
            obj.x += std::cos(simTime_ * 0.5f) * 0.1f * timestep;
            obj.y += std::sin(simTime_ * 0.5f) * 0.1f * timestep;
            
            // Keep within bounds
            obj.x = std::max(WorldConfig::boundaryBuffer, std::min(width_ - WorldConfig::boundaryBuffer, obj.x));
            obj.y = std::max(WorldConfig::boundaryBuffer, std::min(height_ - WorldConfig::boundaryBuffer, obj.y));
        }
    }
}

bool SimpleWorld::isValidPosition(float x, float y) const {
    // Check world boundaries
    if (x < 0.0f || x >= width_ || y < 0.0f || y >= height_) {
        return false;
    }
    
    // Check collision with other objects (simplified)
    for (const auto& obj : objectManager_.getAllObjects()) {
        if (!obj.active) continue;
        
        float dx = obj.x - x;
        float dy = obj.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj.radius + WorldConfig::objectRadius) {
            return false;
        }
    }
    
    return true;
}

WorldObject* SimpleWorld::getObjectAt(float x, float y) {
    return objectManager_.getObjectAt(x, y);
}

const WorldObject* SimpleWorld::getObjectAt(float x, float y) const {
    return objectManager_.getObjectAt(x, y);
}

void SimpleWorld::addObject(const WorldObject& obj) {
    objectManager_.addObject(obj);
    
    if (enableDebugLogging_) {
        logDebug("Object added at (" + std::to_string(obj.x) + "," + std::to_string(obj.y) + ")");
    }
    
    notifyEventListeners("object_created", 
                        std::to_string(obj.x) + "," + std::to_string(obj.y) + "," +
                        std::to_string(static_cast<int>(obj.type)) + "," + std::to_string(obj.value));
}

void SimpleWorld::removeObject(float x, float y) {
    objectManager_.removeObjectAt(x, y);
    
    if (enableDebugLogging_) {
        logDebug("Object removed at (" + std::to_string(x) + "," + std::to_string(y) + ")");
    }
    
    notifyEventListeners("object_destroyed", std::to_string(x) + "," + std::to_string(y));
}

void SimpleWorld::generateVision() {
    double startTime = simTime_;
    
    // Cast rays in a cone in front of the agent
    std::vector<float> vision = visionCaster_.castRays(agent_.x, agent_.y, agent_.orientation);
    
    sensory_.setVision(vision);
    sensory_.setTimestamp(simTime_);
    
    // Record performance metrics
    if (enablePerformanceMetrics_) {
        metrics_.recordVisionTime(simTime_ - startTime);
    }
}

void SimpleWorld::generateTouchSensors() {
    std::vector<float> touch = touchSystem_.computeTouchSensors(
        agent_.x, agent_.y, agent_.orientation, objectManager_.getAllObjects());
    
    sensory_.setTouch(touch);
}

void SimpleWorld::generateInternalSignals() {
    std::vector<float> internal(WorldConfig::internalSensorCount, 0.0f);
    internal[0] = agent_.energy / maxEnergy_;  // Energy level
    internal[1] = agent_.health;               // Health
    internal[2] = agent_.isMoving ? 1.0f : 0.0f;  // Movement state
    internal[3] = agent_.isTurning ? 1.0f : 0.0f;  // Turning state
    
    sensory_.setInternal(internal);
}

void SimpleWorld::generateProprioception() {
    std::vector<float> proprioception(WorldConfig::proprioceptionSensorCount, 0.0f);
    proprioception[0] = agent_.x / width_;     // Normalized X
    proprioception[1] = agent_.y / height_;    // Normalized Y
    proprioception[2] = (agent_.orientation + M_PI) / (2.0f * M_PI);  // Normalized orientation
    proprioception[3] = std::abs(agent_.velocityX) / WorldConfig::maxVelocity;  // Speed X
    proprioception[4] = std::abs(agent_.velocityY) / WorldConfig::maxVelocity;  // Speed Y
    proprioception[5] = std::abs(agent_.angularVelocity) / WorldConfig::maxAngularVelocity;  // Turn rate
    
    sensory_.setProprioception(proprioception);
}

void SimpleWorld::logDebug(const std::string& message) const {
    if (enableDebugLogging_) {
        std::cout << "[DEBUG] " << message << std::endl;
    }
}

void SimpleWorld::notifyEventListeners(const std::string& eventType, const std::string& data) const {
    for (auto* listener : eventListeners_) {
        if (listener) {
            if (eventType == "action_applied") {
                size_t pos = data.find(',');
                if (pos != std::string::npos) {
                    float reward = std::stof(data.substr(0, pos));
                    bool success = data.substr(pos + 1) == "true";
                    listener->onInteraction(reward, success, "");
                }
            } else if (eventType == "object_created") {
                size_t pos1 = data.find(',');
                size_t pos2 = data.find(',', pos1 + 1);
                size_t pos3 = data.find(',', pos2 + 1);
                if (pos1 != std::string::npos && pos2 != std::string::npos && pos3 != std::string::npos) {
                    float x = std::stof(data.substr(0, pos1));
                    float y = std::stof(data.substr(pos1 + 1, pos2 - pos1 - 1));
                    int type = std::stoi(data.substr(pos2 + 1, pos3 - pos2 - 1));
                    float value = std::stof(data.substr(pos3 + 1));
                    listener->onObjectCreated(x, y, static_cast<WorldObjectType>(type), value);
                }
            } else if (eventType == "object_destroyed") {
                size_t pos = data.find(',');
                if (pos != std::string::npos) {
                    float x = std::stof(data.substr(0, pos));
                    float y = std::stof(data.substr(pos + 1));
                    listener->onObjectDestroyed(x, y, WorldObjectType::Empty); // Type could be tracked
                }
            } else if (eventType == "object_interacted") {
                size_t pos1 = data.find(',');
                size_t pos2 = data.find(',', pos1 + 1);
                size_t pos3 = data.find(',', pos2 + 1);
                if (pos1 != std::string::npos && pos2 != std::string::npos && pos3 != std::string::npos) {
                    float x = std::stof(data.substr(0, pos1));
                    float y = std::stof(data.substr(pos1 + 1, pos2 - pos1 - 1));
                    int type = std::stoi(data.substr(pos2 + 1, pos3 - pos2 - 1));
                    listener->onCollision(x, y, static_cast<WorldObjectType>(type));
                }
            }
        }
    }
}

void SimpleWorld::removeEventListener(WorldEventListener* listener) {
    auto it = std::find(eventListeners_.begin(), eventListeners_.end(), listener);
    if (it != eventListeners_.end()) {
        eventListeners_.erase(it);
    }
}

void SimpleWorld::printDebugInfo() const {
    std::cout << "=== SimpleWorld Debug Information ===" << std::endl;
    std::cout << "World size: " << width_ << "x" << height_ << std::endl;
    std::cout << "Agent position: (" << agent_.x << ", " << agent_.y << ")" << std::endl;
    std::cout << "Agent energy: " << agent_.energy << " / " << maxEnergy_ << std::endl;
    std::cout << "Agent health: " << agent_.health << std::endl;
    std::cout << "Simulation time: " << simTime_ << std::endl;
    std::cout << "Object count: " << getObjectCount() << std::endl;
    std::cout << "Resource objects: " << getResourceCount() << std::endl;
    std::cout << "Hazard objects: " << getHazardCount() << std::endl;
    std::cout << "Wall objects: " << getWallCount() << std::endl;
    
    if (enablePerformanceMetrics_) {
        std::cout << "=== Performance Metrics ===" << std::endl;
        std::cout << "Vision processing: " << metrics_.getVisionTime() * 1000 << " ms" << std::endl;
        std::cout << "Physics update: " << metrics_.getPhysicsTime() * 1000 << " ms" << std::endl;
        std::cout << "Collision detection: " << metrics_.getCollisionTime() * 1000 << " ms" << std::endl;
        std::cout << "Sensory processing: " << metrics_.getSensoryTime() * 1000 << " ms" << std::endl;
        std::cout << "Interaction processing: " << metrics_.getInteractionTime() * 1000 << " ms" << std::endl;
        std::cout << "FPS: " << metrics_.getFPS() << std::endl;
    }
    
    std::cout << "=== End Debug Information ===" << std::endl;
}

std::string SimpleWorld::getDebugInfo() const {
    std::stringstream ss;
    ss << "=== SimpleWorld Debug Information ===\n";
    ss << "World size: " << width_ << "x" << height_ << "\n";
    ss << "Agent position: (" << agent_.x << ", " << agent_.y << ")\n";
    ss << "Agent energy: " << agent_.energy << " / " << maxEnergy_ << "\n";
    ss << "Agent health: " << agent_.health << "\n";
    ss << "Simulation time: " << simTime_ << "\n";
    ss << "Object count: " << getObjectCount() << "\n";
    ss << "Resource objects: " << getResourceCount() << "\n";
    ss << "Hazard objects: " << getHazardCount() << "\n";
    ss << "Wall objects: " << getWallCount() << "\n";
    
    if (enablePerformanceMetrics_) {
        ss << "=== Performance Metrics ===\n";
        ss << "Vision processing: " << metrics_.getVisionTime() * 1000 << " ms\n";
        ss << "Physics update: " << metrics_.getPhysicsTime() * 1000 << " ms\n";
        ss << "Collision detection: " << metrics_.getCollisionTime() * 1000 << " ms\n";
        ss << "Sensory processing: " << metrics_.getSensoryTime() * 1000 << " ms\n";
        ss << "Interaction processing: " << metrics_.getInteractionTime() * 1000 << " ms\n";
        ss << "FPS: " << metrics_.getFPS() << "\n";
    }
    
    ss << "=== End Debug Information ===\n";
    return ss.str();
}

void SimpleWorld::validateWorldState() const {
    // Basic validation of world state
    if (width_ <= 0.0f || height_ <= 0.0f) {
        std::cout << "[WARNING] Invalid world dimensions: " << width_ << "x" << height_ << std::endl;
    }
    
    if (agent_.energy < 0.0f || agent_.energy > maxEnergy_) {
        std::cout << "[WARNING] Invalid agent energy: " << agent_.energy << std::endl;
    }
    
    if (agent_.health < 0.0f || agent_.health > WorldConfig::defaultHealth) {
        std::cout << "[WARNING] Invalid agent health: " << agent_.health << std::endl;
    }
    
    if (simTime_ < 0.0 || simTime_ > WorldConfig::maxSimulationTime) {
        std::cout << "[WARNING] Invalid simulation time: " << simTime_ << std::endl;
    }
}

uint64_t SimpleWorld::lcg(uint64_t& state) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    return state;
}

float SimpleWorld::urand(uint64_t& state, float min, float max) {
    uint64_t r = lcg(state);
    float normalized = (r & 0xFFFFFFFFFFFFULL) / static_cast<float>(0xFFFFFFFFFFFFULL);
    return min + normalized * (max - min);
}

} // namespace nlm