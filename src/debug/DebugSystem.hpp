// DebugSystem header - contains declarations for the debug system
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

using namespace std;

namespace nlm {

// Forward declarations
class Brain;
class SpikeEvent;

// Debug level for controlling verbosity
enum class DebugLevel {
    BASIC,
    INTERMEDIATE,
    ADVANCED,
    EXPERT,
    DEVELOPMENTAL
};

// Core debugging system for NLM
class DebugSystem {
public:
    DebugSystem();
    ~DebugSystem();
    
    // Initialize with brain reference and configuration
    bool initialize(shared_ptr<Brain> brain, const string& configFile = "");
    
    // Debugging control
    void setDebugLevel(DebugLevel level);
    void enableFeature(const string& feature, bool enable);
    void setBreakpoint(const string& condition);
    void addCustomAnalyzer(const string& name, function<void()> analyzer);
    
    // Monitoring control
    void startMonitoring(double interval = 0.01);
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Core analysis functions
    void analyzeNeuronBehavior(int neuronId);
    void analyzeSynapseDynamics(int synapseId);
    void analyzeNetworkTopology();
    void analyzePlasticityDynamics();
    void analyzeMemorySystemState();
    void analyzeNeuromodulationState();
    
    // Data collection
    void collectData(const string& category, const string& key, double value);
    void collectNeuronData(const struct NeuronData& neuron);
    void collectSynapseData(const struct SynapseData& synapse);
    void collectSpikeData(const struct SpikeData& spike);
    
    // Analysis methods
    void runPatternAnalysis();
    void runTemporalAnalysis();
    void runStatisticalAnalysis();
    void runAnomalyDetection();
    
    // Reporting
    string generateReport() const;
    void exportReport(const string& filepath, const string& format = "text");
    void showConsoleOutput(bool show);
    
    // Configuration
    void setLogLevel(const string& level);
    void setOutputDirectory(const string& dir);
    void setReportFormat(const string& format);
    
    // Advanced features
    void enableMemoryProfiling(bool enable);
    void enablePerformanceProfiling(bool enable);
    void enableEventTracing(bool enable);
    void enableCustomMetrics(const string& name, function<double()> metric);
    
private:
    struct Impl;
    unique_ptr<Impl> pImpl;
};

} // namespace nlm
