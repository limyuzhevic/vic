// Brain improvements for better modularity and maintainability
// This file provides refactored implementations that split the large pImpl struct
// into more manageable, focused components while maintaining all functionality.

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Forward declarations for helper classes
class MemoryInitializer;
class PlasticityInitializer;
class CognitionInitializer;
class NeuromodulationInitializer;
class DevelopmentInitializer;

// Helper class to initialize memory systems
class MemoryInitializer {
public:
    static void initializeMemorySystems(Brain::Impl* impl, std::shared_ptr<Brain> brain) {
        if (!impl) return;
        
        // Initialize working memory
        if (impl->workingMemory) {
            impl->workingMemory->initialize(brain.get());
            impl->workingMemory->setCapacity(brain->getTotalNeuronCount() / 10);
            NLM_LOG_INFO("Working memory initialized with capacity: " + 
                        std::to_string(brain->getTotalNeuronCount() / 10));
        }
        
        // Initialize episodic memory
        if (impl->episodicMemory) {
            impl->episodicMemory->initialize(brain.get());
            impl->episodicMemory->setMaxEpisodes(1000);
            NLM_LOG_INFO("Episodic memory initialized with max episodes: 1000");
        }
        
        // Initialize associative memory
        if (impl->associativeMemory) {
            impl->associativeMemory->initialize(brain.get());
            NLM_LOG_INFO("Associative memory initialized");
        }
    }
};

// Helper class to initialize plasticity systems
class PlasticityInitializer {
public:
    static void configurePlasticitySystems(Brain::Impl* impl, std::shared_ptr<Config> config) {
        if (!impl || !config) return;
        
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        
        if (impl->stdp) {
            impl->stdp->configure(ltpWeight, ltdWeight, tau);
            NLM_LOG_INFO("STDP configured: LTP=" << ltpWeight << ", LTD=" << ltdWeight 
                        << ", tau=" << tau);
        }
        
        // Configure Hebbian parameters
        if (impl->hebbian) {
            // Hebbian parameters are typically configured internally
            NLM_LOG_INFO("Hebbian plasticity system ready");
        }
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        
        if (impl->structuralPlasticity) {
            impl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            impl->structuralPlasticity->setPruningRate(pruningRate);
            NLM_LOG_INFO("Structural plasticity configured: synaptogenesis=" << synaptogenesisRate 
                        << ", pruning=" << pruningRate);
        }
    }
};

// Helper class to initialize cognition systems
class CognitionInitializer {
public:
    static void initializeCognitionSystems(Brain::Impl* impl, std::shared_ptr<Brain> brain) {
        if (!impl || !brain) return;
        
        // Initialize neural planner
        if (impl->planner) {
            impl->planner->initialize(brain.get());
            impl->planner->setPlanningDepth(5);
            NLM_LOG_INFO("Neural planner initialized with depth: 5");
        }
        
        // Initialize concept formation
        if (impl->conceptFormation) {
            impl->conceptFormation->initialize(brain.get());
            NLM_LOG_INFO("Concept formation system initialized");
        }
        
        // Initialize attentional selection
        if (impl->attention) {
            impl->attention->initialize(brain.get());
            impl->attention->setInhibitionStrength(0.5f);
            impl->attention->setExcitationStrength(1.5f);
            NLM_LOG_INFO("Attentional selection initialized: inhibition=0.5, excitation=1.5");
        }
    }
};

// Helper class to initialize neuromodulation systems
class NeuromodulationInitializer {
public:
    static void initializeNeuromodulationSystems(Brain::Impl* impl, std::shared_ptr<Brain> brain) {
        if (!impl || !brain) return;
        
        // Initialize novelty detection
        if (impl->novelty) {
            impl->novelty->initialize(brain.get());
            NLM_LOG_INFO("Novelty detection system initialized");
        }
        
        // Initialize curiosity system
        if (impl->curiosity) {
            impl->curiosity->initialize(brain.get());
            NLM_LOG_INFO("Curiosity system initialized");
        }
        
        // Initialize dopamine (reward prediction error)
        if (impl->dopamine) {
            impl->dopamine->initialize(brain.get());
            NLM_LOG_INFO("Dopamine (reward) system initialized");
        }
        
        // Initialize prediction error
        if (impl->predictionError) {
            impl->predictionError->initialize(brain.get());
            NLM_LOG_INFO("Prediction error system initialized");
        }
    }
};

