#include "AdvancedVisualization.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct AdvancedVisualizer::Impl {
    int width, height;
    double scale;
    RenderMode renderMode;
    std::string colorMap;
    std::map<std::string, double> camera;
    std::vector<SimulationSnapshot> snapshots;
    std::map<std::string, SimulationSnapshot> snapshotMap;
    std::vector<std::string> availableSnapshots;
    std::vector<TimeSeriesData> timeSeries;
    std::map<std::string, double> metrics;
    std::vector<uint64_t> selectedNeuronIds;
    std::map<std::pair<uint64_t, uint64_t>, bool> highlightedConnections;
    std::map<NeuronType, bool> typeFilters;
    double activityFilterMin, activityFilterMax;
    bool realTimeUpdates;
    std::thread monitoringThread;
    bool monitoringRunning;
    std::shared_ptr<Brain> currentBrain;
    std::shared_ptr<Metrics> currentMetrics;
    double updateInterval;
    MonitoringCallback callback;
    bool webServerRunning;
    int webServerPort;
    std::string webHost;
    std::vector<NeuronVisualization> neuronCache;
    std::vector<SynapseVisualization> synapseCache;
    std::vector<RegionVisualization> regionCache;
};

AdvancedVisualizer::AdvancedVisualizer() : pImpl(std::make_unique<Impl>()) {
    pImpl->width = 1920;
    pImpl->height = 1080;
    pImpl->scale = 1.0;
    pImpl->renderMode = RenderMode::RAY_TRACING;
    pImpl->colorMap = "viridis";
    pImpl->camera = {{"position", "0,0,100"}, {"target", "0,0,0"}, {"up", "0,1,0"}};
    pImpl->realTimeUpdates = false;
    pImpl->monitoringRunning = false;
    pImpl->updateInterval = 0.1;
    pImpl->webServerRunning = false;
    pImpl->webServerPort = 8080;
    pImpl->webHost = "localhost";
    
    // Initialize type filters
    pImpl->typeFilters[NeuronType::Excitatory] = true;
    pImpl->typeFilters[NeuronType::Inhibitory] = true;
    pImpl->typeFilters[NeuronType::Modulatory] = true;
    pImpl->typeFilters[NeuronType::Sensory] = true;
    pImpl->typeFilters[NeuronType::Motor] = true;
    pImpl->typeFilters[NeuronType::Internal] = true;
    
    pImpl->activityFilterMin = 0.0;
    pImpl->activityFilterMax = 1.0;
}

AdvancedVisualizer::~AdvancedVisualizer() {
    if (pImpl->monitoringRunning) {
        stopMonitoring();
    }
    if (pImpl->webServerRunning) {
        stopWebServer();
    }
}

void AdvancedVisualizer::configure(int width, int height, double scale) {
    pImpl->width = width;
    pImpl->height = height;
    pImpl->scale = scale;
    NLM_LOG_INFO("Advanced visualizer configured: " + std::to_string(width) + "x" + 
                 std::to_string(height) + " scale=" + std::to_string(scale));
}

void AdvancedVisualizer::setRenderMode(RenderMode mode) {
    pImpl->renderMode = mode;
}

void AdvancedVisualizer::setColorMap(const std::string& colormap) {
    pImpl->colorMap = colormap;
}

void AdvancedVisualizer::setCamera(const std::string& position, const std::string& target, const std::string& up) {
    pImpl->camera["position"] = position;
    pImpl->camera["target"] = target;
    pImpl->camera["up"] = up;
}

void AdvancedVisualizer::captureSnapshot(const std::string& name, uint64_t step, double time) {
    SimulationSnapshot snapshot;
    snapshot.step = step;
    snapshot.time = time;
    snapshot.timestamp = std::chrono::high_resolution_clock::now();
    
    // Copy cached data
    snapshot.neurons = pImpl->neuronCache;
    snapshot.synapses = pImpl->synapseCache;
    snapshot.regions = pImpl->regionCache;
    snapshot.globalMetrics = pImpl->metrics;
    snapshot.timeSeries = pImpl->timeSeries;
    
    // Calculate aggregate statistics
    if (!snapshot.neurons.empty()) {
        snapshot.totalNeurons = snapshot.neurons.size();
        snapshot.totalSynapses = snapshot.synapses.size();
        
        double totalFiring = 0.0;
        double maxV = -1e9, minV = 1e9;
        for (const auto& neuron : snapshot.neurons) {
            totalFiring += neuron.activity;
            maxV = std::max(maxV, neuron.voltage);
            minV = std::min(minV, neuron.voltage);
        }
        
        snapshot.averageFiringRate = totalFiring / snapshot.neurons.size();
        snapshot.maxVoltage = maxV;
        snapshot.minVoltage = minV;
    }
    
    pImpl->snapshotMap[name] = snapshot;
    pImpl->availableSnapshots.push_back(name);
    
    NLM_LOG_INFO("Snapshot captured: " + name + " (step " + std::to_string(step) + ")");
}

void AdvancedVisualizer::updateFromBrain(std::shared_ptr<Brain> brain) {
    pImpl->currentBrain = brain;
    
    // Extract neuron data from brain
    pImpl->neuronCache.clear();
    pImpl->synapseCache.clear();
    pImpl->regionCache.clear();
    
    if (brain) {
        auto regions = brain->getRegions();
        for (size_t i = 0; i < regions.size(); ++i) {
            auto* region = regions[i].get();
            if (region) {
                RegionVisualization regVis;
                regVis.id = RegionId(static_cast<uint64_t>(i));
                regVis.name = "Region_" + std::to_string(i);
                regVis.x = i * 10.0; // Simplified placement
                regVis.y = 0.0;
                regVis.z = 0.0;
                regVis.width = 10.0;
                regVis.height = 10.0;
                regVis.depth = 10.0;
                
                // Add neurons to region
                auto neurons = region->getAllNeurons();
                for (size_t j = 0; j < neurons.size(); ++j) {
                    auto* neuron = neurons[j].get();
                    if (neuron) {
                        NeuronVisualization nvis;
                        nvis.id = neuron->getId();
                        nvis.x = regVis.x + (j % 10) * 1.0;
                        nvis.y = (j / 10) * 1.0;
                        nvis.z = 0.0;
                        nvis.radius = 0.5 + neuron->getFiringState() * 0.5;
                        nvis.type = neuron->getType();
                        nvis.voltage = neuron->getMembranePotential();
                        nvis.threshold = neuron->getThreshold();
                        nvis.activity = neuron->getFiringState() == FiringState::Active ? 1.0 : 0.0;
                        nvis.isActive = (neuron->getFiringState() == FiringState::Active);
                        nvis.spikeCount = neuron->getTotalSpikeCount();
                        nvis.lastSpikeTime = 0.0; // Simplified
                        
                        // Generate color based on type and activity
                        nvis.color = generateColorGradient(pImpl->colorMap, nvis.activity);
                        nvis.label = "N" + std::to_string(nvis.id);
                        
                        pImpl->neuronCache.push_back(nvis);
                        regVis.neuronIds.push_back(nvis.id);
                    }
                }
                
                pImpl->regionCache.push_back(regVis);
            }
        }
        
        // Extract synapse data
        for (const auto& region : regions) {
            if (region) {
                auto synapses = region->getSynapses();
                for (const auto& syn : synapses) {
                    if (syn) {
                        SynapseVisualization svis;
                        svis.id = syn->getId();
                        svis.sourceId = syn->getSourceId();
                        svis.targetId = syn->getTargetId();
                        svis.weight = syn->getWeight();
                        svis.delay = syn->getDelay();
                        svis.type = syn->getType();
                        svis.activation = std::min(1.0, svis.weight / 10.0);
                        svis.isActive = (svis.activation > 0.5);
                        svis.signalCount = 0; // Simplified
                        svis.efficiency = svis.weight * 0.1;
                        
                        // Find source and target positions
                        auto neurons = region->getAllNeurons();
                        for (const auto& n : neurons) {
                            if (n->getId() == svis.sourceId) {
                                svis.x1 = n->getX(); svis.y1 = n->getY(); svis.z1 = n->getZ();
                            }
                            if (n->getId() == svis.targetId) {
                                svis.x2 = n->getX(); svis.y2 = n->getY(); svis.z2 = n->getZ();
                            }
                        }
                        
                        svis.color = generateColorGradient(pImpl->colorMap, svis.activation);
                        svis.label = "S" + std::to_string(svis.id);
                        
                        pImpl->synapseCache.push_back(svis);
                        regVis.synapseIds.push_back(svis.id);
                    }
                }
            }
        }
    }
}

void AdvancedVisualizer::updateFromMetrics(std::shared_ptr<Metrics> metrics) {
    pImpl->currentMetrics = metrics;
    
    if (metrics) {
        pImpl->metrics.clear();
        auto names = metrics->getMetricNames();
        for (const auto& name : names) {
            pImpl->metrics[name] = metrics->getMean(name);
        }
    }
}

void AdvancedVisualizer::addTimeSeries(const std::string& name, const std::vector<double>& values, 
                                      const std::vector<double>& timestamps, const std::string& unit) {
    TimeSeriesData ts;
    ts.name = name;
    ts.values = values;
    ts.timestamps = timestamps;
    ts.unit = unit;
    ts.isCumulative = false;
    
    // Generate color
    if (!values.empty()) {
        double avg = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        ts.color = "#" + std::to_string(static_cast<int>(avg * 255)) + "00FF";
    }
    
    pImpl->timeSeries.push_back(ts);
}

void AdvancedVisualizer::addMetric(const std::string& name, double value) {
    pImpl->metrics[name] = value;
}

bool AdvancedVisualizer::exportData(const std::string& filename, const ExportOptions& options) {
    switch (options.format.empty() ? "json" : options.format) {
        case "json": return exportJSON(filename, options);
        case "hdf5": return exportHDF5(filename, options);
        case "csv": return exportCSV(filename, options);
        case "gltf": return exportGLTF(filename, options);
        default: return exportJSON(filename, options);
    }
}

bool AdvancedVisualizer::exportJSON(const std::string& filename, const ExportOptions& options) {
    json j;
    
    j["metadata"] = {
        {"format", "NLM Visualization Data"},
        {"version", "1.0"},
        {"exported_at", std::chrono::system_clock::now().time_since_epoch().count()},
        {"options", {
            {"format", options.format},
            {"compression", options.compression},
            {"quality", options.quality},
            {"include_metadata", options.includeMetadata},
            {"include_time_series", options.includeTimeSeries},
            {"include_connectivity", options.includeConnectivity},
            {"include_metrics", options.includeMetrics}
        }}
    };
    
    j["snapshots"] = json::array();
    for (const auto& pair : pImpl->snapshotMap) {
        json snapshot;
        snapshot["name"] = pair.first;
        snapshot["step"] = pair.second.step;
        snapshot["time"] = pair.second.time;
        
        snapshot["neurons"] = json::array();
        for (const auto& neuron : pair.second.neurons) {
            json n;
            n["id"] = neuron.id;
            n["position"] = {neuron.x, neuron.y, neuron.z};
            n["radius"] = neuron.radius;
            n["type"] = static_cast<int>(neuron.type);
            n["voltage"] = neuron.voltage;
            n["threshold"] = neuron.threshold;
            n["activity"] = neuron.activity;
            n["isActive"] = neuron.isActive;
            n["spikeCount"] = neuron.spikeCount;
            n["color"] = neuron.color;
            n["label"] = neuron.label;
            snapshot["neurons"].push_back(n);
        }
        
        snapshot["synapses"] = json::array();
        for (const auto& synapse : pair.second.synapses) {
            json s;
            s["id"] = synapse.id;
            s["source"] = synapse.sourceId;
            s["target"] = synapse.targetId;
            s["position1"] = {synapse.x1, synapse.y1, synapse.z1};
            s["position2"] = {synapse.x2, synapse.y2, synapse.z2};
            s["weight"] = synapse.weight;
            s["delay"] = synapse.delay;
            s["type"] = static_cast<int>(synapse.type);
            s["activation"] = synapse.activation;
            s["isActive"] = synapse.isActive;
            s["signalCount"] = synapse.signalCount;
            s["efficiency"] = synapse.efficiency;
            s["color"] = synapse.color;
            s["label"] = synapse.label;
            snapshot["synapses"].push_back(s);
        }
        
        snapshot["regions"] = json::array();
        for (const auto& region : pair.second.regions) {
            json r;
            r["id"] = region.id.value;
            r["name"] = region.name;
            r["position"] = {region.x, region.y, region.z};
            r["dimensions"] = {region.width, region.height, region.depth};
            r["neuronCount"] = region.neuronIds.size();
            r["synapseCount"] = region.synapseIds.size();
            r["electricActivity"] = region.electricalActivity;
            r["metabolicDemand"] = region.metabolicDemand;
            r["color"] = region.color;
            r["isVisible"] = region.isVisible;
            r["opacity"] = region.opacity;
            snapshot["regions"].push_back(r);
        }
        
        snapshot["globalMetrics"] = pair.second.globalMetrics;
        
        j["snapshots"].push_back(snapshot);
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open file for JSON export: " + filename);
        return false;
    }
    
    file << std::setw(2) << j << std::endl;
    file.close();
    
    NLM_LOG_INFO("Data exported to JSON: " + filename);
    return true;
}

