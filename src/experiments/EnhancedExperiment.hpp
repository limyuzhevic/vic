#pragma once

#include "Experiment.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <ctime>

namespace nlm {

// Experiment lifecycle states
enum class ExperimentState {
    UNINITIALIZED,
    INITIALIZED,
    RUNNING,
    COMPLETED,
    FAILED,
    PAUSED
};

// Enhanced experiment with comprehensive lifecycle management
class EnhancedExperiment {
public:
    EnhancedExperiment();
    EnhancedExperiment(const std::string& name);
    EnhancedExperiment(const std::string& name, const std::string& description);
    ~EnhancedExperiment();
    
    // Experiment metadata
    const std::string& getName() const;
    void setName(const std::string& name);
    const std::string& getDescription() const;
    void setDescription(const std::string& description);
    
    // Lifecycle management
    ExperimentState getState() const;
    bool isRunning() const;
    bool isCompleted() const;
    bool isFailed() const;
    
    void initialize();
    void start();
    void pause();
    void resume();
    void complete();
    void fail(const std::string& errorMessage);
    
    // Timing and duration
    void setStartTime();
    void setEndTime();
    const std::chrono::system_clock::time_point& getStartTime() const;
    const std::chrono::system_clock::time_point& getEndTime() const;
    double getDuration() const; // in seconds
    
    // State tracking and persistence
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);
    bool hasCheckpoint() const;
    bool clearCheckpoint();
    
    // Configuration management
    void setConfig(const std::shared_ptr<Config>& config);
    const std::shared_ptr<Config>& getConfig() const;
    void setParameter(const std::string& key, const std::string& value);
    std::string getParameter(const std::string& key) const;
    bool hasParameter(const std::string& key) const;
    std::vector<std::string> getParameterKeys() const;
    
    // Enhanced metrics collection
    void recordMetric(const std::string& name, double value, 
                     const std::string& category = "general",
                     const std::unordered_map<std::string, std::string>& metadata = {});
    void recordMetric(const std::string& name, float value,
                     const std::string& category = "general",
                     const std::unordered_map<std::string, std::string>& metadata = {});
    void recordMetric(const std::string& name, int value,
                     const std::string& category = "general",
                     const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Get metrics by category
    const std::vector<std::pair<std::string, double>>& getMetrics() const;
    const std::vector<std::pair<std::string, double>>& getMetrics(const std::string& category) const;
    double getMetric(const std::string& name) const;
    double getMetric(const std::string& name, const std::string& category) const;
    bool hasMetric(const std::string& name) const;
    bool hasMetric(const std::string& name, const std::string& category) const;
    
    // Metrics statistics
    double getMetricMean(const std::string& name) const;
    double getMetricStdDev(const std::string& name) const;
    double getMetricMin(const std::string& name) const;
    double getMetricMax(const std::string& name) const;
    
    // Enhanced step recording
    struct EnhancedStepData {
        SimulationStep step;
        std::chrono::system_clock::time_point timestamp;
        size_t neuronCount;
        size_t synapseCount;
        size_t firingCount;
        float avgFiringRate;
        double reward;
        std::unordered_map<std::string, double> additionalMetrics;
    };
    
    void recordStep(const EnhancedStepData& stepData);
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                   size_t firingCount, float avgFiringRate, double reward = 0.0);
    const std::vector<EnhancedStepData>& getStepData() const;
    
    // Notes and metadata
    void addNote(const std::string& note);
    void addNote(const std::string& category, const std::string& note);
    const std::vector<std::pair<std::string, std::string>>& getNotes() const;
    
    // Experiment results
    std::string getResultsSummary() const;
    std::string getResultsJSON() const;
    std::string getResultsCSV() const;
    
    // Random seed and reproducibility
    uint64_t getSeed() const;
    void setSeed(uint64_t seed);
    
    // Validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Template system
    void setTemplate(const std::string& templateName);
    const std::string& getTemplate() const;
    static std::vector<std::string> getAvailableTemplates();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
