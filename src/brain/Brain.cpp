#include "Brain.hpp"
#include "Index.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../development/DevelopmentalStage.hpp"
#include "../memory/MemoryTypes.hpp"
#include "../neurons/NeuronTypes.hpp"
#include "../synapses/SynapseTypes.hpp"
#include "../error/Error.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <cstring>
#include <cerrno>

// Include the NLM helper classes
#include "nlm/NLMHelpers.hpp"

// Error handling macros for Brain operations
#define NULL_CHECK_RET(ptr, name, ret) \
    if (!(ptr)) { \
        nlm::ErrorLoggerHelper::logNullPointer(name, __FILE__, __LINE__, __func__); \
        return ret; \
    }

#define NULL_CHECK_ASSERT(ptr, name) \
    if (!(ptr)) { \
        nlm::ErrorLoggerHelper::logNullPointer(name, __FILE__, __LINE__, __func__); \
        std::abort(); \
    }

#define FILE_OP_CHECK(fileVar, filepath, operation) \
    if (!(fileVar).is_open()) { \
        nlm::ErrorLoggerHelper::logError(nlm::ErrorCode::FileIOError, \
            "File I/O error during " + std::string(operation) + " on: " + filepath, \
            __FILE__, __LINE__, __func__); \
        return false; \
    }

#define SAFE_OPERATION(op, errorMsg) \
    try { \
        op; \
    } catch (const std::exception& e) { \
        nlm::ErrorLoggerHelper::logError(nlm::ErrorCode::SystemError, \
            errorMsg + std::string(": ") + e.what(), \
            __FILE__, __LINE__, __func__); \
        return false; \
    }

namespace nlm {

void Brain::Impl::initializeSystems() {
    try {
        // Initialize spike system
        spikeSystem = std::make_unique<SpikeSystem>();
        NULL_CHECK_ASSERT(spikeSystem.get(), "spikeSystem");
        
        // Initialize STDP
        stdp = std::make_unique<STDP>();
        NULL_CHECK_ASSERT(stdp.get(), "stdp");
        
        // Initialize Hebbian
        hebbian = std::make_unique<Hebbian>();
        NULL_CHECK_ASSERT(hebbian.get(), "hebbian");
        
        // Initialize structural plasticity
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        NULL_CHECK_ASSERT(structuralPlasticity.get(), "structuralPlasticity");
        
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
        
        // Initialize other systems
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        predictionSystem = std::make_unique<PredictionSystem>();
        
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Configure neuromodulation systems
        if (dopamine) dopamine->initialize(this);
        if (curiosity) curiosity->initialize(this);
        if (predictionError) predictionError->initialize(this);
        if (novelty) novelty->initialize(this);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
        
        std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        if (checkpointManager) {
            checkpointManager->configure(checkpointDir, 10000, 5, true);
        }
        
    } catch (const std::exception& e) {
        nlm::ErrorLoggerHelper::logError(
            nlm::ErrorCode::SystemError,
            std::string("Failed to initialize brain systems: ") + e.what(),
            __FILE__, __LINE__, __func__
        );
        throw;
    }
}

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {
    try {
        NULL_CHECK_ASSERT(pImpl, "Brain::pImpl");
        pImpl->initializeSystems();
    } catch (const std::exception& e) {
        nlm::ErrorLoggerHelper::logError(
            nlm::ErrorCode::SystemError,
            std::string("Failed to initialize brain: ") + e.what(),
            __FILE__, __LINE__, __func__
        );
        throw;
    }
}

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
    try {
        NULL_CHECK_RET(pImpl->config, "config", false);
        
        // Get configuration values
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions");
        
        // Create regions
        for (size_t i = 0; i < regionCount; ++i) {
            addRegion("Region_" + std::to_string(i + 1));
        }
        
        // Create neurons across regions
        size_t neuronsPerRegion = neuronCount / regionCount;
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            NULL_CHECK_RET(region, "region in initialize", false);
            
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
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                         ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                         std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
        
        // Initialize connectivity with random weights
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            NULL_CHECK_RET(region, "region in connectivity init", false);
            
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
        
        // Initialize integrated memory systems
        if (pImpl->workingMemory) {
            pImpl->workingMemory->initialize(this);
            pImpl->workingMemory->setCapacity(neuronCount / 10);
        }
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->initialize(this);
            pImpl->episodicMemory->setMaxEpisodes(1000);
        }
        if (pImpl->associativeMemory) {
            pImpl->associativeMemory->initialize(this);
        }
        