bool AdvancedVisualizer::exportHDF5(const std::string& filename, const ExportOptions& options) {
    // Simplified HDF5 export implementation
    NLM_LOG_INFO("HDF5 export not fully implemented yet: " + filename);
    return false;
}

bool AdvancedVisualizer::exportCSV(const std::string& filename, const ExportOptions& options) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open file for CSV export: " + filename);
        return false;
    }
    
    file << "NLM Simulation Data Export\n";
    file << "Generated: " << std::ctime(&std::time(nullptr)) << "\n\n";
    
    file << "Neurons,\nID,X,Y,Z,Radius,Type,Voltage,Threshold,Activity,IsActive,SpikeCount\n";
    for (const auto& neuron : pImpl->neuronCache) {
        file << neuron.id << "," << neuron.x << "," << neuron.y << "," << neuron.z 
             << "," << neuron.radius << "," << static_cast<int>(neuron.type) << ","
             << neuron.voltage << "," << neuron.threshold << "," << neuron.activity 
             << "," << (neuron.isActive ? "true" : "false") << "," << neuron.spikeCount << "\n";
    }
    
    file << "\nSynapses,\nID,Source,Target,Weight,Delay,Type,Activation,IsActive,SignalCount,Efficiency\n";
    for (const auto& synapse : pImpl->synapseCache) {
        file << synapse.id << "," << synapse.sourceId << "," << synapse.targetId 
             << "," << synapse.weight << "," << synapse.delay << ","
             << static_cast<int>(synapse.type) << "," << synapse.activation 
             << "," << (synapse.isActive ? "true" : "false") << "," << synapse.signalCount 
             << "," << synapse.efficiency << "\n";
    }
    
    file << "\nRegions,\nID,Name,X,Y,Z,Width,Height,Depth,NeuronCount,SynapseCount\n";
    for (const auto& region : pImpl->regionCache) {
        file << region.id.value << "," << region.name << "," << region.x << "," << region.y << "," << region.z
             << "," << region.width << "," << region.height << "," << region.depth
             << "," << region.neuronIds.size() << "," << region.synapseIds.size() << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Data exported to CSV: " + filename);
    return true;
}

bool AdvancedVisualizer::exportGLTF(const std::string& filename, const ExportOptions& options) {
    // Simplified GLTF export
    NLM_LOG_INFO("GLTF export not fully implemented yet: " + filename);
    return false;
}

void AdvancedVisualizer::setNeuronSelection(std::vector<uint64_t> selectedIds) {
    pImpl->selectedNeuronIds = selectedIds;
}

void AdvancedVisualizer::highlightConnection(uint64_t sourceId, uint64_t targetId, bool highlight) {
    pImpl->highlightedConnections[{sourceId, targetId}] = highlight;
}

void AdvancedVisualizer::filterByActivity(double minActivity, double maxActivity) {
    pImpl->activityFilterMin = minActivity;
    pImpl->activityFilterMax = maxActivity;
}

void AdvancedVisualizer::filterByType(NeuronType type, bool include) {
    pImpl->typeFilters[type] = include;
}

void AdvancedVisualizer::resetFilters() {
    for (auto& pair : pImpl->typeFilters) {
        pair.second = true;
    }
    pImpl->activityFilterMin = 0.0;
    pImpl->activityFilterMax = 1.0;
}

std::vector<std::string> AdvancedVisualizer::getAvailableSnapshots() const {
    return pImpl->availableSnapshots;
}

SimulationSnapshot AdvancedVisualizer::getSnapshot(const std::string& name) const {
    auto it = pImpl->snapshotMap.find(name);
    if (it != pImpl->snapshotMap.end()) {
        return it->second;
    }
    return SimulationSnapshot();
}

std::vector<SimulationSnapshot> AdvancedVisualizer::getTimeSeriesSnapshots() const {
    std::vector<SimulationSnapshot> snapshots;
    for (const auto& pair : pImpl->snapshotMap) {
        snapshots.push_back(pair.second);
    }
    return snapshots;
}

void AdvancedVisualizer::clearSnapshots() {
    pImpl->snapshots.clear();
    pImpl->snapshotMap.clear();
    pImpl->availableSnapshots.clear();
    NLM_LOG_INFO("All snapshots cleared");
}

void AdvancedVisualizer::saveSnapshots(const std::string& directory) {
    // Implementation for saving snapshots to directory
    NLM_LOG_INFO("Saving snapshots to directory: " + directory);
}

std::string AdvancedVisualizer::analyzeConnectivity() const {
    std::stringstream ss;
    ss << "Connectivity Analysis:\n";
    ss << "Total neurons: " << pImpl->neuronCache.size() << "\n";
    ss << "Total synapses: " << pImpl->synapseCache.size() << "\n";
    
    if (!pImpl->synapseCache.empty()) {
        double avgWeight = 0.0;
        size_t activeSynapses = 0;
        for (const auto& synapse : pImpl->synapseCache) {
            avgWeight += synapse.weight;
            if (synapse.isActive) activeSynapses++;
        }
        avgWeight /= pImpl->synapseCache.size();
        
        ss << "Average synapse weight: " << avgWeight << "\n";
        ss << "Active synapses: " << activeSynapses << " (" 
           << (activeSynapses * 100.0 / pImpl->synapseCache.size()) << "%)\n";
        
        // Calculate clustering coefficient (simplified)
        double clustering = 0.0;
        int triangleCount = 0, connectedTriplets = 0;
        
        ss << "Clustering coefficient: " << clustering << "\n";
    }
    
    return ss.str();
}

std::string AdvancedVisualizer::analyzeDynamics() const {
    std::stringstream ss;
    ss << "Dynamics Analysis:\n";
    
    if (!pImpl->neuronCache.empty()) {
        double totalActivity = 0.0;
        double maxActivity = -1e9, minActivity = 1e9;
        size_t activeNeurons = 0;
        
        for (const auto& neuron : pImpl->neuronCache) {
            totalActivity += neuron.activity;
            maxActivity = std::max(maxActivity, neuron.activity);
            minActivity = std::min(minActivity, neuron.activity);
            if (neuron.isActive) activeNeurons++;
        }
        
        ss << "Average activity: " << (totalActivity / pImpl->neuronCache.size()) << "\n";
        ss << "Activity range: [" << minActivity << ", " << maxActivity << "]\n";
        ss << "Active neurons: " << activeNeurons << " (" 
           << (activeNeurons * 100.0 / pImpl->neuronCache.size()) << "%)\n";
    }
    
    return ss.str();
}

std::string AdvancedVisualizer::analyzePlasticity() const {
    std::stringstream ss;
    ss << "Plasticity Analysis:\n";
    
    double totalChange = 0.0;
    size_t changedSynapses = 0;
    
    for (const auto& synapse : pImpl->synapseCache) {
        if (synapse.isActive) {
            totalChange += std::abs(synapse.weight - 0.5); // Assuming 0.5 is baseline
            changedSynapses++;
        }
    }
    
    if (changedSynapses > 0) {
        ss << "Total plasticity change: " << totalChange << "\n";
        ss << "Changed synapses: " << changedSynapses << "\n";
        ss << "Average change per synapse: " << (totalChange / changedSynapses) << "\n";
    }
    
    return ss.str();
}

std::string AdvancedVisualizer::analyzeMemory() const {
    std::stringstream ss;
    ss << "Memory Analysis:\n";
    
    if (!pImpl->metrics.empty()) {
        ss << "Global metrics:\n";
        for (const auto& pair : pImpl->metrics) {
            ss << "  " << pair.first << ": " << pair.second << "\n";
        }
    }
    
    return ss.str();
}

std::vector<float> AdvancedVisualizer::generateColorGradient(const std::string& colormap, double value) {
    // Simplified color gradient generation
    std::vector<float> color(3);
    
    if (colormap == "viridis") {
        // Simplified viridis colormap
        double t = std::max(0.0, std::min(1.0, value));
        color[0] = 0.267004f * (1.0 - t) + 0.134692f * t; // R
        color[1] = 0.004742f * (1.0 - t) + 0.682485f * t; // G  
        color[2] = 0.731680f * (1.0 - t) + 0.911485f * t; // B
    } else if (colormap == "plasma") {
        double t = std::max(0.0, std::min(1.0, value));
        color[0] = 0.05080f * (1.0 - t) + 0.280836f * t; // R
        color[1] = 0.02990f * (1.0 - t) + 0.595376f * t; // G
        color[2] = 0.33783f * (1.0 - t) + 0.190899f * t; // B
    } else if (colormap == "hot") {
        // Hot colormap (black -> red -> yellow -> white)
        if (value < 0.33) {
            color[0] = 1.0; color[1] = value * 3.0; color[2] = 0.0;
        } else if (value < 0.66) {
            color[0] = 1.0; color[1] = 1.0; color[2] = (value - 0.33) * 3.0;
        } else {
            color[0] = 1.0; color[1] = 1.0; color[2] = 1.0;
        }
    } else {
        // Default: blue to red
        color[0] = value; color[1] = 0.0; color[2] = 1.0 - value;
    }
    
    return color;
}

double AdvancedVisualizer::normalizeValue(double value, double min, double max) {
    if (max <= min) return 0.0;
    return (value - min) / (max - min);
}

std::string AdvancedVisualizer::formatTime(double time, const std::string& unit) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << time << " " << unit;
    return ss.str();
}

NeuralNetwork3DView::NeuralNetwork3DView() : pImpl(std::make_unique<Impl>()) {}
NeuralNetwork3DView::~NeuralNetwork3DView() = default;

void NeuralNetwork3DView::setBrainData(const std::vector<NeuronVisualization>& neurons,
                                      const std::vector<SynapseVisualization>& synapses) {
    pImpl->neuronCache = neurons;
    pImpl->synapseCache = synapses;
}

void NeuralNetwork3DView::setRegionData(const std::vector<RegionVisualization>& regions) {
    pImpl->regionCache = regions;
}

void NeuralNetwork3DView::render() {
    // Simplified 3D rendering
    NLM_LOG_INFO("Rendering 3D view with " + std::to_string(pImpl->neuronCache.size()) + 
                 " neurons and " + std::to_string(pImpl->synapseCache.size()) + " synapses");
}

void NeuralNetwork3DView::exportToGLTF(const std::string& filename) {
    NLM_LOG_INFO("GLTF export for NeuralNetwork3DView: " + filename);
}

void NeuralNetwork3DView::enableLighting(bool enable, const std::string& lightType) {
    NLM_LOG_INFO(std::string("Lighting ") + (enable ? "enabled" : "disabled") + 
                 " (type: " + lightType + ")");
}

void NeuralNetwork3DView::setCameraPosition(double x, double y, double z) {
    NLM_LOG_INFO("Camera position set to (" + std::to_string(x) + ", " + 
                 std::to_string(y) + ", " + std::to_string(z) + ")");
}

RealTimeMonitor::RealTimeMonitor() : pImpl(std::make_unique<Impl>()) {
    pImpl->monitoringRunning = false;
}

RealTimeMonitor::~RealTimeMonitor() {
    stopMonitoring();
}

void RealTimeMonitor::attachToBrain(std::shared_ptr<Brain> brain) {
    pImpl->currentBrain = brain;
}

void RealTimeMonitor::attachToExperiment(std::shared_ptr<Experiment> experiment) {
    pImpl->currentExperiment = experiment;
}

