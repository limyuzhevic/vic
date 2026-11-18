#include "TestRunner.hpp"
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include <iostream>

namespace nlm {

// Forward declarations for test classes
class BasicConnectivityTest;
class PlasticityExperimentTest;
class StdpVerificationTest;

class TestDemo {
public:
    void run() {
        std::cout << "=========================================" << std::endl;
        std::cout << "NLM TEST RUNNER DEMO" << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << "\nThis demo shows the new modular test framework.\n";
        
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("random_seed", static_cast<int64_t>(42));
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cout << "Failed to initialize brain!" << std::endl;
            return;
        }
        
        std::cout << "Brain initialized with " << brain->getTotalNeuronCount() 
                  << " neurons and " << brain->getTotalSynapseCount() << " synapses.\n\n";
        
        // Create test runner
        TestRunner runner(config);
        
        // Add tests (in real usage, these would be registered automatically)
        runner.addTest(std::make_unique<BasicConnectivityTest>());
        runner.addTest(std::make_unique<PlasticityExperimentTest>());
        runner.addTest(std::make_unique<StdpVerificationTest>());
        
        // Setup tests
        for (auto& test : runner.getTests()) {
            test->setup(brain);
        }
        
        // Run tests
        auto results = runner.runAllTests();
        
        // Print results
        runner.printResults(results);
        
        std::cout << "\nDemo complete! The new test framework provides:\n";
        std::cout << "  • Modular, reusable test components\n";
        std::cout << "  • Structured result reporting\n";
        std::cout << "  • Automatic test setup/cleanup\n";
        std::cout << "  • Performance timing\n";
        std::cout << "  • Easy integration with existing code\n";
    }
    
private:
};

} // namespace nlm

int main(int argc, char* argv[]) {
    nlm::TestDemo demo;
    demo.run();
    return 0;
}


