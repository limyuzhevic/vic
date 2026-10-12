#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <chrono>
#include <mutex>
#include <atomic>
#include <functional>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <complex>
#include "../experiments/Metrics.hpp"

namespace nlm {

// Advanced research metrics collection and analysis system
class ResearchMetrics {
public:
    // Metric categories
    enum class Category {
        Performance,      // Simulation performance metrics
        Learning,         // Learning progress metrics
        Memory,           // Memory system metrics
        Neuromodulation,  // Neuromodulation system metrics
        Cognition,        // Cognitive system metrics
        Development,      // Development system metrics
        Statistical,       // Statistical analysis metrics
        MachineLearning   // Machine learning metrics
    };

    // Statistical measures
    struct StatisticalSummary {
        double mean = 0.0;
        double variance = 0.0;
        double standardDeviation = 0.0;
        double median = 0.0;
        double min = 0.0;
        double max = 0.0;
        double range = 0.0;
        double skewness = 0.0;
        double kurtosis = 0.0;
        uint64_t count = 0;
    };

    // Time series data
    struct TimeSeries {
        std::string name;
        std::vector<double> values;
        std::vector<std::chrono::system_clock::time_point> timestamps;
        double samplingInterval = 1.0; // seconds
        Category category = Category::Performance;
    };

    // Pattern recognition metrics
    struct PatternMetrics {
        std::string patternName;
        double accuracy = 0.0;
        double precision = 0.0;
        double recall = 0.0;
        double f1Score = 0.0;
        double confidence = 0.0;
        uint64_t totalPredictions = 0;
        uint64_t correctPredictions = 0;
        uint64_t falsePositives = 0;
        uint64_t falseNegatives = 0;
        std::vector<double> confidenceScores;
    };

    // Learning progress metrics
    struct LearningProgress {
        std::string learningPhase;
        double progressValue = 0.0;
        double learningRate = 0.0;
        double retentionFactor = 0.0;
        double generalizationScore = 0.0;
        uint64_t epochsCompleted = 0;
        std::chrono::system_clock::time_point lastUpdate;
    };

    // Memory system metrics
    struct MemoryMetrics {
        uint64_t workingMemoryCapacity = 0;
        uint64_t workingMemoryUtilization = 0;
        uint64_t episodicMemoryCapacity = 0;
        uint64_t episodicMemoryStored = 0;
        double consolidationEffectiveness = 0.0;
        uint64_t replayEvents = 0;
        double memoryDecayRate = 0.0;
        std::vector<double> memoryRetentionValues;
    };

    // Neuromodulation system metrics
    struct NeuromodulationMetrics {
        double dopamineSignalStrength = 0.0;
        double dopamineTimingPrecision = 0.0;
        double curiositySignalStrength = 0.0;
        double noveltyDetectionAccuracy = 0.0;
        double predictionErrorMagnitude = 0.0;
        double attentionFocusScore = 0.0;
        double modulationEffectiveness = 0.0;
        std::vector<double> signalTimings;
    };

    // Cognition system metrics
    struct CognitionMetrics {
        double conceptFormationAccuracy = 0.0;
        double planningAccuracy = 0.0;
        double attentionEffectiveness = 0.0;
        double workingMemoryLoad = 0.0;
        double cognitiveFlexibility = 0.0;
        uint64_t conceptCount = 0;
        uint64_t planExecutions = 0;
        double cognitiveReserve = 0.0;
    };

    // Development system metrics
    struct DevelopmentMetrics {
        double developmentalStageProgress = 0.0;
        double plasticityRate = 0.0;
        double maturationProgress = 0.0;
        double stageTransitionSmoothness = 0.0;
        uint64_t stageTransitionCount = 0;
        double criticalPeriodEffectiveness = 0.0;
        double experienceDependentPlasticity = 0.0;
    };

    // Custom research metric
    struct CustomMetric {
        std::string name;
        std::string description;
        Category category;
        std::function<double()> calculator;
        std::chrono::system_clock::time_point lastCalculated;
    };

public:
    ResearchMetrics();
    ~ResearchMetrics();
    
    // Core metrics collection
    void record(const std::string& name, double value, Category category = Category::Performance);
    void recordTimeSeries(const std::string& name, double value, const std::chrono::system_clock::time_point& timestamp = std::chrono::system_clock::now());
    void recordPattern(const std::string& patternName, bool predictionCorrect, double confidence);
    void recordLearningProgress(const std::string& phase, double progress, double learningRate);
    void recordMemoryMetrics(const MemoryMetrics& metrics);
    void recordNeuromodulationMetrics(const NeuromodulationMetrics& metrics);
    void recordCognitionMetrics(const CognitionMetrics& metrics);
    void recordDevelopmentMetrics(const DevelopmentMetrics& metrics);
    
    // Statistical analysis
    StatisticalSummary calculateStatistics(const std::string& name) const;
    double calculateCorrelation(const std::string& name1, const std::string& name2) const;
    double calculateTrend(const std::string& name, int windowSize = 10) const;
    std::vector<std::string> findCorrelatedMetrics(const std::string& name, double threshold = 0.8) const;
    
    // Machine learning metrics
    void recordPatternMetrics(const PatternMetrics& metrics);
    void recordPredictionQuality(const std::string& predictionType, double accuracy, uint64_t sampleCount);
    void recordClassificationMetrics(const std::string& className, const std::vector<bool>& predictions, const std::vector<bool>& truths);
    
    // Research utilities
    std::vector<std::string> getMetricNames() const;
    std::vector<std::string> getMetricNames(Category category) const;
    std::vector<TimeSeries> getTimeSeries() const;
    std::vector<PatternMetrics> getPatternMetrics() const;
    std::vector<LearningProgress> getLearningProgress() const;
    
    // Analysis and reporting
    std::string generateReport() const;
    std::string generateStatisticalReport() const;
    std::string generateTimeSeriesReport() const;
    std::string generateMLReport() const;
    
    // Data export
    bool exportToCSV(const std::string& filepath) const;
    bool exportToJSON(const std::string& filepath) const;
    bool exportToMat(const std::string& filepath) const; // MATLAB format
    
    // Research tools
    void enableAblationMode(bool enable);
    void setParameter(const std::string& parameter, double value);
    double getParameter(const std::string& parameter) const;
    void reset();
    
    // Integration with existing Metrics system
    void integrateWithExistingMetrics(const Metrics& existingMetrics);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
