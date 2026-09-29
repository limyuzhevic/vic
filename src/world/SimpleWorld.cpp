#include "SimpleWorld.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <random>
#include <unordered_map>
#include <utility>
#include <functional>
#include <chrono>

namespace nlm {

// Spatial grid cell structure for efficient collision detection
struct GridCell {
    std::vector<WorldObject*> objects;
};

// Dynamic behavior types for objects
enum class ObjectBehavior {
    Static,
    Chasing,
    Wandering,
    Rotating,
    Oscillating,
    Periodic
};

// Environmental hazard types
enum class HazardType {
    StaticHazard,
    RotatingHazard,
    ExpandingHazard,
    PulsingHazard,
    StormHazard
};

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

// Spatial grid manager for O(1) collision detection
class SpatialGrid {
public:
    SpatialGrid(float width, float height, float cellSize = 1.0f) 
        : width_(width), height_(height), cellSize_(cellSize) {
        grid_.resize(static_cast<size_t>(width_ / cellSize_) + 1,
                    std::vector<GridCell>(static_cast<size_t>(height_ / cellSize_) + 1));
    }
    
    void clear() {
        for (auto& row : grid_) {
            for (auto& cell : row) {
                cell.objects.clear();
            }
        }
    }
    
    void insert(WorldObject* obj) {
        int x = static_cast<int>(obj->x / cellSize_);
        int y = static_cast<int>(obj->y / cellSize_);
        if (x >= 0 && x < static_cast<int>(grid_.size()) && 
            y >= 0 && y < static_cast<int>(grid_[0].size())) {
            grid_[x][y].objects.push_back(obj);
        }
    }
    
    void updateObject(WorldObject* oldObj, WorldObject* newObj) {
        if (oldObj != newObj) {
            // Remove from old position
            int oldX = static_cast<int>(oldObj->x / cellSize_);
            int oldY = static_cast<int>(oldObj->y / cellSize_);
            if (oldX >= 0 && oldX < static_cast<int>(grid_.size()) && 
                oldY >= 0 && oldY < static_cast<int>(grid_[0].size())) {
                auto& oldCell = grid_[oldX][oldY];
                oldCell.objects.erase(
                    std::remove(oldCell.objects.begin(), oldCell.objects.end(), oldObj),
                    oldCell.objects.end()
                );
            }
            
            // Add to new position
            int newX = static_cast<int>(newObj->x / cellSize_);
            int newY = static_cast<int>(newObj->y / cellSize_);
            if (newX >= 0 && newX < static_cast<int>(grid_.size()) && 
                newY >= 0 && newY < static_cast<int>(grid_[0].size())) {
                grid_[newX][newY].objects.push_back(newObj);
            }
        }
    }
    
    std::vector<WorldObject*> getObjectsInCell(int x, int y) const {
        std::vector<WorldObject*> result;
        if (x >= 0 && x < static_cast<int>(grid_.size()) && 
            y >= 0 && y < static_cast<int>(grid_[0].size())) {
            result = grid_[x][y].objects;
        }
        return result;
    }
    
    // Get all objects in the immediate vicinity (3x3 neighborhood)
    std::vector<WorldObject*> getNearbyObjects(float x, float y, float range) const {
        std::vector<WorldObject*> result;
        int cellX = static_cast<int>(x / cellSize_);
        int cellY = static_cast<int>(y / cellSize_);
        int rangeCells = static_cast<int>(range / cellSize_);
        
        for (int dx = -rangeCells; dx <= rangeCells; ++dx) {
            for (int dy = -rangeCells; dy <= rangeCells; ++dy) {
                int nx = cellX + dx;
                int ny = cellY + dy;
                auto nearby = getObjectsInCell(nx, ny);
                result.insert(result.end(), nearby.begin(), nearby.end());
            }
        }
        
        return result;
    }
    
    float getCellSize() const { return cellSize_; }
    
private:
    float width_;
    float height_;
    float cellSize_;
    std::vector<std::vector<GridCell>> grid_;
};

// Moving object with dynamic behavior
struct DynamicObject {
    WorldObject* obj;
    float velocityX;
    float velocityY;
    float angularVelocity;
    ObjectBehavior behavior;
    float behaviorTimer;
    float behaviorPeriod;
    float chaseDistance;
    float wanderAngle;
    
    DynamicObject(WorldObject* o) 
        : obj(o), velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f),
          behavior(ObjectBehavior::Static), behaviorTimer(0.0f), behaviorPeriod(0.0f),
          chaseDistance(0.0f), wanderAngle(0.0f) {}
};

// Environmental hazard with time-varying properties
struct EnvironmentalHazard {
    HazardType type;
    float radius;
    float intensity;  // 0.0-1.0 for pulsing, angle for rotating, etc.
    float growthRate;
    float damagePerSecond;
    float activationTimer;
    bool active;
    
    EnvironmentalHazard() 
        : type(HazardType::StaticHazard), radius(2.0f), intensity(0.0f),
          growthRate(0.0f), damagePerSecond(0.0f), activationTimer(0.0f),
          active(false) {}
};

// Collision prediction structure
struct CollisionPrediction {
    float timeToCollision;
    WorldObject* obj1;
    WorldObject* obj2;
    float normalX;
    float normalY;
    
