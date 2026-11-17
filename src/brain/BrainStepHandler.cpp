// Brain Step Handler Implementation
// Phase 2: Main simulation step logic

#include "BrainStepHandler.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <algorithm>
#include <numeric>

namespace nlm {

BrainStepHandler::BrainStepHandler() {
    stepState_.stepCompleted = false;
    perfStats_.stepsProcessed = 0;
}

BrainStepHandler::~BrainStepHandler() {
    // Cleanup handled by Brain destructor
}

void BrainStepHandler::step(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    auto stepStartTime = std::chrono::high_resolution_clock::now();
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot execute step with null brain");
        return;
    }
    
    // Validate step parameters
    if (!validateStepExecution(brain, currentStep)) {
        NLM_LOG_WARN("Step validation failed for step " + std::to_string(currentStep));
    }
    
    // Setup step environment
    setupStepEnvironment(brain, currentStep);
    stepState_.currentTime = currentTime;
    stepState_.stepCompleted = false;
    
    try {
        // Process spike events from previous step
        processSpikeEvents(brain);
        
        // Process delayed spikes
        processDelayedSpikes(brain);
        
        // Update plasticity rules based on current activity
        updatePlasticity(brain);
        
        // Update development
        updateDevelopment(brain, stepState_.currentTime);
        
        // Update neuromodulation
        updateNeuromodulation(brain, stepState_.currentTime);
        
        // Update integrated systems
        updateIntegratedSystems(brain, currentStep);
        
        // Reset step state for next iteration
        resetStepState(brain);
        
        stepState_.stepCompleted = true;
        
        // Calculate performance metrics
        calculatePerformanceMetrics(brain);
        
        // Log step statistics
        logStepStats(brain, currentStep);
        
    } catch (const std::exception& e) {
        if (!handleStepErrors(brain, e)) {
            NLM_LOG_ERROR("Step " + std::to_string(currentStep) + " failed: " + std::string(e.what()));
        }
    }
    
    auto stepEndTime = std::chrono::high_resolution_clock::now();
    stepState_.stepTime = std::chrono::duration<double>(stepEndTime - stepStartTime).count();
    perfStats_.totalStepTime += stepState_.stepTime;
    perfStats_.stepsProcessed++;
    
    if (perfStats_.stepsProcessed > 0) {
        perfStats_.averageStepTime = perfStats_.totalStepTime / perfStats_.stepsProcessed;
    }
}

void BrainStepHandler::step(Brain* brain, SimulationStep currentStep) {
    // Derive timestamp from step number
    Timestamp currentTime = calculateCurrentTime(currentStep, 0.001); // Default timestep
    step(brain, currentStep, currentTime);
}

void BrainStepHandler::processSpikeEvents(Brain* brain) {
    if (!brain->pImpl->spikeSystem) return;
    
    // Get all pending spike events
    auto pendingEvents = brain->pImpl->spikeSystem->getPendingEvents();
    stepState_.totalSpikesProcessed = pendingEvents.size();
    
    // Process each spike event
    for (const auto& event : pendingEvents) {
        // Deliver synaptic input to target neuron
        auto it = brain->pImpl->neuronById.find(event.destination_neuron.index());
        if (it != brain->pImpl->neuronById.end()) {
            Neuron* targetNeuron = it->second;
            targetNeuron->injectCurrent(event.weight);
        }
    }
    
    // Clear processed events
    brain->pImpl->spikeSystem->clearPendingEvents();
}

void BrainStepHandler::processDelayedSpikes(Brain* brain) {
    if (!brain->pImpl->spikeSystem) return;
    
    // Get delayed spike events that should fire this step
    auto delayedEvents = brain->pImpl->spikeSystem->getDelayedEvents(stepState_.currentTime);
    
    for (const auto& event : delayedEvents) {
        // Deliver synaptic input
        auto it = brain->pImpl->neuronById.find(event.destination_neuron.index());
        if (it != brain->pImpl->neuronById.end()) {
            Neuron* targetNeuron = it->second;
            targetNeuron->injectCurrent(event.weight);
            
            // Record spike
            targetNeuron->spike(stepState_.currentTime);
            brain->pImpl->totalSpikesThisStep++;
            brain->pImpl->totalSpikesTotal++;
        }
    }
}

