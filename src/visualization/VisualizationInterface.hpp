// Enhanced Visualization and Monitoring Interface
// Provides real-time visualization and monitoring capabilities for NLM

#include "visualization/VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>
#include <cmath>
#include <sstream>

using namespace nlm;

// Forward declarations
class ConsoleVisualization;
class MonitoringInterface;

// Visualization Interface Base Class
class VisualizationInterface {
public:
    virtual ~VisualizationInterface() = default;
    
    virtual void initialize(std::shared_ptr<Brain> brain) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void setRefreshRate(int fps) = 0;
    
    virtual void showNeurons(bool show) = 0;
    virtual void showSynapses(bool show) = 0;
    virtual void showMemory(bool show) = 0;
    virtual void showNeuromodulation(bool show) = 0;
    virtual void showStatistics(bool show) = 0;
    virtual void highlightActiveNeurons(bool highlight) = 0;
    virtual void showConnectionWeights(bool show) = 0;
    virtual void setAutoRefresh(bool autoRefresh) = 0;
    
    virtual void updateDisplay() = 0;
};

// Monitoring Interface Base Class
class MonitoringInterface {
public:
    virtual ~MonitoringInterface() = default;
    
    virtual void initialize(std::shared_ptr<Brain> brain) = 0;
    virtual void startMonitoring() = 0;
    virtual void stopMonitoring() = 0;
    virtual void enableMonitoring(bool enable) = 0;
    
    virtual void setRealTimeMonitoring(bool realTime) = 0;
    virtual void setLogFile(const std::string& filepath) = 0;
    virtual void setLogLevel(int level) = 0;
    virtual void setMetricsInterval(size_t intervalMs) = 0;
    virtual void enableAutoSave(bool enable) = 0;
    virtual void setMaxLogSize(size_t maxSize) = 0;
    
    virtual void logStep() = 0;
    virtual std::vector<MetricData> getMetrics(size_t count) = 0;
    virtual void clearMetrics() = 0;
    virtual float getAverageFiringRate(size_t steps) = 0;
    virtual float getAverageEIRatio(size_t steps) = 0;
    virtual size_t getTotalSpikeCount(size_t steps) = 0;
    virtual size_t getTotalMemoryUsage() = 0;
    virtual std::string getPerformanceSummary() = 0;
    virtual void saveMetrics() = 0;
    virtual void exportMetrics(const std::string& filepath) = 0;
};

// Performance metrics structure
struct MetricData {
    size_t step;
    size_t totalNeurons;
    size_t totalSynapses;
    float avgFiringRate;
    float eiRatio;
    size_t totalSpikes;
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    std::string timestamp;
};

// Monitor levels
enum class MonitorLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3
};

// Console Visualization Implementation
class ConsoleVisualization : public VisualizationInterface {
public:
    ConsoleVisualization() : refreshRate(10), showNeurons(true), showSynapses(true), 
                           showMemory(true), showNeuromodulation(true), showStats(true),
                           highlightActiveNeurons(true), showConnectionWeights(true),
                           autoRefresh(false), running(false), updateCount(0) {}
    
    ~ConsoleVisualization() {
        running = false;
        if (visualizationThread.joinable()) {
            visualizationThread.join();
        }
    }
    
    void initialize(std::shared_ptr<Brain> brain) override {
        this->brain = brain;
        NLM_LOG_INFO("Console visualization initialized");
    }
    
    void start() override {
        if (!brain) {
            NLM_LOG_ERROR("Brain not initialized for visualization");
            return;
        }
        
        running = true;
        visualizationThread = std::thread(&ConsoleVisualization::visualizationLoop, this);
        NLM_LOG_INFO("Console visualization started");
    }
    
    void stop() override {
        running = false;
        if (visualizationThread.joinable()) {
            visualizationThread.join();
        }
        NLM_LOG_INFO("Console visualization stopped");
    }
    
    void setRefreshRate(int fps) override {
        refreshRate = fps;
        NLM_LOG_INFO("Visualization refresh rate set to " + std::to_string(refreshRate) + " FPS");
    }
    
