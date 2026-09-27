// Visualizer implementation

#include "debug/Visualizer.hpp"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

namespace nlm {

Visualizer::Visualizer() : liveMode(false), serverRunning(false), 
                          visualizationPort(8080), updateInterval(1.0),
                          windowWidth(1024), windowHeight(768), 
                          serverThreadRunning(false) {
    loadDefaultConfiguration();
    
    // Initialize renderers
    networkRenderer = std::make_unique<NetworkRenderer>();
    spikesRenderer = std::make_unique<SpikesRenderer>();
    memoryRenderer = std::make_unique<MemoryRenderer>();
    
    // Configure renderers
    networkRenderer->setSize(windowWidth, windowHeight);
    spikesRenderer->setSize(windowWidth, windowHeight);
    memoryRenderer->setSize(windowWidth, windowHeight);
}

void Visualizer::startVisualization(const std::vector<VisualizationTarget>& targets) {
    std::lock_guard<std::mutex> lock(mutex);
    
    enabledTargets = targets;
    
    if (enabledTargets.empty()) {
        enabledTargets = {VisualizationTarget::Network, VisualizationTarget::Spikes};
    }
    
    // Update renderers
    if (std::find(enabledTargets.begin(), enabledTargets.end(), VisualizationTarget::Network) != enabledTargets.end()) {
        networkRenderer->render();
    }
    if (std::find(enabledTargets.begin(), enabledTargets.end(), VisualizationTarget::Spikes) != enabledTargets.end()) {
        spikesRenderer->render();
    }
    if (std::find(enabledTargets.begin(), enabledTargets.end(), VisualizationTarget::Memory) != enabledTargets.end()) {
        memoryRenderer->render();
    }
    
    // Start visualization server if in live mode
    if (liveMode && !serverRunning) {
        startVisualizationServer();
    }
}

void Visualizer::stopVisualization() {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Stop visualization server
    if (serverRunning) {
        stopVisualizationServer();
    }
    
    // Clear data
    currentNetworkNodes.clear();
    currentNetworkEdges.clear();
    currentSpikes.clear();
    currentMemoryNodes.clear();
    currentMemoryConnections.clear();
    currentNeuromodulators.clear();
    currentBehaviors.clear();
    
    enabledTargets.clear();
}

bool Visualizer::isVisualizing() const {
    std::lock_guard<std::mutex> lock(mutex);
    return !enabledTargets.empty();
}

void Visualizer::updateNetworkData(const std::vector<NetworkNode>& nodes, const std::vector<NetworkEdge>& edges) {
    std::lock_guard<std::mutex> lock(mutex);
    
    currentNetworkNodes = nodes;
    currentNetworkEdges = edges;
    
    // Update network renderer
    if (networkRenderer) {
        networkRenderer->setNodePositions(nodes);
        networkRenderer->setEdges(edges);
    }
    
    // Notify listeners
    for (const auto& listener : networkListeners) {
        try {
            listener(nodes, edges);
        } catch (const std::exception& e) {
            std::cerr << "Error in network update listener: " << e.what() << std::endl;
        }
    }
}

void Visualizer::updateSpikesData(const std::vector<SpikeEvent>& spikes) {
    std::lock_guard<std::mutex> lock(mutex);
    
    currentSpikes = spikes;
    
    // Update spikes renderer
    if (spikesRenderer) {
        spikesRenderer->setSpikes(spikes);
    }
    
    // Notify listeners
    for (const auto& listener : spikesListeners) {
        try {
            listener(spikes);
        } catch (const std::exception& e) {
            std::cerr << "Error in spikes update listener: " << e.what() << std::endl;
        }
    }
}

void Visualizer::updateMemoryData(const std::vector<MemoryNode>& nodes, const std::vector<MemoryConnection>& connections) {
    std::lock_guard<std::mutex> lock(mutex);
    
    currentMemoryNodes = nodes;
    currentMemoryConnections = connections;
    
    // Update memory renderer
    if (memoryRenderer) {
        memoryRenderer->setMemoryNodes(nodes);
        memoryRenderer->setMemoryConnections(connections);
    }
    
    // Notify listeners
    for (const auto& listener : memoryListeners) {
        try {
            listener(nodes, connections);
        } catch (const std::exception& e) {
            std::cerr << "Error in memory update listener: " << e.what() << std::endl;
        }
    }
}

void Visualizer::updateNeuromodulatorData(const std::vector<NeuromodulatorNode>& neuromodulators) {
    std::lock_guard<std::mutex> lock(mutex);
    
    currentNeuromodulators = neuromodulators;
    
    // Notify listeners
    for (const auto& listener : neuromodulatorListeners) {
        try {
            listener(neuromodulators);
        } catch (const std::exception& e) {
            std::cerr << "Error in neuromodulator update listener: " << e.what() << std::endl;
        }
    }
}

void Visualizer::updateBehaviorData(const std::vector<BehaviorEvent>& behaviors) {
    std::lock_guard<std::mutex> lock(mutex);
    
    currentBehaviors = behaviors;
    
    // Notify listeners
    for (const auto& listener : behaviorListeners) {
        try {
            listener(behaviors);
        } catch (const std::exception& e) {
            std::cerr << "Error in behavior update listener: " << e.what() << std::endl;
        }
    }
}

void Visualizer::setVisualizationPort(int port) {
    std::lock_guard<std::mutex> lock(mutex);
    visualizationPort = port;
}

void Visualizer::setUpdateInterval(double intervalSeconds) {
    std::lock_guard<std::mutex> lock(mutex);
    updateInterval = intervalSeconds;
}

void Visualizer::setWindowSize(int width, int height) {
    std::lock_guard<std::mutex> lock(mutex);
    windowWidth = width;
    windowHeight = height;
    
    if (networkRenderer) networkRenderer->setSize(width, height);
    if (spikesRenderer) spikesRenderer->setSize(width, height);
    if (memoryRenderer) memoryRenderer->setSize(width, height);
}

void Visualizer::setLayoutAlgorithm(const std::string& algorithm) {
    std::lock_guard<std::mutex> lock(mutex);
    layoutAlgorithm = algorithm;
    if (networkRenderer) {
        networkRenderer->setLayoutAlgorithm(algorithm);
    }
}

std::string Visualizer::exportNetwork(const std::string& format) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (format == "dot") {
        return networkRenderer ? networkRenderer->exportAsDOT() : "";
    } else if (format == "gexf") {
        return networkRenderer ? networkRenderer->exportAsGEXF() : "";
    } else if (format == "graphml") {
        return networkRenderer ? networkRenderer->exportAsGraphML() : "";
    } else {
        return networkRenderer ? networkRenderer->exportAsDOT() : "";
    }
}

