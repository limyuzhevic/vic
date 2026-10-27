// PerformanceProfiler Implementation
// Implementation of comprehensive performance monitoring and profiling tools

#include "performance/PerformanceProfiler.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <thread>
#include <chrono>
#include <map>
#include <fstream>
#include <iostream>

using namespace nlm;

// PerformanceProfiler implementation
struct PerformanceProfiler::Impl {
    std::string experimentName;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point lastPauseTime;
    std::chrono::duration<double> totalPausedTime;
    bool isPaused;
    bool isRunning;
    
    // Performance data
    PerformanceMetrics metrics;
    
    // Time profiling
    struct TimeEvent {
        std::string eventName;
        double duration;
        std::chrono::high_resolution_clock::time_point start;
    };
    std::vector<TimeEvent> timeEvents;
    
    // Custom metrics
    std::map<std::string, double> customMetrics;
    std::vector<double> memoryUsageHistory;
    std::vector<std::pair<std::string, double>> stepData;
    
    // Configuration
    size_t samplingInterval;
    bool realTimeReporting;
    size_t maxTimePoints;
    
    // Callbacks
    PerformanceCallback performanceCallback;
};

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>()) {
    pImpl->isPaused = false;
    pImpl->isRunning = false;
    pImpl->totalPausedTime = std::chrono::duration<double>(0);
    
    // Default configuration
    pImpl->samplingInterval = 100;
    pImpl->realTimeReporting = false;
    pImpl->maxTimePoints = 10000;
}

PerformanceProfiler::~PerformanceProfiler() = default;

void PerformanceProfiler::startProfile(const std::string& experimentName) {
    pImpl->experimentName = experimentName;
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    pImpl->isRunning = true;
    pImpl->isPaused = false;
    pImpl->totalPausedTime = std::chrono::duration<double>(0);
    
    pImpl->timeEvents.clear();
    pImpl->customMetrics.clear();
    pImpl->memoryUsageHistory.clear();
    pImpl->stepData.clear();
    
    recordTimePoint("experiment_start", 0.0);
}

void PerformanceProfiler::stopProfile() {
    if (!pImpl->isRunning) return;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - pImpl->startTime);
    duration -= pImpl->totalPausedTime;
    
    recordTimePoint("experiment_end", duration.count());
    pImpl->isRunning = false;
}

void PerformanceProfiler::pauseProfile() {
    if (!pImpl->isRunning || pImpl->isPaused) return;
    
    pImpl->isPaused = true;
    pImpl->lastPauseTime = std::chrono::high_resolution_clock::now();
}

void PerformanceProfiler::resumeProfile() {
    if (!pImpl->isRunning || !pImpl->isPaused) return;
    
    auto resumeTime = std::chrono::high_resolution_clock::now();
    auto pauseDuration = std::chrono::duration<double>(resumeTime - pImpl->lastPauseTime);
    pImpl->totalPausedTime += pauseDuration;
    pImpl->isPaused = false;
}

void PerformanceProfiler::recordTimePoint(const std::string& event, double time) {
    TimeEvent eventData;
    eventData.eventName = event;
    eventData.duration = time;
    eventData.start = std::chrono::high_resolution_clock::now();
    
    pImpl->timeEvents.push_back(eventData);
    
    if (pImpl->realTimeReporting && performanceCallback) {
        PerformanceMetrics metrics = getResults();
        performanceCallback(metrics);
    }
}

void PerformanceProfiler::recordMetric(const std::string& metric, double value) {
    pImpl->customMetrics[metric] = value;
}

void PerformanceProfiler::recordMemoryUsage(size_t memoryBytes) {
    pImpl->memoryUsageHistory.push_back(memoryBytes / (1024.0 * 1024.0)); // Convert to MB
}

void PerformanceProfiler::recordSimulationStep(float reward, float firingRate, float neuromodLevel) {
    pImpl->stepData.push_back({std::to_string(pImpl->stepData.size()), reward});
    
    pImpl->metrics.rewards.push_back(reward);
    pImpl->metrics.firingRates.push_back(firingRate);
    pImpl->metrics.neuromodLevels.push_back(neuromodLevel);
    
    // Add to time steps
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(currentTime - pImpl->startTime);
    duration -= pImpl->totalPausedTime;
    pImpl->metrics.timeSteps.push_back(duration.count());
    
    pImpl->metrics.totalSimulations++;
    pImpl->metrics.totalReward += reward;
    pImpl->metrics.avgFiringRate += firingRate;
    
    // Update memory usage
    pImpl->memoryUsageHistory.push_back(getCurrentMemoryUsageMB());
    
    if (pImpl->realTimeReporting && performanceCallback) {
        PerformanceMetrics metrics = getResults();
        performanceCallback(metrics);
    }
}

