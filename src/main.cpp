// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
// Refactored modular application structure

#include "application/CommandLineParser.hpp"
#include "application/ConfigurationLoader.hpp"
#include "application/ExperimentRunner.hpp"
#include <iostream>

int main(int argc, char** argv) {
    std::cout << "NLM — Neural Learning Machine" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl << std::endl;

    auto parser = CommandLineParser();
    parser.parseArguments(argc, argv);

    auto loader = ConfigurationLoader();
    auto config = loader.load(parser.getConfigFile(), parser.getAdvancedOptions());

    auto runner = ExperimentRunner(parser.getLogLevel(), config);
    return runner.runAllTests();
}