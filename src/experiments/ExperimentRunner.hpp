#pragma once

#include "Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Enhanced experiment result structure
struct ExperimentResult {
    std::string experimentName;
    uint64_t seed;
    SimulationStep startStep;
    SimulationStep endStep;
    double duration;
    
    // Neural metrics
    size_t totalNeurons;
    size_t totalSynapses;
    size_t maxFiringNeurons;
    float minFiringRate;
    float maxFiringRate;
    float avgFiringRate;
    
    // Performance metrics
    double totalTime;
    size_t totalSteps;
    size_t successfulSteps;
    
    // Plasticity metrics
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    float weightChangeMean;
    float weightChangeStd;
    size_t weightsStrengthened;
    size_t weightsWeakened;
    size_t weightsUnchanged;
    
    // Learning detection
    bool learningDetected;
    float learningMagnitude;
    
    // Spike activity
    size_t totalSpikes;
    size_t maxSpikesPerStep;
    float spikeRate;
    
    // Success metrics
    bool success;
    std::string errorMessage;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
    // Export functionality
    std::string toJSON() const;
    std::string toCSV() const;
    std::string toTextReport() const;
    
    // Analysis methods
    double getCoefficientOfVariation() const;
    bool isWithinNormalRange(float threshold) const;
    std::string getPerformanceGrade() const;
};

// Experiment comparison results
struct ExperimentComparison {
    std::vector<std::string> experimentNames;
    std::map<std::string, double> metricValues;
    std::map<std::string, double> metricRanks;
    std::string bestExperiment;
    std::string worstExperiment;
    std::string getComparisonReport() const;
};

// ExperimentRunner: Runs experiments and collects comprehensive results
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // Create new experiment
    std::shared_ptr<Experiment> createExperiment(const std::string& name);
    
    // Run single experiment and collect results
    ExperimentResult runExperimentAndCollectResults(
        std::shared_ptr<Experiment> experiment,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<Environment> environment,
        SimulationStep maxSteps);
    
    // Run batch of experiments and collect results
    std::vector<ExperimentResult> runBatchAndCollectResults(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps);
    
    // Get all experiments
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;
    
    // Get experiment by name
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    
    // Clear all experiments
    void clearExperiments();
    
    // Set random seed for reproducibility
    void setGlobalSeed(uint64_t seed);
    
    // Save experiment results to various formats
    bool saveResults(const std::string& filepath, const std::vector<ExperimentResult>& results) const;
    bool saveResultsJSON(const std::string& filepath, const std::vector<ExperimentResult>& results) const;
    bool saveResultsCSV(const std::string& filepath, const std::vector<ExperimentResult>& results) const;
    bool saveResultsText(const std::string& filepath, const std::vector<ExperimentResult>& results) const;
    
    // Load experiment results from file
    std::vector<ExperimentResult> loadResults(const std::string& filepath) const;
    
    // Enhanced experiment analysis
    ExperimentResult analyzeExperiment(std::shared_ptr<Experiment> experiment) const;
    ExperimentComparison compareExperiments(const std::vector<std::shared_ptr<Experiment>>& experiments) const;
    
    // Statistical analysis
    double calculateMean(const std::vector<double>& values) const;
    double calculateStd(const std::vector<double>& values, double mean) const;
    double calculateCoefficientOfVariation(const std::vector<double>& values) const;
    std::vector<double> normalizeMetrics(const std::map<std::string, double>& metrics) const;
    
    // Performance analysis
    double calculatePerformanceScore(const ExperimentResult& result) const;
    std::string assessExperimentQuality(const ExperimentResult& result) const;
    
    // Result validation
    bool validateExperimentResult(const ExperimentResult& result) const;
    std::vector<std::string> getValidationErrors(const ExperimentResult& result) const;
    
    // Export functionality
    std::string generateComprehensiveReport(const std::vector<ExperimentResult>& results) const;
    std::string generateStatisticalReport(const std::vector<ExperimentResult>& results) const;
    std::string generateComparisonReport(const ExperimentComparison& comparison) const;
    
    // Get experiment statistics
    size_t getTotalExperiments() const;
    double getAveragePerformance(const std::vector<ExperimentResult>& results) const;
    size_t getSuccessfulExperiments(const std::vector<ExperimentResult>& results) const;
    
    // Advanced filtering and querying
    std::vector<ExperimentResult> filterResultsByPerformance(
        const std::vector<ExperimentResult>& results,
        double minScore,
        double maxScore) const;
    
    std::vector<ExperimentResult> filterResultsByLearning(
        const std::vector<ExperimentResult>& results,
        bool learningDetected,
        float minLearningMagnitude) const;
    
    std::vector<ExperimentResult> filterResultsByDomain(
        const std::vector<ExperimentResult>& results,
        const std::string& domain) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