    void showNeurons(bool show) override {
        this->showNeurons = show;
    }
    
    void showSynapses(bool show) override {
        this->showSynapses = show;
    }
    
    void showMemory(bool show) override {
        this->showMemory = show;
    }
    
    void showNeuromodulation(bool show) override {
        this->showNeuromodulation = show;
    }
    
    void showStatistics(bool show) override {
        this->showStats = show;
    }
    
    void highlightActiveNeurons(bool highlight) override {
        this->highlightActiveNeurons = highlight;
    }
    
    void showConnectionWeights(bool show) override {
        this->showConnectionWeights = show;
    }
    
    void setAutoRefresh(bool autoRefresh) override {
        this->autoRefresh = autoRefresh;
    }
    
    void updateDisplay() override {
        std::lock_guard<std::mutex> lock(updateMutex);
        
        clearScreen();
        printHeader();
        
        if (brain) {
            if (showNeurons) {
                printNeuronInfo();
            }
            if (showSynapses) {
                printSynapseInfo();
            }
            if (showMemory) {
                printMemoryInfo();
            }
            if (showNeuromodulation) {
                printNeuromodulationInfo();
            }
            if (showStats) {
                printStatistics();
            }
        }
        
        printFooter();
        updateCount++;
    }
    
private:
    void visualizationLoop() {
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        while (running) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
            
            if (elapsed.count() >= 1000 / refreshRate) {
                updateDisplay();
                lastTime = currentTime;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    void clearScreen() {
        // Clear screen (works on most terminals)
        std::cout << "\033[2J\033[H";
    }
    
    void printHeader() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                        NLM Visualization                          ║" << std::endl;
        std::cout << "║                   Enhanced Brain Activity Display                 ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        
        // Status line
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║ Status: " << (running ? "RUNNING   " : "STOPPED   ") << "║ Time: " << getCurrentTime() << " ";
        std::cout << "║ Updates: " << updateCount << "                             " << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
    }
    
    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");
        return ss.str();
    }
    
    void printNeuronInfo() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                         NEURAL POPULATIONS                           ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        
        if (!brain) {
            std::cout << "No brain initialized" << std::endl;
            return;
        }
        
        std::vector<RegionId> regionIds = brain->getRegionIds();
        
        for (size_t i = 0; i < regionIds.size(); ++i) {
            auto* region = brain->getRegion(regionIds[i]);
            if (!region) continue;
            
            std::cout << "Region " << regionIds[i].index() << ": " << region->getName() << std::endl;
            
            auto populations = region->getPopulations();
            for (size_t j = 0; j < populations.size(); ++j) {
                auto* pop = populations[j];
                if (!pop) continue;
                
                NeuronType type = pop->getNeuronType();
                std::string typeStr;
                switch (type) {
                    case NeuronType::Sensory: typeStr = "SENSORY"; break;
                    case NeuronType::Motor: typeStr = "MOTOR"; break;
                    case NeuronType::Internal: typeStr = "INTERNAL"; break;
                    case NeuronType::Excitatory: typeStr = "EXCITATORY"; break;
                    case NeuronType::Inhibitory: typeStr = "INHIBITORY"; break;
                    case NeuronType::Modulatory: typeStr = "MODULATORY"; break;
                    default: typeStr = "UNKNOWN"; break;
                }
                
                size_t neuronCount = pop->getNeuronCount();
                float avgFiring = pop->getAverageFiringRate();
                size_t activeCount = pop->getActiveNeuronCount();
                
                std::cout << "  Population " << j << " (" << typeStr << "):";
                std::cout << " " << neuronCount << " neurons, " << avgFiring << " Hz avg";
                if (activeCount > 0) {
                    std::cout << ", " << activeCount << " active";
                }
                std::cout << std::endl;
                
                if (showConnectionWeights && neuronCount > 0) {
                    float avgWeight = pop->getAverageSynapticWeight();
                    std::cout << "    Average weight: " << avgWeight << std::endl;
                }
            }
            
            std::cout << std::endl;
        }
    }
    
    void printSynapseInfo() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                       SYNAPTIC CONNECTIVITY                          ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        
        if (!brain) {
            std::cout << "No brain initialized" << std::endl;
            return;
        }
        
        size_t totalSynapses = brain->getTotalSynapseCount();
        std::cout << "Total synapses: " << totalSynapses << std::endl;
        
        float eiRatio = brain->getExcitationInhibitionRatio();
        std::cout << "E/I ratio: " << eiRatio << std::endl;
        
        size_t totalSpikes = brain->getTotalSpikeCount();
        std::cout << "Total spikes generated: " << totalSpikes << std::endl;
        
        size_t firingNeurons = brain->getFiringNeuronCount();
        std::cout << "Firing neurons (this step): " << firingNeurons << std::endl;
        
        if (showConnectionWeights) {
            std::cout << "Connection weight statistics:" << std::endl;
            // This would require additional methods in Brain class
            // For now, we'll just display available information
        }
    }
    
    void printMemoryInfo() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                         MEMORY SYSTEMS                               ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        
        if (!brain) {
            std::cout << "No brain initialized" << std::endl;
            return;
        }
        
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        auto* associativeMemory = brain->getAssociativeMemory();
        
        if (workingMemory) {
            std::cout << "Working Memory:" << std::endl;
            std::cout << "  Active traces: " << workingMemory->getActiveTraces() << std::endl;
            std::cout << "  Memory neurons: " << workingMemory->getMemoryNeuronsCount() << std::endl;
            std::cout << "  Competition winners: " << workingMemory->getWinnerCount() << std::endl;
        }
        
        if (episodicMemory) {
            std::cout << "Episodic Memory:" << std::endl;
            std::cout << "  Episodes stored: " << episodicMemory->getEpisodeCount() << std::endl;
            std::cout << "  Max capacity: " << episodicMemory->getMaxEpisodes() << std::endl;
            
            if (episodicMemory->getEpisodeCount() > 0) {
                auto* recentEpisode = episodicMemory->getRecentEpisode(1);
                if (recentEpisode) {
                    std::cout << "  Most recent episode reward: " << recentEpisode->reward << std::endl;
                }
            }
        }
        
        if (associativeMemory) {
            std::cout << "Associative Memory:" << std::endl;
            // This would require additional methods in NeuralAssociativeMemory
            std::cout << "  Association count: (not available)" << std::endl;
        }
    }
    
    void printNeuromodulationInfo() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                       NEUROMODULATION SYSTEMS                       ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        
        if (!brain) {
            std::cout << "No brain initialized" << std::endl;
            return;
        }
        
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        auto* novelty = brain->getNovelty();
        auto* predictionError = brain->getPredictionErrorSignal();
        
        if (dopamine) {
            std::cout << "Dopamine:" << std::endl;
            std::cout << "  Level: " << dopamine->getLevel() << std::endl;
            std::cout << "  Plasticity factor: " << dopamine->getPlasticityFactor() << std::endl;
        }
        
        if (curiosity) {
            std::cout << "Curiosity:" << std::endl;
            std::cout << "  Level: " << curiosity->getLevel() << std::endl;
            std::cout << "  Exploration drive: " << curiosity->getExplorationDrive() << std::endl;
        }
        
        if (novelty) {
            std::cout << "Novelty:" << std::endl;
            std::cout << "  Level: " << novelty->getLevel() << std::endl;
            std::cout << "  Threshold: " << novelty->getNoveltyThreshold() << std::endl;
        }
        
        if (predictionError) {
            std::cout << "Prediction Error:" << std::endl;
            std::cout << "  Error: " << predictionError->getError() << std::endl;
            std::cout << "  Magnitude: " << predictionError->getMagnitude() << std::endl;
        }
    }
    
    void printStatistics() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                         SYSTEM STATISTICS                              ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        
        if (!brain) {
            std::cout << "No brain initialized" << std::endl;
            return;
        }
        
        // Basic brain statistics
        std::cout << "Brain Statistics:" << std::endl;
        std::cout << "  Regions: " << brain->getRegionCount() << std::endl;
        std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        
        // Performance statistics
        float avgFiringRate = brain->getAverageFiringRate();
        std::cout << "  Average firing rate: " << avgFiringRate << " Hz" << std::endl;
        
        float eiRatio = brain->getExcitationInhibitionRatio();
        std::cout << "  E/I ratio: " << eiRatio << std::endl;
        
        // Spike statistics
        size_t totalSpikes = brain->getTotalSpikeCount();
        std::cout << "  Total spikes: " << totalSpikes << std::endl;
        
        size_t firingNeurons = brain->getFiringNeuronCount();
        std::cout << "  Firing neurons this step: " << firingNeurons << std::endl;
        
        // Developmental status
        auto* developmentSystem = brain->getDevelopmentSystem();
        if (developmentSystem) {
            std::cout << "Development:" << std::endl;
            std::cout << "  Current stage: " << static_cast<int>(developmentSystem->getDevelopmentalStage()) << std::endl;
        }
        
        // Visualization-specific statistics
        std::cout << "Visualization:" << std::endl;
        std::cout << "  Update count: " << updateCount << std::endl;
        std::cout << "  Refresh rate: " << refreshRate << " FPS" << std::endl;
        std::cout << "  Auto refresh: " << (autoRefresh ? "ON" : "OFF") << std::endl;
        
        // Feature visibility
        std::cout << "Display options:" << std::endl;
        std::cout << "  Neurons: " << (showNeurons ? "ON" : "OFF") << std::endl;
        std::cout << "  Synapses: " << (showSynapses ? "ON" : "OFF") << std::endl;
        std::cout << "  Memory: " << (showMemory ? "ON" : "OFF") << std::endl;
        std::cout << "  Neuromodulation: " << (showNeuromodulation ? "ON" : "OFF") << std::endl;
        std::cout << "  Statistics: " << (showStats ? "ON" : "OFF") << std::endl;
        std::cout << "  Active neurons highlight: " << (highlightActiveNeurons ? "ON" : "OFF") << std::endl;
        std::cout << "  Connection weights: " << (showConnectionWeights ? "ON" : "OFF") << std::endl;
    }
    
    void printFooter() {
        std::cout << std::endl;
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║ Controls: h=help r=refresh t=toggle_all space=single_update q=quit  ║" << std::endl;
        std::cout << "║                              Visualization v0.1.0                       ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
    }
    
    // Thread safety
    std::mutex updateMutex;
    std::thread visualizationThread;
    
    // Configuration
    int refreshRate;
    bool showNeurons;
    bool showSynapses;
    bool showMemory;
    bool showNeuromodulation;
    bool showStats;
    bool highlightActiveNeurons;
    bool showConnectionWeights;
    bool autoRefresh;
    bool running;
    size_t updateCount;
    
    // State
    std::shared_ptr<Brain> brain;
};

