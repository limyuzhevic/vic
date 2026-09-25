#pragma once

#include "EnhancedExperiment.hpp"
#include "EnhancedMetrics.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>

namespace nlm {

// Experiment validation framework
class ExperimentValidation {
public:
    ExperimentValidation();
    ~ExperimentValidation();
    
    // Validation functions
    bool validateExperiment(const std::shared_ptr<EnhancedExperiment>& experiment);
    bool validateIntegration(const std::shared_ptr<EnhancedExperiment>& experiment);
    bool validatePerformance(const std::shared_ptr<EnhancedExperiment>& experiment);
    bool validateResults(const std::shared_ptr<EnhancedExperiment>& experiment);
    
    // Validation categories
    struct ValidationResult {
        std::string category;
        bool passed;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        double confidenceScore;
    };
    
    std::vector<ValidationResult> validate(const std::shared_ptr<EnhancedExperiment>& experiment);
    
    // Validation rules
    struct ValidationRule {
        std::string name;
        std::string category;
        std::function<bool(const std::shared_ptr<EnhancedExperiment>&)> check;
        std::vector<std::string> errorMessages;
        double weight;
    };
    
    void addValidationRule(const ValidationRule& rule);
    void removeValidationRule(const std::string& name);
    void clearValidationRules();
    
    // Validation configurations
    void setStrictMode(bool strict);
    void setAutoFix(bool autoFix);
    void setConfidenceThreshold(double threshold);
    
    // Validation reports
    std::string generateValidationReport(const std::shared_ptr<EnhancedExperiment>& experiment) const;
    std::string generateValidationJSON(const std::shared_ptr<EnhancedExperiment>& experiment) const;
    std::string generateValidationCSV(const std::shared_ptr<EnhancedExperiment>& experiment) const;
    
    // Validation statistics
    double getOverallScore(const std::shared_ptr<EnhancedExperiment>& experiment) const;
    std::vector<std::string> getPassedCategories(const std::shared_ptr<EnhancedExperiment>& experiment) const;
    std::vector<std::string> getFailedCategories(const std::shared_ptr<EnhancedExperiment>& experiment) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Multi-agent experiment support
class MultiAgentRunner {
public:
    MultiAgentRunner();
    ~MultiAgentRunner();
    
    // Agent management
    bool addAgent(const std::shared_ptr<EnhancedExperiment>& agent);
    bool removeAgent(const std::string& agentName);
    bool clearAgents();
    
    // Multi-agent execution
    std::vector<std::shared_ptr<EnhancedExperiment>> runMultiAgent(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents,
                                                                  const std::unordered_map<std::string, std::string>& sharedParams);
    std::vector<std::shared_ptr<EnhancedExperiment>> runSocialLearning(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents);
    std::vector<std::shared_ptr<EnhancedExperiment>> runCompetitive(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents);
    
    // Agent interaction and social learning
    bool simulateAgentInteraction(const std::shared_ptr<EnhancedExperiment>& agent1,
                                 const std::shared_ptr<EnhancedExperiment>& agent2,
                                 const std::unordered_map<std::string, std::string>& interactionParams);
    bool simulateSocialLearning(const std::shared_ptr<EnhancedExperiment>& observer,
                               const std::shared_ptr<EnhancedExperiment>& model);
    bool simulateCooperativeBehavior(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents);
    
    // Multi-agent metrics
    struct MultiAgentMetrics {
        double averagePerformance;
        double performanceVariance;
        double socialLearningEffect;
        double competitivePressure;
        double cooperationLevel;
        double culturalSpread;
        double knowledgeTransfer;
    };
    
    MultiAgentMetrics computeMultiAgentMetrics(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents);
    
    // Agent management utilities
    std::vector<std::string> getAgentNames() const;
    std::shared_ptr<EnhancedExperiment> getAgent(const std::string& name) const;
    bool hasAgent(const std::string& name) const;
    
    // Configuration
    void setAgentInteractionModel(const std::string& model);
    void setSocialLearningRules(const std::unordered_map<std::string, double>& rules);
    void setCompetitiveParameters(const std::unordered_map<std::string, double>& params);
    
