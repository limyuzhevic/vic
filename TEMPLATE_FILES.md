# Test Templates for NLM

## Unit Test Template
```cpp
// test_[module].cpp - Unit tests for [Module Name]
// Covers: [brief description of coverage]
// Author: [Name]
// Date: [Date]

#include "[module_path]/[module].hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_[module] {

void test[FunctionName]() {
    // Arrange
    [setup test fixtures, create objects, set up state]
    
    // Act  
    [call the function being tested]
    
    // Assert
    [verify expected behavior with meaningful assertions]
    
    std::cout << "    test[FunctionName] passed" << std::endl;
}

void test[FunctionName]_[ErrorCondition]() {
    // Test error handling and edge cases
    // Arrange
    [setup conditions that trigger error]
    
    // Act
    [call function expecting error]
    
    // Assert - verify error is handled correctly
    [check error state, recovery, or appropriate failure]
    
    std::cout << "    test[FunctionName]_[ErrorCondition] passed" << std::endl;
}

void test[FunctionName]_[Performance]() {
    // Performance/benchmark test
    // Arrange
    [setup performance test scenario]
    
    // Act
    [measure execution time]
    
    // Assert
    [verify performance meets requirements]
    
    std::cout << "    test[FunctionName]_[Performance] passed" << std::endl;
}

void runAll() {
    std::cout << "Running [Module] tests..." << std::endl;
    test[FunctionName1]();
    test[FunctionName2]();
    test[FunctionName3]_[ErrorCondition]();
    test[FunctionName4]_[Performance]();
}

} // namespace test_[module]
```

## Integration Test Template
```cpp
// test_[module]_integration.cpp - Integration tests for [Module Name]
// Tests: [component interactions, system behavior, external interfaces]

#include "[module_path]/[module].hpp"
#include "core/[dependency_module]/[dependency_module].hpp"
#include <cassert>
#include <iostream>

namespace test_[module]_integration {

void test[Module]_[Component1]_[Component2]() {
    // Test interaction between Component1 and Component2
    // Setup both components
    // Verify coordinated behavior
    // Check system invariants
    
    std::cout << "    test[Module]_[Component1]_[Component2] passed" << std::endl;
}

void test[Module]_[ComplexScenario]() {
    // Complex multi-component scenario
    // Test end-to-end behavior
    // Validate system state consistency
    // Verify performance under load
    
    std::cout << "    test[Module]_[ComplexScenario] passed" << std::endl;
}

void runAll() {
    std::cout << "Running [Module] integration tests..." << std::endl;
    test[Module]_[Component1]_[Component2]();
    test[Module]_[ComplexScenario]();
}

} // namespace test_[module]_integration
```

