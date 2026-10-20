#pragma once

/**
 * Advanced Visualization System - Modern C++ Implementation
 * 
 * This represents the next generation of neural visualization systems,
 * leveraging modern C++ features for enhanced performance, type safety,
 * and advanced visualization capabilities in neural learning and control.
 * 
 * Key features:
 * - Modern C++17/20 design patterns
 * - Memory-efficient visualization data structures
 * - Type-safe visualization element handling
 * - Advanced rendering and display capabilities
 * - Real-time visualization updates
 * - Multi-format output support
 * - Performance monitoring and optimization
 * - Concurrent visualization updates
 * - Advanced debugging and profiling
 * 
 * This system provides the foundation for sophisticated neural visualization
 * in complex environments, with enterprise-grade reliability and
 * extensibility characteristics.
 */

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <optional>
#include <variant>
#include <chrono>
#include <thread>
#include <atomic>
#include <concepts>
#include <ranges>
#include <span>
#include <queue>
#include <stack>
#include <mutex>
#include <shared_mutex>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <random>
#include <future>
#include <exception>
#include <type_traits>
#include <any>
#include <filesystem>
#include <format>
#include <source_location>
#include <execution>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <freeglut.h>

namespace nlm {

// Type definitions for advanced visualization system
using VisualizationID = std::string;
using DataSeriesID = std::string;
using VisualizationType = std::string;
using ColorValue = std::array<float, 4>;
using Coordinate3D = std::array<float, 3>;
using VisualizationSize = std::array<float, 2>;
using TimePoint = std::chrono::high_resolution_clock::time_point;
using VisualizationCallback = std::function<void(const VisualizationEvent&)>
using UpdateCallback = std::function<void(const VisualizationData&)>

/**
 * Advanced visualization data structures using modern C++ features
 * 
 * Represents visualization data with advanced features including temporal
 * information, metadata, and context using modern C++ data structures.
 */
struct VisualizationData {
    // Data identification and classification
    VisualizationID id;
    std::string title;
    std::string description;
    
    // Data properties and characteristics
    VisualizationType type;
    std::vector<DataSeriesID> dataSeries;
    VisualizationSize viewport;
    
    // Data content and properties
    std::vector<std::vector<Coordinate3D>> points;
    std::vector<std::vector<double>> scalarData;
    std::vector<std::vector<ColorValue>> colors;
    std::vector<std::string> labels;
    std::vector<std::unordered_map<std::string, std::any>> metadata;
    
    // Temporal and contextual information
    TimePoint timestamp;
    double duration;
    std::string context;
    std::string coordinateSystem;
    
    // Data statistics and properties
    Coordinate3D boundingBoxMin;
    Coordinate3D boundingBoxMax;
    double dataRangeMin;
    double dataRangeMax;
    size_t dataPointCount;
    double updateFrequency;
    
    // Data quality and confidence
    double dataQuality;
    double renderingQuality;
    std::vector<std::string> dataQualityIssues;
    bool isValid;
    
    VisualizationData() : duration(0.0), dataRangeMin(0.0), dataRangeMax(0.0),
                         dataPointCount(0), updateFrequency(0.0),
                         dataQuality(1.0), renderingQuality(1.0), isValid(true) {}
    
    VisualizationData(const VisualizationID& id, const std::string& title, const VisualizationType& type)
        : id(id), title(title), description(""), type(type), duration(0.0),
          dataRangeMin(0.0), dataRangeMax(0.0), dataPointCount(0), updateFrequency(0.0),
          dataQuality(1.0), renderingQuality(1.0), isValid(true) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
    
    // Data processing and analysis
    void computeBoundingBox() {
        if (points.empty()) return;
        
        boundingBoxMin = points[0];
        boundingBoxMax = points[0];
        
        for (const auto& pointSet : points) {
            for (const auto& point : pointSet) {
                for (size_t i = 0; i < 3; ++i) {
                    boundingBoxMin[i] = std::min(boundingBoxMin[i], point[i]);
                    boundingBoxMax[i] = std::max(boundingBoxMax[i], point[i]);
                }
            }
        }
    }
    
