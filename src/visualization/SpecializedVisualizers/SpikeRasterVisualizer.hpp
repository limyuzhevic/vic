#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <deque>
#include <array>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "core/Types/Types.hpp"
#include "visualization/VisualizationInterface.hpp"
#include "visualization/VisualizationEngine.hpp"

namespace nlm {

// Spike data for raster plots
struct SpikeData {
    NeuronId neuronId;
    NeuronType neuronType;
    Timestamp time;              // Spike time
    float amplitude;             // Spike amplitude
    float width;                 // Spike width (duration)
    float baseline;              // Baseline membrane potential
    float threshold;             // Threshold at which spike occurred
    float adaptationLevel;       // Adaptation level at spike time
    bool isBurst;                // Whether part of a burst
    uint32_t burstId;            // Burst identifier
    float interSpikeInterval;    // Interval since previous spike
    float instantaneousFiringRate; // Instantaneous firing rate
    
    SpikeData() : neuronId(), neuronType(NeuronType::Internal), time(0.0),
                  amplitude(0.0f), width(0.0f), baseline(0.0f), threshold(0.0f),
                  adaptationLevel(0.0f), isBurst(false), burstId(0),
                  interSpikeInterval(0.0f), instantaneousFiringRate(0.0f) {}
};

// Population activity statistics
struct PopulationActivity {
    NeuronType neuronType;       // Type of neurons in population
    std::vector<float> firingRates; // Firing rate time series
    std::vector<float> activities;   // Activity level time series
    std::vector<float> membranePotentials; // Membrane potential time series
    float meanFiringRate;        // Mean firing rate
    float stdFiringRate;         // Standard deviation of firing rates
    float minFiringRate;         // Minimum firing rate
    float maxFiringRate;         // Maximum firing rate
    float cvFiringRate;          // Coefficient of variation of firing rates
    std::vector<float> spikeCounts; // Spike count per time bin
    
    PopulationActivity() : neuronType(NeuronType::Internal),
                          meanFiringRate(0.0f), stdFiringRate(0.0f),
                          minFiringRate(0.0f), maxFiringRate(0.0f),
                          cvFiringRate(0.0f) {}
};

// Raster plot configuration
struct RasterPlotConfig {
    // Display options
    bool showSpikeTrains;        // Whether to show spike trains
    bool showNeuronLabels;       // Whether to show neuron labels
    bool showFiringRateOverlay;  // Whether to show firing rate overlay
    bool showRefractoryPeriods;  // Whether to show refractory periods
    bool showBurstPatterns;      // Whether to show burst patterns
    
    // Visual styling
    float spikeDotSize;          // Size of spike dots
    float lineWidth;             // Width of spike lines
    std::string spikeColor;      // Color for spikes
    std::string neuronColor;     // Color for neurons
    std::string labelColor;      // Color for labels
    std::string burstColor;      // Color for bursts
    std::string refractoryColor; // Color for refractory periods
    
    // Time display
    bool showTimeAxis;           // Whether to show time axis
    float timeScale;             // Time scale (seconds per pixel)
    float timeWindow;            // Time window to display
    bool autoScroll;             // Whether to auto-scroll with time
    
    // Population grouping
    bool groupByType;            // Whether to group neurons by type
    bool groupByRegion;          // Whether to group neurons by region
    bool showPopulationStats;    // Whether to show population statistics
    
    // Interaction
    bool enableZoom;             // Whether to enable zooming
    bool enablePan;              // Whether to enable panning
    bool enableSelection;        // Whether to enable neuron selection
    float selectionAlpha;        // Alpha for selected neurons
    
    // Analysis
    bool showStatistics;         // Whether to show statistical overlays
    bool showCorrelations;       // Whether to show cross-correlation
    bool showHistograms;          // Whether to show spike train histograms
    
    RasterPlotConfig() : showSpikeTrains(true), showNeuronLabels(false),
                         showFiringRateOverlay(true), showRefractoryPeriods(true),
                         showBurstPatterns(true), spikeDotSize(3.0f), lineWidth(1.0f),
                         spikeColor("#FF4444"), neuronColor("#888888"), labelColor("#FFFFFF"),
                         burstColor("#FFAA00"), refractoryColor("#8888FF"), showTimeAxis(true),
                         timeScale(0.01f), timeWindow(10.0f), autoScroll(true),
                         groupByType(true), groupByRegion(true), showPopulationStats(true),
                         enableZoom(true), enablePan(true), enableSelection(true),
                         selectionAlpha(0.5f), showStatistics(true), showCorrelations(false),
                         showHistograms(false) {}
};

// Spike raster visualizer class
class SpikeRasterVisualizer : public VisualizationInterface {
public:
    SpikeRasterVisualizer();
    ~SpikeRasterVisualizer();
    
    // VisualizationInterface interface
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    bool isActive() const override;
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    // Spike visualization methods
    void visualizeSpikes(const class Brain& brain);
    void setSpikeData(const std::vector<SpikeData>& spikes);
    void updateSpikes(const class Brain& brain);
    
    // Analysis methods
    void computePopulationActivity();
    void computeSpikeStatistics();
    void analyzeBurstPatterns();
    void computeCrossCorrelation();
    
    // Configuration
    void setConfig(const RasterPlotConfig& config);
    void setTimeWindow(Timestamp window);
    void setActivityScale(float min, float max);
    void enableAdvancedAnalysis(bool enable);
    
    // Interaction
    void zoom(float factor);
    void pan(float dx, float dy);
    void selectNeuron(NeuronId neuronId);
    void clearSelection();
    std::vector<NeuronId> getSelectedNeurons() const;
    
    // Export and analysis
    void exportSpikeData(const std::string& filename, const std::string& format = "csv");
    void exportAnalysisResults(const std::string& filename);
    std::vector<std::string> analyzeSpikePatterns() const;
    std::vector<std::string> analyzePopulationDynamics() const;
    
    // Performance optimization
    void enableLOD(bool enable);  // Level of detail
    void setMaxSpikeCount(size_t maxSpikes);
    void enableCompression(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal methods
    void updateSpikeTrains(const class Brain& brain);
    void updatePopulationStats();
    void renderTimeAxis();
    void renderSpikeTrains();
    void renderPopulationGroups();
    void renderNeuronLabels();
    void renderStatistics();
    void renderSelection();
    void renderLegend();
    void handleInteraction();
    
    // Utility methods
    void computeFiringRateEstimate(NeuronId neuronId, float timeWindow);
    void detectBursts(const std::vector<SpikeData>& spikes, std::map<uint32_t, std::vector<SpikeData>>& bursts);
    float computeCrossCorrelation(const std::vector<float>& series1, const std::vector<float>& series2) const;
    std::vector<float> computeHistogram(const std::vector<float>& data, size_t binCount = 20) const;
    std::string getNeuronColor(NeuronType type, bool selected) const;
    float getSpikeAmplitude(const SpikeData& spike) const;
    float getRefractoryDuration(const SpikeData& spike) const;
};

} // namespace nlm
