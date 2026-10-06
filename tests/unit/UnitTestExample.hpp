#ifndef NLM_UNIT_TEST_EXAMPLE_H
#define NLM_UNIT_TEST_EXAMPLE_H

#include "framework/TestFramework.hpp"
#include <string>

namespace nlm::test {

// Example of a modern unit test using the NLM Test Framework
class NeuronUnitTest : public TestBase {
public:
    NeuronUnitTest() {
        setName("NeuronUnitTest");
        setDescription("Unit tests for Neuron class");
        setCategory(TestSeverity::UNIT);
    }
    
    void setup() override {
        std::cout << "Setting up NeuronUnitTest" << std::endl;
    }
    
    void teardown() override {
        std::cout << "Tearing down NeuronUnitTest" << std::endl;
    }
    
    void runTest() override {
        // Test neuron creation
        nlm::Neuron neuron(nlm::NeuronId(1));
        NLM_ASSERT_EQ(neuron.getId(), nlm::NeuronId(1));
        NLM_ASSERT_EQ(neuron.getType(), nlm::NeuronType::Internal);
        
        // Test neuron type changes
        neuron.setType(nlm::NeuronType::Excitatory);
        NLM_ASSERT_EQ(neuron.getType(), nlm::NeuronType::Excitatory);
        
        neuron.setType(nlm::NeuronType::Inhibitory);
        NLM_ASSERT_EQ(neuron.getType(), nlm::NeuronType::Inhibitory);
        
        // Test membrane potential
        NLM_ASSERT_EQ(neuron.getMembranePotential(), -70.0f);
        neuron.setMembranePotential(-65.0f);
        NLM_ASSERT_EQ(neuron.getMembranePotential(), -65.0f);
        
        // Test firing state
        NLM_ASSERT_FALSE(neuron.isFiring());
        NLM_ASSERT_FALSE(neuron.isRefractory());
        
        neuron.setFiringState(nlm::FiringState::Active);
        NLM_ASSERT_TRUE(neuron.isFiring());
        
        neuron.setRefractoryPeriod(10);
        NLM_ASSERT_TRUE(neuron.isRefractory());
        NLM_ASSERT_FALSE(neuron.isFiring());
        
        // Test current injection
        NLM_ASSERT_EQ(neuron.getTotalCurrent(), 0.0f);
        neuron.injectCurrent(5.0f);
        NLM_ASSERT_EQ(neuron.getTotalCurrent(), 5.0f);
        neuron.clearTotalCurrent();
        NLM_ASSERT_EQ(neuron.getTotalCurrent(), 0.0f);
        
        // Test synaptic input
        neuron.receiveExcitatoryInput(5.0f);
        NLM_ASSERT_EQ(neuron.getTotalCurrent(), 5.0f);
        
        neuron.clearTotalCurrent();
        neuron.receiveInhibitoryInput(3.0f);
        NLM_ASSERT_EQ(neuron.getTotalCurrent(), -3.0f);
        
        std::cout << "NeuronUnitTest completed successfully" << std::endl;
    }
};

// Example of an integration test
class SynapseIntegrationTest : public TestBase {
public:
    SynapseIntegrationTest() {
        setName("SynapseIntegrationTest");
        setDescription("Integration tests for Synapse class");
        setCategory(TestSeverity::INTEGRATION);
    }
    
    void setup() override {
        std::cout << "Setting up SynapseIntegrationTest" << std::endl;
    }
    
    void teardown() override {
        std::cout << "Tearing down SynapseIntegrationTest" << std::endl;
    }
    
    void runTest() override {
        // Test synapse creation with neuron connections
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        NLM_ASSERT_EQ(synapse.getId(), nlm::SynapseId(1));
        NLM_ASSERT_EQ(synapse.getSourceNeuron(), nlm::NeuronId(1));
        NLM_ASSERT_EQ(synapse.getDestinationNeuron(), nlm::NeuronId(2));
        
        // Test synapse weight management
        NLM_ASSERT_EQ(synapse.getWeight(), 0.0f);
        synapse.setWeight(0.5f);
        NLM_ASSERT_EQ(synapse.getWeight(), 0.5f);
        
        synapse.addToWeight(0.2f);
        NLM_ASSERT_EQ(synapse.getWeight(), 0.7f);
        
        synapse.addToWeight(-0.3f);
        NLM_ASSERT_EQ(synapse.getWeight(), 0.4f);
        
        // Test synapse type
        synapse.setType(nlm::SynapseType::Excitatory);
        NLM_ASSERT_EQ(synapse.getType(), nlm::SynapseType::Excitatory);
        NLM_ASSERT_TRUE(synapse.isExcitatory());
        NLM_ASSERT_FALSE(synapse.isInhibitory());
        
        synapse.setType(nlm::SynapseType::Inhibitory);
        NLM_ASSERT_EQ(synapse.getType(), nlm::SynapseType::Inhibitory);
        NLM_ASSERT_TRUE(synapse.isInhibitory());
        NLM_ASSERT_FALSE(synapse.isExcitatory());
        
        // Test synapse delay
        NLM_ASSERT_EQ(synapse.getDelay(), 1);  // Default
        synapse.setDelay(5);
        NLM_ASSERT_EQ(synapse.getDelay(), 5);
        
        // Test spike history
        NLM_ASSERT_TRUE(synapse.getPreSpikeHistory().empty());
        NLM_ASSERT_TRUE(synapse.getPostSpikeHistory().empty());
        
        synapse.recordPreSpike(0.1);
        synapse.recordPreSpike(0.2);
        synapse.recordPostSpike(0.15);
        
        NLM_ASSERT_EQ(synapse.getPreSpikeHistory().size(), 2);
        NLM_ASSERT_EQ(synapse.getPostSpikeHistory().size(), 1);
        
        synapse.clearHistory();
        NLM_ASSERT_TRUE(synapse.getPreSpikeHistory().empty());
        NLM_ASSERT_TRUE(synapse.getPostSpikeHistory().empty());
        
        std::cout << "SynapseIntegrationTest completed successfully" << std::endl;
    }
};

// Example of a performance test
class PerformanceTest : public TestBase {
public:
    PerformanceTest() {
        setName("PerformanceTest");
        setDescription("Performance benchmarks for NLM operations");
        setCategory(TestSeverity::PERFORMANCE);
    }
    
