#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include "../brain/Brain.hpp"

namespace nlm {
namespace debug {

// Neuron state information for debugging
struct NeuronDebugInfo {
    NeuronId id;
    NeuronType type;
    float membranePotential;
    float threshold;
    float restingPotential;
    float resetPotential;
    float leakConductance;
    float synapticConductance;
    float firingRate;
    FiringState firingState;
    uint32_t refractoryRemaining;
    uint32_t refractoryPeriod;
    float adaptationVariable;
    float lastSpikeTime;
    size_t synapseCount;
    bool plasticityEnabledHebbian;
    bool plasticityEnabledStdp;
    bool plasticityEnabledReward;
    
    NeuronDebugInfo() : id(0), type(NeuronType::Internal),
                       membranePotential(0.0f), threshold(0.0f),
                       restingPotential(0.0f), resetPotential(0.0f),
                       leakConductance(0.0f), synapticConductance(0.0f),
                       firingRate(0.0f), firingState(FiringState::Resting),
                       refractoryRemaining(0), refractoryPeriod(0),
                       adaptationVariable(0.0f), lastSpikeTime(-1.0f),
                       synapseCount(0), plasticityEnabledHebbian(false),
                       plasticityEnabledStdp(false), plasticityEnabledReward(false) {}
};

// Synapse state information for debugging
struct SynapseDebugInfo {
    SynapseId id;
    NeuronId preId;
    NeuronId postId;
    float weight;
    Delay delay;
    bool enabled;
    bool plasticityHebbian;
    bool plasticityStdp;
    bool plasticityReward;
    float eligibilityTrace;
    float presynapticSpikeHistory;
    float postsynapticSpikeHistory;
    float lastUpdateTime;
    
    SynapseDebugInfo() : id(0), preId(0), postId(0), weight(0.0f), delay(1),
                        enabled(false), plasticityHebbian(false),
                        plasticityStdp(false), plasticityReward(false),
                        eligibilityTrace(0.0f), presynapticSpikeHistory(0.0f),
                        postsynapticSpikeHistory(0.0f), lastUpdateTime(0.0f) {}
};

// Brain system state information for debugging
struct BrainSystemDebugInfo {
    std::string systemName;
    std::string systemType;
    std::map<std::string, std::string> parameters;
    std::vector<std::string> statusMessages;
    std::chrono::high_resolution_clock::time_point lastUpdate;
    bool active;
    
    BrainSystemDebugInfo(const std::string& name, const std::string& type)
        : systemName(name), systemType(type), lastUpdate(std::chrono::high_resolution_clock::now()), active(true) {}
};

// Debug configuration
struct DebugConfig {
    bool enableNeuronDebugging;
    bool enableSynapseDebugging;
    bool enableSystemDebugging;
    bool enableMemoryDebugging;
    bool enablePerformanceDebugging;
    bool enablePatternAnalysis;
    double debugInterval;
    std::string outputFile;
    bool outputToConsole;
    bool enableStepByStepDebugging;
    bool captureDetailedSpikeTrains;
    bool analyzeSynapticDynamics;
    bool trackMemoryUsage;
    bool monitorPerformance;
    
    DebugConfig() : enableNeuronDebugging(true), enableSynapseDebugging(true),
                   enableSystemDebugging(true), enableMemoryDebugging(true),
                   enablePerformanceDebugging(true), enablePatternAnalysis(true),
                   debugInterval(0.1), outputFile("debug_output.txt"),
                   outputToConsole(true), enableStepByStepDebugging(false),
                   captureDetailedSpikeTrains(false), analyzeSynapticDynamics(true),
                   trackMemoryUsage(true), monitorPerformance(true) {}
};

// Debug output formatter
class DebugOutputFormatter {
public:
    enum class OutputFormat {
        TEXT,
        CSV,
        JSON,
        BINARY
    };
    
    static std::string formatNeuronInfo(const NeuronDebugInfo& info, OutputFormat format = OutputFormat::TEXT);
    static std::string formatSynapseInfo(const SynapseDebugInfo& info, OutputFormat format = OutputFormat::TEXT);
    static std::string formatSystemInfo(const BrainSystemDebugInfo& info, OutputFormat format = OutputFormat::TEXT);
    static std::string formatDebugMessage(const std::string& message, OutputFormat format = OutputFormat::TEXT);
};

// Brain debugger class
class BrainDebugger {
public:
    BrainDebugger(std::shared_ptr<Brain> brain, const DebugConfig& config = DebugConfig());
    ~BrainDebugger();
    
