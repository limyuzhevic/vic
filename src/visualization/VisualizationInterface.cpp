#include "VisualizationInterface.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <queue>
#include <stack>
#include <random>
#include <thread>
#include <mutex>
#include <nlohmann/json.hpp>

namespace nlm {

// VisualizationInterface implementation
struct VisualizationInterface::Impl {
    VisualizationMode mode;
    NetworkStyle style;
    VisualizationConfig config;
    bool active;
    double updateRate;
    int frameRateLimit;
    bool realTimeMode;
    bool antiAliasing;
    bool shadows;
    bool lighting;
    
    // Data storage
    std::vector<NeuronVisualizationData> neurons;
    std::vector<SynapseVisualizationData> synapses;
    std::vector<SpikeVisualizationData> spikes;
    std::vector<std::vector<float>> activityHistory;
    
    // State management
    bool recording;
    std::chrono::system_clock::time_point recordingStart;
    std::vector<std::chrono::system_clock::time_point> frameTimes;
    
    // Interaction state
    std::vector<NeuronId> selectedNeurons;
    std::vector<SynapseId> selectedSynapses;
    double viewX, viewY, viewScale, viewRotation;
    
    // Performance tracking
    VisualizationPerformance perf;
    
    // Layout calculation
    std::vector<NeuronId> layerOrder;
    std::unordered_map<NeuronId, std::vector<NeuronId>> adjacencyList;
    
    // Rendering state
    std::vector<std::vector<std::string>> renderBuffer;
    int bufferWidth, bufferHeight;
    
    // Random number generation for layout
    std::mt19937 rng;
    std::uniform_real_distribution<float> floatDist;
    std::normal_distribution<float> normalDist;
    
    Impl() : mode(VisualizationMode::NetworkGraph),
              style(NetworkStyle::Biological),
              active(false),
              updateRate(30.2),
              frameRateLimit(60),
              realTimeMode(true),
              antiAliasing(true),
              shadows(false),
              lighting(false),
              recording(false),
              viewX(0.0), viewY(0.0), viewScale(1.0), viewRotation(0.0),
              bufferWidth(80), bufferHeight(40),
              floatDist(0.0f, 1.0f),
              normalDist(0.0f, 1.0f) {
        rng.seed(std::chrono::system_clock::now().time_since_epoch().count());
        renderBuffer.resize(bufferHeight, std::vector<std::string>(bufferWidth, " "));
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>() {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize visualization system
    pImpl->active = true;
    
    // Set up default configuration
    pImpl->config.mode = VisualizationMode::NetworkGraph;
    pImpl->config.style = NetworkStyle::Biological;
    pImpl->config.scale = 1.0;
    pImpl->config.nodeSize = 1.0;
    pImpl->config.edgeWidth = 0.5;
    pImpl->config.showLabels = true;
    pImpl->config.showTooltips = true;
    pImpl->config.backgroundColor = "#000000";
    pImpl->config.neuronColor = "#00ff00";
    pImpl->config.synapseColor = "#ffffff";
    pImpl->config.animationSpeed = 1.0;
    pImpl->config.enableAntialiasing = true;
    pImpl->config.enableShadows = false;
    pImpl->config.enableLighting = false;
    
    // Calculate initial layout
    calculateNetworkLayout();
    
    return true;
}

void VisualizationInterface::close() {
    pImpl->active = false;
    pImpl->neurons.clear();
    pImpl->synapses.clear();
    pImpl->spikes.clear();
    pImpl->activityHistory.clear();
    stopRecording();
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    auto currentTime = std::chrono::system_clock::now();
    pImpl->perf.framesRendered++;
    
    // Calculate frame time
    if (!pImpl->perf.frameTimes.empty()) {
        auto lastFrame = pImpl->perf.frameTimes.back();
        auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastFrame);
        pImpl->perf.averageFrameTime = (pImpl->perf.averageFrameTime * (pImpl->perf.framesRendered - 1) + frameTime.count()) / pImpl->perf.framesRendered;
    }
    pImpl->perf.frameTimes.push_back(currentTime);
    
    // Limit frame history
    if (pImpl->perf.frameTimes.size() > 100) {
        pImpl->perf.frameTimes.erase(pImpl->perf.frameTimes.begin());
    }
    
    // Calculate FPS
    if (!pImpl->perf.frameTimes.empty()) {
        auto totalTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - pImpl->perf.frameTimes.front());
        if (totalTime.count() > 0) {
            pImpl->perf.fps = pImpl->perf.framesRendered / static_cast<double>(totalTime.count());
        }
    }
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    // Clear render buffer
    for (auto& row : pImpl->renderBuffer) {
        std::fill(row.begin(), row.end(), " ");
    }
    
