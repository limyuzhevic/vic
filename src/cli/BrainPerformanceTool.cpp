#include "BrainPerformanceTool.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <thread>

namespace nlm {
namespace cli {

BrainPerformanceTool::BrainPerformanceTool() {
    performanceThresholds["cpu_usage"] = 80.0;
    performanceThresholds["memory_usage"] = 1024.0;
    performanceThresholds["inference_time"] = 100.0;
    performanceThresholds["training_time"] = 50.0;
    performanceThresholds["throughput"] = 10.0;
    performanceThresholds["accuracy"] = 0.9;
}

BrainPerformanceTool::~BrainPerformanceTool() {
}

bool BrainPerformanceTool::BenchmarkBrainPerformance(const std::string& brainPath, PerformanceMetrics& metrics) {
    try {
        // Simulate brain performance benchmarking
        metrics.cpuUsage = 45.5;
        metrics.memoryUsage = 768.0;
        metrics.diskUsage = 0.3;
        metrics.networkUsage = 0.1;
        metrics.timestamp = std::chrono::system_clock::now();
        
        // Generate random metrics
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> cpuDis(20.0, 80.0);
        std::uniform_real_distribution<> memDis(256.0, 2048.0);
        std::uniform_real_distribution<> diskDis(0.1, 0.5);
        std::uniform_real_distribution<> netDis(0.0, 0.2);
        
        metrics.cpuUsage = cpuDis(gen);
        metrics.memoryUsage = memDis(gen);
        metrics.diskUsage = diskDis(gen);
        metrics.networkUsage = netDis(gen);
        
        metrics.customMetrics["inference_speed"] = 125.5;
        metrics.customMetrics["training_speed"] = 45.2;
        metrics.customMetrics["accuracy"] = 0.94;
        metrics.customMetrics["precision"] = 0.92;
        metrics.customMetrics["recall"] = 0.91;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error benchmarking brain performance: " << e.what() << std::endl;
        return false;
    }
}

bool BrainPerformanceTool::MonitorPerformance(const std::string& brainPath, double duration) {
    try {
        // Monitor performance for a specified duration
        auto startTime = std::chrono::system_clock::now();
        auto endTime = startTime + std::chrono::seconds(static_cast<int>(duration));
        
        while (std::chrono::system_clock::now() < endTime) {
            PerformanceMetrics metrics;
            if (!BenchmarkBrainPerformance(brainPath, metrics)) {
                return false;
            }
            
            performanceHistory.push_back(metrics);
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error monitoring performance: " << e.what() << std::endl;
        return false;
    }
}

bool BrainPerformanceTool::ProfileBrainActivity(const std::string& brainPath, double duration) {
    try {
        // Profile brain activity for a specified duration
        auto startTime = std::chrono::system_clock::now();
        auto endTime = startTime + std::chrono::seconds(static_cast<int>(duration));
        
        while (std::chrono::system_clock::now() < endTime) {
            // Simulate activity profiling
            PerformanceMetrics metrics;
            BenchmarkBrainPerformance(brainPath, metrics);
            
            // Add activity-specific metrics
            metrics.customMetrics["active_neurons"] = 850.0;
            metrics.customMetrics["synapse_activity"] = 0.75;
            metrics.customMetrics["spike_rate"] = 12.5;
            metrics.customMetrics["firing_pattern"] = "burst";
            
            performanceHistory.push_back(metrics);
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error profiling brain activity: " << e.what() << std::endl;
        return false;
    }
}

bool BrainPerformanceTool::OptimizeBrainPerformance(const std::string& brainPath) {
    try {
        // Optimize brain performance (simplified)
        // In a real implementation, we would run optimization algorithms
        
        // Simulate performance improvements
        double improvementFactor = 1.15; // 15% improvement
        
        for (auto& metrics : performanceHistory) {
            metrics.cpuUsage /= improvementFactor;
            metrics.memoryUsage /= improvementFactor;
            metrics.customMetrics["inference_speed"] *= improvementFactor;
            metrics.customMetrics["accuracy"] *= 1.02; // 2% accuracy improvement
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error optimizing brain performance: " << e.what() << std::endl;
        return false;
    }
}

bool BrainPerformanceTool::CollectPerformanceData(const std::string& brainPath, std::vector<PerformanceMetrics>& metrics) {
    try {
        // Collect performance data for a brain
        metrics.clear();
        
        for (int i = 0; i < 10; ++i) { // Collect 10 data points
            PerformanceMetrics m;
            if (!BenchmarkBrainPerformance(brainPath, m)) {
                return false;
            }
            
            // Add timestamp
            m.timestamp = std::chrono::system_clock::now() - 
                         std::chrono::seconds(600 - i * 60); // Spread over 10 minutes
            
            metrics.push_back(m);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error collecting performance data: " << e.what() << std::endl;
        return false;
    }
}

bool BrainPerformanceTool::AnalyzePerformanceTrends(const std::vector<PerformanceMetrics>& metrics) {
    try {
        if (metrics.empty()) {
            return false;
        }
        
        // Analyze performance trends
        double totalCpu = 0.0, totalMem = 0.0;
        double minCpu = std::numeric_limits<double>::max(), maxCpu = std::numeric_limits<double>::min();
        double minMem = std::numeric_limits<double>::max(), maxMem = std::numeric_limits<double>::min();
        
        for (const auto& m : metrics) {
            totalCpu += m.cpuUsage;
            totalMem += m.memoryUsage;
            
            if (m.cpuUsage < minCpu) minCpu = m.cpuUsage;
            if (m.cpuUsage > maxCpu) maxCpu = m.cpuUsage;
            
            if (m.memoryUsage < minMem) minMem = m.memoryUsage;
            if (m.memoryUsage > maxMem) maxMem = m.memoryUsage;
        }
        
        double avgCpu = totalCpu / metrics.size();
        double avgMem = totalMem / metrics.size();
        
        // Check if performance is within thresholds
        double cpuThreshold = performanceThresholds["cpu_usage"];
        double memThreshold = performanceThresholds["memory_usage"];
        
        bool cpuOk = avgCpu <= cpuThreshold;
        bool memOk = avgMem <= memThreshold;
        
        if (!cpuOk) {
            std::cout << "WARNING: Average CPU usage " << avgCpu << "% exceeds threshold " << cpuThreshold << "%\n";
        }
        
        if (!memOk) {
            std::cout << "WARNING: Average memory usage " << avgMem << " MB exceeds threshold " << memThreshold << " MB\n";
        }
        
        // Calculate performance score
        double cpuScore = std::min(1.0, 1.0 - (avgCpu / cpuThreshold));
        double memScore = std::min(1.0, 1.0 - (avgMem / memThreshold));
        
        double performanceScore = (cpuScore + memScore) / 2.0;
        
        std::cout << "Performance Analysis:\n";
        std::cout << "  CPU usage: " << avgCpu << "% (min: " << minCpu << "%, max: " << maxCpu << "%)\n";
        std::cout << "  Memory usage: " << avgMem << " MB (min: " << minMem << " MB, max: " << maxMem << " MB)\n";
        std::cout << "  Performance score: " << performanceScore << "\n";
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error analyzing performance trends: " << e.what() << std::endl;
        return false;
    }
}

bool BrainPerformanceTool::GeneratePerformanceReport(const std::vector<PerformanceMetrics>& metrics, const std::string& reportPath) const {
    try {
        // Generate performance report
        std::ofstream file(reportPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# Brain Performance Report\n";
        file << "# Generated: " << formatTime(std::chrono::system_clock::now()) << "\n\n";
        
        if (metrics.empty()) {
            file << "No performance data available.\n";
            return true;
        }
        
        file << "## Summary\n";
        file << "Data points: " << metrics.size() << "\n";
        
        double totalCpu = 0.0, totalMem = 0.0;
        for (const auto& m : metrics) {
            totalCpu += m.cpuUsage;
            totalMem += m.memoryUsage;
        }
        
        double avgCpu = totalCpu / metrics.size();
        double avgMem = totalMem / metrics.size();
        
        file << "Average CPU usage: " << avgCpu << "%\n";
        file << "Average memory usage: " << avgMem << " MB\n";
        
        // Performance details
        file << "\n## Detailed Metrics\n";
        file << "CPU Usage:\n";
        for (const auto& m : metrics) {
            file << "  " << formatTime(m.timestamp) << ": " << m.cpuUsage << "%\n";
        }
        
        file << "\nMemory Usage:\n";
        for (const auto& m : metrics) {
            file << "  " << formatTime(m.timestamp) << ": " << m.memoryUsage << " MB\n";
        }
        
        // Custom metrics summary
        std::map<std::string, std::vector<double>> customMetricValues;
        for (const auto& m : metrics) {
            for (const auto& pair : m.customMetrics) {
                customMetricValues[pair.first].push_back(pair.second);
            }
        }
        
        if (!customMetricValues.empty()) {
            file << "\nCustom Metrics:\n";
            for (const auto& pair : customMetricValues) {
                double sum = 0.0;
                for (double val : pair.second) sum += val;
                double avg = sum / pair.second.size();
                file << "  " << pair.first << ": " << avg << " (avg of " << pair.second.size() << " samples)\n";
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error generating performance report: " << e.what() << std::endl;
        return false;
    }
}

bool BrainPerformanceTool::CreatePerformanceBenchmark(const std::string& benchmarkPath, const PerformanceMetrics& metrics) {
    try {
        // Create a performance benchmark file
        std::ofstream file(benchmarkPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# Performance Benchmark\n";
        file << "# Created: " << formatTime(metrics.timestamp) << "\n\n";
        
        file << "cpu_usage: " << metrics.cpuUsage << "\n";
        file << "memory_usage: " << metrics.memoryUsage << "\n";
        file << "disk_usage: " << metrics.diskUsage << "\n";
        file << "network_usage: " << metrics.networkUsage << "\n";
        
        file << "\ncustom_metrics:\n";
        for (const auto& pair : metrics.customMetrics) {
            file << "  " << pair.first << ": " << pair.second << "\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating performance benchmark: " << e.what() << std::endl;
        return false;
    }
}

bool BrainPerformanceTool::ComparePerformance(const std::string& brainPath1, const std::string& brainPath2) {
    try {
        // Compare performance of two brains
        PerformanceMetrics metrics1, metrics2;
        
        if (!BenchmarkBrainPerformance(brainPath1, metrics1) || 
            !BenchmarkBrainPerformance(brainPath2, metrics2)) {
            return false;
        }
        
        std::cout << "Brain Performance Comparison:\n";
        std::cout << "==========================\n";
        std::cout << "Brain 1: " << brainPath1 << "\n";
        std::cout << "  CPU usage: " << metrics1.cpuUsage << "%\n";
        std::cout << "  Memory usage: " << metrics1.memoryUsage << " MB\n";
        std::cout << "  Inference speed: " << metrics1.customMetrics["inference_speed"] << " ms\n";
        std::cout << "\nBrain 2: " << brainPath2 << "\n";
        std::cout << "  CPU usage: " << metrics2.cpuUsage << "%\n";
        std::cout << "  Memory usage: " << metrics2.memoryUsage << " MB\n";
        std::cout << "  Inference speed: " << metrics2.customMetrics["inference_speed"] << " ms\n";
        
        // Determine which brain performs better
        double brain1Score = 100.0 / metrics1.cpuUsage + 100.0 / metrics1.memoryUsage + metrics1.customMetrics["accuracy"] * 100.0;
        double brain2Score = 100.0 / metrics2.cpuUsage + 100.0 / metrics2.memoryUsage + metrics2.customMetrics["accuracy"] * 100.0;
        
        std::cout << "\nPerformance Scores:\n";
        std::cout << "  Brain 1: " << brain1Score << "\n";
        std::cout << "  Brain 2: " << brain2Score << "\n";
        
        if (brain1Score > brain2Score) {
            std::cout << "  Winner: Brain 1\n";
        } else if (brain2Score > brain1Score) {
            std::cout << "  Winner: Brain 2\n";
        } else {
            std::cout << "  Tie\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error comparing performance: " << e.what() << std::endl;
        return false;
    }
}

bool BrainPerformanceTool::SetPerformanceThresholds(const std::map<std::string, double>& thresholds) {
    performanceThresholds = thresholds;
    return true;
}

bool BrainPerformanceTool::ExportPerformanceData(const std::string& outputPath) const {
    try {
        // Export performance data
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# Performance Data Export\n";
        file << "# Exported: " << formatTime(std::chrono::system_clock::now()) << "\n\n";
        
        file << "thresholds:\n";
        for (const auto& pair : performanceThresholds) {
            file << "  " << pair.first << ": " << pair.second << "\n";
        }
        
        if (!performanceHistory.empty()) {
            file << "\nperformance_history:\n";
            for (size_t i = 0; i < performanceHistory.size(); ++i) {
                const auto& m = performanceHistory[i];
                file << "  entry_" << i << ":\n";
                file << "    timestamp: " << formatTime(m.timestamp) << "\n";
                file << "    cpu_usage: " << m.cpuUsage << "\n";
                file << "    memory_usage: " << m.memoryUsage << "\n";
                file << "    disk_usage: " << m.diskUsage << "\n";
                file << "    network_usage: " << m.networkUsage << "\n";
                file << "    custom_metrics:\n";
                for (const auto& pair : m.customMetrics) {
                    file << "      " << pair.first << ": " << pair.second << "\n";
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting performance data: " << e.what() << std::endl;
        return false;
    }
}

bool BrainPerformanceTool::ImportPerformanceData(const std::string& inputPath) {
    try {
        // Import performance data
        std::ifstream file(inputPath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        std::string currentEntry;
        PerformanceMetrics currentMetrics;
        
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            if (line.find(":") != std::string::npos && line[0] != ' ') {
                // New field
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    std::string field = line.substr(0, colonPos);
                    std::string value = line.substr(colonPos + 1);
                    
                    if (field == "timestamp") {
                        currentMetrics.timestamp = parseTime(value);
                    } else if (field == "cpu_usage") {
                        currentMetrics.cpuUsage = std::stod(value);
                    } else if (field == "memory_usage") {
                        currentMetrics.memoryUsage = std::stod(value);
                    } else if (field == "disk_usage") {
                        currentMetrics.diskUsage = std::stod(value);
                    } else if (field == "network_usage") {
                        currentMetrics.networkUsage = std::stod(value);
                    }
                }
            } else if (line.find("  ") != std::string::npos && line[0] == ' ') {
                // Custom metric
                size_t spacePos = line.find(' ');
                if (spacePos != std::string::npos) {
                    std::string field = line.substr(2, spacePos - 2);
                    std::string value = line.substr(spacePos + 1);
                    currentMetrics.customMetrics[field] = std::stod(value);
                }
            } else if (line.find("entry_") != std::string::npos && line.find(":") == std::string::npos) {
                // New entry
                if (!currentEntry.empty()) {
                    // Save previous entry
                    performanceHistory.push_back(currentMetrics);
                }
                currentEntry = line;
                currentMetrics = PerformanceMetrics();
            }
        }
        
        // Save last entry
        if (!currentEntry.empty()) {
            performanceHistory.push_back(currentMetrics);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error importing performance data: " << e.what() << std::endl;
        return false;
    }
}

// Helper functions
std::string BrainPerformanceTool::formatTime(const std::chrono::system_clock::time_point& tp) const {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}

std::chrono::system_clock::time_point BrainPerformanceTool::parseTime(const std::string& timeStr) const {
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

} // namespace cli
} // namespace nlm