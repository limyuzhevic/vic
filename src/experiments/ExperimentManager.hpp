// ExperimentManager - Main experiment management system
#pragma once

#include "IExperiment.hpp"
#include "ExperimentResult.hpp"
#include "ExperimentRegistry.hpp"
#include "ExperimentFactory.hpp"
#include "ExperimentScheduler.hpp"
#include "ExperimentComparison.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace nlm {

class ExperimentManager {
public:
    // Singleton pattern
    static ExperimentManager& getInstance() {
        static ExperimentManager instance;
        return instance;
    }
    
    // Configuration
    void setConfig(const std::shared_ptr<Config>& config) {
        this->config = config;
    }
    
    // Initialization
    bool initialize();
    bool shutdown();
    bool isInitialized() const { return initialized; }
    
    // Experiment lifecycle
    bool registerExperiment(const std::string& name, const std::string& description,
                           const std::vector<std::string>& tags = {});
    bool unregisterExperiment(const std::string& name);
    
    // Experiment execution
    ExperimentResult runExperiment(const std::string& name,
                                   const std::map<std::string, double>& parameters = {});
    ExperimentResult runExperiment(const std::string& name,
                                   const ExperimentFactory::ExperimentConfig& config);
    
    // Batch operations
    std::vector<ExperimentResult> runExperimentBatch(const std::vector<std::string>& names,
                                                     bool parallel = false);
    std::vector<ExperimentResult> runExperimentSchedule(const std::vector<ExperimentSchedule>& schedule);
    
    // Experiment management
    bool saveExperiment(const ExperimentResult& result, const std::string& filename = "");
    ExperimentResult loadExperiment(const std::string& filename);
    
    // Results management
    bool addExperimentResult(const ExperimentResult& result);
    bool removeExperimentResult(const std::string& experimentName);
    ExperimentResult getExperimentResult(const std::string& experimentName) const;
    std::vector<ExperimentResult> getAllResults() const { return results; }
    
    // Comparison and analysis
    ComparisonResult compareExperiments(const std::string& name1, const std::string& name2);
    std::vector<ComparisonResult> compareAllExperiments();
    
    // Reporting
    std::string generateReport(const std::string& experimentName = "") const;
    std::string generateSummaryReport() const;
    std::string generateDetailedReport() const;
    
    // Configuration management
    bool loadConfigFromFile(const std::string& filepath);
    bool saveConfigToFile(const std::string& filepath) const;
    
    // Status and monitoring
    double getOverallProgress() const;
    size_t getCompletedExperiments() const;
    size_t getTotalExperiments() const;
    bool isRunning() const { return running; }
    std::chrono::system_clock::time_point getStartTime() const { return startTime; }
    double getElapsedTime() const;
    
    // Experiment discovery
    std::vector<std::string> getAvailableExperiments() const;
    std::vector<std::string> getExperimentsByTag(const std::string& tag) const;
    ExperimentInfo* getExperimentInfo(const std::string& name);
    
    // Utility functions
    static std::string formatTime(std::chrono::system_clock::time_point time);
    static std::string formatDuration(double seconds);
    static std::string sanitizeFilename(const std::string& filename);
    
private:
    ExperimentManager() : initialized(false), running(false), 
                         completedCount(0), totalCount(0) {}
    ~ExperimentManager() = default;
    
    // Helper methods
    std::unique_ptr<IExperiment> createExperimentInstance(const std::string& name,
                                                         const ExperimentFactory::ExperimentConfig& config);
    ExperimentResult convertToResult(const std::unique_ptr<IExperiment>& experiment) const;
    std::string generateUniqueFilename(const std::string& baseName) const;
    
    // Member variables
    bool initialized;
    bool running;
    std::chrono::system_clock::time_point startTime;
    size_t completedCount;
    size_t totalCount;
    
    std::shared_ptr<Config> config;
    std::vector<ExperimentResult> results;
    std::map<std::string, std::unique_ptr<IExperiment>> experimentInstances;
    
    // Components
    ExperimentRegistry registry;
    ExperimentFactory factory;
    ExperimentScheduler scheduler;
    ExperimentComparison comparator;
};

} // namespace nlm
