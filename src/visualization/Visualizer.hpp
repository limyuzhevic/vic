#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <functional>

namespace nlm {

class Brain;
class NeuralRegion;
class Neuron;
class Synapse;
class SpikeEvent;
class Visualization;

/**
 * Core visualization system for NLM
 * 
 * Provides comprehensive visualization capabilities for:
 * - Neural network structures and connections
 * - Brain region organization and activity
 * - Spike activity and firing patterns
 * - Synaptic connectivity and plasticity
 * - Learning progress and performance metrics
 * - Real-time and batch visualization
 * 
 * Integration with existing systems:
 * - Data collection from simulation runs
 * - Export in multiple formats (PNG, PDF, SVG, HTML)
 * - Interactive web-based dashboards
 * - Command-line interface integration
 * 
 * Configuration options:
 * - Multiple visualization types
 * - Customizable rendering parameters
 * - Export format selection
 * - Real-time update rates
 * 
 * @see InteractiveVisualizer
 * @see NetworkVisualizer
 * @see SpikeVisualizer
 * @see DataExporter
 * @see AnimationGenerator
 */
class Visualizer {
public:
    /**
     * Visualization type enumeration
     */
    enum class VisualizationType {
        NEURAL_NETWORK,
        BRAIN_REGIONS,
        SPIKE_RASTER,
        SYNAPTIC_CONNECTIVITY,
        LEARNING_PROGRESS,
        PERFORMANCE_METRICS,
        HEAT_MAP,
        VOLUME_RENDERING,
        TIME_SERIES,
        COMPARISON,
        ANIMATION_SEQUENCE
    };

    /**
     * Visualization configuration
     */
    struct VisualizationConfig {
        VisualizationType type = VisualizationType::NEURAL_NETWORK;
        std::string title = "NLM Visualization";
        std::string outputPath = ".";
        double width = 1920.0;
        double height = 1080.0;
        double updateRate = 30.0;
        bool interactive = true;
        bool showLabels = true;
        bool showConnections = true;
        bool animate = false;
        double animationSpeed = 1.0;
        std::string colorScheme = "default"; // default, rainbow, cool, hot
        bool enableExport = true;
        bool enableWebExport = false;
        int webPort = 8080;
        std::string format; // PNG, PDF, SVG, HTML
        bool highQuality = false;
    };

    /**
     * Visualization statistics
     */
    struct VisualizationStats {
        double generationTime = 0.0;
        double renderTime = 0.0;
        uint64_t neuronsVisualized = 0;
        uint64_t synapsesVisualized = 0;
        uint64_t spikesRecorded = 0;
        uint64_t regionsRendered = 0;
        double memoryUsage = 0.0;
        int fps = 0;
    };

    /**
     * Visualization constructor
     */
    Visualizer();
    
    /**
     * Visualization constructor with configuration
     */
    explicit Visualizer(const VisualizationConfig& config);
    
    /**
     * Visualization destructor
     */
    ~Visualizer();

    /**
     * Configure visualization parameters
     */
    void configure(const VisualizationConfig& config);
    
    /**
     * Initialize visualization system
     */
    bool initialize();
    
    /**
     * Update visualization with new data
     */
    void update(const Brain& brain, double timeStep);
    
    /**
     * Render current visualization state
     */
    bool render();
    
    /**
     * Export visualization to file
     */
    bool exportVisualization(const std::string& filename) const;
    
    /**
     * Generate web-based visualization
     */
    bool generateWebVisualization(const std::string& filename) const;
    
    /**
     * Start real-time visualization
     */
    void startRealTimeVisualization();
    
    /**
     * Stop visualization
     */
    void stop();
    
    /**
     * Check if visualization is active
     */
    bool isActive() const;
    
    /**
     * Get visualization statistics
     */
    VisualizationStats getStats() const;
    
    /**
     * Set data source for visualization
     */
    void setDataSource(std::function<void(Brain&)> dataSource);
    
    /**
     * Add visualization filter
     */
    void addFilter(std::function<bool(const Neuron&)> filter);
    
    /**
     * Remove all filters
     */
    void clearFilters();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Suppress warnings about unused private members
    Visualizer(const Visualizer&) = delete;
    Visualizer& operator=(const Visualizer&) = delete;
};

} // namespace nlm
