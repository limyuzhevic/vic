# NLM Phase 2 Demo Test Utilities
// This module provides improved separation of concerns for Phase 2 experiments.
// It separates test orchestration from experiment execution and reduces code duplication.

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Type aliases for better readability
using BrainPtr = std::shared_ptr<Brain>;
using ConfigPtr = std::shared_ptr<Config>;

// Brain management helper functions
namespace BrainManager {
    
    // Initialize brain with proper error handling
    BrainPtr createBrain(ConfigPtr config) {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return nullptr;
        }
        NLM_LOG_INFO("Brain initialized successfully");
        return brain;
    }
    
    // Clean brain reset and reinitialize
    bool resetBrain(BrainPtr& brain) {
        if (!brain) {
            return false;
        }
        
        NLM_LOG_INFO("Resetting brain for new experiment...");
        brain->reset();
        bool initialized = brain->initialize();
        if (initialized) {
            NLM_LOG_INFO("Brain reset and reinitialized successfully");
        } else {
            NLM_LOG_ERROR("Failed to reinitialize brain after reset");
        }
        return initialized;
    }
    
    // Configure test-specific brain settings
    void configureBrainForTest(BrainPtr brain, const std::string& testType) {
        NLM_LOG_INFO("Configuring brain for " + testType + "...");
        
        if (testType == "basic_connectivity") {
            // Configure for basic connectivity test
            // No special configuration needed for basic connectivity
        } else if (testType == "plasticity") {
            // Enable plasticity systems
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
                }
            }
            NLM_LOG_INFO("Plasticity systems enabled for test");
        } else if (testType == "stdp") {
            // Configure for STDP verification
            auto* region = brain->getRegion(RegionId(1));
            if (region && !region->getSynapses().empty()) {
                auto syn = region->getSynapses()[0].get();
                syn->enablePlasticity(false, true, false);  // STDP only
                NLM_LOG_INFO("STDP configured for verification");
            }
        }
    }
    
    // Cleanup brain after test
    void cleanupBrainAfterTest(BrainPtr brain, const std::string& testType) {
        NLM_LOG_INFO("Cleaning up brain after " + testType + "...");
        // Reset any test-specific configuration
        if (testType == "plasticity") {
            // Reset plasticity settings if needed
        } else if (testType == "stdp") {
            // Restore original plasticity settings
        }
    }
}

// Test orchestration helper functions
namespace TestOrchestrator {
    
    // Run a test with proper lifecycle management
    bool runTestWithLifecycle(
        const std::string& testName,
        const std::string& testDescription,
        BrainPtr brain,
        std::function<void(BrainPtr)> setupFunc,
        std::function<void(BrainPtr)> executeFunc,
        std::function<void(BrainPtr)> cleanupFunc,
        bool requiresReset = true
    ) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== " + testName + " ===");
        NLM_LOG_INFO(testDescription);
        
        // Setup phase
        if (setupFunc) {
            NLM_LOG_INFO("Setting up test...");
            setupFunc(brain);
        }
        
        // Execute phase
        if (executeFunc) {
            NLM_LOG_INFO("Executing test...");
            executeFunc(brain);
        }
        
        // Cleanup phase
        if (cleanupFunc) {
            NLM_LOG_INFO("Cleaning up test...");
            cleanupFunc(brain);
        }
        
        // Reset brain if required
        if (requiresReset) {
            if (BrainManager::resetBrain(brain)) {
                NLM_LOG_INFO("Test completed, brain ready for next test");
            } else {
                NLM_LOG_ERROR("Failed to reset brain for next test");
                return false;
            }
        }
        
        return true;
    }
    
    // Run multiple tests sequentially
    bool runMultipleTests(
        BrainPtr brain,
        std::vector<std::tuple<std::string, std::string, 
                              std::function<void(BrainPtr)>, 
                              std::function<void(BrainPtr)>, 
                              std::function<void(BrainPtr)>, 
                              bool>> tests
    ) {
        NLM_LOG_INFO("Starting test suite with " + std::to_string(tests.size()) + " tests");
        
        for (size_t i = 0; i < tests.size(); ++i) {
            const auto& test = tests[i];
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Running test " + std::to_string(i + 1) + "/" + std::to_string(tests.size()) + ": " + 
                        std::get<0>(test));
            
            bool testSuccess = runTestWithLifecycle(
                std::get<0>(test),
                std::get<1>(test),
                brain,
                std::get<3>(test),
                std::get<4>(test),
                std::get<5>(test),
                std::get<6>(test)
            );
            
            if (!testSuccess) {
                NLM_LOG_ERROR("Test " + std::get<0>(test) + " failed");
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
}

} // namespace nlm