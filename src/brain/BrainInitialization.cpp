#include "BrainInitialization.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include <algorithm>

namespace nlm {

struct BrainInitialization::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    
    // Configuration values
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    TimestepDuration timestep;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Neurons for I/O
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg) {
        // Get configuration values
        neuronCount = cfg->getOr<size_t>("neuron_count", 1000);
        regionCount = cfg->getOr<size_t>("region_count", 1);
        connectionProbability = cfg->getOr<float>("connection_probability", 0.1f);
        timestep = cfg->getOr<double>("simulation_timestep", 0.001);
        replayInterval = cfg->getOr<size_t>("replay_interval", 100);
        consolidationInterval = cfg->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
    }
};

BrainInitialization::BrainInitialization(std::shared_ptr<Config> config)
    : pImpl(std::make_unique<Impl>(config)) {}

BrainInitialization::~BrainInitialization() = default;

bool BrainInitialization::initialize(Brain* brain) {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Create regions
    createRegions(brain);
    
    // Create neurons across regions
    size_t neuronsPerRegion = pImpl->neuronCount / pImpl->regionCount;
    for (size_t i = 0; i < pImpl->regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
    
    // Setup random connectivity
    setupRandomConnectivity(brain);
    
    // Initialize all integrated systems
    initializeMemorySystems(brain);
    initializePredictionSystem(brain);
    initializeCognitionSystems(brain);
    initializeNeuromodulationSystems(brain);
    configurePlasticitySystems(brain);
    configureCheckpointManager(brain);
    registerSpikeHandlers(brain);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

size_t BrainInitialization::getNeuronCount() const {
    return pImpl->neuronCount;
}

size_t BrainInitialization::getRegionCount() const {
    return pImpl->regionCount;
}

float BrainInitialization::getConnectionProbability() const {
    return pImpl->connectionProbability;
}

TimestepDuration BrainInitialization::getTimestep() const {
    return pImpl->timestep;
}

size_t BrainInitialization::getReplayInterval() const {
    return pImpl->replayInterval;
}

size_t BrainInitialization::getConsolidationInterval() const {
    return pImpl->consolidationInterval;
}

void BrainInitialization::configurePlasticitySystems(Brain* brain) {
    // Configure STDP parameters
    float ltpWeight = pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = pImpl->config->getOr<float>("stdp_tau", 20.0f);
    brain->getSTDP()->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = pImpl->config->getOr<float>("pruning_rate", 0.00001f);
    brain->getStructuralPlasticity()->setSynaptogenesisRate(synaptogenesisRate);
    brain->getStructuralPlasticity()->setPruningRate(pruningRate);
}

void BrainInitialization::initializeMemorySystems(Brain* brain) {
    // Initialize working memory
    brain->getWorkingMemory()->initialize(brain);
    brain->getWorkingMemory()->setCapacity(pImpl->neuronCount / 10);
    
    // Initialize episodic memory
    brain->getEpisodicMemory()->initialize(brain);
    brain->getEpisodicMemory()->setMaxEpisodes(1000);
    
    // Initialize associative memory
    brain->getAssociativeMemory()->initialize(brain);
}

void BrainInitialization::initializePredictionSystem(Brain* brain) {
    // (PredictionSystem doesn't have initialize method currently)
}

void BrainInitialization::initializeCognitionSystems(Brain* brain) {
    // Initialize cognition systems
    brain->getPlanner()->initialize(brain);
    brain->getPlanner()->setPlanningDepth(5);
    
    brain->getConceptFormation()->initialize(brain);
    
    brain->getAttention()->initialize(brain);
    brain->getAttention()->setInhibitionStrength(0.5f);
    brain->getAttention()->setExcitationStrength(1.5f);
}

void BrainInitialization::initializeNeuromodulationSystems(Brain* brain) {
    brain->getNovelty()->initialize(brain);
    brain->getCuriosity()->initialize(brain);
}

void BrainInitialization::configureCheckpointManager(Brain* brain) {
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    brain->getSpikeSystem()->getCheckpointManager()->configure(checkpointDir, 10000, 5, true);
}

void BrainInitialization::createRegions(Brain* brain) {
    for (size_t i = 0; i < pImpl->regionCount; ++i) {
        brain->addRegion("Region_" + std::to_string(i + 1));
    }
}

void BrainInitialization::setupRandomConnectivity(Brain* brain) {
    for (size_t i = 0; i < pImpl->regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, pImpl->connectionProbability, 0.2f, 0.1f);
        }
    }
}

void BrainInitialization::registerSpikeHandlers(Brain* brain) {
    // Register spike handlers for event-driven processing
    brain->getSpikeSystem()->registerHandler([brain](const DetailedSpikeEvent& event) {
        // Count spikes
        ++brain->getTotalSpikeCount();
    });
    
    // Register delayed spike handler to deliver synaptic input
    brain->getSpikeSystem()->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : brain->getRegions()) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current
                    MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;
                }
            }
        }
    });
}

const std::vector<Neuron*>& BrainInitialization::getSensoryNeurons() const {
    return pImpl->sensoryNeurons;
}

const std::vector<Neuron*>& BrainInitialization::getMotorNeurons() const {
    return pImpl->motorNeurons;
}

// Note: We need access to the brain instance for some getters
// This will be handled when integrating with Brain class

} // namespace nlm
