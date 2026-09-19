#include "BrainCore.hpp"

namespace nlm {

BrainCore::BrainCore(std::shared_ptr<Config> config)
    : config(config)
    , rng(nullptr)
    , spikeSystem(nullptr)
    , stdp(nullptr)
    , hebbian(nullptr)
    , structuralPlasticity(nullptr)
    , timestep(0.001)
    , currentStep(0)
    , currentTime(0.0)
    , totalSpikesThisStep(0)
    , totalSpikesTotal(0)
{
    // Initialize random generator with seed from config
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = config->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng = std::make_unique<RandomGenerator>(seed);
    
    // Initialize plasticity systems
    spikeSystem = std::make_unique<SpikeSystem>();
    stdp = std::make_unique<STDP>();
    hebbian = std::make_unique<Hebbian>();
    structuralPlasticity = std::make_unique<StructuralPlasticity>();
}

BrainCore::~BrainCore() = default;

bool BrainCore::initialize() {
    // Get configuration values
    size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config->getOr<size_t>("region_count", 1);
    float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    
    // TODO: Log configuration values
    
    // Create regions (placeholder for actual region creation logic)
    // For now, we assume regions are added externally
    
    // Collect sensory and motor neurons
    collectSensoryAndMotorNeurons();
    
    // Initialize connectivity with random weights (placeholder)
    // region->initializeRandomConnectivity(*rng, connectionProbability, 0.2f, 0.1f);
    
    // Configure plasticity systems
    initializePlasticitySystems();
    
    return true;
}

void BrainCore::step(SimulationStep currentStep, Timestamp currentTime) {
    this->currentStep = currentStep;
    this->currentTime = currentTime;
    totalSpikesThisStep = 0;
    
    // Step 1: Process pending delayed spikes
    processDelayedSpikes(currentStep, currentTime);
    
    // Step 2: Update all neurons (LIF dynamics)
    updateNeurons(currentTime, timestep);
    
    // Step 3: Detect spikes and schedule spike events
    detectSpikes(currentStep, currentTime);
    
    // Step 4: Process immediate spikes
    processSpikes(currentStep);
}

void BrainCore::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Forward to spike system for now
    if (spikeSystem) {
        spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
}

void BrainCore::updateNeurons(Timestamp currentTime, TimestepDuration timestep) {
    // Forward to regions for now
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void BrainCore::detectSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Forward to regions for now
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);
                
                if (justFired) {
                    // Increment spike count
                    ++totalSpikesThisStep;
                    ++totalSpikesTotal;
                }
            }
        }
    }
}

void BrainCore::processSpikes(SimulationStep currentStep) {
    // Forward to spike system for now
    if (spikeSystem) {
        spikeSystem->processSpikes(currentStep);
    }
}

void BrainCore::processDelayedSpikesCore(SimulationStep currentStep, Timestamp currentTime) {
    if (!spikeSystem) return;
    
    // Implementation of delayed spike processing
    spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void BrainCore::updateNeuronsCore(Timestamp currentTime, TimestepDuration timestep) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void BrainCore::detectSpikesCore(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);
                
                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    if (spikeSystem) {
                        spikeSystem->queueSpike(event);
                    }
                    
                    // Record pre-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        if (spikeSystem) {
                            spikeSystem->registerPreSpike(syn->getId(), neuron->getId(), currentTime);
                        }
                        syn->recordPostSpike(currentTime);
                    }
                    
                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * timestep;
                        
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
                        
                        if (spikeSystem) {
                            spikeSystem->queueDelayedSpike(delayedEvent);
                        }
                        
                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                }
            }
        }
    }
}

NeuralRegion* BrainCore::getRegion(RegionId id) {
    for (auto& region : regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t BrainCore::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t BrainCore::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getSynapseCount();
    }
    total += interRegionConnections.size();
    return total;
}

size_t BrainCore::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t BrainCore::getFiringNeuronCount() const {
    return totalSpikesThisStep;
}

float BrainCore::getAverageFiringRate() const {
    if (regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(regions.size());
}

float BrainCore::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t BrainCore::getTotalSpikeCount() const {
    return totalSpikesTotal;
}

size_t BrainCore::getPendingSpikeEventCount() const {
    if (!spikeSystem) return 0;
    return spikeSystem->getPendingSpikeCount() + spikeSystem->getPendingDelayedCount();
}

void BrainCore::receiveSensoryInput(const SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        if (sensoryNeurons[i]) {
            sensoryNeurons[i]->injectCurrent(normalizedValue);
        }
    }
}

void BrainCore::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void BrainCore::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

std::unique_ptr<Action> BrainCore::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    
    return action;
}

void BrainCore::reset() {
    currentStep = 0;
    currentTime = 0.0;
    totalSpikesThisStep = 0;
    totalSpikesTotal = 0;
    
    // Reset all regions
    for (auto& region : regions) {
        region->reset();
    }
    
    // Reset spike system
    if (spikeSystem) {
        spikeSystem->reset();
    }
}

void BrainCore::logStatus() const {
    // TODO: Add logging implementation
}

void BrainCore::initializePlasticitySystems() {
    // Configure STDP parameters
    configureSTDPParameters();
    
    // Configure structural plasticity
    configureStructuralPlasticityParameters();
}

void BrainCore::configureSTDPParameters() {
    if (!stdp) return;
    
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    
    stdp->configure(ltpWeight, ltdWeight, tau);
}

void BrainCore::configureStructuralPlasticityParameters() {
    if (!structuralPlasticity) return;
    
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    
    structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity->setPruningRate(pruningRate);
}

void BrainCore::collectSensoryAndMotorNeurons() {
    // Clear existing collections
    sensoryNeurons.clear();
    motorNeurons.clear();
    
    // Collect sensory and motor neurons from regions
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            // TODO: Check population type based on NeuronType enum
            // For now, just collect all neurons
            auto neurons = pop->getNeurons();
            // TODO: Filter based on neuron type if needed
        }
    }
}

} // namespace nlm