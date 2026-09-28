#pragma once

#include "OptimizedAgentBrain.hpp"
#include "OptimizedNeuralWorkingMemory.hpp"
#include "OptimizedNeuromodulator.cpp"
#include "OptimizedNeuralPrediction.hpp"
#include "../core/Config/Config.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/EventQueue.hpp"
#include "../performance/ParallelProcessing.hpp"
#include <memory>
#include <chrono>

namespace nlm {

// Phase 6 Integration: Optimized Brain System
// Integrates all optimized subsystems with the Performance framework
class OptimizedBrainIntegration {
public:
    OptimizedBrainIntegration(std::shared_ptr<Config> config);
    ~OptimizedBrainIntegration();
    
    // Initialize all optimized subsystems
    bool initialize();
    
    // Main simulation step using optimized processing
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Optimized sensory processing with event-driven approach
    void processSensoryInputOptimized(const SensoryInput& input);
    
    // Optimized action production with prediction integration
    std::unique_ptr<Action> produceActionOptimized();
    
    // Optimized neuromodulation with smooth transitions
    void applyNeuromodulationOptimized(const Neuromodulator& signal);
    
    // Optimized plasticity updates
    void updatePlasticityOptimized();
    
    // Optimized development updates
    void developOptimized();
    
    // Optimized checkpointing with performance metrics
    bool saveOptimized(const std::string& filepath) const;
    bool loadOptimized(const std::string& filepath);
    
    // Performance monitoring and statistics
    struct IntegrationPerformanceStats {
        double simulationSpeed;        // Steps per second
        double memoryEfficiency;       // Bytes per neuron
        double cpuUtilization;         // Percentage
        double accuracy;               // Prediction accuracy
        size_t activeNeurons;          // Current active neuron count
        size_t totalSpikes;            // Total spikes generated
        float memoryUsageMB;           // Current memory usage
        double stepTimeMs;             // Average step time
        size_t cacheHits;              // Cache hit count
        size_t cacheMisses;            // Cache miss count
        float neuromodulatorStability; // Neuromodulator level stability
        double predictionError;       // Average prediction error
    };
    
    IntegrationPerformanceStats getPerformanceStats() const;
    
    // Get optimized subsystems
    OptimizedAgentBrain* getAgentBrain() { return agentBrain_.get(); }
    OptimizedNeuralWorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    OptimizedNeuromodulator* getNeuromodulator() { return neuromodulator_.get(); }
    OptimizedNeuralPrediction* getPredictionSystem() { return predictionSystem_.get(); }
    
    // Optimization controls
    void enableEventDriven(bool enable) { eventDrivenEnabled_ = enable; }
    void enableSIMD(bool enable) { simdEnabled_ = enable; }
    void enableMultithreading(bool enable) { multithreadingEnabled_ = enable; }
    void enableCaching(bool enable) { cachingEnabled_ = enable; }
    
    bool isEventDrivenEnabled() const { return eventDrivenEnabled_; }
    bool isSIMDEnabled() const { return simdEnabled_; }
    bool isMultithreadingEnabled() const { return multithreadingEnabled_; }
    bool isCachingEnabled() const { return cachingEnabled_; }
    
    // Performance tuning
    void setThreadCount(size_t count) { threadCount_ = count; }
    void setCacheSize(size_t size) { 
        if (predictionSystem_) predictionSystem_->setCacheSize(size);
    }
    size_t getThreadCount() const { return threadCount_; }
    
    // Memory pool management
    void reserveMemoryPools(size_t neurons, size_t synapses);
    size_t getMemoryUsage() const;
    
private:
    // Core optimized subsystems
    std::unique_ptr<OptimizedAgentBrain> agentBrain_;
    std::unique_ptr<OptimizedNeuralWorkingMemory> workingMemory_;
    std::unique_ptr<OptimizedNeuromodulator> neuromodulator_;
    std::unique_ptr<OptimizedNeuralPrediction> predictionSystem_;
    
    // Performance framework integration
    std::unique_ptr<MemoryPool<NeuronActivation>> neuronPool_;
    std::unique_ptr<MemoryPool<float>> activationPool_;
    std::unique_ptr<SpikeRingBuffer> spikeQueue_;
    std::unique_ptr<ParallelNeuralProcessor> parallelProcessor_;
    
    // Event-driven processing
    bool eventDrivenEnabled_;
    bool simdEnabled_;
    bool multithreadingEnabled_;
    bool cachingEnabled_;
    size_t threadCount_;
    
    // Simulation state
    SimulationStep lastStep_;
    Timestamp lastTime_;
    double totalSimulationTime_;
    size_t stepCount_;
    
    // Performance monitoring
    mutable IntegrationPerformanceStats perfStats_;
    std::chrono::high_resolution_clock::time_point lastStepTime_;
    
    // Optimized integration methods
    void integrateWithPerformanceFramework();
    void setupEventDrivenProcessing();
    void setupParallelProcessing();
    void setupMemoryPools();
    
    // Batch processing optimizations
    void batchProcessEvents();
    void batchProcessNeurons();
    void batchProcessSynapses();
    
    // Cache management
    void updateCacheEfficiency();
    void optimizeMemoryLayout();
    
    // Statistics and monitoring
    void updatePerformanceMetrics();
    void logPerformanceStats() const;
    
    // Validation and verification
    bool validateIntegration() const;
    bool benchmarkPerformance() const;
    
    // Disable copying
    OptimizedBrainIntegration(const OptimizedBrainIntegration&) = delete;
    OptimizedBrainIntegration& operator=(const OptimizedBrainIntegration&) = delete;
};

} // namespace nlm