PerformanceMetrics PerformanceProfiler::getResults() const {
    PerformanceMetrics results = pImpl->metrics;
    
    // Calculate averages
    if (!results.rewards.empty()) {
        double rewardSum = std::accumulate(results.rewards.begin(), results.rewards.end(), 0.0);
        results.totalReward = rewardSum;
    }
    
    if (!results.firingRates.empty()) {
        double firingSum = std::accumulate(results.firingRates.begin(), results.firingRates.end(), 0.0);
        results.avgFiringRate = firingSum / results.firingRates.size();
    }
    
    if (!results.timeSteps.empty()) {
        results.wallClockTime = results.timeSteps.back();
    }
    
    // Calculate memory usage average
    if (!pImpl->memoryUsageHistory.empty()) {
        double memorySum = std::accumulate(pImpl->memoryUsageHistory.begin(), 
                                         pImpl->memoryUsageHistory.end(), 0.0);
        results.memoryUsageMB = memorySum / pImpl->memoryUsageHistory.size();
    }
    
    return results;
}

std::string PerformanceProfiler::getProfileSummary() const {
    std::stringstream ss;
    ss << "=== Performance Profile Summary ===\n";
    ss << "Experiment: " << pImpl->experimentName << "\n";
    ss << "Total Simulations: " << pImpl->metrics.totalSimulations << "\n";
    ss << "Total Reward: " << std::fixed << std::setprecision(3) << pImpl->metrics.totalReward << "\n";
    ss << "Average Firing Rate: " << std::fixed << std::setprecision(3) << pImpl->metrics.avgFiringRate << "\n";
    ss << "Wall Clock Time: " << std::fixed << std::setprecision(2) << pImpl->metrics.wallClockTime << "s\n";
    ss << "Memory Usage: " << std::fixed << std::setprecision(2) << pImpl->metrics.memoryUsageMB << " MB\n";
    ss << "Number of Time Events: " << pImpl->timeEvents.size() << "\n";
    ss << "Number of Custom Metrics: " << pImpl->customMetrics.size() << "\n";
    
    return ss.str();
}

std::string PerformanceProfiler::getResultsJSON() const {
    std::stringstream json;
    json << "{\n";
    json << "  \"experimentName\": \"" << pImpl->experimentName << "\",\n";
    json << "  \"totalSimulations\": " << pImpl->metrics.totalSimulations << ',\n";
    json << "  \"totalReward\": " << std::fixed << std::setprecision(3) << pImpl->metrics.totalReward << ',\n";
    json << "  \"avgFiringRate\": " << std::fixed << std::setprecision(3) << pImpl->metrics.avgFiringRate << ',\n";
    json << "  \"wallClockTime\": " << std::fixed << std::setprecision(2) << pImpl->metrics.wallClockTime << ',\n";
    json << "  \"memoryUsageMB\": " << std::fixed << std::setprecision(2) << pImpl->metrics.memoryUsageMB << ',\n";
    json << "  \"neuronsCount\": " << pImpl->metrics.neuronsCount << ',\n";
    json << "  \"synapsesCount\": " << pImpl->metrics.synapsesCount << ',\n";
    json << "  \"totalSpikes\": " << pImpl->metrics.totalSpikes << ',\n";
    
    // Add custom metrics
    if (!pImpl->customMetrics.empty()) {
        json << "  \"customMetrics\": {\n";
        bool first = true;
        for (const auto& pair : pImpl->customMetrics) {
            if (!first) json << ',\n';
            json << "    \"" << pair.first << "\": " << pair.second;
            first = false;
        }
        json << "\n  },\n";
    }
    
    json << "  \"rewards\": [";
    bool firstReward = true;
    for (float reward : pImpl->metrics.rewards) {
        if (!firstReward) json << ', ';
        json << reward;
        firstReward = false;
    }
    json << "],\n";
    
    json << "  \"firingRates\": [";
    bool firstFiring = true;
    for (float rate : pImpl->metrics.firingRates) {
        if (!firstFiring) json << ', ';
        json << rate;
        firstFiring = false;
    }
    json << "]\n";
    
    json << "}\n";
    return json.str();
}

bool PerformanceProfiler::exportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "TimeStep,Reward,FiringRate,NeuromodLevel,MemoryUsage\n";
    
    for (size_t i = 0; i < pImpl->metrics.timeSteps.size(); ++i) {
        double time = pImpl->metrics.timeSteps[i];
        float reward = (i < pImpl->metrics.rewards.size()) ? pImpl->metrics.rewards[i] : 0.0f;
        float firingRate = (i < pImpl->metrics.firingRates.size()) ? pImpl->metrics.firingRates[i] : 0.0f;
        float neuromodLevel = (i < pImpl->metrics.neuromodLevels.size()) ? pImpl->metrics.neuromodLevels[i] : 0.0f;
        double memoryUsage = (i < pImpl->memoryUsageHistory.size()) ? pImpl->memoryUsageHistory[i] : 0.0;
        
        file << time << "," << reward << "," << firingRate << "," << neuromodLevel << "," << memoryUsage << "\n";
    }
    
    file.close();
    return true;
}

bool PerformanceProfiler::exportToJSON(const std::string& filename) const {
    return saveResults(filename, getResultsJSON());
}

bool PerformanceProfiler::exportToHTML(const std::string& filename) const {
    return saveResults(filename, generateHTMLReport());
}

double PerformanceProfiler::getAverageFiringRate() const {
    return getResults().avgFiringRate;
}

 double PerformanceProfiler::getStandardDeviationFiringRate() const {
    const auto& rates = pImpl->metrics.firingRates;
    if (rates.size() < 2) return 0.0;
    
    double mean = getAverageFiringRate();
    double sumSquares = 0.0;
    for (double rate : rates) {
        double diff = rate - mean;
        sumSquares += diff * diff;
    }
    
    return std::sqrt(sumSquares / (rates.size() - 1));
}

double PerformanceProfiler::getAverageReward() const {
    const auto& rewards = pImpl->metrics.rewards;
    if (rewards.empty()) return 0.0;
    
    double sum = std::accumulate(rewards.begin(), rewards.end(), 0.0);
    return sum / rewards.size();
}

double PerformanceProfiler::getRewardImprovement() const {
    if (pImpl->metrics.rewards.size() < 2) return 0.0;
    
    double firstHalfSum = 0.0;
    double secondHalfSum = 0.0;
    size_t halfSize = pImpl->metrics.rewards.size() / 2;
    
    for (size_t i = 0; i < halfSize; ++i) {
        firstHalfSum += pImpl->metrics.rewards[i];
    }
    
    for (size_t i = halfSize; i < pImpl->metrics.rewards.size(); ++i) {
        secondHalfSum += pImpl->metrics.rewards[i];
    }
    
    double firstHalfAvg = firstHalfSum / halfSize;
    double secondHalfAvg = secondHalfSum / (pImpl->metrics.rewards.size() - halfSize);
    
    return secondHalfAvg - firstHalfAvg;
}

double PerformanceProfiler::getPerformanceScaling() const {
    return calculateThroughput(pImpl->metrics.wallClockTime, pImpl->metrics.totalSimulations);
}

std::vector<std::pair<double, double>> PerformanceProfiler::getTimeSeriesData() const {
    std::vector<std::pair<double, double>> timeSeries;
    
    for (size_t i = 0; i < pImpl->metrics.timeSteps.size(); ++i) {
        double time = pImpl->metrics.timeSteps[i];
        double value = (i < pImpl->metrics.firingRates.size()) ? pImpl->metrics.firingRates[i] : 0.0;
        timeSeries.push_back(std::make_pair(time, value));
    }
    
    return timeSeries;
}

std::map<std::string, double> PerformanceProfiler::getStatisticalAnalysis() const {
    std::map<std::string, double> stats;
    
    const auto& rewards = pImpl->metrics.rewards;
    if (!rewards.empty()) {
        double sum = std::accumulate(rewards.begin(), rewards.end(), 0.0);
        double mean = sum / rewards.size();
        
        double sumSquares = 0.0;
        for (double reward : rewards) {
            double diff = reward - mean;
            sumSquares += diff * diff;
        }
        
        double variance = sumSquares / rewards.size();
        double stdDev = std::sqrt(variance);
        
        stats["reward_mean"] = mean;
        stats["reward_std"] = stdDev;
        stats["reward_min"] = *std::min_element(rewards.begin(), rewards.end());
        stats["reward_max"] = *std::max_element(rewards.begin(), rewards.end());
        stats["reward_range"] = stats["reward_max"] - stats["reward_min"];
    }
    
    const auto& firingRates = pImpl->metrics.firingRates;
    if (!firingRates.empty()) {
        double sum = std::accumulate(firingRates.begin(), firingRates.end(), 0.0);
        double mean = sum / firingRates.size();
        
        double sumSquares = 0.0;
        for (double rate : firingRates) {
            double diff = rate - mean;
            sumSquares += diff * diff;
        }
        
        double variance = sumSquares / firingRates.size();
        double stdDev = std::sqrt(variance);
        
        stats["firing_rate_mean"] = mean;
        stats["firing_rate_std"] = stdDev;
    }
    
    return stats;
}

