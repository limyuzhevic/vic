// Fix for Config.cpp - safer seed access
uint64_t seed = 42;
if (auto seedOpt = config->get<uint64_t>("random_seed")) {
    // Check if the value is valid before using it
    // The get<>() method should return a valid value or throw/bad value
    // In a safe implementation, we should validate the range
    seed = *seedOpt;
}

// Additional safety checks for other config values
if (auto neuronCountOpt = config->get<int64_t>("neuron_count")) {
    int64_t neuronCount = *neuronCountOpt;
    // Validate reasonable bounds
    if (neuronCount > 0 && neuronCount < 1000000) {
        config->set("neuron_count", neuronCount);
    }
}