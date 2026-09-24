#pragma once

#include <memory>
#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>

namespace nlm {

// Forward declarations
class AgentBrain;
class SensoryPercept;
class MotorCommand;

// Performance monitoring
class PerformanceMonitor {
public:
    struct Metrics {
        // Timing metrics
        double averageSensoryProcessTime = 0.0;
        double averageMotorDecodeTime = 0.0;
        double averageRewardApplicationTime = 0.0;
        double averageDevelopmentUpdateTime = 0.0;
        
        // Operation counts
        uint64_t sensoryProcessCount = 0;
        uint64_t motorDecodeCount = 0;
        uint64_t rewardApplicationCount = 0;
        uint64_t developmentUpdateCount = 0;
        
        // Resource usage
        size_t peakMemoryUsage = 0;
        float cpuUsage = 0.0f;
        
        // Network/activity metrics
        float firingRate = 0.0f;
        float explorationRate = 0.0f;
        float noveltyRate = 0.0f;
        float predictionError = 0.0f;
    };
    
    // Performance monitor callback
    using PerformanceCallback = std::function<void(const Metrics& metrics, double timestamp)>;
    
    // Start monitoring
    static void startMonitoring(AgentBrain* agent);
    
    // Stop monitoring
    static void stopMonitoring();
    
    // Get current metrics
    static Metrics getMetrics();
    
    // Register performance callback
    static void registerPerformanceCallback(const PerformanceCallback& callback);
    
    // Reset metrics
    static void resetMetrics();
    
    // Enable/disable monitoring
    static void setEnabled(bool enabled);
    
private:
    static std::unique_ptr<PerformanceMonitor> instance_;
    static Metrics metrics_;
    static std::atomic<bool> enabled_;
    static std::vector<PerformanceCallback> callbacks_;
    static std::chrono::steady_clock::time_point startTime_;
};

// Neural activity debugger
class NeuralActivityDebugger {
public:
    struct NeuronState {
        float membranePotential = 0.0f;
        float restingPotential = -70.0f;
        float threshold = -55.0f;
        float refractoryPeriod = 0.0f;
        float adaptation = 0.0f;
        float synapticInput = 0.0f;
        float outputCurrent = 0.0f;
        double lastSpikeTime = 0.0;
        uint32_t spikeCount = 0;
    };
    
    struct PopulationStats {
        float averageActivity = 0.0f;
        float peakActivity = 0.0f;
        float standardDeviation = 0.0f;
        float firingRate = 0.0f;
        float synchrony = 0.0f;
    };
    
    // Debugger callback
    using DebugCallback = std::function<void(const std::vector<NeuronState>& neurons, const std::unordered_map<std::string, PopulationStats>& populations)>;
    
    // Start neural debugging
    static void startDebugging(AgentBrain* agent);
    
    // Stop neural debugging
    static void stopDebugging();
    
    // Get neuron states
    static std::vector<NeuronState> getNeuronStates(size_t regionIndex = 0, size_t populationIndex = 0);
    
    // Get population statistics
    static PopulationStats getPopulationStats(const std::string& regionName, const std::string& populationName);
    
    // Get all population statistics
    static std::unordered_map<std::string, PopulationStats> getAllPopulationStats();
    
    // Register debug callback
    static void registerDebugCallback(const DebugCallback& callback);
    
    // Enable/disable specific debug features
    static void setFeatureEnabled(const std::string& feature, bool enabled);
    
    // Get feature status
    static bool isFeatureEnabled(const std::string& feature);
    
private:
    static std::unique_ptr<NeuralActivityDebugger> instance_;
    static std::atomic<bool> enabled_;
    static std::vector<DebugCallback> callbacks_;
    static std::thread monitoringThread_;
};

// Advanced visualization
class AdvancedVisualizer {
public:
    struct VisualizationData {
        // Neural data
        std::vector<float> neuralActivity;
        std::vector<std::pair<float, float>> synapticWeights;
        std::vector<std::pair<double, float>> spikeTimes;
        
        // Sensor data
        std::vector<float> sensoryInput;
        std::vector<float> motorOutput;
        
        // Environmental data
        std::vector<std::pair<float, float>> agentPosition;
        std::vector<float> agentOrientation;
        
        // Internal state
        std::vector<float> neuromodulators;
        std::vector<float> developmentalStages;
    };
    
    // Visualization format
    enum class Format {
        Text,
        ASCII,
        JSON,
        CSV,
        Binary,
        Custom
    };
    
    // Visualization callback
    using VisualizationCallback = std::function<void(const VisualizationData& data, Format format)>;
    
    // Start visualization
    static void startVisualization(AgentBrain* agent, Format format = Format::ASCII);
    
    // Stop visualization
    static void stopVisualization();
    
    // Get current visualization data
    static VisualizationData getVisualizationData();
    
    // Register visualization callback
    static void registerVisualizationCallback(const VisualizationCallback& callback);
    
    // Save visualization to file
    static bool saveVisualization(const std::string& filename, Format format = Format::ASCII);
    
