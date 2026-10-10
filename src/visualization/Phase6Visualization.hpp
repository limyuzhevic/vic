#pragma once

/**
 * Phase 6 Visualization - Real-time neural visualization system
 * 
 * This system provides comprehensive visualization capabilities for Phase 6 neural demonstrations:
 * - Real-time neural activity monitoring
 * - Brain system integration visualization
 * - Development stage progression visualization
 * - Performance metrics visualization
 * - Educational demonstration visualizations
 * 
 * Visualization Features:
 * - Neural network graphs and activity patterns
 * - Real-time data streaming and display
 * - Development timeline and stage progression
 * - Performance dashboard and metrics
 * - System integration status monitoring
 * - Educational demonstration mode
 * - Research visualization capabilities
 * 
 * Design Philosophy:
 * - Real-time data visualization
 * - Modular visualization components
 * - Educational demonstration support
 * - Research visualization capabilities
 * - System status monitoring
 * - Performance metrics visualization
 */

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>

namespace nlm {

/**
 * Neural visualization data structure
 */
struct NeuralVisualizationData {
    SimulationStep step;
    double timestamp;
    double firingRate;
    double firingRateNormalized;
    double synapticWeight;
    double synapticWeightNormalized;
    size_t activeNeurons;
    size_t totalNeurons;
    size_t newConnections;
    size_t prunedConnections;
    std::vector<double> firingRateHistory;
    std::vector<double> synapticWeightHistory;
    std::vector<double> firingRateMovingAverage;
    std::vector<double> synapticWeightMovingAverage;
    double memoryUtilization;
    double attentionLevel;
    double curiosityLevel;
    double dopamineLevel;
    double rewardValue;
    std::string developmentalStage;
    double developmentalAge;
};

/**
 * Performance visualization data
 */
struct PerformanceVisualizationData {
    double initializationTime;
    double learningTime;
    double peakFiringRate;
    double avgFiringRate;
    double rewardEfficiency;
    double memoryEfficiency;
    double plasticityEfficiency;
    double convergenceTime;
    std::vector<double> stabilityMetrics;
    std::vector<double> learningCurves;
    std::vector<double> performanceTrajectories;
    double performanceScore;
};

/**
 * Development visualization data
 */
struct DevelopmentVisualizationData {
    std::string currentStage;
    double stageAge;
    double plasticityModifier;
    double criticalPeriodProgress;
    double stageDuration;
    double stageProgress;
    std::vector<std::string> achievedMilestones;
    std::vector<std::string> upcomingMilestones;
    std::map<std::string, float> stageCharacteristics;
    std::map<std::string, float> stageCapabilities;
    std::vector<double> plasticityHistory;
    std::vector<double> criticalPeriodProgressHistory;
};

/**
 * System integration visualization data
 */
struct IntegrationVisualizationData {
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    bool replayWorks;
    double integrationScore;
    std::unordered_map<std::string, float> systemConnectivity;
    std::vector<std::string> integratedSystems;
    std::vector<std::string> disconnectedSystems;
    double totalConnectionHealth;
};

/**
 * Real-time visualization display data
 */
struct VisualizationDisplayData {
    double fps;
    double updateInterval;
    size_t frameCount;
    double lastFrameTime;
    double totalTime;
    std::chrono::system_clock::time_point lastUpdateTime;
    bool visualizationActive;
};

/**
 * Phase 6 comprehensive visualization system
 */
class Phase6Visualization {
public:
    Phase6Visualization();
    ~Phase6Visualization();
    
    /**
     * Initialize visualization system
     */
    bool initialize();
    
    /**
     * Shutdown visualization system
     */
    void shutdown();
    
    /**
     * Check if visualization is active
     */
    bool isActive() const;
    
    /**
     * Update visualization data
     */
    void update(double timestep);
    
    /**
     * Render current visualization
     */
    void render();
    
    /**
     * Set display mode
     */
    void setDisplayMode(const std::string& mode);
    
    /**
     * Get current display mode
     */
    std::string getDisplayMode() const;
    
    /**
     * Set visualization update rate
     */
    void setUpdateRate(double hz);
    
    /**
     * Get visualization update rate
     */
    double getUpdateRate() const;
    
    /**
     * Set data source for visualization
     */
    void setDataSource(std::shared_ptr<Phase6Demo> demo);
    
    /**
     * Get visualization display data
     */
    VisualizationDisplayData getDisplayData() const;
    
    /**
     * Get neural visualization data
     */
    NeuralVisualizationData getNeuralData() const;
    
    /**
     * Get performance visualization data
     */
    PerformanceVisualizationData getPerformanceData() const;
    
    /**
     * Get development visualization data
     */
    DevelopmentVisualizationData getDevelopmentData() const;
    
    /**
     * Get integration visualization data
     */
    IntegrationVisualizationData getIntegrationData() const;
    
    /**
     * Update neural visualization data
     */
    void updateNeuralData(const NeuralVisualizationData& data);
    
    /**
     * Update performance visualization data
     */
    void updatePerformanceData(const PerformanceVisualizationData& data);
    
    /**
     * Update development visualization data
     */
    void updateDevelopmentData(const DevelopmentVisualizationData& data);
    
    /**
     * Update integration visualization data
     */
    void updateIntegrationData(const IntegrationVisualizationData& data);
    
    /**
     * Visualize neural network
     */
    void visualizeNetwork(const std::string& title = "Neural Network");
    
    /**
     * Visualize neural activity
     */
    void visualizeActivity(const std::string& title = "Neural Activity");
    
    /**
     * Visualize development progression
     */
    void visualizeDevelopment(const std::string& title = "Development Progression");
    
    /**
     * Visualize performance metrics
     */
    void visualizePerformance(const std::string& title = "Performance Metrics");
    
    /**
     * Visualize system integration
     */
    void visualizeIntegration(const std::string& title = "System Integration");
    
    /**
     * Visualize real-time data stream
     */
    void visualizeRealTimeStream(const std::string& title = "Real-time Data Stream");
    
    /**
     * Save visualization to file
     */
    bool saveVisualization(const std::string& filename);
    
    /**
     * Export visualization data
     */
    bool exportVisualizationData(const std::string& filename);
    
    /**
     * Load visualization configuration
     */
    bool loadConfiguration(const std::string& filename);
    
    /**
     * Save visualization configuration
     */
    bool saveConfiguration(const std::string& filename);
    
    /**
     * Set educational mode
     */
    void setEducationalMode(bool enabled);
    
    /**
     * Set research mode
     */
    void setResearchMode(bool enabled);
    
    /**
     * Set demo mode
     */
    void setDemoMode(bool enabled);
    
    /**
     * Get current mode
     */
    std::string getCurrentMode() const;
    
    /**
     * Print visualization status
     */
    void printStatus() const;
    
    /**
     * Print performance information
     */
    void printPerformanceInfo() const;
    
    /**
     * Print development information
     */
    void printDevelopmentInfo() const;
    
    /**
     * Print integration information
     */
    void printIntegrationInfo() const;
    
    /**
     * Enable real-time data streaming
     */
    void enableRealTimeStreaming();
    
    /**
     * Disable real-time data streaming
     */
    void disableRealTimeStreaming();
    
    /**
     * Check if real-time streaming is enabled
     */
    bool isRealTimeStreamingEnabled() const;

private:
    /**
     * Initialize display system
     */
    bool initializeDisplay();
    
    /**
     * Initialize data pipeline
     */
    bool initializeDataPipeline();
    
    /**
     * Initialize rendering pipeline
     */
    bool initializeRenderingPipeline();
    
    /**
     * Initialize audio system
     */
    bool initializeAudioSystem();
    
    /**
     * Update display data
     */
    void updateDisplayData();
    
    /**
     * Update neural visualization
     */
    void updateNeuralVisualization();
    
    /**
     * Update performance visualization
     */
    void updatePerformanceVisualization();
    
    /**
     * Update development visualization
     */
    void updateDevelopmentVisualization();
    
    /**
     * Update integration visualization
     */
    void updateIntegrationVisualization();
    
    /**
     * Update real-time data stream
     */
    void updateRealTimeStream();
    
    /**
     * Generate neural visualization
     */
    void generateNeuralVisualization();
    
    /**
     * Generate performance visualization
     */
    void generatePerformanceVisualization();
    
    /**
     * Generate development visualization
     */
    void generateDevelopmentVisualization();
    
    /**
     * Generate integration visualization
     */
    void generateIntegrationVisualization();
    
    /**
     * Render neural visualization
     */
    void renderNeuralVisualization();
    
    /**
     * Render performance visualization
     */
    void renderPerformanceVisualization();
    
    /**
     * Render development visualization
     */
    void renderDevelopmentVisualization();
    
    /**
     * Render integration visualization
     */
    void renderIntegrationVisualization();
    
    /**
     * Render real-time stream
     */
    void renderRealTimeStream();
    
    /**
     * Render dashboard
     */
    void renderDashboard();
    
    /**
     * Render timeline
     */
    void renderTimeline();
    
    /**
     * Render network
     */
    void renderNetwork();
    
    /**
     * Handle user input
     */
    void handleInput();
    
    /**
     * Calculate visualization metrics
     */
    void calculateVisualizationMetrics();
    
    /**
     * Update visualization statistics
     */
    void updateVisualizationStatistics();
    
    /**
     * Generate visualization report
     */
    void generateVisualizationReport();
    
    /**
     * Generate performance report
     */
    void generatePerformanceReport();
    
    /**
     * Generate development report
     */
    void generateDevelopmentReport();
    
    /**
     * Generate integration report
     */
    void generateIntegrationReport();

private:
    // Core systems
    std::shared_ptr<Phase6Demo> demo_;
    
    // Display system
    struct DisplaySystem;
    std::unique_ptr<DisplaySystem> display_;
    
    // Data pipeline
    struct DataPipeline;
    std::unique_ptr<DataPipeline> dataPipeline_;
    
    // Rendering pipeline
    struct RenderingPipeline;
    std::unique_ptr<RenderingPipeline> renderingPipeline_;
    
    // Audio system
    struct AudioSystem;
    std::unique_ptr<AudioSystem> audio_;
    
    // Visualization data
    NeuralVisualizationData neuralData_;
    PerformanceVisualizationData performanceData_;
    DevelopmentVisualizationData developmentData_;
    IntegrationVisualizationData integrationData_;
    VisualizationDisplayData displayData_;
    
    // Display state
    std::string currentMode_;
    std::string displayMode_;
    double updateRate_;
    bool realTimeStreamingEnabled_;
    bool educationalMode_;
    bool researchMode_;
    bool demoMode_;
    
    // Timing
    double lastUpdateTime_;
    double totalTime_;
    size_t frameCount_;
    double fps_;
    
    // Configuration
    std::string configFile_;
    bool configurationLoaded_;
    
    // State tracking
    bool active_;
    bool initialized_;
    
    // Resource tracking
    size_t totalFramesRendered_;
    size_t totalDataPointsProcessed_;
    size_t totalVisualizationsGenerated_;
    
    // Threading and synchronization
    mutable std::mutex dataMutex_;
    std::condition_variable updateCondition_;
    std::atomic<bool> running_;
};