#pragma once

#include "VisualizationEngine.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/RegionConnection.hpp"
#include <map>
#include <set>

namespace nlm {

/**
 * @file NetworkVisualizer.hpp
 * 
 * Network topology visualization for brain connectivity analysis.
 * 
 * This file provides the NetworkVisualizer class, which renders
 * the structural and functional connectivity between neural regions.
 * This visualization is essential for understanding how different
 * brain regions interact, the flow of information through the network,
 * and the emergence of large-scale network dynamics.
 * 
 * Key features:
 * - Directed and undirected network graphs
 * - Multiple layout algorithms (force-directed, circular, hierarchical)
 * - Node coloring by region type or activity level
 * - Edge thickness by connection strength
 * - Interactive node and edge selection
 * - Network statistics and metrics overlay
 * - Real-time connectivity updates
 * - Adjustable graph resolution
 * - Support for multi-layer networks
 * 
 * @note This visualizer works with NeuralRegion and RegionConnection objects
 * @see NeuralRegion - Brain regions to visualize
 * @see RegionConnection - Connections between regions
 * @see VisualizationEngine - Main visualization framework
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class NetworkVisualizer : public Visualizer {
public:
    NetworkVisualizer();
    ~NetworkVisualizer() override;
    
    // Disable copying - PImpl idiom
    NetworkVisualizer(const NetworkVisualizer&) = delete;
    NetworkVisualizer& operator=(const NetworkVisualizer&) = delete;
    NetworkVisualizer(NetworkVisualizer&&) noexcept;
    NetworkVisualizer& operator=(NetworkVisualizer&&) noexcept;
    
    /**
     * @brief Initialize the network visualizer
     * 
     * @param config Visualization configuration
     * @return true if initialization successful
     */
    bool initialize(const VisualizationConfig& config) override;
    
    /**
     * @brief Update with new connectivity data
     * 
     * @param data Connectivity data from visualization data container
     * @post Network topology updated and ready for rendering
     */
    void update(const VisualizationData& data) override;
    
    /**
     * @brief Render network graph
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
     * @return VisualizationType::Connectivity
     */
    VisualizationType getType() const override;
    
    /**
     * @brief Get visualizer name
     * 
     * @return "NetworkVisualizer"
     */
    std::string getName() const override;
    
    /**
     * @brief Add brain to visualization
     * 
     * @param brain Reference to brain system
     */
    void setBrain(const Brain& brain);
    
    /**
     * @brief Add region to display
     * 
     * @param regionId Region identifier
     */
    void addRegionToDisplay(RegionId regionId);
    
    /**
     * @brief Add connection to display
     * 
     * @param connectionId Connection identifier
     */
    void addConnectionToDisplay(ConnectionId connectionId);
    
    /**
     * @brief Set layout algorithm
     * 
     * @param layoutName Layout algorithm name
     */
    void setLayoutAlgorithm(const std::string& layoutName);
    
    /**
     * @brief Get network statistics
     * 
     * @return Statistics including nodes, edges, clustering, etc.
     */
    std::unordered_map<std::string, double> getNetworkStatistics() const;
    
    /**
     * @brief Export network data
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
 * @file RegionVisualizer.hpp
 * 
 * Brain region visualization for anatomical and functional mapping.
 * 
 * This file provides the RegionVisualizer class, which renders
 * individual neural regions with detailed anatomical information,
 * functional properties, and metabolic characteristics. This visualization
 * is essential for mapping brain structure, understanding regional
 * specialization, and studying how different regions contribute to
 * overall brain function.
 * 
 * Key features:
 * - 3D anatomical models of brain regions
 * - Regional properties visualization (size, activity, metabolism)
 * - Interactive region selection and inspection
 * - Multi-region comparison views
 * - Anatomical atlas overlays
 * - Functional connectivity per region
 * - Real-time region state updates
 * - Adjustable region detail levels
 * - Custom region styling
 * 
 * @note This visualizer works with NeuralRegion objects
 * @see NeuralRegion - Brain regions to visualize
 * @see VisualizationEngine - Main visualization framework
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class RegionVisualizer : public Visualizer {
public:
    RegionVisualizer();
    ~RegionVisualizer() override;
    
    // Disable copying - PImpl idiom
    RegionVisualizer(const RegionVisualizer&) = delete;
    RegionVisualizer& operator=(const RegionVisualizer&) = delete;
    RegionVisualizer(RegionVisualizer&&) noexcept;
    RegionVisualizer& operator=(RegionVisualizer&&) noexcept;
    
    /**
     * @brief Initialize the region visualizer
     * 
     * @param config Visualization configuration
     * @return true if initialization successful
     */
    bool initialize(const VisualizationConfig& config) override;
    