    // Render based on current mode
    switch (pImpl->mode) {
        case VisualizationMode::NetworkGraph:
            renderNetworkGraph();
            break;
        case VisualizationMode::SpikeRaster:
            renderSpikeRaster();
            break;
        case VisualizationMode::WeightMatrix:
            renderWeightMatrix();
            break;
        case VisualizationMode::ActivityHeatmap:
            renderActivityHeatmap();
            break;
        default:
            renderNetworkGraph();
            break;
    }
    
    pImpl->perf.drawCalls++;
}

void VisualizationInterface::setMode(VisualizationMode mode) {
    pImpl->mode = mode;
}

VisualizationMode VisualizationInterface::getMode() const {
    return pImpl->mode;
}

void VisualizationInterface::setStyle(NetworkStyle style) {
    pImpl->style = style;
    calculateNetworkLayout();
}

NetworkStyle VisualizationInterface::getStyle() const {
    return pImpl->style;
}

void VisualizationInterface::setConfig(const VisualizationConfig& config) {
    pImpl->config = config;
}

const VisualizationConfig& VisualizationInterface::getConfig() const {
    return pImpl->config;
}

void VisualizationInterface::clearData() {
    pImpl->neurons.clear();
    pImpl->synapses.clear();
    pImpl->spikes.clear();
    pImpl->activityHistory.clear();
}

void VisualizationInterface::setNetworkData(const std::vector<NeuronVisualizationData>& neurons,
                                           const std::vector<SynapseVisualizationData>& synapses) {
    pImpl->neurons = neurons;
    pImpl->synapses = synapses;
    calculateNetworkLayout();
}

void VisualizationInterface::addSpikeData(const SpikeVisualizationData& spike) {
    pImpl->spikes.push_back(spike);
    
    // Keep only recent spikes (last 1000)
    if (pImpl->spikes.size() > 1000) {
        pImpl->spikes.erase(pImpl->spikes.begin());
    }
}

void VisualizationInterface::addActivityData(const std::vector<float>& activityProfile) {
    pImpl->activityHistory.push_back(activityProfile);
    
    // Keep only recent activity (last 100 frames)
    if (pImpl->activityHistory.size() > 100) {
        pImpl->activityHistory.erase(pImpl->activityHistory.begin());
    }
}

void VisualizationInterface::updateFromBrain(const class Brain& brain, double timestep) {
    // Generate visualization data from brain state
    pImpl->neurons.clear();
    pImpl->synapses.clear();
    
    // Add neurons
    for (const auto& region : brain.getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                NeuronVisualizationData nData;
                nData.id = neuron->getId();
                nData.firingRate = neuron->getFiringRate();
                nData.timeActive = static_cast<float>(neuron->getState().lastSpikeTime > 0 ? 
                    std::chrono::duration<double>(std::chrono::system_clock::now() - 
                        std::chrono::system_clock::time_point(std::chrono::duration<uint64_t>(neuron->getState().lastSpikeTime))).count() : 0.0);
                
                // Calculate position based on population and region
                size_t popIndex = 0;
                for (size_t i = 0; i < region->getPopulations().size(); ++i) {
                    if (region->getPopulations()[i].get() == pop.get()) {
                        popIndex = i;
                        break;
                    }
                }
                
                float angle = (static_cast<float>(popIndex) / static_cast<float>(region->getPopulations().size())) * 2.0f * 3.14159f;
                nData.x = cos(angle) * 2.0;
                nData.y = sin(angle) * 2.0;
                
                // Calculate radius based on firing rate
                nData.radius = std::min(10.0f, 1.0f + nData.firingRate / 10.0f);
                
                // Set color based on neuron type
                switch (neuron->getType()) {
                    case NeuronType::Sensory:
                        nData.color[0] = 0.1f; nData.color[1] = 0.5f; nData.color[2] = 1.0f;
                        break;
                    case NeuronType::Motor:
                        nData.color[0] = 1.0f; nData.color[1] = 0.5f; nData.color[2] = 0.1f;
                        break;
                    case NeuronType::Internal:
                        nData.color[0] = 0.0f; nData.color[1] = 0.8f; nData.color[2] = 0.0f;
                        break;
                    default:
                        nData.color[0] = 0.5f; nData.color[1] = 0.5f; nData.color[2] = 0.5f;
                        break;
                }
                
                nData.opacity = 0.8f;
                nData.highlighted = false;
                nData.selected = std::find(pImpl->selectedNeurons.begin(), pImpl->selectedNeurons.end(), nData.id) != pImpl->selectedNeurons.end();
                nData.label = "Neuron " + std::to_string(neuron->getId());
                
                pImpl->neurons.push_back(nData);
            }
        }
    }
    
    // Add synapses
    for (const auto& region : brain.getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            SynapseVisualizationData sData;
            sData.sourceId = syn->getSourceNeuron().index();
            sData.targetId = syn->getDestinationNeuron().index();
            sData.weight = syn->getWeight();
            sData.opacity = 0.5f + sData.weight * 0.5f;
            sData.active = syn->getWeight() > 0.5f;
            sData.pulseTime = 0.0f;
            sData.color[0] = sData.weight;
            sData.color[1] = 1.0f - sData.weight;
            sData.color[2] = 0.5f;
            
            pImpl->synapses.push_back(sData);
        }
    }
}

void VisualizationInterface::visualizeBrain(const class Brain& brain) {
    visualizeBrainState(brain, pImpl->mode);
}

void VisualizationInterface::visualizeBrainState(const class Brain& brain, VisualizationMode mode) {
    switch (mode) {
        case VisualizationMode::NetworkGraph:
            visualizeNetworkGraph(brain, pImpl->config);
            break;
        case VisualizationMode::SpikeRaster:
            visualizeSpikeRaster(brain, pImpl->config);
            break;
        case VisualizationMode::WeightMatrix:
            visualizeWeightMatrix(brain, pImpl->config);
            break;
        case VisualizationMode::ActivityHeatmap:
            visualizeActivityHeatmap(brain, pImpl->config);
            break;
        default:
            visualizeNetworkGraph(brain, pImpl->config);
            break;
    }
}

void VisualizationInterface::exportData(const std::string& filepath, const std::string& format) {
    if (format == "json") {
        std::ofstream file(filepath);
        if (file.is_open()) {
            nlohmann::json j;
            
            // Export neurons
            nlohmann::json neuronsJson = nlohmann::json::array();
            for (const auto& neuron : pImpl->neurons) {
                nlohmann::json n;
                n["id"] = neuron.id;
                n["x"] = neuron.x;
                n["y"] = neuron.y;
                n["radius"] = neuron.radius;
                n["color"] = {neuron.color[0], neuron.color[1], neuron.color[2]};
                n["firing_rate"] = neuron.firingRate;
                neuronsJson.push_back(n);
            }
            j["neurons"] = neuronsJson;
            
            // Export synapses
            nlohmann::json synapsesJson = nlohmann::json::array();
            for (const auto& synapse : pImpl->synapses) {
                nlohmann::json s;
                s["source"] = synapse.sourceId;
                s["target"] = synapse.targetId;
                s["weight"] = synapse.weight;
                s["active"] = synapse.active;
                synapsesJson.push_back(s);
            }
            j["synapses"] = synapsesJson;
            
            // Export spikes
            nlohmann::json spikesJson = nlohmann::json::array();
            for (const auto& spike : pImpl->spikes) {
                nlohmann::json sp;
                sp["time"] = spike.time;
                sp["neuron_id"] = spike.neuronId;
                sp["intensity"] = spike.intensity;
                spikesJson.push_back(sp);
            }
            j["spikes"] = spikesJson;
            
            file << j.dump(2);
        }
    }
}

std::string VisualizationInterface::generateReport() const {
    std::ostringstream oss;
    oss << "=== Visualization Report ===\n";
    oss << "Generated: " << std::chrono::system_clock::now().time_since_epoch().count() << "\n";
    oss << "Neurons: " << pImpl->neurons.size() << "\n";
    oss << "Synapses: " << pImpl->synapses.size() << "\n";
    oss << "Spikes (recorded): " << pImpl->spikes.size() << "\n";
    oss << "Activity frames: " << pImpl->activityHistory.size() << "\n";
    oss << "Mode: ";
    switch (pImpl->mode) {
        case VisualizationMode::NetworkGraph: oss << "Network Graph"; break;
        case VisualizationMode::SpikeRaster: oss << "Spike Raster"; break;
        case VisualizationMode::WeightMatrix: oss << "Weight Matrix"; break;
        case VisualizationMode::ActivityHeatmap: oss << "Activity Heatmap"; break;
        default: oss << "Unknown"; break;
    }
    oss << "\n";
    oss << "Style: ";
    switch (pImpl->style) {
        case NetworkStyle::Circular: oss << "Circular"; break;
        case NetworkStyle::Hierarchical: oss << "Hierarchical"; break;
        case NetworkStyle::Grid: oss << "Grid"; break;
        case NetworkStyle::Random: oss << "Random"; break;
        case NetworkStyle::Biological: oss << "Biological"; break;
        default: oss << "Unknown"; break;
    }
    oss << "\n";
    oss << "Performance: " << pImpl->perf.fps << " FPS, " << pImpl->perf.averageFrameTime << " ms average frame time\n";
    oss << "=== End Report ===";
    return oss.str();
}

void VisualizationInterface::saveScreenshot(const std::string& filepath) {
    // Save current render buffer as text file (simple visualization)
    std::ofstream file(filepath);
    if (file.is_open()) {
        for (const auto& row : pImpl->renderBuffer) {
            for (const auto& ch : row) {
                file << ch;
            }
            file << "\n";
        }
    }
}

VisualizationPerformance VisualizationInterface::getPerformance() const {
    return pImpl->perf;
}

