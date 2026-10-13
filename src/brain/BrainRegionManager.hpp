#pragma once

#include "../core/Config/Config.hpp"
#include "NeuralRegion.hpp"
#include "BrainInitializer.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class Brain;
class Neuron;

class BrainRegionManager {
public:
    explicit BrainRegionManager(std::shared_ptr<Config> config);
    ~BrainRegionManager() = default;
    
    // Create and initialize regions
    void createRegions(Brain* brain, size_t neuronCount, size_t regionCount, 
                      float connectionProbability);
    
    // Create neurons within regions
    void createNeuronsInRegions(Brain* brain, size_t neuronCount, size_t regionCount);
    
    // Setup connectivity between neurons
    void setupConnectivity(Brain* brain, float connectionProbability);
    
    // Collect sensory and motor neurons for I/O
    void collectSensoryMotorNeurons(Brain* brain);
    
    // Get region information
    std::vector<std::unique_ptr<NeuralRegion>>& getRegions() { return regions; }
    NeuralRegion* getRegion(Brain* brain, RegionId id);
    const NeuralRegion* getRegion(Brain* brain, RegionId id) const;
    
    // Region statistics
    size_t getRegionCount() const { return regionCount; }
    size_t getTotalNeuronCount() const { return totalNeuronCount; }
    size_t getTotalSynapseCount() const { return totalSynapseCount; }
    float getAverageSynapticWeight() const { return averageSynapticWeight; }
    
    // Add and remove regions
    RegionId addRegion(Brain* brain, const std::string& name);
    void removeRegion(Brain* brain, RegionId id);
    
    // Inter-region connection management
    void addInterRegionConnection(Brain* brain, RegionId source, RegionId target, 
                                 float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(Brain* brain, RegionId source, RegionId target);
    
    // Reset all regions
    void resetRegions(Brain* brain);

private:
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    
    // Region management
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Configuration parameters
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    size_t totalNeuronCount;
    size_t totalSynapseCount;
    float averageSynapticWeight;
    
    // Helper methods
    void initializeRegionConnectivity(Brain* brain, float connectionProbability);
    void distributeNeurons(Brain* brain, size_t neuronCount, size_t regionCount);
    void collectSensoryNeurons(Brain* brain);
    void collectMotorNeurons(Brain* brain);
    void updateRegionStatistics(Brain* brain);
    
    // Population creation
    void createPopulationsInRegion(Brain* brain, NeuralRegion* region, 
                                  size_t neuronsPerRegion);
    
    // Connectivity initialization
    void initializeRandomConnectivity(Brain* brain, float connectionProbability, 
                                     float weightMean, float weightStdDev);
};