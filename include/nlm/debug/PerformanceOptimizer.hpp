// ==============================================================================
// Performance Optimizer
// ==============================================================================
#ifndef PERFORMANCEOPTIMIZER_HPP
#define PERFORMANCEOPTIMIZER_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <unordered_set>
#include "../performance/PerformanceProfiler.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../config/OptimizedConfig.hpp"

namespace nlm {
namespace debug {

/**
 * @class PerformanceOptimizer
 * @brief Performance optimization recommendations for NLM
 * 
 * Provides performance optimization recommendations based on performance analysis
 * and machine learning including neural network analysis and experiment comparison.
 */
class PerformanceOptimizer {
public:
    /**
     * @enum OptimizationStrategy
     * @brief Optimization strategy
     */
    enum class OptimizationStrategy {
        BASIC,
        INTERMEDIATE,
        ADVANCED,
        AGGRESSIVE,
        MACHINE_LEARNING_BASED,
        NEURAL_NETWORK_BASED,
        EXPERIMENT_BASED
    };

    /**
     * @enum OptimizationTarget
     * @brief Optimization target
     */
    enum class OptimizationTarget {
        CPU_USAGE,
        MEMORY_USAGE,
        NEURAL_ACTIVITY,
        SYNAPTIC_ACTIVITY,
        MEMORY_ACCESS,
        NEUROMODULATION_ACTIVITY,
        THROUGHPUT,
        LATENCY,
        EFFICIENCY,
        SCALABILITY
    };

    /**
     * @struct OptimizationResult
     * @brief Optimization result
     */
    struct OptimizationResult {
        std::string optimizationType;
        double improvementScore;
        double cost;
        double roi;
        std::vector<std::string> changes;
        std::chrono::steady_clock::time_point optimizationTime;
        bool isImplemented;
    };

    /**
     * @struct NeuralNetworkAnalysis
     * @brief Neural network analysis
     */
    struct NeuralNetworkAnalysis {
        double networkEfficiency;
        double synapticStrength;
        double connectionDensity;
        double activityPattern;
        double plasticityRate;
        double memoryAccessPattern;
        double neuromodulationImpact;
        double overallPerformance;
        std::map<std::string, double> layerAnalysis;
    };

    PerformanceOptimizer();
    ~PerformanceOptimizer();

    /**
     * @brief Optimize performance
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     * @param strategy Optimization strategy
     */
    void optimizePerformance(Brain& brain, NeuralWorkingMemory& memory, Neuromodulator& neuromodulator, Config& config, OptimizationStrategy strategy = OptimizationStrategy::INTERMEDIATE);

    /**
     * @brief Analyze neural network
     * @param brain Neural network reference
     * @return Neural network analysis
     */
    NeuralNetworkAnalysis analyzeNeuralNetwork(const Brain& brain);

    /**
     * @brief Optimize neural network
     * @param brain Neural network reference
     * @param strategy Optimization strategy
     */
    void optimizeNeuralNetwork(Brain& brain, OptimizationStrategy strategy = OptimizationStrategy::INTERMEDIATE);

    /**
     * @brief Optimize memory system
     * @param memory Memory system reference
     * @param strategy Optimization strategy
     */
    void optimizeMemorySystem(NeuralWorkingMemory& memory, OptimizationStrategy strategy = OptimizationStrategy::INTERMEDIATE);

    /**
     * @brief Optimize neuromodulation system
     * @param neuromodulator Neuromodulation system reference
     * @param strategy Optimization strategy
     */
    void optimizeNeuromodulationSystem(Neuromodulator& neuromodulator, OptimizationStrategy strategy = OptimizationStrategy::INTERMEDIATE);

    /**
     * @brief Optimize configuration
     * @param config Configuration reference
     * @param strategy Optimization strategy
     */
    void optimizeConfiguration(Config& config, OptimizationStrategy strategy = OptimizationStrategy::INTERMEDIATE);

    /**
     * @brief Get optimization results
     * @return Vector of optimization results
     */
    std::vector<OptimizationResult> getOptimizationResults() const;

    /**
     * @brief Get optimization recommendations
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     * @param target Optimization target
     * @return Vector of optimization recommendations
     */
    std::vector<std::string> getOptimizationRecommendations(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config, OptimizationTarget target);

