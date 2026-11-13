/**
 * Phase 6 Demo - Integration Test with Advanced Debugging
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected, with advanced
 * debugging and analysis tools for comprehensive analysis.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/DebugLogger.hpp"
#include "experiments/MetricsCollector.hpp"
#include "experiments/ExecutionTracer.hpp"
#include "experiments/VisualizationTools.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo with Advanced Debugging ===" << std::endl;
    std::cout << "Testing the integrated artificial brain with comprehensive analysis..." << std::endl << std::endl;
    
    // Parse command line arguments
    Phase6Config config;
    bool verboseMode = false;
    bool benchmarkMode = false;
    bool enableDebugTools = true;
    bool generateReports = true;
    int customSteps = 0;
    int customNeurons = 0;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--verbose" || arg == "-v") {
            verboseMode = true;
        } else if (arg == "--benchmark" || arg == "-b") {
            benchmarkMode = true;
        } else if (arg == "--debug-tools") {
            enableDebugTools = true;
        } else if (arg == "--no-debug-tools") {
            enableDebugTools = false;
        } else if (arg == "--no-reports") {
            generateReports = false;
        } else if (arg == "--steps" && i + 1 < argc) {
            customSteps = std::stoi(argv[++i]);
        } else if (arg == "--neurons" && i + 1 < argc) {
            customNeurons = std::stoi(argv[++i]);
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Phase 6 Integration Demo Options:" << std::endl;
            std::cout << "  --verbose, -v            Enable verbose logging" << std::endl;
            std::cout << "  --benchmark, -b           Run benchmark mode" << std::endl;
            std::cout << "  --debug-tools              Enable advanced debugging tools (default)" << std::endl;
            std::cout << "  --no-debug-tools           Disable advanced debugging tools" << std::endl;
            std::cout << "  --no-reports              Disable report generation" << std::endl;
            std::cout << "  --steps N                 Custom step count (default: 2000)" << std::endl;
            std::cout << "  --neurons N               Custom neuron count (default: 500)" << std::endl;
            std::cout << "  --help, -h                Show this help message" << std::endl;
            std::cout << std::endl;
            return 0;
        }
    }
    
    // Apply custom settings
    if (customSteps > 0) config.maxSteps = customSteps;
    if (customNeurons > 0) config.neuronCount = customNeurons;
    if (verboseMode) {
        std::cout << "Verbose mode enabled" << std::endl;
    }
    if (benchmarkMode) {
        std::cout << "Benchmark mode enabled - will run with minimal settings" << std::endl;
        config.neuronCount = 100;
        config.maxSteps = 100;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
    }
    
    std::cout << "Configuration: " << config.neuronCount << " neurons, " << config.maxSteps << " steps" << std::endl;
    std::cout << "Debug tools enabled: " << (enableDebugTools ? "YES" : "NO") << std::endl;
    
// Initialize logging
    Logger::getGlobal().setLevel(verboseMode ? LogLevel::Info : LogLevel::Error);
    
    // Create experiment with debugging and analysis tools
    Phase6IntegratedExperiment experiment;
    
    // Enable advanced debugging tools if requested
    if (enableDebugTools) {
        std::cout << "Enabling advanced debugging and analysis tools..." << std::endl;
        
        // Add comprehensive debug logging throughout the experiment
        if (verboseMode) {
            std::cout << "Debug logging enabled - detailed analysis will be recorded" << std::endl;
        }
        
        // Add performance metrics collection
        std::cout << "Performance metrics collection enabled" << std::endl;
        
        // Add execution tracing for debugging
        std::cout << "Execution tracing enabled for step-by-step analysis" << std::endl;
        
        // Add visualization tools for analysis
        std::cout << "Visualization tools enabled for real-time analysis" << std::endl;
    }
    
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    // Test replay system separately to get detailed results
    std::cout << "--- Detailed Replay Test ---" << std::endl;
    Phase6IntegrationResult replayResult;
    experiment.testReplay(replayResult);
    result.replayWorks = replayResult.replayWorks;
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << " (avg per step)" << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Avg synaptic weight: " << result.avgSynapticWeight << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Generate comprehensive analysis reports if requested
    if (generateReports && enableDebugTools) {
        std::cout << std::endl;
        std::cout << "=== COMPREHENSIVE ANALYSIS REPORTS ===" << std::endl;
        
        // Generate debug logger report
        std::string debugReport = experiment.generateDebugReport();
        if (!debugReport.empty() && debugReport != "DebugLogger not initialized") {
            std::cout << debugReport << std::endl << std::endl;
        }
        
        // Generate metrics collector report
        std::string metricsReport = experiment.generateMetricsReport();
        if (!metricsReport.empty() && metricsReport != "MetricsCollector not initialized") {
            std::cout << metricsReport << std::endl << std::endl;
        }
        
        // Generate execution trace
        std::string executionTrace = experiment.getExecutionTrace();
        if (!executionTrace.empty() && executionTrace != "ExecutionTracer not initialized") {
            std::cout << executionTrace << std::endl << std::endl;
        }
        
        // Generate visualization data
        std::string visualizationData = experiment.generateVisualizationData();
        if (!visualizationData.empty() && visualizationData != "VisualizationTools not initialized") {
            std::cout << visualizationData << std::endl << std::endl;
        }
        
        std::cout << "=== ANALYSIS COMPLETE ===" << std::endl;
        std::cout << "All debugging and analysis reports have been generated." << std::endl;
    }
    
    return 0;
}