std::string Visualizer::exportSpikes(const std::string& format) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (format == "txt") {
        return spikesRenderer ? spikesRenderer->exportAsTXT() : "";
    } else if (format == "csv") {
        return spikesRenderer ? spikesRenderer->exportAsCSV() : "";
    } else {
        return spikesRenderer ? spikesRenderer->exportAsTXT() : "";
    }
}

std::string Visualizer::exportMemory(const std::string& format) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (format == "txt") {
        return memoryRenderer ? memoryRenderer->exportAsTXT() : "";
    } else if (format == "json") {
        return memoryRenderer ? memoryRenderer->exportAsJSON() : "";
    } else {
        return memoryRenderer ? memoryRenderer->exportAsTXT() : "";
    }
}

std::string Visualizer::exportBehavior(const std::string& format) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Behavior export implementation
    std::stringstream ss;
    
    if (format == "txt") {
        for (const auto& behavior : currentBehaviors) {
            ss << "Step: " << behavior.step << "\n";
            ss << "Action: " << behavior.actionType << " - " << behavior.actionDescription << "\n";
            ss << "Confidence: " << behavior.confidence << "\n";
            ss << "Reward: " << behavior.reward << "\n";
            ss << "---\n";
        }
    } else if (format == "csv") {
        ss << "step,action_type,action_description,confidence,reward\n";
        for (const auto& behavior : currentBehaviors) {
            ss << behavior.step << "," << behavior.actionType << "," << behavior.actionDescription
               << "," << behavior.confidence << "," << behavior.reward << "\n";
        }
    }
    
    return ss.str();
}

void Visualizer::enableLiveMode() {
    std::lock_guard<std::mutex> lock(mutex);
    liveMode = true;
    
    if (!serverRunning && !enabledTargets.empty()) {
        startVisualizationServer();
    }
}

void Visualizer::disableLiveMode() {
    std::lock_guard<std::mutex> lock(mutex);
    liveMode = false;
    
    if (serverRunning) {
        stopVisualizationServer();
    }
}

bool Visualizer::isInLiveMode() const {
    std::lock_guard<std::mutex> lock(mutex);
    return liveMode && serverRunning;
}

void Visualizer::addNetworkUpdateListener(const NetworkUpdateListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    networkListeners.push_back(listener);
}

void Visualizer::removeNetworkUpdateListener(const NetworkUpdateListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    networkListeners.erase(std::remove(networkListeners.begin(), networkListeners.end(), listener), networkListeners.end());
}

void Visualizer::addSpikesUpdateListener(const SpikesUpdateListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    spikesListeners.push_back(listener);
}

void Visualizer::removeSpikesUpdateListener(const SpikesUpdateListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    spikesListeners.erase(std::remove(spikesListeners.begin(), spikesListeners.end(), listener), spikesListeners.end());
}

void Visualizer::addMemoryUpdateListener(const MemoryUpdateListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    memoryListeners.push_back(listener);
}

void Visualizer::removeMemoryUpdateListener(const MemoryUpdateListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    memoryListeners.erase(std::remove(memoryListeners.begin(), memoryListeners.end(), listener), memoryListeners.end());
}

void Visualizer::addNeuromodulatorUpdateListener(const NeuromodulatorUpdateListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    neuromodulatorListeners.push_back(listener);
}

void Visualizer::removeNeuromodulatorUpdateListener(const NeuromodulatorUpdateListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    neuromodulatorListeners.erase(std::remove(neuromodulatorListeners.begin(), neuromodulatorListeners.end(), listener), neuromodulatorListeners.end());
}

void Visualizer::addBehaviorUpdateListener(const BehaviorUpdateListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    behaviorListeners.push_back(listener);
}

void Visualizer::removeBehaviorUpdateListener(const BehaviorUpdateListener& listener) {
    std::lock_guard<std::mutex> lock(mutex);
    behaviorListeners.erase(std::remove(behaviorListeners.begin(), behaviorListeners.end(), listener), behaviorListeners.end());
}

void Visualizer::loadConfiguration(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::string filename = filepath.empty() ? "visualizer_config.json" : filepath;
    
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            loadDefaultConfiguration();
            return;
        }
        
        nlohmann::json jsonData = nlohmann::json::parse(file);
        
        visualizationPort = jsonData.value("visualizationPort", 8080);
        updateInterval = jsonData.value("updateInterval", 1.0);
        windowWidth = jsonData.value("windowWidth", 1024);
        windowHeight = jsonData.value("windowHeight", 768);
        layoutAlgorithm = jsonData.value("layoutAlgorithm", "force-directed");
        liveMode = jsonData.value("liveMode", false);
        
        // Load visualization targets
        enabledTargets.clear();
        if (jsonData.contains("enabledTargets")) {
            for (int target : jsonData["enabledTargets"]) {
                enabledTargets.push_back(static_cast<VisualizationTarget>(target));
            }
        }
        
        // Configure renderers
        setWindowSize(windowWidth, windowHeight);
        setLayoutAlgorithm(layoutAlgorithm);
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading visualizer configuration: " << e.what() << std::endl;
        loadDefaultConfiguration();
    }
}

