#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>
#include "../core/Logger/Logger.hpp"
#include "../visualization/VisualizationEngine.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/Brain.hpp"
#include "../debugging/DebugInterface.hpp"

namespace nlm {

/**
 * @file InteractiveVisualizer.hpp
 * 
 * Interactive visualization system for NLM.
 * 
 * This file provides the InteractiveVisualizer class, which offers
 * comprehensive interactive visualization capabilities for NLM applications.
 * It integrates visualization, debugging, and analysis tools into a unified
 * interactive interface for exploring and understanding neural system behavior.
 * 
 * Key features:
 * - Interactive neural visualization
 * - Real-time system control
 * - Experiment management
 * - Interactive analysis tools
 * - Visualization customization
 * - Control panel integration
 * - Interactive debugging
 * - Real-time data exploration
 * - Custom visualization layouts
 * - Interactive simulation control
 * - Export interactive results
 * - Visualization presets
 * - Interactive scripting
 * 
 * @note Integrates with NLM's visualization and debugging systems
 * @see VisualizationEngine - For visualization
 * @see DebugInterface - For debugging
 * @see DataAnalyzer - For analysis
 * @see PerformanceProfiler - For performance monitoring
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class InteractiveVisualizer {
public:
    InteractiveVisualizer();
    ~InteractiveVisualizer();
    
    // Disable copying - PImpl idiom
    InteractiveVisualizer(const InteractiveVisualizer&) = delete;
    InteractiveVisualizer& operator=(const InteractiveVisualizer&) = delete;
    InteractiveVisualizer(InteractiveVisualizer&&) noexcept;
    InteractiveVisualizer& operator=(InteractiveVisualizer&&) noexcept;
    
    /**
     * @brief Initialize the interactive visualizer
     * 
     * @param visualizationEngine Reference to visualization engine
     * @return true if initialization successful
     */
    bool initialize(VisualizationEngine& visualizationEngine);
    
    /**
     * @brief Start interactive visualization session
     * 
     * @param brain Reference to brain system
     * @return true if interactive session started
     */
    bool startInteractiveSession(const Brain& brain);
    
    /**
     * @brief Stop interactive visualization session
     * 
     * @return true if interactive session stopped
     */
    bool stopInteractiveSession();
    
    /**
     * @brief Show control panel
     * 
     * @return true if control panel shown
     */
    bool showControlPanel();
    
    /**
     * @brief Show visualization controls
     * 
     * @return true if visualization controls shown
     */
    bool showVisualizationControls();
    
    /**
     * @brief Show analysis controls
     * 
     * @return true if analysis controls shown
     */
    bool showAnalysisControls();
    
    /**
     * @brief Show debugging controls
     * 
     * @return true if debugging controls shown
     */
    bool showDebuggingControls();
    
    /**
     * @brief Show experiment controls
     * 
     * @return true if experiment controls shown
     */
    bool showExperimentControls();
    
    /**
     * @brief Set visualization preset
     * 
     * @param presetName Name of visualization preset
     */
    void setVisualizationPreset(const std::string& presetName);
    
    /**
     * @brief Load experiment configuration
     * 
     * @param experimentName Name of experiment to load
     */
    void loadExperiment(const std::string& experimentName);
    
    /**
     * @brief Save experiment configuration
     * 
     * @param experimentName Name of experiment to save
     */
    void saveExperiment(const std::string& experimentName);
    
    /**
     * @brief Run experiment
     * 
     * @param experimentName Name of experiment to run
     */
    void runExperiment(const std::string& experimentName);
    
    /**
     * @brief Get current experiment state
     * 
     * @return Current experiment state
     */
    std::unordered_map<std::string, std::string> getExperimentState() const;
    
    /**
     * @brief Set interactive callback
     * 
     * @param callback Function to call on interactive event
     */
    void setInteractiveCallback(std::function<void(const std::string&, const std::unordered_map<std::string, std::string>&)> callback);
    
    /**
     * @brief Get interactive history
     * 
     * @param count Number of history entries to retrieve
     * @return Interactive history
     */
    std::vector<std::unordered_map<std::string, std::string>> getInteractiveHistory(size_t count = 100) const;
    
