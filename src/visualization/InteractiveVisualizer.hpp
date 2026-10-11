#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include "Visualizer.hpp"

namespace nlm {

class Brain;

/**
 * Interactive visualization system for NLM
 * 
 * Provides real-time, interactive visualization capabilities with:
 * - Mouse and keyboard interaction
 * - Zoom and pan controls
 * - Selection and highlighting of neural elements
 * - Real-time data updates
 * - Interactive data exploration
 * - Multiple visualization modes
 * 
 * Features:
 * - Real-time rendering with specified frame rate
 * - Interactive element selection
 * - Zoom and pan functionality
 * - Color scheme customization
 * - Data point inspection
 * - Export of interactive sessions
 * 
 * Integration with:
 * - Core Visualizer system
 * - Real-time simulation data
 * - User input handling
 * - Export systems
 * 
 * @see Visualizer
 * @see NetworkVisualizer
 * @see DataExporter
 */
class InteractiveVisualizer {
public:
    /**
     * Interaction event types
     */
    enum class InteractionEvent {
        MOUSE_CLICK,
        MOUSE_MOVE,
        KEY_PRESS,
        KEY_RELEASE,
        MOUSE_WHEEL,
        WINDOW_RESIZE,
        WINDOW_CLOSE,
        DRAG_DROP
    };

    /**
     * Interactive visualization configuration
     */
    struct InteractiveConfig {
        bool enableMouseInteraction = true;
        bool enableKeyboardControls = true;
        bool enableZoom = true;
        bool enablePan = true;
        bool enableSelection = true;
        bool enableHighlighting = true;
        double zoomLevel = 1.0;
        double panX = 0.0;
        double panY = 0.0;
        std::string selectedElementId;
        std::vector<std::string> highlightedElements;
        double interactionSensitivity = 1.0;
        int bufferSize = 1024;
        bool enableRecording = false;
        std::string recordingFile;
        bool enableAutoSave = false;
        double autoSaveInterval = 60.0;
    };

    /**
     * Interaction handler function type
     */
    using InteractionHandler = std::function<bool(const InteractionEvent&, const std::vector<double>& parameters)>;

    /**
     * Element selection callback
     */
    using SelectionCallback = std::function<void(const std::string& elementId, const std::vector<double>& properties)>;

    /**
     * Interactive visualization constructor
     */
    InteractiveVisualizer();
    
    /**
     * Interactive visualization constructor with configuration
     */
    explicit InteractiveVisualizer(const InteractiveConfig& config);
    
    /**
     * Interactive visualization destructor
     */
    ~InteractiveVisualizer();

    /**
     * Configure interactive visualization
     */
    void configure(const InteractiveConfig& config);
    
    /**
     * Initialize interactive visualization
     */
    bool initialize(int width, int height, const std::string& title = "Interactive NLM Visualization");
    
    /**
     * Update visualization with new data
     */
    void update(const Brain& brain, double timeStep, bool forceUpdate = false);
    
    /**
     * Handle interaction event
     */
    bool handleEvent(const InteractionEvent& event, const std::vector<double>& parameters);
    
    /**
     * Process user input
     */
    void processInput(double deltaTime);
    
    /**
     * Render current state
     */
    bool render();
    
    /**
     * Export current visualization state
     */
    bool exportState(const std::string& filename) const;
    
    /**
     * Start interactive session
     */
    void start();
    
    /**
     * Stop interactive session
     */
    void stop();
    
    /**
     * Check if interactive visualization is active
     */
    bool isActive() const;
    
    /**
     * Check if recording is enabled
     */
    bool isRecording() const;
    
    /**
     * Set interaction handler
     */
    void setInteractionHandler(const InteractionHandler& handler);
    
    /**
     * Set selection callback
     */
    void setSelectionCallback(const SelectionCallback& callback);
    
    /**
     * Get current configuration
     */
    InteractiveConfig getConfig() const;
    
    /**
     * Set zoom level
     */
    void setZoom(double zoomLevel);
    
    /**
     * Get zoom level
     */
    double getZoom() const;
    
    /**
     * Set pan offset
     */
    void setPan(double x, double y);
    
    /**
     * Get pan offset
     */
    std::pair<double, double> getPan() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Suppress warnings about unused private members
    InteractiveVisualizer(const InteractiveVisualizer&) = delete;
    InteractiveVisualizer& operator=(const InteractiveVisualizer&) = delete;
};

} // namespace nlm
