#include "ProgressStats.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

ProgressTracker::ProgressTracker() : totalSteps(0), currentStep(0), speed(0.0) {
}

void ProgressTracker::startProgress(const std::string& operation, size_t total) {
    std::lock_guard<std::mutex> lock(mutex);
    operationName = operation;
    totalSteps = total;
    currentStep = 0;
    startTime = std::chrono::steady_clock::now();
}

void ProgressTracker::updateProgress(size_t current, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex);
    currentStep = current;
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    
    if (elapsed > 0) {
        speed = (currentStep * 1000.0) / elapsed;
    }
}

void ProgressTracker::finishProgress(const std::string& finalMessage) {
    std::lock_guard<std::mutex> lock(mutex);
    updateProgress(totalSteps, finalMessage);
}

void ProgressTracker::setSpeed(double speed) {
    std::lock_guard<std::mutex> lock(mutex);
    this->speed = speed;
}

std::string ProgressTracker::getProgressDisplay() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::stringstream ss;
    if (totalSteps > 0) {
        double percentage = (totalSteps > 0) ? (static_cast<double>(currentStep) / totalSteps) * 100.0 : 0.0;
        ss << "[" << std::string(static_cast<size_t>(percentage / 10), '=') << ">" 
           << std::string(10 - static_cast<size_t>(percentage / 10), ' ')
           << "] " << std::fixed << std::setprecision(1) << percentage << "%";
        
        if (speed > 0) {
            auto remaining = totalSteps - currentStep;
            auto remainingTime = (remaining / speed);
            ss << " ETA: " << remainingTime << "s";
        }
        
        ss << " (" << currentStep << "/" << totalSteps << ")";
        if (!operationName.empty()) {
            ss << " - " << operationName;
        }
    }
    
    return ss.str();
}

StatisticsCollector::StatisticsCollector() {
    stats_.startTime = std::chrono::steady_clock::now();
}

void StatisticsCollector::recordExecutionTime(const std::string& operation, std::chrono::nanoseconds time) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Update operation stats
    bool found = false;
    for (auto& op : stats_.operations) {
        if (op.name == operation) {
            op.totalTime += time;
            op.minTime = std::min(op.minTime, time);
            op.maxTime = std::max(op.maxTime, time);
            op.callCount++;
            found = true;
            break;
        }
    }
    
    if (!found) {
        OperationStats op;
        op.name = operation;
        op.totalTime = time;
        op.minTime = time;
        op.maxTime = time;
        op.callCount = 1;
        stats_.operations.push_back(op);
    }
}

void StatisticsCollector::recordMemoryUsage(size_t memoryBytes) {
    std::lock_guard<std::mutex> lock(mutex);
    stats_.peakMemory = std::max(stats_.peakMemory, memoryBytes);
    stats_.memoryUsageGB = static_cast<double>(stats_.peakMemory) / (1024 * 1024 * 1024);
}

void StatisticsCollector::recordResourceUsage(double cpuPercent, double memoryPercent) {
    std::lock_guard<std::mutex> lock(mutex);
    stats_.peakCPU = std::max(stats_.peakCPU, cpuPercent);
    stats_.peakMemoryPercent = std::max(stats_.peakMemoryPercent, memoryPercent);
}

void StatisticsCollector::recordTestResult(const std::string& testName, bool passed, std::chrono::nanoseconds time) {
    std::lock_guard<std::mutex> lock(mutex);
    stats_.testResults.push_back({testName, passed});
}

std::string StatisticsCollector::generateStatistics(const std::string& format) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (format == "json") {
        return generateJSONStatistics();
    } else if (format == "xml") {
        return generateXMLStatistics();
    } else if (format == "csv") {
        return generateCSVStatistics();
    } else {
        return "Statistics in console format:" << std::endl;
    }
}

std::string StatisticsCollector::generateJSONStatistics() const {
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"timestamp\": \"" << getCurrentTimestamp() << "\"," << std::endl;
    ss << "  \"peak_memory_bytes\": " << stats_.peakMemory << "," << std::endl;
    ss << "  \"peak_cpu_percent\": " << stats_.peakCPU << "," << std::endl;
    ss << "  \"peak_memory_percent\": " << stats_.peakMemoryPercent << "," << std::endl;
    ss << "  \"total_operations\": " << stats_.operations.size() << "," << std::endl;
    ss << "  \"test_results\": [";
    
    for (size_t i = 0; i < stats_.testResults.size(); ++i) {
        const auto& result = stats_.testResults[i];
        ss << "{\"name\": \"" << result.first << "\", \"passed\": " << (result.second ? "true" : "false");
        if (i < stats_.testResults.size() - 1) {
            ss << "},";
        } else {
            ss << "}";
        }
    }
    ss << "]" << std::endl;
    ss << "}" << std::endl;
    
    return ss.str();
}

std::string StatisticsCollector::generateXMLStatistics() const {
    std::stringstream ss;
    ss << "<statistics>" << std::endl;
    ss << "  <timestamp>" << getCurrentTimestamp() << "</timestamp>" << std::endl;
    ss << "  <memory peak_bytes=\"" << stats_.peakMemory << "\"/>" << std::endl;
    ss << "  <cpu peak_percent=\"" << stats_.peakCPU << "\"/>" << std::endl;
    ss << "  <memory_usage peak_percent=\"" << stats_.peakMemoryPercent << "\"/>" << std::endl;
    ss << "  <operations>" << std::endl;
    
    for (const auto& op : stats_.operations) {
        ss << "    <operation name=\"" << op.name << "\" total_ns=\"" << op.totalTime.count()
           << "\" min_ns=\"" << op.minTime.count() << "\" max_ns=\"" << op.maxTime.count()
           << "\" calls=\"" << op.callCount << "\"/>" << std::endl;
    }
    
    ss << "  </operations>" << std::endl;
    ss << "  <test_results>" << std::endl;
    
    for (const auto& result : stats_.testResults) {
        ss << "    <test name=\"" << result.first << "\" passed=\"" << (result.second ? "true" : "false") << "/>" << std::endl;
    }
    
    ss << "  </test_results>" << std::endl;
    ss << "</statistics>" << std::endl;
    
    return ss.str();
}

std::string StatisticsCollector::generateCSVStatistics() const {
    std::stringstream ss;
    ss << "Operation,Total(ns),Min(ns),Max(ns),Calls" << std::endl;
    
    for (const auto& op : stats_.operations) {
        ss << op.name << "," << op.totalTime.count() << ","
           << op.minTime.count() << "," << op.maxTime.count() << ","
           << op.callCount << std::endl;
    }
    
    ss << std::endl << "Test Name,Passed" << std::endl;
    for (const auto& result : stats_.testResults) {
        ss << result.first << "," << (result.second ? "true" : "false") << std::endl;
    }
    
    return ss.str();
}

std::string StatisticsCollector::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

} // namespace nlm
