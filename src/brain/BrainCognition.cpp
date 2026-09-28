#include "BrainCognition.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainCognition::initializeCognitionSystems(Brain::Impl& pImpl) {
    try {
        NLM_LOG_INFO("Initializing cognition systems...");
        
        // Validate Brain::Impl
        if (!pImpl->config) {
            NLM_LOG_ERROR("Failed to initialize cognition systems: Configuration is null");
            return;
        }
        
        // Initialize cognition systems
        if (pImpl->planner) {
            try {
                pImpl->planner->initialize(pImpl.get());
                pImpl->planner->setPlanningDepth(5);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to initialize neural planner: " + e.what());
                return;
            }
        }
        
        if (pImpl->conceptFormation) {
            try {
                pImpl->conceptFormation->initialize(pImpl.get());
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to initialize concept formation: " + e.what());
                return;
            }
        }
        
        if (pImpl->attention) {
            try {
                pImpl->attention->initialize(pImpl.get());
                pImpl->attention->setInhibitionStrength(0.5f);
                pImpl->attention->setExcitationStrength(1.5f);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to initialize attention system: " + e.what());
                return;
            }
        }
        
        NLM_LOG_INFO("Cognition systems initialization complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during cognition systems initialization: ") + e.what());
    }
}

void BrainCognition::updateCognitionSystems(Brain::Impl& pImpl, double timestep) {
    // Update attention system
    if (pImpl->attention) {
        pImpl->attention->update(timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // Update concept formation
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding and pattern clustering
        pImpl->conceptFormation->update(timestep);
    }
    
    // Update neural planner
    if (pImpl->planner) {
        pImpl->planner->update(timestep);
    }
}

void BrainCognition::logCognitionStats(const Brain::Impl& pImpl) const {
    if (pImpl->attention) {
        NLM_LOG_INFO("Attention system active: competitive selection of neural representations");
    }
    
    if (pImpl->conceptFormation) {
        NLM_LOG_INFO("Concept formation active: extracting patterns from experience");
    }
    
    if (pImpl->planner) {
        NLM_LOG_INFO("Neural planner active: planning action sequences");
    }
}

void BrainCognition::integrateCognitionWithMemory(Brain::Impl& pImpl) {
    // Integrate cognition systems with memory systems
    if (pImpl->attention && pImpl->workingMemory) {
        // Use attention to select memory traces for cognitive processing
        std::vector<NeuronId> memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        pImpl->attention->processCompetition(memoryNeurons);
    }
    
    if (pImpl->conceptFormation && pImpl->episodicMemory) {
        // Use episodic memory to inform concept formation
        auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(10);
        pImpl->conceptFormation->integrateWithEpisodicMemory(recentEpisodes);
    }
}

void BrainCognition::updateAttentionSystem(Brain::Impl& pImpl) {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
    }
}

void BrainCognition::updateConceptFormation(Brain::Impl& pImpl) {
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->update(pImpl->timestep);
    }
}

void BrainCognition::updateNeuralPlanner(Brain::Impl& pImpl) {
    if (pImpl->planner) {
        pImpl->planner->update(pImpl->timestep);
    }
}

} // namespace nlm
