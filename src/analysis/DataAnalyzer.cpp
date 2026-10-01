#include "DataAnalyzer.hpp"
#include "../core/Logger/Logger.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <complex>
#include <set>
#include <unordered_map>
#include <thread>

namespace nlm {

struct DataAnalyzer::Impl {
    bool active;
    double analysisWindow;
    double samplingRate;
    double spikeThreshold;
    bool spectralEnabled;
    bool patternEnabled;
    std::string statisticalMethod;
    std::unordered_map<std::string, bool> metricsEnabled;
    std::unordered_map<std::string, std::vector<double>> timeSeriesData;
    std::vector<AnalysisResult> analysisResults;
    std::chrono::steady_clock::time_point lastUpdateTime;
    double processingTime;
    size_t memoryUsage;
    size_t dataPointsProcessed;
    PerformanceProfile performanceProfile;
    
    Impl() : active(false), analysisWindow(1.0), samplingRate(1000.0), spikeThreshold(1.0),
             spectralEnabled(false), patternEnabled(false), statisticalMethod("mean_variance"),
             processingTime(0.0), memoryUsage(0), dataPointsProcessed(0) {}
};

DataAnalyzer::DataAnalyzer() : pImpl(std::make_unique<Impl>()) {}

DataAnalyzer::~DataAnalyzer() = default;

bool DataAnalyzer::initialize() {
    // Initialize analyzer
    pImpl->active = true;
    NLM_LOG_INFO("DataAnalyzer: Initialized successfully");
    return true;
}

void DataAnalyzer::processBrainData(const Brain& brain) {
    auto startTime = std::chrono::steady_clock::now();
    
    // Collect data from brain
    if (pImpl->metricsEnabled["totalNeuronCount"]) {
        double metric = static_cast<double>(brain.getTotalNeuronCount());
        pImpl->timeSeriesData["totalNeuronCount"].push_back(metric);
    }
    
    if (pImpl->metricsEnabled["totalSpikeCount"]) {
        double metric = static_cast<double>(brain.getTotalSpikeCount());
        pImpl->timeSeriesData["totalSpikeCount"].push_back(metric);
    }
    
    if (pImpl->metricsEnabled["activeNeuronCount"]) {
        double metric = static_cast<double>(brain.getActiveNeuronCount());
        pImpl->timeSeriesData["activeNeuronCount"].push_back(metric);
    }
    
    if (pImpl->metricsEnabled["firingNeuronCount"]) {
        double metric = static_cast<double>(brain.getFiringNeuronCount());
        pImpl->timeSeriesData["firingNeuronCount"].push_back(metric);
    }
    
    if (pImpl->metricsEnabled["averageFiringRate"]) {
        double metric = brain.getAverageFiringRate();
        pImpl->timeSeriesData["averageFiringRate"].push_back(metric);
    }
    
    if (pImpl->metricsEnabled["eIratio"]) {
        double metric = brain.getExcitationInhibitionRatio();
        pImpl->timeSeriesData["eIratio"].push_back(metric);
    }
    
    pImpl->dataPointsProcessed++;
    
    auto endTime = std::chrono::steady_clock::now();
    pImpl->processingTime = std::chrono::duration<double>(endTime - startTime).count();
    pImpl->lastUpdateTime = endTime;
}

std::vector<double> DataAnalyzer::getTimeSeriesData(const std::string& metricName) const {
    auto it = pImpl->timeSeriesData.find(metricName);
    return (it != pImpl->timeSeriesData.end()) ? it->second : std::vector<double>();
}

double DataAnalyzer::getStatistic(const std::string& metricName, const std::string& statType) const {
    std::vector<double> data = getTimeSeriesData(metricName);
    if (data.empty()) return 0.0;
    
    if (statType == "mean") {
        return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
    }
    else if (statType == "median") {
        std::vector<double> sorted = data;
        std::sort(sorted.begin(), sorted.end());
        size_t middle = sorted.size() / 2;
        return sorted[middle];
    }
    else if (statType == "min") {
        return *std::min_element(data.begin(), data.end());
    }
    else if (statType == "max") {
        return *std::max_element(data.begin(), data.end());
    }
    else if (statType == "variance") {
        double mean = getStatistic(metricName, "mean");
        double sqSum = 0.0;
        for (double val : data) {
            sqSum += (val - mean) * (val - mean);
        }
        return sqSum / data.size();
    }
    else if (statType == "stddev") {
        double variance = getStatistic(metricName, "variance");
        return std::sqrt(variance);
    }
    
    return 0.0;
}

std::complex<double> DataAnalyzer::getSpectralComponent(const std::string& metricName, double frequency) const {
    // Mock spectral component - in real implementation would use FFT
    return std::complex<double>(0.0, 0.0);
}

std::vector<std::vector<double>> DataAnalyzer::getCorrelationMatrix(const std::vector<std::string>& metricNames) const {
    std::vector<std::vector<double>> matrix(metricNames.size(), std::vector<double>(metricNames.size(), 0.0));
    
    for (size_t i = 0; i < metricNames.size(); ++i) {
        for (size_t j = 0; j < metricNames.size(); ++j) {
            if (i == j) {
                matrix[i][j] = 1.0;  // Perfect correlation with itself
            }
            else {
                // Mock correlation - in real implementation would compute actual correlation
                matrix[i][j] = 0.5;
            }
        }
    }
    
    return matrix;
}

std::vector<SpikeEvent> DataAnalyzer::detectSpikes(const std::string& metricName, double threshold) const {
    std::vector<SpikeEvent> spikes;
    std::vector<double> data = getTimeSeriesData(metricName);
    
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i] > threshold && data[i-1] <= threshold) {
            SpikeEvent spike;
            spike.neuronId = 0;  // Mock neuron ID
            spike.time = i;
            spike.amplitude = data[i] - data[i-1];
            spike.duration = 0.001;  // Mock duration
            spikes.push_back(spike);
        }
    }
    
    return spikes;
}

std::vector<std::vector<double>> DataAnalyzer::detectStateTransitions(const std::string& metricName) const {
    std::vector<std::vector<double>> transitions;
    std::vector<double> data = getTimeSeriesData(metricName);
    
    for (size_t i = 0; i < data.size() - 1; ++i) {
        if (data[i] != data[i+1]) {
            transitions.push_back({static_cast<double>(i), data[i], data[i+1]});
        }
    }
    
    return transitions;
}

std::vector<std::vector<double>> DataAnalyzer::extractFeatures(const std::string& metricName) const {
    std::vector<std::vector<double>> features;
    std::vector<double> data = getTimeSeriesData(metricName);
    
    if (data.empty()) return features;
    
    // Extract basic features
    features.push_back({getStatistic(metricName, "mean")} );  // Mean
    features.push_back({getStatistic(metricName, "variance")});  // Variance
    features.push_back({getStatistic(metricName, "min")} );  // Minimum
    features.push_back({getStatistic(metricName, "max")} );  // Maximum
    
    return features;
}

bool DataAnalyzer::exportData(const std::string& filename, const std::string& format) const {
    NLM_LOG_INFO("DataAnalyzer: Exporting data to " + filename + "." + format);
    // Export implementation would go here
    return true;
}

bool DataAnalyzer::exportStatistics(const std::string& filename, const std::string& format) const {
    NLM_LOG_INFO("DataAnalyzer: Exporting statistics to " + filename + "." + format);
    // Export implementation would go here
    return true;
}

bool DataAnalyzer::exportAnalysisResults(const std::string& filename, const std::string& format) const {
    NLM_LOG_INFO("DataAnalyzer: Exporting analysis results to " + filename + "." + format);
    // Export implementation would go here
    return true;
}

void DataAnalyzer::setAnalysisWindow(double windowSize) {
    pImpl->analysisWindow = windowSize;
}

double DataAnalyzer::getAnalysisWindow() const {
    return pImpl->analysisWindow;
}

void DataAnalyzer::setSamplingRate(double rate) {
    pImpl->samplingRate = rate;
}

double DataAnalyzer::getSamplingRate() const {
    return pImpl->samplingRate;
}

void DataAnalyzer::enableMetric(const std::string& metricName) {
    pImpl->metricsEnabled[metricName] = true;
}

void DataAnalyzer::disableMetric(const std::string& metricName) {
    pImpl->metricsEnabled[metricName] = false;
}

bool DataAnalyzer::isMetricEnabled(const std::string& metricName) const {
    auto it = pImpl->metricsEnabled.find(metricName);
    return (it != pImpl->metricsEnabled.end()) && it->second;
}

void DataAnalyzer::setStatisticalMethod(const std::string& method) {
    pImpl->statisticalMethod = method;
}

