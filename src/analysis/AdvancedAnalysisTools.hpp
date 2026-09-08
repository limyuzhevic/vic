#pragma once

/**
 * Advanced Analysis Tools for NLM
 * 
 * This module provides advanced analysis capabilities for neural network simulations,
 * including neural pattern analysis, network topology analysis, statistical analysis,
 * and visualization tools for power users.
 * 
 * These tools are designed to be optional and do not affect the core NLM functionality.
 * They provide enhanced capabilities for research, debugging, and performance optimization.
 */

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <map>
#include <string>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;

// Connectivity analysis structure
struct ConnectivityAnalysis {
    double averageDegree;
    double clusteringCoefficient;
    double density;
    double averagePathLength;
    double characteristicPathLength;
    double modularity;
    std::vector<double> degreeDistribution;
    std::map<std::string, double> efficiencyMetrics;
    
    ConnectivityAnalysis()
        : averageDegree(0.0), clusteringCoefficient(0.0), density(0.0),
          averagePathLength(0.0), characteristicPathLength(0.0), modularity(0.0) {}
};

// Neural activity statistics structure
struct NeuralActivityStatistics {
    float mean;
    float median;
    float stdDev;
    float variance;
    float skewness;
    float kurtosis;
    float min;
    float max;
    float percentile25;
    float percentile75;
    float percentile95;
    size_t sampleCount;
    
    NeuralActivityStatistics()
        : mean(0.0f), median(0.0f), stdDev(0.0f), variance(0.0f), skewness(0.0f),
          kurtosis(0.0f), min(0.0f), max(0.0f), percentile25(0.0f),
          percentile75(0.0f), percentile95(0.0f), sampleCount(0) {}
};

// Neural pattern structure
struct NeuralPattern {
    std::vector<float> firingRate;
    std::vector<Timestamp> spikeTimes;
    std::vector<float> phaseOscillation;
    float burstIndex;
    float synchronyIndex;
    float irregularity;
    float meanFiringRate;
    float cvFiringRate;  // Coefficient of variation
    
    NeuralPattern(size_t size = 100)
        : firingRate(size, 0.0f), phaseOscillation(size, 0.0f),
          burstIndex(0.0f), synchronyIndex(0.0f),
          irregularity(0.0f), meanFiringRate(0.0f), cvFiringRate(0.0f) {}
};

// Clustering result structure
struct ClusteringResult {
    std::vector<std::vector<NeuronId>> clusters;
    size_t noisePointCount;
    double silhouetteScore;
    std::string algorithm;
    
    ClusteringResult()
        : noisePointCount(0), silhouetteScore(0.0), algorithm("DBSCAN") {}
};

// AdvancedAnalysisTools class
class AdvancedAnalysisTools {
public:
    /**
     * Perform DBSCAN-like clustering on neurons based on their activity patterns
     * 
     * @param brain The brain instance containing neurons to analyze
     * @param eps The maximum distance between two samples for them to be considered
     *           as in the same neighborhood
     * @param minSamples The minimum number of samples in a neighborhood for a point
     *                   to be considered a core point
     * @return ClusteringResult containing the clustering results
     */
    static ClusteringResult clusterNeuronsByActivity(
        const Brain& brain,
        double eps = 2.0,
        size_t minSamples = 5);
    
    /**
     * Perform network topology analysis on the brain network
     * 
     * @param brain The brain instance to analyze
     * @return ConnectivityAnalysis containing network topology metrics
     */
    static ConnectivityAnalysis analyzeNetworkTopology(const Brain& brain);
    
    /**
     * Analyze neural activity patterns in a specific region
     * 
     * @param brain The brain instance
     * @param regionId The ID of the region to analyze
     * @return NeuralPattern containing the analyzed patterns
     */
    static NeuralPattern analyzeNeuralPatterns(const Brain& brain, NeuronId regionId);
    
    /**
     * Compute statistical measures on neural activity data
     * 
     * @param data Vector of neural activity values
     * @return NeuralActivityStatistics containing statistical measures
     */
    static NeuralActivityStatistics computeNeuralStatistics(const std::vector<float>& data);
    
    /**
     * Detect activity bursts in neural time series data
     * 
     * @param data Neural activity time series
     * @param threshold Burst detection threshold
     * @return Vector of burst intervals (start, end timestamps)
     */
    static std::vector<std::pair<Timestamp, Timestamp>> detectActivityBursts(
        const std::vector<float>& data,
        float threshold = 2.0f);
    
    /**
     * Compute correlation matrix between different neural populations
     * 
     * @param brain The brain instance
     * @param populationIds Vector of population IDs to analyze
     * @return Correlation matrix as 2D vector
     */
    static std::vector<std::vector<double>> computePopulationCorrelation(
        const Brain& brain,
        const std::vector<PopulationId>& populationIds);
    
    /**
     * Generate time-frequency representation of neural activity
     * 
     * @param brain The brain instance
     * @param regionId Region to analyze
     * @param samplingRate Sampling rate for analysis
     * @return 2D vector representing time-frequency power
     */
    static std::vector<std::vector<float>> computeTimeFrequency(
        const Brain& brain,
        NeuronId regionId,
        float samplingRate = 1000.0f);
    
    /**
     * Calculate network efficiency metrics
     * 
     * @param brain The brain instance
     * @return Map of efficiency metric names to values
     */
    static std::map<std::string, double> calculateNetworkEfficiency(const Brain& brain);
    
    /**
     * Perform dimensionality reduction on neural data
     * 
     * @param data Input neural data matrix
     * @param targetDim Target dimensionality
     * @return Reduced data matrix
     */
    static std::vector<std::vector<float>> performPCA(
        const std::vector<std::vector<float>>& data,
        size_t targetDim = 2);
    
    /**
     * Detect outliers in neural activity data
     * 
     * @param data Neural activity data
     * @param method Outlier detection method ("zscore", "iqr", "isolation")
     * @return Vector of outlier flags (true if point is outlier)
     */
    static std::vector<bool> detectOutliers(
        const std::vector<float>& data,
        const std::string& method = "zscore",
        float threshold = 2.0f);
    
    /**
     * Compute Granger causality between neural signals
     * 
     * @param signal1 First neural signal
     * @param signal2 Second neural signal
     * @param maxLag Maximum lag for causality analysis
     * @return Granger causality value
     */
    static double computeGrangerCausality(
        const std::vector<float>& signal1,
        const std::vector<float>& signal2,
        size_t maxLag = 10);
    
    /**
     * Generate comprehensive neural activity report
     * 
     * @param brain The brain instance
     * @return String containing formatted analysis report
     */
    static std::string generateActivityReport(const Brain& brain);
    
    /**
     * Export analysis results to file
     * 
     * @param results Analysis results to export
     * @param filepath Output file path
     * @param format Export format ("csv", "json", "text")
     * @return Success status
     */
    static bool exportResults(const std::string& results, 
                             const std::string& filepath,
                             const std::string& format = "text");
};

} // namespace nlm
