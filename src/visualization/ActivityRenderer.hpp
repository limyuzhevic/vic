#pragma once

#include "Brain.hpp"
#include "ColorManager.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <map>

namespace nlm {

class ActivityRenderer {
public:
    enum class DisplayMode {
        RasterPlot,
        Histogram,
        Heatmap,
        Timeline,
        CrossSection,
        NetworkActivity,
        StatisticalOverlay
    };
    
    enum class AnalysisType {
        None,
        FiringRate,
        Synchrony,
        Bursting,
        Irregularity,
        Correlations
    };
    
    struct SpikeData {
        size_t neuronId;
        Timestamp time;
        SimulationStep step;
        float amplitude;
        
        SpikeData() : neuronId(0), time(0.0f), step(0), amplitude(0.0f) {}
        SpikeData(size_t id, Timestamp t, SimulationStep s, float amp = 1.0f)
            : neuronId(id), time(t), step(s), amplitude(amp) {}
    };
    
    struct ActivityStats {
        float mean;
        float variance;
        float skewness;
        float kurtosis;
        float stddev;
        size_t min;
        size_t max;
        
        ActivityStats() : mean(0.0f), variance(0.0f), skewness(0.0f), kurtosis(0.0f),
                         stddev(0.0f), min(0), max(0) {}
    };
    
    ActivityRenderer();
    ~ActivityRenderer() = default;
    
    void setDisplayMode(DisplayMode mode) { displayMode_ = mode; }
    void setAnalysisType(AnalysisType analysis) { analysisType_ = analysis; }
    void setTimeWindow(float seconds) { timeWindow_ = seconds; }
    void setShowSpikes(bool show) { showSpikes_ = show; }
    void setShowHistograms(bool show) { showHistograms_ = show; }
    void setShowStatistics(bool show) { showStatistics_ = show; }
    void setUpdateInterval(float seconds) { updateInterval_ = seconds; }
    
    void render(const Brain& brain);
    void update(const Brain& brain);
    
    // Spike visualization
    void renderSpikes(const Brain& brain);
    void renderSpikeTrains(const Brain& brain);
    void renderRasterPlot(const Brain& brain);
    
    // Activity visualization
    void renderFiringRates(const Brain& brain);
    void renderPopulationActivity(const Brain& brain);
    void renderActivityHeatmap(const Brain& brain);
    
    // Time series analysis
    void renderTimeSeries(const Brain& brain);
    void renderLearningCurve(const Brain& brain);
    void renderProgressMetrics(const Brain& brain);
    
    // Statistical overlays
    void renderStatisticalOverlays(const Brain& brain);
    void renderFiringRateDistribution(const Brain& brain);
    void renderCrossCorrelation(const Brain& brain);
    
    // Event visualization
    void renderEvents(const Brain& brain);
    void renderRewardEvents(const Brain& brain);
    void renderEpisodeMarkers(const Brain& brain);
    
    // Interactive features
    void setSelectedNeuron(size_t neuronId) { selectedNeuron_ = neuronId; }
    void highlightNeuron(size_t neuronId) { highlightedNeurons_.insert(neuronId); }
    void clearHighlights() { highlightedNeurons_.clear(); }
    void setTimeOffset(float offset) { timeOffset_ = offset; }
    
    // Data management
    void collectSpikeData(const Brain& brain);
    void collectActivityData(const Brain& brain);
    void updateStatistics();
    
    // Export and analysis
    std::string exportActivityData() const;
    void generateActivityReport() const;
    
    // Configuration
    void loadConfiguration(const std::string& configFile);
    void saveConfiguration() const;
    
    // Utility functions
    float getCurrentTime() const { return currentTime_; }
    size_t getCollectedSpikeCount() const { return spikeData_.size(); }
    float getMaxActivity() const { return maxActivity_; }
    
private:
    DisplayMode displayMode_;
    AnalysisType analysisType_;
    float timeWindow_;
    bool showSpikes_;
    bool showHistograms_;
    bool showStatistics_;
    float updateInterval_;
    
    std::vector<SpikeData> spikeData_;
    std::vector<float> activityHistory_;
    std::map<size_t, std::vector<float>> neuronActivity_;
    std::map<size_t, std::vector<SpikeData>> neuronSpikeTrains_;
    
    // Statistics
    ActivityStats firingRateStats_;
    ActivityStats synchronyStats_;
    ActivityStats burstingStats_;
    
    // Interactive state
    size_t selectedNeuron_;
    std::unordered_set<size_t> highlightedNeurons_;
    float timeOffset_;
    float currentTime_;
    
    // Configuration
    size_t maxSpikeHistory_;
    size_t maxActivityHistory_;
    bool smoothActivity_;
    float smoothingFactor_;
    std::string colorScheme_;
    
    // Time series buffers
    std::vector<Timestamp> timePoints_;
    std::vector<float> meanActivity_;
    std::vector<float> varianceActivity_;
    std::vector<float> firingRate_;
    
    void initializeDefaultConfiguration();
    void calculateStatistics();
    void updateTimeSeries();
    std::string getActivityColor(float activity) const;
    std::string getSpikeColor(const SpikeData& spike) const;
    std::string getStatisticColor(float value, float mean, float stddev) const;
    
    // Rendering helpers
    void renderHistogram(const std::vector<float>& data, int x, int y, int width, int height);
    void renderTimeline(const std::vector<Timestamp>& times, const std::vector<float>& values);
    void renderHeatmap(const std::map<size_t, std::vector<float>>& activityMap);
    void renderCrossSection(const Brain& brain);
    void renderStatisticalPlots() const;
    
    // Analysis functions
    void analyzeFiringRate(const Brain& brain);
    void analyzeSynchrony(const Brain& brain);
    void analyzeBursting(const Brain& brain);
    void analyzeCorrelations(const Brain& brain);
    
    // Utility functions
    float calculateMean(const std::vector<float>& data) const;
    float calculateVariance(const std::vector<float>& data, float mean) const;
    float calculateSkewness(const std::vector<float>& data, float mean, float variance) const;
    float calculateKurtosis(const std::vector<float>& data, float mean, float variance) const;
};

} // namespace nlm