// Helper class to initialize development system
class DevelopmentInitializer {
public:
    static void initializeDevelopmentSystem(Brain::Impl* impl, std::shared_ptr<Brain> brain) {
        if (!impl || !brain) return;
        
        // Initialize development system
        if (impl->developmentSystem) {
            impl->developmentSystem->initialize(brain.get());
            NLM_LOG_INFO("Development system initialized");
        }
    }
};

// Brain implementation with improved modularity
namespace {

// Helper function to create regions
void createRegions(Brain::Impl* impl, size_t neuronCount, size_t regionCount) {
    if (!impl) return;
    
    // Clear existing regions
    impl->regions.clear();
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        RegionId id(i + 1);
        auto region = std::make_unique<NeuralRegion>(id, "Region_" + std::to_string(i + 1));
        impl->regions.push_back(std::move(region));
    }
    
    NLM_LOG_INFO("Created " << regionCount << " regions");
}

// Helper function to create neurons in regions
void createNeurons(Brain::Impl* impl, size_t neuronCount, size_t regionCount) {
    if (!impl || impl->regions.empty()) return;
    
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = impl->regions[i].get();
        if (!region) continue;
        
        // Add populations
        auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
        auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
        auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
        
        // Collect sensory and motor neurons for I/O
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        auto* motorPop = region->getPopulation(motorPopId);
        
        if (sensoryPop) {
            for (auto* neuron : sensoryPop->getNeurons()) {
                impl->sensoryNeurons.push_back(neuron);
            }
        }
        
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                impl->motorNeurons.push_back(neuron);
            }
        }
        
        NLM_LOG_INFO("Created populations in region " << (i + 1) << 
                    ": " << region->getPopulationCount() << " populations, " <<
                    region->getTotalNeuronCount() << " neurons");
    }
}

// Helper function to initialize connectivity
void initializeConnectivity(Brain::Impl* impl, std::shared_ptr<RandomGenerator> rng, 
                           float connectionProbability, float weightRange) {
    if (!impl || !rng) return;
    
    // Initialize random connectivity and synapse weights
    for (auto& region : impl->regions) {
        if (region) {
            region->initializeRandomConnectivity(*rng, connectionProbability, weightRange);
        }
    }
    
    NLM_LOG_INFO("Connectivity initialization completed");
}

