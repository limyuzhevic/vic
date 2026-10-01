// SimpleWorldDesign.cpp - Implementations for design classes
#include "SimpleWorldDesign.hpp"
#include <random>
#include <stdexcept>
#include <cmath>

namespace nlm {

// RandomGenerator implementations
RandomGenerator::RandomGenerator() : currentSeed_(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {
    generator_.seed(currentSeed_);
}

RandomGenerator::RandomGenerator(uint64_t seed) : currentSeed_(seed) {
    generator_.seed(currentSeed_);
}

void RandomGenerator::seed(uint64_t seed) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentSeed_ = seed;
    generator_.seed(seed);
}

uint64_t RandomGenerator::next() {
    std::lock_guard<std::mutex> lock(mutex_);
    return generator_() ^ (generator_() << 32);
}

float RandomGenerator::nextFloat(float min, float max) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::uniform_real_distribution<float> dist(min, max);
    return dist(generator_);
}

// WorldConfig validation
bool WorldConfig::validate() const {
    if (validationLevel == ValidationLevel::None) return true;
    
    bool valid = true;
    
    if (validationLevel == ValidationLevel::Strict) {
        if (width < MIN_WIDTH || width > MAX_WIDTH) {
            throw WorldError("World width " + std::to_string(width) + 
                           " is out of range [" + std::to_string(MIN_WIDTH) + ", " +
                           std::to_string(MAX_WIDTH) + "]", ErrorCode::InvalidParameter);
        }
        if (height < MIN_HEIGHT || height > MAX_HEIGHT) {
            throw WorldError("World height " + std::to_string(height) + 
                           " is out of range [" + std::to_string(MIN_HEIGHT) + ", " +
                           std::to_string(MAX_HEIGHT) + "]", ErrorCode::InvalidParameter);
        }
        if (visionWidth < MIN_VISION_DIM || visionWidth > MAX_VISION_DIM) {
            throw WorldError("Vision width " + std::to_string(visionWidth) + 
                           " is out of range [" + std::to_string(MIN_VISION_DIM) + ", " +
                           std::to_string(MAX_VISION_DIM) + "]", ErrorCode::InvalidParameter);
        }
        if (visionHeight < MIN_VISION_DIM || visionHeight > MAX_VISION_DIM) {
            throw WorldError("Vision height " + std::to_string(visionHeight) + 
                           " is out of range [" + std::to_string(MIN_VISION_DIM) + ", " +
                           std::to_string(MAX_VISION_DIM) + "]", ErrorCode::InvalidParameter);
        }
        if (maxEnergy < MIN_ENERGY || maxEnergy > MAX_ENERGY) {
            throw WorldError("Max energy " + std::to_string(maxEnergy) + 
                           " is out of range [" + std::to_string(MIN_ENERGY) + ", " +
                           std::to_string(MAX_ENERGY) + "]", ErrorCode::InvalidParameter);
        }
        if (visionMaxRange < MIN_RANGE || visionMaxRange > MAX_RANGE) {
            throw WorldError("Vision max range " + std::to_string(visionMaxRange) + 
                           " is out of range [" + std::to_string(MIN_RANGE) + ", " +
                           std::to_string(MAX_RANGE) + "]", ErrorCode::InvalidParameter);
        }
        if (interactRange < MIN_RANGE || interactRange > MAX_RANGE) {
            throw WorldError("Interact range " + std::to_string(interactRange) + 
                           " is out of range [" + std::to_string(MIN_RANGE) + ", " +
                           std::to_string(MAX_RANGE) + "]", ErrorCode::InvalidParameter);
        }
        if (maxObjects == 0 || maxObjects > 1000000) {
            throw WorldError("Max objects " + std::to_string(maxObjects) + 
                           " is out of valid range", ErrorCode::InvalidParameter);
        }
        if (objectPoolSize == 0) {
            throw WorldError("Object pool size must be positive", ErrorCode::InvalidParameter);
        }
    } else {
        // Lenient validation - just warn for severe cases
        if (width <= 0.0f || height <= 0.0f) valid = false;
        if (visionWidth == 0 || visionHeight == 0) valid = false;
        if (maxEnergy <= 0.0f) valid = false;
    }
    
    return valid;
}

