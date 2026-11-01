// NLM Brain State Visualization Interface
// Basic visualization for monitoring brain state and metrics
//
// This module provides a simple interface for visualizing brain state,
// including neural activity, memory states, and system metrics.
// It's designed to be lightweight and integrate easily with the
// existing NLM framework.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <functional>
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "prediction/PredictionSystem.hpp"

namespace nlm {

// Visualization output formats
enum class VisualizationFormat {
    TEXT,
    ASCII_ART,
    JSON,
    CSV,
    HTML,
    TERMINAL_DASHBOARD
};

// Visualization update frequency
enum class VisualizationUpdateFrequency {
    HIGH,      // Every simulation step (high detail, less performance)
    MEDIUM,    // Every 10 steps
    LOW,       // Every 100 steps
    CUSTOM    // User-defined interval
};

// Brain state snapshot for visualization
struct BrainStateSnapshot {
    SimulationStep step;
    Timestamp timestamp;
    double simulationTime;
    
    // Neural activity metrics
    size_t totalNeuronCount;
    size_t activeNeuronCount;
    size_t firingNeuronCount;
    double averageFiringRate;
    size_t totalSpikeCount;
    
    // Network metrics
    size_t totalSynapseCount;
    double averageSynapticWeight;
    double excitationInhibitionRatio;
    
    // Memory system states
    size_t workingMemoryTraces;
    size_t episodicMemoryCount;
    size_t conceptCount;
    
    // Neuromodulation states
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    
    // Cognitive states
    std::vector<std::string> activeConcepts;
    std::string planningAction;
    float selfModelConfidence;
    
    // Environment interaction
    std::string currentAction;
    float actionReward;
    
    // Performance metrics
    double stepExecutionTime;
    double memoryUsageMB;
    double cpuUsagePercent;
    
    BrainStateSnapshot()
        : step(0), timestamp(0.0), simulationTime(0.0),
          totalNeuronCount(0), activeNeuronCount(0), firingNeuronCount(0),
          averageFiringRate(0.0), totalSpikeCount(0), totalSynapseCount(0),
          averageSynapticWeight(0.0), excitationInhibitionRatio(0.0),
          workingMemoryTraces(0), episodicMemoryCount(0), conceptCount(0),
          dopamineLevel(0.0), curiosityLevel(0.0), noveltyLevel(0.0),
          predictionError(0.0), selfModelConfidence(0.0),
          actionReward(0.0), stepExecutionTime(0.0),
          memoryUsageMB(0.0), cpuUsagePercent(0.0) {}
};

// Visualization update callback
using VisualizationUpdateCallback = std::function<void(const BrainStateSnapshot&)>;

// Visualization export callback
using VisualizationExportCallback = std::function<void(const std::string& filename, const std::string& format)>;

// Main visualization interface
class BrainStateVisualizer {
public:
    BrainStateVisualizer();
    ~BrainStateVisualizer();
    
    // Setup visualization
    bool initialize(const std::string& configFile = "");
    
    // Start real-time visualization
    bool startVisualization(std::shared_ptr<Brain> brain,
                           VisualizationUpdateFrequency frequency = VisualizationUpdateFrequency::MEDIUM,
                           size_t updateInterval = 10);
    
    // Stop visualization
    void stopVisualization();
    
    // Update visualization with current brain state
    void updateVisualization(std::shared_ptr<Brain> brain);
    
    // Get current visualization format
    VisualizationFormat getFormat() const;
    
    // Set visualization format
    void setFormat(VisualizationFormat format);
    
    // Set output file
    void setOutputFile(const std::string& filename);
    
    // Set update frequency
    void setUpdateFrequency(VisualizationUpdateFrequency frequency);
    
    // Set custom update interval
    void setUpdateInterval(size_t interval);
    
    // Enable/disable real-time display
    void setRealTimeDisplay(bool enabled);
    
    // Enable/disable colors
    void setColorsEnabled(bool enabled);
    
    // Add visualization update callback
    void addUpdateCallback(VisualizationUpdateCallback callback);
    
    // Add visualization export callback
    void addExportCallback(VisualizationExportCallback callback);
    
    // Generate visualization report
    std::string generateReport(const std::string& format = "text") const;
    
    // Export visualization data
    bool exportVisualization(const std::string& filename, const std::string& format = "text");
    
    // Get current brain state snapshot
    BrainStateSnapshot getCurrentState(std::shared_ptr<Brain> brain) const;
    
    // Get visualization statistics
    std::string getStatistics() const;
    
    // Check if visualization is active
    bool isActive() const;
    
    // Check if visualization is running
    bool isRunning() const;
    
    // Reset visualization state
    void reset();
    
    // Cleanup visualization resources
    void cleanup();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Update thread function
    void visualizationUpdateThread(std::shared_ptr<Brain> brain);
    
    // Generate text visualization
    std::string generateTextVisualization(const BrainStateSnapshot& snapshot) const;
    
    // Generate ASCII art visualization
    std::string generateAsciiArtVisualization(const BrainStateSnapshot& snapshot) const;
    
