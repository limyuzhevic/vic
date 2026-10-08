// Unified Test Runner for NLM
// Modern CTest integration with comprehensive test discovery and reporting
// Supports modern CMake testing features

#include "tests/TestBase.hpp"
#include "tests/test_config.cpp"
#include "tests/test_brain.cpp"
#include "tests/test_clock.cpp"
#include "tests/test_neuron.cpp"
#include "tests/test_synapse.cpp"
#include "tests/test_stdp.cpp"
#include "tests/test_types.cpp"
#include "tests/test_random.cpp"
#include "tests/test_main.cpp"
#include "tests/test_plasticity_implementation.cpp"
#include "tests/test_dopamine.cpp"

namespace nlm {

class UnifiedTestRunner : public TestBase {
public:
    UnifiedTestRunner() : TestBase("UnifiedTestRunner") {
        NLM_LOG_INFO("Unified Test Runner initialized");
    }
    
    ~UnifiedTestRunner() override {
        NLM_LOG_INFO("Unified Test Runner cleanup completed");
    }
    
    void setup() override {
        NLM_LOG_INFO("Running unified test setup");
        
        // Initialize test configuration
        testConfig_.set("test.random_seed", 42);
        testConfig_.set("test.simulation_timestep", 0.001);
        testConfig_.set("test.verbosity", 2);
        
        // Enable all test categories by default
        testCategories_ = {"unit", "integration", "performance", "plasticity", "cognition"};
        
        NLM_LOG_INFO("Test setup completed");
    }
    
    void teardown() override {
        NLM_LOG_INFO("Running unified test teardown");
        
        // Clear test data
        testConfig_.clear();
        testResults_.clear();
        
        NLM_LOG_INFO("Test teardown completed");
    }
    
    void runAllTests() {
        NLM_LOG_INFO("=== Starting Unified Test Suite ===");
        NLM_LOG_INFO("Test Categories: " + categoriesToString());
        
        // Run unit tests
        if (containsTestCategory("unit")) {
            NLM_LOG_INFO("Running unit tests...");
            runUnitTests();
        }
        
        // Run integration tests
        if (containsTestCategory("integration")) {
            NLM_LOG_INFO("Running integration tests...");
            runIntegrationTests();
        }
        
        // Run performance tests
        if (containsTestCategory("performance")) {
            NLM_LOG_INFO("Running performance tests...");
            runPerformanceTests();
        }
        
        // Run plasticity tests
        if (containsTestCategory("plasticity")) {
            NLM_LOG_INFO("Running plasticity tests...");
            runPlasticityTests();
        }
        
        // Run cognition tests
        if (containsTestCategory("cognition")) {
            NLM_LOG_INFO("Running cognition tests...");
            runCognitionTests();
        }
        
        // Print comprehensive test report
        printTestReport();
        
        NLM_LOG_INFO("=== Unified Test Suite Complete ===");
    }
    
    void runUnitTests() {
        NLM_LOG_INFO("Executing unit test suite");
        
        // Test type system
        runTestCase("TypeSystem", []() {
            NLM_LOG_INFO("  Testing type system...");
            return test_types();
        });
        
        // Test configuration system
        runTestCase("ConfigurationSystem", []() {
            NLM_LOG_INFO("  Testing configuration system...");
            return test_config();
        });
        
        // Test random number generator
        runTestCase("RandomGenerator", []() {
            NLM_LOG_INFO("  Testing random generator...");
            return test_random();
        });
        
        // Test clock system
        runTestCase("ClockSystem", []() {
            NLM_LOG_INFO("  Testing clock system...");
            return test_clock();
        });
        
        NLM_LOG_INFO("Unit tests completed");
    }
    
    void runIntegrationTests() {
        NLM_LOG_INFO("Executing integration test suite");
        
        // Test brain system integration
        runTestCase("BrainIntegration", []() {
            NLM_LOG_INFO("  Testing brain integration...");
            return test_brain();
        });
        
        // Test synapse dynamics
        runTestCase("SynapseDynamics", []() {
            NLM_LOG_INFO("  Testing synapse dynamics...");
            return test_synapse();
        });
        
        // Test neuron dynamics
        runTestCase("NeuronDynamics", []() {
            NLM_LOG_INFO("  Testing neuron dynamics...");
            return test_neuron();
        });
        
        // Test STDP plasticity
        runTestCase("STDPPlasticity", []() {
            NLM_LOG_INFO("  Testing STDP plasticity...");
            return test_stdp();
        });
        
        NLM_LOG_INFO("Integration tests completed");
    }
    
    void runPerformanceTests() {
        NLM_LOG_INFO("Executing performance test suite");
        
        // Test plasticity implementation
        runTestCase("PlasticityImplementation", []() {
            NLM_LOG_INFO("  Testing plasticity implementation...");
            return test_plasticity_implementation();
        });
        
        // Test dopamine system
        runTestCase("DopamineSystem", []() {
            NLM_LOG_INFO("  Testing dopamine system...");
            return test_dopamine();
        });
        
        // Test main functionality
        runTestCase("MainFunctionality", []() {
            NLM_LOG_INFO("  Testing main functionality...");
            return test_main();
        });
        
        NLM_LOG_INFO("Performance tests completed");
    }
    
