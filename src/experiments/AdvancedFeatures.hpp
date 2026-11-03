#pragma once

/**
 * Advanced Features for NLM Phase 6
 * 
 * Provides comprehensive advanced control over brain simulation, performance analysis,
 * visualization, and export capabilities for power users and researchers.
 * 
 * Features:
 * - Performance profiling and optimization
 * - Memory tracking and leak detection
 * - Neural activity tracing and analysis
 * - Synaptic density mapping
 * - Advanced export/import with multiple formats
 * - Pattern analysis and behavior clustering
 * - Learning curve analysis
 * - Network metrics calculation
 * - Advanced visualization (terminal, matplotlib, plotly)
 * - Batch mode processing
 * - Parallel processing support
 * - Auto-save functionality
 * - Performance bottleneck detection
 */

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <sstream>

namespace nlm {

/**
 * Advanced configuration options for NLM Phase 6
 */
struct AdvancedConfig {
    // Performance optimization
    bool enableProfiling = false;              // Enable comprehensive profiling
    bool enableProfilingGPU = false;           // Enable GPU-specific profiling
    bool enableMemoryTracking = false;         // Enable memory usage tracking
    bool enableNeuronTracing = false;          // Enable detailed neuron activity tracing
    bool enableSynapticDensityMap = false;      // Enable synaptic density mapping
    
    // Export/Import options
    std::string exportFormat = "json";        // Export format: json, xml, binary, csv
    std::string exportPath = "./exports";     // Export directory path
    bool enableCompression = true;            // Enable export compression
    
    // Analysis options
    bool enablePatternAnalysis = false;       // Enable neural pattern analysis
    bool enableBehaviorClustering = false;    // Enable behavior clustering analysis
    bool enableLearningCurveAnalysis = false; // Enable learning curve analysis
    bool enableNetworkMetrics = false;        // Enable network metrics calculation
    
    // Visualization options
    std::string visualizationEngine = "terminal";  // "terminal", "matplotlib", "plotly"
    bool enableLiveVisualization = false;           // Enable live visualization updates
    std::string visualizationFormat = "ascii";     // "ascii", "html", "svg"
    
    // Advanced control
    bool enableBatchMode = false;                 // Enable batch processing mode
    bool enableParallelProcessing = false;        // Enable parallel processing
    int parallelThreads = std::thread::hardware_concurrency(); // Number of parallel threads
    bool enableAutoSave = true;                   // Enable automatic checkpoint saving
    uint64_t autoSaveInterval = 10000;            // Auto-save interval in steps
    
    // Debug and development
    bool enableMemoryLeakDetection = false;       // Enable memory leak detection
    bool enablePerformanceBottleneckDetection = false; // Enable bottleneck detection
    bool enableNeuralActivityProfiling = false;   // Enable detailed neural activity profiling
    bool enableSynapseDynamicsTracking = false;  // Enable synaptic dynamics tracking
};

/**
 * Performance profile data structure
 */
struct PerformanceProfile {
    double totalTime;                          // Total execution time in seconds
    size_t neuronsProcessed;                   // Number of neurons processed
    size_t synapsesCreated;                    // Number of synapses created
    size_t spikesGenerated;                    // Total spikes generated
    double memoryUsage;                        // Peak memory usage in MB
    double cpuUsage;                           // Average CPU usage percentage
    std::map<std::string, double> metricBreakdown; // Detailed metric breakdown
};

/**
 * Neural pattern analysis results
 */
struct PatternAnalysisResult {
    std::vector<std::vector<float>> patterns;   // Extracted neural patterns
    std::vector<std::string> patternLabels;     // Labels for each pattern
    std::vector<float> patternStrengths;       // Strength/confidence of each pattern
    std::vector<size_t> neuronInvolvement;      // Number of patterns each neuron participates in
};

/**
 * Advanced features controller for NLM Phase 6
 */
class AdvancedFeatures {
public:
    AdvancedFeatures();
    ~AdvancedFeatures();
    
    /**
     * Parse advanced command-line arguments
     * 
     * @param argc Command line argument count
     * @param argv Command line argument array
     * @param config Advanced configuration to be populated
     * @return true if parsing successful, false if help/usage requested
     */
    bool parseAdvancedArguments(int argc, char* argv[], AdvancedConfig& config);
    
    /**
     * Generate comprehensive performance profile
     * 
     * @param sessionFile Optional session file to load existing performance data
     * @return Performance profile with timing and resource usage metrics
     */
    PerformanceProfile generatePerformanceProfile(const std::string& sessionFile = "");
    
    /**
     * Export simulation results in specified format
     * 
     * @param data Simulation data to export
     * @param config Export configuration
     * @return true if export successful, false otherwise
     */
    bool exportResults(const std::string& data, const AdvancedConfig& config);
    
    /**
     * Analyze neural patterns from simulation data
     * 
     * @param neuralData Neural activity data to analyze
     * @return Pattern analysis results including extracted patterns and labels
     */
    PatternAnalysisResult analyzeNeuralPatterns(const std::string& neuralData);
    
    /**
     * Generate network metrics from simulation results
     * 
     * @param simulationResults Simulation output data
     * @return Network metrics report as string
     */
    std::string generateNetworkMetrics(const std::string& simulationResults);
    
    /**
     * Generate visualization code for specified engine
     * 
     * @param config Visualization configuration
     * @return Visualization code as string
     */
    std::string generateVisualizationCode(const AdvancedConfig& config);
    
    /**
     * Generate performance benchmark report
     * 
     * @return Performance benchmark report as string
     */
    std::string generatePerformanceBenchmark();
    
private:
    std::string getCurrentTimestamp();
    std::string generatePythonVisualizationCode(const AdvancedConfig& config);
    std::string generateTerminalVisualizationCode(const AdvancedConfig& config);
};

} // namespace nlm