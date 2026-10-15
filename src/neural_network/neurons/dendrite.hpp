#pragma once

#include "neuron_base.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * Dendritic Processing Units
 * 
 * Multi-compartment dendritic model for:
 * - Local dendritic computation
 * - Branch-specific synaptic integration
 * - Dendritic nonlinearity
 * - Spike initiation in dendrites
 */
class Dendrite : public EnhancedNeuron {
public:
    Dendrite(NeuronId id);
    ~Dendrite() override;

    // ===== DENDRITIC PARAMETERS =====

    // Single-compartment or multi-compartment model
    enum class CompartmentType {
        SINGLE,           // Single-compartment model
        RALL,            // Equivalent cable model
        compartmental,   // Full compartmental model
        NEURON,          // Multi-neuron dendritic arbor
    };

    // Dendritic cable properties
    struct CableProperties {
        float length;        // Dendritic length (μm)
        float diameter;      // Dendritic diameter (μm)
        float resistivity;   // Axial resistivity (Ω·cm)
        float capacitance;   // Membrane capacitance (μF/cm²)
        float conductance;   // Membrane conductance (mS/cm²)
        float reversal;      // Reversal potential (mV)
    };

    // Synaptic location parameters
    struct SynapticLocation {
        float position;      // Position along dendrite (0-1)
        float weight;        // Synaptic weight
        float reversal;      // Reversal potential (mV)
        float tau;           // Synaptic time constant (ms)
    };

    // ===== CABLE MODEL =====

    // Cable equation parameters
    struct CableEquation {
        float alpha;         // Attenuation constant
        float beta;          // Decay constant
        float gamma;         // Integration constant
        float delta;         // Nonlinearity constant
    };

    // Solve cable equation for one time step
    float solveCableEquation(float input, float dt) const;

    // Spatial integration along dendrite
    float spatialIntegration(const std::vector<float>& potentials, float dt) const;

    // ===== NONLINEAR DENDRITIC DYNAMICS =====

    // Dendritic nonlinearity functions
    enum class NonlinearityType {
        LINEAR,
        SIGMOIDAL,
        THRESHOLD_LINEAR,
        RECTIFIED_LINEAR,
        EXPONENTIAL,
        GAUSSIAN,
        SIPHON,              // Siphon-like nonlinearity
        BRANCHING,           // Branching nonlinearity
    };

    NonlinearityType getNonlinearityType() const { return nonlinearityType_; }
    void setNonlinearityType(NonlinearityType type) { nonlinearityType_ = type; }

    // Apply nonlinearity to potential
    float applyNonlinearity(float potential) const;

    // Nonlinearity parameters
    struct NonlinearityParameters {
        float threshold;     // Threshold potential (mV)
        float gain;          // Nonlinearity gain
        float saturation;    // Saturation level (mV)
        float base;          // Offset/bias
        float width;         // Width parameter for sigmoid
    };

    void setNonlinearityParameters(const NonlinearityParameters& params) { 
        nonlinearityParams_ = params; 
    }

    // ===== BRANCH-SPECIFIC DYNAMICS =====

    // Multiple dendritic branches
    struct DendriticBranch {
        float length;        // Branch length (μm)
        float diameter;      // Branch diameter (μm)
        float resistance;    // Axial resistance (Ω·cm)
        float capacitance;   // Membrane capacitance (μF/cm²)
        float conductance;   // Membrane conductance (mS/cm²)
        float reversal;      // Reversal potential (mV)
        
        // Branch-specific state
        float potential;     // Membrane potential
        float current;       // Axial current
        float input;         // Total synaptic input
        
        // Synaptic density along branch
        std::vector<float> synapticDensities;
    };

    std::vector<DendriticBranch> branches_;

    // Add dendritic branch
    void addBranch(float length, float diameter, float resistance,
                   float capacitance, float conductance, float reversal);

    // ===== DENDRITIC SPIKING =====

    // Spike initiation in dendrites
    struct DendriticSpiking {
        float dendriticThreshold;     // Dendritic spike threshold (mV)
        float somaticThreshold;       // Somatic spike threshold (mV)
        float dendriticReset;         // Dendritic reset potential (mV)
        float dendriticRefractory;    // Dendritic refractory period (ms)
        float couplingStrength;       // Dendritic-somatic coupling
    };

    void setDendriticSpiking(const DendriticSpiking& params) { 
        dendriticSpiking_ = params; 
    }

    // Dendritic spike state
    bool isDendriticSpiking() const { return dendriticSpikingActive_; }
    void setDendriticSpiking(bool active) { dendriticSpikingActive_ = active; }
    void recordDendriticSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getDendriticSpikeHistory() const { 
        return dendriticSpikeHistory_; 
    }

    // ===== BACKWARD COMPATIBILITY =====

    // Override EnhancedNeuron::step
    void step(Timestamp currentTime) override {
        stepAdvanced(currentTime, 1.0f); // Default dt = 1.0ms
    }

    // ===== DENDRITIC-SOMATIC INTEGRATION =====

    // Integrate dendritic potentials into soma
    float integrateDendriticToSomatic() const;

    // Calculate dendritic-somatic coupling
    float calculateCoupling() const;

    // Update dendritic dynamics
    void updateDendriticDynamics(TimestepDuration dt);

    // Main enhanced step function
    bool stepAdvanced(Timestamp currentTime, TimestepDuration dt);

private:
    // Configuration
    CompartmentType compartmentType_;
    CableProperties cableProps_;
    CableEquation cableEq_;
    
    // Nonlinearity
    NonlinearityType nonlinearityType_;
    NonlinearityParameters nonlinearityParams_;

    // Spiking parameters
    DendriticSpiking dendriticSpiking_;
    bool dendriticSpikingActive_;
    std::vector<Timestamp> dendriticSpikeHistory_;

    // Helper methods
    float calculateSynapticInput(const SynapticLocation& synLoc) const;
    float solveRallEquation(const std::vector<float>& potentials, float dt) const;
    float calculateBranchResistance(float length, float diameter) const;
    bool checkDendriticSpikeCondition(float potential) const;
    float computeDendriticCoupling(const DendriticBranch& branch) const;
};

} // namespace nlm