    // Main debugging interface
    void startDebugging();
    void stopDebugging();
    void stepDebug();
    void saveDebugState(const std::string& filename) const;
    void loadDebugState(const std::string& filename);
    
    // Get current debug information
    NeuronDebugInfo getNeuronDebugInfo(NeuronId id) const;
    SynapseDebugInfo getSynapseDebugInfo(SynapseId id) const;
    BrainSystemDebugInfo getSystemDebugInfo(const std::string& systemName) const;
    
    // Get all debug information
    std::vector<NeuronDebugInfo> getAllNeuronDebugInfo() const;
    std::vector<SynapseDebugInfo> getAllSynapseDebugInfo() const;
    std::vector<BrainSystemDebugInfo> getAllSystemDebugInfo() const;
    
    // Analysis functions
    void analyzeNeuronDynamics() const;
    void analyzeSynapseDynamics() const;
    void analyzeNetworkActivity() const;
    void detectAnomalies() const;
    void generatePatternReport() const;
    
    // Memory and performance debugging
    void debugMemoryUsage() const;
    void debugPerformance() const;
    void capturePerformanceProfile() const;
    
    // Configuration
    void updateDebugConfig(const DebugConfig& config);
    void enableDebugFeature(const std::string& feature, bool enable);
    bool isDebugFeatureEnabled(const std::string& feature) const;
    
    // Export/import
    void exportDebugData(const std::string& filename) const;
    void importDebugData(const std::string& filename);
    
    // Statistical analysis
    double calculateNeuronFiringRate(NeuronId id) const;
    std::vector<float> getNeuronSpikeTrain(NeuronId id) const;
    float getSynapseWeightChange(SynapseId id) const;
    double calculateNetworkEfficiency() const;
    double calculateLearningProgress() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike train analyzer
class SpikeTrainAnalyzer {
public:
    SpikeTrainAnalyzer();
    ~SpikeTrainAnalyzer();
    
    // Analyze spike train
    struct SpikeTrainAnalysis {
        std::vector<double> spikeTimes;
        double meanFiringRate;
        double cvFiringRate; // Coefficient of variation
        double burstRate;
        double synchronyIndex;
        std::vector<double> frequencySpectrum;
        double dominantFrequency;
        std::vector<size_t> refractoryViolations;
        std::string patternType;
        double patternConfidence;
    };
    
    SpikeTrainAnalysis analyzeSpikeTrain(const std::vector<double>& spikeTimes) const;
    
    // Pattern detection
    enum class PatternType {
        REGULAR_SPIKING,
        BURSTING,
        PAUSES,
        CHAOTIC,
        SYNCHRONIZED,
        RELAXATION,
        UNKNOWN
    };
    
    PatternType detectPatternType(const std::vector<double>& spikeTimes) const;
    
    // Cross-correlation analysis
    double calculateCrossCorrelation(const std::vector<double>& train1, const std::vector<double>& train2) const;
    
    // Population analysis
    struct PopulationAnalysis {
        double globalFiringRate;
        double population synchrony;
        double spatialCorrelation;
        std::vector<double> activityProfile;
        std::vector<std::vector<double>> pairwiseCorrelations;
    };
    
    PopulationAnalysis analyzePopulation(const std::vector<std::vector<double>>& spikeTrains) const;
    
    // Statistical analysis
    double calculatePairwisePhase Synchrony(const std::vector<double>& train1, const std::vector<double>& train2) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Synaptic dynamics analyzer
class SynapticDynamicsAnalyzer {
public:
    SynapticDynamicsAnalyzer();
    ~SynapticDynamicsAnalyzer();
    
    // Analyze synaptic changes
    struct SynapticChange {
        SynapseId synapseId;
        NeuronId preNeuronId;
        NeuronId postNeuronId;
        double weightChange;
        double changeRate;
        double stabilityIndex;
        std::string plasticityType;
        std::chrono::high_resolution_clock::time_point timestamp;
        bool contributingToLearning;
    };
    
    std::vector<SynapticChange> analyzeSynapticChanges(const Brain* brain, double timeWindow) const;
    
    // STDP analysis
    struct StdpAnalysis {
        double ltpMagnitude;
        double ltdMagnitude;
        double timingPrecision;
        double asymmetryIndex;
        std::vector<double> learningTraces;
        bool stdpEffective;
    };
    
    StdpAnalysis analyzeStdp(const Brain* brain) const;
    
    // Hebbian analysis
    struct HebbianAnalysis {
        double correlationStrength;
        double coactivationFrequency;
        double weightUpdateEfficiency;
        std::vector<double> weightHistograms;
        bool hebbianLearningActive;
    };
    
    HebbianAnalysis analyzeHebbian(const Brain* brain) const;
    
    // Reward-modulated plasticity analysis
    struct RewardModulatedAnalysis {
        double rewardPredictionError;
        double eligibilityTraceDecay;
        double rewardModulationStrength;
        std::vector<double> rewardResponses;
        bool rewardModulationActive;
    };
    
    RewardModulatedAnalysis analyzeRewardModulated(const Brain* brain) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Memory debugger
class MemoryDebugger {
public:
    MemoryDebugger();
    ~MemoryDebugger();
    
    // Memory tracking
    struct MemoryInfo {
        size_t totalMemory;
        size_t usedMemory;
        size_t freeMemory;
        std::unordered_map<std::string, size_t> allocationsByType;
        std::vector<std::pair<void*, size_t>> allocationHistory;
        double fragmentationRatio;
    };
    
    MemoryInfo getMemoryInfo() const;
    
    // Leak detection
    struct MemoryLeak {
        void* leakedAddress;
        size_t size;
        std::string allocationSite;
        std::chrono::high_resolution_clock::time_point allocationTime;
        size_t allocationCount;
    };
    
    std::vector<MemoryLeak> detectMemoryLeaks() const;
    
    // Performance analysis
    struct PerformanceMetrics {
        double allocationTimePerByte;
        double deallocationTimePerByte;
        double memoryAccessLatency;
        size_t cacheMissRate;
        size_t pageFaultRate;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Pattern recognizer
class PatternRecognizer {
public:
    PatternRecognizer();
    ~PatternRecognizer();
    
    // Learning patterns
    enum class LearningPattern {
        SENSORY_REWARD_ASSOCIATION,
        CONTEXTUAL_MEMORY,
        PREDICTIVE_LEARNING,
        SOCIAL_LEARNING,
        CONTINUAL_LEARNING,
        CONCEPT_FORMATION,
        HABITUATION,
        SENSITIZATION
    };
    
    struct PatternInstance {
        LearningPattern patternType;
        double confidence;
        std::vector<double> patternFeatures;
        std::chrono::high_resolution_clock::time_point occurrenceTime;
        std::string contextDescription;
    };
    
    std::vector<PatternInstance> recognizePatterns(const Brain* brain) const;
    
    // Pattern stability analysis
    struct PatternStability {
        double stabilityScore;
        double decayRate;
        std::vector<double> reinforcementEvents;
        bool isStable;
    };
    
    PatternStability analyzePatternStability(const LearningPattern& pattern) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Debug log analyzer
class DebugLogAnalyzer {
public:
    DebugLogAnalyzer();
    ~DebugLogAnalyzer();
    
    // Parse debug log file
    bool loadLogFile(const std::string& filename);
    bool loadLogContent(const std::string& content);
    
    // Analysis functions
    struct LogAnalysis {
        std::unordered_map<std::string, size_t> eventCounts;
        std::unordered_map<std::string, double> eventDurations;
        std::vector<std::string> errorMessages;
        std::vector<std::string> warningMessages;
        std::unordered_map<std::string, std::vector<std::string>> eventSequences;
        double averageEventInterval;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        size_t totalEvents;
    };
    
    LogAnalysis analyzeLog() const;
    
    // Timeline analysis
    struct TimelineAnalysis {
        std::vector<std::pair<double, std::string>> events;
        std::vector<std::pair<size_t, std::string>> criticalEvents;
        double peakActivityTime;
        double calmestPeriod;
    };
    
    TimelineAnalysis analyzeTimeline() const;
    
    // Pattern detection in logs
    struct LogPattern {
        std::string patternId;
        std::string patternType;
        double frequency;
        std::string description;
        std::vector<std::string> exampleSequences;
    };
    
