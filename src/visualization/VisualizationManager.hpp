#pragma once

/**
 * @file VisualizationManager.hpp
 * @brief Advanced visualization system for NLM brain simulation
 * 
 * This file defines the VisualizationManager class, providing a comprehensive
 * visualization system for the NLM brain simulation. It supports multiple
 * visualization types including 2D network graphs, spike rasters, activity
 * plots, and weight matrices with real-time rendering capabilities.
 * 
 * @author NLM Research Team
 * @date Phase 6 Enhancement
 * @version 2.0
 * 
 * @features
 * - Multi-mode visualization (network, activity, spikes, weights)
 * - Real-time rendering with configurable update rates
 * - Export capabilities (PNG, SVG, JSON)
 * - Performance monitoring integration
 * - Scalable vector graphics support
 * - Multiple output backends (SDL, OpenGL, Software)
 */

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../performance/Performance.hpp"

namespace nlm {

/**
 * @enum VisualizationMode
 * @brief Enumeration of available visualization modes
 * 
 * @enumclass VisualizationMode
 * 
 * @var VisualizationMode::NETWORK Network topology visualization
 *       showing neurons as nodes and synapses as edges
 * 
 * @var VisualizationMode::ACTIVITY Neural activity over time visualization
 * 
 * @var VisualizationMode::SPIKE Raster plot of spike events over time
 * 
 * @var VisualizationMode::WEIGHTS Synaptic weight matrix visualization
 * 
 * @var VisualizationMode::BRAINSTATE Integrated brain state overview
 * 
 * @var VisualizationMode::PERFORMANCE Performance metrics visualization
 */
enum class VisualizationMode {
    NETWORK,
    ACTIVITY,
    SPIKE,
    WEIGHTS,
    BRAINSTATE,
    PERFORMANCE
};

/**
 * @struct VisualizationData
 * @brief Container for visualization data and metadata
 * 
 * This structure holds all data needed for a single visualization frame,
 * including the visualization mode, data arrays, metadata, and rendering
 * parameters. It is designed for efficient memory access and minimal copying.
 */
struct VisualizationData {
    VisualizationMode mode;                    ///< Current visualization mode
    double timestamp;                           ///< Simulation time of data capture
    SimulationStep step;                        ///< Simulation step of data capture
    
    // Network visualization data
    std::vector<nlm::NeuronId> neuronIds;        ///< Neuron identifiers for network view
    std::vector<std::pair<nlm::NeuronId, nlm::NeuronId>> connections; ///< Synapse connections
    std::vector<float> neuronExcitability;       ///< Current neuron excitability (0-1)
    std::vector<float> neuronFiringRates;       ///< Current firing rates (Hz)
    
    // Activity visualization data
    std::vector<std::vector<float>> activityHeatmap; ///< 2D activity heatmap data
    std::vector<float> populationActivity;     ///< Activity per neuron population
    std::vector<float> regionActivity;         ///< Activity per brain region
    
    // Spike visualization data
    std::vector<std::pair<SimulationStep, nlm::NeuronId>> spikeEvents; ///< Spike timestamps and source neurons
    std::map<SimulationStep, std::vector<nlm::NeuronId>> spikesByStep; ///< Spikes grouped by step
    
    // Weight visualization data
    std::vector<std::vector<float>> weightMatrix; ///< Synaptic weight matrix (neuron x neuron)
    std::vector<std::vector<float>> connectionStrength; ///< Normalized connection strengths
    
    // Brain state data
    std::vector<float> brainStateVector;        ///< Compressive brain state representation
    std::map<std::string, float> systemMetrics; ///< Various system metrics
    
    // Performance data
    PerformanceMetrics perfMetrics;              ///< Performance monitoring data
    
