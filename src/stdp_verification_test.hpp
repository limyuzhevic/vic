#pragma once

#include <memory>
#include <string>
#include <vector>
#include "brain/Brain.hpp"

namespace nlm {

/**
 * Run STDP verification test
 * 
 * This function verifies spike-timing dependent plasticity (STDP) by creating
 * correlated pre->post neural activity and measuring weight changes.
 * 
 * @param brain Shared pointer to the brain instance to test
 * @note Extracted from main.cpp for better modularity and testability.
 *       Isolated STDP verification to enable independent testing of the
 *       STDP mechanism without involving full experimental workflows.
 */
void runStdpVerification(std::shared_ptr<Brain> brain);

} // namespace nlm
