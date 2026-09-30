// SpikeVisualization.hpp - Spike train visualization for NLM

#pragma once

#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Spike train visualization for monitoring and analysis of neural activity patterns
class SpikeVisualization {
public:
    SpikeVisualization(std::shared_ptr<Logger> logger);
    ~SpikeVisualization();
    
    // Initialize spike visualization system
    bool initialize();
    
    // Render spike raster plot (simultaneous spike times across neurons)
    void renderSpikeRaster(const Brain& brain, 
                          double timeWindow = 10.0, 
                          double binWidth = 0.001,
                          bool normalize = true,
                          const std::string& title = "Spike Raster Plot");
    
    // Render spike frequency histogram for individual neurons
    void renderSpikeFrequency(const Brain& brain,
                             double timeWindow = 60.0,
                             double binWidth = 0.01,
                             NeuronId neuronId = std::numeric_limits<NeuronId>::max(),
                             const std::string& title = "Spike Frequency");
    
    // Render population activity heat map
    void renderPopulationActivity(const Brain& brain,
                                  NeuronType neuronType = NeuronType::Interneuron,
                                  double timeWindow = 30.0,
                                  const std::string& title = "Population Activity");
    
    // Render spike trains for specific neurons
    void renderSpikeTimes(const std::vector<SpikeEvent>& spikes,
                         const std::string& title = "Spike Times",
                         double maxTime = 100.0);
    
    // Real-time spike monitoring
    void startRealTimeMonitoring(double updateInterval = 0.1);
    void stopRealTimeMonitoring();
    bool isMonitoring() const;
    
    // Configuration methods
    void setTimeWindow(double window);
    void setBinWidth(double binWidth);
    void enableNormalize(bool normalize);
    void setMaxTime(double maxTime);
    
    // Export capabilities
    bool exportSpikeData(const std::string& filename, const Brain& brain);
    bool exportRasterData(const std::string& filename, const Brain& brain);
    bool exportFrequencyData(const std::string& filename, const Brain& brain);
    
    // Statistical overlay on spike visualizations
    struct SpikeStats {
        float totalSpikes;
        float firingRate;
        float synchronizationIndex;
        float burstFrequency;
        float refractoryPeriod;
        float coefficientOfVariation;
        float meanISI;
        float stdISI;
        float skewISI;
        float kurtosisISI;
    };
    
    SpikeStats computeSpikeStatistics(const Brain& brain, double timeWindow = 10.0);
    
    // Pattern detection on spike trains
    struct BurstPattern {
        NeuronId neuronId;
        double startTime;
        double endTime;
        size_t spikeCount;
        float frequency;
        float synchrony;
    };
    
    std::vector<BurstPattern> detectBursts(const Brain& brain, double minISI = 5.0);
    
    // Inter-neuron connectivity from spike correlations
    struct ConnectivityMatrix {
        std::vector<std::vector<float>> correlationMatrix;
        std::vector<size_t> neuronIds;
        float threshold;
        std::string title;
    };
    
    ConnectivityMatrix computeConnectivityMatrix(const Brain& brain, float correlationThreshold = 0.5f);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

