#include "NetworkVisualizer.hpp"
#include "../core/Logger.hpp"
#include "../brain/SpikeSystem.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

namespace nlm {

struct NetworkVisualizer::Impl {
    // Data containers
    std::vector<NeuronInfo> neuronData;
    std::vector<SynapseInfo> synapseData;
    std::vector<PopulationInfo> populationData;
    std::vector<RegionInfo> regionData;
    
    // Filter settings
    std::string regionFilter;
    NeuronType neuronTypeFilter;
    bool activityFilter;
    bool connectionFilter;
    
    // Visualization parameters
    float nodeSize;
    float edgeWidth;
    std::string layoutAlgorithm;
    std::string colorScheme;
    
    // Update tracking
    double lastUpdateTime;
    bool initialized;
    
    Impl() : neuronTypeFilter(NeuronType::EXCITATORY), 
             activityFilter(false), connectionFilter(false),
             nodeSize(2.0f), edgeWidth(1.0f),
             layoutAlgorithm("force-directed"), colorScheme("default"),
             lastUpdateTime(0.0), initialized(false) {}
};

NetworkVisualizer::NetworkVisualizer() : pImpl(std::make_unique<Impl>()) {}
NetworkVisualizer::~NetworkVisualizer() = default;

bool NetworkVisualizer::initialize() {
    try {
        pImpl->initialized = true;
        Logger::getInstance().logInfo("Network visualizer initialized");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to initialize network visualizer: " + std::string(e.what()));
        return false;
    }
}

void NetworkVisualizer::update(double deltaTime) {
    pImpl->lastUpdateTime += deltaTime;
}

void NetworkVisualizer::renderNetworkTopology(const Brain& brain) {
    // Extract and process brain data
    pImpl->regionData.clear();
    pImpl->populationData.clear();
    pImpl->neuronData.clear();
    pImpl->synapseData.clear();
    
    // Collect region information
    auto regions = brain.getRegions();
    for (const auto& regionPtr : regions) {
        RegionInfo info;
        info.id = regionPtr->getId();
        info.name = regionPtr->getName();
        // Extract spatial coordinates if available
        for (int i = 0; i < 3; ++i) {
            info.spatialCoordinates[i] = 0.0f; // Placeholder
        }
        info.volume = 1000.0f; // Placeholder
        info.neuronCount = regionPtr->getNeuronCount();
        info.averageFiringRate = regionPtr->getAverageFiringRate();
        info.totalSynapticWeight = regionPtr->getTotalSynapticWeight();
        info.efficiency = (info.neuronCount > 0) ? info.totalSynapticWeight / info.neuronCount : 0.0f;
        info.isActive = (info.averageFiringRate > 0.0f);
        info.isConnectedToOthers = (brain.getTotalSynapseCount() > 0);
        
        pImpl->regionData.push_back(info);
    }
    
    // Collect population information
    for (const auto& regionPtr : regions) {
        auto populations = regionPtr->getPopulations();
        for (const auto& populationPtr : populations) {
            PopulationInfo info;
            info.id = populationPtr->getId();
            info.name = populationPtr->getName();
            info.neuronType = populationPtr->getNeuronType();
            info.regionId = regionPtr->getId();
            info.neuronCount = populationPtr->getNeuronCount();
            info.averageFiringRate = populationPtr->getAverageFiringRate();
            info.totalSynapticWeight = populationPtr->getTotalSynapticWeight();
            info.efficiency = (info.neuronCount > 0) ? info.totalSynapticWeight / info.neuronCount : 0.0f;
            info.isActive = (info.averageFiringRate > 0.0f);
            
            pImpl->populationData.push_back(info);
        }
    }
    
    // Collect neuron information
    for (const auto& regionPtr : regions) {
        auto neurons = regionPtr->getNeurons();
        for (const auto& neuronPtr : neurons) {
            NeuronInfo info;
            info.id = neuronPtr->getId();
            info.type = neuronPtr->getType();
            info.membranePotential = neuronPtr->getMembranePotential();
            info.firingRate = neuronPtr->getFiringRate();
            info.efficiency = neuronPtr->getConnectivity();
            info.regionName = regionPtr->getName();
            info.populationName = neuronPtr->getPopulationName();
            info.isActive = neuronPtr->isActive();
            info.isFiring = neuronPtr->isFiring();
            info.connectivity = neuronPtr->getConnectivity();
            
            pImpl->neuronData.push_back(info);
        }
    }
    
    // Collect synapse information
    for (const auto& regionPtr : regions) {
        auto neurons = regionPtr->getNeurons();
        for (const auto& neuronPtr : neurons) {
            auto synapses = neuronPtr->getOutgoingSynapses();
            for (const auto& synapsePtr : synapses) {
                SynapseInfo info;
                info.id = synapsePtr->getId();
                info.preNeuron = synapsePtr->getPreNeuron();
                info.postNeuron = synapsePtr->getPostNeuron();
                info.weight = synapsePtr->getWeight();
                info.maxWeight = synapsePtr->getMaxWeight();
                info.delay = synapsePtr->getDelay();
                info.plasticityType = synapsePtr->getPlasticityType();
                info.isActive = synapsePtr->isActive();
                info.isLTP = (synapsePtr->getPlasticityType() == PlasticityType::LTP);
                info.isLTD = (synapsePtr->getPlasticityType() == PlasticityType::LTD);
                info.preRegion = regionPtr->getName();
                
                // Find post region
                for (const auto& otherRegionPtr : regions) {
                    auto otherNeurons = otherRegionPtr->getNeurons();
                    for (const auto& otherNeuronPtr : otherNeurons) {
                        if (otherNeuronPtr->getId() == synapsePtr->getPostNeuron()) {
                            info.postRegion = otherRegionPtr->getName();
                            break;
                        }
                    }
                }
                
                pImpl->synapseData.push_back(info);
            }
        }
    }
}

void NetworkVisualizer::renderConnections(const Brain& brain) {
    renderNetworkTopology(brain);
    // Apply filters and render connections
}

void NetworkVisualizer::renderRegions(const Brain& brain) {
    renderNetworkTopology(brain);
    // Render region structures and connections
}

void NetworkVisualizer::renderPopulations(const Brain& brain) {
    renderNetworkTopology(brain);
    // Render population clusters within regions
}

void NetworkVisualizer::renderNeurons(const Brain& brain) {
    renderNetworkTopology(brain);
    // Render individual neurons based on their properties
}

void NetworkVisualizer::setRegionFilter(const std::string& regionName) {
    pImpl->regionFilter = regionName;
}

void NetworkVisualizer::setNeuronTypeFilter(NeuronType type) {
    pImpl->neuronTypeFilter = type;
}

void NetworkVisualizer::setActivityFilter(bool showActiveOnly) {
    pImpl->activityFilter = showActiveOnly;
}

void NetworkVisualizer::setConnectionFilter(bool showStrongConnectionsOnly) {
    pImpl->connectionFilter = showStrongConnectionsOnly;
}

void NetworkVisualizer::setNodeSize(float size) {
    pImpl->nodeSize = size;
}

void NetworkVisualizer::setEdgeWidth(float width) {
    pImpl->edgeWidth = width;
}

void NetworkVisualizer::setLayoutAlgorithm(const std::string& algorithm) {
    pImpl->layoutAlgorithm = algorithm;
}

void NetworkVisualizer::setColorScheme(const std::string& scheme) {
    pImpl->colorScheme = scheme;
}

size_t NetworkVisualizer::getTotalNeuronCount() const {
    return pImpl->neuronData.size();
}

size_t NetworkVisualizer::getTotalSynapseCount() const {
    return pImpl->synapseData.size();
}

size_t NetworkVisualizer::getActiveRegionCount() const {
    size_t activeCount = 0;
    for (const auto& region : pImpl->regionData) {
        if (region.isActive) activeCount++;
    }
    return activeCount;
}

float NetworkVisualizer::getAverageNetworkEfficiency() const {
    if (pImpl->neuronData.empty()) return 0.0f;
    
    float totalEfficiency = 0.0f;
    for (const auto& neuron : pImpl->neuronData) {
        totalEfficiency += neuron.efficiency;
    }
    
    return totalEfficiency / pImpl->neuronData.size();
}

bool NetworkVisualizer::exportNetworkGraph(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Export in DOT format for graph visualization
    file << "digraph NetworkTopology {\n";
    file << "  rankdir=TB;\n";
    file << "  node [shape=circle, style=filled];\n";
    
    // Export neurons as nodes
    for (const auto& neuron : pImpl->neuronData) {
        std::string color = neuron.isFiring ? "red" : "blue";
        file << "  neuron_" << neuron.id << " [label=\"" << neuron.id 
             << "\", fillcolor=" << color << "];\n";
    }
    
    // Export synapses as edges
    for (const auto& synapse : pImpl->synapseData) {
        file << "  neuron_" << synapse.preNeuron << " -> neuron_" << synapse.postNeuron
             << " [label=\"" << synapse.weight << "\", color=" << 
             (synapse.isLTP ? "green" : (synapse.isLTD ? "orange" : "black"))
             << "];\n";
    }
    
    file << "}\n";
    file.close();
    
    Logger::getInstance().logInfo("Network graph exported to: " + filename);
    return true;
}

bool NetworkVisualizer::exportNetworkData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Export comprehensive network data
    file << "NeuronCount," << pImpl->neuronData.size() << "\n";
    file << "SynapseCount," << pImpl->synapseData.size() << "\n";
    file << "RegionCount," << pImpl->regionData.size() << "\n";
    file << "PopulationCount," << pImpl->populationData.size() << "\n";
    file << "\n";
    
