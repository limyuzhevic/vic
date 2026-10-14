#include "TestFramework.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <cmath>

// Unit Tests for Core Components
TestSuite createUnitTests() {
    TestSuite suite("Unit Tests");
    
    // Test 1: Configuration loading
    suite.addTest(TestCase(
        "Config_LoadFromFile",
        "Test loading configuration from file",
        TestCategory::Unit,
        []() {
            auto config = std::make_shared<nlm::Config>();
            return config->loadFromFile("tests/config_test.cfg");
        }
    ));
    
    // Test 2: Configuration validation
    suite.addTest(TestCase(
        "Config_Validation",
        "Test configuration validation",
        TestCategory::Unit,
        []() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(1000));
            config->set("region_count", static_cast<int64_t>(1));
            return config->isValidAll();
        }
    ));
    
    // Test 3: Brain initialization
    suite.addTest(TestCase(
        "Brain_Initialize",
        "Test brain initialization",
        TestCategory::Unit,
        []() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100));
            config->set("region_count", static_cast<int64_t>(1));
            
            auto brain = std::make_shared<nlm::Brain>(config);
            return brain->initialize();
        }
    ));
    
    // Test 4: Configuration constraints
    suite.addTest(TestCase(
        "Config_Constraints",
        "Test configuration constraint validation",
        TestCategory::Unit,
        []() {
            auto config = std::make_shared<nlm::Config>();
            config->addConstraint(nlm::ConfigConstraint(
                "test_value", "Test value for constraint testing", "int64", false, "10"
            ));
            
            config->set("test_value", static_cast<int64_t>(5));
            return config->isValid("test_value");
        }
    ));
    
    // Test 5: Command-line argument parsing
    suite.addTest(TestCase(
        "Config_LoadFromArgs",
        "Test loading configuration from command line arguments",
        TestCategory::Unit,
        []() {
            const char* argv[] = {"test", "--config=neuron_count=500", "--config=region_count=2"};
            int argc = 3;
            auto config = std::make_shared<nlm::Config>();
            return config->loadFromArgs(argc, argv);
        }
    ));
    
    return suite;
}

// Integration Tests for Brain Systems
TestSuite createIntegrationTests() {
    TestSuite suite("Integration Tests");
    
    // Test 1: Complete brain integration
    suite.addTest(TestCase(
        "Brain_Integration",
        "Test complete brain integration with all systems",
        TestCategory::Integration,
        []() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100));
            config->set("region_count", static_cast<int64_t>(1));
            
            auto brain = std::make_shared<nlm::Brain>(config);
            if (!brain->initialize()) {
                return false;
            }
            
            // Run a few steps
            for (int i = 0; i < 10; ++i) {
                brain->step(i, i * 0.001);
            }
            
            // Check that all systems are integrated
            return (brain->getWorkingMemory() != nullptr) &&
                   (brain->getEpisodicMemory() != nullptr) &&
                   (brain->getDopamine() != nullptr) &&
                   (brain->getCuriosity() != nullptr);
        }
    ));
    
    // Test 2: Memory system integration
    suite.addTest(TestCase(
        "Memory_Integration",
        "Test memory system integration",
        TestCategory::Integration,
        []() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100));
            
            auto brain = std::make_shared<nlm::Brain>(config);
            if (!brain->initialize()) {
                return false;
            }
            
            // Store some data in working memory
            if (auto* wm = brain->getWorkingMemory()) {
                wm->storeToNeuron(1, 0.5f);
                wm->update(0.001);
                return wm->getActiveTraces() > 0;
            }
            
            return false;
        }
    ));
    
    // Test 3: Neuromodulation integration
    suite.addTest(TestCase(
        "Neuromodulation_Integration",
        "Test neuromodulation system integration",
        TestCategory::Integration,
        []() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100));
            
            auto brain = std::make_shared<nlm::Brain>(config);
            if (!brain->initialize()) {
                return false;
            }
            
            // Inject current to stimulate neuromodulation
            for (int i = 0; i < 5; ++i) {
                brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
                brain->step(i, i * 0.001);
            }
            
            // Check that neuromodulation systems are responding
            auto* dopamine = brain->getDopamine();
            if (dopamine) {
                // Dopamine level should be changing based on stimulation
                return true;
            }
            
            return false;
        }
    ));
    
    return suite;
}

// Performance Tests
TestSuite createPerformanceTests() {
    TestSuite suite("Performance Tests");
    
    // Test 1: Configuration loading performance
    suite.addTest(TestCase(
        "Config_Loading_Performance",
        "Test configuration loading performance",
        TestCategory::Performance,
        []() {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            auto config = std::make_shared<nlm::Config>();
            config->loadFromArgs(3, const_cast<char**>(
                new const char*[3]{ "test", "--config=neuron_count=1000", "--config=region_count=4" }
            ));
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return duration < 0.1; // Should complete within 100ms
        },
        1.0
    ));
    
    // Test 2: Brain initialization performance
    suite.addTest(TestCase(
        "Brain_Initialization_Performance",
        "Test brain initialization performance",
        TestCategory::Performance,
        []() {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(1000));
            config->set("region_count", static_cast<int64_t>(4));
            
            auto brain = std::make_shared<nlm::Brain>(config);
            bool initialized = brain->initialize();
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            return initialized && duration < 1.0; // Should initialize within 1 second
        },
        2.0
    ));
    
    return suite;
}

int main() {
    std::cout << "NLM Test Framework" << std::endl;
    std::cout << "==================" << std::endl;
    
    // Create test suites
    TestSuite unitTests = createUnitTests();
    TestSuite integrationTests = createIntegrationTests();
    TestSuite performanceTests = createPerformanceTests();
    
    // Add suites to framework
    TestFramework::getInstance().addSuite(unitTests);
    TestFramework::getInstance().addSuite(integrationTests);
    TestFramework::getInstance().addSuite(performanceTests);
    
    // Run all tests
    TestFramework::getInstance().runAllSuites();
    
    // Save results
    TestFramework::getInstance().saveResults("tests/test_results.txt");
    
    return 0;
}
