#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <unordered_map>
#include <algorithm>
#include "Visualizer.hpp"

namespace nlm {

class Brain;
class Neuron;
class SpikeEvent;

/**
 * Spike activity visualization system for NLM
 * 
 * Provides comprehensive visualization of spike activity,
 * firing patterns, and temporal dynamics including:
 * - Raster plots and peristimulus time histograms
 * - Spike trains and rate maps
 * - Cross-correlation and coherence analysis
 * - Population dynamics and ensemble activity
 * - Synchrony and correlation analysis
 * - Input-output mapping visualization
 * - Learning-related spike changes
 * - Reward-modulated firing patterns
 * 
 * Features:
 * - High-resolution temporal visualization
 * - Multi-electrode and population views
 * - Statistical overlay and analysis
 * - Real-time spike tracking
 * - Export in multiple formats
 * - Interactive time slicing
 * - Color-coded firing rates
 * 
 * Integration with:
 * - SpikeSystem and event recording
 * - Brain state monitoring
 * - Core visualization system
 * - Export systems
 * 
 * @see Visualizer
 * @see InteractiveVisualizer
 * @see NetworkVisualizer
 * @see DataExporter
 */
class SpikeVisualizer {
public:
    /**
     * Spike visualization mode enumeration
     */
    enum class VisualizationMode {
        RASTER_PLOT,               // Spike times as dots
        PERISTIMULUS_HISTOGRAM,     // Time bin histograms
        SPIKE_TRAIN,               // Continuous spike trains
        RATE_MAP,                  // Firing rate over space/time
        CROSS_CORRELATION,         // Cross-correlation between neurons
        COHERENCE,                 // Signal coherence
        POPULATION_DENSITY,        // 2D density plots
        SYNCHRONY,                 // Synchronization measures
        INPUT_OUTPUT_MAPPING,      // Input-output relationships
        TIME_SLICE,                // Selected time windows
        COMPARING,                 // Multiple conditions
        STATISTICS_OVERLAY         // Statistical distributions
    };

    /**
     * Time window definition
     */
    struct TimeWindow {
        double start = 0.0;
        double end = 0.0;
        std::string label;
        bool enabled = true;
    };

    /**
     * Spike bin definition
     */
    struct SpikeBin {
        int64_t timeBin;
        int64_t neuronId;
        double firingRate;
        int spikeCount;
        double rateInHz;
        double standardDeviation;
    };

    /**
     * Correlation data
     */
    struct CorrelationData {
        int64_t neuronId1;
        int64_t neuronId2;
        double crossCorrelation;
        double coherence;
        int lag;                   // Time lag in bins
        std::vector<double> lagValues;
        std::vector<double> correlationValues;
    };

    /**
     * Spike visualization configuration
     */
    struct SpikeVisualizationConfig {
        VisualizationMode mode = VisualizationMode::RASTER_PLOT;
        double timeScale = 1.0;           // Time scaling for display
        double binWidth = 0.001;          // Histogram bin width (seconds)
        bool showIndividualSpikes = true;
        bool showRateMaps = true;
        bool showStatisticalOverlays = true;
        bool enableRealTimeUpdates = false;
        double updateInterval = 0.1;      // Update interval (seconds)
        std::string colorScheme = "default"; // default, fire, cool, cyclic
        double alpha = 1.0;               // Opacity for spikes
        bool showTimeScale = true;
        bool showTimeAxis = true;
        bool showNeuronLabels = false;
        bool enableSelection = true;
        std::vector<TimeWindow> timeWindows;
        bool enableCrossCorrelation = false;
        double correlationWindow = 1.0;  // Correlation window size
        bool showLearningRelated = false;
        bool showRewardRelated = false;
        double spikeThreshold = 0.0;      // Threshold for detection
        bool detectSpikes = true;         // Enable spike detection
        bool enableExport = true;
        double animationSpeed = 1.0;
    };

    /**
     * Spike statistics
     */
    struct SpikeStats {
        size_t totalSpikes = 0;
        size_t totalSpikingNeurons = 0;
        double meanFiringRate = 0.0;
        double maxFiringRate = 0.0;
        double minFiringRate = 0.0;
        double cvFiringRate = 0.0;        // Coefficient of variation
        double mean synchrony = 0.0;
        double meanCrossCorrelation = 0.0;
        std::unordered_map<int64_t, double> neuronStats; // neuronId -> firing rate
        std::vector<std::pair<int64_t, int64_t>> synchronizedPairs; // neuronId pairs
    };

    /**
     * SpikeVisualizer constructor
     */
    SpikeVisualizer();
    
    /**
     * SpikeVisualizer constructor with configuration
     */
    explicit SpikeVisualizer(const SpikeVisualizationConfig& config);
    
    /**
     * SpikeVisualizer destructor
     */
    ~SpikeVisualizer();

    /**
     * Configure spike visualization
     */
    void configure(const SpikeVisualizationConfig& config);
    
    /**
     * Initialize spike visualization
     */
    bool initialize(const Brain& brain);
    
    /**
     * Update visualization with new spike data
     */
    void update(const Brain& brain, const std::vector<SpikeEvent>& spikes, double timeStep);
    
    /**
     * Render spike visualization
     */
    bool render();
    
    /**
     * Export spike visualization
     */
    bool exportVisualization(const std::string& filename) const;
    
    /**
     * Generate spike analysis report
     */
    std::string generateSpikeAnalysis() const;
    
    /**
     * Get current spike statistics
     */
    SpikeStats getSpikeStats() const;
    
    /**
     * Get spike count for a neuron
     */
    size_t getSpikeCount(int64_t neuronId) const;
    
    /**
     * Get firing rate for a neuron
     */
    double getFiringRate(int64_t neuronId) const;
    
    /**
     * Get spike times for a neuron
     */
    std::vector<int64_t> getSpikeTimes(int64_t neuronId) const;
    
    /**
     * Get spike visualization mode
     */
    VisualizationMode getVisualizationMode() const;
    
    /**
     * Set spike visualization mode
     */
    void setVisualizationMode(VisualizationMode mode);
    
    /**
     * Set brain data source
     */
    void setBrain(const Brain& brain);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Suppress warnings about unused private members
    SpikeVisualizer(const SpikeVisualizer&) = delete;
    SpikeVisualizer& operator=(const SpikeVisualizer&) = delete;
};

} // namespace nlm