    /**
     * @brief Get optimization score
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     * @return Optimization score
     */
    double getOptimizationScore(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Get neural network efficiency
     * @param brain Neural network reference
     * @return Neural network efficiency
     */
    double getNeuralNetworkEfficiency(const Brain& brain);

    /**
     * @brief Get memory efficiency
     * @param memory Memory system reference
     * @return Memory efficiency
     */
    double getMemoryEfficiency(const NeuralWorkingMemory& memory);

    /**
     * @brief Get neuromodulation efficiency
     * @param neuromodulator Neuromodulation system reference
     * @return Neuromodulation efficiency
     */
    double getNeuromodulationEfficiency(const Neuromodulator& neuromodulator);

    /**
     * @brief Get configuration efficiency
     * @param config Configuration reference
     * @return Configuration efficiency
     */
    double getConfigurationEfficiency(const Config& config);

    /**
     * @brief Generate optimization report
     * @return Optimization report
     */
    struct OptimizationReport {
        std::chrono::steady_clock::time_point reportTime;
        size_t totalOptimizations;
        double totalImprovement;
        double totalCost;
        double totalROI;
        std::vector<OptimizationResult> optimizations;
        NeuralNetworkAnalysis neuralNetworkAnalysis;
    };

    OptimizationReport generateOptimizationReport();

    /**
     * @brief Export optimization results
     * @param filename Output file path
     */
    void exportOptimizationResults(const std::string& filename);

    /**
     * @brief Import optimization results
     * @param filename Input file path
     */
    void importOptimizationResults(const std::string& filename);

    /**
     * @brief Clear optimization results
     */
    void clearOptimizationResults();

    /**
     * @brief Generate performance optimization visualization
     * @param filename Output file path
     */
    void generatePerformanceOptimizationVisualization(const std::string& filename);

    /**
     * @brief Generate neural network optimization visualization
     * @param filename Output file path
     */
    void generateNeuralNetworkOptimizationVisualization(const std::string& filename);

    /**
     * @brief Generate memory optimization visualization
     * @param filename Output file path
     */
    void generateMemoryOptimizationVisualization(const std::string& filename);

    /**
     * @brief Generate neuromodulation optimization visualization
     * @param filename Output file path
     */
    void generateNeuromodulationOptimizationVisualization(const std::string& filename);

    /**
     * @brief Generate configuration optimization visualization
     * @param filename Output file path
     */
    void generateConfigurationOptimizationVisualization(const std::string& filename);

    /**
     * @brief Get optimization strategies
     * @return Vector of optimization strategies
     */
    std::vector<OptimizationStrategy> getAvailableOptimizationStrategies() const;

    /**
     * @brief Get optimization targets
     * @return Vector of optimization targets
     */
    std::vector<OptimizationTarget> getAvailableOptimizationTargets() const;

    /**
     * @brief Set optimization strategy
     * @param strategy Optimization strategy
     */
    void setOptimizationStrategy(OptimizationStrategy strategy);

    /**
     * @brief Get optimization strategy
     * @return Optimization strategy
     */
    OptimizationStrategy getOptimizationStrategy() const;

    /**
     * @brief Enable machine learning-based optimization
     */
    void enableMachineLearningOptimization();

    /**
     * @brief Disable machine learning-based optimization
     */
    void disableMachineLearningOptimization();

    /**
     * @brief Is machine learning optimization enabled?
     * @return true if machine learning optimization is enabled
     */
    bool isMachineLearningOptimizationEnabled() const;

    /**
     * @brief Train optimization model
     * @param trainingData Training data
     */
    void trainOptimizationModel(const std::vector<std::map<std::string, double>>& trainingData);

    /**
     * @brief Optimize based on model
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     */
    void optimizeBasedOnModel(Brain& brain, NeuralWorkingMemory& memory, Neuromodulator& neuromodulator, Config& config);

protected:
    /**
     * @brief Run optimization
     */
    void runOptimization();

    /**
     * @brief Optimize neural network performance
     */
    void optimizeNeuralNetworkPerformance();

    /**
     * @brief Optimize memory performance
     */
    void optimizeMemoryPerformance();

    /**
     * @brief Optimize neuromodulation performance
     */
    void optimizeNeuromodulationPerformance();

    /**
     * @brief Optimize configuration performance
     */
    void optimizeConfigurationPerformance();

    /**
     * @brief Generate optimization report
     */
    void generateOptimizationReportInternal();

    /**
     * @brief Create visualization
     * @param filename Output file path
     */
    void createVisualization(const std::string& filename);

    /**
     * @brief Create neural network optimization visualization
     * @param filename Output file path
     */
    void createNeuralNetworkOptimizationVisualization(const std::string& filename);

    /**
     * @brief Create memory optimization visualization
     * @param filename Output file path
     */
    void createMemoryOptimizationVisualization(const std::string& filename);

    /**
     * @brief Create neuromodulation optimization visualization
     * @param filename Output file path
     */
    void createNeuromodulationOptimizationVisualization(const std::string& filename);

    /**
     * @brief Create configuration optimization visualization
     * @param filename Output file path
     */
    void createConfigurationOptimizationVisualization(const std::string& filename);

    /**
     * @brief Calculate neural network efficiency
     * @param brain Neural network reference
     * @return Neural network efficiency
     */
    double calculateNeuralNetworkEfficiency(const Brain& brain) const;

    /**
     * @brief Calculate memory efficiency
     * @param memory Memory system reference
     * @return Memory efficiency
     */
    double calculateMemoryEfficiency(const NeuralWorkingMemory& memory) const;

    /**
     * @brief Calculate neuromodulation efficiency
     * @param neuromodulator Neuromodulation system reference
     * @return Neuromodulation efficiency
     */
    double calculateNeuromodulationEfficiency(const Neuromodulator& neuromodulator) const;

    /**
     * @brief Calculate configuration efficiency
     * @param config Configuration reference
     * @return Configuration efficiency
     */
    double calculateConfigurationEfficiency(const Config& config) const;

    /**
     * @brief Generate Python code
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     * @return Python code
     */
    std::string generatePythonCode(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Execute Python code
     * @param pythonCode Python code
     * @return Python execution result
     */
    std::string executePythonCode(const std::string& pythonCode);

    /**
     * @brief Parse optimization result
     * @param result Optimization result
     * @return Parsed result
     */
    std::map<std::string, double> parseOptimizationResult(const OptimizationResult& result);

    /**
     * @brief Generate optimization recommendation
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     * @param target Optimization target
     * @return Optimization recommendation
     */
    std::string generateOptimizationRecommendation(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config, OptimizationTarget target);

    /**
     * @brief Get optimization priority
     * @param target Optimization target
     * @return Optimization priority
     */
    double getOptimizationPriority(OptimizationTarget target) const;

    /**
     * @brief Get optimization impact
     * @param target Optimization target
     * @return Optimization impact
     */
    double getOptimizationImpact(OptimizationTarget target) const;

    /**
     * @brief Get optimization cost
     * @param target Optimization target
     * @return Optimization cost
     */
    double getOptimizationCost(OptimizationTarget target) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // PERFORMANCEOPTIMIZER_HPP
