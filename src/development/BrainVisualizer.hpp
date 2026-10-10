#pragma once

#include "Brain.hpp"
#include "BrainDebugger.hpp"
#include "BrainSimulator.hpp"
#include "../visualization/VisualizationInterface.hpp"
#include "../visualization/VisualizationEngine.hpp"
#include "../visualization/BrainStructureRenderer.hpp"
#include "../visualization/ActivityRenderer.hpp"
#include "../visualization/MemoryRenderer.hpp"
#include "../visualization/ProgressRenderer.hpp"
#include "../visualization/ExportRenderer.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <map>

namespace nlm {

class BrainVisualizer {
public:
    BrainVisualizer(std::shared_ptr<Brain> brain, std::shared_ptr<BrainDebugger> debugger,
                   std::shared_ptr<VisualizationInterface> visualizationInterface);
    ~BrainVisualizer();
    
    // Disable copying, enable moving
    BrainVisualizer(const BrainVisualizer&) = delete;
    BrainVisualizer& operator=(const BrainVisualizer&) = delete;
    BrainVisualizer(BrainVisualizer&&) noexcept = default;
    BrainVisualizer& operator=(BrainVisualizer&&) noexcept = default;
    
    // Initialization and shutdown
    bool initialize();
    void shutdown();
    
    // Complete brain visualization
    void displayBrainStructure();
    void displayBrainStructure(const std::string& layout, const std::string& style);
    void displayBrainStructure(const VisualizationConfig& config);
    
    // Neural structure visualization
    void displayNeurons();
    void displayNeurons(NeuronType type, const std::string& color);
    void displayNeurons(const std::vector<NeuronId>& neuronIds);
    void displayNeurons(const VisualizationFilter& filter);
    
    // Synapse visualization
    void displaySynapses();
    void displaySynapses(float weightThreshold);
    void displaySynapses(const std::vector<SynapseId>& synapseIds);
    void displaySynapses(RegionId sourceRegion, RegionId targetRegion);
    
    // Regional visualization
    void displayRegions();
    void displayRegions(NeuronType dominantType);
    void displayRegionConnections(RegionId regionId);
    void displayRegionActivity(RegionId regionId);
    void displayRegionStatistics(RegionId regionId);
    
    // Neural activity visualization
    void showNeuralActivity();
    void showNeuralActivity(double timeWindow);
    void showNeuralActivity(NeuronId neuronId);
    void showNeuralActivity(RegionId regionId);
    void showNeuralActivity(const std::vector<NeuronId>& neuronIds);
    void showNeuralActivity(const VisualizationFilter& filter);
    
    // Learning curve visualization
    void displayLearningCurves();
    void displayLearningCurves(size_t maxPoints);
    void displayLearningCurves(const std::string& metric, double timeWindow);
    void displayLearningCurves(const std::vector<std::string>& metrics);
    void displayLearningStatistics();
    
    // Memory system visualization
    void showMemoryState();
    void showMemoryState(MemorySystem system);
    void showMemoryState(const std::string& memoryName);
    void showMemoryCapacity();
    void showMemoryUtilization();
    void showMemoryPerformance();
    
    // Performance metrics visualization
    void showPerformanceMetrics();
    void showPerformanceMetrics(const std::vector<std::string>& metrics);
    void showPerformanceMetrics(double timeWindow);
    void showPerformanceDashboard();
    void showPerformanceStatistics();
    
    // Real-time monitoring
    void startRealTimeVisualization(unsigned int updateIntervalMs = 100);
    void stopRealTimeVisualization();
    bool isVisualizationActive() const;
    void setUpdateInterval(unsigned int intervalMs);
    unsigned int getUpdateInterval() const;
    
    // Visualization control
    void pauseVisualization();
    void resumeVisualization();
    void stopVisualization();
    bool isVisualizationPaused() const;
    bool isVisualizationRunning() const;
    
    // Visualization modes
    void setDisplayMode(int mode);
    int getDisplayMode() const;
    void setLayout(const std::string& layout);
    std::string getLayout() const;
    void setStyle(const std::string& style);
    std::string getStyle() const;
    
    // Data export
    bool exportVisualizationData(const std::string& filename, const std::string& format = "json");
    bool exportBrainStructure(const std::string& filename, const std::string& format = "json");
    bool exportActivityData(const std::string& filename, const std::string& format = "csv");
    bool exportLearningData(const std::string& filename, const std::string& format = "csv");
    bool exportPerformanceData(const std::string& filename, const std::string& format = "csv");
    
    // Configuration management
    void loadVisualizationConfiguration(const std::string& configFile);
    void saveVisualizationConfiguration() const;
    void resetVisualizationConfiguration();
    VisualizationConfig getCurrentConfiguration() const;
    
    // Color management
    void setColorScheme(const std::string& scheme);
    std::string getColorScheme() const;
    void setColor(NeuronType type, const std::string& color);
    std::string getColor(NeuronType type) const;
    void setColor(RegionId regionId, const std::string& color);
    std::string getColor(RegionId regionId) const;
    
