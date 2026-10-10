#include "BrainVisualizer.hpp"
#include "BrainDebugger.hpp"
#include "BrainSimulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace nlm {

BrainVisualizer::BrainVisualizer(std::shared_ptr<Brain> brain, std::shared_ptr<BrainDebugger> debugger,
                                 std::shared_ptr<VisualizationInterface> visualizationInterface)
    : brain_(brain), debugger_(debugger), visualizationInterface_(visualizationInterface),
      isActive_(false), isPaused_(false), isRunning_(false), isCollectingData_(false),
      updateIntervalMs_(100), displayMode_(0), interactionEnabled_(true),
      debugModeEnabled_(false), simulationSpeed_(1.0), lastFrameTime_(0.0), 
      frameCount_(0), droppedFrameCount_(0) {
    
    cleanup();
    NLM_LOG_INFO("BrainVisualizer initialized");
}

BrainVisualizer::~BrainVisualizer() {
    shutdown();
}

void BrainVisualizer::cleanup() {
    if (isCollectingData_) {
        stopDataCollection();
    }
    if (isRunning()) {
        stopRealTimeVisualization();
    }
    
    // Cleanup renderers
    brainStructureRenderer_.reset();
    activityRenderer_.reset();
    memoryRenderer_.reset();
    progressRenderer_.reset();
    exportRenderer_.reset();
    
    if (dataFile_.is_open()) {
        dataFile_.close();
    }
    
    // Reset visualization state
    currentConfig_ = VisualizationConfig();
    layout_ = "default";
    style_ = "default";
    colorScheme_ = "default";
    
    // Reset statistics
    visualizationStats_ = VisualizationStats();
    lastFrameTime_ = 0.0;
    frameCount_ = 0;
    droppedFrameCount_ = 0;
}

bool BrainVisualizer::initialize() {
    if (!brain_) {
        NLM_LOG_ERROR("BrainVisualizer: No brain reference available");
        return false;
    }
    
    // Initialize visualization interface if not provided
    if (!visualizationInterface_) {
        visualizationInterface_ = std::make_shared<VisualizationInterface>();
        if (!visualizationInterface_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize visualization interface");
            return false;
        }
    }
    
    // Initialize renderers
    initializeRenderers();
    
    // Setup default configuration
    setupColorScheme();
    setupLayout();
    setupRenderers();
    
    // Validate setup
    if (!validateVisualizationSetup()) {
        NLM_LOG_ERROR("Visualization setup validation failed");
        return false;
    }
    
    isActive_ = true;
    
    NLM_LOG_INFO("BrainVisualizer initialized successfully");
    return true;
}

void BrainVisualizer::shutdown() {
    if (isRunning()) {
        stopRealTimeVisualization();
    }
    
    if (isCollectingData()) {
        stopDataCollection();
    }
    
    if (visualizationInterface_) {
        visualizationInterface_->shutdown();
    }
    
    cleanup();
    
    NLM_LOG_INFO("BrainVisualizer shutdown completed");
}

void BrainVisualizer::initializeRenderers() {
    if (!visualizationInterface_) return;
    
    // Create renderers
    brainStructureRenderer_ = std::make_unique<BrainStructureRenderer>();
    activityRenderer_ = std::make_unique<ActivityRenderer>();
    memoryRenderer_ = std::make_unique<MemoryRenderer>();
    progressRenderer_ = std::make_unique<ProgressRenderer>();
    exportRenderer_ = std::make_unique<ExportRenderer>();
    
    // Initialize renderers
    if (brainStructureRenderer_) {
        brainStructureRenderer_->initialize(visualizationInterface_);
    }
    if (activityRenderer_) {
        activityRenderer_->initialize(visualizationInterface_);
    }
    if (memoryRenderer_) {
        memoryRenderer_->initialize(visualizationInterface_);
    }
    if (progressRenderer_) {
        progressRenderer_->initialize(visualizationInterface_);
    }
    if (exportRenderer_) {
        exportRenderer_->initialize(visualizationInterface_);
    }
    
    NLM_LOG_INFO("All renderers initialized");
}

