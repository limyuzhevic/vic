#include "SimpleCommandLineInterface.h"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>

namespace nlm {

SimpleCommandLineInterface::SimpleCommandLineInterface()
    : simulationSteps_(1000)
    , verboseMode_(false)
    , quietMode_(false)
    , saveCheckpoints_(false)
    , loadCheckpoint_(false) {
    // Initialize logger
    Logger::getInstance().setLevel(Logger::Level::Info);
}

SimpleCommandLineInterface::~SimpleCommandLineInterface() = default;

int SimpleCommandLineInterface::run(int argc, char* argv[]) {
    // Initialize default configuration
    config_ = std::make_shared<Config>();
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return 0;
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                configFile_ = argv[++i];
                loadConfiguration(configFile_);
            }
        } else if (arg == "--brain" || arg == "-b") {
            if (i + 1 < argc) {
                brainFile_ = argv[++i];
                loadBrainState(brainFile_);
            }
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                simulationSteps_ = std::stoi(argv[++i]);
            }
        } else if (arg == "--verbose" || arg == "-V") {
            verboseMode_ = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quietMode_ = true;
        } else if (arg == "--save-checkpoints") {
            saveCheckpoints_ = true;
        } else if (arg == "--load-checkpoint") {
            loadCheckpoint_ = true;
        } else if (arg == "--save") {
            if (i + 1 < argc) {
                outputFile_ = argv[++i];
                saveBrainState(outputFile_);
                return 0;
            }
        } else if (arg == "--integration-test") {
            runIntegrationTest();
            return 0;
        } else if (arg == "--learning-experiment") {
            runLearningExperiment();
            return 0;
        } else if (arg == "--phase3") {
            // Run Phase 3 demo
            Phase3Demo demo;
            demo.run(config_, simulationSteps_);
            return 0;
        } else if (arg == "--phase6") {
            // Run Phase 6 demo
            Phase6Demo phase6Demo;
            phase6Demo.run();
            return 0;
        } else {
            // Unknown argument
            std::cerr << "Unknown argument: " << arg << std::endl;
            printHelp();
            return 1;
        }
    }
    
    // Validate configuration
    if (!validateConfiguration()) {
        return 1;
    }
    
    // Run default simulation if no specific action requested
    return runSimulation(simulationSteps_);
}

void SimpleCommandLineInterface::printHelp() const {
    std::cout << "=== NLM Command Line Interface ===" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help                    Show this help message" << std::endl;
    std::cout << "  -v, --version                 Show version information" << std::endl;
    std::cout << "  -c, --config FILE             Load configuration from FILE" << std::endl;
    std::cout << "  -b, --brain FILE              Load brain state from FILE" << std::endl;
    std::cout << "  -s, --steps N                 Run simulation for N steps (default: 1000)" << std::endl;
    std::cout << "  -V, --verbose                 Enable verbose output" << std::endl;
    std::cout << "  -q, --quiet                   Quiet mode (minimal output)" << std::endl;
    std::cout << "  --save-checkpoints            Save checkpoints during simulation" << std::endl;
    std::cout << "  --load-checkpoint             Load checkpoint at start" << std::endl;
    std::cout << "  --save FILE                   Save brain state to FILE and exit" << std::endl;
    std::cout << "  --integration-test            Run integration test and exit" << std::endl;
    std::cout << "  --learning-experiment         Run learning experiment and exit" << std::endl;
    std::cout << "  --phase3                      Run Phase 3 integration demo" << std::endl;
    std::cout << "  --phase6                      Run Phase 6 integration demo" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --phase3                  Run Phase 3 demo" << std::endl;
    std::cout << "  nlm --config myconfig.cfg     Load configuration" << std::endl;
    std::cout << "  nlm --steps 5000              Run longer simulation" << std::endl;
    std::cout << "  nlm --save brain.bin          Save current brain state" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
}

void SimpleCommandLineInterface::printVersion() const {
    std::cout << "NLM (Neural Learning Machine) " << "0.1.0" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << "C++20 implementation with Python bindings" << std::endl;
}

bool SimpleCommandLineInterface::loadConfiguration(const std::string& configPath) {
    std::cout << "Loading configuration from: " << configPath << std::endl;
    if (config_->loadFromFile(configPath)) {
        std::cout << "Configuration loaded successfully" << std::endl;
        return true;
    } else {
        std::cerr << "Failed to load configuration from: " << configPath << std::endl;
        return false;
    }
}

bool SimpleCommandLineInterface::saveConfiguration(const std::string& configPath) const {
    std::cout << "Saving configuration to: " << configPath << std::endl;
    if (config_->saveToFile(configPath)) {
        std::cout << "Configuration saved successfully" << std::endl;
        return true;
    } else {
        std::cerr << "Failed to save configuration to: " << configPath << std::endl;
        return false;
    }
}

bool SimpleCommandLineInterface::runSimulation(int steps) {
    std::cout << "=== NLM Simulation ===" << std::endl;
    std::cout << "Steps: " << steps << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config_);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return false;
    }
    
    // Load checkpoint if requested
    if (loadCheckpoint_) {
        std::cout << "Loading checkpoint..." << std::endl;
        if (!brain->load("checkpoint.bin")) {
            std::cerr << "Failed to load checkpoint" << std::endl;
            return false;
        }
    }
    
    // Run simulation
    for (int step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        // Save checkpoint if requested
        if (saveCheckpoints_ && step % 1000 == 0) {
            std::string checkpointFile = "checkpoint_" + std::to_string(step) + ".bin";
            if (brain->save(checkpointFile)) {
                std::cout << "Checkpoint saved: " << checkpointFile << std::endl;
            }
        }
        
        // Print progress every 1000 steps
        if (!quietMode_ && step % 1000 == 0) {
            std::cout << "Step " << step << ": "
                      << "Firing: " << brain->getFiringNeuronCount()
                      << " | Spikes: " << brain->getTotalSpikeCount()
                      << " | Avg rate: " << brain->getAverageFiringRate() << std::endl;
        }
    }
    
    // Save final checkpoint
    if (saveCheckpoints_) {
        if (brain->save("checkpoint_final.bin")) {
            std::cout << "Final checkpoint saved: checkpoint_final.bin" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Print results
    printSimulationResults(steps);
    
    std::cout << "=== Simulation Complete ===" << std::endl;
    std::cout << "Duration: " << duration.count() << " ms" << std::endl;
    
    // Keep brain in memory for potential saving
    currentBrain_ = brain;
    
    return true;
}

bool SimpleCommandLineInterface::saveBrainState(const std::string& filepath) const {
    if (!currentBrain_) {
        std::cerr << "No brain loaded" << std::endl;
        return false;
    }
    
    std::cout << "Saving brain state to: " << filepath << std::endl;
    if (currentBrain_->save(filepath)) {
        std::cout << "Brain state saved successfully" << std::endl;
        return true;
    } else {
        std::cerr << "Failed to save brain state" << std::endl;
        return false;
    }
}

bool SimpleCommandLineInterface::loadBrainState(const std::string& filepath) {
    // Initialize a new brain if none exists
    if (!currentBrain_) {
        currentBrain_ = std::make_shared<Brain>(config_);
        currentBrain_->initialize();
    }
    
    std::cout << "Loading brain state from: " << filepath << std::endl;
    if (currentBrain_->load(filepath)) {
        std::cout << "Brain state loaded successfully" << std::endl;
        return true;
    } else {
        std::cerr << "Failed to load brain state" << std::endl;
        return false;
    }
}

bool SimpleCommandLineInterface::runIntegrationTest() {
    std::cout << "=== Running Integration Test ===" << std::endl;
    
    // Create and run Phase 6 integration test
    Phase6IntegratedExperiment experiment;
    bool result = experiment.verifyIntegration();
    
    if (result) {
        std::cout << "=== Integration Test PASSED ===" << std::endl;
        experiment.testMemoryIntegration();
        experiment.testNeuromodulationIntegration();
        experiment.testCheckpointing();
        experiment.testReplay();
    } else {
        std::cout << "=== Integration Test FAILED ===" << std::endl;
    }
    
    return result;
}

bool SimpleCommandLineInterface::runLearningExperiment() {
    std::cout << "=== Running Learning Experiment ===" << std::endl;
    
    // Create brain with learning-enabled configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 500);
    cfg->set("plasticity.stdp.enable", true);
    cfg->set("plasticity.hebbian.enable", true);
    cfg->set("neuromod.dopamine.scale", 1.0f);
    cfg->set("neuromod.curiosity.enable", true);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain for learning experiment" << std::endl;
        return false;
    }
    
    // Apply learning over many steps
    int learningSteps = 2000;
    for (int step = 0; step < learningSteps; ++step) {
        // Inject random sensory input to stimulate learning
        int numSensory = std::min(20, static_cast<int>(brain->getTotalNeuronCount()) / 25);
        for (int i = 0; i < numSensory; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (!quietMode_ && step % 500 == 0) {
            std::cout << "Learning step " << step << ": "
                      << "Firing: " << brain->getFiringNeuronCount()
                      << " | Weight change: N/A" << std::endl;
        }
    }
    
    std::cout << "=== Learning Experiment Complete ===" << std::endl;
    std::cout << "Final stats:" << std::endl;
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "  E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    
    return true;
}

void SimpleCommandLineInterface::printSimulationResults(int steps) const {
    if (!currentBrain_) {
        return;
    }
    
    std::cout << "=== Simulation Results ===" << std::endl;
    std::cout << "Steps completed: " << steps << std::endl;
    std::cout << "Total neurons: " << currentBrain_->getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << currentBrain_->getTotalSynapseCount() << std::endl;
    std::cout << "Active neurons: " << currentBrain_->getActiveNeuronCount() << std::endl;
    std::cout << "Firing neurons (this step): " << currentBrain_->getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << currentBrain_->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << currentBrain_->getAverageFiringRate() << std::endl;
    std::cout << "Excitatory/Inhibitory ratio: " << currentBrain_->getExcitationInhibitionRatio() << std::endl;
    
    // Print memory system status if available
    auto workingMemory = currentBrain_->getWorkingMemory();
    if (workingMemory) {
        std::cout << "Working memory traces: " << workingMemory->getActiveTraces() << std::endl;
    }
    
    auto episodicMemory = currentBrain_->getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "Episodic memory episodes: " << episodicMemory->getEpisodeCount() << std::endl;
    }
}

bool SimpleCommandLineInterface::validateConfiguration() const {
    if (!config_) {
        std::cerr << "No configuration loaded" << std::endl;
        return false;
    }
    
    // Check for required configuration keys
    if (!config_->has("neuron_count")) {
        std::cerr << "Configuration missing 'neuron_count'" << std::endl;
        return false;
    }
    
    if (!config_->has("region_count")) {
        std::cerr << "Configuration missing 'region_count'" << std::endl;
        return false;
    }
    
    if (!config_->has("connection_probability")) {
        std::cerr << "Configuration missing 'connection_probability'" << std::endl;
        return false;
    }
    
    // Validate values
    int64_t neuronCount = config_->getOr<int64_t>("neuron_count", 1000);
    if (neuronCount < 100) {
        std::cerr << "Warning: neuron_count is very low (" << neuronCount << ")" << std::endl;
    }
    
    if (simulationSteps_ < 1) {
        std::cerr << "Invalid simulation steps: " << simulationSteps_ << std::endl;
        return false;
    }
    
    return true;
}

} // namespace nlm
