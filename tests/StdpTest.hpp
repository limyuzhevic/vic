// NLM (熙然) - Neural Learning Machine
// Test Module: STDP Verification
// This test validates Spike-Timing Dependent Plasticity mechanisms

#ifndef NLM_TESTS_STDP_TEST_HPP
#define NLM_TESTS_STDP_TEST_HPP

#include <memory>
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

namespace nlm::tests {

class StdpTest {
public:
    static void run(std::shared_ptr<Brain> brain);
    static void runWithPrecision(std::shared_ptr<Brain> brain, int precision);
    static bool verifyLtp(std::shared_ptr<Brain> brain);
    static bool verifyLtd(std::shared_ptr<Brain> brain);
};

} // namespace nlm::tests

#endif // NLM_TESTS_STDP_TEST_HPP