    CollisionPrediction() 
        : timeToCollision(FLT_MAX), obj1(nullptr), obj2(nullptr), normalX(0.0f), normalY(0.0f) {}
};

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
    , spatialGrid_(width_, height_, 1.0f) // 1x1 meter cells for fine-grained partitioning
    , enableTrajectoryPrediction_(true)
    , enableDynamicObstacles_(true)
    , enableEnvironmentalHazards_(true)
    , hazardSpawnRate_(0.5f)
    , predatorSpawnRate_(0.2f)
    , wanderingResourceRate_(0.3f)
    , rotatingHazardRate_(0.1f)
    , objectRepulsionForce_(1.0f)
    , environmentDecayRate_(0.001f)
    , agent_.hazardDamageMultiplier_(1.0f)
{
    agent_.reset();
}

SimpleWorld::~SimpleWorld() = default;

void SimpleWorld::configure(float width, float height, size_t visionWidth, size_t visionHeight) {
    width_ = width;
    height_ = height;
    visionWidth_ = visionWidth;
    visionHeight_ = visionHeight;
    spatialGrid_ = SpatialGrid(width_, height_, 1.0f);
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
    agent_.health = 1.0f;
    rngState_ = rngSeed_;  // Reset RNG for reproducible object placement
    
    objects_.clear();
    dynamicObjects_.clear();
    hazards_.clear();
    
    // Place some resource objects (positive reward when touched)
    for (int i = 0; i < 5; ++i) {
        float ox = urand(rngState_, 1.0f, width_ - 1.0f);
        float oy = urand(rngState_, 1.0f, height_ - 1.0f);
        objects_.emplace_back(ox, oy, WorldObjectType::Resource, 10.0f, 0.4f);
        dynamicObjects_.emplace_back(&objects_.back());
        setDynamicBehavior(&objects_.back(), ObjectBehavior::Wandering);
        setDynamicBehaviorPeriod(&objects_.back(), 5.0f + urand(rngState_, 0.0f, 5.0f));
    }
    
    // Place some hazard objects (negative reward when touched)
    for (int i = 0; i < 3; ++i) {
        float ox = urand(rngState_, 1.0f, width_ - 1.0f);
        float oy = urand(rngState_, 1.0f, height_ - 1.0f);
        objects_.emplace_back(ox, oy, WorldObjectType::Hazard, -15.0f, 0.3f);
        dynamicObjects_.emplace_back(&objects_.back());
        setDynamicBehavior(&objects_.back(), ObjectBehavior::Rotating);
    }
    
    // Generate initial spatial grid
    updateSpatialGrid();
    generateVision();
}

void SimpleWorld::setAgentStart(float x, float y) {
    agent_.x = x;
    agent_.y = y;
}

void SimpleWorld::update(double timestep) {
    simTime_ += timestep;
    
    // Store previous position for collision prediction
    float prevX = agent_.x;
    float prevY = agent_.y;
    
    // Apply velocity to position (smooth physics)
    float oldX = agent_.x;
    float oldY = agent_.y;
    agent_.x += agent_.velocityX * timestep * 60.0f;  // Scale for consistent simulation
    agent_.y += agent_.velocityY * timestep * 60.0f;
    
    // Apply angular velocity to orientation
    agent_.orientation += agent_.angularVelocity * timestep * 60.0f;
    
    // Normalize orientation to [-PI, PI]
    while (agent_.orientation > M_PI) agent_.orientation -= 2.0f * M_PI;
    while (agent_.orientation < -M_PI) agent_.orientation += 2.0f * M_PI;
    
    // Boundary collision with reflection
    bool bouncedX = false, bouncedY = false;
    if (agent_.x < 0.0f) { agent_.x = 0.0f; agent_.velocityX = -agent_.velocityX * 0.7f; bouncedX = true; }
    if (agent_.x >= width_) { agent_.x = width_ - 0.01f; agent_.velocityX = -agent_.velocityX * 0.7f; bouncedX = true; }
    if (agent_.y < 0.0f) { agent_.y = 0.0f; agent_.velocityY = -agent_.velocityY * 0.7f; bouncedY = true; }
    if (agent_.y >= height_) { agent_.y = height_ - 0.01f; agent_.velocityY = -agent_.velocityY * 0.7f; bouncedY = true; }
    
    if (bouncedX || bouncedY) {
        agent_.isMoving = true;
        // Add some randomness to velocity after collision
        agent_.velocityX += urand(rngState_, -0.5f, 0.5f);
        agent_.velocityY += urand(rngState_, -0.5f, 0.5f);
    }
    
    // Energy decay
    agent_.energy -= energyDecayRate_ * timestep * 60.0f;
    if (agent_.energy < 0.0f) agent_.energy = 0.0f;
    
    // Update age
    agent_.age += timestep;
    
    // Update dynamic objects
    if (enableDynamicObstacles_) {
        updateDynamicObjects(timestep);
    }
    
    // Update environmental hazards
    if (enableEnvironmentalHazards_) {
        updateEnvironmentalHazards(timestep);
    }
    
    // Predict and resolve trajectory collisions
    if (enableTrajectoryPrediction_ && oldX != agent_.x && oldY != agent_.y) {
        resolveTrajectoryCollisions(oldX, oldY, agent_.x, agent_.y, timestep);
    }
    
    // Check for collisions along the movement path
    checkPathCollisions(oldX, oldY, agent_.x, agent_.y);
    
    // Generate vision
    generateVision();
    
    // Check collisions (passive)
    checkCollisions();
    
    // Update spatial grid
    updateSpatialGrid();
}

