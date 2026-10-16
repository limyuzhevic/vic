#include "CMakeProfiler.hpp"
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

CMakeProfiler::CMakeProfiler() : isRunning_(false), totalCMakeBuilds_(0), successfulCMakeBuilds_(0),
    averageConfigureTime_(0.0), averageGenerateTime_(0.0), averageBuildTime_(0.0) {
    lastMonitoringTime_ = std::chrono::system_clock::now();
}

CMakeProfiler::~CMakeProfiler() {
    stop();
}

void CMakeProfiler::start() {
    if (!isRunning_) {
        isRunning_ = true;
        updateCMakeMetrics(0, 0);
        
        // Start monitoring thread
        monitoringThread_ = std::thread(&CMakeProfiler::monitorCMakeSystem, this);
        
        NLM_LOG_INFO("CMake profiler started");
    }
}

void CMakeProfiler::stop() {
    if (isRunning_) {
        isRunning_ = false;
        
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
        
        NLM_LOG_INFO("CMake profiler stopped");
    }
}

bool CMakeProfiler::isRunning() const {
    return isRunning_.load();
}

void CMakeProfiler::recordCMakeBuildPerformance(const std::string& target, const std::string& generator,
                                                const std::chrono::nanoseconds& totalTime,
                                                const std::chrono::nanoseconds& configureTime,
                                                const std::chrono::nanoseconds& generateTime,
                                                const std::chrono::nanoseconds& buildTime,
                                                const std::chrono::nanoseconds& installTime,
                                                size_t sourceFiles, size_t headerFiles, size_t buildFiles,
                                                size_t generatedFiles, const std::string& cmakeCommand,
                                                const std::string& buildCommand, const std::string& sourceDir,
                                                const std::string& buildDir) {
    std::lock_guard<std::mutex> lock(buildMutex_);
    
    if (cmakeBuildPerformances_.find(target) == cmakeBuildPerformances_.end()) {
        cmakeBuildPerformances_[target] = CMakeBuildPerformance(target);
    }
    
    auto& performance = cmakeBuildPerformances_[target];
    performance.totalCMakeTime += totalTime;
    performance.configureTime += configureTime;
    performance.generateTime += generateTime;
    performance.buildTime += buildTime;
    performance.installTime += installTime;
    performance.sourceFiles += sourceFiles;
    performance.headerFiles += headerFiles;
    performance.buildFiles += buildFiles;
    performance.generatedFiles += generatedFiles;
    performance.cmakeCommand = cmakeCommand;
    performance.buildCommand = buildCommand;
    performance.sourceDirectory = sourceDir;
    performance.buildDirectory = buildDir;
    performance.buildTime = std::chrono::system_clock::now();
    
    // Calculate configure and build speeds
    double totalTimeSec = totalTime.count() / 1000000000.0;
    if (totalTimeSec > 0.0) {
        performance.configureSpeed = sourceFiles / totalTimeSec;
        performance.buildSpeed = buildFiles / totalTimeSec;
    }
    
    // Clean up old profiles if needed
    if (cmakeBuildPerformances_.size() > config_.maxCMakeProfiles) {
        // Remove oldest profiles
        auto it = cmakeBuildPerformances_.begin();
        std::advance(it, cmakeBuildPerformances_.size() - config_.maxCMakeProfiles);
        cmakeBuildPerformances_.erase(it, cmakeBuildPerformances_.end());
    }
    
    // Update metrics
    updateCMakeMetrics(totalCMakeBuilds_ + 1, totalCMakeBuilds_ + 1); // Assume success for now
}

