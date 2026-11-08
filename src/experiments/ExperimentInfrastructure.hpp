#pragma once

#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Types/Types.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <map>

namespace nlm {

/**
 * @brief Common logging infrastructure for all experiments
 * Eliminates duplication of ExperimentLogger across experiment files
 */
class ExperimentLogger {
public:
    ExperimentLogger(const std::string& filename);
    ~ExperimentLogger();
    
    void log(const std::string& message);
    void logEpisodeStart(int episode);
    void logEpisodeEnd(int episode, float totalReward, int steps, float finalNovelty);
    void logStep(int step, float reward, float neuromod, float curiosity, 
                 float energy, const std::string& action);
    void logDevelopment(float age, const std::string& stage);
    void logSynapticStats(size_t totalSynapses, float avgWeight, float maxWeight);
    void logPhase3Header();
    
    void flush();
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @brief Common metrics collection for all experiments
 * Eliminates duplication of ExperimentMetrics across experiment files
 */
struct ExperimentMetrics {
    // Episode metrics
    int episode;
    int steps;
    float totalReward;
    float finalEnergy;
    float finalNovelty;
    float finalCuriosity;
    float predictionError;
    
    // Development
    float developmentalAge;
    std::string developmentalStage;
    
    // Network state
    size_t totalSynapses;
    float averageWeight;
    float maxWeight;
    size_t totalSpikes;
    float averageFiringRate;
    
    // Motor behavior
    int forwardCount;
    int backwardCount;
    int turnLeftCount;
    int turnRightCount;
    int interactCount;
    int waitCount;
    int lookLeftCount;
    int lookRightCount;
    
    ExperimentMetrics();
    void reset();
    std::string toCSV() const;
};

/**
 * @brief Base class for all neural experiments
 * Provides common functionality and reduces boilerplate code
 */
class NeuralExperiment {
public:
    NeuralExperiment();
    virtual ~NeuralExperiment();
    
    // Run the experiment
    virtual bool run(std::shared_ptr<Brain> brain) = 0;
    
    // Get results
    virtual const ExperimentMetrics& getMetrics() const { return metrics_; }
    
    // Get name
    virtual const std::string& getName() const { return name_; }
    virtual void setName(const std::string& name) { name_ = name; }
    
    // Get logger
    ExperimentLogger& getLogger() { return logger_; }
    
protected:
    // Common logging methods
    void logProgress(const std::string& message);
    void logError(const std::string& message);
    void logWarning(const std::string& message);
    
    // Common validation
    bool validateBrain(std::shared_ptr<Brain> brain);
    
    // Common metrics recording
    void recordNetworkMetrics(std::shared_ptr<Brain> brain, SimulationStep step);
    
private:
    std::string name_;
    ExperimentLogger logger_;
    ExperimentMetrics metrics_;
};

/**
 * @brief Experiment result aggregator
 * Common result handling across experiments
 */
class ExperimentResultAggregator {
public:
    ExperimentResultAggregator();
    ~ExperimentResultAggregator();
    
    // Add experiment results
    void addResult(const std::string& experimentName, const ExperimentMetrics& metrics);
    
    // Get aggregated results
    double getAverageMetric(const std::string& metricName) const;
    size_t getExperimentCount() const { return experimentCount_; }
    
    // Save results
    bool saveResults(const std::string& filepath) const;
    
    // Get all results as CSV
    std::string getResultsAsCSV() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    size_t experimentCount_;
};

/**
 * @brief Configuration helper for experiments
 * Common configuration handling across experiments
 */
class ExperimentConfig {
public:
    ExperimentConfig();
    ~ExperimentConfig();
    
    // Load configuration
    bool loadConfig(const std::string& configFile);
    
    // Get configuration value
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value
    void setValue(const std::string& key, const std::string& value);
    
    // Validate configuration
    bool validate() const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
