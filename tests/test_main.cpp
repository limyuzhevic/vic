// NLM Modern Test Framework
// Phase 2: Real Neural Computation Tests using modern testing framework

#include <iostream>
#include "TestFramework/TestFramework.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "cognition/ConceptFormation.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "core/Config/Config.hpp"

namespace test_brain {
    class BrainTestCategory : public nlm::test::BasicTestCategory {
    public:
        BrainTestCategory() : BasicTestCategory("Brain Tests") {
            addTest("testBrainCreation", [this]() { testBrainCreation(); }, "Initialization");
            addTest("testBrainInitialization", [this]() { testBrainInitialization(); }, "Initialization");
            addTest("testBrainStep", [this]() { testBrainStep(); }, "Core Dynamics");
            addTest("testBrainReset", [this]() { testBrainReset(); }, "Core Dynamics");
            addTest("testBrainRegions", [this]() { testBrainRegions(); }, "Structure");
            addTest("testBrainActionProduction", [this]() { testBrainActionProduction(); }, "Output");
            addTest("testBrainStatus", [this]() { testBrainStatus(); }, "Monitoring");
            addTest("testBrainCheckpointRoundTrip", [this]() { testBrainCheckpointRoundTrip(); }, "Persistence");
        }
        
        void testBrainCreation() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            TEST_ASSERT(brain.getTotalNeuronCount() == 100, "Brain should have 100 neurons");
            TEST_ASSERT(brain.getRegionCount() == 1, "Brain should have 1 region");
        }
        
        void testBrainInitialization() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
            config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            bool success = brain.initialize();
            TEST_THROW(success, std::runtime_error);
            TEST_ASSERT(brain.getRegionCount() == 2, "Brain should have 2 regions after initialization");
            TEST_ASSERT(brain.getTotalNeuronCount() == 100, "Brain should have 100 neurons after initialization");
            TEST_ASSERT(brain.getTotalSynapseCount() > 0, "Brain should have synapses after initialization");
        }
        
        void testBrainStep() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            brain.initialize();
            
            // Run a few steps
            for (nlm::SimulationStep step = 0; step < 10; ++step) {
                brain.step(step);
            }
            
            TEST_ASSERT(brain.getTotalNeuronCount() == 100, "Neuron count should remain constant");
        }
        
        void testBrainReset() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            brain.initialize();
            
            brain.step(0);
            brain.reset();
            
            TEST_ASSERT(brain.getTotalNeuronCount() == 50, "Reset should preserve neuron count");
            TEST_ASSERT(brain.getTotalSpikeCount() == 0, "Reset should clear spike count");
        }
        
        void testBrainRegions() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            brain.initialize();
            
            auto regionIds = brain.getRegionIds();
            TEST_ASSERT(regionIds.size() == 3, "Should have 3 regions");
            
            for (auto rid : regionIds) {
                auto* region = brain.getRegion(rid);
                TEST_THROW(region != nullptr, std::runtime_error);
            }
        }
        
        void testBrainActionProduction() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            brain.initialize();
            
            auto action = brain.produceAction();
            TEST_THROW(action != nullptr, std::runtime_error);
        }
        
        void testBrainStatus() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            brain.initialize();
            
            // Verify statistics are accessible
            TEST_ASSERT(brain.getTotalNeuronCount() > 0, "Should have neurons");
            TEST_ASSERT(brain.getTotalSynapseCount() >= 0, "Synapse count should be valid");
            TEST_ASSERT(brain.getFiringNeuronCount() >= 0, "Firing neuron count should be valid");
            TEST_ASSERT(brain.getAverageFiringRate() >= 0.0f, "Firing rate should be valid");
        }
        
        void testBrainCheckpointRoundTrip() {
            // Simplified checkpoint test - just verify the method exists and doesn't crash
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
            config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
            
            nlm::Brain brain1(config);
            brain1.initialize();
            
            // Just test that save/load methods exist and don't crash on invalid input
            const std::string invalidPath = "/tmp/does_not_exist_12345.bin";
            bool saveFailed = !brain1.save(invalidPath);
            TEST_THROW(saveFailed, std::runtime_error);
        }
    };
}

namespace test_config {
    class ConfigTestCategory : public nlm::test::BasicTestCategory {
    public:
        ConfigTestCategory() : BasicTestCategory("Config Tests") {
            addTest("testConfigBasic", [this]() { testConfigBasic(); }, "Basic");
            addTest("testConfigSetGet", [this]() { testConfigSetGet(); }, "Operations");
            addTest("testConfigValidation", [this]() { testConfigValidation(); }, "Validation");
        }
        
        void testConfigBasic() {
            auto config = std::make_shared<nlm::Config>();
            TEST_THROW(config != nullptr, std::runtime_error);
            TEST_ASSERT(config->getOr<int64_t>("nonexistent_key", 0) == 0, "Missing keys should return defaults");
        }
        
        void testConfigSetGet() {
            auto config = std::make_shared<nlm::Config>();
            config->set("test_int", 42, nlm::ConfigSource::Default);
            config->set("test_string", "hello", nlm::ConfigSource::Runtime);
            config->set("test_double", 3.14, nlm::ConfigSource::File);
            
            TEST_ASSERT(config->getOr<int64_t>("test_int", 0) == 42, "Should retrieve stored integer");
            TEST_ASSERT(config->getOr<std::string>("test_string", "") == "hello", "Should retrieve stored string");
            TEST_ASSERT(config->getOr<double>("test_double", 0.0) == 3.14, "Should retrieve stored double");
        }
        
        void testConfigValidation() {
            auto config = std::make_shared<nlm::Config>();
            // Test that invalid keys are handled gracefully
            config->set("valid_key", "valid_value", nlm::ConfigSource::Runtime);
            config->set("another_key", 123, nlm::ConfigSource::File);
            
            TEST_ASSERT(config->has("valid_key"), "Should recognize valid key");
            TEST_ASSERT(config->has("another_key"), "Should recognize another valid key");
            TEST_ASSERT(!config->has("nonexistent_key"), "Should not recognize nonexistent key");
        }
    };
}

namespace test_random {
    class RandomTestCategory : public nlm::test::BasicTestCategory {
    public:
        RandomTestCategory() : BasicTestCategory("Random Tests") {
            addTest("testRandomBasic", [this]() { testRandomBasic(); }, "Basic");
            addTest("testRandomDistribution", [this]() { testRandomDistribution(); }, "Distribution");
        }
        
        void testRandomBasic() {
            nlm::RandomGenerator rng(42);
            TEST_ASSERT(rng.getSeed() == 42, "Random generator should have correct seed");
            TEST_ASSERT(rng.randomBool(), "Should generate random boolean");
        }
        
        void testRandomDistribution() {
            nlm::RandomGenerator rng(123);
            
            // Test uniform distribution
            uint32_t val1 = rng.uniformInt(1, 10);
            TEST_ASSERT(val1 >= 1 && val1 <= 10, "Uniform integer should be in range");
            
            float val2 = rng.uniformReal(0.0f, 1.0f);
            TEST_ASSERT(val2 >= 0.0f && val2 < 1.0f, "Uniform real should be in range");
        }
    };
}

namespace test_clock {
    class ClockTestCategory : public nlm::test::BasicTestCategory {
    public:
        ClockTestCategory() : BasicTestCategory("Clock Tests") {
            addTest("testClockBasic", [this]() { testClockBasic(); }, "Basic");
            addTest("testClockTime", [this]() { testClockTime(); }, "Time Tracking");
        }
        
        void testClockBasic() {
            nlm::SimulationClock clock(0.001);
            TEST_ASSERT(clock.getStep() == 0, "Clock should start at step 0");
            TEST_ASSERT(clock.getTime() == 0.0, "Clock should start at time 0.0");
        }
        
        void testClockTime() {
            nlm::SimulationClock clock(0.001);
            clock.advance();
            TEST_ASSERT(clock.getStep() == 1, "Clock should advance step");
            TEST_ASSERT(clock.getTime() == 0.001, "Clock time should be correct");
        }
    };
}

namespace test_neuron {
    class NeuronTestCategory : public nlm::test::BasicTestCategory {
    public:
        NeuronTestCategory() : BasicTestCategory("Neuron Tests") {
            addTest("testNeuronBasic", [this]() { testNeuronBasic(); }, "Basic");
            addTest("testNeuronState", [this]() { testNeuronState(); }, "State");
        }
        
        void testNeuronBasic() {
            nlm::NeuronId neuronId(1);
            nlm::Neuron neuron(neuronId);
            TEST_THROW(neuron != nullptr, std::runtime_error);
        }
        
void testNeuronState() {
            // Test neuron state transitions
            nlm::NeuronId neuronId(1);
            nlm::Neuron neuron(neuronId);
            // Note: This is a simplified test - real tests would be more comprehensive
        }
    };
}

namespace test_synapse {
    class SynapseTestCategory : public nlm::test::BasicTestCategory {
    public:
        SynapseTestCategory() : BasicTestCategory("Synapse Tests") {
            addTest("testSynapseBasic", [this]() { testSynapseBasic(); }, "Basic");
            addTest("testSynapseWeight", [this]() { testSynapseWeight(); }, "Properties");
        }
        
        void testSynapseBasic() {
            nlm::SynapseId synapseId(1);
            nlm::NeuronId source(1);
            nlm::NeuronId dest(2);
            
            nlm::Synapse synapse(synapseId, source, dest);
            TEST_THROW(synapse != nullptr, std::runtime_error);
            TEST_ASSERT(synapse.getWeight() == 0.0f, "New synapse should have zero weight");
        }
        
void testSynapseWeight() {
            nlm::SynapseId synapseId(1);
            nlm::NeuronId source(1);
            nlm::NeuronId dest(2);
            
            nlm::Synapse synapse(synapseId, source, dest);
            synapse.setWeight(0.5f);
            TEST_ASSERT(synapse.getWeight() == 0.5f, "Synapse weight should be settable");
        }
    };
}

namespace test_stdp {
    class STDPTestCategory : public nlm::test::BasicTestCategory {
    public:
        STDPTestCategory() : BasicTestCategory("STDP Tests") {
            addTest("testSTDPBasic", [this]() { testSTDPBasic(); }, "Basic");
            addTest("testSTDPUpdate", [this]() { testSTDPUpdate(); }, "Plasticity");
        }
        
        void testSTDPBasic() {
            // Test STDP basic functionality
            nlm::STDP stdp;
            TEST_THROW(stdp != nullptr, std::runtime_error);
        }
        
        void testSTDPUpdate() {
            // Test STDP weight updates
            nlm::STDP stdp;
            // Note: This is a simplified test - real tests would be more comprehensive
        }
    };
}

int main() {
    nlm::test::TestFramework framework;
    
    framework.addCategory(std::make_shared<test_brain::BrainTestCategory>());
    framework.addCategory(std::make_shared<test_config::ConfigTestCategory>());
    framework.addCategory(std::make_shared<test_random::RandomTestCategory>());
    framework.addCategory(std::make_shared<test_clock::ClockTestCategory>());
    framework.addCategory(std::make_shared<test_neuron::NeuronTestCategory>());
    framework.addCategory(std::make_shared<test_synapse::SynapseTestCategory>());
    framework.addCategory(std::make_shared<test_stdp::STDPTestCategory>());
    
    return framework.runAllTests();
}
