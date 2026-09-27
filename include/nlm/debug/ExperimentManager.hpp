// ==============================================================================
// Experiment Manager
// ==============================================================================
#ifndef EXPERIMENTMANAGER_HPP
#define EXPERIMENTMANAGER_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include "../src/experiments/Experiment.hpp"
#include "../performance/PerformanceProfiler.hpp"

namespace nlm {
namespace debug {

/**
 * @class ExperimentManager
 * @brief Experiment management tool for NLM
 * 
 * Provides tools for managing neural network experiments including creation,
 * execution, monitoring, and analysis.
 */
class ExperimentManager {
public:
    /**
     * @enum ExperimentStatus
     * @brief Status of an experiment
     */
    enum class ExperimentStatus {
        PENDING,
        RUNNING,
        COMPLETED,
        FAILED,
        PAUSED,
        CANCELLED
    };

    /**
     * @struct ExperimentInfo
     * @brief Information about an experiment
     */
    struct ExperimentInfo {
        std::string name;
        std::string description;
        ExperimentStatus status;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        uint64_t duration;
        double performanceScore;
        std::vector<std::string> tags;
        std::map<std::string, std::string> metadata;
    };

    ExperimentManager();
    ~ExperimentManager();

    /**
     * @brief Create a new experiment
     * @param name Experiment name
     * @param description Experiment description
     * @return true if experiment was created
     */
    bool createExperiment(const std::string& name, const std::string& description);

    /**
     * @brief Get experiment by name
     * @param name Experiment name
     * @return Experiment pointer (nullptr if not found)
     */
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;

    /**
     * @brief Delete experiment
     * @param name Experiment name
     * @return true if experiment was deleted
     */
    bool deleteExperiment(const std::string& name);

    /**
     * @brief Start experiment execution
     * @param name Experiment name
     * @return true if experiment was started
     */
    bool startExperiment(const std::string& name);

    /**
     * @brief Pause experiment execution
     * @param name Experiment name
     * @return true if experiment was paused
     */
    bool pauseExperiment(const std::string& name);

    /**
     * @brief Resume experiment execution
     * @param name Experiment name
     * @return true if experiment was resumed
     */
    bool resumeExperiment(const std::string& name);

    /**
     * @brief Stop experiment execution
     * @param name Experiment name
     * @return true if experiment was stopped
     */
    bool stopExperiment(const std::string& name);

    /**
     * @brief Get experiment status
     * @param name Experiment name
     * @return Experiment status
     */
    ExperimentStatus getExperimentStatus(const std::string& name) const;

    /**
     * @brief Get experiment information
     * @param name Experiment name
     * @return Experiment information
     */
    ExperimentInfo getExperimentInfo(const std::string& name) const;

    /**
     * @brief Get all experiment names
     * @return Vector of experiment names
     */
    std::vector<std::string> getExperimentNames() const;

    /**
     * @brief Get experiment results
     * @param name Experiment name
     * @return Experiment results
     */
    std::map<std::string, double> getExperimentResults(const std::string& name) const;

    /**
     * @brief Compare experiment results
     * @param experimentNames Vector of experiment names
     * @return Comparison results
     */
    std::map<std::string, std::map<std::string, double>> compareExperiments(const std::vector<std::string>& experimentNames) const;

    /**
     * @brief Export experiment results
     * @param name Experiment name
     * @param filename Output file path
     * @return true if export was successful
     */
    bool exportExperimentResults(const std::string& name, const std::string& filename);

    /**
     * @brief Import experiment results
     * @param filename Input file path
     * @return true if import was successful
     */
    bool importExperimentResults(const std::string& filename);

    /**
     * @brief Get experiment statistics
     * @param name Experiment name
     * @return Experiment statistics
     */
    struct ExperimentStats {
        uint64_t totalNeurons;
        uint64_t totalSynapses;
        double averageFiringRate;
        double averageConnectionStrength;
        double memoryUsage;
        double executionTime;
    };

    ExperimentStats getExperimentStats(const std::string& name) const;

    /**
     * @brief Get all experiment statistics
     * @return Map of experiment names to statistics
     */
    std::map<std::string, ExperimentStats> getAllExperimentStats() const;

protected:
    /**
     * @brief Update experiment status
     * @param name Experiment name
     * @param status New status
     */
    void updateExperimentStatus(const std::string& name, ExperimentStatus status);

    /**
     * @brief Calculate experiment performance score
     * @param experiment Experiment
     * @return Performance score
     */
    double calculatePerformanceScore(const Experiment& experiment) const;

    /**
     * @brief Format experiment results for display
     * @param name Experiment name
     * @return Formatted string representation
     */
    std::string formatExperimentResults(const std::string& name) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // EXPERIMENTMANAGER_HPP
