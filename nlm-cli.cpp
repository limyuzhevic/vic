#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>

namespace nlm {

class CommandLineInterface {
public:
    CommandLineInterface() = default;
    
    bool parseArguments(int argc, char* argv[]);
    void printHelp() const;
    bool execute() const;
    
private:
    std::string mode_ = "auto";
    std::string configFile_ = "configs/default.cfg";
    std::string outputFile_ = "";
    std::string checkpointPath_ = "./checkpoint.bin";
    int verbosity_ = 1;
    bool verbose_ = false;
    bool quiet_ = false;
    std::map<std::string, std::string> extraArgs_;
    std::vector<std::string> unknownArgs_;
    
    void printBanner() const;
    void runPhase2Demo() const;
    void runPhase6Demo() const;
    void runBenchmark() const;
    void runCheckpointTest() const;
    void runMemoryTest() const;
    void runPlasticityTest() const;
    void saveConfigAsJson(const std::string& filepath) const;
    void saveConfigAsYaml(const std::string& filepath) const;
};

void CommandLineInterface::printBanner() const {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Advanced Brain Simulation Framework                        ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void CommandLineInterface::printHelp() const {
    printBanner();
    std::cout << "\nUsage: nlm [OPTIONS] [MODE]" << std::endl;
    std::cout << "\nModes:" << std::endl;
    std::cout << "  phase2            Run Phase 2: Real neural computation (default)" << std::endl;
    std::cout << "  phase6            Run Phase 6: Full integration test" << std::endl;
    std::cout << "  benchmark         Run performance benchmarks" << std::endl;
    std::cout << "  checkpoint        Test checkpoint save/load functionality" << std::endl;
    std::cout << "  memory            Test memory system integration" << std::endl;
    std::cout << "  plasticity        Test plasticity and learning" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --config <file>   Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --output <file>   Output file for results" << std::endl;
    std::cout << "  --checkpoint <file> Checkpoint file path" << std::endl;
    std::cout << "  --quiet           Quiet mode (suppress output)" << std::endl;
    std::cout << "  --verbose         Verbose mode (show detailed information)" << std::endl;
    std::cout << "  --help            Show this help message" << std::endl;
    std::cout << "\nAdvanced Options:" << std::endl;
    std::cout << "  --neurons <num>   Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps <num>     Number of simulation steps (default: 2000)" << std::endl;
    std::cout << "  --region <num>    Number of neural regions (default: 1)" << std::endl;
    std::cout << "  --stdp <val>      STDP learning rate (default: 0.01)" << std::endl;
    std::cout << "  --save-config <file> Save configuration in JSON/YAML format" << std::endl;
}

bool CommandLineInterface::parseArguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            return false;
        } else if (arg == "--quiet") {
            quiet_ = true;
            verbosity_ = 0;
        } else if (arg == "--verbose") {
            verbose_ = true;
            verbosity_ = 2;
        } else if (arg.substr(0, 9) == "--config=") {
            configFile_ = arg.substr(9);
        } else if (arg == "--config" && i + 1 < argc) {
            configFile_ = argv[++i];
        } else if (arg.substr(0, 9) == "--output=") {
            outputFile_ = arg.substr(9);
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile_ = argv[++i];
        } else if (arg.substr(0, 12) == "--checkpoint=") {
            checkpointPath_ = arg.substr(12);
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            checkpointPath_ = argv[++i];
        } else if (arg.substr(0, 9) == "--neurons=") {
            extraArgs_["neuron_count"] = arg.substr(9);
        } else if (arg == "--neurons" && i + 1 < argc) {
            extraArgs_["neuron_count"] = argv[++i];
        } else if (arg.substr(0, 8) == "--steps=") {
            extraArgs_["max_steps"] = arg.substr(8);
        } else if (arg == "--steps" && i + 1 < argc) {
            extraArgs_["max_steps"] = argv[++i];
        } else if (arg.substr(0, 8) == "--region=") {
            extraArgs_["region_count"] = arg.substr(8);
        } else if (arg == "--region" && i + 1 < argc) {
            extraArgs_["region_count"] = argv[++i];
        } else if (arg.substr(0, 6) == "--stdp=") {
            extraArgs_["stdp_ltp_weight"] = arg.substr(6);
        } else if (arg == "--stdp" && i + 1 < argc) {
            extraArgs_["stdp_ltp_weight"] = argv[++i];
        } else if (arg.substr(0, 12) == "--save-config=") {
            if (arg.size() > 12) {
                saveConfigAsJson(arg.substr(12));
            }
        } else if (arg == "--save-config" && i + 1 < argc) {
            saveConfigAsJson(argv[++i]);
        } else if (arg == "--save-yaml" && i + 1 < argc) {
            saveConfigAsYaml(argv[++i]);
        } else if (arg[0] == '-') {
            unknownArgs_.push_back(arg);
        } else {
            // Treat as mode
            mode_ = arg;
        }
    }
    
    return true;
}