void Visualizer::saveConfiguration(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::string filename = filepath.empty() ? "visualizer_config.json" : filepath;
    
    try {
        nlohmann::json jsonData;
        
        jsonData["visualizationPort"] = visualizationPort;
        jsonData["updateInterval"] = updateInterval;
        jsonData["windowWidth"] = windowWidth;
        jsonData["windowHeight"] = windowHeight;
        jsonData["layoutAlgorithm"] = layoutAlgorithm;
        jsonData["liveMode"] = liveMode;
        
        // Save visualization targets
        std::vector<int> targetsJson;
        for (VisualizationTarget target : enabledTargets) {
            targetsJson.push_back(static_cast<int>(target));
        }
        jsonData["enabledTargets"] = targetsJson;
        
        std::ofstream file(filename);
        if (file.is_open()) {
            file << jsonData.dump(2);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving visualizer configuration: " << e.what() << std::endl;
    }
}

void Visualizer::startVisualizationServer() {
    if (serverRunning) {
        return;
    }
    
    serverRunning = true;
    serverThreadRunning = true;
    serverThread = std::thread(&Visualizer::visualizationServerThread, this);
}

void Visualizer::stopVisualizationServer() {
    serverRunning = false;
    serverThreadRunning = false;
    
    if (serverThread.joinable()) {
        serverThread.join();
    }
}

bool Visualizer::isVisualizationServerRunning() const {
    return serverRunning && serverThreadRunning;
}

void Visualizer::visualizationServerThread() {
    while (serverRunning && serverThreadRunning) {
        try {
            updateAllRenderers();
            notifyListeners();
            std::this_thread::sleep_for(std::chrono::duration<double>(updateInterval));
        } catch (const std::exception& e) {
            std::cerr << "Error in visualization server thread: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void Visualizer::updateAllRenderers() {
    if (std::find(enabledTargets.begin(), enabledTargets.end(), VisualizationTarget::Network) != enabledTargets.end() && networkRenderer) {
        networkRenderer->render();
    }
    if (std::find(enabledTargets.begin(), enabledTargets.end(), VisualizationTarget::Spikes) != enabledTargets.end() && spikesRenderer) {
        spikesRenderer->render();
    }
    if (std::find(enabledTargets.begin(), enabledTargets.end(), VisualizationTarget::Memory) != enabledTargets.end() && memoryRenderer) {
        memoryRenderer->render();
    }
}

void Visualizer::notifyListeners() {
    // Notify listeners with current data
    if (!networkListeners.empty() && !currentNetworkNodes.empty()) {
        for (const auto& listener : networkListeners) {
            try {
                listener(currentNetworkNodes, currentNetworkEdges);
            } catch (const std::exception& e) {
                std::cerr << "Error notifying network listeners: " << e.what() << std::endl;
            }
        }
    }
    
    if (!spikesListeners.empty() && !currentSpikes.empty()) {
        for (const auto& listener : spikesListeners) {
            try {
                listener(currentSpikes);
            } catch (const std::exception& e) {
                std::cerr << "Error notifying spikes listeners: " << e.what() << std::endl;
            }
        }
    }
    
    if (!memoryListeners.empty() && !currentMemoryNodes.empty()) {
        for (const auto& listener : memoryListeners) {
            try {
                listener(currentMemoryNodes, currentMemoryConnections);
            } catch (const std::exception& e) {
                std::cerr << "Error notifying memory listeners: " << e.what() << std::endl;
            }
        }
    }
}

void Visualizer::loadDefaultConfiguration() {
    visualizationPort = 8080;
    updateInterval = 1.0;
    windowWidth = 1024;
    windowHeight = 768;
    layoutAlgorithm = "force-directed";
    liveMode = false;
    
    enabledTargets = {VisualizationTarget::Network, VisualizationTarget::Spikes};
}

void Visualizer::saveDefaultConfiguration() const {
    // Save default configuration to file
    nlohmann::json jsonData;
    jsonData["visualizationPort"] = visualizationPort;
    jsonData["updateInterval"] = updateInterval;
    jsonData["windowWidth"] = windowWidth;
    jsonData["windowHeight"] = windowHeight;
    jsonData["layoutAlgorithm"] = layoutAlgorithm;
    jsonData["liveMode"] = liveMode;
    
    std::vector<int> targetsJson;
    for (VisualizationTarget target : enabledTargets) {
        targetsJson.push_back(static_cast<int>(target));
    }
    jsonData["enabledTargets"] = targetsJson;
    
    std::ofstream file("visualizer_default_config.json");
    if (file.is_open()) {
        file << jsonData.dump(2);
    }
}

} // namespace nlm