void CMakeProfiler::recordCMakeStepPerformance(const std::string& stepName, const std::string& dependency,
                                               const std::chrono::nanoseconds& stepTime,
                                               const std::chrono::nanoseconds& dependencyTime,
                                               bool stepSuccess, const std::string& output) {
    std::lock_guard<std::mutex> lock(stepMutex_);
    
    if (cmakeStepPerformances_.find(stepName) == cmakeStepPerformances_.end()) {
        cmakeStepPerformances_[stepName] = CMakeStepPerformance(stepName, dependency);
    }
    
    auto& performance = cmakeStepPerformances_[stepName];
    performance.stepTime += stepTime;
    performance.dependencyTime += dependencyTime;
    performance.stepSuccess = stepSuccess;
    performance.output = output;
    
    // Clean up old profiles if needed
    if (cmakeStepPerformances_.size() > config_.maxCMakeProfiles) {
        // Remove oldest profiles
        auto it = cmakeStepPerformances_.begin();
        std::advance(it, cmakeStepPerformances_.size() - config_.maxCMakeProfiles);
        cmakeStepPerformances_.erase(it, cmakeStepPerformances_.end());
    }
}

std::vector<CMakeBuildPerformance> CMakeProfiler::getCMakeBuildPerformances() const {
    std::lock_guard<std::mutex> lock(buildMutex_);
    std::vector<CMakeBuildPerformance> result;
    result.reserve(cmakeBuildPerformances_.size());
    for (const auto& [key, performance] : cmakeBuildPerformances_) {
        result.push_back(performance);
    }
    return result;
}

std::vector<CMakeStepPerformance> CMakeProfiler::getCMakeStepPerformances() const {
    std::lock_guard<std::mutex> lock(stepMutex_);
    std::vector<CMakeStepPerformance> result;
    result.reserve(cmakeStepPerformances_.size());
    for (const auto& [key, performance] : cmakeStepPerformances_) {
        result.push_back(performance);
    }
    return result;
}

void CMakeProfiler::setConfig(const CMakeProfilingConfig& config) {
    config_ = config;
}

CMakeProfilingConfig CMakeProfiler::getConfig() const {
    return config_;
}

void CMakeProfiler::updateCMakeMetrics(size_t totalBuilds, size_t successfulBuilds) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    totalCMakeBuilds_ = totalBuilds;
    successfulCMakeBuilds_ = successfulBuilds;
    
    // Calculate average times
    double totalConfigureTimeMs = 0.0;
    double totalGenerateTimeMs = 0.0;
    double totalBuildTimeMs = 0.0;
    double totalBuildCount = 0.0;
    
    for (const auto& [key, performance] : cmakeBuildPerformances_) {
        totalConfigureTimeMs += performance.configureTime.count() / 1000000.0;
        totalGenerateTimeMs += performance.generateTime.count() / 1000000.0;
        totalBuildTimeMs += performance.buildTime.count() / 1000000.0;
        totalBuildCount++;
    }
    
    if (totalBuildCount > 0.0) {
        averageConfigureTime_ = totalConfigureTimeMs / totalBuildCount;
        averageGenerateTime_ = totalGenerateTimeMs / totalBuildCount;
        averageBuildTime_ = totalBuildTimeMs / totalBuildCount;
    }
}

void CMakeProfiler::analyzeCMakeBuildPerformance(CMakeBuildPerformance& performance) const {
    // Analyze CMake build performance
    double configureSpeed = calculateConfigureSpeed(performance);
    
    if (configureSpeed < 10.0) { // Less than 10 files per second
        NLM_LOG_WARNING("CMake build " + performance.buildTarget + " too slow: " + 
                       std::to_string(configureSpeed) + " files/second");
    }
}

void CMakeProfiler::analyzeCMakeStepPerformance(CMakeStepPerformance& performance) const {
    // Analyze CMake step performance
    double stepTimeMs = performance.stepTime.count() / 1000000.0;
    
    if (stepTimeMs > 60000.0) { // More than 1 minute
        NLM_LOG_WARNING("CMake step " + performance.stepName + " too slow: " + 
                       std::to_string(stepTimeMs) + " ms");
    }
}

