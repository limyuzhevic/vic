#pragma once

#include <memory>
#include <string>
#include "brain/Brain.hpp"

namespace nlm {

/**
 * Run basic neural connectivity test
 * 
 * This function tests fundamental neural computation by injecting current
 * into neurons and verifying that spikes propagate through the network.
 * 
 * @param brain Shared pointer to the brain instance to test
 * @note Extracted from main.cpp for modularity and testability
 *       The test verifies basic LIF neuron dynamics and spike transmission.
 */
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);

} // namespace nlm
