#pragma once

#include "PlasticityRule.hpp"
#include "RewardModulatedSTDP.hpp"
#include "TripletSTDP.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Real implementation based on spike timing correlation
// Implements standard STDP with exponential learning windows

// Reward-modulated STDP (R-STDP)
// STDP modulated by reward signals for reinforcement learning
// Combines temporal difference learning with reward prediction

// Spike-timing dependent plasticity with triplet interactions
// Detailed STDP implementation including triplet spike interactions
// Captures more complex biological STDP dynamics

// Note: The actual implementations are in:
// - STDP.cpp (standard STDP)
// - RewardModulatedSTDP.cpp (reward-modulated STDP)
// - TripletSTDP.cpp (triplet STDP)

} // namespace nlm
