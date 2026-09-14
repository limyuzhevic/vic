#pragma once

namespace nlm {

// Neuron configuration constants
struct NeuronConfig {
    // Membrane parameters (already defined in Neuron.cpp, but consolidating here)
    static constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float DEFAULT_TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t DEFAULT_MAX_SPIKE_HISTORY = 100;
    
    // Default physiological parameters
    static constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;  // mV
    static constexpr float DEFAULT_THRESHOLD = -55.0f;  // mV
    static constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;  // mV
    static constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;  // nS
    static constexpr float DEFAULT_ADAPTATION_INCREMENT = 1.0f;
    static constexpr float DEFAULT_RANDOM_RANGE = 3.0f;
    static constexpr float DEFAULT_THRESHOLD_BASE = -55.0f;  // mV
    static constexpr float DEFAULT_RESTING_POTENTIAL_BASE = -70.0f;  // mV
    static constexpr float DEFAULT_ADAPTATION_RESET = 0.0f;
    static constexpr float DEFAULT_LAST_SPIKE_TIME = -1.0f;
    static constexpr float DEFAULT_SYNAPTIC_INPUT_RESET = 0.0f;
    
    // Clamping limits
    static constexpr float MEMBRANE_POTENTIAL_CLAMP_MIN = -100.0f;  // mV
    static constexpr float MEMBRANE_POTENTIAL_CLAMP_MAX = 50.0f;  // mV
    
    // Spike-frequency adaptation constants
    static constexpr float ADAPTATION_DECAY_RATE = 0.95f;
    static constexpr float ADAPTATION_CORRECTION_FACTOR = 0.01f;
};

} // namespace nlm
