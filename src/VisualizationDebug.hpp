// NLM Visualization and Debugging Tools
// Provides visualization and debugging capabilities for NLM brain simulations
// 
// This module provides tools for:
// 
// 1. Brain state visualization
//    - Neural network visualization
//    - Synaptic connection visualization
//    - Activity heat maps
//    - Time series visualization
// 
// 2. Debugging tools
//    - State inspection
//    - Connection validation
//    - Performance debugging
//    - Memory debugging
// 
// 3. Interactive debugging
//    - Live state monitoring
//    - Step-by-step debugging
//    - Breakpoint support
//    - Variable inspection
// 
// 4. Export and analysis tools
//    - State export
//    - Visualization export
//    - Debug log analysis
//    - Performance analysis
// 
// This is particularly useful for:
// - Research visualization
// - Debugging complex behaviors
// - Performance analysis
// - Educational purposes
// - Automated testing

#ifndef NLM_VISUALIZATION_DEBUG_H
#define NLM_VISUALIZATION_DEBUG_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <fstream>

namespace nlm {

/**
 * Brain state visualizer
 * 
 * Provides visualization of NLM brain state, including neural networks,
 * synaptic connections, and activity patterns.
 */
class BrainVisualizer {
public:
    /**
     * Visualization formats
     */
    enum class OutputFormat {
        ASCII,      // ASCII art visualization
        JSON,       // JSON data export
        CSV,        // CSV data export
        TEXT,       // Plain text format
        HTML,       // HTML visualization
        SVG,        // SVG graphics
        PNG,        // PNG image
        PDF         // PDF document
    };
    
    /**
     * Visualization options
     */
    struct VisualizationOptions {
        bool showNeurons;           // Show neuron nodes
        bool showSynapses;          // Show synaptic connections
        bool showActivity;          // Show neuron activity
        bool showConnections;       // Show connection weights
        bool showPopulations;       // Show neural populations
        bool showRegions;           // Show brain regions
        float neuronSize;           // Size of neuron nodes
        float connectionThickness;  // Thickness of connection lines
        std::string colorScheme;    // Color scheme (default, activity, etc.)
        bool includeLabels;         // Include node labels
        bool includeWeights;        // Include connection weights
        OutputFormat format;        // Output format
        std::string outputPath;     // Output file path
        
        VisualizationOptions()
            : showNeurons(true), showSynapses(true), showActivity(true)
            , showConnections(false), showPopulations(true), showRegions(true)
            , neuronSize(1.0f), connectionThickness(0.5f)
            , colorScheme("activity"), includeLabels(true), includeWeights(true)
            , format(OutputFormat::ASCII), outputPath("") {}
    };
    
    BrainVisualizer(std::shared_ptr<Brain> brain);
    ~BrainVisualizer();
    
    /**
     * Generate visualization of brain state
     */
    std::string visualize(const VisualizationOptions& options = VisualizationOptions());
    
    /**
     * Generate neural network visualization
     */
    std::string visualizeNetwork(const VisualizationOptions& options = VisualizationOptions());
    
    /**
     * Generate synaptic connection visualization
     */
    std::string visualizeConnections(const VisualizationOptions& options = VisualizationOptions());
    
    /**
     * Generate activity visualization
     */
    std::string visualizeActivity(const VisualizationOptions& options = VisualizationOptions());
    
    /**
     * Generate population visualization
     */
    std::string visualizePopulations(const VisualizationOptions& options = VisualizationOptions());
    
    /**
     * Generate region visualization
     */
    std::string visualizeRegions(const VisualizationOptions& options = VisualizationOptions());
    
    /**
     * Export visualization to file
     */
    bool exportVisualization(const std::string& filename, const VisualizationOptions& options = VisualizationOptions());
    
    /**
     * Get brain pointer
     */
    Brain* getBrain() { return brain_.get(); }
    
private:
    // ASCII art generation
    std::string generateASCIIArt(const VisualizationOptions& options) const;
    std::string generateNeuronASCII(size_t neuronIndex, bool active, float size) const;
    std::string generateConnectionASCII(size_t sourceIdx, size_t targetIdx, float weight) const;
    
    // JSON generation
    std::string generateJSON(const VisualizationOptions& options) const;
    std::string generateNeuronsJSON(const VisualizationOptions& options) const;
    std::string generateConnectionsJSON(const VisualizationOptions& options) const;
    