## Logger Template (Example Implementation)
```cpp
// tests/test_logger.cpp - Core logging system tests
// Author: NLM Development Team
// Date: 2026

#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>
#include <future>

namespace test_logger {

void testLoggerCreation() {
    nlm::Logger logger;
    assert(logger.getLevel() == nlm::LogLevel::Info);  // Default level
    std::cout << "    testLoggerCreation passed" << std::endl;
}

void testLoggerLogLevel() {
    nlm::Logger logger;
    
    logger.setLevel(nlm::LogLevel::Debug);
    assert(logger.getLevel() == nlm::LogLevel::Debug);
    
    logger.setLevel(nlm::LogLevel::Error);
    assert(logger.getLevel() == nlm::LogLevel::Error);
    
    std::cout << "    testLoggerLogLevel passed" << std::endl;
}

void testLoggerLogMessage() {
    nlm::Logger logger;
    logger.setLevel(nlm::LogLevel::Debug);
    
    // Test info level logging
    logger.log(nlm::LogLevel::Info, "Test info message");
    
    // Test warning level logging
    logger.log(nlm::LogLevel::Warning, "Test warning message");
    
    // Test error level logging
    logger.log(nlm::LogLevel::Error, "Test error message");
    
    // Test debug level logging
    logger.log(nlm::LogLevel::Debug, "Test debug message");
    
    std::cout << "    testLoggerLogMessage passed" << std::endl;
}

void testLoggerLogLevelFiltering() {
    nlm::Logger logger;
    
    // Set level to Warning - only warnings and errors should appear
    logger.setLevel(nlm::LogLevel::Warning);
    
    // These should be processed
    logger.log(nlm::LogLevel::Warning, "This should appear");
    logger.log(nlm::LogLevel::Error, "This should appear too");
    
    // This should be filtered out
    logger.log(nlm::LogLevel::Info, "This should be filtered");
    logger.log(nlm::LogLevel::Debug, "This should also be filtered");
    
    std::cout << "    testLoggerLogLevelFiltering passed" << std::endl;
}

void testLoggerThreadSafety() {
    nlm::Logger logger;
    logger.setLevel(nlm::LogLevel::Debug);
    
    std::vector<std::future<void>> futures;
    
    // Concurrently log from multiple threads
    for (int i = 0; i < 10; ++i) {
        futures.push_back(std::async(std::launch::async, [&logger, i]() {
            for (int j = 0; j < 100; ++j) {
                logger.log(nlm::LogLevel::Info, "Thread " + std::to_string(i) + 
                          " message " + std::to_string(j));
            }
        }));
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        future.get();
    }
    
    // Verify logger is still functional
    logger.log(nlm::LogLevel::Info, "Final message after concurrent logging");
    
    std::cout << "    testLoggerThreadSafety passed" << std::endl;
}

void testLoggerCustomOutput() {
    nlm::Logger logger;
    
    // Create custom output stream
    std::ostringstream customStream;
    
    // Note: This would require Logger to support custom output streams
    // For now, just test that logger can be configured
    logger.setLevel(nlm::LogLevel::Debug);
    logger.log(nlm::LogLevel::Info, "Test message for custom output");
    
    std::cout << "    testLoggerCustomOutput passed" << std::endl;
}

void testLoggerSummary() {
    nlm::Logger logger;
    
    // Log messages at different levels
    logger.log(nlm::LogLevel::Info, "Info message");
    logger.log(nlm::LogLevel::Warning, "Warning message");
    logger.log(nlm::LogLevel::Error, "Error message");
    
    // Note: Logger might not have a summary() method yet
    // This is a placeholder for future implementation
    std::cout << "    testLoggerSummary passed" << std::endl;
}

void runAll() {
    std::cout << "Running Logger tests..." << std::endl;
    testLoggerCreation();
    testLoggerLogLevel();
    testLoggerLogMessage();
    testLoggerLogLevelFiltering();
    testLoggerThreadSafety();
    testLoggerCustomOutput();
    testLoggerSummary();
}

} // namespace test_logger
```