void BrainVisualizer::displayBrainStructure() {
    displayBrainStructure("default", "default");
}

void BrainVisualizer::displayBrainStructure(const std::string& layout, const std::string& style) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    setLayout(layout);
    setStyle(style);
    
    displayBrainStructure(currentConfig_);
}

void BrainVisualizer::displayBrainStructure(const VisualizationConfig& config) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!brainStructureRenderer_) {
        NLM_LOG_ERROR("Brain structure renderer not initialized");
        return;
    }
    
    NLM_LOG_INFO("Displaying brain structure with layout: " + layout_ + ", style: " + style_);
    
    // Render brain structure
    brainStructureRenderer_->renderBrainStructure(brain_, config);
    
    // Render neurons based on configuration
    if (config.showNeurons) {
        displayNeurons();
    }
    
    // Render synapses based on configuration
    if (config.showSynapses) {
        displaySynapses(config.synapseWeightThreshold);
    }
    
    // Render regions based on configuration
    if (config.showRegions) {
        displayRegions();
    }
    
    // Export visualization data if requested
    if (config.exportData && !config.exportFilename.empty()) {
        exportVisualizationData(config.exportFilename, config.exportFormat);
    }
}

void BrainVisualizer::displayNeurons() {
    displayNeurons(NeuronType::Excitatory, colorScheme_ + "_exc");
}

void BrainVisualizer::displayNeurons(NeuronType type, const std::string& color) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!brainStructureRenderer_) {
        NLM_LOG_ERROR("Brain structure renderer not initialized");
        return;
    }
    
    brainStructureRenderer_->renderNeuronsByType(brain_, type, color);
}

void BrainVisualizer::displayNeurons(const std::vector<NeuronId>& neuronIds) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!brainStructureRenderer_) {
        NLM_LOG_ERROR("Brain structure renderer not initialized");
        return;
    }
    
    brainStructureRenderer_->renderSpecificNeurons(brain_, neuronIds);
}

void BrainVisualizer::displayNeurons(const VisualizationFilter& filter) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!brainStructureRenderer_) {
        NLM_LOG_ERROR("Brain structure renderer not initialized");
        return;
    }
    
    brainStructureRenderer_->renderNeuronsWithFilter(brain_, filter);
}

void BrainVisualizer::displaySynapses() {
    displaySynapses(0.0f); // Show all synapses
}

void BrainVisualizer::displaySynapses(float weightThreshold) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!brainStructureRenderer_) {
        NLM_LOG_ERROR("Brain structure renderer not initialized");
        return;
    }
    
    brainStructureRenderer_->renderSynapses(brain_, weightThreshold);
}

void BrainVisualizer::displaySynapses(const std::vector<SynapseId>& synapseIds) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!brainStructureRenderer_) {
        NLM_LOG_ERROR("Brain structure renderer not initialized");
        return;
    }
    
    brainStructureRenderer_->renderSpecificSynapses(brain_, synapseIds);
}

void BrainVisualizer::displaySynapses(RegionId sourceRegion, RegionId targetRegion) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!brainStructureRenderer_) {
        NLM_LOG_ERROR("Brain structure renderer not initialized");
        return;
    }
    
    brainStructureRenderer_->renderRegionToRegionConnections(brain_, sourceRegion, targetRegion);
}

void BrainVisualizer::displayRegions() {
    displayRegions(NeuronType::Excitatory); // Default to excitatory regions
}

void BrainVisualizer::displayRegions(NeuronType dominantType) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!brainStructureRenderer_) {
        NLM_LOG_ERROR("Brain structure renderer not initialized");
        return;
    }
    
    brainStructureRenderer_->renderRegions(brain_, dominantType);
}

void BrainVisualizer::displayRegionConnections(RegionId regionId) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!brainStructureRenderer_) {
        NLM_LOG_ERROR("Brain structure renderer not initialized");
        return;
    }
    
    brainStructureRenderer_->renderRegionConnections(brain_, regionId);
}

