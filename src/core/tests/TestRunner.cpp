// Test Runner Implementation
// Provides the main entry point for running tests and demonstrates usage

#include "core/tests/TestFramework.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <chrono>
#include <thread>

// Performance timing utilities
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& name) : name(name) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    ~PerformanceTimer() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        Logger::getGlobal().debug(name + " took " + std::to_string(duration) + " microseconds");
    }
    
private:
    std::string name;
    std::chrono::high_resolution_clock::time_point startTime;
};

// Helper function to run performance test with timing
bool runPerformanceTest(const std::string& testName, std::function<bool()> testFunc) {
    PerformanceTimer timer(testName);
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool result = testFunc();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    Logger::getGlobal().info(testName + " completed in " + std::to_string(duration) + " ms - " + 
                           (result ? "PASSED" : "FAILED"));
    
    return result;
}

// Example test implementations demonstrating various testing scenarios
namespace example_tests {

// Basic neuron tests
bool testNeuronProperties() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test basic properties
    if (neuron.getId() != nlm::NeuronId(1)) return false;
    if (neuron.getType() != nlm::NeuronType::Internal) return false;
    if (neuron.getMembranePotential() != -70.0f) return false;
    
    // Test setting properties
    neuron.setType(nlm::NeuronType::Excitatory);
    neuron.setMembranePotential(-65.0f);
    neuron.injectCurrent(5.0f);
    
    if (neuron.getMembranePotential() != -65.0f) return false;  // Should not change instantly
    if (neuron.getTotalCurrent() != 5.0f) return false;
    
    return true;
}

bool testNeuronPlasticity() {
    nlm::Neuron neuron(nlm::NeuronId(2));
    neuron.initializeRandom(nlm::RandomGenerator(42));
    
    // Test plasticity flags
    if (neuron.getPlasticityFlags().hebbian) return false;  // Should be false initially
    
    neuron.enablePlasticity(true, true, false);
    if (!neuron.getPlasticityFlags().hebbian) return false;
    if (!neuron.getPlasticityFlags().stdp) return false;
    if (neuron.getPlasticityFlags().reward_modulated) return false;
    
    return true;
}

// Synapse tests
bool testSynapseOperations() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test initial state
    if (synapse.getWeight() != 0.0f) return false;
    if (synapse.getDelay() != 1) return false;  // Default delay
    if (!synapse.isExcitatory()) return false;  // Default type
    
    // Test weight operations
    synapse.setWeight(0.5f);
    if (synapse.getWeight() != 0.5f) return false;
    
    synapse.addToWeight(0.3f);
    if (synapse.getWeight() != 0.8f) return false;
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    if (!synapse.isInhibitory()) return false;
    if (synapse.isExcitatory()) return false;
    
    return true;
}

// STDP tests
bool testSTDPBasic() {
    nlm::STDP stdp;
    
    // Test default values
    if (stdp.getLTPWeight() != 0.01f) return false;
    if (stdp.getLTDWeight() != 0.012f) return false;
    if (stdp.getTimeConstant() != 20.0f) return false;
    
    // Test configuration
    stdp.configure(0.05f, 0.06f, 30.0f);
    if (stdp.getLTPWeight() != 0.05f) return false;
    if (stdp.getLTDWeight() != 0.06f) return false;
    if (stdp.getTimeConstant() != 30.0f) return false;
    
    return true;
}

bool testSTDPActivityDependence() {
    nlm::STDP stdp;
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Create correlated activity (pre before post)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // 5ms after pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Should potentiate (weight increase)
    if (finalWeight <= initialWeight) return false;
    
    return true;
}

// Brain integration tests
bool testBrainInitialization() {
    PerformanceTimer timer("Brain Initialization");
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    if (!success) return false;
    
    // Verify basic properties
    if (brain.getRegionCount() != 2) return false;
    if (brain.getTotalNeuronCount() != 100) return false;
    if (brain.getTotalSynapseCount() <= 0) return false;
    
    return true;
}

