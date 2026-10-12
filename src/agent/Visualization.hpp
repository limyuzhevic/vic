// Advanced Visualization and Debugging Tools for NLM
// Provides comprehensive visualization of neural activity, memory systems, and agent behavior

#ifndef NLM_VISUALIZATION_H
#define NLM_VISUALIZATION_H

#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "world/SimpleWorld.hpp"
#include "memory/Memory.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace nlm;

// Forward declarations
class VisualizationManager;

// Visualization data structures
struct NeuralActivitySnapshot {
    std::vector<float> neuronActivities;
    std::vector<float> synapticWeights;
    std::vector<float> neuromodulationLevels;
    std::chrono::system_clock::time_point timestamp;
    
    NeuralActivitySnapshot() : timestamp(std::chrono::system_clock::now()) {}
};

struct MemorySystemSnapshot {
    WorkingMemory* workingMemory;
    EpisodicMemory* episodicMemory;
    SemanticMemory* semanticMemory;
    ProceduralMemory* proceduralMemory;
    AssociativeMemory* associativeMemory;
    std::chrono::system_clock::time_point timestamp;
    
    MemorySystemSnapshot() : timestamp(std::chrono::system_clock::now()) {}
};

struct AgentBehaviorSnapshot {
    AgentBrain* agentBrain;
    AgentBody* agentBody;
    SimpleWorld* world;
    MotorCommand lastAction;
    ActionResult lastActionResult;
    float energyLevel;
    float healthLevel;
    float noveltyLevel;
    float curiosityLevel;
    float predictionError;
    std::chrono::system_clock::time_point timestamp;
    
    AgentBehaviorSnapshot() : timestamp(std::chrono::system_clock::now()) {}
};

struct PerformanceMetrics {
    double totalReward;
    double averageStepReward;
    size_t totalSteps;
    size_t successfulActions;
    double explorationRate;
    double learningRate;
    float finalMemoryUsage;
    float finalSkillProficiency;
    double timePerStep;
    
    PerformanceMetrics() : totalReward(0.0), averageStepReward(0.0), totalSteps(0), 
                           successfulActions(0), explorationRate(0.0), learningRate(0.0),
                           finalMemoryUsage(0.0f), finalSkillProficiency(0.0f), timePerStep(0.0) {}
};

// Visualization output formats
enum class OutputFormat {
    TEXT,
    JSON,
    CSV,
    HTML,
    GRAPH,
    TERMINAL
};

// Visualization themes
enum class VisualizationTheme {
    DEFAULT,
    DARK,
    LIGHT,
    BIOS,
    NEURAL,
    MINIMAL
};

class NeuralActivityVisualizer {
public:
    NeuralActivityVisualizer(VisualizationTheme theme = VisualizationTheme::DEFAULT);
    ~NeuralActivityVisualizer();
    
    void setTheme(VisualizationTheme theme);
    void addSnapshot(const NeuralActivitySnapshot& snapshot);
    void clearSnapshots();
    
    // Output methods
    std::string toText() const;
    std::string toJSON() const;
    std::string toCSV() const;
    std::string toHTML() const;
    
    // Analysis methods
    float getAverageActivity() const;
    float getMaxActivity() const;
    float getMinActivity() const;
    size_t getActivityVariance() const;
    
    // Statistical methods
    NeuralActivitySnapshot getAverageSnapshot() const;
    NeuralActivitySnapshot getPeakSnapshot() const;
    NeuralActivitySnapshot getLowActivitySnapshot() const;
    
    // Configuration
    void setMaxSnapshots(size_t maxSnapshots) { maxSnapshots_ = maxSnapshots; }
    void setActivityScaling(float min, float max) { activityMin_ = min; activityMax_ = max; }
    
    void setNeuralRegionColors(const std::map<int, std::string>& colors) { regionColors_ = colors; }
    
private:
    std::vector<NeuralActivitySnapshot> snapshots_;
    size_t maxSnapshots_;
    float activityMin_;
    float activityMax_;
    std::map<int, std::string> regionColors_;
    VisualizationTheme theme_;
    
    std::string formatAsText(const NeuralActivitySnapshot& snapshot) const;
    std::string formatAsJSON(const NeuralActivitySnapshot& snapshot) const;
    std::string formatAsCSV(const NeuralActivitySnapshot& snapshot) const;
    std::string formatAsHTML(const NeuralActivitySnapshot& snapshot) const;
};

class MemorySystemVisualizer {
public:
    MemorySystemVisualizer(VisualizationTheme theme = VisualizationTheme::DEFAULT);
    ~MemorySystemVisualizer();
    
    void setTheme(VisualizationTheme theme);
    void setWorkingMemory(WorkingMemory* wm) { workingMemory_ = wm; }
    void setEpisodicMemory(EpisodicMemory* em) { episodicMemory_ = em; }
    void setSemanticMemory(SemanticMemory* sm) { semanticMemory_ = sm; }
    void setProceduralMemory(ProceduralMemory* pm) { proceduralMemory_ = pm; }
    void setAssociativeMemory(AssociativeMemory* am) { associativeMemory_ = am; }
    
    // Output methods
    std::string toText() const;
    std::string toJSON() const;
    std::string toHTML() const;
    
    // Analysis methods
    void analyzeMemoryHealth() const;
    void detectMemoryGaps() const;
    void assessMemoryEfficiency() const;
    
    // Configuration
    void setMemoryCapacityThresholds(float workingThreshold, float episodicThreshold,
                                     float semanticThreshold) {
        workingMemThreshold_ = workingThreshold;
        episodicMemThreshold_ = episodicThreshold;
        semanticMemThreshold_ = semanticThreshold;
    }
    
    void setShowDetailedStats(bool show) { showDetailedStats_ = show; }
    
private:
    WorkingMemory* workingMemory_;
    EpisodicMemory* episodicMemory_;
    SemanticMemory* semanticMemory_;
    ProceduralMemory* proceduralMemory_;
    AssociativeMemory* associativeMemory_;
    
    float workingMemThreshold_;
    float episodicMemThreshold_;
    float semanticMemThreshold_;
    bool showDetailedStats_;
    VisualizationTheme theme_;
    
    std::string formatWorkingMemoryStats() const;
    std::string formatEpisodicMemoryStats() const;
    std::string formatSemanticMemoryStats() const;
    std::string formatProceduralMemoryStats() const;
    std::string formatAssociativeMemoryStats() const;
};

class AgentBehaviorVisualizer {
public:
    AgentBehaviorVisualizer(VisualizationTheme theme = VisualizationTheme::DEFAULT);
    ~AgentBehaviorVisualizer();
    
    void setTheme(VisualizationTheme theme);
    void setAgentBrain(AgentBrain* brain) { agentBrain_ = brain; }
    void setAgentBody(AgentBody* body) { agentBody_ = body; }
    void setWorld(SimpleWorld* world) { world_ = world; }
    
    // Output methods
    std::string toText() const;
    std::string toJSON() const;
    std::string toHTML() const;
    
    // Analysis methods
    void analyzeMovementPatterns() const;
    void analyzeDecisionMaking() const;
    void assessGoalProgress() const;
    
    // Configuration
    void setTrackingResolution(size_t resolution) { trackingResolution_ = resolution; }
    void setShowActionDetails(bool show) { showActionDetails_ = show; }
    
private:
    AgentBrain* agentBrain_;
    AgentBody* agentBody_;
    SimpleWorld* world_;
    
    size_t trackingResolution_;
    bool showActionDetails_;
    VisualizationTheme theme_;
    
    std::string formatMovementStats() const;
    std::string formatDecisionStats() const;
    std::string formatGoalProgress() const;
};

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    void startTiming(const std::string& operation);
    void stopTiming(const std::string& operation);
    void recordMetric(const std::string& name, double value);
    
    PerformanceMetrics getCurrentMetrics() const;
    std::string getPerformanceReport() const;
    
    // Configuration
    void setReportingInterval(size_t interval) { reportingInterval_ = interval; }
    void setMetricThresholds(const std::map<std::string, double>& thresholds) { metricThresholds_ = thresholds; }
    
    void enableAlerting(bool enable) { enableAlerting_ = enable; }
    
private:
    struct TimingInfo {
        std::chrono::system_clock::time_point startTime;
        std::chrono::duration<double> accumulatedTime;
        size_t callCount;
    };
    
    std::map<std::string, TimingInfo> timingData_;
    std::map<std::string, std::vector<double>> metricData_;
    size_t reportingInterval_;
    std::map<std::string, double> metricThresholds_;
    bool enableAlerting_;
    mutable PerformanceMetrics currentMetrics_;
    
    void updateMetrics();
    std::string generateAlertReport() const;
    double calculateMean(const std::vector<double>& values) const;
    double calculateVariance(const std::vector<double>& values) const;
};