void BrainVisualizer::displayRegionActivity(RegionId regionId) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!activityRenderer_) {
        NLM_LOG_ERROR("Activity renderer not initialized");
        return;
    }
    
    activityRenderer_->renderRegionActivity(brain_, regionId);
}

void BrainVisualizer::displayRegionStatistics(RegionId regionId) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!progressRenderer_) {
        NLM_LOG_ERROR("Progress renderer not initialized");
        return;
    }
    
    progressRenderer_->showRegionStatistics(brain_, regionId);
}

void BrainVisualizer::showNeuralActivity() {
    showNeuralActivity(1.0); // Default time window
}

void BrainVisualizer::showNeuralActivity(double timeWindow) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!activityRenderer_) {
        NLM_LOG_ERROR("Activity renderer not initialized");
        return;
    }
    
    activityRenderer_->renderNeuralActivity(brain_, timeWindow);
}

void BrainVisualizer::showNeuralActivity(NeuronId neuronId) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!activityRenderer_) {
        NLM_LOG_ERROR("Activity renderer not initialized");
        return;
    }
    
    activityRenderer_->renderSpecificNeuronActivity(brain_, neuronId);
}

void BrainVisualizer::showNeuralActivity(RegionId regionId) {
    displayRegionActivity(regionId);
}

void BrainVisualizer::showNeuralActivity(const std::vector<NeuronId>& neuronIds) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!activityRenderer_) {
        NLM_LOG_ERROR("Activity renderer not initialized");
        return;
    }
    
    activityRenderer_->renderMultipleNeuronActivity(brain_, neuronIds);
}

void BrainVisualizer::showNeuralActivity(const VisualizationFilter& filter) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!activityRenderer_) {
        NLM_LOG_ERROR("Activity renderer not initialized");
        return;
    }
    
    activityRenderer_->renderActivityWithFilter(brain_, filter);
}

void BrainVisualizer::displayLearningCurves() {
    displayLearningCurves(100); // Default max points
}

void BrainVisualizer::displayLearningCurves(size_t maxPoints) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!progressRenderer_) {
        NLM_LOG_ERROR("Progress renderer not initialized");
        return;
    }
    
    progressRenderer_->showLearningCurves(brain_, maxPoints);
}

void BrainVisualizer::displayLearningCurves(const std::string& metric, double timeWindow) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!progressRenderer_) {
        NLM_LOG_ERROR("Progress renderer not initialized");
        return;
    }
    
    progressRenderer_->showSpecificLearningMetric(brain_, metric, timeWindow);
}

void BrainVisualizer::displayLearningCurves(const std::vector<std::string>& metrics) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!progressRenderer_) {
        NLM_LOG_ERROR("Progress renderer not initialized");
        return;
    }
    
    progressRenderer_->showMultipleLearningMetrics(brain_, metrics);
}

void BrainVisualizer::displayLearningStatistics() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!progressRenderer_) {
        NLM_LOG_ERROR("Progress renderer not initialized");
        return;
    }
    
    progressRenderer_->showLearningStatistics(brain_);
}

void BrainVisualizer::showMemoryState() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!memoryRenderer_) {
        NLM_LOG_ERROR("Memory renderer not initialized");
        return;
    }
    
    memoryRenderer_->renderMemoryState(brain_);
}

void BrainVisualizer::showMemoryState(MemorySystem system) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!memoryRenderer_) {
        NLM_LOG_ERROR("Memory renderer not initialized");
        return;
    }
    
    memoryRenderer_->renderSpecificMemorySystem(brain_, system);
}

void BrainVisualizer::showMemoryState(const std::string& memoryName) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!memoryRenderer_) {
        NLM_LOG_ERROR("Memory renderer not initialized");
        return;
    }
    
    memoryRenderer_->renderMemoryByName(brain_, memoryName);
}

void BrainVisualizer::showMemoryCapacity() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!memoryRenderer_) {
        NLM_LOG_ERROR("Memory renderer not initialized");
        return;
    }
    
    memoryRenderer_->showMemoryCapacity(brain_);
}

