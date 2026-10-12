#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <array>

namespace nlm {

// Forward declarations for brain components
class Brain;
class Neuron;
class Synapse;
class NeuralRegion;
class SpikeEvent;

// Forward declarations for visualization components
class ShaderProgram;
class Texture;
class Framebuffer;

// Color map types
enum class ColorMap {
    Default,
    Hot,
    Cool,
    Rainbow,
    Viridis,
    Plasma,
    Inferno,
    Magma,
    Cividis,
    Grayscale,
    RedGreenBlue,
    Thermal
};

// Visualization mode types
enum class VisualizationMode {
    NeuronView,
    SynapseView,
    PopulationView,
    NetworkView,
    MemoryView,
    CognitionView,
    NeuromodulationView,
    DevelopmentView
};

// Structure for camera/view settings
struct CameraSettings {
    float x, y;
    float scale;
    float rotation;
    float zoom;
};

// Structure for rendering settings
struct RenderingSettings {
    bool enableLighting;
    bool enableShading;
    bool enableAntiAliasing;
    bool enableLOD;
    bool enableCaching;
    bool enableCompression;
    float alpha;
    float shininess;
    float specularIntensity;
    float ambientIntensity;
};

// Structure for performance statistics
struct PerformanceStats {
    double frameTime;
    double updateTime;
    double renderTime;
    uint64_t memoryUsage;
    uint64_t vertexCount;
    uint64_t triangleCount;
    double fps;
};

// Visualization interface for comprehensive brain visualization
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();

    // ========== Core visualization infrastructure ==========
    
    // Initialize visualization system (GLFW, SDL, OpenGL setup)
    bool initialize();
    
    // Clean up visualization resources
    void shutdown();
    
    // Update visualization with current brain state
    void update();
    
    // Render current visualization frame
    void render();
    
    // Start new frame
    void beginFrame();
    
    // End current frame
    void endFrame();
    
    // Set brain reference for visualization
    void setBrain(Brain* brain);

    // ========== Visualization modes ==========
    
    // Neuron activity visualization
    void enableNeuronView();
    void disableNeuronView();
    bool isNeuronViewEnabled() const;
    
    // Synaptic connections visualization
    void enableSynapseView();
    void disableSynapseView();
    bool isSynapseViewEnabled() const;
    
    // Neural population statistics visualization
    void enablePopulationView();
    void disablePopulationView();
    bool isPopulationViewEnabled() const;
    
    // Network connectivity visualization
    void enableNetworkView();
    void disableNetworkView();
    bool isNetworkViewEnabled() const;
    
    // Memory system activity visualization
    void enableMemoryView();
    void disableMemoryView();
    bool isMemoryViewEnabled() const;
    
    // Cognitive system activity visualization
    void enableCognitionView();
    void disableCognitionView();
    bool isCognitionViewEnabled() const;
    
    // Neuromodulator levels visualization
    void enableNeuromodulationView();
    void disableNeuromodulationView();
    bool isNeuromodulationViewEnabled() const;
    
    // Developmental stage visualization
    void enableDevelopmentView();
    void disableDevelopmentView();
    bool isDevelopmentViewEnabled() const;
    
    // Enable all visualization modes
    void enableAllViews();
    
    // Disable all visualization modes
    void disableAllViews();

    // ========== Rendering components ==========
    
    // Render neuron populations
    void renderNeurons();
    
    // Render synaptic connections
    void renderSynapses();
    
    // Render neural population statistics
    void renderPopulations();
    
    // Render network connectivity
    void renderNetworkTopology();
    
    // Render memory system activity
    void renderMemorySystems();
    
    // Render cognitive system activity
    void renderCognitiveSystems();
    
    // Render neuromodulator levels
    void renderNeuromodulators();
    
    // Render developmental stage
    void renderDevelopment();

    // ========== Visualization features ==========
    
    // Set visualization scale
    void setScale(float scale);
    
    // Set visualization position
    void setPosition(float x, float y);
    
    // Set color map
    void setColorMap(const std::string& colormap);
    void setColorMap(ColorMap colormap);
    
    // Set transparency
    void setAlpha(float alpha);
    
    // Enable/disable lighting
    void setLighting(bool enable);
    
    // Enable/disable shading
    void setShading(bool enable);
    
    // Enable/disable anti-aliasing
    void setAntiAliasing(bool enable);
    
    // Set level of detail
    void setLOD(bool enable);
    
    // Enable/disable caching
    void setCaching(bool enable);
    
    // Enable/disable compression
    void setCompression(bool enable);

    // ========== Interaction capabilities ==========
    
    // Set mouse callback
    void setMouseCallback(std::function<void(double, double)> callback);
    
    // Set keyboard callback
    void setKeyboardCallback(std::function<void(int, int, int)> callback);
    
    // Set scroll callback
    void setScrollCallback(std::function<void(double, double)> callback);
    
    // Handle mouse clicks
    void handleMouseClick(int button, int action, double x, double y);
    
    // Handle key presses
    void handleKeyPress(int key, int scancode, int action);
    
    // Handle mouse scroll
    void handleMouseScroll(double xoffset, double yoffset);

    // ========== Export and save capabilities ==========
    
    // Save current frame as image
    bool saveScreenshot(const std::string& filename);
    
    // Export visualization data
    bool exportData(const std::string& filename);
    
    // Save visualization configuration
    bool saveConfiguration(const std::string& filename);
    
    // Load visualization configuration
    bool loadConfiguration(const std::string& filename);

    // ========== Performance optimization ==========
    
    // Get memory usage
    uint64_t getMemoryUsage() const;
    
    // Get performance statistics
    PerformanceStats getPerformanceStats() const;
    
    // Get current FPS
    double getFPS() const;
    
    // Get frame time in milliseconds
    double getFrameTime() const;

    // ========== Utility methods ==========
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    
    // Get update rate
    double getUpdateRate() const;
    
    // Get current brain reference
    Brain* getBrain() const;

    // Clear screen with specific color
    void clearScreen(float r, float g, float b, float a = 1.0f);
    
    // Set viewport
    void setViewport(int x, int y, int width, int height);

private:
    // Internal implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Initialize OpenGL context and resources
    bool initializeOpenGL();
    
    // Create rendering pipeline and shader programs
    bool createRenderingPipeline();
    
    // Set up window management and input handling
    bool setupWindow();
    
    // Configure OpenGL context and rendering state
    void configureRenderingState();
    
    // Update view matrices
    void updateViewMatrix();
    
    // Render current frame based on active visualization modes
    void renderFrame();
    
    // Process input events
    void processInput();
    
    // Update performance statistics
    void updatePerformanceStats();
}; 

} // namespace nlm
