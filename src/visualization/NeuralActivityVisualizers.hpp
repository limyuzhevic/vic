#pragma once

#include "VisualizationEngine.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/SpikeEvent.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <deque>
#include <algorithm>
#include <numeric>

namespace nlm {

/**
 * @file SpikeTrainsVisualizer.hpp
 * 
 * Spike train visualization for NLM neural activity monitoring.
 * 
 * This file provides the SpikeTrainsVisualizer class, which renders
 * neural spike activity as raster plots showing individual neuron
 * firing patterns over time. This visualization is essential for
 * understanding neural coding, burst patterns, and overall network
 * activity dynamics.
 * 
 * Key features:
 * - Raster plot visualization of spike trains
 * - Multiple neuron groups and coloring
 * - Time-synchronized display
 * - Interactive neuron selection
 * - Spike statistics overlay
 * - Real-time updates for live monitoring
 * - Adjustable time scales
 * - Custom neuron filtering
 * 
 * @note This visualizer works with spike data from SpikeSystem
 * @see SpikeEvent - Individual spike event structure
 * @see SpikeSystem - Source of spike data
 * @see VisualizationEngine - Main visualization framework
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class SpikeTrainsVisualizer : public Visualizer {
public:
    SpikeTrainsVisualizer();
    ~SpikeTrainsVisualizer() override;
    
    // Disable copying - PImpl idiom
    SpikeTrainsVisualizer(const SpikeTrainsVisualizer&) = delete;
    SpikeTrainsVisualizer& operator=(const SpikeTrainsVisualizer&) = delete;
    SpikeTrainsVisualizer(SpikeTrainsVisualizer&&) noexcept;
    SpikeTrainsVisualizer& operator=(SpikeTrainsVisualizer&&) noexcept;
    
    /**
     * @brief Initialize the spike trains visualizer
     * 
     * @param config Visualization configuration
     * @return true if initialization successful
     */
    bool initialize(const VisualizationConfig& config) override;
    
    /**
     * @brief Update with new spike data
     * 
     * @param data Spike data from visualization data container
     * @post Spike trains updated and ready for rendering
     */
    void update(const VisualizationData& data) override;
    
    /**
     * @brief Render spike trains as raster plot
     * 
     * @param output Whether to output to display/file
     */
    void render(bool output = true) override;
    
    /**
     * @brief Configure visualizer parameters
     * 
     * @param config New configuration
     */
    void configure(const VisualizationConfig& config) override;
    
    /**
     * @brief Get visualizer type
     * 
     * @return VisualizationType::NeuralActivity
     */
    VisualizationType getType() const override;
    
    /**
     * @brief Get visualizer name
     * 
     * @return "SpikeTrainsVisualizer"
     */
    std::string getName() const override;
    
    /**
     * @brief Add neurons to display
     * 
     * @param neuronIds Vector of neuron IDs to show
     */
    void setNeuronsToDisplay(const std::vector<int64_t>& neuronIds);
    
    /**
     * @brief Add neuron group for organized display
     * 
     * @param regionId Neural region ID
     * @param neuronIds Vector of neuron IDs in this region
     * @param color Color for this group
     */
    void addNeuronGroup(int64_t regionId, const std::vector<int64_t>& neuronIds, 
                       const std::string& color = "#000000");
    
    /**
     * @brief Get spike statistics
     * 
     * @return Statistics including firing rates, bursts, etc.
     */
    std::unordered_map<std::string, double> getSpikeStatistics() const;
    
    /**
     * @brief Export spike data
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportData(const std::string& format, const std::string& filepath) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @file MembranePotentialVisualizer.hpp
 * 
 * Membrane potential visualization for neuron voltage monitoring.
 * 
 * This file provides the MembranePotentialVisualizer class, which
 * renders the time-varying membrane potentials of individual neurons
 * or populations. This visualization is crucial for understanding
 * neuron integration, threshold crossing, and overall neuronal
 * computational properties.
 * 
 * Key features:
 * - Continuous membrane potential traces
 * - Multiple neuron overlay capability
 * - Voltage threshold visualization
 * - Refractory period visualization
 * - Rate-based and voltage-based coloring
 * - Real-time voltage monitoring
 * - Adjustable time windows
 * - Statistical overlays (mean, std dev)
 * 
 * @note This visualizer works with neuron state data
 * @see Neuron::getMembranePotential() - Data source
 * @see NeuronState - Complete neuron state structure
 * @see VisualizationEngine - Main visualization framework
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class MembranePotentialVisualizer : public Visualizer {
public:
    MembranePotentialVisualizer();
    ~MembranePotentialVisualizer() override;
    
    // Disable copying - PImpl idiom
    MembranePotentialVisualizer(const MembranePotentialVisualizer&) = delete;
    MembranePotentialVisualizer& operator=(const MembranePotentialVisualizer&) = delete;
    MembranePotentialVisualizer(MembranePotentialVisualizer&&) noexcept;
    MembranePotentialVisualizer& operator=(MembranePotentialVisualizer&&) noexcept;
    
    /**
     * @brief Initialize the membrane potential visualizer
     * 
     * @param config Visualization configuration
     * @return true if initialization successful
     */
    bool initialize(const VisualizationConfig& config) override;
    