    // Generate JSON visualization
    std::string generateJsonVisualization(const BrainStateSnapshot& snapshot) const;
    
    // Generate CSV visualization
    std::string generateCsvVisualization(const BrainStateSnapshot& snapshot) const;
    
    // Generate HTML visualization
    std::string generateHtmlVisualization(const BrainStateSnapshot& snapshot) const;
    
    // Generate terminal dashboard visualization
    std::string generateTerminalDashboardVisualization(const BrainStateSnapshot& snapshot) const;
}; // class BrainStateVisualizer

// Text visualization formatter
class TextVisualizerFormatter {
public:
    static std::string format(const BrainStateSnapshot& snapshot);
    static std::string formatHeader() const;
    static std::string formatSnapshot(const BrainStateSnapshot& snapshot);
    static std::string formatNeuralInfo(const BrainStateSnapshot& snapshot);
    static std::string formatMemoryInfo(const BrainStateSnapshot& snapshot);
    static std::string formatCognitiveInfo(const BrainStateSnapshot& snapshot);
    static std::string formatNeuromodulationInfo(const BrainStateSnapshot& snapshot);
    static std::string formatPerformanceInfo(const BrainStateSnapshot& snapshot);
    static std::string formatFooter() const;
    
private:
    static std::string formatProgressBar(double progress, int width = 50);
    static std::string getActivityLevel(size_t firingCount, size_t totalCount);
    static std::string getMemoryStatus(size_t traces, size_t capacity);
    static std::string getCognitiveState(const std::vector<std::string>& concepts);
    static std::string getNeuromodulationLevel(float value);
    static std::string getPerformanceLevel(double metric);
}; // class TextVisualizerFormatter

// ASCII art visualization formatter
class AsciiArtVisualizerFormatter {
public:
    static std::string format(const BrainStateSnapshot& snapshot);
    static std::string formatNeuralActivity(const BrainStateSnapshot& snapshot);
    static std::string formatMemoryUsage(const BrainStateSnapshot& snapshot);
    static std::string formatCognitiveState(const BrainStateSnapshot& snapshot);
    static std::string formatNeuromodulationState(const BrainStateSnapshot& snapshot);
    static std::string formatSystemStatus(const BrainStateSnapshot& snapshot);
    
private:
    static std::string createActivityBar(size_t active, size_t total, char fill = '█', char empty = '░');
    static std::string createMemoryBar(size_t used, size_t total, char fill = '▓', char empty = '░');
    static std::string createProgressBar(double progress, char fill = '█', char empty = '░');
}; // class AsciiArtVisualizerFormatter

// JSON visualization formatter
class JsonVisualizerFormatter {
public:
    static std::string format(const BrainStateSnapshot& snapshot);
    static std::string format(const std::vector<BrainStateSnapshot>& snapshots);
    static std::string formatConfiguration();
    static std::string formatHeader();
    
private:
    static std::string escapeJsonString(const std::string& input);
    static std::string formatValue(double value);
    static std::string formatValue(size_t value);
    static std::string formatValue(const std::string& value);
    static std::string formatValue(bool value);
    static std::string formatValue(const std::vector<std::string>& values);
}; // class JsonVisualizerFormatter

// CSV visualization formatter
class CsvVisualizerFormatter {
public:
    static std::string format(const BrainStateSnapshot& snapshot);
    static std::string format(const std::vector<BrainStateSnapshot>& snapshots);
    static std::string formatHeader();
    static std::string formatHeader(const std::vector<std::string>& columns);
    
private:
    static std::string escapeCsvString(const std::string& input);
}; // class CsvVisualizerFormatter

// HTML visualization formatter
class HtmlVisualizerFormatter {
public:
    static std::string format(const BrainStateSnapshot& snapshot);
    static std::string format(const std::vector<BrainStateSnapshot>& snapshots);
    static std::string formatHeader();
    static std::string formatFooter();
    static std::string formatStyle();
    static std::string formatScript();
    
private:
    static std::string escapeHtmlString(const std::string& input);
    static std::string generateChartData(const std::vector<BrainStateSnapshot>& snapshots);
}; // class HtmlVisualizerFormatter

// Terminal dashboard formatter
class TerminalDashboardFormatter {
public:
    static std::string format(const BrainStateSnapshot& snapshot);
    static std::string formatHeader(const std::string& title = "NLM Brain State Monitor");
    static std::string formatFooter();
    static std::string formatSection(const std::string& title, const std::string& content);
    static std::string formatMetric(const std::string& label, double value, const std::string& unit = "");
    static std::string formatStatusIndicator(bool status, const std::string& text = "");
    
private:
    static std::string getColorCode(bool status);
}; // class TerminalDashboardFormatter

// Global visualization manager
BrainStateVisualizer& getBrainStateVisualizer();

// Convenience functions
bool startBrainStateVisualization(std::shared_ptr<Brain> brain,
                                 VisualizationUpdateFrequency frequency = VisualizationUpdateFrequency::MEDIUM);
bool updateBrainStateVisualization(std::shared_ptr<Brain> brain);
void stopBrainStateVisualization();
bool exportBrainStateVisualization(const std::string& filename, const std::string& format = "text");
std::string generateBrainStateReport(std::shared_ptr<Brain> brain, const std::string& format = "text");

// Visualization utilities
namespace viz {
    
    // Generate color codes for terminal output
    std::string getColorCode(int colorCode);
    
    // Generate progress indicators
    std::string generateProgressBar(double progress, int width = 50);
    
    // Format numerical values with units
    std::string formatNumber(size_t value);
    std::string formatNumber(double value, int precision = 2);
    std::string formatPercentage(double value, int precision = 1);
    
    // Format time values
    std::string formatTime(double seconds);
    std::string formatTime(size_t steps, double timestep = 0.001);
    
    // Get status indicators
    std::string getStatusIndicator(bool status, const std::string& good = "●", const std::string& bad = "○");
    std::string getActivityLevel(size_t firingCount, size_t totalCount);
    std::string getMemoryStatus(size_t traces, size_t capacity);
    std::string getCognitiveState(const std::vector<std::string>& concepts);
    
    // Generate visualization configuration
    std::string generateVisualizationConfig(const std::string& filename = "viz_config.txt");
    
} // namespace viz

} // namespace nlm

// Implementation of BrainStateVisualizer
struct BrainStateVisualizer::Impl {
    std::shared_ptr<Brain> currentBrain;
    VisualizationFormat format;
    VisualizationUpdateFrequency updateFrequency;
    size_t updateInterval;
    bool realTimeEnabled;
    bool colorsEnabled;
    std::string outputFile;
    bool isActive;
    bool isRunning;
    std::vector<VisualizationUpdateCallback> updateCallbacks;
    std::vector<VisualizationExportCallback> exportCallbacks;
    std::chrono::steady_clock::time_point startTime;
    size_t updateCounter;
    std::vector<BrainStateSnapshot> stateHistory;
    size_t maxHistorySize;
    bool continuousCapture;
    std::chrono::steady_clock::time_point lastUpdateTime;
    double updateIntervalSeconds;
    std::thread updateThread;
    std::mutex updateMutex;
    bool threadShouldStop;
};

BrainStateVisualizer::BrainStateVisualizer() : pImpl(std::make_unique<Impl>()) {
    pImpl->format = VisualizationFormat::TERMINAL_DASHBOARD;
    pImpl->updateFrequency = VisualizationUpdateFrequency::MEDIUM;
    pImpl->updateInterval = 10;
    pImpl->realTimeEnabled = true;
    pImpl->colorsEnabled = true;
    pImpl->outputFile = "";
    pImpl->isActive = false;
    pImpl->isRunning = false;
    pImpl->updateCounter = 0;
    pImpl->maxHistorySize = 1000;
    pImpl->continuousCapture = false;
    pImpl->updateIntervalSeconds = 0.0;
    pImpl->threadShouldStop = false;
}

BrainStateVisualizer::~BrainStateVisualizer() {
    cleanup();
}

bool BrainStateVisualizer::initialize(const std::string& configFile) {
    try {
        // TODO: Load configuration from file if provided
        pImpl->isActive = true;
        pImpl->startTime = std::chrono::steady_clock::now();
        pImpl->lastUpdateTime = pImpl->startTime;
        
        NLM_LOG_INFO("Brain state visualization initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize brain state visualization: " + std::string(e.what()));
        return false;
    }
}

bool BrainStateVisualizer::startVisualization(std::shared_ptr<Brain> brain,
                                            VisualizationUpdateFrequency frequency,
                                            size_t updateInterval) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot start visualization: brain is null");
        return false;
    }
    
    if (pImpl->isRunning) {
        NLM_LOG_WARNING("Visualization already running");
        return true;
    }
    
    try {
        pImpl->currentBrain = brain;
        pImpl->updateFrequency = frequency;
        pImpl->updateInterval = updateInterval;
        pImpl->isRunning = true;
        pImpl->updateCounter = 0;
        pImpl->threadShouldStop = false;
        
        // Calculate update interval in seconds based on frequency
        switch (frequency) {
            case VisualizationUpdateFrequency::HIGH:
                pImpl->updateIntervalSeconds = 0.001; // Every step
                break;
            case VisualizationUpdateFrequency::MEDIUM:
                pImpl->updateIntervalSeconds = updateInterval * 0.001; // Every N steps
                break;
            case VisualizationUpdateFrequency::LOW:
                pImpl->updateIntervalSeconds = updateInterval * 0.01; // Every N seconds
                break;
            case VisualizationUpdateFrequency::CUSTOM:
                pImpl->updateIntervalSeconds = updateInterval;
                break;
        }
        
        // Start update thread
        pImpl->updateThread = std::thread(&BrainStateVisualizer::visualizationUpdateThread, this, brain);
        
        NLM_LOG_INFO("Brain state visualization started with " + std::to_string(updateInterval) +
                    " step interval and " + std::to_string(pImpl->updateIntervalSeconds) + "s refresh rate");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to start visualization: " + std::string(e.what()));
        pImpl->isRunning = false;
        return false;
    }
}