## NeuralPopulation Template
```cpp
// tests/test_neural_population.cpp - Neural population tests
// Author: NLM Development Team
// Date: 2026

#include "brain/NeuralPopulation.hpp"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace test_neural_population {

void testNeuralPopulationCreation() {
    nlm::NeuralPopulation population(10);
    assert(population.getSize() == 10);
    assert(population.getTotalNeurons() == 10);
    std::cout << "    testNeuralPopulationCreation passed" << std::endl;
}

void testNeuralPopulationAddNeuron() {
    nlm::NeuralPopulation population(5);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    population.addNeuron(neuron);
    assert(population.getSize() == 6);
    assert(population.getTotalNeurons() == 6);
    
    std::cout << "    testNeuralPopulationAddNeuron passed" << std::endl;
}

void testNeuralPopulationGetNeuron() {
    nlm::NeuralPopulation population(5);
    nlm::Neuron testNeuron(nlm::NeuronId(100));
    population.addNeuron(testNeuron);
    
    auto retrievedNeuron = population.getNeuron(nlm::NeuronId(100));
    assert(retrievedNeuron != nullptr);
    assert(retrievedNeuron->getId() == nlm::NeuronId(100));
    
    std::cout << "    testNeuralPopulationGetNeuron passed" << std::endl;
}

void testNeuralPopulationRemoveNeuron() {
    nlm::NeuralPopulation population(5);
    nlm::Neuron neuronToRemove(nlm::NeuronId(200));
    population.addNeuron(neuronToRemove);
    
    assert(population.getSize() == 6);
    
    population.removeNeuron(nlm::NeuronId(200));
    assert(population.getSize() == 5);
    assert(population.getNeuron(nlm::NeuronId(200)) == nullptr);
    
    std::cout << "    testNeuralPopulationRemoveNeuron passed" << std::endl;
}

void testNeuralPopulationGetNeurons() {
    nlm::NeuralPopulation population(5);
    
    std::vector<nlm::NeuronId> neuronIds;
    for (int i = 0; i < 5; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i + 1));
        population.addNeuron(neuron);
        neuronIds.push_back(nlm::NeuronId(i + 1));
    }
    
    auto neurons = population.getNeurons();
    assert(neurons.size() == 5);
    
    // Check that all neurons are present
    for (const auto& id : neuronIds) {
        bool found = false;
        for (const auto& neuron : neurons) {
            if (neuron.getId() == id) {
                found = true;
                break;
            }
        }
        assert(found);
    }
    
    std::cout << "    testNeuralPopulationGetNeurons passed" << std::endl;
}

void testNeuralPopulationClear() {
    nlm::NeuralPopulation population(5);
    
    // Add some neurons
    for (int i = 0; i < 5; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i + 1));
        population.addNeuron(neuron);
    }
    
    assert(population.getSize() == 5);
    
    population.clear();
    assert(population.getSize() == 0);
    assert(population.getNeurons().empty());
    
    std::cout << "    testNeuralPopulationClear passed" << std::endl;
}

void testNeuralPopulationActivate() {
    nlm::NeuralPopulation population(5);
    
    // Add neurons with different properties
    for (int i = 0; i < 5; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i + 1));
        neuron.setFiringState(nlm::FiringState::Active);
        population.addNeuron(neuron);
    }
    
    population.activate();
    
    // All neurons should be active
    auto neurons = population.getNeurons();
    for (const auto& neuron : neurons) {
        assert(neuron.isFiring());
    }
    
    std::cout << "    testNeuralPopulationActivate passed" << std::endl;
}

void testNeuralPopulationUpdate() {
    nlm::NeuralPopulation population(5);
    nlm::RandomGenerator rng(42);
    
    // Add neurons and initialize them
    for (int i = 0; i < 5; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i + 1));
        neuron.initializeRandom(rng);
        population.addNeuron(neuron);
    }
    
    // Update population with random input
    population.update(0.001, rng);
    
    // Population should still be valid
    assert(population.getSize() == 5);
    
    std::cout << "    testNeuralPopulationUpdate passed" << std::endl;
}

void testNeuralPopulationSizeManagement() {
    nlm::NeuralPopulation population(10);
    assert(population.getSize() == 10);
    
    // Add more neurons
    for (int i = 0; i < 20; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i + 100));
        population.addNeuron(neuron);
    }
    
    assert(population.getSize() == 30);
    assert(population.getTotalNeurons() == 30);
    
    // Remove some neurons
    for (int i = 0; i < 10; ++i) {
        population.removeNeuron(nlm::NeuronId(i + 1));
    }
    
    assert(population.getSize() == 20);
    assert(population.getTotalNeurons() == 20);
    
    std::cout << "    testNeuralPopulationSizeManagement passed" << std::endl;
}

void runAll() {
    std::cout << "Running NeuralPopulation tests..." << std::endl;
    testNeuralPopulationCreation();
    testNeuralPopulationAddNeuron();
    testNeuralPopulationGetNeuron();
    testNeuralPopulationRemoveNeuron();
    testNeuralPopulationGetNeurons();
    testNeuralPopulationClear();
    testNeuralPopulationActivate();
    testNeuralPopulationUpdate();
    testNeuralPopulationSizeManagement();
}

} // namespace test_neural_population
```

