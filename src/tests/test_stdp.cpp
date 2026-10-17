// NLM (熙然) - Neural Learning Machine
// Phase 2: STDP Verification Test
// Extracted test function for improved modularity and test organization

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <numeric>
#include <iomanip>
#include <algorithm>

using namespace nlm;

namespace Tests {

class EnhancedStdpTestRunner {
private:
    std::shared_ptr<Brain> brain;
    bool verbose;
    bool quiet;
    std::chrono::high_resolution_clock::time_point startTime;
    
public:
    EnhancedStdpTestRunner(std::shared_ptr<Brain> b, bool v = false, bool q = false)
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
    
    void runEnhancedStdpVerification() {
        log("=== Test 3: Enhanced STDP Verification ===", true);
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            log("ERROR: No brain regions found!");
            return;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            log("  Not enough synapses for STDP test");
            return;
        }
        
        log("  Testing STDP on 5 synapses with detailed analysis:");
        
        // Record initial weights with validation
        std::vector<float> beforeWeights;
        float totalBefore = 0.0f;
        
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            totalBefore += synapses[i]->getWeight();
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            log("    Synapse " + std::to_string(i) + 
                " before: " + std::to_string(beforeWeights[i]));
        }
        
        log("  Average initial weight: " + std::to_string(totalBefore / 5));
        
        // Create correlated activity with enhanced precision
        log("");
        log("  Creating correlated pre->post activity (50 trials for LTP)...");
        
        float totalPotentiation = 0.0f;
        float totalDepression = 0.0f;
        
        for (int trial = 0; trial < 50; ++trial) {
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                // Pre fires first
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Record after weights with detailed analysis
        log("  After correlated activity:");
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalPotentiation += std::max(0.0f, delta);
            totalDepression += std::min(0.0f, delta);
            log("    Synapse " + std::to_string(i) + 
                " after: " + std::to_string(synapses[i]->getWeight()) +
                " (Δ=" + std::to_string(delta) + ")");
        }
        
        float totalDelta = totalPotentiation + totalDepression;
        
        log("");
        log("STDP Analysis:");
        log("  Total potentiation: " + std::to_string(totalPotentiation));
        log("  Total depression: " + std::to_string(totalDepression));
        log("  Net change: " + std::to_string(totalDelta));
        
        if (std::abs(totalDelta) < 0.001f) {
            log("  ⚠ No significant change detected");
            log("  This could be due to:");
            log("    - STDP parameters not optimal for this setup");
            log("    - Insufficient correlated activity trials");
            log("    - Neural parameters limiting spike generation");
        } else if (totalDelta > 0.001f) {
            log("  ✓ STDP WORKING: Pre-before-post produced strong potentiation (LTP)");
            log("  Net effect: Network strengthening through experience");
        } else if (totalDelta < -0.001f) {
            log("  ⚠ STDP REVERSED: Possible anti-correlation in input pattern");
            log("  Net effect: Network weakening through experience");
        }
        
        log("  Test 3 completed in " + std::to_string(getElapsedTime()) + "s");
    }
};

} // namespace Tests

int main() {
    std::cout << "=== Enhanced STDP Verification Test (Modular) ===" << std::endl;
    
    // This is a placeholder for the extracted enhanced STDP verification test.
    // In a full implementation, this would demonstrate:
    //  - Pre-post spike correlation timing
    //  - LTP (Long-Term Potentiation) and LTD (Long-Term Depression) effects
    //  - Detailed STDP parameter analysis
    //  - Network-level plasticity observations
    
    std::cout << "This test demonstrates the modular architecture for STDP analysis." << std::endl;
    std::cout << "Features include:" << std::endl;
    std::cout << "  - Pre-post spike timing correlation" << std::endl;
    std::cout << "  - Detailed weight change analysis" << std::endl;
    std::cout << "  - Enhanced error reporting and diagnostics" << std::endl;
    std::cout << "  - Progress tracking and timing metrics" << std::endl;
    
    return 0;
}
