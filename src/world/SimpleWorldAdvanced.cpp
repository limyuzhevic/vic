// SimpleWorldAdvanced.cpp - Advanced feature implementations
#include "SimpleWorldAdvanced.hpp"
#include <algorithm>
#include <chrono>
#include <execution>
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

// AdvancedVisionGenerator implementations
AdvancedVisionGenerator::AdvancedVisionGenerator(size_t threadCount)
    : threadCount_(threadCount) {}

void AdvancedVisionGenerator::generateVision(const AgentBody& agent, 
                                            const SpatialIndex& spatialIndex,
                                            FixedArray<float, 256>& vision) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    const size_t rayCount = 16;
    const float fov = M_PI / 2.0f;
    const float maxRange = visionOptions_.rayMaxRange;
    
    if (visionOptions_.multithreaded && threadCount_ > 1) {
        // Multithreaded vision generation
        std::vector<std::future<void>> futures;
        futures.reserve(rayCount);
        
        for (size_t i = 0; i < rayCount; ++i) {
            futures.push_back(std::async(std::launch::async, [this, &agent, &spatialIndex, &vision, i, rayCount, fov, maxRange]() {
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
                    
                    // Use spatial index for efficient object detection
                    std::vector<size_t> nearbyIds = spatialIndex.query(px, py, visionOptions_.rayStepSize);
                    bool found = false;
                    
                    for (size_t id : nearbyIds) {
                        // Look up object (implementation depends on access method)
                        // For now, we'll use a simplified approach
                        float objX = 0.0f, objY = 0.0f;
                        float objRadius = 0.0f;
                        float dist = std::sqrt((px - objX) * (px - objX) + (py - objY) * (py - objY));
                        
                        if (dist < objRadius) {
                            found = true;
                            // Set vision value based on object type
                            // This would need actual object type information
                            vision[i] = (maxRange - t) / maxRange * 0.5f;
                            break;
                        }
                    }
                    
                    if (found) break;
                    t += visionOptions_.rayStepSize;
                }
            }));
        }
        
        // Wait for all futures to complete
        for (auto& future : futures) {
            future.wait();
        }
    } else {
        // Single-threaded vision generation
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
                
                // Use spatial index for efficient object detection
                std::vector<size_t> nearbyIds = spatialIndex.query(px, py, visionOptions_.rayStepSize);
                bool found = false;
                
                for (size_t id : nearbyIds) {
                    // Look up object (implementation depends on access method)
                    // For now, we'll use a simplified approach
                    float objX = 0.0f, objY = 0.0f;
                    float objRadius = 0.0f;
                    float dist = std::sqrt((px - objX) * (px - objX) + (py - objY) * (py - objY));
                    
                    if (dist < objRadius) {
                        found = true;
                        // Set vision value based on object type
                        // This would need actual object type information
                        vision[i] = (maxRange - t) / maxRange * 0.5f;
                        break;
                    }
                }
                
                if (found) break;
                t += visionOptions_.rayStepSize;
            }
        }
    }
}

void AdvancedVisionGenerator::setConfig(const WorldConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
}

void AdvancedVisionGenerator::preprocess(const std::vector<WorldObject>& objects) {
    std::lock_guard<std::mutex> lock(mutex_);
    objects_ = objects;
    cacheDirty_ = true;
}

// AdvancedCollisionDetector implementations
AdvancedCollisionDetector::AdvancedCollisionDetector(float width, float height) 
    : spatialIndex_(width, height) {}

void AdvancedCollisionDetector::checkCollisions(const AgentBody& agent, 
                                               const SpatialIndex& spatialIndex,
                                               const std::vector<WorldObject*>& collisions) {
    std::lock_guard<std::mutex> lock(mutex_);
    clearError();
    
    float agentRadius = 0.5f;
    
    try {
        // Query nearby objects using spatial index
        std::vector<size_t> nearbyIds = spatialIndex.query(agent.x, agent.y, agentRadius + 1.0f);
        
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
                    // We need to return actual object pointers, not just IDs
                    // This implementation is simplified - in practice we'd have access to objects_
                    // For now, we'll just indicate collision occurred
                    collisions.push_back(nullptr); // Placeholder
                }
            }
        }
    } catch (const std::exception& e) {
        lastError_ = ErrorCode::SpatialIndexError;
        // Graceful degradation
    }
}

void AdvancedCollisionDetector::addObject(const WorldObject& obj) {
    std::lock_guard<std::mutex> lock(mutex_);
    objects_[obj.id] = {obj.x, obj.y};
    objectRadii_[obj.id] = obj.radius;
    spatialIndex_.insert(obj.x, obj.y, obj.id, obj.radius);
}

void AdvancedCollisionDetector::removeObject(size_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    objects_.erase(id);
    objectRadii_.erase(id);
    spatialIndex_.remove(id);
}

void AdvancedCollisionDetector::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    objects_.clear();
    objectRadii_.clear();
    spatialIndex_.clear();
}

// AdvancedInteractionEngine implementations
AdvancedInteractionEngine::AdvancedInteractionEngine() {
    // Register default plugins
    registerPlugin<ResourceObjectFactory>();
    registerPlugin<HazardObjectFactory>();
}

void AdvancedInteractionEngine::setConfig(const WorldConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
}

ActionResult AdvancedInteractionEngine::interact(const AgentBody& agent,
                                                const SpatialIndex& spatialIndex,
                                                const std::vector<WorldObject*>& nearbyObjects) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ActionResult result;
    result.reward = 0.0f;
    result.success = false;
    
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

// AdvancedPhysicsSimulator implementations
AdvancedPhysicsSimulator::AdvancedPhysicsSimulator() {
    // Set reasonable default physics parameters
    physicsParams_.gravity = 9.81f;
    physicsParams_.dragCoefficient = 0.5f;
    physicsParams_.angularDragCoefficient = 0.5f;
    physicsParams_.restitution = 0.6f;
    physicsParams_.mass = 1.0f;
}

void AdvancedPhysicsSimulator::simulate(AgentBody& agent, double timestep) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Apply velocity to position
    agent.x += agent.velocityX * timestep;
    agent.y += agent.velocityY * timestep;
    
    // Apply angular velocity to orientation
    agent.orientation += agent.angularVelocity * timestep;
    
    // Normalize orientation to [-PI, PI]
    while (agent.orientation > M_PI) agent.orientation -= 2.0f * M_PI;
    while (agent.orientation < -M_PI) agent.orientation += 2.0f * M_PI;
    
    // Apply physics forces
    // Gravity
    agent.velocityY -= physicsParams_.gravity * timestep;
    
    // Air resistance (drag)
    agent.velocityX *= std::pow(1.0f - physicsParams_.dragCoefficient, timestep);
    agent.velocityY *= std::pow(1.0f - physicsParams_.dragCoefficient, timestep);
    agent.angularVelocity *= std::pow(1.0f - physicsParams_.angularDragCoefficient, timestep);
    
    // Apply boundary conditions
    switch (boundaryCondition_) {
        case BoundaryCondition::Reflective:
            if (agent.x < 0.0f) { agent.x = 0.0f; agent.velocityX = -agent.velocityX * physicsParams_.restitution; }
            if (agent.x >= width_) { agent.x = width_ - 0.01f; agent.velocityX = -agent.velocityX * physicsParams_.restitution; }
            if (agent.y < 0.0f) { agent.y = 0.0f; agent.velocityY = -agent.velocityY * physicsParams_.restitution; }
            if (agent.y >= height_) { agent.y = height_ - 0.01f; agent.velocityY = -agent.velocityY * physicsParams_.restitution; }
            break;
            
        case BoundaryCondition::Absorbing:
            if (agent.x < 0.0f) { agent.x = 0.0f; agent.velocityX = 0.0f; }
            if (agent.x >= width_) { agent.x = width_ - 0.01f; agent.velocityX = 0.0f; }
            if (agent.y < 0.0f) { agent.y = 0.0f; agent.velocityY = 0.0f; }
            if (agent.y >= height_) { agent.y = height_ - 0.01f; agent.velocityY = 0.0f; }
            break;
            
        case BoundaryCondition::Periodic:
            if (agent.x < 0.0f) agent.x += width_;
            if (agent.x >= width_) agent.x -= width_;
            if (agent.y < 0.0f) agent.y += height_;
            if (agent.y >= height_) agent.y -= height_;
            break;
            
        case BoundaryCondition::Truncated:
            if (agent.x < 0.0f) agent.x = 0.0f;
            if (agent.x >= width_) agent.x = width_ - 0.01f;
            if (agent.y < 0.0f) agent.y = 0.0f;
            if (agent.y >= height_) agent.y = height_ - 0.01f;
            break;
    }
    
    // Normalize velocities to prevent unbounded growth
    float maxVel = 100.0f;
    float vel = std::sqrt(agent.velocityX * agent.velocityX + agent.velocityY * agent.velocityY);
    if (vel > maxVel) {
        agent.velocityX = agent.velocityX * maxVel / vel;
        agent.velocityY = agent.velocityY * maxVel / vel;
    }
}

