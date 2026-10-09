#pragma once

#include "cli/CLI.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>

namespace nlm {

class DebugCLI {
public:
    DebugCLI();
    ~DebugCLI();
    
    // Disable copying, enable moving
    DebugCLI(const DebugCLI&) = delete;
    DebugCLI& operator=(const DebugCLI&) = delete;
    DebugCLI(DebugCLI&&) noexcept;
    DebugCLI& operator=(DebugCLI&&) noexcept;
    
    // Initialization
    bool initialize();
    
    // Debug commands
    bool verboseCommand(const std::vector<std::string>& args);
    bool stepCommand(const std::vector<std::string>& args);
    bool profileCommand(const std::vector<std::string>& args);
    bool memoryCommand(const std::vector<std::string>& args);
    bool spikeCommand(const std::vector<std::string>& args);
    bool statsCommand(const std::vector<std::string>& args);
    bool logCommand(const std::vector<std::string>& args);
    bool stackCommand(const std::vector<std::string>& args);
    bool threadCommand(const std::vector<std::string>& args);
    bool measureCommand(const std::vector<std::string>& args);
    
    // Debug modes
    bool stepByStepCommand(const std::vector<std::string>& args);
    bool continuousModeCommand(const std::vector<std::string>& args);
    bool pauseCommand(const std::vector<std::string>& args);
    bool resumeCommand(const std::vector<std::string>& args);
    bool stopCommand(const std::vector<std::string>& args);
    
    // Performance monitoring
    bool performanceCommand(const std::vector<std::string>& args);
    bool cpuCommand(const std::vector<std::string>& args);
    bool memoryUsageCommand(const std::vector<std::string>& args);
    bool timingCommand(const std::vector<std::string>& args);
    bool spikeStatsCommand(const std::vector<std::string>& args);
    bool learningStatsCommand(const std::vector<std::string>& args);
    
    // Advanced debugging
    bool analyzeConnectionsCommand(const std::vector<std::string>& args);
    bool analyzeWeightsCommand(const std::vector<std::string>& args);
    bool analyzePlasticityCommand(const std::vector<std::string>& args);
    bool analyzeSpikingPatternsCommand(const std::vector<std::string>& args);
    bool analyzeSynapticDynamicsCommand(const std::vector<std::string>& args);
    
    // Help and documentation
    void showHelp() const;
    void showCommandHelp(const std::string& command) const;
    
    // Integration
    void setConfig(std::shared_ptr<Config> config) { this->config = config; }
    void setBrain(std::shared_ptr<Brain> brain) { this->brain = brain; }
    void setCLI(CLI* cli) { this->cli = cli; }
    
    // State
    bool isInitialized() const { return initialized; }
    
    // Debug state management
    void startStepByStep() { stepByStepMode = true; }
    void stopStepByStep() { stepByStepMode = false; }
    bool isInStepByStepMode() const { return stepByStepMode; }
    void pause() { paused = true; }
    void resume() { paused = false; }
    bool isPaused() const { return paused; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool initialized;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    CLI* cli;
    
    // Debug mode state
    bool stepByStepMode;
    bool paused;
    int verboseLevel;
    std::chrono::steady_clock::time_point stepStartTime;
    
    // Performance monitoring
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double peakMemoryUsage;
        double totalTime;
        size_t spikeCount;
        size_t neuronCount;
        size_t synapseCount;
        std::chrono::steady_clock::time_point lastUpdate;
    } performanceMetrics;
    
    // Helper methods
    void updatePerformanceMetrics();
    double calculateCPULoad() const;
    double calculateMemoryUsage() const;
    void logPerformanceMetrics() const;
    void logSpikeStatistics() const;
    void logLearningStatistics() const;
    void analyzeSynapticConnections() const;
    void analyzeSynapticWeights() const;
    void analyzePlasticityDynamics() const;
    void analyzeSpikingActivity() const;
    void analyzeSynapticDelays() const;
    bool runStepByStep(CLI* cli, const std::vector<std::string>& args);
    std::string formatPerformanceMetrics() const;
    std::string formatSpikeStats() const;
    std::string formatLearningStats() const;
    std::string formatMemoryStats() const;
    std::string formatTimingStats() const;
    void printDebugHeader(const std::string& title) const;
    void printPerformanceHeader() const;
    void printSpikeHeader() const;
    void printLearningHeader() const;
    void printMemoryHeader() const;
    void printTimingHeader() const;
    bool validateDebugParameters(const std::vector<std::string>& args) const;
    void setupDebugConfiguration() const;
    std::string getFormattedTime() const;
    std::string getMemoryUsageString() const;
    std::string getCPULoadString() const;
};