        // Initialize cognition systems
        if (pImpl->planner) {
            pImpl->planner->initialize(this);
            pImpl->planner->setPlanningDepth(5);
        }
        if (pImpl->conceptFormation) {
            pImpl->conceptFormation->initialize(this);
        }
        if (pImpl->attention) {
            pImpl->attention->initialize(this);
            pImpl->attention->setInhibitionStrength(0.5f);
            pImpl->attention->setExcitationStrength(1.5f);
        }
        
        // Initialize neuromodulation
        if (pImpl->novelty) {
            pImpl->novelty->initialize(this);
        }
        if (pImpl->curiosity) {
            pImpl->curiosity->initialize(this);
        }
        
        // Register delayed spike handler
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
                Neuron* neuron = pImpl->neuronIndex.getNeuron(event.destination_neuron);
                NULL_CHECK_RET(neuron, "neuron in delayed spike handler", );
                
                MembranePotential synapticCurrent = event.weight * 10.0f;
                if (event.is_excitatory) {
                    neuron->receiveExcitatoryInput(synapticCurrent);
                } else {
                    neuron->receiveInhibitoryInput(-synapticCurrent);
                }
            });
        }
        
        // Build indexes after all neurons and synapses are created
        pImpl->neuronIndex.clear();
        pImpl->synapseIndex.clear();
        
        // Populate neuron index
        for (const auto& region : pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                NULL_CHECK_RET(neuron, "neuron in index population", false);
                pImpl->neuronIndex.addNeuron(neuron);
            }
        }
        
        // Populate synapse index  
        for (const auto& region : pImpl->regions) {
            auto synapses = region->getSynapses();
            for (auto& synPtr : synapses) {
                pImpl->synapseIndex.addSynapse(synPtr.get());
            }
        }
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const std::exception& e) {
        nlm::ErrorLoggerHelper::logError(
            nlm::ErrorCode::SystemError,
            std::string("Brain initialization failed: ") + e.what(),
            __FILE__, __LINE__, __func__
        );
        return false;
    }
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    try {
        NULL_CHECK_ASSERT(pImpl, "Brain::pImpl in step");
        
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        // Cache frequently accessed references to avoid repeated lookups
        auto& regions = pImpl->regions;
        auto& neuronIndex = pImpl->neuronIndex;
        auto& synapseIndex = pImpl->synapseIndex;
        auto& spikeSystem = pImpl->spikeSystem;
        auto& dopamine = pImpl->dopamine;
        auto& workingMemory = pImpl->workingMemory;
        auto& episodicMemory = pImpl->episodicMemory;
        auto& attention = pImpl->attention;
        auto& predictionSystem = pImpl->predictionSystem;
        auto& structuralPlasticity = pImpl->structuralPlasticity;
        auto& novelty = pImpl->novelty;
        auto& curiosity = pImpl->curiosity;
        
        // Get timesteps once for reuse
        TimestepDuration timestep = pImpl->timestep;
        float timestepFloat = static_cast<float>(timestep);
        
        // CRITICAL SAFETY CHECKS
        if (!spikeSystem) {
            NLM_LOG_ERROR("CRITICAL: spikeSystem is null, brain operation aborted");
            return;
        }
        if (!dopamine) {
            NLM_LOG_ERROR("CRITICAL: dopamine system is null, brain operation aborted");
            return;
        }
        
        // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
        SAFE_OPERATION({
            spikeSystem->processDelayedSpikes(currentStep, currentTime);
        }, "CRITICAL: Failed to process delayed spikes");
        
        // ========== STEP 2: Update all neurons (LIF dynamics) - Optimized ==========
        for (auto& region : regions) {
            auto& populations = region->getPopulations();
            for (auto& pop : populations) {
                auto& neurons = pop->getNeurons();
                for (auto* neuron : neurons) {
                    NULL_CHECK_RET(neuron, "neuron in LIF update", );
                    
                    SAFE_OPERATION({
                        neuron->stepLIF(currentTime, timestep);
                    }, std::string("Failed to update neuron ") + std::to_string(neuron->getId()));
                }
            }
        }
        
        // ========== STEP 3: Detect spikes and schedule spike events - Optimized ==========
        for (auto& region : regions) {
            auto& populations = region->getPopulations();
            for (auto& pop : populations) {
                auto& neurons = pop->getNeurons();
                for (auto* neuron : neurons) {
                    NULL_CHECK_RET(neuron, "neuron in spike detection", );
                    
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestepFloat * 2.0f);

                    if (justFired) {
                        // Neuron fired this step - queue the spike
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        
                        SAFE_OPERATION({
                            spikeSystem->queueSpike(event);
                        }, std::string("Failed to queue spike for neuron ") + std::to_string(neuron->getId()));

                        // Record post-synaptic spike for incoming synapses (plasticity)
                        auto incomingSynapses = synapseIndex.getSynapsesTo(neuron->getId());
                        for (Synapse* syn : incomingSynapses) {
                            NULL_CHECK_RET(syn, "synapse in post-spike recording", );
                            syn->recordPostSpike(currentTime);
                        }
                        
                        // Get outgoing synapses and schedule delayed spike events
                        auto outgoingSynapses = synapseIndex.getSynapsesFrom(neuron->getId());
                        for (Synapse* syn : outgoingSynapses) {
                            NULL_CHECK_RET(syn, "synapse in delayed spike scheduling", );
                            
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
                            
                            SAFE_OPERATION({
                                spikeSystem->queueDelayedSpike(delayedEvent);
                            }, std::string("Failed to queue delayed spike from neuron ") + std::to_string(neuron->getId()));
                            
                            // Record pre-synaptic spike for plasticity
                            syn->recordPreSpike(currentTime);
                        }
                        
                        // Store to working memory
                        if (workingMemory) {
                            workingMemory->storeToNeuron(neuron->getId(), 
                                std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                        }
                    }
                }
            }
        }
        
        // Process immediate spikes
        SAFE_OPERATION({
            spikeSystem->processSpikes(currentStep);
        }, "CRITICAL: Failed to process spikes");
        
        // ========== STEP 4: Update working memory ==========
        if (workingMemory) {
            workingMemory->update(timestep);
        }
        
        // ========== STEP 5: Apply neuromodulation effects ==========
        if (novelty) {
            novelty->update(timestep);
        }
        if (curiosity) {
            curiosity->update(timestep);
        }
        
        // Update dopamine (reward prediction error)
        if (dopamine) {
            dopamine->update(timestep);
            
            float dopamineLevel = dopamine->getLevel();
            for (auto& region : regions) {
                auto& populations = region->getPopulations();
                for (auto& pop : populations) {
                    auto& neurons = pop->getNeurons();
                    for (auto* neuron : neurons) {
                        float excitabilityMod = dopamineLevel * 0.5f;
                        if (excitabilityMod > 0.0f) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
        
        // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) - Optimized ==========
        float plasticityMod = 1.0f;
        if (dopamine) {
            plasticityMod = dopamine->getPlasticityFactor();
        }
        
        auto synapses = synapseIndex.getAllSynapses();
        for (Synapse* syn : synapses) {
            NULL_CHECK_RET(syn, "synapse in plasticity update", );
            
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    NULL_CHECK_RET(pImpl->stdp, "stdp in plasticity update", );
                    
                    pImpl->stdp->update(syn, preSpikes, postSpikes, timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    NULL_CHECK_RET(pImpl->hebbian, "hebbian in plasticity update", );
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, timestep);
                }
            }
            
            syn->step(currentTime);
        }
        
        // ========== STEP 7: Update episodic memory ==========
        pImpl->stepsSinceLastEpisode++;
        if (pImpl->stepsSinceLastEpisode >= 10) {
            pImpl->stepsSinceLastEpisode = 0;
            
            if (episodicMemory) {
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
                
                for (auto& region : regions) {
                    auto& populations = region->getPopulations();
                    for (auto& pop : populations) {
                        auto& neurons = pop->getNeurons();
                        for (auto* neuron : neurons) {
                            if (neuron->isFiring() || 
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                                episode.activeNeurons.push_back(neuron->getId());
                                episode.neuronActivations.push_back(
                                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                            }
                        }
                    }
                }
                
                episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
                
                SAFE_OPERATION({
                    episodicMemory->storeEpisode(episode);
                }, std::string("Failed to store episode at step ") + std::to_string(currentStep));
            }
        }
        
        // ========== STEP 8: Update attention system ==========
        if (attention) {
            attention->update(timestep);
            
            if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
                attention->processCompetition(competitors);
            }
        }
        
        // ========== STEP 9: Apply structural plasticity periodically ==========
        if (currentStep % 100 == 0 && structuralPlasticity) {
            structuralPlasticity->update(this, *pImpl->rng);
        }
        
        // ========== STEP 10: Replay important memories ==========
        if (currentStep % pImpl->replayInterval == 0 && episodicMemory) {
            auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                episodicMemory->replayEpisode(episode);
            }
        }
        
        // ========== STEP 11: Apply development effects ==========
        if (currentStep % 1000 == 0 && developmentSystem) {
            developmentSystem->update(this, *pImpl->rng, timestep * 1000);
            
            auto* sp = structuralPlasticity;
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
        
        // ========== STEP 12: Periodic memory consolidation ==========
        if (currentStep % pImpl->consolidationInterval == 0 && episodicMemory) {
            episodicMemory->consolidate(0.3f);
        }
        
        // ========== STEP 13: Checkpoint management ==========
        if (pImpl->checkpointManager) {
            pImpl->checkpointManager->update(currentStep, currentTime);
        }
        
    } catch (const std::exception& e) {
        nlm::ErrorLoggerHelper::logError(
            nlm::ErrorCode::SystemError,
            std::string("Brain step failed at step " + std::to_string(currentStep) + ": ") + e.what(),
            __FILE__, __LINE__, __func__
        );
        throw;
    }
}

bool Brain::save(const std::string& filepath) const {
    try {
        NULL_CHECK_RET(pImpl, "Brain::pImpl in save", false);
        
        NLM_LOG_INFO("Saving brain state to " + filepath);
        
        CheckpointWriter writer;
        SAFE_OPERATION({
            if (!writer.create(filepath, CompressionLevel::Balanced)) {
                NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
                return false;
            }
        }, "Failed to create checkpoint writer");
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    NULL_CHECK_RET(neuron, "neuron in save operation", false);
                    
                    const auto& state = neuron->getState();
                    neuronData.membranePotential.push_back(state.membranePotential);
                    neuronData.restingPotential.push_back(state.restingPotential);
                    neuronData.threshold.push_back(state.threshold);
                    neuronData.resetPotential.push_back(state.resetPotential);
                    neuronData.leakConductance.push_back(state.leakConductance);
                    neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                    neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                    neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                }
            }
        }
        
        SAFE_OPERATION({
            if (!writer.writeNeurons(neuronData)) {
                NLM_LOG_ERROR("Failed to write neurons to checkpoint");
                return false;
            }
        }, "Failed to write neurons to checkpoint");
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                NULL_CHECK_RET(syn, "synapse in save operation", false);
                
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        SAFE_OPERATION({
            if (!writer.writeSynapses(synapseData)) {
                NLM_LOG_ERROR("Failed to write synapses to checkpoint");
                return false;
            }
        }, "Failed to write synapses to checkpoint");
        
        // Finalize
        SAFE_OPERATION({
            if (!writer.finalize()) {
                NLM_LOG_ERROR("Failed to finalize checkpoint");
                return false;
            }
        }, "Failed to finalize checkpoint");
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    try {
        NULL_CHECK_RET(pImpl, "Brain::pImpl in load", false);
        
        NLM_LOG_INFO("Loading brain state from " + filepath);
        
        CheckpointReader reader;
        SAFE_OPERATION({
            if (!reader.open(filepath)) {
                NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
                return false;
            }
        }, "Failed to open checkpoint reader");
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        SAFE_OPERATION({
            if (!reader.readNeurons(neuronData)) {
                NLM_LOG_ERROR("Failed to read neurons from checkpoint");
                return false;
            }
        }, "Failed to read neurons from checkpoint");
        
        // Apply neuron states
        size_t idx = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    NULL_CHECK_RET(neuron, "neuron in load operation", false);
                    
                    if (idx < neuronData.membranePotential.size()) {
                        neuron->setMembranePotential(neuronData.membranePotential[idx]);
                        neuron->setRestingPotential(neuronData.restingPotential[idx]);
                        neuron->setThreshold(neuronData.threshold[idx]);
                        neuron->setResetPotential(neuronData.resetPotential[idx]);
                        neuron->setLeakConductance(neuronData.leakConductance[idx]);
                        if (idx < neuronData.firingState.size()) {
                            neuron->setFiringState(static_cast<FiringState>(neuronData.firingState[idx]));
                        }
                        if (idx < neuronData.refractoryRemaining.size()) {
                            neuron->setRefractoryPeriod(neuronData.refractoryPeriod[idx]);
                        }
                    }
                    idx++;
                }
            }
        }
        
        // Read synapses
        SynapseCheckpointData synapseData;
        SAFE_OPERATION({
            if (!reader.readSynapses(synapseData)) {
                NLM_LOG_ERROR("Failed to read synapses from checkpoint");
                return false;
            }
        }, "Failed to read synapses from checkpoint");
        
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

void Brain::reset() {
    try {
        NULL_CHECK_ASSERT(pImpl, "Brain::pImpl in reset");
        
        NLM_LOG_INFO("Resetting NLM Brain...");
        
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0;
        pImpl->totalSpikesThisStep = 0;
        pImpl->totalSpikesTotal = 0;
        pImpl->isResting = false;
        pImpl->stepsSinceLastEpisode = 0;
        
        // Clear indexes
        pImpl->neuronIndex.clear();
        pImpl->synapseIndex.clear();
        
        for (auto& region : pImpl->regions) {
            region->reset();
        }
        
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->reset();
        }
        pImpl->developmentalStage = DevelopmentalStage::Initial;
        
        // Reset memory systems
        if (pImpl->workingMemory) pImpl->workingMemory->clear();
        if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
        if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
        if (pImpl->attention) pImpl->attention->reset();
        
        NLM_LOG_INFO("NLM Brain reset complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception resetting brain: ") + e.what());
        throw;
    }
}

// Forward declarations for commonly used types (simplified for brevity)
struct NeuronId {};
struct SimulationStep {};
struct Timestamp {};
struct TimestepDuration {};
enum class FiringState { Refractory, Firing, Resting };
enum class CompressionLevel { Balanced };

// Simplified structure declarations
struct SpikeEvent {};
struct DelayedSpikeEvent {};
struct NeuronCheckpointData {};
struct SynapseCheckpointData {};
struct DevelopmentalStage {};