void AdvancedPhysicsSimulator::setWorldBounds(float width, float height) {
    std::lock_guard<std::mutex> lock(mutex_);
    width_ = width;
    height_ = height;
}

// AdaptiveSpatialIndex implementations
AdaptiveSpatialIndex::AdaptiveSpatialIndex(float width, float height, float initialCellSize)
    : width_(width), height_(height), currentCellSize_(initialCellSize) {}

void AdaptiveSpatialIndex::insert(float x, float y, size_t id, float radius) {
    std::lock_guard<std::mutex> lock(mutex_);
    objects_[id] = {x, y};
    objectRadii_[id] = radius;
    
    size_t cellX = static_cast<size_t>(x / currentCellSize_);
    size_t cellY = static_cast<size_t>(y / currentCellSize_);
    
    grid_[cellX * 1000 + cellY].objectIds.push_back(id);
}

void AdaptiveSpatialIndex::remove(size_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = objects_.find(id);
    if (it != objects_.end()) {
        size_t cellX = static_cast<size_t>(it->second.first / currentCellSize_);
        size_t cellY = static_cast<size_t>(it->second.second / currentCellSize_);
        auto& cell = grid_[cellX * 1000 + cellY];
        cell.objectIds.erase(
            std::remove(cell.objectIds.begin(), cell.objectIds.end(), id),
            cell.objectIds.end()
        );
        
        objects_.erase(it);
        objectRadii_.erase(id);
    }
}

void AdaptiveSpatialIndex::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    objects_.clear();
    objectRadii_.clear();
    grid_.clear();
}

std::vector<size_t> AdaptiveSpatialIndex::query(float x, float y, float radius) const {
    return query(x, y, radius, [](size_t) { return true; });
}

std::vector<size_t> AdaptiveSpatialIndex::query(float x, float y, float radius,
                                                const std::function<bool(size_t)>& filter) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<size_t> result;
    
    size_t centerCellX = static_cast<size_t>(x / currentCellSize_);
    size_t centerCellY = static_cast<size_t>(y / currentCellSize_);
    size_t radiusCells = static_cast<size_t>(radius / currentCellSize_) + 1;
    
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
                            if (filter(objId)) {
                                auto objIt = objects_.find(objId);
                                if (objIt != objects_.end()) {
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
    }
    
    return result;
}

void AdaptiveSpatialIndex::setCellSize(float cellSize) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentCellSize_ = cellSize;
    rebuild();
}

void AdaptiveSpatialIndex::rebuild() {
    std::lock_guard<std::mutex> lock(mutex_);
    // Similar to SpatialIndex rebuild but with current implementation
    std::unordered_map<size_t, std::pair<float, float>> objectsCopy = objects_;
    std::unordered_map<size_t, float> radiiCopy = objectRadii_;
    
    grid_.clear();
    objects_.clear();
    objectRadii_.clear();
    
    for (const auto& pair : objectsCopy) {
        insert(pair.first, pair.second.first, pair.second.second, radiiCopy[pair.first]);
    }
}

