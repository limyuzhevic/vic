// Enhanced NLM Logging System
// Comprehensive logging and statistics for neural network monitoring

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <memory>

namespace nlm {

// Enhanced log entry with neural system metrics
struct EnhancedLogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Neural system metrics
    double neuronActivity;
    double synapticStrength;
    double learningRate;
    double memoryUtilization;
    double predictionAccuracy;
    double rewardSignal;
    
    // Performance metrics
    std::chrono::milliseconds processingTime;
    double energyEfficiency;
    double temperature;
    
    // Event tracking
    std::string eventType;
    std::string neuralSystem;
    std::map<std::string, double> contextMetrics;
    
    EnhancedLogEntry(LogLevel level, const std::string& message) : level(level), message(message), 
        neuronActivity(0.0), synapticStrength(0.0), learningRate(0.0), memoryUtilization(0.0),
        predictionAccuracy(0.0), rewardSignal(0.0), processingTime(0), energyEfficiency(0.0), temperature(0.0) {}
};

// Advanced statistics tracker for neural network monitoring
class NeuralStatisticsTracker {
public:
    struct StatisticsSnapshot {
        std::chrono::system_clock::time_point timestamp;
        double totalNeurons;
        double totalSynapses;
        double activeNeurons;
        double firingRate;
        double energyConsumption;
        double temperature;
        double memoryUsage;
        double predictionAccuracy;
        double rewardAccumulation;
        double plasticityEffectiveness;
        double learningProgress;
        std::map<std::string, double> subsystemMetrics;
    };
    
    NeuralStatisticsTracker(size_t maxHistorySize = 10000);
    ~NeuralStatisticsTracker();
    
    // Record a new statistics snapshot
    void recordSnapshot(const StatisticsSnapshot& snapshot);
    
    // Get current statistics
    StatisticsSnapshot getCurrentSnapshot() const;
    
    // Get historical statistics
    std::vector<StatisticsSnapshot> getHistory() const { return history; }
    
    // Statistical analysis
    double getAverageMetric(const std::string& metricName) const;
    double getMetricVariance(const std::string& metricName) const;
    double getMetricStdDev(const std::string& metricName) const;
    double getMetricPercentile(const std::string& metricName, double percentile) const;
    
    // Time-based analysis
    std::vector<StatisticsSnapshot> getSnapshotsInTimeRange(
        std::chrono::system_clock::time_point start,
        std::chrono::system_clock::time_point end) const;
    
    // Performance analysis
    double calculateEnergyEfficiency() const;
    double calculateLearningRate() const;
    double calculateMemoryEfficiency() const;
    std::string getPerformanceGrade() const;
    
    // Event tracking
    void recordEvent(const std::string& eventType, const std::string& neuralSystem, double value = 0.0);
    std::vector<std::tuple<std::chrono::system_clock::time_point, std::string, std::string, double>> getEventHistory() const;
    
    // Configuration
    void setSamplingRate(double rate); // samples per second
    void setMetricFilter(const std::vector<std::string>& metrics);
    void setOutputFormat(const std::string& format);
    
    // Output and persistence
    void exportStatistics(const std::string& filepath) const;
    void exportCSV(const std::string& filepath) const;
    void exportJSON(const std::string& filepath) const;
    
    // Alerting
    void setAlertThreshold(const std::string& metric, double threshold, LogLevel level);
    void checkAlerts();
    
    // Reset and maintenance
    void clearHistory();
    void resetDailyCounters();
    
    // Query and analysis
    size_t getSnapshotCount() const { return history.size(); }
    std::chrono::system_clock::time_point getFirstSnapshotTime() const;
    std::chrono::system_clock::time_point getLastSnapshotTime() const;
    
private:
    std::vector<StatisticsSnapshot> history;
    size_t maxHistorySize;
    double samplingRate;
    std::vector<std::string> metricFilter;
    std::string outputFormat;
    
    std::vector<std::tuple<std::chrono::system_clock::time_point, std::string, std::string, double>> eventHistory;
    std::map<std::string, std::pair<double, LogLevel>> alertThresholds;
    
    mutable std::mutex mutex;
    
    std::chrono::system_clock::time_point lastSampleTime;
    size_t samplesThisSecond;
    
    StatisticsSnapshot calculateMovingAverage(size_t windowSize) const;
    void cleanupOldHistory();
};

// Enhanced neural-specific logger with advanced metrics
class EnhancedConsoleLogger : public ILogger {
public:
    EnhancedConsoleLogger();
    explicit EnhancedConsoleLogger(LogLevel level);
    ~EnhancedConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced output options
    void setUseColors(bool useColors) override;
    bool getUseColors() const override;
    
    void setOutputFile(const std::string& filepath) override;
    void closeOutputFile() override;
    
    // Enhanced statistics integration
    void setStatisticsTracker(std::shared_ptr<NeuralStatisticsTracker> tracker);
    std::shared_ptr<NeuralStatisticsTracker> getStatisticsTracker() const;
    
