/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * This is now a wrapper for the advanced Phase6DemoMain application.
 * For enhanced features and command-line options, use the main executable.
 */

#include "experiments/Phase6DemoMain.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "This is the legacy demo wrapper." << std::endl;
    std::cout << "For enhanced features, use the advanced Phase6DemoMain application." << std::endl;
    std::cout << std::endl;
    
    // Forward to the advanced demo main
    return nlm::main(argc, argv);
}