void BrainVisualizer::showMemoryUtilization() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!memoryRenderer_) {
        NLM_LOG_ERROR("Memory renderer not initialized");
        return;
    }
    
    memoryRenderer_->showMemoryUtilization(brain_);
}

void BrainVisualizer::showMemoryPerformance() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!memoryRenderer_) {
        NLM_LOG_ERROR("Memory renderer not initialized");
        return;
    }
    
    memoryRenderer_->showMemoryPerformance(brain_);
}

void BrainVisualizer::showPerformanceMetrics() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!progressRenderer_) {
        NLM_LOG_ERROR("Progress renderer not initialized");
        return;
    }
    
    progressRenderer_->showPerformanceMetrics(brain_);
}

void BrainVisualizer::showPerformanceMetrics(const std::vector<std::string>& metrics) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!progressRenderer_) {
        NLM_LOG_ERROR("Progress renderer not initialized");
        return;
    }
    
    progressRenderer_->showSpecificMetrics(brain_, metrics);
}

void BrainVisualizer::showPerformanceMetrics(double timeWindow) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!progressRenderer_) {
        NLM_LOG_ERROR("Progress renderer not initialized");
        return;
    }
    
    progressRenderer_->showMetricsWithTimeWindow(brain_, timeWindow);
}

void BrainVisualizer::showPerformanceDashboard() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!progressRenderer_) {
        NLM_LOG_ERROR("Progress renderer not initialized");
        return;
    }
    
    progressRenderer_->showPerformanceDashboard(brain_);
}

void BrainVisualizer::showPerformanceStatistics() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!progressRenderer_) {
        NLM_LOG_ERROR("Progress renderer not initialized");
        return;
    }
    
    progressRenderer_->showPerformanceStatistics(brain_);
}

void BrainVisualizer::startRealTimeVisualization(unsigned int updateIntervalMs) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (isRunning()) {
        NLM_LOG_WARNING("Real-time visualization already running");
        return;
    }
    
    setUpdateInterval(updateIntervalMs);
    
    monitoringRequested_ = true;
    isRunning_ = true;
    
    visualizationThread_ = std::thread(&BrainVisualizer::updateVisualization, this);
    
    NLM_LOG_INFO("Real-time visualization started with interval " + std::to_string(updateIntervalMs) + "ms");
}

void BrainVisualizer::stopRealTimeVisualization() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!isRunning()) {
        return;
    }
    
    monitoringRequested_ = false;
    isRunning_ = false;
    
    if (visualizationThread_.joinable()) {
        visualizationThread_.join();
    }
    
    NLM_LOG_INFO("Real-time visualization stopped");
}

bool BrainVisualizer::isVisualizationActive() const {
    return isActive_.load();
}

void BrainVisualizer::setUpdateInterval(unsigned int intervalMs) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    updateIntervalMs_ = intervalMs;
}

unsigned int BrainVisualizer::getUpdateInterval() const {
    return updateIntervalMs_.load();
}

void BrainVisualizer::pauseVisualization() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    isPaused_ = true;
    NLM_LOG_INFO("Visualization paused");
}

void BrainVisualizer::resumeVisualization() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    isPaused_ = false;
    NLM_LOG_INFO("Visualization resumed");
}

void BrainVisualizer::stopVisualization() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    isActive_ = false;
    stopRealTimeVisualization();
    NLM_LOG_INFO("Visualization stopped");
}

bool BrainVisualizer::isVisualizationPaused() const {
    return isPaused_.load();
}

bool BrainVisualizer::isVisualizationRunning() const {
    return isRunning_.load();
}

void BrainVisualizer::setDisplayMode(int mode) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    displayMode_ = mode;
}

int BrainVisualizer::getDisplayMode() const {
    return displayMode_.load();
}

void BrainVisualizer::setLayout(const std::string& layout) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    layout_ = layout;
}

std::string BrainVisualizer::getLayout() const {
    return layout_.load();
}

void BrainVisualizer::setStyle(const std::string& style) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    style_ = style;
}

std::string BrainVisualizer::getStyle() const {
    return style_.load();
}

