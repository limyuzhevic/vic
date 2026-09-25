#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "environment/Environment.hpp"

namespace nlm {

// Color codes for console output
namespace Color {
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BOLD = "\033[1m";
    const std::string UNDERLINE = "\033[4m";
}

// Visualization modes
enum class VisualizationMode {
    CONSOLA,      // Text-based console visualization
    TERMINAL,     // Advanced terminal visualization
    HTML,         // HTML web visualization
    JSON,         // JSON data export
    CSV,          // CSV data export
    BINARY,       // Binary format
    COMPACT,      // Compact text format
    DETAILED      // Detailed text format
};

// Debug modes
enum class DebugMode {
    NONE,         // No debugging
    BASIC,        // Basic debugging info
    DETAILED,     // Detailed debugging
    VERBOSE,      // Verbose debugging
    NETWORK,      // Network debugging
    PERFORMANCE,  // Performance debugging
    MEMORY,       // Memory debugging
    THREAD,       // Thread debugging
    PLOTTING,     // Plotting/debug data
};

// Real-time metrics for monitoring
struct RealTimeMetrics {
    std::chrono::system_clock::time_point timestamp;
    
    // Brain metrics
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    size_t totalSpikes = 0;
    size_t activeNeurons = 0;
    size_t firingNeurons = 0;
    float averageFiringRate = 0.0f;
    
    // Plasticity metrics
    float plasticityActivity = 0.0f;
    size_t stdpEvents = 0;
    size_t hebbianEvents = 0;
    
    // Neuromodulation metrics
    float dopamineLevel = 0.0f;
    float curiosityLevel = 0.0f;
    float noveltyLevel = 0.0f;
    float predictionError = 0.0f;
    
    // Environmental metrics
    float episodeReward = 0.0f;
    size_t stepsCompleted = 0;
    bool episodeActive = false;
    
    // Performance metrics
    double simulationTime = 0.0;
    double cpuUsage = 0.0;
    double memoryUsage = 0.0;
    
    // Debug metrics
    size_t errorCount = 0;
    size_t warningCount = 0;
    size_t infoCount = 0;
    
    void reset() {
        timestamp = std::chrono::system_clock::now();
        totalNeurons = totalSynapses = totalSpikes = activeNeurons = firingNeurons = 0;
        averageFiringRate = plasticityActivity = 0.0f;
        stdpEvents = hebbianEvents = 0;
        dopamineLevel = curiosityLevel = noveltyLevel = predictionError = 0.0f;
        episodeReward = 0.0f;
        stepsCompleted = 0;
        episodeActive = false;
        simulationTime = cpuUsage = memoryUsage = 0.0;
        errorCount = warningCount = infoCount = 0;
    }
};

// Neural network visualization data
struct NeuralNetworkVisualization {
    size_t width = 80;
    size_t height = 40;
    std::string title = "Neural Network Visualization";
    
    // Region visualization
    struct RegionData {
        size_t id;
        std::string name;
        float activation;
        size_t neuronCount;
        std::vector<float> neuronActivations;
        std::vector<std::pair<size_t, float>> connections; // (target, weight)
    };
    
    std::vector<RegionData> regions;
    
    // Synapse visualization
    struct SynapseData {
        size_t sourceRegion;
        size_t targetRegion;
        float weight;
        float delay;
        bool isActive;
    };
    
    std::vector<SynapseData> synapses;
    
    // Spike visualization
    struct SpikeData {
        size_t neuronId;
        size_t regionId;
        double timestamp;
        float amplitude;
    };
    
    std::vector<SpikeData> spikes;
    
    // Activity heatmap
    std::vector<std::vector<float>> activityHeatmap;
    
