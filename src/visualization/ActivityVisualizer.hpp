#pragma once

#include <string>
#include <vector>
#include <deque>
#include <memory>
#include <mutex>
#include <chrono>
#include <map>
#include <unordered_set>
#include "../brain/Brain.hpp"
#include "../brain/SpikeSystem.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Spike event for raster plot
struct SpikeEventInfo {
    Timestamp timestamp;
    NeuronId neuronId;
    NeuronType neuronType;
    RegionId regionId;
    std::string regionName;
    float membranePotential;
    bool isBurst;
    float burstFrequency;
};

// Membrane potential trace
struct MembranePotentialTrace {
    Timestamp timestamp;
    NeuronId neuronId;
    NeuronType neuronType;
    RegionId regionId;
    std::string regionName;
    float potential;
    float derivative; // Rate of change
    float thresholdCrossings;
};

// Firing rate heatmap data
struct FiringRateHeatmap {
    Timestamp timestamp;
    int x; // Spatial or population index
    int y; // Spatial or population index
    float firingRate;
    float confidence;
    std::string regionName;
    NeuronType neuronType;
};

// Brain-wide activity pattern
struct BrainActivityPattern {
    Timestamp timestamp;
    float globalFiringRate;
    float coherence; // Measure of synchronization
    float complexity; // Measure of information diversity
    float sparsity; // Proportion of active neurons
    std::map<RegionId, float> regionFiringRates;
    std::map<RegionId, float> regionCoherence;
    std::map<RegionId, float> regionComplexity;
};

class ActivityVisualizer {
public:
    ActivityVisualizer();
    ~ActivityVisualizer();
    
    // Initialize visualizer
    bool initialize();
    
    // Update activity data
    void update(double deltaTime);
    
    // Record spike events
    void recordSpike(const SpikeEventInfo& spike);
    void recordSpikes(const std::vector<SpikeEventInfo>& spikes);
    
    // Record membrane potential traces
    void recordMembranePotential(const MembranePotentialTrace& trace);
    void recordMembranePotential(const std::vector<MembranePotentialTrace>& traces);
    
    // Update firing rate heatmap
    void updateFiringRateHeatmap(const std::vector<FiringRateHeatmap>& heatmapData);
    
    // Update brain activity pattern
    void updateBrainActivityPattern(const BrainActivityPattern& pattern);
    
    // Render spike raster plot
    void renderSpikeRaster(const std::vector<SpikeEventInfo>& spikes);
    void renderSpikeRaster(const std::vector<SpikeEventInfo>& spikes, const std::string& title);
    
    // Render membrane potential traces
    void renderMembranePotentialTraces(const std::vector<MembranePotentialTrace>& traces);
    void renderMembranePotentialTraces(const std::vector<NeuronId>& neuronIds);
    
    // Render firing rate heatmap
    void renderFiringRateHeatmap(const std::vector<FiringRateHeatmap>& heatmapData);
    void renderFiringRateHeatmap(int width, int height, const std::vector<RegionId>& regions);
    
    // Render brain-wide activity pattern
    void renderActivityPattern(const BrainActivityPattern& pattern);
    
    // Configuration
    void setTimeWindow(double seconds);
    void setMaxSpikesPerNeuron(size_t maxSpikes);
    void setTraceHistorySize(size_t size);
    void setHeatmapResolution(int width, int height);
    void setAutoUpdate(bool autoUpdate);
    
    // Data access
    const std::vector<SpikeEventInfo>& getRecentSpikes() const { return recentSpikes; }
    const std::vector<MembranePotentialTrace>& getMembranePotentialTraces() const { return traces; }
    const std::vector<FiringRateHeatmap>& getHeatmapData() const { return heatmapData; }
    const BrainActivityPattern& getCurrentActivityPattern() const { return currentPattern; }
    
    // Statistics
    size_t getTotalSpikeCount() const;
    float getAverageFiringRate() const;
    float getPeakFiringRate() const;
    size_t getActiveNeuronCount() const;
    float getCoherence() const;
    
    // Export capabilities
    bool exportSpikeData(const std::string& filename);
    bool exportTimeSeriesData(const std::string& filename);
    bool exportActivityPattern(const std::string& filename);
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm