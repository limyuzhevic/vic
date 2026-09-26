/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command line arguments
    Phase6Config config;
    
    if (argc > 1) {
        // First argument: number of episodes/steps
        config.maxSteps = static_cast<uint64_t>(std::stoul(argv[1]));
    }
    if (argc > 2) {
        // Second argument: neuron count
        config.neuronCount = static_cast<size_t>(std::stoul(argv[2]));
    }
    if (argc > 3) {
        // Third argument: region count
        config.regionCount = static_cast<size_t>(std::stoul(argv[3]));
    }
    if (argc > 4) {
        // Fourth argument: connection probability
        config.connectionProbability = std::stof(argv[4]);
    }
    if (argc > 5) {
        // Fifth argument: STDP LTP weight
        config.stdpLtpWeight = std::stof(argv[5]);
    }
    if (argc > 6) {
        // Sixth argument: STDP LTD weight
        config.stdpLtdWeight = std::stof(argv[6]);
    }
    if (argc > 7) {
        // Seventh argument: STDP tau
        config.stdpTau = std::stof(argv[7]);
    }
    if (argc > 8) {
        // Eighth argument: synaptogenesis rate
        config.synaptogenesisRate = std::stof(argv[8]);
    }
    if (argc > 9) {
        // Ninth argument: pruning rate
        config.pruningRate = std::stof(argv[9]);
    }
    if (argc > 10) {
        // Tenth argument: enable checkpointing (1/0)
        config.enableCheckpointing = std::stoi(argv[10]) != 0;
    }
    if (argc > 11) {
        // Eleventh argument: enable replay (1/0)
        config.enableReplay = std::stoi(argv[11]) != 0;
    }
    if (argc > 12) {
        // Twelfth argument: enable development (1/0)
        config.enableDevelopment = std::stoi(argv[12]) != 0;
    }
    if (argc > 13) {
        // Thirteenth argument: enable neuromodulation (1/0)
        config.enableNeuromodulation = std::stoi(argv[13]) != 0;
    }
    if (argc > 14) {
        // Fourteenth argument: environment width
        config.environmentWidth = std::stof(argv[14]);
    }
    if (argc > 15) {
        // Fifteenth argument: environment height
        config.environmentHeight = std::stof(argv[15]);
    }
    if (argc > 16) {
        // Sixteenth argument: world time step
        config.worldTimeStep = std::stof(argv[16]);
    }
    if (argc > 17) {
        // Seventeenth argument: neural time step
        config.neuralTimeStep = std::stof(argv[17]);
    }
    if (argc > 18) {
        // Eighteenth argument: vision width
        config.visionWidth = static_cast<size_t>(std::stoul(argv[18]));
    }
    if (argc > 19) {
        // Nineteenth argument: vision height
        config.visionHeight = static_cast<size_t>(std::stoul(argv[19]));
    }
    if (argc > 20) {
        // Twentieth argument: checkpoint path
        config.checkpointPath = argv[20];
    }
    
    // Log configuration
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Steps: " << config.maxSteps << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Regions: " << config.regionCount << std::endl;
    std::cout << "  Connection Probability: " << config.connectionProbability << std::endl;
    std::cout << "  STDP LTP Weight: " << config.stdpLtpWeight << std::endl;
    std::cout << "  STDP LTD Weight: " << config.stdpLtdWeight << std::endl;
    std::cout << "  STDP Tau: " << config.stdpTau << std::endl;
    std::cout << "  Synaptogenesis Rate: " << config.synaptogenesisRate << std::endl;
    std::cout << "  Pruning Rate: " << config.pruningRate << std::endl;
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "enabled" : "disabled") << std::endl;
    std::cout << "  Replay: " << (config.enableReplay ? "enabled" : "disabled") << std::endl;
    std::cout << "  Development: " << (config.enableDevelopment ? "enabled" : "disabled") << std::endl;
    std::cout << "  Neuromodulation: " << (config.enableNeuromodulation ? "enabled" : "disabled") << std::endl;
    std::cout << "  Environment Width: " << config.environmentWidth << std::endl;
    std::cout << "  Environment Height: " << config.environmentHeight << std::endl;
    std::cout << "  World Time Step: " << config.worldTimeStep << std::endl;
    std::cout << "  Neural Time Step: " << config.neuralTimeStep << std::endl;
    std::cout << "  Vision Width: " << config.visionWidth << std::endl;
    std::cout << "  Vision Height: " << config.visionHeight << std::endl;
    std::cout << "  Checkpoint Path: " << config.checkpointPath << std::endl;
    std::cout << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    bool memoryOK = experiment.testMemoryIntegration();
    if (!memoryOK) {
        std::cerr << "ERROR: Memory integration test failed!" << std::endl;
    }
    
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    bool neuromodOK = experiment.testNeuromodulationIntegration();
    if (!neuromodOK) {
        std::cerr << "ERROR: Neuromodulation integration test failed!" << std::endl;
    }
    
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    bool checkpointOK = experiment.testCheckpointing();
    if (!checkpointOK) {
        std::cerr << "ERROR: Checkpoint test failed!" << std::endl;
    }
    
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    bool replayOK = experiment.testReplay();
    if (!replayOK) {
        std::cerr << "ERROR: Replay test failed!" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Run full experiment with parsed configuration
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    auto result = experiment.run(config);
    
    // Check for experiment failures
    if (!result.memoryWorkingMemoryIntegrated || !result.memoryEpisodicMemoryIntegrated || 
        !result.neuromodulationIntegrated || !result.predictionIntegrated || 
        !result.developmentIntegrated) {
        std::cerr << "ERROR: Integration failed! Check logs for details." << std::endl;
    }
    
    if (!result.checkpointingWorks) {
        std::cerr << "ERROR: Checkpointing failed!" << std::endl;
    }
    
    if (!result.replayWorks) {
        std::cerr << "ERROR: Replay system failed!" << std::endl;
    }
}
