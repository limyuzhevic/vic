// ==============================================================================
// Experiment Comparator
// ==============================================================================
#ifndef EXPERIMENTCOMPARATOR_HPP
#define EXPERIMENTCOMPARATOR_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <algorithm>
#include <numeric>
#include "../src/experiments/Experiment.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"

namespace nlm {
namespace debug {

/**
 * @class ExperimentComparator
 * @brief Experiment comparison and analysis tool for NLM
 * 
 * Provides tools for comparing experiments including experiment comparison,
 * experiment analysis, experiment optimization, and experiment benchmarking.
 */
class ExperimentComparator {
public:
    /**
     * @enum ComparisonType
     * @brief Type of comparison
     */
    enum class ComparisonType {
        OVERALL,
        PERFORMANCE,
        EFFICIENCY,
        STABILITY,
        LEARNING,
        CONSOLIDATION,
        NEURAL_DYNAMICS,
        BEHAVIORAL,
        ADAPTIVE,
        COMPARATIVE
    };

    /**
     * @enum BenchmarkType
     * @brief Type of benchmark
     */
    enum class BenchmarkType {
        CPU,
        MEMORY,
        NEURAL_ACTIVITY,
        SYNAPTIC_ACTIVITY,
        MEMORY_ACCESS,
        NEUROMODULATION_ACTIVITY,
        THROUGHPUT,
        LATENCY,
        EFFICIENCY,
        SCALABILITY,
        RELIABILITY,
        ROBUSTNESS
    };

    /**
     * @struct ExperimentComparison
     * @brief Experiment comparison
     */
    struct ExperimentComparison {
        std::string experiment1Name;
        std::string experiment2Name;
        ComparisonType comparisonType;
        double similarityScore;
        double differenceScore;
        std::vector<std::string> differences;
        std::vector<std::string> similarities;
        std::map<std::string, double> metricComparisons;
        std::chrono::steady_clock::time_point comparisonTime;
    };

    /**
     * @struct ExperimentBenchmark
     * @brief Experiment benchmark
     */
    struct ExperimentBenchmark {
        std::string benchmarkName;
        BenchmarkType benchmarkType;
        std::string experimentName;
        double benchmarkScore;
        std::map<std::string, double> metrics;
        std::chrono::steady_clock::time_point benchmarkTime;
    };

    ExperimentComparator();
    ~ExperimentComparator();

    /**
     * @brief Compare experiments
     * @param experiment1 First experiment
     * @param experiment2 Second experiment
     * @param comparisonType Comparison type
     * @return Experiment comparison
     */
    ExperimentComparison compareExperiments(const Experiment& experiment1, const Experiment& experiment2, ComparisonType comparisonType = ComparisonType::OVERALL);

    /**
     * @brief Compare experiments
     * @param experimentNames Vector of experiment names
     * @param comparisonType Comparison type
     * @return Experiment comparisons
     */
    std::vector<ExperimentComparison> compareExperiments(const std::vector<std::string>& experimentNames, ComparisonType comparisonType = ComparisonType::OVERALL);

    /**
     * @brief Get experiment performance
     * @param experimentName Experiment name
     * @return Performance
     */
    double getExperimentPerformance(const std::string& experimentName);

    /**
     * @brief Get experiment efficiency
     * @param experimentName Experiment name
     * @return Efficiency
     */
    double getExperimentEfficiency(const std::string& experimentName);

    /**
     * @brief Get experiment stability
     * @param experimentName Experiment name
     * @return Stability
     */
    double getExperimentStability(const std::string& experimentName);

    /**
     * @brief Get experiment learning
     * @param experimentName Experiment name
     * @return Learning
     */
    double getExperimentLearning(const std::string& experimentName);

    /**
     * @brief Get experiment consolidation
     * @param experimentName Experiment name
     * @return Consolidation
     */
    double getExperimentConsolidation(const std::string& experimentName);

    /**
     * @brief Get experiment neural dynamics
     * @param experimentName Experiment name
     * @return Neural dynamics
     */
    double getExperimentNeuralDynamics(const std::string& experimentName);

    /**
     * @brief Get experiment behavioral performance
     * @param experimentName Experiment name
     * @return Behavioral performance
     */
    double getExperimentBehavioralPerformance(const std::string& experimentName);

    /**
     * @brief Get experiment adaptability
     * @param experimentName Experiment name
     * @return Adaptability
     */
    double getExperimentAdaptability(const std::string& experimentName);

    /**
     * @brief Benchmark experiment
     * @param experimentName Experiment name
     * @param benchmarkType Benchmark type
     * @return Experiment benchmark
     */
    ExperimentBenchmark benchmarkExperiment(const std::string& experimentName, BenchmarkType benchmarkType = BenchmarkType::EFFICIENCY);

