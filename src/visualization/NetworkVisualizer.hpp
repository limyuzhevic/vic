#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../development/DevelopmentSystem.hpp"

namespace nlm {

// Neural network topology visualization
struct NeuronInfo {
    NeuronId id;
    NeuronType type;
    float membranePotential;
    float firingRate;
    float efficiency;
    std::string regionName;
    std::string populationName;
    bool isActive;
    bool isFiring;
    float connectivity; // Number of connections
};

struct SynapseInfo {
    SynapseId id;
    NeuronId preNeuron;
    NeuronId postNeuron;
    float weight;
    float maxWeight;
    float delay;
    PlasticityType plasticityType;
    bool isActive;
    bool isLTP;
    bool isLTD;
    std::string preRegion;
    std::string postRegion;
};

struct PopulationInfo {
    PopulationId id;
    std::string name;
    NeuronType neuronType;
    RegionId regionId;
    size_t neuronCount;
    float averageFiringRate;
    float totalSynapticWeight;
    float efficiency;
    bool isActive;
};

struct RegionInfo {
    RegionId id;
    std::string name;
    float spatialCoordinates[3];
    float volume;
    size_t neuronCount;
    float averageFiringRate;
    float totalSynapticWeight;
    float efficiency;
    bool isActive;
    bool isConnectedToOthers;
};

class NetworkVisualizer {
public:
    NetworkVisualizer();
    ~NetworkVisualizer();
    
    // Initialize visualizer
    bool initialize();
    
    // Update network data
    void update(double deltaTime);
    
    // Render network topology
    void renderNetworkTopology(const Brain& brain);
    
    // Render synaptic connections
    void renderConnections(const Brain& brain);
    
    // Render regions and populations
    void renderRegions(const Brain& brain);
    void renderPopulations(const Brain& brain);
    
    // Render neurons
    void renderNeurons(const Brain& brain);
    
    // Filter settings
    void setRegionFilter(const std::string& regionName);
    void setNeuronTypeFilter(NeuronType type);
    void setActivityFilter(bool showActiveOnly);
    void setConnectionFilter(bool showStrongConnectionsOnly);
    
    // Visualization parameters
    void setNodeSize(float size);
    void setEdgeWidth(float width);
    void setLayoutAlgorithm(const std::string& algorithm);
    void setColorScheme(const std::string& scheme);
    
    // Data access
    const std::vector<NeuronInfo>& getNeuronData() const { return neuronData; }
    const std::vector<SynapseInfo>& getSynapseData() const { return synapseData; }
    const std::vector<PopulationInfo>& getPopulationData() const { return populationData; }
    const std::vector<RegionInfo>& getRegionData() const { return regionData; }
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveRegionCount() const;
    float getAverageNetworkEfficiency() const;
    
    // Export capabilities
    bool exportNetworkGraph(const std::string& filename);
    bool exportNetworkData(const std::string& filename);
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm