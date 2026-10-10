#pragma once

#include "core/Types/Types.hpp"
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <memory>

namespace nlm {

// Neuromodulator signal for monitoring
struct NeuromodulatorSignal {
    std::string name; // "dopamine", "curiosity", "novelty", etc.
    double level;     // Signal strength/level
    Timestamp timestamp;
    Timestamp duration; // How long the signal lasts
    std::string source; // What triggered the signal
    double effect;   // Resulting effect on neural activity
};

// Neuromodulation state
struct NeuromodulationState {
    double dopamine;
    double acetylcholine;
    double norepinephrine;
    double serotonin;
    double curiosity;
    double novelty;
    double predictionError;
    Timestamp lastUpdate;
};

// NeuromodulationMonitor - Neuromodulation monitoring class
class NeuromodulationMonitor {
public:
    NeuromodulationMonitor();
    ~NeuromodulationMonitor();
    
    // Start/stop monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Signal monitoring
    void recordNeuromodulatorSignal(const std::string& name, double level, Timestamp timestamp);
    void recordNeuromodulatorSignal(const std::string& name, double level, Timestamp timestamp, Timestamp duration);
    void recordNeuromodulatorSignal(const std::string& name, double level, Timestamp timestamp, Timestamp duration, const std::string& source);
    
    // State management
    void updateState(const NeuromodulationState& state);
    NeuromodulationState getCurrentState() const;
    
    // Signal analysis
    double getDopamineLevel() const;
    double getCuriosityLevel() const;
    double getNoveltyLevel() const;
    double getPredictionError() const;
    double getAcetylcholineLevel() const;
    double getNorepinephrineLevel() const;
    double getSerotoninLevel() const;
    
    // Signal history
    std::vector<NeuromodulatorSignal> getSignalHistory(const std::string& name, size_t limit = 100) const;
    std::vector<NeuromodulatorSignal> getSignalHistoryByTimeRange(const std::string& name, 
                                                                   Timestamp startTime, 
                                                                   Timestamp endTime) const;
    
    // Signal analysis
    double analyzeSignalPatterns(const std::string& name) const;
    double calculateSignalMean(const std::string& name, Timestamp startTime, Timestamp endTime) const;
    double calculateSignalVariance(const std::string& name, Timestamp startTime, Timestamp endTime) const;
    double calculateSignalPeak(const std::string& name, Timestamp startTime, Timestamp endTime) const;
    
    // Effect analysis
    double analyzeSignalEffect(const std::string& name, const std::string& target) const;
    std::map<std::string, double> analyzeAllEffects() const;
    
    // Performance metrics
    double getSignalToNoiseRatio(const std::string& name) const;
    double getSignalTimingConsistency(const std::string& name) const;
    double getSignalDuration(const std::string& name) const;
    
    // Signal quality
    bool isSignalValid(const std::string& name) const;
    std::vector<std::string> getValidSignals() const;
    bool isSignalWithinRange(const std::string& name, double level) const;
    
    // Recording control
    void clearSignalHistory();
    void setSignalRetentionTime(Timestamp retentionTime);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm