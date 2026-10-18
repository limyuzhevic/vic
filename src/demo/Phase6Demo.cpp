// Phase6Demo.cpp - Phase 6 demo executable implementation

#include "src/demo/Phase6Demo.hpp"

int main(int argc, char* argv[]) {
    try {
        return nlm::runPhase6Demo(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}
