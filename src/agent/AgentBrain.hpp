#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Performance/PerformanceMonitoring.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <unordered_map>
#include <chrono>

namespace nlm {

// Batch operation result structure
struct BatchOperationResult {
    size_t operationsCompleted;
    double totalTime;
    std::vector<double> operationTimes;
    bool success;
    std::string errorMessage;
};

// Performance monitoring for agent brain operations
struct AgentBrainPerformanceStats {
    PerformanceCounter sensoryProcessingCounter;
    PerformanceCounter motorDecodingCounter;
    PerformanceCounter rewardModulationCounter;
    PerformanceCounter developmentUpdateCounter;
    Timer avgSensoryProcessingTimer;
    Timer avgMotorDecodingTimer;
    Timer avgRewardModulationTimer;
    Timer avgDevelopmentUpdateTimer;
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // ========== BATCH OPERATIONS ==========
    
    // Batch sensory processing
    BatchOperationResult batchProcessSensory(const std::vector<const SensoryPercept*>& percepts,
        bool parallel = true, bool vectorized = true);
    
    // Batch command decoding
    std::vector<MotorCommand> batchDecodeCommands(size_t count = 1,
        bool parallel = true, bool vectorized = true);
    
    // Batch reward modulation
    BatchOperationResult batchApplyRewardModulation(const std::vector<std::pair<float, float>>& rewards,
        bool parallel = true);
    
    // Batch development updates
    BatchOperationResult batchUpdateDevelopment(const std::vector<double>& timesteps,
        bool parallel = true);
    
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
    static void processSensoryPerceptSIMD(const std::vector<const SensoryPercept*>& percepts,
        const std::function<void(const SensoryPercept*)>& operation);
    
    static void processMotorCommandSIMD(const std::vector<MotorCommand>& commands,
        const std::function<void(MotorCommand)>& operation);
    
    // Thread pool for parallel operations
    static ThreadPool& getThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    
    // Cache management for frequently accessed data
    void cacheSensoryData(const SensoryPercept& percept);
    void clearSensoryCache();
    const SensoryPercept* getCachedSensory(size_t index) const;
    
    // Compressed data operations
    bool compressState(const std::string& filepath, float compressionLevel = 0.5f) const;
    bool decompressState(std::string& filepath, float compressionLevel = 0.5f);
    
    // Enhanced initialization with batch setup
    bool initializeWithBatchSetup(const std::vector<const SimpleWorld*>& worlds,
        const std::vector<size_t>& perceptSizes);
    
    // Original methods (now use performance-enhanced versions)
    size_t getSensoryInputSize() const;
    size_t getMotorOutputSize() const;
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    DevelopmentalStage getDevelopmentalStage() const;
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper methods for sensory processing
    void processVisionInput(const std::vector<float>& vision);
    void processTouchInput(const std::vector<float>& touch);
    void processInternalInput(const std::vector<float>& internal);
    void processProprioceptionInput(const std::vector<float>& proprio);
    
    // Helper methods for development
    DevelopmentalStage getDevelopmentalStageFromAge(double age) const;
    float getPlasticityFromAge(double age) const;
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Performance monitoring
    mutable AgentBrainPerformanceStats performanceStats_;
    
    // Progress tracking
    mutable ProgressTracker progressTracker_;
    
    // Memory cache
    mutable std::unordered_map<size_t, const SensoryPercept*> sensoryCache_;
    mutable std::mutex cacheMutex_;
    
    // Thread pool for batch operations
    static thread_local ThreadPool threadPool_;
    
    // SIMD optimization flags
    static constexpr bool USE_SIMD = true;
    static constexpr bool USE_VECTORIZATION = true;
    static constexpr bool USE_PARALLEL = true;
};

} // namespace nlm