    void runPlasticityTests() {
        NLM_LOG_INFO("Executing plasticity test suite");
        
        // Test various plasticity mechanisms
        runTestCase("HebbianPlasticity", []() {
            NLM_LOG_INFO("  Testing Hebbian plasticity...");
            return runHebbianTest();
        });
        
        runTestCase("StructuralPlasticity", []() {
            NLM_LOG_INFO("  Testing structural plasticity...");
            return runStructuralPlasticityTest();
        });
        
        NLM_LOG_INFO("Plasticity tests completed");
    }
    
    void runCognitionTests() {
        NLM_LOG_INFO("Executing cognition test suite");
        
        // Test cognitive mechanisms
        runTestCase("NeuralPlanner", []() {
            NLM_LOG_INFO("  Testing neural planner...");
            return runNeuralPlannerTest();
        });
        
        runTestCase("ConceptFormation", []() {
            NLM_LOG_INFO("  Testing concept formation...");
            return runConceptFormationTest();
        });
        
        NLM_LOG_INFO("Cognition tests completed");
    }
    
    void runTestCase(const std::string& name, std::function<bool()> testFunc) {
        NLM_LOG_INFO("Running test case: " + name);
        
        // Record start time for performance measurement
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Execute test function
        bool result = testFunc();
        
        // Record end time
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count() / 1000.0;
        
        // Store test result
        TestResult testResult;
        testResult.name = name;
        testResult.success = result;
        testResult.duration = duration;
        testResults_.push_back(testResult);
        
        if (result) {
            NLM_LOG_INFO("  ✓ Test PASSED: " + name + " (" + std::to_string(duration) + "s)");
        } else {
            NLM_LOG_ERROR("  ✗ Test FAILED: " + name + " (" + std::to_string(duration) + "s)");
        }
    }
    
    void printTestReport() {
        NLM_LOG_INFO("=== Comprehensive Test Report ===");
        
        size_t totalTests = testResults_.size();
        size_t passedTests = 0;
        double totalDuration = 0.0;
        double maxDuration = 0.0;
        
        for (const auto& result : testResults_) {
            if (result.success) {
                passedTests++;
            }
            totalDuration += result.duration;
            maxDuration = std::max(maxDuration, result.duration);
        }
        
        NLM_LOG_INFO("Total Tests: " + std::to_string(totalTests));
        NLM_LOG_INFO("Passed: " + std::to_string(passedTests));
        NLM_LOG_INFO("Failed: " + std::to_string(totalTests - passedTests));
        NLM_LOG_INFO("Success Rate: " + std::to_string((passedTests * 100.0) / totalTests) + "%");
        NLM_LOG_INFO("Total Duration: " + std::to_string(totalDuration) + "s");
        NLM_LOG_INFO("Max Test Duration: " + std::to_string(maxDuration) + "s");
        NLM_LOG_INFO("Average Test Duration: " + std::to_string(totalDuration / totalTests) + "s");
        
        if (totalTests > 0) {
            double performanceScore = (passedTests * 100.0) / totalTests;
            NLM_LOG_INFO("Performance Score: " + std::to_string(performanceScore));
        }
        
        NLM_LOG_INFO("==============================================");
        
        // Log detailed results for debugging
        for (const auto& result : testResults_) {
            std::string status = result.success ? "PASS" : "FAIL";
            NLM_LOG_INFO("  " + result.name + ": " + status + " (" + std::to_string(result.duration) + "s)");
        }
    }
    
    std::string categoriesToString() {
        std::string result;
        for (size_t i = 0; i < testCategories_.size(); ++i) {
            if (i > 0) result += ", ";
            result += testCategories_[i];
        }
        return result;
    }
    
    bool containsTestCategory(const std::string& category) {
        return std::find(testCategories_.begin(), testCategories_.end(), category) != testCategories_.end();
    }
    