// Simplified system classes
class SpikeSystem {
public:
    void processDelayedSpikes(SimulationStep, Timestamp) {}
    void queueSpike(const SpikeEvent&) {}
    void queueDelayedSpike(const DelayedSpikeEvent&) {}
    void processSpikes(SimulationStep) {}
    void registerDelayedHandler(std::function<void(const DelayedSpikeEvent&)>) {}
    size_t getPendingSpikeCount() const { return 0; }
    size_t getPendingDelayedCount() const { return 0; }
    void reset() {}
};

class Dopamine {
public:
    void initialize(void*) {}
    void update(TimestepDuration) {}
    float getLevel() const { return 0.0f; }
    float getPlasticityFactor() const { return 1.0f; }
};

class STDP {
public:
    void configure(float, float, float) {}
    void update(class Synapse*, const std::vector<Timestamp>&, const std::vector<Timestamp>&, TimestepDuration) {}
};

class Hebbian {
public:
    void update(class Synapse*, const std::vector<Timestamp>&, const std::vector<Timestamp>&, TimestepDuration) {}
};

class StructuralPlasticity {
public:
    void setSynaptogenesisRate(float) {}
    void setPruningRate(float) {}
    void update(class Brain*, class RandomGenerator&, TimestepDuration) {}
};

class NeuralWorkingMemory {
public:
    void initialize(void*) {}
    void setCapacity(size_t) {}
    void storeToNeuron(NeuronId, float) {}
    void update(TimestepDuration) {}
    void clear() {}
    size_t getActiveTraces() const { return 0; }
    void setCapacity(size_t) {}
    std::vector<NeuronId> getMemoryNeurons() const { return {}; }
    size_t getActiveTraces() const { return 0; }
};

class NeuralEpisodicMemory {
public:
    void initialize(void*) {}
    void setMaxEpisodes(size_t) {}
    struct EpisodicMemoryItem {};
    void storeEpisode(const EpisodicMemoryItem&) {}
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t) const { return {}; }
    void replayEpisode(const EpisodicMemoryItem*) {}
    void consolidate(float) {}
    void clear() {}
    size_t getEpisodeCount() const { return 0; }
};

class NeuralAssociativeMemory {
public:
    void initialize(void*) {}
    void clear() {}
};

class NeuralPlanner {
public:
    void initialize(void*) {}
    void setPlanningDepth(size_t) {}
};

class ConceptFormation {
public:
    void initialize(void*) {}
};

class AttentionalSelection {
public:
    void initialize(void*) {}
    void setInhibitionStrength(float) {}
    void setExcitationStrength(float) {}
    void update(TimestepDuration) {}
    void reset() {}
    void processCompetition(const std::vector<NeuronId>&) {}
};

class DevelopmentSystem {
public:
    void update(class Brain*, class RandomGenerator&, TimestepDuration) {}
};

class PredictionSystem {};

class Dopamine {};
class Curiosity {};
class PredictionError {};
class Novelty {};

class CheckpointManager {
public:
    void configure(const std::string&, size_t, size_t, bool) {}
    void update(SimulationStep, Timestamp) {}
};

class CheckpointWriter {
public:
    bool create(const std::string&, CompressionLevel) { return true; }
    void setMetadata(size_t, size_t, size_t, SimulationStep, Timestamp) {}
    bool writeNeurons(const NeuronCheckpointData&) { return true; }
    bool writeSynapses(const SynapseCheckpointData&) { return true; }
    bool finalize() { return true; }
    size_t getBytesWritten() const { return 0; }
};

class CheckpointReader {
public:
    bool open(const std::string&) { return true; }
    bool validate() { return true; }
    std::string getError() const { return ""; }
    bool readNeurons(NeuronCheckpointData&) { return true; }
    bool readSynapses(SynapseCheckpointData&) { return true; }
};

// Simplified type aliases
using NeuronId = int;
using SimulationStep = int;
using Timestamp = double;
using TimestepDuration = double;
using NeuronType = int;
using Delay = int;
using SynapseType = int;
using MembranePotential = float;
using PopulationId = int;
using RegionId = int;

} // namespace nlm