// Monitoring Implementation
class MonitoringInterface : public MonitoringInterface {
public:
    MonitoringInterface() : monitoringEnabled(true), realTimeMonitoring(false), 
                           logFile(""), logLevel(MonitorLevel::INFO), 
                           metricsInterval(1000), autoSave(false), maxLogSize(10000),
                           currentStep(0) {}
    
    ~MonitoringInterface() {
        stopMonitoring();
    }
    
    void initialize(std::shared_ptr<Brain> brain) override {
        this->brain = brain;
        NLM_LOG_INFO("Monitoring interface initialized");
    }
    
    void startMonitoring() override {
        if (!brain) {
            NLM_LOG_ERROR("Brain not initialized for monitoring");
            return;
        }
        
        monitoringEnabled = true;
        monitoringThread = std::thread(&MonitoringInterface::monitoringLoop, this);
        NLM_LOG_INFO("Monitoring started");
    }
    
    void stopMonitoring() override {
        monitoringEnabled = false;
        if (monitoringThread.joinable()) {
            monitoringThread.join();
        }
        NLM_LOG_INFO("Monitoring stopped");
    }
    
    void enableMonitoring(bool enable) override {
        if (enable && !monitoringEnabled) {
            startMonitoring();
        } else if (!enable && monitoringEnabled) {
            stopMonitoring();
        }
        monitoringEnabled = enable;
    }
    