void BrainVisualizer::setColorScheme(const std::string& scheme) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    colorScheme_ = scheme;
}

std::string BrainVisualizer::getColorScheme() const {
    return colorScheme_.load();
}

void BrainVisualizer::setColor(NeuronType type, const std::string& color) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    // Set color for specific neuron type
    NLM_LOG_INFO("Set color for neuron type " + std::to_string(static_cast<int>(type)) + " to " + color);
}

std::string BrainVisualizer::getColor(NeuronType type) const {
    // Return color for specific neuron type
    return "default_color";
}

void BrainVisualizer::setColor(RegionId regionId, const std::string& color) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    // Set color for specific region
    NLM_LOG_INFO("Set color for region " + std::to_string(regionId) + " to " + color);
}

std::string BrainVisualizer::getColor(RegionId regionId) const {
    // Return color for specific region
    return "default_color";
}

void BrainVisualizer::setLayoutParameters(const LayoutParameters& params) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    layoutParams_ = params;
}

LayoutParameters BrainVisualizer::getLayoutParameters() const {
    return layoutParams_;
}

void BrainVisualizer::autoLayout() {
    NLM_LOG_INFO("Auto layout triggered");
    // Implement auto layout algorithm
}

void BrainVisualizer::layoutByRegion() {
    NLM_LOG_INFO("Layout by region triggered");
    // Implement region-based layout
}

void BrainVisualizer::layoutByDensity() {
    NLM_LOG_INFO("Layout by density triggered");
    // Implement density-based layout
}

void BrainVisualizer::layoutByActivity() {
    NLM_LOG_INFO("Layout by activity triggered");
    // Implement activity-based layout
}

void BrainVisualizer::enableInteraction(bool enable) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    interactionEnabled_ = enable;
}

bool BrainVisualizer::isInteractionEnabled() const {
    return interactionEnabled_.load();
}

void BrainVisualizer::setInteractionCallback(std::function<void(const std::string& action, const std::string& target)> callback) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    interactionCallback_ = std::move(callback);
}

void BrainVisualizer::highlightNeuron(NeuronId neuronId) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    NLM_LOG_INFO("Highlighted neuron: " + std::to_string(neuronId));
    // Implement neuron highlighting
}

void BrainVisualizer::highlightSynapse(SynapseId synapseId) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    NLM_LOG_INFO("Highlighted synapse: " + std::to_string(synapseId));
    // Implement synapse highlighting
}

void BrainVisualizer::highlightRegion(RegionId regionId) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    NLM_LOG_INFO("Highlighted region: " + std::to_string(regionId));
    // Implement region highlighting
}

void BrainVisualizer::clearHighlights() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    NLM_LOG_INFO("Cleared all highlights");
    // Clear all highlights
}

void BrainVisualizer::syncWithSimulation(std::shared_ptr<BrainSimulator> simulator) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    simulator_ = simulator;
    NLM_LOG_INFO("Visualization synced with simulation");
}

void BrainVisualizer::setSimulationSpeed(double speed) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    simulationSpeed_ = speed;
    NLM_LOG_INFO("Set simulation speed to: " + std::to_string(speed));
}

void BrainVisualizer::stepSimulation() {
    if (simulator_) {
        simulator_->step();
    }
}

void BrainVisualizer::pauseSimulation() {
    if (simulator_) {
        simulator_->pause();
    }
}

void BrainVisualizer::resumeSimulation() {
    if (simulator_) {
        simulator_->resume();
    }
}

void BrainVisualizer::enableDebugMode(bool enable) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    debugModeEnabled_ = enable;
    NLM_LOG_INFO("Debug mode " + std::string(enable ? "enabled" : "disabled"));
}

bool BrainVisualizer::isDebugModeEnabled() const {
    return debugModeEnabled_.load();
}

void BrainVisualizer::showDebugInfo() {
    showDebugInfo("all");
}

void BrainVisualizer::showDebugInfo(const std::string& category) {
    NLM_LOG_INFO("=== Debug Information ===");
    NLM_LOG_INFO("Category: " + category);
    NLM_LOG_INFO("Active Neurons: " + std::to_string(brain_ ? brain_->getActiveNeuronCount() : 0));
    NLM_LOG_INFO("Total Neurons: " + std::to_string(brain_ ? brain_->getTotalNeuronCount() : 0));
    NLM_LOG_INFO("Total Synapses: " + std::to_string(brain_ ? brain_->getTotalSynapseCount() : 0));
    NLM_LOG_INFO("Visualization Stats - FPS: " + std::to_string(getVisualizationStats().fps));
    NLM_LOG_INFO("Visualization Stats - Neurons Rendered: " + std::to_string(getVisualizationStats().neuronsRendered));
    NLM_LOG_INFO("Visualization Stats - Dropped Frames: " + std::to_string(getVisualizationStats().droppedFrames));
}

void BrainVisualizer::logVisualizationEvent(const std::string& event, const std::string& details) {
    logVisualizationEventInternal(event, details);
}

void BrainVisualizer::startDataCollection(const std::string& dataFile) {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (isCollectingData_) {
        NLM_LOG_WARNING("Data collection already started");
        return;
    }
    
    dataCollectionFile_ = dataFile;
    dataFile_.open(dataFile, std::ios::out | std::ios::trunc);
    
    if (!dataFile_.is_open()) {
        NLM_LOG_ERROR("Failed to open data file: " + dataFile);
        return;
    }
    
    // Write header
    dataFile_ << "Timestamp,Step,NeuronsRendered,SynapsesRendered,RegionsRendered,FPS,MemoryUsage,CPUUsage" << std::endl;
    
    isCollectingData_ = true;
    NLM_LOG_INFO("Data collection started: " + dataFile);
}

void BrainVisualizer::stopDataCollection() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!isCollectingData_) {
        return;
    }
    
    isCollectingData_ = false;
    
    if (dataFile_.is_open()) {
        dataFile_.close();
    }
    
    NLM_LOG_INFO("Data collection stopped");
}

void BrainVisualizer::collectVisualizationData() {
    std::lock_guard<std::mutex> lock(visualizationMutex_);
    
    if (!isCollectingData_ || !dataFile_.is_open()) {
        return;
    }
    
    auto now = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    dataFile_ << std::fixed << std::setprecision(3)
              << now << ","
              << (simulator_ ? simulator_->getCurrentStep() : 0) << ","
              << visualizationStats_.neuronsRendered << ","
              << visualizationStats_.synapsesRendered << ","
              << visualizationStats_.regionsRendered << ","
              << visualizationStats_.fps << ","
              << visualizationStats_.memoryUsage << ","
              << visualizationStats_.cpuUsage << std::endl;
}

bool BrainVisualizer::isCollectingData() const {
    return isCollectingData_.load();
}

void BrainVisualizer::updateVisualization() {
    auto lastTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    while (monitoringRequested_ && isRunning_) {
        if (isPaused_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        auto startTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        
        std::lock_guard<std::mutex> lock(visualizationMutex_);
        
        // Update visualization state
        updateVisualizationState();
        
        // Render frame
        renderFrame();
        
        // Update performance statistics
        updatePerformanceStats();
        
        // Collect data if enabled
        if (isCollectingData_) {
            collectVisualizationData();
        }
        
        // Calculate frame time and FPS
        auto endTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        double frameTime = endTime - startTime;
        
        lastFrameTime_ = frameTime;
        frameCount_++;
        
        // Check if we need to sleep to maintain target frame rate
        unsigned int targetIntervalMs = getUpdateInterval();
        if (targetIntervalMs > 0) {
            double targetIntervalSeconds = targetIntervalMs / 1000.0;
            double sleepTime = targetIntervalSeconds - frameTime;
            
            if (sleepTime > 0.0) {
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
            }
        }
    }
}

void BrainVisualizer::updateVisualizationState() {
    if (!brain_) return;
    
    // Update visualization state based on brain state
    // This includes updating neuron positions, synapse connections, etc.
    
    // Example: Update neuron firing states
    for (const auto& region : brain_->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (Neuron* neuron : population->getNeurons()) {
                if (neuron) {
                    // Update neuron visualization state
                    // This would require neuron state update API
                }
            }
        }
    }
}

