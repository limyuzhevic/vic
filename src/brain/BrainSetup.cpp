// BrainSetup.cpp
#include "BrainSetup.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct BrainSetup::Impl {
    std::shared_ptr<BrainCore> core;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Neurons for I/O
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    std::unordered_map<uint64_t, Neuron*> neuronById;
    bool spikeHandlersRegistered;
    
    // Statistics
    size_t totalNeuronCount;
    size_t totalSynapseCount;
    
    Impl(std::shared_ptr<BrainCore> brainCore) : core(brainCore), spikeHandlersRegistered(false),
                                                 totalNeuronCount(0), totalSynapseCount(0) {}
    
    ~Impl() {
        regions.clear();
        sensoryNeurons.clear();
        motorNeurons.clear();
        neuronById.clear();
    }
};

BrainSetup::BrainSetup(std::shared_ptr<BrainCore> core) : pImpl(new Impl(core)) {}

BrainSetup::~BrainSetup() = default;

BrainSetup::BrainSetup(BrainSetup&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

BrainSetup& BrainSetup::operator=(BrainSetup&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool BrainSetup::initialize(std::shared_ptr<Config> config) {
    // Clear previous state
    pImpl->regions.clear();
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    pImpl->neuronById.clear();
    pImpl->totalNeuronCount = 0;
    pImpl->totalSynapseCount = 0;
    
    size_t neuronCount = static_cast<size_t>(config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(config->getOr<int64_t>("region_count", 1));
    float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        pImpl->regions.push_back(std::make_unique<NeuralRegion>(RegionId(i + 1)));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t nextNeuronIndex = 1;  // globally unique IDs across regions
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = pImpl->regions[i].get();
        region->setNextNeuronIndex(nextNeuronIndex);
        
        // Add populations with balanced excitation/inhibition
        auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
        size_t internalCount = neuronsPerRegion / 2;
        size_t internalExcitatory = internalCount * 4 / 5;
        auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
        auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
        (void)internalPopId; (void)inhibitoryPopId;
        auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
        
        // Collect sensory and motor neurons for I/O
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        auto* motorPop = region->getPopulation(motorPopId);
        if (sensoryPop) {
            for (auto* neuron : sensoryPop->getNeurons()) {
                pImpl->sensoryNeurons.push_back(neuron);
                pImpl->neuronById[neuron->getId().index()] = neuron;
            }
        }
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                pImpl->motorNeurons.push_back(neuron);
                pImpl->neuronById[neuron->getId().index()] = neuron;
            }
        }
        
        NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) +
                    ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons");
        nextNeuronIndex += region->getTotalNeuronCount();
    }
    
    pImpl->totalNeuronCount = neuronCount;
    pImpl->totalSynapseCount = static_cast<size_t>(neuronCount * neuronsPerRegion * connectionProbability);
    
    NLM_LOG_INFO("Brain setup complete: " + std::to_string(pImpl->regions.size()) + " regions, " +
                 std::to_string(pImpl->totalNeuronCount) + " neurons, " +
                 std::to_string(pImpl->totalSynapseCount) + " estimated synapses");
    
    return true;
}

void BrainSetup::reset() {
    pImpl->regions.clear();
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    pImpl->neuronById.clear();
    pImpl->spikeHandlersRegistered = false;
    pImpl->totalNeuronCount = 0;
    pImpl->totalSynapseCount = 0;
}

NeuralRegion* BrainSetup::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* BrainSetup::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t BrainSetup::getRegionCount() const {
    return pImpl->regions.size();
}

const std::vector<RegionId>& BrainSetup::getRegionIds() const {
    static std::vector<RegionId> ids;
    ids.clear();
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

void BrainSetup::integrateSystems(std::shared_ptr<Brain> brain) {
    // This method integrates all external systems with the brain
    // The actual integration logic is handled by the main Brain class
    // This is a placeholder for future system integration patterns
    NLM_LOG_DEBUG("Integrating systems with brain");
}

} // namespace nlm