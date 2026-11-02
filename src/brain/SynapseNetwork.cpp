// SynapseNetwork - Manages neural regions, neurons, and connectivity
#include "SynapseNetwork.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct SynapseNetwork::Impl {
    std::shared_ptr<Config> config;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    std::unordered_map<uint64_t, Neuron*> neuronById;
    std::unique_ptr<RandomGenerator> rng;
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    RegionId nextRegionId = RegionId(1);
    size_t totalSynapses = 0;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg) {
        // Initialize random generator
        uint64_t seed = 42;
        if (auto seedOpt = config->get<int64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
        
        // Configure plasticity parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
    }
    
    void injectCurrentToSensoryNeurons(float current) {
        for (auto& region : regions) {
            for (auto& neuron : region->getSensoryNeurons()) {
                neuron->injectCurrent(current);
            }
        }
    }
};

SynapseNetwork::SynapseNetwork(std::shared_ptr<Config> config) {
    pImpl = std::make_unique<Impl>(config);
    config_ = config;
}

SynapseNetwork::~SynapseNetwork() = default;

bool SynapseNetwork::initialize() {
    NLM_LOG_INFO("Initializing SynapseNetwork...");
    
    // Create regions based on configuration
    int64_t regionCount = config_->getOr<int64_t>("region_count", 2);
    int64_t neuronCount = config_->getOr<int64_t>("neuron_count", 500);
    float connectionProbability = config_->getOr<float>("connection_probability", 0.1f);
    
    // Distribute neurons across regions
    size_t neuronsPerRegion = static_cast<size_t>(neuronCount) / regionCount;
    
    for (int64_t i = 0; i < regionCount; ++i) {
        auto region = std::make_unique<NeuralRegion>();
        region->setRegionId(pImpl->nextRegionId++);
        
        // Create neurons in this region
        size_t neuronsInRegion = neuronsPerRegion;
        if (i == regionCount - 1) {
            // Last region gets remaining neurons
            neuronsInRegion = static_cast<size_t>(neuronCount) - (neuronsPerRegion * (regionCount - 1));
        }
        
        // Create neural population for this region
        auto population = std::make_unique<NeuralPopulation>(static_cast<size_t>(neuronsInRegion));
        population->setRegionId(region->getRegionId());
        
        // Generate connectivity within the region
        population->generateConnections(connectionProbability, pImpl->rng.get());
        
        // Add population to region
        region->addPopulation(std::move(population));
        
        // Set up inter-region connections
        if (i < regionCount - 1) {
            RegionId targetRegionId = pImpl->nextRegionId - 1;
            pImpl->addInterRegionConnection(region->getRegionId(), targetRegionId, 0.1f, 5);
        }
        
        pImpl->regions.push_back(std::move(region));
    }
    
    // Register neurons in the lookup map
    for (auto& region : pImpl->regions) {
        for (auto& population : region->getPopulations()) {
            for (auto& neuron : population->getNeurons()) {
                pImpl->neuronById[neuron->getId().value] = neuron.get();
            }
        }
    }
    
    NLM_LOG_INFO("SynapseNetwork initialized with " + std::to_string(getRegionCount()) + 
                " regions and " + std::to_string(getTotalNeuronCount()) + " neurons");
    return true;
}

RegionId SynapseNetwork::addRegion(const std::string& name) {
    auto region = std::make_unique<NeuralRegion>();
    region->setRegionId(pImpl->nextRegionId++);
    
    pImpl->regions.push_back(std::move(region));
    return pImpl->regions.back()->getRegionId();
}

NeuralRegion* SynapseNetwork::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getRegionId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* SynapseNetwork::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getRegionId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t SynapseNetwork::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> SynapseNetwork::getRegionIds() const {
    std::vector<RegionId> ids;
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getRegionId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& SynapseNetwork::getRegions() const {
    return pImpl->regions;
}

NeuralRegion* SynapseNetwork::getRegionForNeuron(NeuronId neuronId) {
    return getNeuron(neuronId) ? getNeuron(neuronId)->getRegion() : nullptr;
}

Neuron* SynapseNetwork::getNeuron(NeuronId neuronId) {
    auto it = pImpl->neuronById.find(neuronId.value);
    return it != pImpl->neuronById.end() ? it->second : nullptr;
}

const Neuron* SynapseNetwork::getNeuron(NeuronId neuronId) const {
    auto it = pImpl->neuronById.find(neuronId.value);
    return it != pImpl->neuronById.end() ? it->second : nullptr;
}

size_t SynapseNetwork::getTotalNeuronCount() const {
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        count += region->getTotalNeuronCount();
    }
    return count;
}

size_t SynapseNetwork::getTotalSynapseCount() const {
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& population : region->getPopulations()) {
            count += population->getSynapseCount();
        }
    }
    return count;
}

size_t SynapseNetwork::getActiveNeuronCount() const {
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& population : region->getPopulations()) {
            count += population->getActiveNeuronCount();
        }
    }
    return count;
}

size_t SynapseNetwork::getFiringNeuronCount() const {
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& population : region->getPopulations()) {
            count += population->getFiringNeuronCount();
        }
    }
    return count;
}

float SynapseNetwork::getAverageFiringRate() const {
    size_t totalNeurons = getTotalNeuronCount();
    if (totalNeurons == 0) return 0.0f;
    
    size_t firingCount = getFiringNeuronCount();
    return static_cast<float>(firingCount) / static_cast<float>(totalNeurons);
}

float SynapseNetwork::getExcitationInhibitionRatio() const {
    size_t excitatoryCount = 0;
    size_t inhibitoryCount = 0;
    
    for (const auto& region : pImpl->regions) {
        for (const auto& population : region->getPopulations()) {
            for (const auto& neuron : population->getNeurons()) {
                if (neuron->getType() == NeuronType::Excitatory) {
                    excitatoryCount++;
                } else if (neuron->getType() == NeuronType::Inhibitory) {
                    inhibitoryCount++;
                }
            }
        }
    }
    
    if (excitatoryCount + inhibitoryCount == 0) return 1.0f;
    return static_cast<float>(excitatoryCount) / static_cast<float>(inhibitoryCount);
}

void SynapseNetwork::injectCurrent(NeuronId neuronId, float current) {
    Neuron* neuron = getNeuron(neuronId);
    if (neuron) {
        neuron->injectCurrent(current);
    }
}

void SynapseNetwork::injectCurrentToNeurons(NeuronType type, float current) {
    for (auto& region : pImpl->regions) {
        for (auto& population : region->getPopulations()) {
            for (auto& neuron : population->getNeurons()) {
                if (neuron->getType() == type) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

void SynapseNetwork::updateSpiking(double timestep) {
    // Collect spikes from all neurons
    std::vector<SpikeEvent> spikes;
    
    for (auto& region : pImpl->regions) {
        for (auto& population : region->getPopulations()) {
            auto populationSpikes = population->updateSpiking(timestep);
            spikes.insert(spikes.end(), populationSpikes.begin(), populationSpikes.end());
        }
    }
    
    // Process spikes through spike system
    if (spikes.size() > 0) {
        pImpl->spikeSystem->processSpikes(spikes, timestep);
    }
}

void SynapseNetwork::addInterRegionConnection(RegionId source, RegionId target,
                                             float weight, Delay delay) {
    pImpl->addInterRegionConnection(source, target, weight, delay);
}

void SynapseNetwork::removeInterRegionConnection(RegionId source, RegionId target) {
    auto it = std::find_if(pImpl->interRegionConnections.begin(),
                         pImpl->interRegionConnections.end(),
                         [source, target](const InterRegionConnection& conn) {
        return conn.sourceRegion == source && conn.targetRegion == target;
    });
    
    if (it != pImpl->interRegionConnections.end()) {
        pImpl->interRegionConnections.erase(it);
    }
}

uint64_t SynapseNetwork::getRandomSeed() const {
    return pImpl->rng->getSeed();
}

uint64_t SynapseNetwork::drawRandomUInt(uint64_t min, uint64_t max) {
    return pImpl->rng->uniformInt(min, max);
}

bool SynapseNetwork::save(const std::string& filepath) const {
    return pImpl->checkpointManager->save(filepath, this);
}

bool SynapseNetwork::load(const std::string& filepath) {
    return pImpl->checkpointManager->load(filepath, this);
}

} // namespace nlm