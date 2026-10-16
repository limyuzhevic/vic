#include "ConfigProfiler.hpp"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace nlm {

ConfigProfiler::ConfigProfiler() : isRunning_(false), totalConfigFiles_(0), totalConfigChanges_(0),
    averageLoadSpeed_(0.0), averageValidationTime_(0.0) {
    lastMonitoringTime_ = std::chrono::system_clock::now();
}

ConfigProfiler::~ConfigProfiler() {
    stop();
}

void ConfigProfiler::start() {
    if (!isRunning_) {
        isRunning_ = true;
        updateConfigMetrics(0, 0);
        
        // Start monitoring thread
        monitoringThread_ = std::thread(&ConfigProfiler::monitorConfigSystem, this);
        
        NLM_LOG_INFO("Config profiler started");
    }
}

void ConfigProfiler::stop() {
    if (isRunning_) {
        isRunning_ = false;
        
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
        
        NLM_LOG_INFO("Config profiler stopped");
    }
}

bool ConfigProfiler::isRunning() const {
    return isRunning_.load();
}

void ConfigProfiler::recordConfigLoading(const std::string& configFile, const std::chrono::nanoseconds& loadTime,
                                        const std::chrono::nanoseconds& parseTime, const std::chrono::nanoseconds& validationTime,
                                        const std::chrono::nanoseconds& applyTime, size_t fileSize, size_t keyCount,
                                        const std::string& format) {
    std::lock_guard<std::mutex> lock(loadingMutex_);
    
    if (configLoadingPerformances_.find(configFile) == configLoadingPerformances_.end()) {
        configLoadingPerformances_[configFile] = ConfigLoadingPerformance(configFile);
    }
    
    auto& performance = configLoadingPerformances_[configFile];
    performance.totalLoadTime += loadTime;
    performance.parseTime += parseTime;
    performance.validationTime += validationTime;
    performance.applyTime += applyTime;
    performance.fileSize += fileSize;
    performance.keyCount += keyCount;
    performance.loadTime = std::chrono::system_clock::now();
    performance.format = format;
    
    // Calculate load speed (MB/s)
    double loadTimeMs = (performance.totalLoadTime.count() / 1000000.0);
    if (loadTimeMs > 0.0) {
        performance.loadSpeed = (performance.fileSize / (1024 * 1024.0)) / loadTimeMs;
    }
    
    // Clean up old profiles if needed
    if (configLoadingPerformances_.size() > config_.maxConfigProfiles) {
        // Remove oldest profiles
        auto it = configLoadingPerformances_.begin();
        std::advance(it, configLoadingPerformances_.size() - config_.maxConfigProfiles);
        configLoadingPerformances_.erase(it, configLoadingPerformances_.end());
    }
    
    // Update metrics
    updateConfigMetrics(totalConfigFiles_ + 1, totalConfigChanges_);
}

void ConfigProfiler::recordConfigValidation(const std::string& validationType, const std::chrono::nanoseconds& validationTime,
                                           const std::chrono::nanoseconds& errorDetectionTime, const std::chrono::nanoseconds& errorCorrectionTime,
                                           size_t validatedKeys, size_t errorCount, size_t warningCount) {
    std::lock_guard<std::mutex> lock(validationMutex_);
    
    if (configValidationPerformances_.find(validationType) == configValidationPerformances_.end()) {
        configValidationPerformances_[validationType] = ConfigValidationPerformance(validationType);
    }
    
    auto& performance = configValidationPerformances_[validationType];
    performance.validationTime += validationTime;
    performance.errorDetectionTime += errorDetectionTime;
    performance.errorCorrectionTime += errorCorrectionTime;
    performance.validatedKeys += validatedKeys;
    performance.errorCount += errorCount;
    performance.warningCount += warningCount;
    performance.timestamp = std::chrono::system_clock::now();
    
    // Clean up old profiles if needed
    if (configValidationPerformances_.size() > config_.maxConfigProfiles) {
        // Remove oldest profiles
        auto it = configValidationPerformances_.begin();
        std::advance(it, configValidationPerformances_.size() - config_.maxConfigProfiles);
        configValidationPerformances_.erase(it, configValidationPerformances_.end());
    }
    
    // Update metrics
    double avgValidationTime = (performance.validationTime.count() / 1000000.0) / performance.validatedKeys;
    averageValidationTime_ = (averageValidationTime_ * (monitoringCount_ - 1) + avgValidationTime) / monitoringCount_;
}

std::vector<ConfigLoadingPerformance> ConfigProfiler::getConfigLoadingPerformances() const {
    std::lock_guard<std::mutex> lock(loadingMutex_);
    std::vector<ConfigLoadingPerformance> result;
    result.reserve(configLoadingPerformances_.size());
    for (const auto& [key, performance] : configLoadingPerformances_) {
        result.push_back(performance);
    }
    return result;
}

std::vector<ConfigValidationPerformance> ConfigProfiler::getConfigValidationPerformances() const {
    std::lock_guard<std::mutex> lock(validationMutex_);
    std::vector<ConfigValidationPerformance> result;
    result.reserve(configValidationPerformances_.size());
    for (const auto& [key, performance] : configValidationPerformances_) {
        result.push_back(performance);
    }
    return result;
}

void ConfigProfiler::setConfig(const ConfigProfilingConfig& config) {
    config_ = config;
}

ConfigProfilingConfig ConfigProfiler::getConfig() const {
    return config_;
}

void ConfigProfiler::updateConfigMetrics(size_t totalFiles, size_t totalChanges) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    totalConfigFiles_ = totalFiles;
    totalConfigChanges_ = totalChanges;
    
    // Calculate average load speed
    double totalLoadTimeMs = 0.0;
    double totalFileSizeMb = 0.0;
    
    for (const auto& [key, performance] : configLoadingPerformances_) {
        totalLoadTimeMs += (performance.totalLoadTime.count() / 1000000.0);
        totalFileSizeMb += performance.fileSize / (1024 * 1024.0);
    }
    
    if (totalLoadTimeMs > 0.0) {
        averageLoadSpeed_ = totalFileSizeMb / totalLoadTimeMs;
    }
}

void ConfigProfiler::analyzeConfigLoading(ConfigLoadingPerformance& performance) const {
    // Analyze loading performance
    double loadSpeed = calculateLoadingSpeed(performance);
    
    if (loadSpeed < config_.loadingSpeedThreshold) {
        NLM_LOG_WARNING("Config file " + performance.configFile + " loading speed below threshold: " + 
                       std::to_string(loadSpeed) + " MB/s < " + std::to_string(config_.loadingSpeedThreshold) + " MB/s");
    }
}

void ConfigProfiler::analyzeConfigValidation(ConfigValidationPerformance& performance) const {
    // Analyze validation performance
    double efficiency = calculateValidationEfficiency(performance);
    
    double validationTimeMs = (performance.validationTime.count() / 1000000.0);
    if (validationTimeMs > config_.validationTimeThreshold) {
        NLM_LOG_WARNING("Config validation " + performance.validationType + " too slow: " + 
                       std::to_string(validationTimeMs) + " ms > " + std::to_string(config_.validationTimeThreshold) + " ms");
    }
}

