// Debug Manager for NLM v2.0+
// Centralized debugging infrastructure with event handling, tracing, and monitoring

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <deque>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <optional>

namespace nlm {

// Event types for debugging and monitoring
enum class DebugEventType {
    SimulationStep,
    NeuronSpike,
    SynapseChange,
    MemoryAccess,
    NeuromodulatorChange,
    PlasticityEvent,
    DevelopmentEvent,
    PredictionError,
    Custom
};

// Debug event structure
struct DebugEvent {
    DebugEventType type;
    std::chrono::system_clock::time_point timestamp;
    std::string source;
    std::string message;
    std::map<std::string, std::string> metadata;
    uint64_t step = 0;
    
    DebugEvent(DebugEventType t, const std::string& src, const std::string& msg)
        : type(t), source(src), message(msg), timestamp(std::chrono::system_clock::now()) {}
};

// Trace point for detailed debugging
struct TracePoint {
    std::chrono::system_clock::time_point timestamp;
    std::string eventType;
    std::string component;
    std::string details;
    uint64_t step = 0;
    int threadId = 0;
    
    TracePoint(const std::string& et, const std::string& comp, const std::string& det)
        : eventType(et), component(comp), details(det), timestamp(std::chrono::system_clock::now()) {}
};

// Performance metrics
struct PerformanceMetrics {
    std::chrono::duration<double> stepDuration;
    size_t neuronCount;
    size_t synapseCount;
    size_t spikeCount;
    double memoryUsage;
    double cpuUsage;
    std::chrono::system_clock::time_point timestamp;
    uint64_t step = 0;
    
    PerformanceMetrics(size_t neurons, size_t synapses, size_t spikes, double memUsage, double cpuUsage)
        : neuronCount(neurons), synapseCount(synapses), spikeCount(spikes), 
          memoryUsage(memUsage), cpuUsage(cpuUsage), timestamp(std::chrono::system_clock::now()) {}
};

// Memory statistics
struct MemoryStats {
    size_t workingMemorySize;
    size_t workingMemoryUsed;
    size_t episodicMemorySize;
    size_t episodicMemoryUsed;
    size_t semanticMemorySize;
    size_t semanticMemoryUsed;
    size_t proceduralMemorySize;
    size_t proceduralMemoryUsed;
    size_t associativeMemorySize;
    size_t associativeMemoryUsed;
    std::chrono::system_clock::time_point timestamp;
    uint64_t step = 0;
    
    MemoryStats(size_t wms, size_t wmu, size_t ems, size_t emu, size_t sms, size_t smu,
                size_t pms, size_t pmu, size_t ams, size_t amu)
        : workingMemorySize(wms), workingMemoryUsed(wmu), episodicMemorySize(ems), episodicMemoryUsed(emu),
          semanticMemorySize(sms), semanticMemoryUsed(smu), proceduralMemorySize(pms), proceduralMemoryUsed(pmu),
          associativeMemorySize(ams), associativeMemoryUsed(amu), timestamp(std::chrono::system_clock::now()) {}
};

// Neuromodulator statistics
struct NeuromodulatorStats {
    double dopamine;
    double curiosity;
    double novelty;
    double acetylcholine;
    double norepinephrine;
    double serotonin;
    double nicotine;
    double orexin;
    std::chrono::system_clock::time_point timestamp;
    uint64_t step = 0;
    
    NeuromodulatorStats(double dop, double cur, double nov, double ach, double ne,
                       double ser, double nic, double ore)
        : dopamine(dop), curiosity(cur), novelty(nov), acetylcholine(ach), norepinephrine(ne),
          serotonin(ser), nicotine(nic), orexin(ore), timestamp(std::chrono::system_clock::now()) {}
};

// Prediction system statistics
struct PredictionStats {
    double predictionError;
    double predictionAccuracy;
    double confidence;
    double learningRate;
    size_t totalPredictions;
    size_t correctPredictions;
    std::chrono::system_clock::time_point timestamp;
    uint64_t step = 0;
    
    PredictionStats(double error, double accuracy, double conf, double lr,
                   size_t total, size_t correct)
        : predictionError(error), predictionAccuracy(accuracy), confidence(conf), learningRate(lr),
          totalPredictions(total), correctPredictions(correct), timestamp(std::chrono::system_clock::now()) {}
};

// Neural activity sample
struct NeuralActivity {
    uint64_t neuronId;
    double membranePotential;
    double threshold;
    double inputCurrent;
    double outputCurrent;
    bool isSpiking;
    std::chrono::system_clock::time_point timestamp;
    uint64_t step = 0;
    
