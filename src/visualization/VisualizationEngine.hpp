#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <optional>
#include <thread>
#include <mutex>
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SpikeEvent.hpp"

namespace nlm {

/**
 * @file VisualizationEngine.hpp
 * 
 * Comprehensive visualization and debugging engine for NLM
 * 
 * This file provides a unified visualization engine that integrates
 * neural activity visualization, brain connectivity visualization,
 * simulation results visualization, debugging tools, performance
 * profiling, analysis tools, and interactive capabilities into a
 * cohesive framework for comprehensive NLM system monitoring and
 * understanding.
 * 
 * Key features:
 * - Multiple visualization modes (2D/3D, interactive, real-time)
 * - Neural activity visualization (spike trains, membrane potentials)
 * - Brain connectivity visualization (networks, regions, populations)
 * - Simulation results visualization (metrics, statistics, time series)
 * - Real-time monitoring and debugging tools
 * - Performance profiling and analysis tools
 * - Interactive visualization interfaces
 * - Data export and analysis tools
 * - Thread-safe for real-time capabilities
 * - Extensible architecture for plugin support
 * 
 * @note This engine is designed to handle large-scale simulations
 *       efficiently with streaming data support
 * @note Integrates with existing NLM logging and configuration systems
 * @note Provides both high-level APIs and low-level hooks for customization
 * 
 * @see VisualizationInterface - Base visualization interface
 * @see RealTimeMonitor - Real-time monitoring system
 * @see PerformanceProfiler - Performance profiling tools
 * @see DataAnalyzer - Analysis and statistics tools
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @enum VisualizationMode
 * 
 * Visualization rendering modes supported by the engine.
 * 
 * @note Modes can be combined for enhanced visualization
 * @see VisualizationEngine::setVisualizationMode()
 */
enum class VisualizationMode {
    /** Basic 2D rendering with minimal overhead */
    Basic2D = 0,
    /** Enhanced 2D with annotations and additional data */
    Enhanced2D = 1,
    /** Full 3D visualization with spatial relationships */
    ThreeD = 2,
    /** Interactive mode with user manipulation capabilities */
    Interactive = 3,
    /** Real-time streaming mode for live updates */
    RealTime = 4,
    /** Scientific visualization mode for research analysis */
    Scientific = 5
};

/**
 * @enum VisualizationType
 * 
 * Specific visualization types within the engine.
 * 
 * @see VisualizationEngine::createVisualizer()
 */
enum class VisualizationType {
    /** Neural activity visualization */
    NeuralActivity = 0,
    /** Brain connectivity visualization */
    Connectivity = 1,
    /** Simulation results visualization */
    SimulationResults = 2,
    /** Performance metrics visualization */
    Performance = 3,
    /** Debug monitoring visualization */
    Debug = 4,
    /** Custom visualization */
    Custom = 99
};

/**
 * @struct VisualizationConfig
 * 
 * Configuration for visualization components.
 * 
 * @note Used by Visualizer base class and all derived visualizers
 * @see VisualizationEngine::configureVisualizer()
 */
struct VisualizationConfig {
    /** Visualization type */
    VisualizationType type;
    
    /** Visualization mode */
    VisualizationMode mode;
    
    /** Update frequency in Hz */
    double updateFrequency;
    
    /** Whether visualization is enabled */
    bool enabled;
    
    /** Maximum data points to retain */
    size_t maxDataPoints;
    
    /** Color scheme name */
    std::string colorScheme;
    
    /** Output file prefix for exports */
    std::string outputPrefix;
    
    /** Custom parameters */
    std::unordered_map<std::string, std::string> parameters;
    
    /** Whether to show legends */
    bool showLegend;
    
    /** Whether to show grid */
    bool showGrid;
    
    /** Line width for plots */
    float lineWidth;
    
    /** Point size for scatter plots */
    float pointSize;
};

/**
 * @class VisualizationData
 * 
 * Container for visualization data with type-safe storage.
 * 
 * This class provides a unified interface for storing and accessing
 * different types of visualization data. It supports efficient data
 * management with automatic cleanup and thread-safe operations.
 * 
 * @note Supports spike data, membrane potentials, synaptic weights,
 *       connectivity matrices, and performance metrics
 * @see VisualizationEngine::updateData()
 * @see SpikeTrainsVisualizer - Example usage
 * 
 * @author NLM Development Team
 */
class VisualizationData {
public:
    /**
     * @enum DataType
     * 
     * Types of visualization data supported.
     * 
     * @see VisualizationData::getDataType()
     */
    enum class DataType {
        /** Time series data */
        TimeSeries,
        /** Spike train data */
        SpikeTrains,
        /** Membrane potential data */
        MembranePotential,
        /** Synaptic weight data */
        SynapticWeights,
        /** Connectivity data */
        Connectivity,
        /** Performance metrics */
        Performance,
        /** Custom data */
        Custom
    };

    VisualizationData();
    ~VisualizationData();
    
    // Disable copying - PImpl idiom
    VisualizationData(const VisualizationData&) = delete;
    VisualizationData& operator=(const VisualizationData&) = delete;
    VisualizationData(VisualizationData&&) noexcept;
    VisualizationData& operator=(VisualizationData&&) noexcept;
    
    /**
     * @brief Set data type
     * 
     * @param type Data type
     * @post Data type is set
     */
    void setType(DataType type);
    
    /**
     * @brief Get data type
     * 
     * @return Current data type
     */
    DataType getType() const;
    
    /**
     * @brief Add time series data
     * 
     * @tparam T Numeric type (float, double)
     * @param timestamp Time point
     * @param value Data value
     * @post Data point added to series
     */
    template<typename T>
    void addTimeSeriesData(double timestamp, T value);
    
    /**
     * @brief Add spike data
     * 
     * @param neuronId Neuron identifier
     * @param timestamp Spike time
     * @param synapticInput Synaptic input at time of spike
     * @post Spike event recorded
     */
    void addSpikeData(int64_t neuronId, double timestamp, float synapticInput);
    
    /**
     * @brief Add membrane potential data
     * 
     * @param neuronId Neuron identifier
     * @param timestamp Time point
     * @param potential Membrane potential (mV)
     * @post Membrane potential recorded
     */
    void addMembranePotentialData(int64_t neuronId, double timestamp, float potential);
    
    /**
     * @brief Add synaptic weight data
     * 
     * @param sourceNeuronId Source neuron identifier
     * @param targetNeuronId Target neuron identifier
     * @param weight Synaptic weight
     * @param timestamp Time of measurement
     * @post Synaptic weight recorded
     */
    void addSynapticWeightData(int64_t sourceNeuronId, int64_t targetNeuronId,
                              float weight, double timestamp);
    
    /**
     * @brief Get time series data
     * 
     * @return Vector of time-value pairs
     */
    std::vector<std::pair<double, float>> getTimeSeriesData() const;
    
    /**
     * @brief Get spike data
     * 
     * @return Vector of spike events
     */
    std::vector<std::tuple<int64_t, double, float>> getSpikeData() const;
    
    /**
     * @brief Get membrane potential data
     * 
     * @return Vector of membrane potential measurements
     */
    std::vector<std::tuple<int64_t, double, float>> getMembranePotentialData() const;
    
    /**
     * @brief Get synaptic weight data
     * 
     * @return Vector of synaptic weight measurements
     */
    std::vector<std::tuple<int64_t, int64_t, float, double>> getSynapticWeightData() const;
    
    /**
     * @brief Clear all data
     * 
     * @post All data removed, memory freed
     */
    void clear();
    
    /**
     * @brief Get data count
     * 
     * @return Number of data points
     */
    size_t size() const;
    
    /**
     * @brief Check if data is empty
     * 
     * @return true if no data present
     */
    bool empty() const;
    
