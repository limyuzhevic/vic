// ==============================================================================
// Debug Performance Interface
// ==============================================================================
#ifndef DEBUGPERFORMANCEINTERFACE_HPP
#define DEBUGPERFORMANCEINTERFACE_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "../performance/PerformanceProfiler.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../config/OptimizedConfig.hpp"

namespace nlm {
namespace debug {

/**
 * @class DebugPerformanceInterface
 * @brief Performance debugging and analysis interface for NLM
 * 
 * Provides tools for performance debugging and analysis including performance profiling,
 * performance bottleneck detection, performance optimization recommendations, and performance trend analysis.
 */
class DebugPerformanceInterface {
public:
    /**
     * @enum PerformanceAnalysisType
     * @brief Type of performance analysis
     */
    enum class PerformanceAnalysisType {
        CPU_ANALYSIS,
        MEMORY_ANALYSIS,
        NEURAL_ACTIVITY_ANALYSIS,
        SYNAPTIC_ACTIVITY_ANALYSIS,
        MEMORY_ACCESS_ANALYSIS,
        NEUROMODULATION_ANALYSIS,
        THROUGHPUT_ANALYSIS,
        LATENCY_ANALYSIS,
        EFFICIENCY_ANALYSIS,
        SCALABILITY_ANALYSIS
    };

    /**
     * @struct PerformanceAnalysisResult
     * @brief Performance analysis result
     */
    struct PerformanceAnalysisResult {
        PerformanceAnalysisType analysisType;
        std::map<std::string, double> metrics;
        std::vector<std::string> insights;
        std::string optimizationRecommendation;
        double performanceScore;
        std::chrono::steady_clock::time_point analysisTime;
    };

    DebugPerformanceInterface();
    ~DebugPerformanceInterface();

    /**
     * @brief Start performance debugging interface
     * @param profiler Performance profiler reference
     */
    void startPerformanceInterface(PerformanceProfiler& profiler);

    /**
     * @brief Stop performance debugging interface
     */
    void stopPerformanceInterface();

