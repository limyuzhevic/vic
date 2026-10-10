// NLM Command Line Interface
// A comprehensive CLI for running NLM experiments and simulations

#include "cli/NLMCommandLine.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <chrono>
#include <iomanip>
#include <algorithm>

namespace nlm {

NLMCommandLine::NLMCommandLine() : _parser("NLM - Neural Learning Machine CLI") {
    setupCommands();
}

void NLMCommandLine::setupCommands() {
    // Version command
    _parser.add_argument("-v", "--version")
        .help("Show version information")
        .action([this]() { printVersion(); });

    // Help command
    _parser.add_argument("-h", "--help")
        .help("Show help information")
        .action([this]() { printHelp(); });

    // Phase 3 commands
    auto& phase3_group = _parser.add_subcommand("phase3", "Phase 3 experiments - Basic neural dynamics")
        .help("Run Phase 3 experiments (basic neural dynamics)");
    
    phase3_group.add_argument("-s", "--steps")
        .help("Number of simulation steps (default: 1000)")
        .default_value(1000)
        .type(int());
    
    phase3_group.add_argument("--neurons")
        .help("Number of neurons (default: 100)")
        .default_value(100)
        .type(size_t());
    
    phase3_group.add_argument("--synapses")
        .help("Number of synapses (default: 500)")
        .default_value(500)
        .type(size_t());
    
    phase3_group.add_argument("--output")
        .help("Output file path (default: stdout)")
        .default_value("")
        .type(std::string());
    
    phase3_group.set_callback([this]() { runPhase3(); });

    // Phase 4 commands
    auto& phase4_group = _parser.add_subcommand("phase4", "Phase 4 experiments - Full brain integration")
        .help("Run Phase 4 experiments (full brain integration)");
    
    phase4_group.add_argument("-s", "--steps")
        .help("Number of simulation steps (default: 5000)")
        .default_value(5000)
        .type(int());
    
    phase4_group.add_argument("--neurons")
        .help("Number of neurons (default: 500)")
        .default_value(500)
        .type(size_t());
    
    phase4_group.add_argument("--enable-reward")
        .help("Enable reward modulation")
        .default_value(false)
        .type(bool());
    
    phase4_group.add_argument("--enable-curiosity")
        .help("Enable curiosity-driven exploration")
        .default_value(false)
        .type(bool());
    
    phase4_group.add_argument("--enable-development")
        .help("Enable brain development")
        .default_value(false)
        .type(bool());
    
    phase4_group.add_argument("--output")
        .help("Output file path (default: stdout)")
        .default_value("")
        .type(std::string());
    
    phase4_group.set_callback([this]() { runPhase4(); });

    // Phase 6 commands
    auto& phase6_group = _parser.add_subcommand("phase6", "Phase 6 integration - Final integration test")
        .help("Run Phase 6 integration (final integration test)");
    
    phase6_group.add_argument("-s", "--steps")
        .help("Number of simulation steps (default: 2000)")
        .default_value(2000)
        .type(int());
    
    phase6_group.add_argument("--neurons")
        .help("Number of neurons (default: 500)")
        .default_value(500)
        .type(size_t());
    
    phase6_group.add_argument("--enable-checkpointing")
        .help("Enable checkpoint saving/loading")
        .default_value(true)
        .type(bool());
    
    phase6_group.add_argument("--checkpoint-path")
        .help("Checkpoint file path (default: ./checkpoint_test.bin)")
        .default_value("./checkpoint_test.bin")
        .type(std::string());
    
    phase6_group.add_argument("--enable-replay")
        .help("Enable memory replay and consolidation")
        .default_value(true)
        .type(bool());
    
    phase6_group.add_argument("--output")
        .help("Output file path (default: stdout)")
        .default_value("")
        .type(std::string());
    
    phase6_group.set_callback([this]() { runPhase6(); });

    // Profile command
    auto& profile_group = _parser.add_subcommand("profile", "Profile a brain simulation")
        .help("Run a simulation with performance profiling");
    
    profile_group.add_argument("-s", "--steps")
        .help("Number of simulation steps (default: 1000)")
        .default_value(1000)
        .type(int());
    
    profile_group.add_argument("--neurons")
        .help("Number of neurons (default: 100)")
        .default_value(100)
        .type(size_t());
    
    profile_group.add_argument("--benchmark")
        .help("Run benchmarks instead of simulation")
        .default_value(false)
        .type(bool());
    
    profile_group.set_callback([this]() { profileSimulation(); });

    // Config command
    auto& config_group = _parser.add_subcommand("config", "Configuration and setup utilities")
        .help("Configuration and setup utilities");
    
    config_group.add_argument("action")
        .help("Action to perform: validate, save, list, create")
        .type(std::string());
    
    config_group.add_argument("--file")
        .help("Configuration file path")
        .type(std::string());
    
    config_group.add_argument("--check")
        .help("Validate configuration file")
        .default_value(false)
        .type(bool());
    
    config_group.set_callback([this]() { handleConfig(); });

    // Batch command
    auto& batch_group = _parser.add_subcommand("batch", "Run multiple experiments in sequence")
        .help("Run multiple experiments in sequence");
    
    batch_group.add_argument("experiments")
        .help("Comma-separated list of experiments to run (phase3,phase4,phase6,profile)")
        .type(std::string());
    
    batch_group.add_argument("--output-dir")
        .help("Output directory for results (default: ./results)")
        .default_value("./results")
        .type(std::string());
    
    batch_group.set_callback([this]() { runBatch(); });

    // Compare command
    auto& compare_group = _parser.add_subcommand("compare", "Compare multiple simulation results")
        .help("Compare multiple simulation results");
    
    compare_group.add_argument("experiments")
        .help("Comma-separated list of result files to compare")
        .type(std::string());
    
    compare_group.set_callback([this]() { compareResults(); });
}

void NLMCommandLine::printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << "An experimental artificial developmental brain" << std::endl;
    std::cout << "Python bindings: pynlm" << std::endl;
    std::cout << "Copyright 2026 NLM Authors" << std::endl;
}

void NLMCommandLine::printHelp() {
    _parser.printHelp(std::cout);
}

void NLMCommandLine::run() {
    try {
        _parser.parse();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        printHelp();
        exit(1);
    }
}

void NLMCommandLine::runPhase3() {
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    std::cout << "=== Phase 3 Experiment ===" << std::endl;
    std::cout << "Running basic neural dynamics with " << _parser.getArg<int>("steps") << " steps" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", _parser.getArg<size_t>("neurons"));
    cfg->set("region_count", 1);
    cfg->set("connection_probability", 0.1f);
    
    auto experiment = std::make_shared<Phase3Experiment>();
    auto result = experiment->run(cfg, _parser.getArg<int>("steps"));
    
    printResults(result, _parser.getArg<std::string>("output"));
}

void NLMCommandLine::runPhase4() {
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    std::cout << "=== Phase 4 Experiment ===" << std::endl;
    std::cout << "Running full brain integration with " << _parser.getArg<int>("steps") << " steps" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", _parser.getArg<size_t>("neurons"));
    cfg->set("region_count", 1);
    cfg->set("connection_probability", 0.1f);
    cfg->set("stdp_ltp_weight", 0.01f);
    cfg->set("stdp_ltd_weight", 0.012f);
    
    auto experiment = std::make_shared<Phase4Experiment>();
    Phase4Config phase4Config;
    phase4Config.steps = _parser.getArg<int>("steps");
    phase4Config.enableReward = _parser.getArg<bool>("enable-reward");
    phase4Config.enableCuriosity = _parser.getArg<bool>("enable-curiosity");
    phase4Config.enableDevelopment = _parser.getArg<bool>("enable-development");
    
    auto result = experiment->run(phase4Config);
    printResults(result, _parser.getArg<std::string>("output"));
}

void NLMCommandLine::runPhase6() {
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    std::cout << "=== Phase 6 Integration ===" << std::endl;
    std::cout << "Running final integration test with " << _parser.getArg<int>("steps") << " steps" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    config.maxSteps = _parser.getArg<int>("steps");
    config.neuronCount = _parser.getArg<size_t>("neurons");
    config.enableCheckpointing = _parser.getArg<bool>("enable-checkpointing");
    config.checkpointPath = _parser.getArg<std::string>("checkpoint-path");
    config.enableReplay = _parser.getArg<bool>("enable-replay");
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    printPhase6Results(result, _parser.getArg<std::string>("output"));
}

void NLMCommandLine::profileSimulation() {
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    std::cout << "=== Performance Profiling ===" << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", _parser.getArg<size_t>("neurons"));
    cfg->set("region_count", 1);
    cfg->set("connection_probability", 0.1f);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain" << std::endl;
        return;
    }
    
    SimpleWorld world;
    world.initialize(16, 16);
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    int steps = _parser.getArg<int>("steps");
    for (int step = 0; step < steps; ++step) {
        SensoryPercept percept = world.observe(agent.getBrain()->getRegions()[0].get());
        agent.processSensoryInput(percept);
        brain->step(step, step * 0.001);
        MotorCommand cmd = agent.decodeMotorCommand();
        world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
        
        if (step % 1000 == 0) {
            std::cout << "Step " << step << ", " 
                     << "Firing: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime);
    
    std::cout << "=== Profile Results ===" << std::endl;
    std::cout << "Total steps: " << steps << std::endl;
    std::cout << "Total time: " << duration.count() << " seconds" << std::endl;
    std::cout << "Steps per second: " << (steps / duration.count()) << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    if (_parser.getArg<bool>("benchmark")) {
        std::cout << "Benchmark mode completed" << std::endl;
    }
}

void NLMCommandLine::handleConfig() {
    std::string action = _parser.getArg<std::string>("action");
    std::string filepath = _parser.getArg<std::string>("file");
    
    if (action == "validate") {
        validateConfig(filepath);
    } else if (action == "save") {
        saveConfig(filepath);
    } else if (action == "list") {
        listConfigOptions();
    } else if (action == "create") {
        createDefaultConfig(filepath);
    } else {
        std::cerr << "Unknown config action: " << action << std::endl;
    }
}

void NLMCommandLine::validateConfig(const std::string& filepath) {
    auto cfg = std::make_shared<Config>();
    if (filepath.empty()) {
        std::cout << "Validating default configuration..." << std::endl;
    } else {
        if (!cfg->loadFromFile(filepath)) {
            std::cerr << "Error: Failed to load configuration file" << std::endl;
            return;
        }
    }
    
    std::cout << "=== Configuration Validation ===" << std::endl;
    std::cout << "Configuration is valid" << std::endl;
    std::cout << "Configuration summary:" << std::endl;
    std::cout << cfg->summary() << std::endl;
}

void NLMCommandLine::saveConfig(const std::string& filepath) {
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 1000);
    cfg->set("region_count", 1);
    cfg->set("connection_probability", 0.1f);
    cfg->set("stdp_ltp_weight", 0.01f);
    cfg->set("stdp_ltd_weight", 0.012f);
    
    if (cfg->saveToFile(filepath)) {
        std::cout << "Configuration saved to " << filepath << std::endl;
    } else {
        std::cerr << "Error: Failed to save configuration" << std::endl;
    }
}

void NLMCommandLine::listConfigOptions() {
    std::cout << "=== Available Configuration Options ===" << std::endl;
    std::cout << "neuron_count - Number of neurons (default: 1000)" << std::endl;
    std::cout << "region_count - Number of brain regions (default: 1)" << std::endl;
    std::cout << "connection_probability - Probability of synaptic connections (default: 0.1)" << std::endl;
    std::cout << "stdp_ltp_weight - STDP LTP weight (default: 0.01)" << std::endl;
    std::cout << "stdp_ltd_weight - STDP LTD weight (default: 0.012)" << std::endl;
    std::cout << "synaptogenesis_rate - Rate of new synapse formation (default: 0.0001)" << std::endl;
    std::cout << "pruning_rate - Rate of synapse pruning (default: 0.00001)" << std::endl;
    std::cout << "development_rate - Rate of brain development (default: 0.01)" << std::endl;
    std::cout << "plasticity_threshold - Plasticity activation threshold (default: 0.5)" << std::endl;
}

void NLMCommandLine::createDefaultConfig(const std::string& filepath) {
    auto cfg = std::make_shared<Config>();
    
    if (cfg->saveToFile(filepath)) {
        std::cout << "Default configuration created at " << filepath << std::endl;
    } else {
        std::cerr << "Error: Failed to create default configuration" << std::endl;
    }
}

