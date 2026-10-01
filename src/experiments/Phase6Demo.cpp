/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/CLI/CLI.hpp"
#include "core/CLI/CLIApplication.hpp"
#include <iostream>

using namespace nlm;

class Phase6DemoApp : public CLIApplication {
public:
    Phase6DemoApp() : CLIApplication("nlm_phase6_demo", 
        "NLM Phase 6: Final Integration Demo\n"
        "Verifies integration of all brain systems:\n"
        "  - Working Memory & Episodic Memory\n"
        "  - Neuromodulation (Dopamine, Curiosity, Novelty)\n"
        "  - Prediction Systems\n"
        "  - Development & Plasticity\n"
        "  - Checkpointing & Replay") {
        setVersion("Phase 6.0");
        registerStandardArguments();
        
        // Add Phase 6 specific arguments
        getParser().addArgument("neuron-count", "n", "Number of neurons", "500",
                              CLI::validatePositiveInt, nullptr, false, "int");
        getParser().addArgument("max-steps", "s", "Maximum simulation steps", "2000",
                              CLI::validatePositiveInt, nullptr, false, "int");
        getParser().addArgument("simulation-time", "t", "Simulation time in seconds", "60",
                              CLI::validatePositiveDouble, nullptr, false, "double");
        getParser().addArgument("experiment-mode", "m", "Experiment mode (quick, full, thorough)", "full",
                              [](const std::string& v) {
                                  return v == "quick" || v == "full" || v == "thorough";
                              }, nullptr, false, "string");
    }

    bool initialize() override {
        if (parser.hasArgument("log-level")) {
            auto levelOpt = parser.getArgument<std::string>("log-level");
            if (levelOpt) {
                logInfo("Log level set to: " + *levelOpt);
            }
        }
        
        if (parser.hasArgument("benchmark")) {
            logInfo("Benchmark mode enabled");
        }
        
        return true;
    }

    int execute() override {
        // Parse Phase 6 specific arguments
        size_t neuronCount = 500;
        size_t maxSteps = 2000;
        double simulationTime = 60.0;
        std::string experimentMode = "full";
        
        if (parser.hasArgument("neuron-count")) {
            auto neuronsOpt = parser.getArgument<size_t>("neuron-count");
            if (neuronsOpt) neuronCount = *neuronsOpt;
        }
        
        if (parser.hasArgument("max-steps")) {
            auto stepsOpt = parser.getArgument<size_t>("max-steps");
            if (stepsOpt) maxSteps = *stepsOpt;
        }
        
        if (parser.hasArgument("simulation-time")) {
            auto timeOpt = parser.getArgument<double>("simulation-time");
            if (timeOpt) simulationTime = *timeOpt;
        }
        
        if (parser.hasArgument("experiment-mode")) {
            auto modeOpt = parser.getArgument<std::string>("experiment-mode");
            if (modeOpt) experimentMode = *modeOpt;
        }
        
        logInfo("Starting Phase 6 integration demo with " + std::to_string(neuronCount) +
                " neurons, " + std::to_string(maxSteps) + " max steps, " + 
                std::to_string(simulationTime) + " simulation time, mode: " + experimentMode);
        
        runDemo(neuronCount, maxSteps, simulationTime, experimentMode);
        
        return 0;
    }

private:
    void runDemo(size_t neuronCount, size_t maxSteps, double simulationTime, 
                 const std::string& experimentMode) {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Create experiment
        Phase6IntegratedExperiment experiment;
        
        // First, run the quick integration verification (unless mode is "thorough")
        if (experimentMode != "thorough") {
            std::cout << "--- Integration Verification ---" << std::endl;
            bool integrationOK = experiment.verifyIntegration();
            
            std::cout << std::endl;
            
            if (!integrationOK) {
                std::cerr << "ERROR: Integration verification failed!" << std::endl;
                return;
            }
            
            std::cout << "Integration verification passed!" << std::endl << std::endl;
        }
        
        // Test individual systems (unless mode is "quick")
        if (experimentMode == "full" || experimentMode == "thorough") {
            std::cout << "--- Memory Integration Test ---" << std::endl;
            experiment.testMemoryIntegration();
            std::cout << std::endl;
            
            std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
            experiment.testNeuromodulationIntegration();
            std::cout << std::endl;
            
            std::cout << "--- Checkpoint Test ---" << std::endl;
            experiment.testCheckpointing();
            std::cout << std::endl;
            
            std::cout << "--- Replay Test ---" << std::endl;
            experiment.testReplay();
            std::cout << std::endl;
        }
        
        // Run full experiment
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config config;
        config.neuronCount = neuronCount;
        config.maxSteps = maxSteps;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto result = experiment.run(config);
        
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "Phase 6 Integration Complete!" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
    }
};

int main(int argc, char* argv[]) {
    Phase6DemoApp app;
    return app.run(argc, argv);
}
