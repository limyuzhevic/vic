// SimpleWorldAdvanced.hpp - Advanced features
#pragma once

#include "SimpleWorldDesign.hpp"
#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include <functional>
#include <type_traits>

namespace nlm {

// Error handling
enum class ErrorCode {
    Success = 0,
    InvalidParameter = 1,
    MemoryAllocationFailed = 2,
    SpatialIndexError = 3,
    ValidationFailed = 4,
    ThreadingError = 5,
    Unknown = 255
};

class WorldError : public std::runtime_error {
public:
    WorldError(const std::string& message, ErrorCode code = ErrorCode::Unknown)
        : std::runtime_error(message), code_(code) {}
    
    ErrorCode code() const { return code_; }
    
private:
    ErrorCode code_;
};

// Plugin architecture for sensors
class SensorPlugin {
public:
    virtual ~SensorPlugin() = default;
    virtual std::vector<float> readSensor(const AgentBody& agent,
                                         const SpatialIndex& spatialIndex,
                                         const std::vector<WorldObject>& objects) = 0;
    virtual std::string getName() const = 0;
    virtual size_t getDataSize() const = 0;
};

// Plugin architecture for world objects
class WorldObjectFactory {
public:
    virtual ~WorldObjectFactory() = default;
    virtual WorldObject* create(float x, float y, WorldObjectType type,
                               float value = 0.0f, float radius = 0.5f) = 0;
    virtual std::string getTypeName() const = 0;
};

// Concrete factory implementations
class ResourceObjectFactory : public WorldObjectFactory {
public:
    WorldObject* create(float x, float y, WorldObjectType type,
                       float value = 0.0f, float radius = 0.5f) override {
        return new WorldObject(x, y, WorldObjectType::Resource, value, radius);
    }
    std::string getTypeName() const override { return "Resource"; }
};

class HazardObjectFactory : public WorldObjectFactory {
public:
    WorldObject* create(float x, float y, WorldObjectType type,
                       float value = 0.0f, float radius = 0.5f) override {
        return new WorldObject(x, y, WorldObjectType::Hazard, value, radius);
    }
    std::string getTypeName() const override { return "Hazard"; }
};

// Enhanced vision generator with multithreading
class AdvancedVisionGenerator : public VisionGenerator {
public:
    AdvancedVisionGenerator(size_t threadCount = std::thread::hardware_concurrency());
    
    void generateVision(const AgentBody& agent, 
                       const SpatialIndex& spatialIndex,
                       FixedArray<float, 256>& vision) override;
    void setConfig(const WorldConfig& config) override;
    void preprocess(const std::vector<WorldObject>& objects) override;
    
    // Vision preprocessing options
    struct VisionOptions {
        bool useCaching = true;
        bool useSpatialIndex = true;
        bool multithreaded = true;
        size_t threadCount = std::thread::hardware_concurrency();
        float rayStepSize = 0.1f;
        float rayMaxRange = 8.0f;
        float fovDegrees = 90.0f;
    };
    
    void setVisionOptions(const VisionOptions& options) { visionOptions_ = options; }
    
private:
    VisionOptions visionOptions_;
    mutable std::mutex mutex_;
    std::vector<WorldObject> objectCache_;
    bool cacheDirty_ = true;
};

// Enhanced collision detector with error handling
class AdvancedCollisionDetector : public CollisionDetector {
public:
    AdvancedCollisionDetector(float width, float height);
    
    void checkCollisions(const AgentBody& agent, 
                        const SpatialIndex& spatialIndex,
                        const std::vector<WorldObject*>& collisions) override;
    void addObject(const WorldObject& obj) override;
    void removeObject(size_t id) override;
    void clear() override;
    
    // Error handling
    ErrorCode getLastError() const { return lastError_; }
    void clearError() { lastError_ = ErrorCode::Success; }
    
private:
    ErrorCode lastError_ = ErrorCode::Success;
    std::mutex mutex_;
    std::unordered_map<size_t, std::pair<float, float>> objects_;
    std::unordered_map<size_t, float> objectRadii_;
};

// Enhanced interaction engine with plugins
class AdvancedInteractionEngine : public InteractionEngine {
public:
    AdvancedInteractionEngine();
    
    void setConfig(const WorldConfig& config) override;
    ActionResult interact(const AgentBody& agent,
                        const SpatialIndex& spatialIndex,
                        const std::vector<WorldObject*>& nearbyObjects) override;
    
