// Simple demonstration of the new experiment framework
#include "Phase6IntegrationExperimentNew.hpp"
#include "BaseExperiment.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

namespace nlm {

class SimpleExperimentDemo {
public:
    static void demonstrate() {
        std::cout << "=== NLM Experiment Framework Demo ===" << std::endl;
        std::cout << "This demo shows the new BaseExperiment framework" << std::endl;
        std::cout << std::endl;
        
        // Create a simple experiment using the base class
        std::cout << "1. Creating a simple experiment..." << std::endl;
        
        // Create experiment with metadata
        Phase6IntegrationExperiment experiment("DemoExperiment", "A simple demo of the new experiment framework");
        
        std::cout << "   - Experiment name: " << experiment.getName() << std::endl;
        std::cout << "   - Description: " << experiment.getDescription() << std::endl;
        
        // Initialize experiment
        std::cout << "2. Initializing experiment..." << std::endl;
        if (!experiment.initialize()) {
            std::cerr << "Failed to initialize experiment!" << std::endl;
            return;
        }
        
        std::cout << "   - Configuration validated successfully" << std::endl;
        
        // Configure with custom settings
        std::cout << "3. Configuring experiment..." << std::endl;
        experiment.setConfigValue<int64_t>("neuron_count", 100);
        experiment.setConfigValue<double>("simulation_timestep", 0.001);
        experiment.setConfigValue<bool>("enable_development", true);
        
        std::cout << "   - Custom configuration applied" << std::endl;
        
        // Demonstrate progress reporting
        std::cout << "4. Running experiment with progress reporting..." << std::endl;
        experiment.setProgressCallback([](double progress) {
            static int last_percent = -1;
            int percent = static_cast<int>(progress * 100);
            if (percent != last_percent) {
                std::cout << "\r   Progress: " << percent << "%";
                std::cout.flush();
                last_percent = percent;
            }
        });
        
        // Note: Actual experiment run would require a full brain setup
        // For this demo, we'll simulate the experiment
        experiment.startTiming();
        
        // Simulate experiment work
        std::cout << "\r   Simulating experiment work..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Demonstrate metrics collection
        std::cout << "5. Collecting experiment metrics..." << std::endl;
        double elapsed = experiment.getElapsedTime();
        std::cout << "   - Experiment elapsed time: " << elapsed << "s" << std::endl;
        
        // Cleanup
        std::cout << "6. Cleaning up..." << std::endl;
        experiment.cleanup();
        
        std::cout << std::endl;
        std::cout << "=== DEMO COMPLETE ===" << std::endl;
        std::cout << "The new BaseExperiment framework provides:" << std::endl;
        std::cout << "  - Configurable experiment metadata" << std::endl;
        std::cout << "  - Automatic progress reporting" << std::endl;
        std::cout << "  - Time tracking and metrics collection" << std::endl;
        std::cout << "  - Type-safe configuration management" << std::endl;
        std::cout << "  - Error handling and validation" << std::endl;
    }
};

} // namespace nlm