    std::vector<LogPattern> detectLogPatterns() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Comprehensive brain debugger interface
class ComprehensiveBrainDebugger {
public:
    ComprehensiveBrainDebugger(std::shared_ptr<Brain> brain);
    ~ComprehensiveBrainDebugger();
    
    // Run comprehensive analysis
    void runComprehensiveAnalysis();
    void generateDebugReport(const std::string& outputFile = "brain_debug_report.txt") const;
    void generateVisualizations(const std::string& outputDir = "debug_visualizations") const;
    
    // Real-time debugging
    void startRealTimeDebugging(double intervalSeconds = 1.0);
    void stopRealTimeDebugging();
    bool isRealTimeDebugging() const;
    
    // Integration with simulation
    void attachToSimulation(std::shared_ptr<Brain> brain);
    void detachFromSimulation();
    
    // Configuration
    void setDebugLevel(int level);
    int getDebugLevel() const;
    
    // Custom debug functions
    void addCustomDebugFunction(const std::string& name, std::function<void(const Brain*, std::ostream&)> func);
    void removeCustomDebugFunction(const std::string& name);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Global debugger manager
class DebuggerManager {
public:
    static DebuggerManager& getInstance();
    
    void registerDebugger(const std::string& name, std::shared_ptr<ComprehensiveBrainDebugger> debugger);
    std::shared_ptr<ComprehensiveBrainDebugger> getDebugger(const std::string& name);
    void unregisterDebugger(const std::string& name);
    void listDebuggers() const;
    
    void startAllDebuggers();
    void stopAllDebuggers();
    void runAllDebuggers();
    
    void generateCombinedReport(const std::string& outputFile) const;
    void exportAllDebugData(const std::string& outputDir) const;
    
private:
    DebuggerManager() = default;
    ~DebuggerManager() = default;
    DebuggerManager(const DebuggerManager&) = delete;
    DebuggerManager& operator=(const DebuggerManager&) = delete;
    
    struct Impl;
    Impl* pImpl;
};

} // namespace debug

// Convenience types and aliases
declare_type_alias(DebugOutputFormatter, debug::DebugOutputFormatter);
declare_type_alias(BrainDebugger, debug::BrainDebugger);
declare_type_alias(SpikeTrainAnalyzer, debug::SpikeTrainAnalyzer);
declare_type_alias(SynapticDynamicsAnalyzer, debug::SynapticDynamicsAnalyzer);
declare_type_alias(MemoryDebugger, debug::MemoryDebugger);
declare_type_alias(PatternRecognizer, debug::PatternRecognizer);
declare_type_alias(DebugLogAnalyzer, debug::DebugLogAnalyzer);
declare_type_alias(ComprehensiveBrainDebugger, debug::ComprehensiveBrainDebugger);

// Global debugging utilities
namespace dbg {
    // Get global debugger manager
    debug::DebuggerManager& getDebuggerManager();
    
    // Quick debugging functions
    void debugBrainState(std::shared_ptr<Brain> brain, const std::string& outputFile = "debug_output.txt");
    void debugNeuron(std::shared_ptr<Brain> brain, NeuronId id);
    void debugSynapse(std::shared_ptr<Brain> brain, SynapseId id);
    
    // Performance debugging
    void debugPerformance(std::shared_ptr<Brain> brain);
    void debugMemory(std::shared_ptr<Brain> brain);
    
    // Analysis functions
    void analyzeSpikeTrains(std::shared_ptr<Brain> brain);
    void analyzeSynapticDynamics(std::shared_ptr<Brain> brain);
    void analyzeLearningProgress(std::shared_ptr<Brain> brain);
    
    // Log analysis
    void analyzeDebugLogs(const std::string& logFile);
    
    // Configuration
    void setDebugLevel(int level);
    void enableFeature(const std::string& feature, bool enable = true);
    bool isFeatureEnabled(const std::string& feature);
    
    // Report generation
    void generateDebugReport(std::shared_ptr<Brain> brain, const std::string& outputFile = "debug_report.txt");
    void generateVisualizations(std::shared_ptr<Brain> brain, const std::string& outputDir = "debug_viz");
    
    // Real-time monitoring
    void startRealTimeMonitoring(std::shared_ptr<Brain> brain, double intervalSeconds = 1.0);
    void stopRealTimeMonitoring();
    bool isMonitoring();
}

} // namespace nlm
