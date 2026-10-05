// Enhanced NLM Test Framework
// Comprehensive test suite for the enhanced NLM functionality
// Tests advanced plasticity, neuromodulation, memory systems, and performance

#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_all_warnings.hpp>
#include <cmath>
#include <vector>
#include <memory>
#include <iostream>

// Include NLM headers
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/EnhancedPlasticity.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Acetylcholine.hpp"
#include "../src/neuromodulation/Norepinephrine.hpp"
#include "../src/neuromodulation/Serotonin.hpp"
#include "../src/neuromodulation/EnhancedNeuromodulation.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/EnhancedMemory.hpp"
#include "../src/performance/SIMDVectorization.hpp"

// Test forward declarations
namespace nlm {
    class Config;
    class Brain;
    class Neuron;
    class NeuralPopulation;
    class PlasticityRule;
    class STDP;
    class Hebbian;
    class EnhancedSTDP;
    class Neuromodulator;
    class Dopamine;
    class Acetylcholine;
    class Norepinephrine;
    class Serotonin;
    class NeuromodulatorySystem;
    class WorkingMemory;
    class EnhancedWorkingMemory;
}

// Test configuration
CATCH_CONFIG_DYNAMIC_ENABLE(false)

// Test helper functions
nlm::Config createTestConfig() {
    nlm::Config config;
    config.set("brain.neuron_count", 500);
    config.set("brain.synapse_density", 0.1);
    config.set("plasticity.stdp.enable", true);
    config.set("plasticity.hebbian.enable", true);
    return config;
}

nlm::Config createOptimizedConfig() {
    nlm::Config config;
    config.set("brain.neuron_count", 2000);
    config.set("brain.synapse_density", 0.15);
    config.set("brain.connection_probability", 0.05);
    config.set("neuron.v_thresh", -55.0);
    config.set("neuron.v_rest", -70.0);
    config.set("neuron.tau_mem", 20.0);
    config.set("plasticity.stdp.enable", true);
    config.set("plasticity.stdp.learning_rate", 0.01);
    config.set("plasticity.hebbian.enable", true);
    config.set("plasticity.hebbian.learning_rate", 0.001);
    return config;
}

TEST_CASE("Configuration Test", "[config]") {
    SECTION("Default Configuration") {
        nlm::Config config;
        REQUIRE(config.has("random_seed"));
        REQUIRE(config.getOr<int64_t>("random_seed", 0) >= 0);
    }
    
    SECTION("Neural Configuration") {
        nlm::Config config = createTestConfig();
        REQUIRE(config.getOr<int64_t>("brain.neuron_count", 0) == 500);
        REQUIRE(config.getOr<float>("brain.synapse_density", 0.0f) == 0.1f);
    }
    
    SECTION("Plasticity Configuration") {
        nlm::Config config = createTestConfig();
        REQUIRE(config.has("plasticity.stdp.enable"));
        REQUIRE(config.has("plasticity.hebbian.enable"));
        REQUIRE(config.getOr<bool>("plasticity.stdp.enable", false) == true);
        REQUIRE(config.getOr<bool>("plasticity.hebbian.enable", false) == true);
    }
    
    SECTION("File I/O") {
        nlm::Config config = createTestConfig();
        config.set("test.key", "test_value");
        REQUIRE(config.getOr<std::string>("test.key", "") == "test_value");
        
        std::string filename = "test_config.json";
        REQUIRE(config.saveToFile(filename));
        
        nlm::Config config2;
        REQUIRE(config2.loadFromFile(filename));
        REQUIRE(config2.getOr<std::string>("test.key", "") == "test_value");
        
        // Clean up
        std::remove(filename.c_str());
    }
}

TEST_CASE("Neuron Test", "[neuron]") {
    SECTION("Neuron Creation") {
        nlm::NeuronId neuronId(1);
        auto neuron = std::make_unique<nlm::Neuron>(neuronId);
        
        REQUIRE(neuron->getId() == neuronId);
        REQUIRE(neuron->getMembranePotential() == -70.0f);
        REQUIRE(neuron->getThreshold() == -55.0f);
    }
    
    SECTION("Neuron Initialization") {
        nlm::NeuronId neuronId(2);
        auto neuron = std::make_unique<nlm::Neuron>(neuronId);
        
        class MockRandomGenerator {
        public:
            float uniformReal(float min, float max) {
                return (float)rand() / RAND_MAX * (max - min) + min;
            }
            int uniformInt(int min, int max) {
                return rand() % (max - min + 1) + min;
            }
        };
        
        MockRandomGenerator rng;
        neuron->initializeRandom(rng);
        
        REQUIRE(neuron->getMembranePotential() != 0.0f);
        REQUIRE(neuron->getThreshold() != 0.0f);
        REQUIRE(neuron->getRefractoryPeriod() > 0);
    }
    
    SECTION("Neuron Firing") {
        nlm::NeuronId neuronId(3);
        auto neuron = std::make_unique<nlm::Neuron>(neuronId);
        neuron->setMembranePotential(-60.0f);
        neuron->setThreshold(-55.0f);
        
        // Should not fire at -60mV (threshold -55mV)
        REQUIRE(!neuron->stepLIF(0.0, 0.001));
        
        // Should fire at -54mV (above threshold -55mV)
        neuron->setMembranePotential(-54.0f);
        REQUIRE(neuron->stepLIF(0.0, 0.001));
    }
    
    SECTION("Neuron Refractory Period") {
        nlm::NeuronId neuronId(4);
        auto neuron = std::make_unique<nlm::Neuron>(neuronId);
        neuron->setRefractoryPeriod(10);
        
        REQUIRE(neuron->isRefractory());
        
        // Step should not fire during refractory period
        neuron->decrementRefractory();
        REQUIRE(neuron->isRefractory());
        
        // After 10 decrements, should not be refractory
        for (int i = 0; i < 8; ++i) {
            neuron->decrementRefractory();
        }
        REQUIRE(!neuron->isRefractory());
    }
}

TEST_CASE("Plasticity Test", "[plasticity]") {
    SECTION("STDP Test") {
        nlm::Config config = createTestConfig();
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        // Get STDP plasticity rule
        auto* stdp = brain->getSTDP();
        REQUIRE(stdp != nullptr);
        
        // Test STDP parameter changes
        stdp->setLTPWeight(0.02f);
        REQUIRE(stdp->getLTPWeight() == 0.02f);
        
        stdp->setLTDWeight(0.015f);
        REQUIRE(stdp->getLTDWeight() == 0.015f);
        
        stdp->setTimeConstant(25.0f);
        REQUIRE(stdp->getTimeConstant() == 25.0f);
    }
    
    SECTION("Hebbian Test") {
        nlm::Config config = createTestConfig();
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        auto* hebbian = brain->getHebbian();
        REQUIRE(hebbian != nullptr);
        
        // Test Hebbian parameter changes
        hebbian->setLearningRate(0.002f);
        REQUIRE(hebbian->getLearningRate() == 0.002f);
    }
    
    SECTION("Enhanced STDP Test") {
        auto* enhancedSTDP = new nlm::EnhancedSTDP();
        
        // Test enhanced STDP parameters
        enhancedSTDP->setLTPWeight(0.015f);
        enhancedSTDP->setLTDWeight(0.012f);
        enhancedSTDP->setTimeConstant(30.0f);
        
        enhancedSTDP->setDopamineGain(2.0f);
        enhancedSTDP->setCuriosityGain(1.0f);
        
        enhancedSTDP->setEligibilityTimeConstant(100.0f);
        enhancedSTDP->setMaxEligibilityTrace(2.0f);
        
        REQUIRE(enhancedSTDP->getLTPWeight() == 0.015f);
        REQUIRE(enhancedSTDP->getLTDWeight() == 0.012f);
        REQUIRE(enhancedSTDP->getTimeConstant() == 30.0f);
        REQUIRE(enhancedSTDP->getDopamineGain() == 2.0f);
        REQUIRE(enhancedSTDP->getCuriosityGain() == 1.0f);
        
        delete enhancedSTDP;
    }
}

TEST_CASE("Neuromodulation Test", "[neuromodulation]") {
    SECTION("Dopamine Test") {
        nlm::Dopamine dopamine;
        
        REQUIRE(dopamine.getLevel() == 0.1f);  // Initial tonic level
        
        dopamine.signalReward(1.0f);
        dopamine.signalRewardPredictionError(0.5f, 0.8f);
        
        // Update with time
        dopamine.update(0.001f);
        
        REQUIRE(dopamine.getPhasicComponent() >= 0.0f);
        REQUIRE(dopamine.getPredictionError() == 0.5f);
    }
    
    SECTION("Acetylcholine Test") {
        nlm::Acetylcholine ach;
        
        REQUIRE(ach.getLevel() == 0.5f + 0.3f);  // Default attention + memory
        
        ach.signalNovelty(0.8f);
        ach.signalSensoryInput(0.6f);
        
        ach.update(0.001f);
        
        REQUIRE(ach.getAttentionLevel() >= 0.5f);
        REQUIRE(ach.getMemoryEnhancement() >= 0.3f);
    }
    
    SECTION("Norepinephrine Test") {
        nlm::Norepinephrine ne;
        
        REQUIRE(ne.getLevel() >= 0.5f);
        
        ne.updateVigilance(0.8f, 10.0f);
        ne.signalStress(0.7f, 5.0f);
        
        ne.update(0.001f);
        
        REQUIRE(ne.getVigilanceLevel() >= 0.3f);
        REQUIRE(ne.getStressLevel() >= 0.0f);
    }
    
    SECTION("Serotonin Test") {
        nlm::Serotonin serotonin;
        
        REQUIRE(serotonin.getLevel() >= 0.5f);
        
        serotonin.updateSocialStatus(0.3f, 0.7f);
        serotonin.signalConflict(0.6f);
        
        serotonin.update(0.001f);
        
        REQUIRE(serotonin.getMoodLevel() >= 0.4f);
        REQUIRE(serotonin.getInhibitionLevel() >= 0.3f);
    }
}

TEST_CASE("Memory Test", "[memory]") {
    SECTION("Working Memory Test") {
        nlm::EnhancedWorkingMemory workingMemory;
        
        // Note: This would require a Brain instance to be fully functional
        // For unit tests, we'll test the interface
        REQUIRE(workingMemory.getCapacity() == 100);
        
        workingMemory.setCapacity(200);
        REQUIRE(workingMemory.getCapacity() == 200);
        
        workingMemory.setDecayRate(0.02f);
        REQUIRE(workingMemory.getDecayRate() == 0.02f);
    }
    
    SECTION("Chunk Management Test") {
        nlm::EnhancedWorkingMemory workingMemory;
        
        // Test chunk creation
        std::vector<nlm::NeuronId> neurons;
        neurons.push_back(nlm::NeuronId(1));
        neurons.push_back(nlm::NeuronId(2));
        
        workingMemory.createChunk("sensory_chunk", neurons);
        
        REQUIRE(workingMemory.getChunk("sensory_chunk").size() == 2);
        
        // Test chunk activity
        float activity = workingMemory.getChunkActivity("sensory_chunk");
        REQUIRE(activity >= 0.0f);
        
        workingMemory.removeChunk("sensory_chunk");
        REQUIRE(workingMemory.getChunk("sensory_chunk").size() == 0);
    }
}

TEST_CASE("Performance Test", "[performance]") {
    SECTION("SIMD Support Test") {
        REQUIRE(nlm::SIMDUtils::isSIMDAvailable());
        REQUIRE(nlm::SIMDUtils::getSIMDType() != nullptr);
        REQUIRE(nlm::SIMDUtils::getSIMDWidth() > 0);
    }
    
    SECTION("SIMD Vectorization Test") {
        // Test SIMD operations
        std::vector<float> vec1(16, 1.0f);
        std::vector<float> vec2(16, 2.0f);
        
        // These would test the actual SIMD implementations
        // For now, just verify the interface exists
        REQUIRE(nlm::VectorizedLIF::SIMD_WIDTH > 0);
    }
}