    /**
     * @brief Update with new region data
     * 
     * @param data Region data from visualization data container
     * @post Region information updated and ready for rendering
     */
    void update(const VisualizationData& data) override;
    
    /**
     * @brief Render brain regions
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
     * @return VisualizationType::Connectivity
     */
    VisualizationType getType() const override;
    
    /**
     * @brief Get visualizer name
     * 
     * @return "RegionVisualizer"
     */
    std::string getName() const override;
    
    /**
     * @brief Add brain to visualization
     * 
     * @param brain Reference to brain system
     */
    void setBrain(const Brain& brain);
    
    /**
     * @brief Add region to display
     * 
     * @param regionId Region identifier
     */
    void addRegionToDisplay(RegionId regionId);
    
    /**
     * @brief Set 3D view parameters
     * 
     * @param cameraPosition Camera position
     * @param lookAt Look-at point
     * @param upVector Up vector
     */
    void set3DView(const std::array<float, 3>& cameraPosition,
                  const std::array<float, 3>& lookAt,
                  const std::array<float, 3>& upVector);
    
    /**
     * @brief Get region statistics
     * 
     * @return Statistics including size, activity, connections, etc.
     */
    std::unordered_map<std::string, double> getRegionStatistics() const;
    
    /**
     * @brief Export region data
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
 * @file PopulationVisualizer.hpp
 * 
 * Neural population visualization for group activity analysis.
 * 
 * This file provides the PopulationVisualizer class, which visualizes
 * populations of neurons organized by type, function, or connectivity
 * patterns. This visualization is essential for understanding how
 * neural populations work together to encode information, generate
 * behavior, and support cognitive functions.
 * 
 * Key features:
 * - Population-level activity visualization
 * - Population-type classification (sensory, motor, association)
 * - Population size and density visualization
 * - Population connectivity patterns
 * - Real-time population monitoring
 * - Population statistics and metrics
 * - Interactive population selection
 * - Population-level plasticity visualization
 * - Adjustable population resolution
 * 
 * @note This visualizer works with NeuralPopulation objects
 * @see NeuralPopulation - Neural populations to visualize
 * @see VisualizationEngine - Main visualization framework
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class PopulationVisualizer : public Visualizer {
public:
    PopulationVisualizer();
    ~PopulationVisualizer() override;
    
    // Disable copying - PImpl idiom
    PopulationVisualizer(const PopulationVisualizer&) = delete;
    PopulationVisualizer& operator=(const PopulationVisualizer&) = delete;
    PopulationVisualizer(PopulationVisualizer&&) noexcept;
    PopulationVisualizer& operator=(PopulationVisualizer&&) noexcept;
    
    /**
     * @brief Initialize the population visualizer
     * 
     * @param config Visualization configuration
     * @return true if initialization successful
     */
    bool initialize(const VisualizationConfig& config) override;
    
    /**
     * @brief Update with new population data
     * 
     * @param data Population data from visualization data container
     * @post Population information updated and ready for rendering
     */
    void update(const VisualizationData& data) override;
    
    /**
     * @brief Render population visualization
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
     * @return VisualizationType::Connectivity
     */
    VisualizationType getType() const override;
    
    /**
     * @brief Get visualizer name
     * 
     * @return "PopulationVisualizer"
     */
    std::string getName() const override;
    
    /**
     * @brief Add brain to visualization
     * 
     * @param brain Reference to brain system
     */
    void setBrain(const Brain& brain);
    
    /**
     * @brief Add population to display
     * 
     * @param populationId Population identifier
     */
    void addPopulationToDisplay(PopulationId populationId);
    
    /**
     * @brief Set population type filter
     * 
     * @param type Population type (e.g., "sensory", "motor", "association")
     */
    void setPopulationTypeFilter(const std::string& type);
    
    /**
     * @brief Get population statistics
     * 
     * @return Statistics including size, activity, density, etc.
     */
    std::unordered_map<std::string, double> getPopulationStatistics() const;
    
    /**
     * @brief Export population data
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