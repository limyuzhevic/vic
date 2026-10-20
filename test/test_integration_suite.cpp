#include "test_suite_runner.hpp"
#include "test_phase2_integration.cpp"
#include "test_phase3_integration.cpp"
#include "test_phase4_integration.cpp"
#include "test_phase5_integration.cpp"
#include "test_phase6_integration.cpp"

int main() {
    nlm::integration_test::TestSuiteRunner runner;
    
    // Add all integration tests
    runner.addTest(std::make_unique<nlm::integration_test::Phase2IntegrationTest>());
    runner.addTest(std::make_unique<nlm::integration_test::Phase3IntegrationTest>());
    runner.addTest(std::make_unique<nlm::integration_test::Phase4IntegrationTest>());
    runner.addTest(std::make_unique<nlm::integration_test::Phase5IntegrationTest>());
    runner.addTest(std::make_unique<nlm::integration_test::Phase6IntegrationTest>());
    
    // Run all tests
    auto results = runner.runAllTests();
    
    // Return success if all tests passed
    for (const auto& result : results) {
        if (!result.passed) {
            return 1;
        }
    }
    
    return 0;
}