void BrainStateVisualizer::stopVisualization() {
    if (pImpl->isRunning) {
        pImpl->threadShouldStop = true;
        
        if (pImpl->updateThread.joinable()) {
            pImpl->updateThread.join();
        }
        
        pImpl->isRunning = false;
        pImpl->currentBrain.reset();
        
        NLM_LOG_INFO("Brain state visualization stopped");
    }
}

void BrainStateVisualizer::updateVisualization(std::shared_ptr<Brain> brain) {
    if (!brain || !pImpl->isRunning) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->updateMutex);
    
    // Update current brain reference
    pImpl->currentBrain = brain;
    
    // Get current state snapshot
    BrainStateSnapshot snapshot = getCurrentState(brain);
    
    // Add to history
    pImpl->stateHistory.push_back(snapshot);
    if (pImpl->stateHistory.size() > pImpl->maxHistorySize) {
        pImpl->stateHistory.erase(pImpl->stateHistory.begin());
    }
    
    // Update callbacks
    for (const auto& callback : pImpl->updateCallbacks) {
        try {
            callback(snapshot);
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Visualization callback failed: " + std::string(e.what()));
        }
    }
    
    // Export if continuous capture is enabled
    if (pImpl->continuousCapture && !pImpl->outputFile.empty()) {
        try {
            exportVisualization(pImpl->outputFile, getFormatString(pImpl->format));
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Visualization export failed: " + std::string(e.what()));
        }
    }
}

VisualizationFormat BrainStateVisualizer::getFormat() const {
    return pImpl->format;
}

void BrainStateVisualizer::setFormat(VisualizationFormat format) {
    pImpl->format = format;
}

void BrainStateVisualizer::setOutputFile(const std::string& filename) {
    pImpl->outputFile = filename;
    
    if (!filename.empty() && pImpl->continuousCapture) {
        NLM_LOG_INFO("Visualization output file set to: " + filename);
    }
}

void BrainStateVisualizer::setUpdateFrequency(VisualizationUpdateFrequency frequency) {
    pImpl->updateFrequency = frequency;
}

void BrainStateVisualizer::setUpdateInterval(size_t interval) {
    pImpl->updateInterval = interval;
}

void BrainStateVisualizer::setRealTimeDisplay(bool enabled) {
    pImpl->realTimeEnabled = enabled;
    
    if (enabled) {
        NLM_LOG_INFO("Real-time display enabled");
    } else {
        NLM_LOG_INFO("Real-time display disabled");
    }
}

void BrainStateVisualizer::setColorsEnabled(bool enabled) {
    pImpl->colorsEnabled = enabled;
}

void BrainStateVisualizer::addUpdateCallback(VisualizationUpdateCallback callback) {
    if (callback) {
        pImpl->updateCallbacks.push_back(callback);
    }
}

void BrainStateVisualizer::addExportCallback(VisualizationExportCallback callback) {
    if (callback) {
        pImpl->exportCallbacks.push_back(callback);
    }
}

std::string BrainStateVisualizer::generateReport(const std::string& format) const {
    try {
        if (format == "text" || format == "terminal") {
            return generateTextReport();
        } else if (format == "json") {
            return generateJsonReport();
        } else if (format == "csv") {
            return generateCsvReport();
        } else if (format == "html") {
            return generateHtmlReport();
        } else {
            return generateTextReport();
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to generate report: " + std::string(e.what()));
        return "Error generating report: " + std::string(e.what());
    }
}

bool BrainStateVisualizer::exportVisualization(const std::string& filename, const std::string& format) {
    if (pImpl->stateHistory.empty()) {
        NLM_LOG_WARNING("No visualization data to export");
        return false;
    }
    
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }
        
        std::string content;
        if (format == "text" || format == "terminal") {
            content = generateTextReport();
        } else if (format == "json") {
            content = generateJsonReport();
        } else if (format == "csv") {
            content = generateCsvReport();
        } else if (format == "html") {
            content = generateHtmlReport();
        } else {
            content = generateTextReport();
        }
        
        file << content;
        file.close();
        
        // Call export callbacks
        for (const auto& callback : pImpl->exportCallbacks) {
            try {
                callback(filename, format);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Export callback failed: " + std::string(e.what()));
            }
        }
        
        NLM_LOG_INFO("Visualization exported to: " << filename << " (format: " << format << ")");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export visualization: " + std::string(e.what()));
        return false;
    }
}

