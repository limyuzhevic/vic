#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../core/Performance/PerformanceMonitoring.hpp"
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <unordered_map>
#include <chrono>

namespace nlm {

// World object - represents an object in the world
// Position and type only - no semantic labels
struct WorldObject {
    float x;
    float y;
    float radius;
    WorldObjectType type;
    float value;        // Reward value (for resources) or damage (for hazards)
    bool active;
    
    WorldObject() 
        : x(0.0f), y(0.0f), radius(0.5f), type(WorldObjectType::Empty)
        , value(0.0f), active(true) {}
    
    WorldObject(float px, float py, WorldObjectType t, float v = 0.0f, float r = 0.5f)
        : x(px), y(py), radius(r), type(t), value(v), active(true) {}
};

// Batch operation result structure
struct BatchOperationResult {
    size_t operationsCompleted;
    double totalTime;
    std::vector<double> operationTimes;
    bool success;
    std::string errorMessage;
};

// Performance monitoring for world operations
struct WorldPerformanceStats {
    PerformanceCounter updateCounter;
    PerformanceCounter collisionCheckCounter;
    PerformanceCounter interactionCounter;
    PerformanceCounter visionGenerationCounter;
    Timer avgUpdateTimer;
    Timer avgCollisionTimer;
    Timer avgInteractionTimer;
    Timer avgVisionTimer;
};

// Simple 2D world for NLM development
// Provides sensory experience without semantic labels
class SimpleWorld {
public:
    SimpleWorld();
    ~SimpleWorld();
    
    // Configuration
    void configure(float width, float height, size_t visionWidth, size_t visionHeight);
    
    // ========== BATCH OPERATIONS ==========
    
    // Batch update for multiple time steps
    BatchOperationResult batchUpdate(const std::vector<double>& timesteps,
        bool parallel = true, bool vectorized = true);
    
    // Batch object operations (add/remove/check)
    BatchOperationResult batchAddObjects(const std::vector<WorldObject>& objects,
        bool parallel = true);
    
    BatchOperationResult batchRemoveObjects(const std::vector<std::pair<float, float>>& positions,
        bool parallel = true);
    
    BatchOperationResult batchGetObjectsAt(const std::vector<std::pair<float, float>>& positions,
        bool parallel = true);
    
    // Batch collision checks
    BatchOperationResult batchCheckCollisions(const std::vector<std::pair<float, float>>& positions,
        bool parallel = true);
    
    // Batch vision generation
    BatchOperationResult batchGenerateVision(const std::vector<std::pair<float, float>>& agentStates,
        bool parallel = true);
    
    // Batch motor command processing
    BatchOperationResult batchApplyMotorCommands(const std::vector<MotorCommand>& commands,
        const std::vector<double>& times, bool parallel = true);
    
    // Performance monitoring methods
    void recordPerformanceStat(const std::string& name, double time);
    void recordPerformanceCounter(const std::string& name, size_t count, double timePerOp = 0.0);
    void printPerformanceReport() const;
    void resetPerformanceStats();
    
    // Memory optimization utilities
    void preAllocateMemory();
    void optimizeMemoryAccess();
    size_t getCurrentMemoryUsage() const;
    
    // Progress tracking
    void startLongOperation(const std::string& description, size_t totalSteps);
    void advanceProgress(size_t steps = 1);
    std::string getProgressStatus() const;
    
    // Benchmarking
    struct BenchmarkResult {
        std::string operationName;
        size_t iterations;
        double totalTime;
        double avgTime;
        double minTime;
        double maxTime;
        size_t operationsPerSecond;
    };
    
    BenchmarkResult benchmarkOperation(const std::string& name,
        std::function<void()> operation, size_t iterations = 100);
    
    // SIMD/parallel processing
    static void processObjectsSIMD(const std::vector<WorldObject*>& objects,
        const std::function<void(WorldObject*)>& operation);
    
    static void processAgentsSIMD(const std::vector<AgentBody*>& agents,
        const std::function<void(AgentBody*)>& operation);
    
    // Thread pool for parallel operations
    static ThreadPool& getThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    
    // Cache management for frequently accessed data
    void cacheObjectPositions();
    void clearObjectCache();
    const WorldObject* getCachedObject(float x, float y) const;
    
    // Compressed data operations
    bool compressState(const std::string& filepath, float compressionLevel = 0.5f) const;
    bool decompressState(std::string& filepath, float compressionLevel = 0.5f);
    
    // Enhanced initialization with batch setup
    bool initializeWithBatchSetup(const std::vector<std::pair<float, float>>& agentStarts,
        const std::vector<float>& maxEnergies);
    
    // Original methods (now use performance-enhanced versions)
    void reset();
    void setAgentStart(float x, float y);
    void update(double timestep);
    ActionResult applyMotorCommand(MotorCommand cmd, double currentTime);
    const SensoryPercept& getSensoryPercept() const { return sensory_; }
    const AgentBody& getAgentBody() const { return agent_; }
    void addObject(const WorldObject& obj);
    void removeObject(float x, float y);
    bool isValidPosition(float x, float y) const;
    WorldObject* getObjectAt(float x, float y);
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }
    float getMaxEnergy() const { return maxEnergy_; }
    void setMaxEnergy(float e) { maxEnergy_ = e; }
    float getEnergyDecayRate() const { return energyDecayRate_; }
    void setEnergyDecayRate(float r) { energyDecayRate_ = r; }
    double getSimulationTime() const { return simTime_; }
    void setRandomSeed(uint64_t seed);
    uint64_t getRandomSeed() const { return rngSeed_; }
    
private:
    // Generate vision from current view
    void generateVision();
    
    // Check for collisions
    void checkCollisions();
    
    // Interact with nearby objects
    ActionResult interact();
    
    // World geometry
    float width_;
    float height_;
    
    // Agent
    AgentBody agent_;
    
    // Objects in world
    std::vector<WorldObject> objects_;
    
    // Sensory state
    SensoryPercept sensory_;
    
    // Vision configuration
    size_t visionWidth_;
    size_t visionHeight_;
    
    // Energy system
    float maxEnergy_;
    float energyDecayRate_;
    float resourceEnergyGain_;
    
    // Simulation time
    double simTime_;
    
    // Random seed
    uint64_t rngSeed_;
    
    // Simple RNG state
    uint64_t rngState_;
    uint64_t nextRandom();
    
    // Performance monitoring
    mutable WorldPerformanceStats performanceStats_;
    
    // Progress tracking
    mutable ProgressTracker progressTracker_;
    
    // Memory cache
    mutable std::unordered_map<std::pair<float, float>, WorldObject*> objectCache_;
    mutable std::mutex cacheMutex_;
    
    // Thread pool for batch operations
    static thread_local ThreadPool threadPool_;
    
    // SIMD optimization flags
    static constexpr bool USE_SIMD = true;
    static constexpr bool USE_VECTORIZATION = true;
    static constexpr bool USE_PARALLEL = true;
};

} // namespace nlm
