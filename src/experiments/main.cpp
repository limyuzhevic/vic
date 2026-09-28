// Main entry point for NLM experiments
// Provides unified command-line interface for all NLM experiments

#include "src/experiments/NLMExperimentRunner.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    nlm::NLMExperimentRunner runner;
    return runner.run(argc, argv) ? 0 : 1;
}
