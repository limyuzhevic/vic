// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "cli/CLIApp.hpp"

int main(int argc, char** argv) {
    // Create and run the CLI application
    nlm::cli::CLIApp app;
    return app.run(argc, argv);
}