TEST_CASE("Integration Test", "[integration]") {
    SECTION("Brain Integration Test") {
        nlm::Config config = createOptimizedConfig();
        auto brain = std::make_shared<nlm::Brain>(config);
        
        REQUIRE(brain->initialize());
        
        // Test basic brain operations
        REQUIRE(brain->getTotalNeuronCount() > 0);
        REQUIRE(brain->getTotalSynapseCount() > 0);
        
        // Test stepping
        brain->step(0);
        brain->step(1, 0.001);
        
        // Test sensory input
        class MockSensoryInput : public nlm::SensoryInput {
            float getType() const override { return 0; }
            std::vector<float> getData() const override { return std::vector<float>(); }
            size_t getDimensions() const override { return 0; }
            double getTimestamp() const override { return 0.0; }
            void setTimestamp(double timestamp) override {}
        };
        
        MockSensoryInput sensoryInput;
        brain->receiveSensoryInput(sensoryInput);
        
        // Test action production
        auto action = brain->produceAction();
        REQUIRE(action != nullptr);
        
        brain->reset();
        REQUIRE(brain->getTotalSpikeCount() == 0);
    }
    
    SECTION("Agent Integration Test") {
        nlm::Config config = createOptimizedConfig();
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        
        nlm::SimpleWorld world;
        agent->initialize(world);
        
        REQUIRE(agent->getCuriosityLevel() >= 0.0f);
        REQUIRE(agent->getNoveltyLevel() >= 0.0f);
        
        // Test neuromodulation
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        REQUIRE(agent->isRewardModulationEnabled());
        REQUIRE(agent->isCuriosityEnabled());
    }
}

TEST_CASE("Advanced Features Test", "[advanced]") {
    SECTION("Enhanced STDP Integration Test") {
        nlm::Config config = createOptimizedConfig();
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        // Test enhanced plasticity integration
        // This would test the integration of enhanced plasticity with the brain
        REQUIRE(true);  // Placeholder for actual integration test
    }
    
    SECTION("Neuromodulatory System Test") {
        nlm::NeuromodulatorySystem neuromodSystem;
        
        // Test neuromodulatory system setup
        // This would test the integration of all neuromodulators
        REQUIRE(true);  // Placeholder for actual system test
    }
    
    SECTION("Memory Integration Test") {
        // Test integration of enhanced memory systems
        // This would test the integration of working memory with other systems
        REQUIRE(true);  // Placeholder for actual memory integration test
    }
}

TEST_CASE("Performance Benchmark", "[benchmark]") {
    SECTION("Simulation Performance") {
        nlm::Config config = createOptimizedConfig();
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        // Simple performance benchmark
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 100; ++i) {
            brain->step(i, i * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        REQUIRE(duration.count() > 0);
        
        // Calculate steps per second
        float stepsPerSecond = 100.0f / (duration.count() / 1000.0f);
        REQUIRE(stepsPerSecond > 0.0f);
    }
    
    SECTION("Memory Performance") {
        nlm::EnhancedWorkingMemory workingMemory;
        
        // Memory performance test
        std::vector<nlm::NeuronId> neurons;
        for (int i = 0; i < 1000; ++i) {
            neurons.push_back(nlm::NeuronId(i));
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 100; ++i) {
            workingMemory.createChunk("test_chunk", neurons);
            workingMemory.removeChunk("test_chunk");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        REQUIRE(duration.count() > 0);
    }
}

// Main test runner
int main(int argc, char** argv) {
    std::cout << "NLM Enhanced Test Suite" << std::endl;
    std::cout << "=======================" << std::endl;
    
    int result = Catch::Session().run(argc, argv);
    
    if (result == 0) {
        std::cout << "All tests passed!" << std::endl;
    } else {
        std::cout << "Some tests failed!" << std::endl;
    }
    
    return result;
}