std::string DataAnalyzer::getStatisticalMethod() const {
    return pImpl->statisticalMethod;
}

void DataAnalyzer::enableSpectralAnalysis(bool enable) {
    pImpl->spectralEnabled = enable;
}

bool DataAnalyzer::isSpectralAnalysisEnabled() const {
    return pImpl->spectralEnabled;
}

void DataAnalyzer::enablePatternDetection(bool enable) {
    pImpl->patternEnabled = enable;
}

bool DataAnalyzer::isPatternDetectionEnabled() const {
    return pImpl->patternEnabled;
}

void DataAnalyzer::setSpikeDetectionThreshold(double threshold) {
    pImpl->spikeThreshold = threshold;
}

double DataAnalyzer::getSpikeDetectionThreshold() const {
    return pImpl->spikeThreshold;
}

double DataAnalyzer::getProcessingTime() const {
    return pImpl->processingTime;
}

size_t DataAnalyzer::getMemoryUsage() const {
    return pImpl->memoryUsage;
}

size_t DataAnalyzer::getDataPointsProcessed() const {
    return pImpl->dataPointsProcessed;
}

void DataAnalyzer::clearData() {
    pImpl->timeSeriesData.clear();
    pImpl->analysisResults.clear();
    pImpl->dataPointsProcessed = 0;
}

size_t DataAnalyzer::getDataPointCount() const {
    size_t total = 0;
    for (const auto& pair : pImpl->timeSeriesData) {
        total += pair.second.size();
    }
    return total;
}

std::vector<DataAnalyzer::AnalysisResult> DataAnalyzer::getAnalysisResults() const {
    return pImpl->analysisResults;
}

DataAnalyzer::AnalysisResult DataAnalyzer::getAnalysisResult(const std::string& metricName, const std::string& analysisType) const {
    for (const auto& result : pImpl->analysisResults) {
        if (result.metricName == metricName && result.analysisType == analysisType) {
            return result;
        }
    }
    return AnalysisResult(metricName, analysisType);
}

DataAnalyzer::Statistics DataAnalyzer::computeStatistics(const std::vector<double>& data) const {
    Statistics stats;
    if (data.empty()) return stats;
    
    // Compute mean
    stats.mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
    
    // Compute variance
    double sqSum = 0.0;
    for (double val : data) {
        sqSum += (val - stats.mean) * (val - stats.mean);
    }
    stats.variance = sqSum / data.size();
    stats.standardDeviation = std::sqrt(stats.variance);
    
    // Compute min/max
    stats.min = *std::min_element(data.begin(), data.end());
    stats.max = *std::max_element(data.begin(), data.end());
    
    // Compute median
    std::vector<double> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    size_t middle = sorted.size() / 2;
    stats.median = sorted[middle];
    
    // For skewness and kurtosis, we would need more sophisticated calculations
    stats.skewness = 0.0;
    stats.kurtosis = 0.0;
    
    // Compute percentiles
    stats.percentiles = {25.0, 50.0, 75.0};  // Placeholder
    
    return stats;
}

std::vector<DataAnalyzer::SpectralComponent> DataAnalyzer::computeFFT(const std::vector<double>& data, double samplingRate) const {
    // Mock FFT implementation - in real implementation would use actual FFT
    std::vector<SpectralComponent> spectrum;
    
    size_t n = data.size();
    for (size_t i = 0; i < n / 2; ++i) {
        double frequency = i * samplingRate / n;
        std::complex<double> magnitude = std::complex<double>(1.0, 0.0);  // Mock value
        spectrum.emplace_back(frequency, magnitude);
    }
    
    return spectrum;
}

std::vector<DataAnalyzer::Pattern> DataAnalyzer::detectPatterns(const std::vector<double>& data) const {
    // Mock pattern detection - in real implementation would use sophisticated algorithms
    std::vector<Pattern> patterns;
    
    // Simple pattern: if data shows periodic behavior
    if (data.size() > 10) {
        Pattern pattern("periodic");
        pattern.confidence = 0.8;  // Mock confidence
        pattern.occurrenceIndices = {1, 5, 9, 13};  // Mock occurrences
        patterns.push_back(pattern);
    }
    
    return patterns;
}

DataAnalyzer::PerformanceProfile DataAnalyzer::getPerformanceProfile() const {
    return pImpl->performanceProfile;
}

} // namespace nlm