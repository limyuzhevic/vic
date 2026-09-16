struct MemoryCheckpointData {
    uint64_t type;                    // Memory system type
    std::vector<uint64_t> neuronIds;  // Neuron IDs in memory
    std::vector<float> activations;   // Activation levels
    std::vector<uint64_t> timestamps; // When stored
    std::vector<float> strengths;     // Memory strengths
};

struct EpisodicMemoryCheckpointData {
    uint64_t timestamp;              // Episode timestamp (step)
    float reward;                    // Reward value
    std::vector<uint64_t> activeNeurons;    // Neurons active in episode
    std::vector<float> neuronActivations;   // Their activation levels
};

struct AssociativeMemoryCheckpointData {
    std::vector<uint64_t> pattern1;   // First pattern (neuron IDs)
    std::vector<uint64_t> pattern2;   // Second pattern (neuron IDs)
    float strength;                   // Connection strength
    uint64_t lastUsedStep;            // When last used
    float associations;               // Association count/frequency
};

} // namespace nlm