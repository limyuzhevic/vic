#include "BrainHeaders.hpp"

namespace nlm {

void BrainDevelopment::updateDevelopment(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    auto* devSystem = brain->getDevelopmentSystem();
    if (devSystem) {
        devSystem->update(timestep);
        
        // Get current developmental stage
        DevelopmentalStage stage = brain->getDevelopmentalStage();
        
        // Apply stage-specific changes
        switch (stage) {
            case DevelopmentalStage::Initial:
                applyInitialDevelopment(brain);
                break;
            case DevelopmentalStage::Growth:
                applyGrowthDevelopment(brain);
                break;
            case DevelopmentalStage::Maturation:
                applyMaturationDevelopment(brain);
                break;
            case DevelopmentalStage::Decline:
                applyDeclineDevelopment(brain);
                break;
            case DevelopmentalStage::Rest:
                applyRestDevelopment(brain);
                break;
        }
    }
}

void BrainDevelopment::applyDevelopmentalChanges(Brain* brain) {
    if (!brain) return;
    
    auto* devSystem = brain->getDevelopmentSystem();
    if (!devSystem) return;
    
    // Update developmental stage
    DevelopmentalStage currentStage = brain->getDevelopmentalStage();
    DevelopmentalStage nextStage = devSystem->getNextStage(currentStage);
    
    if (nextStage != currentStage) {
        brain->setDevelopmentalStage(nextStage);
        NLM_LOG_INFO("Brain developmental stage changed to: " + std::to_string(static_cast<int>(nextStage)));
    }
}

void BrainDevelopment::triggerDevelopmentalStage(Brain* brain, DevelopmentalStage stage) {
    if (!brain) return;
    brain->setDevelopmentalStage(stage);
    NLM_LOG_INFO("Brain developmental stage manually set to: " + std::to_string(static_cast<int>(stage)));
}

void BrainDevelopment::regulateSynaptogenesis(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    // Activity-dependent synaptogenesis
    float synaptogenesisRate = brain->pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getType() == NeuronType::Internal) {
                float activityLevel = pop->getAverageFiringRate();
                int newSynapses = static_cast<int>(synaptogenesisRate * timestep * 100.0f * activityLevel);
                
                for (int i = 0; i < newSynapses; ++i) {
                    region->addSynapseToPopulation(pop->getId());
                }
            }
        }
    }
}

void BrainDevelopment::applyPruning(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    // Activity-dependent pruning
    float pruningRate = brain->pImpl->config->getOr<float>("pruning_rate", 0.00001f);
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getType() == NeuronType::Internal) {
                float activityLevel = pop->getAverageFiringRate();
                float pruneThreshold = 0.1f;
                
                if (activityLevel < pruneThreshold) {
                    int prunedSynapses = static_cast<int>(pruningRate * timestep * 100.0f);
                    region->removeInactiveSynapsesFromPopulation(pop->getId(), pruneThreshold);
                }
            }
        }
    }
}

void BrainDevelopment::applyInitialDevelopment(Brain* brain) {
    if (!brain) return;
    
    // Initial growth phase - establish basic connectivity
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getType() == NeuronType::Internal) {
                // Add initial connectivity
                region->addSynapseToPopulation(pop->getId());
            }
        }
    }
}

void BrainDevelopment::applyGrowthDevelopment(Brain* brain) {
    if (!brain) return;
    
    // Growth phase - increase complexity
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getType() == NeuronType::Internal && pop->getNeuronCount() < 500) {
                // Add more neurons and connections
                pop->addNeurons(10);
                region->addSynapsesToPopulation(pop->getId(), 20);
            }
        }
    }
}

void BrainDevelopment::applyMaturationDevelopment(Brain* brain) {
    if (!brain) return;
    
    // Maturation phase - refine connections
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getType() == NeuronType::Internal) {
                // Strengthen active connections
                float activityLevel = pop->getAverageFiringRate();
                if (activityLevel > 0.3f) {
                    region->strengthenActiveConnections(pop->getId(), 0.1f);
                }
            }
        }
    }
}

void BrainDevelopment::applyDeclineDevelopment(Brain* brain) {
    if (!brain) return;
    
    // Decline phase - prune unused connections
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getType() == NeuronType::Internal) {
                float activityLevel = pop->getAverageFiringRate();
                if (activityLevel < 0.05f) {
                    region->removeInactiveSynapsesFromPopulation(pop->getId(), 0.05f);
                }
            }
        }
    }
}

void BrainDevelopment::applyRestDevelopment(Brain* brain) {
    if (!brain) return;
    
    // Rest phase - consolidate memories
    if (brain->pImpl->stepsSinceLastEpisode >= brain->pImpl->consolidationInterval) {
        if (brain->getEpisodicMemory()) {
            brain->getEpisodicMemory()->consolidate();
        }
        brain->pImpl->stepsSinceLastEpisode = 0;
    }
}

} // namespace nlm