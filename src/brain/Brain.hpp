#pragma once

#include "Brain.hpp"
#include "BrainInitializer.hpp"
#include "BrainStep.hpp"
#include "BrainIntegration.hpp"
#include "BrainState.hpp"
#include "BrainMetrics.hpp"
#include "BrainUtils.hpp"

namespace nlm {

// Convenience typedefs
using DefaultConfig = std::shared_ptr<Config>;
using RegionPtr = std::unique_ptr<NeuralRegion>;
using SynapsePtr = Synapse*;

// ========== BRAIN INITIALIZER ==========

namespace BrainInitializer {
    
    bool initializeSystem(Brain::Impl* impl, size_t neuronCount, size_t regionCount, 
                          float connectionProbability, Timestamp currentTime);
    
    void setupMemorySystems(Brain::Impl* impl, size_t neuronCount);
    
    void setupCognitionSystems(Brain::Impl* impl);
    
    void setupNeuromodulationSystems(Brain::Impl* impl);
    
    void setupSpikeHandlers(Brain::Impl* impl);
    
    void configureCheckpointManager(Brain::Impl* impl, Timestamp currentTime);
}

// ========== BRAIN STEP PROCESS ==========

namespace BrainStep {
    
    void executeStep(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    
    void processDelayedSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    
    void updateNeurons(Brain::Impl* impl, Timestamp currentTime, TimestepDuration timestep);
    
    void processSpikes(Brain::Impl* impl, SimulationStep currentStep);
    
    void updateWorkingMemory(Brain::Impl* impl, TimestepDuration timestep);
    
    void applyNeuromodulationEffects(Brain::Impl* impl, TimestepDuration timestep);
    
    void applyPlasticity(Brain::Impl* impl, float plasticityMod, Timestamp currentTime);
    
    void updateEpisodicMemory(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    
    void updatePredictionSystem(Brain::Impl* impl);
    
    void updateAttentionSystem(Brain::Impl* impl);
    
    void updateConceptFormation(Brain::Impl* impl);
    
    void applyStructuralPlasticity(Brain::Impl* impl, SimulationStep currentStep);
    
    void replayMemories(Brain::Impl* impl, SimulationStep currentStep);
    
    void applyDevelopment(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime, float plasticityMod);
    
    void consolidateMemories(Brain::Impl* impl, SimulationStep currentStep);
    
    void updateCheckpointManager(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
}

// ========== BRAIN INTEGRATION ==========

namespace BrainIntegration {
    
    void integrateSensoryInput(Brain::Impl* impl, const class SensoryInput& input);
    
    void injectCurrentToNeuron(Brain::Impl* impl, NeuronId neuron, MembranePotential current);
    
    void injectCurrentToNeurons(Brain::Impl* impl, NeuronType type, MembranePotential current);
    
    std::unique_ptr<class Action> produceAction(Brain::Impl* impl);
    
    void applyNeuromodulation(Brain::Impl* impl, const class Neuromodulator& signal);
    
    void updatePlasticity(Brain::Impl* impl);
    
    void develop(Brain::Impl* impl);
}

// ========== BRAIN STATE MANAGEMENT ==========

namespace BrainState {
    
    bool saveBrainState(Brain::Impl* impl, const std::string& filepath);
    
    bool loadBrainState(Brain::Impl* impl, const std::string& filepath);
    
    void resetBrain(Brain::Impl* impl);
    
    void collectStatistics(Brain::Impl* impl);
}

// ========== BRAIN METRICS ==========

namespace BrainMetrics {
    
    size_t getTotalNeuronCount(const Brain::Impl* impl);
    
    size_t getTotalSynapseCount(const Brain::Impl* impl);
    
    size_t getActiveNeuronCount(const Brain::Impl* impl);
    
    size_t getFiringNeuronCount(const Brain::Impl* impl);
    
    float getAverageFiringRate(const Brain::Impl* impl);
    
    float getExcitationInhibitionRatio(const Brain::Impl* impl);
    
    size_t getTotalSpikeCount(const Brain::Impl* impl);
    
    size_t getPendingSpikeEventCount(const Brain::Impl* impl);
    
    DevelopmentalStage getDevelopmentalStage(const Brain::Impl* impl);
}

// ========== BRAIN UTILS ==========

namespace BrainUtils {
    
    void createRegions(Brain::Impl* impl, size_t regionCount);
    
    void setupNeurons(Brain::Impl* impl, size_t neuronCount, size_t regionCount);
    
    void setupConnectivity(Brain::Impl* impl, size_t regionCount, float connectionProbability);
    
    void collectSensoryAndMotorNeurons(Brain::Impl* impl, size_t neuronsPerRegion, size_t regionCount);
    
    void logBrainStatus(const Brain::Impl* impl);
    
    std::string formatLogMessage(const std::string& message);
}

} // namespace nlm

// Implementation stubs
namespace nlm {

bool BrainInitializer::initializeSystem(Brain::Impl* impl, size_t neuronCount, size_t regionCount, 
                                       float connectionProbability, Timestamp currentTime) {
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        impl->addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = impl->getRegion(RegionId(i + 1));
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
                    impl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    impl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = impl->getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*impl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    return true;
}

void BrainInitializer::setupMemorySystems(Brain::Impl* impl, size_t neuronCount) {
    // Initialize working memory
    impl->workingMemory->initialize(nullptr); // Note: this would need the Brain* 
    impl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    impl->episodicMemory->initialize(nullptr); // Note: this would need the Brain*
    impl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    impl->associativeMemory->initialize(nullptr); // Note: this would need the Brain*
}

void BrainInitializer::setupCognitionSystems(Brain::Impl* impl) {
    // Initialize cognition systems
    impl->planner->initialize(nullptr); // Note: this would need the Brain*
    impl->planner->setPlanningDepth(5);
    
    impl->conceptFormation->initialize(nullptr); // Note: this would need the Brain*
    
    impl->attention->initialize(nullptr); // Note: this would need the Brain*
    impl->attention->setInhibitionStrength(0.5f);
    impl->attention->setExcitationStrength(1.5f);
}

void BrainInitializer::setupNeuromodulationSystems(Brain::Impl* impl) {
    // Initialize neuromodulation
    impl->novelty->initialize(nullptr); // Note: this would need the Brain*
    impl->curiosity->initialize(nullptr); // Note: this would need the Brain*
}

void BrainInitializer::setupSpikeHandlers(Brain::Impl* impl) {
    // Register spike handlers for event-driven processing
    impl->spikeSystem->registerHandler([impl](const DetailedSpikeEvent& event) {
        // Count spikes
        ++impl->totalSpikesThisStep;
        ++impl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    impl->spikeSystem->registerDelayedHandler([impl](const DelayedSpikeEvent& event) {
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

void BrainInitializer::configureCheckpointManager(Brain::Impl* impl, Timestamp currentTime) {
    // Configure checkpoint manager
    std::string checkpointDir = impl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    impl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

} // namespace nlm