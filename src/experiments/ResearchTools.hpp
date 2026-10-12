#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <chrono>
#include <mutex>
#include <atomic>
#include <functional>
#include <algorithm>
#include <cmath>
#include <variant>
#include <any>
#include <filesystem>
#include "../experiments/Metrics.hpp"
#include "../experiments/ResearchMetrics.hpp"

namespace nlm {

// Research tools for experimental control and analysis
class ResearchTools {
public:
    // Experimental condition types
    struct ExperimentalCondition {
        std::string name;
        std::string description;
        std::map<std::string, double> parameters;
        std::vector<std::string> enabledSystems;
        std::vector<std::string> disabledSystems;
        bool isBaseline = false;
    };

    // Parameter tuning configuration
    struct ParameterTune {
        std::string parameterName;
        double initialValue;
        double minValue;
        double maxValue;
        double stepSize;
        std::function<double(const std::map<std::string, double>&)> performanceFunction;
        bool optimizeForMax = true;
    };

    // Statistical test results
    struct StatisticalTestResult {
        std::string testName;
        std::string testType;
        double statistic;
        double pValue;
        bool isSignificant;
        std::string interpretation;
        std::string recommendation;
    };

    // Data comparison result
    struct DataComparisonResult {
        std::string condition1Name;
        std::string condition2Name;
        std::string metricName;
        double value1;
        double value2;
        double difference;
        double relativeDifference;
        bool isBetter;
        std::string comparisonType;
    };

    // Ablation configuration
    struct AblationConfig {
        std::string systemName;
        bool isAblated;
        std::string ablationMethod;
        std::chrono::system_clock::time_point ablationTime;
        std::chrono::system_clock::time_point restorationTime;
        bool isRestored;
    };

    // Parameter tuning result
    struct ParameterTuningResult {
        std::string parameterName;
        std::vector<double> testedValues;
        std::vector<double> performanceValues;
        double optimalValue;
        double optimalPerformance;
        bool tuningCompleted;
    };

public:
    ResearchTools();
    ~ResearchTools();
    
    // Experimental condition management
    void createExperimentalCondition(const ExperimentalCondition& condition);
    bool runExperimentalCondition(const std::string& conditionName);
    bool restoreExperimentalCondition(const std::string& conditionName);
    std::vector<ExperimentalCondition> getExperimentalConditions() const;
    bool deleteExperimentalCondition(const std::string& conditionName);
    
    // Parameter tuning
    void addParameterTune(const ParameterTune& tune);
    bool runParameterTuning(const std::string& tuneName);
    std::vector<ParameterTuningResult> getParameterTuningResults() const;
    void clearParameterTuningResults();
    
    // Data comparison
    void addComparisonData(const std::string& conditionName, const Metrics& metrics);
    void addComparisonData(const std::string& conditionName, const ResearchMetrics& metrics);
    std::vector<DataComparisonResult> compareConditions(const std::string& condition1Name, const std::string& condition2Name) const;
    std::vector<DataComparisonResult> compareAllConditions() const;
    
    // Statistical testing
    void addStatisticalTest(const StatisticalTestResult& test);
    std::vector<StatisticalTestResult> runStatisticalTests() const;
    std::vector<StatisticalTestResult> getStatisticalTestResults() const;
    bool clearStatisticalTestResults();
    
    // Ablation studies
    void configureAblation(const AblationConfig& config);
    bool enableAblation(const std::string& systemName);
    bool disableAblation(const std::string& systemName);
    std::vector<AblationConfig> getAblationConfigs() const;
    bool restoreAllAblations();
    
    // Data export and import
    bool exportResults(const std::string& filepath) const;
    bool importResults(const std::string& filepath);
    bool exportConditions(const std::string& filepath) const;
    bool importConditions(const std::string& filepath);
    
    // Research utilities
    void setBaselineCondition(const std::string& conditionName);
    std::string getBaselineCondition() const;
    void setCurrentCondition(const std::string& conditionName);
    std::string getCurrentCondition() const;
    
    // Control functions
    void pauseAllSystems(bool pause);
    void enableDebugMode(bool enable);
    void setResearchMode(bool enable);
    bool isResearchMode() const;
    bool isDebugMode() const;
    bool areSystemsPaused() const;
    
    // Metrics integration
    void integrateWithMetrics(const Metrics& metrics);
    void integrateWithResearchMetrics(const ResearchMetrics& metrics);
    std::vector<DataComparisonResult> generatePerformanceReport() const;
    std::vector<DataComparisonResult> generateLearningReport() const;
    
    // Visualization integration
    bool generateVisualizationData(const std::string& outputFilepath) const;
    bool generateStatisticalVisualization(const std::string& outputFilepath) const;
    bool generateComparisonVisualization(const std::string& outputFilepath) const;
    
    // Configuration
    void setDefaultParameters();
    void loadParametersFromFile(const std::string& filepath);
    void saveParametersToFile(const std::string& filepath) const;
    
    // Status and monitoring
    bool isRunning() const;
    std::chrono::system_clock::time_point getSessionStartTime() const;
    double getSessionDuration() const;
    std::vector<std::string> getActiveSystems() const;
    std::map<std::string, double> getCurrentParameters() const;
    
    // Cleanup
    void reset();
    void stop();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
