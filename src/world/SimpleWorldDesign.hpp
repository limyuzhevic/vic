// SimpleWorldDesign.hpp - New architecture design
#pragma once

#include <array>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <functional>
#include <optional>
#include <variant>
#include <limits>

namespace nlm {

// Forward declarations for new architecture
class VisionGenerator;
class CollisionDetector;
class InteractionEngine;
class PhysicsSimulator;
class SpatialIndex;
class ObjectPool;

// Modern random number generator
class RandomGenerator {
public:
    RandomGenerator();
    RandomGenerator(uint64_t seed);
    void seed(uint64_t seed);
    uint64_t next();
    float nextFloat(float min = 0.0f, float max = 1.0f);
    
private:
    std::mt19937_64 generator_;
    uint64_t currentSeed_;
    mutable std::mutex mutex_;
};

// Validation levels
enum class ValidationLevel {
    None = 0,
    Lenient = 1,
    Moderate = 2,
    Strict = 3
};

// Configuration parameters with validation
struct WorldConfig {
    float width = 20.0f;
    float height = 20.0f;
    size_t visionWidth = 16;
    size_t visionHeight = 16;
    float maxEnergy = 100.0f;
    float energyDecayRate = 0.01f;
    float resourceEnergyGain = 30.0f;
    float visionMaxRange = 8.0f;
    float interactRange = 1.0f;
    size_t maxObjects = 100;
    size_t objectPoolSize = 64;
    
    // Validation
    ValidationLevel validationLevel = ValidationLevel::Moderate;
    
    bool validate() const;
    static constexpr float MIN_WIDTH = 1.0f;
    static constexpr float MAX_WIDTH = 1000.0f;
    static constexpr float MIN_HEIGHT = 1.0f;
    static constexpr float MAX_HEIGHT = 1000.0f;
    static constexpr size_t MIN_VISION_DIM = 4;
    static constexpr size_t MAX_VISION_DIM = 1024;
    static constexpr float MIN_ENERGY = 0.0f;
    static constexpr float MAX_ENERGY = 1000.0f;
    static constexpr float MIN_RANGE = 0.1f;
    static constexpr float MAX_RANGE = 100.0f;
};

// Thread-safe fixed-size array for vision data
template<typename T, size_t N>
class FixedArray {
public:
    FixedArray() = default;
    FixedArray(const std::array<T, N>& data) : data_(data) {}
    
    T& operator[](size_t index) {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_[index];
    }
    
    const T& operator[](size_t index) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_[index];
    }
    
    size_t size() const { return N; }
    std::array<T, N>& get() { return data_; }
    const std::array<T, N>& get() const { return data_; }
    
private:
    std::array<T, N> data_;
    mutable std::mutex mutex_;
};

// Spatial indexing for efficient collision detection
class SpatialIndex {
public:
    SpatialIndex(float width, float height, float cellSize = 1.0f);
    ~SpatialIndex() = default;
    
    void insert(float x, float y, size_t id, float radius);
    void remove(size_t id);
    void clear();
    
    std::vector<size_t> query(float x, float y, float radius) const;
    std::vector<size_t> query(float x, float y, float radius, 
                              const std::function<bool(size_t)>& filter) const;
    
    void setCellSize(float cellSize);
    void rebuild();
    
private:
    struct Cell {
        std::vector<size_t> objectIds;
    };
    
    float width_;
    float height_;
    float cellSize_;
    std::unordered_map<size_t, std::pair<float, float>> objects_;
    std::unordered_map<size_t, float> objectRadii_;
    mutable std::unordered_map<size_t, Cell> grid_;
    mutable std::mutex mutex_;
};

// Object pooling for efficient memory management
class ObjectPool {
public:
    ObjectPool(size_t capacity);
    ~ObjectPool() = default;
    
    WorldObject* acquire(float x, float y, WorldObjectType type, 
                         float value = 0.0f, float radius = 0.5f);
    void release(WorldObject* obj);
    void clear();
    size_t size() const { return pool_.size(); }
    
private:
    struct Node {
        WorldObject object;
        std::unique_ptr<Node> next;
        Node* prev;
        
        Node(float x, float y, WorldObjectType type, float value, float radius)
            : object(x, y, type, value, radius), next(nullptr), prev(nullptr) {}
    };
    
    std::unique_ptr<Node> head_;
    std::vector<WorldObject*> freeList_;
    size_t capacity_;
    std::mutex mutex_;
};

