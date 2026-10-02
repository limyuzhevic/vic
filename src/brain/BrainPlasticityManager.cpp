// Brain Plasticity Manager implementation

void BrainPlasticityManager::applyPlasticity(Timestamp currentTime) {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (brain_->getDopamine()) {
        plasticityMod = brain_->getDopamine()->getPlasticityFactor();
    }
    
    for (auto& region : brain_->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    brain_->getSTDP()->update(syn, preSpikes, postSpikes, brain_->pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    brain_->getHebbian()->update(syn, preSpikes, postSpikes, brain_->pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

STDP* BrainPlasticityManager::getSTDP() const {
    return brain_->getSTDP();
}

Hebbian* BrainPlasticityManager::getHebbian() const {
    return brain_->getHebbian();
}

StructuralPlasticity* BrainPlasticityManager::getStructuralPlasticity() const {
    return brain_->getStructuralPlasticity();
}

void BrainPlasticityManager::applyNeuromodulationFactor() {
    // Neuromodulation is applied during system updates
    // This method is kept for API compatibility
}