    // Metadata
    std::string description;                    ///< Human-readable description
    std::chrono::system_clock::time_point captureTime; ///< When data was captured
};

/**
 * @class VisualizationManager
 * @brief Comprehensive visualization system for NLM brain simulation
 * 
 * The VisualizationManager provides a complete visualization framework for
 * the NLM brain simulation, supporting real-time rendering, data export,
 * and multiple visualization modes. It integrates with the brain simulation
 * to provide insights into neural dynamics and system performance.
 * 
 * @details The manager uses a pipeline architecture:
 * 1. Data collection: Gathers visualization data from brain simulation
 * 2. Processing: Transforms and filters data for rendering
 * 3. Rendering: Creates visual representations using appropriate backends
 * 4. Output: Saves visualizations to files or displays them
 * 
 * The system supports multiple output backends (SDL, OpenGL, Software) and
 * multiple export formats (PNG, SVG, JSON). It also provides performance
 * monitoring integration to ensure visualizations don't impact simulation
 * performance.
 * 
 * @note The visualization system is designed to be non-blocking and
 *       efficient, with configurable update rates to balance visual fidelity
 *       with simulation performance.
 */
class VisualizationManager {
public:
    /**
     * @brief Construct visualization manager with specified configuration
     * 
     * @param targetFPS Frames per second for real-time visualization
     *                  (default: 30.0)
     * @param backend Visualization backend (default: "auto-detect")
     * @param exportDir Directory for saved visualizations (default: ".")
     */
    VisualizationManager(double targetFPS = 30.0, 
                         const std::string& backend = "auto-detect",
                         const std::string& exportDir = ".");
    
    /**
     * @brief Destructor for cleanup
     * 
     * Properly cleans up all resources including rendering contexts,
     * file handles, and allocated memory.
     */
    ~VisualizationManager();
    
    /**
     * @brief Disable copying to prevent resource conflicts
     * 
     * @param other The VisualizationManager to copy from
     * @throw std::runtime_error if copying is attempted
     */
    VisualizationManager(const VisualizationManager&) = delete;
    VisualizationManager& operator=(const VisualizationManager&) = delete;
    
    /**
     * @brief Enable move construction for efficient transfer
     * 
     * @param other The VisualizationManager to move from
     */
    VisualizationManager(VisualizationManager&&) noexcept;
    
    /**
     * @brief Enable move assignment for efficient transfer
     * 
     * @param other The VisualizationManager to move from
     * @return Reference to this VisualizationManager after assignment
     */
    VisualizationManager& operator=(VisualizationManager&&) noexcept;
    
    /**
     * @brief Initialize visualization system
     * 
     * @param brain Reference to the brain simulation for data collection
     * @return true if initialization succeeded, false otherwise
     * @throws std::runtime_error if initialization fails
     * 
     * @details This method must be called before any visualization operations.
     * It initializes the rendering backend, allocates resources, and prepares
     * data structures for visualization.
     */
    bool initialize(const Brain& brain);
    
