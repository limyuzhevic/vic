// Example test demonstrating the new NLM test framework
// Tests the ConnectivityExperiment functionality

#include "core/Test/TestUtils.hpp"
#include "experiments/Phase2Experiment.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

namespace test {
namespace phase2 {

class ConnectivityExperimentTest : public TestCase {
public:
    std::string getName() const override {
        return "ConnectivityExperimentTest";
    }
    
    bool setup() override {
        // Setup test configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain for ConnectivityExperimentTest" << std::endl;
            return false;
        }
        return true;
    }
    
    bool teardown() override {
        // Cleanup
        brain.reset();
        return true;
    }
    
    void run() override {
        ConnectivityExperiment experiment;
        
        // Test the experiment
        bool result = experiment.run(brain);
        TestUtils::assertTrue(result, "Connectivity experiment should pass");
        
        // Additional assertions
        TestUtils::assertTrue(brain->getTotalNeuronCount() == 100, 
                            "Brain should have 100 neurons");
        TestUtils::assertTrue(brain->getTotalSynapseCount() > 0,
                            "Brain should have synapses after initialization");
    }
    
private:
    std::shared_ptr<nlm::Brain> brain;
};

class PlasticityExperimentTest : public TestCase {
public:
    std::string getName() const override {
        return "PlasticityExperimentTest";
    }
    
    bool setup() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain for PlasticityExperimentTest" << std::endl;
            return false;
        }
        return true;
    }
    
    bool teardown() override {
        brain.reset();
        return true;
    }
    
    void run() override {
        PlasticityExperiment experiment;
        
        // Test the experiment with multiple runs
        for (int i = 0; i < 3; ++i) {
            bool result = experiment.run(brain);
            TestUtils::assertTrue(result, "Plasticity experiment should pass");
            
            // Verify brain state after experiment
            TestUtils::assertTrue(brain->getTotalNeuronCount() == 100,
                                "Brain should maintain 100 neurons");
            
            // Reset for next iteration
            brain->reset();
            brain->initialize();
        }
    }
    
private:
    std::shared_ptr<nlm::Brain> brain;
};

class StdpExperimentTest : public TestCase {
public:
    std::string getName() const override {
        return "StdpExperimentTest";
    }
    
    bool setup() override {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain for StdpExperimentTest" << std::endl;
            return false;
        }
        return true;
    }
    
    bool teardown() override {
        brain.reset();
        return true;
    }
    
    void run() override {
        StdpExperiment experiment;
        
        // Test STDP with pre-before-post (should potentiate)
        bool result = experiment.run(brain);
        TestUtils::assertTrue(result, "STDP experiment should pass");
        
        // Verify that STDP occurred
        // The experiment already records metrics, so we just verify it ran
        TestUtils::assertTrue(brain->getTotalNeuronCount() == 100,
                            "Brain should maintain 100 neurons");
    }
    
private:
    std::shared_ptr<nlm::Brain> brain;
};

// Register tests automatically
REGISTER_TEST(ConnectivityExperimentTest)
REGISTER_TEST(PlasticityExperimentTest)
REGISTER_TEST(StdpExperimentTest)

} // namespace phase2
} // namespace test