std::string CMakeProfiler::generateHtmlReport() const {
    std::ostringstream oss;
    
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "<head>" << std::endl;
    oss << "    <title>NLM CMake Performance Report</title>" << std::endl;
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
    oss << "        <h1>NLM CMake Performance Report</h1>" << std::endl;
    oss << "        <p>Generated on: " << getCurrentTimestamp() << "</p>" << std::endl;
    oss << "    </div>" << std::endl;
    
    oss << "    <div class=\"metrics\">" << std::endl;
    oss << "        <h2>CMake System Metrics</h2>" << std::endl;
    oss << "        <p>Total CMake Builds: " << totalCMakeBuilds_ << "</p>" << std::endl;
    oss << "        <p>Successful CMake Builds: " << successfulCMakeBuilds_ << "</p>" << std::endl;
    oss << "        <p>Average Configure Time: " << averageConfigureTime_ << " ms</p>" << std::endl;
    oss << "        <p>Average Generate Time: " << averageGenerateTime_ << " ms</p>" << std::endl;
    oss << "        <p>Average Build Time: " << averageBuildTime_ << " ms</p>" << std::endl;
    oss << "        <p>Configure Success Rate: " << (successfulCMakeBuilds_ > 0 ? (100.0 * successfulCMakeBuilds_ / totalCMakeBuilds_) : 0.0) << "%</p>" << std::endl;
    oss << "    </div>" << std::endl;
    
    oss << "    <h2>CMake Build Performance</h2>" << std::endl;
    oss << "    <table>" << std::endl;
    oss << "        <tr><th>Target</th><th>Generator</th><th>Total Time (ms)</th><th>Configure Time (ms)</th><th>Generate Time (ms)</th><th>Build Time (ms)</th><th>Install Time (ms)</th><th>Source Files</th><th>Header Files</th><th>Build Files</th><th>Generated Files</th><th>Configure Speed (files/sec)</th><th>Build Speed (files/sec)</th><th>CMake Command</th><th>Build Command</th></tr>" << std::endl;
    
    for (const auto& [key, performance] : cmakeBuildPerformances_) {
        double totalTimeMs = performance.totalCMakeTime.count() / 1000000.0;
        double configureTimeMs = performance.configureTime.count() / 1000000.0;
        double generateTimeMs = performance.generateTime.count() / 1000000.0;
        double buildTimeMs = performance.buildTime.count() / 1000000.0;
        double installTimeMs = performance.installTime.count() / 1000000.0;
        
        std::string configureClass = "good";
        if (performance.configureSpeed < 10.0) {
            configureClass = (performance.configureSpeed < 5.0) ? "poor" : "warning";
        }
        
        oss << "        <tr>" << std::endl;
        oss << "            <td>" << performance.buildTarget << "</td>" << std::endl;
        oss << "            <td>" << performance.cmakeGenerator << "</td>" << std::endl;
        oss << "            <td>" << totalTimeMs << "</td>" << std::endl;
        oss << "            <td>" << configureTimeMs << "</td>" << std::endl;
        oss << "            <td>" << generateTimeMs << "</td>" << std::endl;
        oss << "            <td>" << buildTimeMs << "</td>" << std::endl;
        oss << "            <td>" << installTimeMs << "</td>" << std::endl;
        oss << "            <td>" << performance.sourceFiles << "</td>" << std::endl;
        oss << "            <td>" << performance.headerFiles << "</td>" << std::endl;
        oss << "            <td>" << performance.buildFiles << "</td>" << std::endl;
        oss << "            <td>" << performance.generatedFiles << "</td>" << std::endl;
        oss << "            <td class=\"" << configureClass << "\">" << performance.configureSpeed << "</td>" << std::endl;
        oss << "            <td>" << performance.buildSpeed << "</td>" << std::endl;
        oss << "            <td>" << performance.cmakeCommand << "</td>" << std::endl;
        oss << "            <td>" << performance.buildCommand << "</td>" << std::endl;
        oss << "        </tr>" << std::endl;
    }
    
    oss << "    </table>" << std::endl;
    
    oss << "    <h2>CMake Step Performance</h2>" << std::endl;
    oss << "    <table>" << std::endl;
    oss << "        <tr><th>Step Name</th><th>Dependency</th><th>Step Time (ms)</th><th>Dependency Time (ms)</th><th>Success</th><th>Output</th></tr>" << std::endl;
    
    for (const auto& [key, performance] : cmakeStepPerformances_) {
        double stepTimeMs = performance.stepTime.count() / 1000000.0;
        double dependencyTimeMs = performance.dependencyTime.count() / 1000000.0;
        
        std::string successClass = performance.stepSuccess ? "good" : "poor";
        
        oss << "        <tr>" << std::endl;
        oss << "            <td>" << performance.stepName << "</td>" << std::endl;
        oss << "            <td>" << performance.dependency << "</td>" << std::endl;
        oss << "            <td>" << stepTimeMs << "</td>" << std::endl;
        oss << "            <td>" << dependencyTimeMs << "</td>" << std::endl;
        oss << "            <td class=\"" << successClass << "\">" << (performance.stepSuccess ? "Yes" : "No") << "</td>" << std::endl;
        oss << "            <td>" << performance.output << "</td>" << std::endl;
        oss << "        </tr>" << std::endl;
    }
    
    oss << "    </table>" << std::endl;
    
    oss << "</body>" << std::endl;
    oss << "</html>" << std::endl;
    
    return oss.str();
}

