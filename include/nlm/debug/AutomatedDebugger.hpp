// ==============================================================================
// Automated Debugger
// ==============================================================================
#ifndef AUTOMATEDDEBUGGER_HPP
#define AUTOMATEDDEBUGGER_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include "../src/brain/Brain.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../config/OptimizedConfig.hpp"
#include "../performance/PerformanceProfiler.hpp"

namespace nlm {
namespace debug {

/**
 * @class AutomatedDebugger
 * @brief Automated debugging system for NLM
 * 
 * Provides automated debugging capabilities with error detection, recovery, and
 * optimization recommendations including machine learning-based anomaly detection
 * and automated error recovery.
 */
class AutomatedDebugger {
public:
    /**
     * @enum DebugLevel
     * @brief Debug level
     */
    enum class DebugLevel {
        NONE,
        BASIC,
        INTERMEDIATE,
        ADVANCED,
        COMPREHENSIVE
    };

    /**
     * @enum RecoveryAction
     * @brief Recovery action
     */
    enum class RecoveryAction {
        NONE,
        RESTART,
        ROLLBACK,
        OPTIMIZE,
        RECONFIGURE,
        REPLACE_MODULE,
        ADJUST_PARAMETERS
    };

    /**
     * @struct AnomalyDetectionResult
     * @brief Anomaly detection result
     */
    struct AnomalyDetectionResult {
        bool isAnomaly;
        double anomalyScore;
        std::string anomalyType;
        std::vector<std::string> symptoms;
        std::chrono::steady_clock::time_point detectionTime;
    };

    /**
     * @struct ErrorRecoveryResult
     * @brief Error recovery result
     */
    struct ErrorRecoveryResult {
        bool isRecovered;
        RecoveryAction action;
        std::string message;
        std::chrono::steady_clock::time_point recoveryTime;
        double performanceImpact;
    };

    /**
     * @struct OptimizationRecommendation
     * @brief Optimization recommendation
     */
    struct OptimizationRecommendation {
        std::string recommendationType;
        std::string description;
        double expectedImprovement;
        double implementationCost;
        std::vector<std::string> steps;
        bool isImplemented;
    };

    AutomatedDebugger();
    ~AutomatedDebugger();

    /**
     * @brief Start automated debugging
     * @param level Debug level
     */
    void startAutomatedDebugging(DebugLevel level = DebugLevel::INTERMEDIATE);

    /**
     * @brief Stop automated debugging
     */
    void stopAutomatedDebugging();

