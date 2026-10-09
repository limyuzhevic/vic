// Activity analyzer for neural network analysis
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <queue>

namespace nlm {

struct TimeSeriesData {
    SimulationStep step;
    Timestamp timestamp;
    std::vector<float> neuronActivities;
    std::vector<float> neuronFiringRates;
    std::vector<float> membranePotentials;
    std::vector<size_t> spikeCounts;
    std::vector<float> neuromodulatorLevels;
};

struct PatternInfo {
    std::string patternId;
    std::string patternType;
    float occurrenceRate;
    std::vector<float> signature;
    float similarityScore;
    SimulationStep firstOccurrence;
    SimulationStep lastOccurrence;
    std::vector<SimulationStep> occurrenceSteps;
};

struct NetworkMetrics {
    SimulationStep step;
    Timestamp timestamp;
    float meanFiringRate;
    float varianceFiringRate;
    float standardDeviationFiringRate;
    float skewnessFiringRate;
    float kurtosisFiringRate;
    float synchronyLevel;
    float coherenceLevel;
    float correlationLength;
    std::vector<float> firingRateHistogram;
    std::map<size_t, float> firingRateBins;
    std::vector<float> regionalMetrics;
};

struct CrossRegionCorrelation {
    size_t sourceRegion;
    size_t targetRegion;
    float correlationCoefficient;
    float lag; // Time lag in steps
    bool isSignificant;
};

struct BurstInfo {
    size_t neuronId;
    SimulationStep startStep;
    SimulationStep endStep;
    float averageFiringRate;
    float peakFiringRate;
    float totalSpikes;
    std::vector<float> firingRateProfile;
};

class ActivityAnalyzer {
public:
    ActivityAnalyzer();
    ~ActivityAnalyzer();
    
    bool initialize(std::shared_ptr<Brain> brain);
    void shutdown();
    
    void update(const VisualizationData& data);
    void analyze(const TimeSeriesData& timeSeriesData);
    
    // Core analysis methods
    ActivityStats computeStats(const TimeSeriesData& data) const;
    std::vector<PatternInfo> detectPatterns(const TimeSeriesData& data) const;
    std::vector<CrossRegionCorrelation> computeCrossRegionCorrelations(const TimeSeriesData& data) const;
    std::vector<BurstInfo> detectBursts(const TimeSeriesData& data) const;
    NetworkMetrics computeNetworkMetrics(const TimeSeriesData& data) const;
    
    // Activity visualization
    void computeFiringRateMaps(const TimeSeriesData& data, std::vector<std::vector<float>>& rateMaps) const;
    void computeActivityHeatmaps(const TimeSeriesData& data, std::vector<std::vector<float>>& heatmaps) const;
    
    // Statistical analysis
    void computeProbabilityDistributions(const TimeSeriesData& data, 
                                        std::vector<float>& firingRateDist, 
                                        std::vector<float>& membranePotentialDist) const;
    void computeSpectralAnalysis(const TimeSeriesData& data, 
                                std::vector<float>& powerSpectrum,
                                std::vector<float>& dominantFrequencies) const;
    void computeRecurrenceAnalysis(const TimeSeriesData& data,
                                  float& determinism,
                                  float& laminarity,
                                  float& entropy) const;
    
    // Pattern recognition
    bool recognizePattern(const TimeSeriesData& data, const std::string& patternId) const;
    std::vector<PatternInfo> recognizeAllPatterns(const TimeSeriesData& data) const;
    void classifyPatternType(const TimeSeriesData& data, std::vector<std::string>& patternTypes) const;
    
    // Network dynamics analysis
    float computeSynchrony(const TimeSeriesData& data) const;
    float computeCoherence(const TimeSeriesData& data, size_t region1, size_t region2) const;
    float computeCorrelationLength(const TimeSeriesData& data) const;
    float computeGlobalEfficiency(const TimeSeriesData& data) const;
    float computeLocalEfficiency(const TimeSeriesData& data, size_t regionId) const;
    
    // Visualization data
    void prepareActivityVisualizationData(const TimeSeriesData& data, 
                                         std::vector<float>& activityData,
                                         std::vector<float>& regionalActivity) const;
    void prepareConnectivityVisualizationData(const TimeSeriesData& data,
                                            std::vector<std::vector<float>>& connectivityMatrix) const;
    
