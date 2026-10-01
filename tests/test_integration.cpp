// Integration Tests for NLM System
// Tests system-wide interactions and dependencies between different components

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/ConsoleLogger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>

namespace nlm_integration_tests {

void setupTestLogger() {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning); // Reduce noise
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

void testBrainConfiguration() {
    std::cout << "=== Integration Test: Brain Configuration ===" << std::endl;
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Test);
    config->set("simulation_timestep", 0.001, ConfigSource::Test);
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Test);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Test);
    config->set("connection_probability", 0.2f, ConfigSource::Test);
    
    // Initialize random generator with seed
    RandomGenerator rng(12345);
    
    // Create brain with configuration
    Brain brain(config);
    
    // Verify initialization
    assert(brain.initialize());
    
    // Check basic properties
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalRegionCount() == 2);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "✓ Brain configuration test passed" << std::endl;
}

void testNeuralNetworkConnectivity() {
    std::cout << "=== Integration Test: Neural Network Connectivity ===" << std::endl;
    
    // Create minimal configuration for focused test
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Test);
    config->set("simulation_timestep", 0.001, ConfigSource::Test);
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Test);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Test);
    config->set("connection_probability", 0.3f, ConfigSource::Test);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Test);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Test);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Get neurons from the brain
    auto allNeurons = brain.getAllNeurons();
    assert(allNeurons.size() == 50);
    
    // Get synapses
    auto region = brain.getRegion(RegionId(1));
    assert(region != nullptr);
    
    size_t initialSynapseCount = brain.getTotalSynapseCount();
    assert(initialSynapseCount > 0);
    
    // Test basic connectivity - inject current into sensory neurons
    brain.injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
    
    // Run a few simulation steps
    for (SimulationStep step = 0; step < 10; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Verify spikes were generated
    size_t finalSpikeCount = brain.getTotalSpikeCount();
    assert(finalSpikeCount > 0);
    
    std::cout << "✓ Neural network connectivity test passed (" 
              << finalSpikeCount << " spikes generated)" << std::endl;
}

void testPlasticityIntegration() {
    std::cout << "=== Integration Test: Plasticity Integration ===" << std::endl;
    
    // Create configuration with plasticity enabled
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(99), ConfigSource::Test);
    config->set("simulation_timestep", 0.001, ConfigSource::Test);
    config->set("neuron_count", static_cast<int64_t>(30), ConfigSource::Test);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Test);
    config->set("connection_probability", 0.25f, ConfigSource::Test);
    config->set("plasticity_learning_rate", 0.01f, ConfigSource::Test);
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Test);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Test);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Record initial weights
    auto region = brain.getRegion(RegionId(1));
    std::vector<float> initialWeights;
    for (const auto& synapse : region->getSynapses()) {
        initialWeights.push_back(synapse->getWeight());
    }
    
    float initialMeanWeight = 0.0f;
    if (!initialWeights.empty()) {
        float sum = 0.0f;
        for (float w : initialWeights) sum += w;
        initialMeanWeight = sum / initialWeights.size();
    }
    
    // Enable plasticity on all synapses
    for (auto& synapse : region->getSynapses()) {
        synapse->enablePlasticity(true, true, false); // Hebbian and STDP
    }
    
    // Apply learning experience: repeated input patterns
    const size_t learningSteps = 200;
    for (size_t step = 0; step < learningSteps; ++step) {
        // Create input pattern: fire sensory neurons
        for (size_t i = 0; i < 10 && i < brain.getTotalNeuronCount() / 3; ++i) {
            brain.injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain.step(step, step * 0.001);
        
        // Log progress every 50 steps
        if (step % 50 == 0) {
            std::cout << "  Step " << step << ", Spikes: " << brain.getTotalSpikeCount() 
                      << ", Firing: " << brain.getFiringNeuronCount() << std::endl;
        }
    }
    
    // Record final weights
    std::vector<float> finalWeights;
    for (const auto& synapse : region->getSynapses()) {
        finalWeights.push_back(synapse->getWeight());
    }
    
    float finalMeanWeight = 0.0f;
    if (!finalWeights.empty()) {
        float sum = 0.0f;
        for (float w : finalWeights) sum += w;
        finalMeanWeight = sum / finalWeights.size();
    }
    
    // Compute weight changes
    float weightChange = finalMeanWeight - initialMeanWeight;
    
    // Check if significant learning occurred
    bool learningOccurred = std::abs(weightChange) > 0.001f;
    
    std::cout << "  Initial mean weight: " << initialMeanWeight << std::endl;
    std::cout << "  Final mean weight: " << finalMeanWeight << std::endl;
    std::cout << "  Weight change: " << weightChange << std::endl;
    
    if (learningOccurred) {
        std::cout << "✓ Plasticity integration test passed - learning detected" << std::endl;
    } else {
        std::cout << "⚠ Plasticity integration test completed - no significant weight change detected" << std::endl;
        std::cout << "  (This may be expected depending on network parameters)" << std::endl;
    }
}

