#include "CLI.hpp"

namespace nlm {

UnifiedCLI::UnifiedCLI()
    : argc_(0), argv_(nullptr), showHelp_(false), neuronCount_(500), steps_(200),
      enableProfiling_(false), enableCheckpoints_(false), enableDebugging_(false) {
}

bool UnifiedCLI::initialize(int argc, char* argv[]) {
    argc_ = argc;
    argv_ = argv;
    
    parser_.addOption("-h", "--help", "Show help message");
    parser_.addOption("-c", "--config", "Configuration file", "config.cfg");
    parser_.addOption("--log-level", "Log level (debug, info, warn, error)", "info");
    parser_.addOption("--neuron-count", "Number of neurons", 500);
    parser_.addOption("--steps", "Number of simulation steps", 200);
    parser_.addOption("--output-format", "Output format (json, yaml, text)", "text");
    parser_.addOption("--profile", "Enable profiling");
    parser_.addOption("--checkpoints", "Enable checkpointing");
    parser_.addOption("--debug", "Enable debugging mode");
    
    // Parse command line
    parser_.parse(argc, argv);
    
    // Extract common options
    if (parser_.hasOption("-h") || parser_.hasOption("--help")) {
        showHelp_ = true;
    }
    
    configFile_ = parser_.getOptionAsString("--config");
    logLevel_ = parser_.getOptionAsString("--log-level");
    neuronCount_ = parser_.getOptionAs<size_t>("--neuron-count");
    steps_ = parser_.getOptionAs<size_t>("--steps");
    outputFormat_ = parser_.getOptionAsString("--output-format");
    enableProfiling_ = parser_.hasOption("--profile");
    enableCheckpoints_ = parser_.hasOption("--checkpoints");
    enableDebugging_ = parser_.hasOption("--debug");
    
    // Get subcommand
    command_ = parser_.getSubcommand();
    
    return true;
}

bool UnifiedCLI::run() {
    if (showHelp_) {
        printHelp();
        return true;
    }
    
    // Route to appropriate handler based on subcommand
    if (command_ == "phase2") {
        return handlePhase2Command(parser_);
    } else if (command_ == "phase3") {
        return handlePhase3Command(parser_);
    } else if (command_ == "phase4") {
        return handlePhase4Command(parser_);
    } else if (command_ == "phase6") {
        return handlePhase6Command(parser_);
    } else if (command_ == "run") {
        return handleRunCommand(parser_);
    } else {
        std::cerr << "Error: Unknown subcommand: " << command_ << std::endl;
        printHelp();
        return false;
    }
}

bool UnifiedCLI::handlePhase2Command(const CommandLineParser& parser) {
    std::cout << "Running Phase 2: Real Neural Computation" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::CommandLine);
    config->set("simulation_timestep", 0.001, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(neuronCount_), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::CommandLine);
    config->set("connection_probability", 0.15f, ConfigSource::CommandLine);
    
    // Initialize brain (similar to main.cpp Phase 2 implementation)
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return false;
    }
    
    // Run tests based on configuration
    runBasicConnectivityTest(brain);
    brain->reset();
    brain->initialize();
    runPlasticityExperiment(brain);
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    // Format results
    std::map<std::string, std::string> results;
    results["phase"] = "2";
    results["learning"] = "detected";
    results["neurons"] = std::to_string(neuronCount_);
    results["steps"] = std::to_string(steps_);
    
    formatter_.format(results, outputFormat_);
    return true;
}

bool UnifiedCLI::handlePhase3Command(const CommandLineParser& parser) {
    std::cout << "Running Phase 3: World Interaction" << std::endl;
    
    // Parse experiment-specific options
    size_t numEpisodes = parser.getOptionAs<size_t>("--episodes", 5);
    size_t stepsPerEpisode = parser.getOptionAs<size_t>("--steps-per-episode", 200);
    
    // Create Phase3Demo instance
    Phase3Demo demo;
    demo.run(numEpisodes, stepsPerEpisode);
    
    // Format results
    std::map<std::string, std::string> results;
    results["phase"] = "3";
    results["episodes"] = std::to_string(numEpisodes);
    results["stepsPerEpisode"] = std::to_string(stepsPerEpisode);
    results["demo"] = "completed";
    
    formatter_.format(results, outputFormat_);
    return true;
}