    // Generate visualization string
    std::string generateTextVisualization() const {
        std::ostringstream oss;
        
        oss << Color::BOLD << Color::CYAN << title << Color::RESET << std::endl;
        oss << Color::CYAN << std::string(width, '=') << Color::RESET << std::endl;
        
        // Region headers
        oss << Color::YELLOW << "REGIONS:" << Color::RESET << std::endl;
        for (const auto& region : regions) {
            oss << "  Region " << region.id << " (" << region.name << "):";
            oss << std::string(4 - std::to_string(region.id).length(), ' ') << " ";
            
            // Draw activation bar
            int barWidth = std::min<size_t>(20, width / 2);
            int filled = static_cast<int>(region.activation * barWidth);
            oss << Color::GREEN;
            for (int i = 0; i < filled; ++i) oss << "█";
            oss << Color::RED;
            for (int i = filled; i < barWidth; ++i) oss << "░";
            oss << Color::RESET;
            
            oss << " " << region.activation << " (" << region.neuronCount << " neurons)" << std::endl;
        }
        
        oss << std::endl;
        oss << Color::YELLOW << "NETWORK CONNECTIVITY:" << Color::RESET << std::endl;
        
        // Simple connectivity diagram
        for (const auto& synapse : synapses) {
            oss << "  Synapse " << synapse.sourceRegion << " -> " << synapse.targetRegion;
            oss << std::string(4 - std::to_string(synapse.sourceRegion).length(), ' ') << " ";
            oss << std::string(4 - std::to_string(synapse.targetRegion).length(), ' ') << " ";
            
            // Draw weight indicator
            char weightChar = (synapse.weight > 0.8) ? '●' : (synapse.weight > 0.5) ? '○' : '.';
            oss << weightChar << " (" << std::fixed << std::setprecision(2) << synapse.weight << ")" << std::endl;
        }
        
        oss << std::endl;
        oss << Color::YELLOW << "SPIKE ACTIVITY:" << Color::RESET << std::endl;
        oss << "  Recent spikes: " << spikes.size() << std::endl;
        for (size_t i = 0; i < std::min<size_t>(5, spikes.size()); ++i) {
            const auto& spike = spikes[i];
            oss << "    Neuron " << spike.neuronId << " (Region " << spike.regionId << ")"
                << " at " << std::fixed << std::setprecision(3) << spike.timestamp << "s" << std::endl;
        }
        
        if (spikes.size() > 5) {
            oss << "    ... and " << (spikes.size() - 5) << " more spikes" << std::endl;
        }
        
        oss << std::endl;
        oss << Color::YELLOW << "PERFORMANCE METRICS:" << Color::RESET << std::endl;
        // Performance metrics would be updated from real-time data
        oss << "  (Metrics updated from real-time monitoring)" << std::endl;
        
        return oss.str();
    }
};

// Advanced debugging system
class DebugSystem {
public:
    DebugSystem() {
        setupDefaultConfiguration();
    }
    
    ~DebugSystem() = default;
    
    // Initialize debug system
    void initialize(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agentBrain) {
        brain_ = brain;
        agentBrain_ = agentBrain;
        
        // Setup monitoring thread
        startMonitoringThread();
        
        // Setup performance profiler
        setupPerformanceProfiler();
        
        // Setup memory tracker
        setupMemoryTracker();
        
        // Setup network monitor
        setupNetworkMonitor();
        
        debugActive_ = true;
    }
    
    void shutdown() {
        debugActive_ = false;
        
        stopMonitoringThread();
        stopPerformanceProfiler();
        stopMemoryTracker();
        stopNetworkMonitor();
        
        // Save debug session
        saveDebugSession();
        
        brain_.reset();
        agentBrain_.reset();
    }
    
    // Update real-time metrics
    void updateMetrics() {
        if (!brain_) return;
        
        metrics_.timestamp = std::chrono::system_clock::now();
        metrics_.totalNeurons = brain_->getTotalNeuronCount();
        metrics_.totalSynapses = brain_->getTotalSynapseCount();
        metrics_.totalSpikes = brain_->getTotalSpikeCount();
        metrics_.activeNeurons = brain_->getActiveNeuronCount();
        metrics_.firingNeurons = brain_->getFiringNeuronCount();
        metrics_.averageFiringRate = brain_->getAverageFiringRate();
        
        if (agentBrain_) {
            metrics_.dopamineLevel = agentBrain_->getNeuromodulationLevel();
            metrics_.curiosityLevel = agentBrain_->getCuriosityLevel();
            metrics_.noveltyLevel = agentBrain_->getNoveltyLevel();
            metrics_.predictionError = agentBrain_->getPredictionError();
        }
        
        // Update performance metrics
        updatePerformanceMetrics();
        updateMemoryMetrics();
    }
    
    // Get current metrics
    const RealTimeMetrics& getMetrics() const {
        return metrics_;
    }
    
    // Set debug mode
    void setDebugMode(DebugMode mode) {
        currentDebugMode_ = mode;
        logInfo("Debug mode set to: " + debugModeToString(mode));
    }
    
