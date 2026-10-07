// Enhanced Debug and Visualization Interface
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <chrono>

namespace nlm {

class DebugOutput {
public:
    DebugOutput();
    ~DebugOutput();
    
    void initialize(const std::string& configFile = "configs/debug.cfg");
    void update(std::shared_ptr<class Brain> brain);
    void saveToFile(const std::string& filepath) const;
    void printSummary() const;
    
    // Network visualization
    void generateNeuronGraph(const std::string& filename) const;
    void generateConnectivityMatrix(const std::string& filename) const;
    
    // Debug statistics
    void collectNeuronStats(std::shared_ptr<class Brain> brain);
    void collectSynapseStats(std::shared_ptr<class Brain> brain);
    void collectPlasticityStats(std::shared_ptr<class Brain> brain);
    
    // Output formats
    struct NeuronInfo {
        size_t id;
        float potential;
        float threshold;
        float resting;
        float reset;
        float firingRate;
        bool isActive;
        std::string type;
    };
    
    struct SynapseInfo {
        size_t sourceId;
        size_t destId;
        float weight;
        float delay;
        bool isExcitatory;
        bool hebbianPlasticity;
        bool stdpPlasticity;
        float eligibilityTrace;
    };
    
    struct PlasticityInfo {
        std::string type; // "STDP", "Hebbian", "Structural"
        float rate;
        float totalChange;
        float averageChange;
        size_t affectedConnections;
    };
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    void initialize(const std::string& configFile = "configs/viz.cfg");
    void renderNetwork(std::shared_ptr<class Brain> brain);
    void saveFrame(const std::string& filename);
    void animateNetwork(const std::string& outputDir, int duration = 100);
    
    // Rendering options
    void setColorScheme(const std::string& scheme);
    void setRenderMode(const std::string& mode);
    void setOutputSize(int width, int height);
    
    // Network analysis
    void analyzeNetworkProperties(std::shared_ptr<class Brain> brain);
    void detectCommunities(std::shared_ptr<class Brain> brain);
    void identifyHubNeurons(std::shared_ptr<class Brain> brain);
    
    // Export capabilities
    void exportForMatlab(const std::string& filename) const;
    void exportForPython(const std::string& filename) const;
    void exportForMAT(const std::string& filename) const;
    
    // Statistics and metrics
    struct NetworkMetrics {
        size_t neuronCount;
        size_t synapseCount;
        float connectionDensity;
        float averageFiringRate;
        float averageWeight;
        float excitationInhibitionRatio;
        float clusteringCoefficient;
        float pathLength;
        size_t maxDegree;
        size_t minDegree;
    };
    
    NetworkMetrics computeMetrics(std::shared_ptr<class Brain> brain) const;
    
    // Activity analysis
    void analyzeSpikePatterns(std::shared_ptr<class Brain> brain);
    void analyzeSynapseDynamics(std::shared_ptr<class Brain> brain);
    void analyzeLearningTrajectories(std::shared_ptr<class Brain> brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class BrainDebugger {
public:
    BrainDebugger();
    ~BrainDebugger();
    
    void attach(std::shared_ptr<class Brain> brain);
    void step(int steps = 1);
    void pause();
    void resume();
    void reset();
    
    // Breakpoints and debugging
    void setBreakpoint(size_t step);
    void clearBreakpoints();
    void enableDebugger(bool enable);
    
    // Memory inspection
    void inspectNeuron(size_t neuronId);
    void inspectSynapse(size_t synapseId);
    void inspectMemoryUsage();
    
    // Performance analysis
    void profileCurrentStep();
    void analyzeMemoryAllocation();
    void measureNetworkMetrics();
    
    // State capture and replay
    void captureState(const std::string& filename);
    void replayState(const std::string& filename);
    void generateReplayScript(const std::string& filename);
    
    // Integration with logging
    void enableLogFiltering(const std::string& filterLevel);
    void enablePerformanceLogging(const std::string& outputFile);
    
    // Advanced debugging features
    void enableNeuralEye(const std::vector<size_t>& neuronIds);
    void enableSynapseTracking(const std::vector<size_t>& synapseIds);
    void enablePlasticityMonitoring(const std::string& plasticityType);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Utility functions for debugging
namespace DebugUtils {
    // File operations
    bool writeToFile(const std::string& filename, const std::string& content);
    bool readFromFile(const std::string& filename, std::string& content);
    
    // Network analysis
    float computeNetworkDensity(size_t neuronCount, size_t synapseCount);
    float computeAverageDegree(const std::vector<size_t>& degrees);
    float computeClusteringCoefficient(const std::vector<size_t>& neighbors);
    
    // Data formatting
    std::string formatFloat(float value, int precision = 3);
    std::string formatTime(double seconds);
    std::string formatBytes(size_t bytes);
    
    // Export utilities
    void exportToCSV(const std::string& filename, const std::vector<std::vector<float>>& data);
    void exportToJSON(const std::string& filename, const std::map<std::string, std::string>& data);
    void exportToMatlab(const std::string& filename, const std::vector<std::vector<double>>& data);
}

} // namespace nlm