    /**
     * @brief Analyze performance
     * @param analysisType Analysis type
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     * @return Performance analysis result
     */
    PerformanceAnalysisResult analyzePerformance(PerformanceAnalysisType analysisType, const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Get performance analysis results
     * @param analysisType Analysis type
     * @return Vector of analysis results
     */
    std::vector<PerformanceAnalysisResult> getPerformanceAnalysisResults(PerformanceAnalysisType analysisType) const;

    /**
     * @brief Get available performance analysis types
     * @return Vector of analysis types
     */
    std::vector<PerformanceAnalysisType> getAvailablePerformanceAnalysisTypes() const;

    /**
     * @brief Identify performance bottlenecks
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @return Vector of performance bottlenecks
     */
    std::vector<std::string> identifyPerformanceBottlenecks(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator);

    /**
     * @brief Get optimization recommendations
     * @param analysisType Analysis type
     * @return Optimization recommendations
     */
    std::vector<std::string> getOptimizationRecommendations(PerformanceAnalysisType analysisType);

    /**
     * @brief Get performance statistics
     * @param analysisType Analysis type
     * @return Performance statistics
     */
    struct PerformanceStatistics {
        double averageCPUUsage;
        double averageMemoryUsage;
        double averageNeuralActivity;
        double averageSynapticActivity;
        double averageMemoryAccess;
        double averageNeuromodulationActivity;
        double averageThroughput;
        double averageLatency;
        double averageEfficiency;
        double performanceTrend;
        double stabilityScore;
        uint64_t samplesCollected;
    };

    PerformanceStatistics getPerformanceStatistics(PerformanceAnalysisType analysisType) const;

    /**
     * @brief Get performance trends
     * @param analysisType Analysis type
     * @param timeWindow Time window in seconds
     * @return Performance trends
     */
    std::vector<PerformanceStatistics> getPerformanceTrends(PerformanceAnalysisType analysisType, double timeWindow = 60.0) const;

    /**
     * @brief Generate performance report
     * @param analysisType Analysis type
     * @return Performance report
     */
    struct PerformanceReport {
        std::chrono::steady_clock::time_point reportTime;
        PerformanceAnalysisType analysisType;
        std::vector<PerformanceAnalysisResult> analysisResults;
        std::vector<std::string> optimizationRecommendations;
        double overallScore;
    };

    PerformanceReport generatePerformanceReport(PerformanceAnalysisType analysisType);

    /**
     * @brief Export performance analysis
     * @param result Analysis result
     * @param filename Output file path
     */
    void exportPerformanceAnalysis(const PerformanceAnalysisResult& result, const std::string& filename);

    /**
     * @brief Import performance analysis
     * @param filename Input file path
     * @return Imported analysis result
     */
    PerformanceAnalysisResult importPerformanceAnalysis(const std::string& filename);

    /**
     * @brief Clear performance analysis
     */
    void clearPerformanceAnalysis();

    /**
     * @brief Generate performance debugging visualization
     * @param analysisType Analysis type
     * @param filename Output file path
     */
    void generatePerformanceDebuggingVisualization(PerformanceAnalysisType analysisType, const std::string& filename);

    /**
     * @brief Generate performance analysis visualization
     * @param result Analysis result
     * @param filename Output file path
     */
    void generatePerformanceAnalysisVisualization(const PerformanceAnalysisResult& result, const std::string& filename);

    /**
     * @brief Get performance metrics
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @return Performance metrics
     */
    std::map<std::string, double> getPerformanceMetrics(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator);

    /**
     * @brief Get resource usage
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @return Resource usage
     */
    struct ResourceUsage {
        double cpuUsage;
        double memoryUsage;
        uint64_t threadCount;
        uint64_t fileHandleCount;
        double networkUsage;
        double diskUsage;
    };

    ResourceUsage getResourceUsage(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator);

    /**
     * @brief Get efficiency score
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @return Efficiency score
     */
    double getEfficiencyScore(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator);

    /**
     * @brief Get stability score
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @return Stability score
     */
    double getStabilityScore(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator);

    /**
     * @brief Get scalability score
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @return Scalability score
     */
    double getScalabilityScore(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator);

    /**
     * @brief Start real-time performance monitoring
     */
    void startRealTimePerformanceMonitoring();

    /**
     * @brief Stop real-time performance monitoring
     */
    void stopRealTimePerformanceMonitoring();

    /**
     * @brief Is real-time performance monitoring active?
     * @return true if real-time performance monitoring is active
     */
    bool isRealTimePerformanceMonitoringActive() const;

    /**
     * @brief Get performance monitoring statistics
     * @return Monitoring statistics
     */
    struct MonitoringStatistics {
        uint64_t samplesCollected;
        std::chrono::steady_clock::time_point lastSample;
        double averageCPUUsage;
        double averageMemoryUsage;
    };

    MonitoringStatistics getMonitoringStatistics() const;

protected:
    /**
     * @brief Update performance analysis
     */
    void updatePerformanceAnalysis();

    /**
     * @brief Analyze CPU performance
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     */
    void analyzeCPUPerformance(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Analyze memory performance
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     */
    void analyzeMemoryPerformance(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Analyze neural activity performance
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     */
    void analyzeNeuralActivityPerformance(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Analyze synaptic activity performance
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     */
    void analyzeSynapticActivityPerformance(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Analyze memory access performance
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     */
    void analyzeMemoryAccessPerformance(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Analyze neuromodulation performance
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     */
    void analyzeNeuromodulationPerformance(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Analyze throughput performance
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     */
    void analyzeThroughputPerformance(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Analyze latency performance
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     */
    void analyzeLatencyPerformance(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Generate performance report
     */
    void generatePerformanceReportInternal();

    /**
     * @brief Create visualization
     * @param analysisType Analysis type
     * @param filename Output file path
     */
    void createVisualization(PerformanceAnalysisType analysisType, const std::string& filename);

    /**
     * @brief Create analysis visualization
     * @param result Analysis result
     * @param filename Output file path
     */
    void createAnalysisVisualization(const PerformanceAnalysisResult& result, const std::string& filename);

    /**
     * @brief Generate Python code
     * @param analysisType Analysis type
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     * @return Python code
     */
    std::string generatePythonCode(PerformanceAnalysisType analysisType, const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Execute Python code
     * @param pythonCode Python code
     * @return Python execution result
     */
    std::string executePythonCode(const std::string& pythonCode);

    /**
     * @brief Parse analysis result
     * @param result Analysis result
     * @return Parsed result
     */
    std::map<std::string, double> parseAnalysisResult(const PerformanceAnalysisResult& result);

    /**
     * @brief Generate error report
     * @param analysisType Analysis type
     * @return Error report
     */
    std::vector<std::string> generateErrorReport(PerformanceAnalysisType analysisType) const;

    /**
     * @brief Generate warning report
     * @param analysisType Analysis type
     * @return Warning report
     */
    std::vector<std::string> generateWarningReport(PerformanceAnalysisType analysisType) const;

    /**
     * @brief Generate diagnostic report
     * @param analysisType Analysis type
     * @return Diagnostic report
     */
    std::vector<std::string> generateDiagnosticReport(PerformanceAnalysisType analysisType) const;

    /**
     * @brief Generate optimization report
     * @param analysisType Analysis type
     * @return Optimization report
     */
    std::vector<std::string> generateOptimizationReport(PerformanceAnalysisType analysisType) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // DEBUGPERFORMANCEINTERFACE_HPP
