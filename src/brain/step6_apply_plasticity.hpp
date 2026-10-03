// Step 6: Apply plasticity rules (STDP and Hebbian)
// This module applies synaptic plasticity rules including STDP and Hebbian learning
// Implements activity-dependent synaptic weight changes

#pragma once

#include <memory>
#include <vector>
#include "../brain/NeuralRegion.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step6 {

// Helper function to apply STDP with neuromodulation
void ApplySTDPWithNeuromodulation(Brain::Impl* impl,
                                   Synapse* syn,
                                   const std::vector<Timestamp>& preSpikeHistory,
                                   const std::vector<Timestamp>& postSpikeHistory,
                                   Timestamp currentTime,
                                   float plasticityMod);

// Helper function to apply Hebbian learning
void ApplyHebbianLearning(Brain::Impl* impl,
                          Synapse* syn,
                          const std::vector<Timestamp>& preSpikeHistory,
                          const std::vector<Timestamp>& postSpikeHistory,
                          Timestamp currentTime);

// Helper function to update synapse eligibility trace
void UpdateSynapseEligibilityTrace(Synapse* syn, Timestamp currentTime);

// Apply plasticity rules to all synapses
void ApplyPlasticity(Brain::Impl* impl,
                     SimulationStep currentStep,
                     Timestamp currentTime);

// Optimize plasticity application using batch processing
void OptimizedPlasticityApplication(Brain::Impl* impl,
                                    SimulationStep currentStep,
                                    Timestamp currentTime);

} // namespace Step6

} // namespace nlm