BrainStateSnapshot BrainStateVisualizer::getCurrentState(std::shared_ptr<Brain> brain) const {
    BrainStateSnapshot snapshot;
    
    if (!brain) {
        return snapshot;
    }
    
    auto now = std::chrono::steady_clock::now();
    snapshot.timestamp = std::chrono::duration<double>(now.time_since_epoch()).count();
    snapshot.step = 0; // Will be set by caller
    snapshot.simulationTime = snapshot.timestamp - std::chrono::duration<double>(pImpl->startTime.time_since_epoch()).count();
    
    // Neural activity metrics
    snapshot.totalNeuronCount = brain->getTotalNeuronCount();
    snapshot.firingNeuronCount = brain->getFiringNeuronCount();
    snapshot.averageFiringRate = brain->getAverageFiringRate();
    snapshot.totalSpikeCount = brain->getTotalSpikeCount();
    
    // Network metrics
    snapshot.totalSynapseCount = brain->getTotalSynapseCount();
    
    // Memory system states
    if (auto* workingMemory = brain->getWorkingMemory()) {
        snapshot.workingMemoryTraces = workingMemory->getActiveTraces();
    }
    
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        snapshot.episodicMemoryCount = episodicMemory->getEpisodeCount();
    }
    
    if (auto* conceptFormation = brain->getConceptFormation()) {
        snapshot.conceptCount = conceptFormation->getConceptCount();
        
        // Get active concepts
        for (size_t i = 0; i < snapshot.conceptCount; ++i) {
            if (const auto* concept = conceptFormation->getConcept(i)) {
                snapshot.activeConcepts.push_back(concept->categoryHint);
            }
        }
    }
    
    // Neuromodulation states
    if (auto* dopamine = brain->getDopamine()) {
        snapshot.dopamineLevel = dopamine->getLevel();
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        snapshot.curiosityLevel = curiosity->getLevel();
    }
    
    if (auto* novelty = brain->getNovelty()) {
        snapshot.noveltyLevel = novelty->getLevel();
    }
    
    if (auto* predictionError = brain->getPredictionError()) {
        snapshot.predictionError = predictionError->getLevel();
    }
    
    // Cognitive states
    if (auto* planner = brain->getPlanner()) {
        // Get current planning action (simplified)
        snapshot.planningAction = "Planning..."; // This would need to be implemented
    }
    
    if (auto* selfModel = brain->getSelfModel()) {
        snapshot.selfModelConfidence = selfModel->getConfidence();
    }
    
    // Performance metrics
    snapshot.stepExecutionTime = 0.001; // Simulated
    snapshot.memoryUsageMB = measureMemoryUsage();
    snapshot.cpuUsagePercent = measureCpuUsage();
    
    return snapshot;
}

std::string BrainStateVisualizer::getStatistics() const {
    std::stringstream stats;
    
    stats << "Brain State Visualization Statistics" << std::endl;
    stats << "==================================" << std::endl;
    stats << "Active: " << (pImpl->isActive ? "Yes" : "No") << std::endl;
    stats << "Running: " << (pImpl->isRunning ? "Yes" : "No") << std::endl;
    stats << "Update frequency: " << static_cast<int>(pImpl->updateFrequency) << std::endl;
    stats << "Update interval: " << pImpl->updateInterval << " steps" << std::endl;
    stats << "Real-time display: " << (pImpl->realTimeEnabled ? "Yes" : "No") << std::endl;
    stats << "Colors enabled: " << (pImpl->colorsEnabled ? "Yes" : "No") << std::endl;
    stats << "State history size: " << pImpl->stateHistory.size() << "/" << pImpl->maxHistorySize << std::endl;
    stats << "Update callbacks: " << pImpl->updateCallbacks.size() << std::endl;
    stats << "Export callbacks: " << pImpl->exportCallbacks.size() << std::endl;
    
    if (!pImpl->currentBrain.expired()) {
        auto brain = pImpl->currentBrain.lock();
        stats << "Current brain neurons: " << brain->getTotalNeuronCount() << std::endl;
        stats << "Current brain synapses: " << brain->getTotalSynapseCount() << std::endl;
    }
    
    return stats.str();
}

bool BrainStateVisualizer::isActive() const {
    return pImpl->isActive;
}

bool BrainStateVisualizer::isRunning() const {
    return pImpl->isRunning;
}

void BrainStateVisualizer::reset() {
    std::lock_guard<std::mutex> lock(pImpl->updateMutex);
    
    pImpl->stateHistory.clear();
    pImpl->updateCounter = 0;
    pImpl->updateThread = std::thread();
    pImpl->threadShouldStop = false;
    
    NLM_LOG_INFO("Brain state visualization reset");
}

void BrainStateVisualizer::cleanup() {
    stopVisualization();
    
    std::lock_guard<std::mutex> lock(pImpl->updateMutex);
    
    pImpl->currentBrain.reset();
    pImpl->updateCallbacks.clear();
    pImpl->exportCallbacks.clear();
    
    pImpl->isActive = false;
    pImpl->isRunning = false;
    
    NLM_LOG_INFO("Brain state visualization cleaned up");
}