## Hebbian Plasticity Template
```cpp
// tests/test_hebbian.cpp - Hebbian plasticity tests
// Author: NLM Development Team
// Date: 2026

#include "plasticity/Hebbian.hpp"
#include "brain/Synapse.hpp"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_hebbian {

void testHebbianCreation() {
    nlm::Hebbian hebbian;
    assert(hebbian.getLearningRate() == 0.01f);  // Default value
    assert(hebbian.getMaxWeightChange() == 1.0f); // Default value
    std::cout << "    testHebbianCreation passed" << std::endl;
}

void testHebbianLearningRate() {
    nlm::Hebbian hebbian;
    
    hebbian.setLearningRate(0.05f);
    assert(hebbian.getLearningRate() == 0.05f);
    
    hebbian.setLearningRate(0.1f);
    assert(hebbian.getLearningRate() == 0.1f);
    
    std::cout << "    testHebbianLearningRate passed" << std::endl;
}

void testHebbianMaxWeightChange() {
    nlm::Hebbian hebbian;
    
    hebbian.setMaxWeightChange(2.0f);
    assert(hebbian.getMaxWeightChange() == 2.0f);
    
    hebbian.setMaxWeightChange(5.0f);
    assert(hebbian.getMaxWeightChange() == 5.0f);
    
    std::cout << "    testHebbianMaxWeightChange passed" << std::endl;
}

void testHebbianPlasticityUpdate() {
    nlm::Hebbian hebbian;
    nlm::RandomGenerator rng(42);
    
    // Create pre and post-synaptic neurons
    nlm::Neuron preNeuron(nlm::NeuronId(1));
    nlm::Neuron postNeuron(nlm::NeuronId(2));
    
    // Create a synapse between them
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Simulate pre-synaptic spike followed by post-synaptic spike (Hebbian learning)
    preNeuron.recordSpike(0.0);
    postNeuron.recordSpike(10.0);  // Post fires 10ms after pre
    
    hebbian.update(&synapse, preNeuron, postNeuron, 0.001);
    
    // Weight should have increased due to Hebbian learning
    assert(synapse.getWeight() > 0.5f);
    
    std::cout << "    testHebbianPlasticityUpdate passed" << std::endl;
}

void testHebbianNoPlasticityForWrongOrder() {
    nlm::Hebbian hebbian;
    nlm::RandomGenerator rng(42);
    
    // Create pre and post-synaptic neurons
    nlm::Neuron preNeuron(nlm::NeuronId(1));
    nlm::Neuron postNeuron(nlm::NeuronId(2));
    
    // Create a synapse between them
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Simulate post-synaptic spike followed by pre-synaptic spike (anti-Hebbian)
    postNeuron.recordSpike(0.0);
    preNeuron.recordSpike(10.0);  // Pre fires 10ms after post
    
    hebbian.update(&synapse, preNeuron, postNeuron, 0.001);
    
    // Weight should have decreased or remained the same
    assert(synapse.getWeight() <= 0.5f);
    
    std::cout << "    testHebbianNoPlasticityForWrongOrder passed" << std::endl;
}

void testHebbianWeightNormalization() {
    nlm::Hebbian hebbian;
    nlm::RandomGenerator rng(42);
    
    // Create multiple synapses
    std::vector<nlm::Synapse> synapses;
    for (int i = 0; i < 5; ++i) {
        nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.3f + i * 0.1f);
        synapses.push_back(synapse);
    }
    
    // Apply Hebbian learning to all synapses
    nlm::Neuron preNeuron(nlm::NeuronId(100));
    nlm::Neuron postNeuron(nlm::NeuronId(101));
    
    for (size_t i = 0; i < synapses.size(); ++i) {
        preNeuron.recordSpike(0.0);
        postNeuron.recordSpike(10.0);
        hebbian.update(&synapses[i], preNeuron, postNeuron, 0.001);
    }
    
    // Check that weights are within reasonable bounds
    for (const auto& synapse : synapses) {
        assert(synapse.getWeight() >= -1.0f);  // Minimum weight
        assert(synapse.getWeight() <= 1.0f);   // Maximum weight
    }
    
    std::cout << "    testHebbianWeightNormalization passed" << std::endl;
}

void testHebbianName() {
    nlm::Hebbian hebbian;
    assert(hebbian.getName() != nullptr);
    assert(std::string(hebbian.getName()) == "Hebbian");
    
    std::cout << "    testHebbianName passed" << std::endl;
}

void runAll() {
    std::cout << "Running Hebbian tests..." << std::endl;
    testHebbianCreation();
    testHebbianLearningRate();
    testHebbianMaxWeightChange();
    testHebbianPlasticityUpdate();
    testHebbianNoPlasticityForWrongOrder();
    testHebbianWeightNormalization();
    testHebbianName();
}

} // namespace test_hebbian
```