    // Layout management
    void setLayoutParameters(const LayoutParameters& params);
    LayoutParameters getLayoutParameters() const;
    void autoLayout();
    void layoutByRegion();
    void layoutByDensity();
    void layoutByActivity();
    
    // Interaction and manipulation
    void enableInteraction(bool enable);
    bool isInteractionEnabled() const;
    void setInteractionCallback(std::function<void(const std::string& action, const std::string& target)> callback);
    void highlightNeuron(NeuronId neuronId);
    void highlightSynapse(SynapseId synapseId);
    void highlightRegion(RegionId regionId);
    void clearHighlights();
    
    // Simulation integration
    void syncWithSimulation(std::shared_ptr<BrainSimulator> simulator);
    void setSimulationSpeed(double speed);
    double getSimulationSpeed() const;
    void stepSimulation();
    void pauseSimulation();
    void resumeSimulation();
    
    // Debugging and analysis
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const;
    void showDebugInfo();
    void showDebugInfo(const std::string& category);
    void logVisualizationEvent(const std::string& event, const std::string& details = "");
    
    // Data collection
    void startDataCollection(const std::string& dataFile);
    void stopDataCollection();
    void collectVisualizationData();
    bool isCollectingData() const;
    
    // Statistics and analysis
    struct VisualizationStats {
        double fps;
        double frameTime;
        size_t neuronsRendered;
        size_t synapsesRendered;
        size_t regionsRendered;
        size_t memorySystemsActive;
        double updateInterval;
        size_t totalFrames;
        size_t droppedFrames;
        double memoryUsage;
        double cpuUsage;
        
        VisualizationStats() : fps(0.0), frameTime(0.0), neuronsRendered(0), 
                              synapsesRendered(0), regionsRendered(0), 
                              memorySystemsActive(0), updateInterval(0.0), 
                              totalFrames(0), droppedFrames(0), memoryUsage(0.0),
                              cpuUsage(0.0) {}
    };
    
    VisualizationStats getVisualizationStats() const;
    
    // Configuration
    void setMaxNeuronsToRender(size_t maxNeurons);
    size_t getMaxNeuronsToRender() const;
    void setMaxConnectionsToRender(size_t maxConnections);
    size_t getMaxConnectionsToRender() const;
    void setRenderQuality(float quality);
    float getRenderQuality() const;
    
    // Thread safety
    void lockVisualization() const;
    void unlockVisualization() const;
    bool tryLockVisualization() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<BrainDebugger> debugger_;
    std::shared_ptr<BrainSimulator> simulator_;
    std::shared_ptr<VisualizationInterface> visualizationInterface_;
    std::shared_ptr<Config> config_;
    
    mutable std::mutex visualizationMutex_;
    std::atomic<bool> isActive_;
    std::atomic<bool> isPaused_;
    std::atomic<bool> isRunning_;
    std::atomic<bool> isCollectingData_;
    unsigned int updateIntervalMs_;
    
    // Visualization state
    int displayMode_;
    std::string layout_;
    std::string style_;
    std::string colorScheme_;
    VisualizationConfig currentConfig_;
    LayoutParameters layoutParams_;
    
    // Rendering components
    std::unique_ptr<BrainStructureRenderer> brainStructureRenderer_;
    std::unique_ptr<ActivityRenderer> activityRenderer_;
    std::unique_ptr<MemoryRenderer> memoryRenderer_;
    std::unique_ptr<ProgressRenderer> progressRenderer_;
    std::unique_ptr<ExportRenderer> exportRenderer_;
    
    // Data management
    std::string dataCollectionFile_;
    std::ofstream dataFile_;
    
    // Performance monitoring
    VisualizationStats visualizationStats_;
    double lastFrameTime_;
    size_t frameCount_;
    size_t droppedFrameCount_;
    
    // Interaction and callbacks
    bool interactionEnabled_;
    std::function<void(const std::string& action, const std::string& target)> interactionCallback_;
    
    // Real-time monitoring
    std::thread visualizationThread_;
    std::atomic<bool> monitoringRequested_;
    
    // Debugging
    bool debugModeEnabled_;
    
    // Simulation integration
    double simulationSpeed_;
    
    // Internal helper methods
    void cleanup();
    void initializeRenderers();
    void updateVisualization();
    void renderFrame();
    void processEvents();
    void updatePerformanceStats();
    void generateVisualizationReport() const;
    void exportVisualizationDataInternal() const;
    void logVisualizationEventInternal(const std::string& event, const std::string& details);
    void setupColorScheme();
    void setupLayout();
    void setupRenderers();
    void validateVisualizationSetup() const;
    double calculateFPS() const;
    void handleInteraction(const std::string& action, const std::string& target);
    std::vector<NeuronId> getHighlightedNeurons() const;
    std::vector<SynapseId> getHighlightedSynapses() const;
    std::vector<RegionId> getHighlightedRegions() const;
    void clearHighlightsInternal();
};

} // namespace nlm