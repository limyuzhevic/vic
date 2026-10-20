// Add the missing getter methods to Neuron.cpp after the existing methods

NeuronId Neuron::getId() const {
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    return pImpl->type;
}

}