    // Plugin management
    template<typename T, typename... Args>
    void registerPlugin(Args&&... args) {
        static_assert(std::is_base_of<SensorPlugin, T>::value, "T must derive from SensorPlugin");
        plugins_.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }
    
    template<typename T, typename... Args>
    void registerObjectFactory(Args&&... args) {
        static_assert(std::is_base_of<WorldObjectFactory, T>::value, "T must derive from WorldObjectFactory");
        factories_[std::string(T().getTypeName())] = std::make_unique<T>(std::forward<Args>(args)...);
    }
    
private:
    std::vector<std::unique_ptr<SensorPlugin>> plugins_;
    std::unordered_map<std::string, std::unique_ptr<WorldObjectFactory>> factories_;
    std::mutex mutex_;
};

// Physics simulator with advanced features
class AdvancedPhysicsSimulator : public PhysicsSimulator {
public:
    AdvancedPhysicsSimulator();
    
    void simulate(AgentBody& agent, double timestep) override;
    void setWorldBounds(float width, float height) override;
    
    // Physics parameters
    struct PhysicsParams {
        float gravity = 9.81f;
        float dragCoefficient = 0.5f;
        float angularDragCoefficient = 0.5f;
        float restitution = 0.6f;
        float mass = 1.0f;
    };
    
    void setPhysicsParams(const PhysicsParams& params) { physicsParams_ = params; }
    
    // Boundary conditions
    enum class BoundaryCondition {
        Reflective,
        Absorbing,
        Periodic,
        Truncated
    };
    
    void setBoundaryCondition(BoundaryCondition condition) { boundaryCondition_ = condition; }
    
private:
    PhysicsParams physicsParams_;
    BoundaryCondition boundaryCondition_ = BoundaryCondition::Reflective;
    mutable std::mutex mutex_;
};

// Enhanced spatial index with adaptive grid
class AdaptiveSpatialIndex : public SpatialIndex {
public:
    AdaptiveSpatialIndex(float width, float height, float initialCellSize = 1.0f);
    
    void insert(float x, float y, size_t id, float radius) override;
    void remove(size_t id) override;
    void clear() override;
    
    std::vector<size_t> query(float x, float y, float radius) const override;
    std::vector<size_t> query(float x, float y, float radius,
                              const std::function<bool(size_t)>& filter) const override;
    
    void setCellSize(float cellSize) override;
    void rebuild() override;
    
    // Adaptive features
    void adaptGridSize();
    float getOptimalCellSize(float objectDensity) const;
    
private:
    float width_;
    float height_;
    float currentCellSize_;
    std::unordered_map<size_t, std::pair<float, float>> objects_;
    std::unordered_map<size_t, float> objectRadii_;
    mutable std::unordered_map<size_t, std::vector<size_t>> grid_;
    mutable std::mutex mutex_;
};

// Memory pool implementation
class MemoryPool {
public:
    MemoryPool(size_t chunkSize = 1024, size_t maxChunks = 1024);
    ~MemoryPool();
    
    void* allocate(size_t size);
    void deallocate(void* ptr, size_t size);
    size_t allocatedSize() const { return allocatedSize_; }
    size_t usedChunks() const { return usedChunks_; }
    
private:
    struct Chunk {
        void* memory;
        size_t size;
        Chunk* next;
        
        Chunk(size_t s) : memory(nullptr), size(s), next(nullptr) {
            memory = std::malloc(s);
        }
        ~Chunk() { std::free(memory); }
    };
    
    Chunk* head_;
    size_t chunkSize_;
    size_t maxChunks_;
    size_t allocatedSize_;
    size_t usedChunks_;
    mutable std::mutex mutex_;
};

// Validation utilities
class ValidationUtils {
public:
    template<typename T>
    static bool isInRange(T value, T min, T max, ValidationLevel level = ValidationLevel::Moderate) {
        if (level == ValidationLevel::None) return true;
        bool inRange = value >= min && value <= max;
        if (!inRange && level == ValidationLevel::Strict) {
            throw WorldError("Value " + std::to_string(value) + 
                           " is out of range [" + std::to_string(min) + ", " + 
                           std::to_string(max) + "]", ErrorCode::InvalidParameter);
        }
        return inRange;
    }
    
    static bool validateConfig(const WorldConfig& config, ValidationLevel level);
    
    // Zero-cost validation macros
    #define VALIDATE_RANGE(value, min, max, level) \
        if (!(value >= min && value <= max) && level == ValidationLevel::Strict) { \
            throw WorldError("Validation failed for value " + std::to_string(value), \
                           ErrorCode::ValidationFailed); \
        }
};

} // namespace nlm