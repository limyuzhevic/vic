#pragma once

#include "MonitoringUtils.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include <numeric>

namespace nlm {

std::string MonitoringUtils::formatBytes(uint64_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB", "PB"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unitIndex < 5) {
        size /= 1024.0;
        unitIndex++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
    return ss.str();
}

std::string MonitoringUtils::formatDuration(std::chrono::duration<double> duration) {
    auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    auto hours = totalSeconds / 3600;
    auto minutes = (totalSeconds % 3600) / 60;
    auto seconds = totalSeconds % 60;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
    
    std::stringstream ss;
    if (hours > 0) {
        ss << hours << "h ";
    }
    if (minutes > 0 || hours > 0) {
        ss << minutes << "m ";
    }
    ss << seconds << "." << std::setfill('0') << std::setw(3) << milliseconds << "s";
    
    return ss.str();
}

std::string MonitoringUtils::formatTimestamp(std::chrono::system_clock::time_point timestamp) {
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

double MonitoringUtils::calculatePercentage(double value, double total) {
    if (total == 0.0) return 0.0;
    return (value / total) * 100.0;
}

double MonitoringUtils::calculateTrend(const std::vector<double>& values) {
    if (values.size() < 2) return 0.0;
    
    size_t n = values.size();
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        double y = values[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    return slope;
}

std::vector<double> MonitoringUtils::movingAverage(const std::vector<double>& values, size_t window) {
    std::vector<double> result;
    if (values.empty() || window == 0) return result;
    
    result.reserve(values.size());
    double windowSum = 0.0;
    size_t windowSize = std::min(window, values.size());
    
    // Initial window
    for (size_t i = 0; i < windowSize; ++i) {
        windowSum += values[i];
    }
    result.push_back(windowSum / windowSize);
    
    // Slide window
    for (size_t i = windowSize; i < values.size(); ++i) {
        windowSum -= values[i - windowSize];
        windowSum += values[i];
        result.push_back(windowSum / windowSize);
    }
    
    return result;
}

std::vector<size_t> MonitoringUtils::detectOutliers(const std::vector<double>& values, double threshold) {
    std::vector<size_t> outliers;
    if (values.size() < 2) return outliers;
    
    double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    double variance = 0.0;
    
    for (double value : values) {
        variance += (value - mean) * (value - mean);
    }
    variance /= values.size();
    
    double stdDev = std::sqrt(variance);
    
    for (size_t i = 0; i < values.size(); ++i) {
        if (stdDev > 0.0 && std::abs(values[i] - mean) > threshold * stdDev) {
            outliers.push_back(i);
        }
    }
    
    return outliers;
}

std::string MonitoringUtils::getSystemInfo() {
    std::stringstream ss;
    ss << "System Information:\n";
    ss << "  Platform: Linux\n";
    ss << "  Architecture: x86_64\n";
    ss << "  CPU Cores: " << std::thread::hardware_concurrency() << "\n";
    ss << "  Memory: " << getCurrentMemoryUsage() << " MB\n";
    ss << "  CPU Usage: " << getCurrentCPUUsage() << "%\n";
    return ss.str();
}

double MonitoringUtils::getCurrentMemoryUsage() {
    // This is a simplified implementation
    // In a real implementation, you would read from /proc/self/status or use platform-specific APIs
    double memoryMB = 0.0;
    
    // Try to read from /proc/self/status
    std::ifstream statusFile("/proc/self/status");
    if (statusFile.is_open()) {
        std::string line;
        while (std::getline(statusFile, line)) {
            if (line.substr(0, 6) == "VmRSS:") {
                std::string value = line.substr(7);
                // Extract numeric value and units
                size_t spacePos = value.find(' ');
                if (spacePos != std::string::npos) {
                    double memValue = std::stod(value.substr(0, spacePos));
                    char unit = value[spacePos];
                    
                    switch (unit) {
                        case 'k': memoryMB = memValue / 1024.0; break;
                        case 'm': memoryMB = memValue; break;
                        case 'g': memoryMB = memValue * 1024.0; break;
                        default: memoryMB = memValue / 1024.0; break;
                    }
                }
                break;
            }
        }
    }
    
    return memoryMB;
}

double MonitoringUtils::getCurrentCPUUsage() {
    // This is a simplified implementation
    // In a real implementation, you would read from /proc/stat or use platform-specific APIs
    static double lastCPUUsage = 0.0;
    static std::chrono::system_clock::time_point lastTime;
    
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime).count();
    
    if (elapsed >= 1.0) {
        // Read CPU times from /proc/stat
        std::ifstream statFile("/proc/stat");
        if (statFile.is_open()) {
            std::string line;
            while (std::getline(statFile, line)) {
                if (line.substr(0, 3) == "cpu") {
                    std::istringstream iss(line);
                    std::string cpuLabel;
                    unsigned long user, nice, system, idle, iowait, irq, softirq, steal, total;
                    
                    iss >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
                    
                    // Calculate CPU usage percentage
                    unsigned long idle_total = idle + iowait;
                    unsigned long total_work = user + nice + system + irq + softirq + steal;
                    unsigned long total = total_work + idle_total;
                    
                    if (total > 0) {
                        lastCPUUsage = 100.0 * (1.0 - static_cast<double>(idle_total) / total);
                    }
                    
                    lastTime = now;
                    break;
                }
            }
        }
    }
    
    return lastCPUUsage;
}

} // namespace nlm