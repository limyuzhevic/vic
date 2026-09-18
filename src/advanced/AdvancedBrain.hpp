#pragma once

/**
 * AdvancedBrain - Enhanced brain class with advanced control capabilities
 * 
 * This class provides enhanced functionality for expert users while
 * maintaining compatibility with the basic Brain API.
 */

#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/Performance.hpp"
#include "../visualization/VisualizationInterface.hpp"
#include "../experiments/Experiment.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>

namespace nlm {

/**
 * AdvancedBrain configuration options for expert control
 */
struct AdvancedBrainConfig {
    // Neural network configuration
    uint64_t neuronCount;
    uint64_t regionCount;
    float connectionProbability;
    bool enableDevelopment;
    bool enableCuriosity;
    bool enableRewardModulation;
    bool enableStructuralPlasticity;
    
    // Simulation control
    uint64_t maxSteps;
    double timestep;
    std::string checkpointPath;
    
    // Performance optimization
    bool enableSIMD;
    bool enableMultithreading;
    bool enableMemoryPool;
    bool enableSparseConnectivity;
    bool enableEventQueue;
    
    // Visualization and debugging
    bool enableVisualization;
    double visualizationUpdateRate;
    bool enableDetailedLogging;
    std::string logLevel;
    
    // Batch processing
    uint64_t batchSize;
    uint64_t batchCount;
    
    // Expert mode settings
    bool expertMode;
    bool enableDebugInjection;
    std::vector<std::string> customPlasticityRules;
    
    AdvancedBrainConfig() 
        : neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableDevelopment(true)
        , enableCuriosity(true)
        , enableRewardModulation(true)
        , enableStructuralPlasticity(true)
        , maxSteps(10000)
        , timestep(0.001)
        , checkpointPath("./advanced_brain_checkpoint.bin")
        , enableSIMD(true)
        , enableMultithreading(false)
        , enableMemoryPool(true)
        , enableSparseConnectivity(true)
        , enableEventQueue(true)
        , enableVisualization(false)
        , visualizationUpdateRate(10.0)
        , enableDetailedLogging(true)
        , logLevel("INFO")
        , batchSize(10)
        , batchCount(1)
        , expertMode(false)
        , enableDebugInjection(false) {}
};

/**
 * AdvancedBrain - Enhanced brain interface for expert control
 * 
 * Provides advanced control options including:
 * - Performance monitoring and profiling
 * - Advanced configuration management
 * - Batch processing utilities
 * - Visualization and debugging tools
 * - Expert mode with detailed control
 * - Performance optimization controls
 */
class AdvancedBrain {
public:
    AdvancedBrain(const AdvancedBrainConfig& config);
    ~AdvancedBrain();
    
    // Initialize the advanced brain
    bool initialize();
    
    // Main simulation with advanced options
    void step(uint64_t currentStep);
    void step(uint64_t currentStep, double currentTime);
    
    // Enhanced control methods
    
    // Performance and profiling
    void enableProfiler(bool enable);
    double getCurrentFPS() const;
    uint64_t getTotalSpikesProcessed() const;
    double getAverageMemoryUsage() const;
    
    // Advanced configuration
    void setConfig(const AdvancedBrainConfig& config);
    AdvancedBrainConfig getConfig() const;
    void updateConfigValue(const std::string& key, const std::string& value);
    void applyConfigPresets(const std::string& presetName);
    
    // Visualization and debugging
    void enableVisualizer(bool enable, double updateRate = 10.0);
    void updateVisualizer();
    std::string exportNetworkGraphViz(const std::string& format = "dot") const;
    std::vector<float> getNeuralActivityMap() const;
    
    // Expert control methods
    void enableExpertMode(bool enable);
    void injectDebugCurrent(NeuronId neuron, float current);
    void injectDebugCurrentToType(NeuronType type, float current);
    void enableDebugInjection(bool enable);
    
    // Batch processing
    void startBatchProcessing(uint64_t batchCount, uint64_t batchSize);
    bool isInBatchProcessing() const;
    uint64_t getCurrentBatchIndex() const;
    uint64_t getCurrentBatchSize() const;
    double getBatchProgress() const;
    
    // Serialization and checkpointing with advanced options
    bool saveWithTimestamp(const std::string& name = "");
    bool loadWithTimestamp(const std::string& name);
    std::vector<std::string> listCheckpoints() const;
    bool deleteCheckpoint(const std::string& name);
    
    // Performance optimization controls
    void enableSIMD(bool enable);
    void enableMultithreading(bool enable, uint64_t threadCount = 0);
    void enableMemoryPool(bool enable);
    void enableSparseConnectivity(bool enable);
    void enableEventQueue(bool enable);
    
    // Neuromodulation and development control
    void setDevelopmentRate(float rate);
    void setPlasticityStrength(float strength);
    void enableStructuralPlasticity(bool enable);
    void enableRewardModulation(bool enable);
    void enableCuriosity(bool enable);
    
    // Statistics and metrics
    struct AdvancedMetrics {
        // Performance metrics
        double fps;
        double avgStepTime;
        double memoryUsageMB;
        uint64_t totalSpikes;
        uint64_t activeNeurons;
        
        // Neural activity metrics
        float avgFiringRate;
        float excitationInhibitionRatio;
        float totalPlasticityEvents;
        
        // System metrics
        double simulationTime;
        float energyLevel;
        float healthLevel;
        float curiosityLevel;
        
        // Memory metrics
        size_t workingMemorySize;
        size_t episodicMemoryCount;
        
        // Development metrics
        DevelopmentalStage developmentalStage;
        double developmentProgress;
        
        // Debug metrics
        uint64_t debugInjections;
        uint64_t visualizationFrames;
    };
    
    AdvancedMetrics getMetrics() const;
    
    // Callback system for expert control
    void registerStepCallback(std::function<void(AdvancedBrain&, uint64_t)> callback);
    void registerPreStepCallback(std::function<void(AdvancedBrain&, uint64_t)> callback);
    void registerPostStepCallback(std::function<void(AdvancedBrain&, uint64_t)> callback);
    void clearCallbacks();
    
    // Debug and diagnostic tools
    std::string getBrainStateSummary() const;
    std::vector<std::string> getDebugInfo() const;
    bool validateSystemHealth() const;
    std::vector<std::string> getSystemWarnings() const;
    
    // Backward compatibility
    std::shared_ptr<Brain> getBrain() const { return brain_.get(); }
    bool isInitialized() const { return isInitialized_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool isInitialized_;
    
    // Internal helper methods
    void updatePerformanceMetrics() const;
    void handleBatchProcessing() const;
    void processCallbacks(uint64_t step) const;
};

} // namespace nlm