    // Analysis configuration
    void setAnalysisWindow(size_t windowSize);
    void setMinCorrelationThreshold(float threshold);
    void setMinPatternSignificance(float significance);
    void setDetectBursts(bool enable);
    void setComputeSpectral(bool enable);
    void setComputeRecurrence(bool enable);
    
    // Configuration
    size_t getAnalysisWindow() const;
    float getMinCorrelationThreshold() const;
    float getMinPatternSignificance() const;
    bool getDetectBursts() const;
    bool getComputeSpectral() const;
    bool getComputeRecurrence() const;
    
    // Data access
    std::vector<TimeSeriesData> getTimeSeriesData() const;
    std::vector<ActivityStats> getActivityStats() const;
    std::vector<NetworkMetrics> getNetworkMetrics() const;
    std::map<std::string, std::vector<PatternInfo>> getPatternDatabase() const;
    
    // Export
    void exportTimeSeriesData(const std::string& filepath) const;
    void exportActivityStats(const std::string& filepath) const;
    void exportPatternDatabase(const std::string& filepath) const;
    void exportNetworkMetrics(const std::string& filepath) const;
    
    // Visualization preparation
    void prepareForRealTimeVisualization(std::vector<float>& visualizationData) const;
    void prepareForStaticVisualization(const std::string& format, std::vector<float>& visualizationData) const;

private:
    std::shared_ptr<Brain> brain_;
    
    // Configuration
    size_t analysisWindow_;
    float minCorrelationThreshold_;
    float minPatternSignificance_;
    bool detectBursts_;
    bool computeSpectral_;
    bool computeRecurrence_;
    
    // Data storage
    std::vector<TimeSeriesData> timeSeriesData_;
    std::vector<ActivityStats> activityStats_;
    std::vector<NetworkMetrics> networkMetrics_;
    std::map<std::string, std::vector<PatternInfo>> patternDatabase_;
    
    // Sliding window for real-time analysis
    std::queue<TimeSeriesData> analysisWindow_;
    size_t maxWindowSize_;
    
    // Statistical computation helpers
    float computeMean(const std::vector<float>& values) const;
    float computeVariance(const std::vector<float>& values, float mean) const;
    float computeStandardDeviation(float variance) const;
    float computeSkewness(const std::vector<float>& values, float mean, float stddev) const;
    float computeKurtosis(const std::vector<float>& values, float mean, float stddev) const;
    
    // Correlation and coherence computation
    void computeCrossCorrelation(const std::vector<float>& signal1, 
                                const std::vector<float>& signal2,
                                std::vector<float>& crossCorrelation,
                                std::vector<float>& lags) const;
    
    // Pattern detection helpers
    void extractFeatures(const TimeSeriesData& data, std::vector<float>& features) const;
    float computePatternSimilarity(const std::vector<float>& pattern1, 
                                  const std::vector<float>& pattern2) const;
    void clusterPatterns(const std::vector<std::vector<float>>& featureVectors,
                        std::vector<size_t>& clusterAssignments) const;
    
    // Burst detection helpers
    void findBurstBoundaries(const std::vector<float>& firingRates,
                           std::vector<size_t>& burstStartIndices,
                           std::vector<size_t>& burstEndIndices) const;
    float computeBurstStatistics(const std::vector<float>& firingRates,
                                size_t startIdx, size_t endIdx,
                                float& avgRate, float& peakRate) const;
    
    // Spectral analysis helpers
    void computePowerSpectrum(const std::vector<float>& signal,
                             std::vector<float>& powerSpectrum,
                             float samplingRate = 1.0f) const;
    void computeFFT(const std::vector<float>& real, const std::vector<float>& imag,
                   std::vector<float>& spectrum) const;
    
    // Recurrence analysis helpers
    void computeRecurrencePlot(const std::vector<float>& signal,
                              std::vector<std::vector<bool>>& recurrencePlot) const;
    void computeRecurrenceQuantification(const std::vector<std::vector<bool>>& recurrencePlot,
                                       float& determinism,
                                       float& laminarity,
                                       float& entropy) const;
    
    // Visualization helpers
    void normalizeForVisualization(std::vector<float>& data, float minVal, float maxVal) const;
    void quantizeForVisualization(std::vector<float>& data, size_t numQuantiles) const;
    void applyColormap(const std::vector<float>& data, std::vector<Color>& colors) const;
    
    mutable std::mutex mutex_;
};
