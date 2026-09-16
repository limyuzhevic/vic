void Brain::forEachNeuron(std::function<void(Neuron*, Timestamp)> callback) const {
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                callback(const_cast<Neuron*>(neuron), pImpl->currentTime);
            }
        }
    }
}

void Brain::forEachSynapse(std::function<void(Synapse*, Timestamp)> callback) const {
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            callback(const_cast<Synapse*>(syn.get()), pImpl->currentTime);
        }
    }
}

void Brain::forEachRegionPopulationNeuron(std::function<void(NeuralRegion*, NeuralPopulation*, Neuron*)> callback) const {
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                callback(const_cast<NeuralRegion*>(region.get()),
                        const_cast<NeuralPopulation*>(pop.get()),
                        const_cast<Neuron*>(neuron));
            }
        }
    }
}
