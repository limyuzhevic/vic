// ============================================================================
// Visualization Integration and Main Entry Points
// ============================================================================

#pragma once

#include <string>
#include <memory>
#include <vector>
#include "VisualizationInterface.hpp"
#include "VisualizationRegistry.hpp"
#include "VisualizationConfig.hpp"
#include "VisualizationDataStream.hpp"
#include "VisualizationRenderer.hpp"
#include "VisualizationExporter.hpp"

namespace nlm {

/**
 * @class VisualizationSystem
 * @brief Main visualization system integration
 * 
 * This class serves as the main entry point for the visualization system,
 * coordinating all components and providing a unified interface for
 * initialization, configuration, and operation.
 */
class VisualizationSystem {
public:
    static VisualizationSystem& getInstance() {
        static VisualizationSystem instance;
        return instance;
    }
    
    VisualizationSystem(const VisualizationSystem&) = delete;
    VisualizationSystem& operator=(const VisualizationSystem&) = delete;
    
    // System management
    bool initialize(const VisualizationConfig& config);
    void update();
    void render();
    void close();
    
    // Configuration
    bool setConfiguration(const VisualizationConfig& config);
    const VisualizationConfig& getConfiguration() const;
    
    // Data streaming
    bool attachToBrain(class Brain* brain);
    bool attachToMemory(class MemorySystem* memory);
    bool attachToPlasticity(class PlasticitySystem* plasticity);
    
    // Visualizer management
    bool registerVisualizer(std::unique_ptr<VisualizationBase> visualizer);
    bool unregisterVisualizer(const std::string& name);
    VisualizationBase* getVisualizer(const std::string& name);
    
    // System control
    void activateVisualizers(const std::vector<std::string>& names);
    void deactivateVisualizers(const std::vector<std::string>& names);
    void pauseVisualization();
    void resumeVisualization();
    
    // System status and monitoring
    bool isHealthy() const;
    std::string getSystemStatus() const;
    double getSystemFPS() const;
    double getAverageFrameTime() const;
    
    // Export and reporting
    bool exportData(const std::string& outputPath, VisualizationOutputFormat format);
    std::string generateReport() const;
    
    // Performance monitoring
    void setUpdateFrequency(uint32_t frequency);
    void setTargetFPS(uint32_t fps);
    uint32_t getCurrentFPS() const { return currentFPS_; }
    
protected:
    VisualizationSystem();
    ~VisualizationSystem();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    uint32_t currentFPS_;
    bool systemPaused_;
};

/**
 * @class VisualizationMain
 * @brief Main entry point for visualization demonstration
 * 
 * This class provides a complete example of how to use the visualization
 * system with the NLM brain simulation, demonstrating all the key features
 * and capabilities.
 */
class VisualizationMain {
public:
    VisualizationMain();
    ~VisualizationMain();
    
    // Demonstration setup
    bool initializeDemo(const VisualizationConfig& config);
    void runDemoLoop();
    void cleanupDemo();
    
    // Configuration utilities
    static VisualizationConfig createDefaultVisualizationConfig();
    static VisualizationConfig createDemoVisualizationConfig();
    static VisualizationConfig createResearchVisualizationConfig();
    
    // Demo scenarios
    void runBasicDemo();
    void runLearningDemo();
    void runRealTimeDemo();
    void runAnalysisDemo();
    
    // Visualization setup
    void setupVisualizationSystem();
    void setupBrainIntegration();
    void setupMemoryVisualization();
    void setupLearningVisualization();
    void setupNeuromodulationVisualization();
    
    // Data collection and analysis
    void setupDataCollection();
    void setupRealTimeAnalysis();
    void setupExportPipeline();
    
    // User interaction
    void setupInteractiveControls();
    void handleUserInput();
    void updateVisualizationControls();
    
    // Visualization output
    void setupMultiFormatOutput();
    void setupWebDashboard();
    void setupTerminalOutput();
    
    // Performance and monitoring
    void setupPerformanceMonitoring();
    void monitorSystemPerformance();
    void collectMetrics();
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