    // CSV generation
    std::string generateCSV(const VisualizationOptions& options) const;
    
    // HTML generation
    std::string generateHTML(const VisualizationOptions& options) const;
    
    // Text generation
    std::string generateText(const VisualizationOptions& options) const;
    
    std::shared_ptr<Brain> brain_;
};

/**
 * Interactive debugger
 * 
 * Provides interactive debugging capabilities for NLM brain simulations,
 * including breakpoints, variable inspection, and step-by-step execution.
 */
class InteractiveDebugger {
public:
    /**
     * Debug modes
     */
    enum class DebugMode {
        NORMAL,     // Normal execution
        STEP,       // Step-by-step execution
        BREAKPOINT, // Execute with breakpoints
        TRACE,      // Trace execution
        MONITOR,    // Monitor variables
        INSPECT     // Inspect state
    };
    
    /**
     * Breakpoint
     */
    struct Breakpoint {
        uint64_t step;              // Step number
        std::string condition;      // Condition (empty for any step)
        std::string action;         // Action to take
        bool enabled;              // Whether breakpoint is enabled
        
        Breakpoint() : step(0), enabled(true) {}
        Breakpoint(uint64_t s) : step(s), enabled(true) {}
    };
    
    /**
     * Variable inspector
     */
    struct VariableInfo {
        std::string name;           // Variable name
        std::string type;           // Variable type
        std::string value;          // Variable value (string representation)
        std::string description;    // Variable description
        
        VariableInfo() {}
        VariableInfo(const std::string& n, const std::string& t, const std::string& v, 
                    const std::string& d = "")
            : name(n), type(t), value(v), description(d) {}
    };
    
    InteractiveDebugger(std::shared_ptr<Brain> brain);
    ~InteractiveDebugger();
    
    /**
     * Set debug mode
     */
    void setDebugMode(DebugMode mode) { currentMode_ = mode; }
    
    /**
     * Get current debug mode
     */
    DebugMode getDebugMode() const { return currentMode_; }
    
    /**
     * Add a breakpoint
     */
    void addBreakpoint(const Breakpoint& breakpoint);
    
    /**
     * Remove a breakpoint
     */
    void removeBreakpoint(uint64_t step);
    
    /**
     * Check if breakpoint should trigger at current step
     */
    bool shouldBreak(uint64_t step) const;
    
    /**
     * Get all breakpoints
     */
    const std::vector<Breakpoint>& getBreakpoints() const { return breakpoints_; }
    
    /**
     * Add a variable to monitor
     */
    void addVariableToMonitor(const std::string& varName);
    
    /**
     * Remove a variable from monitoring
     */
    void removeVariableFromMonitor(const std::string& varName);
    
    /**
     * Get all monitored variables
     */
    const std::vector<std::string>& getMonitoredVariables() const { return monitoredVariables_; }
    
    /**
     * Inspect current state
     */
    std::vector<VariableInfo> inspectState() const;
    
    /**
     * Get variable value
     */
    std::string getVariableValue(const std::string& varName) const;
    
    /**
     * Step execution
     */
    void stepExecution();
    
    /**
     * Continue execution
     */
    void continueExecution();
    
    /**
     * Get current step
     */
    uint64_t getCurrentStep() const { return currentStep_; }
    
    /**
     * Check if debugger is active
     */
    bool isActive() const { return isActive_; }
    
    /**
     * Start debugging
     */
    void startDebugging();
    
    /**
     * Stop debugging
     */
    void stopDebugging();
    
private:
    DebugMode currentMode_;
    std::vector<Breakpoint> breakpoints_;
    std::vector<std::string> monitoredVariables_;
    uint64_t currentStep_;
    bool isActive_;
    std::shared_ptr<Brain> brain_;
    std::chrono::steady_clock::time_point sessionStart_;
};

/**
 * Debug log analyzer
 * 
 * Provides analysis of debug logs and simulation traces,
 * helping identify issues and performance problems.
 */
class DebugLogAnalyzer {
public:
    /**
     * Analysis result
     */
    struct AnalysisResult {
        std::string testName;        // Name of the test
        double duration;             // Duration (seconds)
        uint64_t totalSteps;        // Total steps
        uint64_t totalSpikes;       // Total spikes
        double avgFiringRate;       // Average firing rate
        double memoryUsage;         // Memory usage (MB)
        double cpuUsage;            // CPU usage (%)
        
