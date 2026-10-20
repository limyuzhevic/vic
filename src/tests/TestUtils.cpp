#include "tests/TestUtils.hpp"
#include "experiments/LearningExperiment.hpp"

// Re-export test functions for backward compatibility
void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    nlm::test::TestUtils::runConnectivityTest(brain);
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    nlm::test::TestUtils::runPlasticityTest(brain);
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    nlm::test::TestUtils::runStdpTest(brain);
}