#pragma once

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "core/Config/Config.hpp"
#include <chrono>
#include <sstream>

namespace nlm {

// Debug Information Structures

struct DebugReport {
    std::string reportId;
    std::chrono::steady_clock::time_point generationTime;
    SimulationStep currentStep;
    std::string brainStateSummary;
    std::vector<std::string> performanceIssues;
    std::vector<std::string> memoryIssues;
    std::vector<std::string> consistencyIssues;
    std::map<std::string, double> metrics;
    std::string recommendations;
    bool hasCriticalIssues;
};

struct ValidationResult {
    bool isValid;
    std::string validationType;
    std::string message;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    SimulationStep step;
    std::chrono::steady_clock::time_point timestamp;
    std::string sourceComponent;
};

class DebuggingTools {
public:
    DebuggingTools();
    ~DebuggingTools();

    // Core debugging functionality
    void integrateWithBrain(Brain* brain);
    void setStep(SimulationStep step) { currentStep_ = step; }
    void updateAnalysisData();

    // Debug reporting
    DebugReport generateDebugReport() const;
    void saveDebugReport(const DebugReport& report, const std::string& filepath) const;
    std::string formatDebugReport(const DebugReport& report) const;

    // State validation
    ValidationResult validateBrainState(const Brain* brain) const;
    ValidationResult validateMemoryConsistency(const Brain* brain) const;
    ValidationResult validateNeuralConnections(const Brain* brain) const;
    ValidationResult validateConfiguration(const Config& config) const;

    // Memory debugging
    void exportMemoryAnalysis(const std::string& filepath) const;

    // Configuration and control
    void setDebugLevel(int level) { debugLevel_ = level; }
    void enableFeature(const std::string& featureName, bool enable);
    bool isFeatureEnabled(const std::string& featureName) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Helper methods
    std::string getTimeString(const std::chrono::steady_clock::time_point& tp) const;
    std::string generateDebugRecommendations(const DebugReport& report) const;

    // Integration state
    Brain* brain_;
    SimulationStep currentStep_;
    std::chrono::steady_clock::time_point startTime_;
    int debugLevel_;
    std::unordered_map<std::string, bool> enabledFeatures_;
};

} // namespace nlm