std::string ConfigProfiler::generateHtmlReport() const {
    std::ostringstream oss;
    
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "<head>" << std::endl;
    oss << "    <title>NLM Config Performance Report</title>" << std::endl;
    oss << "    <style>" << std::endl;
    oss << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    oss << "        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }" << std::endl;
    oss << "        .metrics { margin: 20px 0; padding: 20px; background-color: #f9f9f9; border-radius: 5px; }" << std::endl;
    oss << "        table { border-collapse: collapse; width: 100%; margin: 10px 0; }" << std::endl;
    oss << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
    oss << "        th { background-color: #f2f2f2; }" << std::endl;
    oss << "        .good { color: green; }" << std::endl;
    oss << "        .warning { color: orange; }" << std::endl;
    oss << "        .poor { color: red; }" << std::endl;
    oss << "    </style>" << std::endl;
    oss << "</head>" << std::endl;
    oss << "<body>" << std::endl;
    
    oss << "    <div class=\"header\">" << std::endl;
    oss << "        <h1>NLM Config Performance Report</h1>" << std::endl;
    oss << "        <p>Generated on: " << getCurrentTimestamp() << "</p>" << std::endl;
    oss << "    </div>" << std::endl;
    
    oss << "    <div class=\"metrics\">" << std::endl;
    oss << "        <h2>Config System Metrics</h2>" << std::endl;
    oss << "        <p>Total Config Files: " << totalConfigFiles_ << "</p>" << std::endl;
    oss << "        <p>Total Config Changes: " << totalConfigChanges_ << "</p>" << std::endl;
    oss << "        <p>Average Load Speed: " << averageLoadSpeed_ << " MB/s</p>" << std::endl;
    oss << "        <p>Average Validation Time: " << averageValidationTime_ << " ms</p>" << std::endl;
    oss << "    </div>" << std::endl;
    
    oss << "    <h2>Config Loading Performance</h2>" << std::endl;
    oss << "    <table>" << std::endl;
    oss << "        <tr><th>Config File</th><th>Total Load Time (ms)</th><th>Parse Time (ms)</th><th>Validation Time (ms)</th><th>Apply Time (ms)</th><th>File Size (MB)</th><th>Key Count</th><th>Format</th><th>Load Speed (MB/s)</th></tr>" << std::endl;
    
    for (const auto& [key, performance] : configLoadingPerformances_) {
        double totalLoadTimeMs = performance.totalLoadTime.count() / 1000000.0;
        double parseTimeMs = performance.parseTime.count() / 1000000.0;
        double validationTimeMs = performance.validationTime.count() / 1000000.0;
        double applyTimeMs = performance.applyTime.count() / 1000000.0;
        double fileSizeMb = performance.fileSize / (1024 * 1024.0);
        
        std::string speedClass = "good";
        if (performance.loadSpeed < config_.loadingSpeedThreshold) {
            speedClass = (performance.loadSpeed < config_.loadingSpeedThreshold * 0.5) ? "poor" : "warning";
        }
        
        oss << "        <tr>" << std::endl;
        oss << "            <td>" << performance.configFile << "</td>" << std::endl;
        oss << "            <td>" << totalLoadTimeMs << "</td>" << std::endl;
        oss << "            <td>" << parseTimeMs << "</td>" << std::endl;
        oss << "            <td>" << validationTimeMs << "</td>" << std::endl;
        oss << "            <td>" << applyTimeMs << "</td>" << std::endl;
        oss << "            <td>" << fileSizeMb << "</td>" << std::endl;
        oss << "            <td>" << performance.keyCount << "</td>" << std::endl;
        oss << "            <td>" << performance.format << "</td>" << std::endl;
        oss << "            <td class=\"" << speedClass << "\">" << performance.loadSpeed << "</td>" << std::endl;
        oss << "        </tr>" << std::endl;
    }
    
    oss << "    </table>" << std::endl;
    
    oss << "    <h2>Config Validation Performance</h2>" << std::endl;
    oss << "    <table>" << std::endl;
    oss << "        <tr><th>Validation Type</th><th>Validation Time (ms)</th><th>Error Detection Time (ms)</th><th>Error Correction Time (ms)</th><th>Validated Keys</th><th>Error Count</th><th>Warning Count</th></tr>" << std::endl;
    
    for (const auto& [key, performance] : configValidationPerformances_) {
        double validationTimeMs = performance.validationTime.count() / 1000000.0;
        double errorDetectionTimeMs = performance.errorDetectionTime.count() / 1000000.0;
        double errorCorrectionTimeMs = performance.errorCorrectionTime.count() / 1000000.0;
        
        std::string timeClass = "good";
        if (validationTimeMs > config_.validationTimeThreshold) {
            timeClass = (validationTimeMs > config_.validationTimeThreshold * 2.0) ? "poor" : "warning";
        }
        
        oss << "        <tr>" << std::endl;
        oss << "            <td>" << performance.validationType << "</td>" << std::endl;
        oss << "            <td class=\"" << timeClass << "\">" << validationTimeMs << "</td>" << std::endl;
        oss << "            <td>" << errorDetectionTimeMs << "</td>" << std::endl;
        oss << "            <td>" << errorCorrectionTimeMs << "</td>" << std::endl;
        oss << "            <td>" << performance.validatedKeys << "</td>" << std::endl;
        oss << "            <td>" << performance.errorCount << "</td>" << std::endl;
        oss << "            <td>" << performance.warningCount << "</td>" << std::endl;
        oss << "        </tr>" << std::endl;
    }
    
    oss << "    </table>" << std::endl;
    
    oss << "</body>" << std::endl;
    oss << "</html>" << std::endl;
    
    return oss.str();
}

std::string ConfigProfiler::generateJsonReport() const {
    std::ostringstream oss;
    
    oss << "{
    \"configReport\": {
        \"timestamp\": \"" << getCurrentTimestamp() << "\",
        \"totalConfigFiles\": " << totalConfigFiles_ << ",
        \"totalConfigChanges\": " << totalConfigChanges_ << ",
        \"averageLoadSpeed\": " << averageLoadSpeed_ << ",
        \"averageValidationTime\": " << averageValidationTime_ << ",
        \"configLoadingPerformances\": [";
    
    for (size_t i = 0; i < configLoadingPerformances_.size(); ++i) {
        const auto& performance = configLoadingPerformances_.begin()->second;
        oss << "{
            \"configFile\": \"" << performance.configFile << "\",
            \"totalLoadTime\": " << (performance.totalLoadTime.count() / 1000000.0) << ",
            \"parseTime\": " << (performance.parseTime.count() / 1000000.0) << ",
            \"validationTime\": " << (performance.validationTime.count() / 1000000.0) << ",
            \"applyTime\": " << (performance.applyTime.count() / 1000000.0) << ",
            \"fileSize\": " << performance.fileSize << ",
            \"keyCount\": " << performance.keyCount << ",
            \"loadSpeed\": " << performance.loadSpeed << ",
            \"format\": \"" << performance.format << "\"
        }";
        
        if (i < configLoadingPerformances_.size() - 1) {
            oss << ",";
        }
    }
    
    oss << "],
        \"configValidationPerformances\": [";
    
    for (size_t i = 0; i < configValidationPerformances_.size(); ++i) {
        const auto& performance = configValidationPerformances_.begin()->second;
        oss << "{
            \"validationType\": \"" << performance.validationType << "\",
            \"validationTime\": " << (performance.validationTime.count() / 1000000.0) << ",
            \"errorDetectionTime\": " << (performance.errorDetectionTime.count() / 1000000.0) << ",
            \"errorCorrectionTime\": " << (performance.errorCorrectionTime.count() / 1000000.0) << ",
            \"validatedKeys\": " << performance.validatedKeys << ",
            \"errorCount\": " << performance.errorCount << ",
            \"warningCount\": " << performance.warningCount
        }";
        
        if (i < configValidationPerformances_.size() - 1) {
            oss << ",";
        }
    }
    
    oss << "]
    }
}";
    
    return oss.str();
}

std::string ConfigProfiler::generateTextReport() const {
    std::ostringstream oss;
    
    oss << "NLM Config Performance Report" << std::endl;
    oss << std::string(50, '=') << std::endl;
    oss << "Generated on: " << getCurrentTimestamp() << std::endl;
    oss << "Total Config Files: " << totalConfigFiles_ << std::endl;
    oss << "Total Config Changes: " << totalConfigChanges_ << std::endl;
    oss << "Average Load Speed: " << averageLoadSpeed_ << " MB/s" << std::endl;
    oss << "Average Validation Time: " << averageValidationTime_ << " ms" << std::endl;
    oss << std::endl;
    
    oss << "Config Loading Performance:" << std::endl;
    for (const auto& [key, performance] : configLoadingPerformances_) {
        double loadSpeed = calculateLoadingSpeed(performance);
        oss << "  - " << performance.configFile << ": " << (performance.totalLoadTime.count() / 1000000.0) << " ms, " 
            << performance.fileSize / (1024 * 1024.0) << " MB, " << performance.loadSpeed << " MB/s" << std::endl;
    }
    
    oss << std::endl << "Config Validation Performance:" << std::endl;
    for (const auto& [key, performance] : configValidationPerformances_) {
        double validationTimeMs = performance.validationTime.count() / 1000000.0;
        oss << "  - " << performance.validationType << ": " << validationTimeMs << " ms, "
            << performance.validatedKeys << " keys validated, " << performance.errorCount << " errors, " << performance.warningCount << " warnings" << std::endl;
    }
    
    return oss.str();
}

double ConfigProfiler::calculateLoadingSpeed(ConfigLoadingPerformance& performance) const {
    double loadTimeMs = (performance.totalLoadTime.count() / 1000000.0);
    if (loadTimeMs > 0.0) {
        return (performance.fileSize / (1024 * 1024.0)) / loadTimeMs;
    }
    return 0.0;
}

double ConfigProfiler::calculateValidationEfficiency(ConfigValidationPerformance& performance) const {
    double totalTimeMs = performance.validationTime.count() / 1000000.0;
    if (totalTimeMs > 0.0) {
        return (performance.validatedKeys / totalTimeMs);
    }
    return 0.0;
}

void ConfigProfiler::monitorConfigSystem() {
    while (isRunning_) {
        // Simulate config metrics update
        static size_t fileCount = 0;
        static size_t changeCount = 0;
        
        // Simulate config changes
        fileCount = 10 + (rand() % 50);
        changeCount = 5 + (rand() % 20);
        
        updateConfigMetrics(fileCount, changeCount);
        
        // Analyze performance
        for (auto& [key, performance] : configLoadingPerformances_) {
            analyzeConfigLoading(performance);
        }
        
        for (auto& [key, performance] : configValidationPerformances_) {
            analyzeConfigValidation(performance);
        }
        
        monitoringCount_++;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.profilingIntervalMs));
    }
}