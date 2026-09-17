// Store percept in working memory
void NeuralWorkingMemory::storePercept(const std::vector<float>& percept) {
    if (!brain_ || percept.empty()) return;
    
    // Encode percept into neural activity
    size_t neuronsNeeded = std::min(percept.size(), memoryNeurons_.size());
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = percept[i] * decayRate_;
        
        // Store activation
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation;
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            memoryNeurons_.push_back(neuron);
        }
        
        // Inject current into neuron
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}
