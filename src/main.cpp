#include "main_impl.hpp"

int main(int argc, char** argv) {
    return nlm::mainImpl(argc, argv);
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Integrated Artificial Brain                       ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║     - Memory systems (working, episodic, associative)        ║
    ║     - Neuromodulation (dopamine, curiosity, novelty)         ║
    ║     - Prediction and cognition systems                       ║
    ║     - Development and adaptation                              ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}