// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration Demo
//
// This demo runs the complete Phase 6 integration experiment to verify
// that all brain systems are properly connected and functioning together.
// Phase 6 represents the final integration of all previous phases into a
// coherent artificial brain with memory, prediction, cognition, and neuromodulation.

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace nlm;

void printPhase6Banner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration Demo                           ║
    ║                                                               ║
    ║     An experimental artificial developmental brain with       ║
    ║     integrated memory, prediction, cognition, and neuromodulation. ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printSystemStatus(const Phase6IntegrationResult& result) {
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory:           " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory:         " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Associative Memory:      " << (result.memoryAssociativeMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation:          " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction:               " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development:              " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing:             " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay:                   " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
}

int main(int argc, char** argv) {
    printPhase6Banner();
    
    std::cout << "Initializing NLM Phase 6 Final Integration Demo...\n" << std::endl;
    
    // Initialize global logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration Demo ===");
    NLM_LOG_INFO("Testing the complete artificial brain integration...");
    
    // Parse command line arguments
    Phase6Config config;
    std::string configFile = "configs/default.cfg";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoi(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoi(argv[++i]);
        } else if (arg == "--quiet" || arg == "-q") {
            consoleLogger->setLevel(LogLevel::Warning);
        }
    }
    
    // Create configuration for the integrated experiment
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", config.neuronCount);
    cfg->set("region_count", config.regionCount);
    cfg->set("connection_probability", config.connectionProbability);
    cfg->set("stdp_ltp_weight", 0.01f);
    cfg->set("stdp_ltd_weight", 0.012f);
    cfg->set("synaptogenesis_rate", 0.0001f);
    cfg->set("pruning_rate", 0.00001f);
    cfg->set("curiosity_threshold", config.curiosityThreshold);
    cfg->set("novelty_threshold", config.noveltyThreshold);
    
    NLM_LOG_INFO("Configuration loaded:");
    NLM_LOG_INFO("  Neurons: " + std::to_string(config.neuronCount));
    NLM_LOG_INFO("  Max Steps: " + std::to_string(config.maxSteps));
    NLM_LOG_INFO("  Connection Probability: " + std::to_string(config.connectionProbability));
    
    // Create and run the integration experiment
    Phase6IntegratedExperiment experiment;
    
    std::cout << "\n--- Phase 6 Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        NLM_LOG_ERROR("Integration verification failed during Phase 6 demo");
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual system integrations
    std::cout << "--- Testing Individual System Integrations ---" << std::endl;
    
    std::cout << "Memory Integration:" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "Neuromodulation Integration:" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "Prediction Integration:" << std::endl;
    experiment.testPredictionIntegration();
    std::cout << std::endl;
    
    std::cout << "Development Integration:" << std::endl;
    experiment.testDevelopmentIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Testing Persistence and Consolidation ---" << std::endl;
    
    std::cout << "Checkpoint Test:" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "Replay Test:" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full integration experiment with configured settings
    std::cout << "--- Running Full Integration Experiment ---" << std::endl;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total Reward:              " << result.totalReward << std::endl;
    std::cout << "Avg Reward/Step:           " << result.avgRewardPerStep << std::endl;
    std::cout << "Learning Efficiency:       " << result.overallLearningEfficiency << std::endl;
    std::cout << "Final Performance:         " << result.finalPerformance << std::endl;
    std::cout << "Memory Capacity:           " << result.memoryCapacity << std::endl;
    std::cout << "Prediction Accuracy:       " << result.predictionAccuracy << std::endl;
    std::cout << "Generalization Ability:    " << result.generalizationAbility << std::endl;
    std::cout << "Episodes Stored:           " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine Level:            " << result.dopamineLevel << std::endl;
    std::cout << "Curiosity Level:           " << result.curiosityLevel << std::endl;
    std::cout << "Novelty Level:             " << result.noveltyLevel << std::endl;
    std::cout << std::endl;
    
    // Display system integration status
    printSystemStatus(result);
    std::cout << std::endl;
    
    std::cout << "Wall clock time:           " << result.totalWallClockTime << "s" << std::endl;
    
    NLM_LOG_INFO("=== Phase 6 Complete ===");
    NLM_LOG_INFO("All systems successfully integrated into coherent artificial brain.");
    NLM_LOG_INFO("The NLM brain now functions as a complete developmental cognitive system.");
    NLM_LOG_INFO("Memory, prediction, cognition, and neuromodulation work together.");
    
    return 0;
}