void BrainStateVisualizer::visualizationUpdateThread(std::shared_ptr<Brain> brain) {
    while (!pImpl->threadShouldStop) {
        if (pImpl->currentBrain.expired() || !pImpl->currentBrain.lock()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        // Update visualization
        updateVisualization(pImpl->currentBrain.lock());
        
        // Increment update counter
        pImpl->updateCounter++;
        
        // Sleep based on update interval
        std::this_thread::sleep_for(std::chrono::milliseconds(
            static_cast<int>(pImpl->updateIntervalSeconds * 1000.0)));
    }
}

std::string BrainStateVisualizer::generateTextVisualization(const BrainStateSnapshot& snapshot) const {
    std::stringstream ss;
    
    ss << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    ss << "║                     NLM Brain State Monitor                      ║" << std::endl;
    ss << "╠═══════════════════════════════════════════════════════════════╣" << std::endl;
    ss << "║                                                               ║" << std::endl;
    
    // Neural activity section
    ss << "║  Neural Activity:                                             ║" << std::endl;
    ss << "║    Neurons: " << std::setw(6) << snapshot.totalNeuronCount 
       << " | Active: " << std::setw(6) << snapshot.activeNeuronCount << " ║" << std::endl;
    ss << "║    Firing: " << std::setw(6) << snapshot.firingNeuronCount 
       << " | Rate: " << std::setw(9) << std::fixed << std::setprecision(2) 
       << snapshot.averageFiringRate << " Hz ║" << std::endl;
    ss << "║    Spikes: " << std::setw(6) << snapshot.totalSpikeCount << "                         ║" << std::endl;
    ss << "║                                                               ║" << std::endl;
    
    // Network metrics section
    ss << "║  Network Metrics:                                             ║" << std::endl;
    ss << "║    Synapses: " << std::setw(6) << snapshot.totalSynapseCount 
       << " | Weight Avg: " << std::setw(9) << std::fixed << std::setprecision(4) 
       << snapshot.averageSynapticWeight << " ║" << std::endl;
    ss << "║    E/I Ratio: " << std::setw(9) << std::fixed << std::setprecision(3) 
       << snapshot.excitationInhibitionRatio << "                      ║" << std::endl;
    ss << "║                                                               ║" << std::endl;
    
    // Memory system section
    ss << "║  Memory Systems:                                              ║" << std::endl;
    ss << "║    Working Memory: " << std::setw(6) << snapshot.workingMemoryTraces 
       << " traces                         ║" << std::endl;
    ss << "║    Episodic Memory: " << std::setw(6) << snapshot.episodicMemoryCount 
       << " episodes                       ║" << std::endl;
    ss << "║    Concepts: " << std::setw(6) << snapshot.conceptCount 
       << "                                     ║" << std::endl;
    ss << "║                                                               ║" << std::endl;
    
    // Neuromodulation section
    ss << "║  Neuromodulation:                                             ║" << std::endl;
    ss << "║    Dopamine: " << std::setw(9) << std::fixed << std::setprecision(3) 
       << snapshot.dopamineLevel << "                              ║" << std::endl;
    ss << "║    Curiosity: " << std::setw(9) << std::fixed << std::setprecision(3) 
       << snapshot.curiosityLevel << "                            ║" << std::endl;
    ss << "║    Novelty: " << std::setw(9) << std::fixed << std::setprecision(3) 
       << snapshot.noveltyLevel << "                               ║" << std::endl;
    ss << "║    Prediction Error: " << std::setw(9) << std::fixed << std::setprecision(3) 
       << snapshot.predictionError << "                      ║" << std::endl;
    ss << "║                                                               ║" << std::endl;
    
    // Cognitive section
    ss << "║  Cognitive State:                                             ║" << std::endl;
    ss << "║    Active Concepts: " << std::setw(6) << snapshot.activeConcepts.size() << "                              ║" << std::endl;
    if (!snapshot.activeConcepts.empty()) {
        for (size_t i = 0; i < std::min<size_t>(3, snapshot.activeConcepts.size()); ++i) {
            ss << "║      - " << snapshot.activeConcepts[i] << std::endl;
        }
        if (snapshot.activeConcepts.size() > 3) {
            ss << "║      ... and " << (snapshot.activeConcepts.size() - 3) << " more" << std::endl;
        }
    } else {
        ss << "║      (none)                                                   ║" << std::endl;
    }
    ss << "║    Self Model Confidence: " << std::setw(9) << std::fixed << std::setprecision(3) 
       << snapshot.selfModelConfidence << "                  ║" << std::endl;
    ss << "║                                                               ║" << std::endl;
    
    // Performance section
    ss << "║  Performance:                                                 ║" << std::endl;
    ss << "║    Memory Usage: " << std::setw(9) << std::fixed << std::setprecision(2) 
       << snapshot.memoryUsageMB << " MB                    ║" << std::endl;
    ss << "║    CPU Usage: " << std::setw(9) << std::fixed << std::setprecision(1) 
       << snapshot.cpuUsagePercent << "%                   ║" << std::endl;
    ss << "║    Step Time: " << std::setw(9) << std::fixed << std::setprecision(4) 
       << snapshot.stepExecutionTime << " s                 ║" << std::endl;
    ss << "║                                                               ║" << std::endl;
    
    ss << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    
    return ss.str();
}

std::string BrainStateVisualizer::generateAsciiArtVisualization(const BrainStateSnapshot& snapshot) const {
    std::stringstream ss;
    
    ss << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    ss << "║                     NLM Brain State Monitor                      ║" << std::endl;
    ss << "╠═══════════════════════════════════════════════════════════════╣" << std::endl;
    ss << "║                                                               ║" << std::endl;
    
    // Neural activity bar
    ss << "║  Neural Activity:                                             ║" << std::endl;
    ss << "║    Neurons: " << std::setw(6) << snapshot.totalNeuronCount 
       << " | Active: " << std::setw(6) << snapshot.activeNeuronCount << " ║" << std::endl;
    ss << "║    Firing Rate: " << std::setw(9) << std::fixed << std::setprecision(2) 
       << snapshot.averageFiringRate << " Hz                  ║" << std::endl;
    ss << "║    Total Spikes: " << std::setw(6) << snapshot.totalSpikeCount << "                        ║" << std::endl;
    ss << "║                                                               ║" << std::endl;
    
    // Network activity
    ss << "║  Network Activity:                                            ║" << std::endl;
    ss << "║    Synapses: " << std::setw(6) << snapshot.totalSynapseCount 
       << " | Avg Weight: " << std::setw(9) << std::fixed << std::setprecision(4) 
       << snapshot.averageSynapticWeight << " ║" << std::endl;
    ss << "║    E/I Ratio: " << std::setw(9) << std::fixed << std::setprecision(3) 
       << snapshot.excitationInhibitionRatio << "                 ║" << std::endl;
    ss << "║                                                               ║" << std::endl;
    
    // Memory systems
    ss << "║  Memory Systems:                                              ║" << std::endl;
    ss << "║    Working Memory: " << std::setw(6) << snapshot.workingMemoryTraces 
       << " traces (capacity: ?)                   ║" << std::endl;
    ss << "║    Episodic Memory: " << std::setw(6) << snapshot.episodicMemoryCount 
       << " episodes (capacity: ?)              ║" << std::endl;
    ss << "║    Concepts: " << std::setw(6) << snapshot.conceptCount 
       << "                                     ║" << std::endl;
    ss << "║                                                               ║" << std::endl;
    
    // Neuromodulation
    ss << "║  Neuromodulation:                                             ║" << std::endl;
    ss << "║    Dopamine: " << std::setw(9) << std::fixed << std::setprecision(3) 
       << snapshot.dopamineLevel << "                      ║" << std::endl;
    ss << "║    Curiosity: " << std::setw(9) << std::fixed << std::setprecision(3) 
       << snapshot.curiosityLevel << "                    ║" << std::endl;
    ss << "║    Novelty: " << std::setw(9) << std::fixed << std::setprecision(3) 
       << snapshot.noveltyLevel << "                     ║" << std::endl;
    ss << "║    Prediction Error: " << std::setw(9) << std::fixed << std::setprecision(3) 
       << snapshot.predictionError << "               ║" << std::endl;
    ss << "║                                                               ║" << std::endl;
    
    // Status
    ss << "║  Status: " << std::setw(52) << "Active" << "║" << std::endl;
    ss << "║  Time: " << std::setw(57) << "Simulation " << std::setw(6) << snapshot.step << "s" << "║" << std::endl;
    ss << "║                                                               ║" << std::endl;
    
    ss << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    
    return ss.str();
}

// Additional formatter implementations would continue...

// Implementation of convenience functions
bool startBrainStateVisualization(std::shared_ptr<Brain> brain,
                                  VisualizationUpdateFrequency frequency) {
    return getBrainStateVisualizer().startVisualization(brain, frequency);
}

bool updateBrainStateVisualization(std::shared_ptr<Brain> brain) {
    getBrainStateVisualizer().updateVisualization(brain);
    return true;
}

void stopBrainStateVisualization() {
    getBrainStateVisualizer().stopVisualization();
}

bool exportBrainStateVisualization(const std::string& filename, const std::string& format) {
    return getBrainStateVisualizer().exportVisualization(filename, format);
}

std::string generateBrainStateReport(std::shared_ptr<Brain> brain, const std::string& format) {
    BrainStateSnapshot snapshot = getBrainStateVisualizer().getCurrentState(brain);
    
    if (format == "text" || format == "terminal") {
        return TextVisualizerFormatter::format(snapshot);
    } else if (format == "ascii") {
        return AsciiArtVisualizerFormatter::format(snapshot);
    } else if (format == "json") {
        return JsonVisualizerFormatter::format(snapshot);
    } else if (format == "csv") {
        return CsvVisualizerFormatter::format(snapshot);
    } else if (format == "html") {
        return HtmlVisualizerFormatter::format(snapshot);
    } else {
        return TextVisualizerFormatter::format(snapshot);
    }
}

// Global visualization manager instance
BrainStateVisualizer& getBrainStateVisualizer() {
    static BrainStateVisualizer visualizer;
    return visualizer;
}

// Helper function implementations
std::string getFormatString(VisualizationFormat format) {
    switch (format) {
        case VisualizationFormat::TEXT: return "text";
        case VisualizationFormat::ASCII_ART: return "ascii";
        case VisualizationFormat::JSON: return "json";
        case VisualizationFormat::CSV: return "csv";
        case VisualizationFormat::HTML: return "html";
        case VisualizationFormat::TERMINAL_DASHBOARD: return "terminal";
        default: return "text";
    }
}

double measureMemoryUsage() {
    // Simplified memory measurement
    return 100.0 + rand() % 100; // Simulated
}

double measureCpuUsage() {
    // Simplified CPU measurement
    return 10.0 + (rand() % 50); // Simulated
}

} // namespace nlm

