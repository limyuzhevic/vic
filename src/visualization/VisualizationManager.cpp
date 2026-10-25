#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <optional>
#include <array>

namespace nlm {

// Visualization system version
constexpr int VISUALIZATION_SYSTEM_VERSION = 1;

// Visualization types
enum class VisualizationType {
    NeuralNetwork2D,
    NeuralNetwork3D,
    SpikeRasterPlot,
    WeightMatrix,
    ActivityHeatmap,
    BrainConnectivity,
    MemoryVisualization,
    PredictionVisualization,
    DevelopmentTimeline,
    PerformanceProfiler,
    Custom
};

// Visualization output formats
enum class VisualizationFormat {
    PNG,
    SVG,
    PDF,
    JSON,
    CSV,
    Text,
    HTML,
    WebGL,
    GlTF,
    Custom
};

// Visualization rendering modes
enum class RenderingMode {
    Static,
    RealTime,
    Interactive,
    Animated,
    TimeLapse,
    Comparative
};

// Visualization update strategies
enum class UpdateStrategy {
    OnDemand,
    Periodic,
    EventDriven,
    TriggerBased
};

// Visualization filter modes
enum class FilterMode {
    None,
    RegionBased,
    NeuronTypeBased,
    ActivityThreshold,
    ConnectionType,
    TemporalWindow,
    Custom
};

// Visualization data structure
struct VisualizationData {
    std::string id;
    VisualizationType type;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::any> data;
    std::vector<std::string> dataSources;
    std::unordered_map<std::string, std::string> metadata;
    
    // Performance data
    double renderTime = 0.0;
    size_t memoryUsage = 0;
    int vertexCount = 0;
    int triangleCount = 0;
    
    // Quality settings
    double qualityLevel = 1.0;
    bool antiAliasing = true;
    int resolutionLevel = 3;
    
    // Animation
    bool animated = false;
    double animationSpeed = 1.0;
    std::chrono::system_clock::time_point animationStart;
    
    // Selection and highlighting
    std::vector<std::string> selectedRegions;
    std::vector<size_t> selectedNeurons;
    std::vector<std::pair<size_t, size_t>> selectedConnections;
    
    // Filters
    FilterMode filterMode = FilterMode::None;
    std::unordered_map<std::string, std::any> filters;
    
    VisualizationData() : timestamp(std::chrono::system_clock::now()), animated(false) {}
};

// Visualization configuration
struct VisualizationConfig {
    std::string name;
    VisualizationType type;
    std::string title;
    std::string description;
    VisualizationFormat format;
    RenderingMode renderingMode;
    UpdateStrategy updateStrategy;
    
    // Display settings
    double width = 1024.0;
    double height = 768.0;
    double aspectRatio = 1.33;
    int fps = 60;
    double targetFrameTime = 1.0 / 60.0;
    
    // Color scheme
    std::string colorScheme;
    std::unordered_map<std::string, std::string> customColors;
    bool useGradients = true;
    double opacity = 1.0;
    
    // Interaction
    bool interactive = true;
    bool zoomEnabled = true;
    bool panEnabled = true;
    bool selectionEnabled = true;
    bool searchEnabled = true;
    
    // Performance
    bool enableHardwareAcceleration = true;
    int maxTextureSize = 4096;
    int maxBufferSize = 64 * 1024 * 1024; // 64MB
    bool compressionEnabled = false;
    
    // Export
    std::string exportPath;
    bool autoExport = false;
    double exportInterval = 5.0; // seconds
    
    // Data
    bool showDataPoints = true;
    bool showConnections = true;
    bool showLabels = true;
    bool showTooltips = true;
    
    // Filters
    double activityThreshold = 0.5;
    std::vector<std::string> includeRegions;
    std::vector<std::string> excludeRegions;
    std::vector<std::string> includeNeuronTypes;
    std::vector<std::string> excludeNeuronTypes;
    
    std::map<std::string, std::any> customSettings;
};

// Visualization event types
enum class VisualizationEventType {
    DataUpdated,
    VisualizationCreated,
    VisualizationDestroyed,
    VisualizationStarted,
    VisualizationStopped,
    VisualizationPaused,
    VisualizationResumed,
    ExportCompleted,
    ExportFailed,
    ErrorOccurred,
    UserInteraction,
    SettingsChanged,
    FilterChanged
};

// Visualization event data
struct VisualizationEventData {
    VisualizationEventType type;
    std::string source;
    std::chrono::system_clock::time_point timestamp;
    std::string message;
    std::unordered_map<std::string, std::any> data;
    
    template<typename T>
    T getData(const std::string& key) const {
        auto it = data.find(key);
        if (it != data.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                throw std::runtime_error("Data type mismatch for key: " + key);
            }
        }
        throw std::runtime_error("Key not found: " + key);
    }
    
    template<typename T>
    void setData(const std::string& key, const T& value) {
        data[key] = value;
    }
};

// Visualization interface
class IVisualization {
public:
    virtual ~IVisualization() = default;
    
    // Visualization identification
    virtual std::string getId() const = 0;
    virtual std::string getName() const = 0;
    virtual VisualizationType getType() const = 0;
    virtual std::string getVersion() const = 0;
    
    // Configuration
    virtual bool configure(const VisualizationConfig& config) = 0;
    virtual VisualizationConfig getConfig() const = 0;
    virtual bool setData(const std::string& source, const VisualizationData& data) = 0;
    
    // Rendering
    virtual bool initialize() = 0;
    virtual bool render(double deltaTime) = 0;
    virtual bool shutdown() = 0;
    virtual bool update(double deltaTime) = 0;
    
    // Interaction
    virtual bool handleMouseClick(double x, double y, int button) = 0;
    virtual bool handleMouseMove(double x, double y) = 0;
    virtual bool handleMouseScroll(double delta) = 0;
    virtual bool handleKeyPress(int key, int scancode) = 0;
    
    // Visualization control
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool pause() = 0;
    virtual bool resume() = 0;
    virtual bool reset() = 0;
    
    // Export
    virtual bool exportToFile(const std::string& filepath, VisualizationFormat format) = 0;
    virtual bool exportToStream(std::ostream& stream, VisualizationFormat format) = 0;
    virtual bool saveScreenshot(const std::string& filepath) = 0;
    
    // Event handling
    virtual bool handleEvent(const VisualizationEventData& event) = 0;
    virtual void addEventListener(VisualizationEventType eventType,
                                 std::function<bool(const VisualizationEventData&)> callback) = 0;
    virtual void removeEventListener(VisualizationEventType eventType,
                                    std::function<bool(const VisualizationEventData&)> callback) = 0;
    
    // State
    virtual bool isRunning() const = 0;
    virtual bool isPaused() const = 0;
    virtual bool isInitialized() const = 0;
    virtual double getProgress() const = 0;
    virtual std::chrono::system_clock::time_point getStartTime() const = 0;
    
    // Performance
    virtual double getFPS() const = 0;
    virtual double getRenderTime() const = 0;
    virtual size_t getMemoryUsage() const = 0;
    virtual int getVertexCount() const = 0;
    virtual int getTriangleCount() const = 0;
    
    // Data management
    virtual std::vector<std::string> getDataSources() const = 0;
    virtual VisualizationData getData(const std::string& source) const = 0;
    virtual bool removeData(const std::string& source) = 0;
    virtual bool clearData() = 0;
    
    // Configuration management
    virtual bool addPreset(const std::string& name, const VisualizationConfig& config) = 0;
    virtual bool removePreset(const std::string& name) = 0;
    virtual VisualizationConfig getPreset(const std::string& name) const = 0;
    virtual std::vector<std::string> getPresets() const = 0;
    virtual bool applyPreset(const std::string& name) = 0;
    
    // Extension points
    virtual std::vector<std::any> getExtensions() const = 0;
    virtual bool addExtension(const std::any& extension) = 0;
    virtual bool removeExtension(const std::any& extension) = 0;
    
    // Configuration
    virtual void setVisualizationManager(void* manager) = 0;
    virtual void* getVisualizationManager() const = 0;
};

// Visualization manager interface
class IVisualizationManager {
public:
    virtual ~IVisualizationManager() = default;
    
    // Visualization lifecycle
    virtual bool createVisualization(const std::string& id, const VisualizationConfig& config) = 0;
    virtual bool destroyVisualization(const std::string& id) = 0;
    virtual bool showVisualization(const std::string& id) = 0;
    virtual bool hideVisualization(const std::string& id) = 0;
    virtual bool activateVisualization(const std::string& id) = 0;
    virtual bool deactivateVisualization(const std::string& id) = 0;
    
    // Visualization control
    virtual bool startAllVisualizations() = 0;
    virtual bool stopAllVisualizations() = 0;
    virtual bool pauseAllVisualizations() = 0;
    virtual bool resumeAllVisualizations() = 0;
    virtual bool resetAllVisualizations() = 0;
    
    // Visualization management
    virtual bool setActiveVisualization(const std::string& id) = 0;
    virtual std::string getActiveVisualization() const = 0;
    virtual std::vector<std::string> getVisibleVisualizations() const = 0;
    virtual std::vector<std::string> getAllVisualizationIds() const = 0;
    
    // Data management
    virtual bool addDataSource(const std::string& sourceId, const std::string& visualizationId = "") = 0;
    virtual bool removeDataSource(const std::string& sourceId) = 0;
    virtual bool updateData(const std::string& sourceId, const VisualizationData& data) = 0;
    virtual VisualizationData getData(const std::string& sourceId) const = 0;
    virtual bool clearData(const std::string& sourceId) = 0;
    
    // Event system
    virtual bool addEventListener(VisualizationEventType eventType,
                                 std::function<bool(const VisualizationEventData&)> callback) = 0;
    virtual bool removeEventListener(VisualizationEventType eventType,
                                    std::function<bool(const VisualizationEventData&)> callback) = 0;
    virtual void emitEvent(const VisualizationEventData& event) = 0;
    
