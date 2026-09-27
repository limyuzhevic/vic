// Implementation of StepProfiler

#include "nlm/performance/StepProfiler.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <random>

namespace nlm {

StepProfiler::StepProfiler(
    bool enableRealTimeAnalysis,
    std::chrono::milliseconds analysisInterval,
    bool enableRegressionDetection)
    : enableRealTimeAnalysis_(enableRealTimeAnalysis)
    , analysisInterval_(analysisInterval)
    , enableRegressionDetection_(enableRegressionDetection) {
}

void StepProfiler::start(bool enableNeuralMetrics) {
    if (running_.load(std::memory_order_relaxed)) {
        return;
    }
    
    enableNeuralMetrics_ = enableNeuralMetrics;
    running_.store(true, std::memory_order_relaxed);
    profilingThread_ = std::thread(&StepProfiler::profilingThreadFunc, this);
}

void StepProfiler::stop() {
    if (!running_.load(std::memory_order_relaxed)) {
        return;
    }
    
    {
        std::unique_lock<std::mutex> lock(stopMutex_);
        running_.store(false, std::memory_order_relaxed);
        stopCondition_.notify_one();
    }
    
    if (profilingThread_.joinable()) {
        profilingThread_.join();
    }
}

void StepProfiler::profilingThreadFunc() {
    while (running_.load(std::memory_order_relaxed)) {
        std::unique_lock<std::mutex> lock(stopMutex_);
        if (stopCondition_.wait_for(lock, analysisInterval_, 
                                   [this] { return !running_.load(std::memory_order_relaxed); })) {
            break;
        }
        lock.unlock();
        
        analyzePerformanceTrends();
        
        if (enableRegressionDetection_) {
            detectPerformanceRegressions();
        }
        
        identifyBottlenecks();
        
        checkPerformanceThresholds();
    }
}

void StepProfiler::startStep(size_t stepNumber, const std::string& description) {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    
    // Complete current step if it exists
    if (currentStep_.has_value()) {
        completeStep(currentStep_->spikesCount, currentStep_->neuronsUpdated, 
                     currentStep_->synapticOperations);
    }
    
    // Start new step
    currentStep_ = StepRecord();
    currentStep_->stepNumber = stepNumber;
    currentStep_->startTime = std::chrono::high_resolution_clock::now();
    currentStep_->stepDescription = description;
    
    currentStepNumber_.store(stepNumber, std::memory_order_relaxed);
}

void StepProfiler::endStep(size_t spikesCount, size_t neuronsUpdated, size_t synapticOperations) {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    
    if (!currentStep_.has_value()) {
        return;
    }
    
    completeStep(spikesCount, neuronsUpdated, synapticOperations);
}

void StepProfiler::completeStep(size_t spikesCount, size_t neuronsUpdated, size_t synapticOperations) {
    if (!currentStep_.has_value()) {
        return;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    currentStep_->endTime = endTime;
    currentStep_->stepDuration = endTime - currentStep_->startTime;
    currentStep_->spikesCount = spikesCount;
    currentStep_->neuronsUpdated = neuronsUpdated;
    currentStep_->synapticOperations = synapticOperations;
    
    // Calculate efficiency
    currentStep_->stepEfficiency = calculateStepEfficiency(*currentStep_);
    
    // Add to step records
    stepRecords_.push_back(*currentStep_);
    
    // Maintain history size
    while (stepRecords_.size() > 10000) {
        stepRecords_.pop_front();
    }
    
    // Update operations records
    for (const auto& [operationName, duration] : currentStep_->operationTimes) {
        operationRecords_[currentStep_->stepNumber].push_back(
            std::make_pair(operationName, duration)
        );
    }
    
    // Call callbacks
    for (const auto& callback : stepCallbacks_) {
        callback(*currentStep_);
    }
    
    // Reset current step
    currentStep_.reset();
    
    totalSteps_.fetch_add(1, std::memory_order_relaxed);
}

void StepProfiler::recordOperation(const std::string& operationName,
                                   std::chrono::nanoseconds duration,
                                   std::optional<size_t> stepNumber) {
    std::lock_guard<std::mutex> lock(operationsMutex_);
    
    size_t step = stepNumber.has_value() ? stepNumber.value() : getCurrentStepNumber();
    
    if (step == 0 && !currentStep_.has_value()) {
        // No current step, store for next step
        if (currentStep_.has_value()) {
            currentStep_->operationTimes[operationName] += duration;
        }
    } else {
        operationRecords_[step][operationName] += duration;
    }
}

std::optional<StepRecord> StepProfiler::getCurrentStep() const {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    return currentStep_;
}

std::vector<StepRecord> StepProfiler::getStepRecords(size_t startStep, size_t endStep) const {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    
    std::vector<StepRecord> records;
    
    if (endStep == 0) {
        // Get all records
        records.reserve(stepRecords_.size());
        for (const auto& record : stepRecords_) {
            if (startStep == 0 || record.stepNumber >= startStep) {
                records.push_back(record);
            }
        }
    } else {
        // Get records for specific range
        for (const auto& record : stepRecords_) {
            if (record.stepNumber >= startStep && record.stepNumber <= endStep) {
                records.push_back(record);
            }
        }
    }
    
    return records;
}

std::vector<BottleneckInfo> StepProfiler::getBottlenecks() const {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    
    std::vector<BottleneckInfo> bottlenecks;
    
    // Analyze step times
    std::vector<double> stepTimes;
    for (const auto& record : stepRecords_) {
        stepTimes.push_back(static_cast<double>(record.stepDuration.count()) / 1e9); // Convert to seconds
    }
    
    if (stepTimes.empty()) {
        return bottlenecks;
    }
    
    // Find slow steps (top 10% of step times)
    std::vector<double> sortedTimes = stepTimes;
    std::sort(sortedTimes.begin(), sortedTimes.end());
    size_t slowStepCount = std::max(size_t(1), stepTimes.size() / 10);
    double slowStepThreshold = sortedTimes[stepTimes.size() - slowStepCount];
    
    for (const auto& record : stepRecords_) {
        double stepTimeSeconds = static_cast<double>(record.stepDuration.count()) / 1e9;
        if (stepTimeSeconds > slowStepThreshold) {
            BottleneckInfo bottleneck;
            bottleneck.name = "slow_step_" + std::to_string(record.stepNumber);
            bottleneck.impactScore = std::min(1.0, stepTimeSeconds / 10.0); // Normalize to 0-1
            bottleneck.duration = record.stepDuration;
            bottleneck.location = record.stepDescription;
            bottleneck.description = "Step " + std::to_string(record.stepNumber) + " took " + 
                                    std::to_string(stepTimeSeconds) + " seconds";
            bottleneck.suggestedFixes = {
                "Optimize synaptic operations",
                "Reduce memory bandwidth usage",
                "Improve vectorization"
            };
            bottleneck.firstDetected = std::chrono::high_resolution_clock::now();
            bottleneck.detectionCount = 1;
            bottleneck.severity = (stepTimeSeconds > 5.0) ? "critical" : 
                                 (stepTimeSeconds > 2.0) ? "high" : "medium";
            
            bottlenecks.push_back(bottleneck);
        }
    }
    
    // Analyze operation times
    std::unordered_map<std::string, std::vector<double>> operationTimes;
    for (const auto& [stepNum, ops] : operationRecords_) {
        for (const auto& [opName, duration] : ops) {
            operationTimes[opName].push_back(static_cast<double>(duration.count()) / 1e9);
        }
    }
    
    for (const auto& [opName, times] : operationTimes) {
        if (times.empty()) continue;
        
        double avgTime = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
        double maxTime = *std::max_element(times.begin(), times.end());
        
        if (maxTime > avgTime * 3.0) { // Outlier detection
            BottleneckInfo bottleneck;
            bottleneck.name = "slow_operation_" + opName;
            bottleneck.impactScore = std::min(1.0, maxTime / 1.0); // Normalize to 0-1
            bottleneck.duration = std::chrono::nanoseconds(static_cast<long long>(maxTime * 1e9));
            bottleneck.location = opName;
            bottleneck.description = "Operation \"" + opName + "\" has high variance in execution time";
            bottleneck.suggestedFixes = {
                "Profile this operation more carefully",
                "Consider caching results",
                "Optimize data access patterns"
            };
            bottleneck.firstDetected = std::chrono::high_resolution_clock::now();
            bottleneck.detectionCount = 1;
            bottleneck.severity = (maxTime > 0.1) ? "high" : "medium";
            
            bottlenecks.push_back(bottleneck);
        }
    }
    
    return bottlenecks;
}

std::vector<PerformanceTrend> StepProfiler::getTrends(
    const std::string& metricName,
    std::chrono::high_resolution_clock::duration timeWindow) const {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    
    std::vector<PerformanceTrend> trends;
    
    // Map metric names to step records
    std::unordered_map<std::string, std::vector<std::pair<std::chrono::high_resolution_clock::time_point, double>>> metricData;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto cutoffTime = now - timeWindow;
    
    for (const auto& record : stepRecords_) {
        if (metricName == "step_time") {
            double stepTimeSeconds = static_cast<double>(record.stepDuration.count()) / 1e9;
            metricData[metricName].push_back({record.startTime, stepTimeSeconds});
        } else if (metricName == "spikes_count") {
            metricData[metricName].push_back({record.startTime, static_cast<double>(record.spikesCount)});
        } else if (metricName == "neurons_updated") {
            metricData[metricName].push_back({record.startTime, static_cast<double>(record.neuronsUpdated)});
        } else if (metricName == "synaptic_operations") {
            metricData[metricName].push_back({record.startTime, static_cast<double>(record.synapticOperations)});
        } else if (metricName == "efficiency") {
            double efficiency = calculateStepEfficiency(record);
            metricData[metricName].push_back({record.startTime, efficiency});
        }
    }
    
    // Analyze trends for each metric
    for (const auto& [name, dataPoints] : metricData) {
        if (dataPoints.size() < 2) continue;
        
        PerformanceTrend trend;
        trend.type = PerformanceTrend::TrendType::STABLE;
        trend.metricName = name;
        trend.startTime = dataPoints.front().first;
        trend.endTime = dataPoints.back().first;
        
        // Filter data points within time window
        std::vector<std::pair<std::chrono::high_resolution_clock::time_point, double>> filteredPoints;
        for (const auto& point : dataPoints) {
            if (point.first >= cutoffTime) {
                filteredPoints.push_back(point);
            }
        }
        
        if (filteredPoints.size() < 2) continue;
        
        // Calculate linear regression for trend
        double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
        double minX = filteredPoints.front().first.time_since_epoch().count();
        double maxX = minX;
        
        for (const auto& point : filteredPoints) {
            double x = static_cast<double>(point.first.time_since_epoch().count());
            double y = point.second;
            
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumX2 += x * x;
            
            minX = std::min(minX, x);
            maxX = std::max(maxX, x);
        }
        
        double n = filteredPoints.size();
        double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
        double intercept = (sumY - slope * sumX) / n;
        
        // Calculate R-squared
        double ssTot = 0.0, ssRes = 0.0;
        double meanY = sumY / n;
        
        for (const auto& point : filteredPoints) {
            double x = static_cast<double>(point.first.time_since_epoch().count());
            double y = point.second;
            double yPred = slope * x + intercept;
            
            ssTot += (y - meanY) * (y - meanY);
            ssRes += (y - yPred) * (y - yPred);
        }
        
        double rSquared = 1.0 - (ssRes / ssTot);
        
        // Determine trend type and confidence
        if (std::abs(slope) < 0.01 * (maxX - minX)) {
            trend.type = PerformanceTrend::TrendType::STABLE;
        } else if (slope > 0.01 * (maxX - minX)) {
            trend.type = PerformanceTrend::TrendType::IMPROVING;
        } else {
            trend.type = PerformanceTrend::TrendType::DEGRADING;
        }
        
        trend.trendValue = slope;
        trend.confidence = std::max(0.0, rSquared);
        trend.dataPoints = filteredPoints;
        
        trends.push_back(trend);
    }
    
    return trends;
}

std::vector<PerformanceRegression> StepProfiler::getPerformanceRegressions() const {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    
    std::vector<PerformanceRegression> regressions;
    
    // Analyze step time regression
    if (stepRecords_.size() >= 10) {
        // Compare recent steps with older steps
        size_t recentStart = stepRecords_.size() >= 20 ? stepRecords_.size() - 10 : 0;
        size_t olderStart = recentStart >= 10 ? recentStart - 10 : 0;
        
        std::vector<double> recentTimes, olderTimes;
        
        for (size_t i = recentStart; i < stepRecords_.size(); ++i) {
            double stepTime = static_cast<double>(stepRecords_[i].stepDuration.count()) / 1e9;
            recentTimes.push_back(stepTime);
        }
        
        for (size_t i = olderStart; i < recentStart && i < stepRecords_.size(); ++i) {
            double stepTime = static_cast<double>(stepRecords_[i].stepDuration.count()) / 1e9;
            olderTimes.push_back(stepTime);
        }
        
        if (!recentTimes.empty() && !olderTimes.empty()) {
            double recentAvg = std::accumulate(recentTimes.begin(), recentTimes.end(), 0.0) / recentTimes.size();
            double olderAvg = std::accumulate(olderTimes.begin(), olderTimes.end(), 0.0) / olderTimes.size();
            
            if (olderAvg > 0.0) {
                double regressionPercentage = (recentAvg - olderAvg) / olderAvg * 100.0;
                
                if (regressionPercentage > 10.0) { // 10% regression threshold
                    PerformanceRegression regression;
                    regression.metricName = "step_time";
                    regression.regressionValue = -regressionPercentage / 100.0;
                    regression.regressionPercentage = regressionPercentage;
                    regression.detectionTime = std::chrono::high_resolution_clock::now();
                    regression.regressionStartTime = stepRecords_[recentStart].startTime;
                    regression.regressionCause = "Step execution time increased over time";
                    regression.suggestedFix = "Review neural network optimizations and synaptic operations";
                    regression.confidence = std::min(1.0, regressionPercentage / 50.0);
                    
                    regressions.push_back(regression);
                }
            }
        }
    }
    
    return regressions;
}

std::vector<ProfilerAlert> StepProfiler::getRecentAlerts(size_t count) const {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    
    std::vector<ProfilerAlert> alerts;
    size_t actualCount = std::min(count, alerts_.size());
    
    if (actualCount == 0) {
        return alerts;
    }
    
    size_t startIndex = alerts_.size() - actualCount;
    alerts.reserve(actualCount);
    
    for (size_t i = startIndex; i < alerts_.size(); ++i) {
        alerts.push_back(alerts_[i]);
    }
    
    return alerts;
}

void StepProfiler::addStepCallback(const StepCallback& callback) {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    stepCallbacks_.push_back(callback);
}

void StepProfiler::addBottleneckCallback(const BottleneckCallback& callback) {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    bottleneckCallbacks_.push_back(callback);
}

void StepProfiler::addTrendCallback(const TrendCallback& callback) {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    trendCallbacks_.push_back(callback);
}

void StepProfiler::addRegressionCallback(const RegressionCallback& callback) {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    regressionCallbacks_.push_back(callback);
}

void StepProfiler::addAlertCallback(const AlertCallback& callback) {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    alertCallbacks_.push_back(callback);
}

StepProfiler::StepPerformanceStats StepProfiler::getStepStats(size_t stepNumber) const {
    StepPerformanceStats stats;
    
    std::lock_guard<std::mutex> lock(stepsMutex_);
    
    if (stepNumber == 0) {
        // Calculate for all steps
        for (const auto& record : stepRecords_) {
            double stepTime = static_cast<double>(record.stepDuration.count()) / 1e9;
            stats.averageStepTime += stepTime;
            stats.minStepTime = std::min(stats.minStepTime, stepTime);
            stats.maxStepTime = std::max(stats.maxStepTime, stepTime);
            stats.averageStepEfficiency += calculateStepEfficiency(record);
            stats.averageSpikesPerStep += record.spikesCount;
            stats.averageNeuronsPerStep += record.neuronsUpdated;
            stats.totalSteps++;
        }
    } else {
        // Find specific step
        for (const auto& record : stepRecords_) {
            if (record.stepNumber == stepNumber) {
                double stepTime = static_cast<double>(record.stepDuration.count()) / 1e9;
                stats.averageStepTime = stepTime;
                stats.minStepTime = stepTime;
                stats.maxStepTime = stepTime;
                stats.averageStepEfficiency = calculateStepEfficiency(record);
                stats.averageSpikesPerStep = record.spikesCount;
                stats.averageNeuronsPerStep = record.neuronsUpdated;
                stats.totalSteps = 1;
                break;
            }
        }
    }
    
    if (stats.totalSteps > 0) {
        stats.averageStepTime /= stats.totalSteps;
        stats.averageStepEfficiency /= stats.totalSteps;
        stats.averageSpikesPerStep /= stats.totalSteps;
        stats.averageNeuronsPerStep /= stats.totalSteps;
    }
    
    return stats;
}

std::vector<std::string> StepProfiler::getOptimizationRecommendations() const {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    
    std::vector<std::string> recommendations;
    
    // Check for slow steps
    auto stepStats = getStepStats(0);
    if (stepStats.maxStepTime > 5.0) { // 5 seconds threshold
        recommendations.push_back("Some steps are taking too long. Consider optimizing synaptic operations.");
    }
    
    if (stepStats.averageStepEfficiency < 0.5) {
        recommendations.push_back("Step efficiency is low. Consider improving data locality and reducing synchronization.");
    }
    
    // Check for bottleneck operations
    std::unordered_map<std::string, std::vector<double>> operationTimes;
    for (const auto& [stepNum, ops] : operationRecords_) {
        for (const auto& [opName, duration] : ops) {
            operationTimes[opName].push_back(static_cast<double>(duration.count()) / 1e9);
        }
    }
    
    for (const auto& [opName, times] : operationTimes) {
        double avgTime = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
        double maxTime = *std::max_element(times.begin(), times.end());
        
        if (maxTime > avgTime * 5.0 && avgTime > 0.1) {
            recommendations.push_back("Operation \"" + opName + "\" has high execution time variance. Consider optimizing.")
        }
    }
    
    // Check for performance regression
    auto regressions = getPerformanceRegressions();
    if (!regressions.empty()) {
        recommendations.push_back("Performance regression detected. Consider rolling back recent changes or optimizing affected code.");
    }
    
    return recommendations;
}

std::string StepProfiler::generateReport(const std::string& format,
                                         bool includeTrends,
                                         bool includeRegressions) const {
    std::stringstream report;
    
    if (format == "text") {
        report << "=== NLM Step Profiler Report ===" << std::endl;
        report << "Total Steps: " << getTotalSteps() << std::endl;
        report << "Current Step: " << getCurrentStepNumber() << std::endl;
        report << std::endl;
        
        auto stepStats = getStepStats(0);
        report << "--- Step Performance ---" << std::endl;
        report << "Average Step Time: " << stepStats.averageStepTime << " seconds" << std::endl;
        report << "Min Step Time: " << stepStats.minStepTime << " seconds" << std::endl;
        report << "Max Step Time: " << stepStats.maxStepTime << " seconds" << std::endl;
        report << "Average Efficiency: " << stepStats.averageStepEfficiency << std::endl;
        report << "Average Spikes per Step: " << stepStats.averageSpikesPerStep << std::endl;
        report << "Average Neurons Updated per Step: " << stepStats.averageNeuronsPerStep << std::endl;
        report << std::endl;
        
        if (includeTrends) {
            report << "--- Performance Trends ---" << std::endl;
            auto trends = getTrends();
            for (const auto& trend : trends) {
                std::string trendType = "stable";
                switch (trend.type) {
                    case PerformanceTrend::TrendType::IMPROVING: trendType = "improving"; break;
                    case PerformanceTrend::TrendType::DEGRADING: trendType = "degrading"; break;
                    case PerformanceTrend::TrendType::STABLE: trendType = "stable"; break;
                    case PerformanceTrend::TrendType::FLUCTUATING: trendType = "fluctuating"; break;
                }
                
                report << trend.metricName << ": " << trendType << " (confidence: " << trend.confidence << ")" << std::endl;
            }
            report << std::endl;
        }
        
        if (includeRegressions) {
            report << "--- Performance Regressions ---" << std::endl;
            auto regressions = getPerformanceRegressions();
            if (regressions.empty()) {
                report << "No regressions detected." << std::endl;
            } else {
                for (const auto& regression : regressions) {
                    report << regression.metricName << ": regression of " << regression.regressionPercentage << "%" << std::endl;
                    report << "  Cause: " << regression.regressionCause << std::endl;
                    report << "  Suggested Fix: " << regression.suggestedFix << std::endl;
                }
            }
            report << std::endl;
        }
        
        // Add bottlenecks
        report << "--- Performance Bottlenecks ---" << std::endl;
        auto bottlenecks = getBottlenecks();
        if (bottlenecks.empty()) {
            report << "No bottlenecks detected." << std::endl;
        } else {
            for (const auto& bottleneck : bottlenecks) {
                report << bottleneck.name << ": " << bottleneck.description << std::endl;
                report << "  Severity: " << bottleneck.severity << std::endl;
                report << "  Suggested fixes: ";
                for (size_t i = 0; i < bottleneck.suggestedFixes.size(); ++i) {
                    if (i > 0) report << ", ";
                    report << bottleneck.suggestedFixes[i];
                }
                report << std::endl;
            }
        }
        
    } else if (format == "json") {
        // JSON report generation
        report << "{\"step_profiler_report\": {" << std::endl;
        report << "  \"total_steps\": " << getTotalSteps() << "," << std::endl;
        report << "  \"current_step\": " << getCurrentStepNumber() << "," << std::endl;
        
        auto stepStats = getStepStats(0);
        report << "  \"step_performance\": {" << std::endl;
        report << "    \"average_step_time\": " << stepStats.averageStepTime << "," << std::endl;
        report << "    \"min_step_time\": " << stepStats.minStepTime << "," << std::endl;
        report << "    \"max_step_time\": " << stepStats.maxStepTime << "," << std::endl;
        report << "    \"average_efficiency\": " << stepStats.averageStepEfficiency << "," << std::endl;
        report << "    \"average_spikes_per_step\": " << stepStats.averageSpikesPerStep << "," << std::endl;
        report << "    \"average_neurons_updated_per_step\": " << stepStats.averageNeuronsPerStep << std::endl;
        report << "  }," << std::endl;
        
        if (includeTrends) {
            report << "  \"trends\": [" << std::endl;
            auto trends = getTrends();
            for (size_t i = 0; i < trends.size(); ++i) {
                auto& trend = trends[i];
                std::string trendType = "stable";
                switch (trend.type) {
                    case PerformanceTrend::TrendType::IMPROVING: trendType = "improving"; break;
                    case PerformanceTrend::TrendType::DEGRADING: trendType = "degrading"; break;
                    case PerformanceTrend::TrendType::STABLE: trendType = "stable"; break;
                    case PerformanceTrend::TrendType::FLUCTUATING: trendType = "fluctuating"; break;
                }
                
                report << "    {\"metric_name\": \"" << trend.metricName << "\",\"trend_type\": \"" << trendType << "\",\"trend_value\": " << trend.trendValue << ",\"confidence\": " << trend.confidence << "}";
                if (i < trends.size() - 1) report << ",";
                report << std::endl;
            }
            report << "  ]," << std::endl;
        }
        
        report << "}";
    }
    
    return report.str();
}

void StepProfiler::saveRecords(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }
    
    // Save step records in CSV format
    file << "step_number,start_time,end_time,step_duration,step_efficiency,spikes_count,neurons_updated" << std::endl;
    
    for (const auto& record : stepRecords_) {
        double startTime = std::chrono::duration<double>(record.startTime.time_since_epoch()).count();
        double endTime = std::chrono::duration<double>(record.endTime.time_since_epoch()).count();
        double duration = std::chrono::duration<double>(record.stepDuration).count();
        
        file << record.stepNumber << "," << startTime << "," << endTime << "," << duration << ","
             << record.stepEfficiency << "," << record.spikesCount << "," << record.neuronsUpdated << std::endl;
    }
}

void StepProfiler::loadRecords(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for reading: " + filename);
    }
    
