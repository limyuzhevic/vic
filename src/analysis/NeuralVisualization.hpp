#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../visualization/VisualizationInterface.hpp"
#include "../experiments/ExperimentMetrics.hpp"

namespace nlm {

/**
 * @brief Comprehensive neural network visualization class
 * 
 * This class provides extensive visualization capabilities for neural network simulations,
 * including network topology visualization, activity visualization, weight visualization,
 * connection strength visualization, and heat map generation. Supports multiple output
 * formats (PNG, SVG, ASCII) and customizable color schemes.
 * 
 * Designed for:
 * - Research visualization - Publication-quality visualizations
 * - Debugging - Real-time monitoring and debugging
 * - Performance analysis - Bottleneck identification and optimization
 * - Statistical analysis - Hypothesis testing and validation
 * - Machine learning - Activity patterns as ML features
 * - Education - Teaching neural network concepts
 */
class NeuralVisualization {
public:
    /**
     * @brief Output format enumeration
     */
    enum class OutputFormat {
        PNG,
        SVG,
        ASCII
    };
    
    /**
     * @brief Color scheme enumeration
     */
    enum class ColorScheme {
        Default,
        Thermal,
        CoolWarm,
        Viridis,
        Plasma,
        Custom
    };

    /**
     * @brief Constructor
     * 
     * @param brain Shared pointer to the brain instance to visualize
     * @param outputFormat Output format for visualizations (default: PNG)
     * @param colorScheme Color scheme for visualizations (default: Default)
     */
    NeuralVisualization(
        std::shared_ptr<Brain> brain,
        OutputFormat outputFormat = OutputFormat::PNG,
        ColorScheme colorScheme = ColorScheme::Default
    );

    /**
     * @brief Destructor
     */
    ~NeuralVisualization();

    /**
     * @brief Copy constructor
     */
    NeuralVisualization(const NeuralVisualization& other);

    /**
     * @brief Copy assignment operator
     */
    NeuralVisualization& operator=(const NeuralVisualization& other);

    /**
     * @brief Move constructor
     */
    NeuralVisualization(NeuralVisualization&& other) noexcept;

    /**
     * @brief Move assignment operator
     */
    NeuralVisualization& operator=(NeuralVisualization&& other) noexcept;

    /**
     * @brief Get the brain being visualized
     * 
     * @return Shared pointer to the brain instance
     */
    std::shared_ptr<Brain> getBrain() const;

    /**
     * @brief Set output format
     * 
     * @param format Output format
     */
    void setOutputFormat(OutputFormat format);

    /**
     * @brief Get output format
     * 
     * @return Current output format
     */
    OutputFormat getOutputFormat() const;

    /**
     * @brief Set color scheme
     * 
     * @param scheme Color scheme
     */
    void setColorScheme(ColorScheme scheme);

    /**
     * @brief Get color scheme
     * 
     * @return Current color scheme
     */
    ColorScheme getColorScheme() const;

    /**
     * @brief Set custom color mapping
     * 
     * @param colormap Map from normalized values to RGB colors (0.0 to 1.0)
     */
    void setCustomColormap(const std::map<float, std::tuple<float, float, float>>& colormap);

    /**
     * @brief Get current custom colormap
     * 
     * @return Custom colormap
     */
    std::map<float, std::tuple<float, float, float>> getCustomColormap() const;

    /**
     * @brief Visualize network topology
     * 
     * Creates a visualization of the network topology, showing neurons, regions,
     * and connections between them. The visualization includes node sizes based
     * on neuron counts and edge widths based on connection strengths.
     * 
     * @return Visualization data (PNG/SVG bytes if format supports it, empty if ASCII)
     */
    std::vector<uint8_t> visualizeNetwork();

    /**
     * @brief Visualize current neuron activity
     * 
     * Creates a visualization of current neuron activity, showing which neurons
     * are active, their firing rates, and spatial distribution within the network.
     * 
     * @return Visualization data (PNG/SVG bytes if format supports it, empty if ASCII)
     */
    std::vector<uint8_t> visualizeActivity();