    // Set visualization mode
    void setVisualizationMode(VisualizationMode mode) {
        currentVisualizationMode_ = mode;
        logInfo("Visualization mode set to: " + visualizationModeToString(mode));
    }
    
    // Get visualization data
    NeuralNetworkVisualization getVisualizationData() {
        NeuralNetworkVisualization vis;
        
        if (!brain_) return vis;
        
        // Get brain regions
        auto regions = brain_->getRegions();
        for (size_t i = 0; i < regions.size(); ++i) {
            NeuralNetworkVisualization::RegionData region;
            region.id = i;
            region.name = "Region " + std::to_string(i);
            
            // Get region statistics
            region.neuronCount = regions[i]->getNeuronCount();
            
            // Get average activation
            auto neurons = regions[i]->getAllNeurons();
            float totalActivation = 0.0f;
            for (auto* neuron : neurons) {
                totalActivation += neuron->getMembranePotential();
            }
            region.activation = (neurons.empty()) ? 0.0f : (totalActivation / neurons.size());
            
            // Get neuron activations
            for (auto* neuron : neurons) {
                region.neuronActivations.push_back(neuron->getMembranePotential());
            }
            
            vis.regions.push_back(region);
        }
        
        // Get synapses
        for (const auto& region : regions) {
            auto synapses = region->getSynapses();
            for (const auto& syn : synapses) {
                // In a real implementation, we would track which regions each synapse connects
                // For now, we'll create a simplified visualization
                NeuralNetworkVisualization::SynapseData synData;
                synData.sourceRegion = region->getId();
                synData.targetRegion = region->getId(); // Simplified
                synData.weight = syn->getWeight();
                synData.delay = 1; // Default
                synData.isActive = syn->getWeight() > 0.0f;
                vis.synapses.push_back(synData);
            }
        }
        
        // Get recent spikes
        auto spikeSystem = brain_->getSpikeSystem();
        if (spikeSystem) {
            auto recentSpikes = spikeSystem->getRecentSpikes(100); // Last 100 spikes
            for (const auto& spike : recentSpikes) {
                NeuralNetworkVisualization::SpikeData spikeData;
                spikeData.neuronId = spike.first;
                spikeData.regionId = spike.second;
                spikeData.timestamp = spike.first;
                spikeData.amplitude = 1.0f; // Default amplitude
                vis.spikes.push_back(spikeData);
            }
        }
        
        return vis;
    }
    
    // Generate console visualization
    std::string generateConsoleVisualization() {
        NeuralNetworkVisualization vis = getVisualizationData();
        return vis.generateTextVisualization();
    }
    
    // Generate HTML visualization
    std::string generateHTMLVisualization() {
        std::ostringstream oss;
        
        oss << "<!DOCTYPE html>" << std::endl;
        oss << "<html>" << std::endl;
        oss << "<head>" << std::endl;
        oss << "    <title>NLM Neural Network Visualization</title>" << std::endl;
        oss << "    <style>" << std::endl;
        oss << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
        oss << "        .network { border: 1px solid #ccc; padding: 10px; }" << std::endl;
        oss << "        .region { margin: 10px 0; padding: 10px; border: 1px solid #ddd; }" << std::endl;
        oss << "        .synapse { margin: 5px 0; }" << std::endl;
        oss << "        .spike { margin: 5px 0; color: #666; }" << std::endl;
        oss << "    </style>" << std::endl;
        oss << "</head>" << std::endl;
        oss << "<body>" << std::endl;
        oss << "    <h1>Neural Network Visualization</h1>" << std::endl;
        oss << "    <div class=\"network\">" << std::endl;
        
        // Add region information
        for (const auto& region : getVisualizationData().regions) {
            oss << "        <div class=\"region\">" << std::endl;
            oss << "            <h3>Region " << region.id << " (" << region.name << ")</h3>" << std::endl;
            oss << "            <p>Neurons: " << region.neuronCount << ", Activation: " 
                << std::fixed << std::setprecision(3) << region.activation << "</p>" << std::endl;
            oss << "        </div>" << std::endl;
        }
        
        // Add synapse information
        oss << "        <h3>Synapses</h3>" << std::endl;
        for (const auto& synapse : getVisualizationData().synapses) {
            oss << "        <div class=\"synapse\">" << std::endl;
            oss << "            Synapse " << synapse.sourceRegion << " -> " << synapse.targetRegion
                << ", Weight: " << std::fixed << std::setprecision(3) << synapse.weight << "";
            if (synapse.isActive) oss << " (Active)";
            oss << "</div>" << std::endl;
        }
        
        // Add spike information
        oss << "        <h3>Recent Spikes</h3>" << std::endl;
        for (const auto& spike : getVisualizationData().spikes) {
            oss << "        <div class=\"spike\">" << std::endl;
            oss << "            Neuron " << spike.neuronId << " (Region " << spike.regionId << ")"
                << " at time " << std::fixed << std::setprecision(3) << spike.timestamp
                << ", Amplitude: " << spike.amplitude << "" << std::endl;
            oss << "        </div>" << std::endl;
        }
        
        oss << "    </div>" << std::endl;
        oss << "</body>" << std::endl;
        oss << "</html>" << std::endl;
        
        return oss.str();
    }
    
    // Generate JSON data
    std::string generateJSONData() {
        std::ostringstream oss;
        
        oss << "{" << std::endl;
        oss << "  \"timestamp\": \"" << std::chrono::system_clock::to_time_t(metrics_.timestamp) << "\"," << std::endl;
        oss << "  \"metrics\": {" << std::endl;
        oss << "    \"total_neurons\": " << metrics_.totalNeurons << "," << std::endl;
        oss << "    \"total_synapses\": " << metrics_.totalSynapses << "," << std::endl;
        oss << "    \"total_spikes\": " << metrics_.totalSpikes << "," << std::endl;
        oss << "    \"active_neurons\": " << metrics_.activeNeurons << "," << std::endl;
        oss << "    \"firing_neurons\": " << metrics_.firingNeurons << "," << std::endl;
        oss << "    \"average_firing_rate\": " << metrics_.averageFiringRate << "," << std::endl;
        oss << "    \"dopamine_level\": " << metrics_.dopamineLevel << "," << std::endl;
        oss << "    \"curiosity_level\": " << metrics_.curiosityLevel << "," << std::endl;
        oss << "    \"novelty_level\": " << metrics_.noveltyLevel << "," << std::endl;
        oss << "    \"prediction_error\": " << metrics_.predictionError << "," << std::endl;
        oss << "    \"episode_reward\": " << metrics_.episodeReward << "," << std::endl;
        oss << "    \"steps_completed\": " << metrics_.stepsCompleted << "," << std::endl;
        oss << "    \"episode_active\": " << (metrics_.episodeActive ? "true" : "false") << std::endl;
        oss << "  }," << std::endl;
        oss << "  \"debug_info\": {" << std::endl;
        oss << "    \"debug_mode\": \"" << debugModeToString(currentDebugMode_) << "\"," << std::endl;
        oss << "    \"visualization_mode\": \"" << visualizationModeToString(currentVisualizationMode_) << "\"," << std::endl;
        oss << "    \"error_count\": " << metrics_.errorCount << "," << std::endl;
        oss << "    \"warning_count\": " << metrics_.warningCount << "," << std::endl;
        oss << "    \"info_count\": " << metrics_.infoCount << std::endl;
        oss << "  }" << std::endl;
        oss << "}" << std::endl;
        
        return oss.str();
    }
    
    // Generate CSV data
    std::string generateCSVData() {
        std::ostringstream oss;
        
        oss << "timestamp,total_neurons,total_synapses,total_spikes,active_neurons,firing_neurons,average_firing_rate,dopamine_level,curiosity_level,novelty_level,prediction_error,episode_reward,steps_completed,episode_active" << std::endl;
        
        // Convert timestamp to readable format
        auto time_t = std::chrono::system_clock::to_time_t(metrics_.timestamp);
        std::tm tm_buf;
        std::ostringstream timeStream;
        
        #ifdef _WIN32
        gmtime_s(&tm_buf, &time_t);
        #else
        gmtime_r(&time_t, &tm_buf);
        #endif
        
        timeStream << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        
        oss << timeStream.str() << ",";
        oss << metrics_.totalNeurons << ",";
        oss << metrics_.totalSynapses << ",";
        oss << metrics_.totalSpikes << ",";
        oss << metrics_.activeNeurons << ",";
        oss << metrics_.firingNeurons << ",";
        oss << metrics_.averageFiringRate << ",";
        oss << metrics_.dopamineLevel << ",";
        oss << metrics_.curiosityLevel << ",";
        oss << metrics_.noveltyLevel << ",";
        oss << metrics_.predictionError << ",";
        oss << metrics_.episodeReward << ",";
        oss << metrics_.stepsCompleted << ",";
        oss << (metrics_.episodeActive ? "true" : "false") << std::endl;
        
        return oss.str();
    }
    