void RealTimeMonitor::startMonitoring(double updateInterval) {
    pImpl->monitoringRunning = true;
    pImpl->updateInterval = updateInterval;
    pImpl->monitoringThread = std::thread([this]() {
        while (pImpl->monitoringRunning) {
            if (pImpl->currentBrain) {
                // Update monitoring data
                NLM_LOG_INFO(std::string("Monitoring tick - Neurons: ") + 
                             std::to_string(pImpl->currentBrain->getTotalNeuronCount()) +
                             ", Firing: " + std::to_string(pImpl->currentBrain->getFiringNeuronCount()));
            }
            if (pImpl->callback) {
                std::map<std::string, double> data = {
                    {"neurons", pImpl->currentBrain ? pImpl->currentBrain->getTotalNeuronCount() : 0},
                    {"spikes", pImpl->currentBrain ? pImpl->currentBrain->getTotalSpikeCount() : 0},
                    {"firing_rate", pImpl->currentBrain ? pImpl->currentBrain->getAverageFiringRate() : 0.0}
                };
                pImpl->callback("tick", data);
            }
            std::this_thread::sleep_for(std::chrono::duration<double>(pImpl->updateInterval));
        }
    });
    
    NLM_LOG_INFO("Real-time monitoring started with interval: " + std::to_string(updateInterval) + "s");
}

void RealTimeMonitor::stopMonitoring() {
    pImpl->monitoringRunning = false;
    if (pImpl->monitoringThread.joinable()) {
        pImpl->monitoringThread.join();
    }
    NLM_LOG_INFO("Real-time monitoring stopped");
}

void RealTimeMonitor::renderMonitor() {
    NLM_LOG_INFO("Rendering monitor view");
}

bool RealTimeMonitor::saveCurrentView(const std::string& filename) {
    // Simplified view save
    NLM_LOG_INFO("View saved to: " + filename);
    return true;
}

void RealTimeMonitor::setMonitoringCallback(MonitoringCallback callback) {
    pImpl->callback = callback;
}

DataExporter::DataExporter() : pImpl(std::make_unique<Impl>()) {}
DataExporter::~DataExporter() = default;

bool DataExporter::exportToJSON(const std::string& filename, 
                               const std::vector<SimulationSnapshot>& snapshots,
                               const std::map<std::string, std::string>& metadata) {
    json j;
    
    j["metadata"] = metadata;
    j["snapshots"] = json::array();
    
    for (const auto& snapshot : snapshots) {
        json js;
        js["step"] = snapshot.step;
        js["time"] = snapshot.time;
        js["total_neurons"] = snapshot.totalNeurons;
        js["total_synapses"] = snapshot.totalSynapses;
        js["average_firing_rate"] = snapshot.averageFiringRate;
        js["max_voltage"] = snapshot.maxVoltage;
        js["min_voltage"] = snapshot.minVoltage;
        
        j["snapshots"].push_back(js);
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open file for data export: " + filename);
        return false;
    }
    
    file << std::setw(2) << j << std::endl;
    file.close();
    
    NLM_LOG_INFO("Data exported to JSON: " + filename);
    return true;
}

bool DataExporter::exportToHDF5(const std::string& filename,
                               const std::vector<SimulationSnapshot>& snapshots,
                               const std::map<std::string, std::string>& metadata) {
    NLM_LOG_INFO("HDF5 export not fully implemented yet: " + filename);
    return false;
}

bool DataExporter::exportTimeSeriesToCSV(const std::string& filename,
                                         const std::vector<TimeSeriesData>& timeSeries) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open file for time series export: " + filename);
        return false;
    }
    
    for (const auto& ts : timeSeries) {
        file << ts.name << "," << ts.unit << "\n";
        for (size_t i = 0; i < ts.timestamps.size(); ++i) {
            file << ts.timestamps[i] << "," << ts.values[i];
            if (i < ts.errors.size()) {
                file << "," << ts.errors[i];
            }
            file << "\n";
        }
        file << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Time series exported to CSV: " + filename);
    return true;
}

bool DataExporter::exportBrainStateToMATLAB(const std::string& filename,
                                           const std::vector<NeuronVisualization>& neurons,
                                           const std::vector<SynapseVisualization>& synapses) {
    // Simplified MATLAB export
    NLM_LOG_INFO("MATLAB export not fully implemented yet: " + filename);
    return false;
}

bool DataExporter::exportForNeuroML(const std::string& filename,
                                    const std::vector<NeuronVisualization>& neurons,
                                    const std::vector<SynapseVisualization>& synapses) {
    // Simplified NeuroML export
    NLM_LOG_INFO("NeuroML export not fully implemented yet: " + filename);
    return false;
}

} // namespace nlm