void VisualizationInterface::resetPerformanceStats() {
    pImpl->perf.framesRendered = 0;
    pImpl->perf.averageFrameTime = 0.0;
    pImpl->perf.fps = 0.0;
    pImpl->perf.memoryUsage = 0;
    pImpl->perf.drawCalls = 0;
    pImpl->perf.cpuTime = 0.0;
    pImpl->perf.gpuTime = 0.0;
    pImpl->perf.frameTimes.clear();
}

void VisualizationInterface::startRecording() {
    pImpl->recording = true;
    pImpl->recordingStart = std::chrono::system_clock::now();
}

void VisualizationInterface::stopRecording() {
    pImpl->recording = false;
}

bool VisualizationInterface::isRecording() const {
    return pImpl->recording;
}

std::vector<SpikeVisualizationData> VisualizationInterface::getSpikeHistory() const {
    return pImpl->spikes;
}

std::vector<std::vector<float>> VisualizationInterface::getActivityHistory() const {
    return pImpl->activityHistory;
}

void VisualizationInterface::visualizeNetworkGraph(const class Brain& brain, const VisualizationConfig& config) {
    // Simple text-based network visualization
    clearRenderBuffer();
    
    // Draw synapses as lines
    for (const auto& synapse : pImpl->synapses) {
        auto sourceIt = std::find_if(pImpl->neurons.begin(), pImpl->neurons.end(),
                                   [synapse](const NeuronVisualizationData& n) { return n.id == synapse.sourceId; });
        auto targetIt = std::find_if(pImpl->neurons.begin(), pImpl->neurons.end(),
                                   [synapse](const NeuronVisualizationData& n) { return n.id == synapse.targetId; });
        
        if (sourceIt != pImpl->neurons.end() && targetIt != pImpl->neurons.end()) {
            // Calculate screen coordinates
            int x1 = static_cast<int>((sourceIt->x + 2.0) * (pImpl->bufferWidth / 4.0));
            int y1 = static_cast<int>((sourceIt->y + 2.0) * (pImpl->bufferHeight / 4.0));
            int x2 = static_cast<int>((targetIt->x + 2.0) * (pImpl->bufferWidth / 4.0));
            int y2 = static_cast<int>((targetIt->y + 2.0) * (pImpl->bufferHeight / 4.0));
            
            // Draw line using Bresenham's algorithm
            drawLine(x1, y1, x2, y2, "│");
        }
    }
    
    // Draw neurons as circles
    for (const auto& neuron : pImpl->neurons) {
        int cx = static_cast<int>((neuron.x + 2.0) * (pImpl->bufferWidth / 4.0));
        int cy = static_cast<int>((neuron.y + 2.0) * (pImpl->bufferHeight / 4.0));
        int radius = static_cast<int>(neuron.radius * 2.0);
        
        // Draw circle using midpoint algorithm
        drawCircle(cx, cy, radius, "@");
        
        // Add label if enabled
        if (config.showLabels && neuron.label.size() < pImpl->bufferWidth) {
            int labelY = cy - radius - 1;
            if (labelY >= 0 && labelY < pImpl->bufferHeight) {
                for (size_t i = 0; i < neuron.label.size() && i < static_cast<size_t>(pImpl->bufferWidth - cx); ++i) {
                    if (cx + i >= 0 && cx + i < pImpl->bufferWidth && labelY >= 0 && labelY < pImpl->bufferHeight) {
                        pImpl->renderBuffer[labelY][cx + i] = neuron.label[i];
                    }
                }
            }
        }
    }
}

void VisualizationInterface::visualizeSpikeRaster(const class Brain& brain, const VisualizationConfig& config) {
    clearRenderBuffer();
    
    // Collect spike times
    std::vector<std::pair<uint64_t, NeuronId>> spikeData;
    for (const auto& spike : pImpl->spikes) {
        spikeData.push_back({spike.time, spike.neuronId});
    }
    
    // Sort by time
    std::sort(spikeData.begin(), spikeData.end());
    
    // Find neuron positions for spike display
    std::unordered_map<NeuronId, std::pair<int, int>> neuronPositions;
    for (const auto& neuron : pImpl->neurons) {
        int x = static_cast<int>((neuron.x + 2.0) * (pImpl->bufferWidth / 4.0));
        int y = static_cast<int>((neuron.y + 2.0) * (pImpl->bufferHeight / 4.0));
        neuronPositions[neuron.id] = {x, y};
    }
    
    // Draw spike raster
    for (const auto& spike : spikeData) {
        auto it = neuronPositions.find(spike.neuronId);
        if (it != neuronPositions.end()) {
            int x = it->second.first;
            int y = static_cast<int>(spike.time % pImpl->bufferHeight);
            if (y >= 0 && y < pImpl->bufferHeight) {
                pImpl->renderBuffer[y][x] = "*";
            }
        }
    }
    
    // Draw time axis
    for (int x = 0; x < pImpl->bufferWidth; ++x) {
        if (x % 10 == 0) {
            for (int y = 0; y < pImpl->bufferHeight; ++y) {
                pImpl->renderBuffer[y][x] = "-";
            }
        }
    }
}

void VisualizationInterface::visualizeWeightMatrix(const class Brain& brain, const VisualizationConfig& config) {
    clearRenderBuffer();
    
    // Create weight matrix display
    int matrixSize = static_cast<int>(std::sqrt(pImpl->synapses.size()) + 1);
    matrixSize = std::min(matrixSize, pImpl->bufferWidth / 2);
    
    // Draw weight matrix as a heatmap
    for (int i = 0; i < matrixSize && i * 2 < static_cast<int>(pImpl->synapses.size()); ++i) {
        for (int j = 0; j < matrixSize && i * matrixSize + j < static_cast<int>(pImpl->synapses.size()); ++j) {
            const auto& synapse = pImpl->synapses[i * matrixSize + j];
            int x = i * 2;
            int y = j * 2;
            
            // Use grayscale to represent weight
            char intensity = static_cast<char>(' ' + synapse.weight * 255);
            pImpl->renderBuffer[y][x] = intensity;
            if (x + 1 < pImpl->bufferWidth && y + 1 < pImpl->bufferHeight) {
                pImpl->renderBuffer[y][x + 1] = intensity;
                pImpl->renderBuffer[y + 1][x] = intensity;
                pImpl->renderBuffer[y + 1][x + 1] = intensity;
            }
        }
    }
}

void VisualizationInterface::visualizeActivityHeatmap(const class Brain& brain, const VisualizationConfig& config) {
    clearRenderBuffer();
    
    // Calculate average activity for each neuron
    std::vector<float> avgActivity;
    for (const auto& neuron : pImpl->neurons) {
        avgActivity.push_back(neuron.firingRate);
    }
    
    // Find min and max for normalization
    float minActivity = *std::min_element(avgActivity.begin(), avgActivity.end());
    float maxActivity = *std::max_element(avgActivity.begin(), avgActivity.end());
    if (maxActivity > minActivity) {
        // Normalize activity to 0-1 range
        for (auto& activity : avgActivity) {
            activity = (activity - minActivity) / (maxActivity - minActivity);
        }
    }
    
    // Draw heat map
    for (size_t i = 0; i < avgActivity.size() && i * 2 < pImpl->bufferWidth; ++i) {
        int x = static_cast<int>(i * 2);
        float intensity = avgActivity[i];
        char heatChar = intensity > 0.8 ? '@' : intensity > 0.6 ? '#' : intensity > 0.4 ? '*' : intensity > 0.2 ? '.' : ' ';
        
        int y = static_cast<int>((avgActivity.size() - 1 - i) * pImpl->bufferHeight / avgActivity.size());
        if (y >= 0 && y < pImpl->bufferHeight && x >= 0 && x < pImpl->bufferWidth) {
            pImpl->renderBuffer[y][x] = heatChar;
        }
    }
}

void VisualizationInterface::calculateNetworkLayout() {
    pImpl->layerOrder.clear();
    pImpl->adjacencyList.clear();
    
    switch (pImpl->style) {
        case NetworkStyle::Circular:
            calculateCircularLayout();
            break;
        case NetworkStyle::Hierarchical:
            calculateHierarchicalLayout();
            break;
        case NetworkStyle::Grid:
            calculateGridLayout();
            break;
        case NetworkStyle::Random:
            calculateRandomLayout();
            break;
        case NetworkStyle::Biological:
            calculateBiologicalLayout();
            break;
        default:
            calculateCircularLayout();
            break;
    }
}

void VisualizationInterface::calculateCircularLayout() {
    float angleStep = 2.0 * 3.14159f / std::max(1.0f, static_cast<float>(pImpl->neurons.size()));
    
    for (size_t i = 0; i < pImpl->neurons.size(); ++i) {
        pImpl->neurons[i].x = cos(i * angleStep);
        pImpl->neurons[i].y = sin(i * angleStep);
        pImpl->layerOrder.push_back(pImpl->neurons[i].id);
    }
}

void VisualizationInterface::calculateHierarchicalLayout() {
    // Simplified hierarchical layout: group by neuron type
    std::vector<NeuronVisualizationData> sensory, motor, internal;
    
    for (const auto& neuron : pImpl->neurons) {
        // This would need access to neuron type, simplified here
        if (neuron.id % 3 == 0) sensory.push_back(neuron);
        else if (neuron.id % 3 == 1) motor.push_back(neuron);
        else internal.push_back(neuron);
    }
    
    // Arrange in layers
    float y = 1.5f;
    float dy = 0.8f;
    
    for (const auto& n : sensory) {
        n.x = -1.5f; n.y = y; y -= dy;
    }
    
    y = 1.5f;
    for (const auto& n : motor) {
        n.x = 1.5f; n.y = y; y -= dy;
    }
    
    y = 1.5f;
    for (const auto& n : internal) {
        n.x = 0.0f; n.y = y; y -= dy;
    }
    
    // Update main neurons array
    pImpl->neurons.clear();
    pImpl->neurons.insert(pImpl->neurons.end(), sensory.begin(), sensory.end());
    pImpl->neurons.insert(pImpl->neurons.end(), motor.begin(), motor.end());
    pImpl->neurons.insert(pImpl->neurons.end(), internal.begin(), internal.end());
    
    for (const auto& neuron : pImpl->neurons) {
        pImpl->layerOrder.push_back(neuron.id);
    }
}

void VisualizationInterface::calculateGridLayout() {
    size_t cols = static_cast<size_t>(std::ceil(std::sqrt(pImpl->neurons.size())));
    size_t rows = static_cast<size_t>(std::ceil(static_cast<double>(pImpl->neurons.size()) / cols));
    
    float cellWidth = 3.0f / cols;
    float cellHeight = 2.0f / rows;
    
    for (size_t i = 0; i < pImpl->neurons.size(); ++i) {
        size_t row = i / cols;
        size_t col = i % cols;
        
        pImpl->neurons[i].x = -1.5f + (col + 0.5f) * cellWidth;
        pImpl->neurons[i].y = 1.0f - (row + 0.5f) * cellHeight;
        pImpl->layerOrder.push_back(pImpl->neurons[i].id);
    }
}

void VisualizationInterface::calculateRandomLayout() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(-1.5f, 1.5f);
    std::uniform_real_distribution<float> distY(-1.0f, 1.0f);
    
    for (auto& neuron : pImpl->neurons) {
        neuron.x = distX(gen);
        neuron.y = distY(gen);
        pImpl->layerOrder.push_back(neuron.id);
    }
}

void VisualizationInterface::calculateBiologicalLayout() {
    // Simplified biological layout: cluster by similarity
    if (pImpl->neurons.size() < 3) {
        calculateCircularLayout();
        return;
    }
    
    // Find neuron with highest firing rate as center
    size_t centerIndex = 0;
    float maxFiring = 0.0f;
    for (size_t i = 0; i < pImpl->neurons.size(); ++i) {
        if (pImpl->neurons[i].firingRate > maxFiring) {
            maxFiring = pImpl->neurons[i].firingRate;
            centerIndex = i;
        }
    }
    
    // Arrange neurons in concentric circles based on firing rate
    for (size_t i = 0; i < pImpl->neurons.size(); ++i) {
        float distance = (pImpl->neurons[i].firingRate / maxFiring) * 2.0f;
        float angle = static_cast<float>(i) * 2.0f * 3.14159f / static_cast<float>(pImpl->neurons.size());
        
        pImpl->neurons[i].x = distance * cos(angle);
        pImpl->neurons[i].y = distance * sin(angle);
        pImpl->layerOrder.push_back(pImpl->neurons[i].id);
    }
}

void VisualizationInterface::drawLine(int x1, int y1, int x2, int y2, const std::string& ch) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        if (x1 >= 0 && x1 < pImpl->bufferWidth && y1 >= 0 && y1 < pImpl->bufferHeight) {
            pImpl->renderBuffer[y1][x1] = ch[0];
        }
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void VisualizationInterface::drawCircle(int cx, int cy, int radius, const std::string& ch) {
    int x = 0;
    int y = radius;
    int err = 2 * radius - 1;
    
    while (y >= x) {
        // Plot eight points
        if (cx + x >= 0 && cx + x < pImpl->bufferWidth && cy + y >= 0 && cy + y < pImpl->bufferHeight) {
            pImpl->renderBuffer[cy + y][cx + x] = ch[0];
        }
        if (cx - x >= 0 && cx - x < pImpl->bufferWidth && cy + y >= 0 && cy + y < pImpl->bufferHeight) {
            pImpl->renderBuffer[cy + y][cx - x] = ch[0];
        }
        if (cx + x >= 0 && cx + x < pImpl->bufferWidth && cy - y >= 0 && cy - y < pImpl->bufferHeight) {
            pImpl->renderBuffer[cy - y][cx + x] = ch[0];
        }
        if (cx - x >= 0 && cx - x < pImpl->bufferWidth && cy - y >= 0 && cy - y < pImpl->bufferHeight) {
            pImpl->renderBuffer[cy - y][cx - x] = ch[0];
        }
        
        if (err < 0) {
            y--;
            err += 2 * (y - x) + 1;
        }
        x++;
        err -= 2 * (x - y) + 1;
    }
}

void VisualizationInterface::clearRenderBuffer() {
    for (auto& row : pImpl->renderBuffer) {
        std::fill(row.begin(), row.end(), " ");
    }
}

void VisualizationInterface::handleMouseClick(double x, double y, bool controlKey) {
    // Convert screen coordinates to buffer coordinates
    int bufferX = static_cast<int>((x / 2.0 + 1.0) * pImpl->bufferWidth / 2.0);
    int bufferY = static_cast<int>((-y / 2.0 + 1.0) * pImpl->bufferHeight / 2.0);
    
    if (bufferX >= 0 && bufferX < pImpl->bufferWidth && bufferY >= 0 && bufferY < pImpl->bufferHeight) {
        if (controlKey) {
            // Add to selection
            for (const auto& neuron : pImpl->neurons) {
                // Simple proximity check
                int nx = static_cast<int>((neuron.x + 1.0) * pImpl->bufferWidth / 2.0);
                int ny = static_cast<int>((neuron.y + 1.0) * pImpl->bufferHeight / 2.0);
                if (std::abs(nx - bufferX) <= 2 && std::abs(ny - bufferY) <= 2) {
                    if (std::find(pImpl->selectedNeurons.begin(), pImpl->selectedNeurons.end(), neuron.id) == pImpl->selectedNeurons.end()) {
                        pImpl->selectedNeurons.push_back(neuron.id);
                    }
                }
            }
        } else {
            // Clear selection and select only this neuron
            pImpl->selectedNeurons.clear();
            // Find and select neuron (simplified logic)
        }
    }
}

void VisualizationInterface::handleKeyPress(int key, int action) {
    if (action == 1) {  // Key press
        switch (key) {
            case 256:  // ESC key
                pImpl->selectedNeurons.clear();
                pImpl->selectedSynapses.clear();
                break;
            case 265:  // Arrow up
                pImpl->viewY += 0.1;
                break;
            case 264:  // Arrow down
                pImpl->viewY -= 0.1;
                break;
            case 263:  // Arrow left
                pImpl->viewX -= 0.1;
                break;
            case 262:  // Arrow right
                pImpl->viewX += 0.1;
                break;
        }
    }
}

std::vector<NeuronId> VisualizationInterface::getSelectedNeurons() const {
    return pImpl->selectedNeurons;
}

std::vector<SynapseId> VisualizationInterface::getSelectedSynapses() const {
    return pImpl->selectedSynapses;
}

void VisualizationInterface::printDebugInfo() const {
    std::cout << "=== Visualization Debug Info ===" << std::endl;
    std::cout << "Mode: ";
    switch (pImpl->mode) {
        case VisualizationMode::NetworkGraph: std::cout << "Network Graph"; break;
        case VisualizationMode::SpikeRaster: std::cout << "Spike Raster"; break;
        case VisualizationMode::WeightMatrix: std::cout << "Weight Matrix"; break;
        case VisualizationMode::ActivityHeatmap: std::cout << "Activity Heatmap"; break;
        default: std::cout << "Unknown"; break;
    }
    std::cout << std::endl;
    std::cout << "Neurons: " << pImpl->neurons.size() << std::endl;
    std::cout << "Synapses: " << pImpl->synapses.size() << std::endl;
    std::cout << "Spikes: " << pImpl->spikes.size() << std::endl;
    std::cout << "FPS: " << pImpl->perf.fps << std::endl;
    std::cout << "Frame time: " << pImpl->perf.averageFrameTime << " ms" << std::endl;
}

void VisualizationInterface::logVisualizationStats() const {
    NLM_LOG_INFO("Visualization Statistics: " + std::to_string(pImpl->neurons.size()) + " neurons, " +
                 std::to_string(pImpl->synapses.size()) + " synapses, " + std::to_string(pImpl->perf.framesRendered) + " frames rendered");
    if (pImpl->perf.framesRendered > 0) {
        NLM_LOG_INFO("Performance: " + std::to_string(pImpl->perf.fps) + " FPS, " + std::to_string(pImpl->perf.averageFrameTime) + " ms average frame time");
    }
}

// TextVisualizer implementation
struct TextVisualizer::Impl {
    std::vector<std::string> buffer;
    int width, height;
    std::shared_ptr<class Brain> currentBrain;
};

TextVisualizer::TextVisualizer() : pImpl(std::make_unique<Impl>()) {
    pImpl->width = 80;
    pImpl->height = 40;
    pImpl->buffer.resize(pImpl->height, std::string(pImpl->width, ' ');
}

void TextVisualizer::setBrain(const class Brain& brain) {
    pImpl->currentBrain = std::make_shared<Brain>(brain);
    // Generate simple text visualization
    renderNetworkGraph();
}

void TextVisualizer::renderNetworkGraph() {
    if (!pImpl->currentBrain) return;
    
    // Clear buffer
    for (auto& row : pImpl->buffer) {
        std::fill(row.begin(), row.end(), ' ');
    }
    
    // Add title
    std::string title = "NLM Network Visualization";
    int titleX = (pImpl->width - title.length()) / 2;
    if (titleX >= 0 && titleX + title.length() < pImpl->width) {
        for (size_t i = 0; i < title.length(); ++i) {
            pImpl->buffer[2][titleX + i] = title[i];
        }
    }
    
    // Add neuron count
    std::string neuronInfo = "Neurons: " + std::to_string(pImpl->currentBrain->getTotalNeuronCount());
    int infoX = (pImpl->width - neuronInfo.length()) / 2;
    if (infoX >= 0 && infoX + neuronInfo.length() < pImpl->width) {
        for (size_t i = 0; i < neuronInfo.length(); ++i) {
            pImpl->buffer[3][infoX + i] = neuronInfo[i];
        }
    }
    
    // Add synapse count
    std::string synapseInfo = "Synapses: " + std::to_string(pImpl->currentBrain->getTotalSynapseCount());
    int synapseInfoX = (pImpl->width - synapseInfo.length()) / 2;
    if (synapseInfoX >= 0 && synapseInfoX + synapseInfo.length() < pImpl->width) {
        for (size_t i = 0; i < synapseInfo.length(); ++i) {
            pImpl->buffer[4][synapseInfoX + i] = synapseInfo[i];
        }
    }
    
    // Add spike count
    std::string spikeInfo = "Spikes: " + std::to_string(pImpl->currentBrain->getTotalSpikeCount());
    int spikeInfoX = (pImpl->width - spikeInfo.length()) / 2;
    if (spikeInfoX >= 0 && spikeInfoX + spikeInfo.length() < pImpl->width) {
        for (size_t i = 0; i < spikeInfo.length(); ++i) {
            pImpl->buffer[5][spikeInfoX + i] = spikeInfo[i];
        }
    }
}

void TextVisualizer::renderSpikeRaster() {
    // Simple spike raster visualization
    clearBuffer();
    
    std::string title = "Spike Raster Plot";
    int titleX = (pImpl->width - title.length()) / 2;
    if (titleX >= 0 && titleX + title.length() < pImpl->width) {
        for (size_t i = 0; i < title.length(); ++i) {
            pImpl->buffer[2][titleX + i] = title[i];
        }
    }
    
    // Add spike histogram (simplified)
    size_t spikeCount = pImpl->currentBrain->getTotalSpikeCount();
    size_t maxHeight = pImpl->height - 5;
    size_t barWidth = 5;
    
    for (size_t i = 0; i < spikeCount && i < maxHeight; ++i) {
        for (size_t x = 0; x < barWidth && x + i * barWidth < pImpl->width; ++x) {
            pImpl->buffer[maxHeight - i][i * barWidth + x] = '*';
        }
    }
}

void TextVisualizer::renderWeightMatrix() {
    clearBuffer();
    
    std::string title = "Weight Matrix";
    int titleX = (pImpl->width - title.length()) / 2;
    if (titleX >= 0 && titleX + title.length() < pImpl->width) {
        for (size_t i = 0; i < title.length(); ++i) {
            pImpl->buffer[2][titleX + i] = title[i];
        }
    }
    
    // Add weight matrix visualization (simplified)
    std::string weightInfo = "Weight statistics will be displayed here.";
    int infoX = (pImpl->width - weightInfo.length()) / 2;
    if (infoX >= 0 && infoX + weightInfo.length() < pImpl->width) {
        for (size_t i = 0; i < weightInfo.length(); ++i) {
            pImpl->buffer[3][infoX + i] = weightInfo[i];
        }
    }
}

void TextVisualizer::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        for (const auto& row : pImpl->buffer) {
            file << row << std::endl;
        }
    }
}

std::string TextVisualizer::getOutput() const {
    std::ostringstream oss;
    for (const auto& row : pImpl->buffer) {
        oss << row << std::endl;
    }
    return oss.str();
}

void TextVisualizer::clearBuffer() {
    for (auto& row : pImpl->buffer) {
        std::fill(row.begin(), row.end(), ' ');
    }
}

// Convenience functions
void createStandardVisualization(const std::string& title, const class Brain& brain) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Creating standard visualization: " + title);
    // Implementation would create and display visualization
}

void createNetworkVisualization(const class Brain& brain, const std::string& outputFile) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Creating network visualization: " + outputFile);
    // Implementation would create network visualization
}

void createSpikeVisualization(const std::vector<SpikeVisualizationData>& spikes, const std::string& outputFile) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Creating spike visualization: " + outputFile);
    // Implementation would create spike visualization
}

void createWeightMatrixVisualization(const class Brain& brain, const std::string& outputFile) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Creating weight matrix visualization: " + outputFile);
    // Implementation would create weight matrix visualization
}

void createActivityHeatmap(const std::vector<float>& activityProfile, const std::string& outputFile) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Creating activity heatmap: " + outputFile);
    // Implementation would create activity heatmap
}

} // namespace nlm