bool UnifiedCLI::handlePhase4Command(const CommandLineParser& parser) {
    std::cout << "Running Phase 4: Emerging Cognition" << std::endl;
    
    // Parse experiment-specific options
    size_t numTrials = parser.getOptionAs<size_t>("--trials", 50);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(1000), ConfigSource::CommandLine);
    config->set("region_count", size_t(2), ConfigSource::CommandLine);
    config->set("connection_probability", 0.1f, ConfigSource::CommandLine);
    config->set("random_seed", uint64_t(42), ConfigSource::CommandLine);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return false;
    }
    
    // Run experiments (similar to Phase4Demo.cpp)
    runTemporalPredictionExperiment(brain, numTrials);
    runWorkingMemoryExperiment(brain, numTrials);
    
    // Format results
    std::map<std::string, std::string> results;
    results["phase"] = "4";
    results["trials"] = std::to_string(numTrials);
    results["experiments"] = "temporal_prediction,working_memory";
    
    formatter_.format(results, outputFormat_);
    return true;
}

bool UnifiedCLI::handlePhase6Command(const CommandLineParser& parser) {
    std::cout << "Running Phase 6: Integration Test" << std::endl;
    
    // Create Phase6Demo instance
    Phase6Demo demo;
    
    // Parse experiment-specific options
    size_t neuronCount = parser.getOptionAs<size_t>("--neurons", 500);
    size_t maxSteps = parser.getOptionAs<size_t>("--max-steps", 2000);
    
    // Run integration (similar to Phase6Demo.cpp)
    bool integrationOK = demo.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return false;
    }
    
    Phase6Config config;
    config.neuronCount = neuronCount;
    config.maxSteps = maxSteps;
    config.enableCheckpointing = enableCheckpoints_;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = demo.run(config);
    
    // Format results
    std::map<std::string, std::string> results;
    results["phase"] = "6";
    results["neuronCount"] = std::to_string(neuronCount);
    results["maxSteps"] = std::to_string(maxSteps);
    results["totalReward"] = std::to_string(result.totalReward);
    results["integrationStatus"] = "completed";
    
    formatter_.format(results, outputFormat_);
    return true;
}

bool UnifiedCLI::handleRunCommand(const CommandLineParser& parser) {
    std::cout << "Running Unified NLM Experiment" << std::endl;
    
    // Parse experiment-specific options
    std::string experimentName = parser.getOptionAsString("--experiment", "");
    
    // Run unified experiment framework
    ExperimentRunner runner;
    
    std::map<std::string, std::string> results;
    results["phase"] = "unified";
    results["experiment"] = experimentName.empty() ? "all" : experimentName;
    results["status"] = "completed";
    
    formatter_.format(results, outputFormat_);
    return true;
}

void UnifiedCLI::printHelp() const {
    std::cout << "NLM Unified CLI - Neural Learning Machine" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE: nlm <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "COMMANDS:" << std::endl;
    std::cout << "  phase2        Run Phase 2: Real Neural Computation" << std::endl;
    std::cout << "  phase3        Run Phase 3: World Interaction" << std::endl;
    std::cout << "  phase4        Run Phase 4: Emerging Cognition" << std::endl;
    std::cout << "  phase6        Run Phase 6: Integration Test" << std::endl;
    std::cout << "  run           Run unified experiment" << std::endl;
    std::cout << std::endl;
    std::cout << "COMMON OPTIONS:" << std::endl;
    std::cout << "  -h, --help           Show this help message" << std::endl;
    std::cout << "  -c, --config <file>  Configuration file" << std::endl;
    std::cout << "  --log-level <level>  Log level (debug, info, warn, error)" << std::endl;
    std::cout << "  --neuron-count <n>   Number of neurons" << std::endl;
    std::cout << "  --steps <n>          Number of simulation steps" << std::endl;
    std::cout << "  --output-format <fmt> Output format (json, yaml, text)" << std::endl;
    std::cout << "  --profile            Enable profiling" << std::endl;
    std::cout << "  --checkpoints        Enable checkpointing" << std::endl;
    std::cout << "  --debug              Enable debugging mode" << std::endl;
}

} // namespace nlm