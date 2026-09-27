// Main CLI entry point for NLM
// 
// This is the main entry point for the NLM command-line interface
// providing comprehensive support for running simulations, experiments,
// benchmarks, and analysis through a unified command-line interface.

#include "cli/NLMCLI.hpp"

int main(int argc, char** argv) {
    try {
        nlm::cli::NLMCLI cli;
        return cli.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
