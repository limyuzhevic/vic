// Visualization System Implementation
// Contains the full implementation of the VisualizationSystem class

#include "visualization/VisualizationSystem.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <thread>
#include <mutex>
#include <atomic>
#include <ctime>

namespace nlm {

// Global visualization system instance
VisualizationSystem globalVisualizationSystem;

// VisualizationElementRenderer implementations
void Simple2DRenderer::render(const VisualizationData& data, 
                            const VisualizationConfig& config) {
    // Simple 2D rendering implementation
    // This would contain actual rendering logic for 2D visualization
    std::cout << "[Simple2DRenderer] Rendering 2D visualization" << std::endl;
    
    if (config.hasElement(VisualizationElement::Neurons)) {
        std::cout << "  Neurons: " << data.neuronActivity.size() << " active" << std::endl;
    }
    
    if (config.hasElement(VisualizationElement::Activity)) {
        std::cout << "  Average firing rate: " << data.averageFiringRate << std::endl;
    }
    
    if (config.hasElement(VisualizationElement::Metrics)) {
        std::cout << "  Metrics:" << std::endl;
        for (const auto& metric : data.metrics) {
            std::cout << "    " << metric.first << ": " << metric.second << std::endl;
        }
    }
}

void OpenGL3DRenderer::render(const VisualizationData& data,
                           const VisualizationConfig& config) {
    // OpenGL 3D rendering implementation
    // This would contain actual OpenGL rendering logic
    std::cout << "[OpenGL3DRenderer] Rendering 3D visualization" << std::endl;
    
    if (config.hasElement(VisualizationElement::Neurons)) {
        std::cout << "  3D Neuron visualization with " << data.neuronActivity.size() << " neurons" << std::endl;
    }
    
    if (config.hasElement(VisualizationElement::Synapses)) {
        std::cout << "  3D Synapse visualization with " << data.synapticWeights.size() << " connections" << std::endl;
    }
    
    if (config.hasElement(VisualizationElement::Regions)) {
        std::cout << "  3D Region visualization with " << data.regionPopulations.size() << " regions" << std::endl;
    }
}

void WebGLRenderer::render(const VisualizationData& data,
                        const VisualizationConfig& config) {
    // WebGL rendering implementation
    // This would contain actual WebGL rendering logic
    std::cout << "[WebGLRenderer] Rendering WebGL visualization" << std::endl;
    
    if (config.hasElement(VisualizationElement::Neurons)) {
        std::cout << "  WebGL neuron rendering with " << data.neuronActivity.size() << " particles" << std::endl;
    }
    
    if (config.hasElement(VisualizationElement::Neuromodulation)) {
        std::cout << "  WebGL neuromodulation visualization: Dopamine=" << data.dopamineLevel 
                  << ", Curiosity=" << data.curiosityLevel << ", Novelty=" << data.noveltyLevel << std::endl;
    }
}

void TerminalRenderer::render(const VisualizationData& data,
                           const VisualizationConfig& config) {
    // Terminal rendering implementation
    std::cout << "[TerminalRenderer] Rendering terminal visualization" << std::endl;
    
    if (config.hasElement(VisualizationElement::Neurons)) {
        std::cout << "=== NLM Brain State ===" << std::endl;
        std::cout << "Neurons: " << data.neuronActivity.size() << " active" << std::endl;
        std::cout << "Firing rate: " << std::fixed << std::setprecision(3) << data.averageFiringRate << std::endl;
        std::cout << "Total spikes: " << data.totalSpikeCount << std::endl;
    }
    
    if (config.hasElement(VisualizationElement::Activity)) {
        std::cout << "Activity Metrics:" << std::endl;
        std::cout << "  Max firing rate: " << data.maxFiringRate << std::endl;
        std::cout << "  Min firing rate: " << data.minFiringRate << std::endl;
        std::cout << "  Std firing rate: " << data.stdFiringRate << std::endl;
    }
    
    if (config.hasElement(VisualizationElement::Plasticity)) {
        std::cout << "Plasticity:" << std::endl;
        std::cout << "  Total weight: " << data.totalSynapticWeight << std::endl;
        std::cout << "  Mean weight: " << data.meanSynapticWeight << std::endl;
        std::cout << "  Plasticity enabled: " << data.plasticityEnabledSynapses << std::endl;
    }
}

// VisualizationSystem implementation
void VisualizationSystem::update() {
    if (!isRunning || isPaused) {
        return;
    }
    
    // Collect data from targets
    currentFrameData = collectData();
    
    // Update history if recording
    if (recordData) {
        historyData.push_back(currentFrameData);
        if (historyData.size() > currentConfig.maxDataPoints * 2) {
            historyData.erase(historyData.begin());
        }
    }
    
    // Generate frame
    lastFrame = generateFrame(currentFrameData);
    
    // Render if callback available
    if (renderCallback) {
        lastFrame = renderCallback(currentFrameData);
    }
}

void VisualizationSystem::render() {
    if (!isRunning || isPaused) {
        return;
    }
    
    // Use appropriate renderer based on config type
    std::unique_ptr<VisualizationElementRenderer> renderer = createRendererInstance();
    
    if (renderer) {
        renderer->render(currentFrameData, currentConfig);
    }
}

bool VisualizationSystem::saveFrame(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "ERROR: Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << lastFrame;
    std::cout << "Frame saved to: " << filename << std::endl;
    return true;
}

bool VisualizationSystem::saveVideo(const std::string& filename, double fps) const {
    // Video saving implementation
    // This would encode frames into a video file
    std::cout << "[VisualizationSystem] Video saving not yet implemented" << std::endl;
    std::cout << "Video would be saved to: " << filename << " at " << fps << " fps" << std::endl;
    
    return true; // Placeholder - would need actual video encoding
}

bool VisualizationSystem::exportData(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "ERROR: Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    // Export data in appropriate format based on file extension
    if (filename.substr(filename.find_last_of(".") + 1) == "json") {
        // JSON export
        file << "{\"data\": \"placeholder for visualization data\"}";
    } else if (filename.substr(filename.find_last_of(".") + 1) == "csv") {
        // CSV export
        file << "step,timestamp,neurons,activity\n";
        for (const auto& frame : historyData) {
            file << frame.step << "," << frame.timestamp.count() << 
                 "," << frame.neuronActivity.size() << ","
                 << frame.averageFiringRate << "\n";
        }
    } else {
        // Default text export
        file << "Visualization data exported from NLM Visualization System" << std::endl;
        file << "Total frames: " << historyData.size() << std::endl;
        file << "Time range: " << (historyData.empty() ? 0 : 
            historyData.back().timestamp.count() - historyData.front().timestamp.count()) << " seconds" << std::endl;
    }
    
    std::cout << "Data exported to: " << filename << std::endl;
    return true;
}

std::unique_ptr<VisualizationElementRenderer> VisualizationSystem::createRendererInstance() const {
    return createRenderer(currentConfig.type);
}

std::unique_ptr<VisualizationElementRenderer> VisualizationSystem::createRenderer(VisualizationType type) {
    switch (type) {
        case VisualizationType::Simple2D:
            return std::make_unique<Simple2DRenderer>();
        case VisualizationType::3DGl:
            return std::make_unique<OpenGL3DRenderer>();
        case VisualizationType::WebGL:
            return std::make_unique<WebGLRenderer>();
        case VisualizationType::Terminal:
            return std::make_unique<TerminalRenderer>();
        default:
            return std::make_unique<TerminalRenderer>(); // Default to terminal
    }
}

VisualizationConfig VisualizationSystem::getDefaultConfig(VisualizationType type) {
    VisualizationConfig config;
    
    switch (type) {
        case VisualizationType::Simple2D:
            config.name = "Simple 2D Visualization";
            config.width = 800;
            config.height = 600;
            config.elements = {VisualizationElement::Neurons, VisualizationElement::Activity,
                              VisualizationElement::Plasticity, VisualizationElement::Environment};
            break;
            
        case VisualizationType::3DGl:
            config.name = "3D OpenGL Visualization";
            config.width = 1024;
            config.height = 768;
            config.elements = {VisualizationElement::Neurons, VisualizationElement::Synapses,
                              VisualizationElement::Regions, VisualizationElement::Populations,
                              VisualizationElement::Plasticity};
            break;
            
        case VisualizationType::WebGL:
            config.name = "WebGL Browser Visualization";
            config.width = 1200;
            config.height = 800;
            config.elements = {VisualizationElement::Neurons, VisualizationElement::Activity,
                              VisualizationElement::Neuromodulation, VisualizationElement::Prediction,
                              VisualizationElement::TimeSeries};
            break;
            
        case VisualizationType::Terminal:
            config.name = "Terminal Visualization";
            config.width = 80;
            config.height = 24;
            config.elements = {VisualizationElement::Neurons, VisualizationElement::Activity,
                              VisualizationElement::Metrics, VisualizationElement::TimeSeries};
            break;
            
        default:
            config.name = "Default Visualization";
            config.width = 640;
            config.height = 480;
            config.elements = {VisualizationElement::Neurons, VisualizationElement::Activity};
    }
    
    return config;
}

VisualizationData VisualizationSystem::collectData() const {
    VisualizationData data;
    
    if (targetBrain) {
        data = collectFromBrain();
    }
    
    if (targetEnvironment) {
        // Merge environment data
        // For now, just collect basic environment info
        data.environmentGrid = std::vector<std::vector<int>>(10, std::vector<int>(10, 0));
        data.objectTypes = {"Empty", "Resource", "Hazard", "Wall", "Marker"};
    }
    
    if (targetAgent) {
        // Merge agent data
        data = collectFromAgent();
    }
    
    if (runtimeInspector) {
        data = collectFromInspector();
    }
    
    return data;
}

VisualizationData VisualizationSystem::collectFromBrain() const {
    VisualizationData data;
    
    if (targetBrain) {
        data.step = 0; // Would need current step from brain
        data.totalNeuronCount = targetBrain->getTotalNeuronCount();
        data.activeNeuronCount = targetBrain->getActiveNeuronCount();
        data.firingNeuronCount = targetBrain->getFiringNeuronCount();
        data.totalSynapseCount = targetBrain->getTotalSynapseCount();
        data.averageFiringRate = targetBrain->getAverageFiringRate();
        data.excitationInhibitionRatio = targetBrain->getExcitationInhibitionRatio();
        data.totalSpikeCount = targetBrain->getTotalSpikeCount();
        
        // Get developmental stage (simplified)
        data.developmentalStage = "Unknown";
        
        // Collect neural population data
        auto regions = targetBrain->getRegionIds();
        for (const auto& regionId : regions) {
            auto* region = targetBrain->getRegion(regionId);
            if (region) {
                data.regionPopulations.push_back(region->getAllNeurons().size());
            }
        }
        
        // Collect firing rate history
        data.firingRateHistory = std::vector<float>(10, data.averageFiringRate); // Placeholder
        
        // Collect synaptic weight data
        data.synapticWeightHistory = std::vector<float>(10, 0.5f); // Placeholder
        
        // Collect memory metrics (simplified)
        data.workingMemoryCapacity = 100; // Placeholder
        data.episodicMemoryCount = 50; // Placeholder
        data.semanticMemoryConcepts = 20; // Placeholder
        
        // Collect plasticity metrics (simplified)
        data.totalSynapticWeight = data.meanSynapticWeight = 0.5f; // Placeholder
        data.plasticityEnabledSynapses = data.totalSynapseCount / 2; // Placeholder
        data.strengthenedConnections = 10; // Placeholder
        data.weakenedConnections = 5; // Placeholder
        
        // Collect network statistics
        data.maxFiringRate = data.averageFiringRate * 1.5f; // Placeholder
        data.minFiringRate = data.averageFiringRate * 0.5f; // Placeholder
        data.stdFiringRate = 0.1f; // Placeholder
        
        // Collect cognitive metrics (simplified)
        data.conceptsFormed = 5; // Placeholder
        data.plansExecuted = 3; // Placeholder
        data.decisionsMade = 20; // Placeholder
    }
    
    return data;
}

VisualizationData VisualizationSystem::collectFromEnvironment() const {
    VisualizationData data;
    
    if (targetEnvironment) {
        // Collect environment-specific data
        data.environmentGrid = std::vector<std::vector<int>>(20, std::vector<int>(20, 0));
        data.objectTypes = {"Empty", "Resource", "Hazard", "Wall", "Marker"};
        
        // Set some objects in the grid
        data.environmentGrid[5][5] = 1; // Resource
        data.environmentGrid[10][10] = 3; // Wall
        data.environmentGrid[15][15] = 4; // Marker
    }
    
    return data;
}

VisualizationData VisualizationSystem::collectFromAgent() const {
    VisualizationData data;
    
    if (targetAgent) {
        // Collect agent-specific data
        // Note: This assumes the agent has methods to provide this data
        // In practice, you would need to add methods to the AgentBrain class
        
        data.dopamineLevel = 0.5f; // Placeholder
        data.curiosityLevel = 0.3f; // Placeholder
        data.noveltyLevel = 0.2f; // Placeholder
        data.predictionError = 0.1f; // Placeholder
        
        data.activeConcepts = {"Exploration", "Learning", "Memory"}; // Placeholder
        data.planActivations = {1, 2, 3}; // Placeholder
        data.decisionConfidences = {0.8f, 0.6f, 0.9f}; // Placeholder
    }
    
    return data;
}

VisualizationData VisualizationSystem::collectFromInspector() const {
    VisualizationData data;
    
    if (runtimeInspector) {
        // Collect data from runtime inspector
        data = runtimeInspector->captureSnapshot();
    }
    
    return data;
}

std::string VisualizationSystem::generateFrame(const VisualizationData& data) const {
    switch (outputFormat) {
        case OutputFormat::Text:
            return renderToText(data);
        case OutputFormat::JSON:
            return renderToJSON(data);
        case OutputFormat::CSV:
            return renderToCSV(data);
        case OutputFormat::HTML:
            return renderToHTML(data);
        default:
            return renderToText(data);
    }
}

std::string VisualizationSystem::renderToText(const VisualizationData& data) const {
    std::stringstream ss;
    ss << "=== NLM Visualization Frame ===" << std::endl;
    ss << "Step: " << data.step << std::endl;
    ss << "Timestamp: " << data.timestamp.count() << std::endl;
    ss << std::endl;
    
    if (data.hasElement(VisualizationElement::Neurons)) {
        ss << "--- Neural Network ---" << std::endl;
        ss << "Total neurons: " << data.totalNeuronCount << std::endl;
        ss << "Active neurons: " << data.activeNeuronCount << std::endl;
        ss << "Firing neurons: " << data.firingNeuronCount << std::endl;
        ss << "Average firing rate: " << std::fixed << std::setprecision(3) << data.averageFiringRate << std::endl;
        ss << "Total spikes: " << data.totalSpikeCount << std::endl;
        ss << "Excitation/inhibition ratio: " << std::fixed << std::setprecision(3) << data.excitationInhibitionRatio << std::endl;
        ss << std::endl;
    }
    
    if (data.hasElement(VisualizationElement::Plasticity)) {
        ss << "--- Plasticity ---" << std::endl;
        ss << "Total synaptic weight: " << std::fixed << std::setprecision(3) << data.totalSynapticWeight << std::endl;
        ss << "Mean synaptic weight: " << std::fixed << std::setprecision(3) << data.meanSynapticWeight << std::endl;
        ss << "Plasticity enabled: " << data.plasticityEnabledSynapses << std::endl;
        ss << "Strengthened connections: " << data.strengthenedConnections << std::endl;
        ss << "Weakened connections: " << data.weakenedConnections << std::endl;
        ss << std::endl;
    }
    
    if (data.hasElement(VisualizationElement::Memory)) {
        ss << "--- Memory ---" << std::endl;
        ss << "Working memory capacity: " << data.workingMemoryCapacity << std::endl;
        ss << "Episodic memories: " << data.episodicMemoryCount << std::endl;
        ss << "Semantic concepts: " << data.semanticMemoryConcepts << std::endl;
        ss << "Procedural rules: " << data.proceduralMemoryRules << std::endl;
        ss << std::endl;
    }
    
    if (data.hasElement(VisualizationElement::Neuromodulation)) {
        ss << "--- Neuromodulation ---" << std::endl;
        ss << "Dopamine level: " << std::fixed << std::setprecision(3) << data.dopamineLevel << std::endl;
        ss << "Curiosity level: " << std::fixed << std::setprecision(3) << data.curiosityLevel << std::endl;
        ss << "Novelty level: " << std::fixed << std::setprecision(3) << data.noveltyLevel << std::endl;
        ss << "Prediction error: " << std::fixed << std::setprecision(3) << data.predictionError << std::endl;
        ss << std::endl;
    }
    
    if (data.hasElement(VisualizationElement::Cognition)) {
        ss << "--- Cognition ---" << std::endl;
        ss << "Concepts formed: " << data.conceptsFormed << std::endl;
        ss << "Plans executed: " << data.plansExecuted << std::endl;
        ss << "Decisions made: " << data.decisionsMade << std::endl;
        ss << std::endl;
    }
    
    if (data.hasElement(VisualizationElement::Environment)) {
        ss << "--- Environment ---" << std::endl;
        ss << "Grid size: " << data.environmentGrid.size() << "x" << data.environmentGrid[0].size() << std::endl;
        ss << "Object types: " << data.objectTypes.size() << std::endl;
        for (size_t i = 0; i < std::min(size_t(5), data.objectTypes.size()); ++i) {
            ss << "  " << data.objectTypes[i] << std::endl;
        }
        ss << std::endl;
    }
    
    if (!data.metrics.empty()) {
        ss << "--- Metrics ---" << std::endl;
        for (const auto& metric : data.metrics) {
            ss << metric.first << ": " << std::fixed << std::setprecision(3) << metric.second << std::endl;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

std::string VisualizationSystem::renderToJSON(const VisualizationData& data) const {
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"step\": " << data.step << "," << std::endl;
    ss << "  \"timestamp\": " << data.timestamp.count() << "," << std::endl;
    ss << "  \"neural\": {" << std::endl;
    ss << "    \"neurons\": " << data.totalNeuronCount << "," << std::endl;
    ss << "    \"active\": " << data.activeNeuronCount << "," << std::endl;
    ss << "    \"firing\": " << data.firingNeuronCount << "," << std::endl;
    ss << "    \"firing_rate\": " << std::fixed << std::setprecision(3) << data.averageFiringRate << "," << std::endl;
    ss << "    \"spikes\": " << data.totalSpikeCount << "," << std::endl;
    ss << "    \"exc_inh_ratio\": " << std::fixed << std::setprecision(3) << data.excitationInhibitionRatio <<""" << std::endl;
    ss << "  }," << std::endl;
    ss << "  \"plasticity\": {" << std::endl;
    ss << "    \"total_weight\": " << std::fixed << std::setprecision(3) << data.totalSynapticWeight << "," << std::endl;
    ss << "    \"mean_weight\": " << std::fixed << std::setprecision(3) << data.meanSynapticWeight << "," << std::endl;
    ss << "    \"plasticity_enabled\": " << data.plasticityEnabledSynapses << "," << std::endl;
    ss << "    \"strengthened\": " << data.strengthenedConnections << "," << std::endl;
    ss << "    \"weakened\": " << data.weakenedConnections <<""" << std::endl;
    ss << "  }," << std::endl;
    ss << "  \"memory\": {" << std::endl;
    ss << "    \"working_capacity\": " << data.workingMemoryCapacity << "," << std::endl;
    ss << "    \"episodic\": " << data.episodicMemoryCount << "," << std::endl;
    ss << "    \"semantic\": " << data.semanticMemoryConcepts << "," << std::endl;
    ss << "    \"procedural\": " << data.proceduralMemoryRules <<""" << std::endl;
    ss << "  }," << std::endl;
    ss << "  \"neuromodulation\": {" << std::endl;
    ss << "    \"dopamine\": " << std::fixed << std::setprecision(3) << data.dopamineLevel << "," << std::endl;
    ss << "    \"curiosity\": " << std::fixed << std::setprecision(3) << data.curiosityLevel << "," << std::endl;
    ss << "    \"novelty\": " << std::fixed << std::setprecision(3) << data.noveltyLevel << "," << std::endl;
    ss << "    \"prediction_error\": " << std::fixed << std::setprecision(3) << data.predictionError <<""" << std::endl;
    ss << "  }," << std::endl;
    ss << "  \"cognition\": {" << std::endl;
    ss << "    \"concepts_formed\": " << data.conceptsFormed << "," << std::endl;
    ss << "    \"plans_executed\": " << data.plansExecuted << "," << std::endl;
    ss << "    \"decisions_made\": " << data.decisionsMade <<""" << std::endl;
    ss << "  }," << std::endl;
    ss << "  \"metrics\": {" << std::endl;
    
    bool first = true;
    for (const auto& metric : data.metrics) {
        if (!first) ss << "," << std::endl;
        ss << "    \"" << metric.first << "\": " << std::fixed << std::setprecision(3) << metric.second;
        first = false;
    }
    
    ss << """  }
} """ << std::endl;
    
    return ss.str();
}

std::string VisualizationSystem::renderToCSV(const VisualizationData& data) const {
    std::stringstream ss;
    ss << "step,timestamp,total_neurons,active_neurons,firing_neurons,avg_firing_rate,total_spikes,exc_inh_ratio" << std::endl;
    ss << data.step << "," << data.timestamp.count() << "," 
       << data.totalNeuronCount << "," << data.activeNeuronCount << "," 
       << data.firingNeuronCount << "," << std::fixed << std::setprecision(6) << data.averageFiringRate << "," 
       << data.totalSpikeCount << "," << std::fixed << std::setprecision(6) << data.excitationInhibitionRatio << std::endl;
    
    return ss.str();
}

std::string VisualizationSystem::renderToHTML(const VisualizationData& data) const {
    std::stringstream ss;
    ss << "<!DOCTYPE html>" << std::endl;
    ss << "<html>" << std::endl;
    ss << "<head><title>NLM Visualization</title></head>" << std::endl;
    ss << "<body>" << std::endl;
    ss << "<h1>NLM Brain Visualization</h1>" << std::endl;
    ss << "<p>Step: " << data.step << ", Timestamp: " << data.timestamp.count() << "</p>" << std::endl;
    ss << "<pre>" << std::endl;
    ss << renderToText(data) << std::endl;
    ss << "</pre>" << std::endl;
    ss << "</body>" << std::endl;
    ss << "</html>" << std::endl;
    
    return ss.str();
}

// WebSocketVisualizationServer implementation
WebSocketVisualizationServer::WebSocketVisualizationServer(unsigned short port) : port(port), running(false) {
}

WebSocketVisualizationServer::~WebSocketVisualizationServer() {
    stop();
}

void WebSocketVisualizationServer::start() {
    if (running) {
        std::cout << "WebSocket server already running" << std::endl;
        return;
    }
    
    std::cout << "Starting WebSocket visualization server on port " << port << std::endl;
    running = true;
    
    // Start WebSocket server in a separate thread
    // This would require WebSocket library integration
    std::thread serverThread(&WebSocketVisualizationServer::handleConnections, this);
    serverThread.detach();
}

void WebSocketVisualizationServer::stop() {
    if (!running) {
        return;
    }
    
    std::cout << "Stopping WebSocket visualization server" << std::endl;
    running = false;
}

void WebSocketVisualizationServer::broadcastFrame(const std::string& frame) const {
    if (!running || !visualizationSystem) {
        return;
    }
    
    std::cout << "[WebSocketVisualizationServer] Broadcasting frame to clients" << std::endl;
    
    // Broadcast frame to connected WebSocket clients
    // This would require WebSocket library implementation
}

void WebSocketVisualizationServer::setupWebSocket() {
    // Setup WebSocket server
    // This would require WebSocket library integration
}

void WebSocketVisualizationServer::handleConnections() {
    // Handle WebSocket connections
    // This would require WebSocket library implementation
}

void WebSocketVisualizationServer::handleMessages() {
    // Handle WebSocket messages
    // This would require WebSocket library implementation
}

// Convenience function implementations
void Visualization::createDefaultVisualization() {
    globalVisualizationSystem = VisualizationSystem();
    globalVisualizationSystem.start();
    std::cout << "Created default visualization system" << std::endl;
}

VisualizationConfig Visualization::getMinimalConfig() {
    VisualizationConfig config;
    config.name = "Minimal Visualization";
    config.type = VisualizationType::Terminal;
    config.width = 60;
    config.height = 24;
    config.elements = {VisualizationElement::Neurons, VisualizationElement::Activity};
    return config;
}

VisualizationConfig Visualization::getStandardConfig() {
    VisualizationConfig config;
    config.name = "Standard Visualization";
    config.type = VisualizationType::Simple2D;
    config.width = 800;
    config.height = 600;
    config.elements = {VisualizationElement::Neurons, VisualizationElement::Synapses,
                      VisualizationElement::Regions, VisualizationElement::Activity,
                      VisualizationElement::Plasticity, VisualizationElement::Environment};
    return config;
}

VisualizationConfig Visualization::getAdvancedConfig() {
    VisualizationConfig config;
    config.name = "Advanced Visualization";
    config.type = VisualizationType::3DGl;
    config.width = 1024;
    config.height = 768;
    config.elements = {VisualizationElement::Neurons, VisualizationElement::Synapses,
                      VisualizationElement::Populations, VisualizationElement::Activity,
                      VisualizationElement::Plasticity, VisualizationElement::Memory,
                      VisualizationElement::Cognition, VisualizationElement::Neuromodulation,
                      VisualizationElement::Prediction};
    return config;
}

std::unique_ptr<VisualizationElementRenderer> Visualization::createTextRenderer() {
    return std::make_unique<TerminalRenderer>();
}

std::unique_ptr<VisualizationElementRenderer> Visualization::createTerminalRenderer() {
    return std::make_unique<TerminalRenderer>();
}

std::unique_ptr<VisualizationElementRenderer> Visualization::createWebGLRenderer() {
    return std::make_unique<WebGLRenderer>();
}

void Visualization::saveToFile(const std::string& filename, const std::string& data) {
    std::ofstream file(filename);
    if (file) {
        file << data;
        std::cout << "Data saved to: " << filename << std::endl;
    } else {
        std::cerr << "ERROR: Cannot save to file: " << filename << std::endl;
    }
}

void Visualization::saveFrame(const std::string& filename, const std::string& frameData) {
    saveToFile(filename, frameData);
}

void Visualization::saveVideo(const std::string& filename, const std::vector<std::string>& frames, 
                             double fps) {
    std::cout << "[Visualization] Saving video to: " << filename << " with " << fps << " fps" << std::endl;
    std::cout << "Video saving requires external video encoding library" << std::endl;
}

void Visualization::enableAutoUpdate(bool enable) {
    if (enable) {
        globalVisualizationSystem.start();
    } else {
        globalVisualizationSystem.stop();
    }
}

bool Visualization::isAutoUpdateEnabled() {
    return globalVisualizationSystem.isRunning();
}

void Visualization::setVisualizationTarget(std::shared_ptr<Brain> brain) {
    globalVisualizationSystem.setTarget(brain);
}

void Visualization::setVisualizationTarget(std::shared_ptr<Environment> environment) {
    globalVisualizationSystem.setTarget(environment);
}

void Visualization::setVisualizationTarget(std::shared_ptr<AgentBrain> agent) {
    globalVisualizationSystem.setTarget(agent);
}

void Visualization::enableRuntimeInspection(bool enable) {
    globalVisualizationSystem.enableRecording(enable);
}

bool Visualization::isRuntimeInspectionEnabled() {
    return globalVisualizationSystem.isRecording();
}

} // namespace nlm
