#pragma once

// Main implementation with comprehensive CLI support
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/CLI/CLI.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace nlm {

// Forward declarations for experiment functions
void runPhase3Experiment(const std::shared_ptr<Config>& config, bool verbose, bool progress);
void runPhase4Experiment(const std::shared_ptr<Config>& config, bool verbose, bool progress);
void runPhase5Experiment(const std::shared_ptr<Config>& config, bool verbose, bool progress);
void runPhase6Experiment(const std::shared_ptr<Config>& config, bool verbose, bool progress);
void runBenchmark(const std::shared_ptr<Config>& config);

// Banner function (declared in main.cpp)
void printBanner();

const std::string VERSION = "NLM Phase 6.1 (Integrated Artificial Brain)";

// Progress reporter
class ProgressReporter {
public:
    ProgressReporter(bool enabled, size_t total, const std::string& label = "Progress")
        : enabled_(enabled), total_(total), label_(label), start_time_(std::chrono::steady_clock::now()) {}
    
    void update(size_t current) {
        if (!enabled_) return;
        
        float percentage = static_cast<float>(current) / total_ * 100.0f;
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - start_time_);
        
        std::cout << "\r" << label_ << ": [";
        size_t bar_width = 50;
        size_t pos = bar_width * percentage / 100.0f;
        for (size_t i = 0; i < bar_width; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << std::fixed << std::setprecision(1) << percentage << "% "
                  << current << "/" << total_ << " (" << elapsed.count() << "s)";
        
        if (current >= total_) {
            std::cout << "\n" << std::endl;
        }
    }
    
    void finish() {
        if (enabled_ && total_ > 0) {
            std::cout << std::endl;
        }
    }
    
private:
    bool enabled_;
    size_t total_;
    std::string label_;
    std::chrono::steady_clock::time_point start_time_;
};

// Helper function to print experiment header
void printExperimentHeader(const std::string& phase, const std::string& description) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "NLM " << phase << ": " << description << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::endl;
}

// Helper function to print configuration
void printConfig(const std::shared_ptr<Config>& config) {
    std::cout << "Configuration:" << std::endl;
    auto keys = config->getKeys();
    for (const auto& key : keys) {
        if (key == "random_seed") {
            std::cout << "  random_seed: " << config->getOr<int64_t>(key, 0) << std::endl;
        } else if (key == "simulation_timestep") {
            std::cout << "  simulation_timestep: " << config->getOr<double>(key, 0.0) << "s" << std::endl;
        } else if (key == "neuron_count") {
            std::cout << "  neuron_count: " << config->getOr<int64_t>(key, 0) << std::endl;
        } else if (key == "region_count") {
            std::cout << "  region_count: " << config->getOr<int64_t>(key, 0) << std::endl;
        } else if (key == "connection_probability") {
            std::cout << "  connection_probability: " << config->getOr<float>(key, 0.0f) << std::endl;
        } else if (key == "stdp_ltp_weight") {
            std::cout << "  stdp_ltp_weight: " << config->getOr<float>(key, 0.0f) << std::endl;
        } else if (key == "stdp_ltd_weight") {
            std::cout << "  stdp_ltd_weight: " << config->getOr<float>(key, 0.0f) << std::endl;
        } else if (key == "max_steps") {
            std::cout << "  max_steps: " << config->getOr<int64_t>(key, 0) << std::endl;
        } else if (key == "stdp_tau") {
            std::cout << "  stdp_tau: " << config->getOr<float>(key, 0.0f) << std::endl;
        } else if (key == "synaptogenesis_rate") {
            std::cout << "  synaptogenesis_rate: " << config->getOr<float>(key, 0.0f) << std::endl;
        } else if (key == "pruning_rate") {
            std::cout << "  pruning_rate: " << config->getOr<float>(key, 0.0f) << std::endl;
        }
    }
}

// Phase 3 experiment implementation
void runPhase3Experiment(const std::shared_ptr<Config>& config, bool verbose, bool progress) {
    printExperimentHeader("Phase 3", "World Interaction Demo");
    
    // Create world
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setRandomSeed(config->getOr<int64_t>("random_seed", 42));
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Create agent brain interface
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Configure agent brain
    agentBrain.enableRewardModulation(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    agentBrain.enableCuriosity(true);
    
    std::cout << "Initial brain state:" << std::endl;
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << std::endl;
    std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << std::endl;
    std::cout << std::endl;
    
    // Run episodes
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    
    if (progress) {
        ProgressReporter reporter(true, numEpisodes, "Episodes");
        for (int episode = 0; episode < numEpisodes; ++episode) {
            // Reset world and brain
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            // Run episode
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // Get sensory percept
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // Agent processes percept and produces action
                agentBrain.process(percept);
                
                // World applies action and returns result
                const ActionResult& result = world.applyAction(agentBrain.getAction());
                
                // Agent receives reward and updates
                agentBrain.receiveReward(result.reward);
                
                // Store metrics
            }
            
            reporter.update(episode + 1);
        }
    } else {
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " ---" << std::endl;
            // Run episode...
            std::cout << "  (episode would run here)" << std::endl;
        }
    }
    
    std::cout << "\nPhase 3 complete!" << std::endl;
}