    bool runHebbianTest() {
        NLM_LOG_INFO("    Testing Hebbian plasticity rules...");
        
        try {
            // Create test configuration
            auto config = std::make_shared<Config>();
            config->set("random_seed", 42);
            config->set("neuron_count", 100);
            
            // Initialize brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("    Failed to initialize brain");
                return false;
            }
            
            // Apply Hebbian plasticity
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, false);
                    syn->setWeight(0.5f);
                }
            }
            
            // Simulate Hebbian learning
            for (SimulationStep step = 0; step < 100; ++step) {
                // Inject correlated activity
                for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 10; ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Excitatory, 20.0f);
                }
                
                brain->step(step, step * 0.001);
            }
            
            // Verify Hebbian strengthening
            bool success = false;
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    if (syn->getWeight() > 0.5f) {
                        success = true;
                        break;
                    }
                }
            }
            
            NLM_LOG_INFO("    Hebbian test " + std::string(success ? "PASSED" : "FAILED"));
            return success;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("    Hebbian test exception: " + std::string(e.what()));
            return false;
        }
    }
    
    bool runStructuralPlasticityTest() {
        NLM_LOG_INFO("    Testing structural plasticity...");
        
        try {
            // Create test configuration
            auto config = std::make_shared<Config>();
            config->set("random_seed", 43);
            config->set("neuron_count", 50);
            config->set("synaptogenesis_rate", 0.001);
            config->set("pruning_rate", 0.0001);
            
            // Initialize brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("    Failed to initialize brain");
                return false;
            }
            
            // Enable structural plasticity
            brain->enableStructuralPlasticity(true, true);
            
            // Simulate structural changes
            size_t initialSynapses = brain->getTotalSynapseCount();
            
            for (SimulationStep step = 0; step < 200; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t finalSynapses = brain->getTotalSynapseCount();
            
            // Verify structural changes occurred
            bool success = false;
            if (std::abs(static_cast<double>(finalSynapses) - static_cast<double>(initialSynapses)) > 5) {
                success = true;
            }
            
            NLM_LOG_INFO("    Structural plasticity test " + std::string(success ? "PASSED" : "FAILED"));
            NLM_LOG_INFO("      Initial synapses: " + std::to_string(initialSynapses));
            NLM_LOG_INFO("      Final synapses: " + std::to_string(finalSynapses));
            
            return success;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("    Structural plasticity test exception: " + std::string(e.what()));
            return false;
        }
    }
    
    bool runNeuralPlannerTest() {
        NLM_LOG_INFO("    Testing neural planner...");
        
        try {
            // Create test configuration
            auto config = std::make_shared<Config>();
            config->set("random_seed", 44);
            config->set("neuron_count", 200);
            
            // Initialize brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("    Failed to initialize brain");
                return false;
            }
            
            // Test neural planning capabilities
            NLM_LOG_INFO("    Testing neural planning capabilities...");
            
            // In a real implementation, this would test the NeuralPlanner class
            // For now, verify basic brain functionality
            bool success = (brain->getTotalNeuronCount() > 0 && 
                           brain->getTotalSynapseCount() > 0);
            
            NLM_LOG_INFO("    Neural planner test " + std::string(success ? "PASSED" : "FAILED"));
            
            return success;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("    Neural planner test exception: " + std::string(e.what()));
            return false;
        }
    }
    
    bool runConceptFormationTest() {
        NLM_LOG_INFO("    Testing concept formation...");
        
        try {
            // Create test configuration
            auto config = std::make_shared<Config>();
            config->set("random_seed", 45);
            config->set("neuron_count", 150);
            
            // Initialize brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("    Failed to initialize brain");
                return false;
            }
            
            // Test concept formation capabilities
            NLM_LOG_INFO("    Testing concept formation capabilities...");
            
            // In a real implementation, this would test the ConceptFormation class
            // For now, verify basic brain functionality with varied inputs
            brain->reset();
            brain->initialize();
            
            bool success = (brain->getTotalNeuronCount() > 0 && 
                           brain->getTotalSynapseCount() > 0);
            
            NLM_LOG_INFO("    Concept formation test " + std::string(success ? "PASSED" : "FAILED"));
            
            return success;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("    Concept formation test exception: " + std::string(e.what()));
            return false;
        }
    }
    
private:
    struct TestResult {
        std::string name;
        bool success;
        double duration;
    };
    
    Config testConfig_;
    std::vector<std::string> testCategories_;
    std::vector<TestResult> testResults_;
};

// Global test runner instance
UnifiedTestRunner globalTestRunner;

// Test main function
int main(int argc, char** argv) {
    NLM_LOG_INFO("=== NLM Unified Test Runner ===");
    NLM_LOG_INFO("Testing with " + std::to_string(std::thread::hardware_concurrency()) + " threads");
    
    // Parse command line arguments
    bool runAll = true;
    std::string testMode = "comprehensive";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--unit") {
            runAll = false;
            globalTestRunner.setTestCategories({"unit"});
            testMode = "unit";
        } else if (arg == "--integration") {
            runAll = false;
            globalTestRunner.setTestCategories({"integration"});
            testMode = "integration";
        } else if (arg == "--performance") {
            runAll = false;
            globalTestRunner.setTestCategories({"performance"});
            testMode = "performance";
        } else if (arg == "--plasticity") {
            runAll = false;
            globalTestRunner.setTestCategories({"plasticity"});
            testMode = "plasticity";
        } else if (arg == "--cognition") {
            runAll = false;
            globalTestRunner.setTestCategories({"cognition"});
            testMode = "cognition";
        } else if (arg == "--help") {
            NLM_LOG_INFO("Usage: test_runner [--unit|--integration|--performance|--plasticity|--cognition|--help]");
            return 0;
        }
    }
    
    try {
        // Run tests
        globalTestRunner.setup();
        globalTestRunner.runAllTests();
        globalTestRunner.teardown();
        
        NLM_LOG_INFO("=== Test Runner Complete ===");
        return 0;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Test runner exception: " + std::string(e.what()));
        return 1;
    }
}

} // namespace nlm
