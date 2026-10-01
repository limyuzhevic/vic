#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <complex>
#include <map>
#include <set>
#include <unordered_map>
#include "../brain/Brain.hpp"

namespace nlm {

// Forward declarations
class TimeSeriesAnalyzer;
class StatisticalAnalyzer;
class SpectralAnalyzer;
class PatternDetector;

// Data analyzer for neural simulation data
class DataAnalyzer {
public:
    DataAnalyzer();
    ~DataAnalyzer();

    // Initialize analyzer
    bool initialize();

    // Process brain data
    void processBrainData(const Brain& brain);

    // Get processed data
    std::vector<double> getTimeSeriesData(const std::string& metricName) const;
    double getStatistic(const std::string& metricName, const std::string& statType) const;
    std::complex<double> getSpectralComponent(const std::string& metricName, double frequency) const;
    std::vector<std::vector<double>> getCorrelationMatrix(const std::vector<std::string>& metricNames) const;
    std::vector<SpikeEvent> detectSpikes(const std::string& metricName, double threshold) const;
    std::vector<std::vector<double>> detectStateTransitions(const std::string& metricName) const;
    std::vector<std::vector<double>> extractFeatures(const std::string& metricName) const;

    // Export data
    bool exportData(const std::string& filename, const std::string& format = "csv") const;
    bool exportStatistics(const std::string& filename, const std::string& format = "json") const;
    bool exportAnalysisResults(const std::string& filename, const std::string& format = "hdf5") const;

    // Configuration
    void setAnalysisWindow(double windowSize);
    double getAnalysisWindow() const;

    void setSamplingRate(double rate);
    double getSamplingRate() const;

    void enableMetric(const std::string& metricName);
    void disableMetric(const std::string& metricName);
    bool isMetricEnabled(const std::string& metricName) const;

    void setStatisticalMethod(const std::string& method);
    std::string getStatisticalMethod() const;

    void enableSpectralAnalysis(bool enable);
    bool isSpectralAnalysisEnabled() const;

    void enablePatternDetection(bool enable);
    bool isPatternDetectionEnabled() const;

    void setSpikeDetectionThreshold(double threshold);
    double getSpikeDetectionThreshold() const;

    // Performance metrics
    double getProcessingTime() const;
    double getMemoryUsage() const;
    size_t getDataPointsProcessed() const;

    // Data management
    void clearData();
    size_t getDataPointCount() const;

    // Analysis results storage
    struct AnalysisResult {
        std::string metricName;
        std::string analysisType;
        std::chrono::steady_clock::time_point timestamp;
        std::vector<double> results;
        std::string description;

        AnalysisResult(const std::string& metric, const std::string& type)
            : metricName(metric), analysisType(type), timestamp(std::chrono::steady_clock::now()) {}
    };

    std::vector<AnalysisResult> getAnalysisResults() const;
    AnalysisResult getAnalysisResult(const std::string& metricName, const std::string& analysisType) const;

    // Statistical analysis
    struct Statistics {
        double mean;
        double variance;
        double standardDeviation;
        double median;
        double min;
        double max;
        double skewness;
        double kurtosis;
        std::vector<double> percentiles;
    };

    Statistics computeStatistics(const std::vector<double>& data) const;

    // Spectral analysis
    struct SpectralComponent {
        double frequency;
        std::complex<double> magnitude;
        double power;
        double phase;

        SpectralComponent(double freq, const std::complex<double>& mag)
            : frequency(freq), magnitude(mag), power(0.0), phase(0.0) {}
    };

    std::vector<SpectralComponent> computeFFT(const std::vector<double>& data, double samplingRate) const;

    // Pattern detection
    struct Pattern {
        std::string patternType;
        std::vector<double> parameters;
        double confidence;
        std::chrono::steady_clock::time_point detectedAt;
        std::vector<size_t> occurrenceIndices;

        Pattern(const std::string& type) : patternType(type), confidence(0.0) {}
    };

    std::vector<Pattern> detectPatterns(const std::vector<double>& data) const;

    // Performance profiler
    struct PerformanceProfile {
        double cpuTime;
        double memoryTime;
        double iowaitTime;
        size_t instructionsPerSecond;
        size_t memoryOperations;
        double cacheHitRate;
        double branchPredictionRate;
    };

    PerformanceProfile getPerformanceProfile() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm