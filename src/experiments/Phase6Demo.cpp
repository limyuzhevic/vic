// Phase 6 demo configuration options
static void printUsage() {
    std::cout << "=== NLM Phase 6 Demo Configuration ===\n";
    std::cout << "Usage: nlm_phase6_demo [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --neurons N           Number of neurons (default: 500)\n";
    std::cout << "  --steps S             Maximum simulation steps (default: 2000)\n";
    std::cout << "  --regions R           Number of brain regions (default: 1)\n";
    std::cout << "  --checkpoint PATH     Checkpoint save/load path (default: ./checkpoint_test.bin)\n";
    std::cout << "  --no-checkpointing    Disable checkpointing\n";
    std::cout << "  --no-replay           Disable replay system\n";
    std::cout << "  --no-development      Disable developmental system\n";
    std::cout << "  --verbose             Enable verbose logging\n";
    std::cout << "  --help                Show this help message\n\n";
    std::cout << "Example:\n";
    std::cout << "  nlm_phase6_demo --neurons 1000 --steps 5000 --checkpoint ./demo_check.bin --verbose\n";
}

static Phase6Config parseCommandLineArgs(int argc, char* argv[]) {
    Phase6Config config;
    
    // Set default values
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./checkpoint_test.bin";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage();
            std::exit(0);
        } else if (arg == "--verbose") {
            Logger::getInstance().setLevel(Logger::Level::Info);
        } else if (arg == "--no-checkpointing") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg.find("--neurons=") == 0) {
            config.neuronCount = std::stoull(arg.substr(10));
        } else if (arg.find("--steps=") == 0) {
            config.maxSteps = std::stoull(arg.substr(8));
        } else if (arg.find("--regions=") == 0) {
            config.regionCount = std::stoull(arg.substr(10));
        } else if (arg.find("--checkpoint=") == 0) {
            config.checkpointPath = arg.substr(13);
        }
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        Phase6Config config = parseCommandLineArgs(argc, argv);
        
        std::cout << "=== NLM Phase 6 Integration Demo ===\n";
        std::cout << "Testing the integrated artificial brain...\n" << std::endl;
        
        // Initialize logging based on verbose flag
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Create experiment
        Phase6IntegratedExperiment experiment;
        
        // First, run the quick integration verification
        std::cout << "--- Integration Verification ---\n";
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!\n";
            return 1;
        }
        
        std::cout << "Integration verification passed!\n" << std::endl;
        
        // Test individual systems
        std::cout << "--- Memory Integration Test ---\n";
        experiment.testMemoryIntegration();
        std::cout << std::endl;
        
        std::cout << "--- Neuromodulation Integration Test ---\n";
        experiment.testNeuromodulationIntegration();
        std::cout << std::endl;
        
        std::cout << "--- Checkpoint Test ---\n";
        experiment.testCheckpointing();
        std::cout << std::endl;
        
        std::cout << "--- Replay Test ---\n";
        experiment.testReplay();
        std::cout << std::endl;
        
        // Run full experiment with user-specified settings
        std::cout << "--- Full Integration Experiment ---\n";
        std::cout << "Configuration: " << config.neuronCount << " neurons, " 
                  << config.maxSteps << " steps\n";
        std::cout << "Checkpointing: " << (config.enableCheckpointing ? "enabled" : "disabled") << "\n";
        std::cout << "Replay: " << (config.enableReplay ? "enabled" : "disabled") << "\n";
        std::cout << "Development: " << (config.enableDevelopment ? "enabled" : "disabled") << "\n\n";
        
        auto result = experiment.run(config);
        
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===\n";
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===\n";
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        std::cout << std::endl;
        
        if (result.memoryWorkingMemoryIntegrated && 
            result.memoryEpisodicMemoryIntegrated && 
            result.neuromodulationIntegrated && 
            result.checkpointingWorks) {
            std::cout << "=== SUCCESS: All critical systems integrated! ===\n";
            return 0;
        } else {
            std::cout << "=== WARNING: Some systems may not be properly integrated ===\n";
            return 0;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    }
}
