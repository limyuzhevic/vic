#include "VisualizationEngine.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <ctime>

namespace nlm {

VisualizationEngine::VisualizationEngine()
    : currentDisplayMode_(DisplayMode::Consolidated), renderingMode_(RenderingMode::Terminal),
      isRunning_(false), sessionId_(""), monitorPerformance_(true), targetFPS_(30.0),
      debugMode_(false), elapsedTime_(0.0), frameCount_(0) {
    initializeComponents();
}

void VisualizationEngine::initializeComponents() {
    // Initialize core components
    colorManager_ = std::make_unique<ColorManager>();
    layoutManager_ = std::make_unique<LayoutManager>();
    inputHandler_ = std::make_unique<InputHandler>();
    brainStructureRenderer_ = std::make_unique<BrainStructureRenderer>();
    activityRenderer_ = std::make_unique<ActivityRenderer>();
    memoryRenderer_ = std::make_unique<MemoryRenderer>();
    progressRenderer_ = std::make_unique<ProgressRenderer>();
    exportRenderer_ = std::make_unique<ExportRenderer>();
    
    // Configure screen size
    layoutManager_->setScreenSize(config_.screenWidth, config_.screenHeight);
    
    // Configure color scheme
    colorManager_->setColorScheme(config_.colorScheme);
    
    // Set up input handling
    inputHandler_->setInputMode(InputHandler::InputType::Keyboard);
    inputHandler_->enableInputHistory(true);
    
    // Configure renderers
    brainStructureRenderer_->setShowNeuronLabels(config_.showFPS);
    activityRenderer_->setShowStatistics(config_.showFPS);
    memoryRenderer_->setShowMemoryLabels(config_.showFPS);
    progressRenderer_->setShowProgressHistory(config_.showFPS);
    
    // Initialize export renderer
    exportRenderer_->setOutputDirectory("./exports");
    exportRenderer_->enableExportFormat(ExportRenderer::ExportFormat::ASCII, true);
    exportRenderer_->enableExportFormat(ExportRenderer::ExportFormat::CSV, true);
    exportRenderer_->enableExportFormat(ExportRenderer::ExportFormat::JSON, true);
}

bool VisualizationEngine::initialize(std::shared_ptr<Brain> brain) {
    if (!brain) {
        lastError_ = "Invalid brain pointer provided";
        return false;
    }
    
    setBrain(brain);
    
    // Validate brain availability
    if (!validateBrainAvailability()) {
        lastError_ = "Brain validation failed";
        return false;
    }
    
    // Initialize components with brain data
    brainStructureRenderer_->setBrain(*brain);
    activityRenderer_->setBrain(*brain);
    memoryRenderer_->setBrain(*brain);
    progressRenderer_->setBrain(*brain);
    
    // Start session
    startSession("session_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
    
    isRunning_ = true;
    lastUpdateTime_ = std::chrono::steady_clock::now();
    lastFrameTime_ = lastUpdateTime_;
    
    // Log initialization
    std::cout << "Visualization engine initialized successfully" << std::endl;
    std::cout << "Display mode: " << static_cast<int>(currentDisplayMode_) << std::endl;
    std::cout << "Rendering mode: " << static_cast<int>(renderingMode_) << std::endl;
    std::cout << "Screen size: " << config_.screenWidth << "x" << config_.screenHeight << std::endl;
    
    return true;
}

void VisualizationEngine::update() {
    if (!isRunning_ || !brain_) {
        return;
    }
    
    // Calculate elapsed time
    auto currentTime = std::chrono::steady_clock::now();
    elapsedTime_ = std::chrono::duration<double>(currentTime - lastUpdateTime_).count();
    lastUpdateTime_ = currentTime;
    
    // Update performance metrics
    updatePerformanceMetrics();
    
    // Update brain renderers
    brain_->step(brain_->getCurrentStep(), brain_->getSimulationTime());
    
    brainStructureRenderer_->update(*brain_);
    activityRenderer_->update(*brain_);
    memoryRenderer_->update(*brain_);
    progressRenderer_->update(*brain_);
    
    // Handle display mode changes
    handleDisplayModeChanges();
    
    // Update layout and components
    updateLayoutAndComponents();
    
    // Log frame time
    double frameTime = std::chrono::duration<double>(currentTime - lastFrameTime_).count();
    lastFrameTime_ = currentTime;
    logFrameTime(frameTime);
    
    frameCount_++;
}

void VisualizationEngine::render() {
    if (!isRunning_ || !brain_) {
        return;
    }
    
    // Clear screen
    std::cout << "\033[2J\033[H";
    
    // Render based on current display mode
    renderCurrentMode();
    
    // Render performance metrics if enabled
    if (config_.showFPS) {
        renderPerformanceDisplay();
    }
    
    // Call visualization callback if registered
    if (visualizationCallback_) {
        visualizationCallback_();
    }
}

void VisualizationEngine::renderCurrentMode() {
    switch (currentDisplayMode_) {
        case DisplayMode::Structural:
            brainStructureRenderer_->render(*brain_);
            break;
        case DisplayMode::Activity:
            activityRenderer_->render(*brain_);
            break;
        case DisplayMode::Memory:
            memoryRenderer_->render(*brain_);
            break;
        case DisplayMode::Progress:
            progressRenderer_->render(*brain_);
            break;
        case DisplayMode::Debug:
            renderDebugMode();
            break;
        case DisplayMode::Consolidated:
            renderConsolidatedView(*brain_);
            break;
    }
}

void VisualizationEngine::renderConsolidatedView(const Brain& brain) {
    // Render consolidated view showing all major components
    ColorManager colorManager;
    
    // Title
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightYellow) << "=== NLM NEURAL VISUALIZATION SYSTEM ===" << "\033[0m" << std::endl;
    std::cout << std::endl;
    
    // Status bar
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Status: RUNNING" << "\033[0m" 
              << "  Step: " << brain.getCurrentStep()
              << "  Time: " << brain.getSimulationTime() << "s"
              << "  Neurons: " << brain.getTotalNeuronCount()
              << "  Synapses: " << brain.getTotalSynapseCount()
              << std::endl;
    std::cout << std::endl;
    
    // Three-column layout
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightGreen) << "[ NEURAL STRUCTURE ]" << "\033[0m" << std::endl;
    brainStructureRenderer_->renderNodes(brain);
    std::cout << std::endl;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightCyan) << "[ ACTIVITY MONITOR ]" << "\033[0m" << std::endl;
    activityRenderer_->renderFiringRates(brain);
    std::cout << std::endl;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightRed) << "[ MEMORY SYSTEM ]" << "\033[0m" << std::endl;
    memoryRenderer_->renderWorkingMemory(brain);
    std::cout << std::endl;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightBlue) << "[ EXPERIMENT STATUS ]" << "\033[0m" << std::endl;
    progressRenderer_->renderExperimentState(brain);
    std::cout << std::endl;
}