    void setRealTimeMonitoring(bool realTime) override {
        realTimeMonitoring = realTime;
        NLM_LOG_INFO("Real-time monitoring: " + std::string(realTimeMonitoring ? "enabled" : "disabled"));
    }
    
    void setLogFile(const std::string& filepath) override {
        logFile = filepath;
        NLM_LOG_INFO("Monitoring log file set to: " + filepath);
    }
    
    void setLogLevel(MonitorLevel level) override {
        logLevel = level;
        NLM_LOG_INFO("Monitoring log level set to: " + getMonitorLevelString(level));
    }
    
    void setMetricsInterval(size_t intervalMs) override {
        metricsInterval = intervalMs;
        NLM_LOG_INFO("Metrics collection interval set to: " + std::to_string(intervalMs) + " ms");
    }
    
    void enableAutoSave(bool enable) override {
        autoSave = enable;
        NLM_LOG_INFO("Auto-save: " + std::string(enable ? "enabled" : "disabled"));
    }
    
    void setMaxLogSize(size_t maxSize) override {
        maxLogSize = maxSize;
        NLM_LOG_INFO("Maximum log size set to: " + std::to_string(maxSize) + " bytes");
    }
    
    void logStep() override {
        currentStep++;
        
        if (monitoringEnabled) {
            std::lock_guard<std::mutex> lock(logMutex);
            
            // Collect metrics
            MetricData metrics = collectMetrics();
            
            // Log metrics
            logMetrics(metrics);
            
            // Auto-save if enabled
            if (autoSave) {
                saveMetrics();
            }
        }
    }
    
    std::vector<MetricData> getMetrics(size_t count) override {
        std::lock_guard<std::mutex> lock(metricsMutex);
        
        std::vector<MetricData> result;
        result.reserve(std::min(count, recentMetrics.size()));
        
        size_t start = recentMetrics.size() > count ? recentMetrics.size() - count : 0;
        for (size_t i = start; i < recentMetrics.size(); ++i) {
            result.push_back(recentMetrics[i]);
        }
        
        return result;
    }
    
    void clearMetrics() override {
        std::lock_guard<std::mutex> lock(metricsMutex);
        recentMetrics.clear();
        NLM_LOG_INFO("Metrics cleared");
    }
    
    float getAverageFiringRate(size_t steps) override {
        std::lock_guard<std::mutex> lock(metricsMutex);
        
        if (recentMetrics.empty()) {
            return 0.0f;
        }
        
        float totalFiring = 0.0f;
        size_t count = 0;
        
        size_t start = recentMetrics.size() > steps ? recentMetrics.size() - steps : 0;
        for (size_t i = start; i < recentMetrics.size(); ++i) {
            totalFiring += recentMetrics[i].avgFiringRate;
            count++;
        }
        
        return count > 0 ? totalFiring / count : 0.0f;
    }
    
    float getAverageEIRatio(size_t steps) override {
        std::lock_guard<std::mutex> lock(metricsMutex);
        
        if (recentMetrics.empty()) {
            return 0.0f;
        }
        
        float totalEIRatio = 0.0f;
        size_t count = 0;
        
        size_t start = recentMetrics.size() > steps ? recentMetrics.size() - steps : 0;
        for (size_t i = start; i < recentMetrics.size(); ++i) {
            totalEIRatio += recentMetrics[i].eiRatio;
            count++;
        }
        
        return count > 0 ? totalEIRatio / count : 0.0f;
    }
    
    size_t getTotalSpikeCount(size_t steps) override {
        std::lock_guard<std::mutex> lock(metricsMutex);
        
        if (recentMetrics.empty()) {
            return 0;
        }
        
        size_t totalSpikes = 0;
        size_t start = recentMetrics.size() > steps ? recentMetrics.size() - steps : 0;
        for (size_t i = start; i < recentMetrics.size(); ++i) {
            totalSpikes += recentMetrics[i].totalSpikes;
        }
        
        return totalSpikes;
    }
    
