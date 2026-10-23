// BrainConnectivityTest: Tests spike propagation through neural network
//
// This test verifies that neural connectivity works properly by injecting
// current into neurons and observing spike propagation.

#ifndef NLM_BRAIN_CONNECTIVITY_TEST_HPP
#define NLM_BRAIN_CONNECTIVITY_TEST_HPP

#include <brain/Brain.hpp>
#include <brain/Neuron.hpp>
#include <core/SimulationClock/SimulationClock.hpp>
#include <core/Config/Config.hpp>

namespace nlm {
namespace tests {

class BrainConnectivityTest {
public:
    static void run(std::shared_ptr<Brain> brain);
    static void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
};

} // namespace tests
} // namespace nlm

#endif // NLM_BRAIN_CONNECTIVITY_TEST_HPP