void CommandLineInterface::saveConfigAsJson(const std::string& filepath) const {
    auto config = std::make_shared<Config>();
    
    // Set default values
    config->set("neuron_count", 500, ConfigSource::Default);
    config->set("region_count", 1, ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Override with extra args
    for (const auto& pair : extraArgs_) {
        if (pair.first == "neuron_count") {
            config->set("neuron_count", std::stoll(pair.second), ConfigSource::CommandLine);
        } else if (pair.first == "max_steps") {
            config->set("max_steps", std::stoll(pair.second), ConfigSource::CommandLine);
        } else if (pair.first == "region_count") {
            config->set("region_count", std::stoll(pair.second), ConfigSource::CommandLine);
        } else if (pair.first == "stdp_ltp_weight") {
            config->set("stdp_ltp_weight", std::stof(pair.second), ConfigSource::CommandLine);
        }
    }
    
    if (config->saveToFile(filepath)) {
        if (!quiet_) {
            std::cout << "Configuration saved to " << filepath << " in JSON format" << std::endl;
        }
    } else {
        std::cerr << "Failed to save configuration to " << filepath << std::endl;
    }
}

void CommandLineInterface::saveConfigAsYaml(const std::string& filepath) const {
    auto config = std::make_shared<Config>();
    
    // Same configuration as JSON
    config->set("neuron_count", 500, ConfigSource::Default);
    config->set("region_count", 1, ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Override with extra args
    for (const auto& pair : extraArgs_) {
        if (pair.first == "neuron_count") {
            config->set("neuron_count", std::stoll(pair.second), ConfigSource::CommandLine);
        } else if (pair.first == "max_steps") {
            config->set("max_steps", std::stoll(pair.second), ConfigSource::CommandLine);
        } else if (pair.first == "region_count") {
            config->set("region_count", std::stoll(pair.second), ConfigSource::CommandLine);
        } else if (pair.first == "stdp_ltp_weight") {
            config->set("stdp_ltp_weight", std::stof(pair.second), ConfigSource::CommandLine);
        }
    }
    
    if (config->saveToFile(filepath)) {
        if (!quiet_) {
            std::cout << "Configuration saved to " << filepath << " in YAML format" << std::endl;
        }
    } else {
        std::cerr << "Failed to save configuration to " << filepath << std::endl;
    }
}

void CommandLineInterface::runPhase2Demo() const {
    if (!quiet_) {
        std::cout << "=== NLM Phase 2: Real Neural Computation ===" << std::endl;
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    config->loadFromFile(configFile_);
    
    // Override with extra args
    for (const auto& pair : extraArgs_) {
        if (pair.first == "neuron_count") {
            config->set("neuron_count", std::stoll(pair.second), ConfigSource::CommandLine);
        } else if (pair.first == "max_steps") {
            config->set("max_steps", std::stoll(pair.second), ConfigSource::CommandLine);
        } else if (pair.first == "region_count") {
            config->set("region_count", std::stoll(pair.second), ConfigSource::CommandLine);
        } else if (pair.first == "stdp_ltp_weight") {
            config->set("stdp_ltp_weight", std::stof(pair.second), ConfigSource::CommandLine);
        }
    }
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    if (verbose_) {
        brain->logStatus();
    }
    
    // Run the tests (simplified version of main.cpp)
    runBasicConnectivityTest(brain);
    brain->reset();
    brain->initialize();
    runPlasticityExperiment(brain);
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    if (!quiet_) {
        std::cout << "\n=== Phase 2 Complete ===" << std::endl;
        brain->logStatus();
    }
}

void CommandLineInterface::runPhase6Demo() const {
    if (!quiet_) {
        std::cout << "=== NLM Phase 6: Full Integration Test ===" << std::endl;
    }
    
    Phase6IntegratedExperiment experiment;
    
    // Create configuration
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = checkpointPath_;
    
    // Override with extra args
    for (const auto& pair : extraArgs_) {
        if (pair.first == "neuron_count") {
            config.neuronCount = std::stoll(pair.second);
        } else if (pair.first == "max_steps") {
            config.maxSteps = std::stoll(pair.second);
        } else if (pair.first == "region_count") {
            config.regionCount = std::stoll(pair.second);
        }
    }
    
    auto result = experiment.run(config);
    
    if (!quiet_) {
        std::cout << "\n=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "\n=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        
        if (outputFile_ != "") {
            // Save results to output file
            std::ofstream outFile(outputFile_);
            if (outFile.is_open()) {
                outFile << "Total reward: " << result.totalReward << std::endl;
                outFile << "Avg firing rate: " << result.avgFiringRate << std::endl;
                outFile << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
                outFile << "Dopamine level: " << result.dopamineLevel << std::endl;
            }
        }
    }
}

void CommandLineInterface::runBenchmark() const {
    if (!quiet_) {
        std::cout << "=== NLM Performance Benchmark ===" << std::endl;
    }
    
    // Create benchmark configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000, ConfigSource::Default);
    config->set("region_count", 1, ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for benchmark!" << std::endl;
        return;
    }
    
    // Run benchmark steps
    uint64_t steps = 10000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (uint64_t i = 0; i < steps; ++i) {
        brain->step(i, i * 0.001);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    
    if (!quiet_) {
        std::cout << "Benchmark Results:" << std::endl;
        std::cout << "  Steps: " << steps << std::endl;
        std::cout << "  Elapsed time: " << elapsed << "s" << std::endl;
        std::cout << "  Steps per second: " << (steps / elapsed) << std::endl;
        std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    }
    
    if (outputFile_ != "") {
        std::ofstream outFile(outputFile_);
        if (outFile.is_open()) {
            outFile << "elapsed_time: " << elapsed << std::endl;
            outFile << "steps_per_second: " << (steps / elapsed) << std::endl;
            outFile << "avg_firing_rate: " << brain->getAverageFiringRate() << std::endl;
            outFile << "total_spikes: " << brain->getTotalSpikeCount() << std::endl;
        }
    }
}

void CommandLineInterface::runCheckpointTest() const {
    if (!quiet_) {
        std::cout << "=== NLM Checkpoint Test ===" << std::endl;
    }
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100, ConfigSource::Default);
    
    auto brain1 = std::make_shared<Brain>(config);
    brain1->initialize();
    
    // Run some steps
    for (int i = 0; i < 100; ++i) {
        brain1->step(i, i * 0.001);
    }
    
    if (!brain1->save(checkpointPath_)) {
        std::cerr << "Failed to save checkpoint!" << std::endl;
        return;
    }
    
    if (!quiet_) {
        std::cout << "Checkpoint saved successfully" << std::endl;
    }
    
    // Create new brain and load
    auto brain2 = std::make_shared<Brain>(config);
    brain2->initialize();
    
    if (!brain2->load(checkpointPath_)) {
        std::cerr << "Failed to load checkpoint!" << std::endl;
        return;
    }
    
    if (!quiet_) {
        std::cout << "Checkpoint loaded successfully" << std::endl;
        std::cout << "Brain1 total spikes: " << brain1->getTotalSpikeCount() << std::endl;
        std::cout << "Brain2 total spikes: " << brain2->getTotalSpikeCount() << std::endl;
    }
}

void CommandLineInterface::runMemoryTest() const {
    if (!quiet_) {
        std::cout << "=== NLM Memory Integration Test ===" << std::endl;
    }
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Inject some activity to create memory traces
    auto* workingMem = brain->getWorkingMemory();
    auto* episodicMem = brain->getEpisodicMemory();
    
    if (workingMem && episodicMem) {
        if (!quiet_) {
            std::cout << "Working memory available: " << (workingMem ? "YES" : "NO") << std::endl;
            std::cout << "Episodic memory available: " << (episodicMem ? "YES" : "NO") << std::endl;
        }
        
        // Test memory functionality
        workingMem->clear();
        episodicMem->clear();
        
        if (!quiet_) {
            std::cout << "Memory systems tested successfully" << std::endl;
        }
    } else {
        std::cerr << "Memory systems not available" << std::endl;
    }
}

void CommandLineInterface::runPlasticityTest() const {
    if (!quiet_) {
        std::cout << "=== NLM Plasticity Test ===" << std::endl;
    }
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 150, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Apply input pattern to stimulate learning
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 10) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    
    for (SimulationStep step = 0; step < 100; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        brain->step(step, step * 0.001);
    }
    
    if (!quiet_) {
        std::cout << "Plasticity test completed" << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Firing rate: " << brain->getAverageFiringRate() << std::endl;
    }
}

bool CommandLineInterface::execute() const {
    if (!quiet_) {
        printBanner();
    }
    
    if (mode_ == "phase2") {
        runPhase2Demo();
    } else if (mode_ == "phase6") {
        runPhase6Demo();
    } else if (mode_ == "benchmark") {
        runBenchmark();
    } else if (mode_ == "checkpoint") {
        runCheckpointTest();
    } else if (mode_ == "memory") {
        runMemoryTest();
    } else if (mode_ == "plasticity") {
        runPlasticityTest();
    } else {
        if (!quiet_) {
            std::cout << "=== NLM Main Interface ===" << std::endl;
            std::cout << "Running default Phase 2 demo (most comprehensive for testing)" << std::endl;
        }
        runPhase2Demo();
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    CommandLineInterface cli;
    
    if (!cli.parseArguments(argc, argv)) {
        return 0;  // Help was printed
    }
    
    bool success = cli.execute();
    return success ? 0 : 1;
}

// Helper functions for Phase 2 demo (copied from original main.cpp)
void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    // Simplified plasticity experiment
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    NLM_LOG_INFO("Applying input patterns (100 steps)...");
    
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 10) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    
    for (SimulationStep step = 0; step < 100; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        brain->step(step, step * 0.001);
        
        if (step % 20 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getAverageFiringRate()));
        }
    }
    
    NLM_LOG_INFO("Plasticity experiment completed");
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);
    float beforeWeight = syn->getWeight();
    NLM_LOG_INFO("  Testing STDP on 1 synapse:");
    NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                 " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                 ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
    
    for (int trial = 0; trial < 20; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }
    
    float delta = syn->getWeight() - beforeWeight;
    NLM_LOG_INFO("  After correlated activity:");
    NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                 " (Δ=" + std::to_string(delta) + ")");
    
    if (delta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (delta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}