    size_t getTotalMemoryUsage() override {
        // Estimate memory usage (simplified implementation)
        if (!brain) {
            return 0;
        }
        
        size_t neurons = brain->getTotalNeuronCount();
        size_t synapses = brain->getTotalSynapseCount();
        
        // Rough estimate (each neuron/synapse uses ~100 bytes)
        return (neurons + synapses) * 100;
    }
    
    std::string getPerformanceSummary() override {
        std::lock_guard<std::mutex> lock(metricsMutex);
        
        if (recentMetrics.empty()) {
            return "No metrics available";
        }
        
        std::stringstream ss;
        
        // Current metrics
        const MetricData& current = recentMetrics.back();
        
        ss << "Performance Summary:" << std::endl;
        ss << "  Current step: " << current.step << std::endl;
        ss << "  Neurons: " << current.totalNeurons << std::endl;
        ss << "  Synapses: " << current.totalSynapses << std::endl;
        ss << "  Firing rate: " << current.avgFiringRate << " Hz" << std::endl;
        ss << "  E/I ratio: " << current.eiRatio << std::endl;
        ss << "  Total spikes: " << current.totalSpikes << std::endl;
        ss << "  Memory usage: " << getTotalMemoryUsage() << " bytes" << std::endl;
        
        if (recentMetrics.size() > 1) {
            ss << "  Average (last " << recentMetrics.size() << " steps):" << std::endl;
            ss << "    Firing rate: " << getAverageFiringRate(recentMetrics.size()) << " Hz" << std::endl;
            ss << "    E/I ratio: " << getAverageEIRatio(recentMetrics.size()) << std::endl;
            ss << "    Total spikes: " << getTotalSpikeCount(recentMetrics.size()) << std::endl;
        }
        
        return ss.str();
    }
    
    void saveMetrics() override {
        if (logFile.empty()) {
            NLM_LOG_WARNING("No log file specified for auto-save");
            return;
        }
        
        // In a real implementation, this would save metrics to the log file
        // For now, we'll just log the action
        NLM_LOG_INFO("Auto-save metrics to: " + logFile);
    }
    
    void exportMetrics(const std::string& filepath) override {
        if (recentMetrics.empty()) {
            NLM_LOG_ERROR("No metrics to export");
            return;
        }
        
        // In a real implementation, this would export metrics to a file
        // For now, we'll just log the action
        NLM_LOG_INFO("Export metrics to: " + filepath);
    }
    
private:
    MetricData collectMetrics() {
        MetricData data;
        data.step = currentStep;
        
        if (brain) {
            data.totalNeurons = brain->getTotalNeuronCount();
            data.totalSynapses = brain->getTotalSynapseCount();
            data.avgFiringRate = brain->getAverageFiringRate();
            data.eiRatio = brain->getExcitationInhibitionRatio();
            data.totalSpikes = brain->getTotalSpikeCount();
            
            // Neuromodulation levels
            auto* dopamine = brain->getDopamine();
            if (dopamine) {
                data.dopamineLevel = dopamine->getLevel();
            }
            
            auto* curiosity = brain->getCuriosity();
            if (curiosity) {
                data.curiosityLevel = curiosity->getLevel();
            }
            
            auto* novelty = brain->getNovelty();
            if (novelty) {
                data.noveltyLevel = novelty->getLevel();
            }
        }
        
        // Timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        data.timestamp = std::ctime(&time);
        
        return data;
    }
    
    void logMetrics(const MetricData& metrics) {
        // Log based on log level
        switch (logLevel) {
            case MonitorLevel::DEBUG:
                logDebugMetrics(metrics);
                break;
            case MonitorLevel::INFO:
                logInfoMetrics(metrics);
                break;
            case MonitorLevel::WARNING:
                logWarningMetrics(metrics);
                break;
            case MonitorLevel::ERROR:
                logErrorMetrics(metrics);
                break;
        }
        
        // Store in recent metrics
        std::lock_guard<std::mutex> lock(metricsMutex);
        recentMetrics.push_back(metrics);
        
        // Limit size of recent metrics
        if (recentMetrics.size() > 1000) {
            recentMetrics.erase(recentMetrics.begin());
        }
    }
    
