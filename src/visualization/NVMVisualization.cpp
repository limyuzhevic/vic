#include "NVMVisualization.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct NVMVisualization::Impl {
    bool active;
    double updateRate;
    bool visible;
    bool realTimeUpdates;
    std::string theme;
    std::string quality;
    std::map<std::string, std::unique_ptr<VisualizationComponent>> visualizationModules;
    std::map<std::string, std::function<void(double)>> dataSources;
    NetworkTopology networkTopology;
    std::vector<MemoryTrace> memoryTraces;
    std::vector<NeuromodulationData> neuromodulationData;
    std::vector<PlasticityData> plasticityData;
    std::vector<DevelopmentData> developmentData;
    SystemIntegrationData systemIntegrationData;
    
    Impl() : active(false), updateRate(60.0), visible(true), realTimeUpdates(true), 
             theme("default"), quality("high") {}
};

NVMVisualization::NVMVisualization() : pImpl(std::make_unique<Impl>()) {
}

NVMVisualization::~NVMVisualization() = default;

bool NVMVisualization::initialize(const Brain& brain) {
    // Initialize the visualization with brain data
    pImpl->active = true;
    
    // Convert brain data to visualization data
    updateVisualizationData(brain);
    
    // Initialize visualization modules
    for (auto& pair : pImpl->visualizationModules) {
        pair.second->initialize();
    }
    
    return true;
}

void NVMVisualization::update(double deltaTime) {
    if (!pImpl->active || !pImpl->realTimeUpdates) {
        return;
    }
    
    // Update data sources
    for (auto& pair : pImpl->dataSources) {
        pair.second(deltaTime);
    }
    
    // Update visualization modules
    for (auto& pair : pImpl->visualizationModules) {
        pair.second->update(deltaTime);
    }
}

void NVMVisualization::render() {
    if (!pImpl->active || !pImpl->visible) {
        return;
    }
    
    // Render visualization modules
    for (auto& pair : pImpl->visualizationModules) {
        pair.second->render();
    }
}

void NVMVisualization::handleInput(const std::string& eventType, const std::map<std::string, float>& parameters) {
    if (!pImpl->active) {
        return;
    }
    
    // Handle input events for visualization modules
    for (auto& pair : pImpl->visualizationModules) {
        pair.second->handleInput(eventType, parameters);
    }
}

bool NVMVisualization::isActive() const {
    return pImpl->active;
}

void NVMVisualization::setVisible(bool visible) {
    pImpl->visible = visible;
}

std::string NVMVisualization::getName() const {
    return "NVMVisualization";
}

void NVMVisualization::getBounds(float& x, float& y, float& width, float& height) const {
    x = 0;
    y = 0;
    width = 1920;
    height = 1080;
}

void NVMVisualization::setBounds(float x, float y, float width, float height) {
    // In a real implementation, this would set the viewport bounds
}

void NVMVisualization::addVisualizationModule(std::unique_ptr<VisualizationComponent> module) {
    if (module) {
        std::string name = module->getName();
        pImpl->visualizationModules[name] = std::move(module);
    }
}

void NVMVisualization::removeVisualizationModule(const std::string& name) {
    pImpl->visualizationModules.erase(name);
}

VisualizationComponent* NVMVisualization::getVisualizationModule(const std::string& name) {
    auto it = pImpl->visualizationModules.find(name);
    if (it != pImpl->visualizationModules.end()) {
        return it->second.get();
    }
    return nullptr;
}

void NVMVisualization::updateVisualizationData(const Brain& brain) {
    // Update network topology from brain
    pImpl->networkTopology = convertBrainToNetworkTopology(brain);
    
    // Update memory traces
    pImpl->memoryTraces = convertBrainMemoryToMemoryTraces(brain);
    
    // Update neuromodulation data
    pImpl->neuromodulationData = convertBrainNeuromodulationToNeuromodulationData(brain);
    
    // Update plasticity data
    pImpl->plasticityData = convertBrainPlasticityToPlasticityData(brain);
    
    // Update development data
    pImpl->developmentData = convertBrainDevelopmentToDevelopmentData(brain);
    
    // Update system integration data
    pImpl->systemIntegrationData = calculateSystemIntegrationData();
}

bool NVMVisualization::exportData(const std::string& filename, const std::string& format) {
    try {
        if (format == "json") {
            return exportToJSON(filename);
        } else if (format == "csv") {
            return exportToCSV(filename);
        } else if (format == "xml") {
            return exportToXML(filename);
        } else if (format == "binary") {
            return exportToBinary(filename);
        } else {
            return false;
        }
    } catch (const std::exception& e) {
        return false;
    }
}

bool NVMVisualization::importData(const std::string& filename, const std::string& format) {
    try {
        if (format == "json") {
            return importFromJSON(filename);
        } else if (format == "csv") {
            return importFromCSV(filename);
        } else if (format == "xml") {
            return importFromXML(filename);
        } else if (format == "binary") {
            return importFromBinary(filename);
        } else {
            return false;
        }
    } catch (const std::exception& e) {
        return false;
    }
}

const NetworkTopology& NVMVisualization::getNetworkTopology() const {
    return pImpl->networkTopology;
}

const std::vector<MemoryTrace>& NVMVisualization::getMemoryTraces() const {
    return pImpl->memoryTraces;
}

const std::vector<NeuromodulationData>& NVMVisualization::getNeuromodulationData() const {
    return pImpl->neuromodulationData;
}

const std::vector<PlasticityData>& NVMVisualization::getPlasticityData() const {
    return pImpl->plasticityData;
}

const std::vector<DevelopmentData>& NVMVisualization::getDevelopmentData() const {
    return pImpl->developmentData;
}

const SystemIntegrationData& NVMVisualization::getSystemIntegrationData() const {
    return pImpl->systemIntegrationData;
}

void NVMVisualization::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double NVMVisualization::getUpdateRate() const {
    return pImpl->updateRate;
}

void NVMVisualization::setRealTimeUpdates(bool enabled) {
    pImpl->realTimeUpdates = enabled;
}

bool NVMVisualization::getRealTimeUpdates() const {
    return pImpl->realTimeUpdates;
}

void NVMVisualization::addDataSource(const std::string& name, const std::function<void(double)>& callback) {
    pImpl->dataSources[name] = callback;
}

void NVMVisualization::removeDataSource(const std::string& name) {
    pImpl->dataSources.erase(name);
}

void NVMVisualization::setTheme(const std::string& theme) {
    pImpl->theme = theme;
}

std::string NVMVisualization::getTheme() const {
    return pImpl->theme;
}

void NVMVisualization::setQuality(const std::string& quality) {
    pImpl->quality = quality;
}