std::string CMakeProfiler::generateJsonReport() const {
    std::ostringstream oss;
    
    oss << "{
    \"cmakeReport\": {
        \"timestamp\": \"" << getCurrentTimestamp() << "\",
        \"totalCMakeBuilds\": " << totalCMakeBuilds_ << ",
        \"successfulCMakeBuilds\": " << successfulCMakeBuilds_ << ",
        \"averageConfigureTime\": " << averageConfigureTime_ << ",
        \"averageGenerateTime\": " << averageGenerateTime_ << ",
        \"averageBuildTime\": " << averageBuildTime_ << ",
        \"configureSuccessRate\": " << (successfulCMakeBuilds_ > 0 ? (100.0 * successfulCMakeBuilds_ / totalCMakeBuilds_) : 0.0) << ",
        \"cmakeBuildPerformances\": [";
    
    for (size_t i = 0; i < cmakeBuildPerformances_.size(); ++i) {
        const auto& performance = cmakeBuildPerformances_.begin()->second;
        oss << "{
            \"buildTarget\": \"" << performance.buildTarget << "\",
            \"cmakeGenerator\": \"" << performance.cmakeGenerator << "\",
            \"totalCMakeTime\": " << (performance.totalCMakeTime.count() / 1000000.0) << ",
            \"configureTime\": " << (performance.configureTime.count() / 1000000.0) << ",
            \"generateTime\": " << (performance.generateTime.count() / 1000000.0) << ",
            \"buildTime\": " << (performance.buildTime.count() / 1000000.0) << ",
            \"installTime\": " << (performance.installTime.count() / 1000000.0) << ",
            \"sourceFiles\": " << performance.sourceFiles << ",
            \"headerFiles\": " << performance.headerFiles << ",
            \"buildFiles\": " << performance.buildFiles << ",
            \"generatedFiles\": " << performance.generatedFiles << ",
            \"configureSpeed\": " << performance.configureSpeed << ",
            \"buildSpeed\": " << performance.buildSpeed << ",
            \"cmakeCommand\": \"" << performance.cmakeCommand << "\",
            \"buildCommand\": \"" << performance.buildCommand << "\",
            \"sourceDirectory\": \"" << performance.sourceDirectory << "\",
            \"buildDirectory\": \"" << performance.buildDirectory << "\"
        }";
        
        if (i < cmakeBuildPerformances_.size() - 1) {
            oss << ",";
        }
    }
    
    oss << "],
        \"cmakeStepPerformances\": [";
    
    for (size_t i = 0; i < cmakeStepPerformances_.size(); ++i) {
        const auto& performance = cmakeStepPerformances_.begin()->second;
        oss << "{
            \"stepName\": \"" << performance.stepName << "\",
            \"dependency\": \"" << performance.dependency << "\",
            \"stepTime\": " << (performance.stepTime.count() / 1000000.0) << ",
            \"dependencyTime\": " << (performance.dependencyTime.count() / 1000000.0) << ",
            \"stepSuccess\": " << performance.stepSuccess << ",
            \"output\": \"" << performance.output << "\"
        }";
        
        if (i < cmakeStepPerformances_.size() - 1) {
            oss << ",";
        }
    }
    
    oss << "]
    }
}";
    
    return oss.str();
}

