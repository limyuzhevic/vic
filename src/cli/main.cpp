#include "cli/CommandLineInterface.hpp"

int main(int argc, char* argv[]) {
    try {
        nlm::cli::CommandLineInterface cli("NLM Neural Learning Machine - Phase 6 Integrated Brain Simulator");
        return cli.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}
