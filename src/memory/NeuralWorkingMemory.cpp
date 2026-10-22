void NeuralWorkingMemory::updateActiveTraces() {
    activeTraces_.clear();
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        if (memoryActivations_[i] > 0.01f) {
            activeTraces_.push_back(i);
        }
    }
}
