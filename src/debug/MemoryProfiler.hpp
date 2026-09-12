#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../performance/Performance.hpp"

namespace nlm {

class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Memory analysis types
    enum class AnalysisType {
        WorkingMemory,
        EpisodicMemory,
        AssociativeMemory,
        NeuralState,
        SynapticWeights,
        SpikeHistory,
        All
    };
    
    // Initialize profiler
    bool initialize(AnalysisType type = AnalysisType::All);
    
    // Core profiling methods
    void profileBrain(const Brain& brain);
    void profileWorkingMemory(const Brain& brain);
    void profileEpisodicMemory(const Brain& brain);
    void profileAssociativeMemory(const Brain& brain);
    void profileNeuralState(const Brain& brain);
    void profileSynapticWeights(const Brain& brain);
    void profileSpikeHistory(const Brain& brain);
    
    // Memory analysis
    void analyzeMemoryLeaks(const Brain& brain);
    void analyzeMemoryFragmentation(const Brain& brain);
    void analyzeMemoryEfficiency(const Brain& brain);
    void analyzeMemoryCorruption(const Brain& brain);
    
    // Performance profiling
    void profilePerformance(const Brain& brain);
    void analyzeAllocationPatterns(const Brain& brain);
    void identifyMemoryBottlenecks(const Brain& brain);
    
    // Get profiling results
    const std::string& getProfileReport() const;
    double getProfileProgress() const;
    bool isProfileComplete() const;
    
    // Memory metrics
    struct MemoryMetrics {
        size_t totalMemoryUsage;
        size_t workingMemoryUsage;
        size_t episodicMemoryUsage;
        size_t associativeMemoryUsage;
        double memoryEfficiency;
        size_t memoryBlocks;
        size_t fragments;
        bool corruptionDetected;
        double fragmentationRatio;
    };
    
    const MemoryMetrics& getMemoryMetrics() const;
    
    // Performance metrics
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        size_t activeNeurons;
        size_t activeSynapses;
        double simulationSpeed;
        std::chrono::duration<double> profilingTime;
    };
    
    const PerformanceMetrics& getPerformanceMetrics() const;
    
    // Configuration
    void setAnalysisType(AnalysisType type);
    void setMemoryThreshold(double threshold); // 0.0 to 1.0
    void setPerformanceThreshold(double threshold); // 0.0 to 1.0
    void enableRealTimeAnalysis(bool enable);
    void setAnalysisInterval(double interval); // seconds
    
    // Callbacks
    using ProfileCompleteCallback = std::function<void(const std::string& report, double progress)>;
    void setProfileCompleteCallback(ProfileCompleteCallback callback);
    void setAnomalyDetectedCallback(std::function<void(const std::string& anomaly)> callback);
    
    // Export and analysis
    bool exportProfileReport(const std::string& filename) const;
    bool generateMemoryAnalysisReport(const std::string& filename) const;
    std::vector<std::string> getDetectedAnomalies() const;
    
    // Optimization
    void optimizeMemoryLayout(const Brain& brain);
    void suggestMemoryPooling(const Brain& brain);
    void recommendCompression(const Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
