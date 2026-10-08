#include "MemorySystemManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <stdexcept>

namespace nlm {

MemorySystemManager::MemorySystemManager(std::shared_ptr<Brain> brain)
    : brain(brain) {
    
    if (!brain) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
}

bool MemorySystemManager::initializeAll() {
    if (!brain) {
        return false;
    }
    
    try {
        // Initialize working memory
        if (workingMemory) {
            workingMemory->initialize(brain.get());
        }
        
        // Initialize episodic memory
        if (episodicMemory) {
            episodicMemory->initialize(brain.get());
        }
        
        // Initialize associative memory
        if (associativeMemory) {
            associativeMemory->initialize(brain.get());
        }
        
        return true;
        
    } catch (const std::exception& e) {
        Logger::get()->error("Failed to initialize memory systems: " + std::string(e.what()));
        return false;
    }
}

void MemorySystemManager::resetAll() {
    if (!brain) {
        return;
    }
    
    try {
        if (workingMemory) {
            workingMemory->clear();
        }
        if (episodicMemory) {
            episodicMemory->clear();
        }
        if (associativeMemory) {
            associativeMemory->clear();
        }
        if (attention) {
            attention->reset();
        }
    } catch (const std::exception& e) {
        Logger::get()->error("Failed to reset memory systems: " + std::string(e.what()));
    }
}

void MemorySystemManager::updateAll(double timestep) {
    if (!brain) {
        return;
    }
    
    try {
        // Update working memory
        if (workingMemory) {
            workingMemory->update(timestep);
        }
        
        // Update episodic memory
        if (episodicMemory) {
            // Periodically store new episodes
            static size_t stepsSinceLastEpisode = 0;
            stepsSinceLastEpisode++;
            if (stepsSinceLastEpisode >= 10) {
                stepsSinceLastEpisode = 0;
                EpisodicMemoryItem episode;
                episode.timestamp = brain->getTotalSpikeCount();
                episode.reward = getDopamine() ? getDopamine()->getLevel() : 0.0f;
                episodicMemory->storeEpisode(episode);
            }
        }
        
        // Update cognition systems
        if (planner) {
            planner->update(timestep);
        }
        if (conceptFormation) {
            conceptFormation->update(timestep);
        }
        if (attention) {
            attention->update(timestep);
        }
        
        // Update neuromodulation
        if (novelty) {
            novelty->update(timestep);
        }
        if (curiosity) {
            curiosity->update(timestep);
        }
        if (dopamine) {
            dopamine->update(timestep);
        }
        
    } catch (const std::exception& e) {
        Logger::get()->error("Failed to update memory systems: " + std::string(e.what()));
    }
}

} // namespace nlm