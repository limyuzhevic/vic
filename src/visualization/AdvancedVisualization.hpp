#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../brain/Brain.hpp"

namespace nlm {

// Advanced 3D Neural Network Visualization
class NeuralNetworkRenderer {
public:
    NeuralNetworkRenderer();
    ~NeuralNetworkRenderer();
    
    // Initialize renderer with OpenGL context
    bool initialize(GLFWwindow* window);
    
    // Update neuron positions and connections
    void update(const Brain& brain, double time);
    
    // Render the 3D neural network
    void render();
    
    // Set visualization mode
    void setVisualizationMode(const std::string& mode);
    
    // Configure rendering options
    void setShowNeurons(bool show);
    void setShowConnections(bool show);
    void setShowSpikes(bool show);
    void setConnectionStrength(bool weighted);
    void setColorScale(const std::string& scale);
    
    // Layout algorithms
    void setLayoutAlgorithm(const std::string& algorithm);
    void forceDirectedLayout(const Brain& brain);
    void radialLayout(const Brain& brain);
    void hierarchicalLayout(const Brain& brain);
    
    // Interaction handlers
    void onMouseClick(double x, double y);
    void onMouseScroll(double xoffset, double yoffset);
    void onKeyPress(int key, int scancode, int action);
    
    // Get render statistics
    std::string getStatistics() const;
    
    // Export visualization data
    bool exportToFile(const std::string& filepath, const std::string& format);
    
    // Get rendered data as JSON
    std::string getDataJSON() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Time-series Visualization for Neural Activity
class NeuralTimeSeries {
public:
    NeuralTimeSeries();
    ~NeuralTimeSeries();
    
    // Initialize time-series renderer
    bool initialize();
    
    // Update with new spike data
    void update(const std::vector<std::pair<NeuronId, Timestamp>>& spikes, double time);
    
    // Render time-series visualizations
    void renderSpikingActivity();
    void renderHeatMap(const std::string& metric);
    void renderLearningProgress();
    void renderPredictionAccuracy();
    void renderNeuromodulatorConcentration();
    
    // Configure visualization parameters
    void setTimeWindow(double window);
    void setBinSize(double binSize);
    void setColorScale(const std::string& scale);
    void enableConfidenceIntervals(bool enable);
    
    // Get statistics
    std::string getSpikeStatistics() const;
    std::string getLearningStatistics() const;
    
    // Export data
    bool exportRasterPlot(const std::string& filepath, int width, int height);
    std::string getDataCSV() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Real-time Data Streaming with WebSocket
class VisualizationWebSocketServer {
public:
    VisualizationWebSocketServer();
    ~VisualizationWebSocketServer();
    
    // Start WebSocket server
    bool start(int port = 8080);
    
    // Stop server
    void stop();
    
    // Send brain state updates
    void broadcastBrainState(const Brain& brain, double time);
    
    // Send visualization data
    void sendVisualizationData(const std::string& dataType, const std::string& data);
    
    // Handle WebSocket connections
    void onClientConnect(int clientId);
    void onClientDisconnect(int clientId);
    void onMessageReceived(int clientId, const std::string& message);
    
    // Get server status
    bool isRunning() const;
    std::string getServerStatus() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Multi-layered Brain Structure Visualization
class BrainStructureVisualizer {
public:
    BrainStructureVisualizer();
    ~BrainStructureVisualizer();
    
    // Initialize structure visualizer
    bool initialize();
    
    // Update with brain structure data
    void update(const Brain& brain);
    
    // Render different layers
    void renderCorticalLayer(int layer, const Brain& brain);
    void renderSubcorticalStructures(const Brain& brain);
    void renderConnectivePathways(const Brain& brain);
    
    // Highlight specific regions
    void highlightRegion(const std::string& regionName, const std::string& color);
    void highlightConnections(const std::vector<std::pair<NeuronId, NeuronId>>& connections);
    
    // Set structure view
    void setSliceThickness(double thickness);
    void setTransparentLayer(bool transparent);
    void set3DProjection(bool enabled);
    
    // Get structural statistics
    std::string getStructuralStatistics() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Machine Learning Model Visualization
class MLModelVisualizer {
public:
    MLModelVisualizer();
    ~MLModelVisualizer();
    
    // Initialize ML visualizer
    bool initialize();
    
    // Update with prediction data
    void update(const PredictionSystem& predictionSystem, double time);
    
    // Render decision boundaries
    void renderDecisionBoundaries(const Brain& brain);
    void renderFeatureSpace(const Brain& brain);
    void renderClusterVisualization(const Brain& brain);
    void renderNetworkMetrics(const Brain& brain);
    
    // Configure visualization
    void setMetricToVisualize(const std::string& metric);
    void enableErrorBars(bool enable);
    void setClusterAlgorithm(const std::string& algorithm);
    
    // Get visualization data
    std::string getNetworkMetrics() const;
    std::string getClusterData() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Production-Ready Visualization System
class AdvancedVisualizationSystem {
public:
    AdvancedVisualizationSystem();
    ~AdvancedVisualizationSystem();
    
    // Initialize complete visualization system
    bool initialize(GLFWwindow* window);
    
    // Main update loop
    void update(std::shared_ptr<Brain> brain, double time);
    
    // Render all visualization components
    void render();
    
    // Handle user interactions
    void onWindowResize(int width, int height);
    void onMouseButton(int button, int action, double x, double y);
    void onCursorPosition(double x, double y);
    
    // Configuration and control
    void setVisualizationMode(const std::string& mode);
    void toggleComponent(const std::string& component);
    void exportAllData(const std::string& basePath);
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    std::string getPerformanceStats() const;
    
    // Cleanup
    void shutdown();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