    // Configuration
    virtual bool setGlobalConfiguration(const VisualizationConfig& config) = 0;
    virtual VisualizationConfig getGlobalConfiguration() const = 0;
    virtual bool addPreset(const std::string& name, const VisualizationConfig& config) = 0;
    virtual bool removePreset(const std::string& name) = 0;
    virtual VisualizationConfig getPreset(const std::string& name) const = 0;
    virtual std::vector<std::string> getPresets() const = 0;
    
    // Performance
    virtual double getSystemFPS() const = 0;
    virtual size_t getSystemMemoryUsage() const = 0;
    virtual size_t getActiveVisualizationCount() const = 0;
    virtual std::chrono::nanoseconds getSystemUptime() const = 0;
    
    // Export
    virtual bool exportAllVisualizations(const std::string& directory, VisualizationFormat format) = 0;
    virtual bool saveSnapshot(const std::string& filepath, const std::string& visualizationId = "") = 0;
    
    // Diagnostics
    virtual std::string getSystemStatus() const = 0;
    virtual std::string getVisualizationStatus(const std::string& id) const = 0;
    virtual std::vector<std::string> getVisualizationErrors(const std::string& id) const = 0;
    virtual void clearVisualizationErrors(const std::string& id) = 0;
    
    // Statistics
    virtual size_t getVisualizationCreateCount(const std::string& id) const = 0;
    virtual size_t getVisualizationDestroyCount(const std::string& id) const = 0;
    virtual double getVisualizationSuccessRate(const std::string& id) const = 0;
    virtual std::chrono::nanoseconds getAverageVisualizationRenderTime(const std::string& id) const = 0;
    
    // Visualization factory methods
    virtual std::shared_ptr<IVisualization> createVisualizationInstance(VisualizationType type) = 0;
    
    // Resource management
    virtual bool cleanup() = 0;
    virtual bool optimize() = 0;
    virtual bool clearCache() = 0;
    
    // Lifecycle callbacks
    virtual void setVisualizationCreatedCallback(std::function<void(const std::string&)> callback) = 0;
    virtual void setVisualizationDestroyedCallback(std::function<void(const std::string&)> callback) = 0;
    virtual void setErrorCallback(std::function<void(const std::string&, const std::string&)> callback) = 0;
    virtual void setProgressCallback(std::function<void(const std::string&, double)> callback) = 0;
};

// Visualization registration macro
#define REGISTER_VISUALIZATION(VisualizationClass) \
    extern "C" IVisualization* createVisualization() { \
        return new VisualizationClass(); \
    } \
    extern "C" void destroyVisualization(IVisualization* visualization) { \
        delete visualization; \
    } \
    extern "C" int getVisualizationVersion() { \
        return VISUALIZATION_SYSTEM_VERSION; \
    } \
    extern "C" const char* getVisualizationType() { \
        return #VisualizationClass; \
    }

// Visualization configuration macro
#define VISUALIZATION_CONFIG(name, type, width, height) \
    VisualizationConfig getDefaultConfig() { \
        VisualizationConfig config; \
        config.name = name; \
        config.type = type; \
        config.width = width; \
        config.height = height; \
        return config; \
    }

// Visualization data processors
namespace VisualizationDataProcessor {
    
    // Neural network data to visualization data
    static VisualizationData convertNeuralNetworkData(const std::vector<std::vector<double>>& connectivityMatrix,
                                                     const std::vector<double>& neuronActivity,
                                                     const std::vector<std::string>& neuronLabels);
    
    // Spike data to raster plot data
    static VisualizationData convertSpikeData(const std::vector<std::vector<double>>& spikeTimes,
                                              const std::vector<std::string>& neuronIds);
    
    // Weight matrix to visualization data
    static VisualizationData convertWeightMatrixData(const std::vector<std::vector<double>>& weights,
                                                     const std::vector<std::string>& sourceNeurons,
                                                     const std::vector<std::string>& targetNeurons);
    
    // Activity heatmap data
    static VisualizationData convertActivityHeatmapData(const std::vector<std::vector<double>>& activityMap,
                                                         const std::vector<std::string>& regionLabels,
                                                         const std::chrono::system_clock::time_point& timestamp);
    
    // Performance data
    static VisualizationData convertPerformanceData(const std::vector<double>& cpuUsage,
                                                     const std::vector<size_t>& memoryUsage,
                                                     const std::vector<double>& fps,
                                                     const std::chrono::system_clock::time_point& timestamp);
    
    // Memory usage data
    static VisualizationData convertMemoryData(size_t totalMemory, size_t usedMemory,
                                               const std::map<std::string, size_t>& regionMemory);
}

// Visualization theme system
struct VisualizationTheme {
    std::string name;
    std::string description;
    
    // Colors
    std::string backgroundColor;
    std::string neuronColor;
    std::string connectionColor;
    std::string activeNeuronColor;
    std::string selectedNeuronColor;
    std::string labelColor;
    std::string gridColor;
    
    // Styles
    std::string neuronShape;
    double neuronSize;
    std::string connectionStyle;
    double connectionWidth;
    bool showLabels;
    bool showConnections;
    
    // Animation
    std::string animationStyle;
    double animationSpeed;
    bool smoothScrolling;
    
