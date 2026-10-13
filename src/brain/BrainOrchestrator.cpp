#include "BrainOrchestrator.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>

namespace nlm {

BrainOrchestrator::BrainOrchestrator(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
    , neuralController_(brain_.get())
    , memoryController_(brain_.get())
    , neuromodController_(brain_.get())
    , plasticityController_(brain_.get())
    , cognitionController_(brain_.get())
    , developmentController_(brain_.get())
    , checkpointController_(brain_.get()) {
    startTime_ = std::chrono::high_resolution_clock::now();
    NLM_LOG_INFO("BrainOrchestrator initialized");
}

BrainOrchestrator::~BrainOrchestrator() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime_).count();
    NLM_LOG_INFO("BrainOrchestrator destroyed after " + std::to_string(duration) + "s");
}

BrainOrchestrator::BrainOrchestrator(BrainOrchestrator&& other) noexcept
    : brain_(std::move(other.brain_))
    , neuralController_(other.neuralController_.brain_)
    , memoryController_(other.memoryController_.brain_)
    , neuromodController_(other.neuromController_.brain_)
    , plasticityController_(other.plasticityController_.brain_)
    , cognitionController_(other.cognitionController_.brain_)
    , developmentController_(other.developmentController_.brain_)
    , checkpointController_(other.checkpointController_.brain_)
    , stats_(std::move(other.stats_)) {
    other.brain_ = nullptr;
}

BrainOrchestrator& BrainOrchestrator::operator=(BrainOrchestrator&& other) noexcept {
    if (this != &other) {
        brain_ = std::move(other.brain_);
        neuralController_ = std::move(other.neuralController_);
        memoryController_ = std::move(other.memoryController_);
        neuromodController_ = std::move(other.neuromController_);
        plasticityController_ = std::move(other.plasticityController_);
        cognitionController_ = std::move(other.cognitionController_);
        developmentController_ = std::move(other.developmentController_);
        checkpointController_ = std::move(other.checkpointController_);
        stats_ = std::move(other.stats_);
    }
    return *this;
}

void BrainOrchestrator::executeStep(SimulationStep step, Timestamp time) {
    auto stepStart = std::chrono::high_resolution_clock::now();
    
    // Execute neural computation (Phase 1: Basic spiking dynamics)
    neuralController_.processDelayedSpikes(step, time);
    neuralController_.updateAllNeurons(time, 0.001); // Use default timestep
    neuralController_.detectSpikesAndScheduleEvents(step, time);
    
    // Execute integrated systems (Phase 6: All systems working together)
    memoryController_.updateWorkingMemory(0.001);
    neuromodController_.updateAllNeuromodulators(0.001);
    memoryController_.updateEpisodicMemory(step, time);
    plasticityController_.applyAllPlasticityRules(0.001);
    cognitionController_.updateAllCognitionSystems(0.001);
    developmentController_.updateDevelopmentSystem(step, time, time);
    memoryController_.applyReplay(step);
    memoryController_.applyConsolidation(step);
    checkpointController_.updateCheckpointManager(step, time);
    
    // Update statistics
    stats_.neuralStepsCompleted += 3;
    stats_.memoryUpdatesCompleted += 1;
    stats_.neuromodulationUpdatesCompleted += 1;
    stats_.plasticityUpdatesCompleted += 1;
    stats_.cognitionUpdatesCompleted += 1;
    stats_.developmentUpdatesCompleted += 1;
    stats_.checkpointUpdatesCompleted += 1;
    stats_.totalSpikesThisStep = brain_->getTotalSpikeCount();
    
    auto stepEnd = std::chrono::high_resolution_clock::now();
    auto stepDuration = std::chrono::duration<double>(stepEnd - stepStart).count();
    stats_.orchestrationTime += stepDuration;
    stats_.stepTime = time;
    
    NLM_LOG_DEBUG("Orchestrated step " + std::to_string(step) + " in " + 
                  std::to_string(stepDuration * 1000) + "ms, " +
                  std::to_string(stats_.totalSpikesThisStep) + " spikes");
}

// NeuralStepController implementation
BrainOrchestrator::NeuralStepController::NeuralStepController(Brain* brain) : brain_(brain) {}

void BrainOrchestrator::NeuralStepController::processDelayedSpikes(SimulationStep step, Timestamp time) {
    auto* spikeSystem = brain_->getSpikeSystem();
    if (spikeSystem) {
        spikeSystem->processDelayedSpikes(step, time);
    }
}

void BrainOrchestrator::NeuralStepController::updateAllNeurons(Timestamp time, TimestepDuration dt) {
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(time, dt);
            }
        }
    }
}

void BrainOrchestrator::NeuralStepController::detectSpikesAndScheduleEvents(SimulationStep step, Timestamp time) {
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                if (neuron->isFiring()) {
                    auto* spikeSystem = brain_->getSpikeSystem();
                    if (spikeSystem) {
                        spikeSystem->queueSpike(SpikeEvent(neuron->getId(), time, step));
                    }
                }
            }
        }
    }
    
    auto* spikeSystem = brain_->getSpikeSystem();
    if (spikeSystem) {
        spikeSystem->processSpikes(step);
    }
}

// MemoryIntegrationController implementation
BrainOrchestrator::MemoryIntegrationController::MemoryIntegrationController(Brain* brain) : brain_(brain) {}

void BrainOrchestrator::MemoryIntegrationController::updateWorkingMemory(TimestepDuration dt) {
    auto* workingMemory = brain_->getWorkingMemory();
    if (workingMemory) {
        workingMemory->update(dt);
    }
}

void BrainOrchestrator::MemoryIntegrationController::updateEpisodicMemory(SimulationStep step, Timestamp time) {
    auto* episodicMemory = brain_->getEpisodicMemory();
    if (episodicMemory) {
        // Store episode every 10 steps
        if (step % 10 == 0) {
            brain_->storeCurrentStateAsEpisode();
        }
    }
}

void BrainOrchestrator::MemoryIntegrationController::applyReplay(SimulationStep step) {
    auto* episodicMemory = brain_->getEpisodicMemory();
    if (episodicMemory && step % 100 == 0) {
        episodicMemory->replayImportantEpisodes();
    }
}

void BrainOrchestrator::MemoryIntegrationController::applyConsolidation(SimulationStep step) {
    auto* episodicMemory = brain_->getEpisodicMemory();
    if (episodicMemory && step % 1000 == 0) {
        episodicMemory->consolidate(0.3f);
    }
}

// NeuromodulationController implementation
BrainOrchestrator::NeuromodulationController::NeuromodulationController(Brain* brain) : brain_(brain) {}

void BrainOrchestrator::NeuromodulationController::updateAllNeuromodulators(TimestepDuration dt) {
    auto* dopamine = brain_->getDopamine();
    auto* curiosity = brain_->getCuriosity();
    auto* novelty = brain_->getNovelty();
    auto* predictionError = brain_->getPredictionErrorSignal();
    
    if (dopamine) dopamine->update(dt);
    if (curiosity) curiosity->update(dt);
    if (novelty) novelty->update(dt);
    if (predictionError) predictionError->update(dt);
}

float BrainOrchestrator::NeuromodulationController::getTotalNeuromodulationLevel() const {
    float total = 0.0f;
    auto* dopamine = brain_->getDopamine();
    auto* curiosity = brain_->getCuriosity();
    auto* novelty = brain_->getNovelty();
    
    if (dopamine) total += dopamine->getLevel();
    if (curiosity) total += curiosity->getLevel();
    if (novelty) total += novelty->getLevel();
    
    return total;
}

// PlasticityController implementation
BrainOrchestrator::PlasticityController::PlasticityController(Brain* brain) : brain_(brain) {}

void BrainOrchestrator::PlasticityController::applyAllPlasticityRules(TimestepDuration dt) {
    float plasticityMod = 1.0f;
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        plasticityMod = dopamine->getPlasticityFactor();
    }
    
    for (auto& region : brain_->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().stdp) {
                brain_->getSTDP()->update(syn, syn->getPreSpikeHistory(), 
                                         syn->getPostSpikeHistory(), dt);
            }
            
            if (syn->getPlasticityFlags().hebbian) {
                brain_->getHebbian()->update(syn, syn->getPreSpikeHistory(),
                                            syn->getPostSpikeHistory(), dt);
            }
            
            syn->step(dt); // Update synapse state
        }
    }
}

float BrainOrchestrator::PlasticityController::getPlasticityModulationFactor() const {
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        return dopamine->getPlasticityFactor();
    }
    return 1.0f;
}

// CognitionController implementation
BrainOrchestrator::CognitionController::CognitionController(Brain* brain) : brain_(brain) {}

void BrainOrchestrator::CognitionController::updateAllCognitionSystems(TimestepDuration dt) {
    auto* planner = brain_->getPlanner();
    auto* conceptFormation = brain_->getConceptFormation();
    auto* attention = brain_->getAttention();
    
    if (planner) planner->update(dt);
    if (conceptFormation) conceptFormation->update(dt);
    if (attention) attention->update(dt);
}

// DevelopmentController implementation
BrainOrchestrator::DevelopmentController::DevelopmentController(Brain* brain) : brain_(brain) {}

void BrainOrchestrator::DevelopmentController::updateDevelopmentSystem(
    SimulationStep step, Timestamp time, Timestamp totalTime) {
    auto* developmentSystem = brain_->getDevelopmentSystem();
    if (developmentSystem && step % 1000 == 0) {
        developmentSystem->update(brain_.get(), *brain_->getRandomGenerator(), 
                                totalTime * 0.001f); // Convert to seconds
    }
}

// CheckpointController implementation
BrainOrchestrator::CheckpointController::CheckpointController(Brain* brain) : brain_(brain) {}

void BrainOrchestrator::CheckpointController::updateCheckpointManager(
    SimulationStep step, Timestamp time) {
    auto* checkpointManager = brain_->getCheckpointManager();
    if (checkpointManager) {
        checkpointManager->update(step, time);
    }
}

} // namespace nlm