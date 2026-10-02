// Brain Spike Manager implementation

void BrainSpikeManager::processSpikes(SimulationStep step) {
    if (brain_->getSpikeSystem()) {
        brain_->getSpikeSystem()->processSpikes(step);
    }
}

void BrainSpikeManager::processDelayedSpikes(SimulationStep step, Timestamp time) {
    if (brain_->getSpikeSystem()) {
        brain_->getSpikeSystem()->processDelayedSpikes(step, time);
    }
}

SpikeSystem* BrainSpikeManager::getSpikeSystem() {
    return brain_->getSpikeSystem();
}

const SpikeSystem* BrainSpikeManager::getSpikeSystem() const {
    return brain_->getSpikeSystem();
}

size_t BrainSpikeManager::getTotalSpikesThisStep() const {
    return brain_->pImpl->totalSpikesThisStep;
}

size_t BrainSpikeManager::getTotalSpikesTotal() const {
    return brain_->pImpl->totalSpikesTotal;
}

size_t BrainSpikeManager::getPendingSpikeEventCount() const {
    return brain_->getPendingSpikeEventCount();
}

void BrainSpikeManager::reset() {
    if (brain_->getSpikeSystem()) {
        brain_->getSpikeSystem()->reset();
    }
}