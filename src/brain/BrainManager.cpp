//=============================================================
// BRAIN MANAGER - Brain.cpp Refactoring (Phase 7)
// Single Responsibility: Integrated Systems Management
//=============================================================

#include "BrainManager.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <algorithm>

namespace nlm {

// BRAIN MANAGER IMPLEMENTATION
// Focuses on managing all integrated systems (memory, prediction, cognition, neuromodulation)

BrainManager::BrainManager() : m_coreBrain(nullptr) 
    , m_workingMemory(nullptr) 
    , m_episodicMemory(nullptr) 
    , m_associativeMemory(nullptr)
    , m_predictionSystem(nullptr)
    , m_planner(nullptr)
    , m_conceptFormation(nullptr)
    , m_attention(nullptr)
    , m_developmentSystem(nullptr)
    , m_dopamine(nullptr)
    , m_curiosity(nullptr)
    , m_novelty(nullptr)
    , m_predictionError(nullptr)
    , m_spikeSystem(nullptr)
    , m_stdp(nullptr)
    , m_hebbian(nullptr)
    , m_structuralPlasticity(nullptr)
{
    // Initialize all pointers to nullptr
}

BrainManager::~BrainManager() = default;

void BrainManager::initialize(BrainCore* coreBrain) {
    // Store reference to core brain
    m_coreBrain = coreBrain;
    
    // Initialize memory systems
    if (m_coreBrain->getWorkingMemory()) {
        m_workingMemory = m_coreBrain->getWorkingMemory();
    }
    
    if (m_coreBrain->getEpisodicMemory()) {
        m_episodicMemory = m_coreBrain->getEpisodicMemory();
    }
    
    if (m_coreBrain->getAssociativeMemory()) {
        m_associativeMemory = m_coreBrain->getAssociativeMemory();
    }
    
    // Initialize prediction system
    if (m_coreBrain->getPredictionSystem()) {
        m_predictionSystem = m_coreBrain->getPredictionSystem();
    }
    
    // Initialize cognition systems
    if (m_coreBrain->getPlanner()) {
        m_planner = m_coreBrain->getPlanner();
    }
    
    if (m_coreBrain->getConceptFormation()) {
        m_conceptFormation = m_coreBrain->getConceptFormation();
    }
    
    if (m_coreBrain->getAttention()) {
        m_attention = m_coreBrain->getAttention();
    }
    
    // Initialize development system
    if (m_coreBrain->getDevelopmentSystem()) {
        m_developmentSystem = m_coreBrain->getDevelopmentSystem();
    }
    
    // Initialize neuromodulation systems
    if (m_coreBrain->getDopamine()) {
        m_dopamine = m_coreBrain->getDopamine();
    }
    
    if (m_coreBrain->getCuriosity()) {
        m_curiosity = m_coreBrain->getCuriosity();
    }
    
    if (m_coreBrain->getNovelty()) {
        m_novelty = m_coreBrain->getNovelty();
    }
    
    if (m_coreBrain->getPredictionErrorSignal()) {
        m_predictionError = m_coreBrain->getPredictionErrorSignal();
    }
    
    // Initialize plasticity systems
    if (m_coreBrain->getSTDP()) {
        m_stdp = m_coreBrain->getSTDP();
    }
    
    if (m_coreBrain->getHebbian()) {
        m_hebbian = m_coreBrain->getHebbian();
    }
    
    if (m_coreBrain->getStructuralPlasticity()) {
        m_structuralPlasticity = m_coreBrain->getStructuralPlasticity();
    }
    
    // Initialize spike system
    if (m_coreBrain->getSpikeSystem()) {
        m_spikeSystem = m_coreBrain->getSpikeSystem();
    }
    
    NLM_LOG_INFO("Brain Manager initialized (Phase 7 - Refactored)");
}

void BrainManager::executeStep(SimulationStep currentStep, Timestamp currentTime) {
    this->currentStep = currentStep;
    this->currentTime = currentTime;
    
    // STEP 1: Update working memory (maintenance and competition)
    updateWorkingMemory();
    
    // STEP 2: Update neuromodulation effects
    updateNeuromodulation();
    
    // STEP 3: Update plasticity rules (STDP and Hebbian)
    updatePlasticity();
    
    // STEP 4: Update episodic memory with current experience
    updateEpisodicMemory();
    
    // STEP 5: Update prediction system
    updatePredictionSystem();
    
    // STEP 6: Update attention system
    updateAttention();
    
    // STEP 7: Update concept formation
    updateConceptFormation();
    
    // STEP 8: Apply structural plasticity periodically
    applyStructuralPlasticity();
    
    // STEP 9: Replay important memories
    replayMemories();
    
    // STEP 10: Apply development effects
    applyDevelopment();
    
    // STEP 11: Periodic memory consolidation
    consolidateMemories();
    
    // STEP 12: Update checkpoint system
    updateCheckpoints();
}

void BrainManager::updateWorkingMemory() {
    if (m_workingMemory) {
        m_workingMemory->update(0.001); // Default timestep
    }
}

void BrainManager::updateNeuromodulation() {
    // Update novelty detection
    if (m_novelty) {
        m_novelty->update(0.001);
    }
    
    // Update curiosity
    if (m_curiosity) {
        m_curiosity->update(0.001);
    }
    
    // Update dopamine (reward prediction error)
    if (m_dopamine) {
        m_dopamine->update(0.001);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = m_dopamine->getLevel();
        for (auto& region : m_coreBrain->getRegions()) {
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
}

void BrainManager::updatePlasticity() {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (m_dopamine) {
        plasticityMod = m_dopamine->getPlasticityFactor();
    }
    
    for (auto& region : m_coreBrain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Update STDP
                    if (m_stdp) {
                        m_stdp->update(&syn, preSpikes, postSpikes, 0.001);
                    }
                    
                    // Apply neuromodulation factor
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
                    if (m_hebbian) {
                        m_hebbian->update(&syn, preSpikes, postSpikes, 0.001);
                    }
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void BrainManager::updateEpisodicMemory() {
    if (!m_episodicMemory) return;
    
    // Store episode every 10 steps
    static size_t stepsSinceLastEpisode = 0;
    stepsSinceLastEpisode++;
    
    if (stepsSinceLastEpisode >= 10) {
        stepsSinceLastEpisode = 0;
        
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = m_dopamine ? m_dopamine->getLevel() : 0.0f;
        
        // Store active neurons
        for (auto& region : m_coreBrain->getRegions()) {
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
        
        m_episodicMemory->storeEpisode(episode);
    }
}

void BrainManager::updatePredictionSystem() {
    if (m_predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void BrainManager::updateAttention() {
    if (m_attention) {
        m_attention->update(0.001);
        
        // Apply attention to working memory winners
        if (m_workingMemory && !m_workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = m_workingMemory->getMemoryNeurons();
            m_attention->processCompetition(competitors);
        }
    }
}

void BrainManager::updateConceptFormation() {
    if (m_conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void BrainManager::applyStructuralPlasticity() {
    // Apply structural plasticity every 100 steps
    if (currentStep % 100 == 0 && m_structuralPlasticity) {
        m_structuralPlasticity->update(m_coreBrain, *m_coreBrain->getRandomGenerator());
    }
}

void BrainManager::replayMemories() {
    if (!m_episodicMemory) return;
    
    // Replay important memories every 100 steps
    if (currentStep % 100 == 0) {
        // Get episodes for replay
        auto episodesToReplay = m_episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            m_episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainManager::applyDevelopment() {
    // Apply development effects every 1000 steps
    if (currentStep % 1000 == 0 && m_developmentSystem) {
        m_developmentSystem->update(m_coreBrain, *m_coreBrain->getRandomGenerator(), 1.0);
        
        // Development affects plasticity rates
        if (m_structuralPlasticity) {
            DevelopmentStage stage = m_coreBrain->getDevelopmentalStage();
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
            
            m_structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityMod);
            m_structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void BrainManager::consolidateMemories() {
    if (!m_episodicMemory) return;
    
    // Periodic memory consolidation every 1000 steps
    if (currentStep % 1000 == 0) {
        // Consolidate important memories, remove weak ones
        m_episodicMemory->consolidate(0.3f);
    }
}

void BrainManager::updateCheckpoints() {
    // Update checkpoint system
    if (m_coreBrain->getCheckpointManager()) {
        m_coreBrain->getCheckpointManager()->update(currentStep, currentTime);
    }
}

// Getters for integrated systems
NeuralWorkingMemory* BrainManager::getWorkingMemory() {
    return m_workingMemory;
}

NeuralEpisodicMemory* BrainManager::getEpisodicMemory() {
    return m_episodicMemory;
}

NeuralAssociativeMemory* BrainManager::getAssociativeMemory() {
    return m_associativeMemory;
}

PredictionSystem* BrainManager::getPredictionSystem() {
    return m_predictionSystem;
}

NeuralPlanner* BrainManager::getPlanner() {
    return m_planner;
}

ConceptFormation* BrainManager::getConceptFormation() {
    return m_conceptFormation;
}

AttentionalSelection* BrainManager::getAttention() {
    return m_attention;
}

DevelopmentSystem* BrainManager::getDevelopmentSystem() {
    return m_developmentSystem;
}

Dopamine* BrainManager::getDopamine() {
    return m_dopamine;
}

Curiosity* BrainManager::getCuriosity() {
    return m_curiosity;
}

Novelty* BrainManager::getNovelty() {
    return m_novelty;
}

PredictionError* BrainManager::getPredictionErrorSignal() {
    return m_predictionError;
}

SpikeSystem* BrainManager::getSpikeSystem() {
    return m_spikeSystem;
}

STDP* BrainManager::getSTDP() {
    return m_stdp;
}

Hebbian* BrainManager::getHebbian() {
    return m_hebbian;
}

StructuralPlasticity* BrainManager::getStructuralPlasticity() {
    return m_structuralPlasticity;
}

size_t BrainManager::getTotalSpikeCount() const {
    return m_coreBrain->getTotalSpikeCount();
}

size_t BrainManager::getPendingSpikeEventCount() const {
    return m_spikeSystem ? 
        m_spikeSystem->getPendingSpikeCount() + m_spikeSystem->getPendingDelayedCount() : 0;
}

std::unique_ptr<class Action> BrainManager::produceAction() {
    return m_coreBrain->produceAction();
}

void BrainManager::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    if (m_stdp) {
        m_stdp->setLTPWeight(0.01f * modulation);
        m_stdp->setLTDWeight(0.012f * modulation);
    }
}

void BrainManager::updatePlasticity() {
    // Plasticity is now applied during each step in executeStep
    // This method is kept for API compatibility
}

void BrainManager::develop() {
    // Development updates structural plasticity
    if (m_structuralPlasticity) {
        m_structuralPlasticity->update(m_coreBrain, *m_coreBrain->getRandomGenerator());
    }
}

void BrainManager::reset() {
    // Reset all integrated systems
    if (m_workingMemory) m_workingMemory->clear();
    if (m_episodicMemory) m_episodicMemory->clear();
    if (m_associativeMemory) m_associativeMemory->clear();
    if (m_attention) m_attention->reset();
    if (m_predictionSystem) m_predictionSystem->reset();
    if (m_planner) m_planner->reset();
    if (m_conceptFormation) m_conceptFormation->reset();
    if (m_developmentSystem) m_developmentSystem->reset();
    if (m_dopamine) m_dopamine->reset();
    if (m_curiosity) m_curiosity->reset();
    if (m_novelty) m_novelty->reset();
    if (m_predictionError) m_predictionError->reset();
}

void BrainManager::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Manager Status (Phase 7 - Refactored) ===");
    
    // Memory system status
    if (m_workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(m_workingMemory->getActiveTraces()));
    }
    if (m_episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(m_episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (m_dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(m_dopamine->getLevel()));
    }
    if (m_curiosity) {
        NLM_LOG_INFO("Curiosity level: " + std::to_string(m_curiosity->getLevel()));
    }
    if (m_novelty) {
        NLM_LOG_INFO("Novelty level: " + std::to_string(m_novelty->getLevel()));
    }
}

} // namespace nlm
