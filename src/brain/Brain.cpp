#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <sstream>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Integrated memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Integrated prediction system
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // Integrated cognition systems
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // Development system
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Neuromodulation systems
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
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
    float learnedPreference;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    std::unordered_map<uint64_t, Neuron*> neuronById;
    bool spikeHandlersRegistered;
    
    // Integration state
    bool isResting;
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
        , learnedPreference(0.0f)
        , spikeHandlersRegistered(false)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)
        , consolidationInterval(1000) {
        
        // Initialize random generator with seed from config
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
        
        // Get integration intervals from config
        replayInterval = static_cast<size_t>(config->getOr<int64_t>("replay_interval", 100));
        consolidationInterval = static_cast<size_t>(config->getOr<int64_t>("consolidation_interval", 1000));
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool Brain::initialize() {
    // Re-initialising must rebuild, not append
    pImpl->regions.clear();
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    pImpl->neuronById.clear();
    pImpl->nextRegionId = RegionId(1);
    pImpl->totalSpikesTotal = 0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    
    // Get configuration values
    size_t neuronCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("region_count", 1));
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t nextNeuronIndex = 1;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            region->setNextNeuronIndex(nextNeuronIndex);
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
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
            nextNeuronIndex += region->getTotalNeuronCount();
        }
    }
    
    // Build the neuron lookup
    for (auto& region : pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            pImpl->neuronById[neuron->getId().index()] = neuron;
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // Initialize memory systems
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    pImpl->associativeMemory->initialize(this);
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    pImpl->conceptFormation->initialize(this);
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    
    // Register spike handlers once
    if (!pImpl->spikeHandlersRegistered) {
        pImpl->spikeHandlersRegistered = true;
        pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            ++pImpl->totalSpikesThisStep;
            ++pImpl->totalSpikesTotal;
        });
        
        pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            auto it = pImpl->neuronById.find(event.destination_neuron.index());
            if (it != pImpl->neuronById.end()) {
                Neuron* neuron = it->second;
                MembranePotential synapticCurrent = event.weight * 10.0f;
                if (event.is_excitatory) {
                    neuron->receiveExcitatoryInput(synapticCurrent);
                } else {
                    neuron->receiveInhibitoryInput(-synapticCurrent);
                }
            }
        });
    }
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Execute all 15 integration steps using the step manager
    executeIntegrationSteps(currentStep, currentTime);
}

void Brain::executeIntegrationSteps(SimulationStep step, Timestamp time) {
    // Step 1: Process pending delayed spikes
    pImpl->spikeSystem->processDelayedSpikes(step, time);
    
    // Step 2: Update all neurons
    updateAllNeurons(time);
    
    // Step 3: Detect and queue spikes
    detectAndQueueSpikes(time);
    
    // Step 4: Update working memory
    updateWorkingMemory();
    
    // Step 5: Apply neuromodulation effects
    applyNeuromodulationEffects(time);
    
    // Step 6: Apply plasticity rules
    applyPlasticityRules(time);
    
    // Step 6b: Homeostatic synaptic scaling
    homeostaticSynapticScaling(step);
    
    // Step 7: Update episodic memory
    updateEpisodicMemory(step);
    
    // Step 8: Update prediction system
    updatePredictionSystem();
    
    // Step 9: Update attention system
    updateAttentionSystem();
    
    // Step 10: Update concept formation
    updateConceptFormation();
    
    // Step 11: Apply structural plasticity
    applyStructuralPlasticity(step);
    
    // Step 12: Replay memories
    replayMemories(step);
    
    // Step 13: Apply development effects
    applyDevelopmentEffects(step);
    
    // Step 14: Periodic memory consolidation
    consolidateMemories(step);
    
    // Step 15: Checkpoint management
    updateCheckpoints(step, time);
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(step);
}

void Brain::updateAllNeurons(Timestamp time) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(time, pImpl->timestep);
            }
        }
    }
}

void Brain::detectAndQueueSpikes(Timestamp time) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(time) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    SpikeEvent event(neuron->getId(), time, pImpl->currentStep);
                    pImpl->spikeSystem->queueSpike(event);
                    
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(time);
                    }
                    
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = pImpl->currentStep + delay;
                        Timestamp deliveryTime = time + delay * pImpl->timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            time,
                            deliveryTime,
                            pImpl->currentStep,
                            deliveryStep
                        );

                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                        syn->recordPreSpike(time);
                    }
                    
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 1.0f);
                    }
                }
            }
        }
    }
}

void Brain::updateWorkingMemory() {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void Brain::applyNeuromodulationEffects(Timestamp time) {
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f,
                               pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f,
                               pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void Brain::applyPlasticityRules(Timestamp time) {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->stdp->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
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
                    pImpl->hebbian->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(time);
        }
    }
}

void Brain::homeostaticSynapticScaling(SimulationStep step) {
    if (step > 0 && step % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        for (auto& region : pImpl->regions) {
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

void Brain::updateEpisodicMemory(SimulationStep step) {
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void Brain::updatePredictionSystem() {
    if (pImpl->predictionSystem) {
    }
}

void Brain::updateAttentionSystem() {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void Brain::updateConceptFormation() {
    if (pImpl->conceptFormation) {
    }
}

void Brain::applyStructuralPlasticity(SimulationStep step) {
    if (step % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void Brain::replayMemories(SimulationStep step) {
    if (step % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void Brain::applyDevelopmentEffects(SimulationStep step) {
    if (step % 1000 == 0) {
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        auto* sp = pImpl->structuralPlasticity.get();
        if (sp) {
            DevelopmentalStage stage = pImpl->developmentalStage;
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void Brain::consolidateMemories(SimulationStep step) {
    if (step % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void Brain::updateCheckpoints(SimulationStep step, Timestamp time) {
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(step, time);
    }
}

// The rest of the original Brain.cpp methods would continue here...

} // namespace nlm
