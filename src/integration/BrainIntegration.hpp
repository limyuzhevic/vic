#pragma once

#include "Brain.hpp"
#include "AgentBrain.hpp"
#include "Memory.hpp"
#include "PredictionSystem.hpp"
#include "Neuromodulator.hpp"
#include "DevelopmentSystem.hpp"
#include <memory>
#include <chrono>

namespace nlm {

// Optimized Brain Integration for Phase 6
// Provides optimized interfaces to existing brain subsystems
class BrainIntegration {
public:
    BrainIntegration(std::shared_ptr<Brain> brain);
    ~BrainIntegration();
    
    // Initialize optimized integration
    bool initialize();
    
    // Main simulation step with optimizations
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Optimized sensory processing
    void receiveSensoryInput(const SensoryInput& input);
    
    // Optimized action production
    std::unique_ptr<Action> produceAction();
    
    // Optimized neuromodulation
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // Optimized plasticity updates
    void updatePlasticity();
    
    // Optimized development
    void develop();
    
    // Optimized checkpointing
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Get optimized references to subsystems
    AgentBrain* getAgentBrain() { return agentBrain_.get(); }
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    Neuromodulator* getNeuromodulator() { return neuromodulator_.get(); }
    PredictionSystem* getPredictionSystem() { return predictionSystem_.get(); }
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem_.get(); }
    
    // Performance monitoring
    struct IntegrationStats {
        double simulationSpeed;        // Steps per second
        double memoryUsageMB;          // Current memory usage
        double cpuUtilization;         // Approximate CPU usage
        size_t activeNeurons;          // Current active neuron count
        size_t totalSpikes;            // Total spikes generated
        float avgFiringRate;           // Average firing rate
        float predictionAccuracy;     // Prediction accuracy
        float neuromodulatorStability; // Neuromodulator level stability
        size_t cacheHits;              // Cache hit count
        size_t cacheMisses;            // Cache miss count
        double stepTimeMs;             // Average step time
        bool optimizationsEnabled;     // Whether optimizations are active
    };
    
    IntegrationStats getStats() const;
    
    // Optimization controls
    void enableOptimizations(bool enable) { optimizationsEnabled_ = enable; }
    bool areOptimizationsEnabled() const { return optimizationsEnabled_; }
    
    void enableSIMD(bool enable) { simdEnabled_ = enable; }
    bool isSIMDEnabled() const { return simdEnabled_; }
    
    void enableCaching(bool enable) { cachingEnabled_ = enable; }
    bool isCachingEnabled() const { return cachingEnabled_; }
    
    void enableMultithreading(bool enable) { multithreadingEnabled_ = enable; }
    bool isMultithreadingEnabled() const { return multithreadingEnabled_; }
    
    // Memory pool management
    void reserveMemory(size_t neurons, size_t synapses);
    size_t getMemoryUsage() const;
    
    // Validation and benchmarking
    bool validatePerformance() const;
    void logPerformanceStats() const;
    
private:
    // Core brain
    std::shared_ptr<Brain> brain_;
    
    // Optimized subsystems
    std::unique_ptr<AgentBrain> agentBrain_;
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<Neuromodulator> neuromodulator_;
    std::unique_ptr<PredictionSystem> predictionSystem_;
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
    
    // Performance framework integration
    struct PerformanceState {
        SimulationStep lastStep;
        Timestamp lastTime;
        double totalSimulationTime;
        size_t stepCount;
        double totalStepTime;
        
        size_t cacheHits;
        size_t cacheMisses;
        
        size_t neuronPoolSize;
        size_t synapsePoolSize;
    } perfState_;
    
    // Optimization flags
    bool optimizationsEnabled_;
    bool simdEnabled_;
    bool cachingEnabled_;
    bool multithreadingEnabled_;
    
    // Performance monitoring
    mutable IntegrationStats stats_;
    
    // Integration methods
    void setupOptimizedSubsystems();
    void setupPerformanceMonitoring();
    void integrateWithBrainSystems();
    
    // Optimized processing methods
    void optimizedStep(SimulationStep currentStep, Timestamp currentTime);
    void optimizedNeuromodulation();
    void optimizedPlasticity();
    void optimizedDevelopment();
    
    // Performance monitoring
    void updatePerformanceMetrics();
    void calculateCacheEfficiency();
    void estimateMemoryUsage() const;
    
    // Statistics and logging
    void logIntegrationStats() const;
    
    // Disable copying
    BrainIntegration(const BrainIntegration&) = delete;
    BrainIntegration& operator=(const BrainIntegration&) = delete;
};

} // namespace nlm