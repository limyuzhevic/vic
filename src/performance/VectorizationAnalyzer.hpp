#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <memory>

namespace nlm {

// VectorizationAnalyzer: SIMD and vectorization efficiency analysis
class VectorizationAnalyzer {
public:
    VectorizationAnalyzer();
    ~VectorizationAnalyzer();
    
    // Configuration
    void setEnableSIMDProfiling(bool enable) { enableSIMDProfiling_ = enable; }
    void setEnableVectorizationAnalysis(bool enable) { enableVectorizationAnalysis_ = enable; }
    void setMinVectorizationSamples(size_t min) { minVectorizationSamples_ = min; }
    
    // Vectorization tracking
    void recordVectorizationMeasurement(const std::string& operation, size_t vectorWidth,
                                        double scalarTime, double vectorTime,
                                        double memoryBandwidth, double computeCapability,
                                        const std::string& context = "");
    
    // Query results
    double getAverageVectorizationGain(const std::string& operation, size_t vectorWidth) const;
    double getAverageMemoryBandwidth(const std::string& operation, size_t vectorWidth) const;
    double getAverageComputeCapability(const std::string& operation, size_t vectorWidth) const;
    
    // Vectorization listing
    std::vector<std::string> getVectorizedOperations() const;
    std::vector<size_t> getVectorWidths() const;
    
    // Analysis
    std::string analyzeVectorizationEfficiency() const;
    std::string analyzeSIMDUtilization() const;
    std::string identifyVectorizationBottlenecks() const;
    std::string recommendVectorizationStrategies() const;
    
    // Statistics
    size_t getTotalVectorizationMeasurements() const { return totalMeasurements_; }
    std::string getOptimalVectorWidth(const std::string& operation) const;
    
    // Export
    bool exportVectorizationProfile(const std::string& filepath) const;
    std::string exportVectorizationProfileAsString() const;
    
    // Real-time analysis
    std::string getRealTimeVectorizationStats() const;
    
private:
    struct VectorizationMeasurement {
        std::chrono::system_clock::time_point timestamp;
        std::string operation;
        size_t vectorWidth;
        double scalarTime;
        double vectorTime;
        double memoryBandwidth;
        double computeCapability;
        std::string context;
        
        VectorizationMeasurement(const std::string& op, size_t width, double scalarTime,
                                double vectorTime, double bandwidth, double compute,
                                const std::string& ctx)
            : operation(op), vectorWidth(width), scalarTime(scalarTime), vectorTime(vectorTime),
              memoryBandwidth(bandwidth), computeCapability(compute), context(ctx) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    std::vector<VectorizationMeasurement> measurements_;
    mutable std::mutex measurementsMutex_;
    
    bool enableSIMDProfiling_;
    bool enableVectorizationAnalysis_;
    size_t minVectorizationSamples_;
    size_t totalMeasurements_;
    
    // Helper methods
    void cleanupOldMeasurements();
    double calculateVectorizationGain(double scalarTime, double vectorTime) const;
    double calculateMemoryEfficiency(double memoryBandwidth, size_t vectorWidth) const;
    double calculateComputeEfficiency(double computeCapability, size_t vectorWidth) const;
    std::string getVectorWidthDescription(size_t width) const;
    bool isSIMDRecommended(double gain) const;
    bool isMemoryBound(double memoryBandwidth, double computeCapability) const;
};

} // namespace nlm
