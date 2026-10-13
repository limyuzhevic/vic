// TestUtils implementation

#include "TestUtils.hpp"

namespace nlm {
namespace test {
// Test runner implementation - simplified for single brain instance
void runAllTests(std::shared_ptr<Brain> brain, bool verbose = true) {
    if (!utils::validateBrainHealth(brain)) {
        NLM_LOG_ERROR("Brain health validation failed - cannot run tests");
        return;
    }
    
    NLM_LOG_INFO("=== Starting NLM Phase 2 Test Suite ===");
    NLM_LOG_INFO("Brain health: " + utils::formatNeuronInfo(brain));
    NLM_LOG_INFO("Synapse status: " + utils::formatSynapseInfo(brain));
    
    // Create test instances
    BasicConnectivityTest connectivityTest;
    PlasticityExperimentTest plasticityTest;
    StdpVerificationTest stdpTest;
    
    std::vector<std::unique_ptr<TestBase>> tests;
    tests.push_back(std::make_unique<BasicConnectivityTest>(connectivityTest));
    tests.push_back(std::make_unique<PlasticityExperimentTest>(plasticityTest));
    tests.push_back(std::make_unique<StdpVerificationTest>(stdpTest));
    
    std::vector<TestResult> results;
    
    for (auto& test : tests) {
        if (verbose) {
            NLM_LOG_INFO("Running test: " + test->getName());
        }
        
        TestResult result = test->run(brain);
        results.push_back(result);
        
        if (verbose) {
            std::string status = result.passed ? "PASSED" : "FAILED";
            NLM_LOG_INFO("Test " + test->getName() + ": " + status);
            if (!result.message.empty()) {
                NLM_LOG_INFO("  " + result.message);
            }
            if (result.executionTime > 0) {
                NLM_LOG_INFO("  Execution time: " + std::to_string(result.executionTime) + "s");
            }
        }
    }
    
    // Summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test Suite Summary ===");
    
    int passed = 0, failed = 0;
    for (const auto& result : results) {
        if (result.passed) {
            passed++;
            NLM_LOG_INFO("✓ " + result.name + ": PASSED");
        } else {
            failed++;
            NLM_LOG_INFO("✗ " + result.name + ": FAILED");
            if (!result.message.empty()) {
                NLM_LOG_INFO("  Reason: " + result.message);
            }
        }
    }
    
    NLM_LOG_INFO("Total: " + std::to_string(results.size()) + 
                ", Passed: " + std::to_string(passed) + 
                ", Failed: " + std::to_string(failed));
    
    if (verbose) {
        NLM_LOG_INFO("");
        if (failed == 0) {
            NLM_LOG_INFO("All tests passed! ✓");
        } else {
            NLM_LOG_INFO("Some tests failed. Check output for details.");
        }
    }
}

namespace test {
namespace test {

// Utility functions implementation
namespace utils {

    bool validateBrainHealth(std::shared_ptr<Brain> brain) {
        if (!brain) {
            NLM_LOG_ERROR("Brain is null");
            return false;
        }
        
        if (!brain->getSpikeSystem()) {
            NLM_LOG_ERROR("Brain has no spike system");
            return false;
        }
        
        // Check if brain is properly initialized
        size_t neuronCount = brain->getTotalNeuronCount();
        if (neuronCount == 0) {
            NLM_LOG_WARNING("Brain has no neurons");
            return false;
        }
        
        return true;
    }
    
    std::string formatNeuronInfo(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Neurons: " << brain->getTotalNeuronCount()
           << ", Active: " << brain->getActiveNeuronCount()
           << ", Firing: " << brain->getFiringNeuronCount()
           << ", Avg Firing: " << brain->getAverageFiringRate();
        return ss.str();
    }
    
    std::string formatSynapseInfo(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Synapses: " << brain->getTotalSynapseCount()
           << ", E/I Ratio: " << brain->getExcitationInhibitionRatio();
        return ss.str();
    }
    
    BrainStats calculateBrainStats(std::shared_ptr<Brain> brain) {
        BrainStats stats;
        stats.totalNeurons = brain->getTotalNeuronCount();
        stats.totalSynapses = brain->getTotalSynapseCount();
        stats.activeNeurons = brain->getActiveNeuronCount();
        stats.firingNeurons = brain->getFiringNeuronCount();
        stats.averageFiringRate = brain->getAverageFiringRate();
        stats.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        return stats;
    }
    
}} // namespace nlm::test::utils

// Test runner implementation - simplified for single brain instance
void runAllTests(std::shared_ptr<Brain> brain, bool verbose = true) {
    if (!utils::validateBrainHealth(brain)) {
        NLM_LOG_ERROR("Brain health validation failed - cannot run tests");
        return;
    }
    
    NLM_LOG_INFO("=== Starting NLM Phase 2 Test Suite ===");
    NLM_LOG_INFO("Brain health: " + utils::formatNeuronInfo(brain));
    NLM_LOG_INFO("Synapse status: " + utils::formatSynapseInfo(brain));
    
    // Create test instances
    BasicConnectivityTest connectivityTest;
    PlasticityExperimentTest plasticityTest;
    StdpVerificationTest stdpTest;
    
    std::vector<std::unique_ptr<TestBase>> tests;
    tests.push_back(std::make_unique<BasicConnectivityTest>(connectivityTest));
    tests.push_back(std::make_unique<PlasticityExperimentTest>(plasticityTest));
    tests.push_back(std::make_unique<StdpVerificationTest>(stdpTest));
    
    std::vector<TestResult> results;
    
    for (auto& test : tests) {
        if (verbose) {
            NLM_LOG_INFO("Running test: " + test->getName());
        }
        
        TestResult result = test->run(brain);
        results.push_back(result);
        
        if (verbose) {
            std::string status = result.passed ? "PASSED" : "FAILED";
            NLM_LOG_INFO("Test " + test->getName() + ": " + status);
            if (!result.message.empty()) {
                NLM_LOG_INFO("  " + result.message);
            }
            if (result.executionTime > 0) {
                NLM_LOG_INFO("  Execution time: " + std::to_string(result.executionTime) + "s");
            }
        }
    }
    
    // Summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test Suite Summary ===");
    
    int passed = 0, failed = 0;
    for (const auto& result : results) {
        if (result.passed) {
            passed++;
            NLM_LOG_INFO("✓ " + result.name + ": PASSED");
        } else {
            failed++;
            NLM_LOG_INFO("✗ " + result.name + ": FAILED");
            if (!result.message.empty()) {
                NLM_LOG_INFO("  Reason: " + result.message);
            }
        }
    }
    
    NLM_LOG_INFO("Total: " + std::to_string(results.size()) + 
                ", Passed: " + std::to_string(passed) + 
                ", Failed: " + std::to_string(failed));
    
    if (verbose) {
        NLM_LOG_INFO("");
        if (failed == 0) {
            NLM_LOG_INFO("All tests passed! ✓");
        } else {
            NLM_LOG_INFO("Some tests failed. Check output for details.");
        }
    }
}

}} // namespace nlm::test

// Utility functions implementation
namespace utils {

    bool validateBrainHealth(std::shared_ptr<Brain> brain) {
        if (!brain) {
            NLM_LOG_ERROR("Brain is null");
            return false;
        }
        
        if (!brain->getSpikeSystem()) {
            NLM_LOG_ERROR("Brain has no spike system");
            return false;
        }
        
        // Check if brain is properly initialized
        size_t neuronCount = brain->getTotalNeuronCount();
        if (neuronCount == 0) {
            NLM_LOG_WARNING("Brain has no neurons");
            return false;
        }
        
        return true;
    }
    
    std::string formatNeuronInfo(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Neurons: " << brain->getTotalNeuronCount()
           << ", Active: " << brain->getActiveNeuronCount()
           << ", Firing: " << brain->getFiringNeuronCount()
           << ", Avg Firing: " << brain->getAverageFiringRate();
        return ss.str();
    }
    
    std::string formatSynapseInfo(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Synapses: " << brain->getTotalSynapseCount()
           << ", E/I Ratio: " << brain->getExcitationInhibitionRatio();
        return ss.str();
    }
    
    BrainStats calculateBrainStats(std::shared_ptr<Brain> brain) {
        BrainStats stats;
        stats.totalNeurons = brain->getTotalNeuronCount();
        stats.totalSynapses = brain->getTotalSynapseCount();
        stats.activeNeurons = brain->getActiveNeuronCount();
        stats.firingNeurons = brain->getFiringNeuronCount();
        stats.averageFiringRate = brain->getAverageFiringRate();
        stats.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        return stats;
    }
    
}} // namespace nlm::test::utils

// Test runner implementation - simplified for single brain instance
void runAllTests(std::shared_ptr<Brain> brain, bool verbose = true) {
    if (!utils::validateBrainHealth(brain)) {
        NLM_LOG_ERROR("Brain health validation failed - cannot run tests");
        return;
    }
    
    NLM_LOG_INFO("=== Starting NLM Phase 2 Test Suite ===");
    NLM_LOG_INFO("Brain health: " + utils::formatNeuronInfo(brain));
    NLM_LOG_INFO("Synapse status: " + utils::formatSynapseInfo(brain));
    
    // Create test instances
    BasicConnectivityTest connectivityTest;
    PlasticityExperimentTest plasticityTest;
    StdpVerificationTest stdpTest;
    
    std::vector<std::unique_ptr<TestBase>> tests;
    tests.push_back(std::make_unique<BasicConnectivityTest>(connectivityTest));
    tests.push_back(std::make_unique<PlasticityExperimentTest>(plasticityTest));
    tests.push_back(std::make_unique<StdpVerificationTest>(stdpTest));
    
    std::vector<TestResult> results;
    
    for (auto& test : tests) {
        if (verbose) {
            NLM_LOG_INFO("Running test: " + test->getName());
        }
        
        TestResult result = test->run(brain);
        results.push_back(result);
        
        if (verbose) {
            std::string status = result.passed ? "PASSED" : "FAILED";
            NLM_LOG_INFO("Test " + test->getName() + ": " + status);
            if (!result.message.empty()) {
                NLM_LOG_INFO("  " + result.message);
            }
            if (result.executionTime > 0) {
                NLM_LOG_INFO("  Execution time: " + std::to_string(result.executionTime) + "s");
            }
        }
    }
    
    // Summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test Suite Summary ===");
    
    int passed = 0, failed = 0;
    for (const auto& result : results) {
        if (result.passed) {
            passed++;
            NLM_LOG_INFO("✓ " + result.name + ": PASSED");
        } else {
            failed++;
            NLM_LOG_INFO("✗ " + result.name + ": FAILED");
            if (!result.message.empty()) {
                NLM_LOG_INFO("  Reason: " + result.message);
            }
        }
    }
    
    NLM_LOG_INFO("Total: " + std::to_string(results.size()) + 
                ", Passed: " + std::to_string(passed) + 
                ", Failed: " + std::to_string(failed));
    
    if (verbose) {
        NLM_LOG_INFO("");
        if (failed == 0) {
            NLM_LOG_INFO("All tests passed! ✓");
        } else {
            NLM_LOG_INFO("Some tests failed. Check output for details.");
        }
    }
}

}} // namespace nlm::test