// NLM (熙然) - Neural Learning Machine
// Phase 2 Demo Utility Functions
//
// This header declares utility functions for the Phase 2 demo,
// providing test functions separated from the main demo logic.

#pragma once

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"

namespace nlm {

/**
 * Run basic neural connectivity test
 * 
 * This test verifies that spikes can propagate through the neural network.
 * It injects current into neurons and checks for spike propagation.
 * 
 * @param brain Shared pointer to the brain to test
 */
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);

/**
 * Run STDP verification test
 * 
 * This test verifies that spike-timing dependent plasticity (STDP)
 * produces the expected synaptic weight changes when presynaptic
 * neurons fire before postsynaptic neurons.
 * 
 * @param brain Shared pointer to the brain to test
 */
void runStdpVerification(std::shared_ptr<Brain> brain);

} // namespace nlm
