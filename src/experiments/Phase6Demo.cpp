/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6Demo.h"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

Phase6Demo::Phase6Demo()
    : checkpointPath_("/tmp/nlm_phase6_demo_checkpoint.bin")
    , neuronCount_(500)
    , maxSteps_(2000)
    , enableCheckpointing_(true)
    , enableReplay_(true)
    , enableDevelopment_(true)
    , initialized_(false)
    , success_(false) {
    setupConfiguration();
}

Phase6Demo::~Phase6Demo() {
    cleanup();
}

std::unique_ptr<Phase6Demo> Phase6Demo::createInstance(int argc, char* argv[]) {
    auto demo = std::make_unique<Phase6Demo>();
    
    if (!demo->initialize(argc, argv)) {
        return nullptr;
    }
    
    return demo;
}

bool Phase6Demo::initialize(int argc, char* argv[]) {
    NLM_LOG_INFO("Initializing NLM Phase 6 Demo...");
    
    // Parse command line arguments
    if (!parseCommandLine(argc, argv)) {
        NLM_LOG_ERROR("Failed to parse command line arguments");
        return false;
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    initialized_ = true;
    NLM_LOG_INFO("NLM Phase 6 Demo initialized successfully");
    
    return true;
}

int Phase6Demo::run() {
    if (!initialized_) {
        NLM_LOG_ERROR("Demo not initialized. Call initialize() first.");
        return 1;
    }
    
    printBanner();
    
    try {
        // First, run the quick integration verification
        NLM_LOG_INFO("--- Integration Verification ---");
        bool integrationOK = verifyIntegrationOnly();
        
        if (!integrationOK) {
            NLM_LOG_ERROR("ERROR: Integration verification failed!");
            return 1;
        }
        
        NLM_LOG_INFO("Integration verification passed!");
        
        // Test individual systems
        NLM_LOG_INFO("--- Memory Integration Test ---");
        runMemoryIntegrationTest();
        
        NLM_LOG_INFO("--- Neuromodulation Integration Test ---");
        runNeuromodulationIntegrationTest();
        
        NLM_LOG_INFO("--- Checkpoint Test ---");
        runCheckpointingTest();
        
        NLM_LOG_INFO("--- Replay Test ---");
        runReplayTest();
        
        // Run full experiment
        NLM_LOG_INFO("--- Full Integration Experiment ---");
        Phase6Config config;
        config.neuronCount = neuronCount_;
        config.maxSteps = maxSteps_;
        config.enableCheckpointing = enableCheckpointing_;
        config.enableReplay = enableReplay_;
        config.enableDevelopment = enableDevelopment_;
        config.checkpointPath = checkpointPath_;
        
        results_ = experiment_.run(config);
        
        success_ = results_.memoryWorkingMemoryIntegrated &&
                  results_.memoryEpisodicMemoryIntegrated &&
                  results_.neuromodulationIntegrated &&
                  results_.predictionIntegrated &&
                  results_.developmentIntegrated;
        
        printResults();
        
        NLM_LOG_INFO("=== Phase 6 Demo Complete ===");
        NLM_LOG_INFO("Success: " + std::string(success_ ? "YES" : "NO"));
        
        return success_ ? 0 : 1;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Demo failed with exception: ") + e.what());
        return 1;
    }
}

bool Phase6Demo::verifyIntegrationOnly() {
    return experiment_.verifyIntegration();
}

bool Phase6Demo::runMemoryIntegrationTest() {
    return experiment_.testMemoryIntegration();
}

bool Phase6Demo::runNeuromodulationIntegrationTest() {
    return experiment_.testNeuromodulationIntegration();
}

bool Phase6Demo::runCheckpointingTest() {
    return experiment_.testCheckpointing();
}

bool Phase6Demo::runReplayTest() {
    return experiment_.testReplay();
}

void Phase6Demo::printBanner() const {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << neuronCount_ << std::endl;
    std::cout << "  Steps: " << maxSteps_ << std::endl;
    std::cout << "  Checkpointing: " << (enableCheckpointing_ ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Replay: " << (enableReplay_ ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Development: " << (enableDevelopment_ ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Checkpoint Path: " << checkpointPath_ << std::endl;
    std::cout << std::endl;
}

void Phase6Demo::printResults() const {
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << std::fixed << std::setprecision(4) << results_.totalReward << std::endl;
    std::cout << "Avg firing rate: " << std::fixed << std::setprecision(4) << results_.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << results_.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << std::fixed << std::setprecision(4) << results_.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (results_.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (results_.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (results_.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (results_.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (results_.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (results_.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << std::fixed << std::setprecision(2) << results_.totalWallClockTime << "s" << std::endl;
}

bool Phase6Demo::parseCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--neurons" && i + 1 < argc) {
            neuronCount_ = std::stoul(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            maxSteps_ = std::stoul(argv[++i]);
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            checkpointPath_ = argv[++i];
        } else if (arg == "--disable-checkpointing") {
            enableCheckpointing_ = false;
        } else if (arg == "--disable-replay") {
            enableReplay_ = false;
        } else if (arg == "--disable-development") {
            enableDevelopment_ = false;
        } else if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;
        }
    }
    
    return true;
}

void Phase6Demo::printHelp() const {
    std::cout << "NLM Phase 6 Demo - Command Line Options:" << std::endl;
    std::cout << "  --neurons N    Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps S       Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --checkpoint F  Checkpoint file path (default: /tmp/nlm_phase6_demo_checkpoint.bin)" << std::endl;
    std::cout << "  --disable-checkpointing  Disable checkpointing" << std::endl;
    std::cout << "  --disable-replay      Disable replay system" << std::endl;
    std::cout << "  --disable-development Disable development" << std::endl;
    std::cout << "  --help, -h          Show this help message" << std::endl;
    std::cout << std::endl;
}

void Phase6Demo::setupConfiguration() {
    // Apply configuration defaults from system config
    auto& logger = Logger::getInstance();
    
    // Set up logging based on environment
    #ifdef NDEBUG
    logger.setLevel(Logger::Level::Info);
    #else
    logger.setLevel(Logger::Level::Debug);
    #endif
}

void Phase6Demo::cleanup() {
    if (initialized_) {
        NLM_LOG_INFO("Cleaning up Phase 6 Demo...");
        initialized_ = false;
        success_ = false;
    }
}

} // namespace nlm