void BrainStepHandler::updatePlasticity(Brain* brain) {
    if (!brain->pImpl->spikeSystem) return;
    
    // Update STDP plasticity
    if (brain->pImpl->stdp && shouldUpdatePlasticity(brain->getConfig().get())) {
        brain->pImpl->stdp->update(brain->pImpl->neuronById);
        stepState_.plasticityUpdates++;
    }
    
    // Update Hebbian plasticity
    if (brain->pImpl->hebbian && shouldUpdatePlasticity(brain->getConfig().get())) {
        brain->pImpl->hebbian->update(brain->pImpl->neuronById);
        stepState_.plasticityUpdates++;
    }
    
    // Update structural plasticity
    if (brain->pImpl->structuralPlasticity && shouldUpdatePlasticity(brain->getConfig().get())) {
        brain->pImpl->structuralPlasticity->update(brain->pImpl->neuronById);
        handleStructuralPlasticity(brain);
        stepState_.plasticityUpdates++;
    }
}

void BrainStepHandler::updateDevelopment(Brain* brain, Timestamp timestep) {
    if (!shouldUpdateDevelopment(brain->getConfig().get(), timestep)) return;
    
    // Update developmental stage if needed
    auto currentStage = brain->getDevelopmentalStage();
    auto* developmentSystem = brain->pImpl->developmentSystem.get();
    
    if (developmentSystem) {
        developmentSystem->update(brain, timestep);
        
        // Check if developmental stage has changed
        auto newStage = brain->getDevelopmentalStage();
        if (newStage != currentStage) {
            NLM_LOG_INFO("Developmental stage changed from " + std::to_string(static_cast<int>(currentStage)) + 
                        " to " + std::to_string(static_cast<int>(newStage)));
        }
        
        stepState_.developmentUpdates++;
    }
}

void BrainStepHandler::updateNeuromodulation(Brain* brain, Timestamp timestep) {
    if (!shouldUpdateNeuromodulation(brain->getConfig().get(), timestep)) return;
    
    // Update dopamine system
    if (brain->pImpl->dopamine) {
        brain->pImpl->dopamine->update(brain, timestep);
        stepState_.neuromodulationUpdates++;
    }
    
    // Update curiosity system
    if (brain->pImpl->curiosity) {
        brain->pImpl->curiosity->update(brain, timestep);
        stepState_.neuromodulationUpdates++;
    }
    
    // Update novelty detection
    if (brain->pImpl->novelty) {
        brain->pImpl->novelty->update(brain, timestep);
        stepState_.neuromodulationUpdates++;
    }
    
    // Update prediction error
    if (brain->pImpl->predictionError) {
        brain->pImpl->predictionError->update(brain, timestep);
        stepState_.neuromodulationUpdates++;
    }
}

void BrainStepHandler::updateIntegratedSystems(Brain* brain, SimulationStep currentStep) {
    // Update prediction system
    updatePredictionSystem(brain, currentStep);
    
    // Update memory systems
    updateMemorySystems(brain);
    
    // Update cognition systems
    updateCognitionSystems(brain);
    
    // Update world interface
    updateWorldInterface(brain);
}

void BrainStepHandler::updatePredictionSystem(Brain* brain, SimulationStep currentStep) {
    if (!brain->pImpl->predictionSystem) return;
    
    // Update prediction based on current sensory input
    brain->pImpl->predictionSystem->update(brain, currentStep);
    
    // Calculate prediction error
    float predictionError = brain->pImpl->predictionSystem->getPredictionError();
    
    if (std::abs(predictionError) > 0.01f) {
        NLM_LOG_WARN("High prediction error detected: " + std::to_string(predictionError));
    }
}

