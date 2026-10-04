#include "VectorizationAnalyzer.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_set>

namespace nlm {

VectorizationAnalyzer::VectorizationAnalyzer()
    : enableSIMDProfiling_(true),
      enableVectorizationAnalysis_(true),
      minVectorizationSamples_(3),
      totalMeasurements_(0) {
}

VectorizationAnalyzer::~VectorizationAnalyzer() = default;

void VectorizationAnalyzer::recordVectorizationMeasurement(const std::string& operation, 
                                                           size_t vectorWidth,
                                                           double scalarTime, 
                                                           double vectorTime,
                                                           double memoryBandwidth,
                                                           double computeCapability,
                                                           const std::string& context) {
    VectorizationMeasurement measurement(operation, vectorWidth, scalarTime, vectorTime,
                                        memoryBandwidth, computeCapability, context);
    
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    measurements_.push_back(measurement);
    totalMeasurements_++;
    
    // Cleanup old measurements periodically
    if (measurements_.size() % 1000 == 0) {
        cleanupOldMeasurements();
    }
}

double VectorizationAnalyzer::getAverageVectorizationGain(const std::string& operation, 
                                                         size_t vectorWidth) const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    std::vector<double> gains;
    for (const auto& measurement : measurements_) {
        if (measurement.operation == operation && measurement.vectorWidth == vectorWidth) {
            double gain = calculateVectorizationGain(measurement.scalarTime, measurement.vectorTime);
            gains.push_back(gain);
        }
    }
    
    if (gains.empty()) return 0.0;
    
    double sum = std::accumulate(gains.begin(), gains.end(), 0.0);
    return sum / gains.size();
}

double VectorizationAnalyzer::getAverageMemoryBandwidth(const std::string& operation, 
                                                        size_t vectorWidth) const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    std::vector<double> bandwidths;
    for (const auto& measurement : measurements_) {
        if (measurement.operation == operation && measurement.vectorWidth == vectorWidth) {
            bandwidths.push_back(measurement.memoryBandwidth);
        }
    }
    
    if (bandwidths.empty()) return 0.0;
    
    double sum = std::accumulate(bandwidths.begin(), bandwidths.end(), 0.0);
    return sum / bandwidths.size();
}

double VectorizationAnalyzer::getAverageComputeCapability(const std::string& operation, 
                                                         size_t vectorWidth) const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    std::vector<double> capabilities;
    for (const auto& measurement : measurements_) {
        if (measurement.operation == operation && measurement.vectorWidth == vectorWidth) {
            capabilities.push_back(measurement.computeCapability);
        }
    }
    
    if (capabilities.empty()) return 0.0;
    
    double sum = std::accumulate(capabilities.begin(), capabilities.end(), 0.0);
    return sum / capabilities.size();
}

std::vector<std::string> VectorizationAnalyzer::getVectorizedOperations() const {
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

std::vector<size_t> VectorizationAnalyzer::getVectorWidths() const {
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    
    std::vector<size_t> widths;
    std::unordered_set<size_t> uniqueWidths;
    for (const auto& measurement : measurements_) {
        if (uniqueWidths.find(measurement.vectorWidth) == uniqueWidths.end()) {
            uniqueWidths.insert(measurement.vectorWidth);
            widths.push_back(measurement.vectorWidth);
        }
    }
    
    std::sort(widths.begin(), widths.end());
    return widths;
}

std::string VectorizationAnalyzer::analyzeVectorizationEfficiency() const {
    std::ostringstream oss;
    oss << "Vectorization Efficiency Analysis:\n";
    oss << "============================\n\n";
    
    auto operations = getVectorizedOperations();
    auto vectorWidths = getVectorWidths();
    
    oss << "Vectorization Efficiency by Operation:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        double maxGain = 0.0;
        size_t optimalWidth = 0;
        
        for (size_t width : vectorWidths) {
            double gain = getAverageVectorizationGain(operation, width);
            if (gain > maxGain) {
                maxGain = gain;
                optimalWidth = width;
            }
            
            oss << "    " << width << "-wide: " << (gain * 100) << "% gain\n";
            
            if (gain > 2.0) {
                oss << "      *** EXCELLENT GAIN ***\n";
            } else if (gain > 1.0) {
                oss << "      *** GOOD GAIN ***\n";
            } else if (gain > 0.5) {
                oss << "      *** MODERATE GAIN ***\n";
            } else {
                oss << "      *** POOR GAIN ***\n";
            }
        }
        
        oss << "    Optimal Vector Width: " << optimalWidth << " (" << (maxGain * 100) << "% gain)\n";
        oss << "\n";
    }
    
    return oss.str();
}

