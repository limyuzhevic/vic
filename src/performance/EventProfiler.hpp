#pragma once

#include <chrono>
#include <map>
#include <string>
#include <vector>
#include <mutex>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <optional>
#include <queue>
#include <unordered_map>
#include <memory>

namespace nlm {

class MemoryMonitor;
class PerformanceProfiler;

class EventProfiler {
public:
    // Event types
    enum class EventType {
        SPIKE,                // Neuron spike events
        DELAYED_SPIKE,        // Delayed spike events
        PLASTICITY_UPDATE,    // Plasticity updates
        NEUROMODULATION,     // Neuromodulation signals
        MEMORY_ACCESS,        // Memory system access
        SYNAPTIC_EVENT,       // General synaptic events
        SYSTEM_EVENT          // System-level events
    };

    // Event record for profiling
    struct EventRecord {
        EventType type;
        uint64_t timestamp;
        SimulationStep step;
        std::string component;
        double processingTime;
        size_t queueSize;
        int threadId;
        std::map<std::string, std::string> metadata;
        uint64_t eventId;
    };

    // Event queue statistics
    struct QueueStats {
        std::string queueName;
        size_t currentSize{0};
        size_t maxSize{0};
        double avgProcessingTime{0.0};
        double totalProcessingTime{0.0};
        uint64_t processedCount{0};
        uint64_t droppedCount{0};
        double overflowRate{0.0};
    };

    // Spike delivery statistics
    struct SpikeDeliveryStats {
        SimulationStep step;
        size_t spikesScheduled{0};
        size_t spikesDelivered{0};
        double avgDelay{0.0};
        double processingTime{0.0};
        std::vector<double> delays;  // Histogram
    };

    // Priority analysis
    struct PriorityStats {
        int priorityLevel{0};
        size_t totalEvents{0};
        double avgProcessingTime{0.0};
        size_t highPriorityEvents{0};  // Priority 0-1
        size_t normalPriorityEvents{0};  // Priority 2-3
        size_t lowPriorityEvents{0};  // Priority 4+
    };

    EventProfiler();
    ~EventProfiler();

    // Configuration
    void setEnabled(bool enabled);
    void setMaxEventHistorySize(size_t maxEvents);
    void setSamplingInterval(double interval);

    // Event recording
    void recordEvent(EventType type, uint64_t timestamp, SimulationStep step,
                     const std::string& component, double processingTime,
                     size_t queueSize = 0, int threadId = -1,
                     const std::map<std::string, std::string>& metadata = {});
    void recordSpikeEvent(uint64_t sourceNeuron, uint64_t destNeuron,
                         uint64_t synapseId, float weight, bool isExcitatory,
                         SimulationStep step, Timestamp time,
                         size_t delaySteps);
    void recordDelayedSpikeEvent(uint64_t sourceNeuron, uint64_t destNeuron,
                                uint64_t synapseId, float weight, bool isExcitatory,
                                SimulationStep step, Timestamp time,
                                SimulationStep deliveryStep);
    void recordQueueOperation(const std::string& queueName,
                             size_t queueSize, bool isPush, bool isPop,
                             double processingTime);
    void recordPriorityEvent(int priority, uint64_t timestamp,
                           uint16_t eventType, const uint64_t* data = nullptr);

    // Integration with Brain
    void setBrainIntegration(std::shared_ptr<Brain> brain);
    void setProfiler(PerformanceProfiler* profiler);

    // Statistics queries
    const EventRecord* getLastEvent() const;
    std::vector<EventRecord> getEventsInTimeRange(uint64_t startTime, uint64_t endTime) const;
    const QueueStats* getQueueStats(const std::string& queueName) const;
    const SpikeDeliveryStats* getSpikeDeliveryStats(SimulationStep step) const;
    const PriorityStats* getPriorityStats(int priority) const;
    double getAverageProcessingTime(EventType type) const;
    double getQueueOverflowRate(const std::string& queueName) const;

    // Reporting
    void exportToJSON(const std::string& filename) const;
    void exportToCSV(const std::string& filename) const;
    std::string generateReport() const;

    // Control
    void start();
    void stop();
    void reset();
    bool isRunning() const;

private:
    bool enabled_;
    bool running_;
    std::atomic<uint64_t> nextEventId_{1};

    // Event storage
    std::vector<EventRecord> eventHistory_;
    mutable std::mutex eventHistoryMutex_;

    // Queue statistics
    std::map<std::string, QueueStats> queueStats_;
    mutable std::mutex queueStatsMutex_;

    // Spike delivery stats by step
    std::map<SimulationStep, SpikeDeliveryStats> spikeDeliveryStats_;
    mutable std::mutex spikeStatsMutex_;

    // Priority statistics
    std::map<int, PriorityStats> priorityStats_;
    mutable std::mutex priorityStatsMutex_;

    // Processing time tracking by type
    std::map<EventType, std::vector<double>> typeProcessingTimes_;
    mutable std::mutex typeTimesMutex_;

    // Integration references
    std::shared_ptr<Brain> brain_;
    PerformanceProfiler* profiler_;

    // Control
    std::chrono::high_resolution_clock::time_point startTime_;
    double samplingInterval_;

    // Helper functions
    void cleanupOldEvents();
    void updateQueueStats(const std::string& queueName, size_t size, bool isPush, bool isPop, double processingTime);
    void updateSpikeDeliveryStats(const EventRecord& event);
    void updatePriorityStats(int priority);
    void generateReportInternal(std::ostream& output) const;
    void exportToJSONInternal(nlohmann::json& json) const;
    void exportToCSVInternal(std::ostream& output) const;
};