    void logDebugMetrics(const MetricData& metrics) {
        std::stringstream ss;
        ss << "[DEBUG] Step " << metrics.step 
           << " | Neurons: " << metrics.totalNeurons
           << " | Synapses: " << metrics.totalSynapses
           << " | Firing: " << metrics.avgFiringRate << " Hz"
           << " | E/I: " << metrics.eiRatio
           << " | Spikes: " << metrics.totalSpikes
           << " | Dopamine: " << metrics.dopamineLevel
           << " | Curiosity: " << metrics.curiosityLevel
           << " | Novelty: " << metrics.noveltyLevel;
        
        NLM_LOG_DEBUG(ss.str());
    }
    
    void logInfoMetrics(const MetricData& metrics) {
        std::stringstream ss;
        ss << "[INFO] Step " << metrics.step 
           << " | Neurons: " << metrics.totalNeurons
           << " | Synapses: " << metrics.totalSynapses
           << " | Firing: " << metrics.avgFiringRate << " Hz"
           << " | E/I: " << metrics.eiRatio;
        
        NLM_LOG_INFO(ss.str());
    }
    
    void logWarningMetrics(const MetricData& metrics) {
        std::stringstream ss;
        ss << "[WARNING] Step " << metrics.step 
           << " | Low neurons: " << metrics.totalNeurons
           << " | High firing: " << metrics.avgFiringRate << " Hz"
           << " | Unusual E/I: " << metrics.eiRatio;
        
        NLM_LOG_WARNING(ss.str());
    }
    
    void logErrorMetrics(const MetricData& metrics) {
        std::stringstream ss;
        ss << "[ERROR] Step " << metrics.step 
           << " | System error detected"
           << " | Neurons: " << metrics.totalNeurons
           << " | Synapses: " << metrics.totalSynapses;
        
        NLM_LOG_ERROR(ss.str());
    }
    
    std::string getMonitorLevelString(MonitorLevel level) {
        switch (level) {
            case MonitorLevel::DEBUG: return "DEBUG";
            case MonitorLevel::INFO: return "INFO";
            case MonitorLevel::WARNING: return "WARNING";
            case MonitorLevel::ERROR: return "ERROR";
            default: return "INFO";
        }
    }
    