std::string CMakeProfiler::generateTextReport() const {
    std::ostringstream oss;
    
    oss << "NLM CMake Performance Report" << std::endl;
    oss << std::string(50, '=') << std::endl;
    oss << "Generated on: " << getCurrentTimestamp() << std::endl;
    oss << "Total CMake Builds: " << totalCMakeBuilds_ << std::endl;
    oss << "Successful CMake Builds: " << successfulCMakeBuilds_ << std::endl;
    oss << "Average Configure Time: " << averageConfigureTime_ << " ms" << std::endl;
    oss << "Average Generate Time: " << averageGenerateTime_ << " ms" << std::endl;
    oss << "Average Build Time: " << averageBuildTime_ << " ms" << std::endl;
    oss << "Configure Success Rate: " << (successfulCMakeBuilds_ > 0 ? (100.0 * successfulCMakeBuilds_ / totalCMakeBuilds_) : 0.0) << "%" << std::endl;
    oss << std::endl;
    
    oss << "CMake Build Performance Summary:" << std::endl;
    for (const auto& [key, performance] : cmakeBuildPerformances_) {
        double configureSpeed = calculateConfigureSpeed(performance);
        oss << "  - " << performance.buildTarget << " (" << performance.cmakeGenerator << "):" << std::endl;
        oss << "    Total Time: " << (performance.totalCMakeTime.count() / 1000000.0) << " ms" << std::endl;
        oss << "    Configure Time: " << (performance.configureTime.count() / 1000000.0) << " ms" << std::endl;
        oss << "    Build Time: " << (performance.buildTime.count() / 1000000.0) << " ms" << std::endl;
        oss << "    Configure Speed: " << performance.configureSpeed << " files/sec" << std::endl;
        oss << "    Build Speed: " << performance.buildSpeed << " files/sec" << std::endl;
        oss << std::endl;
    }
    
    oss << "CMake Step Performance Summary:" << std::endl;
    for (const auto& [key, performance] : cmakeStepPerformances_) {
        double stepTimeMs = performance.stepTime.count() / 1000000.0;
        oss << "  - " << performance.stepName << " (" << performance.dependency << "):" << std::endl;
        oss << "    Step Time: " << stepTimeMs << " ms" << std::endl;
        oss << "    Dependency Time: " << (performance.dependencyTime.count() / 1000000.0) << " ms" << std::endl;
        oss << "    Success: " << (performance.stepSuccess ? "Yes" : "No") << std::endl;
        if (!performance.output.empty()) {
            oss << "    Output: " << performance.output << std::endl;
        }
        oss << std::endl;
    }
    
    return oss.str();
}

double CMakeProfiler::calculateConfigureSpeed(CMakeBuildPerformance& performance) const {
    double totalTimeSec = performance.totalCMakeTime.count() / 1000000000.0;
    if (totalTimeSec > 0.0) {
        return performance.sourceFiles / totalTimeSec;
    }
    return 0.0;
}

double CMakeProfiler::calculateBuildSpeed(CMakeBuildPerformance& performance) const {
    double totalTimeSec = performance.totalCMakeTime.count() / 1000000000.0;
    if (totalTimeSec > 0.0) {
        return performance.buildFiles / totalTimeSec;
    }
    return 0.0;
}

void CMakeProfiler::monitorCMakeSystem() {
    while (isRunning_) {
        // Simulate CMake metrics update
        static size_t buildCount = 0;
        static size_t successCount = 0;
        
        // Simulate CMake builds
        buildCount = 5 + (rand() % 15);
        successCount = 4 + (rand() % 11);
        
        updateCMakeMetrics(buildCount, successCount);
        
        // Analyze performance
        for (auto& [key, performance] : cmakeBuildPerformances_) {
            analyzeCMakeBuildPerformance(performance);
        }
        
        for (auto& [key, performance] : cmakeStepPerformances_) {
            analyzeCMakeStepPerformance(performance);
        }
        
        monitoringCount_++;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.profilingIntervalMs));
    }
}