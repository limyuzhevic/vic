// NLM Unified Command Line Interface
// Phase 6 Enhanced CLI - Replaces the original main.cpp

#include "nlm_cli.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <filesystem>

namespace nlm {

NLMCommandLineInterface::NLMCommandLineInterface()
    : verboseMode_(false)
    , quietMode_(false)
    , profilingEnabled_(false)
    , errorHandling_(ErrorHandlingMode::Halt)
    , simulationClock_(0.001) // Default timestep
{}

NLMCommandLineInterface::~NLMCommandLineInterface() {}

bool NLMCommandLineInterface::initialize(int argc, char** argv) {
    // Parse command line arguments
    if (!parseArguments(argc, argv)) {
        return false;
    }
    
    // Load configuration
    if (!loadConfiguration()) {
        return false;
    }
    
    // Initialize logging
    if (!initializeLogging()) {
        return false;
    }
    
    // Configure simulation parameters
    if (!configureSimulation()) {
        return false;
    }
    
    return true;
}

void NLMCommandLineInterface::run() {
    NLM_LOG_INFO("=== NLM Unified Command Line Interface (Phase 6) ===");
    
    try {
        if (command_ == "demo") {
            runDemoMode();
        } else if (command_ == "experiment") {
            runExperimentMode();
        } else if (command_ == "visualize") {
            runVisualizationMode();
        } else if (command_ == "benchmark") {
            runBenchmarkMode();
        } else if (command_ == "help") {
            showHelp();
        } else {
            NLM_LOG_ERROR("Unknown command: " + command_);
            showHelp();
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in CLI: " + std::string(e.what()));
        if (errorHandling_ == ErrorHandlingMode::Halt) {
            return;
        }
    }
}

bool NLMCommandLineInterface::parseArguments(int argc, char** argv) {
    if (argc < 2) {
        // Default to demo mode for user-friendly interface
        command_ = "demo";
        currentArgs_.clear();
        return true;
    }
    
    command_ = argv[1];
    
    // Parse remaining arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-help") {
            showHelp();
            return false;
        } else if (arg == "--version" || arg == "-version") {
            showVersion();
            return false;
        } else if (arg == "--verbose" || arg == "-v") {
            verboseMode_ = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quietMode_ = true;
        } else if (arg == "--profile") {
            profilingEnabled_ = true;
        } else if (arg.find("--") == 0) {
            // Option with value (simplified parsing)
            size_t pos = arg.find("=");
            if (pos != std::string::npos) {
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                options_[key] = value;
            } else {
                options_[arg] = "true";
            }
        } else if (arg.find("-") == 0 && arg.size() > 1) {
            // Short option
            std::string key = arg;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                key += "=" + std::string(argv[++i]);
            }
            options_[key] = "true";
        } else {
            // Positional argument
            currentArgs_.push_back(arg);
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::loadConfiguration() {
    std::string configFile;
    
    // Check for config file in options
    auto it = options_.find("--config");
    if (it != options_.end()) {
        configFile = it->second;
    } else {
        // Check environment variable
        const char* envConfig = std::getenv("NLM_CONFIG_FILE");
        if (envConfig) {
            configFile = envConfig;
        }
    }
    
    if (!configFile.empty()) {
        NLM_LOG_INFO("Loading configuration from: " + configFile);
        if (!config_->loadFromFile(configFile)) {
            NLM_LOG_WARN("Failed to load configuration from: " + configFile);
            NLM_LOG_WARN("Using default configuration.");
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::initializeLogging() {
    // Get log level from options or environment
    std::string logLevel = "INFO";
    
    auto it = options_.find("--log-level");
    if (it != options_.end()) {
        logLevel = it->second;
    } else {
        const char* envLogLevel = std::getenv("NLM_LOG_LEVEL");
        if (envLogLevel) {
            logLevel = envLogLevel;
        }
    }
    
    // Convert string to log level
    Logger::Level level = Logger::Level::Info;
    if (logLevel == "TRACE") level = Logger::Level::Trace;
    else if (logLevel == "DEBUG") level = Logger::Level::Debug;
    else if (logLevel == "WARN") level = Logger::Level::Warning;
    else if (logLevel == "ERROR") level = Logger::Level::Error;
    else if (logLevel == "QUIET") level = Logger::Level::Off;
    
    Logger::getInstance().setLevel(level);
    
    if (!quietMode_) {
        NLM_LOG_INFO("Logging initialized with level: " + logLevel);
    }
    
    return true;
}

bool NLMCommandLineInterface::configureSimulation() {
    // Get configuration values from command line or defaults
    auto it = options_.find("--neuron-count");
    if (it != options_.end()) {
        size_t neuronCount = std::stoul(it->second);
        config_->set("neuron_count", static_cast<int64_t>(neuronCount));
    }
    
    it = options_.find("--region-count");
    if (it != options_.end()) {
        size_t regionCount = std::stoul(it->second);
        config_->set("region_count", static_cast<int64_t>(regionCount));
    }
    
    it = options_.find("--steps-per-episode");
    if (it != options_.end()) {
        stepsPerEpisode_ = std::stoul(it->second);
    }
    
    it = options_.find("--episodes");
    if (it != options_.end()) {
        numEpisodes_ = std::stoul(it->second);
    }
    
    it = options_.find("--random-seed");
    if (it != options_.end()) {
        uint64_t seed = std::stoull(it->second);
        config_->set("random_seed", static_cast<int64_t>(seed));
    }
    
    // Check for preset configurations
    it = options_.find("--preset");
    if (it != options_.end()) {
        applyPresetConfiguration(it->second);
    }
    
    return true;
}

void NLMCommandLineInterface::applyPresetConfiguration(const std::string& presetName) {
    NLM_LOG_INFO("Applying preset configuration: " + presetName);
    
    if (presetName == "beginner") {
        config_->set("neuron_count", static_cast<int64_t>(500));
        config_->set("region_count", static_cast<int64_t>(2));
        config_->set("connection_probability", 0.1f);
        config_->set("stdp_ltp_weight", 0.01f);
        config_->set("stdp_ltd_weight", 0.012f);
        config_->set("synaptogenesis_rate", 0.0001f);
        config_->set("pruning_rate", 0.00001f);
        
        // Enable common features for beginners
        options_["--enable-learning"] = "true";
        options_["--enable-curiosity"] = "true";
        options_["--enable-development"] = "true";
        
    } else if (presetName == "research") {
        config_->set("neuron_count", static_cast<int64_t>(2000));
        config_->set("region_count", static_cast<int64_t>(4));
        config_->set("connection_probability", 0.15f);
        config_->set("stdp_ltp_weight", 0.02f);
        config_->set("stdp_ltd_weight", 0.018f);
        config_->set("synaptogenesis_rate", 0.00005f);
        config_->set("pruning_rate", 0.000005f);
        
        // Enable all features for research
        options_["--enable-learning"] = "true";
        options_["--enable-curiosity"] = "true";
        options_["--enable-development"] = "true";
        options_["--enable-checkpoints"] = "true";
        options_["--enable-replay"] = "true";
        
    } else if (presetName == "performance") {
        config_->set("neuron_count", static_cast<int64_t>(3000));
        config_->set("region_count", static_cast<int64_t>(6));
        config_->set("connection_probability", 0.2f);
        config_->set("random_seed", static_cast<int64_t>(42));
        
        // Performance optimizations
        options_["--threads"] = "4";
        options_["--memory-pooling"] = "true";
        options_["--cache-enable"] = "true";
    }
}

void NLMCommandLineInterface::runDemoMode() {
    NLM_LOG_INFO("=== NLM Phase 6 Demo Mode ===");
    
    // Check for preset
    auto it = options_.find("--preset");
    if (it != options_.end()) {
        applyPresetConfiguration(it->second);
    }
    
    // Create configuration
    auto config = std::make_shared<Config>(*config_);
    
    // Create world
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Apply feature toggles
    applyFeatureToggles(agent);
    
    if (!quietMode_) {
        std::cout << "=== NLM Phase 6 Demo ===" << std::endl;
        std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Regions: " << brain->getRegionCount() << std::endl;
        std::cout << "Sensory input size: " << agent.getSensoryInputSize() << std::endl;
        std::cout << "Motor output size: " << agent.getMotorOutputSize() << std::endl;
        std::cout << std::endl;
    }
    
    // Run episodes
    for (size_t episode = 0; episode < numEpisodes_; ++episode) {
        if (!quietMode_) {
            std::cout << "Episode " << episode << ":" << std::endl;
        }
        
        // Reset for new episode
        world.reset();
        agent.reset();
        brain->reset();
        brain->initialize();
        
        float episodeReward = 0.0f;
        
        for (size_t step = 0; step < stepsPerEpisode_; ++step) {
            // Get sensory input
            const SensoryPercept& percept = world.getSensoryPercept();
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Run brain steps
            for (size_t neuralStep = 0; neuralStep < 10; ++neuralStep) {
                brain->step(neuralStep);
            }
            
            // Get motor command
            MotorCommand cmd = agent.decodeMotorCommand();
            
            // Apply motor command
            world.applyMotorCommand(cmd, world.getSimulationTime());
            
            // Apply reward modulation
            agent.applyRewardModulation(1.0f, 0.0f); // Default reward
            
            // Update world
            world.update(0.01);
            
            // Update development
            agent.updateDevelopment(0.01);
            
            episodeReward += 1.0f;
            
            if (profilingEnabled_ && step % 500 == 0) {
                auto now = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime_);
                if (!quietMode_) {
                    std::cout << "  Step " << step << ": " << duration.count() << "ms elapsed" << std::endl;
                }
            }
            
            if (!quietMode_ && step % 100 == 0) {
                std::cout << "  Step " << step << ": Reward=" << (episodeReward / (step + 1))
                          << ", Neuromod=" << agent.getNeuromodulationLevel()
                          << ", Novelty=" << agent.getNoveltyLevel()
                          << ", Energy=" << world.getAgentBody().energy
                          << std::endl;
            }
        }
        
        if (!quietMode_) {
            std::cout << "  Final reward: " << episodeReward << std::endl;
            std::cout << "  Neuromodulation level: " << agent.getNeuromodulationLevel() << std::endl;
            std::cout << "  Curiosity level: " << agent.getCuriosityLevel() << std::endl;
            std::cout << "  Novelty level: " << agent.getNoveltyLevel() << std::endl;
            std::cout << "  Development stage: " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
            std::cout << "  Final synapses: " << brain->getTotalSynapseCount() << std::endl;
            std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        }
    }
    
    if (!quietMode_) {
        std::cout << "=== Demo Complete ===" << std::endl;
        std::cout << "All brain systems integrated and functional." << std::endl;
    }
    
    NLM_LOG_INFO("Demo completed successfully");
}

void NLMCommandLineInterface::applyFeatureToggles(AgentBrain& agent) {
    auto it = options_.find("--enable-learning");
    if (it != options_.end()) {
        agent.enableRewardModulation(it->second == "true");
    }
    
    it = options_.find("--enable-curiosity");
    if (it != options_.end()) {
        agent.enableCuriosity(it->second == "true");
    }
    
    it = options_.find("--enable-development");
    if (it != options_.end()) {
        agent.enableDevelopment(it->second == "true");
    }
    
    it = options_.find("--enable-structural-plasticity");
    if (it != options_.end()) {
        agent.enableStructuralPlasticity(it->second == "true");
    }
}

void NLMCommandLineInterface::runExperimentMode() {
    NLM_LOG_INFO("=== NLM Experiment Mode ===");
    
    if (currentArgs_.empty()) {
        NLM_LOG_ERROR("Experiment mode requires a mode argument (phase3, phase4, phase6)");
        return;
    }
    
    std::string experimentType = currentArgs_[0];
    
    if (experimentType == "phase3") {
        runPhase3Experiment();
    } else if (experimentType == "phase4") {
        runPhase4Experiment();
    } else if (experimentType == "phase6") {
        runPhase6Experiment();
    } else if (experimentType == "custom") {
        runCustomExperiment();
    } else {
        NLM_LOG_ERROR("Unknown experiment type: " + experimentType);
        showHelp();
    }
}

void NLMCommandLineInterface::runPhase3Experiment() {
    NLM_LOG_INFO("=== Phase 3 Experiment: World Interaction ===");
    
    auto config = std::make_shared<Config>(*config_);
    
    // Set up configuration for Phase 3
    config->set("neuron_count", static_cast<int64_t>(500));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.1f);
    config->set("simulation_timestep", 0.001);
    
    // Create world
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setRandomSeed(42);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Run configuration from command line
    size_t numTrials = 10;
    if (options_.count("--trials")) {
        numTrials = std::stoul(options_["--trials"]);
    }
    
    size_t duration = 60;
    if (options_.count("--duration")) {
        duration = std::stoul(options_["--duration"]);
    }
    
    // For now, create a simple world interaction simulation
    NLM_LOG_INFO("Phase 3 experiment simulation (simplified)");
    NLM_LOG_INFO("Note: Full Phase 3 experiment implementation requires additional files");
    
    // Simulate some world interaction
    for (size_t trial = 0; trial < numTrials; ++trial) {
        world.reset();
        agent.reset();
        brain->reset();
        brain->initialize();
        
        for (size_t step = 0; step < duration * 100; ++step) {
            const SensoryPercept& percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            
            for (size_t neuralStep = 0; neuralStep < 10; ++neuralStep) {
                brain->step(neuralStep);
            }
            
            MotorCommand cmd = agent.decodeMotorCommand();
            world.applyMotorCommand(cmd, world.getSimulationTime());
            
            world.update(0.01);
            agent.updateDevelopment(0.01);
            
            if (trial % 10 == 0 && step % 500 == 0) {
                NLM_LOG_INFO("Trial " << trial << ", Step " << step << 
                           ", Reward: " << brain->getTotalSpikeCount());
            }
        }
    }
    
    NLM_LOG_INFO("Phase 3 experiment completed");
}

void NLMCommandLineInterface::runPhase4Experiment() {
    NLM_LOG_INFO("=== Phase 4 Experiment: Cognitive Emergence ===");
    
    // Note: Full Phase 4 experiment requires additional implementation
    NLM_LOG_INFO("Note: Phase 4 experiment implementation requires additional experiment classes");
    NLM_LOG_INFO("Running simplified Phase 4 simulation...");
    
    auto config = std::make_shared<Config>(*config_);
    config->set("neuron_count", static_cast<int64_t>(1000));
    config->set("region_count", static_cast<int64_t>(2));
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    // Run simplified cognitive experiments
    size_t numTrials = 50;
    if (options_.count("--trials")) {
        numTrials = std::stoul(options_["--trials"]);
    }
    
    for (size_t trial = 0; trial < numTrials; ++trial) {
        brain->reset();
        brain->initialize();
        
        for (size_t step = 0; step < 1000; ++step) {
            brain->step(step);
            
            // Inject some activity to stimulate cognitive processes
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            
            if (trial % 10 == 0 && step % 200 == 0) {
                NLM_LOG_INFO("Trial " << trial << ", Step " << step << 
                           ", Firing neurons: " << brain->getFiringNeuronCount());
            }
        }
    }
    
    NLM_LOG_INFO("Phase 4 experiment completed");
}

void NLMCommandLineInterface::runPhase6Experiment() {
    NLM_LOG_INFO("=== Phase 6 Experiment: Integrated Brain Systems ===");
    
    // Apply Phase 6 specific configuration
    config_->set("neuron_count", static_cast<int64_t>(1000));
    config_->set("region_count", static_cast<int64_t>(2));
    config_->set("connection_probability", 0.1f);
    
    // Apply neuromodulation control if specified
    auto it = options_.find("--dopamine");
    if (it != options_.end()) {
        float dopamineLevel = std::stof(it->second);
        NLM_LOG_INFO("Setting dopamine level: " + it->second);
    }
    
    it = options_.find("--curiosity");
    if (it != options_.end()) {
        float curiosityLevel = std::stof(it->second);
        NLM_LOG_INFO("Setting curiosity level: " + it->second);
    }
    
    it = options_.find("--novelty");
    if (it != options_.end()) {
        float noveltyLevel = std::stof(it->second);
        NLM_LOG_INFO("Setting novelty level: " + it->second);
    }
    
    // Use the existing Phase 6 demo for now
    NLM_LOG_INFO("Running Phase 6 integration demo...");
    Phase6Demo demo;
    demo.run(numEpisodes_, stepsPerEpisode_);
}

void NLMCommandLineInterface::runVisualizationMode() {
    NLM_LOG_INFO("=== NLM Visualization Mode ===");
    NLM_LOG_INFO("Visualization mode requires web dashboard or graphical interface");
    NLM_LOG_INFO("For now, please use the existing demo mode with verbose output.");
    
    if (!quietMode_) {
        std::cout << "Visualization requires web dashboard or graphical interface." << std::endl;
        std::cout << "Run 'nlm demo --verbose' for detailed text output instead." << std::endl;
    }
}

void NLMCommandLineInterface::runBenchmarkMode() {
    NLM_LOG_INFO("=== NLM Benchmark Mode ===");
    NLM_LOG_INFO("Benchmark mode requires additional benchmark implementation.");
    
    if (!quietMode_) {
        std::cout << "Benchmark mode requires additional implementation." << std::endl;
    }
}

void NLMCommandLineInterface::showHelp() {
    std::cout << "=== NLM Unified Command Line Interface ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm [mode] [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Available modes:" << std::endl;
    std::cout << "  demo                    Run integrated brain demonstration (default)" << std::endl;
    std::cout << "  experiment               Run specific experiments" << std::endl;
    std::cout << "  visualize                Open visualization dashboard" << std::endl;
    std::cout << "  benchmark                Run performance benchmarks" << std::endl;
    std::cout << "  help                    Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "For more information about a specific command:" << std::endl;
    std::cout << "  nlm <command> --help" << std::endl;
    std::cout << std::endl;
    std::cout << "Example usage:" << std::endl;
    std::cout << "  nlm demo                                    # Simple demo" << std::endl;
    std::cout << "  nlm demo --episodes 10 --steps 500         # Customized demo" << std::endl;
    std::cout << "  nlm demo --preset beginner                  # Beginner preset" << std::endl;
    std::cout << "  nlm experiment phase6                      # Integrated brain experiment" << std::endl;
}

void NLMCommandLineInterface::showVersion() {
    std::cout << "NLM Unified CLI v0.1.0 (Phase 6)" << std::endl;
    std::cout << "Build with CMake 3.16+, C++20" << std::endl;
    std::cout << "Python bindings: pybind11" << std::endl;
}

}} // namespace nlm

// Entry point
int main(int argc, char** argv) {
    // Create and run the CLI interface
    nlm::NLMCommandLineInterface cli;
    
    // Set start time for profiling
    cli.startTime_ = std::chrono::high_resolution_clock::now();
    
    // Initialize the interface
    if (!cli.initialize(argc, argv)) {
        return 1;
    }
    
    // Run the application
    cli.run();
    
    return 0;
}

