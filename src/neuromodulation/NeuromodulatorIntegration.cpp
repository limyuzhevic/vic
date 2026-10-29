// Neuromodulator integration functions
void Neuromodulator::applyAllNeuromodulators(Brain* brain) {
    if (!brain || !brain->pImpl) return;
    
    auto& impl = *brain->pImpl;
    
    // Apply dopamine effects
    if (impl.dopamine) {
        impl.dopamine->update(0.001f);  // Update with timestep
        impl.dopamine->modulatePlasticity(impl.stdp.get(), impl.hebbian.get());
    }
    
    // Apply acetylcholine effects
    if (impl.acetylcholine) {
        impl.acetylcholine->update(0.001f);
        impl.acetylcholine->enhanceAttention();
        impl.acetylcholine->boostMemoryConsolidation();
    }
    
    // Apply norepinephrine effects
    if (impl.norepinephrine) {
        impl.norepinephrine->update(0.001f);
        impl.norepinephrine->increaseArousal();
        impl.norepinephrine->boostVigilance();
    }
    
    // Apply serotonin effects
    if (impl.serotonin) {
        impl.serotonin->update(0.001f);
        impl.serotonin->improveMood();
        impl.serotonin->reduceImpulsivity();
    }
    
    // Apply curiosity effects
    if (impl.curiosity) {
        // Curiosity is updated based on novelty and prediction error
        float novelty = impl.novelty ? impl.novelty->getLevel() : 0.0f;
        float predictionError = impl.predictionError ? impl.predictionError->getLevel() : 0.0f;
        impl.curiosity->update(novelty, predictionError, 0.001f);
    }
    
    // Apply novelty effects
    if (impl.novelty) {
        impl.novelty->update(0.001f);
    }
    
    // Apply prediction error effects
    if (impl.predictionError) {
        impl.predictionError->update(0.001f);
    }
}

void Neuromodulator::processNeuromodulation(Brain* brain, TimestepDuration dt) {
    if (!brain || !brain->pImpl) return;
    
    auto& impl = *brain->pImpl;
    
    // Update all neuromodulators with the current timestep
    applyAllNeuromodulators(brain);
    
    // Neuromodulation effects on neural excitability
    float totalNeuromodulation = 0.0f;
    if (impl.dopamine) totalNeuromodulation += impl.dopamine->getLevel();
    if (impl.acetylcholine) totalNeuromodulation += impl.acetylcholine->getLevel();
    if (impl.norepinephrine) totalNeuromodulation += impl.norepinephrine->getLevel();
    if (impl.serotonin) totalNeuromodulation += impl.serotonin->getLevel();
    if (impl.curiosity) totalNeuromodulation += impl.curiosity->getLevel();
    if (impl.novelty) totalNeuromodulation += impl.novelty->getLevel();
    if (impl.predictionError) totalNeuromodulation += impl.predictionError->getLevel();
    
    // Apply combined neuromodulatory effects on neurons
    float excitabilityFactor = 1.0f + totalNeuromodulation * 0.5f;
    float plasticityFactor = 1.0f + totalNeuromodulation * 0.3f;
    
    // Apply to all neurons
    for (auto& region : impl.regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Neuromodulatory effects on neuron properties
                float excitabilityMod = excitabilityFactor * neuron->getTotalCurrent() * 0.1f;
                neuron->injectCurrent(excitabilityMod);
                
                // Update plastic synaptic properties based on neuromodulation
                for (auto* syn : region->getSynapses()) {
                    if (syn->getPlasticityFlags().plasticity) {
                        float currentWeight = syn->getWeight();
                        float weightMod = plasticityFactor * 0.01f;
                        syn->addToWeight(weightMod);
                    }
                }
            }
        }
    }
}

void Neuromodulator::integrateNeuromodulationWithMemory(Brain* brain) {
    if (!brain || !brain->pImpl) return;
    
    auto& impl = *brain->pImpl;
    
    // Integrate with working memory
    if (impl.workingMemory) {
        // Acetylcholine enhances working memory encoding
        if (impl.acetylcholine) {
            float acLevel = impl.acetylcholine->getLevel();
            impl.workingMemory->setEncodingStrength(acLevel);
        }
        
        // Dopamine modulates working memory maintenance
        if (impl.dopamine) {
            float daLevel = impl.dopamine->getLevel();
            if (daLevel > 0.5f) {
                // High dopamine enhances working memory stability
                impl.workingMemory->setStabilization(daLevel);
            }
        }
    }
    
    // Integrate with episodic memory
    if (impl.episodicMemory) {
        // Serotonin modulates memory consolidation
        if (impl.serotonin) {
            float serotoninLevel = impl.serotonin->getLevel();
            if (serotoninLevel > 0.3f) {
                // High serotonin promotes memory consolidation
                impl.episodicMemory->setConsolidationRate(serotoninLevel);
            }
        }
        
        // Acetylcholine enhances memory encoding
        if (impl.acetylcholine) {
            float acLevel = impl.acetylcholine->getLevel();
            impl.episodicMemory->setEncodingPriority(acLevel);
        }
    }
}

void Neuromodulator::integrateNeuromodulationWithAttention(Brain* brain) {
    if (!brain || !brain->pImpl || !brain->pImpl->attention) return;
    
    auto& impl = *brain->pImpl;
    
    auto* attention = impl.attention.get();
    
    // Acetylcholine enhances attention
    if (impl.acetylcholine) {
        float acLevel = impl.acetylcholine->getLevel();
        attention->boostPriority(acLevel * 0.5f);
        attention->setGatingStrength(acLevel);
    }
    
    // Norepinephrine enhances vigilance
    if (impl.norepinephrine) {
        float neLevel = impl.norepinephrine->getLevel();
        attention->boostVigilance(neLevel * 0.3f);
    }
    
    // Dopamine modulates attention selection
    if (impl.dopamine) {
        float daLevel = impl.dopamine->getLevel();
        if (daLevel > 0.3f) {
            // High dopamine enhances winner-take-all dynamics
            attention->setSelectionBias(daLevel);
        }
    }
    
    // Serotonin reduces impulsivity
    if (impl.serotonin) {
        float serotoninLevel = impl.serotonin->getLevel();
        attention->reduceImpulsivity(serotoninLevel * 0.2f);
    }
}

void Neuromodulator::integrateNeuromodulationWithPlasticity(Brain* brain) {
    if (!brain || !brain->pImpl) return;
    
    auto& impl = *brain->pImpl;
    
    // Calculate total neuromodulatory influence on plasticity
    float totalPlasticityModulation = 1.0f;
    
    // Dopamine effects on plasticity
    if (impl.dopamine) {
        totalPlasticityModulation *= impl.dopamine->getPlasticityFactor();
    }
    
    // Acetylcholine effects on plasticity
    if (impl.acetylcholine) {
        totalPlasticityModulation *= impl.acetylcholine->getPlasticityFactor();
    }
    
    // Norepinephrine effects on plasticity
    if (impl.norepinephrine) {
        totalPlasticityModulation *= impl.norepinephrine->getPlasticityFactor();
    }
    
    // Serotonin effects on plasticity
    if (impl.serotonin) {
        totalPlasticityModulation *= impl.serotonin->getPlasticityFactor();
    }
    
    // Apply neuromodulatory effects to all plasticity systems
    if (impl.stdp) {
        impl.stdp->setModulationFactor(totalPlasticityModulation);
    }
    
    if (impl.hebbian) {
        impl.hebbian->setModulationFactor(totalPlasticityModulation);
    }
    
    if (impl.structuralPlasticity) {
        impl.structuralPlasticity->setModulationFactor(totalPlasticityModulation);
    }
}