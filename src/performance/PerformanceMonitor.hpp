// Performance monitoring and statistics module
// 
// This module provides comprehensive performance monitoring for NLM
// It includes statistics collection, performance analysis, and debugging tools

#ifndef NLM_PERFORMANCE_H
#define NLM_PERFORMANCE_H

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>

namespace nlm {

class PerformanceMonitor {
public:
    struct PerformanceStats {
        // Timing statistics
        double cpuTime;                    // CPU time in seconds
        double wallClockTime;               // Wall clock time in seconds
        double simulationTime;              // Simulation time in seconds
        double memoryUsage;                 // Memory usage in MB
        
        // Neural statistics
        size_t totalSpikes;                 // Total spike count
        size_t totalNeurons;                // Total neuron count
        size_t totalSynapses;               // Total synapse count
        size_t activeNeurons;               // Currently active neurons
        size_t firingNeurons;               // Currently firing neurons
        
        // Plasticity statistics
        size_t weightUpdates;               // Number of weight updates
        float avgSynapticWeight;            // Average synaptic weight
        float weightChangeRate;             // Rate of weight change
        
        // Memory statistics
        size_t workingMemoryTraces;         // Active working memory traces
        size_t episodicMemoryEpisodes;     // Number of episodes stored
        
        // Neuromodulation statistics
        float dopamineLevel;                // Dopamine concentration
        float curiosityLevel;               // Curiosity level
        float noveltyLevel;                  // Novelty detection level
        
        // Development statistics
        int developmentalStage;             // Current developmental stage
        double developmentalAge;            // Developmental age in steps
        
        // Simulation statistics
        size_t simulationSteps;             // Number of simulation steps
        size_t targetSpikingRate;           // Target spiking rate
        double efficiencyScore;             // Performance efficiency score
        
        PerformanceStats()
            : cpuTime(0.0), wallClockTime(0.0), simulationTime(0.0), memoryUsage(0.0)
            , totalSpikes(0), totalNeurons(0), totalSynapses(0), activeNeurons(0)
            , firingNeurons(0), weightUpdates(0), avgSynapticWeight(0.0f)
            , weightChangeRate(0.0f), workingMemoryTraces(0), episodicMemoryEpisodes(0)
            , dopamineLevel(0.0f), curiosityLevel(0.0f), noveltyLevel(0.0f)
            , developmentalStage(0), developmentalAge(0.0), simulationSteps(0)
            , targetSpikingRate(0), efficiencyScore(0.0) {}
    };
    
    struct PerformanceThresholds {
        // Performance thresholds
        double maxCpuTime;                  // Maximum CPU time (seconds)
        double maxMemoryUsage;              // Maximum memory usage (MB)
        size_t minSpikingRate;              // Minimum desired spiking rate
        float maxWeightChangeRate;          // Maximum weight change rate
        
        PerformanceThresholds()
            : maxCpuTime(300.0), maxMemoryUsage(1024.0), minSpikingRate(10)
            , maxWeightChangeRate(0.1f) {}
    };
    
    struct TimePoint {
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
        std::string operationName;
        
        TimePoint(const std::string& name)
            : operationName(name), start(std::chrono::high_resolution_clock::now()) {}
        
        ~TimePoint() {
            end = std::chrono::high_resolution_clock::now();
        }
        
        double elapsedSeconds() const {
            return std::chrono::duration<double>(end - start).count();
        }
    };
    
    struct SamplePoint {
        double timestamp;                    // Simulation timestamp
        size_t step;                         // Simulation step
        PerformanceStats stats;              // Statistics at this point
        
        SamplePoint(double ts, size_t s, const PerformanceStats& st)
            : timestamp(ts), step(s), stats(st) {}
    };
    
private:
    PerformanceStats currentStats_;
    PerformanceThresholds thresholds_;
    std::vector<SamplePoint> history_;
    std::unordered_map<std::string, double> lastOperationTimes_;
    bool monitoringEnabled_;
    double samplingInterval_;
    size_t maxHistorySize_;
    
    // Performance monitoring flags
    bool cpuMonitoringEnabled_;
    bool memoryMonitoringEnabled_;
    bool neuralMonitoringEnabled_;
    bool plasticityMonitoringEnabled_;
    bool memorySystemMonitoringEnabled_;
    bool neuromodulationMonitoringEnabled_;
    bool developmentMonitoringEnabled_;
    
    // File output
    std::string outputFile_;
    bool writeToFile_;
    
