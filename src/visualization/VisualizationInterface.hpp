#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_set>

namespace nlm {

// Forward declarations
class Brain;
class SpikeEvent;
class Neuron;
class Synapse;

// Visualization modes
enum class VisualizationMode {
    None,           // No visualization
    NetworkGraph,   // Network connectivity graph
    Activity,       // Neural activity heatmap
    Spikes,         // Spike raster plot
    Weights,        // Synaptic weight matrix
    3D,             // 3D neural architecture
    Custom         // Custom visualization
};

// Visualization output formats
enum class VisualizationOutput {
    Console,        // Text-based console output
    File,           // Save to file (SVG, PNG, etc.)
    Live,           // Live interactive display
    Web,            // Web-based visualization
    Both            // Console + File
};

// Visualization interface for debugging and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Disable copying, enable moving
    VisualizationInterface(const VisualizationInterface&) = delete;
    VisualizationInterface& operator=(const VisualizationInterface&) = delete;
    VisualizationInterface(VisualizationInterface&&) noexcept;
    VisualizationInterface& operator=(VisualizationInterface&&) noexcept;
    
    // Initialize visualization with specific backend
    bool initialize(VisualizationOutput output = VisualizationOutput::Console, 
                   VisualizationMode mode = VisualizationMode::None);
    
    // Initialize with GLFW backend (if available)
    bool initializeGLFW(int width = 1024, int height = 768, const std::string& title = "NLM Visualization");
    
    // Initialize with SDL backend (if available)
    bool initializeSDL(int width = 1024, int height = 768, const std::string& title = "NLM Visualization");
    
    // Update visualization data
    void update(double timeStep = 0.0);
    
    // Render current state
    void render();
    
    // Close visualization and cleanup
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Configuration
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    void setVisualizationMode(VisualizationMode mode);
    VisualizationMode getVisualizationMode() const;
    
    void setOutputFormat(VisualizationOutput output);
    VisualizationOutput getOutputFormat() const;
    
    // Network visualization
    void visualizeNetwork(const Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const Brain& brain);
    
    // 3D neural architecture visualization
    void visualize3D(const Brain& brain);
    
    // Export visualization to file
    bool exportVisualization(const std::string& filename, VisualizationOutput format = VisualizationOutput::File);
    
    // Web-based visualization
    bool launchWebVisualization(int port = 8080);
    
    // Get visualization statistics
    std::string getStatistics(const Brain& brain) const;
    
    // Check if specific visualization backend is available
    static bool isGLFWAvailable();
    static bool isSDLAvailable();
    static bool isWebAssemblyAvailable();
    
    // Get available visualization backends
    static std::vector<std::string> getAvailableBackends();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void setupGLFWWindow(int width, int height, const std::string& title);
    void setupSDLWindow(int width, int height, const std::string& title);
    void cleanupBackend();
    
    // Data processing for visualization
    void prepareNetworkData(const Brain& brain, std::vector<std::pair<size_t, size_t>>& connections) const;
    void prepareActivityData(const Brain& brain, std::vector<double>& activityLevels) const;
    void prepareSpikeData(const Brain& brain, std::vector<std::pair<double, size_t>>& spikeTimes) const;
    void prepareWeightData(const Brain& brain, std::vector<std::vector<double>>& weights) const;
};

} // namespace nlm