// Entry point for the enhanced visualization tool
int mainVisualization(int argc, char** argv) {
    try {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                  NLM Brain State Visualization Tool               ║" << std::endl;
        std::cout << "╠═══════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║  Enhanced visualization for monitoring NLM brain state          ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        
        // Parse command line arguments
        std::string outputFile;
        std::string format = "terminal";
        bool startVisualization = false;
        size_t updateInterval = 10;
        std::string frequency = "medium";
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                std::cout << "Usage: nlm_visualization [OPTIONS]" << std::endl;
                std::cout << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  --output=<file>              Output file for visualization" << std::endl;
                std::cout << "  --format=<format>            Visualization format (text, ascii, json, csv, html, terminal)" << std::endl;
                std::cout << "  --start                      Start visualization mode" << std::endl;
                std::cout << "  --update-interval=<n>        Update interval in steps" << std::endl;
                std::cout << "  --frequency=<freq>           Update frequency (high, medium, low, custom)" << std::endl;
                std::cout << "  --help, -h                   Show this help message" << std::endl;
                std::cout << std::endl;
                std::cout << "Examples:" << std::endl;
                std::cout << "  nlm_visualization --start --update-interval=100" << std::endl;
                std::cout << "  nlm_visualization --output=visualization.json --format=json" << std::endl;
                return 0;
            }
            
            if (arg.substr(0, 12) == "--output=") {
                outputFile = arg.substr(12);
            } else if (arg.substr(0, 10) == "--format=") {
                format = arg.substr(10);
            } else if (arg == "--start") {
                startVisualization = true;
            } else if (arg.substr(0, 19) == "--update-interval=") {
                updateInterval = std::stoul(arg.substr(19));
            } else if (arg.substr(0, 13) == "--frequency=") {
                frequency = arg.substr(13);
            }
        }
        
        // Initialize visualization
        nlm::BrainStateVisualizer& visualizer = nlm::getBrainStateVisualizer();
        
        if (!visualizer.initialize()) {
            std::cerr << "Failed to initialize visualization" << std::endl;
            return 1;
        }
        
        // Set output file if specified
        if (!outputFile.empty()) {
            visualizer.setOutputFile(outputFile);
        }
        
        // Set format
        nlm::VisualizationFormat vizFormat;
        if (format == "ascii") {
            vizFormat = nlm::VisualizationFormat::ASCII_ART;
        } else if (format == "json") {
            vizFormat = nlm::VisualizationFormat::JSON;
        } else if (format == "csv") {
            vizFormat = nlm::VisualizationFormat::CSV;
        } else if (format == "html") {
            vizFormat = nlm::VisualizationFormat::HTML;
        } else if (format == "terminal") {
            vizFormat = nlm::VisualizationFormat::TERMINAL_DASHBOARD;
        } else {
            vizFormat = nlm::VisualizationFormat::TEXT;
        }
        
        visualizer.setFormat(vizFormat);
        
        // Convert frequency string to enum
        nlm::VisualizationUpdateFrequency freq;
        if (frequency == "high") {
            freq = nlm::VisualizationUpdateFrequency::HIGH;
        } else if (frequency == "medium") {
            freq = nlm::VisualizationUpdateFrequency::MEDIUM;
        } else if (frequency == "low") {
            freq = nlm::VisualizationUpdateFrequency::LOW;
        } else {
            freq = nlm::VisualizationUpdateFrequency::MEDIUM;
        }
        
        // Start visualization if requested
        if (startVisualization) {
            // This would require a brain instance, which would come from the demo system
            // For now, we'll just show the available options
            std::cout << "Visualization ready. To use visualization, start a demo with visualization enabled." << std::endl;
            std::cout << "Example: nlm_demo --demo=basic --visualization" << std::endl;
        } else {
            // Generate and display current visualization
            std::cout << "Generating visualization..." << std::endl;
            std::cout << visualizer.generateTextVisualization(
                visualizer.getCurrentState(nullptr)) << std::endl;
        }
        
        // Display statistics
        std::cout << std::endl;
        std::cout << visualizer.getStatistics() << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int main(int argc, char** argv) {
    return mainVisualization(argc, argv);
}