// SpatialIndex implementations
SpatialIndex::SpatialIndex(float width, float height, float cellSize)
    : width_(width), height_(height), cellSize_(cellSize) {}

void SpatialIndex::insert(float x, float y, size_t id, float radius) {
    std::lock_guard<std::mutex> lock(mutex_);
    objects_[id] = {x, y};
    objectRadii_[id] = radius;
    
    // Calculate grid cell
    size_t cellX = static_cast<size_t>(x / cellSize_);
    size_t cellY = static_cast<size_t>(y / cellSize_);
    
    // Add to grid
    grid_[cellX * 1000 + cellY].objectIds.push_back(id);
}

void SpatialIndex::remove(size_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = objects_.find(id);
    if (it != objects_.end()) {
        // Remove from grid
        size_t cellX = static_cast<size_t>(it->second.first / cellSize_);
        size_t cellY = static_cast<size_t>(it->second.second / cellSize_);
        auto& cell = grid_[cellX * 1000 + cellY];
        cell.objectIds.erase(
            std::remove(cell.objectIds.begin(), cell.objectIds.end(), id),
            cell.objectIds.end()
        );
        
        objects_.erase(it);
        objectRadii_.erase(id);
    }
}

void SpatialIndex::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    objects_.clear();
    objectRadii_.clear();
    grid_.clear();
}

std::vector<size_t> SpatialIndex::query(float x, float y, float radius) const {
    return query(x, y, radius, [](size_t) { return true; });
}