void VisualizationEngine::renderDebugMode() {
    // Render debug information
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightYellow) << "=== DEBUG MODE ===" << "\033[0m" << std::endl;
    std::cout << std::endl;
    
    // Render debug information from various components
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Color Manager:" << "\033[0m" << std::endl;
    std::cout << "  Colors enabled: " << (colorManager.isColorEnabled() ? "Yes" : "No") << std::endl;
    std::cout << "  True color support: " << (colorManager.isTrueColorSupported() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Layout Manager:" << "\033[0m" << std::endl;
    const auto& screenInfo = layoutManager_->getScreenInfo();
    std::cout << "  Screen: " << screenInfo.width << "x" << screenInfo.height << std::endl;
    std::cout << "  Available: " << screenInfo.availableWidth << "x" << screenInfo.availableHeight << std::endl;
    std::cout << std::endl;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Performance:" << "\033[0m" << std::endl;
    std::cout << "  FPS: " << performanceMetrics_.fps << std::endl;
    std::cout << "  Frame time: " << performanceMetrics_.frameTime * 1000 << "ms" << std::endl;
    std::cout << "  Memory usage: " << performanceMetrics_.memoryUsage << " KB" << std::endl;
    std::cout << "  Active neurons: " << performanceMetrics_.activeNeurons << std::endl;
    std::cout << "  Total spikes: " << performanceMetrics_.totalSpikes << std::endl;
    std::cout << std::endl;
    
    // Render component statistics
    renderComponentStatistics();
}

void VisualizationEngine::renderPerformanceDisplay() const {
    ColorManager colorManager;
    
    // Render FPS and performance information
    std::cout << "\033[2J\033[H"; // Clear screen for performance display
    
    std::cout << colorManager.getColorCode(ColorManager::Color::BrightCyan) << "PERFORMANCE MONITOR" << "\033[0m" << std::endl;
    std::cout << std::endl;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "FPS: " << performanceMetrics_.fps << " (Target: " << targetFPS_ << ")" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Green) << "Frame time: " << performanceMetrics_.frameTime * 1000 << "ms" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Yellow) << "Memory usage: " << performanceMetrics_.memoryUsage << " KB" << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Red) << "Active neurons: " << performanceMetrics_.activeNeurons << "\033[0m" << std::endl;
    std::cout << colorManager.getColorCode(ColorManager::Color::Magenta) << "Total spikes: " << performanceMetrics_.totalSpikes << "\033[0m" << std::endl;
    std::cout << std::endl;
}

void VisualizationEngine::renderComponentStatistics() const {
    ColorManager colorManager;
    
    std::cout << colorManager.getColorCode(ColorManager::Color::White) << "Component Statistics:" << "\033[0m" << std::endl;
    
    // Render statistics from each component
    if (brainStructureRenderer_) {
        std::cout << "  Brain Structure: " << "Ready" << std::endl;
    }
    
    if (activityRenderer_) {
        std::cout << "  Activity Renderer: " << "Ready" << std::endl;
    }
    
    if (memoryRenderer_) {
        std::cout << "  Memory Renderer: " << "Ready" << std::endl;
    }
    
    if (progressRenderer_) {
        std::cout << "  Progress Renderer: " << "Ready" << std::endl;
    }
    
    if (exportRenderer_) {
        std::cout << "  Export Renderer: " << "Ready" << std::endl;
    }
    
    std::cout << std::endl;
}

void VisualizationEngine::updatePerformanceMetrics() {
    if (!monitorPerformance_) return;
    
    auto currentTime = std::chrono::steady_clock::now();
    double frameTime = std::chrono::duration<double>(currentTime - lastFrameTime_).count();
    
    // Calculate FPS
    performanceMetrics_.fps = 1.0 / std::max(0.001, frameTime);
    performanceMetrics_.frameTime = frameTime;
    performanceMetrics_.lastFrameTime = currentTime;
    
    // Update other metrics (simplified)
    if (brain_) {
        performanceMetrics_.activeNeurons = brain_->getActiveNeuronCount();
        performanceMetrics_.totalSpikes = brain_->getTotalSpikeCount();
    }
    
    // Check performance issues
    if (performanceMetrics_.fps < targetFPS_ * 0.5) {
        lastError_ = "Performance warning: FPS below target (" + std::to_string(performanceMetrics_.fps) + ")";
    }
}

void VisualizationEngine::handleDisplayModeChanges() {
    // Handle display mode changes based on user input or system conditions
    // This is a simplified implementation
}

void VisualizationEngine::updateLayoutAndComponents() {
    // Update layout and component positions
    // This is a simplified implementation
}

bool VisualizationEngine::validateBrainAvailability() const {
    return brain_ != nullptr && brain_->getTotalNeuronCount() > 0;
}

std::string VisualizationEngine::calculateFPS(double frameTime) const {
    return std::to_string(1.0 / std::max(0.001, frameTime));
}

void VisualizationEngine::logFrameTime(double frameTime) const {
    // Log frame time for performance monitoring
    // This is a simplified implementation
}

void VisualizationEngine::cleanupResources() {
    // Clean up resources
    isRunning_ = false;
    brain_.reset();
    
    // Clear components
    colorManager_.reset();
    layoutManager_.reset();
    inputHandler_.reset();
    brainStructureRenderer_.reset();
    activityRenderer_.reset();
    memoryRenderer_.reset();
    progressRenderer_.reset();
    exportRenderer_.reset();
}

void VisualizationEngine::validateSystemResources() const {
    // Validate system resources (simplified)
}

void VisualizationEngine::startSession(const std::string& sessionId) {
    sessionId_ = sessionId;
    sessionStartTime_ = std::chrono::steady_clock::now();
    
    std::cout << "Started visualization session: " << sessionId_ << std::endl;
    std::cout << "Session start time: " << getSessionStartTimeString() << std::endl;
}

void VisualizationEngine::endSession() {
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - sessionStartTime_);
    
    std::cout << "Ended visualization session: " << sessionId_ << std::endl;
    std::cout << "Session duration: " << duration.count() << " seconds" << std::endl;
    
    // Generate session report
    generateReport();
    
    // Clean up
    cleanupResources();
}

std::string VisualizationEngine::getSessionStartTimeString() const {
    auto time = std::chrono::system_clock::to_time_t(sessionStartTime_);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    char timeStr[32];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &tm_buf);
    
    return std::string(timeStr);
}

void VisualizationEngine::generateReport() const {
    // Generate a comprehensive report of the visualization session
    std::cout << "Generating visualization report..." << std::endl;
    
    // In a real implementation, this would generate a detailed report
    // with statistics, performance metrics, and session information
}

std::string VisualizationEngine::getSystemStatus() const {
    std::stringstream ss;
    ss << "System Status:" << std::endl;
    ss << "=============" << std::endl;
    ss << "Running: " << (isRunning_ ? "Yes" : "No") << std::endl;
    ss << "Brain Available: " << (brain_ ? "Yes" : "No") << std::endl;
    ss << "Session ID: " << sessionId_ << std::endl;
    ss << "Uptime: " << getUptime() << " seconds" << std::endl;
    ss << "Performance FPS: " << performanceMetrics_.fps << std::endl;
    
    return ss.str();
}

double VisualizationEngine::getUptime() const {
    if (sessionStartTime_.time_since_epoch().count() == 0) return 0.0;
    
    auto currentTime = std::chrono::steady_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - sessionStartTime_);
    
    return uptime.count();
}

void VisualizationEngine::logMessage(const std::string& message) const {
    // Log a message with timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::cout << "[" << time_t << "] " << message << std::endl;
}

std::string VisualizationEngine::getVersionString() const {
    return "NLM Visualization Engine v1.0.0";
}

bool VisualizationEngine::checkSystemHealth() const {
    // Check system health (simplified)
    return isRunning_ && brain_ && !lastError_.empty();
}

void VisualizationEngine::logDebugInfo() const {
    if (!debugMode_) return;
    
    std::cout << "Debug Information:" << std::endl;
    std::cout << "================" << std::endl;
    std::cout << "Session ID: " << sessionId_ << std::endl;
    std::cout << "Brain Pointer: " << (brain_ ? "Valid" : "Invalid") << std::endl;
    std::cout << "Performance FPS: " << performanceMetrics_.fps << std::endl;
    std::cout << "Last Error: " << lastError_ << std::endl;
}

void VisualizationEngine::printSystemInfo() const {
    std::cout << "System Information:" << std::endl;
    std::cout << "==================" << std::endl;
    std::cout << "Visualization Engine Version: " << getVersionString() << std::endl;
    std::cout << "Screen Size: " << config_.screenWidth << "x" << config_.screenHeight << std::endl;
    std::cout << "Display Mode: " << static_cast<int>(currentDisplayMode_) << std::endl;
    std::cout << "Rendering Mode: " << static_cast<int>(renderingMode_) << std::endl;
    std::cout << "Update Interval: " << config_.updateInterval << "s" << std::endl;
    std::cout << "Auto Refresh: " << (config_.autoRefresh ? "Yes" : "No") << std::endl;
    std::cout << "Color Scheme: " << config_.colorScheme << std::endl;
    std::cout << "Theme: " << config_.theme << std::endl;
}