void AdaptiveSpatialIndex::adaptGridSize() {
    // Calculate object density
    float totalArea = width_ * height_;
    float objectArea = static_cast<float>(objects_.size()) * (0.5f * 0.5f); // Assuming 0.5x0.5 radius objects
    float density = objectArea / totalArea;
    
    // Adapt cell size based on density
    float optimalSize = getOptimalCellSize(density);
    if (std::abs(optimalSize - currentCellSize_) > 0.1f) {
        setCellSize(optimalSize);
    }
}

float AdaptiveSpatialIndex::getOptimalCellSize(float objectDensity) const {
    if (objectDensity < 0.01f) {
        return std::min(10.0f, width_ / 10.0f); // Sparse: larger cells
    } else if (objectDensity > 0.1f) {
        return std::max(0.1f, width_ / 200.0f); // Dense: smaller cells
    } else {
        return std::min(1.0f, width_ / 50.0f); // Moderate: medium cells
    }
}

// MemoryPool implementations
MemoryPool::MemoryPool(size_t chunkSize, size_t maxChunks)
    : chunkSize_(chunkSize), maxChunks_(maxChunks), allocatedSize_(0), usedChunks_(0) {
    head_ = std::make_unique<Chunk>(chunkSize);
}

MemoryPool::~MemoryPool() {
    Chunk* current = head_.get();
    while (current) {
        Chunk* next = current->next.get();
        delete current;
        current = next;
    }
}

void* MemoryPool::allocate(size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (size > chunkSize_ || usedChunks_ >= maxChunks_) {
        return nullptr; // Cannot satisfy allocation
    }
    
    // Find suitable chunk
    Chunk* current = head_.get();
    while (current) {
        if (current->memory && allocatedSize_ + size <= chunkSize_) {
            void* ptr = static_cast<char*>(current->memory) + allocatedSize_;
            allocatedSize_ += size;
            return ptr;
        }
        current = current->next.get();
    }
    
    return nullptr;
}

void MemoryPool::deallocate(void* ptr, size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    // Implementation depends on tracking allocated pointers
    // For simplicity, we'll just reduce allocated size
    allocatedSize_ -= size;
}

// ValidationUtils implementations
bool ValidationUtils::validateConfig(const WorldConfig& config, ValidationLevel level) {
    if (level == ValidationLevel::None) return true;
    
    bool valid = true;
    
    if (level == ValidationLevel::Strict) {
        VALIDATE_RANGE(config.width, WorldConfig::MIN_WIDTH, WorldConfig::MAX_WIDTH, level);
        VALIDATE_RANGE(config.height, WorldConfig::MIN_HEIGHT, WorldConfig::MAX_HEIGHT, level);
        VALIDATE_RANGE(config.visionWidth, WorldConfig::MIN_VISION_DIM, WorldConfig::MAX_VISION_DIM, level);
        VALIDATE_RANGE(config.visionHeight, WorldConfig::MIN_VISION_DIM, WorldConfig::MAX_VISION_DIM, level);
        VALIDATE_RANGE(config.maxEnergy, WorldConfig::MIN_ENERGY, WorldConfig::MAX_ENERGY, level);
        VALIDATE_RANGE(config.visionMaxRange, WorldConfig::MIN_RANGE, WorldConfig::MAX_RANGE, level);
        VALIDATE_RANGE(config.interactRange, WorldConfig::MIN_RANGE, WorldConfig::MAX_RANGE, level);
        
        if (config.maxObjects == 0 || config.maxObjects > 1000000) {
            throw WorldError("Max objects " + std::to_string(config.maxObjects) + 
                           " is out of valid range", ErrorCode::InvalidParameter);
        }
        if (config.objectPoolSize == 0) {
            throw WorldError("Object pool size must be positive", ErrorCode::InvalidParameter);
        }
    } else {
        // Lenient validation
        if (config.width <= 0.0f || config.height <= 0.0f) valid = false;
        if (config.visionWidth == 0 || config.visionHeight == 0) valid = false;
        if (config.maxEnergy <= 0.0f) valid = false;
    }
    
    return valid;
}

} // namespace nlm