    NeuralActivity(uint64_t id, double v, double t, double i_in, double i_out, bool spiking)
        : neuronId(id), membranePotential(v), threshold(t), inputCurrent(i_in), outputCurrent(i_out),
          isSpiking(spiking), timestamp(std::chrono::system_clock::now()) {}
};

// Event listener function type
using EventListener = std::function<void(const DebugEvent&)>;

// Trace listener function type
using TraceListener = std::function<void(const TracePoint&)>;

// Performance metric listener function type
using PerformanceListener = std::function<void(const PerformanceMetrics&)>;

// Memory statistics listener function type
using MemoryListener = std::function<void(const MemoryStats&)>;

// Neuromodulator statistics listener function type
using NeuromodulatorListener = std::function<void(const NeuromodulatorStats&)>;

// Prediction statistics listener function type
using PredictionListener = std::function<void(const PredictionStats&)>;

// Neural activity listener function type
using NeuralActivityListener = std::function<void(const NeuralActivity&)>;

// Debug manager implementation
class DebugManager {
public:
    DebugManager();
    ~DebugManager() = default;
    
    // Event logging
    void logEvent(DebugEventType type, const std::string& source, const std::string& message);
    void logEvent(DebugEventType type, const std::string& source, const std::string& message,
                  const std::map<std::string, std::string>& metadata);
    void logEvent(DebugEventType type, const std::string& source, const std::string& message,
                  const std::map<std::string, std::string>& metadata, uint64_t step);
    
    // Trace recording
    void recordTrace(const std::string& eventType, const std::string& component, const std::string& details);
    void recordTrace(const std::string& eventType, const std::string& component, const std::string& details, uint64_t step);
    void recordTrace(const std::string& eventType, const std::string& component, const std::string& details,
                   uint64_t step, int threadId);
    
    // Performance monitoring
    void recordPerformance(const PerformanceMetrics& metrics);
    
    // Memory statistics
    void recordMemoryStats(const MemoryStats& stats);
    
    // Neuromodulator statistics
    void recordNeuromodulatorStats(const NeuromodulatorStats& stats);
    
    // Prediction statistics
    void recordPredictionStats(const PredictionStats& stats);
    
    // Neural activity monitoring
    void recordNeuralActivity(const NeuralActivity& activity);
    
    // Event listeners
    void addEventListener(const EventListener& listener);
    void removeEventListener(const EventListener& listener);
    
    // Trace listeners
    void addTraceListener(const TraceListener& listener);
    void removeTraceListener(const TraceListener& listener);
    
    // Performance metric listeners
    void addPerformanceListener(const PerformanceListener& listener);
    void removePerformanceListener(const PerformanceListener& listener);
    
    // Memory statistics listeners
    void addMemoryListener(const MemoryListener& listener);
    void removeMemoryListener(const MemoryListener& listener);
    
    // Neuromodulator statistics listeners
    void addNeuromodulatorListener(const NeuromodulatorListener& listener);
    void removeNeuromulatorListener(const NeuromodulatorListener& listener);
    
    // Prediction statistics listeners
    void addPredictionListener(const PredictionListener& listener);
    void removePredictionListener(const PredictionListener& listener);
    
    // Neural activity listeners
    void addNeuralActivityListener(const NeuralActivityListener& listener);
    void removeNeuralActivityListener(const NeuralActivityListener& listener);
    
    // Debug control
    void setDebugLevel(int level);
    int getDebugLevel() const;
    
    void enableTracing(const std::vector<std::string>& targets);
    void disableTracing(const std::vector<std::string>& targets);
    bool isTracing(const std::string& target) const;
    
    void enableProfiling(const std::vector<std::string>& targets);
    void disableProfiling(const std::vector<std::string>& targets);
    bool isProfiling(const std::string& target) const;
    
    // Event management
    void setEventBufferSize(size_t size);
    std::vector<DebugEvent> getRecentEvents(size_t count = 100);
    void clearEventHistory();
    
    // Trace management
    void setTraceBufferSize(size_t size);
    std::vector<TracePoint> getRecentTraces(size_t count = 1000);
    void clearTraceHistory();
    
    // Performance metrics management
    void setPerformanceBufferSize(size_t size);
    std::vector<PerformanceMetrics> getRecentPerformanceMetrics(size_t count = 100);
    void clearPerformanceHistory();
    
    // Memory statistics management
    void setMemoryStatsBufferSize(size_t size);
    std::vector<MemoryStats> getRecentMemoryStats(size_t count = 100);
    void clearMemoryStatsHistory();
    
    // Neuromodulator statistics management
    void setNeuromodulatorStatsBufferSize(size_t size);
    std::vector<NeuromodulatorStats> getRecentNeuromodulatorStats(size_t count = 100);
    void clearNeuromodulatorStatsHistory();
    
    // Prediction statistics management
    void setPredictionStatsBufferSize(size_t size);
    std::vector<PredictionStats> getRecentPredictionStats(size_t count = 100);
    void clearPredictionStatsHistory();
    
    // Neural activity management
    void setNeuralActivityBufferSize(size_t size);
    std::vector<NeuralActivity> getRecentNeuralActivity(size_t count = 1000);
    void clearNeuralActivityHistory();
    
    // Checkpointing
    bool saveDebugState(const std::string& filepath);
    bool loadDebugState(const std::string& filepath);
    
    // Event filtering
    void addEventFilter(DebugEventType type, const std::function<bool(const DebugEvent&)>& filter);
    void removeEventFilter(DebugEventType type);
    void clearEventFilters();
    
    // Export functionality
    std::string exportEvents(const std::string& format = "json") const;
    std::string exportTraces(const std::string& format = "json") const;
    std::string exportPerformanceMetrics(const std::string& format = "json") const;
    std::string exportMemoryStats(const std::string& format = "json") const;
    std::string exportNeuromodulatorStats(const std::string& format = "json") const;
    std::string exportPredictionStats(const std::string& format = "json") const;
    std::string exportNeuralActivity(const std::string& format = "json") const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Thread safety
    mutable std::mutex mutex;
    std::condition_variable eventCv;
    
    // Event filtering and processing
    void processEvent(const DebugEvent& event);
    bool shouldFilterEvent(const DebugEvent& event) const;
    
    // Buffer management
    void cleanupBuffers();
    
    // Export helpers
    std::string exportToJSON(const std::vector<DebugEvent>& events) const;
    std::string exportToJSON(const std::vector<TracePoint>& traces) const;
    std::string exportToJSON(const std::vector<PerformanceMetrics>& metrics) const;
    std::string exportToJSON(const std::vector<MemoryStats>& stats) const;
    std::string exportToJSON(const std::vector<NeuromodulatorStats>& stats) const;
    std::string exportToJSON(const std::vector<PredictionStats>& stats) const;
    std::string exportToJSON(const std::vector<NeuralActivity>& activity) const;
    
    // Configuration
    void loadConfiguration(const std::string& filepath = "");
    void saveConfiguration(const std::string& filepath = "") const;
    
    // Event processing thread
    void eventProcessingThread();
    std::thread eventThread;
    std::atomic<bool> eventThreadRunning;
    std::deque<DebugEvent> eventQueue;
    
    // Buffer management
    std::deque<DebugEvent> eventHistory;
    std::deque<TracePoint> traceHistory;
    std::deque<PerformanceMetrics> performanceHistory;
    std::deque<MemoryStats> memoryHistory;
    std::deque<NeuromodulatorStats> neuromodulatorHistory;
    std::deque<PredictionStats> predictionHistory;
    std::deque<NeuralActivity> neuralActivityHistory;
    
    // Listeners
    std::vector<EventListener> eventListeners;
    std::vector<TraceListener> traceListeners;
    std::vector<PerformanceListener> performanceListeners;
    std::vector<MemoryListener> memoryListeners;
    std::vector<NeuromodulatorListener> neuromodulatorListeners;
    std::vector<PredictionListener> predictionListeners;
    std::vector<NeuralActivityListener> neuralActivityListeners;
    
    // Filters
    std::map<DebugEventType, std::vector<std::function<bool(const DebugEvent&)>>> eventFilters;
    
    // Configuration
    int debugLevel;
    std::vector<std::string> traceTargets;
    std::vector<std::string> profileTargets;
    size_t eventBufferSize;
    size_t traceBufferSize;
    size_t performanceBufferSize;
    size_t memoryStatsBufferSize;
    size_t neuromodulatorStatsBufferSize;
    size_t predictionStatsBufferSize;
    size_t neuralActivityBufferSize;
};

} // namespace nlm
