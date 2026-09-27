// Visualization and Debugging System
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <mutex>

namespace nlm {

// Visualization output formats
enum class VisualizationFormat {
    ASCII_ART,
    JSON,
    CSV,
    VRML,
    SVG,
    HTML,
    BINARY
};

// Visualization targets
enum class VisualizationTarget {
    NEURON_ACTIVITY,
    SYNAPSE_WEIGHTS,
    BRAIN_STATES,
    MEMORY_CONTENT,
    NEUROMODULATION,
    DEVELOPMENTAL_STAGES,
    SPATIAL_DISTRIBUTION,
    TEMPORAL_DYNAMICS,
    CONNECTION_PATTERNS,
    ENERGY_DISTRIBUTION,
    CUSTOM
};

// Visualization settings
struct VisualizationSettings {
    VisualizationFormat format;
    double updateInterval; // in seconds
    bool enabled;
    bool saveToFile;
    std::string outputFile;
    std::string outputDirectory;
    bool showLegend;
    bool showGrid;
    std::string colorScheme;
    double scaleFactor;
    bool animated;
    int framesPerSecond;
    std::map<std::string, std::string> customOptions;
    
    VisualizationSettings() : format(VisualizationFormat::ASCII_ART), updateInterval(1.0),
        enabled(true), saveToFile(false), showLegend(true), showGrid(false),
        colorScheme("default"), scaleFactor(1.0), animated(false), framesPerSecond(30) {}
};

// Visualization data
struct VisualizationData {
    Timestamp timestamp;
    std::string title;
    std::map<std::string, std::vector<float>> data;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> labels;
    
    VisualizationData() : timestamp(0) {}
    VisualizationData(const std::string& t) : title(t), timestamp(0) {}
};

// Visualization overlay
struct VisualizationOverlay {
    std::string id;
    std::string type; // "text", "line", "rectangle", "circle", "neuron", "synapse"
    double x;
    double y;
    double width;
    double height;
    std::string color;
    std::string text;
    double opacity;
    bool visible;
    
    VisualizationOverlay() : x(0), y(0), width(1), height(1), color("black"),
        text(""), opacity(1.0), visible(true) {}
};

// Visualization interface
class VisualizationInterface {
public:
    virtual ~VisualizationInterface() = default;
    
    // Initialization
    virtual bool initialize(const VisualizationSettings& settings) = 0;
    virtual void shutdown() = 0;
    
    // Visualization control
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual bool isRunning() const = 0;
    
    // Data visualization
    virtual void visualize(const VisualizationData& data) = 0;
    virtual void addOverlay(const VisualizationOverlay& overlay) = 0;
    virtual void removeOverlay(const std::string& id) = 0;
    
    // Settings
    virtual void setSettings(const VisualizationSettings& settings) = 0;
    virtual VisualizationSettings getSettings() const = 0;
    
    // Output
    virtual bool saveVisualization(const std::string& filename) = 0;
    virtual std::string generateOutput() const = 0;
    
    // Configuration
    virtual bool configure(const std::map<std::string, std::string>& config) = 0;
    virtual std::map<std::string, std::string> getConfig() const = 0;
};

// ASCII art visualization
class ASCIIArtVisualization : public VisualizationInterface {
public:
    bool initialize(const VisualizationSettings& settings) override;
    void shutdown() override;
    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    bool isRunning() const override;
    void visualize(const VisualizationData& data) override;
    void addOverlay(const VisualizationOverlay& overlay) override;
    void removeOverlay(const std::string& id) override;
    void setSettings(const VisualizationSettings& settings) override;
    VisualizationSettings getSettings() const override;
    bool saveVisualization(const std::string& filename) override;
    std::string generateOutput() const override;
    bool configure(const std::map<std::string, std::string>& config) override;
    std::map<std::string, std::string> getConfig() const override;
    
    // ASCII art specific methods
    std::string generateNeuronActivity(const std::vector<float>& activity, int width, int height) const;
    std::string generateSynapseWeights(const std::vector<float>& weights, int width, int height) const;
    std::string generateBrainState(const std::map<std::string, std::vector<float>>& states) const;
    std::string generateMemoryContent(const std::vector<float>& memory) const;
    
private:
    VisualizationSettings settings_;
    bool running_;
    std::mutex mutex_;
    std::vector<VisualizationOverlay> overlays_;
    std::vector<VisualizationData> dataHistory_;
};

// JSON visualization
class JSONVisualization : public VisualizationInterface {
public:
    bool initialize(const VisualizationSettings& settings) override;
    void shutdown() override;
    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    bool isRunning() const override;
    void visualize(const VisualizationData& data) override;
    void addOverlay(const VisualizationOverlay& overlay) override;
    void removeOverlay(const std::string& id) override;
    void setSettings(const VisualizationSettings& settings) override;
    VisualizationSettings getSettings() const override;
    bool saveVisualization(const std::string& filename) override;
    std::string generateOutput() const override;
    bool configure(const std::map<std::string, std::string>& config) override;
    std::map<std::string, std::string> getConfig() const override;
    
    // JSON specific methods
    std::string generateJSONFrame(const VisualizationData& data) const;
    std::string generateJSONMetadata(const VisualizationData& data) const;
    
private:
    VisualizationSettings settings_;
    bool running_;
    std::mutex mutex_;
    std::vector<VisualizationOverlay> overlays_;
    std::vector<VisualizationData> dataHistory_;
};

// CSV visualization
class CSVVisualization : public VisualizationInterface {
public:
    bool initialize(const VisualizationSettings& settings) override;
    void shutdown() override;
    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    bool isRunning() const override;
    void visualize(const VisualizationData& data) override;
    void addOverlay(const VisualizationOverlay& overlay) override;
    void removeOverlay(const std::string& id) override;
    void setSettings(const VisualizationSettings& settings) override;
    VisualizationSettings getSettings() const override;
    bool saveVisualization(const std::string& filename) override;
    std::string generateOutput() const override;
    bool configure(const std::map<std::string, std::string>& config) override;
    std::map<std::string, std::string> getConfig() const override;
    
    // CSV specific methods
    std::string generateCSVData(const VisualizationData& data) const;
    std::string generateCSVMetadata() const;
    
private:
    VisualizationSettings settings_;
    bool running_;
    std::mutex mutex_;
    std::vector<VisualizationOverlay> overlays_;
    std::vector<VisualizationData> dataHistory_;
    std::ofstream csvFile_;
};

// Debug visualization
class DebugVisualization : public VisualizationInterface {
public:
    bool initialize(const VisualizationSettings& settings) override;
    void shutdown() override;
    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    bool isRunning() const override;
    void visualize(const VisualizationData& data) override;
    void addOverlay(const VisualizationOverlay& overlay) override;
    void removeOverlay(const std::string& id) override;
    void setSettings(const VisualizationSettings& settings) override;
    VisualizationSettings getSettings() const override;
    bool saveVisualization(const std::string& filename) override;
    std::string generateOutput() const override;
    bool configure(const std::map<std::string, std::string>& config) override;
    std::map<std::string, std::string> getConfig() const override;
    
    // Debug specific methods
    void showDebugInfo(const std::string& title, const std::string& info);
    void showPerformanceStats(const std::map<std::string, double>& stats);
    void showErrorReport(const std::string& error);
    void showWarningReport(const std::string& warning);
    
    // Debug visualization
    std::string generateDebugInfo(const std::string& title, const std::string& info) const;
    std::string generatePerformanceStats(const std::map<std::string, double>& stats) const;
    
private:
    VisualizationSettings settings_;
    bool running_;
    std::mutex mutex_;
    std::vector<VisualizationOverlay> overlays_;
    std::vector<std::pair<std::string, std::string>> debugMessages_;
    std::vector<std::pair<std::string, double>> performanceStats_;
};

// Visualization manager
class VisualizationManager {
public:
    static VisualizationManager& getInstance() {
        static VisualizationManager instance;
        return instance;
    }
    
    // Visualization interface management
    bool addVisualization(VisualizationInterface* visualization);
    bool removeVisualization(const std::string& id);
    bool enableVisualization(const std::string& id);
    bool disableVisualization(const std::string& id);
    bool setActiveVisualization(const std::string& id);
    std::string getActiveVisualization() const;
    
    // Global visualization control
    void startAllVisualizations();
    void stopAllVisualizations();
    void pauseAllVisualizations();
    void resumeAllVisualizations();
    
    // Data visualization
    void visualize(const VisualizationData& data);
    void addDataSource(const std::string& id, const std::function<VisualizationData()>& source);
    
    // Configuration
    void setGlobalSettings(const VisualizationSettings& settings);
    VisualizationSettings getGlobalSettings() const;
    
    // Output management
    bool saveAllVisualizations(const std::string& directory);
    bool exportAllData(const std::string& filename);
    
    // State management
    void clear();
    bool isEmpty() const;
    
    // Statistics
    size_t getVisualizationCount() const;
    size_t getEnabledVisualizationCount() const;
    
    // Configuration file
    bool loadConfiguration(const std::string& filename);
    bool saveConfiguration(const std::string& filename) const;
    
private:
    VisualizationManager() : activeVisualizationId_("") {}
    
    struct VisualizationEntry {
        std::string id;
        VisualizationInterface* visualization;
        bool enabled;
        VisualizationSettings settings;
    };
    
    std::vector<VisualizationEntry> visualizations_;
    std::string activeVisualizationId_;
    VisualizationSettings globalSettings_;
    std::mutex mutex_;
    std::map<std::string, std::function<VisualizationData()>> dataSources_;
};

// Visualization utilities
namespace visualization_utils {
    // Color utilities
    std::string getColor(const std::string& colorName);
    std::string interpolateColor(const std::string& color1, const std::string& color2, double ratio);
    std::string hexToRGB(const std::string& hex);
    std::string RGBToHex(int r, int g, int b);
    
    // ASCII art utilities
    std::string scaleText(const std::string& text, int scale);
    std::string centerText(const std::string& text, int width);
    std::string wrapText(const std::string& text, int width);
    
    // Data visualization utilities
    std::vector<float> normalizeData(const std::vector<float>& data);
    std::vector<float> clampData(const std::vector<float>& data, float min, float max);
    std::vector<float> smoothData(const std::vector<float>& data, int windowSize);
    
    // File utilities
    bool createDirectory(const std::string& path);
    bool fileExists(const std::string& filename);
    std::string getFileExtension(const std::string& filename);
    std::string getTimestampString();
    
    // Output formatting
    std::string formatNumber(double number, int precision = 2);
    std::string formatPercentage(double value, int precision = 1);
    std::string formatTime(double seconds);
    
    // Visualization generation
    std::string generateHeatmap(const std::vector<float>& data, int width, int height);
    std::string generateBarChart(const std::vector<float>& values, const std::vector<std::string>& labels);
    std::string generateLineChart(const std::vector<float>& values, const std::vector<Timestamp>& timestamps);
    
    // Configuration
    VisualizationSettings parseSettingsFromString(const std::string& settingsStr);
    std::string generateSettingsString(const VisualizationSettings& settings);
} // namespace visualization_utils

// Visualization-specific exceptions
class VisualizationException : public std::runtime_error {
public:
    enum class ErrorCode {
        INITIALIZATION_FAILED = 1,
        INVALID_FORMAT = 2,
        INVALID_SETTINGS = 3,
        OUTPUT_FAILED = 4,
        NOT_SUPPORTED = 5,
        UNKNOWN = 99
    };
    
    VisualizationException(ErrorCode code, const std::string& message, const std::string& target = "")
        : std::runtime_error(message), code_(code), target_(target) {}
    
    ErrorCode getCode() const { return code_; }
    const std::string& getTarget() const { return target_; }
    
private:
    ErrorCode code_;
    std::string target_;
};

// Visualization factory
class VisualizationFactory {
public:
    static VisualizationFactory& getInstance() {
        static VisualizationFactory instance;
        return instance;
    }
    
    // Visualization creation
    static VisualizationInterface* createVisualization(const std::string& type);
    static VisualizationInterface* createVisualization(const std::string& type, const VisualizationSettings& settings);
    
    // Visualization discovery
    static std::vector<std::string> getAvailableVisualizations();
    static bool isVisualizationAvailable(const std::string& type);
    
    // Configuration
    static bool registerVisualization(const std::string& type, const std::string& description);
    static bool unregisterVisualization(const std::string& type);
    
    // Plugin support
    static bool loadVisualizationPlugin(const std::string& path);
    static bool unloadVisualizationPlugin(const std::string& name);
    
private:
    VisualizationFactory() = default;
    
    struct VisualizationInfo {
        std::string type;
        std::string description;
        std::function<VisualizationInterface*()> creator;
    };
    
    std::vector<VisualizationInfo> visualizations_;
};

// Visualization helper macros
#define NLM_VISUALIZE(data, target) VisualizationManager::getInstance().visualize(data, target)
#define NLM_ADD_OVERLAY(overlay) VisualizationManager::getInstance().addOverlay(overlay)
#define NLM_REMOVE_OVERLAY(id) VisualizationManager::getInstance().removeOverlay(id)
#define NLM_SET_ACTIVE_VISUALIZATION(id) VisualizationManager::getInstance().setActiveVisualization(id)

#define NLM_VISUALIZATION_ERROR(code, msg, target) \
    do { \
        throw VisualizationException(code, msg, target); \
    } while(0)

} // namespace nlm