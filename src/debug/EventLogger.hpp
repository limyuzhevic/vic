#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "../core/Logger/Logger.hpp"
#include "DebuggingSystem.hpp"

namespace nlm {

// Event types for simulation monitoring
enum class EventType {
    StepCompleted,
    NeuronFired,
    SynapseActivated,
    MemoryAccessed,
    MemoryWritten,
    MemoryRead,
    PlasticityChange,
    NeuromodulationApplied,
    SensorInputReceived,
    ActionProduced,
    RewardReceived,
    PredictionError,
    DevelopmentStageChanged,
    LearningSignal,
    ConnectionCreated,
    ConnectionRemoved,
    WeightChanged,
    RegionCreated,
    RegionDestroyed,
    Custom
};

// Event priority levels
enum class EventPriority {
    Low,
    Normal,
    High,
    Critical
};

// SimulationEvent: Represents a single simulation event
class SimulationEvent {
public:
    SimulationEvent();
    SimulationEvent(EventType type, const std::string& source, const std::string& message);
    
    // Basic properties
    EventType getType() const { return type_; }
    const std::string& getSource() const { return source_; }
    const std::string& getMessage() const { return message_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    uint64_t getSimulationStep() const { return simulationStep_; }
    
    // Priority
    EventPriority getPriority() const { return priority_; }
    void setPriority(EventPriority priority) { priority_ = priority; }
    
    // Event data
    template<typename T>
    void setData(const std::string& key, const T& value);
    template<typename T>
    T getData(const std::string& key) const;
    bool hasData(const std::string& key) const;
    
    // Simulation context
    void setSimulationStep(uint64_t step) { simulationStep_ = step; }
    void setSimulationTime(double time) { simulationTime_ = time; }
    double getSimulationTime() const { return simulationTime_; }
    
    // Location information
    void setLocation(const std::string& location) { location_ = location; }
    const std::string& getLocation() const { return location_; }
    
    // Serialization
    std::string serialize() const;
    bool deserialize(const std::string& data);
    
    // Comparison
    bool operator==(const SimulationEvent& other) const;
    bool operator!=(const SimulationEvent& other) const { return !(*this == other); }
    
    // Priority ordering
    bool operator<(const SimulationEvent& other) const {
        return priority_ < other.priority_;
    }
    
    bool operator>(const SimulationEvent& other) const {
        return priority_ > other.priority_;
    }
    
private:
    EventType type_;
    std::string source_;
    std::string message_;
    std::chrono::system_clock::time_point timestamp_;
    uint64_t simulationStep_;
    double simulationTime_;
    EventPriority priority_;
    std::string location_;
    std::unordered_map<std::string, std::string> dataMap_;
    mutable std::mutex dataMutex_;
};

// EventLogger: Comprehensive event capture and logging
class EventLogger {
public:
    EventLogger();
    explicit EventLogger(std::shared_ptr<DebuggingSystem> debuggingSystem);
    ~EventLogger();
    
    // Configuration
    void setEventFilter(EventType type, bool include = true);
    void clearEventFilters();
    void setEventPriorityFilter(EventPriority minPriority = EventPriority::Low);
    void setSimulationStepFilter(uint64_t minStep = 0, uint64_t maxStep = UINT64_MAX);
    void setTimeWindow(double startTime, double endTime);
    
    // Event recording
    void logEvent(EventType type, const std::string& source, const std::string& message);
    void logEvent(SimulationEvent event);
    void logEventWithData(EventType type, const std::string& source, const std::string& message,
                         const std::unordered_map<std::string, std::string>& data);
    
    // Event management
    const std::vector<SimulationEvent>& getEvents() const { return events_; }
    size_t getEventCount() const { return events_.size(); }
    std::vector<SimulationEvent> getEvents(EventType type) const;
    std::vector<SimulationEvent> getEventsInTimeRange(
        double startTime, double endTime) const;
    
    // Event filtering
    bool shouldLogEvent(const SimulationEvent& event) const;
    
    // Statistics
    size_t getEventCount(EventType type) const;
    double getAverageTimeBetweenEvents() const;
    std::chrono::system_clock::time_point getFirstEventTime() const;
    std::chrono::system_clock::time_point getLastEventTime() const;
    
    // Analysis
    std::string analyzeEventFrequency() const;
    std::string analyzeEventSequences() const;
    std::string analyzeCorrelation(const std::string& event1, const std::string& event2) const;
    
    // Integration
    void setDebuggingSystem(std::shared_ptr<DebuggingSystem> debuggingSystem) { debuggingSystem_ = debuggingSystem; }
    
    // Export
    bool exportEvents(const std::string& filepath) const;
    std::string exportEventsAsString() const;
    
    // Clearing
    void clearEvents();
    void clearEvents(EventType type);
    
private:
    std::vector<SimulationEvent> events_;
    mutable std::mutex eventsMutex_;
    
    std::shared_ptr<DebuggingSystem> debuggingSystem_;
    
    std::unordered_map<EventType, bool> eventFilters_;
    EventPriority minPriority_;
    uint64_t minStep_;
    uint64_t maxStep_;
    
