// Test function forward declarations
namespace nlm {
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    void runPlasticityExperiment(std::shared_ptr<Brain> brain);
    void runStdpVerification(std::shared_ptr<Brain> brain);
}

// Test runner implementation that doesn't create circular dependencies
namespace nlm {

class Phase2TestRunner {
public:
    Phase2TestRunner(std::shared_ptr<Config> config) : config_(config) {}
    
    // Initialize brain with proper error handling
    std::shared_ptr<Brain> createBrain() {
        auto brain = std::make_shared<Brain>(config_);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return nullptr;
        }
        NLM_LOG_INFO("Brain initialized successfully");
        return brain;
    }
    
    // Clean brain reset and reinitialize
    bool resetBrain(std::shared_ptr<Brain>& brain) {
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
    
    // Run a test with proper lifecycle management
    bool runTest(const std::string& testName, const std::string& testDescription,
                 std::shared_ptr<Brain>& brain,
                 std::function<void(std::shared_ptr<Brain>)> setupFunc,
                 std::function<void(std::shared_ptr<Brain>)> executeFunc,
                 std::function<void(std::shared_ptr<Brain>)> cleanupFunc,
                 bool requiresReset = true) {
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
            if (resetBrain(brain)) {
                NLM_LOG_INFO("Test completed, brain ready for next test");
            } else {
                NLM_LOG_ERROR("Failed to reset brain for next test");
                return false;
            }
        }
        
        return true;
    }
    
    // Run multiple tests sequentially
    bool runTests(const std::vector<std::tuple<std::string, std::string,
                                              std::function<void(std::shared_ptr<Brain>)>,
                                              std::function<void(std::shared_ptr<Brain>)>,
                                              std::function<void(std::shared_ptr<Brain>)>,
                                              bool>>& tests) {
        auto brain = createBrain();
        if (!brain) {
            NLM_LOG_ERROR("Failed to create initial brain");
            return false;
        }
        
        NLM_LOG_INFO("Starting Phase 2 test suite with " + std::to_string(tests.size()) + " tests");
        
        for (size_t i = 0; i < tests.size(); ++i) {
            const auto& test = tests[i];
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Running test " + std::to_string(i + 1) + "/" + std::to_string(tests.size()) + ": " + 
                        std::get<0>(test));
            
            bool testSuccess = runTest(
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
    
private:
    std::shared_ptr<Config> config_;
};

} // namespace nlm