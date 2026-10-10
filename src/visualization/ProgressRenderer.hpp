#pragma once

#include "Brain.hpp"
#include "ColorManager.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

class ProgressRenderer {
public:
    enum class ProgressViewMode {
        LearningCurve,
        PerformanceMetrics,
        ResourceUsage,
        ComparisonChart,
        ExperimentState,
        PhaseProgress
    };
    
    enum class MetricType {
        None,
        FiringRate,
        LearningSpeed,
        MemoryCapacity,
        CPUUsage,
        MemoryUsage,
        IOOperations,
        ResponseTime
    };
    
    struct ProgressData {
        SimulationStep step;
        Timestamp time;
        float learningProgress;
        float performanceScore;
        float resourceUsage;
        std::map<std::string, float> metrics;
        
        ProgressData() : step(0), time(0.0f), learningProgress(0.0f), 
                        performanceScore(0.0f), resourceUsage(0.0f) {}
        ProgressData(SimulationStep s, Timestamp t, float learning, float performance, float resource)
            : step(s), time(t), learningProgress(learning), performanceScore(performance), resourceUsage(resource) {}
    };
    
    struct ComparisonData {
        std::string experimentName;
        std::string configuration;
        float learningCurveScore;
        float performanceScore;
        float efficiencyScore;
        float resourceEfficiency;
        
        ComparisonData() : learningCurveScore(0.0f), performanceScore(0.0f), 
                          efficiencyScore(0.0f), resourceEfficiency(0.0f) {}
        ComparisonData(const std::string& name, const std::string& config)
            : experimentName(name), configuration(config) {}
    };
    
    ProgressRenderer();
    ~ProgressRenderer() = default;
    
    void setProgressViewMode(ProgressViewMode mode) { progressViewMode_ = mode; }
    void setMetricType(MetricType metric) { metricType_ = metric; }
    void setComparisonData(const std::vector<ComparisonData>& data) { comparisonData_ = data; }
    void setShowProgressHistory(bool show) { showProgressHistory_ = show; }
    void setShowResourceMetrics(bool show) { showResourceMetrics_ = show; }
    void setUpdateInterval(float seconds) { updateInterval_ = seconds; }
    
    void render(const Brain& brain);
    void update(const Brain& brain);
    
    // Learning curve visualization
    void renderLearningCurve(const Brain& brain);
    void renderLearningProgress(const Brain& brain);
    void renderSpeedOfLearning(const Brain& brain);
    
    // Performance metrics visualization
    void renderPerformanceMetrics(const Brain& brain);
    void renderEfficiencyMetrics(const Brain& brain);
    void renderQualityMetrics(const Brain& brain);
    
    // Resource usage visualization
    void renderResourceUsage(const Brain& brain);
    void renderMemoryUsage(const Brain& brain);
    void renderCPUUsage(const Brain& brain);
    
    // Comparison charts
    void renderComparisonChart(const Brain& brain);
    void renderExperimentComparison(const Brain& brain);
    void renderConfigurationComparison(const Brain& brain);
    
    // Experiment state visualization
    void renderExperimentState(const Brain& brain);
    void renderPhaseProgress(const Brain& brain);
    void renderStepProgress(const Brain& brain);
    
    // Interactive features
    void setTimeRange(float startTime, float endTime) { timeRangeStart_ = startTime; timeRangeEnd_ = endTime; }
    void setMetricRange(float min, float max) { metricMin_ = min; metricMax_ = max; }
    void setSelectedExperiment(const std::string& experiment) { selectedExperiment_ = experiment; }
    
    // Data management
    void collectProgressData(const Brain& brain);
    void updateProgressStatistics();
    void updateLearningCurve();
    
    // Export and analysis
    std::string exportProgressData() const;
    void generateProgressReport() const;
    
    // Configuration
    void loadConfiguration(const std::string& configFile);
    void saveConfiguration() const;
    
    // Utility functions
    float getCurrentProgress() const { return currentProgress_; }
    size_t getProgressHistorySize() const { return progressHistory_.size(); }
    float getAveragePerformance() const { return averagePerformance_; }
    
private:
    ProgressViewMode progressViewMode_;
    MetricType metricType_;
    bool showProgressHistory_;
    bool showResourceMetrics_;
    float updateInterval_;
    
    std::vector<ProgressData> progressHistory_;
    std::vector<float> learningCurve_;
    std::vector<float> performanceCurve_;
    std::vector<float> resourceUsage_;
    
    // Comparison data
    std::vector<ComparisonData> comparisonData_;
    
    // Statistics
    float currentProgress_;
    float averagePerformance_;
    float totalLearning_;
    float efficiencyScore_;
    
    // Interactive state
    float timeRangeStart_;
    float timeRangeEnd_;
    float metricMin_;
    float metricMax_;
    std::string selectedExperiment_;
    
    // Configuration
    size_t maxProgressHistory_;
    bool smoothProgress_;
    float smoothingFactor_;
    std::string colorScheme_;
    
    void initializeDefaultConfiguration();
    void calculateProgressStatistics();
    void updateLearningCurve();
    std::string getMetricColor(float value, MetricType metric) const;
    std::string getProgressColor(float progress) const;
    std::string getPerformanceColor(float performance) const;
    
    // Rendering helpers
    void renderProgressBar(float progress, int x, int y, int width, int height);
    void renderLearningCurveChart(const std::vector<float>& curve, int x, int y, int width, int height);
    void renderMetricGauge(float value, int x, int y, int width, int height, MetricType metric);
    void renderComparisonGraph(const std::vector<ComparisonData>& data, int x, int y, int width, int height);
    void renderResourceGraph(const std::vector<float>& usage, int x, int y, int width, int height);
    void renderExperimentStatus(const Brain& brain, int x, int y);
    
    // Analysis functions
    void analyzeLearningProgress(const Brain& brain);
    void analyzePerformance(const Brain& brain);
    void analyzeEfficiency(const Brain& brain);
    void analyzeResources(const Brain& brain);
    
    // Utility functions
    float calculateLearningRate(const std::vector<float>& curve) const;
    float calculatePerformanceScore(const std::vector<float>& metrics) const;
    float calculateEfficiency(float performance, float resources) const;
    std::string formatMetricValue(float value, MetricType metric) const;
};

} // namespace nlm