    std::map<std::string, std::any> customProperties;
};

// Base visualization implementation
class BaseVisualization : public IVisualization {
protected:
    std::string id_;
    std::string name_;
    VisualizationType type_;
    VisualizationConfig config_;
    IVisualizationManager* manager_;
    
    std::mutex mutex_;
    std::atomic<bool> running_;
    std::atomic<bool> initialized_;
    std::atomic<bool> paused_;
    
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::nanoseconds totalRenderTime_;
    size_t frameCount_;
    
    std::vector<std::string> dataSources_;
    std::map<std::string, VisualizationData> data_;
    
    std::unordered_map<VisualizationEventType, std::vector<std::function<bool(const VisualizationEventData&)>>> eventListeners_;
    std::vector<std::any> extensions_;
    
    std::vector<std::string> errors_;
    
public:
    BaseVisualization() : running_(false), initialized_(false), paused_(false),
                         frameCount_(0), totalRenderTime_(std::chrono::nanoseconds(0)) {
        startTime_ = std::chrono::steady_clock::now();
    }
    
    virtual ~BaseVisualization() = default;
    
    // IVisualization interface implementation
    std::string getId() const override {
        return id_;
    }
    
    std::string getName() const override {
        return name_;
    }
    
    VisualizationType getType() const override {
        return type_;
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    bool configure(const VisualizationConfig& config) override {
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = config;
        name_ = config.name;
        type_ = config.type;
        
        id_ = generateId();
        
        return onConfigure(config);
    }
    
    VisualizationConfig getConfig() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return config_;
    }
    
    bool setData(const std::string& sourceId, const VisualizationData& data) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (running_ && !paused_) {
            dataSources_.push_back(sourceId);
            data_[sourceId] = data;
            return onDataUpdate(sourceId, data);
        }
        
        return false;
    }
    
    bool initialize() override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (initialized_) {
            return true;
        }
        
        initialized_ = onInitialize();
        return initialized_;
    }
    
    bool render(double deltaTime) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!running_ || paused_) {
            return false;
        }
        
        auto renderStart = std::chrono::steady_clock::now();
        
        bool result = onRender(deltaTime);
        
        auto renderEnd = std::chrono::steady_clock::now();
        auto renderDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(renderEnd - renderStart);
        
        std::lock_guard<std::mutex> lock2(mutex_);
        totalRenderTime_ += renderDuration;
        frameCount_++;
        
        return result;
    }
    
    bool shutdown() override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        bool result = onShutdown();
        
        initialized_ = false;
        running_ = false;
        paused_ = false;
        
        dataSources_.clear();
        data_.clear();
        
        return result;
    }
    
    bool update(double deltaTime) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return onUpdate(deltaTime);
    }
    
    bool handleMouseClick(double x, double y, int button) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return onMouseClick(x, y, button);
    }
    
    bool handleMouseMove(double x, double y) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return onMouseMove(x, y);
    }
    
    bool handleMouseScroll(double delta) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return onMouseScroll(delta);
    }
    
    bool handleKeyPress(int key, int scancode) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return onKeyPress(key, scancode);
    }
    
    bool start() override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!initialized_) {
            return false;
        }
        
        running_ = onStart();
        return running_;
    }
    
    bool stop() override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        running_ = false;
        return onStop();
    }
    
    bool pause() override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!running_) {
            return false;
        }
        
        paused_ = onPause();
        return paused_;
    }
    
    bool resume() override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (paused_) {
            paused_ = false;
            return onResume();
        }
        
        return false;
    }
    
    bool reset() override {
        std::lock_guard<std::mutex> lock(mutex_);
        return onReset();
    }
    
    bool exportToFile(const std::string& filepath, VisualizationFormat format) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return onExportToFile(filepath, format);
    }
    
    bool exportToStream(std::ostream& stream, VisualizationFormat format) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return onExportToStream(stream, format);
    }
    
    bool saveScreenshot(const std::string& filepath) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return onSaveScreenshot(filepath);
    }
    
    bool handleEvent(const VisualizationEventData& event) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = eventListeners_.find(event.type);
        if (it != eventListeners_.end()) {
            for (auto& callback : it->second) {
                if (callback(event)) {
                    return true;
                }
            }
        }
        
        return onEvent(event);
    }
    
    void addEventListener(VisualizationEventType eventType,
                         std::function<bool(const VisualizationEventData&)> callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        eventListeners_[eventType].push_back(callback);
    }
    
    void removeEventListener(VisualizationEventType eventType,
                            std::function<bool(const VisualizationEventData&)> callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = eventListeners_.find(eventType);
        if (it != eventListeners_.end()) {
            it->second.erase(
                std::remove(it->second.begin(), it->second.end(), callback),
                it->second.end()
            );
        }
    }
    
    bool isRunning() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return running_;
    }
    
    bool isPaused() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return paused_;
    }
    
    bool isInitialized() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return initialized_;
    }
    
    double getProgress() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!running_) {
            return 0.0;
        }
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - startTime_);
        double progress = static_cast<double>(elapsed.count()) / 1e9; // Convert to seconds
        return std::min(progress, 100.0);
    }
    
    std::chrono::system_clock::time_point getStartTime() const override {
        return startTime_;
    }
    
    double getFPS() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return frameCount_ > 0 ? static_cast<double>(frameCount_) / (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - startTime_).count()) : 0.0;
    }
    
    double getRenderTime() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return frameCount_ > 0 ? static_cast<double>(totalRenderTime_.count()) / frameCount_ / 1e9 : 0.0; // Convert to seconds
    }
    
    size_t getMemoryUsage() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        // This is a placeholder - actual implementation would track memory usage
        return 0;
    }
    
    int getVertexCount() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return 0;
    }
    
    int getTriangleCount() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return 0;
    }
    
    std::vector<std::string> getDataSources() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return dataSources_;
    }
    
    VisualizationData getData(const std::string& sourceId) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = data_.find(sourceId);
        if (it != data_.end()) {
            return it->second;
        }
        throw std::runtime_error("Data source not found: " + sourceId);
    }
    
    bool removeData(const std::string& sourceId) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = data_.find(sourceId);
        if (it != data_.end()) {
            data_.erase(it);
            
            auto sourceIt = std::find(dataSources_.begin(), dataSources_.end(), sourceId);
            if (sourceIt != dataSources_.end()) {
                dataSources_.erase(sourceIt);
            }
            
            return onDataRemove(sourceId);
        }
        
        return false;
    }
    
    bool clearData() override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        dataSources_.clear();
        data_.clear();
        return onDataClear();
    }
    
    bool addPreset(const std::string& name, const VisualizationConfig& config) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return onAddPreset(name, config);
    }
    
    bool removePreset(const std::string& name) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return onRemovePreset(name);
    }
    
    VisualizationConfig getPreset(const std::string& name) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return onGetPreset(name);
    }
    
    std::vector<std::string> getPresets() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return onGetPresets();
    }
    
    bool applyPreset(const std::string& name) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return onApplyPreset(name);
    }
    
    std::vector<std::any> getExtensions() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return extensions_;
    }
    
    bool addExtension(const std::any& extension) override {
        std::lock_guard<std::mutex> lock(mutex_);
        extensions_.push_back(extension);
        return onAddExtension(extension);
    }
    
    bool removeExtension(const std::any& extension) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = std::find(extensions_.begin(), extensions_.end(), extension);
        if (it != extensions_.end()) {
            extensions_.erase(it);
            return onRemoveExtension(extension);
        }
        
        return false;
    }
    
    void setVisualizationManager(void* manager) override {
        std::lock_guard<std::mutex> lock(mutex_);
        manager_ = static_cast<IVisualizationManager*>(manager);
    }
    
    void* getVisualizationManager() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return manager_;
    }
    
    // Error handling
    bool hasError() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return !errors_.empty();
    }
    
    std::string getError() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return errors_.empty() ? "" : errors_.back();
    }
    
    void clearError() override {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.clear();
    }
    
    void addError(const std::string& error) {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.push_back(error);
    }
    
    void clearErrors() {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.clear();
    }
    
protected:
    // Virtual methods for visualization-specific implementation
    virtual bool onConfigure(const VisualizationConfig& config) {
        return true;
    }
    
    virtual bool onDataUpdate(const std::string& sourceId, const VisualizationData& data) {
        return true;
    }
    
    virtual bool onDataRemove(const std::string& sourceId) {
        return true;
    }
    
    virtual bool onDataClear() {
        return true;
    }
    
    virtual bool onInitialize() {
        return true;
    }
    
    virtual bool onRender(double deltaTime) {
        return true;
    }
    
    virtual bool onUpdate(double deltaTime) {
        return true;
    }
    
    virtual bool onShutdown() {
        return true;
    }
    
    virtual bool onStart() {
        return true;
    }
    
    virtual bool onStop() {
        return true;
    }
    
    virtual bool onPause() {
        return true;
    }
    
    virtual bool onResume() {
        return true;
    }
    
    virtual bool onReset() {
        return true;
    }
    
    virtual bool onMouseClick(double x, double y, int button) {
        return false;
    }
    
    virtual bool onMouseMove(double x, double y) {
        return false;
    }
    
    virtual bool onMouseScroll(double delta) {
        return false;
    }
    
    virtual bool onKeyPress(int key, int scancode) {
        return false;
    }
    
    virtual bool onExportToFile(const std::string& filepath, VisualizationFormat format) {
        return false;
    }
    
    virtual bool onExportToStream(std::ostream& stream, VisualizationFormat format) {
        return false;
    }
    
    virtual bool onSaveScreenshot(const std::string& filepath) {
        return false;
    }
    
    virtual bool onEvent(const VisualizationEventData& event) {
        return false;
    }
    
    virtual bool onAddPreset(const std::string& name, const VisualizationConfig& config) {
        return false;
    }
    
    virtual bool onRemovePreset(const std::string& name) {
        return false;
    }
    
    virtual VisualizationConfig onGetPreset(const std::string& name) const {
        throw std::runtime_error("Preset not found: " + name);
    }
    
    virtual std::vector<std::string> onGetPresets() const {
        return {};
    }
    
    virtual bool onApplyPreset(const std::string& name) {
        return false;
    }
    
    virtual bool onAddExtension(const std::any& extension) {
        return true;
    }
    
    virtual bool onRemoveExtension(const std::any& extension) {
        return true;
    }
    
    std::string generateId() {
        std::ostringstream oss;
        oss << name_ << "_" << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "_" << rand();
        return oss.str();
    }
};

// Base visualization manager implementation
class BaseVisualizationManager : public IVisualizationManager {
private:
    std::map<std::string, std::shared_ptr<IVisualization>> visualizations_;
    std::map<std::string, VisualizationConfig> visualizationConfigs_;
    
    std::mutex mutex_;
    std::shared_mutex readWriteMutex_;
    
    IVisualizationManager* globalInstance_;
    
    // Event system
    std::unordered_map<VisualizationEventType, std::vector<std::function<bool(const VisualizationEventData&)>>> eventListeners_;
    
    // Configuration
    VisualizationConfig globalConfig_;
    
    // Presets
    std::map<std::string, VisualizationConfig> presets_;
    
    // Resource management
    size_t maxVisualizations_;
    size_t maxMemoryUsage_;
    
    // Lifecycle callbacks
    std::function<void(const std::string&)> visualizationCreatedCallback_;
    std::function<void(const std::string&)> visualizationDestroyedCallback_;
    std::function<void(const std::string&, const std::string&)> errorCallback_;
    std::function<void(const std::string&, double)> progressCallback_;
    
public:
    BaseVisualizationManager() : globalInstance_(this), maxVisualizations_(10), maxMemoryUsage_(100 * 1024 * 1024) {
        // Initialize global configuration
        globalConfig_.name = "global";
        globalConfig_.type = VisualizationType::NeuralNetwork2D;
        globalConfig_.width = 1024.0;
        globalConfig_.height = 768.0;
        globalConfig_.interactive = true;
        globalConfig_.zoomEnabled = true;
        globalConfig_.panEnabled = true;
        globalConfig_.selectionEnabled = true;
    }
    
    ~BaseVisualizationManager() {
        shutdownAllVisualizations();
    }
    
    // IVisualizationManager interface implementation
    bool createVisualization(const std::string& id, const VisualizationConfig& config) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        if (visualizations_.find(id) != visualizations_.end()) {
            return true; // Already exists
        }
        
        if (visualizations_.size() >= maxVisualizations_) {
            return false;
        }
        
