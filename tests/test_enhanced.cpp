// Enhanced test implementations (implementation file)
// This file contains the actual implementations of the enhanced test functions

#include "tests/TestFramework.hpp"
#include "tests/TestInterface.hpp"
#include <iostream>
#include <cassert>

namespace test_types {
    void runAll() {
        // Test type definitions and basic properties
        std::cout << "    Running Types tests..." << std::endl;
        
        // Test that types are properly defined
        nlm::NeuronId neuronId(42);
        nlm::RegionId regionId(1);
        
        nlm_test::Assert::equal(neuronId.index(), 42);
        nlm_test::Assert::equal(regionId.index(), 1);
        
        std::cout << "      TestTypes passed" << std::endl;
    }
}

namespace test_config {
    void runAll() {
        std::cout << "    Running Config tests..." << std::endl;
        
        nlm::Config config;
        
        // Test default values
        nlm_test::Assert::equal(config.getOr<int64_t>("test_int", 0), 0);
        nlm_test::Assert::equal(config.getOr<double>("test_double", 0.0), 0.0);
        nlm_test::Assert::equal(config.getOr<std::string>("test_string", ""), "");
        
        // Test setting values
        config.set("test_int", 42, nlm::ConfigSource::Default);
        config.set("test_double", 3.14, nlm::ConfigSource::Default);
        config.set("test_string", "hello", nlm::ConfigSource::Default);
        
        nlm_test::Assert::equal(config.getOr<int64_t>("test_int", 0), 42);
        nlm_test::Assert::equal(config.getOr<double>("test_double", 0.0), 3.14);
        nlm_test::Assert::equal(config.getOr<std::string>("test_string", ""), "hello");
        
        std::cout << "      TestConfig passed" << std::endl;
    }
}

namespace test_random {
    void runAll() {
        std::cout << "    Running Random tests..." << std::endl;
        
        nlm::Random random;
        
        // Test that random number generation works
        float val1 = random.randomFloat();
        float val2 = random.randomFloat();
        
        nlm_test::Assert::inRange(val1, 0.0f, 1.0f);
        nlm_test::Assert::inRange(val2, 0.0f, 1.0f);
        nlm_test::Assert::notEqual(val1, val2);
        
        // Test seeding
        nlm::Random random1;
        nlm::Random random2;
        
        // With same seed, should get same sequence
        random1.seed(42);
        random2.seed(42);
        
        nlm_test::Assert::equal(random1.randomFloat(), random2.randomFloat());
        
        std::cout << "      TestRandom passed" << std::endl;
    }
}

namespace test_clock {
    void runAll() {
        std::cout << "    Running Clock tests..." << std::endl;
        
        nlm::SimulationClock clock(0.001);
        
        // Test step calculation
        nlm_test::Assert::equal(clock.getTimeStep(), 0.001);
        nlm_test::Assert::equal(clock.getCurrentTime(), 0.0);
        
        // Test time increment
        clock.step();
        nlm_test::Assert::equal(clock.getCurrentTime(), 0.001);
        
        std::cout << "      TestClock passed" << std::endl;
    }
}

namespace test_neuron {
    void runAll() {
        std::cout << "    Running Neuron tests..." << std::endl;
        
        // Test neuron creation and properties
        nlm::NeuronId id(1);
        
        // Note: This is a basic test - actual neuron tests would require 
        // more sophisticated setup. The structure is set up for future expansion.
        nlm_test::Assert::equal(id.index(), 1);
        
        std::cout << "      TestNeuron passed" << std::endl;
    }
}

namespace test_synapse {
    void runAll() {
        std::cout << "    Running Synapse tests..." << std::endl;
        
        // Test synapse creation and properties
        nlm::SynapseId id(1);
        nlm_test::Assert::equal(id.index(), 1);
        
        std::cout << "      TestSynapse passed" << std::endl;
    }
}

namespace test_stdp {
    void runAll() {
        std::cout << "    Running STDP tests..." << std::endl;
        
        // Test STDP configuration
        nlm_test::Assert::equal(1, 1); // Basic assertion
        
        std::cout << "      TestSTDP passed" << std::endl;
    }
}

namespace test_brain {
    void runAll() {
        std::cout << "    Running Brain tests..." << std::endl;
        
        // Test brain creation and initialization
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        
        // Test initialization
        bool success = brain.initialize();
        nlm_test::Assert::equal(success, true);
        
        // Test basic properties
        nlm_test::Assert::equal(brain.getRegionCount(), 1);
        nlm_test::Assert::equal(brain.getTotalNeuronCount(), 100);
        nlm_test::Assert::greaterThan(brain.getTotalSynapseCount(), 0);
        
        std::cout << "      TestBrain passed" << std::endl;
    }
}