bool testBrainLearning() {
    PerformanceTimer timer("Brain Learning");
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    if (!initSuccess) return false;
    
    // Enable plasticity
    if (auto* region = brain.getRegion(nlm::RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Simulate learning over time
    float initialWeight = 0.0f;
    if (auto* region = brain.getRegion(nlm::RegionId(1))) {
        if (!region->getSynapses().empty()) {
            initialWeight = region->getSynapses()[0]->getWeight();
        }
    }
    
    // Run some steps
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
        
        // Inject some input to drive learning
        if (step % 10 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
        }
    }
    
    // Check if learning occurred (weight should have changed)
    float finalWeight = 0.0f;
    if (auto* region = brain.getRegion(nlm::RegionId(1))) {
        if (!region->getSynapses().empty()) {
            finalWeight = region->getSynapses()[0]->getWeight();
        }
    }
    
    return finalWeight != initialWeight;  // Learning should change weights
}

// Performance benchmarks
bool benchmarkNeuronStep() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.initializeRandom(nlm::RandomGenerator(42));
    
    const int iterations = 10000;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        neuron.step(0.001f);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    Logger::getGlobal().info(std::string("Neuron step benchmark: ") + 
                           std::to_string(iterations) + " iterations in " + 
                           std::to_string(duration) + " ms (" +
                           std::to_string(static_cast<double>(iterations) / (duration / 1000.0)) + 
                           " iterations/sec)");
    
    return duration < 1000;  // Should complete in less than 1 second
}

bool benchmarkSynapseUpdate() {
    nlm::STDP stdp;
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    const int iterations = 1000;
    
    // Prepare spike data
    std::vector<nlm::Timestamp> preSpikes(100, 0.0f);
    std::vector<nlm::Timestamp> postSpikes(100, 5.0f);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    Logger::getGlobal().info(std::string("Synapse update benchmark: ") + 
                           std::to_string(iterations) + " updates in " + 
                           std::to_string(duration) + " ms (" +
                           std::to_string(static_cast<double>(iterations) / (duration / 1000.0)) + 
                           " updates/sec)");
    
    return duration < 100;  // Should complete in less than 100 ms
}

// Stress test
bool stressTestLargeNetwork() {
    PerformanceTimer timer("Stress Test: Large Network");
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    if (!initSuccess) return false;
    
    // Run many steps
    const int steps = 1000;
    for (nlm::SimulationStep step = 0; step < steps; ++step) {
        brain.step(step, step * 0.001);
        
        // Inject input occasionally
        if (step % 50 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
        }
    }
    
    // Verify network is still in a valid state
    if (brain.getTotalNeuronCount() != 1000) return false;
    if (brain.getTotalSynapseCount() < 0) return false;  // Could be 0 if no connections
    
    return true;
}

// Documentation test (example usage)
bool testDocumentationExample() {
    // This test demonstrates how to use the NLM API
    try {
        auto config = std::make_shared<nlm::Config>();
        config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Step through the simulation
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step, step * 0.001);
        }
        
        // Verify the brain is still functional
        return brain.getTotalNeuronCount() > 0;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error(std::string("Documentation test failed: ") + e.what());
        return false;
    }
}

} // namespace example_tests

// Register example tests
namespace {
    // Unit tests
    UNIT_TEST(Neuron_Properties, "Test basic neuron properties and operations", example_tests::testNeuronProperties)
    UNIT_TEST(Neuron_Plasticity, "Test neuron plasticity configuration", example_tests::testNeuronPlasticity)
    
    // Synapse tests
    UNIT_TEST(Synapse_Operations, "Test synapse creation, weight, and type operations", example_tests::testSynapseOperations)
    
    // STDP tests
    UNIT_TEST(STDP_Basic, "Test STDP basic configuration and properties", example_tests::testSTDPBasic)
    UNIT_TEST(STDP_Activity_Dependence, "Test STDP activity-dependent learning", example_tests::testSTDPActivityDependence)
    