        try {
            std::shared_ptr<IVisualization> visualization = createVisualizationInstance(config.type);
            if (!visualization) {
                return false;
            }
            
            if (!visualization->configure(config)) {
                return false;
            }
            
            if (!visualization->initialize()) {
                return false;
            }
            
            visualizations_[id] = visualization;
            visualizationConfigs_[id] = config;
            
            if (visualizationCreatedCallback_) {
                visualizationCreatedCallback_(id);
            }
            
            return true;
            
        } catch (const std::exception& e) {
            if (errorCallback_) {
                errorCallback_("Visualization creation failed", std::string(e.what()));
            }
            return false;
        }
    }
    
    bool destroyVisualization(const std::string& id) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = visualizations_.find(id);
        if (it == visualizations_.end()) {
            return false;
        }
        
        try {
            std::shared_ptr<IVisualization> visualization = it->second;
            
            if (visualization->isRunning()) {
                visualization->stop();
            }
            
            visualization->shutdown();
            
            visualizations_.erase(it);
            visualizationConfigs_.erase(id);
            
            if (visualizationDestroyedCallback_) {
                visualizationDestroyedCallback_(id);
            }
            
            return true;
            
        } catch (const std::exception& e) {
            if (errorCallback_) {
                errorCallback_(id, std::string(e.what()));
            }
            return false;
        }
    }
    
    bool showVisualization(const std::string& id) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = visualizations_.find(id);
        if (it == visualizations_.end()) {
            return false;
        }
        
        return true;
    }
    
    bool hideVisualization(const std::string& id) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = visualizations_.find(id);
        if (it == visualizations_.end()) {
            return false;
        }
        
        return true;
    }
    
    bool activateVisualization(const std::string& id) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = visualizations_.find(id);
        if (it == visualizations_.end()) {
            return false;
        }
        
        return it->second->start();
    }
    
    bool deactivateVisualization(const std::string& id) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = visualizations_.find(id);
        if (it == visualizations_.end()) {
            return false;
        }
        
        return it->second->stop();
    }
    
    bool startAllVisualizations() override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        bool allSuccess = true;
        for (auto& pair : visualizations_) {
            if (!pair.second->start()) {
                allSuccess = false;
            }
        }
        return allSuccess;
    }
    
    bool stopAllVisualizations() override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        bool allSuccess = true;
        for (auto& pair : visualizations_) {
            if (!pair.second->stop()) {
                allSuccess = false;
            }
        }
        return allSuccess;
    }
    
    bool pauseAllVisualizations() override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        bool allSuccess = true;
        for (auto& pair : visualizations_) {
            if (!pair.second->pause()) {
                allSuccess = false;
            }
        }
        return allSuccess;
    }
    
    bool resumeAllVisualizations() override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        bool allSuccess = true;
        for (auto& pair : visualizations_) {
            if (!pair.second->resume()) {
                allSuccess = false;
            }
        }
        return allSuccess;
    }
    
    bool resetAllVisualizations() override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        bool allSuccess = true;
        for (auto& pair : visualizations_) {
            if (!pair.second->reset()) {
                allSuccess = false;
            }
        }
        return allSuccess;
    }
    
    bool setActiveVisualization(const std::string& id) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = visualizations_.find(id);
        if (it == visualizations_.end()) {
            return false;
        }
        
        // In a real implementation, you would set the active visualization
        // For now, we'll just return success
        return true;
    }
    
    std::string getActiveVisualization() const override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        // Return the first visualization or empty string
        if (!visualizations_.empty()) {
            return visualizations_.begin()->first;
        }
        return "";
    }
    
    std::vector<std::string> getVisibleVisualizations() const override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        
        std::vector<std::string> visible;
        for (const auto& pair : visualizations_) {
            if (pair.second->isRunning() && !pair.second->isPaused()) {
                visible.push_back(pair.first);
            }
        }
        return visible;
    }
    
    std::vector<std::string> getAllVisualizationIds() const override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        
        std::vector<std::string> ids;
        for (const auto& pair : visualizations_) {
            ids.push_back(pair.first);
        }
        return ids;
    }
    
    bool setGlobalConfiguration(const VisualizationConfig& config) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        globalConfig_ = config;
        
        // Update all visualizations
        for (auto& pair : visualizations_) {
            pair.second->configure(config);
        }
        
        return true;
    }
    
    VisualizationConfig getGlobalConfiguration() const override {
        return globalConfig_;
    }
    
    bool addPreset(const std::string& name, const VisualizationConfig& config) override {
        std::lock_guard<std::mutex> lock(mutex_);
        presets_[name] = config;
        return true;
    }
    
    bool removePreset(const std::string& name) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = presets_.find(name);
        if (it == presets_.end()) {
            return false;
        }
        
        presets_.erase(it);
        return true;
    }
    
    VisualizationConfig getPreset(const std::string& name) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = presets_.find(name);
        if (it == presets_.end()) {
            throw std::runtime_error("Preset not found: " + name);
        }
        
        return it->second;
    }
    
    std::vector<std::string> getPresets() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<std::string> names;
        for (const auto& pair : presets_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    double getSystemFPS() const override {
        double totalFPS = 0.0;
        size_t count = 0;
        
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        for (const auto& pair : visualizations_) {
            totalFPS += pair.second->getFPS();
            count++;
        }
        
        return count > 0 ? totalFPS / count : 0.0;
    }
    
    size_t getSystemMemoryUsage() const override {
        size_t totalUsage = 0;
        
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        for (const auto& pair : visualizations_) {
            totalUsage += pair.second->getMemoryUsage();
        }
        
        return totalUsage;
    }
    
    size_t getActiveVisualizationCount() const override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        
        size_t count = 0;
        for (const auto& pair : visualizations_) {
            if (pair.second->isRunning() && !pair.second->isPaused()) {
                count++;
            }
        }
        
        return count;
    }
    
    std::chrono::nanoseconds getSystemUptime() const override {
        return std::chrono::steady_clock::now() - std::chrono::steady_clock::now(); // Placeholder
    }
    
    bool exportAllVisualizations(const std::string& directory, VisualizationFormat format) override {
        bool allSuccess = true;
        
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        for (const auto& pair : visualizations_) {
            std::string filepath = directory + "/" + pair.first + "." + visualizationFormatToString(format);
            if (!pair.second->exportToFile(filepath, format)) {
                allSuccess = false;
            }
        }
        
        return allSuccess;
    }
    
    bool saveSnapshot(const std::string& filepath, const std::string& visualizationId) override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        
        if (!visualizationId.empty()) {
            auto it = visualizations_.find(visualizationId);
            if (it == visualizations_.end()) {
                return false;
            }
            return it->second->saveScreenshot(filepath);
        }
        
        // Save all visualizations if no specific ID provided
        // For now, return success
        return true;
    }
    
    std::string getSystemStatus() const override {
        std::ostringstream oss;
        oss << "Visualization System Status:" << std::endl;
        oss << "  Total visualizations: " << visualizations_.size() << std::endl;
        oss << "  Active visualizations: " << getActiveVisualizationCount() << std::endl;
        oss << "  System FPS: " << getSystemFPS() << std::endl;
        oss << "  System memory usage: " << getSystemMemoryUsage() << " bytes" << std::endl;
        oss << "  Uptime: " << getSystemUptime().count() << " nanoseconds" << std::endl;
        
        return oss.str();
    }
    
    std::string getVisualizationStatus(const std::string& id) const override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = visualizations_.find(id);
        if (it == visualizations_.end()) {
            return "Visualization not found: " + id;
        }
        
        std::ostringstream oss;
        oss << "Visualization: " << id << std::endl;
        oss << "  Type: " << static_cast<int>(it->second->getType()) << std::endl;
        oss << "  Running: " << (it->second->isRunning() ? "Yes" : "No") << std::endl;
        oss << "  Paused: " << (it->second->isPaused() ? "Yes" : "No") << std::endl;
        oss << "  Initialized: " << (it->second->isInitialized() ? "Yes" : "No") << std::endl;
        oss << "  FPS: " << it->second->getFPS() << std::endl;
        oss << "  Progress: " << it->second->getProgress() << "%" << std::endl;
        
        return oss.str();
    }
    
    std::vector<std::string> getVisualizationErrors(const std::string& id) const override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = visualizations_.find(id);
        if (it == visualizations_.end()) {
            return {};
        }
        
        std::vector<std::string> errors;
        if (it->second->hasError()) {
            errors.push_back(it->second->getError());
        }
        return errors;
    }
    
    void clearVisualizationErrors(const std::string& id) override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = visualizations_.find(id);
        if (it != visualizations_.end()) {
            it->second->clearError();
        }
    }
    
    size_t getVisualizationCreateCount(const std::string& id) const override {
        // This would require tracking statistics
        return 0;
    }
    
    size_t getVisualizationDestroyCount(const std::string& id) const override {
        // This would require tracking statistics
        return 0;
    }
    
    double getVisualizationSuccessRate(const std::string& id) const override {
        // This would require tracking statistics
        return 0.0;
    }
    
    std::chrono::nanoseconds getAverageVisualizationRenderTime(const std::string& id) const override {
        // This would require tracking statistics
        return std::chrono::nanoseconds(0);
    }
    
    bool addEventListener(VisualizationEventType eventType,
                         std::function<bool(const VisualizationEventData&)> callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        eventListeners_[eventType].push_back(callback);
        return true;
    }
    
    bool removeEventListener(VisualizationEventType eventType,
                            std::function<bool(const VisualizationEventData&)> callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = eventListeners_.find(eventType);
        if (it == eventListeners_.end()) {
            return false;
        }
        
        it->second.erase(
            std::remove(it->second.begin(), it->second.end(), callback),
            it->second.end()
        );
        
        return true;
    }
    
    void emitEvent(const VisualizationEventData& event) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = eventListeners_.find(event.type);
        if (it != eventListeners_.end()) {
            for (auto& callback : it->second) {
                try {
                    callback(event);
                } catch (const std::exception& e) {
                    if (errorCallback_) {
                        errorCallback_("Event handler error", std::string(e.what()));
                    }
                }
            }
        }
    }
    
    std::shared_ptr<IVisualization> createVisualizationInstance(VisualizationType type) override {
        // This would create a visualization based on the type
        // For now, return nullptr
        return nullptr;
    }
    
    bool cleanup() override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        // Clean up resources
        shutdownAllVisualizations();
        
        return true;
    }
    
    bool optimize() override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        // Optimize visualization resources
        // This is a placeholder
        
        return true;
    }
    
    bool clearCache() override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        // Clear visualization cache
        // This is a placeholder
        
        return true;
    }
    
    void setVisualizationCreatedCallback(std::function<void(const std::string&)> callback) override {
        visualizationCreatedCallback_ = callback;
    }
    
    void setVisualizationDestroyedCallback(std::function<void(const std::string&)> callback) override {
        visualizationDestroyedCallback_ = callback;
    }
    
    void setErrorCallback(std::function<void(const std::string&, const std::string&)> callback) override {
        errorCallback_ = callback;
    }
    
    void setProgressCallback(std::function<void(const std::string&, double)> callback) override {
        progressCallback_ = callback;
    }
    
    IVisualizationManager* getGlobalInstance() const {
        return globalInstance_;
    }
    
    // Helper methods
    std::string visualizationFormatToString(VisualizationFormat format) const {
        switch (format) {
            case VisualizationFormat::PNG: return "png";
            case VisualizationFormat::SVG: return "svg";
            case VisualizationFormat::PDF: return "pdf";
            case VisualizationFormat::JSON: return "json";
            case VisualizationFormat::CSV: return "csv";
            case VisualizationFormat::Text: return "txt";
            case VisualizationFormat::HTML: return "html";
            case VisualizationFormat::WebGL: return "webgl";
            case VisualizationFormat::GlTF: return "gltf";
            case VisualizationFormat::Custom: return "custom";
            default: return "unknown";
        }
        return "unknown";
    }
    
    void shutdownAllVisualizations() {
        std::vector<std::string> ids;
        {
            std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
            ids.reserve(visualizations_.size());
            for (const auto& pair : visualizations_) {
                ids.push_back(pair.first);
            }
        }
        
        for (const auto& id : ids) {
            destroyVisualization(id);
        }
    }
    
    IVisualizationManager* globalInstance_;
};

// Global visualization manager instance
IVisualizationManager& getGlobalVisualizationManager() {
    static BaseVisualizationManager manager;
    return manager;
}

} // namespace nlm
