#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase4Demo.hpp"
#include "experiments/Phase5Demo.hpp"
#include "experiments/Phase6Demo.hpp"
#include <iostream>
#include <string>

using namespace nlm;

enum class DemoPhase {
    Phase2 = 2,
    Phase3,
    Phase4,
    Phase5,
    Phase6
};

DemoPhase parsePhase(const std::string& arg) {
    if (arg == "2" || arg == "phase2") return DemoPhase::Phase2;
    if (arg == "3" || arg == "phase3") return DemoPhase::Phase3;
    if (arg == "4" || arg == "phase4") return DemoPhase::Phase4;
    if (arg == "5" || arg == "phase5") return DemoPhase::Phase5;
    if (arg == "6" || arg == "phase6") return DemoPhase::Phase6;
    return DemoPhase::Phase2;
}

void printUsage() {
    std::cout << "===========================================\n";
    std::cout << "NLM Unified Demo System\n";
    std::cout << "===========================================\n\n";
    std::cout << "Usage: nlm-demo [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --phase <n|name>     Run specific phase (2, 3, 4, 5, or 6)\n";
    std::cout << "  --help               Show this help message\n";
    std::cout << "\nPhase 2: Basic Neural Computation\n";
    std::cout << "  Demonstrates LIF neuron dynamics, STDP plasticity,\n";
    std::cout << "  and structural changes.\n\n";
    std::cout << "Phase 3: World Interaction\n";
    std::cout << "  Agent learns to navigate and interact in a simple world.\n\n";
    std::cout << "Phase 4: Emerging Cognition\n";
    std::cout << "  Demonstrates prediction, memory, planning, and attention.\n\n";
    std::cout << "Phase 5: Brain-Personality\n";
    std::cout << "  Shows sandbox learning and checkpointing capabilities.\n\n";
    std::cout << "Phase 6: Final Integration\n";
    std::cout << "  Comprehensive test of all integrated brain systems.\n\n";
}

int main(int argc, char* argv[]) {
    DemoPhase phase = DemoPhase::Phase2;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        } else if (arg == "--phase" && i + 1 < argc) {
            phase = parsePhase(argv[++i]);
        } else if (arg.substr(0, 8) == "--phase=") {
            phase = parsePhase(arg.substr(8));
        }
    }
    
    std::cout << "===========================================\n";
    std::cout << "NLM Unified Demo - Phase " << static_cast<int>(phase) << "\n";
    std::cout << "===========================================\n\n";
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!\n";
        return 1;
    }
    
    brain->logStatus();
    
    // Run selected phase demo
    switch (phase) {
        case DemoPhase::Phase2:
            std::cout << "Running Phase 2: Basic Neural Computation...\n\n";
            // Run the original Phase 2 tests
            // This would need the original test functions
            std::cout << "Phase 2 tests would run here.\n";
            break;
            
        case DemoPhase::Phase3:
            std::cout << "Running Phase 3: World Interaction Demo...\n\n";
            // This would call Phase3Demo
            std::cout << "Run Phase 3 demo separately: ./nlm_phase3_demo\n";
            break;
            
        case DemoPhase::Phase4:
            std::cout << "Running Phase 4: Emerging Cognition Demo...\n\n";
            // This would call Phase4Demo
            std::cout << "Run Phase 4 demo separately: ./nlm_phase4_demo\n";
            break;
            
        case DemoPhase::Phase5:
            std::cout << "Running Phase 5: Brain-Personality Sandbox Demo...\n\n";
            // This would call Phase5Demo
            std::cout << "Run Phase 5 demo separately: ./nlm_phase5_demo\n";
            break;
            
        case DemoPhase::Phase6:
            std::cout << "Running Phase 6: Final Integration Demo...\n\n";
            // This would call Phase6Demo
            std::cout << "Run Phase 6 demo separately: ./nlm_phase6_demo\n";
            break;
    }
    
    std::cout << "\nDemo completed!\n";
    return 0;
}