void PerformanceProfiler::addBenchmark(const std::string& benchmarkName, const PerformanceMetrics& metrics) {
    // Benchmark storage would be implemented here
}

std::string PerformanceProfiler::getBenchmarkReport() const {
    return "Benchmark report would be generated here";
}

double PerformanceProfiler::getBenchmarkScore(const std::string& benchmarkName) const {
    return 0.0; // Placeholder
}

std::vector<PerformanceProfiler::TimeProfile> PerformanceProfiler::analyzeTimeDistribution() const {
    std::vector<TimeProfile> timeProfiles;
    
    double totalTime = pImpl->metrics.wallClockTime;
    if (totalTime <= 0) return timeProfiles;
    
    for (const auto& event : pImpl->timeEvents) {
        TimeProfile profile;
        profile.eventName = event.eventName;
        profile.duration = event.duration;
        profile.percentage = (event.duration / totalTime) * 100.0;
        timeProfiles.push_back(profile);
    }
    
    return timeProfiles;
}

std::string PerformanceProfiler::getOptimizationSuggestions() const {
    std::stringstream suggestions;
    suggestions << "=== Optimization Suggestions ===\n\n";
    
    auto timeProfiles = analyzeTimeDistribution();
    if (timeProfiles.empty()) {
        suggestions << "No time profile data available.\n";
        return suggestions.str();
    }
    
    // Find most time-consuming operations
    double maxPercentage = 0.0;
    std::string maxEvent;
    
    for (const auto& profile : timeProfiles) {
        if (profile.percentage > maxPercentage) {
            maxPercentage = profile.percentage;
            maxEvent = profile.eventName;
        }
    }
    
    suggestions << "Most time-consuming operation: " << maxEvent 
                << " (" << std::fixed << std::setprecision(1) << maxPercentage << "% of total time)\n\n";
    
    if (maxPercentage > 50.0) {
        suggestions << "Recommendation: Consider optimizing or parallelizing " << maxEvent << ".\n";
    }
    
    if (pImpl->metrics.memoryUsageMB > 100.0) {
        suggestions << "Recommendation: Memory usage is high (" 
                    << std::fixed << std::setprecision(1 << pImpl->metrics.memoryUsageMB 
                    << " MB). Consider reducing memory footprint.\n";
    }
    
    if (pImpl->metrics.totalSimulations < 1000) {
        suggestions << "Recommendation: Low simulation count. Consider increasing scale for better performance analysis.\n";
    }
    
    return suggestions.str();
}

void PerformanceProfiler::setSamplingInterval(size_t interval) {
    pImpl->samplingInterval = interval;
}

void PerformanceProfiler::enableRealTimeReporting(bool enable) {
    pImpl->realTimeReporting = enable;
}

void PerformanceProfiler::setMaxTimePoints(size_t maxPoints) {
    pImpl->maxTimePoints = maxPoints;
}

void PerformanceProfiler::reset() {
    pImpl->metrics = PerformanceMetrics();
    pImpl->timeEvents.clear();
    pImpl->customMetrics.clear();
    pImpl->memoryUsageHistory.clear();
    pImpl->stepData.clear();
}

void PerformanceProfiler::clear() {
    reset();
}

double PerformanceProfiler::calculateThroughput(double time, size_t steps) const {
    if (time <= 0 || steps == 0) return 0.0;
    return steps / time;
}

double PerformanceProfiler::calculateEfficiency(double time, double theoreticalMax) const {
    if (theoreticalMax <= 0) return 0.0;
    return (theoreticalMax / time) * 100.0;
}

std::string PerformanceProfiler::generateHTMLReport() const {
    std::stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html><head><title>Performance Report</title></head><body>\n";
    html << "<h1>Performance Profile: " << pImpl->experimentName << "</h1>\n";
    html << "<h2>Summary</h2>\n";
    html << "<p>Wall Clock Time: " << std::fixed << std::setprecision(2) << pImpl->metrics.wallClockTime << "s</p>\n";
    html << "<p>Total Simulations: " << pImpl->metrics.totalSimulations << "</p>\n";
    html << "<p>Total Reward: " << std::fixed << std::setprecision(3) << pImpl->metrics.totalReward << "</p>\n";
    html << "<p>Average Firing Rate: " << std::fixed << std::setprecision(3) << pImpl->metrics.avgFiringRate << "</p>\n";
    html << "<p>Memory Usage: " << std::fixed << std::setprecision(2) << pImpl->metrics.memoryUsageMB << " MB</p>\n";
    html << "</body></html>";
    return html.str();
}

std::string PerformanceProfiler::generateCSVReport() const {
    return "CSV report would be generated here";
}

std::string PerformanceProfiler::generateJSONReport() const {
    return getResultsJSON();
}

// PerformanceProfiler implementation end