void BrainStepHandler::updateMemorySystems(Brain* brain) {
    // Update working memory
    if (brain->pImpl->workingMemory) {
        brain->pImpl->workingMemory->update(brain);
    }
    
    // Update episodic memory
    if (brain->pImpl->episodicMemory) {
        brain->pImpl->episodicMemory->update(brain);
    }
    
    // Update associative memory
    if (brain->pImpl->associativeMemory) {
        brain->pImpl->associativeMemory->update(brain);
    }
}

void BrainStepHandler::updateCognitionSystems(Brain* brain) {
    // Update planner
    if (brain->pImpl->planner) {
        brain->pImpl->planner->update(brain);
    }
    
    // Update concept formation
    if (brain->pImpl->conceptFormation) {
        brain->pImpl->conceptFormation->update(brain);
    }
    
    // Update attentional selection
    if (brain->pImpl->attention) {
        brain->pImpl->attention->update(brain);
    }
}

void BrainStepHandler::updateWorldInterface(Brain* brain) {
    // Process sensory input from environment
    if (brain->pImpl->sensoryNeurons.size() > 0) {
        // Inject sensory input into sensory neurons
        for (Neuron* neuron : brain->pImpl->sensoryNeurons) {
            // Apply external stimuli based on environment
            neuron->injectCurrent(10.0f);  // Example stimulus
        }
    }
    
    // Generate motor output if needed
    if (brain->pImpl->motorNeurons.size() > 0) {
        // The motor system will generate output based on brain state
        // This is handled by the brain's produceAction method
    }
}

void BrainStepHandler::resetStepState(Brain* brain) {
    if (!brain) return;
    
    // Reset step-specific counters
    brain->pImpl->totalSpikesThisStep = 0;
    stepState_.totalSpikesProcessed = 0;
    stepState_.plasticityUpdates = 0;
    stepState_.developmentUpdates = 0;
    stepState_.neuromodulationUpdates = 0;
    stepState_.stepCompleted = false;
    
    // Clear metrics
    stepMetrics_ = StepMetrics();
}

void BrainStepHandler::processSynapticDelays(Brain* brain) {
    // Process synaptic delays
    if (brain->pImpl->spikeSystem) {
        brain->pImpl->spikeSystem->processDelays(stepState_.currentTime);
    }
}

void BrainStepHandler::handleStructuralPlasticity(Brain* brain) {
    // Handle structural plasticity (synaptogenesis/pruning)
    if (brain->pImpl->structuralPlasticity) {
        // Create new synapses if rate allows
        brain->pImpl->structuralPlasticity->createNewSynapses(brain->pImpl->neuronById);
        
        // Prune weak synapses if rate allows
        brain->pImpl->structuralPlasticity->pruneWeakSynapses(brain->pImpl->neuronById);
    }
}

void BrainStepHandler::calculatePerformanceMetrics(Brain* brain) {
    if (!brain) return;
    
    // Calculate average firing rate
    size_t totalNeurons = brain->getTotalNeuronCount();
    if (totalNeurons > 0) {
        stepMetrics_.avgFiringRate = static_cast<float>(brain->getTotalSpikeCount()) / totalNeurons;
    }
    
    // Calculate excitation/inhibition ratio
    stepMetrics_.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
    
    // Track synaptic activity
    stepMetrics_.synapticActivity = static_cast<double>(brain->getTotalSynapseCount());
    
    // Update performance statistics
    perfStats_.spikesProcessedPerStep = brain->getTotalSpikeCount();
}

void BrainStepHandler::logStepStats(Brain* brain, SimulationStep currentStep) const {
    if (!brain) return;
    
    NLM_LOG_INFO("Step " + std::to_string(currentStep) + ": " +
                 std::to_string(stepState_.totalSpikesProcessed) + " spikes processed, " +
                 std::to_string(stepMetrics_.neuronsFired) + " neurons fired, " +
                 std::to_string(stepState_.plasticityUpdates) + " plasticity updates, " +
                 std::to_string(stepState_.developmentUpdates) + " development updates, " +
                 std::to_string(stepState_.neuromodulationUpdates) + " neuromodulation updates, " +
                 std::to_string(stepState_.stepTime) + " seconds");
}

bool BrainStepHandler::validateStepExecution(Brain* brain, SimulationStep currentStep) const {
    if (!brain) return false;
    
    // Check brain integrity
    if (brain->getTotalNeuronCount() == 0) {
        NLM_LOG_ERROR("Brain has no neurons - cannot execute step");
        return false;
    }
    
    // Check for valid step number
    if (currentStep < 0) {
        NLM_LOG_ERROR("Invalid step number: " + std::to_string(currentStep));
        return false;
    }
    
    return true;
}

bool BrainStepHandler::handleStepErrors(Brain* brain, const std::exception& e) {
    NLM_LOG_ERROR("Step error: " + std::string(e.what()));
    
    // Attempt recovery
    recoverFromStepErrors(brain);
    
    // Check if we should continue despite the error
    if (std::string(e.what()).find("recoverable") != std::string::npos) {
        NLM_LOG_WARN("Step error marked as recoverable, continuing");
        return true;
    }
    
    return false;
}

void BrainStepHandler::recoverFromStepErrors(Brain* brain) {
    if (!brain) return;
    
    // Reset brain to a consistent state
    brain->reset();
    
    // Re-initialize with current configuration
    if (brain->initialize()) {
        NLM_LOG_INFO("Brain recovered and re-initialized successfully");
    } else {
        NLM_LOG_ERROR("Brain recovery failed");
    }
}

void BrainStepHandler::trackPlasticityEfficiency(Brain* brain) {
    if (!brain || !brain->pImpl->stdp) return;
    
    // Calculate plasticity efficiency
    float plasticityRate = static_cast<float>(stepState_.plasticityUpdates) / std::max(1.0, stepState_.stepTime);
    
    if (plasticityRate > 10.0f) {
        NLM_LOG_WARN("High plasticity activity rate: " + std::to_string(plasticityRate) + " events/second");
    }
}

void BrainStepHandler::trackNeuralDynamics(Brain* brain) {
    if (!brain) return;
    
    // Track neural firing patterns
    float firingRate = stepMetrics_.avgFiringRate;
    
    if (firingRate > 50.0f) {
        NLM_LOG_WARN("High firing rate detected: " + std::to_string(firingRate) + "%");
    } else if (firingRate < 5.0f) {
        NLM_LOG_WARN("Low firing rate detected: " + std::to_string(firingRate) + "%");
    }
}

bool BrainStepHandler::shouldUpdatePlasticity(const Config* config) const {
    if (!config) return true;
    
    // Check if plasticity is enabled
    bool plasticityEnabled = config->getOr<bool>("plasticity.stdp.enable", true);
    return plasticityEnabled;
}

bool BrainStepHandler::shouldUpdateDevelopment(const Config* config, Timestamp timestep) const {
    if (!config) return true;
    
    // Check development stage
    auto developmentalStage = brain->getDevelopmentalStage();
    
    // Update development in all stages except maybe aging
    return developmentalStage != DevelopmentalStage::Aging;
}

bool BrainStepHandler::shouldUpdateNeuromodulation(const Config* config, Timestamp timestep) const {
    if (!config) return true;
    
    // Check neuromodulation settings
    bool neuromodulationEnabled = config->getOr<bool>("neuromod.dopamine.enable", true);
    return neuromodulationEnabled;
}

void BrainStepHandler::setupStepEnvironment(Brain* brain, SimulationStep currentStep) {
    if (!brain) return;
    
    // Set current step in brain
    brain->pImpl->currentStep = currentStep;
    
    // Update simulation time
    stepState_.currentTime = calculateCurrentTime(currentStep, 0.001);
    
    // Reset step metrics
    stepMetrics_ = StepMetrics();
}

void BrainStepHandler::cleanupStepEnvironment(Brain* brain) {
    if (!brain) return;
    
    // Clear temporary data
    stepMetrics_ = StepMetrics();
}

Timestamp BrainStepHandler::calculateCurrentTime(SimulationStep step, Timestamp timestep) const {
    return step * timestep;
}

} // namespace nlm