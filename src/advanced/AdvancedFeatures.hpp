// Advanced features for NLM - Expert user functionality
// Provides enhanced experiment configurations, parameter tuning, and custom experiments

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace nlm {

class AdvancedExperimentManager {
public:
    struct AdvancedConfig {
        std::string name;
        std::map<std::string, std::string> parameters;
        std::vector<std::string> customPatterns;
        std::string description;
        bool enableEnhancedPlasticity = true;
        bool enableNeuromodulation = true;
        bool enableStructuralPlasticity = true;
        double customTimestep = 0.001;
        int64_t customNeuronCount = 500;
        std::string customLearningRate;
        std::string customSTDPParameters;
        std::string customDevelopmentStage;
    };
    
    AdvancedExperimentManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Load advanced experiment configurations
    bool loadAdvancedConfig(const std::string& filename);
    
    // Save custom experiment configurations
    bool saveAdvancedConfig(const std::string& filename, const AdvancedConfig& config);
    
    // Run advanced experiment with enhanced features
    bool runAdvancedExperiment(const AdvancedConfig& config);
    
    // Get available advanced experiment templates
    std::vector<std::string> getAvailableTemplates() const;
    
    // Get default advanced configuration
    AdvancedConfig getDefaultAdvancedConfig(const std::string& templateName = "balanced_learning");
    
    // Apply advanced parameter tuning
    void applyParameterTuning(const std::map<std::string, std::string>& tunedParams);
    
    // Generate custom learning patterns
    std::vector<std::vector<float>> generateCustomPatterns(int numPatterns, int patternLength);
    
    // Analyze experiment results with advanced metrics
    void analyzeAdvancedResults(const std::string& outputFile = "advanced_results.json");
    
private:
    std::shared_ptr<Brain> brain_;
    std::map<std::string, AdvancedConfig> experimentTemplates_;
    std::vector<std::string> availableTemplates_;
};

// Parameter optimization for expert users
class ParameterOptimizer {
public:
    struct OptimizationResult {
        std::map<std::string, double> optimizedParameters;
        double performanceScore;
        std::string optimizationMethod;
        int iterations;
        double convergenceTime;
    };
    
    ParameterOptimizer(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Optimize parameters using different methods
    OptimizationResult optimizeLearningRate(const std::vector<double>& candidates);
    OptimizationResult optimizeSTDPParameters(const std::vector<double>& candidates);
    OptimizationResult optimizeStructuralParams(const std::vector<double>& candidates);
    OptimizationResult optimizeNeurons(const std::vector<int64_t>& candidates);
    
    // Run comprehensive parameter search
    OptimizationResult runParameterSearch(const std::map<std::string, std::vector<double>>& parameterRanges);
    
    // Apply optimization results to brain configuration
    bool applyOptimization(const OptimizationResult& result);
    
    // Validate parameter combinations
    bool validateParameterCombination(const std::map<std::string, double>& params);
    
private:
    std::shared_ptr<Brain> brain_;
};

// Advanced visualization and analysis tools
class AdvancedVisualization {
public:
    AdvancedVisualization(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Generate detailed visualization data
    bool generateDetailedVisualization(const std::string& outputDir, bool includeTimeSeries = true);
    
    // Create connectivity heatmap
    bool generateConnectivityHeatmap(const std::string& outputFile);
    
    // Generate activity patterns analysis
    bool analyzeActivityPatterns(const std::string& outputFile);
    
    // Create synaptic weight evolution
    bool generateWeightEvolution(const std::string& outputFile);
    
    // Export data for external analysis tools
    bool exportForExternalTools(const std::string& outputDir);
    
    // Generate scientific plots
    bool generateScientificPlots(const std::string& outputDir);
    
private:
    std::shared_ptr<Brain> brain_;
};

// Expert debugging and monitoring tools
class AdvancedDebugging {
public:
    AdvancedDebugging(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // Start performance monitoring
    bool startPerformanceMonitoring(const std::string& logFile, double interval = 0.1);
    
    // Stop performance monitoring
    void stopPerformanceMonitoring();
    
    // Get current performance statistics
    struct PerformanceStats {
        double cpuUsage;
        double memoryUsage;
        size_t activeNeurons;
        size_t totalSpikes;
        double simulationTime;
        size_t pendingEvents;
        double averageFiringRate;
    };
    
    PerformanceStats getPerformanceStats();
    
    // Get detailed neuron state information
    bool getNeuronDebugInfo(NeuronId neuronId, std::string& output);
    
    // Get detailed synapse information
    bool getSynapseDebugInfo(SynapseId synapseId, std::string& output);
    
    // Check for potential issues
    std::vector<std::string> checkForIssues();
    
    // Generate debug report
    bool generateDebugReport(const std::string& outputFile);
    
private:
    std::shared_ptr<Brain> brain_;
    bool monitoringActive_ = false;
};

} // namespace nlm