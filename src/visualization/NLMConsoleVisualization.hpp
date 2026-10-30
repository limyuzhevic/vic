# Simple terminal-based visualization and monitoring system for NLM
# Provides real-time visualization of brain activity and simulation progress

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <iostream>
#include <iomanip>

namespace nlm {

// Simple console visualization for monitoring brain activity
class ConsoleVisualization {
public:
    ConsoleVisualization();
    ~ConsoleVisualization();
    
    // Initialize the visualization
    bool initialize(size_t width = 80, size_t height = 40);
    
    // Update visualization data
    void update(const std::shared_ptr<class Brain>& brain, uint64_t step, double timestep);
    
    // Render to console
    void render();
    
    // Close visualization
    void close();
    
    // Check if active
    bool isActive() const { return active_; }
    
    // Toggle display
    void toggleDisplay() { showDisplay_ = !showDisplay_; }
    
    // Set update rate
    void setUpdateRate(double hz) { updateRate_ = hz; }
    
    // Set colors for different neuron types
    void setColors(bool useColors) { useColors_ = useColors; }
    
    // Show/hide different visualizations
    void showNetwork(bool show) { showNetwork_ = show; }
    void showActivity(bool show) { showActivity_ = show; }
    void showSpikes(bool show) { showSpikes_ = show; }
    void showMetrics(bool show) { showMetrics_ = show; }
    
    // Get current brain statistics
    std::string getBrainStats() const;
    
    // Get performance metrics
    std::string getPerformanceStats() const;
    
    // Save visualization data
    bool saveFrame(const std::string& filename);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool active_;
    bool showDisplay_;
    bool useColors_;
    double updateRate_;
    size_t width_;
    size_t height_;
    
    mutable std::mutex renderMutex_;
    
    // Internal helper functions
    void clearScreen();
    void drawBox(size_t x, size_t y, size_t w, size_t h);
    std::string formatNumber(float value, int precision = 2) const;
    std::string getNeuronTypeSymbol(NeuronType type) const;
    std::string getColorCode(NeuronType type) const;
    
    // Network visualization helpers
    void visualizeNetwork(const class Brain& brain, char* buffer, size_t width, size_t height);
    
    // Activity visualization helpers
    void visualizeActivity(const class Brain& brain, char* buffer, size_t width, size_t height);
    
    // Spike visualization helpers
    void visualizeSpikes(const class Brain& brain, char* buffer, size_t width, size_t height);
    
    // Metrics visualization helpers
    void visualizeMetrics(const class Brain& brain, char* buffer, size_t width, size_t height);
};

// Real-time monitoring system for simulation performance
class SimulationMonitor {
public:
    SimulationMonitor();
    ~SimulationMonitor();
    
    // Start monitoring
    void startMonitoring();
    
    // Stop monitoring
    void stopMonitoring();
    
    // Record simulation step
    void recordStep(uint64_t step, double timestep, const std::shared_ptr<class Brain>& brain);
    
    // Get performance statistics
    std::string getPerformanceStats() const;
    
    // Get memory usage
    size_t getMemoryUsage() const;
    
    // Get CPU usage
    double getCPUUsage() const;
    
    // Get simulation rate
    double getSimulationRate() const;
    
    // Check if monitoring
    bool isMonitoring() const { return monitoring_; }
    
    // Get number of steps monitored
    size_t getStepCount() const { return stepCount_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool monitoring_;
    mutable std::mutex statsMutex_;
    
    size_t stepCount_;
    double startTime_;
    double lastStepTime_;
    double totalSimulationTime_;
    double cpuTimeTotal_;
    size_t memorySamples_;
};

// Interactive visualization controller
class VisualizationController {
public:
    VisualizationController();
    ~VisualizationController();
    
    // Initialize with brain and world
    bool initialize(
        const std::shared_ptr<class Brain>& brain,
        const std::shared_ptr<class SimpleWorld>& world = nullptr,
        const std::shared_ptr<class AgentBrain>& agent = nullptr
    );
    
    // Run interactive visualization loop
    int runInteractive(const std::string& title = "NLM Visualization");
    
    // Export data for analysis
    bool exportData(const std::string& outputDir, bool includeAll = true);
    
    // Generate report
    std::string generateReport() const;
    
    // Toggle features
    void toggleNetwork() { showNetwork_ = !showNetwork_; }
    void toggleActivity() { showActivity_ = !showActivity_; }
    void toggleSpikes() { showSpikes_ = !showSpikes_; }
    void toggleMetrics() { showMetrics_ = !showMetrics_; }
    void toggleHelp() { showHelp_ = !showHelp_; }
    
    // Set visualization options
    void setUpdateInterval(double seconds) { updateInterval_ = seconds; }
    void setMaxHistory(size_t maxHistory) { maxHistory_ = maxHistory; }
    void setColorScheme(const std::string& scheme) { colorScheme_ = scheme; }
    
    // Get status
    bool isRunning() const { return running_; }
    bool isPaused() const { return paused_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool running_;
    bool paused_;
    double updateInterval_;
    size_t maxHistory_;
    std::string colorScheme_;
    bool showNetwork_;
    bool showActivity_;
    bool showSpikes_;
    bool showMetrics_;
    bool showHelp_;
    
    std::shared_ptr<class Brain> brain_;
    std::shared_ptr<class SimpleWorld> world_;
    std::shared_ptr<class AgentBrain> agent_;
    
    // Interactive input handler
    int handleInput(char input);
    void displayHelp() const;
    void renderFrame(const std::string& title);
};

// Utility functions for visualization
namespace VisualizationUtils {
    // Generate ASCII art of neural network
    std::string generateNetworkASCII(const class Brain& brain, size_t width = 60);
    
    // Generate spike raster plot data
    std::vector<std::string> generateSpikeRaster(const class Brain& brain, size_t timeSteps = 100);
    
    // Generate activity heatmap
    std::string generateActivityHeatmap(const class Brain& brain, size_t width = 40);
    
    // Format simulation metrics
    std::string formatMetrics(double totalReward, double avgFiringRate, 
                             size_t totalSpikes, uint64_t steps, double time);
    
    // Create progress bar
    std::string createProgressBar(double value, double max, size_t width = 20);
    
    // Color utilities
    std::string getColorCode(const std::string& color);
    std::string resetColor();
    
    // Check if output is a terminal
    bool isTerminalOutput();
}

} // namespace nlm