std::string NVMVisualization::getQuality() const {
    return pImpl->quality;
}

// Helper methods for data conversion and export/import
NetworkTopology NVMVisualization::convertBrainToNetworkTopology(const Brain& brain) {
    NetworkTopology topology;
    
    // Convert neurons from brain to visualization data
    for (const auto& neuron : brain.getNeurons()) {
        NeuronData neuronData;
        neuronData.id = neuron.getId();
        neuronData.x = neuron.getX();
        neuronData.y = neuron.getY();
        neuronData.z = neuron.getZ();
        neuronData.activityLevel = neuron.getActivityLevel();
        neuronData.membranePotential = neuron.getMembranePotential();
        neuronData.threshold = neuron.getThreshold();
        neuronData.lastSpikeTime = neuron.getLastSpikeTime();
        
        // Get synaptic weights
        neuronData.synapticWeights = brain.getSynapticWeights(neuron.getId());
        
        // Calculate color based on activity
        float activity = neuronData.activityLevel;
        neuronData.colorR = activity;
        neuronData.colorG = 1.0f - activity;
        neuronData.colorB = 0.5f;
        
        neuronData.isActive = neuron.isActive();
        neuronData.isInRefractory = neuron.isInRefractory();
        
        topology.neurons.push_back(neuronData);
    }
    
    // Convert synapses from brain to visualization data
    for (const auto& synapse : brain.getSynapses()) {
        SynapseData synapseData;
        synapseData.id = synapse.getId();
        synapseData.preNeuronId = synapse.getPreNeuronId();
        synapseData.postNeuronId = synapse.getPostNeuronId();
        synapseData.weight = synapse.getWeight();
        synapseData.delay = synapse.getDelay();
        synapseData.lastUpdateTime = synapse.getLastUpdateTime();
        synapseData.type = synapse.getType();
        
        // Calculate color based on weight
        float weight = synapseData.weight;
        synapseData.colorR = (weight + 1.0f) * 0.5f;
        synapseData.colorG = 0.5f;
        synapseData.colorB = (1.0f - weight) * 0.5f;
        
        synapseData.activity = synapse.getActivity();
        
        topology.synapses.push_back(synapseData);
    }
    
    // Populate regions and populations
    for (const auto& region : brain.getRegions()) {
        std::vector<NeuronId> neurons;
        for (const auto& neuron : brain.getNeurons()) {
            if (neuron.getRegionId() == region.getId()) {
                neurons.push_back(neuron.getId());
            }
        }
        topology.regions[region.getId()] = neurons;
    }
    
    for (const auto& population : brain.getPopulations()) {
        std::vector<NeuronId> neurons;
        for (const auto& neuron : brain.getNeurons()) {
            if (neuron.getPopulationId() == population.getId()) {
                neurons.push_back(neuron.getId());
            }
        }
        topology.populations[population.getId()] = neurons;
    }
    
    return topology;
}

std::vector<MemoryTrace> NVMVisualization::convertBrainMemoryToMemoryTraces(const Brain& brain) {
    std::vector<MemoryTrace> memoryTraces;
    
    // Get memory traces from brain
    auto brainMemoryTraces = brain.getMemoryTraces();
    
    for (const auto& trace : brainMemoryTraces) {
        MemoryTrace memoryTrace;
        memoryTrace.id = trace.getId();
        memoryTrace.creationTime = trace.getCreationTime();
        memoryTrace.lastAccessTime = trace.getLastAccessTime();
        memoryTrace.age = trace.getAge();
        memoryTrace.strength = trace.getStrength();
        memoryTrace.decayRate = trace.getDecayRate();
        memoryTrace.consolidationProgress = trace.getConsolidationProgress();
        memoryTrace.type = trace.getType();
        
        // Get activation pattern
        memoryTrace.activationPattern = brain.getMemoryActivationPattern(trace.getId());
        
        memoryTraces.push_back(memoryTrace);
    }
    
    return memoryTraces;
}

std::vector<NeuromodulationData> NVMVisualization::convertBrainNeuromodulationToNeuromodulationData(const Brain& brain) {
    std::vector<NeuromodulationData> neuromodulationData;
    
    // Get neuromodulation data from brain
    auto brainNeuromodulation = brain.getNeuromodulationData();
    
    for (const auto& neuromod : brainNeuromodulation) {
        NeuromodulationData neuromodData;
        neuromodData.type = neuromod.getType();
        neuromodData.level = neuromod.getLevel();
        neuromodData.baseline = neuromod.getBaseline();
        neuromodData.targetLevel = neuromod.getTargetLevel();
        neuromodData.learningRate = neuromod.getLearningRate();
        neuromodData.noveltySignal = neuromod.getNoveltySignal();
        neuromodData.predictionError = neuromod.getPredictionError();
        neuromodData.curiosity = neuromod.getCuriosity();
        
        // Get time series data
        neuromodData.timeSeries = neuromod.getTimeSeries();
        neuromodData.timePoints = neuromod.getTimePoints();
        
        neuromodulationData.push_back(neuromodData);
    }
    
    return neuromodulationData;
}

std::vector<PlasticityData> NVMVisualization::convertBrainPlasticityToPlasticityData(const Brain& brain) {
    std::vector<PlasticityData> plasticityData;
    
    // Get plasticity data from brain
    auto brainPlasticity = brain.getPlasticityData();
    
    for (const auto& plasticity : brainPlasticity) {
        PlasticityData plasticityData;
        plasticityData.type = plasticity.getType();
        plasticityData.learningRate = plasticity.getLearningRate();
        plasticityData.eligibilityTrace = plasticity.getEligibilityTrace();
        plasticityData.rewardPrediction = plasticity.getRewardPrediction();
        plasticityData.valueFunction = plasticity.getValueFunction();
        
        // Get synaptic weights
        plasticityData.weights = plasticity.getWeights();
        
        // Get eligibility traces
        plasticityData.eligibilityTraces = plasticity.getEligibilityTraces();
        
        // Get reward signals
        plasticityData.rewardSignals = plasticity.getRewardSignals();
        
        plasticityData.push_back(plasticityData);
    }
    
    return plasticityData;
}

