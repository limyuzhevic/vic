// Phase 6 Demo Implementation

Phase6Demo::Phase6Demo() : experiment_(nullptr), brain_(nullptr), agent_(nullptr), world_(nullptr) {}

Phase6Demo::~Phase6Demo() {}

bool Phase6Demo::run(const Phase6DemoConfig& config) {
    // Initialize the demo
    if (!initialize(config)) {
        return false;
    }
    
    // Run integration verification
    if (!runIntegrationVerification()) {
        return false;
    }
    
    // Run system integration tests
    runSystemIntegrationTests();
    
    // Run full integration experiment
    Phase6Config experimentConfig;
    experimentConfig.neuronCount = config.neuronCount;
    experimentConfig.maxSteps = config.maxSteps;
    experimentConfig.enableCheckpointing = config.enableCheckpointing;
    experimentConfig.enableReplay = config.enableReplay;
    experimentConfig.enableDevelopment = config.enableDevelopment;
    experimentConfig.checkpointPath = config.checkpointPath;
    
    result_ = experiment_->run(experimentConfig);
    
    return true;
}

bool Phase6Demo::initialize(const Phase6DemoConfig& config) {
    // Create configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(config.neuronCount));
    cfg->set("region_count", static_cast<int64_t>(config.regionCount));
    cfg->set("connection_probability", config.connectionProbability);
    
    // Initialize experiment
    experiment_ = std::make_unique<Phase6IntegratedExperiment>();
    
    // Create and initialize brain
    brain_ = std::make_shared<Brain>(cfg);
    if (!brain_->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return false;
    }
    
    // Create and configure world
    world_ = std::make_unique<SimpleWorld>();
    world_->configure(config.worldWidth, config.worldHeight, 
                     config.visionWidth, config.visionHeight);
    world_->reset();
    
    // Create and initialize agent
    agent_ = std::make_unique<AgentBrain>(brain_);
    agent_->initialize(*world_);
    
    // Enable learning subsystems
    agent_->enableRewardModulation(true);
    agent_->enableStructuralPlasticity(config.enableDevelopment);
    agent_->enableDevelopment(config.enableDevelopment);
    agent_->enableCuriosity(true);
    
    return true;
}

void Phase6Demo::printBanner() const {
    std::cout << "===========================================\n";
    std::cout << "  NLM Phase 6 Integration Demo\n";
    std::cout << "===========================================\n\n";
}

void Phase6Demo::printConfiguration(const Phase6DemoConfig& config) const {
    std::cout << "Configuration:\n";
    std::cout << "  Neurons: " << config.neuronCount << "\n";
    std::cout << "  Steps: " << config.maxSteps << "\n";
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "Yes" : "No") << "\n";
    std::cout << "  Replay: " << (config.enableReplay ? "Yes" : "No") << "\n";
    std::cout << "  Development: " << (config.enableDevelopment ? "Yes" : "No") << "\n";
    std::cout << std::endl;
}

bool Phase6Demo::runIntegrationVerification() {
    std::cout << "--- Integration Verification ---" << std::endl;
    return experiment_->verifyIntegration();
}

void Phase6Demo::runSystemIntegrationTests() {
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment_->testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment_->testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment_->testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment_->testReplay();
    std::cout << std::endl;
}

void Phase6Demo::runFullIntegration(const Phase6DemoConfig& config) {
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config experimentConfig;
    experimentConfig.neuronCount = config.neuronCount;
    experimentConfig.maxSteps = config.maxSteps;
    experimentConfig.enableCheckpointing = config.enableCheckpointing;
    experimentConfig.enableReplay = config.enableReplay;
    experimentConfig.enableDevelopment = config.enableDevelopment;
    experimentConfig.checkpointPath = config.checkpointPath;
    
    result_ = experiment_->run(experimentConfig);
}

void Phase6Demo::printSummary() const {
    std::cout << "===========================================\n";
    std::cout << "          PHASE 6 DEMO SUMMARY\n";
    std::cout << "===========================================\n\n";
    
    std::cout << "Integration Results:\n";
    std::cout << "  Working Memory: " << (result_.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "  Episodic Memory: " << (result_.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "  Neuromodulation: " << (result_.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "  Prediction: " << (result_.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "  Development: " << (result_.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "  Checkpointing: " << (result_.checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n";
    std::cout << "  Replay: " << (result_.replayWorks ? "FUNCTIONAL" : "NOT FUNCTIONAL") << "\n\n";
    
    std::cout << "Performance Metrics:\n";
    std::cout << "  Total Reward: " << result_.totalReward << "\n";
    std::cout << "  Average Firing Rate: " << result_.avgFiringRate << " Hz\n";
    std::cout << "  Memory Episodes Stored: " << result_.memoryEpisodesStored << "\n";
    std::cout << "  Dopamine Level: " << result_.dopamineLevel << "\n";
    std::cout << "  Novelty Level: " << result_.noveltyLevel << "\n";
    std::cout << "  Curiosity Level: " << result_.curiosityLevel << "\n\n";
    
    std::cout << "Timing:\n";
    std::cout << "  Total Wall Clock Time: " << result_.totalWallClockTime << " seconds\n";
    
    if (success()) {
        std::cout << "\n✓ SUCCESS: All Phase 6 systems are properly integrated!\n";
    } else {
        std::cout << "\n✗ FAILURE: Some Phase 6 systems are not properly integrated.\n";
    }
}

bool Phase6Demo::success() const {
    return result_.memoryWorkingMemoryIntegrated &&
           result_.memoryEpisodicMemoryIntegrated &&
           result_.neuromodulationIntegrated &&
           result_.predictionIntegrated &&
           result_.developmentIntegrated &&
           result_.checkpointingWorks;
}