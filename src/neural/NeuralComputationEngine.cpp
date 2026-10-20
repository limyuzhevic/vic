#include "NeuralComputationEngine.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"

namespace nlm {

struct NeuralComputationEngine::Impl {
    std::shared_ptr<Config> config;
    Brain* brain;
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    Impl(std::shared_ptr<Config> cfg) 
        : config(cfg)
        , brain(nullptr)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0) {
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
    }
    
    ~Impl() = default;
    
    void setupSpikeHandlers();
};

NeuralComputationEngine::NeuralComputationEngine(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

NeuralComputationEngine::~NeuralComputationEngine() {
    delete pImpl;
}

bool NeuralComputationEngine::initialize(Brain* brain) {
    NLM_LOG_INFO("Initializing NeuralComputationEngine...");
    
    pImpl->brain = brain;
    pImpl->setupSpikeHandlers();
    
    // Configure timestep
    pImpl->timestep = pImpl->config->getOr<double>("simulation_timestep", 0.001);
    
    NLM_LOG_INFO("NeuralComputationEngine initialized successfully");
    return true;
}

void NeuralComputationEngine::update(Timestamp time, TimestepDuration timestep) {
    // Phase 1: Process pending delayed spikes
    pImpl->spikeSystem->processDelayedSpikes(pImpl->currentStep, time);
    
    // Phase 2: Update all neurons (LIF dynamics)
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(time, timestep);
            }
        }
    }
    
    // Phase 3: Detect spikes and schedule spike events
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(time) - state.lastSpikeTime) < timestep * 2.0f);
                
                if (justFired) {
                    // Queue the spike
                    SpikeEvent event(neuron->getId(), time, pImpl->currentStep);
                    pImpl->spikeSystem->queueSpike(event);
                    
                    // Record spike for plasticity
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (auto* syn : incomingSynapses) {
                        syn->recordPostSpike(time);
                    }
                    
                    // Schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (auto* syn : outgoingSynapses) {
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = pImpl->currentStep + delay;
                        Timestamp deliveryTime = time + delay * timestep;
                        
                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(), syn->getDestinationNeuron(),
                            syn->getId(), syn->getWeight(), syn->getType(),
                            time, deliveryTime, pImpl->currentStep, deliveryStep
                        );
                        
                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                        syn->recordPreSpike(time);
                    }
                    
                    // Store in working memory
                    if (pImpl->brain->getWorkingMemory()) {
                        pImpl->brain->getWorkingMemory()->storeToNeuron(neuron->getId(),
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    pImpl->spikeSystem->processSpikes(pImpl->currentStep);
    pImpl->totalSpikesThisStep = 0;
}

void NeuralComputationEngine::Impl::setupSpikeHandlers() {
    spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        ++totalSpikesThisStep;
        ++totalSpikesTotal;
    });
    
    spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        for (auto& region : brain->getRegions()) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    MembranePotential synapticCurrent = event.weight * 10.0f;
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

size_t NeuralComputationEngine::getPendingSpikeCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

float NeuralComputationEngine::getExcitatoryInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->brain->getRegions()) {
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

void NeuralComputationEngine::reset() {
    NLM_LOG_INFO("Resetting NeuralComputationEngine...");
    pImpl->totalSpikesThisStep = 0;
    for (auto& region : pImpl->brain->getRegions()) {
        region->reset();
    }
    pImpl->spikeSystem->reset();
    NLM_LOG_INFO("NeuralComputationEngine reset complete");
}

} // namespace nlm