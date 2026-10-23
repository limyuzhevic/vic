// StdpTest: Tests STDP (Spiking Timing-Dependent Plasticity) mechanism
//
// This test specifically verifies that the STDP plasticity rule works correctly
// by creating correlated pre-post neuronal activity and measuring synaptic changes.

#ifndef NLM_STDP_TEST_HPP
#define NLM_STDP_TEST_HPP

#include <brain/Brain.hpp>
#include <brain/Neuron.hpp>
#include <core/SimulationClock/SimulationClock.hpp>

namespace nlm {
namespace tests {

class StdpTest {
public:
    static void run(std::shared_ptr<Brain> brain);
    static void runStdpVerification(std::shared_ptr<Brain> brain);
};

} // namespace tests
} // namespace nlm

#endif // NLM_STDP_TEST_HPP