void SimpleWorld::updateDynamicObjects(double timestep) {
    for (auto& dynObj : dynamicObjects_) {
        if (!dynObj.obj || !dynObj.obj->active) continue;
        
        // Update behavior timer
        dynObj.behaviorTimer += timestep * 60.0f;
        
        // Apply dynamic behavior
        switch (dynObj.behavior) {
            case ObjectBehavior::Chasing:
                chaseAgent(dynObj, timestep);
                break;
            case ObjectBehavior::Wandering:
                wander(dynObj, timestep);
                break;
            case ObjectBehavior::Rotating:
                rotate(dynObj, timestep);
                break;
            case ObjectBehavior::Oscillating:
                oscillate(dynObj, timestep);
                break;
            case ObjectBehavior::Periodic:
                periodic(dynObj, timestep);
                break;
            case ObjectBehavior::Static:
            default:
                // Static objects don't move
                break;
        }
        
        // Update object position in spatial grid
        spatialGrid_.updateObject(dynObj.obj, dynObj.obj);
        
        // Check for collision with agent
        checkDynamicObjectCollision(*dynObj.obj, timestep);
    }
}

void SimpleWorld::updateEnvironmentalHazards(double timestep) {
    // Randomly spawn new hazards
    if (urand(rngState_, 0.0f, 1.0f) < hazardSpawnRate_ * timestep * 60.0f) {
        spawnRandomHazard();
    }
    
    // Update existing hazards
    for (auto& hazard : hazards_) {
        if (!hazard.active) continue;
        
        switch (hazard.type) {
            case HazardType::StaticHazard:
                // Static hazards don't change
                break;
            case HazardType::RotatingHazard:
                hazard.intensity += M_PI * timestep * 60.0f; // Rotate full circle per 60 seconds
                break;
            case HazardType::ExpandingHazard:
                hazard.radius += hazard.growthRate * timestep * 60.0f;
                break;
            case HazardType::PulsingHazard:
                hazard.intensity = std::abs(std::sin(hazard.activationTimer * 2.0f));
                break;
            case HazardType::StormHazard:
                // Storm hazard affects nearby agent
                break;
        }
        
        hazard.activationTimer += timestep * 60.0f;
        hazard.active = hazard.radius < width_ * 0.8f; // Despawn when too large
        
        // Check hazard damage
        checkHazardDamage(hazard, timestep);
    }
}

void SimpleWorld::chaseAgent(DynamicObject& dynObj, double timestep) {
    float dx = agent_.x - dynObj.obj->x;
    float dy = agent_.y - dynObj.obj->y;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist > 0.0f) {
        // Move toward agent with some randomness
        float speed = 1.5f + urand(rngState_, 0.0f, 1.0f);
        dynObj.velocityX = dx / dist * speed;
        dynObj.velocityY = dy / dist * speed;
    }
    
    // Update position
    dynObj.obj->x += dynObj.velocityX * timestep * 60.0f;
    dynObj.obj->y += dynObj.velocityY * timestep * 60.0f;
    
    // Keep within bounds
    dynObj.obj->x = std::max(0.5f, std::min(width_ - 0.5f, dynObj.obj->x));
    dynObj.obj->y = std::max(0.5f, std::min(height_ - 0.5f, dynObj.obj->y));
}

void SimpleWorld::wander(DynamicObject& dynObj, double timestep) {
    // Wander in random direction with periodic changes
    if (dynObj.behaviorTimer > dynObj.behaviorPeriod) {
        dynObj.wanderAngle = urand(rngState_, 0.0f, 2.0f * M_PI);
        dynObj.behaviorTimer = 0.0f;
    }
    
    float speed = 0.8f + urand(rngState_, 0.0f, 0.8f);
    dynObj.velocityX = std::cos(dynObj.wanderAngle) * speed;
    dynObj.velocityY = std::sin(dynObj.wanderAngle) * speed;
    
    // Update position
    dynObj.obj->x += dynObj.velocityX * timestep * 60.0f;
    dynObj.obj->y += dynObj.velocityY * timestep * 60.0f;
    
    // Keep within bounds
    dynObj.obj->x = std::max(0.5f, std::min(width_ - 0.5f, dynObj.obj->x));
    dynObj.obj->y = std::max(0.5f, std::min(height_ - 0.5f, dynObj.obj->y));
}

void SimpleWorld::rotate(DynamicObject& dynObj, double timestep) {
    // Rotate around its center point
    float centerX = dynObj.obj->x;
    float centerY = dynObj.obj->y;
    float angle = std::sin(dynObj.behaviorTimer * 2.0f) * M_PI; // Oscillate between -PI and PI
    
    float radius = 1.0f;
    float newX = centerX + std::cos(angle) * radius;
    float newY = centerY + std::sin(angle) * radius;
    
    dynObj.velocityX = (newX - dynObj.obj->x) / (timestep * 60.0f);
    dynObj.velocityY = (newY - dynObj.obj->y) / (timestep * 60.0f);
    
    dynObj.obj->x = newX;
    dynObj.obj->y = newY;
}

void SimpleWorld::oscillate(DynamicObject& dynObj, double timestep) {
    // Move back and forth between two points
    float amplitude = 2.0f;
    float frequency = 1.0f;
    float offsetX = 5.0f;
    float offsetY = 5.0f;
    
    float x = offsetX + amplitude * std::sin(dynObj.behaviorTimer * frequency);
    float y = offsetY + amplitude * std::cos(dynObj.behaviorTimer * frequency);
    
    dynObj.velocityX = (x - dynObj.obj->x) / (timestep * 60.0f);
    dynObj.velocityY = (y - dynObj.obj->y) / (timestep * 60.0f);
    
    dynObj.obj->x = x;
    dynObj.obj->y = y;
}

void SimpleWorld::periodic(DynamicObject& dynObj, double timestep) {
    // Change behavior periodically
    if (dynObj.behaviorTimer > dynObj.behaviorPeriod) {
        // Cycle through behaviors
        int current = static_cast<int>(dynObj.behavior);
        dynObj.behavior = static_cast<ObjectBehavior>((current + 1) % static_cast<int>(ObjectBehavior::Periodic));
        dynObj.behaviorTimer = 0.0f;
    }
    
    // Apply current behavior
    switch (dynObj.behavior) {
        case ObjectBehavior::Chasing:
            chaseAgent(dynObj, timestep);
            break;
        case ObjectBehavior::Wandering:
            wander(dynObj, timestep);
            break;
        case ObjectBehavior::Rotating:
            rotate(dynObj, timestep);
            break;
        case ObjectBehavior::Oscillating:
            oscillate(dynObj, timestep);
            break;
        case ObjectBehavior::Static:
            // Static
            break;
        case ObjectBehavior::Periodic:
            // Already handled
            break;
    }
}

void SimpleWorld::checkDynamicObjectCollision(WorldObject& obj, double timestep) {
    // Check for collision with agent
    float dx = agent_.x - obj.x;
    float dy = agent_.y - obj.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist < obj.radius + 0.5f) {  // Agent's effective radius
        // Collision detected - apply damage and push back
        if (obj.type == WorldObjectType::Resource) {
            agent_.energy = std::min(maxEnergy_, agent_.energy + obj.value);
        } else if (obj.type == WorldObjectType::Hazard) {
            agent_.health -= std::abs(obj.value) * 0.01f * timestep * 60.0f;
        }
        
        // Push agent away
        if (dist > 0.0f) {
            float pushForce = 0.1f / dist;
            agent_.x += dx / dist * pushForce;
            agent_.y += dy / dist * pushForce;
        }
        
        // Update velocities
        if (obj.type == WorldObjectType::Hazard) {
            agent_.velocityX += dx / dist * 0.5f;
            agent_.velocityY += dy / dist * 0.5f;
        }
    }
}

void SimpleWorld::resolveTrajectoryCollisions(float prevX, float prevY, float newX, float newY, double timestep) {
    // Check for collisions along the agent's movement trajectory
    const int trajectorySteps = 10;  // Divide movement into steps for accuracy
    float stepX = (newX - prevX) / trajectorySteps;
    float stepY = (newY - prevY) / trajectorySteps;
    
    for (int i = 1; i <= trajectorySteps; ++i) {
        float checkX = prevX + stepX * i;
        float checkY = prevY + stepY * i;
        
        // Check for object collisions along trajectory
        for (auto& obj : objects_) {
            if (!obj.active) continue;
            
            float dx = obj.x - checkX;
            float dy = obj.y - checkY;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist < obj.radius) {
                // Collision detected - adjust position and apply physics
                float pushBack = obj.radius - dist;
                float dirX = dx / dist;
                float dirY = dy / dist;
                
                // Push agent back along trajectory
                agent_.x = checkX - dirX * pushBack * 2.0f;
                agent_.y = checkY - dirY * pushBack * 2.0f;
                
                // Apply collision response
                if (obj.type == WorldObjectType::Resource) {
                    agent_.energy = std::min(maxEnergy_, agent_.energy + obj.value);
                } else if (obj.type == WorldObjectType::Hazard) {
                    agent_.health -= std::abs(obj.value) * 0.005f * timestep * 60.0f;
                }
                
                // Stop movement at collision point
                return;
            }
        }
        
        // Check for hazard field collisions
        for (const auto& hazard : hazards_) {
            if (!hazard.active) continue;
            
            float dx = checkX - hazard.radius;  // Hazard center is at radius distance
            float dy = checkY - hazard.radius;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist < hazard.radius * hazard.intensity) {  // Effective hazard radius
                // Hazard collision
                agent_.health -= hazard.damagePerSecond * timestep * 60.0f;
            }
        }
    }
}

void SimpleWorld::checkPathCollisions(float startX, float startY, float endX, float endY) {
    // Fast path collision check using spatial grid
    int startCellX = static_cast<int>(startX / spatialGrid_.getCellSize());
    int startCellY = static_cast<int>(startY / spatialGrid_.getCellSize());
    int endCellX = static_cast<int>(endX / spatialGrid_.getCellSize());
    int endCellY = static_cast<int>(endY / spatialGrid_.getCellSize());
    
    // Check bounding box of movement
    int minX = std::min(startCellX, endCellX);
    int maxX = std::max(startCellX, endCellX);
    int minY = std::min(startCellY, endCellY);
    int maxY = std::max(startCellY, endCellY);
    
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            auto objects = spatialGrid_.getObjectsInCell(x, y);
            for (auto obj : objects) {
                if (!obj || !obj->active) continue;
                
                float dx = obj->x - (startX + endX) / 2.0f;  // Check midpoint
                float dy = obj->y - (startY + endY) / 2.0f;
                float dist = std::sqrt(dx * dx + dy * dy);
                
                if (dist < obj->radius) {
                    // Passive damage from hazards
                    if (obj->type == WorldObjectType::Hazard) {
                        agent_.health -= 0.005f;  // Slower passive damage
                    }
                }
            }
        }
    }
}

void SimpleWorld::checkCollisions() {
    // Passive collision check (for damage when moving into objects)
    // Use spatial grid for efficiency
    auto nearbyObjects = spatialGrid_.getNearbyObjects(agent_.x, agent_.y, 2.0f);
    
    for (auto obj : nearbyObjects) {
        if (!obj || !obj->active) continue;
        
        float dx = obj->x - agent_.x;
        float dy = obj->y - agent_.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj->radius) {
            // Passive damage from hazards only
            if (obj->type == WorldObjectType::Resource) {
                // Resources don't cause passive damage
            } else if (obj->type == WorldObjectType::Hazard) {
                agent_.health -= 0.01f;  // Passive damage
            }
        }
    }
}