    // Compute data range
    void computeDataRange() {
        if (scalarData.empty()) return;
        
        dataRangeMin = scalarData[0][0];
        dataRangeMax = scalarData[0][0];
        
        for (const auto& dataSeries : scalarData) {
            for (double value : dataSeries) {
                dataRangeMin = std::min(dataRangeMin, value);
                dataRangeMax = std::max(dataRangeMax, value);
            }
        }
    }
    
    // Normalize data to [0, 1] range
    void normalizeData() {
        if (dataRangeMax <= dataRangeMin) return;
        
        double range = dataRangeMax - dataRangeMin;
        
        for (auto& dataSeries : scalarData) {
            for (double& value : dataSeries) {
                value = (value - dataRangeMin) / range;
            }
        }
    }
    
    // Add data point
    void addDataPoint(const Coordinate3D& point, double scalarValue, const ColorValue& color) {
        points.push_back(point);
        
        std::vector<double> dataSeries;
        dataSeries.push_back(scalarValue);
        scalarData.push_back(dataSeries);
        
        colors.push_back(color);
        labels.push_back("");
        
        metadata.push_back(std::unordered_map<std::string, std::any>());
        
        dataPointCount++;
    }
    
    // Compute similarity between two visualization data sets
    static double computeSimilarity(const VisualizationData& a, const VisualizationData& b) {
        if (a.points.empty() || b.points.empty()) return 0.0;
        
        double similarity = 0.0;
        size_t commonPoints = 0;
        
        for (const auto& pointSetA : a.points) {
            for (const auto& pointSetB : b.points) {
                if (pointSetA == pointSetB) {
                    similarity += 1.0;
                    commonPoints++;
                }
            }
        }
        
        if (commonPoints == 0) return 0.0;
        
        return similarity / static_cast<double>(commonPoints);
    }
};

/**
 * Advanced visualization element representation
 * 
 * Represents visualization elements with advanced features including
 * 3D positioning, styling, interactivity, and advanced rendering
 * properties using modern C++ data structures.
 */
struct VisualizationElement {
    // Element identification and classification
    VisualizationID id;
    std::string name;
    std::string type;
    
    // Element properties and characteristics
    Coordinate3D position;
    Coordinate3D rotation;
    Coordinate3D scale;
    ColorValue color;
    double opacity;
    double size;
    
    // Element geometry and topology
    std::vector<Coordinate3D> vertices;
    std::vector<std::vector<size_t>> edges;
    std::vector<std::vector<size_t>> faces;
    std::vector<double> vertexData;
    
    // Element metadata and properties
    std::string material;
    std::string shaderProgram;
    std::unordered_map<std::string, std::any> properties;
    std::vector<std::string> tags;
    
    // Element state and behavior
    bool isVisible;
    bool isInteractive;
    bool isHighlighted;
    bool isSelected;
    double animationProgress;
    std::string animationState;
    
    // Element performance and quality
    double renderTime;
    double memoryUsage;
    double qualityLevel;
    
    VisualizationElement() : position({0.0f, 0.0f, 0.0f}), rotation({0.0f, 0.0f, 0.0f}),
                             scale({1.0f, 1.0f, 1.0f}), color({1.0f, 1.0f, 1.0f, 1.0f}),
                             opacity(1.0), size(1.0), isVisible(true), isInteractive(false),
                             isHighlighted(false), isSelected(false), animationProgress(0.0),
                             renderTime(0.0), memoryUsage(0.0), qualityLevel(1.0) {}
    