    std::string line;
    std::getline(file, line); // Skip header
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        
        StepRecord record;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 7) {
            record.stepNumber = std::stoul(tokens[0]);
            record.startTime = std::chrono::high_resolution_clock::time_point(
                std::chrono::duration<double>(std::stod(tokens[1]))
            );
            record.endTime = std::chrono::high_resolution_clock::time_point(
                std::chrono::duration<double>(std::stod(tokens[2]))
            );
            record.stepDuration = std::chrono::nanoseconds(static_cast<long long>(std::stod(tokens[3]) * 1e9));
            record.stepEfficiency = std::stod(tokens[4]);
            record.spikesCount = std::stoul(tokens[5]);
            record.neuronsUpdated = std::stoul(tokens[6]);
            
            stepRecords_.push_back(record);
        }
    }
}

void StepProfiler::reset() {
    std::lock_guard<std::mutex> lock(stepsMutex_);
    
    currentStep_.reset();
    stepRecords_.clear();
    operationRecords_.clear();
    
    currentStepNumber_.store(0, std::memory_order_relaxed);
    totalSteps_.store(0, std::memory_order_relaxed);
    
    // Clear alerts
    alerts_.clear();
}

double StepProfiler::calculateStepEfficiency(const StepRecord& record) const {
    if (record.neuronsUpdated == 0) {
        return 0.0;
    }
    
    // Efficiency = (spikes + neurons_updated) / step_time
    double stepTimeSeconds = static_cast<double>(record.stepDuration.count()) / 1e9;
    return (static_cast<double>(record.spikesCount) + static_cast<double>(record.neuronsUpdated)) / stepTimeSeconds;
}

void StepProfiler::analyzePerformanceTrends() {
    auto trends = getTrends();
    
    for (const auto& trend : trends) {
        if (trend.confidence > 0.8) {
            ProfilerAlert alert;
            alert.type = ProfilerAlert::AlertType::EFFICIENCY_DROP;
            alert.description = "Performance trend detected: " + trend.metricName + " is " + 
                               (trend.type == PerformanceTrend::TrendType::DEGRADING ? "degrading" : "improving") + 
                               " with confidence " + std::to_string(trend.confidence);
            alert.timestamp = std::chrono::high_resolution_clock::now();
            alert.severity = 0.7;
            alert.details["metric_name"] = trend.metricName;
            alert.details["trend_type"] = (trend.type == PerformanceTrend::TrendType::DEGRADING ? "degrading" : "improving");
            
            alerts_.push_back(alert);
            
            for (const auto& callback : alertCallbacks_) {
                callback(alert);
            }
        }
    }
}

void StepProfiler::detectPerformanceRegressions() {
    auto regressions = getPerformanceRegressions();
    
    for (const auto& regression : regressions) {
        ProfilerAlert alert;
        alert.type = ProfilerAlert::AlertType::PERFORMANCE_REGRESSION;
        alert.description = "Performance regression detected in " + regression.metricName + 
                           ": " + std::to_string(regression.regressionPercentage) + "%";
        alert.timestamp = std::chrono::high_resolution_clock::now();
        alert.severity = 0.8;
        alert.details["metric_name"] = regression.metricName;
        alert.details["regression_percentage"] = std::to_string(regression.regressionPercentage);
        alert.details["suggested_fix"] = regression.suggestedFix;
        
        alerts_.push_back(alert);
        
        for (const auto& callback : alertCallbacks_) {
            callback(alert);
        }
    }
}

