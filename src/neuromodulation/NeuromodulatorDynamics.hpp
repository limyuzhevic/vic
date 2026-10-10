#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "Neuromodulator.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Forward declarations
class NeuromodulationEngine;

// Neuromodulator state variables
struct NeuromodulatorStateVars {
    float level;                    // Current level
    float target;                   // Target level
    float accumulation;             // Accumulated input
    float decayRate;                // Decay rate
    float productionRate;            // Production rate
    float inhibitionRate;            // Inhibition rate
    float homeostasisValue;          // Homeostatic set point
    float plasticityInfluence;       // Plasticity modulation influence
    float adaptationSpeed;           // Adaptation speed
    float noiseLevel;                // Noise level
    float saturation;                // Saturation level
    float timeConstant;              // Time constant for dynamics
    float externalInput;             // External input
    float internalInput;             // Internal input
    
    NeuromodulatorStateVars()
        : level(0.0f), target(0.0f), accumulation(0.0f), decayRate(0.1f),
          productionRate(0.0f), inhibitionRate(0.0f), homeostasisValue(0.5f),
          plasticityInfluence(1.0f), adaptationSpeed(0.01f), noiseLevel(0.0f),
          saturation(1.0f), timeConstant(0.5f), externalInput(0.0f), internalInput(0.0f) {}
};

// State transition parameters
struct StateTransition {
    float timeStep;                 // Time step for transition
    float inputCurrent;             // Input current
    float voltage;                  // Membrane potential
    float gateVariable;             // Gate variable
    float adaptationVariable;       // Adaptation variable
    float noise;                    // Noise component
    
    StateTransition()
        : timeStep(0.0f), inputCurrent(0.0f), voltage(0.0f), gateVariable(0.0f),
          adaptationVariable(0.0f), noise(0.0f) {}
};

// Temporal dynamics parameters
struct TemporalDynamics {
    float intrinsicDynamics;         // Intrinsic dynamics
    float externalForcing;           // External forcing
    float couplingStrength;          // Coupling strength
    float phase;                     // Phase
    float frequency;                 // Frequency
    float amplitude;                 // Amplitude
    float damping;                   // Damping
    float nonlinearity;              // Nonlinearity coefficient
    
    TemporalDynamics()
        : intrinsicDynamics(1.0f), externalForcing(0.0f), couplingStrength(0.0f),
          phase(0.0f), frequency(1.0f), amplitude(1.0f), damping(0.1f),
          nonlinearity(0.0f) {}
};

// State space variables
struct StateSpace {
    std::vector<float> state;        // State vector
    std::vector<float> derivative;   // State derivative
    std::vector<float> Jacobian;     // Jacobian matrix
    std::vector<float> eigenvalues;  // Eigenvalues
    std::vector<float> eigenvectors; // Eigenvectors
    
    StateSpace() {
        state.resize(5, 0.0f);   // Default 5-state system
        derivative.resize(5, 0.0f);
        Jacobian.resize(25, 0.0f);  // 5x5 matrix
        eigenvalues.resize(5, 0.0f);
        eigenvectors.resize(25, 0.0f);  // 5x5 matrix
    }
};

// Bifurcation analysis results
struct BifurcationAnalysis {
    float bifurcationPoint;           // Bifurcation point
    float stabilityMargin;           // Stability margin
    float oscillationAmplitude;       // Oscillation amplitude
    float limitCycleFrequency;       // Limit cycle frequency
    bool hasLimitCycle;               // Whether system has limit cycle
    bool isChaotic;                   // Whether system is chaotic
    float LyapunovExponent;           // Lyapunov exponent
    float fractalDimension;           // Fractal dimension
    
    BifurcationAnalysis()
        : bifurcationPoint(0.0f), stabilityMargin(1.0f), oscillationAmplitude(0.0f),
          limitCycleFrequency(0.0f), hasLimitCycle(false), isChaotic(false),
          LyapunovExponent(0.0f), fractalDimension(2.0f) {}
};

// Neuromodulator dynamics: State dynamics and evolution
class NeuromodulatorDynamics {
public:
    NeuromodulatorDynamics();
    ~NeuromodulatorDynamics();
    
    // Disable copying, enable moving
    NeuromodulatorDynamics(const NeuromodulatorDynamics&) = delete;
    NeuromodulatorDynamics& operator=(const NeuromodulatorDynamics&) = delete;
    NeuromodulatorDynamics(NeuromodulatorDynamics&&) noexcept;
    NeuromodulatorDynamics& operator=(NeuromodulatorDynamics&&) noexcept;
    
    // Initialization
    bool initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine);
    
    // Main update step
    void update(TimestepDuration dt, SimulationStep step);
    
    // State dynamics computation
    void computeStateDynamics(const NeuromodulatorStateVars& currentState,
                             NeuromodulatorStateVars& nextState, TimestepDuration dt);
    void computeFiringRateDynamics(float& firingRate, const NeuromodulatorStateVars& state, TimestepDuration dt);
    void computeIntrinsicDynamics(float& intrinsic, const NeuromodulatorStateVars& state, TimestepDuration dt);
    void computeExternalDynamics(float& external, const NeuromodulatorStateVars& state, TimestepDuration dt);
    
    // Temporal dynamics analysis
    void analyzeTemporalDynamics();
    void computeTemporalProperties(TemporalDynamics& temporal, const NeuromodulatorStateVars& state);
    void computeStateSpace(StateSpace& stateSpace, const NeuromodulatorStateVars& state);
    
    // Bifurcation analysis
    void performBifurcationAnalysis();
    void computeBifurcationParameters(BifurcationAnalysis& analysis, const NeuromodulatorStateVars& state);
    void detectBifurcations(const std::vector<float>& parameters);
    
    // Stability analysis
    void analyzeStability();
    void computeStabilityMetrics(float& stabilityMargin, float& recoveryTime);
    void computeEigenvalues(StateSpace& stateSpace);
    bool isStable(const StateSpace& stateSpace) const;
    
    // Nonlinear dynamics
    void computeNonlinearDynamics(float& result, const NeuromodulatorStateVars& state);
    void computeHopfBifurcation(float& bifurcationParameter, const NeuromodulatorStateVars& state);
    void computeSaddleNodeBifurcation(float& bifurcationParameter, const NeuromodulatorStateVars& state);
    
    // State management
    void updateState(const std::string& neuromodulatorName, const NeuromodulatorStateVars& state);
    NeuromodulatorStateVars getState(const std::string& neuromodulatorName) const;
    
    // Configuration
    void setTimeConstant(float constant) { pImpl->defaultTimeConstant = constant; }
    void setNoiseLevel(float level) { pImpl->defaultNoiseLevel = level; }
    void setAdaptationSpeed(float speed) { pImpl->defaultAdaptationSpeed = speed; }
    void enableBifurcationAnalysis(bool enable) { pImpl->enableBifurcationAnalysis_ = enable; }
    void enableTemporalAnalysis(bool enable) { pImpl->enableTemporalAnalysis_ = enable; }
    
    // Integration with other systems
    void integrateWithNetwork(const class NeuromodulatorNetwork& network);
    void integrateWithLearning(const class PlasticityController& plasticity);
    void integrateWithSynthesis(const class NeuromodulatorSynthesis& synthesis);
    
    // State space simulation
    void simulateStateSpace(const StateSpace& initialState, std::vector<StateSpace>& trajectory);
    void computeLimitCycle(const StateSpace& initialState, StateSpace& limitCycle);
    void computeAttractorBasin(const StateSpace& attractor, std::vector<StateSpace>& basin);
    
    // Analysis and diagnostics
    float computeGlobalStability() const;
    float computeRecoveryTime() const;
    float computeOscillationAmplitude() const;
    float computeFractalDimension() const;
    
    // Export/import
    void exportDynamics(std::ostream& stream) const;
    void importDynamics(std::istream& stream);
    
    // Testing interface
    float getStateLevel(const std::string& neuromodulatorName) const;
    float getTemporalStability() const { return pImpl->temporalStability; }
    float getBifurcationDistance() const { return pImpl->bifurcationDistance; }
    bool isDynamicallyStable() const { return pImpl->isStable; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<NeuromodulationEngine> engine_;
    
    float defaultTimeConstant;
    float defaultNoiseLevel;
    float defaultAdaptationSpeed;
    
    bool enableBifurcationAnalysis_;
    bool enableTemporalAnalysis_;
    
    // Analysis metrics
    float temporalStability;
    float bifurcationDistance;
    bool isStable;
};

} // namespace nlm