std::vector<DevelopmentData> NVMVisualization::convertBrainDevelopmentToDevelopmentData(const Brain& brain) {
    std::vector<DevelopmentData> developmentData;
    
    // Get development data from brain
    auto brainDevelopment = brain.getDevelopmentData();
    
    for (const auto& development : brainDevelopment) {
        DevelopmentData developmentData;
        developmentData.stage = development.getStage();
        developmentData.maturationProgress = development.getMaturationProgress();
        
        // Get structural plasticity data
        developmentData.structuralPlasticity = development.getStructuralPlasticity();
        
        // Get experience-dependent development data
        developmentData.experienceDependence = development.getExperienceDependence();
        
        // Get connectivity maturation data
        developmentData.connectivityMaturation = development.getConnectivityMaturation();
        
        // Get neuron maturation data
        developmentData.neuronMaturation = development.getNeuronMaturation();
        
        // Get experience timestamps and data
        developmentData.experienceTimestamps = development.getExperienceTimestamps();
        developmentData.experiences = development.getExperiences();
        
        developmentData.push_back(developmentData);
    }
    
    return developmentData;
}

SystemIntegrationData NVMVisualization::calculateSystemIntegrationData() {
    SystemIntegrationData systemData;
    
    // Calculate performance metrics
    systemData.performance.cpuUsage = 45.0; // Example value
    systemData.performance.memoryUsage = 1024.0; // Example value
    systemData.performance.networkUsage = 1024.0; // Example value
    systemData.performance.renderingFrameTime = 16.67; // Example value
    systemData.performance.updateTime = 8.33; // Example value
    systemData.performance.renderTime = 8.34; // Example value
    systemData.performance.fps = 60.0; // Example value
    systemData.performance.dataProcessingTime = 16.67; // Example value
    
    // Calculate learning curves
    for (int i = 0; i < 10; ++i) {
        LearningCurve curve;
        curve.metricName = "performance";
        for (int j = 0; j <= i; ++j) {
            curve.timePoints.push_back(j * 1.0);
            curve.performanceValues.push_back(static_cast<float>(i * 0.1));
            curve.explorationValues.push_back(static_cast<float>(0.5 - i * 0.05));
            curve.exploitationValues.push_back(static_cast<float>(0.5 + i * 0.05));
            curve.curiosityValues.push_back(static_cast<float>(0.3 + i * 0.02));
        }
        systemData.learningCurves.push_back(curve);
    }
    
    // Calculate network activity patterns
    for (int i = 0; i < 5; ++i) {
        NetworkActivityPattern pattern;
        pattern.patternType = "oscillatory";
        pattern.synchrony = static_cast<float>(0.5 + i * 0.1);
        pattern.informationFlow = static_cast<float>(0.3 + i * 0.05);
        
        for (int j = 0; j < 100; ++j) {
            pattern.firingRates.push_back(static_cast<float>(10.0 + j * 0.1));
            pattern.phases.push_back(static_cast<float>(j * 0.06));
            pattern.coherence.push_back(static_cast<float>(0.7 + sin(j * 0.1)));
            pattern.connections.push_back(std::make_pair(j % 50, (j + 1) % 50));
        }
        
        systemData.activityPatterns.push_back(pattern);
    }
    
    // Calculate cross-system correlations
    systemData.crossSystemCorrelations["neural_memory"] = 0.8;
    systemData.crossSystemCorrelations["neural_plasticity"] = 0.6;
    systemData.crossSystemCorrelations["memory_neuromodulation"] = 0.7;
    systemData.crossSystemCorrelations["plasticity_development"] = 0.5;
    
    // Calculate checkpoint data
    for (int i = 0; i < 5; ++i) {
        systemData.checkpointData.push_back(std::make_pair(i * 1000, "checkpoint_" + std::to_string(i)));
    }
    
    // Calculate resource utilization
    for (int i = 0; i < 100; ++i) {
        systemData.resourceUtilization.push_back(std::make_pair(i * 1.0, static_cast<double>(50 + sin(i * 0.1) * 20)));
    }
    
    return systemData;
}

bool NVMVisualization::exportToJSON(const std::string& filename) {
    try {
        json data;
        
        // Convert network topology to JSON
        json networkTopologyJSON = convertNetworkTopologyToJSON(pImpl->networkTopology);
        data["networkTopology"] = networkTopologyJSON;
        
        // Convert memory traces to JSON
        json memoryTracesJSON = convertMemoryTracesToJSON(pImpl->memoryTraces);
        data["memoryTraces"] = memoryTracesJSON;
        
        // Convert neuromodulation data to JSON
        json neuromodulationDataJSON = convertNeuromodulationDataToJSON(pImpl->neuromodulationData);
        data["neuromodulationData"] = neuromodulationDataJSON;
        
        // Convert plasticity data to JSON
        json plasticityDataJSON = convertPlasticityDataToJSON(pImpl->plasticityData);
        data["plasticityData"] = plasticityDataJSON;
        
        // Convert development data to JSON
        json developmentDataJSON = convertDevelopmentDataToJSON(pImpl->developmentData);
        data["developmentData"] = developmentDataJSON;
        
        // Convert system integration data to JSON
        json systemIntegrationDataJSON = convertSystemIntegrationDataToJSON(pImpl->systemIntegrationData);
        data["systemIntegrationData"] = systemIntegrationDataJSON;
        
        // Add metadata
        data["metadata"] = {
            {"version", "1.0.0"},
            {"created", getCurrentTimestamp()},
            {"author", "NLM Visualization System"}
        };
        
        // Write to file
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << data.dump(2);
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NVMVisualization::exportToCSV(const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Write network topology to CSV
        file << "NeuronID,X,Y,Z,ActivityLevel,MembranePotential,Threshold,LastSpikeTime\n";
        for (const auto& neuron : pImpl->networkTopology.neurons) {
            file << neuron.id << "," << neuron.x << "," << neuron.y << "," << neuron.z << ",";
            file << neuron.activityLevel << "," << neuron.membranePotential << "," << neuron.threshold << ";";
            file << neuron.lastSpikeTime << "\n";
        }
        
        file << "\nSynapseID,PreNeuronID,PostNeuronID,Weight,Delay,LastUpdateTime,Type\n";
        for (const auto& synapse : pImpl->networkTopology.synapses) {
            file << synapse.id << "," << synapse.preNeuronId << "," << synapse.postNeuronId << ",";
            file << synapse.weight << "," << synapse.delay << "," << synapse.lastUpdateTime << ",";
            file << synapse.type << "\n";
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NVMVisualization::exportToXML(const std::string& filename) {
    try {
        json data = convertVisualizationToJSON();
        
        // Convert JSON to XML (simplified implementation)
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        file << "<nlm_visualization>\n";
        
        // Write network topology
        file << "  <network_topology>\n";
        for (const auto& neuron : pImpl->networkTopology.neurons) {
            file << "    <neuron>\n";
            file << "      <id>" << neuron.id << "</id>\n";
            file << "      <x>" << neuron.x << "</x>\n";
            file << "      <y>" << neuron.y << "</y>\n";
            file << "      <z>" << neuron.z << "</z>\n";
            file << "      <activityLevel>" << neuron.activityLevel << "</activityLevel>\n";
            file << "      <membranePotential>" << neuron.membranePotential << "</membranePotential>\n";
            file << "      <threshold>" << neuron.threshold << "</threshold>\n";
            file << "      <lastSpikeTime>" << neuron.lastSpikeTime << "</lastSpikeTime>\n";
            file << "    </neuron>\n";
        }
        file << "  </network_topology>\n";
        
        // Write memory traces
        file << "  <memory_traces>\n";
        for (const auto& trace : pImpl->memoryTraces) {
            file << "    <memory_trace>\n";
            file << "      <id>" << trace.id << "</id>\n";
            file << "      <creationTime>" << trace.creationTime << "</creationTime>\n";
            file << "      <lastAccessTime>" << trace.lastAccessTime << "</lastAccessTime>\n";
            file << "      <age>" << trace.age << "</age>\n";
            file << "      <strength>" << trace.strength << "</strength>\n";
            file << "      <decayRate>" << trace.decayRate << "</decayRate>\n";
            file << "      <consolidationProgress>" << trace.consolidationProgress << "</consolidationProgress>\n";
            file << "      <type>" << trace.type << "</type>\n";
            file << "    </memory_trace>\n";
        }
        file << "  </memory_traces>\n";
        
        file << "</nlm_visualization>\n";
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NVMVisualization::exportToBinary(const std::string& filename) {
    try {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Write binary data (simplified implementation)
        json data = convertVisualizationToJSON();
        std::string jsonString = data.dump();
        
        // Write string length and content
        uint32_t length = static_cast<uint32_t>(jsonString.length());
        file.write(reinterpret_cast<const char*>(&length), sizeof(length));
        file.write(jsonString.c_str(), length);
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NVMVisualization::importFromJSON(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        json data;
        file >> data;
        file.close();
        
        // Convert JSON to visualization data
        return convertJSONToVisualization(data);
    } catch (const std::exception& e) {
        return false;
    }
}

bool NVMVisualization::importFromCSV(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        std::vector<std::string> headers;
        bool readingNeurons = false;
        bool readingSynapses = false;
        
        while (std::getline(file, line)) {
            if (line.empty()) {
                continue;
            }
            
            if (line.find("NeuronID") != std::string::npos) {
                headers = splitLine(line);
                readingNeurons = true;
                continue;
            }
            
            if (line.find("SynapseID") != std::string::npos) {
                headers = splitLine(line);
                readingSynapses = true;
                continue;
            }
            
            if (readingNeurons) {
                NeuronData neuron = parseNeuronFromCSV(line, headers);
                pImpl->networkTopology.neurons.push_back(neuron);
            } else if (readingSynapses) {
                SynapseData synapse = parseSynapseFromCSV(line, headers);
                pImpl->networkTopology.synapses.push_back(synapse);
            }
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NVMVisualization::importFromXML(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        bool readingNeurons = false;
        bool readingMemoryTraces = false;
        
        while (std::getline(file, line)) {
            if (line.find("<neuron>") != std::string::npos) {
                readingNeurons = true;
                continue;
            } else if (line.find("</neuron>") != std::string::npos) {
                readingNeurons = false;
                continue;
            } else if (line.find("<memory_trace>") != std::string::npos) {
                readingMemoryTraces = true;
                continue;
            } else if (line.find("</memory_trace>") != std::string::npos) {
                readingMemoryTraces = false;
                continue;
            }
            
            if (readingNeurons && line.find("<id>") != std::string::npos) {
                // Parse neuron data from XML
            } else if (readingMemoryTraces && line.find("<id>") != std::string::npos) {
                // Parse memory trace data from XML
            }
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NVMVisualization::importFromBinary(const std::string& filename) {
    try {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Read binary data (simplified implementation)
        uint32_t length;
        file.read(reinterpret_cast<char*>(&length), sizeof(length));
        
        std::vector<char> buffer(length);
        file.read(buffer.data(), length);
        
        std::string jsonString(buffer.begin(), buffer.end());
        json data = json::parse(jsonString);
        
        // Convert JSON to visualization data
        return convertJSONToVisualization(data);
    } catch (const std::exception& e) {
        return false;
    }
}

json NVMVisualization::convertVisualizationToJSON() const {
    json data;
    
    // Convert network topology to JSON
    json networkTopologyJSON = convertNetworkTopologyToJSON(pImpl->networkTopology);
    data["networkTopology"] = networkTopologyJSON;
    
    // Convert memory traces to JSON
    json memoryTracesJSON = convertMemoryTracesToJSON(pImpl->memoryTraces);
    data["memoryTraces"] = memoryTracesJSON;
    
    // Convert neuromodulation data to JSON
    json neuromodulationDataJSON = convertNeuromodulationDataToJSON(pImpl->neuromodulationData);
    data["neuromodulationData"] = neuromodulationDataJSON;
    
    // Convert plasticity data to JSON
    json plasticityDataJSON = convertPlasticityDataToJSON(pImpl->plasticityData);
    data["plasticityData"] = plasticityDataJSON;
    
    // Convert development data to JSON
    json developmentDataJSON = convertDevelopmentDataToJSON(pImpl->developmentData);
    data["developmentData"] = developmentDataJSON;
    
    // Convert system integration data to JSON
    json systemIntegrationDataJSON = convertSystemIntegrationDataToJSON(pImpl->systemIntegrationData);
    data["systemIntegrationData"] = systemIntegrationDataJSON;
    
    return data;
}

json NVMVisualization::convertNetworkTopologyToJSON(const NetworkTopology& topology) const {
    json data;
    
    // Convert neurons to JSON
    json neuronsJSON = json::array();
    for (const auto& neuron : topology.neurons) {
        json neuronJSON;
        neuronJSON["id"] = neuron.id;
        neuronJSON["x"] = neuron.x;
        neuronJSON["y"] = neuron.y;
        neuronJSON["z"] = neuron.z;
        neuronJSON["activityLevel"] = neuron.activityLevel;
        neuronJSON["membranePotential"] = neuron.membranePotential;
        neuronJSON["threshold"] = neuron.threshold;
        neuronJSON["lastSpikeTime"] = neuron.lastSpikeTime;
        neuronsJSON.push_back(neuronJSON);
    }
    data["neurons"] = neuronsJSON;
    
    // Convert synapses to JSON
    json synapsesJSON = json::array();
    for (const auto& synapse : topology.synapses) {
        json synapseJSON;
        synapseJSON["id"] = synapse.id;
        synapseJSON["preNeuronId"] = synapse.preNeuronId;
        synapseJSON["postNeuronId"] = synapse.postNeuronId;
        synapseJSON["weight"] = synapse.weight;
        synapseJSON["delay"] = synapse.delay;
        synapseJSON["lastUpdateTime"] = synapse.lastUpdateTime;
        synapseJSON["type"] = synapse.type;
        synapsesJSON.push_back(synapseJSON);
    }
    data["synapses"] = synapsesJSON;
    
    // Convert regions to JSON
    json regionsJSON;
    for (const auto& pair : topology.regions) {
        regionsJSON[std::to_string(pair.first)] = pair.second;
    }
    data["regions"] = regionsJSON;
    
    // Convert populations to JSON
    json populationsJSON;
    for (const auto& pair : topology.populations) {
        populationsJSON[std::to_string(pair.first)] = pair.second;
    }
    data["populations"] = populationsJSON;
    
    return data;
}

json NVMVisualization::convertMemoryTracesToJSON(const std::vector<MemoryTrace>& memoryTraces) const {
    json data = json::array();
    
    for (const auto& trace : memoryTraces) {
        json traceJSON;
        traceJSON["id"] = trace.id;
        traceJSON["creationTime"] = trace.creationTime;
        traceJSON["lastAccessTime"] = trace.lastAccessTime;
        traceJSON["age"] = trace.age;
        traceJSON["strength"] = trace.strength;
        traceJSON["decayRate"] = trace.decayRate;
        traceJSON["consolidationProgress"] = trace.consolidationProgress;
        traceJSON["type"] = trace.type;
        
        // Convert activation pattern to JSON
        json patternJSON = json::array();
        for (float value : trace.activationPattern) {
            patternJSON.push_back(value);
        }
        traceJSON["activationPattern"] = patternJSON;
        
        data.push_back(traceJSON);
    }
    
    return data;
}

json NVMVisualization::convertNeuromodulationDataToJSON(const std::vector<NeuromodulationData>& neuromodulationData) const {
    json data = json::array();
    
    for (const auto& neuromod : neuromodulationData) {
        json neuromodJSON;
        neuromodJSON["type"] = neuromod.type;
        neuromodJSON["level"] = neuromod.level;
        neuromodJSON["baseline"] = neuromod.baseline;
        neuromodJSON["targetLevel"] = neuromod.targetLevel;
        neuromodJSON["learningRate"] = neuromod.learningRate;
        neuromodJSON["noveltySignal"] = neuromod.noveltySignal;
        neuromodJSON["predictionError"] = neuromod.predictionError;
        neuromodJSON["curiosity"] = neuromod.curiosity;
        
        // Convert time series to JSON
        json timeSeriesJSON = json::array();
        for (float value : neuromod.timeSeries) {
            timeSeriesJSON.push_back(value);
        }
        neuromodJSON["timeSeries"] = timeSeriesJSON;
        
        // Convert time points to JSON
        json timePointsJSON = json::array();
        for (Timestamp timePoint : neuromod.timePoints) {
            timePointsJSON.push_back(timePoint);
        }
        neuromodJSON["timePoints"] = timePointsJSON;
        
        data.push_back(neuromodJSON);
    }
    
    return data;
}

json NVMVisualization::convertPlasticityDataToJSON(const std::vector<PlasticityData>& plasticityData) const {
    json data = json::array();
    
    for (const auto& plasticity : plasticityData) {
        json plasticityJSON;
        plasticityJSON["type"] = plasticity.type;
        plasticityJSON["learningRate"] = plasticity.learningRate;
        plasticityJSON["eligibilityTrace"] = plasticity.eligibilityTrace;
        plasticityJSON["rewardPrediction"] = plasticity.rewardPrediction;
        plasticityJSON["valueFunction"] = plasticity.valueFunction;
        
        // Convert weights to JSON
        json weightsJSON = json::array();
        for (float weight : plasticity.weights) {
            weightsJSON.push_back(weight);
        }
        plasticityJSON["weights"] = weightsJSON;
        
        // Convert eligibility traces to JSON
        json eligibilityTracesJSON = json::array();
        for (float trace : plasticity.eligibilityTraces) {
            eligibilityTracesJSON.push_back(trace);
        }
        plasticityJSON["eligibilityTraces"] = eligibilityTracesJSON;
        
        // Convert reward signals to JSON
        json rewardSignalsJSON = json::array();
        for (float signal : plasticity.rewardSignals) {
            rewardSignalsJSON.push_back(signal);
        }
        plasticityJSON["rewardSignals"] = rewardSignalsJSON;
        
        data.push_back(plasticityJSON);
    }
    
    return data;
}

json NVMVisualization::convertDevelopmentDataToJSON(const std::vector<DevelopmentData>& developmentData) const {
    json data = json::array();
    
    for (const auto& devData : developmentData) {
        json devJSON;
        devJSON["stage"] = devData.stage;
        devJSON["maturationProgress"] = devData.maturationProgress;
        
        // Convert structural plasticity to JSON
        json structuralPlasticityJSON = json::array();
        for (float value : devData.structuralPlasticity) {
            structuralPlasticityJSON.push_back(value);
        }
        devJSON["structuralPlasticity"] = structuralPlasticityJSON;
        
        // Convert experience dependence to JSON
        json experienceDependenceJSON = json::array();
        for (float value : devData.experienceDependence) {
            experienceDependenceJSON.push_back(value);
        }
        devJSON["experienceDependence"] = experienceDependenceJSON;
        
        // Convert connectivity maturation to JSON
        json connectivityMaturationJSON = json::array();
        for (float value : devData.connectivityMaturation) {
            connectivityMaturationJSON.push_back(value);
        }
        devJSON["connectivityMaturation"] = connectivityMaturationJSON;
        
        // Convert neuron maturation to JSON
        json neuronMaturationJSON = json::array();
        for (float value : devData.neuronMaturation) {
            neuronMaturationJSON.push_back(value);
        }
        devJSON["neuronMaturation"] = neuronMaturationJSON;
        
        // Convert experience timestamps to JSON
        json experienceTimestampsJSON = json::array();
        for (Timestamp timeStamp : devData.experienceTimestamps) {
            experienceTimestampsJSON.push_back(timeStamp);
        }
        devJSON["experienceTimestamps"] = experienceTimestampsJSON;
        
        // Convert experiences to JSON
        json experiencesJSON = json::array();
        for (const std::string& experience : devData.experiences) {
            experiencesJSON.push_back(experience);
        }
        devJSON["experiences"] = experiencesJSON;
        
        data.push_back(devJSON);
    }
    
    return data;
}

json NVMVisualization::convertSystemIntegrationDataToJSON(const SystemIntegrationData& systemData) const {
    json data;
    
    // Convert performance metrics to JSON
    json performanceJSON;
    performanceJSON["cpuUsage"] = systemData.performance.cpuUsage;
    performanceJSON["memoryUsage"] = systemData.performance.memoryUsage;
    performanceJSON["networkUsage"] = systemData.performance.networkUsage;
    performanceJSON["renderingFrameTime"] = systemData.performance.renderingFrameTime;
    performanceJSON["updateTime"] = systemData.performance.updateTime;
    performanceJSON["renderTime"] = systemData.performance.renderTime;
    performanceJSON["fps"] = systemData.performance.fps;
    performanceJSON["dataProcessingTime"] = systemData.performance.dataProcessingTime;
    data["performance"] = performanceJSON;
    
    // Convert learning curves to JSON
    json learningCurvesJSON = json::array();
    for (const auto& curve : systemData.learningCurves) {
        json curveJSON;
        curveJSON["metricName"] = curve.metricName;
        
        // Convert time points to JSON
        json timePointsJSON = json::array();
        for (Timestamp timePoint : curve.timePoints) {
            timePointsJSON.push_back(timePoint);
        }
        curveJSON["timePoints"] = timePointsJSON;
        
        // Convert performance values to JSON
        json performanceValuesJSON = json::array();
        for (float value : curve.performanceValues) {
            performanceValuesJSON.push_back(value);
        }
        curveJSON["performanceValues"] = performanceValuesJSON;
        
        // Convert exploration values to JSON
        json explorationValuesJSON = json::array();
        for (float value : curve.explorationValues) {
            explorationValuesJSON.push_back(value);
        }
        curveJSON["explorationValues"] = explorationValuesJSON;
        
        // Convert exploitation values to JSON
        json exploitationValuesJSON = json::array();
        for (float value : curve.exploitationValues) {
            exploitationValuesJSON.push_back(value);
        }
        curveJSON["exploitationValues"] = exploitationValuesJSON;
        
        // Convert curiosity values to JSON
        json curiosityValuesJSON = json::array();
        for (float value : curve.curiosityValues) {
            curiosityValuesJSON.push_back(value);
        }
        curveJSON["curiosityValues"] = curiosityValuesJSON;
        
        learningCurvesJSON.push_back(curveJSON);
    }
    data["learningCurves"] = learningCurvesJSON;
    
    // Convert network activity patterns to JSON
    json activityPatternsJSON = json::array();
    for (const auto& pattern : systemData.activityPatterns) {
        json patternJSON;
        patternJSON["patternType"] = pattern.patternType;
        patternJSON["synchrony"] = pattern.synchrony;
        patternJSON["informationFlow"] = pattern.informationFlow;
        
        // Convert firing rates to JSON
        json firingRatesJSON = json::array();
        for (float rate : pattern.firingRates) {
            firingRatesJSON.push_back(rate);
        }
        patternJSON["firingRates"] = firingRatesJSON;
        
        // Convert phases to JSON
        json phasesJSON = json::array();
        for (float phase : pattern.phases) {
            phasesJSON.push_back(phase);
        }
        patternJSON["phases"] = phasesJSON;
        
        // Convert coherence to JSON
        json coherenceJSON = json::array();
        for (float coherence : pattern.coherence) {
            coherenceJSON.push_back(coherence);
        }
        patternJSON["coherence"] = coherenceJSON;
        
        // Convert connections to JSON
        json connectionsJSON = json::array();
        for (const auto& connection : pattern.connections) {
            json connectionJSON;
            connectionJSON["preNeuronId"] = connection.first;
            connectionJSON["postNeuronId"] = connection.second;
            connectionsJSON.push_back(connectionJSON);
        }
        patternJSON["connections"] = connectionsJSON;
        
        activityPatternsJSON.push_back(patternJSON);
    }
    data["activityPatterns"] = activityPatternsJSON;
    
    // Convert cross-system correlations to JSON
    json correlationsJSON;
    for (const auto& pair : systemData.crossSystemCorrelations) {
        correlationsJSON[pair.first] = pair.second;
    }
    data["crossSystemCorrelations"] = correlationsJSON;
    
    // Convert checkpoint data to JSON
    json checkpointDataJSON = json::array();
    for (const auto& checkpoint : systemData.checkpointData) {
        json checkpointJSON;
        checkpointJSON["timestamp"] = checkpoint.first;
        checkpointJSON["data"] = checkpoint.second;
        checkpointDataJSON.push_back(checkpointJSON);
    }
    data["checkpointData"] = checkpointDataJSON;
    
    // Convert resource utilization to JSON
    json resourceUtilizationJSON = json::array();
    for (const auto& resource : systemData.resourceUtilization) {
        json resourceJSON;
        resourceJSON["timestamp"] = resource.first;
        resourceJSON["utilization"] = resource.second;
        resourceUtilizationJSON.push_back(resourceJSON);
    }
    data["resourceUtilization"] = resourceUtilizationJSON;
    
    return data;
}

bool NVMVisualization::convertJSONToVisualization(const json& data) {
    try {
        // Convert network topology from JSON
        if (data.contains("networkTopology")) {
            pImpl->networkTopology = convertJSONToNetworkTopology(data["networkTopology"]);
        }
        
        // Convert memory traces from JSON
        if (data.contains("memoryTraces")) {
            pImpl->memoryTraces = convertJSONToMemoryTraces(data["memoryTraces"]);
        }
        
        // Convert neuromodulation data from JSON
        if (data.contains("neuromodulationData")) {
            pImpl->neuromodulationData = convertJSONToNeuromodulationData(data["neuromodulationData"]);
        }
        
        // Convert plasticity data from JSON
        if (data.contains("plasticityData")) {
            pImpl->plasticityData = convertJSONToPlasticityData(data["plasticityData"]);
        }
        
        // Convert development data from JSON
        if (data.contains("developmentData")) {
            pImpl->developmentData = convertJSONToDevelopmentData(data["developmentData"]);
        }
        
        // Convert system integration data from JSON
        if (data.contains("systemIntegrationData")) {
            pImpl->systemIntegrationData = convertJSONToSystemIntegrationData(data["systemIntegrationData"]);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

NetworkTopology NVMVisualization::convertJSONToNetworkTopology(const json& data) {
    NetworkTopology topology;
    
    // Convert neurons from JSON
    if (data.contains("neurons")) {
        for (const auto& neuronJSON : data["neurons"]) {
            NeuronData neuron;
            neuron.id = neuronJSON["id"];
            neuron.x = neuronJSON["x"];
            neuron.y = neuronJSON["y"];
            neuron.z = neuronJSON["z"];
            neuron.activityLevel = neuronJSON["activityLevel"];
            neuron.membranePotential = neuronJSON["membranePotential"];
            neuron.threshold = neuronJSON["threshold"];
            neuron.lastSpikeTime = neuronJSON["lastSpikeTime"];
            topology.neurons.push_back(neuron);
        }
    }
    
    // Convert synapses from JSON
    if (data.contains("synapses")) {
        for (const auto& synapseJSON : data["synapses"]) {
            SynapseData synapse;
            synapse.id = synapseJSON["id"];
            synapse.preNeuronId = synapseJSON["preNeuronId"];
            synapse.postNeuronId = synapseJSON["postNeuronId"];
            synapse.weight = synapseJSON["weight"];
            synapse.delay = synapseJSON["delay"];
            synapse.lastUpdateTime = synapseJSON["lastUpdateTime"];
            synapse.type = synapseJSON["type"];
            topology.synapses.push_back(synapse);
        }
    }
    
    return topology;
}

std::vector<MemoryTrace> NVMVisualization::convertJSONToMemoryTraces(const json& data) {
    std::vector<MemoryTrace> memoryTraces;
    
    for (const auto& traceJSON : data) {
        MemoryTrace trace;
        trace.id = traceJSON["id"];
        trace.creationTime = traceJSON["creationTime"];
        trace.lastAccessTime = traceJSON["lastAccessTime"];
        trace.age = traceJSON["age"];
        trace.strength = traceJSON["strength"];
        trace.decayRate = traceJSON["decayRate"];
        trace.consolidationProgress = traceJSON["consolidationProgress"];
        trace.type = traceJSON["type"];
        
        // Convert activation pattern from JSON
        if (traceJSON.contains("activationPattern")) {
            for (const auto& value : traceJSON["activationPattern"]) {
                trace.activationPattern.push_back(value);
            }
        }
        
        memoryTraces.push_back(trace);
    }
    
    return memoryTraces;
}

std::vector<NeuromodulationData> NVMVisualization::convertJSONToNeuromodulationData(const json& data) {
    std::vector<NeuromodulationData> neuromodulationData;
    
    for (const auto& neuromodJSON : data) {
        NeuromodulationData neuromod;
        neuromod.type = neuromodJSON["type"];
        neuromod.level = neuromodJSON["level"];
        neuromod.baseline = neuromodJSON["baseline"];
        neuromod.targetLevel = neuromodJSON["targetLevel"];
        neuromod.learningRate = neuromodJSON["learningRate"];
        neuromod.noveltySignal = neuromodJSON["noveltySignal"];
        neuromod.predictionError = neuromodJSON["predictionError"];
        neuromod.curiosity = neuromodJSON["curiosity"];
        
        // Convert time series from JSON
        if (neuromodJSON.contains("timeSeries")) {
            for (const auto& value : neuromodJSON["timeSeries"]) {
                neuromod.timeSeries.push_back(value);
            }
        }
        
        // Convert time points from JSON
        if (neuromodJSON.contains("timePoints")) {
            for (const auto& timePoint : neuromodJSON["timePoints"]) {
                neuromod.timePoints.push_back(timePoint);
            }
        }
        
        neuromodulationData.push_back(neuromod);
    }
    
    return neuromodulationData;
}

std::vector<PlasticityData> NVMVisualization::convertJSONToPlasticityData(const json& data) {
    std::vector<PlasticityData> plasticityData;
    
    for (const auto& plasticityJSON : data) {
        PlasticityData plasticity;
        plasticity.type = plasticityJSON["type"];
        plasticity.learningRate = plasticityJSON["learningRate"];
        plasticity.eligibilityTrace = plasticityJSON["eligibilityTrace"];
        plasticity.rewardPrediction = plasticityJSON["rewardPrediction"];
        plasticity.valueFunction = plasticityJSON["valueFunction"];
        
        // Convert weights from JSON
        if (plasticityJSON.contains("weights")) {
            for (const auto& weight : plasticityJSON["weights"]) {
                plasticity.weights.push_back(weight);
            }
        }
        
        // Convert eligibility traces from JSON
        if (plasticityJSON.contains("eligibilityTraces")) {
            for (const auto& trace : plasticityJSON["eligibilityTraces"]) {
                plasticity.eligibilityTraces.push_back(trace);
            }
        }
        
        // Convert reward signals from JSON
        if (plasticityJSON.contains("rewardSignals")) {
            for (const auto& signal : plasticityJSON["rewardSignals"]) {
                plasticity.rewardSignals.push_back(signal);
            }
        }
        
        plasticityData.push_back(plasticity);
    }
    
    return plasticityData;
}

std::vector<DevelopmentData> NVMVisualization::convertJSONToDevelopmentData(const json& data) {
    std::vector<DevelopmentData> developmentData;
    
    for (const auto& devJSON : data) {
        DevelopmentData dev;
        dev.stage = devJSON["stage"];
        dev.maturationProgress = devJSON["maturationProgress"];
        
        // Convert structural plasticity from JSON
        if (devJSON.contains("structuralPlasticity")) {
            for (const auto& value : devJSON["structuralPlasticity"]) {
                dev.structuralPlasticity.push_back(value);
            }
        }
        
        // Convert experience dependence from JSON
        if (devJSON.contains("experienceDependence")) {
            for (const auto& value : devJSON["experienceDependence"]) {
                dev.experienceDependence.push_back(value);
            }
        }
        
        // Convert connectivity maturation from JSON
        if (devJSON.contains("connectivityMaturation")) {
            for (const auto& value : devJSON["connectivityMaturation"]) {
                dev.connectivityMaturation.push_back(value);
            }
        }
        
        // Convert neuron maturation from JSON
        if (devJSON.contains("neuronMaturation")) {
            for (const auto& value : devJSON["neuronMaturation"]) {
                dev.neuronMaturation.push_back(value);
            }
        }
        
        // Convert experience timestamps from JSON
        if (devJSON.contains("experienceTimestamps")) {
            for (const auto& timeStamp : devJSON["experienceTimestamps"]) {
                dev.experienceTimestamps.push_back(timeStamp);
            }
        }
        
        // Convert experiences from JSON
        if (devJSON.contains("experiences")) {
            for (const auto& experience : devJSON["experiences"]) {
                dev.experiences.push_back(experience);
            }
        }
        
        developmentData.push_back(dev);
    }
    
    return developmentData;
}

SystemIntegrationData NVMVisualization::convertJSONToSystemIntegrationData(const json& data) {
    SystemIntegrationData systemData;
    
    // Convert performance metrics from JSON
    if (data.contains("performance")) {
        systemData.performance.cpuUsage = data["performance"]["cpuUsage"];
        systemData.performance.memoryUsage = data["performance"]["memoryUsage"];
        systemData.performance.networkUsage = data["performance"]["networkUsage"];
        systemData.performance.renderingFrameTime = data["performance"]["renderingFrameTime"];
        systemData.performance.updateTime = data["performance"]["updateTime"];
        systemData.performance.renderTime = data["performance"]["renderTime"];
        systemData.performance.fps = data["performance"]["fps"];
        systemData.performance.dataProcessingTime = data["performance"]["dataProcessingTime"];
    }
    
    // Convert learning curves from JSON
    if (data.contains("learningCurves")) {
        for (const auto& curveJSON : data["learningCurves"]) {
            LearningCurve curve;
            curve.metricName = curveJSON["metricName"];
            
            // Convert time points from JSON
            if (curveJSON.contains("timePoints")) {
                for (const auto& timePoint : curveJSON["timePoints"]) {
                    curve.timePoints.push_back(timePoint);
                }
            }
            
            // Convert performance values from JSON
            if (curveJSON.contains("performanceValues")) {
                for (const auto& value : curveJSON["performanceValues"]) {
                    curve.performanceValues.push_back(value);
                }
            }
            
            // Convert exploration values from JSON
            if (curveJSON.contains("explorationValues")) {
                for (const auto& value : curveJSON["explorationValues"]) {
                    curve.explorationValues.push_back(value);
                }
            }
            
            // Convert exploitation values from JSON
            if (curveJSON.contains("exploitationValues")) {
                for (const auto& value : curveJSON["exploitationValues"]) {
                    curve.exploitationValues.push_back(value);
                }
            }
            
            // Convert curiosity values from JSON
            if (curveJSON.contains("curiosityValues")) {
                for (const auto& value : curveJSON["curiosityValues"]) {
                    curve.curiosityValues.push_back(value);
                }
            }
            
            systemData.learningCurves.push_back(curve);
        }
    }
    
    // Convert network activity patterns from JSON
    if (data.contains("activityPatterns")) {
        for (const auto& patternJSON : data["activityPatterns"]) {
            NetworkActivityPattern pattern;
            pattern.patternType = patternJSON["patternType"];
            pattern.synchrony = patternJSON["synchrony"];
            pattern.informationFlow = patternJSON["informationFlow"];
            
            // Convert firing rates from JSON
            if (patternJSON.contains("firingRates")) {
                for (const auto& rate : patternJSON["firingRates"]) {
                    pattern.firingRates.push_back(rate);
                }
            }
            
            // Convert phases from JSON
            if (patternJSON.contains("phases")) {
                for (const auto& phase : patternJSON["phases"]) {
                    pattern.phases.push_back(phase);
                }
            }
            
            // Convert coherence from JSON
            if (patternJSON.contains("coherence")) {
                for (const auto& coherence : patternJSON["coherence"]) {
                    pattern.coherence.push_back(coherence);
                }
            }
            
            // Convert connections from JSON
            if (patternJSON.contains("connections")) {
                for (const auto& connectionJSON : patternJSON["connections"]) {
                    std::pair<NeuronId, NeuronId> connection;
                    connection.first = connectionJSON["preNeuronId"];
                    connection.second = connectionJSON["postNeuronId"];
                    pattern.connections.push_back(connection);
                }
            }
            
            systemData.activityPatterns.push_back(pattern);
        }
    }
    
    // Convert cross-system correlations from JSON
    if (data.contains("crossSystemCorrelations")) {
        for (auto it = data["crossSystemCorrelations"].begin(); it != data["crossSystemCorrelations"].end(); ++it) {
            systemData.crossSystemCorrelations[it->first] = it->second;
        }
    }
    
    // Convert checkpoint data from JSON
    if (data.contains("checkpointData")) {
        for (const auto& checkpointJSON : data["checkpointData"]) {
            std::pair<Timestamp, std::string> checkpoint;
            checkpoint.first = checkpointJSON["timestamp"];
            checkpoint.second = checkpointJSON["data"];
            systemData.checkpointData.push_back(checkpoint);
        }
    }
    
    // Convert resource utilization from JSON
    if (data.contains("resourceUtilization")) {
        for (const auto& resourceJSON : data["resourceUtilization"]) {
            std::pair<Timestamp, double> resource;
            resource.first = resourceJSON["timestamp"];
            resource.second = resourceJSON["utilization"];
            systemData.resourceUtilization.push_back(resource);
        }
    }
    
    return systemData;
}

std::string NVMVisualization::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::vector<std::string> NVMVisualization::splitLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    
    while (std::getline(ss, token, ',')) {
        // Remove leading/trailing whitespace
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
        tokens.push_back(token);
    }
    
    return tokens;
}

NeuronData NVMVisualization::parseNeuronFromCSV(const std::string& line, const std::vector<std::string>& headers) {
    NeuronData neuron;
    std::stringstream ss(line);
    std::string token;
    
    for (size_t i = 0; i < headers.size() && std::getline(ss, token, ','); ++i) {
        if (headers[i] == "NeuronID") {
            neuron.id = std::stoi(token);
        } else if (headers[i] == "X") {
            neuron.x = std::stof(token);
        } else if (headers[i] == "Y") {
            neuron.y = std::stof(token);
        } else if (headers[i] == "Z") {
            neuron.z = std::stof(token);
        } else if (headers[i] == "ActivityLevel") {
            neuron.activityLevel = std::stof(token);
        } else if (headers[i] == "MembranePotential") {
            neuron.membranePotential = std::stof(token);
        } else if (headers[i] == "Threshold") {
            neuron.threshold = std::stof(token);
        } else if (headers[i] == "LastSpikeTime") {
            neuron.lastSpikeTime = std::stof(token);
        }
    }
    
    return neuron;
}

SynapseData NVMVisualization::parseSynapseFromCSV(const std::string& line, const std::vector<std::string>& headers) {
    SynapseData synapse;
    std::stringstream ss(line);
    std::string token;
    
    for (size_t i = 0; i < headers.size() && std::getline(ss, token, ','); ++i) {
        if (headers[i] == "SynapseID") {
            synapse.id = std::stoi(token);
        } else if (headers[i] == "PreNeuronID") {
            synapse.preNeuronId = std::stoi(token);
        } else if (headers[i] == "PostNeuronID") {
            synapse.postNeuronId = std::stoi(token);
        } else if (headers[i] == "Weight") {
            synapse.weight = std::stof(token);
        } else if (headers[i] == "Delay") {
            synapse.delay = std::stof(token);
        } else if (headers[i] == "LastUpdateTime") {
            synapse.lastUpdateTime = std::stof(token);
        } else if (headers[i] == "Type") {
            synapse.type = token;
        }
    }
    
    return synapse;
}

} // namespace nlm