    /**
     * @brief Export interactive session
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportInteractiveSession(const std::string& format, const std::string& filepath);
    
    /**
     * @brief Check if interactive session is active
     * 
     * @return true if interactive session is active
     */
    bool isInteractiveSessionActive() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @file VisualizationController.hpp
 * 
 * Visualization control system for NLM.
 * 
 * This file provides the VisualizationController class, which offers
 * comprehensive control over visualization components. It manages
 * visualization settings, provides control over visualization elements,
 * and integrates with interactive visualization systems.
 * 
 * Key features:
 * - Visualization setting management
 * - Element control (neurons, synapses, regions)
 * - View control (camera, zoom, rotation)
 * - Data control (time, filters)
 * - Style control (colors, themes, layouts)
 * - Interaction control (selection, highlighting)
 * - Animation control
 * - Export control
 * - Preset management
 * - Real-time control
 * - Custom control panels
 * - Control validation
 * - Control history
 * 
 * @note Integrates with visualization systems
 * @see InteractiveVisualizer - For integration
 * @see VisualizationEngine - For visualization
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class VisualizationController {
public:
    VisualizationController();
    ~VisualizationController();
    
    // Disable copying - PImpl idiom
    VisualizationController(const VisualizationController&) = delete;
    VisualizationController& operator=(const VisualizationController&) = delete;
    VisualizationController(VisualizationController&&) noexcept;
    VisualizationController& operator=(VisualizationController&&) noexcept;
    
    /**
     * @brief Initialize the visualization controller
     * 
     * @param visualizationEngine Reference to visualization engine
     * @return true if initialization successful
     */
    bool initialize(VisualizationEngine& visualizationEngine);
    
    /**
     * @brief Set visualization elements
     * 
     * @param elementType Type of visualization elements
     * @param elements Vector of elements
     */
    void setVisualizationElements(const std::string& elementType, const std::vector<std::string>& elements);
    
    /**
     * @brief Set view parameters
     * 
     * @param viewType Type of view
     * @param parameters View parameters
     */
    void setViewParameters(const std::string& viewType, const std::unordered_map<std::string, double>& parameters);
    
    /**
     * @brief Set data filters
     * 
     * @param filterType Type of data filter
     * @param filters Vector of filters
     */
    void setDataFilters(const std::string& filterType, const std::vector<std::string>& filters);
    
    /**
     * @brief Set visualization styles
     * 
     * @param styleType Type of visualization style
     * @param styles Vector of styles
     */
    void setVisualizationStyles(const std::string& styleType, const std::vector<std::string>& styles);
    
    /**
     * @brief Set interaction mode
     * 
     * @param interactionMode Interaction mode
     */
    void setInteractionMode(const std::string& interactionMode);
    
    /**
     * @brief Set animation settings
     * 
     * @param animationSettings Animation settings
     */
    void setAnimationSettings(const std::unordered_map<std::string, double>& animationSettings);
    
    /**
     * @brief Export visualization settings
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportVisualizationSettings(const std::string& format, const std::string& filepath);
    
    /**
     * @brief Load visualization settings
     * 
     * @param filepath Settings file path
     * @return true if settings loaded
     */
    bool loadVisualizationSettings(const std::string& filepath);
    
    /**
     * @brief Get visualization settings
     * 
     * @return Current visualization settings
     */
    std::unordered_map<std::string, std::string> getVisualizationSettings() const;
    
    /**
     * @brief Check if visualization settings are valid
     * 
     * @param settings Settings to validate
     * @return true if settings are valid
     */
    bool validateVisualizationSettings(const std::unordered_map<std::string, std::string>& settings) const;
    
    /**
     * @brief Get available visualization presets
     * 
     * @return Available presets
     */
    std::vector<std::string> getAvailablePresets() const;
    
    /**
     * @brief Set visualization preset
     * 
     * @param presetName Preset name
     */
    void setVisualizationPreset(const std::string& presetName);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @file ExperimentManager.hpp
 * 
 * Experiment management system for NLM.
 * 
 * This file provides the ExperimentManager class, which offers
 * comprehensive experiment management capabilities. It manages
 * experiment configurations, execution, results, and analysis.
 * 
 * Key features:
 * - Experiment configuration management
 * - Experiment execution control
 * - Experiment result storage
 * - Experiment comparison
 * - Experiment analysis
 * - Experiment export
 * - Experiment history
 * - Experiment automation
 * - Experiment validation
 * - Experiment documentation
 * - Experiment parameter management
 * - Experiment result visualization
 * - Experiment reproducibility
 * - Experiment sharing
 * - Experiment templates
 * - Experiment monitoring
 * 
 * @note Integrates with NLM's analysis and visualization systems
 * @see InteractiveVisualizer - For visualization
 * @see DataAnalyzer - For analysis
 * @see PerformanceProfiler - For performance monitoring
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class ExperimentManager {
public:
    ExperimentManager();
    ~ExperimentManager();
    
    // Disable copying - PImpl idiom
    ExperimentManager(const ExperimentManager&) = delete;
    ExperimentManager& operator=(const ExperimentManager&) = delete;
    ExperimentManager(ExperimentManager&&) noexcept;
    ExperimentManager& operator=(ExperimentManager&&) noexcept;
    
    /**
     * @brief Initialize the experiment manager
     * 
     * @param brain Reference to brain system
     * @return true if initialization successful
     */
    bool initialize(const Brain& brain);
    
    /**
     * @brief Create new experiment
     * 
     * @param experimentName Name of experiment
     * @param configuration Experiment configuration
     * @return true if experiment created
     */
    bool createExperiment(const std::string& experimentName, const std::unordered_map<std::string, std::string>& configuration);
    
    /**
     * @brief Load experiment
     * 
     * @param experimentName Name of experiment to load
     * @return true if experiment loaded
     */
    bool loadExperiment(const std::string& experimentName);
    
    /**
     * @brief Run experiment
     * 
     * @param experimentName Name of experiment to run
     */
    void runExperiment(const std::string& experimentName);
    
    /**
     * @brief Stop experiment
     * 
     * @param experimentName Name of experiment to stop
     */
    void stopExperiment(const std::string& experimentName);
    
    /**
     * @brief Get experiment results
     * 
     * @param experimentName Name of experiment
     * @return Experiment results
     */
    std::unordered_map<std::string, double> getExperimentResults(const std::string& experimentName) const;
    
    /**
     * @brief Get experiment configuration
     * 
     * @param experimentName Name of experiment
     * @return Experiment configuration
     */
    std::unordered_map<std::string, std::string> getExperimentConfiguration(const std::string& experimentName) const;
    
    /**
     * @brief Compare experiments
     * 
     * @param experimentNames Vector of experiment names to compare
     * @return Comparison results
     */
    std::vector<std::unordered_map<std::string, double>> compareExperiments(const std::vector<std::string>& experimentNames) const;
    
    /**
     * @brief Export experiment results
     * 
     * @param experimentName Name of experiment
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportExperimentResults(const std::string& experimentName, const std::string& format, const std::string& filepath);
    
    /**
     * @brief Get experiment history
     * 
     * @param experimentName Name of experiment
     * @param count Number of history entries to retrieve
     * @return Experiment history
     */
    std::vector<std::pair<double, std::unordered_map<std::string, double>>> getExperimentHistory(const std::string& experimentName, size_t count = 100) const;
    
    /**
     * @brief Check if experiment is running
     * 
     * @param experimentName Name of experiment
     * @return true if experiment is running
     */
    bool isExperimentRunning(const std::string& experimentName) const;
    
    /**
     * @brief Get available experiment templates
     * 
     * @return Available experiment templates
     */
    std::vector<std::string> getAvailableExperimentTemplates() const;
    
    /**
     * @brief Set experiment callback
     * 
     * @param callback Function to call on experiment event
     */
    void setExperimentCallback(std::function<void(const std::string&, const std::string&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm