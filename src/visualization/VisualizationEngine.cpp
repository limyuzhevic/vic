#include "VisualizationEngine.hpp"
#include "../brain/Brain.hpp"
#include "../performance/PerformanceStats.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <random>

namespace nlm {

struct VisualizationEngine::Impl {
    std::vector<VisualizationData> frameHistory;
    size_t maxFrames = 1000;
    std::mutex dataMutex;
    std::string lastRenderedFile;
    bool initialized = false;
    std::chrono::high_resolution_clock::time_point lastFrameTime;
};

VisualizationEngine::VisualizationEngine() : running_(false) {
    pImpl = std::make_unique<Impl>();
}

VisualizationEngine::~VisualizationEngine() {
    stop();
}

bool VisualizationEngine::initialize(const VisualizationConfig& config) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    config_ = config;
    pImpl->initialized = true;
    
    if (config_.enableExportEnabled && config_.exportDirectory.empty()) {
        config_.exportDirectory = "./exports";
    }
    
    NLM_LOG_INFO("VisualizationEngine initialized");
    return true;
}

void VisualizationEngine::start() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    if (!running_) {
        running_ = true;
        lastUpdateTime_ = std::chrono::high_resolution_clock::now();
        NLM_LOG_INFO("VisualizationEngine started");
    }
}

void VisualizationEngine::stop() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    if (running_) {
        running_ = false;
        NLM_LOG_INFO("VisualizationEngine stopped");
    }
}

void VisualizationEngine::configure(const VisualizationConfig& config) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    config_ = config;
    NLM_LOG_INFO("VisualizationEngine reconfigured");
}

void VisualizationEngine::update() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    update(std::nullopt, std::nullopt, std::nullopt);
}

void VisualizationEngine::update(const Brain& brain, Timestamp currentTime, SimulationStep currentStep) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime_);
    
    if (!running_ || elapsed < config_.updateInterval) {
        return;
    }
    
    lastUpdateTime_ = now;
    updateData(brain, currentTime, currentStep);
}

void VisualizationEngine::updateData(const Brain& brain, Timestamp currentTime, SimulationStep currentStep) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    currentData_.fromBrain(brain, currentTime, currentStep);
    
    // Store frame in history if tracking is enabled
    pImpl->frameHistory.push_back(currentData_);
    
    // Keep only recent frames
    if (pImpl->frameHistory.size() > pImpl->maxFrames) {
        pImpl->frameHistory.erase(pImpl->frameHistory.begin(),
                                pImpl->frameHistory.begin() + (pImpl->frameHistory.size() - pImpl->maxFrames));
    }
}

void VisualizationEngine::render() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    render(RenderTarget::WINDOW, "");
}

void VisualizationEngine::render(RenderTarget target, const std::string& filename) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    if (!running_) {
        return;
    }
    
    switch (target) {
        case RenderTarget::CONSOLE:
            renderConsole();
            break;
        case RenderTarget::WINDOW:
            renderWindow();
            break;
        case RenderTarget::FILE:
            renderFile(filename);
            break;
        case RenderTarget::WEB:
            renderWeb(filename);
            break;
    }
}

void VisualizationEngine::visualizeNetwork(const Brain& brain) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    // Placeholder for network visualization
    NLM_LOG_INFO("Network visualization requested for brain with " + 
                std::to_string(brain.getTotalNeuronCount()) + " neurons");
}

void VisualizationEngine::visualizeActivity(const Brain& brain) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    // Placeholder for activity visualization
    NLM_LOG_INFO("Activity visualization requested for brain with " + 
                std::to_string(brain.getFiringNeuronCount()) + " firing neurons");
}

void VisualizationEngine::visualizeSpikes(const std::vector<DetailedSpikeEvent>& spikes) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    // Placeholder for spike visualization
    if (!spikes.empty()) {
        NLM_LOG_INFO("Spike raster visualization requested with " + 
                    std::to_string(spikes.size()) + " spikes");
    }
}

void VisualizationEngine::visualizeWeights(const Brain& brain) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    // Placeholder for weight matrix visualization
    NLM_LOG_INFO("Weight matrix visualization requested for brain with " + 
                std::to_string(brain.getTotalSynapseCount()) + " synapses");
}

void VisualizationEngine::visualizeConnectivity(const Brain& brain) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    // Placeholder for connectivity map visualization
    NLM_LOG_INFO("Connectivity map visualization requested");
}

void VisualizationEngine::visualizeHeatmaps(const Brain& brain) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    // Placeholder for activity heatmap visualization
    NLM_LOG_INFO("Activity heatmap visualization requested");
}

bool VisualizationEngine::exportVisualization(const std::string& filename, ExportFormat format) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    std::string extension;
    switch (format) {
        case ExportFormat::PNG: extension = ".png"; break;
        case ExportFormat::JPG: extension = ".jpg"; break;
        case ExportFormat::SVG: extension = ".svg"; break;
        case ExportFormat::PDF: extension = ".pdf"; break;
        case ExportFormat::HTML: extension = ".html"; break;
        case ExportFormat::JSON: extension = ".json"; break;
        case ExportFormat::CSV: extension = ".csv"; break;
        case ExportFormat::TEXT: extension = ".txt"; break;
    }
    
    std::string fullFilename = filename;
    if (fullFilename.find(extension) == std::string::npos) {
        fullFilename += extension;
    }
    
    switch (format) {
        case ExportFormat::PNG:
        case ExportFormat::JPG:
            return exportToPNG(fullFilename);
        case ExportFormat::SVG:
            return exportToSVG(fullFilename);
        case ExportFormat::PDF:
            return exportToPDF(fullFilename);
        case ExportFormat::HTML:
            return exportToHTML(fullFilename);
        case ExportFormat::JSON:
            return exportToJSON(fullFilename);
        case ExportFormat::CSV:
            return exportToCSV(fullFilename);
        case ExportFormat::TEXT:
            return exportToText(fullFilename);
    }
    
    NLM_LOG_ERROR("Unsupported export format");
    return false;
}

bool VisualizationEngine::exportFrame(const std::string& filename, ExportFormat format) {
    return exportVisualization(filename, format);
}

void VisualizationEngine::setUpdateInterval(const std::chrono::milliseconds& interval) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    config_.updateInterval = interval;
    NLM_LOG_INFO("Visualization update interval set to " + std::to_string(interval.count()) + " ms");
}

void VisualizationEngine::renderConsole() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    std::string consoleOutput = formatConsoleOutput(currentData_);
    
    // Output to console
    std::cout << "=== Visualization Engine - Console Output ===\n";
    std::cout << consoleOutput << "\n";
    std::cout << "========================================\n";
}

void VisualizationEngine::renderWindow() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    // Placeholder for window rendering
    // In a real implementation, this would use a graphics library like OpenGL, Vulkan, or similar
    NLM_LOG_INFO("Window rendering requested (placeholder implementation)");
    
    if (config_.enableDebugVisualization) {
        NLM_LOG_DEBUG("Window dimensions: " + std::to_string(config_.windowWidth) + " x " + 
                     std::to_string(config_.windowHeight));
        NLM_LOG_DEBUG("Backend: " + config_.backend);
    }
}

void VisualizationEngine::renderFile(const std::string& filename) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    if (filename.empty()) {
        NLM_LOG_ERROR("Filename required for file rendering");
        return;
    }
    
    std::string fileExtension = filename.substr(filename.find_last_of(".") + 1);
    
    // Convert extension to ExportFormat enum
    ExportFormat format;
    if (fileExtension == "png" || fileExtension == "PNG") {
        format = ExportFormat::PNG;
    } else if (fileExtension == "jpg" || fileExtension == "JPG" || fileExtension == "jpeg") {
        format = ExportFormat::JPG;
    } else if (fileExtension == "svg") {
        format = ExportFormat::SVG;
    } else if (fileExtension == "pdf") {
        format = ExportFormat::PDF;
    } else if (fileExtension == "html") {
        format = ExportFormat::HTML;
    } else if (fileExtension == "json") {
        format = ExportFormat::JSON;
    } else if (fileExtension == "csv") {
        format = ExportFormat::CSV;
    } else {
        format = ExportFormat::TEXT;
    }
    
    if (exportVisualization(filename, format)) {
        pImpl->lastRenderedFile = filename;
        NLM_LOG_INFO("Visualization exported to file: " + filename);
    }
}

void VisualizationEngine::renderWeb(const std::string& filename) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    NLM_LOG_INFO("Web rendering requested (placeholder implementation)");
    
    // Placeholder for web rendering
    // In a real implementation, this would create an HTML file with interactive visualization
}

void VisualizationEngine::renderNetworkGraph(const Brain& brain) {
    if (config_.enableNetworkVisualization) {
        NLM_LOG_INFO("Rendering network graph with " + std::to_string(brain.getTotalNeuronCount()) + 
                    " neurons and " + std::to_string(brain.getTotalSynapseCount()) + " synapses");
    }
}

void VisualizationEngine::renderActivityHeatmap(const Brain& brain) {
    if (config_.enableActivityHeatmaps) {
        NLM_LOG_INFO("Rendering activity heatmap for " + std::to_string(brain.getFiringNeuronCount()) + 
                    " firing neurons");
    }
}

void VisualizationEngine::renderSpikeRaster(const std::vector<DetailedSpikeEvent>& spikes) {
    if (config_.enableSpikeRaster && !spikes.empty()) {
        NLM_LOG_INFO("Rendering spike raster for " + std::to_string(spikes.size()) + " spikes");
    }
}

void VisualizationEngine::renderWeightMatrix(const Brain& brain) {
    if (config_.enableWeightMatrix) {
        NLM_LOG_INFO("Rendering weight matrix for " + std::to_string(brain.getTotalSynapseCount()) + 
                    " synapses");
    }
}

void VisualizationEngine::renderConnectivityMap(const Brain& brain) {
    if (config_.enableConnectivityMaps) {
        NLM_LOG_INFO("Rendering connectivity map");
    }
}

void VisualizationEngine::renderDebugInfo(const Brain& brain) {
    if (config_.enableDebugVisualization) {
        NLM_LOG_DEBUG("Debug mode active - " + debugMode_);
        NLM_LOG_DEBUG("Neuron count: " + std::to_string(brain.getTotalNeuronCount()));
        NLM_LOG_DEBUG("Synapse count: " + std::to_string(brain.getTotalSynapseCount()));
        NLM_LOG_DEBUG("Active neurons: " + std::to_string(brain.getActiveNeuronCount()));
        NLM_LOG_DEBUG("Firing neurons: " + std::to_string(brain.getFiringNeuronCount()));
    }
}

bool VisualizationEngine::exportToPNG(const std::string& filename) {
    NLM_LOG_INFO("PNG export requested (placeholder implementation)");
    // In a real implementation, this would use a graphics library to create PNG files
    return true;
}

bool VisualizationEngine::exportToJPG(const std::string& filename) {
    NLM_LOG_INFO("JPG export requested (placeholder implementation)");
    // In a real implementation, this would use a graphics library to create JPG files
    return true;
}

bool VisualizationEngine::exportToSVG(const std::string& filename) {
    NLM_LOG_INFO("SVG export requested (placeholder implementation)");
    // In a real implementation, this would generate SVG files
    return true;
}

bool VisualizationEngine::exportToPDF(const std::string& filename) {
    NLM_LOG_INFO("PDF export requested (placeholder implementation)");
    // In a real implementation, this would generate PDF files
    return true;
}

bool VisualizationEngine::exportToHTML(const std::string& filename) {
    NLM_LOG_INFO("HTML export requested (placeholder implementation)");
    // In a real implementation, this would create an HTML file with interactive visualization
    return true;
}

bool VisualizationEngine::exportToJSON(const std::string& filename) {
    NLM_LOG_INFO("JSON export requested (placeholder implementation)");
    // In a real implementation, this would serialize data to JSON
    return true;
}

bool VisualizationEngine::exportToCSV(const std::string& filename) {
    NLM_LOG_INFO("CSV export requested (placeholder implementation)");
    // In a real implementation, this would write CSV files
    return true;
}

bool VisualizationEngine::exportToText(const std::string& filename) {
    NLM_LOG_INFO("Text export requested (placeholder implementation)");
    // In a real implementation, this would write text files
    return true;
}