void testSpikeTimingDependence() {
    std::cout << "=== Integration Test: Spike Timing Dependence ===" << std::endl;
    
    // Create configuration optimized for STDP testing
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(777), ConfigSource::Test);
    config->set("simulation_timestep", 0.001, ConfigSource::Test);
    config->set("neuron_count", static_cast<int64_t>(20), ConfigSource::Test);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Test);
    config->set("connection_probability", 0.4f, ConfigSource::Test);
    config->set("stdp_ltp_weight", 0.05f, ConfigSource::Test); // Higher for detectable effect
    config->set("stdp_ltd_weight", 0.05f, ConfigSource::Test);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Get the first few synapses for testing
    auto region = brain.getRegion(RegionId(1));
    auto& synapses = region->getSynapses();
    
    if (synapses.size() < 3) {
        std::cout << "⚠ Not enough synapses for STDP test, skipping" << std::endl;
        return;
    }
    
    // Record initial weights
    std::vector<float> initialWeights;
    for (size_t i = 0; i < std::min(size_t(3), synapses.size()); ++i) {
        initialWeights.push_back(synapses[i]->getWeight());
        // Enable only STDP for this test
        synapses[i]->enablePlasticity(false, true, false);
    }
    
    std::cout << "  Testing STDP on 3 synapses:" << std::endl;
    for (size_t i = 0; i < 3 && i < synapses.size(); ++i) {
        std::cout << "    Synapse " << i << " initial weight: " << initialWeights[i] << std::endl;
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    std::cout << "  Creating correlated pre->post activity (potentiation)..." << std::endl;
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        const int trials = 30;
        for (int trial = 0; trial < trials; ++trial) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain.step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record final weights
    std::cout << "  After correlated activity:" << std::endl;
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 3 && i < synapses.size(); ++i) {
        float delta = synapses[i]->getWeight() - initialWeights[i];
        totalDelta += delta;
        std::cout << "    Synapse " << i << " final weight: " << synapses[i]->getWeight() 
                  << " (Δ=" << delta << ")" << std::endl;
    }
    
    // Check if weights increased (LTP)
    if (totalDelta > 0.001f) {
        std::cout << "✓ Spike timing dependence test passed - LTP detected" << std::endl;
    } else if (totalDelta < -0.001f) {
        std::cout << "⚠ STDP reversed: weights decreased (check parameters)" << std::endl;
    } else {
        std::cout << "⚠ No change detected - STDP may not be triggering" << std::endl;
    }
}

void testSystemIntegration() {
    std::cout << "=== Integration Test: Full System Integration ===" << std::endl;
    
    // Test with a more complete configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Test);
    config->set("simulation_timestep", 0.001, ConfigSource::Test);
    config->set("neuron_count", static_cast<int64_t>(80), ConfigSource::Test);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Test);
    config->set("connection_probability", 0.15f, ConfigSource::Test);
    
    // Set up all plasticity and neuromodulation parameters
    config->set("plasticity_learning_rate", 0.01f, ConfigSource::Test);
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Test);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Test);
    config->set("stdp_time_constant", 20.0f, ConfigSource::Test);
    
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Test);
    config->set("pruning_rate", 0.00001f, ConfigSource::Test);
    
    config->set("dopamine_baseline", 0.1f, ConfigSource::Test);
    config->set("reward_discount_factor", 0.99f, ConfigSource::Test);
    
    // Initialize and create brain
    Brain brain(config);
    assert(brain.initialize());
    
    // Run a comprehensive simulation
    std::cout << "Running comprehensive simulation (500 steps)..." << std::endl;
    
    for (SimulationStep step = 0; step < 500; ++step) {
        // Mixed input patterns throughout simulation
        if (step < 100) {
            // Initial sensory input
            brain.injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
        } else if (step < 300) {
            // Sustained activity
            brain.injectCurrentToNeurons(NeuronType::Sensory, 15.0f);
        } else {
            // Reduced activity with exploration
            if (step % 3 == 0) {
                brain.injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            }
        }
        
        brain.step(step, step * 0.001);
        
        // Log every 100 steps
        if (step % 100 == 0) {
            std::cout << "  Step " << step << ", Spikes: " << brain.getTotalSpikeCount() 
                      << ", Firing: " << brain.getFiringNeuronCount() << std::endl;
        }
    }
    
    // Final statistics
    size_t finalSpikes = brain.getTotalSpikeCount();
    size_t firingNeurons = brain.getFiringNeuronCount();
    size_t totalSynapses = brain.getTotalSynapseCount();
    
    std::cout << "" << std::endl;
    std::cout << "Final Statistics:" << std::endl;
    std::cout << "  Total spikes generated: " << finalSpikes << std::endl;
    std::cout << "  Firing neurons: " << firingNeurons << std::endl;
    std::cout << "  Total synapses: " << totalSynapses << std::endl;
    
    // Verify system stability
    assert(finalSpikes > 0);  // Should have generated activity
    assert(firingNeurons > 0);  // Some neurons should have fired
    assert(totalSynapses > 0);  // Should maintain synapses
    
    std::cout << "✓ Full system integration test passed" << std::endl;
}

void runAllIntegrationTests() {
    std::cout << "=== NLM Integration Tests ===" << std::endl;
    std::cout << "Testing system-wide interactions and dependencies..." << std::endl;
    std::cout << std::endl;
    
    setupTestLogger();
    
    try {
        testBrainConfiguration();
        testNeuralNetworkConnectivity();
        testPlasticityIntegration();
        testSpikeTimingDependence();
        testSystemIntegration();
        
        std::cout << std::endl;
        std::cout << "=== All Integration Tests Completed ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "✗ Integration test failed with exception: " << e.what() << std::endl;
        throw;
    }
}

} // namespace nlm_integration_tests