#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @enum VisualizationOutputFormat
 * @brief Output format enumeration for visualization rendering
 */
enum class VisualizationOutputFormat {
    Terminal,      // Text-based visualization for remote access
    OpenGL,        // Hardware-accelerated rendering
    Vulkan,        // Modern GPU rendering API
    WebGL,         // Browser-based visualization
    HTML,          // Web-based dashboard
    CSV,           // Data export for analysis
    JSON,          // Structured data export
    PNG,           // Still image export
    Video,         // Video sequence export
    Database       // Direct database storage
};

/**
 * @enum ColorScheme
 * @brief Color scheme enumeration for visualization themes
 */
enum class ColorScheme {
    Default,        // Standard blue-white-red scheme
    Scientific,     // Publication-ready colors
    Dark,           // Dark background with bright foreground
    Light,          // Light background with dark foreground
    NeuroRed,       // Red tones for excitation/inhibition
    BlueGreen,      // Cyan-blue-green for neural activity
    Metabolism,     // Orange-red for energy/activity
    Custom         // User-defined color palette
};

/**
 * @enum VisualizationMode
 * @brief Visualization mode enumeration
 */
enum class VisualizationMode {
    Static,        // Static snapshots
    RealTime,      // Continuous updates
    EventDriven,   // Update on significant events
    TimeLapse,     // Accelerated time viewing
    Replay,        // Replay recorded data
    Comparison     // Multiple states/comparisons
};

/**
 * @struct VisualizationSettings
 * @brief Comprehensive configuration for visualization components
 * 
 * This structure holds all configuration parameters for the visualization system,
 * including display settings, output formats, colors, performance parameters,
 * and interaction controls.
 */
struct VisualizationSettings {
    // Display configuration
    VisualizationOutputFormat outputFormat;
    std::string outputPath;
    uint32_t width;
    uint32_t height;
    uint32_t dpi;
    bool fullScreen;
    
    // Color and appearance
    ColorScheme colorScheme;
    std::map<std::string, std::string> customColors;
    float opacity;
    bool useTransparency;
    
    // Update and timing
    VisualizationMode mode;
    uint32_t updateFrequency;  // Hz
    double timeScale;
    uint64_t bufferSize;
    uint32_t maxLatency;  // milliseconds
    
    // Visualization content
    bool showNeurons;
    bool showSynapses;
    bool showConnections;
    bool showMemory;
    bool showPlasticity;
    bool showNeuromodulation;
    bool showDevelopment;
    bool showPredictions;
    
    // 3D visualization
    bool enable3D;
    float cameraDistance;
    float cameraAngleX;
    float cameraAngleY;
    float fieldOfView;
    
    // Data visualization
    bool showHeatmaps;
    bool showTimeSeries;
    bool showStatistics;
    bool showProbabilityMaps;
    uint32_t dataPointsPerFrame;
    
    // Interaction
    bool enableInteraction;
    bool enableZoom;
    bool enablePan;
    bool enableRotate;
    bool enableSelect;
    std::string interactionMode;  // "mouse", "keyboard", "gesture"
    
    // Performance
    uint32_t targetFPS;
    bool enableVSync;
    bool enableAsyncRendering;
    uint32_t threadCount;
    bool enableCaching;
    uint32_t cacheSize;
    
    // Output
    bool exportData;
    std::vector<std::string> exportFormats;
    bool generateThumbnails;
    bool generateVideo;
    uint32_t videoFPS;
    uint32_t videoBitrate;
    
    // Analysis
    bool enableAnalysis;
    bool enableStatistics;
    bool enablePatternDetection;
    bool enableAnomalyDetection;
    std::vector<std::string> analysisTypes;
    
    // Network and streaming
    uint32_t streamPort;
    std::string streamHost;
    uint32_t streamBufferSize;
    bool enableStreaming;
    std::string streamProtocol;  // "tcp", "udp", "websockets"
    
    // Security
    bool enableAuthentication;
    std::string authenticationToken;
    bool enableEncryption;
    
    // Logging and debugging
    bool enableLogging;
    std::string logLevel;  // "debug", "info", "warning", "error"
    bool logPerformance;
    bool logDataAccess;
    bool logErrors;
    
    // Validation and constraints
    uint32_t maxNeurons;
    uint32_t maxSynapses;
    uint32_t maxMemoryEntries;
    double maxUpdateLatency;
    uint32_t maxFrameTime;  // milliseconds
};

/**
 * @class VisualizationConfig
 * @brief Main configuration manager for visualization system
 * 
 * This class provides a comprehensive configuration system for all visualization
 * components. It supports loading from files, saving to files, validation,
 * and dynamic configuration updates.
 */
class VisualizationConfig {
public:
    VisualizationConfig();
    ~VisualizationConfig();
    
    // Loading and saving
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    bool loadFromString(const std::string& configString);
    std::string saveToString() const;
    
    // Configuration access
    const VisualizationSettings& getSettings() const { return settings_; }
    VisualizationSettings& getSettings() { return settings_; }
    
    // Individual setting access
    template<typename T>
    bool get(const std::string& key, T& value) const;
    
    template<typename T>
    void set(const std::string& key, const T& value);
    
    // Setting validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const { return validationErrors_; }
    
    // Configuration profiles
    bool setProfile(const std::string& profileName);
    std::vector<std::string> getAvailableProfiles() const;
    
    // Preset configurations
    static VisualizationSettings createDefaultSettings();
    static VisualizationSettings createPerformanceSettings();
    static VisualizationSettings createQualitySettings();
    static VisualizationSettings createResearchSettings();
    static VisualizationSettings createEducationSettings();
    
    // Configuration comparison
    bool equals(const VisualizationConfig& other) const;
    double calculateDifference(const VisualizationConfig& other) const;
    
    // Configuration statistics
    size_t getSize() const;
    double getMemoryUsage() const;
    
    // Configuration merging
    void merge(const VisualizationConfig& other, bool overwrite = true);
    void resetToDefaults();
    
    // Configuration hashing
    uint64_t calculateHash() const;
    
    // Configuration serialization
    std::vector<uint8_t> serialize() const;
    bool deserialize(const std::vector<uint8_t>& data);
    
    // Configuration monitoring
    bool isDirty() const { return dirty_; }
    void markDirty() { dirty_ = true; }
    void markClean() { dirty_ = false; }
    
    // Configuration history
    void enableHistory(bool enable) { enableHistory_ = enable; }
    bool isHistoryEnabled() const { return enableHistory_; }
    std::vector<VisualizationSettings> getHistory() const { return history_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    VisualizationSettings settings_;
    std::map<std::string, std::string> settingsMap_;
    std::vector<std::string> validationErrors_;
    
    mutable bool dirty_;
    bool enableHistory_;
    std::vector<VisualizationSettings> history_;
    uint64_t version_;
    std::string profileName_;
};

} // namespace nlm
