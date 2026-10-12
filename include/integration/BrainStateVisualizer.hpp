#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>

namespace nlm {

// Forward declarations
class Brain;
class ExperimentManager;
class AlertManager;

// Base integration component
class IntegrationComponent {
public:
    virtual ~IntegrationComponent() = default;
    virtual bool initialize() = 0;
    virtual void update() = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
};

// Brain state visualization - Real-time visualization of brain state changes
class BrainStateVisualizer : public IntegrationComponent {
public:
    BrainStateVisualizer();
    ~BrainStateVisualizer() override = default;
    
    // Initialize with brain reference
    bool initialize() override;
    
    // Set brain to visualize
    void setBrain(std::shared_ptr<Brain> brain);
    
    // Update brain state visualization
    void update() override;
    
    // Shutdown visualization
    void shutdown() override;
    
    // Check if initialized
    bool isInitialized() const override;
    
    // State visualization controls
    void setShowNeuronStates(bool show);
    void setShowSynapseStates(bool show);
    void setShowRegionStates(bool show);
    void setShowMemoryStates(bool show);
    void setShowCognitionStates(bool show);
    void setShowNeuromodulationStates(bool show);
    
    // Time series
    void setTimeWindow(double window);
    void setHistoricalDataSize(size_t size);
    
    // Get current brain state data
    std::vector<float> getNeuronStateData() const;
    std::vector<float> getSynapseStateData() const;
    std::vector<float> getRegionStateData() const;
    std::vector<float> getMemoryStateData() const;
    std::vector<float> getCognitionStateData() const;
    std::vector<float> getNeuromodulationStateData() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Experiment progress monitor - Monitor experiment execution progress
class ExperimentProgressMonitor : public IntegrationComponent {
public:
    ExperimentProgressMonitor();
    ~ExperimentProgressMonitor() override = default;
    
    // Initialize with experiment manager
    bool initialize() override;
    
    // Set experiment manager
    void setExperimentManager(std::shared_ptr<ExperimentManager> manager);
    
    // Update experiment progress
    void update() override;
    
    // Shutdown monitoring
    void shutdown() override;
    
    // Check if initialized
    bool isInitialized() const override;
    
    // Progress controls
    void setAutoUpdate(bool autoUpdate);
    void setUpdateInterval(double interval);
    void setShowDetailedProgress(bool show);
    
    // Progress information
    double getProgress() const;
    std::string getCurrentPhase() const;
    std::string getCurrentStage() const;
    std::string getCurrentExperiment() const;
    std::vector<std::string> getPendingExperiments() const;
    
    // Experiment management
    void startExperiment(const std::string& name);
    void pauseExperiment(const std::string& name);
    void stopExperiment(const std::string& name);
    void skipExperiment(const std::string& name);
    
    // Results
    bool hasResults() const;
    std::vector<std::string> getResultFiles() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Result visualizer - Visualize experiment results and conclusions
class ResultVisualizer : public IntegrationComponent {
public:
    ResultVisualizer();
    ~ResultVisualizer() override = default;
    
    // Initialize result visualizer
    bool initialize() override;
    
    // Set results file
    void setResultsFile(const std::string& filename);
    
    // Load results
    bool loadResults();
    
    // Update result visualization
    void update() override;
    
    // Shutdown visualization
    void shutdown() override;
    
    // Check if initialized
    bool isInitialized() const override;
    
    // Visualization controls
    void setChartType(const std::string& type);
    void setAnalysisType(const std::string& type);
    void setComparisonMode(bool enabled);
    void setStatisticalAnalysis(bool enable);
    
    // Result display
    void setShowRawData(bool show);
    void setShowStatisticalData(bool show);
    void setShowErrorBars(bool show);
    void setShowConfidenceIntervals(bool show);
    
    // Export
    bool exportResults(const std::string& filename);
    bool exportVisualization(const std::string& filename);
    
    // Get result data
    std::vector<float> getMetricData(const std::string& metric) const;
    std::vector<std::vector<float>> getResultsMatrix() const;
    std::vector<std::string> getMetricNames() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Comparison visualizer - Compare results across different experiments
class ComparisonVisualizer : public IntegrationComponent {
public:
    ComparisonVisualizer();
    ~ComparisonVisualizer() override = default;
    
    // Initialize comparison visualizer
    bool initialize() override;
    
    // Add experiment for comparison
    void addExperiment(const std::string& name, const std::string& resultsFile);
    void removeExperiment(const std::string& name);
    void clearExperiments();
    
    // Update comparison visualization
    void update() override;
    
    // Shutdown comparison
    void shutdown() override;
    
    // Check if initialized
    bool isInitialized() const override;
    
    // Comparison controls
    void setComparisonMetric(const std::string& metric);
    void setNormalizationMethod(const std::string& method);
    void setStatisticalTest(const std::string& test);
    void setSignificanceLevel(double level);
    
    // Visualization options
    void setShowRawData(bool show);
    void setShowStatisticalData(bool show);
    void setShowSignificance(bool show);
    void setShowEffectSizes(bool show);
    
    // Export comparison results
    bool exportComparison(const std::string& filename);
    bool generateReport(const std::string& filename);
    
    // Get comparison data
    std::vector<std::string> getExperimentNames() const;
    std::vector<float> getComparisonData(const std::string& metric) const;
    std::vector<std::vector<float>> getComparisonMatrix() const;
    std::vector<std::string> getSignificantDifferences() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Parameter impact visualizer - Show how parameter changes affect results
class ParameterImpactVisualizer : public IntegrationComponent {
public:
    ParameterImpactVisualizer();
    ~ParameterImpactVisualizer() override = default;
    
    // Initialize parameter impact visualizer
    bool initialize() override;
    
    // Set parameter
    void setParameter(const std::string& name, double value);
    void setParameterRange(const std::string& name, double min, double max);
    void addParameter(const std::string& name, const std::vector<double>& values);
    
    // Update parameter impact visualization
    void update() override;
    
    // Shutdown parameter impact
    void shutdown() override;
    
    // Check if initialized
    bool isInitialized() const override;
    
    // Visualization controls
    void setSensitivityAnalysis(bool enable);
    void setOptimization(bool enable);
    void setMonteCarlo(bool enable);
    void setLocalSearch(bool enable);
    
    // Parameter impact analysis
    void setImpactThreshold(double threshold);
    void setConfidenceInterval(double interval);
    void setAnalysisType(const std::string& type);
    
    // Get parameter impact data
    double getParameterImpact(const std::string& parameter) const;
    std::vector<double> getOptimalParameters() const;
    std::vector<double> getParameterSensitivities() const;
    std::string getOptimalStrategy() const;
    
    // Export parameter analysis
    bool exportParameterAnalysis(const std::string& filename);
    bool generateOptimizationReport(const std::string& filename);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm