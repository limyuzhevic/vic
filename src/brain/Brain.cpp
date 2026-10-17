#include "Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <cassert>

namespace nlm {

// Brain implementation with improved error handling and memory management

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {
    if (!pImpl->config) {
        NLM_LOG_ERROR("Brain: Config pointer is null");
        throw std::invalid_argument("Config pointer cannot be null");
    }
    NLM_LOG_INFO("Brain object created successfully");
}

Brain::~Brain() {
    delete pImpl;
}

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
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    try {
        // Validate configuration before proceeding
        if (!validateConfiguration()) {
            NLM_LOG_ERROR("Brain initialization failed: invalid configuration");
            return false;
        }
        
        // Get configuration values with validation
        size_t neuronCount = getValidatedNeuronCount();
        size_t regionCount = getValidatedRegionCount();
        float connectionProbability = getValidatedConnectionProbability();
        
        if (neuronCount == 0) {
            NLM_LOG_ERROR("Brain initialization failed: must have at least one neuron");
            return false;
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions, " +
                     std::to_string(connectionProbability * 100) + "% connectivity");
        
        // Create regions with validation
        if (!createRegions(regionCount)) {
            NLM_LOG_ERROR("Brain initialization failed: could not create regions");
            return false;
        }
        
        // Create neurons across regions with validation
        if (!createNeurons(neuronCount, regionCount, connectionProbability)) {
            NLM_LOG_ERROR("Brain initialization failed: could not create neurons or connectivity");
            return false;
        }
        
        // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
        
        // Initialize memory systems with error checking
        if (pImpl->workingMemory) {
            if (!pImpl->workingMemory->initialize(this)) {
                NLM_LOG_WARN("Working memory initialization failed");
            }
            pImpl->workingMemory->setCapacity(neuronCount / 10);
        }
        
        if (pImpl->episodicMemory) {
            if (!pImpl->episodicMemory->initialize(this)) {
                NLM_LOG_WARN("Episodic memory initialization failed");
            }
            pImpl->episodicMemory->setMaxEpisodes(1000);
        }
        
        if (pImpl->associativeMemory) {
            if (!pImpl->associativeMemory->initialize(this)) {
                NLM_LOG_WARN("Associative memory initialization failed");
            }
        }
        
        // Initialize cognition systems with error checking
        if (pImpl->planner) {
            if (!pImpl->planner->initialize(this)) {
                NLM_LOG_ERROR("Planner initialization failed");
                return false;
            }
            pImpl->planner->setPlanningDepth(5);
        }
        
        if (pImpl->conceptFormation) {
            if (!pImpl->conceptFormation->initialize(this)) {
                NLM_LOG_ERROR("Concept formation initialization failed");
                return false;
            }
        }
        
        if (pImpl->attention) {
            if (!pImpl->attention->initialize(this)) {
                NLM_LOG_ERROR("Attention system initialization failed");
                return false;
            }
            pImpl->attention->setInhibitionStrength(0.5f);
            pImpl->attention->setExcitationStrength(1.5f);
        }
        
        // Initialize neuromodulation
        if (pImpl->novelty) {
            if (!pImpl->novelty->initialize(this)) {
                NLM_LOG_ERROR("Novelty detection system initialization failed");
                return false;
            }
        }
        
        if (pImpl->curiosity) {
            if (!pImpl->curiosity->initialize(this)) {
                NLM_LOG_ERROR("Curiosity system initialization failed");
                return false;
            }
        }
        
        // Register spike handlers for event-driven processing
        pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes with bounds checking
            if (pImpl->totalSpikesThisStep < std::numeric_limits<size_t>::max()) {
                ++pImpl->totalSpikesThisStep;
            }
            if (pImpl->totalSpikesTotal < std::numeric_limits<size_t>::max()) {
                ++pImpl->totalSpikesTotal;
            }
        });
        
        // Register delayed spike handler
        pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            // Validate event before processing
            if (event.destination_neuron >= std::numeric_limits<NeuronId>::max() ||
                event.weight < -1000.0f || event.weight > 1000.0f) {
                NLM_LOG_ERROR("Invalid delayed spike event, skipping");
                return;
            }
            
            // Find destination neuron and deliver synaptic input
            for (auto& region : pImpl->regions) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (neuron->getId() == event.destination_neuron) {
                        // Validate neuron state before accessing
                        if (!neuron || neuron->getId() != event.destination_neuron) {
                            continue;
                        }
                        
                        // Apply synaptic weight as current with bounds checking
                        MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                        if (synapticCurrent > 1000.0f || synapticCurrent < -1000.0f) {
                            NLM_LOG_ERROR("Synaptic current out of bounds, clamping");
                            synapticCurrent = std::max(-1000.0f, std::min(1000.0f, synapticCurrent));
                        }
                        
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
        
        // Configure checkpoint manager with error checking
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        if (!pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true)) {
            NLM_LOG_WARN("Checkpoint manager configuration failed");
        }
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        logStatus();
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain initialization: " + std::string(e.what()));
        return false;
    } catch (...) {
        NLM_LOG_ERROR("Unknown exception during brain initialization");
        return false;
    }
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    try {
        // Validate input parameters
        if (std::isnan(currentTime) || currentTime < 0.0) {
            NLM_LOG_ERROR("Invalid simulation time, skipping step");
            return;
        }
        
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        }
        
        // ========== STEP 2: Update all neurons (LIF dynamics) ==========
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron) {
                        neuron->stepLIF(currentTime, pImpl->timestep);
                    }
                }
            }
        }
        
        // ========== STEP 3: Detect spikes and schedule spike events ==========
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) continue;
                    
                    // Check if neuron just fired this step
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);
                    
                    if (justFired) {
                        // Neuron fired this step - queue the spike
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        if (pImpl->spikeSystem) {
                            pImpl->spikeSystem->queueSpike(event);
                        }
                        
                        // Record post-synaptic spike for incoming synapses (plasticity)
                        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                        for (Synapse* syn : incomingSynapses) {
                            if (syn) {
                                syn->recordPostSpike(currentTime);
                            }
                        }
                        
                        // Get outgoing synapses and schedule delayed spike events
                        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                        for (Synapse* syn : outgoingSynapses) {
                            if (!syn) continue;
                            
                            // Create delayed spike event
                            Delay delay = syn->getDelay();
                            SimulationStep deliveryStep = currentStep + delay;
                            Timestamp deliveryTime = currentTime + delay * pImpl->timestep;
                            
                            // Validate delivery times
                            if (deliveryStep < currentStep || deliveryTime < currentTime) {
                                NLM_LOG_ERROR("Invalid spike delivery timing, skipping");
                                continue;
                            }
                            
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
                            
                            if (pImpl->spikeSystem) {
                                pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                            }
                            
                            // Record pre-synaptic spike for plasticity
                            syn->recordPreSpike(currentTime);
                        }
                        
                        // Store to working memory
                        if (pImpl->workingMemory) {
                            float neuronValue = std::abs(state.membranePotential - state.restingPotential) / 10.0f;
                            pImpl->workingMemory->storeToNeuron(neuron->getId(), neuronValue);
                        }
                    }
                }
            }
        }
        
        // Process immediate spikes
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->processSpikes(currentStep);
        }
        
        // ========== STEP 4: Update working memory ==========
        if (pImpl->workingMemory) {
            pImpl->workingMemory->update(pImpl->timestep);
        }
        
        // ========== STEP 5: Apply neuromodulation effects ==========
        // Update novelty detection
        if (pImpl->novelty) {
            pImpl->novelty->update(pImpl->timestep);
        }
        
        // Update curiosity
        if (pImpl->curiosity) {
            pImpl->curiosity->update(pImpl->timestep);
        }
        
        // Update dopamine (reward prediction error) with bounds checking
        if (pImpl->dopamine) {
            pImpl->dopamine->update(pImpl->timestep);
            float dopamineLevel = pImpl->dopamine->getLevel();
            
            // Apply reasonable bounds to dopamine level
            dopamineLevel = std::max(0.0f, std::min(10.0f, dopamineLevel));
            
            // Apply dopamine effects on neural excitability
            if (dopamineLevel > 0.0f) {
                float excitabilityMod = dopamineLevel * 0.5f;
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (neuron) {
                                // Dopamine modulates excitability by injecting additional current
                                neuron->injectCurrent(excitabilityMod);
                            }
                        }
                    }
                }
            }
        }
        
        // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
        // Calculate neuromodulation factor for plasticity
        float plasticityMod = 1.0f;
        if (pImpl->dopamine) {
            plasticityMod = pImpl->dopamine->getPlasticityFactor();
        }
        
        // Apply reasonable bounds to plasticity modulation
        plasticityMod = std::max(0.1f, std::min(10.0f, plasticityMod));
        
        for (auto& region : pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                if (!syn) continue;
                
                // Apply STDP with neuromodulation
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Modify weight change based on dopamine
                        if (pImpl->stdp) {
                            pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                            float weight = syn->getWeight();
                            
                            // Apply weight bounds to prevent runaway excitation/inhibition
                            weight = std::max(-100.0f, std::min(100.0f, weight));
                            
                            // Apply neuromodulation factor
                            weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                            
                            // Final weight bounds
                            weight = std::max(-100.0f, std::min(100.0f, weight));
                            syn->setWeight(weight);
                        }
                    }
                }
                
                // Apply Hebbian learning
                if (syn->getPlasticityFlags().hebbian) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Simplified Hebbian rule
                        float currentWeight = syn->getWeight();
                        float hebbianUpdate = 0.0001f;  // Learning rate
                        
                        // Ensure weight stays within reasonable bounds
                        float newWeight = currentWeight + hebbianUpdate;
                        newWeight = std::max(-100.0f, std::min(100.0f, newWeight));
                        
                        syn->setWeight(newWeight);
                    }
                }
            }
        }
        
        // ========== STEP 7-14: Other integrated systems ==========
        // Update episodic memory, prediction system, attention, concept formation,
        // structural plasticity, replay, development, and collect statistics
        
        // Update episodic memory
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->update(currentTime);
        }
        
        // Update prediction system
        if (pImpl->predictionSystem) {
            pImpl->predictionSystem->update(pImpl->timestep);
        }
        
        // Update attention system
        if (pImpl->attention) {
            pImpl->attention->update(pImpl->timestep);
        }
        
        // Update concept formation
        if (pImpl->conceptFormation) {
            pImpl->conceptFormation->update(pImpl->timestep);
        }
        
        // Apply structural plasticity
        if (pImpl->structuralPlasticity) {
            pImpl->structuralPlasticity->update(this, *pImpl->rng);
        }
        
        // Update development
        if (pImpl->developmentSystem) {
            pImpl->developmentSystem->update(pImpl->timestep);
        }
        
        // Collect statistics
        collectStatistics();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during simulation step: ") + e.what());
    } catch (...) {
        NLM_LOG_ERROR("Unknown exception during simulation step");
    }
}

bool Brain::validateConfiguration() const {
    try {
        // Check if essential components are initialized
        if (!pImpl->config) {
            NLM_LOG_ERROR("Brain: Config is null");
            return false;
        }
        
        if (!pImpl->rng) {
            NLM_LOG_ERROR("Brain: Random generator is not initialized");
            return false;
        }
        
        if (!pImpl->spikeSystem) {
            NLM_LOG_ERROR("Brain: Spike system is not initialized");
            return false;
        }
        
        if (!pImpl->stdp) {
            NLM_LOG_ERROR("Brain: STDP system is not initialized");
            return false;
        }
        
        if (!pImpl->hebbian) {
            NLM_LOG_ERROR("Brain: Hebbian system is not initialized");
            return false;
        }
        
        if (!pImpl->structuralPlasticity) {
            NLM_LOG_ERROR("Brain: Structural plasticity system is not initialized");
            return false;
        }
        
        // Check configuration values
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 0);
        if (neuronCount == 0) {
            NLM_LOG_ERROR("Brain: neuron_count must be greater than 0");
            return false;
        }
        
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 0);
        if (regionCount == 0) {
            NLM_LOG_ERROR("Brain: region_count must be greater than 0");
            return false;
        }
        
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 1.0f);
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            NLM_LOG_ERROR("Brain: connection_probability must be between 0 and 1");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during configuration validation: ") + e.what());
        return false;
    }
}

bool Brain::createRegions(size_t regionCount) {
    try {
        pImpl->regions.clear();
        
        for (size_t i = 0; i < regionCount; ++i) {
            RegionId id(pImpl->nextRegionId++);
            auto region = std::make_unique<NeuralRegion>(id, "Region_" + std::to_string(i + 1));
            if (!region) {
                NLM_LOG_ERROR("Brain: Failed to create region " + std::to_string(i));
                return false;
            }
            pImpl->regions.push_back(std::move(region));
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception creating regions: ") + e.what());
        return false;
    }
}

bool Brain::createNeurons(size_t neuronCount, size_t regionCount, float connectionProbability) {
    try {
        if (regionCount == 0 || neuronCount == 0) {
            return false;
        }
        
        size_t neuronsPerRegion = neuronCount / regionCount;
        
        // Create neurons across regions
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_ERROR("Brain: Failed to get region " + std::to_string(i + 1));
                return false;
            }
            
            // Add populations with validation
            size_t sensoryNeurons = std::max<size_t>(1, neuronsPerRegion / 4);
            size_t internalNeurons = std::max<size_t>(1, neuronsPerRegion / 2);
            size_t motorNeurons = std::max<size_t>(1, neuronsPerRegion / 4);
            
            auto sensoryPopId = region->addPopulation(sensoryNeurons, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(internalNeurons, NeuronType::Internal);
            auto motorPopId = region->addPopulation(motorNeurons, NeuronType::Motor);
            
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
            if (region) {
                region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception creating neurons: ") + e.what());
        return false;
    }
}

void Brain::collectStatistics() {
    try {
        // Update development system
        if (pImpl->developmentSystem) {
            pImpl->developmentSystem->update(pImpl->timestep);
        }
        
        // Update episodic memory
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->update(pImpl->currentTime);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception collecting statistics: ") + e.what());
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    try {
        // Reset core simulation state
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0;
        pImpl->totalSpikesThisStep = 0;
        pImpl->totalSpikesTotal = 0;
        pImpl->isResting = false;
        pImpl->stepsSinceLastEpisode = 0;
        pImpl->developmentalStage = DevelopmentalStage::Initial;
        
        // Reset all regions
        for (auto& region : pImpl->regions) {
            if (region) {
                region->reset();
            }
        }
        
        // Reset spike system
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->reset();
        }
        
        // Reset memory systems
        if (pImpl->workingMemory) {
            pImpl->workingMemory->clear();
        }
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->clear();
        }
        if (pImpl->associativeMemory) {
            pImpl->associativeMemory->clear();
        }
        if (pImpl->attention) {
            pImpl->attention->reset();
        }
        
        NLM_LOG_INFO("NLM Brain reset complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during brain reset: ") + e.what());
    }
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        if (filepath.empty()) {
            NLM_LOG_ERROR("Brain save: filepath is empty");
            return false;
        }
        
        if (!validateBrainState()) {
            NLM_LOG_ERROR("Brain save: invalid brain state for saving");
            return false;
        }
        
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write neurons
        if (!writeNeuronsToCheckpoint(writer)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        if (!writeSynapsesToCheckpoint(writer)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        if (filepath.empty()) {
            NLM_LOG_ERROR("Brain load: filepath is empty");
            return false;
        }
        
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states with validation
        if (!applyNeuronStates(neuronData)) {
            NLM_LOG_ERROR("Failed to apply neuron states from checkpoint");
            return false;
        }
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

bool Brain::writeNeuronsToCheckpoint(CheckpointWriter& writer) const {
    try {
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        neuronData.firingState.reserve(getTotalNeuronCount());
        neuronData.refractoryRemaining.reserve(getTotalNeuronCount());
        neuronData.refractoryPeriod.reserve(getTotalNeuronCount());
        neuronData.lastSpikeTime.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (!neuron) continue;
                    
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
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint writer");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing neurons to checkpoint: ") + e.what());
        return false;
    }
}

bool Brain::writeSynapsesToCheckpoint(CheckpointWriter& writer) const {
    try {
        SynapseCheckpointData synapseData;
        
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                if (!syn) continue;
                
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint writer");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing synapses to checkpoint: ") + e.what());
        return false;
    }
}

bool Brain::applyNeuronStates(const NeuronCheckpointData& neuronData) {
    try {
        if (neuronData.membranePotential.size() != getTotalNeuronCount()) {
            NLM_LOG_ERROR("Neuron checkpoint data size mismatch");
            return false;
        }
        
        size_t idx = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron || idx >= neuronData.membranePotential.size()) {
                        continue;
                    }
                    
                    neuron->setMembranePotential(neuronData.membranePotential[idx]);
                    neuron->setRestingPotential(neuronData.restingPotential[idx]);
                    neuron->setThreshold(neuronData.threshold[idx]);
                    neuron->setResetPotential(neuronData.resetPotential[idx]);
                    neuron->setLeakConductance(neuronData.leakConductance[idx]);
                    
                    if (idx < neuronData.firingState.size()) {
                        neuron->setFiringState(static_cast<FiringState>(neuronData.firingState[idx]));
                    }
                    
                    if (idx < neuronData.refractoryRemaining.size()) {
                        neuron->setRefractoryRemaining(neuronData.refractoryRemaining[idx]);
                    }
                    
                    if (idx < neuronData.refractoryPeriod.size()) {
                        neuron->setRefractoryPeriod(neuronData.refractoryPeriod[idx]);
                    }
                    
                    idx++;
                }
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception applying neuron states: ") + e.what());
        return false;
    }
}

bool Brain::validateBrainState() const {
    // Check if brain has essential components
    if (pImpl->regions.empty()) {
        return false;
    }
    
    if (getTotalNeuronCount() == 0) {
        return false;
    }
    
    // Check for NaN or invalid values in neuron states
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (!neuron) continue;
                
                const auto& state = neuron->getState();
                if (std::isnan(state.membranePotential) || 
                    std::isinf(state.membranePotential)) {
                    return false;
                }
                
                if (state.threshold <= 0.0f) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

size_t Brain::getValidatedNeuronCount() const {
    return pImpl->config->getOr<size_t>("neuron_count", 1000);
}

size_t Brain::getValidatedRegionCount() const {
    return pImpl->config->getOr<size_t>("region_count", 1);
}

float Brain::getValidatedConnectionProbability() const {
    float prob = pImpl->config->getOr<float>("connection_probability", 0.1f);
    return std::max(0.0f, std::min(1.0f, prob));  // Clamp to [0,1]
}

}} // namespace nlm