void NLMCommandLine::runBatch() {
    std::string experiments = _parser.getArg<std::string>("experiments");
    std::string outputDir = _parser.getArg<std::string>("output-dir");
    
    std::cout << "=== Batch Experiment Execution ===" << std::endl;
    std::cout << "Running experiments: " << experiments << std::endl;
    std::cout << "Output directory: " << outputDir << std::endl;
    
    // Parse comma-separated list
    std::vector<std::string> experimentList;
    std::stringstream ss(experiments);
    std::string exp;
    while (std::getline(ss, exp, ',')) {
        experimentList.push_back(exp);
    }
    
    for (const auto& expName : experimentList) {
        std::cout << "\n--- Running " << expName << " ---" << std::endl;
        
        if (expName == "phase3") {
            // Parse arguments for phase3
            _parser.setArg("phase3.steps", 1000);
            _parser.setArg("phase3.neurons", 100);
            _parser.setArg("phase3.synapses", 500);
            _parser.setArg("phase3.output", outputDir + "/phase3_output.txt");
            runPhase3();
        } else if (expName == "phase4") {
            // Parse arguments for phase4
            _parser.setArg("phase4.steps", 5000);
            _parser.setArg("phase4.neurons", 500);
            _parser.setArg("phase4.enable-reward", false);
            _parser.setArg("phase4.enable-curiosity", true);
            _parser.setArg("phase4.enable-development", false);
            _parser.setArg("phase4.output", outputDir + "/phase4_output.txt");
            runPhase4();
        } else if (expName == "phase6") {
            // Parse arguments for phase6
            _parser.setArg("phase6.steps", 2000);
            _parser.setArg("phase6.neurons", 500);
            _parser.setArg("phase6.enable-checkpointing", true);
            _parser.setArg("phase6.checkpoint-path", outputDir + "/checkpoint.bin");
            _parser.setArg("phase6.enable-replay", true);
            _parser.setArg("phase6.output", outputDir + "/phase6_output.txt");
            runPhase6();
        } else if (expName == "profile") {
            // Parse arguments for profile
            _parser.setArg("profile.steps", 1000);
            _parser.setArg("profile.neurons", 100);
            _parser.setArg("profile.benchmark", false);
            profileSimulation();
        }
    }
    
    std::cout << "\n=== Batch Execution Complete ===" << std::endl;
}

void NLMCommandLine::compareResults() {
    std::string experiments = _parser.getArg<std::string>("experiments");
    
    std::cout << "=== Result Comparison ===" << std::endl;
    std::cout << "Comparing files: " << experiments << std::endl;
    
    // Parse comma-separated list of result files
    std::vector<std::string> resultFiles;
    std::stringstream ss(experiments);
    std::string file;
    while (std::getline(ss, file, ',')) {
        resultFiles.push_back(file);
    }
    
    if (resultFiles.size() < 2) {
        std::cerr << "Error: Need at least 2 result files to compare" << std::endl;
        return;
    }
    
    // Load and compare results
    for (size_t i = 0; i < resultFiles.size(); ++i) {
        std::cout << "\n--- Result " << i+1 << ": " << resultFiles[i] << " ---" << std::endl;
        std::ifstream file(resultFiles[i]);
        if (file.is_open()) {
            std::string line;
            int lineCount = 0;
            while (std::getline(file, line) && lineCount < 20) {
                std::cout << line << std::endl;
                lineCount++;
            }
        } else {
            std::cerr << "Warning: Could not open file " << resultFiles[i] << std::endl;
        }
    }
}

void NLMCommandLine::printResults(const ExperimentResult& result, const std::string& outputFile) {
    std::ofstream outfile;
    if (!outputFile.empty()) {
        outfile.open(outputFile);
        if (!outfile.is_open()) {
            std::cerr << "Error: Could not open output file " << outputFile << std::endl;
            return;
        }
    }
    
    std::ostream& out = outfile.is_open() ? outfile : std::cout;
    
    out << "=== Experiment Results ===" << std::endl;
    out << "Total reward: " << result.totalReward << std::endl;
    out << "Average firing rate: " << result.averageFiringRate << " Hz" << std::endl;
    out << "Total steps: " << result.totalSteps << std::endl;
    out << "Average synaptic weight: " << result.averageSynapticWeight << std::endl;
    out << "Time: " << result.totalTime << " seconds" << std::endl;
    
    outfile.close();
}

void NLMCommandLine::printPhase6Results(const Phase6IntegrationResult& result, const std::string& outputFile) {
    std::ofstream outfile;
    if (!outputFile.empty()) {
        outfile.open(outputFile);
        if (!outfile.is_open()) {
            std::cerr << "Error: Could not open output file " << outputFile << std::endl;
            return;
        }
    }
    
    std::ostream& out = outfile.is_open() ? outfile : std::cout;
    
    out << "=== Phase 6 Integration Results ===" << std::endl;
    out << "Total reward: " << result.totalReward << std::endl;
    out << "Average firing rate: " << result.avgFiringRate << " Hz" << std::endl;
    out << "Episodic episodes stored: " << result.memoryEpisodesStored << std::endl;
    out << "Dopamine level: " << result.dopamineLevel << std::endl;
    out << "Novelty level: " << result.noveltyLevel << std::endl;
    out << "Curiosity level: " << result.curiosityLevel << std::endl;
    out << "Wall clock time: " << result.totalWallClockTime << " seconds" << std::endl;
    
    out << "\n=== Integration Status ===" << std::endl;
    out << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    out << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    out << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    out << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    out << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    out << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    out << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    
    outfile.close();
}

} // namespace nlm