std::string VisualizationEngine::formatConsoleOutput(const VisualizationData& data) const {
    std::ostringstream oss;
    oss << formatNetworkInfo(data.neuralData.neuronActivations.size() > 0 ? static_cast<const Brain&>(*reinterpret_cast<const void*>(&data)) : std::nullopt);
    oss << "\n" << formatActivityInfo(data.neuralData.neuronActivations.size() > 0 ? static_cast<const Brain&>(*reinterpret_cast<const void*>(&data)) : std::nullopt);
    oss << "\n" << formatSpikeInfo(data.spikeEvents);
    oss << "\n" << formatPerformanceInfo(data.performanceMetrics);
    return oss.str();
}

std::string VisualizationEngine::formatNetworkInfo(const std::vector<double> activations) const {
    std::ostringstream oss;
    oss << "=== Network Information ===\n";
    oss << "Network is active with " << activations.size() << " neurons\n";
    oss << "========================================\n";
    return oss.str();
}

std::string VisualizationEngine::formatActivityInfo(const std::vector<double> activations) const {
    std::ostringstream oss;
    oss << "=== Activity Information ===\n";
    oss << "Total activations recorded: " << activations.size() << "\n";
    if (!activations.empty()) {
        double maxActivation = *std::max_element(activations.begin(), activations.end());
        double minActivation = *std::min_element(activations.begin(), activations.end());
        double sumActivation = std::accumulate(activations.begin(), activations.end(), 0.0);
        double avgActivation = sumActivation / activations.size();
        
        oss << "Max activation: " << std::fixed << std::setprecision(4) << maxActivation << "\n";
        oss << "Min activation: " << std::fixed << std::setprecision(4) << minActivation << "\n";
        oss << "Average activation: " << std::fixed << std::setprecision(4) << avgActivation << "\n";
    }
    oss << "========================================\n";
    return oss.str();
}

std::string VisualizationEngine::formatSpikeInfo(const std::vector<DetailedSpikeEvent>& spikes) const {
    std::ostringstream oss;
    oss << "=== Spike Information ===\n";
    oss << "Total spikes: " << spikes.size() << "\n";
    if (!spikes.empty()) {
        oss << "Time range: " << std::fixed << std::setprecision(4) << spikes.front().timestamp << " - " 
            << std::fixed << std::setprecision(4) << spikes.back().timestamp << " s\n";
    }
    oss << "========================================\n";
    return oss.str();
}

std::string VisualizationEngine::formatPerformanceInfo(const PerformanceMetrics& metrics) const {
    std::ostringstream oss;
    oss << "=== Performance Metrics ===\n";
    oss << "CPU Usage: " << std::fixed << std::setprecision(1) << metrics.cpuUsage << "%\n";
    oss << "Memory Usage: " << std::fixed << std::setprecision(1) << metrics.memoryUsageMB << " MB\n";
    oss << "Active Neurons: " << metrics.activeNeurons << "\n";
    oss << "Firing Neurons: " << metrics.firingNeurons << "\n";
    oss << "Total Spikes: " << metrics.totalSpikes << "\n";
    oss << "Average Firing Rate: " << std::fixed << std::setprecision(2) << metrics.avgFiringRate << " Hz\n";
    oss << "Simulation Speed: " << std::fixed << std::setprecision(2) << metrics.spikesPerSecond << " spikes/s\n";
    oss << "========================================\n";
    return oss.str();
}

std::string VisualizationEngine::getActivityColor(double activation) const {
    if (activation < 0.25) return "\033[34m"; // Blue - low
    if (activation < 0.5) return "\033[32m"; // Green - medium
    if (activation < 0.75) return "\033[33m"; // Yellow - high
    return "\033[31m"; // Red - very high
}

std::string VisualizationEngine::getSpikeColor(const DetailedSpikeEvent& spike) const {
    return "\033[31m"; // Red for spikes
}

std::string VisualizationEngine::getConnectionColor(float weight) const {
    if (weight > 0.5) return "\033[31m"; // Red - strong connection
    if (weight > 0.1) return "\033[33m"; // Yellow - moderate connection
    return "\033[90m"; // Gray - weak connection
}

void VisualizationEngine::updateGlobal(const Brain& brain, Timestamp currentTime, SimulationStep currentStep) {
    getInstance().update(brain, currentTime, currentStep);
}

void VisualizationEngine::renderGlobal(RenderTarget target, const std::string& filename) {
    getInstance().render(target, filename);
}

} // namespace nlm
