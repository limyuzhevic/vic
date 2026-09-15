#pragma once

namespace nlm {

class VisualizationEngine;

class RasterVisualizer {
public:
    RasterVisualizer();
    ~RasterVisualizer();
    
    // Initialize the visualizer
    bool initialize(VisualizationEngine* engine);
    
    // Update spike raster data
    void update(const class Brain& brain, double timestep, const class SpikeSystem& spikeSystem);
    
    // Render spike raster plot
    void render();
    
    // Close and cleanup
    void close();
    
    // Configuration
    void setTimeWindow(double seconds);
    void setShowAllNeurons(bool show);
    void setShowByType(bool show);
    void setShowByRegion(bool show);
    void setColorByType(bool color);
    void setColorByRegion(bool color);
    void setHighlightBursts(bool highlight);
    void setShowInterSpikeIntervals(bool show);
    void setFilterNeurons(const std::vector<int>& neuronIds);
    void setFilterRegions(const std::vector<int>& regionIds);
    void setFilterTypes(const std::vector<NeuronType>& types);
    
    // Raster configuration
    void setBinWidth(double ms);
    void setBinHeight(double pixels);
    void setXRange(double min, double max);
    void setYRange(double min, double max);
    
    // Data export
    bool exportRasterData(const std::string& filepath, ExportFormat format);
    
    // Getters
    bool isActive() const;
    
    // Statistics
    struct RasterStats {
        size_t totalSpikes;
        size_t spikeCount;                    // Spikes in current time window
        double averageInterSpikeInterval;     // Average ISI (ms)
        double coefficientOfVariation;        // CV of ISIs
        size_t maxSpikesPerNeuron;            // Maximum spikes for any neuron
        size_t minSpikesPerNeuron;            // Minimum spikes for any neuron
        std::vector<size_t> neuronSpikeCounts; // Spike count per neuron
        std::vector<double> interSpikeIntervals; // All ISIs in window
        std::map<NeuronType, size_t> typeSpikeCounts; // Spikes by type
        std::map<int, size_t> regionSpikeCounts;     // Spikes by region
    };
    
    RasterStats getStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
