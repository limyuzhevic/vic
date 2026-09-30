// AdvancedExperimentController.hpp

/**
 * @file AdvancedExperimentController.hpp
 * @brief Advanced experiment control and analysis module for NLM
 * 
 * This file defines the AdvancedExperimentController class, which provides
 * advanced users with sophisticated control over experiments and detailed
 * analysis capabilities for the NLM (熙然) neural simulation framework.
 * 
 * @author NLM Research Team
 * @date Phase 2+: Advanced Features
 * @version 0.1.0
 */

#ifndef NLM_ADVANCED_EXPERIMENT_CONTROLLER_HPP
#define NLM_ADVANCED_EXPERIMENT_CONTROLLER_HPP

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Logger/Logger.hpp"

namespace nlm {

/**
 * @class AdvancedExperimentController
 * @brief Advanced experiment control and analysis module
 * 
 * The AdvancedExperimentController provides advanced users with sophisticated
 * control over experiments, including:
 * - Multiple advanced stimulus patterns
 * - Real-time monitoring and analysis
 * - Performance metrics tracking
 * - Automatic experiment control
 * - Comprehensive reporting
 * - Advanced parameter configuration
 * 
 * This class is designed for experienced users who require fine-grained
 * control over neural simulation experiments and detailed analysis of results.
 */
class AdvancedExperimentController {
private:
    struct StimulusPattern;
    
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    
    // Experiment state tracking
    struct ExperimentState {
        size_t currentStep;
        size_t totalSteps;
        size_t experimentStartTime;
        std::vector<std::string> stimulusHistory;
        std::vector<float> performanceMetrics;
        std::vector<float> synapticChanges;
        bool experimentRunning;
        bool experimentPaused;
    } state;
    
    // Current stimulus pattern
    StimulusPattern currentPattern;
    
    // Performance tracking
    struct PerformanceMetrics {
        float learningRate;
        float synapticStability;
        float networkStability;
        float explorationExploitationBalance;
        float memoryRetention;
        float predictionAccuracy;
    } currentMetrics;
    
public:
    /**
     * @brief Construct a new AdvancedExperimentController object
     * 
     * @param brain Shared pointer to the NLM brain
     * @param config Shared pointer to the configuration
     */
    AdvancedExperimentController(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy the AdvancedExperimentController object
     */
    ~AdvancedExperimentController();

    // Experiment control methods
    /**
     * @brief Start an experiment with the specified stimulus pattern
     * 
     * @param patternName Name of the stimulus pattern to use
     */
    void startExperiment(const std::string& patternName);
    
    /**
     * @brief Stop the current experiment
     */
    void stopExperiment();
    
    /**
     * @brief Pause the current experiment
     */
    void pauseExperiment();
    
    /**
     * @brief Resume the current experiment
     */
    void resumeExperiment();
    
    /**
     * @brief Run a single step of the current experiment
     */
    void runExperimentStep();

    // Experiment information methods
    /**
     * @brief Check if an experiment is currently running
     * 
     * @return bool True if experiment is running, false otherwise
     */
    bool isExperimentRunning() const;
    
    /**
     * @brief Check if the current experiment is paused
     * 
     * @return bool True if experiment is paused, false otherwise
     */
    bool isExperimentPaused() const;
    
    /**
     * @brief Get the current step of the experiment
     * 
     * @return size_t Current step number
     */
    size_t getCurrentStep() const;
    
    /**
     * @brief Get the total number of steps for the experiment
     * 
     * @return size_t Total number of steps
     */
    size_t getTotalSteps() const;
    
    /**
     * @brief Get the current experiment status
     * 
     * @return std::string Status description
     */
    std::string getExperimentStatus() const;

    // Performance metrics methods
    /**
     * @brief Get the current learning rate
     * 
     * @return float Learning rate
     */
    float getLearningRate() const;
    
    /**
     * @brief Get the current synaptic stability
     * 
     * @return float Synaptic stability (0.0 = unstable, 1.0 = stable)
     */
    float getSynapticStability() const;
    
    /**
     * @brief Get the current network stability
     * 
     * @return float Network stability (0.0 = unstable, 1.0 = stable)
     */
    float getNetworkStability() const;
    
    /**
     * @brief Get the current memory retention
     * 
     * @return float Memory retention (0.0 = poor, 1.0 = excellent)
     */
    float getMemoryRetention() const;
    
    /**
     * @brief Get the current prediction accuracy
     * 
     * @return float Prediction accuracy (0.0 = poor, 1.0 = excellent)
     */
    float getPredictionAccuracy() const;

    // Configuration methods
    /**
     * @brief Set the duration of the experiment in steps
     * 
     * @param steps Number of steps for the experiment
     */
    void setExperimentDuration(size_t steps);
    
    /**
     * @brief Configure advanced parameters
     * 
     * @param prefix Parameter prefix
     * @param value Parameter value
     */
    void configureAdvancedParameters(const std::string& prefix, const std::string& value);
    
    /**
     * @brief Get the list of available experiment patterns
     * 
     * @return std::vector<std::string> List of available pattern names
     */
    std::vector<std::string> getAvailablePatterns() const;

    // Monitoring methods
    /**
     * @brief Enable or disable real-time monitoring
     * 
     * @param enable True to enable monitoring, false to disable
     */
    void enableRealTimeMonitoring(bool enable);
    
    /**
     * @brief Enable or disable automatic saving
     * 
     * @param enable True to enable automatic saving, false to disable
     */
    void enableAutoSave(bool enable);

    // Analysis methods
    /**
     * @brief Generate a comprehensive experiment report
     */
    void generateExperimentReport();

private:
    // Advanced stimulus pattern structure
    struct StimulusPattern {
        std::string name;
        std::function<void(std::shared_ptr<Brain>)> applyStimulus;
        std::vector<float> parameters;
        size_t duration;
        float intensity;
    };
    
    // Initialize advanced features
    void initializeAdvancedFeatures();
    
    // Initialize stimulus patterns
    void initializeStimulusPatterns();
    
    // Initialize performance tracking
    void initializePerformanceTracking();
    
    // Initialize analysis tools
    void initializeAnalysisTools();
    
    // Record experiment state
    void recordExperimentState();
    
    // Update performance metrics
    void updatePerformanceMetrics();
    
    // Compute performance metrics
    void computePerformanceMetrics();
    
    // Apply specific stimulus patterns
    void applySTDI StimulusPattern();
    void applyHebbianLearningWithMemory();
    void applyNeuromodulationBasedLearning();
    void applyDevelopmentalLearning();
    
    // Neuromodulation methods
    void applyDopamineModulation(float level);
    void applyCuriosityModulation(float level);
    void applyNoveltyModulation(float level);
    
    // Analysis helper methods
    void logExperimentProgress();
    
    std::string calculateInitialAverageWeight();
    std::string calculateFinalAverageWeight();
    std::string calculateTotalWeightChange();
    
    std::string getCurrentTimestamp();
    
    void saveExperimentReport(const std::string& report);
    
    // Get current timestamp for logging
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

} // namespace nlm

#endif // NLM_ADVANCED_EXPERIMENT_CONTROLLER_HPP