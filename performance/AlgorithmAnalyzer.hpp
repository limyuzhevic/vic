#pragma once

#include "AlgorithmAnalyzer.hpp"
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <set>
#include <map>
#include <unordered_set>
#include <iostream>

namespace nlm {

class AlgorithmAnalyzer {
public:
    enum ComplexityClass {
        CONSTANT,
        LOGARITHMIC,
        LINEAR,
        LINEARITHMIC,
        QUADRATIC,
        CUBIC,
        EXPONENTIAL,
        FACTORIAL
    };

    struct AlgorithmMetrics {
        std::string algorithmName;
        ComplexityClass complexity;
        double timeComplexity{O(1)};
        double spaceComplexity{O(1)};
        double cacheEfficiency{0.0};
        double memoryBandwidth{0.0};
        double computationalIntensity{0.0};
        size_t inputSize{0};
        double executionTime{0.0};
        size_t memoryUsage{0};
        size_t cacheMisses{0};
        size_t cacheHits{0};
        
        AlgorithmMetrics(const std::string& name) : algorithmName(name) {}
        
        double getCacheHitRate() const {
            size_t total = cacheHits + cacheMisses;
            return total > 0 ? static_cast<double>(cacheHits) / total : 0.0;
        }
    };

    AlgorithmAnalyzer();
    ~AlgorithmAnalyzer();

    // Disable copying, enable moving
    AlgorithmAnalyzer(const AlgorithmAnalyzer&) = delete;
    AlgorithmAnalyzer& operator=(const AlgorithmAnalyzer&) = delete;
    AlgorithmAnalyzer(AlgorithmAnalyzer&&) noexcept;
    AlgorithmAnalyzer& operator=(AlgorithmAnalyzer&&) noexcept;

    // Analysis methods
    ComplexityClass analyzeComplexity(const std::string& algorithmName,
        const std::function<void()>& implementation);

    ComplexityClass analyzeTimeComplexity(const std::string& algorithmName,
        size_t inputSize) const;

    ComplexityClass analyzeSpaceComplexity(const std::string& algorithmName,
        size_t inputSize) const;

    // Specialized analyses
    template<typename Iterator>
    double analyzeTemporalLocality(const Iterator& begin, const Iterator& end) const;

    template<typename Iterator>
    double analyzeSpatialLocality(const Iterator& begin, const Iterator& end) const;

    template<typename Iterator>
    double analyzeCacheEfficiency(const Iterator& begin, const Iterator& end) const;

    template<typename Iterator>
    double analyzeMemoryBandwidth(const Iterator& begin, const Iterator& end) const;

    // Algorithm identification
    ComplexityClass identifyComplexity(double timeMultiples, size_t inputSize) const;

    // Performance profiling
    void profileAlgorithm(const std::string& algorithmName,
        const std::function<void()>& implementation,
        size_t inputSize);

    void profileForInputSizes(const std::string& algorithmName,
        const std::function<void(size_t)>& implementation,
        const std::vector<size_t>& sizes);

    // Comparative analysis
    void compareAlgorithms(const std::vector<std::string>& algorithmNames,
        const std::vector<std::function<void(size_t)>>& implementations,
        size_t inputSize);

    // Optimization recommendations
    std::vector<std::string> getOptimizationRecommendations(
        const AlgorithmMetrics& metrics) const;

    // Learning and adaptation
    void learnFromPerformanceData(const AlgorithmMetrics& metrics);
    void updateComplexityEstimates(const std::string& algorithmName,
        const std::vector<double>& executionTimes,
        const std::vector<size_t>& inputSizes);

    // Data management
    void saveResults(const std::string& filename) const;
    bool loadResults(const std::string& filename);

    // Statistics
    size_t getProfileCount(const std::string& algorithmName) const;
    double getAverageExecutionTime(const std::string& algorithmName) const;
    double getBestExecutionTime(const std::string& algorithmName) const;
    double getWorstExecutionTime(const std::string& algorithmName) const;

    // Analysis helpers
    std::string complexityToString(ComplexityClass complexity) const;
    std::string generateAnalysisReport() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