    /**
     * @brief Export data to JSON format
     * 
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportToJSON(const std::string& filepath) const;
    
    /**
     * @brief Export data to CSV format
     * 
     * @param filepath Output file path
     * @param format Export format
     * @return true if export successful
     */
    bool exportToCSV(const std::string& filepath, const std::string& format = "default") const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @class Visualizer
 * 
 * Abstract base class for all visualization components.
 * 
 * This class provides the foundation for creating visualization
 * components for neural activity, connectivity, simulation results,
 * and debugging information. Each visualizer handles a specific
 * type of data and rendering approach.
 * 
 * @note Visualizers should be registered with VisualizationEngine
 *       for discovery and management
 * @see VisualizationEngine::registerVisualizer()
 * @see VisualizationConfig - Configuration structure
 * 
 * @author NLM Development Team
 */
class Visualizer {
public:
    Visualizer();
    virtual ~Visualizer();
    
    // Disable copying - PImpl idiom
    Visualizer(const Visualizer&) = delete;
    Visualizer& operator=(const Visualizer&) = delete;
    Visualizer(Visualizer&&) noexcept;
    Visualizer& operator=(Visualizer&&) noexcept;
    
    /**
     * @brief Initialize the visualizer
     * 
     * @param config Visualization configuration
     * @return true if initialization successful
     * @post Visualizer is ready for rendering
     */
    virtual bool initialize(const VisualizationConfig& config);
    
    /**
     * @brief Update visualization data
     * 
     * @param data New visualization data
     * @post Visualization updated with new data
     */
    virtual void update(const VisualizationData& data);
    
    /**
     * @brief Render the visualization
     * 
     * @param output Whether to output to display/file
     */
    virtual void render(bool output = true);
    
    /**
     * @brief Configure the visualizer
     * 
     * @param config New configuration
     * @post Visualizer reconfigured
     */
    virtual void configure(const VisualizationConfig& config);
    
    /**
     * @brief Check if visualizer is active
     * 
     * @return true if visualizer is ready
     */
    virtual bool isActive() const;
    
    /**
     * @brief Get visualizer type
     * 
     * @return Visualization type
     */
    virtual VisualizationType getType() const = 0;
    
    /**
     * @brief Get visualizer name
     * 
     * @return Human-readable name
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Set callback for interaction events
     * 
     * @param callback Function to call on interaction
     */
    void setInteractionCallback(std::function<void(const std::string&)> callback);
    
    /**
     * @brief Get statistics about rendering performance
     * 
     * @return Rendering statistics
     */
    virtual std::unordered_map<std::string, double> getPerformanceStats() const;
    
    /**
     * @brief Export visualization data
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    virtual bool exportData(const std::string& format, const std::string& filepath);

protected:
    VisualizationConfig config_;
    bool active_;
    std::function<void(const std::string&)> interactionCallback_;
};

/**
 * @class VisualizationEngine
 * 
 * Main visualization engine that coordinates all visualization
 * components and provides the unified interface for NLM visualization.
 * 
 * This engine manages multiple visualizers, handles real-time data
 * streaming, provides performance monitoring, and offers a unified
 * interface for all visualization and debugging operations.
 * 
 * Key features:
 * - Multi-visualizer architecture
 * - Real-time data processing
 * - Performance monitoring
 * - Thread-safe operations
 * - Configuration management
 * - Export capabilities
 * - Interactive support
 * 
 * @note The engine uses the singleton pattern for global access
 * @note Integrates with NLM's logging system for status reporting
 * @see Visualizer - Base visualizer class
 * @see RealTimeMonitor - Real-time monitoring system
 * 
 * @author NLM Development Team
 */
class VisualizationEngine {
public:
    VisualizationEngine();
    ~VisualizationEngine();
    
    // Disable copying - PImpl idiom
    VisualizationEngine(const VisualizationEngine&) = delete;
    VisualizationEngine& operator=(const VisualizationEngine&) = delete;
    VisualizationEngine(VisualizationEngine&&) noexcept;
    VisualizationEngine& operator=(VisualizationEngine&&) noexcept;
    
    /**
     * @brief Get global engine instance (singleton)
     * 
     * @return Reference to global engine instance
     */
    static VisualizationEngine& getInstance();
    
    /**
     * @brief Initialize the visualization engine
     * 
     * @param config Engine-wide configuration
     * @return true if initialization successful
     * @post Engine ready for visualization operations
     */
    bool initialize(const VisualizationConfig& config);
    
    /**
     * @brief Update all visualizers with new data
     * 
     * @param data Visualization data to distribute
     * @post All visualizers updated
     */
    void update(const VisualizationData& data);
    
