// Progress monitoring system for NLM simulations
#pragma once

#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>

namespace nlm {

class ProgressTracker {
public:
    struct ProgressMetrics {
        uint64_t currentStep;
        uint64_t maxSteps;
        uint64_t currentNeuronCount;
        uint64_t maxNeuronCount;
        float currentFiringRate;
        float avgFiringRate;
        float currentReward;
        float totalReward;
        size_t activeMemoryTraces;
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point stepStartTime;
        std::string currentPhase;
        std::vector<std::string> statusMessages;
    };
    
    ProgressTracker(uint64_t maxSteps, uint64_t maxNeurons,
                   std::ostream& output = std::cout);
    
    void setMaxSteps(uint64_t maxSteps);
    void setMaxNeurons(uint64_t maxNeurons);
    void updateStep(uint64_t step);
    void updateMetrics(const ProgressMetrics& metrics);
    void setPhase(const std::string& phase);
    void addStatusMessage(const std::string& message);
    void logProgress(bool force = false);
    
    float getProgressPercentage() const;
    double getElapsedTime() const;
    double getEstimatedTimeRemaining() const;
    double getAverageStepsPerSecond() const;
    
    const ProgressMetrics& getCurrentMetrics() const;
    
    // Progress bar display modes
    enum class DisplayMode {
        Simple,
        Detailed,
        Compact,
        ProgressOnly,
        MetricsOnly
    };
    
    void setDisplayMode(DisplayMode mode);
    void enableRealTimeUpdates(bool enable);
    void setUpdateInterval(uint64_t steps);
    
private:
    void updateProgressBar(const ProgressMetrics& metrics);
    void formatTime(std::stringstream& ss, double seconds) const;
    std::string formatNumber(uint64_t number) const;
    std::string truncateString(const std::string& str, size_t maxLength) const;
    
    ProgressMetrics currentMetrics_;
    uint64_t maxSteps_;
    uint64_t maxNeurons_;
    std::ostream& output_;
    DisplayMode displayMode_;
    bool realTimeUpdates_;
    uint64_t updateInterval_;
    uint64_t lastUpdateStep_;
    size_t maxStatusMessages_;
    bool initialized_;
};

} // namespace nlm
