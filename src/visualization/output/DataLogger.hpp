// ============================================================================
// Data Management and Output
// ============================================================================

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include "VisualizationBase.hpp"
#include "VisualizationConfig.hpp"

namespace nlm {

/**
 * @enum OutputFormat
 * @brief Output format enumeration for visualization data
 */
enum class OutputFormat {
    CSV,           // Comma-separated values for spreadsheet analysis
    JSON,          // JavaScript Object Notation for web visualization
    HDF5,          // Hierarchical Data Format for scientific computing
    Parquet,       // Columnar storage format
    NDArray,       // NumPy-style array format
    VTK,           // Visualization Toolkit for 3D graphics
    MATLAB,        // MATLAB workspace format
    Custom        // User-defined format
};

/**
 * @struct VisualizationMetrics
 * @brief Performance and accuracy metrics for visualization
 */
struct VisualizationMetrics {
    double frameRate;                 // Frames per second
    double renderTime;                // Rendering time in milliseconds
    double updateTime;                // Update time in milliseconds
    double memoryUsage;               // Memory usage in MB
    double cpuUsage;                  // CPU usage percentage
    uint32_t objectsRendered;         // Number of objects rendered
    uint32_t trianglesRendered;       // Number of triangles rendered
    uint32_t drawCalls;               // Number of draw calls
    double compressionRatio;          // Data compression efficiency
    double throughput;                // Data throughput in MB/s
    double latency;                   // End-to-end latency in milliseconds
    double jitter;                    // Frame time variability
    uint32_t errorCount;              // Number of rendering errors
    uint32_t warningCount;            // Number of warnings
};

/**
 * @class DataLogger
 * @brief Data collection and streaming for visualization
 * 
 * This class provides comprehensive data logging capabilities, capturing
 * neural data, visualization state, performance metrics, and user interactions
 * for post-processing, analysis, and replay.
 */
class DataLogger {
public:
    DataLogger();
    ~DataLogger();
    
    // Data collection
    bool startLogging(const std::string& outputPath);
    void stopLogging();
    void pauseLogging();
    void resumeLogging();
    
    // Data recording
    void recordFrame(const VisualizationDataFrame& frame);
    void recordPerformance(const VisualizationMetrics& metrics);
    void recordUserInteraction(const std::string& interactionType, const std::string& parameters);
    void recordVisualizationState(const VisualizationConfig& config);
    
    // Data access
    const std::vector<VisualizationDataFrame>& getRecordedFrames() const { return recordedFrames_; }
    const std::vector<VisualizationMetrics>& getPerformanceData() const { return performanceData_; }
    const std::vector<std::pair<std::string, std::string>>& getUserInteractions() const { return userInteractions_; }
    
    // File operations
    bool saveToFile(const std::string& filename, OutputFormat format) const;
    bool loadFromFile(const std::string& filename, OutputFormat format);
    
    // Data processing
    void clear();
    void setFrameRate(double fps) { frameRate_ = fps; }
    void setBufferSize(size_t size) { bufferSize_ = size; }
    double getRecordingDuration() const { return recordingDuration_; }
    
    // Analysis
    double calculateFrameRate() const;
    double calculateAverageRenderTime() const;
    double calculateMemoryUsage() const;
    std::string generateStatistics() const;
    
    // Health monitoring
    bool isHealthy() const;
    std::string getHealthReport() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Recorded data
    std::vector<VisualizationDataFrame> recordedFrames_;
    std::vector<VisualizationMetrics> performanceData_;
    std::vector<std::pair<std::string, std::string>> userInteractions_;
    
    // Configuration
    double frameRate_;
    size_t bufferSize_;
    double recordingDuration_;
    std::string outputPath_;
    bool isLogging_;
};

/**
 * @class VisualizationInterface
 * @brief Output interface (OpenGL, WebGL, Terminal, etc.)
 * 
 * This class provides the main output interface for visualization systems,
 * supporting multiple rendering backends and output formats. It handles
 * the conversion from internal data representation to specific output formats.
 */
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Interface management
    bool initialize(const VisualizationConfig& config);
    void update(const VisualizationDataFrame& frame);
    void render();
    void close();
    
    // Output format control
    bool setOutputFormat(VisualizationOutputFormat format);
    VisualizationOutputFormat getOutputFormat() const { return outputFormat_; }
    
    // Backend management
    bool setRendererType(RendererType type);
    RendererType getRendererType() const { return rendererType_; }
    
    // Configuration
    bool configure(const VisualizationConfig& config);
    VisualizationConfig getConfiguration() const { return config_; }
    
    // Data streaming
    void setDataStream(VisualizationDataStream* stream) { dataStream_ = stream; }
    bool pushFrame(const VisualizationDataFrame& frame);
    bool getNextFrame(VisualizationDataFrame& frame);
    
    // Performance control
    void setTargetFPS(double fps) { targetFPS_ = fps; }
    double getCurrentFPS() const { return currentFPS_; }
    void setVSync(bool enabled) { vsync_ = enabled; }
    bool isVSync() const { return vsync_; }
    
    // Output control
    bool setOutputSize(uint32_t width, uint32_t height);
    void setOutputPath(const std::string& path) { outputPath_ = path; }
    
    // Window and display
    void showWindow();
    void hideWindow();
    bool isWindowVisible() const { return windowVisible_; }
    void setWindowTitle(const std::string& title) { windowTitle_ = title; }
    
    // Interaction
    void setInteractive(bool enabled) { interactive_ = enabled; }
    bool isInteractive() const { return interactive_; }
    void setMouseCapture(bool capture) { mouseCapture_ = capture; }
    bool isMouseCaptured() const { return mouseCapture_; }
    
    // Statistics and monitoring
    VisualizationMetrics getMetrics() const { return metrics_; }
    double getFrameTime() const { return frameTime_; }
    double getRenderTime() const { return renderTime_; }
    
    // Error handling
    bool hasError() const { return hasError_; }
    std::string getErrorMessage() const { return errorMessage_; }
    void clearError() { hasError_ = false; errorMessage_.clear(); }
    
    // Health monitoring
    bool isHealthy() const;
    std::string getHealthReport() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration
    VisualizationConfig config_;
    VisualizationOutputFormat outputFormat_;
    RendererType rendererType_;
    
    // Performance
    double targetFPS_;
    double currentFPS_;
    double frameTime_;
    double renderTime_;
    bool vsync_;
    
    // Display
    uint32_t outputWidth_;
    uint32_t outputHeight_;
    std::string outputPath_;
    std::string windowTitle_;
    bool windowVisible_;
    
    // Interaction
    bool interactive_;
    bool mouseCapture_;
    
    // Data
    VisualizationDataStream* dataStream_;
    VisualizationMetrics metrics_;
    
    // Error handling
    bool hasError_;
    std::string errorMessage_;
};

/**
 * @class VisualizationExporter
 * @brief Data export and analysis
 * 
 * This class provides comprehensive data export capabilities, converting
 * visualization data into various formats for analysis, reporting, and
 * integration with other systems. It supports statistical analysis,
 * pattern detection, and data transformation.
 */
class VisualizationExporter {
public:
    VisualizationExporter();
    ~VisualizationExporter();
    
    // Export configuration
    bool configure(const VisualizationConfig& config);
    VisualizationConfig getConfiguration() const { return config_; }
    
    // Export to file
    bool exportData(const std::string& filename, OutputFormat format, const VisualizationDataFrame& frame);
    bool exportMultipleFrames(const std::string& filename, OutputFormat format, const std::vector<VisualizationDataFrame>& frames);
    bool exportAnalysisResults(const std::string& filename, OutputFormat format, const std::string& analysisType);
    
    // Export to database
    bool exportToDatabase(const std::string& connectionString, const std::string& tableName, const VisualizationDataFrame& frame);
    bool exportToDatabaseBatch(const std::string& connectionString, const std::string& tableName, const std::vector<VisualizationDataFrame>& frames);
    
    // Statistical analysis
    double calculateCorrelation(const VisualizationDataFrame& frame, const std::string& metric1, const std::string& metric2) const;
    double calculateRegression(const VisualizationDataFrame& frame, const std::string& dependent, const std::string& independent) const;
    std::vector<double> calculatePrincipalComponents(const VisualizationDataFrame& frame, size_t numComponents) const;
    std::vector<std::vector<double>> calculateClustering(const VisualizationDataFrame& frame, size_t numClusters) const;
    
    // Pattern detection
    std::vector<std::vector<uint32_t>> detectTemporalPatterns(const VisualizationDataFrame& frame, uint32_t windowSize, double threshold) const;
    std::vector<std::vector<uint32_t>> detectSpatialPatterns(const VisualizationDataFrame& frame, double similarityThreshold) const;
    std::vector<std::vector<uint32_t>> detectAnomalies(const VisualizationDataFrame& frame, double anomalyThreshold) const;
    
    // Data transformation
    VisualizationDataFrame transformData(const VisualizationDataFrame& frame, const std::function<double(double)>& transform) const;
    VisualizationDataFrame normalizeData(const VisualizationDataFrame& frame) const;
    VisualizationDataFrame filterData(const VisualizationDataFrame& frame, const std::function<bool(const VisualizationDataFrame&)>& filter) const;
    
    // Quality assessment
    double calculateQualityScore(const VisualizationDataFrame& frame) const;
    std::string assessDataQuality(const VisualizationDataFrame& frame) const;
    std::vector<std::string> getQualityIssues(const VisualizationDataFrame& frame) const;
    
    // Health monitoring
    bool isHealthy() const;
    std::string getHealthReport() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    VisualizationConfig config_;
};

/**
 * @class VisualizationRenderer
 * @brief Rendering engine for different output formats
 * 
 * This class provides the core rendering engine that handles the actual
 * rendering of visualization data across different output formats and
 * backends. It manages shader programs, texture assets, 3D transformations,
 * and rendering pipelines.
 */
class VisualizationRenderer {
public:
    VisualizationRenderer();
    ~VisualizationRenderer();
    
    // Rendering configuration
    bool initialize(const VisualizationConfig& config);
    void update(const VisualizationConfig& config);
    void render(const VisualizationDataFrame& frame);
    void close();
    
    // Rendering pipeline
    void beginFrame();
    void endFrame();
    void clear(const std::string& color = "black");
    void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    
    // 3D transformations
    void pushMatrix();
    void popMatrix();
    void loadIdentity();
    void translate(float x, float y, float z);
    void rotate(float angle, float x, float y, float z);
    void scale(float x, float y, float z);
    void multMatrix(const std::vector<float>& matrix);
    
    // Shader management
    bool createShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
    bool bindShader(const std::string& name);
    bool setUniform(const std::string& uniformName, float value);
    bool setUniform(const std::string& uniformName, const std::vector<float>& values);
    bool setUniform(const std::string& uniformName, const std::string& value);
    
    // Texture management
    bool createTexture(const std::string& name, const std::vector<uint8_t>& data, uint32_t width, uint32_t height);
    bool bindTexture(const std::string& name, uint32_t slot = 0);
    
    // Geometric primitives
    void drawPoint(float x, float y, float z, float size = 1.0f);
    void drawLine(float x1, float y1, float z1, float x2, float y2, float z2);
    void drawTriangle(const std::vector<float>& vertices);
    void drawMesh(const std::vector<float>& vertices, const std::vector<uint32_t>& indices);
    void drawSphere(float radius, uint32_t segments = 16);
    void drawCylinder(float radius, float height, uint32_t segments = 8);
    void drawCone(float radius, float height, uint32_t segments = 8);
    void drawBox(float width, float height, float depth);
    
    // Text rendering
    void drawText(const std::string& text, float x, float y, float z, float scale = 1.0f);
    
    // Performance monitoring
    double getFrameTime() const { return frameTime_; }
    double getFPS() const { return fps_; }
    uint32_t getTriangleCount() const { return triangleCount_; }
    uint32_t getDrawCallCount() const { return drawCallCount_; }
    
    // Statistics
    uint64_t getTotalFramesRendered() const { return totalFramesRendered_; }
    uint64_t getTotalPrimitivesRendered() const { return totalPrimitivesRendered_; }
    std::map<std::string, uint32_t> getShaderStats() const { return shaderStats_; }
    
    // Health monitoring
    bool isHealthy() const;
    std::string getHealthReport() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Rendering state
    bool initialized_;
    double frameTime_;
    double fps_;
    uint32_t triangleCount_;
    uint32_t drawCallCount_;
    uint64_t totalFramesRendered_;
    uint64_t totalPrimitivesRendered_;
    
    // Statistics
    std::map<std::string, uint32_t> shaderStats_;
    std::map<std::string, uint32_t> textureStats_;
};

} // namespace nlm
