// BrainProfiler class header
// This file provides the header declaration for the BrainProfiler class

#ifndef BRAIN_PROFILER_HPP
#define BRAIN_PROFILER_HPP

#include <string>
#include <map>
#include <vector>

// NLM includes
#include "../src/advanced_api.hpp"

/**
 * @brief Advanced brain profiling and analysis
 */
class BrainProfiler {
private:
    std::shared_ptr<Brain> brain_;
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point endTime_;
    bool isProfiling_ = false;
    
public:
    BrainProfiler(std::shared_ptr<Brain> brain);
    
    ~BrainProfiler() = default;
    
    // Profile control
    void start();
    void stop();
    bool isProfiling() const;
    double getElapsedTime() const;
    
    // Analysis methods
    std::map<std::string, FloatArray> analyzeSpikeTiming() const;
    std::map<std::string, FloatArray> analyzeNetworkConnectivity() const;
    std::map<std::string, FloatArray> analyzeMemoryUsage() const;
    std::map<std::string, FloatArray> analyzePlasticityDynamics() const;
    std::map<std::string, FloatArray> analyzeNeuromodulation() const;
    
    // Statistical analysis
    double computeCohesion() const;
    double computeModularity() const;
    double computeEfficiency() const;
    double computeSmallWorldness() const;
    
    // Visualization data
    std::string generateHeatmapData() const;
    std::string generateConnectivityMatrix() const;
    std::string generateActivityTimeSeries() const;
    
    // Export
    void exportProfile(const std::string& filepath) const;
    std::string getProfileSummary() const;
};

#endif // BRAIN_PROFILER_HPP
