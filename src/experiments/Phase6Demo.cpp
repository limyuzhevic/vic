/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "Phase6Demo.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>

namespace nlm {

static std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm

void printUsage() {
    std::cout << "Usage: ./nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 6 Integration Demo - Advanced Command Line Interface" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --quick                 Run in quick mode (skip some tests for faster execution)" << std::endl;
    std::cout << "  --neurons N             Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps N               Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --checkpoint PATH       Checkpoint file path (default: ./nlm_checkpoint.bin)" << std::endl;
    std::cout << "  --no-checkpointing      Disable checkpointing" << std::endl;
    std::cout << "  --no-replay             Disable replay system" << std::endl;
    std::cout << "  --no-development        Disable development system" << std::endl;
    std::cout << "  --help                  Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_phase6_demo                                    # Default run" << std::endl;
    std::cout << "  ./nlm_phase6_demo --quick                           # Quick test (faster)" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neurons 1000 --steps 5000       # Larger simulation" << std::endl;
    std::cout << "  ./nlm_phase6_demo --checkpoint ./my_checkpoint.bin   # Custom checkpoint" << std::endl;
    std::cout << "  ./nlm_phase6_demo --no-checkpointing                # Run without checkpointing" << std::endl;
}

bool parseArguments(int argc, char* argv[], nlm::Phase6Config& config, bool& quickMode) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage();
            exit(0);
        } else if (arg == "--quick") {
            quickMode = true;
        } else if (arg == "--no-checkpointing") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = static_cast<uint64_t>(std::stoul(argv[++i]));
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage();
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo (Advanced CLI) ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create demo
    nlm::Phase6Demo demo;
    
    // Phase 6 configuration
    nlm::Phase6Config config;
    bool quickMode = false;
    
    // Parse command line arguments
    if (!parseArguments(argc, argv, config, quickMode)) {
        return 1;
    }
    
    if (quickMode) {
        std::cout << "Quick mode enabled - skipping some tests for faster execution" << std::endl << std::endl;
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo (Advanced CLI) ===" << std::endl;
    std::cout << "Run time: " << getCurrentTimeString() << "" << std::endl << std::endl;
    std::cout << "Command line arguments:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Max steps: " << config.maxSteps << std::endl;
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Replay: " << (config.enableReplay ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Development: " << (config.enableDevelopment ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Checkpoint path: " << config.checkpointPath << std::endl;
    std::cout << "  Quick mode: " << (quickMode ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << std::endl;
    
    // Run the complete Phase 6 integration demo
    demo.run(config, quickMode);
    
    return 0;
}