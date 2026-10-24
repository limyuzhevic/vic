// AdvancedVisualizationInterface.cpp implementation

#include "AdvancedVisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

struct AdvancedVisualizationInterface::Impl {
    VisualizationConfig config;
    std::string currentMode;
    
    // Brain data
    std::vector<NeuronVisualizationData> neurons;
    std::vector<SynapseVisualizationData> synapses;
    std::vector<SpikeVisualizationData> spikes;
    std::vector<ActivityTimeSeries> timeSeries;
    
    // State
    bool paused;
    double currentTime;
    double progress;
    
    // Filters
    double minActivityFilter;
    double maxActivityFilter;
    std::vector<NeuronType> neuronTypeFilter;
    double minSynapseWeightFilter;
    double maxSynapseWeightFilter;
    
    // Selection and highlighting
    std::vector<NeuronId> selectedNeurons;
    std::vector<SynapseId> selectedSynapses;
    std::vector<NeuronId> highlightedNeurons;
    std::vector<SynapseId> highlightedSynapses;
    
    // Interaction state
    bool isDragging;
    double dragStartX, dragStartY;
    double originalPanX, originalPanY;
    
    // Rendering cache
    std::vector<std::vector<float>> neuronPositionsCache;
    std::vector<std::pair<std::vector<float>, std::vector<float>>> synapsePositionsCache;
    
    std::mutex dataMutex;
};

AdvancedVisualizationInterface::AdvancedVisualizationInterface() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration
    pImpl->config = getDefaultVisualizationConfig();
    pImpl->currentMode = "network";
    pImpl->paused = false;
    pImpl->currentTime = 0.0;
    pImpl->progress = 0.0;
    pImpl->minActivityFilter = 0.0;
    pImpl->maxActivityFilter = 1.0;
    pImpl->minSynapseWeightFilter = -1.0;
    pImpl->maxSynapseWeightFilter = 1.0;
    pImpl->isDragging = false;
}

AdvancedVisualizationInterface::~AdvancedVisualizationInterface() = default;

bool AdvancedVisualizationInterface::initialize(const VisualizationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    // Validate configuration
    if (!validateConfig(config)) {
        return false;
    }
    
    pImpl->config = config;
    
    // Initialize with default brain data
    pImpl->neurons.clear();
    pImpl->synapses.clear();
    pImpl->spikes.clear();
    
    // Clear time series
    pImpl->timeSeries.clear();
    
    return true;
}

void AdvancedVisualizationInterface::update(const Brain& brain, double currentTime) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    pImpl->currentTime = currentTime;
    pImpl->progress = currentTime; // Will need to know total simulation time
    
    // Update brain data
    setBrainData(brain);
    
    // Update visualization data
    computeNeuronPositions(brain);
    computeSynapsePositions(brain);
    updateSpikeData(brain, currentTime);
    updateActivityTimeSeries(brain, currentTime);
}

void AdvancedVisualizationInterface::render() {
    if (pImpl->paused) {
        // Render paused state
        return;
    }
    
    // Render based on current mode
    if (pImpl->currentMode == "network") {
        renderNetworkView();
    } else if (pImpl->currentMode == "activity") {
        renderActivityView();
    } else if (pImpl->currentMode == "spikes") {
        renderSpikeView();
    } else if (pImpl->currentMode == "weights") {
        renderWeightView();
    } else if (pImpl->currentMode == "timeseries") {
        renderTimeSeriesView();
    }
    
    // Render UI elements (selection boxes, info panels, etc.)
}

bool AdvancedVisualizationInterface::handleMouseInput(double x, double y, bool leftClick, bool rightClick) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (leftClick) {
        return handleSelection(x, y);
    } else if (rightClick) {
        // Show context menu
        return true;
    }
    
    return false;
}

bool AdvancedVisualizationInterface::handleKeyboardInput(int key, bool pressed) {
    if (pressed) {
        switch (key) {
            case 'W': case 'w':
                return handleZoom(0.1);
            case 'S': case 's':
                return handleZoom(-0.1);
            case 'A': case 'a':
                return handlePan(-0.1, 0.0);
            case 'D': case 'd':
                return handlePan(0.1, 0.0);
            case 'Q': case 'q':
                return handlePan(0.0, -0.1);
            case 'E': case 'e':
                return handlePan(0.0, 0.1);
            case 'R': case 'r':
                // Reset view
                return true;
            case 'F': case 'f':
                // Fit all neurons in view
                return true;
        }
    }
    return false;
}

bool AdvancedVisualizationInterface::handleScrollInput(double delta) {
    return handleZoom(delta * 0.1);
}

void AdvancedVisualizationInterface::setVisualizationMode(const std::string& mode) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    // Validate mode
    std::vector<std::string> validModes = {"network", "activity", "spikes", "weights", "timeseries"};
    if (std::find(validModes.begin(), validModes.end(), mode) != validModes.end()) {
        pImpl->currentMode = mode;
    }
}

std::string AdvancedVisualizationInterface::getCurrentVisualizationMode() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->currentMode;
}

void AdvancedVisualizationInterface::setBrainData(const Brain& brain) {
    // Clear existing data
    pImpl->neurons.clear();
    pImpl->synapses.clear();
    
    // Create neuron visualization data
    for (size_t i = 0; i < brain.getRegionCount(); ++i) {
        auto regionId = brain.getRegionIds()[i];
        auto* region = brain.getRegion(regionId);
        if (!region) continue;
        
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            NeuronVisualizationData vizData;
            vizData.id = neuron->getId();
            
            // Calculate position based on region
            double angle = 2.0 * M_PI * (i + neurons.size() * 0.1) / (brain.getRegionCount() * 10);
            double radius = 100.0 + neurons.size() * 2.0;
            vizData.x = radius * cos(angle);
            vizData.y = radius * sin(angle);
            
            // Set size based on neuron type
            switch (neuron->getType()) {
                case NeuronType::Excitatory:
                    vizData.radius = 5.0;
                    vizData.color = pImpl->config.excitatoryColor;
                    break;
                case NeuronType::Inhibitory:
                    vizData.radius = 4.0;
                    vizData.color = pImpl->config.inhibitoryColor;
                    break;
                case NeuronType::Sensory:
                    vizData.radius = 6.0;
                    vizData.color = pImpl->config.sensoryColor;
                    break;
                case NeuronType::Motor:
                    vizData.radius = 6.0;
                    vizData.color = pImpl->config.motorColor;
                    break;
                case NeuronType::Internal:
                    vizData.radius = 4.5;
                    vizData.color = pImpl->config.internalColor;
                    break;
                case NeuronType::Modulatory:
                    vizData.radius = 5.5;
                    vizData.color = pImpl->config.modulatoryColor;
                    break;
            }
            
            // Get current state from neuron
            const auto& state = neuron->getState();
            vizData.activity = std::abs(state.membranePotential - state.restingPotential) / 20.0f;
            vizData.potential = state.membranePotential;
            vizData.threshold = state.threshold;
            vizData.isActive = neuron->isFiring();
            vizData.regionId = static_cast<int>(regionId.index());
            
            pImpl->neurons.push_back(vizData);
        }
    }
    
    // Create synapse visualization data
    for (size_t i = 0; i < brain.getRegionCount(); ++i) {
        auto regionId = brain.getRegionIds()[i];
        auto* region = brain.getRegion(regionId);
        if (!region) continue;
        
        auto synapses = region->getSynapses();
        for (auto* synapse : synapses) {
            SynapseVisualizationData vizData;
            vizData.id = synapse->getId();
            vizData.preNeuron = synapse->getSourceNeuron();
            vizData.postNeuron = synapse->getDestinationNeuron();
            vizData.weight = synapse->getWeight();
            vizData.delay = synapse->getDelay();
            vizData.type = synapse->getType();
            
            // Calculate activity based on weight and recent usage
            vizData.activity = std::min(1.0f, std::abs(vizData.weight) * 2.0f);
            vizData.isActive = (vizData.activity > 0.5f);
            
            // Set color based on weight
            if (std::abs(vizData.weight) > 0.5f) {
                vizData.color = pImpl->config.strongConnectionColor;
            } else {
                vizData.color = pImpl->config.weakConnectionColor;
            }
            
            if (vizData.isActive) {
                vizData.color = pImpl->config.activeConnectionColor;
            }
            
            vizData.lineWidth = std::min(3.0f, std::max(0.5f, std::abs(vizData.weight) * 4.0f));
            vizData.showArrow = (vizData.type == SynapseType::Excitatory);
            
            pImpl->synapses.push_back(vizData);
        }
    }
}