    // Reports and analysis
    std::string generateMultiAgentReport(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents) const;
    std::string generateSocialLearningReport(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents) const;
    std::string generateCompetitiveReport(const std::vector<std::shared_ptr<EnhancedExperiment>>& agents) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Parameter tuning system
class ParameterTuner {
public:
    ParameterTuner();
    ~ParameterTuner();
    
    // Parameter tuning methods
    std::unordered_map<std::string, std::string> optimizeParameters(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                     const std::unordered_map<std::string, std::string>& initialParams);
    std::unordered_map<std::string, std::string> bayesianOptimization(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                       const std::unordered_map<std::string, std::string>& initialParams);
    std::unordered_map<std::string, std::string> evolutionaryOptimization(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                             const std::unordered_map<std::string, std::string>& initialParams);
    std::unordered_map<std::string, std::string> gridSearch(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                            const std::unordered_map<std::string, std::vector<std::string>>& parameterGrid);
    
    // Result-based parameter adjustment
    std::unordered_map<std::string, std::string> adaptParameters(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                  const std::unordered_map<std::string, std::string>& currentParams,
                                                                  const std::string& adaptationStrategy);
    
    // Parameter validation and ranges
    bool validateParameters(const std::unordered_map<std::string, std::string>& params,
                           const std::shared_ptr<EnhancedExperiment>& experiment);
    std::vector<std::string> getParameterRanges(const std::string& parameterName,
                                               const std::shared_ptr<EnhancedExperiment>& experiment);
    
    // Parameter suggestion and recommendations
    std::unordered_map<std::string, std::string> suggestParameters(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                                    const std::string& experimentType);
    std::vector<std::string> getParameterRecommendations(const std::string& experimentType);
    
    // Configuration management
    void setOptimizationMode(const std::string& mode);
    void setOptimizationIterations(size_t iterations);
    void setPopulationSize(size_t size);
    void setMutationRate(double rate);
    void setCrossoverRate(double rate);
    
    // Reports and analysis
    std::string generateOptimizationReport(const std::unordered_map<std::string, std::string>& bestParams,
                                           const std::vector<double>& fitnessScores) const;
    std::string generateParameterSensitivityReport(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                   const std::unordered_map<std::string, std::string>& params) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Experiment runner with enhanced capabilities
class EnhancedExperimentRunner {
public:
    EnhancedExperimentRunner();
    ~EnhancedExperimentRunner();
    
    // Core functionality
    std::shared_ptr<EnhancedExperiment> createExperiment(const std::string& name, const std::string& templateName);
    bool configureExperiment(std::shared_ptr<EnhancedExperiment> experiment,
                            const std::unordered_map<std::string, std::string>& parameters);
    bool runExperiment(std::shared_ptr<EnhancedExperiment> experiment);
    
    // Advanced features
    std::vector<std::shared_ptr<EnhancedExperiment>> runBatch(const std::vector<std::shared_ptr<EnhancedExperiment>>& experiments);
    std::shared_ptr<EnhancedExperiment> runMultiAgentExperiment(const std::vector<std::string>& templateNames,
                                                              const std::unordered_map<std::string, std::string>& globalParams);
    
    // Results management
    bool saveResults(const std::string& filepath, const std::vector<std::shared_ptr<EnhancedExperiment>>& experiments);
    std::vector<std::shared_ptr<EnhancedExperiment>> loadResults(const std::string& filepath);
    
    // Analysis and reporting
    std::string generateComprehensiveReport(const std::vector<std::shared_ptr<EnhancedExperiment>>& experiments) const;
    std::string generateMetaAnalysis(const std::vector<std::shared_ptr<EnhancedExperiment>>& experiments) const;
    
    // Validation and tuning
    bool validateExperiment(const std::shared_ptr<EnhancedExperiment>& experiment);
    std::unordered_map<std::string, std::string> tuneParameters(const std::shared_ptr<EnhancedExperiment>& experiment,
                                                              const std::string& tuningMethod);
    
    // Experiment management
    void clearExperiments();
    size_t getExperimentCount() const;
    std::vector<std::string> getExperimentNames() const;
    
    // Configuration
    void setValidationFramework(const std::shared_ptr<ExperimentValidation>& validation);
    void setMultiAgentRunner(const std::shared_ptr<MultiAgentRunner>& multiAgentRunner);
    void setParameterTuner(const std::shared_ptr<ParameterTuner>& parameterTuner);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