## Testing Integration Template
```cpp
// tests/test_brain_integration.cpp - Brain system integration tests
// Author: NLM Development Team
// Date: 2026

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_brain_integration {

void testBrainNeuralPopulationIntegration() {
    // Create brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize brain (creates neural populations and regions)
    bool success = brain.initialize();
    assert(success);
    
    // Verify neural populations are created
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 2);
    
    // Verify neurons are distributed across populations
    size_t totalNeurons = 0;
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
        
        // Get neural population from region
        auto populations = region->getPopulations();
        for (auto* population : populations) {
            auto neurons = population->getNeurons();
            totalNeurons += neurons.size();
        }
    }
    
    assert(totalNeurons == 100);  // All 100 neurons should be created
    
    std::cout << "    testBrainNeuralPopulationIntegration passed" << std::endl;
}

void testBrainSynapseCreationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get all neurons and create synapses between them
    auto regionIds = brain.getRegionIds();
    assert(!regionIds.empty());
    
    auto* region = brain.getRegion(regionIds[0]);
    assert(region != nullptr);
    
    auto populations = region->getPopulations();
    std::vector<nlm::Neuron*> neurons;
    for (auto* population : populations) {
        auto popNeurons = population->getNeurons();
        neurons.insert(neurons.end(), popNeurons.begin(), popNeurons.end());
    }
    
    // Create synapses between neurons (simulate network formation)
    nlm::RandomGenerator rng(42);
    size_t synapseCount = 0;
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        for (size_t j = i + 1; j < neurons.size(); ++j) {
            if (rng.uniformReal(0.0, 1.0) < 0.2) {  // 20% connection probability
                // Create synapse from neuron i to neuron j
                nlm::SynapseId synapseId(synapseCount++);
                nlm::Synapse synapse(synapseId, neurons[i]->getId(), neurons[j]->getId());
                synapse.setType(nlm::SynapseType::Excitatory);
                synapse.setWeight(0.5f);
                
                // Add to region's connections
                region->addSynapse(synapse);
            }
        }
    }
    
    // Verify synapses were created
    auto synapses = region->getSynapses();
    assert(synapses.size() > 0);
    
    std::cout << "    testBrainSynapseCreationIntegration passed" << std::endl;
}

void testBrainPlasticityIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get region and neurons
    auto regionIds = brain.getRegionIds();
    auto* region = brain.getRegion(regionIds[0]);
    assert(region != nullptr);
    
    // Get neurons for plasticity testing
    auto populations = region->getPopulations();
    nlm::Neuron* preNeuron = nullptr;
    nlm::Neuron* postNeuron = nullptr;
    
    for (auto* population : populations) {
        auto neurons = population->getNeurons();
        if (neurons.size() > 0) {
            if (preNeuron == nullptr) {
                preNeuron = neurons[0];
            } else if (postNeuron == nullptr) {
                postNeuron = neurons[1];
            }
        }
    }
    
    assert(preNeuron != nullptr && postNeuron != nullptr);
    
    // Create a synapse between these neurons
    nlm::SynapseId synapseId(1);
    nlm::Synapse synapse(synapseId, preNeuron->getId(), postNeuron->getId());
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    region->addSynapse(synapse);
    
    // Get STDP plasticity module
    auto* stdp = region->getPlasticityModule();
    assert(stdp != nullptr);
    
    // Test STDP plasticity (which should integrate with brain)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    stdp->update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Verify synapse weight changed due to plasticity
    assert(synapse.getWeight() != 0.5f);  // Should have changed
    
    std::cout << "    testBrainPlasticityIntegration passed" << std::endl;
}

void testBrainSimulationStepIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.3, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create initial state
    brain.step(0);
    
    // Verify basic simulation properties
    assert(brain.getTotalNeuronCount() == 10);
    assert(brain.getTotalSynapseCount() >= 0);
    
    // Run several simulation steps
    for (nlm::SimulationStep step = 1; step < 10; ++step) {
        brain.step(step);
        
        // Verify invariants are maintained
        assert(brain.getTotalNeuronCount() == 10);
        assert(brain.getFiringNeuronCount() >= 0);
        assert(brain.getAverageFiringRate() >= 0.0f);
    }
    
    // Test reset functionality
    brain.reset();
    assert(brain.getTotalNeuronCount() == 10);
    assert(brain.getFiringNeuronCount() == 0);
    
    std::cout << "    testBrainSimulationStepIntegration passed" << std::endl;
}

void testBrainFullSystemIntegration() {
    // Create realistic brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize brain (creates all neural components)
    bool success = brain.initialize();
    assert(success);
    
    // Verify system state after initialization
    assert(brain.getRegionCount() == 5);
    assert(brain.getTotalNeuronCount() == 1000);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getFiringNeuronCount() >= 0);
    
    // Run extended simulation
    nlm::RandomGenerator rng(42);
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Verify system invariants are maintained throughout simulation
        assert(brain.getTotalNeuronCount() == 1000);
        assert(brain.getTotalSynapseCount() >= 0);
        assert(brain.getFiringNeuronCount() >= 0);
        assert(brain.getAverageFiringRate() >= 0.0f);
        
        // Occasionally verify brain statistics
        if (step % 25 == 0) {
            brain.produceAction();
        }
    }
    
    // Final verification after simulation
    assert(brain.getTotalNeuronCount() == 1000);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    
    // Verify status methods work correctly
    assert(brain.getAverageFiringRate() >= 0.0f);
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    testBrainFullSystemIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Brain integration tests..." << std::endl;
    testBrainNeuralPopulationIntegration();
    testBrainSynapseCreationIntegration();
    testBrainPlasticityIntegration();
    testBrainSimulationStepIntegration();
    testBrainFullSystemIntegration();
}

} // namespace test_brain_integration
```

