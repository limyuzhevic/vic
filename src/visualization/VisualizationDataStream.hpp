#pragma once

#include <string>
#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <chrono>
#include "../core/Types/Types.hpp"
#include "VisualizationBase.hpp"

namespace nlm {

/**
 * @struct VisualizationDataFrame
 * @brief Container for visualization data at a specific point in time
 * 
 * This structure holds all the data needed for a single visualization frame.
 * It includes neural data, memory state, plasticity information, and timestamps.
 */
struct VisualizationDataFrame {
    uint64_t step;
    Timestamp timestamp;
    double systemTime;
    
    // Neural data
    std::vector<NeuronState> neuronStates;
    std::vector<SynapseWeight> synapseWeights;
    std::vector<NeuronId> activeNeurons;
    std::vector<std::pair<NeuronId, NeuronId>> activeSynapses;
    
    // Memory system data
    std::vector<MemoryEntry> memoryEntries;
    std::vector<MemoryId> workingMemoryIds;
    std::vector<MemoryId> episodicMemoryIds;
    
    // Plasticity data
    std::vector<PlasticityEvent> plasticityEvents;
    std::vector<std::pair<NeuronId, NeuronId>> stdpConnections;
    std::vector<NeuronId> hebbianConnections;
    
    // Neuromodulation data
    std::vector<NeuromodulatorLevel> neuromodulatorLevels;
    std::vector<float> dopamineLevels;
    std::vector<float> curiosityLevels;
    std::vector<float> noveltyLevels;
    
    // Performance metrics
    double cpuUsage;
    double memoryUsage;
    uint32_t spikeCount;
    uint32_t firingRate;
};

/**
 * @class VisualizationDataStream
 * @brief Real-time data streaming system for visualization updates
 * 
 * This class provides a real-time data pipeline that continuously captures,
 * processes, and streams visualization data from the NLM simulation.
 * It supports compression, buffering, and efficient data transmission.
 */
class VisualizationDataStream {
public:
    VisualizationDataStream();
    ~VisualizationDataStream();
    
    // Data production
    void pushFrame(const VisualizationDataFrame& frame);
    void startProduction(const class Brain* brain);
    void stopProduction();
    
    // Data consumption
    bool hasData() const;
    bool getFrame(VisualizationDataFrame& frame);
    
    // Data processing
    void setCompressionLevel(uint32_t level) { compressionLevel_ = level; }
    uint32_t getCompressionLevel() const { return compressionLevel_; }
    
    void setBufferSize(size_t size) { bufferSize_ = size; }
    size_t getBufferSize() const { return bufferSize_; }
    
    void setMaxLatency(double maxLatency) { maxLatency_ = maxLatency; }
    double getMaxLatency() const { return maxLatency_; }
    
    // Performance monitoring
    uint64_t getTotalFramesProduced() const { return totalFramesProduced_; }
    uint64_t getTotalFramesConsumed() const { return totalFramesConsumed_; }
    double getAverageFrameSize() const { return averageFrameSize_; }
    double getProcessingLatency() const { return processingLatency_; }
    
    // Data statistics
    double getCompressionRatio() const { return compressionRatio_; }
    double getThroughput() const { return throughput_; }
    
    // Data filtering
    void setFilter(const std::function<bool(const VisualizationDataFrame&)>& filter);
    void clearFilter() { dataFilter_ = nullptr; }
    
    // Configuration
    void setUpdateFrequency(uint32_t frequency) { updateFrequency_ = frequency; }
    uint32_t getUpdateFrequency() const { return updateFrequency_; }
    
    // Health monitoring
    bool isHealthy() const;
    std::string getHealthReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    
    // Buffer management
    std::queue<VisualizationDataFrame> frameBuffer_;
    std::queue<VisualizationDataFrame> processedBuffer_;
    size_t bufferSize_;
    
    // Data processing
    uint32_t compressionLevel_;
    std::function<bool(const VisualizationDataFrame&)> dataFilter_;
    
    // Performance metrics
    uint64_t totalFramesProduced_;
    uint64_t totalFramesConsumed_;
    double averageFrameSize_;
    double compressionRatio_;
    double throughput_;
    double processingLatency_;
    double maxLatency_;
    
    // Timing
    uint32_t updateFrequency_;
    std::chrono::steady_clock::time_point lastFrameTime_;
    uint64_t frameCount_;
    double accumulatedLatency_;
};

} // namespace nlm