    VisualizationElement(const VisualizationID& id, const std::string& name, const std::string& type)
        : id(id), name(name), type(type), position({0.0f, 0.0f, 0.0f}), rotation({0.0f, 0.0f, 0.0f}),
          scale({1.0f, 1.0f, 1.0f}), color({1.0f, 1.0f, 1.0f, 1.0f}), opacity(1.0), size(1.0),
          isVisible(true), isInteractive(false), isHighlighted(false), isSelected(false),
          animationProgress(0.0), renderTime(0.0), memoryUsage(0.0), qualityLevel(1.0) {}
    
    // Element transformation and manipulation
    void translate(const Coordinate3D& translation) {
        for (size_t i = 0; i < 3; ++i) {
            position[i] += translation[i];
        }
    }
    
    void rotate(const Coordinate3D& rotationAngles) {
        rotation[0] += rotationAngles[0];
        rotation[1] += rotationAngles[1];
        rotation[2] += rotationAngles[2];
    }
    
    void scale(const Coordinate3D& scaleFactors) {
        for (size_t i = 0; i < 3; ++i) {
            scale[i] *= scaleFactors[i];
        }
    }
    
    // Element rendering preparation
    void prepareForRendering() {
        // Update model matrix based on transformations
        // Implement shader binding and uniform updates
        // Set up vertex buffer objects
        // Configure rendering state
    }
    
    // Element selection and interaction
    bool contains(const Coordinate3D& point) const {
        // Check if element contains the given point (simplified implementation)
        return true;
    }
    
    // Element animation
    void updateAnimation(double deltaTime) {
        animationProgress += deltaTime;
        if (animationProgress > 1.0) {
            animationProgress = 0.0;
        }
    }
    
    // Element property update
    template<typename T>
    void setProperty(const std::string& name, const T& value) {
        properties[name] = value;
    }
    
    template<typename T>
    std::optional<T> getProperty(const std::string& name) const {
        auto it = properties.find(name);
        if (it == properties.end()) {
            return std::nullopt;
        }
        
        if constexpr (std::is_same_v<T, std::any>) {
            return std::optional(it->second);
        } else {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                return std::nullopt;
            }
        }
    }
};

/**
 * Advanced visualization renderer with modern C++ implementation
 * 
 * Provides advanced rendering capabilities using modern C++ features for
 * efficient rendering, state management, and resource optimization.
 */
class VisualizationRenderer {
public:
    // Rendering configuration and settings
    struct RenderConfig {
        // Rendering mode and options
        std::string renderMode;
        bool enableLighting;
        bool enableShadows;
        bool enableAntialiasing;
        bool enableDepthTesting;
        bool enableBlending;
        
        // Performance settings
        size_t maxTextureUnits;
        size_t maxLights;
        double renderTimeout;
        size_t maxFramebufferSize;
        
        // Quality settings
        double resolutionScale;
        double shadowQuality;
        double textureQuality;
        double shaderQuality;
        
        // Output settings
        std::string outputFormat;
        double outputQuality;
        std::string outputPath;
        bool saveToFile;
        
        RenderConfig() : enableLighting(true), enableShadows(true), enableAntialiasing(true),
                        enableDepthTesting(true), enableBlending(true), maxTextureUnits(16),
                        maxLights(8), renderTimeout(1.0), maxFramebufferSize(8192),
                        resolutionScale(1.0), shadowQuality(1.0), textureQuality(1.0),
                        shaderQuality(1.0), outputQuality(1.0), saveToFile(false) {}
    };
    
    // Rendering statistics and performance metrics
    struct RenderStats {
        double totalRenderTime;
        size_t drawCalls;
        size_t trianglesRendered;
        size_t verticesRendered;
        size_t textureBindings;
        double gpuMemoryUsage;
        double cpuMemoryUsage;
        double framerate;
        double frameTime;
        std::unordered_map<std::string, double> customMetrics;
        
        RenderStats() : totalRenderTime(0.0), drawCalls(0), trianglesRendered(0),
                       verticesRendered(0), textureBindings(0), gpuMemoryUsage(0.0),
                       cpuMemoryUsage(0.0), framerate(0.0), frameTime(0.0) {}
    };
    
