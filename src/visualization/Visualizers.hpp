#pragma once

#include <vector>
#include <string>
#include <memory>
#include <chrono>

namespace nlm {

class VisualizationInterface;
class Brain;
class Neuron;
class Synapse;

// Spike train visualization
class SpikeTrainVisualizer {
public:
    SpikeTrainVisualizer();
    ~SpikeTrainVisualizer();
    
    bool initialize(std::shared_ptr<VisualizationInterface> interface);
    void visualize(const std::vector<SpikeEvent>& spikes, double timeScale = 1.0, int windowWidth = 800, int windowHeight = 600);
    void setSpikeColor(const std::string& color) { spikeColor = color; }
    void setBackgroundColor(const std::string& color) { backgroundColor = color; }
    void setGridVisible(bool visible) { gridVisible = visible; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string spikeColor;
    std::string backgroundColor;
    bool gridVisible;
};

// Membrane potential visualization
class MembranePotentialVisualizer {
public:
    MembranePotentialVisualizer();
    ~MembranePotentialVisualizer();
    
    bool initialize(std::shared_ptr<VisualizationInterface> interface);
    void visualize(const Brain& brain, double timeScale = 1.0, int windowWidth = 800, int windowHeight = 600);
    void setVoltageRange(double minV, double maxV);
    void setTimeScale(double scale) { timeScale = scale; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    double voltageMin;
    double voltageMax;
    double timeScale;
};

// Synaptic connection visualization
class SynapticConnectionVisualizer {
public:
    SynapticConnectionVisualizer();
    ~SynapticConnectionVisualizer();
    
    bool initialize(std::shared_ptr<VisualizationInterface> interface);
    void visualize(const Brain& brain, double threshold = 0.5, bool showWeights = true, bool showTypes = true);
    void setConnectionColorScheme(const std::string& scheme) { colorScheme = scheme; }
    void setHighlightActive(bool highlight) { highlightActive = highlight; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string colorScheme;
    bool highlightActive;
};

// Brain region activity visualization
class RegionActivityVisualizer {
public:
    RegionActivityVisualizer();
    ~RegionActivityVisualizer();
    
    bool initialize(std::shared_ptr<VisualizationInterface> interface);
    void visualize(const Brain& brain, bool showHeatmap = true, bool showLabels = true);
    void updateRegionActivity(size_t regionId, double activityLevel);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Development visualization
class DevelopmentVisualizer {
public:
    DevelopmentVisualizer();
    ~DevelopmentVisualizer();
    
    bool initialize(std::shared_ptr<VisualizationInterface> interface);
    void visualize(const Brain& brain, int showStages = 1, bool showTransitions = true);
    void setShowPlasticity(bool show) { showPlasticity = show; }
    void setShowCriticalPeriod(bool show) { showCriticalPeriod = show; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool showPlasticity;
    bool showCriticalPeriod;
};

// Plasticity visualization
class PlasticityVisualizer {
public:
    PlasticityVisualizer();
    ~PlasticityVisualizer();
    
    bool initialize(std::shared_ptr<VisualizationInterface> interface);
    void visualize(const Brain& brain, const std::string& plasticityType = "all", bool showWeights = true);
    void visualizeWeightChanges(const Brain& brain, double threshold = 0.01);
    void setPlasticityOpacity(double opacity) { plasticityOpacity = opacity; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    double plasticityOpacity;
};

// Memory system visualization
class MemorySystemVisualizer {
public:
    MemorySystemVisualizer();
    ~MemorySystemVisualizer();
    
    bool initialize(std::shared_ptr<VisualizationInterface> interface);
    void visualize(const Brain& brain, bool showWorkingMemory = true, bool showEpisodicMemory = true, bool showAssociativeMemory = true);
    void visualizeWorkingMemory(const WorkingMemory& memory, int maxItems = 10);
    void visualizeEpisodicMemory(const EpisodicMemory& memory, int maxEpisodes = 10);
    void visualizeAssociativeMemory(const AssociativeMemory& memory);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Real-time monitoring visualizer
class MonitoringVisualizer {
public:
    MonitoringVisualizer();
    ~MonitoringVisualizer();
    
    bool initialize(std::shared_ptr<VisualizationInterface> interface);
    void visualize(const PerformanceMetrics& metrics, const std::vector<SystemAlert>& alerts);
    void setMonitoringLayout(const std::string& layout) { currentLayout = layout; }
    void toggleOverlay(bool show) { showOverlay = show; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string currentLayout;
    bool showOverlay;
};

// Command-line visualization
class CommandLineVisualizer {
public:
    CommandLineVisualizer();
    ~CommandLineVisualizer();
    
    void visualizeNeuralActivity(const Brain& brain, int maxSteps = 100);
    void visualizeSystemStatus(const PerformanceMetrics& metrics, const std::vector<SystemAlert>& alerts);
    void visualizePerformanceAnalysis(const std::vector<PerformanceMetrics>& history);
    void visualizeIntegrationVerification(bool verbose = false);
    
private:
    void printSpikeTrain(const std::vector<SpikeEvent>& spikes, int maxSpikes = 50);
    void printMembranePotentials(const Brain& brain, int neuronCount = 10);
    void printSynapticConnections(const Brain& brain, int maxConnections = 20);
    void printRegionActivity(const Brain& brain);
    void printMemoryStatus(const Brain& brain);
    void printPerformanceMetrics(const PerformanceMetrics& metrics);
    void printAlertSummary(const std::vector<SystemAlert>& alerts);
    void printProgressBar(int current, int total, int width = 50);
    
    std::string getStatusColor(SystemAlert::Level level);
    std::string getTimeString(std::chrono::system_clock::time_point time);
};

// Web-based visualization (optional)
class WebVisualizationInterface {
public:
    WebVisualizationInterface();
    ~WebVisualizationInterface();
    
    bool initialize(const std::string& host = "localhost", int port = 8080);
    void startServer();
    void stopServer();
    bool isServerRunning() const;
    void updateData(const Brain& brain, const PerformanceMetrics& metrics);
    void broadcastUpdate();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
