// NLM (熙然) - Neural Learning Machine
// Phase 2: Basic Connectivity Test
// Extracted test function for improved modularity and test organization

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <numeric>
#include <iomanip>

using namespace nlm;

namespace Tests {

class TestRunner {
private:
    std::shared_ptr<Brain> brain;
    bool verbose;
    bool quiet;
    std::chrono::high_resolution_clock::time_point startTime;
    
public:
    TestRunner(std::shared_ptr<Brain> b, bool v = false, bool q = false)
        : brain(b), verbose(v), quiet(q) {}
    
    void startTimer() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    double getElapsedTime() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        return duration.count() / 1000.0;
    }
    
    void log(const std::string& message, bool important = false) {
        if (quiet && !important) return;
        
        auto elapsed = getElapsedTime();
        std::string timestamp = "[" + std::to_string(elapsed) + "s]";
        
        if (verbose || important) {
            std::cout << timestamp << " " << message << std::endl;
        } else {
            std::cout << message << std::endl;
        }
    }
    
    void runBasicConnectivityTest() {
        log("=== Test 1: Basic Neural Connectivity ===", true);
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            log("ERROR: No brain regions found!");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            log("  No neurons found!");
            return;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        log("  Initial spike count: " + std::to_string(initialSpikes));
        log("  Injecting current into 10 neurons...");
        
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        log("  Running 50 simulation steps...");
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        log("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            log("  ✓ SUCCESS: Spikes propagate through network", true);
        } else {
            log("  ⚠ WARNING: No spikes detected");
            log("    Checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                log("      Neuron " + std::to_string(i) + 
                    " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                    " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
        }
        log("  Test 1 completed in " + std::to_string(getElapsedTime()) + "s");
    }
};

} // namespace Tests

int main() {
    std::cout << "=== Basic Connectivity Test (Modular) ===" << std::endl;
    
    // Simple test - this would normally be integrated with the full NLM system
    std::cout << "This is a placeholder for the extracted basic connectivity test." << std::endl;
    std::cout << "In a full implementation, this would be integrated with the main CLI system." << std::endl;
    
    return 0;
}
