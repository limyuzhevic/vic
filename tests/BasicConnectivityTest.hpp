// NLM (熙然) - Neural Learning Machine
// Test Module: Basic Neural Connectivity
// This test validates that neural connections can propagate spikes through the network

#ifndef NLM_TESTS_BASIC_CONNECTIVITY_TEST_HPP
#define NLM_TESTS_BASIC_CONNECTIVITY_TEST_HPP

#include <memory>
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"

namespace nlm::tests {

class BasicConnectivityTest {
public:
    static void run(std::shared_ptr<Brain> brain);
    static void demonstrate(std::shared_ptr<Brain> brain);
};

} // namespace nlm::tests

#endif // NLM_TESTS_BASIC_CONNECTIVITY_TEST_HPP
