#ifndef NLM_CORE_TEST_H
#define NLM_CORE_TEST_H

#include "core/Config/Config.hpp"
#include "core/Config/ConfigManager.hpp"
#include "core/Logger/LoggerInitializer.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace nlm {

class ConfigTest {
public:
    static void runTests() {
        std::cout << "Running Config Tests..." << std::endl;
        
        auto config = std::make_shared<Config>();
        
        // Test basic setting and getting
        config->set("test_int", 42);
        assert(config->getOr<int64_t>("test_int", 0) == 42);
        
        config->set("test_double", 3.14);
        assert(config->getOr<double>("test_double", 0.0) == 3.14);
        
        config->set("test_float", 2.718f);
        assert(config->getOr<float>("test_float", 0.0f) == 2.718f);
        
        config->set("test_string", "hello");
        assert(config->getOr<std::string>("test_string", "") == "hello");
        
        // Test default values
        assert(config->getOr<int64_t>("nonexistent_key", 99) == 99);
        assert(config->getOr<double>("nonexistent_key", 3.14) == 3.14);
        
        // Test config sources
        config->set("key1", "source1", ConfigSource::File);
        assert(config->getOr<std::string>("key1", "") == "source1");
        
        config->set("key2", "source2", ConfigSource::CommandLine);
        assert(config->getOr<std::string>("key2", "") == "source2");
        
        config->set("key3", "source3", ConfigSource::Default);
        assert(config->getOr<std::string>("key3", "") == "source3");
        
        std::cout << "✓ Config tests passed" << std::endl;
    }
};

class ConfigManagerTest {
public:
    static void runTests() {
        std::cout << "Running ConfigManager Tests..." << std::endl;
        
        ConfigManager& manager = ConfigManager::getInstance();
        
        // Clear any existing config
        manager.set("test_key1", "value1");
        manager.set("test_key2", "123");
        manager.set("test_key3", "true");
        
        // Test getting values
        assert(manager.get<std::string>("test_key1") == "value1");
        assert(manager.get<std::string>("test_key2") == "123");
        assert(manager.get<std::string>("test_key3") == "true");
        
        // Test with defaults
        assert(manager.get<std::string>("nonexistent", "default") == "default");
        
        // Test validation
        try {
            manager.set("invalid_key", "invalid_value");
            assert(false && "Should have thrown exception");
        } catch (const std::runtime_error&) {
            // Expected
        }
        
        // Test schema validation
        manager.registerSchema("valid_key", "string", "Valid test key", false, "default", "", 0, 100);
        manager.set("valid_key", "valid_value");
        assert(manager.get<std::string>("valid_key") == "valid_value");
        
        std::cout << "✓ ConfigManager tests passed" << std::endl;
    }
};

class RandomTest {
public:
    static void runTests() {
        std::cout << "Running Random Tests..." << std::endl;
        
        auto rng1 = std::make_unique<RandomGenerator>(42);
        auto rng2 = std::make_unique<RandomGenerator>(42);
        auto rng3 = std::make_unique<RandomGenerator>(123);
        
        // Test reproducibility
        assert(rng1->getUniform() == rng2->getUniform());
        assert(rng1->getUniform() != rng3->getUniform());
        
        // Test different distributions
        float uniform1 = rng1->getUniform();
        float uniform2 = rng1->getUniform(0.0f, 1.0f);
        
        // Test normal distribution
        float normal1 = rng1->getNormal(0.0f, 1.0f);
        float normal2 = rng1->getNormal(0.0f, 1.0f);
        
        // Test integer distribution
        int int1 = rng1->getInteger(0, 100);
        int int2 = rng1->getInteger(0, 100);
        
        assert(int1 >= 0 && int1 <= 100);
        assert(int2 >= 0 && int2 <= 100);
        
        std::cout << "✓ Random tests passed" << std::endl;
    }
};

class SimulationClockTest {
public:
    static void runTests() {
        std::cout << "Running SimulationClock Tests..." << std::endl;
        
        SimulationClock clock(0.001);
        
        assert(clock.getTimestep() == 0.001);
        assert(clock.getCurrentStep() == 0);
        assert(clock.getCurrentTime() == 0.0);
        
        // Test stepping
        clock.step();
        assert(clock.getCurrentStep() == 1);
        assert(clock.getCurrentTime() == 0.001);
        
        clock.step(50);
        assert(clock.getCurrentStep() == 51);
        assert(clock.getCurrentTime() == 0.051);
        
        // Test time calculation
        clock.setCurrentTime(0.5);
        clock.setCurrentStep(500);
        assert(clock.getCurrentTime() == 0.5);
        assert(clock.getCurrentStep() == 500);
        
        std::cout << "✓ SimulationClock tests passed" << std::endl;
    }
};