// Phase 4 experiment implementation
void runPhase4Experiment(const std::shared_ptr<Config>& config, bool verbose, bool progress) {
    printExperimentHeader("Phase 4", "Emerging Cognition");
    
    std::cout << "Phase 4 runs 8 different cognitive experiments:" << std::endl;
    std::cout << "  1. Temporal Prediction Learning" << std::endl;
    std::cout << "  2. Working Memory" << std::endl;
    std::cout << "  3. Episodic Memory" << std::endl;
    std::cout << "  4. Concept Formation" << std::endl;
    std::cout << "  5. Neural Attention" << std::endl;
    std::cout << "  6. Multi-Step Planning" << std::endl;
    std::cout << "  7. Self-Model" << std::endl;
    std::cout << "  8. Social Learning" << std::endl;
    std::cout << "  9. Continual Learning" << std::endl;
    std::cout << "  10. Integrated Phase 4" << std::endl;
    std::cout << std::endl;
    
    size_t numTrials = 50;
    if (progress) {
        ProgressReporter reporter(true, 10, "Experiments");
        for (int i = 0; i < 10; ++i) {
            reporter.update(i + 1);
            std::cout << "  Experiment " << (i + 1) << " would run here..." << std::endl;
        }
    } else {
        for (int i = 0; i < 10; ++i) {
            std::cout << "  Experiment " << (i + 1) << " would run here..." << std::endl;
        }
    }
    
    std::cout << "\nPhase 4 complete!" << std::endl;
}

// Phase 5 experiment implementation
void runPhase5Experiment(const std::shared_ptr<Config>& config, bool verbose, bool progress) {
    printExperimentHeader("Phase 5", "Personality/Brain-Sandbox Integration");
    
    std::cout << "Phase 5 performs three key experiments:" << std::endl;
    std::cout << "  1. Pattern probe: Testing with different sensory patterns" << std::endl;
    std::cout << "  2. Sandbox run: Phase-3-style closed loop in SimpleWorld" << std::endl;
    std::cout << "  3. Checkpoint round-trip: Save, reload, and continue" << std::endl;
    std::cout << std::endl;
    
    if (progress) {
        ProgressReporter reporter(true, 3, "Experiments");
        for (int i = 0; i < 3; ++i) {
            reporter.update(i + 1);
            std::cout << "  Experiment " << (i + 1) << " would run here..." << std::endl;
        }
    } else {
        for (int i = 0; i < 3; ++i) {
            std::cout << "  Experiment " << (i + 1) << " would run here..." << std::endl;
        }
    }
    
    std::cout << "\nPhase 5 complete!" << std::endl;
}

// Phase 6 experiment implementation
void runPhase6Experiment(const std::shared_ptr<Config>& config, bool verbose, bool progress) {
    printExperimentHeader("Phase 6", "Integration Test");
    
    std::cout << "Phase 6 integrated experiment tests system connectivity:" << std::endl;
    std::cout << "  - Memory systems integration" << std::endl;
    std::cout << "  - Neuromodulation integration" << std::endl;
    std::cout << "  - Prediction system integration" << std::endl;
    std::cout << "  - Development integration" << std::endl;
    std::cout << "  - Checkpointing functionality" << std::endl;
    std::cout << "  - Replay system functionality" << std::endl;
    std::cout << std::endl;
    
    Phase6IntegratedExperiment experiment;
    Phase6Config phase6Config;
    
    // Override config with experiment settings
    phase6Config.maxSteps = config->getOr<int64_t>("max_steps", 10000);
    phase6Config.neuronCount = config->getOr<int64_t>("neuron_count", 1000);
    phase6Config.regionCount = config->getOr<int64_t>("region_count", 1);
    phase6Config.connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    phase6Config.enableCheckpointing = true;
    phase6Config.enableReplay = true;
    phase6Config.enableDevelopment = true;
    
    if (progress) {
        ProgressReporter reporter(true, 100, "Experiment Steps");
        auto result = experiment.run(phase6Config);
        
        std::cout << "\n=== PHASE 6 RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Avg synaptic weight: " << result.avgSynapticWeight << std::endl;
        std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
        std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    } else {
        auto result = experiment.run(phase6Config);
        
        std::cout << "\n=== PHASE 6 RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Avg synaptic weight: " << result.avgSynapticWeight << std::endl;
        std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
        std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    }
    
    std::cout << "\nPhase 6 complete!" << std::endl;
}

// Benchmark implementation
void runBenchmark(const std::shared_ptr<Config>& config) {
    printExperimentHeader("BENCHMARK", "Performance Testing");
    
    std::cout << "Running performance benchmark..." << std::endl;
    std::cout << "Configuration:" << std::endl;
    printConfig(config);
    
    // TODO: Implement actual benchmark logic
    std::cout << "Benchmark would run here..." << std::endl;
    std::cout << "Benchmark complete!" << std::endl;
}

// Main implementation function
int mainImpl(int argc, char** argv);

} // namespace nlm