    // Neuron data
    file << "NeuronData:NeuronId,NeuronType,MembranePotential,FiringRate,Efficiency,Region,Population,Active,IsFiring,Connectivity\n";
    for (const auto& neuron : pImpl->neuronData) {
        file << neuron.id << "," << static_cast<int>(neuron.type) << "," 
             << neuron.membranePotential << "," << neuron.firingRate << "," 
             << neuron.efficiency << "," << neuron.regionName << "," 
             << neuron.populationName << "," << neuron.isActive << "," 
             << neuron.isFiring << "," << neuron.connectivity << "\n";
    }
    
    file << "\nSynapseData:SynapseId,PreNeuron,PostNeuron,Weight,MaxWeight,Delay,PlasticityType,Active,IsLTP,IsLTD,PreRegion,PostRegion\n";
    for (const auto& synapse : pImpl->synapseData) {
        file << synapse.id << "," << synapse.preNeuron << "," << synapse.postNeuron << "," 
             << synapse.weight << "," << synapse.maxWeight << "," << synapse.delay << "," 
             << static_cast<int>(synapse.plasticityType) << "," << synapse.isActive << "," 
             << synapse.isLTP << "," << synapse.isLTD << "," << synapse.preRegion << "," 
             << synapse.postRegion << "\n";
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Network data exported to: " + filename);
    return true;
}

} // namespace nlm