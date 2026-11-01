#include "test_runner.h"
#include "main.cpp"  // Include main.cpp for test functions

namespace nlm {

// Implementation of Phase2TestRunner methods

std::shared_ptr<Brain> Phase2TestRunner::createBrain() {
    auto brain = std::make_shared<Brain>(config_);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain in test runner");
        return nullptr;
    }
    return brain;
}

bool Phase2TestRunner::resetBrain(std::shared_ptr<Brain>& brain) {
    if (!brain) {
        brain = createBrain();
        return brain != nullptr;
    }
    
    brain->reset();
    return brain->initialize();
}

bool Phase2TestRunner::runTest(const TestInfo& test, std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== " + test.name + " ===");
    NLM_LOG_INFO(test.description);
    
    // Setup phase
    if (test.setup) {
        test.setup(brain);
    }
    
    // Execute phase
    if (test.execute) {
        test.execute(brain);
    }
    
    // Teardown phase
    if (test.teardown) {
        test.teardown(brain);
    }
    
    // Reset brain if required
    if (test.requiresBrainReset && resetBrain(brain)) {
        NLM_LOG_INFO("Test completed, brain reset for next test");
    }
    
    return true;
}

bool Phase2TestRunner::runTests(const std::vector<TestInfo>& tests) {
    auto brain = createBrain();
    if (!brain) {
        NLM_LOG_ERROR("Failed to create initial brain");
        return false;
    }
    
    NLM_LOG_INFO("Starting Phase 2 test suite with " + std::to_string(tests.size()) + " tests");
    
    for (size_t i = 0; i < tests.size(); ++i) {
        const auto& test = tests[i];
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Running test " + std::to_string(i + 1) + "/" + std::to_string(tests.size()) + ": " + test.name);
        
        bool testSuccess = runTest(test, brain);
        if (!testSuccess) {
            NLM_LOG_ERROR("Test " + test.name + " failed");
            return false;
        }
        
        if (i < tests.size() - 1) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Preparing for next test...");
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("All tests completed successfully");
    return true;
}

} // namespace nlm