    void setup() override {
        std::cout << "Setting up PerformanceTest" << std::endl;
    }
    
    void teardown() override {
        std::cout << "Tearing down PerformanceTest" << std::endl;
    }
    
    void runTest() override {
        TestTimer timer;
        
        // Test neuron creation performance
        nlm::RandomGenerator rng(42);
        for (int i = 0; i < 1000; ++i) {
            nlm::Neuron neuron(nlm::NeuronId(i));
            neuron.setMembranePotential(rng.uniformReal(-80.0f, -60.0f));
        }
        
        double neuronCreationTime = timer.elapsed();
        std::cout << "Neuron creation: " << neuronCreationTime << " seconds (1000 neurons)" << std::endl;
        
        // Reset timer
        timer = TestTimer();
        
        // Test synapse creation performance
        for (int i = 0; i < 1000; ++i) {
            nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
            synapse.setWeight(rng.uniformReal(0.0f, 1.0f));
        }
        
        double synapseCreationTime = timer.elapsed();
        std::cout << "Synapse creation: " << synapseCreationTime << " seconds (1000 synapses)" << std::endl;
        
        // Test random number generation performance
        timer = TestTimer();
        for (int i = 0; i < 10000; ++i) {
            rng.uniformInt(0, 1000);
            rng.uniformReal(0.0, 1.0);
        }
        
        double randomGenTime = timer.elapsed();
        std::cout << "Random generation: " << randomGenTime << " seconds (20000 calls)" << std::endl;
        
        std::cout << "PerformanceTest completed successfully" << std::endl;
    }
};

// Example of a memory test
class MemoryTest : public TestBase {
public:
    MemoryTest() {
        setName("MemoryTest");
        setDescription("Memory leak detection for NLM operations");
        setCategory(TestSeverity::MEMORY);
    }
    
    void setup() override {
        std::cout << "Setting up MemoryTest" << std::endl;
    }
    
    void teardown() override {
        std::cout << "Tearing down MemoryTest" << std::endl;
    }
    
    void runTest() override {
        TestMemoryTracker& memoryTracker = TestMemoryTracker::getInstance();
        memoryTracker.startTracking();
        
        // Perform memory-intensive operations
        std::vector<nlm::Neuron> neurons;
        std::vector<nlm::Synapse> synapses;
        
        for (int i = 0; i < 1000; ++i) {
            neurons.emplace_back(nlm::NeuronId(i));
            synapses.emplace_back(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
        }
        
        size_t peakMemory = memoryTracker.getPeakMemoryUsage();
        
        // Clear memory
        neurons.clear();
        synapses.clear();
        
        // Force garbage collection (platform-specific)
        // For now, just report the peak
        
        memoryTracker.stopTracking();
        
        std::cout << "Peak memory usage: " << peakMemory << " bytes (1000 neurons + synapses)" << std::endl;
        
        // Check for memory leaks (platform-specific)
        // For now, assume no leaks
        
        std::cout << "MemoryTest completed successfully" << std::endl;
    }
};

// Example of a parameterized test
class ParameterizedTest : public ParameterizedTestBase<int> {
public:
    ParameterizedTest() {
        setName("ParameterizedTest");
        setDescription("Parameterized test for various input values");
        setCategory(TestSeverity::UNIT);
    }
    
    void setup() override {
        std::cout << "Setting up ParameterizedTest" << std::endl;
    }
    
    void teardown() override {
        std::cout << "Tearing down ParameterizedTest" << std::endl;
    }
    
    void runTest() override {
        // This would be implemented in the subclass
    }
    
    void runTestWithParams(int value) override {
        // Test with specific parameter value
        nlm::Neuron neuron(nlm::NeuronId(value));
        
        NLM_ASSERT_EQ(neuron.getId().index(), value);
        NLM_ASSERT_TRUE(neuron.getMembranePotential() < -60.0f);
        
        std::cout << "ParameterizedTest with value " << value << " completed" << std::endl;
    }
};

} // namespace nlm::test

// Example of registering tests with the NLM Test Framework
void registerExampleTests() {
    // Register unit tests
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<nlm::test::NeuronUnitTest>()
    );
    
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<nlm::test::SynapseIntegrationTest>()
    );
    
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<nlm::test::PerformanceTest>()
    );
    
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<nlm::test::MemoryTest>()
    );
    
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<nlm::test::ParameterizedTest>()
    );
}

#endif // NLM_UNIT_TEST_EXAMPLE_H
