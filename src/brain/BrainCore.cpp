#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct BrainCore::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Phase 5 learned preference trait
    float learnedPreference = 0.0f;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    std::unordered_map<uint64_t, Neuron*> neuronById;
    bool spikeHandlersRegistered = false;
    
    // Integration state
    bool isResting = false;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)
        , consolidationInterval(1000)
    {}
};

BrainCore::BrainCore(std::shared_ptr<Config> config)
    : pImpl(std::make_unique<Impl>(config)) {
    // Initialize random generator
    uint64_t seed = 42;
    if (auto seedOpt = config->get<int64_t>("random_seed")) {
        seed = *seedOpt;
    }
    pImpl->rng = std::make_unique<RandomGenerator>(seed);
    
    // Initialize plasticity systems
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
}

BrainCore::~BrainCore() = default;

bool BrainCore::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Re-initialising must rebuild, not append
    pImpl->regions.clear();
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    pImpl->neuronById.clear();
    pImpl->totalSpikesTotal = 0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    
    // Get configuration values
    size_t neuronCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("region_count", 1));
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        // Add region to core
        pImpl->regions.push_back(std::make_unique<NeuralRegion>("Region_" + std::to_string(i + 1)));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t nextNeuronIndex = 1;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = pImpl->regions[i].get();
        region->setNextNeuronIndex(nextNeuronIndex);
        
        // Add populations (Sensory, Internal, Inhibitory, Motor)
        size_t sensoryCount = neuronsPerRegion / 4;
        region->addPopulation(sensoryCount, NeuronType::Sensory);
        
        size_t internalCount = neuronsPerRegion / 2;
        size_t internalExcitatory = internalCount * 4 / 5;
        region->addPopulation(internalExcitatory, NeuronType::Internal);
        region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
        
        size_t motorCount = neuronsPerRegion / 4;
        region->addPopulation(motorCount, NeuronType::Motor);
        
        // Collect sensory and motor neurons for I/O
        auto* sensoryPop = region->getPopulation(RegionId(i + 1), NeuronType::Sensory);
        auto* motorPop = region->getPopulation(RegionId(i + 1), NeuronType::Motor);
        
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
        
        nextNeuronIndex += region->getTotalNeuronCount();
    }
    
    // Build the neuron lookup used by synaptic delivery and injection
    for (auto& region : pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            pImpl->neuronById[neuron->getId().index()] = neuron;
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = pImpl->regions[i].get();
        region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
    }
    
    // Configure STDP parameters
    float ltpWeight = pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = pImpl->config->getOr<float>("stdp_tau", 20.0f);
    pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = pImpl->config->getOr<float>("pruning_rate", 0.00001f);
    pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    pImpl->structuralPlasticity->setPruningRate(pruningRate);
    
    // Get timestep
    pImpl->timestep = pImpl->config->getOr<double>("simulation_timestep", 0.001);
    
    // Get integration intervals
    pImpl->replayInterval = static_cast<size_t>(pImpl->config->getOr<int64_t>("replay_interval", 100));
    pImpl->consolidationInterval = static_cast<size_t>(pImpl->config->getOr<int64_t>("consolidation_interval", 1000));
    
    // Initialize checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager = std::make_unique<CheckpointManager>();
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

void BrainCore::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

void BrainCore::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void BrainCore::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Process pending delayed spikes (deliver synaptic input)
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // Update all neurons (LIF dynamics)
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // Detect spikes and schedule spike events
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);
                
                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);
                    
                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }
                    
                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;
                        
                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            currentStep,
                            deliveryStep
                        );
                        
                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                        
                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // Update statistics
    pImpl->totalSpikesTotal += pImpl->totalSpikesThisStep;
}

// Continue with remaining methods implementation...
// (For brevity, showing the key methods)

// Forward all other method calls to pImpl

} // namespace nlm
