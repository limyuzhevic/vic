// Modern NLM Test Fixtures - Enterprise-grade testing infrastructure
// Comprehensive test organization with fixtures, parameterization, and modern assertions

#ifndef NLM_TEST_FIXTURES_H
#define NLM_TEST_FIXTURES_H

#include "nlm_test_framework.h"
#include "core/Types/Types.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "core/Random/Random.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <unordered_map>

// Enhanced Neuron Test Fixture with comprehensive setup and teardown
class NeuronTestFixture : public ::testing::Test {
protected:
    nlm::RandomGenerator rng{42};
    std::vector<std::unique_ptr<nlm::Neuron>> neurons;
    
    void SetUp() override {
        // Initialize test seed for reproducible results
        nlm_test::TestSeedManager::initSeed(42);
        
        // Create a standard set of neurons for testing
        neurons.clear();
        neurons.emplace_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(1)));
        neurons.back()->initializeRandom(rng);
        
        neurons.emplace_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(2)));
        neurons.back()->initializeRandom(rng);
        
        neurons.emplace_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(3)));
        neurons.back()->initializeRandom(rng);
    }
    
    void TearDown() override {
        neurons.clear();
        nlm_test::MemoryTracker::clear();
    }
    
    // Helper methods for common test operations
    nlm::Neuron& getNeuron(size_t index) {
        EXPECT_LT(index, neurons.size());
        return *neurons[index];
    }
    
    void validateNeuronState(const nlm::Neuron& neuron, const std::string& context = "") {
        nlm_test::ConfigurationValidator::validateNeuronParameters();
        EXPECT_GE(neuron.getMembranePotential(), -100.0f) << "Invalid membrane potential in " << context;
        EXPECT_LE(neuron.getMembranePotential(), 50.0f) << "Invalid membrane potential in " << context;
        EXPECT_GE(neuron.getThreshold(), -80.0f) << "Invalid threshold in " << context;
        EXPECT_LE(neuron.getThreshold(), -40.0f) << "Invalid threshold in " << context;
        EXPECT_GE(neuron.getWeight(), 0.0f) << "Invalid weight in " << context;
        EXPECT_LE(neuron.getWeight(), 1.0f) << "Invalid weight in " << context;
    }
};

// Synapse Test Fixture with comprehensive testing scenarios
class SynapseTestFixture : public ::testing::Test {
protected:
    nlm::RandomGenerator rng{42};
    std::vector<std::unique_ptr<nlm::Synapse>> synapses;
    
    void SetUp() override {
        nlm_test::TestSeedManager::initSeed(42);
        
        synapses.clear();
        synapses.emplace_back(std::make_unique<nlm::Synapse>(
            nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2)));
        synapses.back()->setType(nlm::SynapseType::Excitatory);
        synapses.back()->setWeight(0.5f);
        
        synapses.emplace_back(std::make_unique<nlm::Synapse>(
            nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4)));
        synapses.back()->setType(nlm::SynapseType::Inhibitory);
        synapses.back()->setWeight(0.3f);
    }
    
    void TearDown() override {
        synapses.clear();
        nlm_test::MemoryTracker::clear();
    }
    
    nlm::Synapse& getSynapse(size_t index) {
        EXPECT_LT(index, synapses.size());
        return *synapses[index];
    }
    
    void validateSynapticConnection(const nlm::Synapse& synapse) {
        EXPECT_NE(synapse.getPreSynapticNeuron(), synapse.getPostSynapticNeuron()) 
            << "Invalid synaptic connection: pre and post neurons cannot be the same";
        EXPECT_GE(synapse.getWeight(), 0.0f) << "Invalid synaptic weight";
        EXPECT_LE(synapse.getWeight(), 1.0f) << "Invalid synaptic weight";
    }
};

// Plasticity Rule Test Fixture with comprehensive testing
class PlasticityRuleTestFixture : public ::testing::Test {
protected:
    nlm::RandomGenerator rng{42};
    std::unique_ptr<nlm::PlasticityRule> plasticity_rule;
    std::unique_ptr<nlm::STDP> stdp_rule;
    std::unique_ptr<nlm::Hebbian> hebbian_rule;
    
    void SetUp() override {
        nlm_test::TestSeedManager::initSeed(42);
        
        stdp_rule = std::make_unique<nlm::STDP>();
        hebbian_rule = std::make_unique<nlm::Hebbian>();
        
        // Initialize plasticity rule based on test type
        plasticity_rule = std::make_unique<nlm::STDP>();
    }
    
    void TearDown() override {
        stdp_rule.reset();
        hebbian_rule.reset();
        plasticity_rule.reset();
        nlm_test::MemoryTracker::clear();
    }
    
    void validatePlasticityParameters(const nlm::PlasticityRule& rule) {
        EXPECT_TRUE(rule.getName() != nullptr) << "Plasticity rule must have a name";
        EXPECT_GT(std::string(rule.getName()).length(), 0) << "Plasticity rule name cannot be empty";
    }
};

// Parameterized test base class for scalable testing scenarios
class ParameterizedTestBase : public ::testing::TestWithParam<std::tuple<size_t, float, float>> {
protected:
    size_t num_neurons;
    float learning_rate;
    float time_constant;
    
    void SetUp() override {
        num_neurons = std::get<0>(GetParam());
        learning_rate = std::get<1>(GetParam());
        time_constant = std::get<2>(GetParam());
        
        nlm_test::TestSeedManager::initSeed(42);
    }
    
    // Common parameterized test operations
    template<typename T>
    void testWithParameters(const std::string& operation_name, T expected, T actual, 
                           const std::vector<std::string>& tags = {}) {
        nlm_test::PerformanceTimer timer(operation_name);
        
        TestInfo* info = ::testing::UnitTest::GetInstance()->current_test_info();
        if (info) {
            for (const auto& tag : tags) {
                info->RecordProperty("tag", tag);
            }
            info->RecordProperty("learning_rate", learning_rate);
            info->RecordProperty("time_constant", time_constant);
            info->RecordProperty("num_neurons", num_neurons);
        }
        
        EXPECT_EQ(actual, expected) << "Parameterized test failed for " << operation_name;
    }
};

// Integration Test Fixture for system-level testing
class IntegrationTestFixture : public ::testing::Test {
protected:
    std::vector<std::unique_ptr<nlm::Brain>> brains;
    std::vector<std::unique_ptr<nlm::STDP>> stdp_rules;
    
    void SetUp() override {
        nlm_test::TestSeedManager::initSeed(42);
        
        // Create integrated neural systems for testing
        for (size_t i = 0; i < 3; ++i) {
            brains.emplace_back(std::make_unique<nlm::Brain>(nlm::BrainId(i)));
            stdp_rules.emplace_back(std::make_unique<nlm::STDP>());
        }
    }
    
    void TearDown() override {
        brains.clear();
        stdp_rules.clear();
        nlm_test::MemoryTracker::clear();
    }
    
    void validateSystemHealth() {
        for (const auto& brain : brains) {
            EXPECT_NE(brain.get(), nullptr) << "Brain instance is null";
            EXPECT_TRUE(brain->isInitialized()) << "Brain is not properly initialized";
        }
    }
};

// Performance Test Fixture for benchmarking
class PerformanceTestFixture : public ::testing::Test {
protected:
    nlm_test::PerformanceTimer global_timer;
    double baseline_time_us;
    
    void SetUp() override {
        global_timer = nlm_test::PerformanceTimer("Setup");
        baseline_time_us = global_timer.elapsed_us();
    }
    
    void TearDown() override {
        double elapsed = global_timer.elapsed_us();
        EXPECT_LT(elapsed, 100000.0) << "Performance test exceeded baseline by more than 10x";
    }
    
    void assertPerformanceThreshold(double threshold_us, const std::string& test_name) {
        nlm_test::PerformanceTimer timer(test_name);
        double elapsed = timer.elapsed_us();
        EXPECT_LT(elapsed, threshold_us) << test_name << " performance threshold exceeded: " 
                                         << elapsed << " us > " << threshold_us << " us";
    }
};

// Memory Management Test Fixture for leak detection
class MemoryTestFixture : public ::testing::Test {
protected:
    size_t initial_allocated_count;
    
    void SetUp() override {
        initial_allocated_count = nlm_test::MemoryTracker::getAllocationCount();
    }
    
    void TearDown() override {
        nlm_test::MemoryTracker::clear();
        size_t final_allocated_count = nlm_test::MemoryTracker::getAllocationCount();
        
        if (final_allocated_count > initial_allocated_count) {
            std::cout << "Memory leak detected: " << (final_allocated_count - initial_allocated_count) 
                     << " allocations not freed" << std::endl;
            FAIL() << "Memory leak detected in test teardown";
        }
    }
};

// Thread Safety Test Fixture for concurrent access testing
class ThreadSafetyTestFixture : public ::testing::Test {
protected:
    std::atomic<int> shared_counter{0};
    std::mutex shared_mutex;
    std::vector<int> shared_vector;
    
    void SetUp() override {
        shared_counter.store(0);
        shared_vector = std::vector<int>(1000, 0);
        nlm_test::ThreadSafetyTester::resetAccessCount();
    }
    
    void validateThreadSafety(const std::string& operation) {
        EXPECT_EQ(nlm_test::ThreadSafetyTester::getAccessCount(), shared_vector.size()) 
            << "Thread safety validation failed for " << operation;
    }
};

#endif // NLM_TEST_FIXTURES_H