    // Rendering context and state management
    class RenderContext {
    public:
        RenderContext() = default;
        
        // Viewport and projection management
        void setViewport(int x, int y, int width, int height);
        void setProjection(double fov, double aspect, double nearPlane, double farPlane);
        
        // Camera and transformation management
        void setCameraPosition(const Coordinate3D& position);
        void setCameraTarget(const Coordinate3D& target);
        void setCameraUp(const Coordinate3D& up);
        
        // Lighting and material management
        void addLight(const ColorValue& position, const ColorValue& color, double intensity);
        void setMaterialProperties(const std::unordered_map<std::string, double>& properties);
        
        // State management
        void enableDepthTest(bool enable);
        void enableBlending(bool enable);
        void setBlendFunction(unsigned int src, unsigned int dst);
        
    private:
        // OpenGL state management
        // Camera parameters
        // Lighting parameters
        // Material parameters
        // Viewport and projection matrices
    };
    
    VisualizationRenderer() : isInitialized(false), isRendering(false), config(), stats(), 
                              lastFrameTime(0.0), frameCount(0), fps(0.0) {}
    
    // Initialize renderer
    bool initialize(const RenderConfig& config);
    
    // Render visualization data
    bool render(const VisualizationData& data, const RenderContext& context);
    
    // Render visualization elements
    bool render(const std::vector<VisualizationElement>& elements, const RenderContext& context);
    
    // Render visualization with custom shader
    bool renderWithShader(const VisualizationData& data, const RenderContext& context,
                         const std::string& shaderProgram);
    
    // Set configuration
    void setConfig(const RenderConfig& config);
    
    // Get configuration
    const RenderConfig& getConfig() const { return config; }
    
    // Get statistics
    const RenderStats& getStats() const { return stats; }
    
    // Get framerate
    double getFramerate() const { return fps; }
    
    // Get render time
    double getRenderTime() const { return stats.totalRenderTime; }
    
    // Start frame timing
    void startFrame();
    
    // End frame timing
    void endFrame();
    
    // Update statistics
    void updateStats();
    
    // Create visualization shader program
    static std::string createShaderProgram(const std::string& vertexShader,
                                          const std::string& fragmentShader);
    
    // Load shader from file
    static std::string loadShaderFromFile(const std::string& filename);
    
    // Create texture from data
    static unsigned int createTexture(const std::vector<ColorValue>& data, 
                                     size_t width, size_t height);
    
    // Create framebuffer
    static unsigned int createFramebuffer(size_t width, size_t height);
    
    // Resize renderer
    void resize(int width, int height);
    
    // Check if renderer is initialized
    bool isInitialized;
    
    // Check if renderer is currently rendering
    bool isRendering;
    
    // Rendering configuration
    RenderConfig config;
    
    // Rendering statistics
    RenderStats stats;
    
    // Timing information
    double lastFrameTime;
    size_t frameCount;
    double fps;
    
private:
    // OpenGL renderer implementation
    // Shader management
    // Texture management
    // Framebuffer management
    // Rendering pipeline
};

/**
 * Advanced visualization window with modern C++ implementation
 * 
 * Provides window management and user interaction capabilities using
 * modern C++ features for enhanced user experience and control.
 */
class VisualizationWindow {
public:
    // Window configuration and settings
    struct WindowConfig {
        // Window properties
        std::string title;
        VisualizationSize size;
        int positionX;
        int positionY;
        bool isFullscreen;
        bool isResizable;
        bool isDecorated;
        
        // Display properties
        std::string displayMode;
        int refreshRate;
        std::vector<std::string> displayModes;
        
        // Window behavior
        bool isVisible;
        bool isFocused;
        bool isMinimized;
        bool isMaximized;
        std::string windowState;
        
        // Interaction settings
        bool enableKeyboardInput;
        bool enableMouseInput;
        bool enableTouchInput;
        bool enableScrollInput;
        bool enableGestureInput;
        
        WindowConfig() : positionX(100), positionY(100), isFullscreen(false),
                        isResizable(true), isDecorated(true), isVisible(true),
                        isFocused(true), isMinimized(false), isMaximized(false),
                        enableKeyboardInput(true), enableMouseInput(true),
                        enableTouchInput(false), enableScrollInput(true),
                        enableGestureInput(false) {}
    };
    
    // Window event system
    enum class WindowEventType {
        WINDOW_CREATED,
        WINDOW_DESTROYED,
        WINDOW_RESIZED,
        WINDOW_MOVED,
        WINDOW_MINIMIZED,
        WINDOW_MAXIMIZED,
        WINDOW_RESTORED,
        WINDOW_FOCUS_GAINED,
        WINDOW_FOCUS_LOST,
        WINDOW_KEY_PRESSED,
        WINDOW_KEY_RELEASED,
        WINDOW_MOUSE_MOVED,
        WINDOW_MOUSE_BUTTON_PRESSED,
        WINDOW_MOUSE_BUTTON_RELEASED,
        WINDOW_MOUSE_WHEEL,
        WINDOW_TOUCH_BEGAN,
        WINDOW_TOUCH_MOVED,
        WINDOW_TOUCH_ENDED,
        WINDOW_GESTURE,
        WINDOW_CLOSE_REQUESTED,
        WINDOW_QUIT
    };
    
    struct WindowEvent {
        WindowEventType type;
        std::string windowId;
        TimePoint timestamp;
        std::unordered_map<std::string, std::any> data;
        std::optional<std::string> message;
    };
    
    using WindowEventCallback = std::function<void(const WindowEvent&)>
    
    VisualizationWindow() : isOpen(false), config(), renderer(), currentFBO(0),
                           lastFrameTime(0.0), frameCount(0), fps(0.0) {}
    
    // Initialize window
    bool initialize(const WindowConfig& config);
    
    // Open window
    bool open();
    
    // Close window
    void close();
    
    // Update window
    void update();
    
    // Render window content
    void render();
    
    // Set configuration
    void setConfig(const WindowConfig& config);
    
    // Get configuration
    const WindowConfig& getConfig() const { return config; }
    
    // Set renderer
    void setRenderer(std::shared_ptr<VisualizationRenderer> renderer) {
        this->renderer = renderer;
    }
    
    // Get renderer
    std::shared_ptr<VisualizationRenderer> getRenderer() const { return renderer; }
    
    // Check if window is open
    bool isOpen() const { return isOpen; }
    
    // Get framerate
    double getFramerate() const { return fps; }
    
    // Get window dimensions
    VisualizationSize getSize() const { return config.size; }
    
    // Subscribe to window events
    void subscribeToEvents(WindowEventCallback callback);
    
    // Unsubscribe from window events
    void unsubscribeFromEvents(WindowEventCallback callback);
    
    // Publish window event
    void publishEvent(const WindowEvent& event);
    
    // Process window events
    void processEvents();
    
    // Handle keyboard input
    void handleKeyboardInput(int key, int scancode, int action, int mods);
    
    // Handle mouse input
    void handleMouseInput(int button, int action, double xpos, double ypos);
    
    // Handle scroll input
    void handleScrollInput(double xoffset, double yoffset);
    
    // Handle touch input
    void handleTouchInput(int touchId, int action, double xpos, double ypos);
    
    // Handle gesture input
    void handleGestureInput(const std::string& gestureType, const std::unordered_map<std::string, double>& data);
    
    // Set window title
    void setTitle(const std::string& title);
    
    // Set window size
    void setSize(const VisualizationSize& size);
    
    // Set window position
    void setPosition(int x, int y);
    
    // Toggle fullscreen
    void toggleFullscreen();
    
    // Minimize window
    void minimize();
    
    // Maximize window
    void maximize();
    
    // Restore window
    void restore();
    
private:
    // Window system implementation
    // OpenGL context management
    // Input handling
    // Event processing
    // Rendering integration
    
