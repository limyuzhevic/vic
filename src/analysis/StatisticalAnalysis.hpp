// StatisticalAnalysis.hpp - Statistical analysis of neural activity for NLM

#pragma once

#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>

namespace nlm {

// Statistical analysis framework for neural activity patterns and time series data
class StatisticalAnalysis {
public:
    StatisticalAnalysis(std::shared_ptr<Logger> logger);
    ~StatisticalAnalysis();
    
    // Initialize statistical analysis system
    bool initialize();
    
    // Compute comprehensive neural statistics over a time window
    struct NeuralStats {
        // Basic statistics
        float meanFiringRate;
        float stdFiringRate;
        float maxFiringRate;
        float minFiringRate;
        float medianFiringRate;
        float modeFiringRate;
        
        // Distribution properties
        std::vector<float> firingRateHistogram;
        std::vector<size_t> firingRateBins;
        
        // Time series analysis
        std::vector<float> autocorrelation;
        std::vector<float> autocovariance;
        std::vector<float> crossCorrelationMatrix;
        std::vector<float> partialCorrelationMatrix;
        
        // Synchronization measures
        float synchronyIndex;
        float pairwiseCorrelation;
        float global synchrony;
        float phaseSynchronization;
        
        // Burst statistics
        size_t burstCount;
        float burstFrequency;
        float burstDuration;
        float interBurstInterval;
        
        // Network-level statistics
        float excitationInhibitionRatio;
        float networkEfficiency;
        float clusteringCoefficient;
        
        // Performance metrics
        float signalToNoiseRatio;
        float informationThroughput;
        float codingAccuracy;
    };
    
    NeuralStats computeNeuralStatistics(const Brain& brain, double timeWindow = 10.0);
    
    // Population-level statistical analysis
    struct PopulationStats {
        // Population characteristics
        std::vector<float> meanActivity;
        std::vector<float> activityVariance;
        std::vector<float> activitySkewness;
        std::vector<float> activityKurtosis;
        
        // Inter-population relationships
        std::vector<std::vector<float>> crossCorrelation;
        std::vector<std::vector<float>> coherenceMatrix;
        std::vector<std::vector<float>> phaseLag;
        
        // Population synchrony
        float populationSynchrony;
        float crossPopulationS synchrony;
        
        // Dominance patterns
        std::vector<size_t> dominantPopulations;
        std::vector<float> populationInfluence;
        std::vector<size_t> influentialPopulations;
        
        // Spatial statistics
        std::vector<float> spatialCohesion;
        std::vector<float> spatialDispersion;
    };
    
    PopulationStats computePopulationStatistics(const Brain& brain);
    
    // Time series statistical analysis
    struct TimeSeriesStats {
        // Basic time series properties
        std::vector<float> mean;
        std::vector<float> variance;
        std::vector<float> skewness;
        std::vector<float> kurtosis;
        std::vector<float> median;
        std::vector<float> range;
        
        // Spectral analysis
        std::vector<float> powerSpectrum;
        float dominantFrequency;
        float spectralEntropy;
        float spectralSlope;
        
        // Temporal patterns
        std::vector<size_t> peaks;
        std::vector<size_t> troughs;
        std::vector<float> peakHeights;
        std::vector<float> troughDepths;
        
        // Non-stationarity measures
        float detrendedFluctuationAnalysis;
        float hurstExponent;
        float sampleEntropy;
        float approximateEntropy;
    };
    
    TimeSeriesStats computeTimeSeriesAnalysis(const std::vector<float>& timeSeries,
                                             float samplingRate = 1000.0f,
                                             size_t windowSize = 100);
    
    // Statistical tests and inference
    struct HypothesisTest {
        std::string testType; // "t-test", "anova", "chi-square", "ks-test", etc.
        float statistic;
        float pValue;
        float effectSize;
        std::string conclusion; // "reject_null", "fail_to_reject", "inconclusive"
    };
    
    HypothesisTest performStatisticalTest(const std::vector<float>& data1,
                                         const std::vector<float>& data2 = {}, 
                                         const std::string& testType = "t-test");
    
    std::vector<HypothesisTest> performANOVA(const std::vector<std::vector<float>>& groups);
    HypothesisTest performChiSquareTest(const std::vector<size_t>& observed,
                                        const std::vector<size_t>& expected);
    
    // Distribution fitting and modeling
    struct DistributionFit {
        std::string distributionName;
        std::map<std::string, float> parameters;
        float logLikelihood;
        float aic; // Akaike Information Criterion
        float bic; // Bayesian Information Criterion
        float ksStatistic;
        float pValue;
        bool isGoodFit;
    };
    
    DistributionFit fitDistribution(const std::vector<float>& data,
                                    const std::vector<std::string>& candidateDistributions);
    
    // Export statistical results
    bool exportStatistics(const std::string& filename, const NeuralStats& stats);
    bool exportTimeSeriesData(const std::string& filename, const TimeSeriesStats& stats);
    bool exportPopulationData(const std::string& filename, const PopulationStats& stats);
    bool exportHypothesisTests(const std::string& filename, const std::vector<HypothesisTest>& tests);
    
    // Configuration methods
    void setTimeWindow(double window);
    void setBinWidth(double binWidth);
    void enableNormalize(bool normalize);
    void setStatisticalSignificance(float alpha);
    void setConfidenceLevel(float confidence);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