void BrainVisualizer::renderFrame() {
    if (!isActive_ || !brain_) return;
    
    try {
        // Perform rendering operations
        // This would involve calling renderer APIs
        
        // Update renderer statistics
        visualizationStats_.totalFrames++;
        
        if (visualizationStats_.totalFrames % 100 == 0) {
            // Log every 100 frames to avoid spam
            NLM_LOG_DEBUG("Rendered frame " + std::to_string(visualizationStats_.totalFrames));
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during frame rendering: " + std::string(e.what()));
        visualizationStats_.droppedFrames++;
    }
}

void BrainVisualizer::updatePerformanceStats() {
    double now = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    // Update FPS
    if (lastFrameTime_ > 0.0) {
        visualizationStats_.fps = 1.0 / lastFrameTime_;
        visualizationStats_.frameTime = lastFrameTime_ * 1000.0; // Convert to ms
    }
    
    // Update other performance metrics
    // These would be updated by renderer components
    visualizationStats_.memoryUsage = 50.0; // Placeholder
    visualizationStats_.cpuUsage = 10.0; // Placeholder
    
    // Calculate average FPS over last 100 frames
    if (frameCount_ > 0) {
        visualizationStats_.fps = visualizationStats_.fps; // Current FPS
    }
}

void BrainVisualizer::generateVisualizationReport() const {
    std::stringstream report;
    
    report << "Brain Visualization Report" << std::endl;
    report << "==========================" << std::endl;
    report << std::endl;
    
    report << "Configuration:" << std::endl;
    report << "  Display Mode: " << displayMode_ << std::endl;
    report << "  Layout: " << layout_ << std::endl;
    report << "  Style: " << style_ << std::endl;
    report << "  Color Scheme: " << colorScheme_ << std::endl;
    report << "  Update Interval: " << updateIntervalMs_ << "ms" << std::endl;
    report << "  Interaction Enabled: " << (interactionEnabled_ ? "Yes" : "No") << std::endl;
    report << "  Debug Mode: " << (debugModeEnabled_ ? "Yes" : "No") << std::endl;
    report << std::endl;
    
    report << "Performance Metrics:" << std::endl;
    report << "  FPS: " << visualizationStats_.fps << std::endl;
    report << "  Frame Time: " << visualizationStats_.frameTime << "ms" << std::endl;
    report << "  Neurons Rendered: " << visualizationStats_.neuronsRendered << std::endl;
    report << "  Synapses Rendered: " << visualizationStats_.synapsesRendered << std::endl;
    report << "  Regions Rendered: " << visualizationStats_.regionsRendered << std::endl;
    report << "  Memory Systems Active: " << visualizationStats_.memorySystemsActive << std::endl;
    report << "  Total Frames: " << visualizationStats_.totalFrames << std::endl;
    report << "  Dropped Frames: " << visualizationStats_.droppedFrames << std::endl;
    report << "  Memory Usage: " << visualizationStats_.memoryUsage << "%" << std::endl;
    report << "  CPU Usage: " << visualizationStats_.cpuUsage << "%" << std::endl;
    
    report << std::endl << "Brain State:" << std::endl;
    if (brain_) {
        report << "  Total Neurons: " << brain_->getTotalNeuronCount() << std::endl;
        report << "  Total Synapses: " << brain_->getTotalSynapseCount() << std::endl;
        report << "  Active Neurons: " << brain_->getActiveNeuronCount() << std::endl;
        report << "  Firing Neurons: " << brain_->getFiringNeuronCount() << std::endl;
        report << "  E/I Ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
    }
    
    NLM_LOG_INFO("Visualization Report Generated");
    // Note: Report would be written to file in real implementation
}

void BrainVisualizer::exportVisualizationDataInternal() const {
    // Generate visualization report
    generateVisualizationReport();
    
    // Export visualization data
    // This would use exportRenderer_ if available
    if (exportRenderer_) {
        exportRenderer_->exportData("visualization_report.txt", "text");
    }
}

void BrainVisualizer::logVisualizationEventInternal(const std::string& event, const std::string& details) {
    NLM_LOG_INFO("Visualization Event: " + event + (details.empty() ? "" : " - " + details));
}

void BrainVisualizer::setupColorScheme() {
    if (colorScheme_ == "default") {
        // Set default color scheme
        NLM_LOG_INFO("Setup default color scheme");
    } else if (colorScheme_ == "scientific") {
        // Set scientific color scheme
        NLM_LOG_INFO("Setup scientific color scheme");
    } else if (colorScheme_ == "vivid") {
        // Set vivid color scheme
        NLM_LOG_INFO("Setup vivid color scheme");
    }
}

void BrainVisualizer::setupLayout() {
    if (layout_ == "default") {
        // Set default layout
        NLM_LOG_INFO("Setup default layout");
    } else if (layout_ == "circular") {
        // Set circular layout
        NLM_LOG_INFO("Setup circular layout");
    } else if (layout_ == "grid") {
        // Set grid layout
        NLM_LOG_INFO("Setup grid layout");
    } else if (layout_ == "hierarchical") {
        // Set hierarchical layout
        NLM_LOG_INFO("Setup hierarchical layout");
    }
}

void BrainVisualizer::setupRenderers() {
    // Configure renderers based on current configuration
    if (brainStructureRenderer_) {
        brainStructureRenderer_->setColorScheme(colorScheme_);
        brainStructureRenderer_->setLayout(layout_);
        brainStructureRenderer_->setStyle(style_);
    }
    
    if (activityRenderer_) {
        activityRenderer_->setColorScheme(colorScheme_);
    }
    
    if (memoryRenderer_) {
        memoryRenderer_->setColorScheme(colorScheme_);
    }
    
    if (progressRenderer_) {
        progressRenderer_->setColorScheme(colorScheme_);
    }
}

bool BrainVisualizer::validateVisualizationSetup() const {
    if (!brain_) {
        NLM_LOG_ERROR("No brain reference for visualization setup");
        return false;
    }
    
    if (!visualizationInterface_) {
        NLM_LOG_ERROR("No visualization interface for visualization setup");
        return false;
    }
    
    // Check if brain has required components
    if (brain_->getTotalNeuronCount() == 0) {
        NLM_LOG_WARNING("Brain has no neurons for visualization");
    }
    
    if (brain_->getTotalSynapseCount() == 0) {
        NLM_LOG_WARNING("Brain has no synapses for visualization");
    }
    
    if (brain_->getRegionCount() == 0) {
        NLM_LOG_WARNING("Brain has no regions for visualization");
    }
    
    NLM_LOG_INFO("Visualization setup validated successfully");
    
    return true;
}

double BrainVisualizer::calculateFPS() const {
    if (lastFrameTime_ > 0.0) {
        return 1.0 / lastFrameTime_;
    }
    return 0.0;
}

void BrainVisualizer::handleInteraction(const std::string& action, const std::string& target) {
    if (interactionCallback_) {
        interactionCallback_(action, target);
    }
}

std::vector<NeuronId> BrainVisualizer::getHighlightedNeurons() const {
    return std::vector<NeuronId>(); // Placeholder
}

std::vector<SynapseId> BrainVisualizer::getHighlightedSynapses() const {
    return std::vector<SynapseId>(); // Placeholder
}

std::vector<RegionId> BrainVisualizer::getHighlightedRegions() const {
    return std::vector<RegionId>(); // Placeholder
}

void BrainVisualizer::clearHighlightsInternal() {
    // Clear all highlights
}

BrainVisualizer::VisualizationStats BrainVisualizer::getVisualizationStats() const {
    return visualizationStats_;
}

void BrainVisualizer::lockVisualization() const {
    visualizationMutex_.lock();
}

void BrainVisualizer::unlockVisualization() const {
    visualizationMutex_.unlock();
}

bool BrainVisualizer::tryLockVisualization() const {
    return visualizationMutex_.try_lock();
}

} // namespace nlm