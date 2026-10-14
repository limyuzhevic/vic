#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <functional>

namespace nlm {

// Visualization interface for debugging and analysis
// Provides comprehensive tools for visualizing neural network behavior
// Includes network topology, activity patterns, and performance metrics
class VisualizationInterface {
public:
    // Visualization modes
    enum class Mode {
        None,           // No visualization
        Network,        // Network topology visualization
        Activity,       // Neural activity visualization
        Spikes,         // Spike raster plots
        Weights,        // Synaptic weight visualization
        Memory,         // Memory system visualization
        Development,    // Developmental stage visualization
        Neuromodulation, // Neuromodulation dynamics
        Performance,    // Performance metrics
        Custom         // Custom visualization
    };

    // Analysis modes
    enum class Analysis {
        None,           // No analysis
        Statistics,     // Statistical analysis
        Dynamics,       // Dynamic pattern analysis
        Connectivity,   // Network connectivity analysis
        Plasticity,     // Plasticity dynamics analysis
        Learning,       // Learning progress analysis
        All            // All analysis modes
    };

    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with specified capabilities
    bool initialize(Mode mode = Mode::Network, Analysis analysis = Analysis::Statistics,
                   double updateRate = 30.0, bool enableDebug = false);
    
    // Update visualization data
    void update();
    
    // Render current state with specified options
    void render(bool showLabels = true, bool showValues = false,
                bool animated = true, double timeScale = 1.0);
    
    // Close visualization and cleanup resources
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set visualization mode
    void setMode(Mode mode);
    Mode getMode() const;
    
    // Set analysis mode
    void setAnalysis(Analysis analysis);
    Analysis getAnalysis() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Toggle debug mode
    void setDebugEnabled(bool enabled);
    bool isDebugEnabled() const;
    
    // Network visualization with multiple views
    void visualizeNetwork(const class Brain& brain,
                         const std::string& title = "Network Topology",
                         bool showRegions = true,
                         bool showPopulations = true,
                         bool showConnections = true);
    
    // Activity visualization with time series
    void visualizeActivity(const class Brain& brain,
                          const std::string& title = "Neural Activity",
                          const std::vector<std::string>& metrics = {"spike_rate", "firing_rate"});
    
    // Spike raster plot with advanced features
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes,
                        const std::string& title = "Spike Raster",
                        double timeWindow = 1.0, int maxNeurons = 100,
                        bool showInterSpikeIntervals = false);
    
    // Weight matrix visualization with analysis
    void visualizeWeights(const class Brain& brain,
                         const std::string& title = "Synaptic Weights",
                         bool showWeightDistribution = true,
                         bool showExcitatoryInhibitory = true,
                         double minWeight = -1.0, double maxWeight = 1.0);
    
    // Memory system visualization
    void visualizeMemory(const class Brain& brain,
                        const std::string& title = "Memory Systems",
                        bool showWorkingMemory = true,
                        bool showEpisodicMemory = true,
                        bool showAssociativeMemory = true);
    
    // Development stage visualization
    void visualizeDevelopment(const class Brain& brain,
                             const std::string& title = "Developmental Stages");
    
    // Neuromodulation dynamics visualization
    void visualizeNeuromodulation(const class Brain& brain,
                                  const std::string& title = "Neuromodulation Dynamics",
                                  bool showDopamine = true,
                                  bool showCuriosity = true,
                                  bool showNovelty = true,
                                  bool showPredictionError = true);
    
    // Performance metrics visualization
    void visualizePerformance(const class Brain& brain,
                             const std::string& title = "Performance Metrics",
                             const std::vector<std::string>& metrics = {"firing_rate", "spike_count", "e_i_ratio"});
    
    // Export visualization data
    bool exportData(const std::string& filepath, const std::string& format = "csv");
    
    // Generate visualization report
    std::string generateReport(const class Brain& brain) const;
    
    // Callback for interactive visualization
    using VisualizationCallback = std::function<void(const std::string& event, const py::dict& data)>;
    void setCallback(VisualizationCallback callback);
    
    // Save visualization configuration
    bool saveConfig(const std::string& filepath);
    
    // Load visualization configuration
    bool loadConfig(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