bool VisualizationEngine::exportData(const ExportRenderer::ExportOptions& options) {
    if (!exportRenderer_) {
        lastError_ = "Export renderer not available";
        return false;
    }
    
    // Collect data for export
    auto activityData = collectExportData();
    auto memoryData = exportRenderer_->collectMemoryData();
    auto progressData = exportRenderer_->collectProgressData();
    
    // Export based on format
    switch (options.format) {
        case ExportRenderer::ExportFormat::ASCII:
            return exportRenderer_->exportActivityData(options, activityData);
        case ExportRenderer::ExportFormat::CSV:
            return exportRenderer_->exportActivityData(options, activityData);
        case ExportRenderer::ExportFormat::JSON:
            return exportRenderer_->exportActivityData(options, activityData);
        default:
            lastError_ = "Export format not supported";
            return false;
    }
}

bool VisualizationEngine::saveVisualization(const std::string& filename) {
    // Save visualization state
    std::cout << "Saving visualization to: " << filename << std::endl;
    
    // In a real implementation, this would save the visualization state
    // to a file or database
    
    return true;
}

std::string VisualizationEngine::captureScreenshot() const {
    // Capture screenshot (simplified)
    return "Screenshot functionality not yet implemented";
}

bool VisualizationEngine::handleInput(const std::string& input) {
    if (!inputHandler_) {
        return false;
    }
    
    return inputHandler_->processInput(input);
}

bool VisualizationEngine::handleMouseEvent(int x, int y, InputHandler::MouseButton button, bool pressed) {
    if (!inputHandler_) {
        return false;
    }
    
    if (pressed) {
        inputHandler_->onMouseButtonPress(button, x, y);
    } else {
        inputHandler_->onMouseButtonRelease(button, x, y);
    }
    
    inputHandler_->onMouseMove(x, y);
    
    return true;
}

bool VisualizationEngine::handleKeyboardEvent(InputHandler::Key key, bool ctrl, bool alt, bool shift, bool command) {
    if (!inputHandler_) {
        return false;
    }
    
    inputHandler_->onKeyPress(key, ctrl, alt, shift, command);
    
    return true;
}

void VisualizationEngine::loadConfiguration(const std::string& configFile) {
    configFile_ = configFile;
    // In a real implementation, this would load configuration from file
    std::cout << "Loading configuration from: " << configFile_ << std::endl;
}

void VisualizationEngine::saveConfiguration() const {
    // Save current configuration
    std::cout << "Saving configuration..." << std::endl;
    
    // In a real implementation, this would save configuration to file
}

void VisualizationEngine::resetConfiguration() {
    // Reset to default configuration
    config_ = VisualizationConfig();
    initializeComponents();
    std::cout << "Configuration reset to defaults" << std::endl;
}

std::string VisualizationEngine::getCurrentConfiguration() const {
    std::stringstream ss;
    ss << "Current Configuration:" << std::endl;
    ss << "=====================" << std::endl;
    ss << "Display Mode: " << static_cast<int>(currentDisplayMode_) << std::endl;
    ss << "Rendering Mode: " << static_cast<int>(renderingMode_) << std::endl;
    ss << "Screen Size: " << config_.screenWidth << "x" << config_.screenHeight << std::endl;
    ss << "Update Interval: " << config_.updateInterval << "s" << std::endl;
    ss << "Auto Refresh: " << (config_.autoRefresh ? "Yes" : "No") << std::endl;
    ss << "Color Scheme: " << config_.colorScheme << std::endl;
    ss << "Theme: " << config_.theme << std::endl;
    
    return ss.str();
}

std::string VisualizationEngine::getAvailableThemes() const {
    return "default,dark,light,high_contrast";
}

std::string VisualizationEngine::getAvailableColorSchemes() const {
    return "default,monochrome,heatmap,retro";
}

std::vector<ExportRenderer::ActivityData> VisualizationEngine::collectExportData() const {
    // Collect data for export
    std::vector<ExportRenderer::ActivityData> data;
    
    if (!brain_) return data;
    
    // Add sample data
    ExportRenderer::ActivityData sample;
    sample.neuronId = "neuron_001";
    sample.activity = 0.75f;
    sample.timestamp = brain_->getSimulationTime();
    sample.step = brain_->getCurrentStep();
    sample.regionId = "cortex";
    sample.populationId = "exc_pop";
    
    data.push_back(sample);
    
    return data;
}

void VisualizationEngine::close() {
    // Close visualization engine
    endSession();
    
    std::cout << "Visualization engine closed" << std::endl;
}

} // namespace nlm