class NeuronTest {
public:
    static void runTests() {
        std::cout << "Running Neuron Tests..." << std::endl;
        
        NeuronId id(1);
        Neuron neuron(id);
        
        // Test basic properties
        neuron.setType(NeuronType::Sensory);
        assert(neuron.getType() == NeuronType::Sensory);
        
        neuron.setMembranePotential(-70.0f);
        assert(neuron.getMembranePotential() == -70.0f);
        
        neuron.setThreshold(-55.0f);
        assert(neuron.getThreshold() == -55.0f);
        
        // Test LIF dynamics
        neuron.setMembranePotential(-70.0f);
        neuron.setThreshold(-55.0f);
        neuron.setFiringState(FiringState::Resting);
        assert(neuron.getFiringState() == FiringState::Resting);
        
        // Test refractory period
        neuron.setRefractoryPeriod(10);
        assert(neuron.getRefractoryPeriod() == 10);
        
        // Test firing rate
        neuron.setFiringRate(5.0f);
        assert(neuron.getFiringRate() == 5.0f);
        
        std::cout << "✓ Neuron tests passed" << std::endl;
    }
};

class SynapseTest {
public:
    static void runTests() {
        std::cout << "Running Synapse Tests..." << std::endl;
        
        Synapse synapse(1, 2, 0.5f);
        
        // Test basic properties
        assert(synapse.getPreSynapticId() == NeuronId(1));
        assert(synapse.getPostSynapticId() == NeuronId(2));
        assert(synapse.getWeight() == 0.5f);
        
        // Test plasticity flags
        synapse.enablePlasticity(true, true, true);
        assert(synapse.isPlastic() == true);
        assert(synapse.isHebbianPlastic() == true);
        assert(synapse.isSTDPPlastic() == true);
        
        // Test weight bounds
        assert(synapse.getMaxWeight() == 1.0f);
        assert(synapse.getMinWeight() == 0.0f);
        
        // Test weight changes
        synapse.setWeight(0.8f);
        assert(synapse.getWeight() == 0.8f);
        
        // Test plasticity rules
        synapse.setPlasticityRule(PlasticityRule::STDP);
        assert(synapse.getPlasticityRule() == PlasticityRule::STDP);
        
        std::cout << "✓ Synapse tests passed" << std::endl;
    }
};

class CoreSystemTest {
public:
    static void runTests() {
        std::cout << "Running Core System Tests..." << std::endl;
        
        // Initialize logger first
        LoggerInitializer loggerInitializer;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 10);
        config->set("region_count", 1);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Test basic brain functionality
        assert(brain->getTotalNeuronCount() >= 10);
        assert(brain->getTotalRegionCount() >= 1);
        
        // Test stepping
        brain->step(0, 0.001);
        
        // Test memory systems
        auto workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            assert(workingMemory->getActiveTraces() >= 0);
        }
        
        auto episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            assert(episodicMemory->getEpisodeCount() >= 0);
        }
        
        // Test neuromodulation
        auto dopamine = brain->getDopamine();
        if (dopamine) {
            assert(dopamine->getLevel() >= 0.0f);
        }
        
        // Test prediction system
        auto predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            assert(predictionSystem->isInitialized());
        }
        
        // Test development system
        auto developmentSystem = brain->getDevelopmentSystem();
        if (developmentSystem) {
            assert(developmentSystem->getCurrentStage() != DevelopmentalStage::Invalid);
        }
        
        std::cout << "✓ Core system tests passed" << std::endl;
    }
};

class BenchmarkTest {
public:
    static void runTests() {
        std::cout << "Running Benchmark Tests..." << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 2);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        // Measure performance
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation steps
        for (size_t i = 0; i < 1000; ++i) {
            brain->step(i, i * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        
        std::cout << "Performance: " << (1000.0 / duration.count()) << " steps/second" << std::endl;
        
        // Check minimum performance
        assert(duration.count() < 10.0);  // Should complete 1000 steps in under 10 seconds
        
        std::cout << "✓ Benchmark tests passed" << std::endl;
    }
};

class TestSuite {
public:
    static void runAllTests() {
        std::cout << "=== NLM Core Tests ===" << std::endl;
        std::cout << "Testing NLM (Neural Learning Machine) core functionality..." << std::endl;
        std::cout << std::endl;
        
        try {
            ConfigTest::runTests();
            std::cout << std::endl;
            
            ConfigManagerTest::runTests();
            std::cout << std::endl;
            
            RandomTest::runTests();
            std::cout << std::endl;
            
            SimulationClockTest::runTests();
            std::cout << std::endl;
            
            NeuronTest::runTests();
            std::cout << std::endl;
            
            SynapseTest::runTests();
            std::cout << std::endl;
            
            CoreSystemTest::runTests();
            std::cout << std::endl;
            
            BenchmarkTest::runTests();
            std::cout << std::endl;
            
            std::cout << "=== All Tests Passed ===" << std::endl;
            std::cout << "NLM core functionality is working correctly!" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "Test failed with exception: " << e.what() << std::endl;
            throw;
        }
    }
};

} // namespace nlm

#endif // NLM_CORE_TEST_H
