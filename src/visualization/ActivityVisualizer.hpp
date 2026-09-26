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
#include <thread>
#include "core/Types/Types.hpp"
#include "visualization/VisualizationEngine.hpp"

namespace nlm {

// Activity data types for visualization
struct ActivitySample {
    Timestamp timestamp;
    SimulationStep step;
    NeuronId neuronId;
    NeuronType neuronType;
    MembranePotential membranePotential;
    FiringState firingState;
    float synapticCurrent;
    float inputCurrent;
    float adaptationVariable;
    std::vector<NeuronId> postsynapticNeurons;
    std::vector<float> postsynapticWeights;
    Neuromodulator neuromodulator;
    float rewardSignal;
    bool isActive;
    float firingRate;
    
    ActivitySample() : timestamp(0.0), step(0), neuronId(), neuronType(NeuronType::Internal),
                      membranePotential(0.0f), firingState(FiringState::Resting),
                      synapticCurrent(0.0f), inputCurrent(0.0f), adaptationVariable(0.0f),
                      neuromodulator(0.0f), rewardSignal(0.0f), isActive(false), firingRate(0.0f) {}
};

// Population activity statistics
struct PopulationStats {
    NeuronType type;
    uint32_t neuronCount;
    float meanFiringRate;
    float stdFiringRate;
    float meanMembranePotential;
    float stdMembranePotential;
    float meanSynapticCurrent;
    float meanInputCurrent;
    float minFiringRate;
    float maxFiringRate;
    float minMembranePotential;
    float maxMembranePotential;
    std::vector<float> firingRateHistory;
    std::vector<float> membranePotentialHistory;
    std::vector<float> activityHistory;
    
    PopulationStats() : type(NeuronType::Internal), neuronCount(0),
                       meanFiringRate(0.0f), stdFiringRate(0.0f),
                       meanMembranePotential(0.0f), stdMembranePotential(0.0f),
                       meanSynapticCurrent(0.0f), meanInputCurrent(0.0f),
                       minFiringRate(0.0f), maxFiringRate(0.0f),
                       minMembranePotential(0.0f), maxMembranePotential(0.0f) {}
};

// Time-series data for plotting
struct TimeSeriesData {
    std::deque<Timestamp> timestamps;
    std::deque<float> values;
    std::deque<float> errorBounds;
    std::string label;
    std::string color;
    float scaleMin;
    float scaleMax;
    bool smoothingEnabled;
    
    TimeSeriesData(size_t capacity = 1000) : scaleMin(0.0f), scaleMax(1.0f),
                                             smoothingEnabled(true) {
        timestamps.reserve(capacity);
        values.reserve(capacity);
        errorBounds.reserve(capacity);
    }
    
    void addSample(Timestamp ts, float value, float error = 0.0f) {
        timestamps.push_back(ts);
        values.push_back(value);
        errorBounds.push_back(error);
        
        if (timestamps.size() > timestamps.max_size()) {
            timestamps.pop_front();
            values.pop_front();
            errorBounds.pop_front();
        }
    }
    
    float getMean() const {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }
    
    float getMax() const {
        if (values.empty()) return 0.0f;
        return *std::max_element(values.begin(), values.end());
    }
    
    float getMin() const {
        if (values.empty()) return 0.0f;
        return *std::min_element(values.begin(), values.end());
    }
};

// Heatmap data for spatial activity visualization
struct HeatMapData {
    struct HeatMapCell {
        float x, y;
        float activity;    // Activity level (0.0 to 1.0)
        uint32_t neuronCount;
        float meanFiringRate;
        float meanPotential;
        float maxActivity;
        std::array<float, 256> histogram;  // Activity histogram
    };
    
    std::vector<HeatMapCell> cells;
    float width;      // Heatmap width
    float height;     // Heatmap height
    float cellSize;   // Size of each cell
    float minActivity;
    float maxActivity;
    Timestamp timeWindow;  // Time window for activity accumulation
    
    HeatMapData(float w = 100.0f, float h = 100.0f, float cellSize = 1.0f)
        : width(w), height(h), cellSize(cellSize), minActivity(0.0f), maxActivity(0.0f),
          timeWindow(10.0) {
        size_t cols = static_cast<size_t>(width / cellSize);
        size_t rows = static_cast<size_t>(height / cellSize);
        cells.resize(cols * rows);
        
        for (size_t i = 0; i < cells.size(); ++i) {
            cells[i].x = (i % cols) * cellSize + cellSize / 2;
            cells[i].y = (i / cols) * cellSize + cellSize / 2;
            cells[i].activity = 0.0f;
            cells[i].neuronCount = 0;
            cells[i].meanFiringRate = 0.0f;
            cells[i].meanPotential = 0.0f;
            cells[i].maxActivity = 0.0f;
        }
    }
    