    /**
     * @brief Run automated diagnostics
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     * @return Vector of anomaly detection results
     */
    std::vector<AnomalyDetectionResult> runAutomatedDiagnostics(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Recover from errors
     * @param anomaly Anomaly detection result
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     * @return Error recovery result
     */
    ErrorRecoveryResult recoverFromError(const AnomalyDetectionResult& anomaly, Brain& brain, NeuralWorkingMemory& memory, Neuromodulator& neuromodulator, Config& config);

    /**
     * @brief Generate optimization recommendations
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     * @return Vector of optimization recommendations
     */
    std::vector<OptimizationRecommendation> generateOptimizationRecommendations(const Brain& brain, const NeuralWorkingMemory& memory, const Neuromodulator& neuromodulator, const Config& config);

    /**
     * @brief Implement optimization recommendation
     * @param recommendation Optimization recommendation
     * @param brain Neural network reference
     * @param memory Memory system reference
     * @param neuromodulator Neuromodulation system reference
     * @param config Configuration reference
     */
    void implementOptimizationRecommendation(const OptimizationRecommendation& recommendation, Brain& brain, NeuralWorkingMemory& memory, Neuromodulator& neuromodulator, Config& config);

    /**
     * @brief Get automated debugging status
     * @return Debugging status
     */
    struct DebuggingStatus {
        bool isActive;
        DebugLevel level;
        uint64_t anomaliesDetected;
        uint64_t recoveriesPerformed;
        uint64_t optimizationsImplemented;
        std::chrono::steady_clock::time_point lastDiagnosticRun;
        std::chrono::steady_clock::time_point lastRecovery;
        std::chrono::steady_clock::time_point lastOptimization;
    };

    DebuggingStatus getDebuggingStatus() const;

    /**
     * @brief Set debug level
     * @param level Debug level
     */
    void setDebugLevel(DebugLevel level);

    /**
     * @brief Get debug level
     * @return Debug level
     */
    DebugLevel getDebugLevel() const;

    /**
     * @brief Enable machine learning-based anomaly detection
     */
    void enableMachineLearningAnomalyDetection();

    /**
     * @brief Disable machine learning-based anomaly detection
     */
    void disableMachineLearningAnomalyDetection();

    /**
     * @brief Is machine learning anomaly detection enabled?
     * @return true if machine learning anomaly detection is enabled
     */
    bool isMachineLearningAnomalyDetectionEnabled() const;

    /**
     * @brief Train anomaly detection model
     * @param trainingData Training data
     */
    void trainAnomalyDetectionModel(const std::vector<std::map<std::string, double>>& trainingData);

    /**
     * @brief Predict anomalies
     * @param data Data to analyze
     * @return Anomaly detection result
     */
    AnomalyDetectionResult predictAnomalies(const std::map<std::string, double>& data);

    /**
     * @brief Generate debugging report
     * @return Debugging report
     */
    struct DebuggingReport {
        std::chrono::steady_clock::time_point reportTime;
        DebugLevel debugLevel;
        size_t anomaliesDetected;
        size_t recoveriesPerformed;
        size_t optimizationsImplemented;
        std::vector<AnomalyDetectionResult> anomalies;
        std::vector<ErrorRecoveryResult> recoveries;
        std::vector<OptimizationRecommendation> optimizations;
    };

    DebuggingReport generateDebuggingReport();

    /**
     * @brief Export debugging data
     * @param filename Output file path
     */
    void exportDebuggingData(const std::string& filename);

    /**
     * @brief Import debugging data
     * @param filename Input file path
     */
    void importDebuggingData(const std::string& filename);

    /**
     * @brief Clear debugging data
     */
    void clearDebuggingData();

    /**
     * @brief Generate automated debugging visualization
     * @param filename Output file path
     */
    void generateAutomatedDebuggingVisualization(const std::string& filename);

    /**
     * @brief Generate anomaly detection visualization
     * @param filename Output file path
     */
    void generateAnomalyDetectionVisualization(const std::string& filename);

    /**
     * @brief Generate recovery visualization
     * @param filename Output file path
     */
    void generateRecoveryVisualization(const std::string& filename);

    /**
     * @brief Generate optimization visualization
     * @param filename Output file path
     */
    void generateOptimizationVisualization(const std::string& filename);

protected:
    /**
     * @brief Run diagnostic checks
     */
    void runDiagnosticChecks();

    /**
     * @brief Check for anomalies
     */
    void checkForAnomalies();

    /**
     * @brief Run recovery actions
     */
    void runRecoveryActions();

    /**
     * @brief Run optimization actions
     */
    void runOptimizationActions();

    /**
     * @brief Generate debugging report
     */
    void generateDebuggingReportInternal();

    /**
     * @brief Create visualization
     * @param filename Output file path
     */
    void createVisualization(const std::string& filename);

    /**
     * @brief Create anomaly detection visualization
     * @param filename Output file path
     */
    void createAnomalyDetectionVisualization(const std::string& filename);

    /**
     * @brief Create recovery visualization
     * @param filename Output file path
     */
    void createRecoveryVisualization(const std::string& filename);

    /**
     * @brief Create optimization visualization
     * @param filename Output file path
     */
    void createOptimizationVisualization(const std::string& filename);

    /**
     * @brief Train machine learning model
     * @param trainingData Training data
     */
    void trainMachineLearningModel(const std::vector<std::map<std::string, double>>& trainingData);

    /**
     * @brief Predict anomaly
     * @param data Data to analyze
     * @return Anomaly detection result
     */
    AnomalyDetectionResult predictAnomaly(const std::map<std::string, double>& data);

    /**
     * @brief Generate error message
     * @param anomaly Anomaly detection result
     * @return Error message
     */
    std::string generateErrorMessage(const AnomalyDetectionResult& anomaly) const;

    /**
     * @brief Generate recovery message
     * @param recovery Error recovery result
     * @return Recovery message
     */
    std::string generateRecoveryMessage(const ErrorRecoveryResult& recovery) const;

    /**
     * @brief Generate optimization message
     * @param recommendation Optimization recommendation
     * @return Optimization message
     */
    std::string generateOptimizationMessage(const OptimizationRecommendation& recommendation) const;

    /**
     * @brief Is anomaly severe?
     * @param anomaly Anomaly detection result
     * @return true if anomaly is severe
     */
    bool isSevereAnomaly(const AnomalyDetectionResult& anomaly) const;

    /**
     * @brief Get recommended recovery action
     * @param anomaly Anomaly detection result
     * @return Recommended recovery action
     */
    RecoveryAction getRecommendedRecoveryAction(const AnomalyDetectionResult& anomaly) const;

    /**
     * @brief Get recommended optimization action
     * @param anomaly Anomaly detection result
     * @return Recommended optimization action
     */
    std::string getRecommendedOptimizationAction(const AnomalyDetectionResult& anomaly) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // AUTOMATEDDEBUGGER_HPP
