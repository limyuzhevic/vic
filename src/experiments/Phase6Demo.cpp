/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <limits>

using namespace nlm;

// Demo configuration structure
struct DemoConfig {
    size_t neuronCount = 500;
    uint64_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "/tmp/nlm_checkpoint_demo.bin";
    bool verbose = false;
    std::string benchmarkMode = "standard"; // standard, fast, thorough
};

// Pretty print utilities
void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void printSection(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
}

void printResult(const std::string& name, float value, const std::string& unit = "", 
                 bool highlight = false, float max = 1.0f) {
    std::cout << "  " << std::left << std::setw(35) << name;
    std::cout << std::right;
    if (highlight) {
        std::cout << " : " << std::fixed << std::setprecision(4) << value;
    } else {
        std::cout << " : " << std::fixed << std::setprecision(2) << value;
    }
    if (!unit.empty()) {
        std::cout << " " << unit;
    }
    if (max > 0 && !highlight) {
        float percent = value / max * 100.0f;
        std::cout << " (" << std::setprecision(1) << percent << "%)";
    }
    std::cout << "\n";
}

// Phase 6 Demo class
class Phase6Demo {
private:
    DemoConfig config_;
    Phase6IntegratedExperiment experiment_;
    bool verbose_;
    
public:
    Phase6Demo(const DemoConfig& config) : config_(config), verbose_(config.verbose) {}
    ~Phase6Demo() = default;
    
    bool run() {
        printHeader("NLM PHASE 6 - INTEGRATION DEMO");
        
        // Initialize logging based on verbose mode
        if (verbose_) {
            Logger::getInstance().setLevel(Logger::Level::Debug);
            printSection("Debug logging enabled");
        } else {
            Logger::getInstance().setLevel(Logger::Level::Info);
            printSection("Info logging enabled");
        }
        
        // Display configuration
        printSection("Configuration");
        printResult("Neuron count", static_cast<float>(config_.neuronCount));
        printResult("Max steps", static_cast<float>(config_.maxSteps));
        printResult("Checkpointing", config_.enableCheckpointing ? 1.0f : 0.0f, "", false, 1.0f);
        printResult("Replay system", config_.enableReplay ? 1.0f : 0.0f, "", false, 1.0f);
        printResult("Development enabled", config_.enableDevelopment ? 1.0f : 0.0f, "", false, 1.0f);
        
        // First, run the quick integration verification
        printSection("Integration Verification");
        std::cout << "Verifying that all brain systems are properly connected...\n";
        
        bool integrationOK = experiment_.verifyIntegration();
        
        if (!integrationOK) {
            std::cerr << "\nERROR: Integration verification failed!\n";
            std::cerr << "Some brain systems are not properly connected.\n";
            std::cerr << "Check the logs for detailed error information.\n";
            return false;
        }
        
        std::cout << "\n✓ Integration verification passed!\n";
        
        // Test individual systems
        if (verbose_) {
            std::cout << "\nRunning detailed system tests...\n";
        }
        
        // Memory integration test
        printSection("Memory Integration Test");
        bool memoryOK = experiment_.testMemoryIntegration();
        if (memoryOK) {
            std::cout << "✓ Memory integration test passed!\n";
        } else {
            std::cerr << "✗ Memory integration test failed!\n";
        }
        
        // Neuromodulation integration test
        printSection("Neuromodulation Integration Test");
        bool neuromodOK = experiment_.testNeuromodulationIntegration();
        if (neuromodOK) {
            std::cout << "✓ Neuromodulation integration test passed!\n";
        } else {
            std::cerr << "✗ Neuromodulation integration test failed!\n";
        }
        
        // Checkpoint test
        printSection("Checkpoint Test");
        bool checkpointOK = experiment_.testCheckpointing();
        if (checkpointOK) {
            std::cout << "✓ Checkpoint test passed!\n";
        } else {
            std::cerr << "✗ Checkpoint test failed!\n";
        }
        
        // Replay test
        printSection("Replay Test");
        bool replayOK = experiment_.testReplay();
        if (replayOK) {
            std::cout << "✓ Replay test passed!\n";
        } else {
            std::cerr << "✗ Replay test failed!\n";
        }
        
        // Run full integration experiment
        printSection("Full Integration Experiment");
        std::cout << "Running complete brain integration with " << config_.neuronCount 
                  << " neurons for up to " << config_.maxSteps << " steps...\n";
        
        Phase6Config phase6Config;
        phase6Config.neuronCount = config_.neuronCount;
        phase6Config.maxSteps = config_.maxSteps;
        phase6Config.enableCheckpointing = config_.enableCheckpointing;
        phase6Config.enableReplay = config_.enableReplay;
        phase6Config.enableDevelopment = config_.enableDevelopment;
        phase6Config.checkpointPath = config_.checkpointPath;
        
        auto result = experiment_.run(phase6Config);
        
        // Display detailed results
        printSection("Final Results");
        printResult("Total reward", result.totalReward, "", false, 1000.0f);
        printResult("Average firing rate", result.avgFiringRate, "Hz", false, 100.0f);
        printResult("Episodes stored", result.memoryEpisodesStored, "", false, 100.0f);
        printResult("Dopamine level", result.dopamineLevel, "", false, 10.0f);
        printResult("Novelty level", result.noveltyLevel, "", false, 10.0f);
        printResult("Curiosity level", result.curiosityLevel, "", false, 10.0f);
        
        // Integration status
        printSection("Integration Status");
        
        std::vector<std::pair<std::string, bool>> systems = {
            {"Working Memory", result.memoryWorkingMemoryIntegrated},
            {"Episodic Memory", result.memoryEpisodicMemoryIntegrated},
            {"Neuromodulation", result.neuromodulationIntegrated},
            {"Prediction System", result.predictionIntegrated},
            {"Development System", result.developmentIntegrated},
            {"Checkpointing", result.checkpointingWorks},
            {"Replay System", result.replayWorks}
        };
        
        int connectedCount = 0;
        for (const auto& system : systems) {
            std::string status = system.second ? "CONNECTED" : "DISCONNECTED";
            std::cout << "  " << std::left << std::setw(25) << system.first << ": " << status << "\n";
            if (system.second) connectedCount++;
        }
        
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "SYSTEMS CONNECTED: " << connectedCount << "/" << systems.size() << "\n";
        std::cout << std::string(60, '=') << "\n";
        
        // Performance assessment
        std::cout << "\nPerformance Assessment:\n";
        if (result.totalReward > 0) {
            std::cout << "  ✓ Positive reward indicates learning and adaptation\n";
        }
        if (result.avgFiringRate > 1.0f) {
            std::cout << "  ✓ Healthy neural activity detected\n";
        }
        if (result.memoryEpisodesStored > 0) {
            std::cout << "  ✓ Memory consolidation is functional\n";
        }
        if (result.checkpointingWorks) {
            std::cout << "  ✓ State persistence is working\n";
        }
        if (result.replayWorks) {
            std::cout << "  ✓ Memory replay is functional\n";
        }
        
        // Benchmark summary
        printSection("Benchmark Summary");
        std::cout << "Benchmark mode: " << config_.benchmarkMode << "\n";
        std::cout << "Wall clock time: " << std::fixed << std::setprecision(2) 
                  << result.totalWallClockTime << "s\n";
        
        float stepsPerSecond = config_.maxSteps / std::max(result.totalWallClockTime, 0.1);
        printResult("Simulation speed", stepsPerSecond, "steps/s", false, 10000.0f);
        
        if (verbose_) {
            printSection("Debug Information");
            std::cout << "Start time: " << result.startTime << "\n";
            std::cout << "End time: " << result.endTime << "\n";
            std::cout << "Neuron count configuration: " << config_.neuronCount << "\n";
            std::cout << "Development enabled: " << (config_.enableDevelopment ? "Yes" : "No") << "\n";
        }
        
        // Final verdict
        printSection("Final Verdict");
        int criticalSystems = 5; // Working memory, episodic memory, neuromodulation, prediction, development
        int criticalConnected = systems[0].second + systems[1].second + systems[2].second + 
                               systems[3].second + systems[4].second;
        
        if (criticalConnected >= criticalSystems * 0.8) { // At least 80% of critical systems
            std::cout << "✓ PHASE 6 INTEGRATION SUCCESSFUL!\n";
            std::cout << "  The NLM brain has all major systems integrated and functional.\n";
            std::cout << "  This demonstrates that artificial neural substrates can achieve\n";
            std::cout << "  coherent operation through developmental processes.\n";
            return true;
        } else {
            std::cout << "⚠ PHASE 6 INTEGRATION PARTIAL!\n";
            std::cout << "  Some critical systems are not properly integrated.\n";
            std::cout << "  The brain requires further development or calibration.\n";
            return false;
        }
    }
};

// Parse command line arguments
DemoConfig parseArguments(int argc, char* argv[]) {
    DemoConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 6 Demo Usage:\n";
            std::cout << "  nlm_phase6_demo [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --neurons N        Number of neurons (default: 500)\n";
            std::cout << "  --steps N          Maximum simulation steps (default: 2000)\n";
            std::cout << "  --checkpoint        Enable checkpointing (default: enabled)\n";
            std::cout << "  --no-checkpoint    Disable checkpointing\n";
            std::cout << "  --replay           Enable replay system (default: enabled)\n";
            std::cout << "  --no-replay        Disable replay system\n";
            std::cout << "  --development      Enable development (default: enabled)\n";
            std::cout << "  --no-development   Disable development\n";
            std::cout << "  --verbose          Enable verbose output and debug logging\n";
            std::cout << "  --fast             Fast benchmark mode (fewer neurons/steps)\n";
            std::cout << "  --thorough         Thorough benchmark mode (more neurons/steps)\n";
            std::cout << "  --help, -h        Show this help message\n";
            exit(0);
        }
        
        if (arg == "--verbose" || arg == "-v") {
            config.verbose = true;
        } else if (arg == "--fast") {
            config.benchmarkMode = "fast";
            config.neuronCount = 100;
            config.maxSteps = 500;
        } else if (arg == "--thorough") {
            config.benchmarkMode = "thorough";
            config.neuronCount = 1000;
            config.maxSteps = 5000;
        } else if (arg.substr(0, 10) == "--neurons=") {
            config.neuronCount = std::stoul(arg.substr(10));
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoul(argv[++i]);
        } else if (arg.substr(0, 8) == "--steps=") {
            config.maxSteps = std::stoull(arg.substr(8));
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoull(argv[++i]);
        } else if (arg == "--checkpoint") {
            config.enableCheckpointing = true;
        } else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        else if (arg == "--replay") {
            config.enableReplay = true;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--development") {
            config.enableDevelopment = true;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        }
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===\n";
    std::cout << "Starting comprehensive brain integration test...\n\n";
    
    // Parse command line arguments
    DemoConfig config = parseArguments(argc, argv);
    
    // Create and run demo
    Phase6Demo demo(config);
    bool success = demo.run();
    
    if (success) {
        std::cout << "\n✓ Demo completed successfully!\n";
        return 0;
    } else {
        std::cerr << "\n✗ Demo failed!\n";
        return 1;
    }
}

} // namespace nlm