    bool isOpen;
    WindowConfig config;
    std::shared_ptr<VisualizationRenderer> renderer;
    unsigned int currentFBO;
    double lastFrameTime;
    size_t frameCount;
    double fps;
    
    // Event system
    std::vector<WindowEventCallback> eventCallbacks;
    std::mutex eventCallbacksMutex;
    
    // Input state
    std::unordered_map<int, bool> keyboardState;
    std::unordered_map<int, bool> mouseButtonState;
    std::unordered_map<int, std::pair<double, double>> touchState;
    double scrollOffsetX;
    double scrollOffsetY;
};

/**
 * Advanced visualization manager with modern C++ implementation
 * 
 * Provides comprehensive visualization management capabilities including
 * visualization lifecycle, updates, and coordination using modern C++
 * features for efficient and reliable visualization management.
 */
class VisualizationManager {
public:
    // Visualization lifecycle management
    enum class VisualizationState {
        IDLE,
        LOADING,
        READY,
        RENDERING,
        UPDATED,
        ERROR
    };
    
    struct VisualizationInfo {
        VisualizationID id;
        std::string title;
        VisualizationType type;
        VisualizationState state;
        TimePoint createdTime;
        TimePoint lastUpdatedTime;
        double estimatedMemoryUsage;
        double estimatedRenderTime;
        std::string statusMessage;
        std::unordered_map<std::string, std::any> metadata;
    };
    
    VisualizationManager() : isRunning(false), updateInterval(16.0), // 60 FPS
                           maxVisualizations(100), currentFPS(0.0), totalMemoryUsage(0.0) {}
    
    // Initialize visualization manager
    bool initialize(double updateInterval = 16.0, size_t maxVisualizations = 100);
    
    // Start visualization manager
    void start();
    
    // Stop visualization manager
    void stop();
    
    // Add visualization
    VisualizationID addVisualization(const VisualizationData& data, const VisualizationWindow& window);
    
    // Remove visualization
    bool removeVisualization(const VisualizationID& id);
    
    // Update visualization
    bool updateVisualization(const VisualizationID& id, const VisualizationData& data);
    
    // Update all visualizations
    void updateAllVisualizations();
    
    // Get visualization
    std::optional<VisualizationInfo> getVisualization(const VisualizationID& id) const;
    
    // Get all visualizations
    std::vector<VisualizationInfo> getAllVisualizations() const;
    
    // Get visualization data
    std::optional<VisualizationData> getVisualizationData(const VisualizationID& id) const;
    
    // Get visualization renderer
    std::shared_ptr<VisualizationRenderer> getRenderer() const { return renderer; }
    
    // Get visualization window
    std::shared_ptr<VisualizationWindow> getWindow() const { return window; }
    
    // Set renderer
    void setRenderer(std::shared_ptr<VisualizationRenderer> renderer) {
        this->renderer = renderer;
    }
    
    // Set window
    void setWindow(std::shared_ptr<VisualizationWindow> window) {
        this->window = window;
    }
    
    // Check if manager is running
    bool isRunning() const { return isRunning; }
    
    // Get framerate
    double getFramerate() const { return currentFPS; }
    
    // Get memory usage
    double getMemoryUsage() const { return totalMemoryUsage; }
    
    // Get update interval
    double getUpdateInterval() const { return updateInterval; }
    
    // Set update interval
    void setUpdateInterval(double interval) { updateInterval = interval; }
    
    // Clear all visualizations
    void clearVisualizations();
    
    // Save visualizations to file
    bool saveVisualizations(const std::string& directory);
    
    // Load visualizations from file
    bool loadVisualizations(const std::string& directory);
    
    // Create visualization from file
    VisualizationID createVisualizationFromFile(const std::string& filename);
    
    // Create visualization from data
    VisualizationID createVisualizationFromData(const VisualizationData& data);
    