// Base class for vision generation
class VisionGenerator {
public:
    virtual ~VisionGenerator() = default;
    virtual void generateVision(const AgentBody& agent, 
                               const SpatialIndex& spatialIndex,
                               FixedArray<float, 256>& vision) = 0;
    virtual void setConfig(const WorldConfig& config) = 0;
    virtual void preprocess(const std::vector<WorldObject>& objects) = 0;
};

// Base class for collision detection
class CollisionDetector {
public:
    virtual ~CollisionDetector() = default;
    virtual void checkCollisions(const AgentBody& agent, 
                               const SpatialIndex& spatialIndex,
                               const std::vector<WorldObject*>& collisions) = 0;
    virtual void addObject(const WorldObject& obj) = 0;
    virtual void removeObject(size_t id) = 0;
    virtual void clear() = 0;
};

// Base class for interaction engine
class InteractionEngine {
public:
    virtual ~InteractionEngine() = default;
    virtual ActionResult interact(const AgentBody& agent,
                                const SpatialIndex& spatialIndex,
                                const std::vector<WorldObject*>& nearbyObjects) = 0;
    virtual void setConfig(const WorldConfig& config) = 0;
};

// Base class for physics simulation
class PhysicsSimulator {
public:
    virtual ~PhysicsSimulator() = default;
    virtual void simulate(AgentBody& agent, double timestep) = 0;
    virtual void setWorldBounds(float width, float height) = 0;
};

// Simple vision generator implementation
class SimpleVisionGenerator : public VisionGenerator {
public:
    void generateVision(const AgentBody& agent, 
                       const SpatialIndex& spatialIndex,
                       FixedArray<float, 256>& vision) override;
    void setConfig(const WorldConfig& config) override;
    void preprocess(const std::vector<WorldObject>& objects) override;
    
private:
    WorldConfig config_;
    std::vector<WorldObject> objects_;
    std::mutex mutex_;
};

// Simple collision detector implementation
class SimpleCollisionDetector : public CollisionDetector {
public:
    SimpleCollisionDetector(float width, float height);
    void checkCollisions(const AgentBody& agent, 
                        const SpatialIndex& spatialIndex,
                        const std::vector<WorldObject*>& collisions) override;
    void addObject(const WorldObject& obj) override;
    void removeObject(size_t id) override;
    void clear() override;
    
private:
    SpatialIndex spatialIndex_;
    std::mutex mutex_;
};

// Simple interaction engine implementation
class SimpleInteractionEngine : public InteractionEngine {
public:
    void setConfig(const WorldConfig& config) override;
    ActionResult interact(const AgentBody& agent,
                        const SpatialIndex& spatialIndex,
                        const std::vector<WorldObject*>& nearbyObjects) override;
    
private:
    WorldConfig config_;
    std::mutex mutex_;
};

// Simple physics simulator implementation
class SimplePhysicsSimulator : public PhysicsSimulator {
public:
    void simulate(AgentBody& agent, double timestep) override;
    void setWorldBounds(float width, float height) override;
    
private:
    float width_ = 20.0f;
    float height_ = 20.0f;
    std::mutex mutex_;
};

// Main world object with improved design
struct WorldObject {
    float x;
    float y;
    float radius;
    WorldObjectType type;
    float value;
    bool active;
    size_t id;
    double lastInteractionTime;
    
    WorldObject()
        : x(0.0f), y(0.0f), radius(0.5f), type(WorldObjectType::Empty)
        , value(0.0f), active(true), id(0), lastInteractionTime(0.0), 
        cachedDistanceSquared(0.0f) {}
    
    WorldObject(float px, float py, WorldObjectType t, float v = 0.0f, float r = 0.5f)
        : x(px), y(py), radius(r), type(t), value(v), active(true), id(0)
        , lastInteractionTime(0.0), cachedDistanceSquared(0.0f) {}
    
    // Cached distance calculation for performance
    float getDistanceSquared(float otherX, float otherY) const {
        if (cachedDistanceSquared == 0.0f) {
            float dx = x - otherX;
            float dy = y - otherY;
            cachedDistanceSquared = dx * dx + dy * dy;
        }
        return cachedDistanceSquared;
    }
    
    void invalidateCache() { cachedDistanceSquared = 0.0f; }
    
private:
    mutable float cachedDistanceSquared;
};

} // namespace nlm