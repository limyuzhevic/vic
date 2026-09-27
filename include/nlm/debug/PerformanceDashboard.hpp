// ==============================================================================
// Performance Dashboard
// ==============================================================================
#ifndef PERFORMANCEDASHBOARD_HPP
#define PERFORMANCEDASHBOARD_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "../performance/PerformanceProfiler.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"

namespace nlm {
namespace debug {

/**
 * @class PerformanceDashboard
 * @brief Performance monitoring dashboard for NLM
 * 
 * Provides real-time performance monitoring and visualization for neural network systems
 * including performance metrics, resource usage, and trend analysis.
 */
class PerformanceDashboard {
public:
    /**
     * @enum MetricType
     * @brief Type of performance metric
     */
    enum class MetricType {
        CPU_USAGE,
        MEMORY_USAGE,
        NETWORK_TRAFFIC,
        NEURAL_ACTIVITY,
        SYNAPTIC_ACTIVITY,
        MEMORY_ACTIVITY,
        NEUROMODULATION_ACTIVITY,
        PROCESSING_TIME,
        THROUGHPUT,
        LATENCY
    };

    /**
     * @struct MetricData
     * @brief Data for a performance metric
     */
    struct MetricData {
        MetricType type;
        std::string name;
        std::vector<double> values;
        std::vector<std::chrono::steady_clock::time_point> timestamps;
        double currentValue;
        double averageValue;
        double minValue;
        double maxValue;
        double trend;
        std::string unit;
    };

    PerformanceDashboard();
    ~PerformanceDashboard();

    /**
     * @brief Start performance monitoring
     * @param profiler Performance profiler reference
     */
    void startMonitoring(PerformanceProfiler& profiler);

    /**
     * @brief Stop performance monitoring
     */
    void stopMonitoring();

    /**
     * @brief Update dashboard data
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     */
    void update(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator);

    /**
     * @brief Get metric data
     * @param type Metric type
     * @return Metric data
     */
    const MetricData* getMetricData(MetricType type) const;

    /**
     * @brief Get all metric data
     * @return Map of metric types to data
     */
    std::map<MetricType, MetricData> getAllMetricData() const;

    /**
     * @brief Add custom metric
     * @param name Metric name
     * @param type Metric type
     * @param unit Unit of measurement
     */
    void addCustomMetric(const std::string& name, MetricType type, const std::string& unit);

    /**
     * @brief Get dashboard statistics
     * @return Dashboard statistics
     */
    struct DashboardStats {
        uint64_t totalMetrics;
        uint64_t activeMetrics;
        std::chrono::steady_clock::time_point lastUpdate;
        double systemLoad;
        double memoryUsage;
        uint64_t dataPointsCollected;
    };

    DashboardStats getDashboardStats() const;

    /**
     * @brief Generate dashboard report
     * @param filename Output file path
     * @return true if report was generated
     */
    bool generateReport(const std::string& filename);

    /**
     * @brief Get performance alerts
     * @return Vector of performance alerts
     */
    std::vector<std::string> getPerformanceAlerts() const;

    /**
     * @brief Clear metric data
     */
    void clearMetricData();

    /**
     * @brief Get metric history
     * @param type Metric type
     * @param count Number of historical data points
     * @return Vector of metric values
     */
    std::vector<double> getMetricHistory(MetricType type, size_t count = 100) const;

    /**
     * @brief Set update interval
     * @param interval Update interval in milliseconds
     */
    void setUpdateInterval(int interval);

    /**
     * @brief Get current update interval
     * @return Update interval in milliseconds
     */
    int getUpdateInterval() const;

protected:
    /**
     * @brief Update CPU metrics
     */
    void updateCPUMetrics();

    /**
     * @brief Update memory metrics
     */
    void updateMemoryMetrics();

    /**
     * @brief Update neural activity metrics
     * @param brain Neural network reference
     */
    void updateNeuralActivityMetrics(const Brain& brain);

    /**
     * @brief Update synaptic activity metrics
     * @param brain Neural network reference
     */
    void updateSynapticActivityMetrics(const Brain& brain);

    /**
     * @brief Update memory activity metrics
     * @param memory Memory system reference
     */
    void updateMemoryActivityMetrics(const NeuralWorkingMemory& memory);

    /**
     * @brief Update neuromodulation activity metrics
     * @param neuromodulator Neuromodulation system reference
     */
    void updateNeuromodulationActivityMetrics(const Neuromodulator& neuromodulator);

    /**
     * @brief Calculate metric statistics
     * @param metric Metric data
     * @return Statistics
     */
    struct Statistics {
        double mean;
        double median;
        double standardDeviation;
        double variance;
        double min;
        double max;
        double range;
        double percentiles[4]; // 25%, 50%, 75%, 95%
    };

    static Statistics calculateStatistics(const std::vector<double>& data);

    /**
     * @brief Format metric for display
     * @param metric Metric data
     * @return Formatted string representation
     */
    std::string formatMetric(const MetricData& metric) const;

    /**
     * @brief Check for performance alerts
     * @return Vector of alerts
     */
    std::vector<std::string> checkPerformanceAlerts() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // PERFORMANCEDASHBOARD_HPP