    // Export visualization to file
    bool exportVisualization(const std::string& filename, VisualizationMode format) {
        std::string data;
        std::string extension;
        
        switch (format) {
            case VisualizationMode::HTML:
                data = generateHTMLVisualization();
                extension = ".html";
                break;
            case VisualizationMode::JSON:
                data = generateJSONData();
                extension = ".json";
                break;
            case VisualizationMode::CSV:
                data = generateCSVData();
                extension = ".csv";
                break;
            case VisualizationMode::DETAILED:
                data = generateConsoleVisualization();
                extension = ".txt";
                break;
            default:
                logWarning("Unsupported visualization format: " + std::to_string(static_cast<int>(format)));
                return false;
        }
        
        std::ofstream file(filename + extension);
        if (!file.is_open()) {
            logError("Failed to export visualization to: " + filename + extension);
            return false;
        }
        
        file << data;
        file.close();
        
        logSuccess("Exported visualization to: " + filename + extension);
        return true;
    }
    
    // Log message with appropriate level
    void logInfo(const std::string& message) {
        metrics_.infoCount++;
        std::cout << Color::BLUE << "[INFO] " << Color::RESET << message << std::endl;
    }
    
    void logWarning(const std::string& message) {
        metrics_.warningCount++;
        std::cout << Color::YELLOW << "[WARN] " << Color::RESET << message << std::endl;
    }
    
    void logError(const std::string& message) {
        metrics_.errorCount++;
        std::cerr << Color::RED << "[ERROR] " << Color::RESET << message << std::endl;
    }
    
    // Check if debugging is active
    bool isDebugActive() const {
        return debugActive_ && currentDebugMode_ != DebugMode::NONE;
    }
    
    // Get current debug mode
    DebugMode getDebugMode() const {
        return currentDebugMode_;
    }
    
    // Get current visualization mode
    VisualizationMode getVisualizationMode() const {
        return currentVisualizationMode_;
    }
    
    // Save debug session
    bool saveDebugSession() {
        std::string filename = "debug_session_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".json";
        std::string data = generateJSONData();
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            logError("Failed to save debug session to: " + filename);
            return false;
        }
        
        file << data;
        file.close();
        
        logSuccess("Debug session saved to: " + filename);
        return true;
    }
    
    // Load debug session
    bool loadDebugSession(const std::string& filename) {
        // In a real implementation, this would load debug session data
        // For now, we'll just acknowledge the request
        logInfo("Loading debug session from: " + filename);
        
        // Check if file exists
        std::ifstream file(filename);
        if (!file.is_open()) {
            logError("Failed to load debug session from: " + filename);
            return false;
        }
        
        file.close();
        
        logSuccess("Debug session loaded from: " + filename);
        return true;
    }
    
    // Get debug session list
    std::vector<std::string> getDebugSessionList() {
        // In a real implementation, this would scan for debug session files
        // For now, we'll return an empty list
        return std::vector<std::string>();
    }
    
    // Setup default configuration
    void setupDefaultConfiguration() {
        currentDebugMode_ = DebugMode::BASIC;
        currentVisualizationMode_ = VisualizationMode::CONSOLA;
        
        // Setup default visualization parameters
        visualizationWidth_ = 80;
        visualizationHeight_ = 40;
        enableRealTimeUpdates_ = true;
        updateInterval_ = std::chrono::milliseconds(1000); // 1 second
        
        // Setup default debug parameters
        enableMemoryTracking_ = true;
        enablePerformanceTracking_ = true;
        enableNetworkTracking_ = false;
        
        // Setup default logging
        logFile_ = "debug.log";
        enableLogFile_ = false;
        maxLogFileSize_ = 10 * 1024 * 1024; // 10 MB
    }
    
    // Start monitoring thread
    void startMonitoringThread() {
        monitoringActive_ = true;
        monitoringThread_ = std::thread(&DebugSystem::monitoringLoop, this);
    }
    
