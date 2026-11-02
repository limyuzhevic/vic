#include "core/TestingFramework.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <cassert>

namespace nlm {
namespace testing {

// Simple smoke test to verify basic functionality
class SmokeTest : public Test {
public:
    std::string getName() const override { return "Smoke Test - Basic Functionality"; }
    
    bool run() override {
        std::cout << "  Testing basic NLM functionality..." << std::endl;
        
        // Test 1: Create a test brain
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(10), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cout << "  FAILED: Could not initialize brain" << std::endl;
            return false;
        }
        
        // Test 2: Check basic statistics
        size_t neuronCount = brain->getTotalNeuronCount();
        size_t regionCount = brain->getRegionCount();
        
        if (neuronCount == 0 || regionCount == 0) {
            std::cout << "  FAILED: Invalid brain statistics" << std::endl;
            return false;
        }
        
        // Test 3: Run simulation steps
        double timestep = brain->getTimestep();
        for (SimulationStep step = 0; step < 10; ++step) {
            brain->step(step, timestep * (step + 1));
        }
        
        // Test 4: Check spike statistics
        size_t spikeCount = brain->getTotalSpikeCount();
        if (spikeCount < 0) {
            std::cout << "  FAILED: Invalid spike count" << std::endl;
            return false;
        }
        
        // Test 5: Check that brain can be reset
        brain->reset();
        brain->initialize();
        
        std::cout << "  PASSED" << std::endl;
        return true;
    }
};

// Register smoke test
system InitializeSmokeTest() {
    auto& framework = TestFramework::getFramework();
    framework.addTest(std::make_unique<SmokeTest>());
}

// Global test registrar
static SmokeTest smokeTest;

} // namespace testing
} // namespace nlm
