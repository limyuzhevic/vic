// NLM Phase 2 Demo Test Runner
// This module provides a clean separation of concerns for Phase 2 experiments.
// It separates test orchestration from experiment execution.

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Test metadata structure for better organization
struct TestInfo {
    std::string name;
    std::string description;
    std::function<void(std::shared_ptr<Brain>)> setup;
    std::function<void(std::shared_ptr<Brain>)> execute;
    std::function<void(std::shared_ptr<Brain>)> teardown;
    bool requiresBrainReset = true;
};

// Test runner class that manages brain lifecycle
class Phase2TestRunner {
public:
    Phase2TestRunner(std::shared_ptr<Config> config) : config_(config) {}
    
    // Initialize brain with clean setup
    std::shared_ptr<Brain> createBrain() {
        auto brain = std::make_shared<Brain>(config_);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain in test runner");
            return nullptr;
        }
        return brain;
    }
    
    // Clean brain reset and reinitialize
    bool resetBrain(std::shared_ptr<Brain>& brain) {
        if (!brain) {
            brain = createBrain();
            return brain != nullptr;
        }
        
        brain->reset();
        return brain->initialize();
    }
    
    // Run a single test with proper lifecycle management
    bool runTest(const TestInfo& test, std::shared_ptr<Brain>& brain) {
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
    
    // Run multiple tests sequentially
    bool runTests(const std::vector<TestInfo>& tests) {
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
    
private:
    std::shared_ptr<Config> config_;
};

// Test helper functions that can be reused across different tests
namespace TestHelpers {
    
    // Basic connectivity test setup
    void setupBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Setting up basic connectivity test...");
        // Any specific setup for basic connectivity test
    }
    
    void executeBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Executing basic connectivity test...");
        // This is the actual test logic
        runBasicConnectivityTest(brain);
    }
    
    void teardownBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Cleaning up basic connectivity test...");
        // Any cleanup logic
    }
    
    // Plasticity test setup
    void setupPlasticityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Setting up plasticity test...");
        // Enable plasticity systems
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
    }
    
    void executePlasticityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Executing plasticity test...");
        runPlasticityExperiment(brain);
    }
    
    void teardownPlasticityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Cleaning up plasticity test...");
        // Reset plasticity systems if needed
    }
    
    // STDP test setup
    void setupStdpTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Setting up STDP test...");
        // Configure STDP parameters for test
        auto* region = brain->getRegion(RegionId(1));
        if (region && !region->getSynapses().empty()) {
            auto syn = region->getSynapses()[0].get();
            syn->enablePlasticity(false, true, false);  // STDP only
        }
    }
    
    void executeStdpTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Executing STDP test...");
        runStdpVerification(brain);
    }
    
    void teardownStdpTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Cleaning up STDP test...");
        // Restore original plasticity settings
    }
}

} // namespace nlm