std::string VectorizationAnalyzer::analyzeSIMDUtilization() const {
    std::ostringstream oss;
    oss << "SIMD Utilization Analysis:\n";
    oss << "=========================\n\n";
    
    auto operations = getVectorizedOperations();
    auto vectorWidths = getVectorWidths();
    
    oss << "SIMD Utilization by Operation:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        for (size_t width : vectorWidths) {
            double gain = getAverageVectorizationGain(operation, width);
            double memoryBandwidth = getAverageMemoryBandwidth(operation, width);
            double computeCapability = getAverageComputeCapability(operation, width);
            
            oss << "    " << width << "-wide: " << (gain * 100) << "% speedup, " <<
                (memoryBandwidth * 100) << "% bandwidth, " <<
                (computeCapability * 100) << "% compute\n";
            
            if (isSIMDRecommended(gain)) {
                oss << "      *** SIMD RECOMMENDED ***\n";
            }
            
            if (isMemoryBound(memoryBandwidth, computeCapability)) {
                oss << "      *** MEMORY BOUND ***\n";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string VectorizationAnalyzer::identifyVectorizationBottlenecks() const {
    std::ostringstream oss;
    oss << "Vectorization Bottleneck Identification:\n";
    oss << "==============================\n\n";
    
    auto operations = getVectorizedOperations();
    auto vectorWidths = getVectorWidths();
    
    oss << "Identified Vectorization Bottlenecks:\n";
    
    for (const auto& operation : operations) {
        bool hasBottleneck = false;
        
        for (size_t width : vectorWidths) {
            double gain = getAverageVectorizationGain(operation, width);
            
            if (gain < 0.5) {
                hasBottleneck = true;
                oss << "  " << operation << " with " << width << "-wide vector: " <<
                    (gain * 100) << "% gain\n";
                oss << "    *** VECTORIZATION BOTTLENECK ***\n";
                
                if (width < 8) {
                    oss << "    Suggestion: Try wider vectorization\n";
                } else {
                    oss << "    Suggestion: Check memory access patterns\n";
                }
            }
        }
        
        if (!hasBottleneck) {
            oss << "  " << operation << ": No vectorization bottlenecks detected\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string VectorizationAnalyzer::recommendVectorizationStrategies() const {
    std::ostringstream oss;
    oss << "Vectorization Strategy Recommendations:\n";
    oss << "==============================\n\n";
    
    auto operations = getVectorizedOperations();
    
    oss << "Recommended Vectorization Strategies:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        size_t optimalWidth = std::stoull(getOptimalVectorWidth(operation).substr(
            getOptimalVectorWidth(operation).find_last_of(' ') + 1));
        
        oss << "    Recommended Vector Width: " << optimalWidth << "\n";
        
        if (optimalWidth == 8) {
            oss << "      - Use AVX2 instructions\n";
        } else if (optimalWidth == 16) {
            oss << "      - Use AVX512 instructions\n";
        } else if (optimalWidth == 4) {
            oss << "      - Use SSE instructions\n";
        } else {
            oss << "      - Use scalar or custom SIMD\n";
        }
        
        double gain = getAverageVectorizationGain(operation, optimalWidth);
        if (gain > 2.0) {
            oss << "      *** High priority optimization ***\n";
        } else if (gain > 1.0) {
            oss << "      *** Medium priority optimization ***\n";
        } else {
            oss << "      *** Low priority optimization ***\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string VectorizationAnalyzer::getOptimalVectorWidth(const std::string& operation) const {
    auto vectorWidths = getVectorWidths();
    
    if (vectorWidths.empty()) return "No data available";
    
    double maxGain = 0.0;
    size_t optimalWidth = vectorWidths[0];
    
    for (size_t width : vectorWidths) {
        double gain = getAverageVectorizationGain(operation, width);
        if (gain > maxGain) {
            maxGain = gain;
            optimalWidth = width;
        }
    }
    
    std::ostringstream oss;
    oss << "Optimal vector width for " << operation << ": " << optimalWidth << " (" << (maxGain * 100) << "% gain)";
    
    return oss.str();
}

bool VectorizationAnalyzer::exportVectorizationProfile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << exportVectorizationProfileAsString();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::string VectorizationAnalyzer::exportVectorizationProfileAsString() const {
    std::ostringstream oss;
    oss << "Vectorization Profile\n";
    oss << "====================\n\n";
    
    oss << "Vectorization Analysis Summary:\n";
    oss << "  Total Vectorization Measurements: " << totalMeasurements_ << "\n";
    oss << "  Number of Operations: " << getVectorizedOperations().size() << "\n";
    oss << "  Number of Vector Widths: " << getVectorWidths().size() << "\n\n";
    
    oss << "Vector Widths Analyzed:\n";
    auto vectorWidths = getVectorWidths();
    for (size_t width : vectorWidths) {
        oss << "  " << width << "-wide\n";
    }
    
    oss << "\nVectorized Operations:\n";
    auto operations = getVectorizedOperations();
    for (const auto& operation : operations) {
        oss << "  " << operation << "\n";
    }
    
    oss << "\nVectorization Gains by Operation:\n";
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        size_t optimalWidth = std::stoull(getOptimalVectorWidth(operation).substr(
            getOptimalVectorWidth(operation).find_last_of(' ') + 1));
        double gain = getAverageVectorizationGain(operation, optimalWidth);
        oss << "    Optimal: " << optimalWidth << "-wide (" << (gain * 100) << "% gain)\n";
    }
    
    return oss.str();
}

std::string VectorizationAnalyzer::getRealTimeVectorizationStats() const {
    std::ostringstream oss;
    oss << "Real-Time Vectorization Statistics:\n";
    oss << "==========================\n\n";
    
    oss << "Current Vectorization State:\n";
    oss << "  Total Vectorization Measurements: " << totalMeasurements_ << "\n";
    oss << "  Active Operations: " << getVectorizedOperations().size() << "\n";
    oss << "  Vector Widths Analyzed: " << getVectorWidths().size() << "\n";
    
    if (!getVectorizedOperations().empty() && !getVectorWidths().empty()) {
        oss << "\nRecent Vectorization Measurements:\n";
        std::lock_guard<std::mutex> lock(measurementsMutex_);
        
        size_t recentCount = std::min(measurements_.size(), size_t(5));
        for (size_t i = 0; i < recentCount; ++i) {
            const auto& measurement = measurements_[measurements_.size() - 1 - i];
            oss << "  " << measurement.operation << " with " << measurement.vectorWidth << 
                "-wide vector: " << (measurement.vectorTime / measurement.scalarTime) << "x speedup\n";
        }
    }
    
    return oss.str();
}

void VectorizationAnalyzer::cleanupOldMeasurements() {
    // Remove measurements older than 24 hours
    auto now = std::chrono::system_clock::now();
    auto oneDayAgo = now - std::chrono::hours(24);
    
    std::lock_guard<std::mutex> lock(measurementsMutex_);
    auto it = std::remove_if(measurements_.begin(), measurements_.end(),
                             [oneDayAgo](const VectorizationMeasurement& m) {
                                 return m.timestamp < oneDayAgo;
                             });
    measurements_.erase(it, measurements_.end());
}

double VectorizationAnalyzer::calculateVectorizationGain(double scalarTime, double vectorTime) const {
    if (scalarTime <= 0) return 0.0;
    return scalarTime / vectorTime - 1.0;
}

double VectorizationAnalyzer::calculateMemoryEfficiency(double memoryBandwidth, size_t vectorWidth) const {
    if (memoryBandwidth <= 0) return 0.0;
    return memoryBandwidth / vectorWidth;
}

double VectorizationAnalyzer::calculateComputeEfficiency(double computeCapability, size_t vectorWidth) const {
    if (computeCapability <= 0) return 0.0;
    return computeCapability / vectorWidth;
}

std::string VectorizationAnalyzer::getVectorWidthDescription(size_t width) const {
    if (width == 1) return "Scalar";
    else if (width == 2) return "SSE";
    else if (width == 4) return "SSE (AVX)";
    else if (width == 8) return "AVX2";
    else if (width == 16) return "AVX512";
    else return "Custom";
}

bool VectorizationAnalyzer::isSIMDRecommended(double gain) const {
    return gain > 1.0;
}

bool VectorizationAnalyzer::isMemoryBound(double memoryBandwidth, double computeCapability) const {
    return memoryBandwidth < computeCapability * 0.5;
}

} // namespace nlm
