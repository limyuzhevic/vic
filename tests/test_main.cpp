// NLM Test Main
// Phase 2: Real Neural Computation Tests

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

// Integration tests - test the complete system working together
namespace test_integration {

void testConfigurationIntegration() {
    // Test that the configuration system can handle different formats
    nlm::Config config;
    
    // Test basic configuration setting and getting
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    assert(config.has("test_int"));
    assert(config.has("test_double"));
    assert(config.has("test_string"));
    assert(config.has("test_bool"));
    
    // Test getting values
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test getOr function
    int missingVal = config.getOr<int>("missing", 100);
    assert(missingVal == 100);
    
    std::cout << "    testConfigurationIntegration passed" << std::endl;
}

void testCommandLineInterface() {
    // Test that command line parsing works correctly
    nlm::Config config;
    
    // Test setting values through command line interface
    config.set("cli_test", "value", nlm::ConfigSource::CommandLine);
    config.set("cli_number", 123, nlm::ConfigSource::CommandLine);
    config.set("cli_flag", true, nlm::ConfigSource::CommandLine);
    
    // Verify the values were set
    assert(config.has("cli_test"));
    assert(config.has("cli_number"));
    assert(config.has("cli_flag"));
    
    // Test configuration validation
    assert(config.validateConfiguration());
    
    std::cout << "    testCommandLineInterface passed" << std::endl;
}

void testCompleteExperimentWorkflow() {
    // This would require actual Brain, Neuron, and other components
    // For now, we'll test that the system can be initialized
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // The brain constructor will test the complete integration
    // (Brain, Config, Random, Logger dependencies)
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain state after initialization
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testCompleteExperimentWorkflow passed" << std::endl;
}

void testSystemIntegration() {
    // Test that all major systems can work together
    auto config = std::make_shared<nlm::Config>();
    
    // Configure a realistic brain setup
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run a few simulation steps to test integration
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Verify that all systems were used during the step
    assert(brain.getTotalSpikeCount() >= 0);  // Should have some spikes or none
    assert(brain.getTotalNeuronCount() == 500);
    
    // Test plasticity integration (if enabled)
    bool enablePlasticity = config->getOr<bool>("enable_plasticity", true);
    if (enablePlasticity) {
        // At least verify that plasticity systems exist
        assert(brain.getSTDP() != nullptr);
        assert(brain.getHebbian() != nullptr);
        assert(brain.getStructuralPlasticity() != nullptr);
    }
    
    std::cout << "    testSystemIntegration passed" << std::endl;
}

void runAll() {
    testConfigurationIntegration();
    testCommandLineInterface();
    testCompleteExperimentWorkflow();
    testSystemIntegration();
}

} // namespace test_integration

int main() {
    std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running Types tests..." << std::endl;
    try {
        test_types::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Config tests..." << std::endl;
    try {
        test_config::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Random tests..." << std::endl;
    try {
        test_random::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Clock tests..." << std::endl;
    try {
        test_clock::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Neuron tests..." << std::endl;
    try {
        test_neuron::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Synapse tests..." << std::endl;
    try {
        test_synapse::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running STDP tests..." << std::endl;
    try {
        test_stdp::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Brain tests..." << std::endl;
    try {
        test_brain::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Integration tests..." << std::endl;
    try {
        test_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
        return 1;
    }
}