    /**
     * @brief Update with new voltage data
     * 
     * @param data Membrane potential data from visualization data container
     * @post Voltage traces updated and ready for rendering
     */
    void update(const VisualizationData& data) override;
    
    /**
     * @brief Render voltage traces
     * 
     * @param output Whether to output to display/file
     */
    void render(bool output = true) override;
    
    /**
     * @brief Configure visualizer parameters
     * 
     * @param config New configuration
     */
    void configure(const VisualizationConfig& config) override;
    
    /**
     * @brief Get visualizer type
     * 
     * @return VisualizationType::NeuralActivity
     */
    VisualizationType getType() const override;
    
    /**
     * @brief Get visualizer name
     * 
     * @return "MembranePotentialVisualizer"
     */
    std::string getName() const override;
    
    /**
     * @brief Add neurons to display
     * 
     * @param neuronIds Vector of neuron IDs to show
     */
    void setNeuronsToDisplay(const std::vector<int64_t>& neuronIds);
    
    /**
     * @brief Set voltage threshold to highlight
     * 
     * @param threshold Voltage threshold (mV)
     */
    void setThresholdHighlight(float threshold);
    
    /**
     * @brief Get voltage statistics
     * 
     * @return Statistics including mean, std dev, max, min voltages
     */
    std::unordered_map<std::string, double> getVoltageStatistics() const;
    
    /**
     * @brief Export voltage data
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportData(const std::string& format, const std::string& filepath) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @file SynapticActivityVisualizer.hpp
 * 
 * Synaptic activity visualization for synaptic dynamics monitoring.
 * 
 * This file provides the SynapticActivityVisualizer class, which
 * visualizes synaptic activity, including synaptic currents, weights,
 * and connection patterns. This visualization is essential for
 * understanding synaptic dynamics, plasticity, and network communication.
 * 
 * Key features:
 * - Synaptic current visualization over time
 * - Synaptic weight matrix display
 * * Connectivity pattern visualization
 * - Synaptic delay visualization
 * - Plasticity state visualization
 * - Real-time synaptic monitoring
 * - Adjustable synaptic filtering
 * - Statistical synaptic analysis
 * 
 * @note This visualizer works with synaptic data from Brain system
 * @see Synapse - Synaptic connection structure
 * @see STDP - Spike-timing-dependent plasticity
 * @see VisualizationEngine - Main visualization framework
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class SynapticActivityVisualizer : public Visualizer {
public:
    SynapticActivityVisualizer();
    ~SynapticActivityVisualizer() override;
    
    // Disable copying - PImpl idiom
    SynapticActivityVisualizer(const SynapticActivityVisualizer&) = delete;
    SynapticActivityVisualizer& operator=(const SynapticActivityVisualizer&) = delete;
    SynapticActivityVisualizer(SynapticActivityVisualizer&&) noexcept;
    SynapticActivityVisualizer& operator=(SynapticActivityVisualizer&&) noexcept;
    
    /**
     * @brief Initialize the synaptic activity visualizer
     * 
     * @param config Visualization configuration
     * @return true if initialization successful
     */
    bool initialize(const VisualizationConfig& config) override;
    
    /**
     * @brief Update with new synaptic data
     * 
     * @param data Synaptic data from visualization data container
     * @post Synaptic activity updated and ready for rendering
     */
    void update(const VisualizationData& data) override;
    
    /**
     * @brief Render synaptic activity
     * 
     * @param output Whether to output to display/file
     */
    void render(bool output = true) override;
    
    /**
     * @brief Configure visualizer parameters
     * 
     * @param config New configuration
     */
    void configure(const VisualizationConfig& config) override;
    
    /**
     * @brief Get visualizer type
     * 
     * @return VisualizationType::NeuralActivity
     */
    VisualizationType getType() const override;
    
    /**
     * @brief Get visualizer name
     * 
     * @return "SynapticActivityVisualizer"
     */
    std::string getName() const override;
    
    /**
     * @brief Set synaptic filter for display
     * 
     * @param minWeight Minimum synaptic weight to show
     * @param maxWeight Maximum synaptic weight to show
     */
    void setSynapticFilter(float minWeight, float maxWeight);
    
    /**
     * @brief Get synaptic statistics
     * 
     * @return Statistics including mean weight, total connections, etc.
     */
    std::unordered_map<std::string, double> getSynapticStatistics() const;
    
    /**
     * @brief Export synaptic data
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportData(const std::string& format, const std::string& filepath) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm