#include <iostream>
#include <string>
#include <memory>
#include <vector>

// NLM includes
#include "src/core/Logger/Logger.hpp"
#include "src/experiments/Phase3Demo.cpp"
#include "src/experiments/Phase4Demo.cpp"
#include "src/experiments/Phase5Demo.cpp"
#include "src/experiments/Phase6Demo.cpp"
#include "src/experiments/Phase6TextDemo.cpp"
#include "src/benchmark.cpp"

namespace nlm {
    
    // CLI command structure
    struct CLICommand {
    public:
        std::string name;
        std::string description;
        std::vector<std::string> args;
        bool (*handler)(int argc, char** argv);
    };
    
    // Phase 2: Basic neural computation
    void runPhase2(int numSteps = 1000) {
        std::cout << "=== NLM Phase 2: Basic Neural Computation ===" << std::endl;
        std::cout << "This demo shows real spiking neural computation with STDP and Hebbian plasticity." << std::endl << std::endl;
        
        // Use the existing Phase 2 demo from src/main.cpp
        // For now, we'll just print a placeholder
        std::cout << "Phase 2 would show: LIF neuron dynamics, event-driven spike propagation, STDP and Hebbian plasticity." << std::endl;
        std::cout << "See src/main.cpp for the full implementation." << std::endl;
    }
    
    // Phase 3: World interaction
    void runPhase3(int numEpisodes = 5, int stepsPerEpisode = 200) {
        Phase3Demo demo;
        demo.run(numEpisodes, stepsPerEpisode);
    }
    
    // Phase : Cognitive capabilities
    void runPhase4(int numEpisodes = 3) {
        // Create Phase4 demo
        Phase4Demo demo;
        demo.run(numEpisodes);
    }
    
    // Phase 5: Advanced features
    void runPhase5(int numEpisodes = 2) {
        Phase5Demo demo;
        demo.run(numEpisodes);
    }
    
    // Phase 6: Integration test
    void runPhase6() {
        Phase6Demo demo;
        demo.run();
    }
    
    // Phase 6 text demo
    void runPhase6Text() {
        Phase6TextDemo demo;
        demo.run();
    }
    
    // Benchmark
    void runBenchmark(int durationSeconds = 60) {
        nlm_benchmark benchmark;
        benchmark.run(durationSeconds);
    }
    
    // Print help
    void printHelp() {
        std::cout << "=== NLM - Neural Learning Machine ===" << std::endl;
        std::cout << "A sophisticated artificial brain simulator with neural systems for memory, prediction, cognition, and neuromodulation." << std::endl << std::endl;
        
        std::cout << "USAGE: nlm <command> [options]" << std::endl << std::endl;
        
        std::cout << "COMMANDS:" << std::endl;
        std::cout << "  phase2 [steps]      Run Phase 2: Basic neural computation (default: 1000 steps)" << std::endl;
        std::cout << "  phase3 [episodes] [steps]  Run Phase 3: World interaction (default: 5 episodes, 200 steps each)" << std::endl;
        std::cout << "  phase4 [episodes]    Run Phase 4: Cognitive capabilities (default: 3 episodes)" << std::endl;
        std::cout << "  phase5 [episodes]    Run Phase 5: Advanced features (default: 2 episodes)" << std::endl;
        std::cout << "  phase6              Run Phase 6: Integration test" << std::endl;
        std::cout << "  phase6-text         Run Phase 6 text demo (text-as-a-sense)" << std::endl;
        std::cout << "  benchmark [seconds]  Run performance benchmark (default: 60 seconds)" << std::endl;
        std::cout << "  help                Show this help message" << std::endl << std::endl;
        
        std::cout << "DESCRIPTION:" << std::endl;
        std::cout << "The NLM brain implements a closed-loop system with neural dynamics, plasticity, memory, prediction, and cognition." << std::endl;
        std::cout << "It demonstrates how cognitive abilities can emerge from neural computation and experience." << std::endl << std::endl;
        
        std::cout << "PHASE SUMMARY:" << std::endl;
        std::cout << "- Phase 2: Real spiking neural computation with LIF neurons, STDP, Hebbian plasticity" << std::endl;
        std::cout << "- Phase 3: World interaction with sensory input, motor output, and reward-based learning" << std::endl;
        std::cout << "- Phase 4: Cognitive capabilities including prediction, working memory, and concept formation" << std::endl;
        std::cout << "- Phase 5: Performance optimizations and checkpointing" << std::endl;
        std::cout << "- Phase 6: Final integration of all systems (memory, neuromodulation, prediction, cognition, development)" << std::endl << std::endl;
    }
    
    // Main unified CLI
    int main(int argc, char** argv) {
        std::cout << "=== NLM - Neural Learning Machine ===" << std::endl;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        if (argc < 2) {
            printHelp();
            return 0;
        }
        
        std::string command = argv[1];
        
        try {
            if (command == "phase2") {
                int numSteps = 1000;
                if (argc > 2) numSteps = std::atoi(argv[2]);
                runPhase2(numSteps);
            } else if (command == "phase3") {
                int numEpisodes = 5;
                int stepsPerEpisode = 200;
                if (argc > 2) numEpisodes = std::atoi(argv[2]);
                if (argc > 3) stepsPerEpisode = std::atoi(argv[3]);
                runPhase3(numEpisodes, stepsPerEpisode);
            } else if (command == "phase4") {
                int numEpisodes = 3;
                if (argc > 2) numEpisodes = std::atoi(argv[2]);
                runPhase4(numEpisodes);
            } else if (command == "phase5") {
                int numEpisodes = 2;
                if (argc > 2) numEpisodes = std::atoi(argv[2]);
                runPhase5(numEpisodes);
            } else if (command == "phase6") {
                runPhase6();
            } else if (command == "phase6-text") {
                runPhase6Text();
            } else if (command == "benchmark") {
                int durationSeconds = 60;
                if (argc > 2) durationSeconds = std::atoi(argv[2]);
                runBenchmark(durationSeconds);
            } else if (command == "help") {
                printHelp();
            } else {
                std::cerr << "ERROR: Unknown command: " << command << std::endl;
                std::cerr << "Run 'nlm help' for usage information." << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            return 1;
        }
        
        return 0;
    }
}

// Entry point
int main(int argc, char** argv) {
    return nlm::main(argc, argv);
}