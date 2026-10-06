// VisualizationTools_integration.hpp
#pragma once

// This header provides integration between VisualizationTools and the main NLM system
// It includes all necessary headers and provides helper functions for seamless integration

#include "visualization/VisualizationTools.hpp"
#include "visualization/VisualizationTools_config.hpp"
#include "brain/Brain.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Types/Types.hpp"
#include "dynamics/SpikeSystem.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Integration helper functions

/**
 * @brief Creates a complete visualization setup for the NLM system
 * 
 * @param brain Reference to the NLM brain system
 * @param outputDirectory Output directory for visualizations
 * @param enable3D Whether to enable 3D visualizations
 * @param enableVR Whether to enable VR visualizations
 * @param enableWebGL Whether to enable WebGL visualizations
 * @return std::unique_ptr<VisualizationTools> Pointer to visualization tools
 */
std::unique_ptr<VisualizationTools> createCompleteVisualizationSetup(
    const Brain& brain,
    const std::string& outputDirectory = "./visualization_output",
    bool enable3D = true,
    bool enableVR = false,
    bool enableWebGL = true
);

/**
 * @brief Creates an interactive dashboard for real-time monitoring
 * 
 * @param title Dashboard title
 * @param updateInterval Update interval in milliseconds
 * @param brain Optional reference to brain system
 * @return std::unique_ptr<VisualizationTools> Pointer to interactive dashboard
 */
std::unique_ptr<VisualizationTools> createInteractiveMonitoringDashboard(
    const std::string& title,
    double updateInterval = 1000.0,
    const Brain* brain = nullptr
);

/**
 * @brief Creates a 3D neural network visualizer
 * 
 * @param brain Reference to brain system
 * @param cameraDistance Camera distance from the network
 * @param fieldOfView Camera field of view in degrees
 * @return std::unique_ptr<VisualizationTools> Pointer to 3D visualizer
 */
std::unique_ptr<VisualizationTools> create3DNeuralNetworkVisualizer(
    const Brain& brain,
    float cameraDistance = 50.0f,
    float fieldOfView = 75.0f
);

/**
 * @brief Creates a VR neural network visualizer
 * 
 * @param brain Reference to brain system
 * @param interactionMode VR interaction mode
 * @param enablePhysics Whether to enable physics simulation
 * @return std::unique_ptr<VisualizationTools> Pointer to VR visualizer
 */
std::unique_ptr<VisualizationTools> createVRNeuralNetworkVisualizer(
    const Brain& brain,
    const std::string& interactionMode = "hand-tracking",
    bool enablePhysics = true
);

/**
 * @brief Creates a comprehensive analysis report
 * 
 * @param brain Reference to brain system
 * @param outputDirectory Output directory for reports
 * @param includeRawData Whether to include raw data in report
 * @param includeStatistics Whether to include statistics in report
 * @return std::unique_ptr<VisualizationTools> Pointer to report generator
 */
std::unique_ptr<VisualizationTools> createComprehensiveReportGenerator(
    const Brain& brain,
    const std::string& outputDirectory = "./reports",
    bool includeRawData = true,
    bool includeStatistics = true
);

/**
 * @brief Creates an animation generator for neural network simulations
 * 
 * @param brain Reference to brain system
 * @param duration Animation duration in seconds
 * @param frameRate Animation frame rate
 * @param loop Whether to loop the animation
 * @return std::unique_ptr<VisualizationTools> Pointer to animation generator
 */
std::unique_ptr<VisualizationTools> createAnimationGenerator(
    const Brain& brain,
    double duration = 10.0,
    double frameRate = 30.0,
    bool loop = false
);

/**
 * @brief Configures VisualizationTools based on NLM configuration
 * 
 * @param vizTools Reference to visualization tools
 * @param config Visualization configuration
 * @param brain Reference to brain system (optional)
 */
void configureVisualizationFromConfig(
    VisualizationTools& vizTools,
    const VisualizationToolsConfig& config,
    const Brain* brain = nullptr
);