class DebugConsole {
public:
    DebugConsole();
    ~DebugConsole();
    
    void logMessage(const std::string& category, const std::string& message, 
                    const std::string& level = "INFO");
    void logNeuralEvent(const std::string& eventType, double timestamp, 
                       const std::map<std::string, std::string>& metadata);
    void logMemoryEvent(const std::string& eventType, double timestamp,
                       const std::map<std::string, std::string>& metadata);
    void logBehaviorEvent(const std::string& eventType, double timestamp,
                         const std::map<std::string, std::string>& metadata);
    
    std::string getDebugLog() const;
    void clearLog();
    
    // Filtering and search
    void setLogFilter(const std::string& category, const std::string& level);
    std::vector<std::string> searchLog(const std::string& pattern) const;
    
    // Real-time monitoring
    void startRealTimeMonitoring(size_t updateInterval);
    void stopRealTimeMonitoring();
    bool isMonitoring() const { return monitoring_; }
    
private:
    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        std::string category;
        std::string level;
        std::string message;
        std::map<std::string, std::string> metadata;
    };
    
    std::vector<LogEntry> logEntries_;
    std::string currentFilterCategory_;
    std::string currentFilterLevel_;
    bool monitoring_;
    size_t updateInterval_;
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    bool shouldLogEntry(const LogEntry& entry) const;
};

class VisualizationManager {
public:
    VisualizationManager(VisualizationTheme theme = VisualizationTheme::DEFAULT);
    ~VisualizationManager();
    
    // Core visualization methods
    void updateNeuralVisualization(const NeuralActivitySnapshot& snapshot);
    void updateMemoryVisualization(const MemorySystemSnapshot& snapshot);
    void updateBehaviorVisualization(const AgentBehaviorSnapshot& snapshot);
    
    // Output methods
    std::string generateFullReport(OutputFormat format = OutputFormat::TEXT) const;
    std::string generateNeuralReport(OutputFormat format = OutputFormat::TEXT) const;
    std::string generateMemoryReport(OutputFormat format = OutputFormat::TEXT) const;
    std::string generateBehaviorReport(OutputFormat format = OutputFormat::TEXT) const;
    
    // Configuration
    void setTheme(VisualizationTheme theme);
    void setOutputFormat(OutputFormat format) { outputFormat_ = format; }
    void setMaxReportSize(size_t maxSize) { maxReportSize_ = maxSize; }
    
    // Integration with NLM systems
    void integrateWithAgent(AgentBrain* agentBrain, AgentBody* agentBody, SimpleWorld* world);
    void integrateWithMemory(WorkingMemory* workingMemory, EpisodicMemory* episodicMemory,
                            SemanticMemory* semanticMemory, ProceduralMemory* proceduralMemory,
                            AssociativeMemory* associativeMemory);
    
    // Real-time monitoring
    void startRealTimeVisualization(size_t updateInterval);
    void stopRealTimeVisualization();
    bool isVisualizing() const { return visualizing_; }
    
private:
    NeuralActivityVisualizer neuralVisualizer_;
    MemorySystemVisualizer memoryVisualizer_;
    AgentBehaviorVisualizer behaviorVisualizer_;
    PerformanceMonitor performanceMonitor_;
    DebugConsole debugConsole_;
    
    OutputFormat outputFormat_;
    size_t maxReportSize_;
    bool visualizing_;
    
    NeuralActivitySnapshot currentNeuralSnapshot_;
    MemorySystemSnapshot currentMemorySnapshot_;
    AgentBehaviorSnapshot currentBehaviorSnapshot_;
    
    std::string generateCombinedReport() const;
    std::string truncateReport(const std::string& report) const;
};

// Convenience functions for common visualization scenarios
namespace Visualization {
    std::string generateQuickReport(AgentBrain* agentBrain, AgentBody* agentBody, 
                                   SimpleWorld* world, WorkingMemory* workingMemory,
                                   EpisodicMemory* episodicMemory,
                                   SemanticMemory* semanticMemory, ProceduralMemory* proceduralMemory,
                                   AssociativeMemory* associativeMemory);
    
    void saveVisualizationToFile(const std::string& filename, const std::string& report);
    
    std::string createHTMLDashboard(const std::string& neuralData, 
                                   const std::string& memoryData,
                                   const std::string& behaviorData);
    
    void displayTerminalSummary(const std::string& title, const std::string& content);
}

#endif // NLM_VISUALIZATION_H