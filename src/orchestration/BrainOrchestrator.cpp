#include "BrainOrchestrator.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <algorithm>
#include <numeric>

namespace nlm {

struct BrainOrchestrator::Impl {
    std::shared_ptr<Brain> brain;
    
    // System orchestration references (cached for performance)
    NeuralWorkingMemory* workingMemory;
    NeuralEpisodicMemory* episodicMemory;
    NeuralAssociativeMemory* associativeMemory;
    PredictionSystem* predictionSystem;
    NeuralPlanner* planner;
    ConceptFormation* conceptFormation;
    AttentionalSelection* attention;
    DevelopmentSystem* developmentSystem;
    Dopamine* dopamine;
    Curiosity* curiosity;
    Novelty* novelty;
    PredictionError* predictionError;
    SpikeSystem* spikeSystem;
    STDP* stdp;
    Hebbian* hebbian;
    StructuralPlasticity* structuralPlasticity;
    
    // Performance tracking
    std::vector<float> stepTimes;
    size_t stepsRecorded;
    
    // Development state tracking
    size_t stepsSinceLastEpisode;
    DevelopmentalStage currentDevelopmentalStage;
    
    Impl(std::shared_ptr<Brain> brainPtr)
        : brain(brainPtr)
        , workingMemory(nullptr)
        , episodicMemory(nullptr)
        , associativeMemory(nullptr)
        , predictionSystem(nullptr)
        , planner(nullptr)
        , conceptFormation(nullptr)
        , attention(nullptr)
        , developmentSystem(nullptr)
        , dopamine(nullptr)
        , curiosity(nullptr)
        , novelty(nullptr)
        , predictionError(nullptr)
        , spikeSystem(nullptr)
        , stdp(nullptr)
        , hebbian(nullptr)
        , structuralPlasticity(nullptr)
        , stepsRecorded(0)
        , stepsSinceLastEpisode(0)
        , currentDevelopmentalStage(DevelopmentalStage::Initial)
    {
        // Cache system references for performance
        if (brain) {
            workingMemory = brain->getWorkingMemory();
            episodicMemory = brain->getEpisodicMemory();
            associativeMemory = brain->getAssociativeMemory();
            predictionSystem = brain->getPredictionSystem();
            planner = brain->getPlanner();
            conceptFormation = brain->getConceptFormation();
            attention = brain->getAttention();
            developmentSystem = brain->getDevelopmentSystem();
            dopamine = brain->getDopamine();
            curiosity = brain->getCuriosity();
            novelty = brain->getNovelty();
            predictionError = brain->getPredictionErrorSignal();
            spikeSystem = brain->getSpikeSystem();
            stdp = brain->getSTDP();
            hebbian = brain->getHebbian();
            structuralPlasticity = brain->getStructuralPlasticity();
        }
        
        stepTimes.reserve(1000);  // Reserve space for efficiency
    }
};

BrainOrchestrator::BrainOrchestrator(std::shared_ptr<Brain> brain)
    : pImpl(std::make_unique<Impl>(brain))
    , brain_(std::move(brain))
    , replayInterval_(100)
    , consolidationInterval_(1000)
    , averageStepTime_(0.0f)
    , totalStepsExecuted_(0)
    , averageSpikesPerStep_(0.0f)
{
    NLM_LOG_INFO("BrainOrchestrator created for brain with " + 
                 std::to_string(brain_->getTotalNeuronCount()) + " neurons");
}

BrainOrchestrator::~BrainOrchestrator() {
    NLM_LOG_INFO("BrainOrchestrator destroyed");
}

BrainOrchestrator::BrainOrchestrator(BrainOrchestrator&& other) noexcept
    : pImpl(std::move(other.pImpl))
    , brain_(std::move(other.brain_))
    , replayInterval_(other.replayInterval_)
    , consolidationInterval_(other.consolidationInterval_)
    , averageStepTime_(other.averageStepTime_)
    , totalStepsExecuted_(other.totalStepsExecuted_)
    , averageSpikesPerStep_(other.averageSpikesPerStep_)
    , preStepHooks_(std::move(other.preStepHooks_))
    , postStepHooks_(std::move(other.postStepHooks_))
    , systemHooks_(std::move(other.systemHooks_))
{
}

BrainOrchestrator& BrainOrchestrator::operator=(BrainOrchestrator&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        brain_ = std::move(other.brain_);
        replayInterval_ = other.replayInterval_;
        consolidationInterval_ = other.consolidationInterval_;
        averageStepTime_ = other.averageStepTime_;
        totalStepsExecuted_ = other.totalStepsExecuted_;
        averageSpikesPerStep_ = other.averageSpikesPerStep_;
        preStepHooks_ = std::move(other.preStepHooks_);
        postStepHooks_ = std::move(other.postStepHooks_);
        systemHooks_ = std::move(other.systemHooks_);
    }
    return *this;
}

bool BrainOrchestrator::initialize() {
    NLM_LOG_INFO("Initializing BrainOrchestrator...");
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize BrainOrchestrator: brain_ is null");
        return false;
    }
    
    // Re-initializing must rebuild, not append: a previous run left
    // reset()+initialize() reporting 1000 neurons for a 500-neuron brain.
    pImpl->stepsSinceLastEpisode = 0;
    pImpl->currentDevelopmentalStage = DevelopmentalStage::Initial;
    
    // Cache system references again after potential re-init
    pImpl->workingMemory = brain_->getWorkingMemory();
    pImpl->episodicMemory = brain_->getEpisodicMemory();
    pImpl->associativeMemory = brain_->getAssociativeMemory();
    pImpl->predictionSystem = brain_->getPredictionSystem();
    pImpl->planner = brain_->getPlanner();
    pImpl->conceptFormation = brain_->getConceptFormation();
    pImpl->attention = brain_->getAttention();
    pImpl->developmentSystem = brain_->getDevelopmentSystem();
    pImpl->dopamine = brain_->getDopamine();
    pImpl->curiosity = brain_->getCuriosity();
    pImpl->novelty = brain_->getNovelty();
    pImpl->predictionError = brain_->getPredictionErrorSignal();
    pImpl->spikeSystem = brain_->getSpikeSystem();
    pImpl->stdp = brain_->getSTDP();
    pImpl->hebbian = brain_->getHebbian();
    pImpl->structuralPlasticity = brain_->getStructuralPlasticity();
    
    NLM_LOG_INFO("BrainOrchestrator initialization complete");
    return true;
}

void BrainOrchestrator::executeStep(SimulationStep currentStep, Timestamp currentTime) {
    auto startTime = std::chrono::steady_clock::now();
    
    // Execute pre-step hooks
    for (const auto& hook : preStepHooks_) {
        hook(currentStep, currentTime);
    }
    
    // Execute main orchestration steps
    orchestrateMemorySystems(currentStep);
    orchestratePredictionSystem(currentStep, currentTime);
    orchestrateCognitionSystems(currentStep, currentTime);
    orchestrateNeuromodulation(currentStep, currentTime);
    orchestratePlasticity(currentStep, 1.0f);  // Plasticity factor calculated in orchestrateNeuromodulation
    orchestrateReplayAndConsolidation(currentStep);
    orchestrateDevelopment(currentStep, currentTime);
    
    // Execute post-step hooks
    for (const auto& hook : postStepHooks_) {
        hook(currentStep, currentTime);
    }
    
    // Update performance metrics
    auto endTime = std::chrono::steady_clock::now();
    float stepDuration = std::chrono::duration<float, std::chrono::milliseconds::period>(endTime - startTime).count();
    
    updatePerformanceMetrics(stepDuration);
    
    // Record step for statistics
    totalStepsExecuted_++;
    
    // Log every 1000 steps
    if (totalStepsExecuted_ % 1000 == 0) {
        logSystemStatus();
    }
}

void BrainOrchestrator::orchestrateMemorySystems(SimulationStep currentStep) {
    // Update working memory (maintenance and competition)
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(brain_->getConfig()->getOr<double>("simulation_timestep", 0.001));
        NLM_LOG_DEBUG("Working memory updated");
    }
    
    // Update episodic memory storage
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons (simplified version)
            // In real implementation, would capture more detailed state
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
            NLM_LOG_DEBUG("Episodic memory updated with new episode");
        }
    }
    
    // Update associative memory (Hebbian learning)
    if (pImpl->associativeMemory) {
        // Would update with current neural patterns
        // This is a simplified version - real implementation would be more complex
        NLM_LOG_DEBUG("Associative memory updated");
    }
    
    // Trigger system hooks
    for (const auto& hook : systemHooks_) {
        hook("MemorySystems", currentStep);
    }
}

void BrainOrchestrator::orchestratePredictionSystem(SimulationStep currentStep, Timestamp currentTime) {
    // Update prediction system
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
        NLM_LOG_DEBUG("Prediction system updated");
    }
    
    // Trigger system hooks
    for (const auto& hook : systemHooks_) {
        hook("PredictionSystem", currentStep);
    }
}

void BrainOrchestrator::orchestrateCognitionSystems(SimulationStep currentStep, Timestamp currentTime) {
    // Update attention system
    if (pImpl->attention) {
        pImpl->attention->update(brain_->getConfig()->getOr<double>("simulation_timestep", 0.001));
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
        NLM_LOG_DEBUG("Attention system updated");
    }
    
    // Update concept formation (simplified)
    if (pImpl->conceptFormation) {
        NLM_LOG_DEBUG("Concept formation updated");
    }
    
    // Update neural planner (simplified)
    if (pImpl->planner) {
        NLM_LOG_DEBUG("Neural planner updated");
    }
    
    // Trigger system hooks
    for (const auto& hook : systemHooks_) {
        hook("CognitionSystems", currentStep);
    }
}

void BrainOrchestrator::orchestrateNeuromodulation(SimulationStep currentStep, Timestamp currentTime) {
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(brain_->getConfig()->getOr<double>("simulation_timestep", 0.001));
        NLM_LOG_DEBUG("Novelty detection updated");
    }
    
    // Update curiosity
    float noveltyLevel = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
    float predictionError = pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f;
    
    if (pImpl->curiosity) {
        pImpl->curiosity->update(noveltyLevel, predictionError, 
                                brain_->getConfig()->getOr<double>("simulation_timestep", 0.001));
        NLM_LOG_DEBUG("Curiosity updated");
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(brain_->getConfig()->getOr<double>("simulation_timestep", 0.001));
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : brain_->getRegions()) {
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
        NLM_LOG_DEBUG("Dopamine updated and applied to neural excitability");
    }
    
    // Calculate plasticity factor for plasticity step
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    // Trigger system hooks
    for (const auto& hook : systemHooks_) {
        hook("Neuromodulation", currentStep);
    }
    
    // Return plasticity factor for next step
    orchestratePlasticity(currentStep, plasticityMod);
}

void BrainOrchestrator::orchestrateDevelopment(SimulationStep currentStep, Timestamp currentTime) {
    // Check developmental stage progression
    size_t currentDevelopmentStep = currentStep % 3000;  // Development stages over 3000 steps
    DevelopmentalStage newStage = DevelopmentalStage::Initial;
    
    if (currentDevelopmentStep < 1000) {
        newStage = DevelopmentalStage::Initial;
    } else if (currentDevelopmentStep < 2000) {
        newStage = DevelopmentalStage::CriticalPeriod;
    } else if (currentDevelopmentStep < 3000) {
        newStage = DevelopmentalStage::Maturation;
    } else {
        newStage = DevelopmentalStage::Adult;
    }
    
    if (newStage != pImpl->currentDevelopmentalStage) {
        pImpl->currentDevelopmentalStage = newStage;
        NLM_LOG_INFO("Developmental stage changed to: " + std::to_string(static_cast<int>(newStage)));
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity.get();
        if (sp) {
            float plasticityMod = 1.0f;
            
            switch (pImpl->currentDevelopmentalStage) {
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
            NLM_LOG_DEBUG("Development system updated plasticity rates for new stage");
        }
    }
    
    // Check if development system should update
    if (currentStep % 1000 == 0 && pImpl->developmentSystem) {
        pImpl->developmentSystem->update(brain_.get(), *brain_->getRandomGenerator(), 
                                        brain_->getConfig()->getOr<double>("simulation_timestep", 0.001) * 1000);
        NLM_LOG_DEBUG("Development system updated");
    }
    
    // Trigger system hooks
    for (const auto& hook : systemHooks_) {
        hook("Development", currentStep);
    }
}

void BrainOrchestrator::orchestratePlasticity(SimulationStep currentStep, float plasticityMod) {
    // Plasticity is already applied during the main step by Brain class
    // This is kept for API compatibility and to coordinate with development system
    if (currentStep % 100 == 0 && pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(brain_.get(), *brain_->getRandomGenerator());
        NLM_LOG_DEBUG("Structural plasticity updated");
    }
    
    // Apply homeostatic synaptic scaling every 100 steps
    if (currentStep > 0 && currentStep % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        for (auto& region : brain_->getRegions()) {
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
        NLM_LOG_DEBUG("Homeostatic synaptic scaling applied");
    }
    
    // Trigger system hooks
    for (const auto& hook : systemHooks_) {
        hook("Plasticity", currentStep);
    }
}

void BrainOrchestrator::orchestrateReplayAndConsolidation(SimulationStep currentStep) {
    // Replay important memories
    if (currentStep % replayInterval_ == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
        NLM_LOG_DEBUG("Memory replay performed");
    }
    
    // Periodic memory consolidation
    if (currentStep % consolidationInterval_ == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
        NLM_LOG_DEBUG("Memory consolidation performed");
    }
    
    // Trigger system hooks
    for (const auto& hook : systemHooks_) {
        hook("ReplayConsolidation", currentStep);
    }
}

void BrainOrchestrator::updatePerformanceMetrics(float stepDuration) {
    // Add to rolling window of step times
    pImpl->stepTimes.push_back(stepDuration);
    if (pImpl->stepTimes.size() > 100) {
        pImpl->stepTimes.erase(pImpl->stepTimes.begin());
    }
    
    // Calculate average step time
    if (!pImpl->stepTimes.empty()) {
        float sum = std::accumulate(pImpl->stepTimes.begin(), pImpl->stepTimes.end(), 0.0f);
        averageStepTime_ = sum / static_cast<float>(pImpl->stepTimes.size());
    }
    
    // Update average spikes per step
    if (brain_) {
        float totalSpikesThisStep = brain_->getTotalSpikeCount();
        averageSpikesPerStep_ = static_cast<float>(totalSpikesThisStep) / std::max(1.0f, static_cast<float>(totalStepsExecuted_));
    }
}

void BrainOrchestrator::logSystemStatus() const {
    if (!brain_) return;
    
    NLM_LOG_INFO("=== BrainOrchestrator Status ===");
    NLM_LOG_INFO("Step count: " + std::to_string(totalStepsExecuted_));
    NLM_LOG_INFO("Average step time: " + std::to_string(averageStepTime_) + " ms");
    NLM_LOG_INFO("Average spikes per step: " + std::to_string(averageSpikesPerStep_));
    NLM_LOG_INFO("Replay interval: " + std::to_string(replayInterval_) + " steps");
    NLM_LOG_INFO("Consolidation interval: " + std::to_string(consolidationInterval_) + " steps");
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain_->getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain_->getTotalSynapseCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(brain_->getPendingSpikeEventCount()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(brain_->getExcitationInhibitionRatio()));
    NLM_LOG_INFO("==============================");
}

void BrainOrchestrator::resetMetrics() {
    averageStepTime_ = 0.0f;
    totalStepsExecuted_ = 0;
    averageSpikesPerStep_ = 0.0f;
    pImpl->stepTimes.clear();
    NLM_LOG_INFO("BrainOrchestrator metrics reset");
}

} // namespace nlm