    // Enhanced neural metrics logging
    void logNeuralMetrics(double neurons, double synapses, double firingRate, double energyEfficiency);
    void logPlasticityEvent(const std::string& event, double effectiveness, double weightChange);
    void logMemoryEvent(const std::string& event, double usage, double efficiency);
    void logLearningEvent(double reward, double predictionError, double learningProgress);
    void logDevelopmentEvent(double stage, double plasticity, double cognitiveLoad);
    
    // Performance monitoring
    void setPerformanceMonitor(std::function<void(double)> monitor);
    void setEnergyMonitor(std::function<double()> monitor);
    void setTemperatureMonitor(std::function<double()> monitor);
    
    // Advanced formatting
    void setFormat(const std::string& format);
    std::string formatLogEntry(const EnhancedLogEntry& entry) const;
    std::string formatNeuralMetrics(const EnhancedLogEntry& entry) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Enhanced main Logger class with statistics integration
class EnhancedLogger : public Logger {
public:
    EnhancedLogger();
    ~EnhancedLogger() override;
    
    // Statistics integration
    void setStatisticsTracker(std::shared_ptr<NeuralStatisticsTracker> tracker);
    std::shared_ptr<NeuralStatisticsTracker> getStatisticsTracker() const;
    
    // Enhanced logging with neural metrics
    void logWithMetrics(LogLevel level, const std::string& message, 
                       double neuronActivity = 0.0, double synapticStrength = 0.0,
                       double learningRate = 0.0, double memoryUtilization = 0.0,
                       double predictionAccuracy = 0.0, double rewardSignal = 0.0);
    
    // Neural system specific logging
    void logNeuralComputation(double neurons, double synapses, double firingRate, double efficiency);
    void logPlasticityUpdate(const std::string& rule, double weightChange, double effectiveness);
    void logMemoryOperation(const std::string& operation, double usage, double efficiency);
    void logLearningProgress(double reward, double error, double progress);
    
    // Performance tracking
    void recordProcessingTime(std::chrono::milliseconds time);
    void recordEnergyConsumption(double energy);
    void recordTemperature(double temperature);
    
    // Alert and warning system
    void addAlert(const std::string& condition, const std::string& message, LogLevel level = LogLevel::Warning);
    std::vector<std::pair<std::string, std::string>> getActiveAlerts() const;
    void clearAlerts();
    
private:
    std::shared_ptr<NeuralStatisticsTracker> statisticsTracker_;
    std::vector<std::pair<std::string, std::string>> activeAlerts_;
};

// Convenience macros for enhanced logging
#define NLM_LOG_DEBUG_WITH_METRICS(msg, neurons, synapses, firing_rate) \
    nlm::EnhancedLogger::getGlobal().logWithMetrics(nlm::LogLevel::Debug, msg, neurons, synapses, firing_rate)

#define NLM_LOG_INFO_WITH_METRICS(msg, neurons, synapses, firing_rate, energy, memory) \
    nlm::EnhancedLogger::getGlobal().logWithMetrics(nlm::LogLevel::Info, msg, neurons, synapses, firing_rate, energy, memory)

#define NLM_LOG_PL plasticity_event(event, effectiveness, weight_change) \
    nlm::EnhancedLogger::getGlobal().logPlasticityUpdate(event, effectiveness, weight_change)

#define NLM_LOG_MEMORY_OPER operation, usage, efficiency) \
    nlm::EnhancedLogger::getGlobal().logMemoryOperation(operation, usage, efficiency)

#define NLM_LOG_LEARNING(reward, error, progress) \
    nlm::EnhancedLogger::getGlobal().logLearningProgress(reward, error, progress)

// Statistics export and analysis utilities
namespace StatisticsUtils {
    std::string generateReport(const std::vector<StatisticsSnapshot>& data);
    double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    std::pair<double, double> linearRegression(const std::vector<double>& x, const std::vector<double>& y);
    std::vector<double> smoothData(const std::vector<double>& data, size_t windowSize);
    std::string formatTime(std::chrono::system_clock::time_point time);
    std::string formatDuration(std::chrono::milliseconds duration);
}

// Global enhanced logger instance
inline EnhancedLogger& getEnhancedLogger() {
    static EnhancedLogger logger;
    return logger;
}

// Convenience functions for easy access
inline void logNeuralMetrics(double neurons, double synapses, double firingRate, double energyEfficiency) {
    getEnhancedLogger().logNeuralComputation(neurons, synapses, firingRate, energyEfficiency);
}

inline void logPlasticityEvent(const std::string& event, double effectiveness, double weightChange) {
    getEnhancedLogger().logPlasticityUpdate(event, effectiveness, weightChange);
}

inline void logMemoryEvent(const std::string& event, double usage, double efficiency) {
    getEnhancedLogger().logMemoryOperation(event, usage, efficiency);
}

inline void logLearningEvent(double reward, double predictionError, double learningProgress) {
    getEnhancedLogger().logLearningProgress(reward, predictionError, learningProgress);
}

} // namespace nlm