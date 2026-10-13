#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include "../environment/Environment.hpp"
#include "../core/Logger/Logger.hpp"
#include "../experiments/Experiment.hpp"

namespace nlm {

// Performance monitoring configuration
struct PerformanceMonitorConfig {
    double samplingInterval; // in seconds
    double timeWindow; // in seconds
    bool enableRealTimeVisualization;
    std::string visualizationFormat;
    bool enableNeuralActivityAnalysis;
    bool enableBottleneckDetection;
    double memoryThreshold;
    double cpuThreshold;
    double spikeRateThreshold;
    
    PerformanceMonitorConfig() : samplingInterval(0.1), timeWindow(60.0), 
                                  enableRealTimeVisualization(true),
                                  visualizationFormat("html"), enableNeuralActivityAnalysis(true),
                                  enableBottleneckDetection(true), memoryThreshold(0.8),
                                  cpuThreshold(0.8), spikeRateThreshold(1000.0) {}
};

// Neural activity pattern analysis results
struct NeuralActivityAnalysis {
    std::string analysisType;
    double threshold;
    double detectedPatterns;
    std::map<std::string, double> metrics;
    std::string output;
};

// Bottleneck detection results
struct BottleneckInfo {
    std::string type;
    std::string description;
    double severity;
    std::string recommendation;
    std::chrono::system_clock::time_point detectedAt;
};

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Configure monitor with settings
    void configure(const PerformanceMonitorConfig& config);
    
    // Set components for monitoring
    void setBrain(std::shared_ptr<Brain> brain);
    void setExperimentRunner(std::shared_ptr<ExperimentRunner> runner);
    void setEnvironment(std::shared_ptr<Environment> environment);
    void setConfig(std::shared_ptr<Config> config);
    
    // Start/stop monitoring
    void startMonitoring();
    void stopMonitoring();
    void pauseMonitoring(bool paused);
    
    // Collect metrics
    void collectMetrics();
    void updateNeuralMetrics();
    void updateSystemMetrics();
    
    // Data access
    std::vector<double> getMemoryUsageHistory() const;
    std::vector<double> getSpikeRateHistory() const;
    std::vector<double> getEnergyUsageHistory() const;
    std::vector<double> getCpuUsageHistory() const;
    std::vector<double> getNetworkUsageHistory() const;
    std::vector<double> getTimePoints() const;
    
    // Analysis methods
    bool analyzeNeuralActivity(const std::string& outputFile, const std::string& analysisType, double threshold);
    bool identifyPerformanceBottlenecks(const std::string& outputFile, const std::string& analysisType);
    
    // Visualization methods
    bool generateRealTimeVisualization(const std::string& outputFile, const std::string& format, double timeWindow);
    bool generateNeuralActivityVisualization(const std::string& outputFile, const std::string& format);
    bool generateBottleneckVisualization(const std::string& outputFile);
    
    // Report generation
    bool generatePerformanceReport(const std::string& outputFile);
    bool generateNeuralActivityReport(const std::string& outputFile);
    bool generateBottleneckReport(const std::string& outputFile);
    
    // Data management
    void clearHistory();
    void saveHistory(const std::string& filepath);
    void loadHistory(const std::string& filepath);
    
    // Helper methods
    double calculateAverageFiringRate(const std::vector<double>& rates) const;
    double calculateAverageEnergy(const std::vector<double>& energy) const;
    double getCurrentCpuUsage() const;
    double getCurrentMemoryUsage() const;
    double getCurrentNetworkUsage() const;
    std::vector<double> getNeuronFiringRates() const;
    std::vector<double> getSynapticActivity() const;
    std::vector<double> getEnergyConsumption() const;
    
    // Configuration access
    PerformanceMonitorConfig getConfig() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void generateHtmlVisualization(std::ofstream& file, double timeWindow);
    void generateSvgVisualization(std::ofstream& file, double timeWindow);
    void generateCsvVisualization(std::ofstream& file, double timeWindow);
    void analyzeBurstPatterns(std::ofstream& file, const std::vector<double>& firingRates, double threshold);
    void analyzeOscillationPatterns(std::ofstream& file, const std::vector<double>& firingRates, double threshold);
    void analyzeNeuralCorrelations(std::ofstream& file, const std::vector<double>& firingRates, const std::vector<double>& synapticActivity);
    void analyzeNeuralClusters(std::ofstream& file, const std::vector<double>& firingRates);
    void detectMemoryBottlenecks();
    void detectCpuBottlenecks();
    void detectNetworkBottlenecks();
    void detectNeuralBottlenecks();
    std::string getJavaScriptArray(const std::vector<double>& data) const;
    void logAnalysisResult(const std::string& analysisType, const std::map<std::string, double>& results);
};

} // namespace nlm