void StepProfiler::identifyBottlenecks() {
    auto bottlenecks = getBottlenecks();
    
    for (const auto& bottleneck : bottlenecks) {
        if (bottleneck.severity == "critical" || bottleneck.severity == "high") {
            ProfilerAlert alert;
            alert.type = ProfilerAlert::AlertType::BOTTLENECK_DETECTED;
            alert.description = "Performance bottleneck detected: " + bottleneck.name + " - " + 
                               bottleneck.description;
            alert.timestamp = std::chrono::high_resolution_clock::now();
            alert.severity = (bottleneck.severity == "critical") ? 0.9 : 0.7;
            alert.details["bottleneck_name"] = bottleneck.name;
            alert.details["location"] = bottleneck.location;
            alert.details["severity"] = bottleneck.severity;
            
            alerts_.push_back(alert);
            
            for (const auto& callback : alertCallbacks_) {
                callback(alert);
            }
            
            // Call bottleneck callbacks
            for (const auto& callback : bottleneckCallbacks_) {
                callback(bottleneck);
            }
        }
    }
}

void StepProfiler::checkPerformanceThresholds() {
    auto stepStats = getStepStats(0);
    
    // Check for slow steps
    if (stepStats.maxStepTime > 5.0) { // 5 seconds threshold
        ProfilerAlert alert;
        alert.type = ProfilerAlert::AlertType::SLOW_STEP;
        alert.description = "Slow step detected: maximum step time is " + 
                           std::to_string(stepStats.maxStepTime) + " seconds";
        alert.timestamp = std::chrono::high_resolution_clock::now();
        alert.severity = 0.6;
        
        alerts_.push_back(alert);
        
        for (const auto& callback : alertCallbacks_) {
            callback(alert);
        }
    }
    
    // Check for low efficiency
    if (stepStats.averageStepEfficiency < 0.5) {
        ProfilerAlert alert;
        alert.type = ProfilerAlert::AlertType::EFFICIENCY_DROP;
        alert.description = "Low step efficiency detected: " + std::to_string(stepStats.averageStepEfficiency);
        alert.timestamp = std::chrono::high_resolution_clock::now();
        alert.severity = 0.7;
        
        alerts_.push_back(alert);
        
        for (const auto& callback : alertCallbacks_) {
            callback(alert);
        }
    }
}