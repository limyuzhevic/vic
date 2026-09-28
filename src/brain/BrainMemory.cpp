#include "BrainMemory.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainMemory::initializeMemorySystems(Brain::Impl& pImpl) {
    try {
        NLM_LOG_INFO("Initializing memory systems...");
        
        // Validate Brain::Impl
        if (!pImpl->config) {
            NLM_LOG_ERROR("Failed to initialize memory systems: Configuration is null");
            return;
        }
        
        // Initialize working memory
        if (pImpl->workingMemory) {
            try {
                pImpl->workingMemory->initialize(pImpl.get());
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to initialize working memory: " + e.what());
                return;
            }
        }
        
        // Set working memory capacity
        if (pImpl->workingMemory) {
            try {
                pImpl->workingMemory->setCapacity(pImpl->config->getOr<size_t>("neuron_count", 1000) / 10);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to set working memory capacity: " + e.what());
            }
        }
        
        // Initialize episodic memory
        if (pImpl->episodicMemory) {
            try {
                pImpl->episodicMemory->initialize(pImpl.get());
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to initialize episodic memory: " + e.what());
                return;
            }
        }
        
        // Set episodic memory max episodes
        if (pImpl->episodicMemory) {
            try {
                pImpl->episodicMemory->setMaxEpisodes(1000);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to set episodic memory max episodes: " + e.what());
            }
        }
        
        // Initialize associative memory
        if (pImpl->associativeMemory) {
            try {
                pImpl->associativeMemory->initialize(pImpl.get());
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to initialize associative memory: " + e.what());
                return;
            }
        }
        
        NLM_LOG_INFO("Memory systems initialization complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during memory systems initialization: ") + e.what());
    }
}

void BrainMemory::updateWorkingMemory(Brain::Impl& pImpl, double timestep) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(timestep);
    }
}

void BrainMemory::updateEpisodicMemory(Brain::Impl& pImpl, SimulationStep currentStep) {
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
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
            
            // Store reward in episode
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainMemory::updateAssociativeMemory(Brain::Impl& pImpl, SimulationStep currentStep) {
    // Associative memory updates would happen here
    // Currently placeholder for Phase 6 integration
    if (pImpl->associativeMemory) {
        // Update with current neural patterns
        pImpl->associativeMemory->update(currentStep);
    }
}

void BrainMemory::captureCurrentState(Brain::Impl& pImpl, SimulationStep currentStep) {
    if (pImpl->episodicMemory) {
        updateEpisodicMemory(pImpl, currentStep);
    }
}

void BrainMemory::logMemoryStats(const Brain::Impl& pImpl) const {
    if (pImpl.workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl.workingMemory->getActiveTraces()));
    }
    if (pImpl.episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl.episodicMemory->getEpisodeCount()));
    }
}

void BrainMemory::integrateMemoryWithNeurons(Brain::Impl& pImpl) {
    // Integrate memory systems with neural processing
    if (pImpl->workingMemory) {
        // Store to working memory - neurons that fire become part of working memory
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    if (state.firingState == FiringState::Refractory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void BrainMemory::updateMemoryWithNeuromodulation(Brain::Impl& pImpl) {
    // Update memory systems with neuromodulation effects
    if (pImpl->dopamine) {
        float dopamineLevel = pImpl->dopamine->getLevel();
        // Modulate memory strength based on dopamine
        if (pImpl->workingMemory) {
            pImpl->workingMemory->setModulationFactor(dopamineLevel);
        }
    }
}

void BrainMemory::handleMemoryConsolidation(Brain::Impl& pImpl, SimulationStep currentStep) {
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

} // namespace nlm