    /**
     * @brief Update visualization data for current simulation state
     * 
     * @param brain Reference to the brain simulation
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * @return true if update succeeded, false otherwise
     * 
     * @details This method collects data from the brain simulation, processes
     * it for visualization, and schedules rendering. It should be called once
     * per simulation step.
     */
    bool update(const Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Render current visualization frame
     * 
     * @param brain Reference to the brain simulation
     * @return true if rendering succeeded, false otherwise
     * 
     * @details This method creates the visual representation of the current
     * data and displays it or saves it to file, depending on configuration.
     * Rendering is typically called at a fixed frame rate.
     */
    bool render(const Brain& brain);
    
    /**
     * @brief Close visualization and clean up resources
     * 
     * @details This method properly shuts down the visualization system,
     * releasing all allocated resources and closing any open files or
     * rendering contexts.
     */
    void close();
    
    /**
     * @brief Set the current visualization mode
     * 
     * @param mode The visualization mode to set
     * @throws std::invalid_argument if mode is not supported
     */
    void setVisualizationMode(VisualizationMode mode);
    
    /**
     * @brief Get the current visualization mode
     * 
     * @return The current visualization mode
     */
    VisualizationMode getVisualizationMode() const;
    
    /**
     * @brief Check if visualization is active and ready
     * 
     * @return true if visualization system is initialized and active
     */
    bool isActive() const;
    
    /**
     * @brief Set visualization update rate
     * 
     * @param fps Frames per second for visualization updates
     * @throws std::invalid_argument if fps is not positive
     */
    void setUpdateRate(double fps);
    
    /**
     * @brief Get current visualization update rate
     * 
     * @return Current frames per second
     */
    double getUpdateRate() const;
    
    /**
     * @brief Set visualization backend
     * 
     * @param backend Name of visualization backend
     * @throws std::invalid_argument if backend is not supported
     */
    void setBackend(const std::string& backend);
    
    /**
     * @brief Get current visualization backend
     * 
     * @return Name of current visualization backend
     */
    std::string getBackend() const;
    
    /**
     * @brief Set output directory for saved visualizations
     * 
     * @param exportDir Path to directory for saved visualizations
     */
    void setExportDirectory(const std::string& exportDir);
    
    /**
     * @brief Get output directory for saved visualizations
     * 
     * @return Path to current export directory
     */
    std::string getExportDirectory() const;
    
    /**
     * @brief Export current visualization to file
     * 
     * @param filename Base filename for export (without extension)
     * @param format Export format (png, svg, json)
     * @return true if export succeeded, false otherwise
     * @throws std::invalid_argument if format is not supported
     * 
     * @details This method saves the current visualization data to a file
     * in the specified format. Supported formats include:
     * - PNG: Bitmap image format for quick sharing
     * - SVG: Scalable vector graphics for web display
     * - JSON: Data format for programmatic access
     */
    bool exportVisualization(const std::string& filename, 
                           const std::string& format = "png");
    
    /**
     * @brief Add a visualization overlay with custom data
     * 
     * @param overlayData Visualization data to overlay
     * @return true if overlay was added successfully
     * 
     * @details Overlays are additional visualization layers that can be
     * combined with the main visualization. They are useful for:
     * - Highlighting specific neurons or synapses
     * - Showing simulation metrics alongside neural activity
     * - Adding reference data or annotations
     */
    bool addOverlay(const VisualizationData& overlayData);
    
    /**
     * @brief Remove all visualization overlays
     */
    void clearOverlays();
    
    /**
     * @brief Enable or disable real-time rendering
     * 
     * @param enabled true to enable real-time rendering, false for export-only mode
     */
    void setRealTimeRendering(bool enabled);
    
    /**
     * @brief Check if real-time rendering is enabled
     * 
     * @return true if real-time rendering is enabled
     */
    bool isRealTimeRendering() const;
    
    /**
     * @brief Set visualization quality settings
     * 
     * @param quality Quality level (0-100, higher = better quality)
     * @throws std::invalid_argument if quality is not in valid range
     */
    void setQuality(int quality);
    
    /**
     * @brief Get current visualization quality setting
     * 
     * @return Current quality level (0-100)
     */
    int getQuality() const;
    
    /**
     * @brief Get current visualization data
     * 
     * @return Reference to current visualization data
     */
    const VisualizationData& getCurrentData() const;
    
    /**
     * @brief Get last successful visualization frame timestamp
     * 
     * @return Timestamp of last visualization frame
     */
    Timestamp getLastRenderTime() const;
    
    /**
     * @brief Get number of frames rendered
     * 
     * @return Total number of frames rendered since initialization
     */
    uint64_t getFrameCount() const;
    
    /**
     * @brief Check if performance is acceptable for current settings
     * 
     * @return true if performance is within acceptable bounds
     */
    bool isPerformanceAcceptable() const;
    
    /**
     * @brief Get performance statistics
     * 
     * @return Performance monitoring data
     */
    PerformanceMetrics getPerformanceStats() const;
    
    /**
     * @brief Set callback for visualization events
     * 
     * @param callback Function to call for visualization events
     */
    void setEventCallback(std::function<void(VisualizationEvent)> callback);
    
    /**
     * @brief Set callback for rendering errors
     * 
     * @param callback Function to call for rendering errors
     */
    void setErrorCallback(std::function<void(const std::string&)> callback);
    
    /**
     * @brief Get list of available visualization backends
     * 
     * @return Vector of available backend names
     */
    static std::vector<std::string> getAvailableBackends();
    
    /**
     * @brief Get list of supported export formats
     * 
     * @return Vector of supported format names
     */
    static std::vector<std::string> getSupportedFormats();
    
private:
    /**
     * @struct Impl
     * @brief Private implementation details for VisualizationManager
     * 
     * @details This structure contains all implementation details including:
     * - Rendering backend implementation
     * - Data processing pipeline
     * - File I/O operations
     * - Performance monitoring
     * - Event handling
     */
    struct Impl;
    
    std::unique_ptr<Impl> pImpl; ///< Private implementation pointer
    
    // Event types for visualization system
    enum class VisualizationEvent {
        FRAME_RENDERED,      ///< A visualization frame was rendered
        ERROR_OCCURRED,      ///< A visualization error occurred
        EXPORT_COMPLETED,    ///< A visualization export completed
        BACKEND_CHANGED,     ///< Visualization backend changed
        PERFORMANCE_WARNING  ///< Performance warning triggered
    };
};

} // namespace nlm