    // Stop monitoring thread
    void stopMonitoringThread() {
        monitoringActive_ = false;
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
    }
    
    // Monitoring loop
    void monitoringLoop() {
        while (monitoringActive_ && debugActive_) {
            updateMetrics();
            
            // Sleep for update interval
            std::this_thread::sleep_for(updateInterval_);
        }
    }
    
    // Setup performance profiler
    void setupPerformanceProfiler() {
        // In a real implementation, this would setup a performance profiler
        // For now, we'll just acknowledge the setup
        performanceProfilerActive_ = true;
    }
    
    // Stop performance profiler
    void stopPerformanceProfiler() {
        performanceProfilerActive_ = false;
    }
    
    // Update performance metrics
    void updatePerformanceMetrics() {
        // In a real implementation, this would update performance metrics
        // For now, we'll just acknowledge the update
        metrics_.simulationTime = 0.0; // Placeholder
        metrics_.cpuUsage = 0.0; // Placeholder
        metrics_.memoryUsage = 0.0; // Placeholder
    }
    
    // Setup memory tracker
    void setupMemoryTracker() {
        // In a real implementation, this would setup memory tracking
        // For now, we'll just acknowledge the setup
        memoryTrackerActive_ = true;
    }
    
    // Stop memory tracker
    void stopMemoryTracker() {
        memoryTrackerActive_ = false;
    }
    
    // Update memory metrics
    void updateMemoryMetrics() {
        // In a real implementation, this would update memory metrics
        // For now, we'll just acknowledge the update
        // metrics_.memoryUsage = getMemoryUsage(); // Placeholder
    }
    
    // Setup network monitor
    void setupNetworkMonitor() {
        // In a real implementation, this would setup network monitoring
        // For now, we'll just acknowledge the setup
        networkMonitorActive_ = false; // Disabled by default
    }
    
    // Stop network monitor
    void stopNetworkMonitor() {
        networkMonitorActive_ = false;
    }
    
    // Convert debug mode to string
    std::string debugModeToString(DebugMode mode) const {
        switch (mode) {
            case DebugMode::NONE: return "NONE";
            case DebugMode::BASIC: return "BASIC";
            case DebugMode::DETAILED: return "DETAILED";
            case DebugMode::VERBOSE: return "VERBOSE";
            case DebugMode::NETWORK: return "NETWORK";
            case DebugMode::PERFORMANCE: return "PERFORMANCE";
            case DebugMode::MEMORY: return "MEMORY";
            case DebugMode::THREAD: return "THREAD";
            case DebugMode::PLOTTING: return "PLOTTING";
            default: return "UNKNOWN";
        }
    }
    
    // Convert visualization mode to string
    std::string visualizationModeToString(VisualizationMode mode) const {
        switch (mode) {
            case VisualizationMode::CONSOLA: return "CONSOLA";
            case VisualizationMode::TERMINAL: return "TERMINAL";
            case VisualizationMode::HTML: return "HTML";
            case VisualizationMode::JSON: return "JSON";
            case VisualizationMode::CSV: return "CSV";
            case VisualizationMode::BINARY: return "BINARY";
            case VisualizationMode::COMPACT: return "COMPACT";
            case VisualizationMode::DETAILED: return "DETAILED";
            default: return "UNKNOWN";
        }
    }
    
private:
    // Internal state
    bool debugActive_ = false;
    DebugMode currentDebugMode_ = DebugMode::BASIC;
    VisualizationMode currentVisualizationMode_ = VisualizationMode::CONSOLA;
    
    // Monitoring state
    bool monitoringActive_ = false;
    std::thread monitoringThread_;
    std::chrono::milliseconds updateInterval_ = std::chrono::milliseconds(1000);
    
    // Performance state
    bool performanceProfilerActive_ = false;
    
    // Memory state
    bool memoryTrackerActive_ = false;
    
    // Network state
    bool networkMonitorActive_ = false;
    
    // Metrics
    RealTimeMetrics metrics_;
    
    // Visualization parameters
    size_t visualizationWidth_ = 80;
    size_t visualizationHeight_ = 40;
    bool enableRealTimeUpdates_ = true;
    
    // Logging parameters
    std::string logFile_;
    bool enableLogFile_ = false;
    size_t maxLogFileSize_ = 10 * 1024 * 1024;
};

} // namespace nlm
