#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace nlm {

    class MetricsCollector {
    public:
        struct PerformanceMetric {
            std::string name;
            double value;
            double timestamp;
            std::string unit;
            std::chrono::system_clock::time_point captureTime;
        };

        struct MemoryMetric {
            double totalMemoryMB;
            double usedMemoryMB;
            double availableMemoryMB;
            double memoryUsagePercent;
            std::chrono::system_clock::time_point timestamp;
            std::string memoryState; // "normal", "warning", "critical"
        };

        struct FiringRateMetric {
            uint64_t activeNeurons;
            uint64_t totalNeurons;
            float firingRateHz;
            float avgFiringRate;
            std::chrono::system_clock::time_point timestamp;
            std::string brainRegion;
        };

        struct LearningProgress {
            std::string algorithm;
            double progressPercent;
            uint64_t stepsCompleted;
            uint64_t targetSteps;
            double learningRate;
            std::chrono::system_clock::time_point timestamp;
            std::string learningPhase;
        };

        struct SystemHealth {
            bool isOperational;
            double cpuUsagePercent;
            double memoryUsagePercent;
            std::vector<std::string> activeComponents;
            std::vector<std::string> inactiveComponents;
            std::chrono::system_clock::time_point timestamp;
            std::string healthStatus; // "healthy", "degraded", "critical"
        };

    public:
        MetricsCollector();
        ~MetricsCollector();

        // Performance metrics collection
        void recordFiringRate(float firingRate, uint64_t activeNeurons, uint64_t totalNeurons,
                             const std::string& brainRegion = "default");
        void recordMemoryUsage(double memoryMB);
        void recordCpuUsage(double cpuPercent);
        void recordProcessingTime(const std::string& operation, double timeMs);
        void recordLearningProgress(const std::string& algorithm, double progress,
                                   uint64_t completedSteps, uint64_t targetSteps,
                                   const std::string& learningPhase = "unknown");
        void recordSystemHealth(const SystemHealth& health);

        // Metric queries
        std::vector<FiringRateMetric> getFiringRates(const std::string& brainRegion = "") const;
        std::vector<MemoryMetric> getMemoryUsageHistory() const;
        std::vector<PerformanceMetric> getProcessingTimes(const std::string& operation = "") const;
        std::vector<LearningProgress> getLearningProgress() const;
        std::vector<SystemHealth> getSystemHealthHistory() const;

        // Analysis methods
        double getAverageFiringRate(const std::string& brainRegion = "") const;
        double getAverageMemoryUsage() const;
        double getAverageCpuUsage() const;
        double getProcessingTime(const std::string& operation) const;
        double getLearningProgress(const std::string& algorithm) const;
        SystemHealth getCurrentSystemHealth() const;

        // Performance analysis
        struct PerformanceAnalysis {
            double avgFiringRate;
            double avgMemoryUsage;
            double avgCpuUsage;
            double minProcessingTime;
            double maxProcessingTime;
            double stdDevProcessingTime;
            std::string bottleneckDetection;
            std::vector<std::string> optimizationSuggestions;
        };

        PerformanceAnalysis analyzePerformance() const;

        // Learning analysis
        struct LearningAnalysis {
            std::string mostAdvancedAlgorithm;
            double fastestLearningRate;
            double bestProgressRate;
            std::string recommendedAlgorithm;
            std::vector<std::string> improvementSuggestions;
        };

        LearningAnalysis analyzeLearningProgress() const;

        // System health analysis
        struct HealthAnalysis {
            bool isSystemHealthy;
            std::string overallHealthStatus;
            std::vector<std::string> issues;
            std::vector<std::string> recommendations;
            std::chrono::system_clock::time_point lastHealthCheck;
        };

        HealthAnalysis analyzeSystemHealth() const;

        // Data management
        void clear();
        void clearOldData(std::chrono::hours olderThanHours = std::chrono::hours(24));
        size_t getDataSize() const;
        std::string getMetricsSummary() const;

        // Export
        std::string exportToCSV() const;
        std::string exportToJSON() const;
        std::string generatePerformanceReport() const;
        std::string generateLearningReport() const;
        std::string generateHealthReport() const;

        // Monitoring
        void startAutoCollection();
        void stopAutoCollection();
        bool isAutoCollectionRunning() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;

        // Thread safety
        mutable std::mutex mutex_;

        // Helper methods
        double calculateMean(const std::vector<double>& values) const;
        double calculateStdDev(const std::vector<double>& values, double mean) const;
        double calculatePercentile(const std::vector<double>& values, double percentile) const;
        std::string detectMemoryState(double memoryUsagePercent) const;
        std::string detectHealthStatus(const SystemHealth& health) const;
    };

} // namespace nlm