void AdvancedVisualizationInterface::clearBrainData() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->neurons.clear();
    pImpl->synapses.clear();
    pImpl->spikes.clear();
}

bool AdvancedVisualizationInterface::exportScreenshot(const std::string& filepath, const std::string& format) {
    // TODO: Implement actual screenshot functionality
    // This would involve rendering the current state and saving it
    std::cout << "Screenshot export not yet implemented. File: " << filepath << ", Format: " << format << std::endl;
    return false;
}

bool AdvancedVisualizationInterface::exportNetworkGraph(const std::string& filepath) {
    // Export network data in graph format (GraphML, GML, etc.)
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "// NLM Network Visualization Export\n";
    file << "// Generated at: " << getCurrentTimestamp() << "\n\n";
    
    file << "graph [\n";
    file << "  directed 1\n";
    file << "\n";
    
    // Export neurons
    file << "  node [\n";
    for (const auto& neuron : pImpl->neurons) {
        file << "    node [\n";
        file << "      id " << neuron.id << "\n";
        file << "      label \"" << neuron.id << "\"\n";
        file << "      x " << neuron.x << "\n";
        file << "      y " << neuron.y << "\n";
        file << "      radius " << neuron.radius << "\n";
        file << "      color \"" << neuron.color << "\"\n";
        file << "      type \"" << static_cast<int>(neuron.type) << "\"\n";
        file << "    ]\n";
    }
    file << "  ]\n\n";
    
    // Export synapses
    file << "  edge [\n";
    for (const auto& synapse : pImpl->synapses) {
        file << "    edge [\n";
        file << "      id " << synapse.id << "\n";
        file << "      source " << synapse.preNeuron << "\n";
        file << "      target " << synapse.postNeuron << "\n";
        file << "      weight " << synapse.weight << "\n";
        file << "      delay " << synapse.delay << "\n";
        file << "      color \"" << synapse.color << "\"\n";
        file << "    ]\n";
    }
    file << "  ]\n";
    file << "]\n";
    
    file.close();
    return true;
}

bool AdvancedVisualizationInterface::exportActivityData(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "time,neuron_id,activity,potential,type\n";
    
    for (const auto& neuron : pImpl->neurons) {
        file << pImpl->currentTime << ","
             << neuron.id << ","
             << neuron.activity << ","
             << neuron.potential << ","
             << static_cast<int>(neuron.type) << "\n";
    }
    
    file.close();
    return true;
}