    void updateActivity(float x, float y, float activity, uint32_t neuronCount = 1) {
        if (cellSize <= 0.0f) return;
        
        size_t col = static_cast<size_t>(x / cellSize);
        size_t row = static_cast<size_t>(y / cellSize);
        
        if (col < width / cellSize && row < height / cellSize) {
            size_t index = row * static_cast<size_t>(width / cellSize) + col;
            if (index < cells.size()) {
                // Update statistics
                cells[index].neuronCount += neuronCount;
                cells[index].activity = (cells[index].activity * cells[index].neuronCount +
                                         activity * neuronCount) / (cells[index].neuronCount + neuronCount);
                cells[index].maxActivity = std::max(cells[index].maxActivity, activity);
            }
        }
    }
    
    void normalizeActivity() {
        float maxAct = 0.0f;
        for (const auto& cell : cells) {
            maxAct = std::max(maxAct, cell.maxActivity);
        }
        
        if (maxAct > 0.0f) {
            for (auto& cell : cells) {
                cell.activity /= maxAct;
            }
        }
    }
};

// Spike train representation for raster plots
struct SpikeTrain {
    NeuronId neuronId;
    NeuronType neuronType;
    float x;  // Time position
    std::vector<float> spikeTimes;  // Spike times within current window
    std::vector<float> spikeAmplitudes;  // Spike amplitudes
    float firingRate;  // Estimated firing rate (spikes per second)
    float refractoryViolation;  // How much refractory period was violated
    float adaptationLevel;  // Spike-frequency adaptation level
    bool isBursting;  // Whether in burst mode
    float burstFrequency;  // Frequency of bursting
    
    SpikeTrain() : neuronId(), neuronType(NeuronType::Internal), x(0.0f),
                  firingRate(0.0f), refractoryViolation(0.0f), adaptationLevel(0.0f),
                  isBursting(false), burstFrequency(0.0f) {}
};

// Activity visualization configuration
struct ActivityVisualizationConfig {
    // Time display options
    bool showTimeAxis;               // Whether to show time axis
    bool showTimeLabels;             // Whether to show time labels
    float timeScale;                 // Time scale (seconds per pixel)
    float timeWindow;                // Time window to display
    bool autoScroll;                 // Whether to auto-scroll with time
    
    // Raster plot options
    bool showSpikeRasters;          // Whether to show spike raster plots
    bool showNeuronLabels;           // Whether to show neuron labels in raster
    float rasterDotSize;             // Size of spike dots
    std::string rasterColor;        // Color for raster spikes
    bool showRefractoryPeriods;      // Whether to show refractory periods
    float refractoryAlpha;           // Alpha for refractory visualization
    
    // Heatmap options
    bool showHeatmaps;              // Whether to show activity heatmaps
    bool showPopulationHeatmaps;    // Whether to show population-level heatmaps
    float heatmapOpacity;            // Opacity for heatmaps
    std::string heatmapColorMap;    // Color map for heatmaps
    bool smoothHeatmaps;            // Whether to smooth heatmaps
    
    // Membrane potential options
    bool showMembranePotentials;    // Whether to show membrane potentials
    bool showActionPotentials;      // Whether to show action potentials
    float potentialScale;           // Scale for membrane potential display
    std::string potentialColor;     // Color for potentials
    bool showVoltageTraces;         // Whether to show continuous voltage traces
    
    // Synaptic current options
    bool showSynapticCurrents;      // Whether to show synaptic currents
    float currentScale;             // Scale for current display
    std::string currentColor;       // Color for currents
    
    // Neuromodulation options
    bool showNeuromodulation;       // Whether to show neuromodulation signals
    bool showRewardSignals;         // Whether to show reward signals
    bool showPredictionErrors;      // Whether to show prediction errors
    float neuromodulationScale;      // Scale for neuromodulation display
    
    // Statistical analysis options
    bool showStatistics;            // Whether to show statistical overlays
    bool showFiringRateDistribution; // Whether to show firing rate distribution
    bool showCrossCorrelation;      // Whether to show cross-correlation
    float statisticsAlpha;          // Alpha for statistical overlays
    
