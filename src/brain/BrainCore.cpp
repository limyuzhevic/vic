#include "BrainCore.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include <algorithm>

namespace nlm {

BrainCore::BrainCore(std::shared_ptr<Config> config)
    : config(config)
    , timestep(0.001), currentStep(0), currentTime(0.0)
    , totalSpikesThisStep(0), totalSpikesTotal(0)
    , developmentalStage(DevelopmentalStage::Initial)
    , learnedPreference(0.0f)
    , spikeHandlersRegistered(false) {
    
    // Initialize random generator with seed from config
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = config->get<int64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng = std::make_unique<RandomGenerator>(seed);
    
    // Configure plasticity systems
    spikeSystem = std::make_shared<SpikeSystem>();
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
    
    // Get timestep
    timestep = config->getOr<double>("simulation_timestep", 0.001);
    
    // Initialize checkpoint manager
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    checkpointManager = std::make_unique<CheckpointManager>();
    checkpointManager->configure(checkpointDir, 10000, 5, true);
}

BrainCore::~BrainCore() = default;

bool BrainCore::initialize(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                          std::vector<Neuron*>& outSensoryNeurons,
                          std::vector<Neuron*>& outMotorNeurons,
                          std::unordered_map<uint64_t, Neuron*>& outNeuronById) {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = static_cast<size_t>(config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(config->getOr<int64_t>("region_count", 1));
    float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create neurons and regions
    createNeurons(neuronCount, regionCount, connectionProbability,
                 regions, outSensoryNeurons, outMotorNeurons, outNeuronById);
    
    // Register spike handlers
    registerSpikeHandlers();
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(outSensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(outMotorNeurons.size()));
    
    return true;
}

void BrainCore::reset(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                     std::vector<Neuron*>& outSensoryNeurons,
                     std::vector<Neuron*>& outMotorNeurons,
                     std::unordered_map<uint64_t, Neuron*>& outNeuronById) {
    currentStep = 0;
    currentTime = 0.0;
    totalSpikesThisStep = 0;
    totalSpikesTotal = 0;
    
    // Reset regions
    for (auto& region : regions) {
        region->reset();
    }
    
    // Clear I/O neuron lists
    outSensoryNeurons.clear();
    outMotorNeurons.clear();
    outNeuronById.clear();
    
    // Reset spike system
    spikeSystem->reset();
    
    // Reset development stage
    developmentalStage = DevelopmentalStage::Initial;
}

void BrainCore::step(SimulationStep currentStep, Timestamp currentTime) {
    this->currentStep = currentStep;
    this->currentTime = currentTime;
    totalSpikesThisStep = 0;
    
    // Process pending delayed spikes (deliver synaptic input)
    spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // Update all neurons (LIF dynamics)
    updateNeurons();
    
    // Detect spikes and schedule spike events
    detectSpikes();
    
    // Process immediate spikes
    spikeSystem->processSpikes(currentStep);
    
    // Store working memory
    storeWorkingMemory();
    
    // Apply plasticity rules (STDP and Hebbian)
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    // Note: Neuromodulation factors would be obtained from integrated systems
    applyPlasticity(plasticityMod);
    
    // Apply homeostatic synaptic scaling (Phase 4)
    applyHomeostaticScaling(currentStep);
}

void BrainCore::updateNeurons() {
    // Update all neurons (LIF dynamics)
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void BrainCore::detectSpikes() {
    // Detect spikes and schedule outgoing spike events
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    spikeSystem->queueSpike(event);

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

                        spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                }
            }
        }
    }
}

void BrainCore::applyPlasticity(float plasticityMod) {
    // Apply STDP and Hebbian learning
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    stdp->update(syn.get(), preSpikes, postSpikes, timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    hebbian->update(syn.get(), preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void BrainCore::applyHomeostaticScaling(SimulationStep currentStep) {
    // Every 100 steps, cap each neuron's total incoming excitatory weight
    if (currentStep > 0 && currentStep % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        for (auto& region : regions) {
            for (auto* neuron : region->getAllNeurons()) {
                auto incoming = region->getSynapsesTo(neuron->getId());
                float sum = 0.0f;
                for (Synapse* syn : incoming) {
                    if (syn->getWeight() > 0.0f) sum += syn->getWeight();
                }
                if (sum > kIncomingExcitatoryCap) {
                    float scale = kIncomingExcitatoryCap / sum;
                    for (Synapse* syn : incoming) {
                        if (syn->getWeight() > 0.0f) {
                            syn->setWeight(syn->getWeight() * scale);
                        }
                    }
                }
            }
        }
    }
}

void BrainCore::storeWorkingMemory() {
    // Store active neurons in working memory
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool shouldStore = (state.firingState == FiringState::Refractory &&
                                   state.lastSpikeTime >= 0.0f &&
                                   std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);
                
                if (shouldStore) {
                    // Note: working memory would be accessed via BrainIntegration
                    // For now, we'll store to neuron itself
                    // workingMemory->storeToNeuron(neuron->getId(), 1.0f);
                }
            }
        }
    }
}

void BrainCore::registerSpikeHandlers() {
    if (spikeHandlersRegistered) return;
    spikeHandlersRegistered = true;
    
    // Register spike handlers for event-driven processing
    spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++totalSpikesThisStep;
        ++totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        auto it = neuronById.find(event.destination_neuron.index());
        if (it != neuronById.end()) {
            Neuron* neuron = it->second;
            MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
            if (event.is_excitatory) {
                neuron->receiveExcitatoryInput(synapticCurrent);
            } else {
                neuron->receiveInhibitoryInput(-synapticCurrent);
            }
        }
    });
}

void BrainCore::createNeurons(size_t neuronCount, size_t regionCount, float connectionProbability,
                             std::vector<std::unique_ptr<NeuralRegion>>& outRegions,
                             std::vector<Neuron*>& outSensoryNeurons,
                             std::vector<Neuron*>& outMotorNeurons,
                             std::unordered_map<uint64_t, Neuron*>& outNeuronById) {
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t nextNeuronIndex = 1;
    
    for (size_t i = 0; i < regionCount; ++i) {
        RegionId regionId(i + 1);
        auto region = std::make_unique<NeuralRegion>(regionId, "Region_" + std::to_string(i + 1));
        
        // Add populations
        auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
        size_t internalCount = neuronsPerRegion / 2;
        size_t internalExcitatory = internalCount * 4 / 5;
        auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
        auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
        auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
        
        // Collect sensory and motor neurons for I/O
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        auto* motorPop = region->getPopulation(motorPopId);
        if (sensoryPop) {
            for (auto* neuron : sensoryPop->getNeurons()) {
                outSensoryNeurons.push_back(neuron);
                outNeuronById[neuron->getId().index()] = neuron;
            }
        }
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                outMotorNeurons.push_back(neuron);
                outNeuronById[neuron->getId().index()] = neuron;
            }
        }
        
        NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) +
                     ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons");
        
        nextNeuronIndex += region->getTotalNeuronCount();
        outRegions.push_back(std::move(region));
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = outRegions[i].get();
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

} // namespace nlm