    // Set visualization parameters
    static void setVisualizationParameter(const std::string& parameter, const std::string& value);
    
private:
    static std::unique_ptr<AdvancedVisualizer> instance_;
    static std::atomic<bool> enabled_;
    static std::vector<VisualizationCallback> callbacks_;
    static std::thread visualizationThread_;
};

// Real-time analysis system
class RealTimeAnalyzer {
public:
    struct AnalysisResult {
        // Behavioral patterns
        std::vector<std::string> detectedPatterns;
        std::vector<float> patternConfidence;
        std::vector<std::string> behavioralGoals;
        
        // Cognitive metrics
        float attentionLevel = 0.0f;
        float workingMemoryLoad = 0.0f;
        float planningHorizon = 0.0f;
        float decisionConfidence = 0.0f;
        
        // Learning metrics
        float learningRate = 0.0f;
        float retentionStrength = 0.0f;
        float generalizationAbility = 0.0f;
        
        // Adaptation metrics
        float adaptationSpeed = 0.0f;
        float resilienceScore = 0.0f;
        float flexibilityScore = 0.0f;
    };
    
    // Analysis callback
    using AnalysisCallback = std::function<void(const AnalysisResult& result, double timestamp)>;
    
    // Start real-time analysis
    static void startAnalysis(AgentBrain* agent);
    
    // Stop real-time analysis
    static void stopAnalysis();
    
    // Get current analysis results
    static AnalysisResult getAnalysisResults();
    
    // Register analysis callback
    static void registerAnalysisCallback(const AnalysisCallback& callback);
    
    // Add analysis rule
    static void addAnalysisRule(const std::string& name, const std::function<bool(const AnalysisResult&)>& rule);
    
    // Remove analysis rule
    static void removeAnalysisRule(const std::string& name);
    
private:
    static std::unique_ptr<RealTimeAnalyzer> instance_;
    static std::atomic<bool> enabled_;
    static std::vector<AnalysisCallback> callbacks_;
    static std::vector<std::pair<std::string, std::function<bool(const AnalysisResult&)>>> analysisRules_;
};

// Comprehensive debugging interface
class DebugInterface {
public:
    // Debug command
    struct DebugCommand {
        std::string name;
        std::string description;
        std::function<void()> execute;
        std::function<std::string()> getStatus;
    };
    
    // Register debug command
    static void registerCommand(const DebugCommand& command);
    
    // Execute debug command
    static bool executeCommand(const std::string& name);
    
    // Get command status
    static std::string getCommandStatus(const std::string& name);
    
    // List all commands
    static std::vector<std::string> listCommands();
    
    // Start comprehensive debugging session
    static void startDebugSession(AgentBrain* agent);
    
    // Stop debugging session
    static void stopDebugSession();
    
    // Export debug data
    static bool exportDebugData(const std::string& filename);
    
    // Import debug data
    static bool importDebugData(const std::string& filename);
    
    // Get debug statistics
    static std::string getDebugStatistics();
    
private:
    static std::vector<DebugCommand> commands_;
    static std::unique_ptr<DebugInterface> instance_;
};

// High-performance profiler
class Profiler {
public:
    struct ProfileData {
        std::string functionName;
        double totalTime;
        uint64_t callCount;
        double averageTime;
        double minTime;
        double maxTime;
        std::vector<double> timeSamples;
    };
    
    // Start profiling
    static void startProfiling(AgentBrain* agent, bool detailed = false);
    
    // Stop profiling
    static void stopProfiling();
    
    // Get profile data
    static std::vector<ProfileData> getProfileData();
    
    // Get profile report
    static std::string getProfileReport();
    
    // Save profile data
    static bool saveProfile(const std::string& filename);
    
    // Load profile data
    static bool loadProfile(const std::string& filename);
    
    // Clear profile data
    static void clearProfileData();
    
private:
    static std::unique_ptr<Profiler> instance_;
    static std::atomic<bool> enabled_;
    static std::vector<ProfileData> profileData_;
};

// Unified debugging system
class AgentDebugger {
public:
    // Initialize with agent
    static void initialize(AgentBrain* agent);
    
    // Start all debugging systems
    static void startAllDebugSystems();
    
    // Stop all debugging systems
    static void stopAllDebugSystems();
    
    // Get unified debug state
    struct DebugState {
        bool performanceMonitoring = false;
        bool neuralActivityDebugging = false;
        bool visualization = false;
        bool realTimeAnalysis = false;
        bool profiling = false;
    };
    
    static DebugState getDebugState();
    
    // Enable/disable debug systems
    static void enableDebugSystem(const std::string& systemName, bool enabled);
    
    // Get debug system status
    static bool isDebugSystemEnabled(const std::string& systemName);
    
    // Save debug session
    static bool saveDebugSession(const std::string& filename);
    
    // Load debug session
    static bool loadDebugSession(const std::string& filename);
    
private:
    static std::unique_ptr<AgentDebugger> instance_;
    static AgentBrain* agent_;
    static DebugState debugState_;
};

} // namespace nlm