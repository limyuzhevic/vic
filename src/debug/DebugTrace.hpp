#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>

namespace nlm {

// Debug event types
enum class DebugEventType {
    StepBegin,
    StepEnd,
    NeuronSpike,
    SynapseFire,
    MemoryAccess,
    PlasticityChange,
    Neuromodulation,
    PerformanceMetric,
    ResourceUsage,
    MemoryAllocation,
    MemoryDeallocation,
    Custom
};

// DebugEvent: Represents a single debug event in the NLM system
class DebugEvent {
public:
    DebugEvent();
    DebugEvent(DebugEventType type, const std::string& source, const std::string& message);
    
    // Basic properties
    DebugEventType getType() const { return type_; }
    const std::string& getSource() const { return source_; }
    const std::string& getMessage() const { return message_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    uint64_t getThreadId() const { return threadId_; }
    
    // Event data
    template<typename T>
    void setData(const std::string& key, const T& value);
    template<typename T>
    T getData(const std::string& key) const;
    bool hasData(const std::string& key) const;
    
    // Performance data
    double getExecutionTime() const { return executionTime_; }
    void setExecutionTime(double time) { executionTime_ = time; }
    
    // Additional properties
    int getLineNumber() const { return lineNumber_; }
    const std::string& getFileName() const { return fileName_; }
    const std::string& getFunctionName() const { return functionName_; }
    
    // Serialization
    std::string serialize() const;
    bool deserialize(const std::string& data);
    
    // Comparison
    bool operator==(const DebugEvent& other) const;
    bool operator!=(const DebugEvent& other) const { return !(*this == other); }
    
private:
    DebugEventType type_;
    std::string source_;
    std::string message_;
    std::chrono::system_clock::time_point timestamp_;
    uint64_t threadId_;
    double executionTime_;
    int lineNumber_;
    std::string fileName_;
    std::string functionName_;
    std::unordered_map<std::string, std::string> dataMap_;
    mutable std::mutex dataMutex_;
};

// DebugTrace: Detailed execution trace capture
class DebugTrace {
public:
    DebugTrace();
    ~DebugTrace();
    
    // Event management
    void addEvent(const DebugEvent& event);
    void addEvent(DebugEventType type, const std::string& source, const std::string& message);
    const std::vector<DebugEvent>& getEvents() const { return events_; }
    size_t getEventCount() const { return events_.size(); }
    
    // Filtering
    void addEventFilter(DebugEventType type, bool include = true);
    void clearEventFilters();
    bool shouldLogEvent(const DebugEvent& event) const;
    
    // Callbacks
    void addEventCallback(std::function<void(const DebugEvent&)> callback);
    void removeEventCallback(std::function<void(const DebugEvent&)> callback);
    void clearEventCallbacks();
    
    // Performance tracking
    void recordExecutionTime(const std::string& operation, double time);
    double getAverageExecutionTime(const std::string& operation) const;
    double getMaxExecutionTime(const std::string& operation) const;
    
    // State tracking
    void recordState(const std::string& stateName, const std::string& stateData);
    std::string getState(const std::string& stateName) const;
    
    // Serialization
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    std::string serialize() const;
    bool deserialize(const std::string& data);
    
    // Filtering
    void setMinSeverity(int minSeverity) { minSeverity_ = minSeverity; }
    int getMinSeverity() const { return minSeverity_; }
    
    void setMaxEvents(size_t maxEvents) { maxEvents_ = maxEvents; }
    size_t getMaxEvents() const { return maxEvents_; }
    
    // Time window filtering
    void setTimeWindow(double startTime, double endTime);
    bool isInTimeWindow(const DebugEvent& event) const;
    
    // Resource monitoring
    struct ResourceSnapshot {
        std::chrono::system_clock::time_point timestamp;
        double cpuUsage;
        double memoryUsage;
        size_t activeMemoryAllocations;
        size_t threadCount;
    };
    
    void recordResourceUsage(double cpuUsage, double memoryUsage, 
                           size_t memoryAllocations, size_t threadCount);
    const std::vector<ResourceSnapshot>& getResourceSnapshots() const { return resourceSnapshots_; }
    
private:
    std::vector<DebugEvent> events_;
    std::vector<std::function<void(const DebugEvent&)>> callbacks_;
    mutable std::mutex eventsMutex_;
    
    std::unordered_map<DebugEventType, bool> eventFilters_;
    int minSeverity_;
    size_t maxEvents_;
    
    double timeWindowStart_;
    double timeWindowEnd_;
    bool timeWindowEnabled_;
    
    std::unordered_map<std::string, std::vector<double>> executionTimes_;
    std::unordered_map<std::string, std::string> stateData_;
    
    std::vector<ResourceSnapshot> resourceSnapshots_;
};

} // namespace nlm
