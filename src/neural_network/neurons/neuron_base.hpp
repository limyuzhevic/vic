#pragma once

#include "../brain/Neuron.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <string>

namespace nlm {

/**
 * EnhancedNeuron - Base class for advanced neuron models
 * 
 * Extends the basic Neuron class with additional features:
 * - Ion channel dynamics
 * - Dendritic processing
 * - Stochastic dynamics
 * - Adaptive properties
 * - Resonant behavior
 * 
 * Maintains backward compatibility with existing Neuron interface
 */
class EnhancedNeuron : public Neuron {
public:
    EnhancedNeuron(NeuronId id);
    ~EnhancedNeuron() override;

    // ===== ENHANCED DYNAMICS =====

    // Ion channel dynamics (Hodgkin-Huxley style)
    struct IonChannelState {
        float m;      // Activation variable
        float h;      // Inactivation variable
        float n;      // Fast activation variable
        float t;      // Time variable for adaptation
        float s;      // Slow adaptation variable
    };

    // Get/set ion channel state
    const IonChannelState& getIonChannelState() const { return ionChannelState_; }
    void setIonChannelState(const IonChannelState& state) { ionChannelState_ = state; }

    // Update ion channels
    void updateIonChannels(TimestepDuration dt);

    // ===== DENDRITIC PROCESSING =====

    // Dendritic compartment model
    struct Dendrite {
        float potential;      // Dendritic membrane potential
        float inputWeight;    // Synaptic input weight
        float decay;          // Membrane time constant
        std::vector<float> synapticInputs;  // List of incoming synaptic inputs
    };

    // Multiple dendritic branches
    std::vector<Dendrite> dendrites_;

    // Add dendritic branch
    void addDendrite(float decay = 10.0f, float weight = 1.0f);

    // Process dendritic input
    void processDendriticInput(float input);

    // Integrate dendritic potentials into soma
    float integrateDendriticPotentials() const;

    // ===== STOCHASTIC DYNAMICS =====

    // Noise parameters for stochastic behavior
    struct StochasticParameters {
        float membraneNoise;      // Voltage noise (mV/√s)
        float spikeNoise;        // Spike timing noise (probability)
        float inputNoise;        // Synaptic input noise
        float thresholdNoise;    // Threshold fluctuation
    };

    StochasticParameters stochasticParams_;

    // Enable stochastic dynamics
    void enableStochasticDynamics(const StochasticParameters& params);

    // Generate stochastic current
    float generateStochasticCurrent(TimestepDuration dt) const;

    // ===== ADAPTIVE PROPERTIES =====

    // Adaptive exponential integrate-and-fire parameters
    struct AdaptiveParameters {
        float a;       // Subthreshold adaptation
        float b;       // Spike-triggered adaptation
        float tau;     // Adaptation time constant
        float v_peak;  // Peak voltage for adaptation
    };

    AdaptiveParameters adaptiveParams_;

    // Adaptive variable
    float adaptiveVariable_;

    // Enable adaptive dynamics
    void enableAdaptiveDynamics(const AdaptiveParameters& params);

    // Update adaptive variable
    void updateAdaptiveVariable(float spikeCount);

    // ===== RESONANT BEHAVIOR =====

    // Resonant parameters (for resonant neuron models)
    struct ResonantParameters {
        float resonanceFreq;     // Resonant frequency (Hz)
        float resonanceQ;        // Quality factor
        float alpha;             // Resonance strength
        float beta;              // Resonance decay
    };

    ResonantParameters resonantParams_;

    // Enable resonant behavior
    void enableResonantBehavior(const ResonantParameters& params);

    // Generate resonant response
    float generateResonantResponse(float inputFrequency, float dt) const;

    // ===== ENHANCED PLASTICITY =====

    // Neuromodulatory effects on plasticity
    struct NeuromodulationEffects {
        float dopamineFactor;    // Dopamine modulation
        float acetylcholineFactor;  // ACh modulation
        float norepinephrineFactor; // NE modulation
        float serotoninFactor;   // 5-HT modulation
    };

    NeuromodulationEffects neuromodEffects_;

    // Apply neuromodulatory effects to plasticity
    void applyNeuromodulatoryPlasticity(float dopamine, float acetylcholine, 
                                       float norepinephrine, float serotonin);

    // ===== PLACEMENT SPECIFIC =====

    // Neuron placement in 3D space
    struct Position {
        float x;  // X coordinate (μm)
        float y;  // Y coordinate (μm)
        float z;  // Z coordinate (μm)
    };

    Position position_;

    void setPosition(float x, float y, float z) {
        position_ = {x, y, z};
    }

    const Position& getPosition() const { return position_; }

    // Distance to another position
    float distanceTo(const Position& other) const {
        float dx = position_.x - other.x;
        float dy = position_.y - other.y;
        float dz = position_.z - other.z;
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    }

    // ===== EXTRAS =====

    // Burst detection
    bool isInBurst() const { return burstCounter_ > 0; }
    void recordBurst() { burstCounter_++; }
    void resetBurstCounter() { burstCounter_ = 0; }

    // Check for resonant spiking
    bool shouldResonate(float input, float dt) const;

    // Enhanced step function (hybrid model)
    bool stepEnhanced(Timestamp currentTime, TimestepDuration dt);

    // ===== BACKWARD COMPATIBILITY =====

    // Override original Neuron::step for compatibility
    void step(Timestamp currentTime) override {
        stepEnhanced(currentTime, 1.0f); // Default dt = 1.0ms
    }

private:
    IonChannelState ionChannelState_;
    float burstCounter_;
};

} // namespace nlm
