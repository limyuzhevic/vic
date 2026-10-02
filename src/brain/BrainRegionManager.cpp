// Brain Region Manager implementation

RegionId BrainRegionManager::addRegion(std::shared_ptr<Brain> brain, const std::string& name) {
    return brain->addRegion(name);
}

NeuralRegion* BrainRegionManager::getRegion(std::shared_ptr<Brain> brain, RegionId id) {
    return brain->getRegion(id);
}

const NeuralRegion* BrainRegionManager::getRegion(std::shared_ptr<Brain> brain, RegionId id) const {
    return brain->getRegion(id);
}

const std::vector<std::unique_ptr<NeuralRegion>>& BrainRegionManager::getRegions(std::shared_ptr<Brain> brain) const {
    return brain->getRegions();
}

size_t BrainRegionManager::getRegionCount(std::shared_ptr<Brain> brain) const {
    return brain->getRegionCount();
}

void BrainRegionManager::createNeuronsInRegion(std::shared_ptr<Brain> brain, RegionId regionId,
                                               size_t neuronCount, size_t regionCount,
                                               float connectionProbability,
                                               std::shared_ptr<RandomGenerator> rng) {
    auto* region = getRegion(brain, regionId);
    if (!region) {
        NLM_LOG_ERROR("Region not found: " + std::to_string(regionId.index()));
        return;
    }
    
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    // Add populations
    auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
    auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
    auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
    
    // Collect sensory and motor neurons for I/O
    collectSensoryAndMotorNeurons(brain, regionId);
    
    // Initialize random connectivity and synapse weights
    region->initializeRandomConnectivity(*rng, connectionProbability, 0.2f, 0.1f);
}

void BrainRegionManager::collectSensoryAndMotorNeurons(std::shared_ptr<Brain> brain, RegionId regionId) {
    auto* region = getRegion(brain, regionId);
    if (!region) return;
    
    auto* brainImpl = getImpl(brain);
    if (!brainImpl) return;
    
    // Clear existing collections
    brainImpl->sensoryNeurons.clear();
    brainImpl->motorNeurons.clear();
    
    // Actually we need to get populations by type
    for (auto& pop : region->getPopulations()) {
        if (pop->getNeuronType() == NeuronType::Sensory) {
            for (auto* neuron : pop->getNeurons()) {
                brainImpl->sensoryNeurons.push_back(neuron);
            }
        } else if (pop->getNeuronType() == NeuronType::Motor) {
            for (auto* neuron : pop->getNeurons()) {
                brainImpl->motorNeurons.push_back(neuron);
            }
        }
    }
}

// Helper method to get the Impl pointer from Brain
Brain::Impl* BrainRegionManager::getImpl(std::shared_ptr<Brain> brain) {
    return brain->pImpl;
}