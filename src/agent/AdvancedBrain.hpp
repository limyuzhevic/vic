// AdvancedBrain.hpp - Enhanced brain interface with advanced monitoring and analytics
// Provides advanced features for power users and researchers

#pragma once

#include "Brain.hpp"
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <memory>
#include <algorithm>

namespace nlm {

class AdvancedBrain {
public:
    AdvancedBrain(std::shared_ptr<Config> config);
    ~AdvancedBrain();
    
    // Core brain functionality
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    void receiveSensoryInput(const class SensoryInput& input);
    std::unique_ptr<class Action> produceAction();
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Core brain accessors
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    // Advanced monitoring and analytics
    void enableMonitoring(size_t interval = 100);
    bool isMonitoringEnabled() const { return monitoringEnabled_; }
    
    // Advanced statistics and analytics
    std::map<std::string, double> getAdvancedStats() const;
    std::vector<double> getPerformanceData() const { return performanceData_; }
    
    // Learning curve analysis
    std::map<std::string, double> analyzeLearningCurve(const std::vector<double>& rewardHistory) const;
    
    // Advanced experiment capabilities
    bool runAdvancedExperiment(std::shared_ptr<class SimpleWorld> world,
                               std::shared_ptr<class AgentBrain> agent,
                               const std::map<std::string, double>& experimentConfig);
    
    // Development and memory management
    void forceConsolidation();
    void optimizeMemoryUsage();
    void adjustPlasticityParameters();
    
    // Performance optimization
    void enableOptimization(bool enable) { optimizationEnabled_ = enable; }
    bool isOptimizationEnabled() const { return optimizationEnabled_; }
    
    // Custom configuration
    void setConfigValue(const std::string& key, double value);
    double getConfigValue(const std::string& key, double defaultValue = 0.0) const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    std::shared_ptr<Brain> brain_;
    
    bool monitoringEnabled_ = false;
    size_t monitoringInterval_ = 100;
    std::vector<double> performanceData_;
    bool optimizationEnabled_ = false;
    
    // Internal methods
    void updateMonitoringData();
    void analyzePerformanceTrends();
    void optimizeBasedOnData();
    void consolidateMemorySystem();
};

} // namespace nlm