void SimpleWorld::spawnRandomHazard() {
    HazardType hazardTypes[5] = {
        HazardType::StaticHazard,
        HazardType::RotatingHazard,
        HazardType::ExpandingHazard,
        HazardType::PulsingHazard,
        HazardType::StormHazard
    };
    
    HazardType type = hazardTypes[static_cast<int>(urand(rngState_, 0.0f, 4.0f))];
    
    EnvironmentalHazard hazard;
    hazard.type = type;
    hazard.active = true;
    hazard.radius = 1.0f;
    hazard.intensity = 0.0f;
    hazard.growthRate = 0.1f;
    hazard.damagePerSecond = 0.1f;
    
    // Place hazard in middle of world
    hazard.radius = width_ * 0.1f + urand(rngState_, 0.0f, width_ * 0.1f);
    
    hazards_.push_back(hazard);
}

void SimpleWorld::checkHazardDamage(const EnvironmentalHazard& hazard, double timestep) {
    // Check if agent is within hazard range
    float dx = agent_.x - hazard.radius;  // Hazard center is at radius distance
    float dy = agent_.y - hazard.radius;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist < hazard.radius * hazard.intensity) {
        // Agent takes damage
        agent_.health -= hazard.damagePerSecond * timestep * 60.0f;
        
        // Visual effect for storm hazards
        if (hazard.type == HazardType::StormHazard) {
            // Storm creates visual distortion
            // (Could add visual effects here)
        }
    }
}

void SimpleWorld::updateSpatialGrid() {
    spatialGrid_.clear();
    
    // Insert all active objects into spatial grid
    for (auto& obj : objects_) {
        if (obj.active) {
            spatialGrid_.insert(&obj);
        }
    }
}

void SimpleWorld::setDynamicBehavior(WorldObject* obj, ObjectBehavior behavior) {
    if (!obj) return;
    
    // Find or create dynamic object
    for (auto& dynObj : dynamicObjects_) {
        if (dynObj.obj == obj) {
            dynObj.behavior = behavior;
            return;
        }
    }
    
    // Create new dynamic object if not found
    dynamicObjects_.emplace_back(obj);
    dynamicObjects_.back().behavior = behavior;
}

void SimpleWorld::setDynamicBehaviorPeriod(WorldObject* obj, float period) {
    if (!obj) return;
    
    for (auto& dynObj : dynamicObjects_) {
        if (dynObj.obj == obj) {
            dynObj.behaviorPeriod = period;
            return;
        }
    }
}

// Configuration methods for dynamic behaviors
void SimpleWorld::setEnableTrajectoryPrediction(bool enabled) {
    enableTrajectoryPrediction_ = enabled;
}

void SimpleWorld::setEnableDynamicObstacles(bool enabled) {
    enableDynamicObstacles_ = enabled;
}

void SimpleWorld::setEnableEnvironmentalHazards(bool enabled) {
    enableEnvironmentalHazards_ = enabled;
}

void SimpleWorld::setHazardSpawnRate(float rate) {
    hazardSpawnRate_ = rate;
}

void SimpleWorld::setPredatorSpawnRate(float rate) {
    predatorSpawnRate_ = rate;
}

void SimpleWorld::setWanderingResourceRate(float rate) {
    wanderingResourceRate_ = rate;
}

void SimpleWorld::setRotatingHazardRate(float rate) {
    rotatingHazardRate_ = rate;
}

void SimpleWorld::update(double timestep) {
    simTime_ += timestep;
    
    // Apply velocity to position (smooth physics)
    float oldX = agent_.x;
    float oldY = agent_.y;
    agent_.x += agent_.velocityX * timestep * 60.0f;  // Scale for consistent simulation
    agent_.y += agent_.velocityY * timestep * 60.0f;
    
    // Apply angular velocity to orientation
    agent_.orientation += agent_.angularVelocity * timestep * 60.0f;
    
    // Normalize orientation to [-PI, PI]
    while (agent_.orientation > M_PI) agent_.orientation -= 2.0f * M_PI;
    while (agent_.orientation < -M_PI) agent_.orientation += 2.0f * M_PI;
    
    // Boundary collision with reflection
    bool bouncedX = false, bouncedY = false;
    if (agent_.x < 0.0f) { agent_.x = 0.0f; agent_.velocityX = -agent_.velocityX * 0.7f; bouncedX = true; }
    if (agent_.x >= width_) { agent_.x = width_ - 0.01f; agent_.velocityX = -agent_.velocityX * 0.7f; bouncedX = true; }
    if (agent_.y < 0.0f) { agent_.y = 0.0f; agent_.velocityY = -agent_.velocityY * 0.7f; bouncedY = true; }
    if (agent_.y >= height_) { agent_.y = height_ - 0.01f; agent_.velocityY = -agent_.velocityY * 0.7f; bouncedY = true; }
    
    if (bouncedX || bouncedY) {
        agent_.isMoving = true;
        // Add some randomness to velocity after collision
        agent_.velocityX += urand(rngState_, -0.5f, 0.5f);
        agent_.velocityY += urand(rngState_, -0.5f, 0.5f);
    }
    
    // Energy decay
    agent_.energy -= energyDecayRate_ * timestep * 60.0f;
    if (agent_.energy < 0.0f) agent_.energy = 0.0f;
    
    // Update age
    agent_.age += timestep;
    
    // Update dynamic objects
    if (enableDynamicObstacles_) {
        updateDynamicObjects(timestep);
    }
    
    // Update environmental hazards
    if (enableEnvironmentalHazards_) {
        updateEnvironmentalHazards(timestep);
    }
    
    // Predict and resolve trajectory collisions
    if (enableTrajectoryPrediction_ && oldX != agent_.x && oldY != agent_.y) {
        resolveTrajectoryCollisions(oldX, oldY, agent_.x, agent_.y, timestep);
    }
    
    // Check for collisions along the movement path
    checkPathCollisions(oldX, oldY, agent_.x, agent_.y);
    
    // Generate vision
    generateVision();
    
    // Check collisions (passive)
    checkCollisions();
    
    // Update spatial grid
    updateSpatialGrid();
}

// Additional methods for configuration options
void SimpleWorld::enableTrajectoryPrediction(bool enabled) {
    enableTrajectoryPrediction_ = enabled;
}

void SimpleWorld::enableDynamicObstacles(bool enabled) {
    enableDynamicObstacles_ = enabled;
}

void SimpleWorld::enableEnvironmentalHazards(bool enabled) {
    enableEnvironmentalHazards_ = enabled;
}

void SimpleWorld::setHazardSpawnRate(float rate) {
    hazardSpawnRate_ = rate;
}

void SimpleWorld::setPredatorSpawnRate(float rate) {
    predatorSpawnRate_ = rate;
}

void SimpleWorld::setWanderingResourceRate(float rate) {
    wanderingResourceRate_ = rate;
}

void SimpleWorld::setRotatingHazardRate(float rate) {
    rotatingHazardRate_ = rate;
}

// Getters for configuration
bool SimpleWorld::isTrajectoryPredictionEnabled() const {
    return enableTrajectoryPrediction_;
}

bool SimpleWorld::isDynamicObstaclesEnabled() const {
    return enableDynamicObstacles_;
}

bool SimpleWorld::isEnvironmentalHazardsEnabled() const {
    return enableEnvironmentalHazards_;
}

float SimpleWorld::getHazardSpawnRate() const {
    return hazardSpawnRate_;
}

float SimpleWorld::getPredatorSpawnRate() const {
    return predatorSpawnRate_;
}

float SimpleWorld::getWanderingResourceRate() const {
    return wanderingResourceRate_;
}

float SimpleWorld::getRotatingHazardRate() const {
    return rotatingHazardRate_;
}

// Additional utility methods
void SimpleWorld::addHazard(const EnvironmentalHazard& hazard) {
    hazards_.push_back(hazard);
}

void SimpleWorld::removeHazard(size_t index) {
    if (index < hazards_.size()) {
        hazards_.erase(hazards_.begin() + index);
    }
}

void SimpleWorld::clearHazards() {
    hazards_.clear();
}

void SimpleWorld::setAgentMaxHealth(float health) {
    agent_.health = health;
}

void SimpleWorld::setAgentDamagePerHazard(float damage) {
    agent_.hazardDamageMultiplier_ = damage;
}

void SimpleWorld::setObjectRepulsionForce(float force) {
    objectRepulsionForce_ = force;
}

void SimpleWorld::setEnvironmentDecayRate(float rate) {
    environmentDecayRate_ = rate;
}

// Getters for object count
int SimpleWorld::getObjectCount() const {
    return objects_.size();
}

int SimpleWorld::getHazardCount() const {
    return hazards_.size();
}

int SimpleWorld::getDynamicObjectCount() const {
    return dynamicObjects_.size();
}

// Utility method to get spatial grid cell size
float SimpleWorld::getCellSize() const {
    return spatialGrid_.getCellSize();
}

// Update the spatial grid when objects move
void SimpleWorld::updateSpatialGrid() {
    spatialGrid_.clear();
    
    for (auto& obj : objects_) {
        if (obj.active) {
            spatialGrid_.insert(&obj);
        }
    }
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
                // Check for trajectory collision before moving
                if (enableTrajectoryPrediction_) {
                    CollisionPrediction pred = predictCollision(agent_.x, agent_.y, newX, newY);
                    if (pred.timeToCollision < 0.1f) {
                        // Collision predicted, don't move
                        result.reward = -0.05f;  // Penalty for attempting to move into obstacle
                        result.success = false;
                        result.message = "collision_predicted";
                        break;
                    }
                }
                
                agent_.x = newX;
                agent_.y = newY;
                agent_.velocityX = dx * 10.0f;  // Convert to velocity
                agent_.velocityY = dy * 10.0f;
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
                if (enableTrajectoryPrediction_) {
                    CollisionPrediction pred = predictCollision(agent_.x, agent_.y, newX, newY);
                    if (pred.timeToCollision < 0.1f) {
                        result.reward = -0.05f;
                        result.success = false;
                        result.message = "collision_predicted";
                        break;
                    }
                }
                
                agent_.x = newX;
                agent_.y = newY;
                agent_.velocityX = dx * 10.0f;
                agent_.velocityY = dy * 10.0f;
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
    
    return result;
}