std::vector<size_t> SpatialIndex::query(float x, float y, float radius,
                                        const std::function<bool(size_t)>& filter) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<size_t> result;
    
    // Determine cell bounds
    size_t centerCellX = static_cast<size_t>(x / cellSize_);
    size_t centerCellY = static_cast<size_t>(y / cellSize_);
    size_t radiusCells = static_cast<size_t>(radius / cellSize_) + 1;
    
    // Check neighboring cells
    for (size_t dx = 0; dx <= radiusCells * 2; ++dx) {
        for (size_t dy = 0; dy <= radiusCells * 2; ++dy) {
            size_t cellX1 = centerCellX + dx - radiusCells;
            size_t cellY1 = centerCellY + dy - radiusCells;
            size_t cellX2 = centerCellX + dx + radiusCells;
            size_t cellY2 = centerCellY + dy + radiusCells;
            
            for (size_t cx = cellX1; cx <= cellX2; ++cx) {
                for (size_t cy = cellY1; cy <= cellY2; ++cy) {
                    auto gridIt = grid_.find(cx * 1000 + cy);
                    if (gridIt != grid_.end()) {
                        for (size_t objId : gridIt->second.objectIds) {
                            auto objIt = objects_.find(objId);
                            if (objIt != objects_.end() && filter(objId)) {
                                float objX = objIt->second.first;
                                float objY = objIt->second.second;
                                float distSquared = (x - objX) * (x - objX) + 
                                                  (y - objY) * (y - objY);
                                float radiusSquared = radius * radius;
                                
                                if (distSquared <= radiusSquared) {
                                    result.push_back(objId);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

void SpatialIndex::setCellSize(float cellSize) {
    std::lock_guard<std::mutex> lock(mutex_);
    cellSize_ = cellSize;
    // Could trigger rebuild, but for simplicity we just rebuild
    rebuild();
}

void SpatialIndex::rebuild() {
    std::lock_guard<std::mutex> lock(mutex_);
    // Clear and reinsert all objects
    std::unordered_map<size_t, std::pair<float, float>> objectsCopy = objects_;
    std::unordered_map<size_t, float> radiiCopy = objectRadii_;
    
    grid_.clear();
    objects_.clear();
    objectRadii_.clear();
    
    for (const auto& pair : objectsCopy) {
        insert(pair.first, pair.second.first, pair.second.second, radiiCopy[pair.first]);
    }
}

// ObjectPool implementations
ObjectPool::ObjectPool(size_t capacity) : capacity_(capacity) {
    head_ = std::make_unique<Node>(0.0f, 0.0f, WorldObjectType::Empty, 0.0f, 0.0f);
}

WorldObject* ObjectPool::acquire(float x, float y, WorldObjectType type, 
                                 float value, float radius) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (freeList_.empty()) {
        if (pool_.size() >= capacity_) {
            throw WorldError("Object pool is full", ErrorCode::MemoryAllocationFailed);
        }
        freeList_.push_back(&pool_.emplace_back(x, y, type, value, radius));
    }
    
    WorldObject* obj = freeList_.back();
    freeList_.pop_back();
    obj->x = x;
    obj->y = y;
    obj->type = type;
    obj->value = value;
    obj->radius = radius;
    obj->active = true;
    return obj;
}

void ObjectPool::release(WorldObject* obj) {
    std::lock_guard<std::mutex> lock(mutex_);
    obj->active = false;
    freeList_.push_back(obj);
}

void ObjectPool::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    freeList_.clear();
    for (auto& obj : pool_) {
        obj.active = false;
    }
}

// SimpleVisionGenerator implementations
void SimpleVisionGenerator::generateVision(const AgentBody& agent, 
                                           const SpatialIndex& spatialIndex,
                                           FixedArray<float, 256>& vision) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    const size_t rayCount = 16;
    const float fov = M_PI / 2.0f;
    const float maxRange = config_.visionMaxRange;
    
    for (size_t i = 0; i < rayCount; ++i) {
        float angle = agent.orientation - fov / 2.0f + (fov * i / (rayCount - 1));
        float dx = std::cos(angle);
        float dy = std::sin(angle);
        
        float t = 0.0f;
        while (t < maxRange) {
            float px = agent.x + dx * t;
            float py = agent.y + dy * t;
            
            // Check world boundaries
            if (px < 0.0f || px >= config_.width || 
                py < 0.0f || py >= config_.height) {
                vision[i] = t / maxRange;
                break;
            }
            
            // Find objects at this position using spatial index
            bool found = false;
            for (const auto& obj : objects_) {
                if (!obj.active) continue;
                
                float dx = px - obj.x;
                float dy = py - obj.y;
                float dist = std::sqrt(dx * dx + dy * dy);
                
                if (dist < obj.radius) {
                    found = true;
                    if (obj.type == WorldObjectType::Resource) {
                        vision[i] = (maxRange - t) / maxRange * 0.8f;
                    } else if (obj.type == WorldObjectType::Hazard) {
                        vision[i] = (maxRange - t) / maxRange * 0.3f;
                    } else {
                        vision[i] = 0.5f;
                    }
                    break;
                }
            }
            
            if (found) break;
            
            t += config_.visionStepSize;
        }
    }
}

void SimpleVisionGenerator::setConfig(const WorldConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
}

void SimpleVisionGenerator::preprocess(const std::vector<WorldObject>& objects) {
    std::lock_guard<std::mutex> lock(mutex_);
    objects_ = objects;
    cacheDirty_ = true;
}

// SimpleCollisionDetector implementations
SimpleCollisionDetector::SimpleCollisionDetector(float width, float height) 
    : spatialIndex_(width, height) {}

void SimpleCollisionDetector::checkCollisions(const AgentBody& agent, 
                                             const SpatialIndex& spatialIndex,
                                             const std::vector<WorldObject*>& collisions) {
    std::lock_guard<std::mutex> lock(mutex_);
    clearError();
    
    // Convert world coordinates to spatial index space
    float objX = agent.x;
    float objY = agent.y;
    float agentRadius = 0.5f;  // Agent radius
    
    try {
        // Query nearby objects
        std::vector<size_t> nearbyIds = spatialIndex.query(objX, objY, agentRadius + 1.0f);
        
        for (size_t id : nearbyIds) {
            auto it = objects_.find(id);
            if (it != objects_.end()) {
                float objX = it->second.first;
                float objY = it->second.second;
                float objRadius = objectRadii_[id];
                
                float dx = objX - agent.x;
                float dy = objY - agent.y;
                float dist = std::sqrt(dx * dx + dy * dy);
                
                if (dist < agentRadius + objRadius) {
                    collisions.push_back(&objects_.at(id));
                }
            }
        }
    } catch (const std::exception& e) {
        lastError_ = ErrorCode::SpatialIndexError;
        // Graceful degradation - return empty collisions
    }
}

void SimpleCollisionDetector::addObject(const WorldObject& obj) {
    std::lock_guard<std::mutex> lock(mutex_);
    objects_[obj.id] = {obj.x, obj.y};
    objectRadii_[obj.id] = obj.radius;
    spatialIndex_.insert(obj.x, obj.y, obj.id, obj.radius);
}

void SimpleCollisionDetector::removeObject(size_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    objects_.erase(id);
    objectRadii_.erase(id);
    spatialIndex_.remove(id);
}

void SimpleCollisionDetector::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    objects_.clear();
    objectRadii_.clear();
    spatialIndex_.clear();
}

// SimpleInteractionEngine implementations
void SimpleInteractionEngine::setConfig(const WorldConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
}

ActionResult SimpleInteractionEngine::interact(const AgentBody& agent,
                                              const SpatialIndex& spatialIndex,
                                              const std::vector<WorldObject*>& nearbyObjects) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ActionResult result;
    result.reward = 0.0f;
    result.success = false;
    
    // Check for objects in interaction range
    const float interactRange = config_.interactRange;
    
    for (WorldObject* obj : nearbyObjects) {
        if (!obj || !obj->active) continue;
        
        float dx = obj->x - agent.x;
        float dy = obj->y - agent.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < interactRange + obj->radius) {
            obj->active = false;
            
            if (obj->type == WorldObjectType::Resource) {
                float newEnergy = agent.energy + config_.resourceEnergyGain;
                agent.energy = std::min(config_.maxEnergy, newEnergy);
                result.reward = obj->value;
                result.success = true;
                result.message = "consumed_resource";
            } else if (obj->type == WorldObjectType::Hazard) {
                agent.health -= 0.2f;
                result.reward = obj->value;
                result.success = true;
                result.message = "hit_hazard";
            }
            
            break;
        }
    }
    
    if (!result.success) {
        result.message = "no_object";
    }
    
    return result;
}

// SimplePhysicsSimulator implementations
void SimplePhysicsSimulator::simulate(AgentBody& agent, double timestep) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Apply velocity to position
    agent.x += agent.velocityX * timestep;
    agent.y += agent.velocityY * timestep;
    
    // Apply angular velocity to orientation
    agent.orientation += agent.angularVelocity * timestep;
    
    // Normalize orientation to [-PI, PI]
    while (agent.orientation > M_PI) agent.orientation -= 2.0f * M_PI;
    while (agent.orientation < -M_PI) agent.orientation += 2.0f * M_PI;
    
    // Boundary collision
    if (agent.x < 0.0f) {
        agent.x = 0.0f;
        agent.velocityX = 0.0f;
    }
    if (agent.x >= width_) {
        agent.x = width_ - 0.01f;
        agent.velocityX = 0.0f;
    }
    if (agent.y < 0.0f) {
        agent.y = 0.0f;
        agent.velocityY = 0.0f;
    }
    if (agent.y >= height_) {
        agent.y = height_ - 0.01f;
        agent.velocityY = 0.0f;
    }
    
    // Apply gravity if enabled
    if (physicsParams_.gravity > 0.0f) {
        agent.velocityY -= physicsParams_.gravity * timestep;
    }
    
    // Apply drag
    agent.velocityX *= std::pow(1.0f - physicsParams_.dragCoefficient, timestep);
    agent.velocityY *= std::pow(1.0f - physicsParams_.dragCoefficient, timestep);
    agent.angularVelocity *= std::pow(1.0f - physicsParams_.angularDragCoefficient, timestep);
    
    // Normalize velocities
    float maxVel = 10.0f;
    float vel = std::sqrt(agent.velocityX * agent.velocityX + agent.velocityY * agent.velocityY);
    if (vel > maxVel) {
        agent.velocityX = agent.velocityX * maxVel / vel;
        agent.velocityY = agent.velocityY * maxVel / vel;
    }
}

void SimplePhysicsSimulator::setWorldBounds(float width, float height) {
    std::lock_guard<std::mutex> lock(mutex_);
    width_ = width;
    height_ = height;
}

} // namespace nlm