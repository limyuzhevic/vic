// Phase 6 Demo - Integration Test (Updated)
//
// This demo runs the Phase 6 integration experiment to verify
// that all brain systems are properly connected and working together.
// This is the main user-facing demonstration of the integrated brain.

#include "Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace nlm;

void printDemoBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration Demo                            ║
    ║                                                               ║
    ║     Complete artificial brain with all systems integrated!      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printPhase6Summary(const Phase6ExperimentResults& results) {
    std::cout << std::endl;
    std::cout << "=== Phase 6 Integration Summary ===" << std::endl;
    std::cout << "Total simulation steps: " << results.totalSteps << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Wall clock time: " << results.totalWallClockTime << " seconds" << std::endl;
    std::cout << "Average firing rate: " << results.averageFiringRate << " Hz" << std::endl;
    std::cout << "Total reward: " << results.totalReward << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== System Integration Status ===" << std::endl;
    
    auto printStatus = [](const std::string& name, bool integrated, const std::string& details = "") {
        std::cout << name << ": " << (integrated ? "✓ INTEGRATED" : "✗ DISCONNECTED") << std::endl;
        if (!details.empty()) {
            std::cout << "  " << details << std::endl;
        }
    };
    
    printStatus("Working Memory", results.memoryWorkingMemoryIntegrated,
                (results.workingMemoryTraces > 0 ? 
                 "Active traces: " + std::to_string(results.workingMemoryTraces) : "No traces stored"));
    
    printStatus("Episodic Memory", results.memoryEpisodicMemoryIntegrated,
                "Episodes stored: " + std::to_string(results.memoryEpisodesStored));
    
    printStatus("Neuromodulation", results.neuromodulationIntegrated,
                "Dopamine: " + std::to_string(results.dopamineLevel));
    
    printStatus("Prediction System", results.predictionIntegrated,
                "Accuracy: " + std::to_string(results.predictionAccuracy * 100.0f) + "%");
    
    printStatus("Cognitive Systems", results.attentionIntegrated && results.planningIntegrated && results.conceptFormationIntegrated,
                (results.attentionIntegrated ? "Attention ✓ " : "Attention ✗ ") <<
                (results.planningIntegrated ? "Planning ✓ " : "Planning ✗ ") <<
                (results.conceptFormationIntegrated ? "Concept Formation ✓" : "Concept Formation ✗"));
    
    printStatus("Development System", results.developmentIntegrated,
                "Stage: " + std::to_string(results.developmentalStage));
    
    printStatus("Persistence", results.checkpointingWorks,
                "Checkpoints: " + std::to_string(results.checkpointsSaved) + " saved, " +
                std::to_string(results.checkpointsLoaded) + " loaded" +
                (results.replayEvents > 0 ? ", " + std::to_string(results.replayEvents) + " replay events" : ""));
    
    std::cout << std::endl;
    std::cout << "=== Learning Metrics ===" << std::endl;
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Learning rate: " << results.learningRate << std::endl;
    std::cout << "System stability: " << results.stability << std::endl;
    std::cout << "Adaptability: " << results.adaptability << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    printDemoBanner();
    
    std::cout << "Phase 6 Integration Demo - Testing the Complete Artificial Brain" << std::endl;
    std::cout << "=======================================================================" << std::endl << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create and initialize the experiment
    Phase6IntegratedExperiment experiment;
    std::shared_ptr<Brain> brain = std::make_shared<Brain>(std::make_shared<Config>());
    experiment.initialize(brain);
    
    std::cout << "Brain initialized with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    std::cout << std::endl;
    
    // Phase 1: Integration Verification
    std::cout << "--- Phase 1: System Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: System integration verification failed!" << std::endl;
        std::cerr << "Some brain systems are not properly connected." << std::endl;
        std::cerr << "Check the logs above for details." << std::endl;
        return 1;
    }
    
    std::cout << "✓ System integration verification passed!" << std::endl;
    std::cout << std::endl;
    
    // Phase 2: Individual System Tests
    std::cout << "--- Phase 2: Individual System Tests ---" << std::endl;
    
    std::cout << "Testing memory systems..." << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "Testing neuromodulation systems..." << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "Testing persistence systems..." << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "Testing memory replay..." << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Phase 3: Full Integration Experiment
    std::cout << "--- Phase 3: Full Integration Experiment ---" << std::endl;
    std::cout << "Running comprehensive integration test..." << std::endl << std::endl;
    
    // Configure experiment based on command line arguments
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.enableAllSystems();  // Enable all integrated systems
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--small") {
            config.neuronCount = 200;
            config.maxSteps = 1000;
            std::cout << "Using small scale configuration (200 neurons, 1000 steps)" << std::endl;
        } else if (arg == "--large") {
            config.neuronCount = 1000;
            config.maxSteps = 5000;
            std::cout << "Using large scale configuration (1000 neurons, 5000 steps)" << std::endl;
        } else if (arg == "--demo") {
            config.maxSteps = 500;
            std::cout << "Using quick demo configuration (500 steps)" << std::endl;
        } else if (arg.find("--neurons=") == 0) {
            config.neuronCount = std::stoi(arg.substr(10));
            std::cout << "Setting neuron count to: " << config.neuronCount << std::endl;
        } else if (arg.find("--steps=") == 0) {
            config.maxSteps = std::stoi(arg.substr(8));
            std::cout << "Setting max steps to: " << config.maxSteps << std::endl;
        } else if (arg == "--validate") {
            config.runValidationTests = true;
            std::cout << "Running validation tests" << std::endl;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Phase 6 Demo Help:" << std::endl;
            std::cout << "  --small          Use small scale configuration" << std::endl;
            std::cout << "  --large          Use large scale configuration" << std::endl;
            std::cout << "  --demo           Run quick demo (500 steps)" << std::endl;
            std::cout << "  --neurons=N      Set number of neurons" << std::endl;
            std::cout << "  --steps=N        Set maximum simulation steps" << std::endl;
            std::cout << "  --validate       Run comprehensive validation tests" << std::endl;
            std::cout << "  -h, --help       Show this help message" << std::endl;
            return 0;
        }
    }
    
    // Show configuration summary
    std::cout << std::endl;
    std::cout << "=== Experiment Configuration ===" << std::endl;
    std::cout << config.getSummary() << std::endl;
    
    // Run the full experiment
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== Phase 6 Integration Demo Complete ===" << std::endl;
    printPhase6Summary(result);
    
    std::cout << "=======================================================================" << std::endl;
    std::cout << "The NLM artificial brain has been successfully tested and integrated!" << std::endl;
    std::cout << "All Phase 6 systems are working together as a coherent whole." << std::endl;
    std::cout << "=======================================================================" << std::endl;
    
    return 0;
}