    // Interaction options
    bool enableSelection;           // Whether to enable neuron selection
    bool enableZoom;                // Whether to enable zooming
    bool enablePan;                 // Whether to enable panning
    float selectionAlpha;           // Alpha for selected neurons
    
    ActivityVisualizationConfig() : showTimeAxis(true), showTimeLabels(true), timeScale(0.01f),
                                   timeWindow(10.0f), autoScroll(true), showSpikeRasters(true),
                                   showNeuronLabels(false), rasterDotSize(2.0f), rasterColor("#FF4444"),
                                   showRefractoryPeriods(true), refractoryAlpha(0.3f), showHeatmaps(true),
                                   showPopulationHeatmaps(true), heatmapOpacity(0.7f),
                                   heatmapColorMap("viridis"), smoothHeatmaps(true), showMembranePotentials(true),
                                   showActionPotentials(true), potentialScale(100.0f), potentialColor("#00FF00"),
                                   showVoltageTraces(false), showSynapticCurrents(true), currentScale(1.0f),
                                   currentColor("#0000FF"), showNeuromodulation(true), showRewardSignals(true),
                                   showPredictionErrors(true), neuromodulationScale(1.0f), showStatistics(true),
                                   showFiringRateDistribution(false), showCrossCorrelation(false),
                                   statisticsAlpha(0.5f), enableSelection(true), enableZoom(true),
                                   enablePan(true), selectionAlpha(0.5f) {}
};

// ActivityVisualizer class
class ActivityVisualizer : public VisualizationInterface {
public:
    ActivityVisualizer();
    ~ActivityVisualizer();
    
    // VisualizationInterface interface
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    bool isActive() const override;
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    // Activity visualization methods
    void visualizeActivity(const class Brain& brain);
    void visualizeSpikes(const class Brain& brain);
    void visualizeMembranePotentials(const class Brain& brain);
    void visualizeSynapticCurrents(const class Brain& brain);
    void visualizeHeatmaps(const class Brain& brain);
    void visualizeNeuromodulation(const class Brain& brain);
    
    // Time-series management
    void addTimeSeries(const std::string& name, const TimeSeriesData& data);
    void updateTimeSeries(const std::string& name, Timestamp timestamp, float value, float error = 0.0f);
    void removeTimeSeries(const std::string& name);
    
    // Heatmap management
    void updateHeatmap(const HeatMapData& heatmap);
    void setHeatmapResolution(float width, float height, float cellSize);
    
    // Configuration
    void setConfig(const ActivityVisualizationConfig& config);
    void setTimeWindow(Timestamp window);
    void setActivityScale(float min, float max);
    void enableAdvancedFeatures(bool enable);
    
    // Interaction
    void zoom(float factor);
    void pan(float dx, float dy);
    void selectNeuron(NeuronId neuronId);
    void clearSelection();
    std::vector<NeuronId> getSelectedNeurons() const;
    
    // Analysis and statistics
    void computePopulationStatistics(const class Brain& brain);
    void computeSpatioTemporalStatistics(const class Brain& brain);
    std::vector<PopulationStats> getPopulationStatistics() const;
    std::vector<std::string> analyzeActivityPatterns() const;
    
    // Export and analysis
    void exportActivityData(const std::string& filename, const std::string& format = "json");
    void exportSpikeTrains(const std::string& filename);
    
    // Performance optimization
    void enableLOD(bool enable);  // Level of detail
    void setMaxSpikeCount(size_t maxSpikes);
    void enableTimeSeriesCompression(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal methods
    void updateTimeSeriesData();
    void updateSpikeTrains(const class Brain& brain);
    void updateHeatmaps(const class Brain& brain);
    void updatePopulationStats(const class Brain& brain);
    void renderTimeAxis();
    void renderSpikeRasters();
    void renderHeatmaps();
    void renderMembranePotentials();
    void renderSynapticCurrents();
    void renderNeuromodulationSignals();
    void renderStatisticalOverlays();
    void renderSelection();
    void renderLegend();
    void handleInteraction();
    
    // Utility methods
    void addSampleToTimeSeries(const std::string& name, Timestamp timestamp, float value);
    void updateTimeSeriesBounds(const std::string& name);
    void normalizeHeatmapActivity();
    float getNeuronActivityLevel(const ActivitySample& sample) const;
    std::string getNeuronColor(NeuronType type, FiringState state, bool selected) const;
    float getSpikeAmplitude(const ActivitySample& sample) const;
};

} // namespace nlm
