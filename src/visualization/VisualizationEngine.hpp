#pragma once

#include "Brain.hpp"
#include "ColorManager.hpp"
#include "LayoutManager.hpp"
#include "InputHandler.hpp"
#include "BrainStructureRenderer.hpp"
#include "ActivityRenderer.hpp"
#include "MemoryRenderer.hpp"
#include "ProgressRenderer.hpp"
#include "ExportRenderer.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>

namespace nlm {

class VisualizationEngine {
public:
    enum class DisplayMode {
        Structural,
        Activity,
        Memory,
        Progress,
        Debug,
        Consolidated
    };
    
    enum class RenderingMode {
        Terminal,
        Export,
        Hybrid
    };
    
    struct VisualizationConfig {
        DisplayMode displayMode;
        RenderingMode renderingMode;
        int screenWidth;
        int screenHeight;
        double updateInterval;
        bool autoRefresh;
        bool showFPS;
        std::string colorScheme;
        std::string theme;
        bool enableAnimations;
        float animationSpeed;
        std::map<std::string, std::string> customSettings;
        
        VisualizationConfig() : displayMode(DisplayMode::Consolidated), renderingMode(RenderingMode::Terminal),
                               screenWidth(80), screenHeight(40), updateInterval(0.1f), autoRefresh(true),
                               showFPS(false), colorScheme("default"), theme("default"),
                               enableAnimations(true), animationSpeed(1.0f) {}
    };
    
    struct PerformanceMetrics {
        double fps;
        double frameTime;
        size_t memoryUsage;
        size_t activeNeurons;
        size_t totalSpikes;
        float cpuUsage;
        float memoryUsagePercent;
        std::chrono::steady_clock::time_point lastFrameTime;
        
        PerformanceMetrics() : fps(0.0), frameTime(0.0), memoryUsage(0), activeNeurons(0),
                              totalSpikes(0), cpuUsage(0.0), memoryUsagePercent(0.0) {}
    };
    
    VisualizationEngine();
    ~VisualizationEngine() = default;
    
    // Configuration
    void setConfig(const VisualizationConfig& config) { config_ = config; }
    void setConfigFile(const std::string& configFile) { configFile_ = configFile; }
    VisualizationConfig getConfig() const { return config_; }
    
    // Engine control
    bool initialize(std::shared_ptr<Brain> brain);
    void update();
    void render();
    void close();
    bool isRunning() const { return isRunning_; }
    
    // Display control
    void setDisplayMode(DisplayMode mode) { currentDisplayMode_ = mode; }
    DisplayMode getDisplayMode() const { return currentDisplayMode_; }
    void setRenderingMode(RenderingMode mode) { renderingMode_ = mode; }
    RenderingMode getRenderingMode() const { return renderingMode_; }
    
    // Brain integration
    void setBrain(std::shared_ptr<Brain> brain) { brain_ = brain; }
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    // Component access
    ColorManager* getColorManager() { return colorManager_.get(); }
    LayoutManager* getLayoutManager() { return layoutManager_.get(); }
    InputHandler* getInputHandler() { return inputHandler_.get(); }
    BrainStructureRenderer* getBrainStructureRenderer() { return brainStructureRenderer_.get(); }
    ActivityRenderer* getActivityRenderer() { return activityRenderer_.get(); }
    MemoryRenderer* getMemoryRenderer() { return memoryRenderer_.get(); }
    ProgressRenderer* getProgressRenderer() { return progressRenderer_.get(); }
    ExportRenderer* getExportRenderer() { return exportRenderer_.get(); }
    
    // Performance monitoring
    PerformanceMetrics getPerformanceMetrics() const { return performanceMetrics_; }
    void enablePerformanceMonitoring(bool enable) { monitorPerformance_ = enable; }
    void setTargetFPS(double fps) { targetFPS_ = fps; }
    
    // Export functionality
    bool exportData(const ExportRenderer::ExportOptions& options);
    bool saveVisualization(const std::string& filename);
    std::string captureScreenshot() const;
    
    // Interaction and controls
    bool handleInput(const std::string& input);
    bool handleMouseEvent(int x, int y, InputHandler::MouseButton button, bool pressed);
    bool handleKeyboardEvent(InputHandler::Key key, bool ctrl, bool alt, bool shift, bool command);
    void registerVisualizationCallback(std::function<void()> callback) { visualizationCallback_ = callback; }
    
    // Configuration and settings
    void loadConfiguration(const std::string& configFile);
    void saveConfiguration() const;
    void resetConfiguration();
    std::string getCurrentConfiguration() const;
    
    // Session management
    void startSession(const std::string& sessionId);
    void endSession();
    std::string getSessionId() const { return sessionId_; }
    std::chrono::steady_clock::time_point getSessionStartTime() const { return sessionStartTime_; }
    
    // Statistics and reporting
    void generateReport() const;
    std::string getSystemStatus() const;
    double getUptime() const;
    
    // Utility functions
    void logMessage(const std::string& message) const;
    std::string getVersionString() const;
    bool checkSystemHealth() const;
    void clearErrors() { lastError_.clear(); }
    std::string getLastError() const { return lastError_; }
    
    // Calibration and optimization
    void calibrateDisplay();
    void optimizePerformance();
    void validateResources();
    
    // Theme and styling
    void setTheme(const std::string& theme) { config_.theme = theme; }
    void setColorScheme(const std::string& scheme) { config_.colorScheme = scheme; }
    std::string getAvailableThemes() const;
    std::string getAvailableColorSchemes() const;
    
    // Debug and development
    void enableDebugMode(bool enable) { debugMode_ = enable; }
    bool isDebugMode() const { return debugMode_; }
    void logDebugInfo() const;
    void printSystemInfo() const;
    
private:
    VisualizationConfig config_;
    std::string configFile_;
    std::shared_ptr<Brain> brain_;
    
    DisplayMode currentDisplayMode_;
    RenderingMode renderingMode_;
    bool isRunning_;
    std::string sessionId_;
    std::chrono::steady_clock::time_point sessionStartTime_;
    
    // Core components
    std::unique_ptr<ColorManager> colorManager_;
    std::unique_ptr<LayoutManager> layoutManager_;
    std::unique_ptr<InputHandler> inputHandler_;
    std::unique_ptr<BrainStructureRenderer> brainStructureRenderer_;
    std::unique_ptr<ActivityRenderer> activityRenderer_;
    std::unique_ptr<MemoryRenderer> memoryRenderer_;
    std::unique_ptr<ProgressRenderer> progressRenderer_;
    std::unique_ptr<ExportRenderer> exportRenderer_;
    
    // Performance monitoring
    PerformanceMetrics performanceMetrics_;
    bool monitorPerformance_;
    double targetFPS_;
    std::chrono::steady_clock::time_point lastFrameTime_;
    
    // State management
    std::chrono::steady_clock::time_point lastUpdateTime_;
    double elapsedTime_;
    size_t frameCount_;
    
    // Callbacks and events
    std::function<void()> visualizationCallback_;
    
    // Error handling
    std::string lastError_;
    bool debugMode_;
    
    void initializeComponents();
    void updatePerformanceMetrics();
    void handleDisplayModeChanges();
    void updateLayoutAndComponents();
    void renderCurrentMode();
    bool validateBrainAvailability() const;
    std::string calculateFPS(double frameTime) const;
    void logFrameTime(double frameTime) const;
    void cleanupResources();
    void validateSystemResources() const;
};

} // namespace nlm
