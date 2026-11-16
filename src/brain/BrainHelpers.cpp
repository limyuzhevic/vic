// Brain Helper Methods Implementation
// Refactored helper functions for Brain.cpp

#include "BrainHelpers.hpp"
#include "../brain/Brain.hpp"
#include "../brain/BrainInitialization.hpp"
#include "../brain/BrainIntegration.hpp"
#include "../brain/BrainCoreStep.hpp"
#include "../brain/BrainCheckpoint.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Configuration and initialization helpers
void BrainHelpers::logConfiguration(const Brain::Impl& pImpl, 
                                    size_t neuronCount, 
                                    size_t regionCount, 
                                    float connectionProbability) {
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
}

void BrainHelpers::createRegions(Brain::Impl& pImpl, size_t regionCount) {
    for (size_t i = 0; i < regionCount; ++i) {
        pImpl->regions.push_back(std::make_unique<NeuralRegion>("Region_" + std::to_string(i + 1)));
    }
}

void BrainHelpers::createNeuronsAndPopulateNeurons(Brain::Impl& pImpl, 
                                                   size_t neuronCount, 
                                                   size_t regionCount, 
                                                   float connectionProbability,
                                                   bool isNewInitialization) {
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t nextNeuronIndex = 1;  // globally unique IDs across regions
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto& region = pImpl->regions[i];
        region->setNextNeuronIndex(nextNeuronIndex);
        
        // Add populations with 80/20 excitatory/inhibitory split in internal population
        auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
        size_t internalCount = neuronsPerRegion / 2;
        size_t internalExcitatory = internalCount * 4 / 5;
        auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
        auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
        (void)internalPopId; (void)inhibitoryPopId;
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
        nextNeuronIndex += region->getTotalNeuronCount();
    }
    
    // Build the neuron lookup used by synaptic delivery and injection.
    for (auto& region : pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            pImpl->neuronById[neuron->getId().index()] = neuron;
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto& region = pImpl->regions[i];
        if (region) {
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void BrainHelpers::collectSensoryMotorNeurons(Brain::Impl& pImpl) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            } else if (pop->getNeuronType() == NeuronType::Motor) {
                for (auto* neuron : pop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
}

void BrainHelpers::logInitializationStats(const Brain::Impl& pImpl) {
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(pImpl->regions.size() ? 
                                                   pImpl->regions[0]->getTotalNeuronCount() : 0));
    NLM_LOG_INFO("Total synapses: " + std::to_string(pImpl->regions.size() ? 
                                                   pImpl->regions[0]->getSynapseCount() : 0));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
}

// Step helper methods
void BrainHelpers::registerSpikeHandlers(Brain::Impl& pImpl) {
    // Register spike handlers once; re-initialising used to register them
    // again and double-count every spike.
    if (!pImpl->spikeHandlersRegistered) {
        pImpl->spikeHandlersRegistered = true;
        
        // Register spike handlers for event-driven processing
        pImpl->spikeSystem->registerHandler([](const DetailedSpikeEvent& event) {
            // Count spikes (simple implementation)
        });
        
        // Register delayed spike handler to deliver synaptic input
        pImpl->spikeSystem->registerDelayedHandler([](const DelayedSpikeEvent& event) {
            // Deliver synaptic input (implementation would need access to neuron lookup)
        });
    }
}

void BrainHelpers::applyHomeostaticScaling(Brain::Impl& pImpl, SimulationStep currentStep) {
    if (currentStep > 0 && currentStep % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        for (auto& region : pImpl->regions) {
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
    }
}

void BrainHelpers::updateDevelopmentSystem(Brain::Impl& pImpl, SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(nullptr, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity.get();
        if (sp) {
            DevelopmentalStage stage = pImpl->developmentalStage;
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

// Action and behavior helpers
void BrainHelpers::calculateMotorNeuronActivity(const Brain::Impl& pImpl, 
                                              size_t& firingMotor) {
    firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
}

ActionType BrainHelpers::selectAction(size_t firingMotor) {
    // Simple action selection based on motor neuron activity
    if (firingMotor > 0) {
        return ActionType::MoveForward;
    }
    return ActionType::Wait;
}

void BrainHelpers::applyNeuromodulation(Dopamine& dopamine, float& plasticityMod) {
    // Apply neuromodulation effects on plasticity
    float modulation = dopamine.getLevel();
    
    // Scale STDP learning rates
    plasticityMod = 1.0f + (modulation - 1.0f) * 0.1f;  // Example scaling
}

// Reset and state helpers
void BrainHelpers::resetBrain(Brain::Impl& pImpl) {
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

// Sensory and motor helpers
void BrainHelpers::injectSensoryInput(Brain::Impl& pImpl, const SensoryInput& input) {
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

void BrainHelpers::produceAction(const Brain::Impl& pImpl, ActionType& actionType) {
    // Simple action selection based on motor neuron activity
    if (pImpl->motorNeurons.empty()) {
        actionType = ActionType::Wait;
    } else {
        size_t firingMotor = 0;
        calculateMotorNeuronActivity(pImpl, firingMotor);
        actionType = selectAction(firingMotor);
    }
}

// Statistics helpers
void BrainHelpers::updateStatistics(Brain::Impl& pImpl) {
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    
    for (const auto& region : pImpl->regions) {
        totalNeurons += region->getTotalNeuronCount();
        totalSynapses += region->getSynapseCount();
    }
    
    NLM_LOG_INFO("Statistics - Total neurons: " + std::to_string(totalNeurons) +
                 ", Total synapses: " + std::to_string(totalSynapses) +
                 ", Current spikes: " + std::to_string(pImpl->totalSpikesThisStep) +
                 ", Total spikes: " + std::to_string(pImpl->totalSpikesTotal));
}

// Development helpers
void BrainHelpers::applyDevelopmentEffects(Brain::Impl& pImpl, SimulationStep currentStep) {
    // Development affects plasticity rates
    auto* sp = pImpl->structuralPlasticity.get();
    if (sp) {
        DevelopmentalStage stage = pImpl->developmentalStage;
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

// Memory helpers
void BrainHelpers::updateEpisodicMemory(Brain::Impl& pImpl, SimulationStep currentStep) {
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode (simplified)
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons (simplified)
            for (auto& region : pImpl->regions) {
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
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainHelpers::replayMemories(Brain::Impl& pImpl, SimulationStep currentStep) {
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainHelpers::consolidateMemories(Brain::Impl& pImpl, SimulationStep currentStep) {
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

// Configuration helpers
void BrainHelpers::loadConfiguration(Brain::Impl& pImpl) {
    // Get configuration values
    size_t neuronCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("region_count", 1));
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    // Create regions and neurons
    createRegions(pImpl, regionCount);
    createNeuronsAndPopulateNeurons(pImpl, neuronCount, regionCount, connectionProbability, true);
    
    logConfiguration(pImpl, neuronCount, regionCount, connectionProbability);
}

// Utility functions
std::string BrainHelpers::formatLogMessage(const std::string& prefix, const std::string& message) {
    return "[" + prefix + "] " + message;
}

float BrainHelpers::calculateExcitationInhibitionRatio(const Brain::Impl& pImpl) {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
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

} // namespace nlm