    /**
     * @brief Render all active visualizers
     * 
     * @param output Whether to output to display/file
     * @post All visualizers rendered
     */
    void render(bool output = true);
    
    /**
     * @brief Configure engine with new settings
     * 
     * @param config New configuration
     * @post Engine reconfigured
     */
    void configure(const VisualizationConfig& config);
    
    /**
     * @brief Register a new visualizer
     * 
     * @param visualizer Unique pointer to visualizer
     * @param type Visualization type
     * @return true if registration successful
     */
    bool registerVisualizer(std::unique_ptr<Visualizer> visualizer, VisualizationType type);
    
    /**
     * @brief Unregister a visualizer
     * 
     * @param name Visualizer name
     * @return true if unregistration successful
     */
    bool unregisterVisualizer(const std::string& name);
    
    /**
     * @brief Get active visualizers
     * 
     * @param type Filter by visualization type (optional)
     * @return Vector of active visualizers
     */
    std::vector<Visualizer*> getVisualizers(VisualizationType type = VisualizationType::Custom) const;
    
    /**
     * @brief Enable/disable a visualizer
     * 
     * @param name Visualizer name
     * @param enabled New state
     * @return true if state changed
     */
    bool setVisualizerEnabled(const std::string& name, bool enabled);
    
    /**
     * @brief Get engine configuration
     * 
     * @return Current configuration
     */
    VisualizationConfig getConfig() const;
    
    /**
     * @brief Add data from brain simulation
     * 
     * @param brain Reference to brain system
     * @param timestamp Current simulation time
     * @post Data extracted and added to engine
     */
    void addBrainData(const Brain& brain, double timestamp);
    
    /**
     * @brief Add spike data
     * 
     * @param spikes Vector of spike events
     * @post Spike data added for visualization
     */
    void addSpikeData(const std::vector<SpikeEvent>& spikes);
    
    /**
     * @brief Add performance metrics data
     * 
     * @param metrics Performance metrics
     * @post Metrics added for visualization
     */
    void addPerformanceData(const std::unordered_map<std::string, double>& metrics);
    
    /**
     * @brief Get engine performance statistics
     * 
     * @return Performance statistics
     */
    std::unordered_map<std::string, double> getPerformanceStats() const;
    
    /**
     * @brief Export all visualization data
     * 
     * @param format Export format
     * @param filepath Base output file path
     * @return true if export successful
     */
    bool exportAllData(const std::string& format, const std::string& filepath);
    
    /**
     * @brief Start real-time monitoring
     * 
     * @param brain Reference to brain system
     * @param updateInterval Update interval in seconds
     * @return true if monitoring started
     */
    bool startRealTimeMonitoring(const Brain& brain, double updateInterval = 0.1);
    
    /**
     * @brief Stop real-time monitoring
     * 
     * @return true if monitoring stopped
     */
    bool stopRealTimeMonitoring();
    
    /**
     * @brief Check if engine is active
     * 
     * @return true if engine is operational
     */
    bool isActive() const;
    
    /**
     * @brief Get global visualization interface
     * 
     * @return Reference to global interface
     */
    VisualizationInterface& getVisualizationInterface();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Convenience macro for creating spike train visualizations
 * 
 * @param engine Reference to visualization engine
 * @param spikes Vector of spike events
 */
#define CREATE_SPIKE_VISUALIZATION(engine, spikes) \
    engine.getVisualizationInterface().visualizeSpikes(spikes)

/**
 * @brief Convenience macro for creating brain visualizations
 * 
 * @param engine Reference to visualization engine
 * @param brain Reference to brain system
 */
#define CREATE_BRAIN_VISUALIZATION(engine, brain) \
    engine.getVisualizationInterface().visualizeNetwork(brain)

/**
 * @brief Convenience macro for creating activity visualizations
 * 
 * @param engine Reference to visualization engine
 * @param brain Reference to brain system
 */
#define CREATE_ACTIVITY_VISUALIZATION(engine, brain) \
    engine.getVisualizationInterface().visualizeActivity(brain)

} // namespace nlm