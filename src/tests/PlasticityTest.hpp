// PlasticityTest: Tests learning through experience
//
// This test demonstrates measurable synaptic changes through repeated
// sensory input patterns, showing how the brain adapts through learning.

#ifndef NLM_PLASTICITY_TEST_HPP
#define NLM_PLASTICITY_TEST_HPP

#include <brain/Brain.hpp>
#include "LearningExperiment.hpp"

namespace nlm {
namespace tests {

class PlasticityTest {
public:
    static void run(std::shared_ptr<Brain> brain);
    static void runPlasticityExperiment(std::shared_ptr<Brain> brain);
};

} // namespace tests
} // namespace nlm

#endif // NLM_PLASTICITY_TEST_HPP