    // Integration tests
    INTEGRATION_TEST(Brain_Initialization, "Test brain initialization and basic properties", example_tests::testBrainInitialization)
    INTEGRATION_TEST(Brain_Learning, "Test brain learning capabilities with plasticity", example_tests::testBrainLearning)
    
    // Performance tests
    PERFORMANCE_TEST(Neuron_Step_Benchmark, "Benchmark neuron step operations", example_tests::benchmarkNeuronStep)
    PERFORMANCE_TEST(Synapse_Update_Benchmark, "Benchmark synapse update operations", example_tests::benchmarkSynapseUpdate)
    
    // Stress tests
    STRESS_TEST(Large_Network_Stress, "Stress test with large network", example_tests::stressTestLargeNetwork)
    
    // Regression tests
    REGRESSION_TEST(Documentation_Example, "Test documentation example usage", example_tests::testDocumentationExample)
    
    // Documentation tests
    DOCUMENTATION_TEST(Usage_Example, "Demonstrate basic NLM usage", example_tests::testDocumentationExample)
} // anonymous namespace

// Main function - entry point for test execution
int main(int argc, char** argv) {
    // Configure global logger
    nlm::Logger::configureGlobalLogger("configs/test_config.cfg");
    
    Logger::getGlobal().info("Starting NLM test suite");
    Logger::getGlobal().info(std::string("Test framework version: 2.0"));
    
    // Parse command line arguments
    std::string testMode = "all";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--unit" || arg == "-u") {
            testMode = "unit";
        } else if (arg == "--integration" || arg == "-i") {
            testMode = "integration";
        } else if (arg == "--performance" || arg == "-p") {
            testMode = "performance";
        } else if (arg == "--stress" || arg == "-s") {
            testMode = "stress";
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: test_runner [OPTIONS]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -u, --unit         Run unit tests only" << std::endl;
            std::cout << "  -i, --integration  Run integration tests only" << std::endl;
            std::cout << "  -p, --performance  Run performance tests only" << std::endl;
            std::cout << "  -s, --stress       Run stress tests only" << std::endl;
            std::cout << "  -h, --help         Show this help message" << std::endl;
            return 0;
        }
    }
    
    // Run tests based on mode
    nlm::TestCategory category;
    
    if (testMode == "unit") {
        category = nlm::TestCategory::Unit;
    } else if (testMode == "integration") {
        category = nlm::TestCategory::Integration;
    } else if (testMode == "performance") {
        category = nlm::TestCategory::Performance;
    } else if (testMode == "stress") {
        category = nlm::TestCategory::Stress;
    } else {
        category = nlm::TestCategory::Unit; // Default to unit tests
    }
    
    // Run selected category
    auto& framework = nlm::getTestFramework();
    auto results = framework.runCategory(category);
    
    // Generate and write report
    framework.writeReport("test_report.html");
    framework.writeReport("test_report.txt");
    
    // Print summary
    std::cout << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << "NLM TEST RUNNER SUMMARY" << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << "Test Mode: " << testMode << std::endl;
    std::cout << "Total Tests: " << framework.getResults().size() << std::endl;
    std::cout << "Passed: " << framework.getPassedCount() << std::endl;
    std::cout << "Failed: " << framework.getFailedCount() << std::endl;
    std::cout << "Success Rate: " << std::fixed << std::setprecision(2) 
              << (framework.getResults().empty() ? 0.0 : 
                  (static_cast<double>(framework.getPassedCount()) / framework.getResults().size() * 100.0)) << "%" << std::endl;
    std::cout << "Total Duration: " << std::fixed << std::setprecision(3) << framework.getTotalDuration() << " seconds" << std::endl;
    
    // Exit with appropriate code
    if (framework.getFailedCount() > 0) {
        std::cout << std::endl;
        std::cout << "TESTS FAILED - exiting with error code 1" << std::endl;
        return 1;
    } else {
        std::cout << std::endl;
        std::cout << "ALL TESTS PASSED - exiting with success code 0" << std::endl;
        return 0;
    }
}
