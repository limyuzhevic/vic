#include "Phase6Demo.hpp"
#include <iostream>
#include <string>

using namespace nlm;

int main(int argc, char* argv[]) {
    // Parse command line arguments
    Phase6DemoConfig config;
    
    // Simple argument parsing for demo configuration
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoull(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            config.regionCount = std::stoul(argv[++i]);
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        } else if (arg == "--log" && i + 1 < argc) {
            config.logFile = argv[++i];
        } else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg == "--help") {
            std::cout << "NLM Phase 6 Demo Usage:" << std::endl;
            std::cout << "  --neurons N        Number of neurons (default: 500)" << std::endl;
            std::cout << "  --steps S          Number of simulation steps (default: 2000)" << std::endl;
            std::cout << "  --regions R        Number of brain regions (default: 1)" << std::endl;
            std::cout << "  --checkpoint PATH   Checkpoint file path (default: ./nlm_phase6_checkpoint.bin)" << std::endl;
            std::cout << "  --log FILE         Log file path (default: nlm_phase6_demo.log)" << std::endl;
            std::cout << "  --no-checkpoint     Disable checkpointing" << std::endl;
            std::cout << "  --no-replay        Disable replay system" << std::endl;
            std::cout << "  --no-development    Disable development system" << std::endl;
            std::cout << "  --help             Show this help message" << std::endl;
            return 0;
        }
    }
    
    // Run the Phase 6 demo
    Phase6Demo demo;
    bool success = demo.run(config);
    
    return success ? 0 : 1;
}