    // Threshold monitoring
    bool thresholdViolationsReported_;
    std::vector<std::string> thresholdViolations_;
    
public:
    PerformanceMonitor()
        : monitoringEnabled_(true), samplingInterval_(1.0), maxHistorySize_(1000)
        , cpuMonitoringEnabled_(true), memoryMonitoringEnabled_(true)
        , neuralMonitoringEnabled_(true), plasticityMonitoringEnabled_(true)
        , memorySystemMonitoringEnabled_(true), neuromodulationMonitoringEnabled_(true)
        , developmentMonitoringEnabled_(true)
        , writeToFile_(false), thresholdViolationsReported_(false) {}
    
    ~PerformanceMonitor() {
        if (writeToFile_ && !outputFile_.empty()) {
            // Write statistics to file
            // Implementation would write to outputFile_
        }
    }
    
    void enableMonitoring(bool enabled) {
        monitoringEnabled_ = enabled;
    }
    
    bool isMonitoringEnabled() const {
        return monitoringEnabled_;
    }
    
    void setSamplingInterval(double interval) {
        samplingInterval_ = interval;
    }
    
    double getSamplingInterval() const {
        return samplingInterval_;
    }
    
    void setMaxHistorySize(size_t size) {
        maxHistorySize_ = size;
        if (history_.size() > maxHistorySize_) {
            history_.erase(history_.begin(), history_.begin() + 
                          (history_.size() - maxHistorySize_));
        }
    }
    
    void setOutputFile(const std::string& filename) {
        outputFile_ = filename;
        writeToFile_ = !filename.empty();
    }
    
    void setThresholds(const PerformanceThresholds& thresholds) {
        thresholds_ = thresholds;
    }
    
    void enableCpuMonitoring(bool enabled) {
        cpuMonitoringEnabled_ = enabled;
    }
    
    void enableMemoryMonitoring(bool enabled) {
        memoryMonitoringEnabled_ = enabled;
    }
    
    void enableNeuralMonitoring(bool enabled) {
        neuralMonitoringEnabled_ = enabled;
    }
    
    void enablePlasticityMonitoring(bool enabled) {
        plasticityMonitoringEnabled_ = enabled;
    }
    
    void enableMemorySystemMonitoring(bool enabled) {
        memorySystemMonitoringEnabled_ = enabled;
    }
    
    void enableNeuromodulationMonitoring(bool enabled) {
        neuromodulationMonitoringEnabled_ = enabled;
    }
    
    void enableDevelopmentMonitoring(bool enabled) {
        developmentMonitoringEnabled_ = enabled;
    }
    
    void update(const PerformanceStats& stats, double timestamp, size_t step) {
        if (!monitoringEnabled_) return;
        
        currentStats_ = stats;
        
        // Check thresholds
        checkThresholds(stats);
        
        // Add to history
        history_.emplace_back(timestamp, step, stats);
        
        // Maintain history size
        if (history_.size() > maxHistorySize_) {
            history_.erase(history_.begin());
        }
        
        // Write to file if enabled
        if (writeToFile_) {
            // Write to file
        }
    }
    
    const PerformanceStats& getCurrentStats() const {
        return currentStats_;
    }
    
    const PerformanceStats& getStatsAt(size_t index) const {
        if (index < history_.size()) {
            return history_[index].stats;
        }
        static PerformanceStats emptyStats;
        return emptyStats;
    }
    
    size_t getHistorySize() const {
        return history_.size();
    }
    
    const std::vector<SamplePoint>& getHistory() const {
        return history_;
    }
    
    std::string getStatsSummary() const {
        std::stringstream ss;
        ss << "Performance Statistics Summary:" << std::endl;
        ss << "  Wall Clock Time: " << currentStats_.wallClockTime << " s" << std::endl;
        ss << "  CPU Time: " << currentStats_.cpuTime << " s" << std::endl;
        ss << "  Simulation Time: " << currentStats_.simulationTime << " s" << std::endl;
        ss << "  Memory Usage: " << currentStats_.memoryUsage << " MB" << std::endl;
        ss << "  Total Spikes: " << currentStats_.totalSpikes << std::endl;
        ss << "  Total Neurons: " << currentStats_.totalNeurons << std::endl;
        ss << "  Total Synapses: " << currentStats_.totalSynapses << std::endl;
        ss << "  Active Neurons: " << currentStats_.activeNeurons << std::endl;
        ss << "  Firing Neurons: " << currentStats_.firingNeurons << std::endl;
        ss << "  Weight Updates: " << currentStats_.weightUpdates << std::endl;
        ss << "  Average Synaptic Weight: " << currentStats_.avgSynapticWeight << std::endl;
        ss << "  Weight Change Rate: " << currentStats_.weightChangeRate << std::endl;
        ss << "  Working Memory Traces: " << currentStats_.workingMemoryTraces << std::endl;
        ss << "  Episodic Memory Episodes: " << currentStats_.episodicMemoryEpisodes << std::endl;
        ss << "  Dopamine Level: " << currentStats_.dopamineLevel << std::endl;
        ss << "  Curiosity Level: " << currentStats_.curiosityLevel << std::endl;
        ss << "  Novelty Level: " << currentStats_.noveltyLevel << std::endl;
        ss << "  Developmental Stage: " << currentStats_.developmentalStage << std::endl;
        ss << "  Developmental Age: " << currentStats_.developmentalAge << std::endl;
        ss << "  Simulation Steps: " << currentStats_.simulationSteps << std::endl;
        ss << "  Target Spiking Rate: " << currentStats_.targetSpikingRate << std::endl;
        ss << "  Efficiency Score: " << currentStats_.efficiencyScore << std::endl;
        
        return ss.str();
    }
    
    std::string getThresholdViolations() const {
        if (thresholdViolations_.empty()) {
            return "No threshold violations detected.";
        }
        
        std::stringstream ss;
        ss << "Threshold Violations:" << std::endl;
        for (const auto& violation : thresholdViolations_) {
            ss << "  - " << violation << std::endl;
        }
        return ss.str();
    }
    
    void reset() {
        currentStats_ = PerformanceStats();
        history_.clear();
        lastOperationTimes_.clear();
        thresholdViolations_.clear();
        thresholdViolationsReported_ = false;
    }
    
    double getOperationTime(const std::string& operation) const {
        auto it = lastOperationTimes_.find(operation);
        if (it != lastOperationTimes_.end()) {
            return it->second;
        }
        return 0.0;
    }
    
    void recordOperationTime(const std::string& operation, double time) {
        lastOperationTimes_[operation] = time;
    }
    
    std::string getOperationTimesSummary() const {
        if (lastOperationTimes_.empty()) {
            return "No operation times recorded.";
        }
        
        std::stringstream ss;
        ss << "Operation Times:" << std::endl;
        for (const auto& pair : lastOperationTimes_) {
            ss << "  " << pair.first << ": " << pair.second << " s" << std::endl;
        }
        return ss.str();
    }
    
private:
    void checkThresholds(const PerformanceStats& stats) {
        thresholdViolations_.clear();
        thresholdViolationsReported_ = false;
        
        // Check CPU time threshold
        if (cpuMonitoringEnabled_ && stats.cpuTime > thresholds_.maxCpuTime) {
            std::stringstream ss;
            ss << "CPU time exceeded threshold: " << stats.cpuTime << "s > " << thresholds_.maxCpuTime << "s";
            thresholdViolations_.push_back(ss.str());
        }
        
        // Check memory usage threshold
        if (memoryMonitoringEnabled_ && stats.memoryUsage > thresholds_.maxMemoryUsage) {
            std::stringstream ss;
            ss << "Memory usage exceeded threshold: " << stats.memoryUsage << "MB > " << thresholds_.maxMemoryUsage << "MB";
            thresholdViolations_.push_back(ss.str());
        }
        
        // Check spiking rate threshold
        if (neuralMonitoringEnabled_) {
            double firingRate = (stats.simulationSteps > 0) ? 
                static_cast<double>(stats.firingNeurons) / stats.simulationSteps : 0.0;
            if (firingRate < static_cast<double>(thresholds_.minSpikingRate)) {
                std::stringstream ss;
                ss << "Firing rate below threshold: " << firingRate << " < " << thresholds_.minSpikingRate;
                thresholdViolations_.push_back(ss.str());
            }
        }
        
        // Check weight change rate threshold
        if (plasticityMonitoringEnabled_ && std::abs(stats.weightChangeRate) > thresholds_.maxWeightChangeRate) {
            std::stringstream ss;
            ss << "Weight change rate exceeded threshold: " << stats.weightChangeRate << " > " << thresholds_.maxWeightChangeRate;
            thresholdViolations_.push_back(ss.str());
        }
        
        thresholdViolationsReported_ = true;
    }
};

} // namespace nlm

#endif // NLM_PERFORMANCE_H