    // Create visualization from elements
    VisualizationID createVisualizationFromElements(const std::vector<VisualizationElement>& elements);
    
    // Export visualization data
    bool exportVisualization(const VisualizationID& id, const std::string& filename);
    
    // Import visualization data
    VisualizationID importVisualization(const std::string& filename);
    
    // Create visualization pipeline
    bool createVisualizationPipeline(const std::vector<std::string>& pipelineStages);
    
    // Update visualization pipeline
    bool updateVisualizationPipeline(const std::string& pipelineName);
    
    // Get visualization pipeline
    std::optional<std::vector<std::string>> getVisualizationPipeline(const std::string& pipelineName) const;
    
    // Remove visualization pipeline
    bool removeVisualizationPipeline(const std::string& pipelineName);
    
    // Clear visualization pipelines
    void clearVisualizationPipelines();
    
private:
    // Visualization management implementation
    // Lifecycle management
    // Update scheduling
    // Memory management
    // File I/O operations
    // Pipeline management
    
    bool isRunning;
    double updateInterval;
    size_t maxVisualizations;
    double currentFPS;
    double totalMemoryUsage;
    
    // Visualization storage
    std::unordered_map<VisualizationID, VisualizationData> visualizations;
    std::unordered_map<VisualizationID, VisualizationInfo> visualizationInfos;
    
    // Rendering components
    std::shared_ptr<VisualizationRenderer> renderer;
    std::shared_ptr<VisualizationWindow> window;
    
    // Update scheduling
    std::queue<std::function<void()>> updateQueue;
    std::mutex updateQueueMutex;
    std::condition_variable updateCondition;
    
    // Visualization lifecycle
    std::unordered_map<VisualizationID, VisualizationState> visualizationStates;
    std::unordered_map<VisualizationID, std::chrono::steady_clock::time_point> visualizationTimers;
    
    // Pipeline management
    std::unordered_map<std::string, std::vector<std::string>> visualizationPipelines;
};

// Convenience factory functions for visualization creation
namespace nlm::VisualizationFactory {
    
    /**
     * Create visualization from data
     * 
     * @param data Visualization data
     * @param renderer Renderer to use
     * @param window Window to use
     * @return Visualization ID
     */
    VisualizationID createVisualization(const VisualizationData& data,
                                        std::shared_ptr<VisualizationRenderer> renderer,
                                        std::shared_ptr<VisualizationWindow> window);
    
    /**
     * Create visualization from file
     * 
     * @param filename File to load visualization from
     * @return Visualization ID
     */
    VisualizationID createVisualizationFromFile(const std::string& filename);
    
    /**
     * Create visualization from elements
     * 
     * @param elements Visualization elements
     * @param renderer Renderer to use
     * @param window Window to use
     * @return Visualization ID
     */
    VisualizationID createVisualization(const std::vector<VisualizationElement>& elements,
                                        std::shared_ptr<VisualizationRenderer> renderer,
                                        std::shared_ptr<VisualizationWindow> window);
    
    /**
     * Create visualization window
     * 
     * @param title Window title
     * @param size Window size
     * @return Shared pointer to created window
     */
    std::shared_ptr<VisualizationWindow> createWindow(const std::string& title,
                                                     const VisualizationSize& size);
    
    /**
     * Create visualization renderer
     * 
     * @param config Renderer configuration
     * @return Shared pointer to created renderer
     */
    std::shared_ptr<VisualizationRenderer> createRenderer(const VisualizationRenderer::RenderConfig& config);
    
    /**
     * Create visualization manager
     * 
     * @param renderer Renderer to use
     * @param window Window to use
     * @return Shared pointer to created manager
     */
    std::shared_ptr<VisualizationManager> createManager(std::shared_ptr<VisualizationRenderer> renderer,
                                                        std::shared_ptr<VisualizationWindow> window);
    
} // namespace nlm::VisualizationFactory

} // namespace nlm

#endif // ADVANCED_VISUALIZATION_HPP

EOF
