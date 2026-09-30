#include "NeuronInitializer.hpp"

namespace nlm {

NeuronInitializer::NeuronInitializer() = default;

NeuronInitializer::ParameterBounds NeuronInitializer::bounds_;

void NeuronInitializer::initializeParameters(RandomGenerator& rng, NeuronState& state) {
    randomizeMembranePotential(rng, state);
    randomizeThreshold(rng, state);
    randomizeRestingPotential(rng, state);
    randomizeResetPotential(rng, state);
    randomizeRefractoryPeriod(rng, state);
    
    // Initialize other state variables
    state.firingState = FiringState::Resting;
    state.refractoryRemaining = 0;
    state.adaptationVariable = 0.0f;
    state.lastSpikeTime = -1.0f;
    state.firingRate = 0.0f;
}

void NeuronInitializer::randomizeMembranePotential(RandomGenerator& rng, NeuronState& state) {
    state.membranePotential = state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    state.membranePotential = std::clamp(state.membranePotential, bounds_.minMembranePotential, bounds_.maxMembranePotential);
}

void NeuronInitializer::randomizeThreshold(RandomGenerator& rng, NeuronState& state) {
    state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    state.threshold = std::clamp(state.threshold, bounds_.minThreshold, bounds_.maxThreshold);
}

void NeuronInitializer::randomizeRestingPotential(RandomGenerator& rng, NeuronState& state) {
    state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    state.restingPotential = std::clamp(state.restingPotential, bounds_.minRestingPotential, bounds_.maxRestingPotential);
}

void NeuronInitializer::randomizeResetPotential(RandomGenerator& rng, NeuronState& state) {
    state.resetPotential = state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    state.resetPotential = std::clamp(state.resetPotential, bounds_.minResetPotential, bounds_.maxResetPotential);
}

void NeuronInitializer::randomizeRefractoryPeriod(RandomGenerator& rng, NeuronState& state) {
    state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(bounds_.minRefractoryPeriod, bounds_.maxRefractoryPeriod));
}

} // namespace nlm