    /**
     * @brief Visualize synaptic weights
     * 
     * Creates a visualization of the synaptic weight matrix, showing the strength
     * of connections between neurons or populations. Can be displayed as a heatmap
     * or matrix plot.
     * 
     * @return Visualization data (PNG/SVG bytes if format supports it, empty if ASCII)
     */
    std::vector<uint8_t> visualizeWeights();

    /**
     * @brief Visualize connection strength
     * 
     * Creates a visualization of connection strength distribution across different
     * types of connections (excitatory, inhibitory, modulatory) and between different
     * brain regions.
     * 
     * @return Visualization data (PNG/SVG bytes if format supports it, empty if ASCII)
     */
    std::vector<uint8_t> visualizeConnections();

    /**
     * @brief Save visualization to file
     * 
     * @param filepath Path to save the visualization file
     * @param format Output format (overrides default if specified)
     * @return true if save was successful, false otherwise
     */
    bool saveVisualization(const std::string& filepath, OutputFormat format = OutputFormat::PNG);

    /**
     * @brief Generate heat map of activity
     * 
     * Generates a heat map showing the spatial distribution of neural activity,
     * with intensity representing firing rates or other activity metrics.
     * 
     * @param normalize Whether to normalize the heat map values to [0, 1]
     * @return Heat map data as 2D vector of intensity values
     */
    std::vector<std::vector<float>> getHeatMap(bool normalize = true);

    /**
     * @brief Get neuron positions for visualization
     * 
     * @return Map from neuron IDs to 2D positions (x, y coordinates)
     */
    std::map<NeuronId, std::pair<float, float>> getNeuronPositions() const;

    /**
     * @brief Get region positions for visualization
     * 
     * @return Map from region IDs to 2D positions (x, y coordinates)
     */
    std::map<RegionId, std::pair<float, float>> getRegionPositions() const;

    /**
     * @brief Set layout algorithm
     * 
     * @param layout "circular", "spring", "force-directed", "grid", or "custom"
     */
    void setLayoutAlgorithm(const std::string& layout);

    /**
     * @brief Get current layout algorithm
     * 
     * @return Current layout algorithm
     */
    std::string getLayoutAlgorithm() const;

    /**
     * @brief Enable/disable node labels
     * 
     * @param enabled Whether to show neuron/region labels
     */
    void enableLabels(bool enabled);

    /**
     * @brief Check if labels are enabled
     * 
     * @return true if labels are enabled
     */
    bool areLabelsEnabled() const;

    /**
     * @brief Enable/disable edge labels
     * 
     * @param enabled Whether to show connection strength labels
     */
    void enableEdgeLabels(bool enabled);

    /**
     * @brief Check if edge labels are enabled
     * 
     * @return true if edge labels are enabled
     */
    bool areEdgeLabelsEnabled() const;

    /**
     * @brief Set label font size
     * 
     * @param size Font size in pixels
     */
    void setLabelFontSize(int size);

    /**
     * @brief Get label font size
     */
    int getLabelFontSize() const;

    /**
     * @brief Set edge width scaling factor
     * 
     * @param factor Scaling factor for connection widths (1.0 = default)
     */
    void setEdgeWidthScaling(float factor);

    /**
     * @brief Get edge width scaling factor
     */
    float getEdgeWidthScaling() const;

    /**
     * @brief Create animated visualization sequence
     * 
     * Generates a sequence of visualizations over time, useful for understanding
     * dynamic network behavior and temporal patterns.
     * 
     * @param duration Duration of animation in seconds
     * @param fps Frames per second
     * @return Vector of visualization data (one per frame)
     */
    std::vector<std::vector<uint8_t>> createAnimation(float duration = 10.0f, float fps = 30.0f);

    /**
     * @brief Export visualization metadata
     * 
     * @return Metadata about the visualization (neuron counts, connection stats, etc.)
     */
    std::map<std::string, std::string> getMetadata() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
