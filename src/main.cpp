// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "MainHelperFunctions.hpp"
#include "brain/Brain.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char** argv) {
    try {
        // Step 1: Display application banner
        printBanner();
        
        // Step 2: Initialize logging system
        initializeLogging();
        
        // Step 3: Load and validate configuration
        std::shared_ptr<Config> config = loadConfiguration(argc, argv);
        
        // Step 4: Initialize brain with configuration
        std::shared_ptr<Brain> brain = setupBrain(config);
        
        // Step 5: Run demonstration tests
        runTests(brain);
        
        // Step 6: Display final results and statistics
        displayResults(brain);
        
        // Successful completion
        shutdownApplication(true);
        return 0;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Application error: ") + e.what());
        shutdownApplication(false);
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Unknown error occurred");
        shutdownApplication(false);
        return 1;
    }
}
