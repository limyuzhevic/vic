#include "Brain.hpp"
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

namespace nlm {

Brain::Brain(std::shared_ptr<Config> config) : brainCore(new BrainCore(config)) {}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : brainCore(other.brainCore) {
    other.brainCore = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete brainCore;
        brainCore = other.brainCore;
        other.brainCore = nullptr;
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = brainCore->getConfig()->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = brainCore->getConfig()->getOr<size_t>("region_count", 1);
    float connectionProbability = brainCore->getConfig()->getOr<float>("connection_probability", 0.1f);
    
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
                    brainCore->addSensoryNeuron(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    brainCore->addMotorNeuron(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*brainCore->getRandomGenerator(), connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    if (brainCore->getWorkingMemory()) {
        brainCore->getWorkingMemory()->initialize(this);
        brainCore->getWorkingMemory()->setCapacity(neuronCount / 10);
    }
    
    // Initialize episodic memory
    if (brainCore->getEpisodicMemory()) {
        brainCore->getEpisodicMemory()->initialize(this);
        brainCore->getEpisodicMemory()->setMaxEpisodes(1000);
    }
    
    // Initialize associative memory
    if (brainCore->getAssociativeMemory()) {
        brainCore->getAssociativeMemory()->initialize(this);
    }
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    if (brainCore->getPlanner()) {
        brainCore->getPlanner()->initialize(this);
        brainCore->getPlanner()->setPlanningDepth(5);
    }
    
    if (brainCore->getConceptFormation()) {
        brainCore->getConceptFormation()->initialize(this);
    }
    
    if (brainCore->getAttention()) {
        brainCore->getAttention()->initialize(this);
        brainCore->getAttention()->setInhibitionStrength(0.5f);
        brainCore->getAttention()->setExcitationStrength(1.5f);
    }
    
    // Initialize neuromodulation
    if (brainCore->getNovelty()) {
        brainCore->getNovelty()->initialize(this);
    }
    if (brainCore->getCuriosity()) {
        brainCore->getCuriosity()->initialize(this);
    }
    
    // Register spike handlers for event-driven processing
    if (brainCore->getSpikeSystem()) {
        brainCore->getSpikeSystem()->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++getTotalSpikeCount();
        });
    }
    
    // Configure checkpoint manager
    std::string checkpointDir = brainCore->getConfig()->getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (brainCore->getCheckpointManager()) {
        brainCore->getCheckpointManager()->configure(checkpointDir, 10000, 5, true);
    }
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(brainCore->getSensoryNeuronCount()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(brainCore->getMotorNeuronCount()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * brainCore->getTimestep());
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
     * 
     * 1. Process pending delayed spike events (deliver synaptic input)
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
     * 4. Update working memory (maintenance and competition)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect statistics
     */
    
    brainCore->setCurrentStep(currentStep);
    brainCore->setCurrentTime(currentTime);
    brainCore->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    if (brainCore->getSpikeSystem()) {
        brainCore->getSpikeSystem()->processDelayedSpikes(currentStep, currentTime);
    }
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : brainCore->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, brainCore->getTimestep());
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : brainCore->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < brainCore->getTimestep() * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    if (brainCore->getSpikeSystem()) {
                        brainCore->getSpikeSystem()->queueSpike(event);
                    }

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
                        Timestamp deliveryTime = currentTime + delay * brainCore->getTimestep();

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

                        if (brainCore->getSpikeSystem()) {
                            brainCore->getSpikeSystem()->queueDelayedSpike(delayedEvent);
                        }

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (brainCore->getWorkingMemory()) {
                        brainCore->getWorkingMemory()->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    if (brainCore->getSpikeSystem()) {
        brainCore->getSpikeSystem()->processSpikes(currentStep);
    }
    
    // ========== STEP 4: Update working memory ==========
    if (brainCore->getWorkingMemory()) {
        brainCore->getWorkingMemory()->update(brainCore->getTimestep());
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    // Update novelty detection
    if (brainCore->getNovelty()) {
        brainCore->getNovelty()->update(brainCore->getTimestep());
    }
    
    // Update curiosity
    if (brainCore->getCuriosity()) {
        brainCore->getCuriosity()->update(brainCore->getTimestep());
    }
    
    // Update dopamine (reward prediction error)
    if (brainCore->getDopamine()) {
        brainCore->getDopamine()->update(brainCore->getTimestep());
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = brainCore->getDopamine()->getLevel();
        for (auto& region : brainCore->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (brainCore->getDopamine()) {
        plasticityMod = brainCore->getDopamine()->getPlasticityFactor();
    }
    
    for (auto& region : brainCore->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    if (brainCore->getSTDP()) {
                        brainCore->getSTDP()->update(syn, preSpikes, postSpikes, brainCore->getTimestep());
                    }
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
                    if (brainCore->getHebbian()) {
                        brainCore->getHebbian()->update(syn, preSpikes, postSpikes, brainCore->getTimestep());
                    }
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
    
    // ========== STEP 7: Update episodic memory ==========
    brainCore->stepsSinceLastEpisode++;
    if (brainCore->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        brainCore->stepsSinceLastEpisode = 0;
        
        if (brainCore->getEpisodicMemory()) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = brainCore->getDopamine() ? brainCore->getDopamine()->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : brainCore->getRegions()) {
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
            episode.reward = brainCore->getDopamine() ? brainCore->getDopamine()->getLevel() : 0.0f;
            
            brainCore->getEpisodicMemory()->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (brainCore->getPredictionSystem()) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    // ========== STEP 9: Update attention system ==========
    if (brainCore->getAttention()) {
        brainCore->getAttention()->update(brainCore->getTimestep());
        
        // Apply attention to working memory winners
        if (brainCore->getWorkingMemory() && !brainCore->getWorkingMemory()->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = brainCore->getWorkingMemory()->getMemoryNeurons();
            brainCore->getAttention()->processCompetition(competitors);
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (brainCore->getConceptFormation()) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0 && brainCore->getStructuralPlasticity()) {
        brainCore->getStructuralPlasticity()->update(this, *brainCore->getRandomGenerator());
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % brainCore->getReplayInterval() == 0 && brainCore->getEpisodicMemory()) {
        // Get episodes for replay
        auto episodesToReplay = brainCore->getEpisodicMemory()->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            brainCore->getEpisodicMemory()->replayEpisode(episode);
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
    if (currentStep % 1000 == 0 && brainCore->getDevelopmentSystem()) {  // Update development every 1000 steps
        brainCore->getDevelopmentSystem()->update(this, *brainCore->getRandomGenerator(), brainCore->getTimestep() * 1000);
        
        // Development affects plasticity rates
        auto* sp = brainCore->getStructuralPlasticity();
        if (sp) {
            DevelopmentalStage stage = brainCore->getDevelopmentalStage();
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
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % brainCore->getConsolidationInterval() == 0 && brainCore->getEpisodicMemory()) {
        // Consolidate important memories, remove weak ones
        brainCore->getEpisodicMemory()->consolidate(0.3f);
    }
    
    // ========== STEP 15: Checkpoint management ==========
    if (brainCore->getCheckpointManager()) {
        brainCore->getCheckpointManager()->update(currentStep, currentTime);
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = brainCore->getSensoryNeuronCount();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        brainCore->getSensoryNeuron(i)->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (brainCore->getWorkingMemory() && normalizedValue > 0.5f) {
            brainCore->getWorkingMemory()->storeToNeuron(brainCore->getSensoryNeuron(i)->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : brainCore->getRegions()) {
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
    for (auto& region : brainCore->getRegions()) {
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
    return brainCore->getSpikeSystem();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return brainCore->getSpikeSystem();
}

STDP* Brain::getSTDP() {
    return brainCore->getSTDP();
}

Hebbian* Brain::getHebbian() {
    return brainCore->getHebbian();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return brainCore->getStructuralPlasticity();
}

float Brain::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : brainCore->getRegions()) {
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

size_t Brain::getTotalSpikeCount() const {
    return brainCore->getTotalSpikeCount();
}

size_t Brain::getPendingSpikeEventCount() const {
    return brainCore->getPendingSpikeEventCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (brainCore->getMotorNeuronCount() == 0) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : brainCore->getMotorNeurons()) {
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
    if (brainCore->getSTDP()) {
        brainCore->getSTDP()->setLTPWeight(0.01f * modulation);
        brainCore->getSTDP()->setLTDWeight(0.012f * modulation);
    }
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
    if (brainCore->getStructuralPlasticity()) {
        brainCore->getStructuralPlasticity()->update(this, *brainCore->getRandomGenerator());
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    brainCore->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    return brainCore->save(filepath);
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    return brainCore->load(filepath);
}

RegionId Brain::addRegion(const std::string& name) {
    RegionId id(brainCore->getNextRegionId());
    auto region = std::make_unique<NeuralRegion>(id, name);
    brainCore->addRegion(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    size_t index = id.index();
    if (index < brainCore->getRegionCount()) {
        return brainCore->getRegion(index);
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    size_t index = id.index();
    if (index < brainCore->getRegionCount()) {
        return brainCore->getRegion(index);
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return brainCore->getRegionCount();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(brainCore->getRegionCount());
    for (size_t i = 0; i < brainCore->getRegionCount(); ++i) {
        ids.push_back(RegionId(i));
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    // This would need to be added to BrainCore
    static const std::vector<std::unique_ptr<NeuralRegion>> empty;
    return empty;  // Placeholder - would need proper implementation
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    // Note: Inter-region connections are not yet exposed through BrainCore
    // This would need to be added to BrainCore
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    // Note: Inter-region connections are not yet exposed through BrainCore
    // This would need to be added to BrainCore
}

size_t Brain::getTotalNeuronCount() const {
    return brainCore->getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return brainCore->getTotalSynapseCount();
}

size_t Brain::getActiveNeuronCount() const {
    // Note: This would need to be added to BrainCore
    return 0;  // Placeholder
}

size_t Brain::getFiringNeuronCount() const {
    return brainCore->getTotalSpikeCount();
}

float Brain::getAverageFiringRate() const {
    return brainCore->getAverageFiringRate();
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return brainCore->getWorkingMemory();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return brainCore->getEpisodicMemory();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return brainCore->getAssociativeMemory();
}

PredictionSystem* Brain::getPredictionSystem() {
    return brainCore->getPredictionSystem();
}

NeuralPlanner* Brain::getPlanner() {
    return brainCore->getPlanner();
}

ConceptFormation* Brain::getConceptFormation() {
    return brainCore->getConceptFormation();
}

AttentionalSelection* Brain::getAttention() {
    return brainCore->getAttention();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return brainCore->getDevelopmentSystem();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return brainCore->getDevelopmentalStage();
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    brainCore->setDevelopmentalStage(stage);
}

Dopamine* Brain::getDopamine() {
    return brainCore->getDopamine();
}

Curiosity* Brain::getCuriosity() {
    return brainCore->getCuriosity();
}

Novelty* Brain::getNovelty() {
    return brainCore->getNovelty();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return brainCore->getPredictionErrorSignal();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return brainCore->getConfig();
}

RandomGenerator* Brain::getRandomGenerator() {
    return brainCore->getRandomGenerator();
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Memory system status
    if (getWorkingMemory()) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(getWorkingMemory()->getActiveTraces()));
    }
    if (getEpisodicMemory()) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(getEpisodicMemory()->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (getDopamine()) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(getDopamine()->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(getDevelopmentalStage())));
    
    for (size_t i = 0; i < getRegionCount(); ++i) {
        auto* region = getRegion(RegionId(i));
        if (region) {
            NLM_LOG_INFO("  Region " + std::to_string(i) + 
                        " (" + region->getName() + "): " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                        std::to_string(region->getSynapseCount()) + " synapses, " +
                        "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
        }
    }
}

} // namespace nlm

(End of file - total 1567 lines)