        // Issues detected
        std::vector<std::string> issues;      // Issues found
        std::vector<std::string> warnings;    // Warnings
        std::vector<std::string> info;        // Informational messages
        
        // Performance metrics
        double stabilityScore;      // Stability (0.0-1.0)
        double efficiencyScore;     // Efficiency (0.0-1.0)
        double convergenceRate;     // Convergence rate
        
        AnalysisResult()
            : duration(0.0), totalSteps(0), totalSpikes(0)
            , avgFiringRate(0.0), memoryUsage(0.0), cpuUsage(0.0)
            , stabilityScore(0.0), efficiencyScore(0.0), convergenceRate(0.0) {}
    };
    
    DebugLogAnalyzer();
    ~DebugLogAnalyzer();
    
    /**
     * Load debug log from file
     */
    bool loadLog(const std::string& filename);
    
    /**
     * Load debug log from string
     */
    bool loadLogFromString(const std::string& logContent);
    
    /**
     * Analyze loaded log
     */
    AnalysisResult analyze() const;
    
    /**
     * Get log content
     */
    const std::string& getLogContent() const { return logContent_; }
    
    /**
     * Get analysis results
     */
    const std::vector<AnalysisResult>& getAnalysisResults() const { return analysisResults_; }
    
    /**
     * Generate report
     */
    std::string generateReport(const AnalysisResult& result) const;
    
    /**
     * Generate comparison report
     */
    std::string generateComparisonReport(const std::vector<AnalysisResult>& results) const;
    
    /**
     * Save analysis results
     */
    bool saveResults(const std::string& filename, const AnalysisResult& result) const;
    
    /**
     * Load analysis results
     */
    bool loadResults(const std::string& filename, AnalysisResult& result) const;
    
private:
    std::string logContent_;
    std::vector<AnalysisResult> analysisResults_;
    
    // Log parsing
    void parseLogContent();
    void parseLine(const std::string& line, AnalysisResult& result);
    
    // Issue detection
    void detectIssues(const AnalysisResult& result);
    void detectWarnings(const AnalysisResult& result);
    
    // Score calculation
    double calculateStabilityScore(const AnalysisResult& result) const;
    double calculateEfficiencyScore(const AnalysisResult& result) const;
    double calculateConvergenceRate(const AnalysisResult& result) const;
};

/**
 * Visualization and debugging manager
 * 
 * Main interface for visualization and debugging, coordinating
 * multiple visualization and debugging tools.
 */
class VisualizationDebugManager {
public:
    VisualizationDebugManager(std::shared_ptr<Brain> brain);
    ~VisualizationDebugManager();
    
    /**
     * Get visualizer instance
     */
    BrainVisualizer& getVisualizer() { return visualizer_; }
    
    /**
     * Get debugger instance
     */
    InteractiveDebugger& getDebugger() { return debugger_; }
    
    /**
     * Get log analyzer instance
     */
    DebugLogAnalyzer& getLogAnalyzer() { return logAnalyzer_; }
    
    /**
     * Generate visualization
     */
    std::string generateVisualization(const BrainVisualizer::VisualizationOptions& options = BrainVisualizer::VisualizationOptions());
    
    /**
     * Start debugging
     */
    void startDebugging(InteractiveDebugger::DebugMode mode = InteractiveDebugger::DebugMode::NORMAL);
    
    /**
     * Stop debugging
     */
    void stopDebugging();
    
    /**
     * Load and analyze debug log
     */
    DebugLogAnalyzer::AnalysisResult analyzeLog(const std::string& filename);
    
    /**
     * Export debug log
     */
    bool exportDebugLog(const std::string& filename);
    
    /**
     * Get visualization and debugging status
     */
    bool isActive() const { return visualizer_.getBrain() != nullptr; }
    
    /**
     * Reset all tools
     */
    void reset();
    
private:
    BrainVisualizer visualizer_;
    InteractiveDebugger debugger_;
    DebugLogAnalyzer logAnalyzer_;
};

} // namespace nlm

#endif // NLM_VISUALIZATION_DEBUG_H
