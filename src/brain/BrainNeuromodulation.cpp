#include "BrainNeuromodulation.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainNeuromodulation::initializeNeuromodulationSystems(Brain::Impl& pImpl) {
    try {
        NLM_LOG_INFO("Initializing neuromodulation systems...");
        
        // Validate Brain::Impl
        if (!pImpl->config) {
            NLM_LOG_ERROR("Failed to initialize neuromodulation systems: Configuration is null");
            return;
        }
        
        // Initialize neuromodulation systems
        if (pImpl->novelty) {
            try {
                pImpl->novelty->initialize(pImpl.get());
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to initialize novelty detection: " + e.what());
                return;
            }
        }
        
        if (pImpl->curiosity) {
            try {
                pImpl->curiosity->initialize(pImpl.get());
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to initialize curiosity system: " + e.what());
                return;
            }
        }
        
        if (pImpl->dopamine) {
            try {
                pImpl->dopamine->initialize(pImpl.get());
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to initialize dopamine system: " + e.what());
                return;
            }
        }
        
        if (pImpl->predictionError) {
            try {
                pImpl->predictionError->initialize(pImpl.get());
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to initialize prediction error system: " + e.what());
                return;
            }
        }
        
        NLM_LOG_INFO("Neuromodulation systems initialization complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during neuromodulation systems initialization: ") + e.what());
    }
}

void BrainNeuromodulation::updateNeuromodulationSystems(Brain::Impl& pImpl, double timestep) {
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
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

void BrainNeuromodulation::logNeuromodulationStats(const Brain::Impl& pImpl) const {
    if (pImpl->novelty) {
        NLM_LOG_INFO("Novelty detection active: " + std::to_string(pImpl->novelty->getLevel()));
    }
    
    if (pImpl->curiosity) {
        NLM_LOG_INFO("Curiosity system active: " + std::to_string(pImpl->curiosity->getLevel()));
    }
    
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine system active: " + std::to_string(pImpl->dopamine->getLevel()));
    }
}

void BrainNeuromodulation::applyNeuromodulationToNeurons(Brain::Impl& pImpl) {
    // Apply neuromodulation effects to neurons
    if (pImpl->dopamine) {
        float dopamineLevel = pImpl->dopamine->getLevel();
        
        // Dopamine modulates neural excitability
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by adjusting effective current injection
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void BrainNeuromodulation::updateDopamineEffects(Brain::Impl& pImpl) {
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
    }
}

void BrainNeuromodulation::updateCuriosityEffects(Brain::Impl& pImpl) {
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
}

void BrainNeuromodulation::updateNoveltyEffects(Brain::Impl& pImpl) {
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
}

void BrainNeuromodulation::updatePredictionErrorEffects(Brain::Impl& pImpl) {
    if (pImpl->predictionError) {
        pImpl->predictionError->update(pImpl->timestep);
    }
}

} // namespace nlm
