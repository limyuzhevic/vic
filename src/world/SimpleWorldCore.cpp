// SimpleWorldCore.cpp - Core implementation
#include "SimpleWorldCore.hpp"
#include <algorithm>
#include <chrono>
#include <execution>
#include <cmath>

namespace nlm {

SimpleWorld::SimpleWorld(const WorldConfig& config) : config_(config) {
    // Validate configuration
    if (!config_.validate()) {
        throw WorldError("Initial configuration validation failed", 
                        ErrorCode::ValidationFailed);
    }
    
    // Initialize components
    initializeComponents();
    
    // Create object pool
    objectPool_ = std::make_unique<ObjectPool>(config_.objectPoolSize);
    
    // Create spatial index
    spatialIndex_ = std::make_unique<SpatialIndex>(config_.width, config_.height);
    
    // Create components
    visionGenerator_ = std::make_unique<SimpleVisionGenerator>();
    collisionDetector_ = std::make_unique<SimpleCollisionDetector>(config_.width, config_.height);
    interactionEngine_ = std::make_unique<SimpleInteractionEngine>();
    physicsSimulator_ = std::make_unique<SimplePhysicsSimulator>();
    
    // Set configurations
    visionGenerator_->setConfig(config_);
    interactionEngine_->setConfig(config_);
    physicsSimulator_->setWorldBounds(config_.width, config_.height);
    
    // Initialize RNG with seed
    rng_.seed(config_.randomSeed);
}

void SimpleWorld::configure(const WorldConfig& config) {
    std::unique_lock<std::shared_mutex> lock(worldMutex_);
    
    // Validate new configuration
    if (!config.validate()) {
        throw WorldError("Configuration validation failed", ErrorCode::ValidationFailed);
    }
    
    // Update configuration
    config_ = config;
    
    // Reinitialize components with new configuration
    initializeComponents();
    
    // Update component configurations
    visionGenerator_->setConfig(config_);
    interactionEngine_->setConfig(config_);
    physicsSimulator_->setWorldBounds(config_.width, config_.height);
    
    // Rebuild spatial index
    spatialIndex_->setCellSize(std::min(1.0f, std::max(0.1f, config_.width / 100.0f)));
}

void SimpleWorld::reset() {
    std::unique_lock<std::shared_mutex> lock(worldMutex_);
    
    // Reset simulation time
    simTime_ = 0.0;
    
    // Reset agent
    agent_.reset();
    agent_.x = rng_.nextFloat(2.0f, config_.width - 2.0f);
    agent_.y = rng_.nextFloat(2.0f, config_.height - 2.0f);
    agent_.energy = config_.maxEnergy;
    
    // Reset objects
    std::scoped_lock lock2(objectsMutex_);
    objects_.clear();
    collisionDetector_->clear();
    spatialIndex_->clear();
    
    // Place resource objects
    for (int i = 0; i < 5; ++i) {
        float ox = rng_.nextFloat(1.0f, config_.width - 1.0f);
        float oy = rng_.nextFloat(1.0f, config_.height - 1.0f);
        WorldObject* obj = objectPool_->acquire(ox, oy, WorldObjectType::Resource, 10.0f, 0.4f);
        if (obj) {
            objects_.push_back(*obj);
            collisionDetector_->addObject(objects_.back());
            spatialIndex_->insert(ox, oy, objects_.back().id, 0.4f);
            objectPool_->release(obj);
        }
    }
    
    // Place hazard objects
    for (int i = 0; i < 3; ++i) {
        float ox = rng_.nextFloat(1.0f, config_.width - 1.0f);
        float oy = rng_.nextFloat(1.0f, config_.height - 1.0f);
        WorldObject* obj = objectPool_->acquire(ox, oy, WorldObjectType::Hazard, -15.0f, 0.3f);
        if (obj) {
            objects_.push_back(*obj);
            collisionDetector_->addObject(objects_.back());
            spatialIndex_->insert(ox, oy, objects_.back().id, 0.3f);
            objectPool_->release(obj);
        }
    }
    
    // Generate vision
    generateVision();
}

void SimpleWorld::setAgentStart(float x, float y) {
    std::shared_lock<std::shared_mutex> lock(worldMutex_);
    agent_.x = x;
    agent_.y = y;
}

void SimpleWorld::update(double timestep) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::unique_lock<std::shared_mutex> lock(worldMutex_);
    
    // Record update start
    auto updateStart = std::chrono::high_resolution_clock::now();
    
    // Update simulation time
    simTime_ += timestep;
    
    // Simulate physics
    physicsSimulator_->simulate(agent_, timestep);
    
    // Generate vision
    generateVision();
    
    // Check collisions
    checkCollisions();
    
    // Update performance stats
    auto updateEnd = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(updateEnd - updateStart);
    
    std::scoped_lock lock2(statsMutex_);
    stats_.collisionDetectionTime += duration.count();
    
    lock.unlock(); // Release lock before expensive operations
    
    // Update performance stats
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::scoped_lock lock3(statsMutex_);
    stats_.visionGenerationTime += totalDuration.count();
    stats_.totalUpdates++;
}

ActionResult SimpleWorld::applyMotorCommand(MotorCommand cmd, double currentTime) {
    std::shared_lock<std::shared_mutex> lock(worldMutex_);
    
    ActionResult result;
    const float moveSpeed = MOVE_SPEED;
    const float turnSpeed = TURN_SPEED;
    
    // Only allow one action per step
    if (currentTime - agent_.lastActionTime < MIN_ACTION_INTERVAL) {
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
                result.reward = -0.01f;
                result.success = true;
            } else {
                result.reward = -0.05f;
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
    std::shared_lock<std::shared_mutex> lock(worldMutex_);
    
    ActionResult result;
    result.reward = 0.0f;
    result.success = false;
    
    // Check for objects in interaction range
    const float interactRange = config_.interactRange;
    
    // Use spatial index for efficient query
    std::vector<size_t> nearbyIds = spatialIndex_->query(agent_.x, agent_.y, interactRange + 1.0f);
    
    // Find objects at these IDs
    std::vector<WorldObject*> nearbyObjects;
    for (size_t id : nearbyIds) {
        for (auto& obj : objects_) {
            if (obj.id == id && obj.active) {
                nearbyObjects.push_back(&obj);
                break;
            }
        }
    }
    
    // Use interaction engine
    std::vector<WorldObject*> collisionList;
    collisionDetector_->checkCollisions(agent_, *spatialIndex_, collisionList);
    
    for (WorldObject* obj : collisionList) {
        if (!obj || !obj->active) continue;
        
        float dx = obj->x - agent_.x;
        float dy = obj->y - agent_.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < interactRange + obj->radius) {
            obj->active = false;
            collisionDetector_->removeObject(obj->id);
            spatialIndex_->remove(obj->id);
            
            if (obj->type == WorldObjectType::Resource) {
                float newEnergy = agent_.energy + config_.resourceEnergyGain;
                agent_.energy = std::min(config_.maxEnergy, newEnergy);
                result.reward = obj->value;
                result.success = true;
                result.message = "consumed_resource";
            } else if (obj->type == WorldObjectType::Hazard) {
                agent_.health -= 0.2f;
                result.reward = obj->value;
                result.success = true;
                result.message = "hit_hazard";
            }
            
            // Update performance stats
            std::scoped_lock lock2(statsMutex_);
            stats_.interactionTime++;
            
            break;
        }
    }
    
    if (!result.success) {
        result.message = "no_object";
    }
    
    return result;
}

void SimpleWorld::addObject(const WorldObject& obj) {
    std::shared_lock<std::shared_mutex> lock(worldMutex_);
    
    // Generate unique ID
    static std::atomic<size_t> nextId{1000};
    WorldObject newObj = obj;
    newObj.id = nextId++;
    
    std::scoped_lock lock2(objectsMutex_);
    objects_.push_back(newObj);
    
    collisionDetector_->addObject(objects_.back());
    spatialIndex_->insert(obj.x, obj.y, objects_.back().id, obj.radius);
}

void SimpleWorld::removeObject(float x, float y) {
    std::shared_lock<std::shared_mutex> lock(worldMutex_);
    
    // Find object at position
    std::scoped_lock lock2(objectsMutex_);
    for (auto it = objects_.begin(); it != objects_.end(); ++it) {
        float dx = it->x - x;
        float dy = it->y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < it->radius) {
            // Remove from world and components
            collisionDetector_->removeObject(it->id);
            spatialIndex_->remove(it->id);
            objects_.erase(it);
            break;
        }
    }
}

bool SimpleWorld::isValidPosition(float x, float y) const {
    // Check world boundaries
    if (x < 0.0f || x >= config_.width || 
        y < 0.0f || y >= config_.height) {
        return false;
    }
    return true;
}

WorldObject* SimpleWorld::getObjectAt(float x, float y) {
    std::shared_lock<std::shared_mutex> lock(worldMutex_);
    
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

void SimpleWorld::setRandomSeed(uint64_t seed) {
    std::unique_lock<std::shared_mutex> lock(worldMutex_);
    rng_.seed(seed);
    config_.randomSeed = seed;
}

void SimpleWorld::initializeComponents() {
    // Create fresh components
    objectPool_ = std::make_unique<ObjectPool>(config_.objectPoolSize);
    spatialIndex_ = std::make_unique<SpatialIndex>(config_.width, config_.height);
    
    // Set cell size based on world size
    float cellSize = std::min(1.0f, std::max(0.1f, config_.width / 100.0f));
    spatialIndex_->setCellSize(cellSize);
    
    visionGenerator_ = std::make_unique<SimpleVisionGenerator>();
    collisionDetector_ = std::make_unique<SimpleCollisionDetector>(config_.width, config_.height);
    interactionEngine_ = std::make_unique<SimpleInteractionEngine>();
    physicsSimulator_ = std::make_unique<SimplePhysicsSimulator>();
    
    // Configure components
    visionGenerator_->setConfig(config_);
    interactionEngine_->setConfig(config_);
    physicsSimulator_->setWorldBounds(config_.width, config_.height);
}

void SimpleWorld::generateVision() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::shared_lock<std::shared_mutex> lock(worldMutex_);
    
    // Prepare vision data
    std::array<float, 256> visionData{};
    
    // Generate vision using the vision generator
    visionGenerator_->generateVision(agent_, *spatialIndex_, visionData);
    
    // Convert to SensoryPercept format
    std::vector<float> visionVec(visionData.begin(), visionData.end());
    
    // Update sensory percept
    SensoryPercept oldSensory = sensory_;
    sensory_.setVision(visionVec);
    sensory_.setTimestamp(simTime_);
    
    // Set other sensory inputs
    std::vector<float> touch(8, 0.0f);
    std::vector<float> internal(4, 0.0f);
    std::vector<float> proprioception(6, 0.0f);
    
    internal[0] = agent_.energy / config_.maxEnergy;
    internal[1] = agent_.health;
    internal[2] = agent_.isMoving ? 1.0f : 0.0f;
    internal[3] = agent_.isTurning ? 1.0f : 0.0f;
    
    proprioception[0] = agent_.x / config_.width;
    proprioception[1] = agent_.y / config_.height;
    proprioception[2] = (agent_.orientation + M_PI) / (2.0f * M_PI);
    proprioception[3] = std::abs(agent_.velocityX) / 5.0f;
    proprioception[4] = std::abs(agent_.velocityY) / 5.0f;
    proprioception[5] = std::abs(agent_.angularVelocity) / 3.0f;
    
    sensory_.setTouch(touch);
    sensory_.setInternal(internal);
    sensory_.setProprioception(proprioception);
    sensory_.setAudio({});
    
    lock.unlock(); // Release lock before updating performance stats
    
    // Update performance stats
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::scoped_lock lock2(statsMutex_);
    stats_.visionGenerationTime += duration.count();
}

void SimpleWorld::checkCollisions() {
    std::shared_lock<std::shared_mutex> lock(worldMutex_);
    
    // Use collision detector for passive collision detection
    std::vector<WorldObject*> collisions;
    collisionDetector_->checkCollisions(agent_, *spatialIndex_, collisions);
    
    for (WorldObject* obj : collisions) {
        if (obj && obj->type == WorldObjectType::Hazard) {
            agent_.health -= 0.01f;
        }
    }
}

} // namespace nlm