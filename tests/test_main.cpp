// NLM Test Main
// Phase 2: Real Neural Computation Tests - Using Modern Test Framework

#include "nlm_test_framework.h"
#include <iostream>
#include <cassert>

// Forward declarations of test functions
namespace test_types {
    void runAll();
}

namespace test_config {
    void runAll();
}

namespace test_random {
    void runAll();
}

namespace test_clock {
    void runAll();
}

namespace test_neuron {
    void runAll();
}

namespace test_synapse {
    void runAll();
}

namespace test_stdp {
    void runAll();
}

namespace test_brain {
    void runAll();
}

// Legacy test adapter - create simple test cases from existing test functions
class LegacyTestCase : public nlm::test::TestCase {
public:
    std::function<void()> testFunction;
    
    LegacyTestCase(const std::string& name, std::function<void()> func, const std::string& category = "Compatibility")
        : TestCase(name, category), testFunction(func) {}
    
    virtual nlm::test::TestResult run() {
        nlm::test::TestResult result(name, category);
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            testFunction();
            result.passed = true;
        } catch (const std::exception& e) {
            result.passed = false;
            result.failureMessage = e.what();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        return result;
    }
};

int main() {
    std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    // Initialize the test framework
    nlm::test::initializeTestSuite();
    
    // Create legacy test cases for backward compatibility
    auto typesTest = std::make_unique<LegacyTestCase>("Types Tests", []() { test_types::runAll(); });
    nlm::test::TestSuite::getInstance().registerTest(std::move(typesTest));
    
    auto configTest = std::make_unique<LegacyTestCase>("Config Tests", []() { test_config::runAll(); });
    nlm::test::TestSuite::getInstance().registerTest(std::move(configTest));
    
    auto randomTest = std::make_unique<LegacyTestCase>("Random Tests", []() { test_random::runAll(); });
    nlm::test::TestSuite::getInstance().registerTest(std::move(randomTest));
    
    auto clockTest = std::make_unique<LegacyTestCase>("Clock Tests", []() { test_clock::runAll(); });
    nlm::test::TestSuite::getInstance().registerTest(std::move(clockTest));
    
    auto neuronTest = std::make_unique<LegacyTestCase>("Neuron Tests", []() { test_neuron::runAll(); });
    nlm::test::TestSuite::getInstance().registerTest(std::move(neuronTest));
    
    auto synapseTest = std::make_unique<LegacyTestCase>("Synapse Tests", []() { test_synapse::runAll(); });
    nlm::test::TestSuite::getInstance().registerTest(std::move(synapseTest));
    
    auto stdpTest = std::make_unique<LegacyTestCase>("STDP Tests", []() { test_stdp::runAll(); });
    nlm::test::TestSuite::getInstance().registerTest(std::move(stdpTest));
    
    auto brainTest = std::make_unique<LegacyTestCase>("Brain Tests", []() { test_brain::runAll(); });
    nlm::test::TestSuite::getInstance().registerTest(std::move(brainTest));
    
    // Run all tests using the new modern framework
    std::vector<nlm::test::TestResult> results = nlm::test::TestSuite::getInstance().runAllTests();
    
    // Print report
    nlm::test::TestSuite::getInstance().printReport(results);
    
    // Determine final result
    bool allPassed = true;
    for (const auto& result : results) {
        if (!result.passed) {
            allPassed = false;
            break;
        }
    }
    
    return allPassed ? 0 : 1;
}
