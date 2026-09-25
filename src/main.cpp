// Forward declarations
struct LearningExperiment;
struct BenchmarkResult;

// Phase 2 demo runner
int runPhase2Demo(size_t neuronCount, size_t steps, double simulationTime, float plasticityRate) {
    std::cout << "\n=== NLM Phase 2 Demo ===" << std::endl;
    std::cout << "Running Phase 2 with " << neuronCount << " neurons for " << steps << " steps" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", simulationTime, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    if (plasticityRate > 0.0f) {
        config->set("stdp_ltp_weight", plasticityRate, ConfigSource::Default);
        config->set("stdp_ltd_weight", plasticityRate * 0.9f, ConfigSource::Default);
    }
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    brain->logStatus();
    
    // Run all three tests
    runBasicConnectivityTest(brain);
    
    brain->reset();
    brain->initialize();
    
    runPlasticityExperiment(brain);
    
    brain->reset();
    brain->initialize();
    
    runStdpVerification(brain);
    
    brain->logStatus();
    
    std::cout << "\n=== Phase 2 Demo Complete ===" << std::endl;
    return 0;
}

// Phase 6 demo runner
int runPhase6Demo(size_t steps, size_t neuronCount) {
    std::cout << "\n=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Running Phase 6 with " << neuronCount << " neurons for " << steps << " steps" << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Run Phase 6 experiment using the existing demo
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
    
    // Run full experiment with provided settings
    Phase6Config config;
    config.neuronCount = neuronCount;
    config.maxSteps = steps;
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
    
    std::cout << "\n=== Phase 6 Demo Complete ===" << std::endl;
    return 0;
}

// Benchmark demo runner
int runBenchmarkDemo(size_t neuronCount) {
    std::cout << "\n=== NLM Performance Benchmark ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running benchmark with " << neuronCount << " neurons" << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Run benchmarks for different network sizes
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {neuronCount, 1000},
        {neuronCount * 2, 500},
        {neuronCount * 4, 200},
    };
    
    for (const auto& [neurons, steps] : benchmarks) {
        BenchmarkResult result = runBenchmark(neurons, steps);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    printBenchmarkResults(results);
    
    std::cout << "\n=== Benchmark Complete ===" << std::endl;
    return 0;
}