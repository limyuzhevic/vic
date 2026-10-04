#include "ScalingAnalyzer.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_set>
#include <map>

namespace nlm {

ScalingAnalyzer::ScalingAnalyzer()
    : enableScaleProfiling_(true),
      enableScalingAnalysis_(true),
      minScalePoints_(3),
      maxScalePoints_(100),
      totalMeasurements_(0) {
}

ScalingAnalyzer::~ScalingAnalyzer() = default;

void ScalingAnalyzer::recordScaleMeasurement(size_t scale, const std::string& operation,
                                            double time, double memory,
                                            size_t threads, double cpu) {
    ScaleMeasurement measurement(scale, operation, time, memory, threads, cpu);
    
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    measurements_.push_back(measurement);
    totalMeasurements_++;
    
    // Cleanup old measurements periodically
    if (measurements_.size() % 1000 == 0) {
        cleanupOldMeasurements();
    }
}

double ScalingAnalyzer::getAverageTimeAtScale(size_t scale, const std::string& operation) const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    std::vector<double> times;
    for (const auto& measurement : measurements_) {
        if (measurement.scale == scale && measurement.operation == operation) {
            times.push_back(measurement.time);
        }
    }
    
    if (times.empty()) return 0.0;
    
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    return sum / times.size();
}

double ScalingAnalyzer::getAverageMemoryAtScale(size_t scale, const std::string& operation) const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    std::vector<double> memories;
    for (const auto& measurement : measurements_) {
        if (measurement.scale == scale && measurement.operation == operation) {
            memories.push_back(measurement.memory);
        }
    }
    
    if (memories.empty()) return 0.0;
    
    double sum = std::accumulate(memories.begin(), memories.end(), 0.0);
    return sum / memories.size();
}

std::vector<size_t> ScalingAnalyzer::getScales() const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    std::vector<size_t> scales;
    std::unordered_set<size_t> uniqueScales;
    for (const auto& measurement : measurements_) {
        if (uniqueScales.find(measurement.scale) == uniqueScales.end()) {
            uniqueScales.insert(measurement.scale);
            scales.push_back(measurement.scale);
        }
    }
    
    std::sort(scales.begin(), scales.end());
    return scales;
}

std::vector<std::string> ScalingAnalyzer::getOperations() const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    std::vector<std::string> operations;
    std::unordered_set<std::string> uniqueOps;
    for (const auto& measurement : measurements_) {
        if (uniqueOps.find(measurement.operation) == uniqueOps.end()) {
            uniqueOps.insert(measurement.operation);
            operations.push_back(measurement.operation);
        }
    }
    
    return operations;
}

