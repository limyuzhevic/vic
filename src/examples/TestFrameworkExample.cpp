#pragma once

#include "tests/TestFramework.hpp"
#include "core/Config/Config.hpp"
#include "core/Constants.hpp"
#include <iostream>

namespace nlm {

namespace examples {

// Example test demonstrating the new test framework
class ExampleConnectivityTest : public tests::TestBase {
public:
    ExampleConnectivityTest(const tests::TestConfig& config) : TestBase(config) {}
    
    tests::TestResult run() override {
        std::cout << "ExampleConnectivityTest: Running basic connectivity test..." << std::endl;
        
        // Create brain configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        
        // Setup brain
        auto setupResult = setupBrain(config);
        if (!setupResult.isPassed()) {
            return setupResult;
        }
        
        // Test basic connectivity
        auto* region = brain_->getRegion(RegionId(1));
        auto neurons = region->getAllNeurons();
        
        if (neurons.empty()) {
            return TestResult("ExampleConnectivityTest", tests::TestResult::Status::FAILED,
                             "No neurons found");
        }
        
        // Inject current
        for (size_t i = 0; i < neurons.size(); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run simulation steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain_->step(step, step * 0.001);
        }
        
        // Check results
        size_t spikes = brain_->getTotalSpikeCount();
        if (spikes > 0) {
            return TestResult("ExampleConnectivityTest", tests::TestResult::Status::PASSED,
                             "Basic connectivity test passed. Spikes: " + std::to_string(spikes));
        } else {
            return TestResult("ExampleConnectivityTest", tests::TestResult::Status::FAILED,
                             "Basic connectivity test failed. No spikes generated.");
        }
    }
};

class ExamplePlasticityTest : public tests::TestBase {
public:
    ExamplePlasticityTest(const tests::TestConfig& config) : TestBase(config) {}
    
    tests::TestResult run() override {
        std::cout << "ExamplePlasticityTest: Running plasticity experiment..." << std::endl;
        
        // Create brain configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        
        // Setup brain
        auto setupResult = setupBrain(config);
        if (!setupResult.isPassed()) {
            return setupResult;
        }
        
        // Reset for fresh state
        brain_->reset();
        brain_->initialize();
        
        // Enable plasticity
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Record initial weights
        std::vector<float> initialWeights;
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        // Run plasticity experiment
        for (SimulationStep step = 0; step < 100; ++step) {
            // Inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain_->getTotalNeuronCount() / 4; ++i) {
                brain_->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain_->step(step, step * 0.001);
        }
        
        // Record final weights
        std::vector<float> finalWeights;
        float totalChange = 0.0f;
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
                if (finalWeights.size() - 1 < initialWeights.size()) {
                    totalChange += syn->getWeight() - initialWeights[finalWeights.size() - 1];
                }
            }
        }
        
        // Check if plasticity occurred
        if (std::abs(totalChange) > 0.001f) {
            return TestResult("ExamplePlasticityTest", tests::TestResult::Status::PASSED,
                             "Plasticity experiment passed. Total weight change: " + 
                             std::to_string(totalChange));
        } else {
            return TestResult("ExamplePlasticityTest", tests::TestResult::Status::FAILED,
                             "Plasticity experiment failed. No significant weight change.");
        }
    }
};

class ExampleSTDPTest : public tests::TestBase {
public:
    ExampleSTDPTest(const tests::TestConfig& config) : TestBase(config) {}
    
    tests::TestResult run() override {
        std::cout << "ExampleSTDPTest: Running STDP verification..." << std::endl;
        
        // Create brain configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        
        // Setup brain
        auto setupResult = setupBrain(config);
        if (!setupResult.isPassed()) {
            return setupResult;
        }
        
        // Get synapses
        auto* region = brain_->getRegion(RegionId(1));
        if (!region) {
            return TestResult("ExampleSTDPTest", tests::TestResult::Status::FAILED,
                             "No brain region found");
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            return TestResult("ExampleSTDPTest", tests::TestResult::Status::FAILED,
                             "Not enough synapses for STDP test");
        }
        
        // Record initial weights
        std::vector<float> initialWeights;
        for (size_t i = 0; i < 5; ++i) {
            initialWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        for (int trial = 0; trial < 50; ++trial) {
            // Fire pre-synaptic neuron
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                // Pre fires first
                preNeuron->injectCurrent(60.0f);
                brain_->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain_->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Record final weights and calculate change
        float totalChange = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - initialWeights[i];
            totalChange += delta;
        }
        
        // Check STDP results
        if (totalChange > 0.001f) {
            return TestResult("ExampleSTDPTest", tests::TestResult::Status::PASSED,
                             "STDP verification passed. LTP detected (Δ=" + std::to_string(totalChange) + ")");
        } else if (totalChange < -0.001f) {
            return TestResult("ExampleSTDPTest", tests::TestResult::Status::PASSED,
                             "STDP verification passed. LTD detected (Δ=" + std::to_string(totalChange) + ")");
        } else {
            return TestResult("ExampleSTDPTest", tests::TestResult::Status::FAILED,
                             "STDP verification failed. No significant weight change (Δ=" + std::to_string(totalChange) + ")");
        }
    }
};

// Example: Simple test runner demonstrating the new test framework
void demonstrateTestFramework() {
    std::cout << "=== Demonstrating New Test Framework ===" << std::endl;
    
    // Create test framework
    tests::TestFramework framework;
    
    // Register example tests
    tests::TestConfig basicConfig("Basic Connectivity Test");
    basicConfig.parameters["current_strength"] = "50.0";
    basicConfig.parameters["test_steps"] = "50";
    
    framework.registerTest<ExampleConnectivityTest>("BasicConnectivity", basicConfig);
    
    tests::TestConfig plasticityConfig("Plasticity Experiment");
    plasticityConfig.parameters["num_sensory_neurons"] = "20";
    plasticityConfig.parameters["sensory_current"] = "30.0";
    plasticityConfig.parameters["experiment_steps"] = "100";
    
    framework.registerTest<ExamplePlasticityTest>("PlasticityExperiment", plasticityConfig);
    
    tests::TestConfig stdpConfig("STDP Verification");
    stdpConfig.parameters["stdp_trials"] = "50";
    
    framework.registerTest<ExampleSTDPTest>("STDPVerification", stdpConfig);
    
    // Run all tests
    std::cout << "\n--- Running All Tests ---" << std::endl;
    auto results = framework.runAllTests();
    
    // Display results
    std::cout << "\n--- Test Results Summary ---" << std::endl;
    std::cout << framework.getSummaryReport();
    
    // Example of running a single test
    std::cout << "\n--- Running Single Test (BasicConnectivity) ---" << std::endl;
    auto singleResult = framework.runSingleTest(0);
    std::cout << "Test Status: " << (singleResult.isPassed() ? "PASSED" : "FAILED") << std::endl;
    if (!singleResult.isPassed()) {
        std::cout << "Error: " << singleResult.message << std::endl;
    }
    
    // Example of test filtering
    std::cout << "\n--- Filtering Tests ---" << std::endl;
    framework.filterTests([](const tests::TestBase& test) {
        return test.getConfig().description.find("Connectivity") != std::string::npos;
    });
    
    std::cout << "After filtering, remaining tests: " << framework.getTotalTests() << std::endl;
}

} // namespace examples

} // namespace nlm
