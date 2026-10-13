// ExperimentManager class header
// This file provides the header declaration for the ExperimentManager class

#ifndef EXPERIMENT_MANAGER_HPP
#define EXPERIMENT_MANAGER_HPP

#include <string>
#include <vector>
#include <map>
#include <functional>

// NLM includes
#include "../src/advanced_api.hpp"

/**
 * @brief Comprehensive experiment management
 */
class ExperimentManager {
private:
    std::vector<ExperimentResult> results_;
    StringMap experimentConfig_;
    std::string resultsDirectory_;
    size_t experimentId_;
    
public:
    ExperimentManager();
    
    ~ExperimentManager() = default;
    
    // Experiment setup
    void configure(const StringMap& config);
    StringMap getConfig() const;
    
    // Experiment execution
    ExperimentResult runExperiment(const std::string& name,
                                   std::function<void(std::shared_ptr<SimpleAgent>)> setup,
                                   std::function<void(std::shared_ptr<SimpleAgent>, float)> step,
                                   std::function<double(std::shared_ptr<SimpleAgent>)> evaluate);
    std::vector<ExperimentResult> runBatch(size_t numExperiments);
    
    // Results management
    void addResult(const ExperimentResult& result);
    std::vector<ExperimentResult> getResults() const;
    ExperimentResult getResult(size_t index) const;
    
    // Analysis and comparison
    std::map<std::string, double> compareExperiments(size_t experiment1, size_t experiment2) const;
    std::map<std::string, FloatArray> analyzeAcrossExperiments(const std::string& metric) const;
    
    // Export and reporting
    void exportAllResults(const std::string& directory = ".");
    void generateReport(const std::string& filename = "experiment_report") const;
    
    // Statistics
    double getMeanPerformance(size_t experimentIndex) const;
    double getVariancePerformance(size_t experimentIndex) const;
    std::map<std::string, double> getGlobalStatistics() const;
};

#endif // EXPERIMENT_MANAGER_HPP