    double timeWindowStart_;
    double timeWindowEnd_;
    bool timeWindowEnabled_;
    
    // Helper methods
    void notifyDebuggingSystem(const SimulationEvent& event);
    std::string eventTypeToString(EventType type) const;
    std::string eventPriorityToString(EventPriority priority) const;
};

// StateTracker: State evolution monitoring
class StateTracker {
public:
    StateTracker();
    ~StateTracker();
    
    // State recording
    void recordState(const std::string& stateName, const std::string& stateData, 
                    const std::string& context = "");
    void recordState(const std::string& stateName, double value, const std::string& unit = "");
    void recordState(const std::string& stateName, size_t count, const std::string& description = "");
    
    // State querying
    std::string getState(const std::string& stateName) const;
    std::string getState(const std::string& stateName, const std::string& defaultValue) const;
    
    double getStateAsDouble(const std::string& stateName) const;
    size_t getStateAsSize(const std::string& stateName) const;
    
    // State history
    std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> 
        getStateHistory(const std::string& stateName) const;
    
    // State analysis
    std::string analyzeStateChanges(const std::string& stateName) const;
    std::string analyzeCorrelations(const std::string& state1, const std::string& state2) const;
    
    // Integration
    void setDebuggingSystem(std::shared_ptr<DebuggingSystem> debuggingSystem) { debuggingSystem_ = debuggingSystem; }
    
    // Export
    bool exportState(const std::string& filepath) const;
    std::string exportStateAsString() const;
    
    // Clearing
    void clearStates();
    
private:
    struct StateEntry {
        std::chrono::system_clock::time_point timestamp;
        std::string data;
        std::string context;
    };
    
    std::unordered_map<std::string, std::vector<StateEntry>> stateHistory_;
    mutable std::mutex historyMutex_;
    
    std::shared_ptr<DebuggingSystem> debuggingSystem_;
};

// AnomalyDetector: Automatic anomaly detection
class AnomalyDetector {
public:
    AnomalyDetector();
    explicit AnomalyDetector(std::shared_ptr<EventLogger> eventLogger);
    
    // Configuration
    void setAnomalyThreshold(double threshold) { threshold_ = threshold; }
    void setDetectionWindow(double window) { window_ = window; }
    void setMinSampleCount(size_t minCount) { minSampleCount_ = minCount; }
    
    // Detection
    std::vector<SimulationEvent> detectAnomalies() const;
    std::string getAnomalyReport() const;
    
    // Analysis
    double calculateAnomalyScore(const std::vector<double>& values) const;
    std::string analyzeAnomalyPattern(const SimulationEvent& anomaly) const;
    
    // Integration
    void setEventLogger(std::shared_ptr<EventLogger> eventLogger) { eventLogger_ = eventLogger; }
    
    // Export
    bool exportAnomalies(const std::string& filepath) const;
    std::string exportAnomaliesAsString() const;
    
private:
    std::shared_ptr<EventLogger> eventLogger_;
    
    double threshold_;
    double window_;
    size_t minSampleCount_;
    
    // Helper methods
    std::vector<double> calculateEventMetrics(const std::string& eventType) const;
    bool isAnomaly(double value, double mean, double stddev) const;
};

// SimulationMonitor: High-level simulation monitoring
class SimulationMonitor {
public:
    SimulationMonitor();
    ~SimulationMonitor();
    
    // Monitoring setup
    void addEventLogger(std::shared_ptr<EventLogger> eventLogger);
    void addStateTracker(std::shared_ptr<StateTracker> stateTracker);
    void addAnomalyDetector(std::shared_ptr<AnomalyDetector> anomalyDetector);
    
    // Integration with debugging system
    void setDebuggingSystem(std::shared_ptr<DebuggingSystem> debuggingSystem) { debuggingSystem_ = debuggingSystem; }
    
    // Monitoring control
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const { return monitoring_; }
    
    // Reporting
    std::string getComprehensiveReport() const;
    std::string getPerformanceReport() const;
    std::string getAnomalyReport() const;
    std::string getStateReport() const;
    
    // Configuration
    void setMonitoringInterval(double interval) { monitoringInterval_ = interval; }
    void setReportFormat(const std::string& format) { reportFormat_ = format; }
    
    // Export
    bool exportReports(const std::string& directory) const;
    
private:
    std::vector<std::shared_ptr<EventLogger>> eventLoggers_;
    std::vector<std::shared_ptr<StateTracker>> stateTrackers_;
    std::vector<std::shared_ptr<AnomalyDetector>> anomalyDetectors_;
    
    std::shared_ptr<DebuggingSystem> debuggingSystem_;
    
    bool monitoring_;
    double monitoringInterval_;
    std::string reportFormat_;
    
    std::thread monitoringThread_;
    std::mutex monitorMutex_;
    
    // Helper methods
    void monitoringLoop();
    std::string generateComprehensiveReport() const;
    std::string generatePerformanceReport() const;
    std::string generateAnomalyReport() const;
    std::string generateStateReport() const;
};

} // namespace nlm
