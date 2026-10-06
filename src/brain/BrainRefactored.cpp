// Brain implementation continues with helper functions and remaining methods...

// Implement the remaining Brain class methods

// Constructor
Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

// Destructor
Brain::~Brain() = default;

// Move constructor
Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

// Move assignment operator
Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// Get region
NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

// Get region (const)
const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

// Get region count
size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

// Get region IDs
std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

// Get all regions
const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

// Add inter-region connection
void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                      float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

// Remove inter-region connection
void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

// Get total neuron count
size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

// Get total synapse count
size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

// Get active neuron count
size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

// Get firing neuron count
size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

// Get average firing rate
float Brain::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

// Add region
RegionId Brain::addRegion(const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

// Get working memory
NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

// Get episodic memory
NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

// Get associative memory
NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// Get prediction system
PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// Get planner
NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

// Get concept formation
ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

// Get attention
AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// Get development system
DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

// Get dopamine
Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

// Get curiosity
Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

// Get novelty
Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

// Get prediction error signal
PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

// Get configuration
std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

// Get random generator
RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

} // namespace nlm