    /**
     * @brief Benchmark experiments
     * @param experimentNames Vector of experiment names
     * @param benchmarkType Benchmark type
     * @return Experiment benchmarks
     */
    std::vector<ExperimentBenchmark> benchmarkExperiments(const std::vector<std::string>& experimentNames, BenchmarkType benchmarkType = BenchmarkType::EFFICIENCY);

    /**
     * @brief Get experiment statistics
     * @param experimentName Experiment name
     * @return Experiment statistics
     */
    struct ExperimentStatistics {
        double averagePerformance;
        double standardDeviationPerformance;
        double averageEfficiency;
        double standardDeviationEfficiency;
        double averageStability;
        double standardDeviationStability;
        double averageLearning;
        double standardDeviationLearning;
        double averageConsolidation;
        double standardDeviationConsolidation;
        double averageNeuralDynamics;
        double standardDeviationNeuralDynamics;
        double averageBehavioralPerformance;
        double standardDeviationBehavioralPerformance;
        double averageAdaptability;
        double standardDeviationAdaptability;
        std::chrono::steady_clock::time_point experimentStartTime;
        std::chrono::steady_clock::time_point experimentEndTime;
        uint64_t totalNeurons;
        uint64_t totalSynapses;
        uint64_t totalMemoryAllocations;
    };

    ExperimentStatistics getExperimentStatistics(const std::string& experimentName);

    /**
     * @brief Get experiment comparison statistics
     * @param experimentNames Vector of experiment names
     * @return Experiment comparison statistics
     */
    std::map<std::string, ExperimentStatistics> getExperimentComparisonStatistics(const std::vector<std::string>& experimentNames);

    /**
     * @brief Generate experiment comparison report
     * @param experimentNames Vector of experiment names
     * @return Comparison report
     */
    struct ComparisonReport {
        std::chrono::steady_clock::time_point reportTime;
        size_t totalExperiments;
        std::vector<ExperimentComparison> comparisons;
        std::vector<ExperimentBenchmark> benchmarks;
        std::map<std::string, ExperimentStatistics> statistics;
    };

    ComparisonReport generateExperimentComparisonReport(const std::vector<std::string>& experimentNames);

    /**
     * @brief Generate experiment benchmark report
     * @param experimentNames Vector of experiment names
     * @return Benchmark report
     */
    struct BenchmarkReport {
        std::chrono::steady_clock::time_point reportTime;
        std::map<BenchmarkType, std::vector<ExperimentBenchmark>> benchmarks;
        std::map<std::string, double> averageScores;
        std::string bestExperiment;
        std::string worstExperiment;
    };

    BenchmarkReport generateExperimentBenchmarkReport(const std::vector<std::string>& experimentNames);

    /**
     * @brief Export experiment comparison
     * @param experimentNames Vector of experiment names
     * @param filename Output file path
     */
    void exportExperimentComparison(const std::vector<std::string>& experimentNames, const std::string& filename);

    /**
     * @brief Import experiment comparison
     * @param filename Input file path
     */
    void importExperimentComparison(const std::string& filename);

    /**
     * @brief Clear experiment comparison
     */
    void clearExperimentComparison();

    /**
     * @brief Generate experiment comparison visualization
     * @param experimentNames Vector of experiment names
     * @param filename Output file path
     */
    void generateExperimentComparisonVisualization(const std::vector<std::string>& experimentNames, const std::string& filename);

    /**
     * @brief Generate experiment benchmark visualization
     * @param experimentNames Vector of experiment names
     * @param filename Output file path
     */
    void generateExperimentBenchmarkVisualization(const std::vector<std::string>& experimentNames, const std::string& filename);

    /**
     * @brief Get available comparison types
     * @return Vector of comparison types
     */
    std::vector<ComparisonType> getAvailableComparisonTypes() const;

    /**
     * @brief Get available benchmark types
     * @return Vector of benchmark types
     */
    std::vector<BenchmarkType> getAvailableBenchmarkTypes() const;

    /**
     * @brief Get experiment names
     * @return Vector of experiment names
     */
    std::vector<std::string> getExperimentNames() const;

    /**
     * @brief Is experiment available?
     * @param experimentName Experiment name
     * @return true if experiment is available
     */
    bool isExperimentAvailable(const std::string& experimentName) const;

    /**
     * @brief Get comparison score
     * @param comparison Experiment comparison
     * @return Comparison score
     */
    double getComparisonScore(const ExperimentComparison& comparison);

    /**
     * @brief Get benchmark score
     * @param benchmark Experiment benchmark
     * @return Benchmark score
     */
    double getBenchmarkScore(const ExperimentBenchmark& benchmark);

protected:
    /**
     * @brief Calculate comparison score
     * @param comparison Experiment comparison
     * @return Comparison score
     */
    double calculateComparisonScore(const ExperimentComparison& comparison);

    /**
     * @brief Calculate benchmark score
     * @param benchmark Experiment benchmark
     * @return Benchmark score
     */
    double calculateBenchmarkScore(const ExperimentBenchmark& benchmark);

    /**
     * @brief Calculate experiment performance
     * @param experimentName Experiment name
     * @return Performance
     */
    double calculateExperimentPerformance(const std::string& experimentName);

    /**
     * @brief Calculate experiment efficiency
     * @param experimentName Experiment name
     * @return Efficiency
     */
    double calculateExperimentEfficiency(const std::string& experimentName);

    /**
     * @brief Calculate experiment stability
     * @param experimentName Experiment name
     * @return Stability
     */
    double calculateExperimentStability(const std::string& experimentName);

    /**
     * @brief Calculate experiment learning
     * @param experimentName Experiment name
     * @return Learning
     */
    double calculateExperimentLearning(const std::string& experimentName);

    /**
     * @brief Calculate experiment consolidation
     * @param experimentName Experiment name
     * @return Consolidation
     */
    double calculateExperimentConsolidation(const std::string& experimentName);

    /**
     * @brief Calculate experiment neural dynamics
     * @param experimentName Experiment name
     * @return Neural dynamics
     */
    double calculateExperimentNeuralDynamics(const std::string& experimentName);

    /**
     * @brief Calculate experiment behavioral performance
     * @param experimentName Experiment name
     * @return Behavioral performance
     */
    double calculateExperimentBehavioralPerformance(const std::string& experimentName);

    /**
     * @brief Calculate experiment adaptability
     * @param experimentName Experiment name
     * @return Adaptability
     */
    double calculateExperimentAdaptability(const std::string& experimentName);

    /**
     * @brief Create visualization
     * @param experimentNames Vector of experiment names
     * @param filename Output file path
     */
    void createVisualization(const std::vector<std::string>& experimentNames, const std::string& filename);

    /**
     * @brief Create comparison visualization
     * @param experimentNames Vector of experiment names
     * @param filename Output file path
     */
    void createComparisonVisualization(const std::vector<std::string>& experimentNames, const std::string& filename);

    /**
     * @brief Create benchmark visualization
     * @param experimentNames Vector of experiment names
     * @param filename Output file path
     */
    void createBenchmarkVisualization(const std::vector<std::string>& experimentNames, const std::string& filename);

    /**
     * @brief Generate Python code
     * @param experimentNames Vector of experiment names
     * @return Python code
     */
    std::string generatePythonCode(const std::vector<std::string>& experimentNames);

    /**
     * @brief Execute Python code
     * @param pythonCode Python code
     * @return Python execution result
     */
    std::string executePythonCode(const std::string& pythonCode);

    /**
     * @brief Parse comparison result
     * @param result Experiment comparison
     * @return Parsed result
     */
    std::map<std::string, double> parseComparisonResult(const ExperimentComparison& result);

    /**
     * @brief Parse benchmark result
     * @param result Experiment benchmark
     * @return Parsed result
     */
    std::map<std::string, double> parseBenchmarkResult(const ExperimentBenchmark& result);

    /**
     * @brief Generate error report
     * @param experimentNames Vector of experiment names
     * @return Error report
     */
    std::vector<std::string> generateErrorReport(const std::vector<std::string>& experimentNames) const;

    /**
     * @brief Generate warning report
     * @param experimentNames Vector of experiment names
     * @return Warning report
     */
    std::vector<std::string> generateWarningReport(const std::vector<std::string>& experimentNames) const;

    /**
     * @brief Generate diagnostic report
     * @param experimentNames Vector of experiment names
     * @return Diagnostic report
     */
    std::vector<std::string> generateDiagnosticReport(const std::vector<std::string>& experimentNames) const;

    /**
     * @brief Generate insights
     * @param comparison Comparison result
     * @return Insights
     */
    std::vector<std::string> generateInsights(const ExperimentComparison& comparison);

    /**
     * @brief Calculate similarity score
     * @param comparison Experiment comparison
     * @return Similarity score
     */
    double calculateSimilarityScore(const ExperimentComparison& comparison);

    /**
     * @brief Calculate difference score
     * @param comparison Experiment comparison
     * @return Difference score
     */
    double calculateDifferenceScore(const ExperimentComparison& comparison);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // EXPERIMENTCOMPARATOR_HPP