/**
 * @brief Initializes visualization data from brain state
 * 
 * @param vizTools Reference to visualization tools
 * @param brain Reference to brain system
 * @param dataOutputFile Optional file to save visualization data
 */
void initializeVisualizationDataFromBrain(
    VisualizationTools& vizTools,
    const Brain& brain,
    const std::string& dataOutputFile = ""
);

/**
 * @brief Updates visualization with current brain state
 * 
 * @param vizTools Reference to visualization tools
 * @param brain Reference to brain system
 * @param deltaTime Time step in seconds
 */
void updateVisualizationFromBrain(
    VisualizationTools& vizTools,
    const Brain& brain,
    double deltaTime = 0.0
);

/**
 * @brief Generates all standard visualizations for NLM
 * 
 * @param vizTools Reference to visualization tools
 * @param brain Reference to brain system
 * @param outputPrefix Prefix for output files
 * @return std::vector<std::string> List of generated files
 */
std::vector<std::string> generateStandardVisualizations(
    VisualizationTools& vizTools,
    const Brain& brain,
    const std::string& outputPrefix = "nlm_visualization"
);

/**
 * @brief Checks if VisualizationTools is properly configured
 * 
 * @param vizTools Reference to visualization tools
 * @param brain Reference to brain system
 * @return bool True if properly configured, false otherwise
 */
bool validateVisualizationConfiguration(
    const VisualizationTools& vizTools,
    const Brain& brain
);

/**
 * @brief Gets performance statistics for visualization
 * 
 * @param vizTools Reference to visualization tools
 * @return std::string Formatted performance statistics
 */
std::string getVisualizationPerformanceStats(const VisualizationTools& vizTools);

/**
 * @brief Creates a custom visualization pipeline
 * 
 * @param name Pipeline name
 * @param config Visualization configuration
 * @param pipelineConfig Pipeline-specific configuration
 * @return std::unique_ptr<VisualizationTools> Pointer to visualization pipeline
 */
std::unique_ptr<VisualizationTools> createCustomVisualizationPipeline(
    const std::string& name,
    const VisualizationToolsConfig& config,
    const std::unordered_map<std::string, std::string>& pipelineConfig
);

/**
 * @brief Applies visualization settings to an existing NLM system
 * 
 * @param brain Reference to NLM brain system
 * @param config Visualization configuration
 * @return bool True if successful, false otherwise
 */
bool applyVisualizationSettingsToBrain(
    Brain& brain,
    const VisualizationToolsConfig& config
);

/**
 * @brief Exports visualization data to external analysis tools
 * 
 * @param vizTools Reference to visualization tools
 * @param brain Reference to brain system
 * @param formats Export formats (e.g., {"csv", "json", "xml"})
 * @param outputDirectory Output directory
 * @return std::vector<std::string> List of exported files
 */
std::vector<std::string> exportVisualizationDataForExternalTools(
    VisualizationTools& vizTools,
    const Brain& brain,
    const std::vector<std::string>& formats,
    const std::string& outputDirectory = "./external_analysis"
);

/**
 * @brief Sets up comprehensive visualization monitoring
 * 
 * @param vizTools Reference to visualization tools
 * @param brain Reference to brain system
 * @param monitoringInterval Monitoring interval in seconds
 * @return std::unique_ptr<VisualizationTools> Pointer to monitoring system
 */
std::unique_ptr<VisualizationTools> setupComprehensiveVisualizationMonitoring(
    VisualizationTools& vizTools,
    const Brain& brain,
    double monitoringInterval = 1.0
);

/**
 * @brief Shuts down visualization system gracefully
 * 
 * @param vizTools Reference to visualization tools
 * @param saveState Whether to save current visualization state
 * @param backupFile Optional backup file for state
 */
void shutdownVisualizationSystem(
    VisualizationTools& vizTools,
    bool saveState = true,
    const std::string& backupFile = ""
);

} // namespace nlm