CollisionPrediction SimpleWorld::predictCollision(float startX, float startY, float endX, float endY) {
    CollisionPrediction bestPred;
    bestPred.timeToCollision = FLT_MAX;
    
    // Check for object collisions along trajectory
    const int trajectorySteps = 20;  // More detailed prediction
    float stepX = (endX - startX) / trajectorySteps;
    float stepY = (endY - startY) / trajectorySteps;
    
    for (int i = 1; i <= trajectorySteps; ++i) {
        float checkX = startX + stepX * i;
        float checkY = startY + stepY * i;
        
        for (auto& obj : objects_) {
            if (!obj.active) continue;
            
            float dx = obj.x - checkX;
            float dy = obj.y - checkY;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist < obj.radius) {
                float timeToImpact = (i * 0.1f) / 60.0f;  // Approximate time
                if (timeToImpact < bestPred.timeToCollision) {
                    bestPred.timeToCollision = timeToImpact;
                    bestPred.obj1 = &agent_;
                    bestPred.obj2 = &obj;
                    bestPred.normalX = dx / dist;
                    bestPred.normalY = dy / dist;
                }
            }
        }
        
        // Check for hazard field collisions
        for (const auto& hazard : hazards_) {
            if (!hazard.active) continue;
            
            float dx = checkX - hazard.radius;
            float dy = checkY - hazard.radius;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist < hazard.radius * hazard.intensity) {
                float timeToImpact = (i * 0.1f) / 60.0f;
                if (timeToImpact < bestPred.timeToCollision) {
                    bestPred.timeToCollision = timeToImpact;
                    bestPred.obj1 = &agent_;
                    bestPred.obj2 = nullptr;  // No specific object, but hazard field
                    bestPred.normalX = dx / dist;
                    bestPred.normalY = dy / dist;
                }
            }
        }
    }
    
    return bestPred;
}

void SimpleWorld::checkPathCollisions(float startX, float startY, float endX, float endY) {
    // Fast path collision check using spatial grid
    int startCellX = static_cast<int>(startX / spatialGrid_.getCellSize());
    int startCellY = static_cast<int>(startY / spatialGrid_.getCellSize());
    int endCellX = static_cast<int>(endX / spatialGrid_.getCellSize());
    int endCellY = static_cast<int>(endY / spatialGrid_.getCellSize());
    
    // Check bounding box of movement
    int minX = std::min(startCellX, endCellX);
    int maxX = std::max(startCellX, endCellX);
    int minY = std::min(startCellY, endCellY);
    int maxY = std::max(startCellY, endCellY);
    
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            auto objects = spatialGrid_.getObjectsInCell(x, y);
            for (auto obj : objects) {
                if (!obj || !obj->active) continue;
                
                float dx = obj->x - (startX + endX) / 2.0f;
                float dy = obj->y - (startY + endY) / 2.0f;
                float dist = std::sqrt(dx * dx + dy * dy);
                
                if (dist < obj->radius) {
                    // Passive damage from hazards
                    if (obj->type == WorldObjectType::Hazard) {
                        agent_.health -= 0.005f;
                    }
                }
            }
        }
    }
}

void SimpleWorld::checkCollisions() {
    // Passive collision check (for damage when moving into objects)
    // Use spatial grid for efficiency
    auto nearbyObjects = spatialGrid_.getNearbyObjects(agent_.x, agent_.y, 2.0f);
    
    for (auto obj : nearbyObjects) {
        if (!obj || !obj->active) continue;
        
        float dx = obj->x - agent_.x;
        float dy = obj->y - agent_.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj->radius) {
            // Passive damage from hazards only
            if (obj->type == WorldObjectType::Resource) {
                // Resources don't cause passive damage
            } else if (obj->type == WorldObjectType::Hazard) {
                agent_.health -= 0.01f;
            }
        }
    }
}

void SimpleWorld::generateVision() {
    // Enhanced vision with dynamic object detection
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
            
            // Check object collision with enhanced detection
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
    
    // Enhanced touch sensors (proximity to obstacles in 8 directions)
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
    
    sensory_.setVision(vision);
    sensory_.setTimestamp(simTime_);
    
    // Generate touch sensors (proximity to obstacles in 8 directions)
    std::vector<float> enhancedTouch(8, 0.0f);
    for (int i = 0; i < 8; ++i) {
        float angle = agent_.orientation + 2.0f * M_PI * i / 8.0f;
        float dx = std::cos(angle);
        float dy = std::sin(angle);
        
        float t = 0.0f;
        while (t < 1.5f) {
            float px = agent_.x + dx * t;
            float py = agent_.y + dy * t;
            
            if (!isValidPosition(px, py) || getObjectAt(px, py) != nullptr) {
                enhancedTouch[i] = 1.0f - t / 1.5f;  // Closer = stronger signal
                break;
            }
            t += 0.1f;
        }
    }
    
    // Generate internal signals (energy, health)
    std::vector<float> internal(4, 0.0f);
    internal[0] = agent_.energy / maxEnergy_;  // Energy level
    internal[1] = agent_.health;               // Health
    internal[2] = agent_.isMoving ? 1.0f : 0.0f;  // Movement state
    internal[3] = agent_.isTurning ? 1.0f : 0.0f;  // Turning state
    
    // Generate proprioception (body position/velocity)
    std::vector<float> proprioception(6, 0.0f);
    proprioception[0] = agent_.x / width_;     // Normalized X
    proprioception[1] = agent_.y / height_;    // Normalized Y
    proprioception[2] = (agent_.orientation + M_PI) / (2.0f * M_PI);  // Normalized orientation
    proprioception[3] = std::abs(agent_.velocityX) / 5.0f;  // Speed X
    proprioception[4] = std::abs(agent_.velocityY) / 5.0f;  // Speed Y
    proprioception[5] = std::abs(agent_.angularVelocity) / 3.0f;  // Turn rate
    
    sensory_.setTouch(enhancedTouch);
    sensory_.setInternal(internal);
    sensory_.setProprioception(proprioception);
    
    // No audio for now
    sensory_.setAudio({});
}

void SimpleWorld::addObject(const WorldObject& obj) {
    objects_.push_back(obj);
    dynamicObjects_.emplace_back(&objects_.back());
    setDynamicBehavior(&objects_.back(), ObjectBehavior::Wandering);
}

void SimpleWorld::removeObject(float x, float y) {
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - x;
        float dy = obj.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj.radius) {
            obj.active = false;
            
            // Also remove from dynamic objects
            dynamicObjects_.erase(
                std::remove_if(dynamicObjects_.begin(), dynamicObjects_.end(),
                    [&obj](const DynamicObject& dyn) { return dyn.obj == &obj; }),
                dynamicObjects_.end()
            );
            
            return;
        }
    }
}

WorldObject* SimpleWorld::getObjectAt(float x, float y) {
    // Use spatial grid for efficiency
    auto nearbyObjects = spatialGrid_.getNearbyObjects(x, y, 1.0f);
    
    for (auto obj : nearbyObjects) {
        if (!obj || !obj->active) continue;
        
        float dx = obj->x - x;
        float dy = obj->y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj->radius) {
            return obj;
        }
    }
    
    return nullptr;
}

bool SimpleWorld::isValidPosition(float x, float y) const {
    // Check world boundaries
    if (x < 0.0f || x >= width_ || y < 0.0f || y >= height_) {
        return false;
    }
    
    // Check for object collisions with higher precision
    for (const auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - x;
        float dy = obj.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj.radius + 0.1f) {  // Slightly larger collision margin
            return false;
        }
    }
    
    return true;
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
            
            // Update dynamic objects list
            dynamicObjects_.erase(
                std::remove_if(dynamicObjects_.begin(), dynamicObjects_.end(),
                    [&obj](const DynamicObject& dyn) { return dyn.obj == &obj; }),
                dynamicObjects_.end()
            );
            
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

// Spatial grid implementation (moved from top for clarity)
class SpatialGrid {
public:
    SpatialGrid(float width, float height, float cellSize = 1.0f) 
        : width_(width), height_(height), cellSize_(cellSize) {
        grid_.resize(static_cast<size_t>(width_ / cellSize_) + 1,
                    std::vector<GridCell>(static_cast<size_t>(height_ / cellSize_) + 1));
    }
    
    void clear() {
        for (auto& row : grid_) {
            for (auto& cell : row) {
                cell.objects.clear();
            }
        }
    }
    
    void insert(WorldObject* obj) {
        int x = static_cast<int>(obj->x / cellSize_);
        int y = static_cast<int>(obj->y / cellSize_);
        if (x >= 0 && x < static_cast<int>(grid_.size()) && 
            y >= 0 && y < static_cast<int>(grid_[0].size())) {
            grid_[x][y].objects.push_back(obj);
        }
    }
    
    void updateObject(WorldObject* oldObj, WorldObject* newObj) {
        if (oldObj != newObj) {
            // Remove from old position
            int oldX = static_cast<int>(oldObj->x / cellSize_);
            int oldY = static_cast<int>(oldObj->y / cellSize_);
            if (oldX >= 0 && oldX < static_cast<int>(grid_.size()) && 
                oldY >= 0 && oldY < static_cast<int>(grid_[0].size())) {
                auto& oldCell = grid_[oldX][oldY];
                oldCell.objects.erase(
                    std::remove(oldCell.objects.begin(), oldCell.objects.end(), oldObj),
                    oldCell.objects.end()
                );
            }
            
            // Add to new position
            int newX = static_cast<int>(newObj->x / cellSize_);
            int newY = static_cast<int>(newObj->y / cellSize_);
            if (newX >= 0 && newX < static_cast<int>(grid_.size()) && 
                newY >= 0 && newY < static_cast<int>(grid_[0].size())) {
                grid_[newX][newY].objects.push_back(newObj);
            }
        }
    }
    
    std::vector<WorldObject*> getObjectsInCell(int x, int y) const {
        std::vector<WorldObject*> result;
        if (x >= 0 && x < static_cast<int>(grid_.size()) && 
            y >= 0 && y < static_cast<int>(grid_[0].size())) {
            result = grid_[x][y].objects;
        }
        return result;
    }
    
    // Get all objects in the immediate vicinity (3x3 neighborhood)
    std::vector<WorldObject*> getNearbyObjects(float x, float y, float range) const {
        std::vector<WorldObject*> result;
        int cellX = static_cast<int>(x / cellSize_);
        int cellY = static_cast<int>(y / cellSize_);
        int rangeCells = static_cast<int>(range / cellSize_);
        
        for (int dx = -rangeCells; dx <= rangeCells; ++dx) {
            for (int dy = -rangeCells; dy <= rangeCells; ++dy) {
                int nx = cellX + dx;
                int ny = cellY + dy;
                auto nearby = getObjectsInCell(nx, ny);
                result.insert(result.end(), nearby.begin(), nearby.end());
            }
        }
        
        return result;
    }
    
    float getCellSize() const { return cellSize_; }
    
private:
    float width_;
    float height_;
    float cellSize_;
    std::vector<std::vector<GridCell>> grid_;
};

// Static initialization of the spatial grid
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
    , spatialGrid_(width_, height_, 1.0f) // 1x1 meter cells for fine-grained partitioning
    , enableTrajectoryPrediction_(true)
    , enableDynamicObstacles_(true)
    , enableEnvironmentalHazards_(true)
    , hazardSpawnRate_(0.5f)
    , predatorSpawnRate_(0.2f)
    , wanderingResourceRate_(0.3f)
    , rotatingHazardRate_(0.1f)
    , objectRepulsionForce_(1.0f)
    , environmentDecayRate_(0.001f)
    , agent_.hazardDamageMultiplier_(1.0f)
{
    agent_.reset();
}

SimpleWorld::~SimpleWorld() = default;