    void monitoringLoop() {
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        while (monitoringEnabled) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
            
            if (elapsed.count() >= metricsInterval) {
                logStep();
                lastTime = currentTime;
                
                if (realTimeMonitoring) {
                    std::cout << getPerformanceSummary() << std::endl;
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    // Thread safety
    std::mutex logMutex;
    std::mutex metricsMutex;
    std::thread monitoringThread;
    
    // Configuration
    bool monitoringEnabled;
    bool realTimeMonitoring;
    std::string logFile;
    MonitorLevel logLevel;
    size_t metricsInterval;
    bool autoSave;
    size_t maxLogSize;
    
    // State
    size_t currentStep;
    std::vector<MetricData> recentMetrics;
    
    std::shared_ptr<Brain> brain;
};

// Visualization Interface Implementation
void VisualizationInterface::start() {
    // Base implementation - should be overridden by derived classes
}

void VisualizationInterface::stop() {
    // Base implementation - should be overridden by derived classes
}

void VisualizationInterface::setRefreshRate(int fps) {
    // Base implementation - should be overridden by derived classes
}

void VisualizationInterface::showNeurons(bool show) {
    // Base implementation - should be overridden by derived classes
}

void VisualizationInterface::showSynapses(bool show) {
    // Base implementation - should be overridden by derived classes
}

void VisualizationInterface::showMemory(bool show) {
    // Base implementation - should be overridden by derived classes
}

void VisualizationInterface::showNeuromodulation(bool show) {
    // Base implementation - should be overridden by derived classes
}

void VisualizationInterface::showStatistics(bool show) {
    // Base implementation - should be overridden by derived classes
}

void VisualizationInterface::highlightActiveNeurons(bool highlight) {
    // Base implementation - should be overridden by derived classes
}

void VisualizationInterface::showConnectionWeights(bool show) {
    // Base implementation - should be overridden by derived classes
}

void VisualizationInterface::setAutoRefresh(bool autoRefresh) {
    // Base implementation - should be overridden by derived classes
}

void VisualizationInterface::updateDisplay() {
    // Base implementation - should be overridden by derived classes
}

// Monitoring Interface Implementation
void MonitoringInterface::setRealTimeMonitoring(bool realTime) {
    // Base implementation - should be overridden by derived classes
}

void MonitoringInterface::setLogFile(const std::string& filepath) {
    // Base implementation - should be overridden by derived classes
}

void MonitoringInterface::setLogLevel(MonitorLevel level) {
    // Base implementation - should be overridden by derived classes
}

void MonitoringInterface::setMetricsInterval(size_t intervalMs) {
    // Base implementation - should be overridden by derived classes
}

void MonitoringInterface::enableAutoSave(bool enable) {
    // Base implementation - should be overridden by derived classes
}

void MonitoringInterface::setMaxLogSize(size_t maxSize) {
    // Base implementation - should be overridden by derived classes
}

void MonitoringInterface::logStep() {
    // Base implementation - should be overridden by derived classes
}

std::vector<MetricData> MonitoringInterface::getMetrics(size_t count) {
    // Base implementation - should be overridden by derived classes
    return std::vector<MetricData>();
}

void MonitoringInterface::clearMetrics() {
    // Base implementation - should be overridden by derived classes
}

float MonitoringInterface::getAverageFiringRate(size_t steps) {
    // Base implementation - should be overridden by derived classes
    return 0.0f;
}

float MonitoringInterface::getAverageEIRatio(size_t steps) {
    // Base implementation - should be overridden by derived classes
    return 0.0f;
}

long MonitoringInterface::getTotalSpikeCount(size_t steps) {
    // Base implementation - should be overridden by derived classes
    return 0;
}

size_t MonitoringInterface::getTotalMemoryUsage() {
    // Base implementation - should be overridden by derived classes
    return 0;
}

std::string MonitoringInterface::getPerformanceSummary() {
    // Base implementation - should be overridden by derived classes
    return "No performance summary available";
}

void MonitoringInterface::saveMetrics() {
    // Base implementation - should be overridden by derived classes
}

void MonitoringInterface::exportMetrics(const std::string& filepath) {
    // Base implementation - should be overridden by derived classes
}

// Global function to create default visualization and monitoring
std::shared_ptr<VisualizationInterface> createDefaultVisualization() {
    return std::make_shared<ConsoleVisualization>();
}

std::shared_ptr<MonitoringInterface> createDefaultMonitoring() {
    return std::make_shared<MonitoringInterface>();
}

// Main function for visualization and monitoring
int mainVisualizationMonitoring(int argc, char* argv[]) {
    printEnhancedBanner();
    
    std::cout << "=== NLM Visualization and Monitoring ===" << std::endl;
    std::cout << "Starting visualization and monitoring interface..." << std::endl << std::endl;
    
    // Create brain (in a real implementation, this would be loaded from configuration)
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for visualization!" << std::endl;
        return 1;
    }
    
    // Create visualization and monitoring
    auto visualization = createDefaultVisualization();
    auto monitoring = createDefaultMonitoring();
    
    // Initialize with brain
    visualization->initialize(brain);
    monitoring->initialize(brain);
    
    // Start visualization and monitoring
    visualization->start();
    monitoring->startMonitoring();
    
    std::cout << "Visualization and monitoring started." << std::endl;
    std::cout << "Press Ctrl+C to stop." << std::endl;
    
    try {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    // Stop visualization and monitoring
    visualization->stop();
    monitoring->stopMonitoring();
    
    std::cout << "Visualization and monitoring stopped." << std::endl;
    
    return 0;
}

// Banner for visualization and monitoring
void printEnhancedBanner() {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║                     NLM Visualization & Monitoring                ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║                Real-time Brain Activity Display                   ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

// Entry point for visualization and monitoring executable
int main(int argc, char* argv[]) {
    return mainVisualizationMonitoring(argc, argv);
}

} // namespace nlm
