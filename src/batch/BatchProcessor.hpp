// BatchProcessor header - contains declarations for the batch processing system
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <queue>

using namespace std;

namespace nlm {

// Forward declarations
class Brain;

// Batch processor for running multiple experiments
class BatchProcessor {
public:
    BatchProcessor();
    ~BatchProcessor();
    
    // Initialize batch processor
    bool initialize(const string& configFile = "");
    
    // Experiment management
    bool addExperiment(const string& name, map<string, string> parameters);
    bool removeExperiment(const string& name);
    bool modifyExperiment(const string& name, const string& parameter, const string& value);
    
    // Batch execution
    bool runBatch(const vector<string>& experimentNames);
    bool runParameterSweep(const string& baseExperiment, 
                          const map<string, vector<string>>& parameters);
    bool runMonteCarloSimulation(size_t iterations, const string& experimentName);
    bool runEvolutionaryOptimization(const string& baseExperiment, 
                                     size_t generations, size_t populationSize);
    
    // Results management
    struct ExperimentResult {
        string experimentName;
        double startTime;
        double endTime;
        double duration;
        map<string, double> metrics;
        vector<string> logs;
        map<string, string> parameters;
        bool success;
        string errorMessage;
    };
    
    vector<ExperimentResult> getResults(const string& experimentName = "") const;
    ExperimentResult getLatestResult(const string& experimentName) const;
    
    // Results analysis
    string analyzeResults(const string& experimentName) const;
    string compareExperiments(const vector<string>& experimentNames) const;
    bool exportResults(const string& experimentName, const string& filepath, 
                      const string& format = "csv");
    
    // Configuration and settings
    void setOutputDirectory(const string& dir);
    void setResultFormat(const string& format);
    void setParallelExecution(bool enable);
    void setMaxParallelExperiments(size_t max);
    void enableAutoSave(bool enable);
    
    // Monitoring and control
    bool pauseExecution();
    bool resumeExecution();
    bool cancelExecution();
    bool isExecuting() const;
    string getExecutionStatus() const;
    
    // Progress tracking
    double getExecutionProgress(const string& experimentName) const;
    string getCurrentExperiment() const;
    string getEstimatedTimeRemaining(const string& experimentName) const;
    
private:
    struct Impl;
    unique_ptr<Impl> pImpl;
};

} // namespace nlm
