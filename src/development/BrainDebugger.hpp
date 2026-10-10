#pragma once

#include "Brain.hpp"
#include "AgentBrain.hpp"
#include "Phase6IntegratedExperiment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>

namespace nlm {

class BrainDebugger {
public:
    BrainDebugger(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agentBrain,
                  std::shared_ptr<Phase6IntegratedExperiment> experiment);
    ~BrainDebugger();
    
    // Disable copying, enable moving
    BrainDebugger(const BrainDebugger&) = delete;
    BrainDebugger& operator=(const BrainDebugger&) = delete;
    BrainDebugger(BrainDebugger&&) noexcept = default;
    BrainDebugger& operator=(BrainDebugger&&) noexcept = default;
    
    // Brain analysis and inspection
    struct BrainAnalysis {
        // Structural analysis
        size_t totalNeuronCount;
        size_t totalSynapseCount;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        float averageFiringRate;
        float excitationInhibitionRatio;
        
        // Regional analysis
        std::vector<std::string> regionNames;
        std::vector<size_t> neuronCountsPerRegion;
        std::vector<float> firingRatesPerRegion;
        
        // Network properties
        float averageSynapseWeight;
        float weightDistribution[10]; // binned distribution
        float connectionDensity;
        
        // State information
        bool isBrainInitialized;
        bool isBrainActive;
        SimulationStep currentStep;
        double currentTime;
        
        BrainAnalysis()
            : totalNeuronCount(0), totalSynapseCount(0), activeNeuronCount(0), firingNeuronCount(0),
              averageFiringRate(0.0f), excitationInhibitionRatio(1.0f), connectionDensity(0.0f),
              isBrainInitialized(false), isBrainActive(false), currentStep(0), currentTime(0.0) {
            std::fill(std::begin(weightDistribution), std::end(weightDistribution), 0.0f);
        }
    };
    
    struct NeuronAnalysis {
        NeuronId id;
        NeuronType type;
        std::string regionName;
        float membranePotential;
        float restingPotential;
        float inputCurrent;
        float outputCurrent;
        float firingRate;
        float activationLevel;
        float connectivityScore;
        std::vector<SynapseId> incomingConnections;
        std::vector<SynapseId> outgoingConnections;
        bool isActive;
        bool isFiring;
    };
    
    struct SynapseAnalysis {
        SynapseId id;
        NeuronId preNeuron;
        NeuronId postNeuron;
        std::string preRegion;
        std::string postRegion;
        float weight;
        float delay;
        float eligibilityTrace;
        float contributionToOutput;
        bool isPlastic;
        float plasticityRate;
    };
    
    struct MemoryAnalysis {
        // Working memory
        size_t workingMemorySize;
        size_t workingMemoryCapacity;
        float workingMemoryUtilization;
        
        // Episodic memory
        size_t episodicMemoryCount;
        size_t episodicMemoryCapacity;
        
        // Semantic memory
        size_t semanticMemorySize;
        size_t semanticMemoryCapacity;
        
        // Procedural memory
        size_t proceduralMemorySize;
        size_t proceduralMemoryCapacity;
        
        // Associative memory
        size_t associativeMemorySize;
        size_t associativeMemoryCapacity;
        
        // Memory statistics
        float memoryAccessTime;
        float memoryRetentionTime;
        float memoryRetrievalAccuracy;
    };
    
    struct PerformanceMetrics {
        double fps;
        double frameTime;
        size_t memoryUsage;
        size_t cpuUsagePercent;
        float neuralActivityLevel;
        float learningProgress;
        float stabilityIndex;
        float efficiencyScore;
        uint64_t totalSpikeCount;
        float averageSpikeInterval;
        float peakFiringRate;
    };
    
    // Main debugging interface
    void analyzeBrain();
    bool initializeAnalysis();
    void shutdownAnalysis();
    
    // Brain state inspection
    BrainAnalysis getBrainAnalysis() const;
    NeuronAnalysis inspectNeuron(NeuronId id) const;
    SynapseAnalysis inspectSynapse(SynapseId id) const;
    MemoryAnalysis analyzeMemorySystems() const;
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Connectivity analysis
    void exploreConnections();
    std::vector<NeuronId> getConnectedNeurons(NeuronId id) const;
    std::vector<SynapseId> getOutgoingConnections(NeuronId id) const;
    std::vector<SynapseId> getIncomingConnections(NeuronId id) const;
    float calculateConnectivityScore(NeuronId id) const;
    
    // Configuration examination
    void examineConfiguration() const;
    std::string getBrainConfiguration() const;
    void printBrainConfiguration() const;
    
    // Visualization and export
    void generateDebugReport(const std::string& filename = "debug_report.txt");
    void exportBrainState(const std::string& filename);
    void exportAnalysisData(const std::string& filename);
    
    // Real-time monitoring
    void startRealTimeMonitoring(unsigned int updateIntervalMs = 100);
    void stopRealTimeMonitoring();
    bool isMonitoring() const;
    void setMonitoringCallback(std::function<void(const BrainAnalysis&)> callback);
    
    // Parameter validation
    bool validateBrainState() const;
    std::vector<std::string> getValidationErrors() const;
    bool validateConnectivity() const;
    bool validateMemorySystems() const;
    bool validatePerformance() const;
    
    // Simulation control
    void pauseBrain() const;
    void resumeBrain() const;
    void stepBrain() const;
    bool isBrainPaused() const;
    
    // Data collection
    void startDataCollection(const std::string& dataFile);
    void stopDataCollection();
    void collectDataPoint();
    bool isCollectingData() const;
    
    // Configuration
    void setAnalysisInterval(double interval) { analysisInterval_ = interval; }
    double getAnalysisInterval() const { return analysisInterval_; }
    void setMaxNeuronsToAnalyze(size_t max) { maxNeuronsToAnalyze_ = max; }
    size_t getMaxNeuronsToAnalyze() const { return maxNeuronsToAnalyze_; }
    void enableRealTimeUpdates(bool enable) { realTimeUpdatesEnabled_ = enable; }
    bool isRealTimeUpdatesEnabled() const { return realTimeUpdatesEnabled_; }
    
    // Thread safety
    void lockAnalysis() const;
    void unlockAnalysis() const;
    bool tryLockAnalysis() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<Phase6IntegratedExperiment> experiment_;
    std::shared_ptr<Config> config_;
    
    mutable std::mutex analysisMutex_;
    std::atomic<bool> isMonitoring_;
    std::atomic<bool> isCollectingData_;
    std::atomic<bool> isPaused_;
    double analysisInterval_;
    size_t maxNeuronsToAnalyze_;
    bool realTimeUpdatesEnabled_;
    std::thread monitoringThread_;
    std::function<void(const BrainAnalysis&)> monitoringCallback_;
    std::string dataCollectionFile_;
    std::ofstream dataFile_;
    
    // Internal helper methods
    void performAnalysis();
    void updateRealTimeData();
    void saveAnalysisData();
    void cleanup();
    BrainAnalysis performBrainAnalysis() const;
    void calculateWeightDistribution(const BrainAnalysis& analysis) const;
};