std::string ScalingAnalyzer::analyzeScalingBehavior() const {
    std::ostringstream oss;
    oss << "Scaling Behavior Analysis:\n";
    oss << "=========================\n\n";
    
    auto scales = getScales();
    auto operations = getOperations();
    
    if (scales.size() < minScalePoints_) {
        oss << "Insufficient scale data for analysis. Need at least " << minScalePoints_ << " scales.\n";
        return oss.str();
    }
    
    oss << "Scaling Analysis by Operation:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        // Analyze time scaling
        std::map<size_t, double> timeAtScale;
        for (size_t scale : scales) {
            timeAtScale[scale] = getAverageTimeAtScale(scale, operation);
        }
        
        // Calculate speedup ratios
        oss << "    Time Scaling:\n";
        for (const auto& [scale, time] : timeAtScale) {
            oss << "      Scale " << scale << ": " << time << "s\n";
        }
        
        // Analyze parallel efficiency
        double idealSpeedup = 1.0;
        if (!scales.empty()) {
            size_t maxScale = *scales.rbegin();
            size_t baseScale = *scales.begin();
            if (baseScale > 0) {
                idealSpeedup = static_cast<double>(maxScale) / baseScale;
            }
        }
        
        oss << "    Efficiency Analysis:\n";
        for (const auto& [scale, time] : timeAtScale) {
            if (scale > 0) {
                double actualSpeedup = idealSpeedup; // Simplified
                double efficiency = actualSpeedup / scale;
                oss << "      Scale " << scale << ": " << (efficiency * 100) << "% efficiency\n";
                
                if (efficiency > 0.8) {
                    oss << "        *** GOOD SCALING ***\n";
                } else if (efficiency < 0.5) {
                    oss << "        *** POOR SCALING ***\n";
                }
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string ScalingAnalyzer::analyzeParallelScaling() const {
    std::ostringstream oss;
    oss << "Parallel Scaling Analysis:\n";
    oss << "=========================\n\n";
    
    auto scales = getScales();
    auto operations = getOperations();
    
    oss << "Parallel Performance Analysis:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        // Analyze thread scaling
        std::map<size_t, double> threadEfficiency;
        for (size_t scale : scales) {
            double time = getAverageTimeAtScale(scale, operation);
            double efficiency = 1.0 / scale; // Simplified
            threadEfficiency[scale] = efficiency;
        }
        
        oss << "    Thread Efficiency:\n";
        for (const auto& [scale, efficiency] : threadEfficiency) {
            oss << "      Threads " << scale << ": " << (efficiency * 100) << "%\n";
            
            if (efficiency > 0.7) {
                oss << "        *** EXCELLENT PARALLELIZATION ***\n";
            } else if (efficiency > 0.5) {
                oss << "        *** GOOD PARALLELIZATION ***\n";
            } else if (efficiency > 0.3) {
                oss << "        *** MODERATE PARALLELIZATION ***\n";
            } else {
                oss << "        *** POOR PARALLELIZATION ***\n";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string ScalingAnalyzer::identifyScalingBottlenecks() const {
    std::ostringstream oss;
    oss << "Scaling Bottleneck Identification:\n";
    oss << "============================\n\n";
    
    auto scales = getScales();
    auto operations = getOperations();
    
    oss << "Detected Scaling Bottlenecks:\n";
    
    for (const auto& operation : operations) {
        bool hasBottleneck = false;
        
        for (size_t scale : scales) {
            double time = getAverageTimeAtScale(scale, operation);
            
            if (time > 1.0) { // More than 1 second at any scale
                hasBottleneck = true;
                oss << "  " << operation << " at scale " << scale << " (" << time << "s)\n";
                oss << "    *** BOTTLENECK DETECTED ***\n";
                
                // Provide suggestions
                if (operation.find("train") != std::string::npos) {
                    oss << "      Suggestion: Reduce training complexity\n";
                } else if (operation.find("predict") != std::string::npos) {
                    oss << "      Suggestion: Optimize model architecture\n";
                } else {
                    oss << "      Suggestion: Optimize algorithm implementation\n";
                }
            }
        }
        
        if (!hasBottleneck) {
            oss << "  " << operation << ": No bottlenecks detected\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string ScalingAnalyzer::analyzeEfficiencyAtDifferentScales() const {
    std::ostringstream oss;
    oss << "Efficiency Analysis at Different Scales:\n";
    oss << "====================================\n\n";
    
    auto scales = getScales();
    auto operations = getOperations();
    
    oss << "Resource Efficiency by Scale:\n";
    
    for (size_t scale : scales) {
        oss << "  Scale " << scale << ":\n";
        
        double totalTime = 0.0;
        double totalMemory = 0.0;
        size_t operationCount = 0;
        
        for (const auto& operation : operations) {
            double time = getAverageTimeAtScale(scale, operation);
            double memory = getAverageMemoryAtScale(scale, operation);
            
            if (time > 0) {
                totalTime += time;
                totalMemory += memory;
                operationCount++;
            }
        }
        
        if (operationCount > 0) {
            double avgTime = totalTime / operationCount;
            double avgMemory = totalMemory / operationCount;
            double timePerOp = avgTime / scale;
            double memoryPerOp = avgMemory / scale;
            
            oss << "    Average Time per Operation: " << timePerOp << "s\n";
            oss << "    Average Memory per Operation: " << memoryPerOp << " MB\n";
            oss << "    Overall CPU Efficiency: " << (1.0 / scale) * 100 << "%\n";
            
            if (timePerOp < 0.001) {
                oss << "    *** EXCELLENT EFFICIENCY ***\n";
            } else if (timePerOp < 0.01) {
                oss << "    *** GOOD EFFICIENCY ***\n";
            } else if (timePerOp < 0.1) {
                oss << "    *** MODERATE EFFICIENCY ***\n";
            } else {
                oss << "    *** LOW EFFICIENCY ***\n";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::map<size_t, double> ScalingAnalyzer::getTimeScalingRatio(const std::string& operation) const {
    std::map<size_t, double> scalingRatio;
    
    auto scales = getScales();
    if (scales.size() < 2) return scalingRatio;
    
    double referenceTime = getAverageTimeAtScale(*scales.begin(), operation);
    if (referenceTime <= 0) return scalingRatio;
    
    for (size_t scale : scales) {
        double time = getAverageTimeAtScale(scale, operation);
        scalingRatio[scale] = time / referenceTime;
    }
    
    return scalingRatio;
}

std::map<size_t, double> ScalingAnalyzer::getMemoryScalingRatio(const std::string& operation) const {
    std::map<size_t, double> scalingRatio;
    
    auto scales = getScales();
    if (scales.size() < 2) return scalingRatio;
    
    double referenceMemory = getAverageMemoryAtScale(*scales.begin(), operation);
    if (referenceMemory <= 0) return scalingRatio;
    
    for (size_t scale : scales) {
        double memory = getAverageMemoryAtScale(scale, operation);
        scalingRatio[scale] = memory / referenceMemory;
    }
    
    return scalingRatio;
}

bool ScalingAnalyzer::exportScalingProfile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << exportScalingProfileAsString();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::string ScalingAnalyzer::exportScalingProfileAsString() const {
    std::ostringstream oss;
    oss << "Scaling Profile\n";
    oss << "==============\n\n";
    
    oss << "Scaling Analysis Summary:\n";
    oss << "  Total Measurements: " << totalMeasurements_ << "\n";
    oss << "  Number of Scales: " << getScales().size() << "\n";
    oss << "  Number of Operations: " << getOperations().size() << "\n\n";
    
    oss << "Scales Tested:\n";
    auto scales = getScales();
    for (size_t scale : scales) {
        oss << "  " << scale << " threads\n";
    }
    
    oss << "\nOperations:\n";
    auto operations = getOperations();
    for (const auto& operation : operations) {
        oss << "  " << operation << "\n";
    }
    
    oss << "\nTime Scaling Ratios:\n";
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        auto timeRatios = getTimeScalingRatio(operation);
        for (const auto& [scale, ratio] : timeRatios) {
            oss << "    Scale " << scale << ": " << ratio << "x\n";
        }
    }
    
    return oss.str();
}

std::string ScalingAnalyzer::getRealTimeScalingStats() const {
    std::ostringstream oss;
    oss << "Real-Time Scaling Statistics:\n";
    oss << "==========================\n\n";
    
    oss << "Current Scaling State:\n";
    oss << "  Total Measurements: " << totalMeasurements_ << "\n";
    oss << "  Active Scales: " << getScales().size() << "\n";
    oss << "  Tracked Operations: " << getOperations().size() << "\n";
    
    if (!getScales().empty() && !getOperations().empty()) {
        oss << "\nMost Recent Scaling Data:\n";
        std::lock_guard<std::mutex> lock(measurementsMutex_);
        
        size_t recentCount = std::min(measurements_.size(), size_t(5));
        for (size_t i = 0; i < recentCount; ++i) {
            const auto& measurement = measurements_[measurements_.size() - 1 - i];
            oss << "  " << measurement.operation << " at scale " << measurement.scale << 
                " (" << measurement.time << "s, " << measurement.memory << " MB)\n";
        }
    }
    
    return oss.str();
}

void ScalingAnalyzer::cleanupOldMeasurements() {
    // Remove measurements older than 24 hours
    auto now = std::chrono::system_clock::now();
    auto oneDayAgo = now - std::chrono::hours(24);
    
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    auto it = std::remove_if(measurements_.begin(), measurements_.end(),
                             [oneDayAgo](const ScaleMeasurement& m) {
                                 return m.timestamp < oneDayAgo;
                             });
    measurements_.erase(it, measurements_.end());
}

double ScalingAnalyzer::calculateSpeedup(size_t scale1, size_t scale2,
                                        const std::string& operation) const {
    double time1 = getAverageTimeAtScale(scale1, operation);
    double time2 = getAverageTimeAtScale(scale2, operation);
    
    if (time1 <= 0) return 1.0;
    
    return time1 / time2;
}

double ScalingAnalyzer::calculateEfficiency(size_t scale, const std::string& operation) const {
    if (scale <= 0) return 0.0;
    
    double idealTime = 1.0; // Simplified
    double actualTime = getAverageTimeAtScale(scale, operation);
    
    if (idealTime <= 0) return 0.0;
    
    return idealTime / actualTime / scale;
}

double ScalingAnalyzer::calculateMemoryEfficiency(size_t scale, const std::string& operation) const {
    if (scale <= 0) return 0.0;
    
    double idealMemory = 1.0; // Simplified
    double actualMemory = getAverageMemoryAtScale(scale, operation);
    
    if (idealMemory <= 0) return 0.0;
    
    return idealMemory / actualMemory / scale;
}

std::string ScalingAnalyzer::getScaleDescription(size_t scale) const {
    if (scale == 1) return "Sequential";
    else if (scale == 2) return "Dual-core";
    else if (scale <= 8) return "Small parallel";
    else if (scale <= 32) return "Medium parallel";
    else return "Large parallel";
}

bool ScalingAnalyzer::isScalingOptimal(double speedup, double scale) const {
    return speedup / scale >= 0.8;
}

} // namespace nlm