bool AdvancedVisualizationInterface::exportFullReport(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== NLM Visualization Export ===\n";
    file << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    file << "=== Network Statistics ===\n";
    file << "Total Neurons: " << pImpl->neurons.size() << "\n";
    file << "Total Synapses: " << pImpl->synapses.size() << "\n";
    file << "Average Firing Rate: " << getAverageFiringRate() << " Hz\n";
    file << "Network Density: " << getNetworkDensity() << "\n\n";
    
    file << "=== Neuron Type Distribution ===\n";
    auto typeCounts = getNeuronTypeCounts();
    for (const auto& pair : typeCounts) {
        file << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    file << "\n";
    
    file << "=== Top Active Neurons ===\n";
    auto topNeurons = getTopActiveNeurons(10);
    for (const auto& pair : topNeurons) {
        file << "Neuron " << pair.first << ": " << pair.second << " activity\n";
    }
    file << "\n";
    
    // Export network graph
    std::string graphFile = filepath + ".graphml";
    exportNetworkGraph(graphFile);
    
    // Export activity data
    std::string activityFile = filepath + ".csv";
    exportActivityData(activityFile);
    
    file << "=== Exports Generated ===\n";
    file << "Network graph: " << graphFile << "\n";
    file << "Activity data: " << activityFile << "\n";
    
    file.close();
    return true;
}

void AdvancedVisualizationInterface::updateConfig(const VisualizationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->config = config;
}

VisualizationConfig AdvancedVisualizationInterface::getConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->config;
}

void AdvancedVisualizationInterface::addTimeSeries(const ActivityTimeSeries& series) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->timeSeries.push_back(series);
}

void AdvancedVisualizationInterface::removeTimeSeries(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->timeSeries.erase(
        std::remove_if(pImpl->timeSeries.begin(), pImpl->timeSeries.end(),
                      [&name](const ActivityTimeSeries& series) {
                          return series.name == name;
                      }),
        pImpl->timeSeries.end()
    );
}

void AdvancedVisualizationInterface::clearTimeSeries() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->timeSeries.clear();
}

void AdvancedVisualizationInterface::setTimeSeriesVisible(const std::string& name, bool visible) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    for (auto& series : pImpl->timeSeries) {
        if (series.name == name) {
            series.isVisible = visible;
            break;
        }
    }
}

NeuronId AdvancedVisualizationInterface::getNeuronAtPosition(double x, double y) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    double minDist = std::numeric_limits<double>::max();
    NeuronId nearestNeuron = 0;
    
    for (const auto& neuron : pImpl->neurons) {
        double dx = neuron.x - x;
        double dy = neuron.y - y;
        double dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < minDist) {
            minDist = dist;
            nearestNeuron = neuron.id;
        }
    }
    
    return (minDist < pImpl->config.nodeSpacing * 2.0) ? nearestNeuron : 0;
}

std::vector<NeuronId> AdvancedVisualizationInterface::getNeuronsInRegion(const std::string& regionName) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    std::vector<NeuronId> result;
    
    // Convert region name to region ID (simple implementation)
    int regionId = 0;
    try {
        regionId = std::stoi(regionName);
    } catch (...) {
        return result;
    }
    
    for (const auto& neuron : pImpl->neurons) {
        if (neuron.regionId == regionId) {
            result.push_back(neuron.id);
        }
    }
    
    return result;
}

void AdvancedVisualizationInterface::highlightNeuron(NeuronId id, bool highlight) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (highlight) {
        pImpl->highlightedNeurons.push_back(id);
    } else {
        pImpl->highlightedNeurons.erase(
            std::remove(pImpl->highlightedNeurons.begin(), pImpl->highlightedNeurons.end(), id),
            pImpl->highlightedNeurons.end()
        );
    }
}

void AdvancedVisualizationInterface::selectNeuron(NeuronId id, bool select) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (select) {
        pImpl->selectedNeurons.push_back(id);
    } else {
        pImpl->selectedNeurons.erase(
            std::remove(pImpl->selectedNeurons.begin(), pImpl->selectedNeurons.end(), id),
            pImpl->selectedNeurons.end()
        );
    }
}