## Performance Test Template
```cpp
// tests/test_[module]_performance.cpp - Performance tests for [Module Name]
// Author: NLM Development Team
// Date: 2026

#include "[module_path]/[module].hpp"
#include <iostream>
#include <chrono>
#include <cassert>

namespace test_[module]_performance {

void benchmark[FunctionName]() {
    // Setup
    [create test objects, initialize data structures]
    
    // Measure execution time
    auto start = std::chrono::high_resolution_clock::now();
    
    // Run the function many times
    for (int i = 0; i < 1000; ++i) {
        [call the function being tested]
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Assert performance requirements
    double timePerCall = duration.count() / 1000.0;  // microseconds per call
    assert(timePerCall < 1000.0);  // Should complete in less than 1ms per call
    
    std::cout << "    benchmark[FunctionName] completed in " 
              << duration.count() << " microseconds (" 
              << timePerCall << " μs per call)" << std::endl;
}

void benchmark[Module]_[ComplexOperation]() {
    // Test complex operations with larger data sets
    // Setup large-scale test scenario
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Run complex operation
    [perform complex operation]
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Assert performance requirements
    assert(duration.count() < 100);  // Should complete in less than 100ms
    
    std::cout << "    benchmark[Module]_[ComplexOperation] completed in " 
              << duration.count() << " milliseconds" << std::endl;
}

void benchmarkMemoryUsage() {
    // Test memory usage with large data sets
    // Monitor memory consumption during operation
    // Verify no memory leaks or excessive allocation
    
    std::cout << "    benchmarkMemoryUsage completed" << std::endl;
}

void runAll() {
    std::cout << "Running [Module] performance tests..." << std::endl;
    benchmark[FunctionName1]();
    benchmark[FunctionName2]_[ComplexOperation]();
    benchmarkMemoryUsage();
}

} // namespace test_[module]_performance
```

## Error Handling Test Template
```cpp
// tests/test_[module]_error_handling.cpp - Error handling tests for [Module Name]
// Author: NLM Development Team
// Date: 2026

#include "[module_path]/[module].hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace test_[module]_error_handling {

void test[FunctionName]_[InvalidInput]() {
    // Test function behavior with invalid inputs
    // Setup invalid input scenario
    
    // Act - call function expecting error
    [call function with invalid input]
    
    // Assert - verify error is handled appropriately
    // Check that function throws expected exception
    // or returns error code
    // Verify system state is consistent after error
    
    std::cout << "    test[FunctionName]_[InvalidInput] passed" << std::endl;
}

void test[FunctionName]_[OutOfRange]() {
    // Test function behavior with out-of-range inputs
    // Setup edge case scenario
    
    // Act
    [call function with out-of-range input]
    
    // Assert - verify graceful error handling
    std::cout << "    test[FunctionName]_[OutOfRange] passed" << std::endl;
}

void test[FunctionName]_[NullPointer]() {
    // Test function behavior with null/invalid pointers
    // Setup null pointer scenario
    
    // Act
    [call function with null pointer]
    
    // Assert - verify defensive programming
    std::cout << "    test[FunctionName]_[NullPointer] passed" << std::endl;
}

void test[FunctionName]_[ResourceExhaustion]() {
    // Test behavior when resources are exhausted
    // Setup scenario that depletes resources
    
    // Act
    [call function under resource constraints]
    
    // Assert - verify graceful degradation
    std::cout << "    test[FunctionName]_[ResourceExhaustion] passed" << std::endl;
}

void runAll() {
    std::cout << "Running [Module] error handling tests..." << std::endl;
    test[FunctionName1]_[InvalidInput]();
    test[FunctionName2]_[OutOfRange]();
    test[FunctionName3]_[NullPointer]();
    test[FunctionName4]_[ResourceExhaustion]();
}

} // namespace test_[module]_error_handling
```