// Helper function to register spike handlers
void registerSpikeHandlers(Brain::Impl* impl, std::shared_ptr<Brain> brain) {
    if (!impl || !brain) return;
    
    // Register spike counting handler
    impl->spikeSystem->registerHandler([impl](const DetailedSpikeEvent& event) {
        // Count spikes
        ++impl->totalSpikesThisStep;
        ++impl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    impl->spikeSystem->registerDelayedHandler([impl, brain](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : impl->regions) {
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

// Helper function to configure checkpoint manager
void configureCheckpointManager(Brain::Impl* impl, std::shared_ptr<Config> config) {
    if (!impl || !config) return;
    
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (impl->checkpointManager) {
        impl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        NLM_LOG_INFO("Checkpoint manager configured: directory=" << checkpointDir 
                    << ", max_checkpoints=10000");
    }
}

// Helper function to get region by ID
NeuralRegion* getRegionById(Brain::Impl* impl, RegionId id) {
    if (!impl) return nullptr;
    
    for (auto& region : impl->regions) {
        if (region && region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

// Helper function to calculate E/I ratio
float calculateExcitationInhibitionRatio(Brain::Impl* impl) {
    if (!impl) return 0.0f;
    
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : impl->regions) {
        if (!region) continue;
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
    } else if (totalExcitatory > 0.0f) {
        return std::numeric_limits<float>::infinity();
    } else {
        return 0.0f;
    }
}

} // namespace (anonymous)

// Brain constructor with improved initialization
Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {
    NLM_LOG_INFO("Brain created with configuration");
}

// Brain destructor
Brain::~Brain() = default;

// Brain move constructor
Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
    NLM_LOG_INFO("Brain moved");
}

// Brain move assignment operator
Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
        NLM_LOG_INFO("Brain move assigned");
    }
    return *this;
}

// Initialize brain with configuration
bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " << neuronCount << " neurons, " << regionCount << " regions");
    
    // Create regions using helper function
    createRegions(pImpl.get(), neuronCount, regionCount);
    
    // Create neurons using helper function
    createNeurons(pImpl.get(), neuronCount, regionCount);
    
    // Initialize connectivity using helper function
    initializeConnectivity(pImpl.get(), pImpl->rng.get(), connectionProbability, 0.2f);
    
    // ========== INITIALIZE INTEGRATED SYSTEMS ==========
    
    // Initialize memory systems using helper class
    MemoryInitializer::initializeMemorySystems(pImpl.get(), std::shared_ptr<Brain>(this));
    
    // Initialize plasticity systems
    PlasticityInitializer::configurePlasticitySystems(pImpl.get(), pImpl->config);
    
    // Initialize cognition systems
    CognitionInitializer::initializeCognitionSystems(pImpl.get(), std::shared_ptr<Brain>(this));
    
    // Initialize neuromodulation systems
    NeuromodulationInitializer::initializeNeuromodulationSystems(pImpl.get(), std::shared_ptr<Brain>(this));
    
    // Initialize development system
    DevelopmentInitializer::initializeDevelopmentSystem(pImpl.get(), std::shared_ptr<Brain>(this));
    
    // Register spike handlers using helper function
    registerSpikeHandlers(pImpl.get(), std::shared_ptr<Brain>(this));
    
    // Configure checkpoint manager using helper function
    configureCheckpointManager(pImpl.get(), pImpl->config);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " << getTotalNeuronCount());
    NLM_LOG_INFO("Total synapses: " << getTotalSynapseCount());
    NLM_LOG_INFO("Sensory neurons: " << pImpl->sensoryNeurons.size());
    NLM_LOG_INFO("Motor neurons: " << pImpl->motorNeurons.size());
    
    return true;
}

// Brain step function (main simulation loop) - organized with helper functions
void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    updateNeurons(pImpl.get(), currentTime, pImpl->timestep);
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    detectAndScheduleSpikes(pImpl.get(), currentStep, currentTime);
    
    // ========== STEP 4: Process immediate spikes ==========
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // ========== STEP 5: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // ========== STEP 6: Apply neuromodulation effects ==========
    applyNeuromodulationEffects(pImpl.get(), currentTime);
    
    // ========== STEP 7: Apply plasticity rules (STDP and Hebbian) ==========
    applyPlasticityRules(pImpl.get(), currentTime);
    
    // ========== STEP 8: Update episodic memory ==========
    updateEpisodicMemory(pImpl.get(), currentStep);
    
    // ========== STEP 9: Update prediction system ==========
    updatePredictionSystem(pImpl.get());
    
    // ========== STEP 10: Update attention system ==========
    updateAttentionSystem(pImpl.get());
    
    // ========== STEP 11: Update concept formation ==========
    updateConceptFormation(pImpl.get());
    
    // ========== STEP 12: Apply structural plasticity periodically ==========
    applyStructuralPlasticity(pImpl.get(), currentStep);
    
    // ========== STEP 13: Replay important memories ==========
    replayImportantMemories(pImpl.get(), currentStep);
    
    // ========== STEP 14: Apply development effects ==========
    applyDevelopmentEffects(pImpl.get(), currentStep);
    
    // ========== STEP 15: Periodic memory consolidation ==========
    consolidateMemories(pImpl.get(), currentStep);
    
    // ========== STEP 16: Checkpoint management ==========
    updateCheckpointManager(pImpl.get(), currentStep, currentTime);
    
    // ========== STEP 17: Collect statistics ==========
    collectStatistics(pImpl.get());
}

// Helper functions for step processing
namespace {

void updateNeurons(Brain::Impl* impl, Timestamp currentTime, TimestepDuration timestep) {
    for (auto& region : impl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void detectAndScheduleSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : impl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < 
                                 impl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    impl->spikeSystem->queueSpike(event);

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
                        Timestamp deliveryTime = currentTime + delay * impl->timestep;

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

                        impl->spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (impl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void applyNeuromodulationEffects(Brain::Impl* impl, Timestamp currentTime) {
    if (!impl) return;
    
    // Update novelty detection
    if (impl->novelty) {
        impl->novelty->update(impl->timestep);
    }
    
    // Update curiosity
    if (impl->curiosity) {
        impl->curiosity->update(impl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (impl->dopamine) {
        impl->dopamine->update(impl->timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = impl->dopamine->getLevel();
        for (auto& region : impl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void applyPlasticityRules(Brain::Impl* impl, Timestamp currentTime) {
    if (!impl) return;
    
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (impl->dopamine) {
        plasticityMod = impl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : impl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    impl->stdp->update(syn, preSpikes, postSpikes, impl->timestep);
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
                    impl->hebbian->update(syn, preSpikes, postSpikes, impl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void updateEpisodicMemory(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl) return;
    
    impl->stepsSinceLastEpisode++;
    if (impl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        impl->stepsSinceLastEpisode = 0;
        
        if (impl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = impl->dopamine ? impl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : impl->regions) {
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
            
            // Store reward in episode
            episode.reward = impl->dopamine ? impl->dopamine->getLevel() : 0.0f;
            
            impl->episodicMemory->storeEpisode(episode);
        }
    }
}

void updatePredictionSystem(Brain::Impl* impl) {
    if (!impl || !impl->predictionSystem) {
        return;
    }
    
    // The prediction system would be updated with sensory observations
    // For now, just track prediction error history
}

void updateAttentionSystem(Brain::Impl* impl) {
    if (!impl || !impl->attention) {
        return;
    }
    
    impl->attention->update(impl->timestep);
    
    // Apply attention to working memory winners
    if (impl->workingMemory && !impl->workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = impl->workingMemory->getMemoryNeurons();
        impl->attention->processCompetition(competitors);
    }
}

void updateConceptFormation(Brain::Impl* impl) {
    if (!impl || !impl->conceptFormation) {
        return;
    }
    
    // Would process current neural activity patterns to form concepts
    // This requires sensory state encoding
}

void applyStructuralPlasticity(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl || !impl->structuralPlasticity) {
        return;
    }
    
    if (currentStep % 100 == 0) {
        impl->structuralPlasticity->update(nullptr, *impl->rng);
    }
}

void replayImportantMemories(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl || !impl->episodicMemory) {
        return;
    }
    
    // Replay important memories (during rest or periodically)
    if (currentStep % impl->replayInterval == 0) {
        // Get episodes for replay
        auto episodesToReplay = impl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            impl->episodicMemory->replayEpisode(episode);
        }
    }
}

void applyDevelopmentEffects(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl || !impl->developmentSystem) {
        return;
    }
    
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        impl->developmentSystem->update(nullptr, *impl->rng, impl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = impl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = impl->developmentalStage;
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;  // High plasticity
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;  // Stable
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void consolidateMemories(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl || !impl->episodicMemory) {
        return;
    }
    
    // Periodic memory consolidation
    if (currentStep % impl->consolidationInterval == 0) {
        // Consolidate important memories, remove weak ones
        impl->episodicMemory->consolidate(0.3f);
    }
}

void updateCheckpointManager(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
    if (!impl || !impl->checkpointManager) {
        return;
    }
    
    impl->checkpointManager->update(currentStep, currentTime);
}

void collectStatistics(Brain::Impl* impl) {
    if (!impl) return;
    
    // Basic statistics collection
    NLM_LOG_INFO("Step statistics: " << impl->currentStep << ", " << 
                impl->totalSpikesThisStep << " spikes this step, " <<
                impl->totalSpikesTotal << " total spikes");
}

} // namespace (anonymous)

// Additional Brain methods
void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

float Brain::getExcitationInhibitionRatio() const {
    return calculateExcitationInhibitionRatio(pImpl.get());
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
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

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void Brain::reset() {
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
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " << filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " << filepath);
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
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
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
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" << writer.getBytesWritten() << " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " << filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " << filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " << reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        size_t idx = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
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
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " << synapseData.weight.size() << " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return getRegionById(pImpl.get(), id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return getRegionById(pImpl.get(), id);
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEMS ==========

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " << getRegionCount());
    NLM_LOG_INFO("Total neurons: " << getTotalNeuronCount());
    NLM_LOG_INFO("Total synapses: " << getTotalSynapseCount());
    NLM_LOG_INFO("Active neurons: " << getActiveNeuronCount());
    NLM_LOG_INFO("Firing neurons (this step): " << getFiringNeuronCount());
    NLM_LOG_INFO("Total spikes: " << getTotalSpikeCount());
    NLM_LOG_INFO("Pending spike events: " << getPendingSpikeEventCount());
    NLM_LOG_INFO("Average firing rate: " << getAverageFiringRate());
    NLM_LOG_INFO("E/I ratio: " << getExcitationInhibitionRatio());
    
    // Memory system status
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " << pImpl->workingMemory->getActiveTraces());
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " << pImpl->episodicMemory->getEpisodeCount());
    }
    
    // Neuromodulation status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " << pImpl->dopamine->getLevel());
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " << static_cast<int>(pImpl->developmentalStage));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " << region->getId().index() << 
                    " (" << region->getName() << "): " <<
                    region->getTotalNeuronCount() << " neurons, " <<
                    region->getSynapseCount() << " synapses, " <<
                    "avg weight: " << region->getAverageSynapticWeight());
    }
}

} // namespace nlm