SynapseId AdvancedVisualizationInterface::getSynapseAtPosition(double x, double y) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    double minDist = std::numeric_limits<double>::max();
    SynapseId nearestSynapse = 0;
    
    for (const auto& synapse : pImpl->synapses) {
        // Simple line distance calculation
        double dx = synapse.preNeuron.x - synapse.postNeuron.x;
        double dy = synapse.preNeuron.y - synapse.postNeuron.y;
        double len = std::sqrt(dx * dx + dy * dy);
        
        if (len < 0.001) continue;
        
        // Project point onto line
        double t = ((x - synapse.preNeuron.x) * dx + (y - synapse.preNeuron.y) * dy) / (len * len);
        t = std::max(0.0, std::min(1.0, t));
        
        double projX = synapse.preNeuron.x + t * dx;
        double projY = synapse.preNeuron.y + t * dy;
        
        double dist = std::sqrt((x - projX) * (x - projX) + (y - projY) * (y - projY));
        
        if (dist < minDist) {
            minDist = dist;
            nearestSynapse = synapse.id;
        }
    }
    
    return (minDist < 5.0) ? nearestSynapse : 0;
}

std::vector<SynapseId> AdvancedVisualizationInterface::getSynapsesBetween(NeuronId pre, NeuronId post) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    std::vector<SynapseId> result;
    
    for (const auto& synapse : pImpl->synapses) {
        if (synapse.preNeuron == pre && synapse.postNeuron == post) {
            result.push_back(synapse.id);
        }
    }
    
    return result;
}

void AdvancedVisualizationInterface::highlightSynapse(SynapseId id, bool highlight) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (highlight) {
        pImpl->highlightedSynapses.push_back(id);
    } else {
        pImpl->highlightedSynapses.erase(
            std::remove(pImpl->highlightedSynapses.begin(), pImpl->highlightedSynapses.end(), id),
            pImpl->highlightedSynapses.end()
        );
    }
}

void AdvancedVisualizationInterface::showSynapseDetails(SynapseId id) {
    // Implementation would show detailed information about synapse
    // For now, just log it
    std::cout << "Synapse details requested for ID: " << id << std::endl;
}

void AdvancedVisualizationInterface::pause() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->paused = true;
}

void AdvancedVisualizationInterface::resume() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->paused = false;
}

void AdvancedVisualizationInterface::stepForward() {
    // Implementation would advance by one time step
    std::cout << "Step forward requested" << std::endl;
}

bool AdvancedVisualizationInterface::isPaused() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->paused;
}

double AdvancedVisualizationInterface::getAverageFiringRate() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (pImpl->neurons.empty()) {
        return 0.0;
    }
    
    double totalActivity = 0.0;
    for (const auto& neuron : pImpl->neurons) {
        totalActivity += neuron.activity;
    }
    
    return (totalActivity / pImpl->neurons.size()) * 100.0; // Convert to percentage
}

double AdvancedVisualizationInterface::getNetworkDensity() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (pImpl->neurons.size() < 2) {
        return 0.0;
    }
    
    // Calculate density as ratio of existing connections to possible connections
    size_t possibleConnections = pImpl->neurons.size() * (pImpl->neurons.size() - 1);
    return static_cast<double>(pImpl->synapses.size()) / possibleConnections;
}

std::vector<std::pair<NeuronId, double>> AdvancedVisualizationInterface::getTopActiveNeurons(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    std::vector<std::pair<NeuronId, double>> neuronActivities;
    
    for (const auto& neuron : pImpl->neurons) {
        neuronActivities.emplace_back(neuron.id, neuron.activity);
    }
    
    // Sort by activity (descending)
    std::sort(neuronActivities.begin(), neuronActivities.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    if (count > neuronActivities.size()) {
        count = neuronActivities.size();
    }
    
    return std::vector<std::pair<NeuronId, double>>(neuronActivities.begin(),
                                                   neuronActivities.begin() + count);
}

std::map<NeuronType, size_t> AdvancedVisualizationInterface::getNeuronTypeCounts() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    std::map<NeuronType, size_t> counts;
    
    for (const auto& neuron : pImpl->neurons) {
        counts[neuron.type]++;
    }
    
    return counts;
}

void AdvancedVisualizationInterface::setProgress(double progress) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->progress = std::max(0.0, std::min(1.0, progress));
}

double AdvancedVisualizationInterface::getProgress() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->progress;
}

// Private helper functions

