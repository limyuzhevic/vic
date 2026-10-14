#include "BrainSystems.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "Brain.hpp"
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
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// ========== NEURONAL SYSTEM ==========

NeuronalSystem::NeuronalSystem(SystemPriority priority)
    : SpecializedBrainSystem(priority) {}

bool NeuronalSystem::initialize(Brain* brain) {
    this->brainRef = brain;
    this->config = brain->getConfig();
    this->rng = brain->getRandomGenerator();
    
    NLM_LOG_INFO("Initializing NeuronalSystem...");
    return true;
}

void NeuronalSystem::update(double timestep) {
    // Neuronal system updates are handled by the orchestrator's step execution
}

void NeuronalSystem::reset() {
    NLM_LOG_INFO("Resetting NeuronalSystem...");
}

void NeuronalSystem::processNeuronalDynamics(SimulationStep step, Timestamp time) {
    // STEP 2: Update all neurons (LIF dynamics)
    for (auto& region : brainRef->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(time, static_cast<double>(timestep));
            }
        }
    }
}

void NeuronalSystem::detectAndScheduleSpikes(SimulationStep step, Timestamp time) {
    // STEP 3: Detect spikes and schedule spike events
    for (auto& region : brainRef->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(time) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    SpikeEvent event(neuron->getId(), time, step);
                    brainRef->getSpikeSystem()->queueSpike(event);

                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(time);
                    }

                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = step + delay;
                        Timestamp deliveryTime = time + delay * timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            time,
                            deliveryTime,
                            step,
                            deliveryStep
                        );

                        brainRef->getSpikeSystem()->queueDelayedSpike(delayedEvent);
                        syn->recordPreSpike(time);
                    }

                    if (brainRef->getWorkingMemory() && brainRef->getWorkingMemory()->getActiveTraces() > 0) {
                        brainRef->getWorkingMemory()->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void NeuronalSystem::processDelayedSpikes(SimulationStep step, Timestamp time) {
    // STEP 1: Process pending delayed spikes (deliver synaptic input)
    brainRef->getSpikeSystem()->processDelayedSpikes(step, time);
}

void NeuronalSystem::deliverSynapticInput(const class DelayedSpikeEvent& event) {
    // Find destination neuron and deliver synaptic input
    for (auto& region : brainRef->getRegions()) {
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
}

size_t NeuronalSystem::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : brainRef->getRegions()) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t NeuronalSystem::getFiringNeuronCount() const {
    return brainRef->getFiringNeuronCount();
}

float NeuronalSystem::getAverageFiringRate() const {
    if (brainRef->getRegions().empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : brainRef->getRegions()) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(brainRef->getRegions().size());
}

// ========== MEMORY SYSTEM ==========

MemorySystem::MemorySystem(SystemPriority priority)
    : SpecializedBrainSystem(priority) {}

bool MemorySystem::initialize(Brain* brain) {
    this->brainRef = brain;
    this->config = brain->getConfig();
    this->rng = brain->getRandomGenerator();
    
    NLM_LOG_INFO("Initializing MemorySystem...");
    return true;
}

void MemorySystem::update(double timestep) {
    // Memory system updates are handled by the orchestrator's step execution
}

void MemorySystem::reset() {
    NLM_LOG_INFO("Resetting MemorySystem...");
}

void MemorySystem::updateWorkingMemory(double timestep) {
    // STEP 4: Update working memory
    if (brainRef->getWorkingMemory()) {
        brainRef->getWorkingMemory()->update(timestep);
    }
}

void MemorySystem::storeSpikeActivity(const class SpikeEvent& spike, double membranePotential) {
    if (brainRef->getWorkingMemory() && membranePotential > 0.5f) {
        brainRef->getWorkingMemory()->storeToNeuron(spike.neuron_id, membranePotential / 10.0f);
    }
}

void MemorySystem::updateEpisodicMemory(SimulationStep step, float reward) {
    // STEP 7: Update episodic memory
    static size_t stepsSinceLastEpisode = 0;
    stepsSinceLastEpisode++;
    
    if (stepsSinceLastEpisode >= 10) {
        stepsSinceLastEpisode = 0;
        
        if (brainRef->getEpisodicMemory()) {
            EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.reward = reward;
            
            for (auto& region : brainRef->getRegions()) {
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
            
            episode.reward = reward;
            brainRef->getEpisodicMemory()->storeEpisode(episode);
        }
    }
}

void MemorySystem::captureCurrentState(SimulationStep step, float reward) {
    updateEpisodicMemory(step, reward);
}

void MemorySystem::replayMemories(SimulationStep step, size_t count) {
    // STEP 12: Replay important memories
    if (brainRef->getEpisodicMemory() && step % 100 == 0) {
        auto episodesToReplay = brainRef->getEpisodicMemory()->getEpisodesForReplay(count);
        for (const auto* episode : episodesToReplay) {
            brainRef->getEpisodicMemory()->replayEpisode(episode);
        }
    }
}

size_t MemorySystem::getWorkingMemoryTraceCount() const {
    return brainRef->getWorkingMemory() ? brainRef->getWorkingMemory()->getActiveTraces() : 0;
}

size_t MemorySystem::getEpisodicMemoryEpisodeCount() const {
    return brainRef->getEpisodicMemory() ? brainRef->getEpisodicMemory()->getEpisodeCount() : 0;
}

// ========== PREDICTION SYSTEM ==========

PredictionSystem::PredictionSystem(SystemPriority priority)
    : SpecializedBrainSystem(priority) {}

bool PredictionSystem::initialize(Brain* brain) {
    this->brainRef = brain;
    this->config = brain->getConfig();
    this->rng = brain->getRandomGenerator();
    
    NLM_LOG_INFO("Initializing PredictionSystem...");
    return true;
}

void PredictionSystem::update(double timestep) {
    // Prediction system updates are handled by the orchestrator's step execution
}

void PredictionSystem::reset() {
    NLM_LOG_INFO("Resetting PredictionSystem...");
}

void PredictionSystem::updatePrediction(SimulationStep step) {
    // STEP 8: Update prediction system
    if (brainRef->getPredictionSystem()) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void PredictionSystem::updateAttention(double timestep) {
    // STEP 9: Update attention system
    if (brainRef->getAttention() && brainRef->getWorkingMemory() && 
        !brainRef->getWorkingMemory()->getMemoryNeurons().empty()) {
        brainRef->getAttention()->update(timestep);
        std::vector<NeuronId> competitors = brainRef->getWorkingMemory()->getMemoryNeurons();
        brainRef->getAttention()->processCompetition(competitors);
    }
}

void PredictionSystem::updateConceptFormation() {
    // STEP 10: Update concept formation
    if (brainRef->getConceptFormation()) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void PredictionSystem::processCompetition(std::vector<NeuronId>& competitors) {
    if (brainRef->getAttention()) {
        brainRef->getAttention()->processCompetition(competitors);
    }
}

void PredictionSystem::updatePlanning(SimulationStep step) {
    if (brainRef->getPlanner()) {
        brainRef->getPlanner()->update(step);
    }
}

// ========== NEUROMODULATION SYSTEM ==========

NeuromodulationSystem::NeuromodulationSystem(SystemPriority priority)
    : SpecializedBrainSystem(priority) {}

bool NeuromodulationSystem::initialize(Brain* brain) {
    this->brainRef = brain;
    this->config = brainRef->getConfig();
    this->rng = brainRef->getRandomGenerator();
    
    NLM_LOG_INFO("Initializing NeuromodulationSystem...");
    return true;
}

void NeuromodulationSystem::update(double timestep) {
    // Neuromodulation system updates are handled by the orchestrator's step execution
}

void NeuromodulationSystem::reset() {
    NLM_LOG_INFO("Resetting NeuromodulationSystem...");
}

void NeuromodulationSystem::updateNovelty(double timestep) {
    if (brainRef->getNovelty()) {
        brainRef->getNovelty()->update(timestep);
    }
}

void NeuromodulationSystem::updateCuriosity(double timestep) {
    if (brainRef->getCuriosity()) {
        brainRef->getCuriosity()->update(timestep);
    }
}

void NeuromodulationSystem::updateDopamine(double timestep) {
    if (brainRef->getDopamine()) {
        brainRef->getDopamine()->update(timestep);
    }
}

float NeuromodulationSystem::getDopamineLevel() const {
    return brainRef->getDopamine() ? brainRef->getDopamine()->getLevel() : 0.0f;
}

float NeuromodulationSystem::getPlasticityFactor() const {
    return brainRef->getDopamine() ? brainRef->getDopamine()->getPlasticityFactor() : 1.0f;
}

void NeuromodulationSystem::applyDopamineEffects(SimulationStep step) {
    // STEP 5: Apply neuromodulation effects
    float dopamineLevel = getDopamineLevel();
    
    for (auto& region : brainRef->getRegions()) {
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

void NeuromodulationSystem::applyCuriosityEffects(double timestep) {
    // Apply curiosity-based modulation
    if (brainRef->getCuriosity()) {
        brainRef->getCuriosity()->applyExplorationBonus(timestep);
    }
}

// ========== PLASTICITY SYSTEM ==========

PlasticitySystem::PlasticitySystem(SystemPriority priority)
    : SpecializedBrainSystem(priority) {}

bool PlasticitySystem::initialize(Brain* brain) {
    this->brainRef = brain;
    this->config = brainRef->getConfig();
    this->rng = brainRef->getRandomGenerator();
    
    NLM_LOG_INFO("Initializing PlasticitySystem...");
    return true;
}

void PlasticitySystem::update(double timestep) {
    // Plasticity system updates are handled by the orchestrator's step execution
}

void PlasticitySystem::reset() {
    NLM_LOG_INFO("Resetting PlasticitySystem...");
}

void PlasticitySystem::applySTDP(SimulationStep step) {
    // STEP 6: Apply plasticity rules (STDP and Hebbian)
    float plasticityMod = 1.0f;
    if (brainRef->getDopamine()) {
        plasticityMod = brainRef->getDopamine()->getPlasticityFactor();
    }
    
    for (auto& region : brainRef->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    brainRef->getSTDP()->update(syn, preSpikes, postSpikes, timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    brainRef->getHebbian()->update(syn, preSpikes, postSpikes, timestep);
                }
            }
            
            syn->step(static_cast<Timestamp>(step) * timestep);
        }
    }
}

void PlasticitySystem::applyHebbian(SimulationStep step) {
    // Hebbian learning (already included in applySTDP)
}

void PlasticitySystem::applyStructuralPlasticity(SimulationStep step) {
    // STEP 11: Apply structural plasticity
    if (step % 100 == 0) {
        brainRef->getStructuralPlasticity()->update(brainRef, *brainRef->getRandomGenerator());
    }
}

void PlasticitySystem::updateDevelopment(SimulationStep step) {
    // STEP 13: Apply development effects
    if (step % 1000 == 0) {
        brainRef->getDevelopmentSystem()->update(brainRef, *brainRef->getRandomGenerator(), 1000.0);
        
        auto* sp = brainRef->getStructuralPlasticity();
        if (sp) {
            DevelopmentalStage stage = brainRef->getDevelopmentalStage();
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

void PlasticitySystem::updateDevelopmentalStage(SimulationStep step) {
    updateDevelopment(step);
}

float PlasticitySystem::getPlasticityModulation() const {
    return getPlasticityFactor();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem::DevelopmentSystem(SystemPriority priority)
    : SpecializedBrainSystem(priority), stage(DevelopmentalStage::Initial) {}

bool DevelopmentSystem::initialize(Brain* brain) {
    this->brainRef = brain;
    this->config = brainRef->getConfig();
    this->rng = brainRef->getRandomGenerator();
    
    NLM_LOG_INFO("Initializing DevelopmentSystem...");
    return true;
}

void DevelopmentSystem::update(double timestep) {
    // Development system updates are handled by the orchestrator's step execution
}

void DevelopmentSystem::reset() {
    NLM_LOG_INFO("Resetting DevelopmentSystem...");
}

void DevelopmentSystem::applyDevelopment(SimulationStep step, double timestep) {
    // Development effects
    updatePlasticityRates(getPlasticityModulation());
}

void DevelopmentSystem::updatePlasticityRates(float plasticityMod) {
    auto* sp = brainRef->getStructuralPlasticity();
    if (sp) {
        sp->setSynaptogenesisRate(0.0001f * plasticityMod);
        sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }
}

// ========== CHECKPOINT SYSTEM ==========

CheckpointSystem::CheckpointSystem(SystemPriority priority)
    : SpecializedBrainSystem(priority) {}

bool CheckpointSystem::initialize(Brain* brain) {
    this->brainRef = brain;
    this->config = brainRef->getConfig();
    this->rng = brainRef->getRandomGenerator();
    
    NLM_LOG_INFO("Initializing CheckpointSystem...");
    return true;
}

void CheckpointSystem::update(double timestep) {
    // Checkpoint system updates are handled by the orchestrator's step execution
}

void CheckpointSystem::reset() {
    NLM_LOG_INFO("Resetting CheckpointSystem...");
}

void CheckpointSystem::manageCheckpoints(SimulationStep step, Timestamp time) {
    // STEP 15: Checkpoint management
    if (brainRef->getCheckpointManager()) {
        brainRef->getCheckpointManager()->update(step, time);
    }
}

bool CheckpointSystem::save(const std::string& filepath) const {
    return brainRef->save(filepath);
}

bool CheckpointSystem::load(const std::string& filepath) {
    return brainRef->load(filepath);
}

size_t CheckpointSystem::getCheckpointCount() const {
    return brainRef->getCheckpointManager() ? 
        brainRef->getCheckpointManager()->getCheckpointCount() : 0;
}

// ========== BRAIN ORCHESTRATOR ==========

BrainOrchestrator::BrainOrchestrator(std::shared_ptr<Config> config)
    : config(config), brainRef(nullptr) {
    // Initialize specialized systems
    neuronalSystem = std::make_unique<NeuronalSystem>();
    memorySystem = std::make_unique<MemorySystem>();
    predictionSystem = std::make_unique<PredictionSystem>();
    neuromodulationSystem = std::make_unique<NeuromodulationSystem>();
    plasticitySystem = std::make_unique<PlasticitySystem>();
    developmentSystem = std::make_unique<DevelopmentSystem>();
    checkpointSystem = std::make_unique<CheckpointSystem>();
    
    // Add all systems to the systems list
    systems.push_back(std::move(neuronalSystem));
    systems.push_back(std::move(memorySystem));
    systems.push_back(std::move(predictionSystem));
    systems.push_back(std::move(neuromodulationSystem));
    systems.push_back(std::move(plasticitySystem));
    systems.push_back(std::move(developmentSystem));
    systems.push_back(std::move(checkpointSystem));
    
    organizeSystemsByPriority();
}

BrainOrchestrator::~BrainOrchestrator() = default;

bool BrainOrchestrator::initialize(Brain* brain) {
    this->brainRef = brain;
    
    NLM_LOG_INFO("Initializing BrainOrchestrator with " + std::to_string(systems.size()) + " systems...");
    
    for (auto& system : systems) {
        if (system->initialize(brain)) {
            NLM_LOG_INFO("Initialized system: " + system->getName());
        } else {
            NLM_LOG_ERROR("Failed to initialize system: " + system->getName());
            return false;
        }
    }
    
    NLM_LOG_INFO("BrainOrchestrator initialization complete");
    return true;
}

void BrainOrchestrator::executeStep(SimulationStep step, Timestamp time) {
    // Execute step using Template Method pattern
    preStep(step, time);
    doStep(step, time);
    postStep(step, time);
}

void BrainOrchestrator::preStep(SimulationStep step, Timestamp time) {
    // Common pre-step operations
    brainRef->pImpl->currentStep = step;
    brainRef->pImpl->currentTime = time;
    brainRef->pImpl->totalSpikesThisStep = 0;
}

void BrainOrchestrator::doStep(SimulationStep step, Timestamp time) {
    // STEP 1: Process pending delayed spikes
    neuronalSystem->processDelayedSpikes(step, time);
    
    // STEP 2: Process neuronal dynamics
    neuronalSystem->processNeuronalDynamics(step, time);
    
    // STEP 3: Detect and schedule spikes
    neuronalSystem->detectAndScheduleSpikes(step, time);
    
    // STEP 4: Update working memory
    memorySystem->updateWorkingMemory(static_cast<double>(time) / 1000.0);
    
    // STEP 5: Apply neuromodulation
    neuromodulationSystem->applyDopamineEffects(step);
    neuromodulationSystem->updateNovelty(static_cast<double>(time) / 1000.0);
    neuromodulationSystem->updateCuriosity(static_cast<double>(time) / 1000.0);
    neuromodulationSystem->updateDopamine(static_cast<double>(time) / 1000.0);
    
    // STEP 6: Apply plasticity rules
    plasticitySystem->applySTDP(step);
    plasticitySystem->applyHebbian(step);
    
    // STEP 7: Update episodic memory
    memorySystem->updateEpisodicMemory(step, neuromodulationSystem->getDopamineLevel());
    
    // STEP 8: Update prediction system
    predictionSystem->updatePrediction(step);
    
    // STEP 9: Update attention system
    predictionSystem->updateAttention(static_cast<double>(time) / 1000.0);
    
    // STEP 10: Update concept formation
    predictionSystem->updateConceptFormation();
    
    // STEP 11: Apply structural plasticity
    plasticitySystem->applyStructuralPlasticity(step);
    
    // STEP 12: Replay important memories
    memorySystem->replayMemories(step, 3);
    
    // STEP 13: Apply development effects
    plasticitySystem->updateDevelopment(step);
    
    // STEP 14: Periodic memory consolidation
    if (step % 1000 == 0 && brainRef->getEpisodicMemory()) {
        brainRef->getEpisodicMemory()->consolidate(0.3f);
    }
    
    // STEP 15: Checkpoint management
    checkpointSystem->manageCheckpoints(step, time);
}

void BrainOrchestrator::postStep(SimulationStep step, Timestamp time) {
    // Common post-step operations
    // Process immediate spikes
    brainRef->getSpikeSystem()->processSpikes(step);
}

NeuronalSystem* BrainOrchestrator::getNeuronalSystem() {
    return neuronalSystem.get();
}

MemorySystem* BrainOrchestrator::getMemorySystem() {
    return memorySystem.get();
}

PredictionSystem* BrainOrchestrator::getPredictionSystem() {
    return predictionSystem.get();
}

NeuromodulationSystem* BrainOrchestrator::getNeuromodulationSystem() {
    return neuromodulationSystem.get();
}

PlasticitySystem* BrainOrchestrator::getPlasticitySystem() {
    return plasticitySystem.get();
}

DevelopmentSystem* BrainOrchestrator::getDevelopmentSystem() {
    return developmentSystem.get();
}

CheckpointSystem* BrainOrchestrator::getCheckpointSystem() {
    return checkpointSystem.get();
}

void BrainOrchestrator::addSystem(std::unique_ptr<SpecializedBrainSystem> system) {
    systems.push_back(std::move(system));
    organizeSystemsByPriority();
}

void BrainOrchestrator::removeSystem(const std::string& name) {
    systems.erase(
        std::remove_if(systems.begin(), systems.end(),
                      [&name](const std::unique_ptr<SpecializedBrainSystem>& sys) {
                          return sys->getName() == name;
                      }),
        systems.end()
    );
}

size_t BrainOrchestrator::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : brainRef->getRegions()) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t BrainOrchestrator::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : brainRef->getRegions()) {
        total += region->getSynapseCount();
    }
    return total;
}

size_t BrainOrchestrator::getTotalSpikeCount() const {
    return brainRef->getTotalSpikeCount();
}

float BrainOrchestrator::getExcitationInhibitionRatio() const {
    return brainRef->getExcitationInhibitionRatio();
}

void BrainOrchestrator::setSystemHandler(SystemHandler* handler) {
    this->systemHandler = handler;
}

bool BrainOrchestrator::handleSystemUpdates(Brain* brain, SimulationStep step, Timestamp time) {
    if (systemHandler) {
        return systemHandler->handle(brain, step, time);
    }
    return false;
}

void BrainOrchestrator::organizeSystemsByPriority() {
    // Sort systems by priority (Critical first, then High, Medium, Low)
    std::sort(systems.begin(), systems.end(),
              [](const std::unique_ptr<SpecializedBrainSystem>& a,
                 const std::unique_ptr<SpecializedBrainSystem>& b) {
                  return static_cast<int>(a->getPriority()) > static_cast<int>(b->getPriority());
              });
}

// ========== BRAIN FACADE ==========

void BrainFacade::processInput(const class SensoryInput& input) {
    // Delegate to the brain's receiveSensoryInput method
    // This would require access to the brain instance
    // In a real implementation, this would use a singleton or dependency injection
}

void BrainFacade::generateOutput() {
    // Delegate to the brain's produceAction method
    // Similar to above, would need brain instance access
}

} // namespace nlm
