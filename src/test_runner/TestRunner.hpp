// NLM (熙然) - Neural Learning Machine
// Implementation: Test Runner
// Orchestrates test execution with progress tracking and error handling

#ifndef NLM_TEST_RUNNER_TESTRUNNER_HPP
#define NLM_TEST_RUNNER_TESTRUNNER_HPP

#include <memory>
#include <string>

namespace nlm::test_runner {

enum class TestMode {
    BasicConnectivity,
    PlasticityLearning,
    StdpVerification,
    All
};

class TestRunner {
public:
    void runTest(TestMode mode, std::shared_ptr<Brain> brain);
    void runAllTests(std::shared_ptr<Brain> brain);
    static TestRunner& getInstance();
    
private:
    TestRunner() = default;
};

} // namespace nlm::test_runner

#endif // NLM_TEST_RUNNER_TESTRUNNER_HPP