VisualizationConfig AdvancedVisualizationInterface::getDefaultVisualizationConfig() const {
    VisualizationConfig config;
    
    // Window settings
    config.windowWidth = 1600;
    config.windowHeight = 1200;
    config.updateRate = 30.0;
    config.windowTitle = "NLM Advanced Brain Visualization";
    
    // Network layout
    config.nodeSpacing = 50.0;
    config.connectionDistance = 100.0;
    config.zoomLevel = 1.0;
    config.panX = 0.0;
    config.panY = 0.0;
    
    // Neuron settings
    config.minNeuronSize = 3.0;
    config.maxNeuronSize = 10.0;
    config.showNeuronLabels = true;
    config.showNeuronType = true;
    
    // Synapse settings
    config.minSynapseWidth = 0.5;
    config.maxSynapseWidth = 4.0;
    config.showSynapseWeights = true;
    config.showSynapseArrows = true;
    
    // Activity settings
    config.showActivityRasters = true;
    config.showPopulationRates = true;
    config.showMembranePotential = true;
    config.showCalciumImaging = false;
    
    // Color schemes (using visible colors)
    config.excitatoryColor = "#FF6B6B";      // Red
    config.inhibitoryColor = "#4ECDC4";      // Teal
    config.sensoryColor = "#45B7D1";        // Blue
    config.motorColor = "#96CEB4";           // Green
    config.internalColor = "#FFEAA7";        // Yellow
    config.modulatoryColor = "#DDA0DD";      // Purple
    
    config.strongConnectionColor = "#FF4444";   // Dark red
    config.weakConnectionColor = "#4444FF";     // Dark blue
    config.activeConnectionColor = "#FF8800";   // Orange
    
    // Time series settings
    config.timeSeriesWidth = 300.0;
    config.timeSeriesHeight = 150.0;
    config.showTimeSeries = true;
    config.autoScaleTimeSeries = true;
    
    // Export settings
    config.exportFormat = "png";
    config.exportDPI = 150;
    config.includeAnnotations = true;
    config.exportPath = ".";
    
    return config;
}

bool AdvancedVisualizationInterface::validateConfig(const VisualizationConfig& config) const {
    // Basic validation
    if (config.windowWidth <= 0 || config.windowHeight <= 0) {
        return false;
    }
    
    if (config.updateRate <= 0.0) {
        return false;
    }
    
    if (config.nodeSpacing <= 0.0) {
        return false;
    }
    
    if (config.minNeuronSize <= 0.0 || config.maxNeuronSize <= config.minNeuronSize) {
        return false;
    }
    
    if (config.minSynapseWidth <= 0.0 || config.maxSynapseWidth <= config.minSynapseWidth) {
        return false;
    }
    
    return true;
}

void AdvancedVisualizationInterface::applyConfigurationDefaults(VisualizationConfig& config) const {
    // Ensure config has valid values
    if (config.excitatoryColor.empty()) config.excitatoryColor = "#FF6B6B";
    if (config.inhibitoryColor.empty()) config.inhibitoryColor = "#4ECDC4";
    if (config.sensoryColor.empty()) config.sensoryColor = "#45B7D1";
    if (config.motorColor.empty()) config.motorColor = "#96CEB4";
    if (config.internalColor.empty()) config.internalColor = "#FFEAA7";
    if (config.modulatoryColor.empty()) config.modulatoryColor = "#DDA0DD";
    
    if (config.strongConnectionColor.empty()) config.strongConnectionColor = "#FF4444";
    if (config.weakConnectionColor.empty()) config.weakConnectionColor = "#4444FF";
    if (config.activeConnectionColor.empty()) config.activeConnectionColor = "#FF8800";
}

void AdvancedVisualizationInterface::computeNeuronPositions(const Brain& brain) {
    pImpl->neuronPositionsCache.clear();
    
    for (const auto& neuron : pImpl->neurons) {
        std::vector<float> position{static_cast<float>(neuron.x), static_cast<float>(neuron.y)};
        pImpl->neuronPositionsCache.push_back(position);
    }
}

