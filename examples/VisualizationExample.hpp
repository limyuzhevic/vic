// ============================================================================
// Visualization System Example Usage
// ============================================================================

#pragma once

#include "VisualizationSystem.hpp"
#include "VisualizationConfig.hpp"
#include "VisualizationInterface.hpp"
#include "VisualizationRegistry.hpp"
#include "VisualizationDataStream.hpp"
#include "BrainStateVisualizer.hpp"
#include "NeuralNetworkVisualizer.hpp"
#include "NeuronActivityVisualizer.hpp"
#include "SynapseDynamicsVisualizer.hpp"
#include "MemoryVisualizer.hpp"
#include "WorkingMemoryVisualizer.hpp"
#include "PlasticityVisualizer.hpp"
#include "STDPVisualizer.hpp"
#include "NeuromodulationVisualizer.hpp"
#include "DopamineVisualizer.hpp"
#include "DevelopmentVisualizer.hpp"
#include "CognitionVisualizer.hpp"
#include "VisualizationRenderer.hpp"
#include "VisualizationExporter.hpp"

namespace nlm {

/**
 * @class VisualizationExample
 * @brief Example demonstrating comprehensive visualization system usage
 * 
 * This class provides complete examples of how to use all visualization components
 * with the NLM brain simulation, covering all phases and features.
 */
class VisualizationExample {
public:
    VisualizationExample();
    ~VisualizationExample();
    
    // Example 1: Basic visualization setup
    bool runBasicVisualizationExample();
    
    // Example 2: Neural activity monitoring
    bool runNeuralActivityExample();
    
    // Example 3: Memory system visualization
    bool runMemorySystemExample();
    
    // Example 4: Plasticity and learning visualization
    bool runPlasticityExample();
    
    // Example 5: Neuromodulation visualization
    bool runNeuromodulationExample();
    
    // Example 6: Development and cognition visualization
    bool runDevelopmentExample();
    
    // Example 7: Real-time data streaming
    bool runRealTimeStreamingExample();
    
    // Example 8: Multiple output formats
    bool runMultiFormatOutputExample();
    
    // Example 9: Performance analysis
    bool runPerformanceAnalysisExample();
    
    // Example 10: Complete integration
    bool runCompleteIntegrationExample();
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Common setup methods
    bool setupVisualizationSystem();
    bool setupBrainIntegration();
    bool setupDataCollection();
    bool setupOutputInterfaces();
    bool setupPerformanceMonitoring();
    
    // Cleanup methods
    void cleanupVisualizationSystem();
    void cleanupDataStreams();
    void cleanupOutputInterfaces();
    
    // Utility methods
    void printExampleHeader(const std::string& exampleName);
    void printExampleFooter();
    bool waitForUserInput(const std::string& prompt = "Press Enter to continue...");
};

/**
 * @class VisualizationExampleUtils
 * @brief Utility functions for visualization examples
 * 
 * This class provides common utilities used across visualization examples,
 * including configuration creation, data generation, and output formatting.
 */
class VisualizationExampleUtils {
public:
    VisualizationExampleUtils();
    ~VisualizationExampleUtils();
    
    // Configuration creation
    static VisualizationConfig createTerminalConfig();
    static VisualizationConfig createOpenGLConfig();
    static VisualizationConfig createWebGLConfig();
    static VisualizationConfig createResearchConfig();
    static VisualizationConfig createEducationConfig();
    
    // Data generation
    static VisualizationDataFrame generateTestData(uint64_t step);
    static BrainState generateTestBrainState();
    static MemorySystemData generateTestMemoryData();
    static PlasticityData generateTestPlasticityData();
    static NeuromodulationData generateTestNeuromodulationData();
    
    // Output utilities
    static void printConfiguration(const VisualizationConfig& config);
    static void printDataFrame(const VisualizationDataFrame& frame);
    static void printMetrics(const VisualizationMetrics& metrics);
    
    // Statistics
    static double calculateStandardDeviation(const std::vector<double>& values);
    static double calculateMean(const std::vector<double>& values);
    static uint32_t findPeak(const std::vector<double>& data);
    
    // Validation
    static bool validateConfiguration(const VisualizationConfig& config);
    static bool validateDataFrame(const VisualizationDataFrame& frame);
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
