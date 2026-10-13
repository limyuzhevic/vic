// Neuron Parameters
// Core neuron constants for membrane potentials and thresholds

#ifndef SRC_CORE_NEURAL_CONSTANTS_HPP
#define SRC_CORE_NEURAL_CONSTANTS_HPP

#include <cfloat>
#include <limits>

namespace nlm {

namespace NeuralConstants {
    // Membrane potentials
    static constexpr float RESTING_POTENTIAL = -70.0f;        // Resting membrane potential (mV)
    static constexpr float RESET_POTENTIAL = -70.0f;         // Reset membrane potential (mV)
    static constexpr float THRESHOLD_POTENTIAL = -55.0f;      // Spike threshold (mV)
    static constexpr float ADAPTATION_DECAY_FACTOR = 0.95f;   // Adaptation variable decay
    
    // Clamping ranges
    static constexpr float MIN_VOLTAGE = -100.0f;              // Minimum voltage clamp (mV)
    static constexpr float MAX_VOLTAGE = 50.0f;               // Maximum voltage clamp (mV)
    
    // Time constants
    static constexpr float MEMBRANE_TIME_CONSTANT = 20.0f;    // Membrane time constant (ms)
    static constexpr float MEMBRANE_RESISTANCE = 10.0f;       // Membrane resistance (MOhm)
    
    // Refractoriness
    static constexpr float REFRACTORY_PERIOD = 2.0f;          // Refractory period (ms)
    
    // Adaptation
    static constexpr float AMPLITUDE_ADAPTATION_RATE = 0.1f;   // Amplitude adaptation rate
    
    // Spike detection
    static constexpr float SPIKE_HISTORY_THRESHOLD = 0.001f;   // Minimum absolute eligibility for spike recording
    
    // Voltage integration
    static constexpr float VOLTAGE_INTEGRATION_FACTOR = 1000.0f;  // Voltage integration factor
    
    // Reset range
    static constexpr float RESET_POTENTIAL_RANGE = 5.0f;       // Reset potential range (mV)
    
    // Brain constants
    static constexpr float LEAK_CONDUCTANCE = 10.0f;           // Leak conductance (nS)
}

} // namespace nlm

#endif // SRC_CORE_NEURAL_CONSTANTS_HPP