void AdvancedVisualizationInterface::computeSynapsePositions(const Brain& brain) {
    pImpl->synapsePositionsCache.clear();
    
    for (const auto& synapse : pImpl->synapses) {
        auto itPre = std::find_if(pImpl->neurons.begin(), pImpl->neurons.end(),
                                 [&synapse](const NeuronVisualizationData& n) { return n.id == synapse.preNeuron; });
        auto itPost = std::find_if(pImpl->neurons.begin(), pImpl->neurons.end(),
                                  [&synapse](const NeuronVisualizationData& n) { return n.id == synapse.postNeuron; });
        
        if (itPre != pImpl->neurons.end() && itPost != pImpl->neurons.end()) {
            std::vector<float> prePos{static_cast<float>(itPre->x), static_cast<float>(itPre->y)};
            std::vector<float> postPos{static_cast<float>(itPost->x), static_cast<float>(itPost->y)};
            pImpl->synapsePositionsCache.push_back(std::make_pair(prePos, postPos));
        }
    }
}

void AdvancedVisualizationInterface::updateSpikeData(const Brain& brain, double currentTime) {
    pImpl->spikes.clear();
    
    // Get spike data from brain (implementation would depend on Brain interface)
    // For now, this is a placeholder
    std::cout << "Spike data update not yet implemented" << std::endl;
}

void AdvancedVisualizationInterface::updateActivityTimeSeries(const Brain& brain, double currentTime) {
    for (auto& series : pImpl->timeSeries) {
        series.timestamps.push_back(currentTime);
        
        // Calculate appropriate value based on series name
        double value = 0.0;
        if (series.name == "average_firing_rate") {
            value = getAverageFiringRate();
        } else if (series.name == "network_density") {
            value = getNetworkDensity();
        } else {
            value = 0.0;
        }
        
        series.values.push_back(value);
        
        // Keep only recent data points (last 1000)
        if (series.timestamps.size() > 1000) {
            series.timestamps.erase(series.timestamps.begin());
            series.values.erase(series.values.begin());
        }
    }
}

void AdvancedVisualizationInterface::renderNetworkView() {
    // Implementation would render neurons and synapses
    std::cout << "Rendering network view" << std::endl;
}

void AdvancedVisualizationInterface::renderActivityView() {
    // Implementation would render activity information
    std::cout << "Rendering activity view" << std::endl;
}

void AdvancedVisualizationInterface::renderSpikeView() {
    // Implementation would render spike data
    std::cout << "Rendering spike view" << std::endl;
}

void AdvancedVisualizationInterface::renderWeightView() {
    // Implementation would render weight matrix
    std::cout << "Rendering weight view" << std::endl;
}

void AdvancedVisualizationInterface::renderTimeSeriesView() {
    // Implementation would render time series plots
    std::cout << "Rendering time series view" << std::endl;
}

bool AdvancedVisualizationInterface::handleZoom(double delta) {
    pImpl->config.zoomLevel += delta;
    pImpl->config.zoomLevel = std::max(0.1, std::min(10.0, pImpl->config.zoomLevel));
    return true;
}

bool AdvancedVisualizationInterface::handlePan(double dx, double dy) {
    pImpl->config.panX += dx;
    pImpl->config.panY += dy;
    return true;
}

bool AdvancedVisualizationInterface::handleSelection(double x, double y) {
    // Check if we clicked on a neuron or synapse
    NeuronId neuronId = getNeuronAtPosition(x, y);
    if (neuronId != 0) {
        selectNeuron(neuronId, true);
        return true;
    }
    
    SynapseId synapseId = getSynapseAtPosition(x, y);
    if (synapseId != 0) {
        highlightSynapse(synapseId, true);
        return true;
    }
    
    return false;
}

std::string AdvancedVisualizationInterface::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_cast<std::chrono::system_clock::duration<double>>(now);
    
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));
    
    return std::string(buffer);
}

} // namespace nlm