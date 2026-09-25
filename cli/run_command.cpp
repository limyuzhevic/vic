// NLM Run Command - Execute brain simulations
// Handles running various types of brain simulations

#pragma once

#include "cli/nlm_cli.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "environment/Environment.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <thread>

namespace nlm {

class RunCommand {
public:
    static bool execute(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void showHelp();
    static bool runBasicSimulation(std::shared_ptr<Brain> brain, size_t steps);
    static bool runPlasticitySimulation(std::shared_ptr<Brain> brain, size_t steps);
    static bool runStdpSimulation(std::shared_ptr<Brain> brain, size_t steps);
    static bool runLearningSimulation(std::shared_ptr<Brain> brain, size_t iterations);
    static bool runConnectivitySimulation(std::shared_ptr<Brain> brain, size_t steps);
    static bool runExperimentScenario(std::shared_ptr<Brain> brain, const std::string& scenario, size_t steps);
};

bool RunCommand::execute(NLMCLI& cli, const std::map<std::string, std::string>& args) {
    std::cout << "=== NLM Run Command ===" << std::endl;
    
    // Determine which simulation to run
    std::string experiment = args.count("experiment") ? args.at("experiment") : "basic";
    size_t steps = args.count("steps") ? std::stoul(args.at("steps")) : 1000;
    size_t neurons = args.count("neurons") ? std::stoul(args.at("neurons")) : 1000;
    double duration = args.count("duration") ? std::stod(args.at("duration")) : 1.0;
    
    std::cout << "Experiment: " << experiment << std::endl;
    std::cout << "Steps: " << steps << std::endl;
    std::cout << "Duration: " << duration << "s" << std::endl;
    
    // Load configuration
    auto config = cli.loadConfiguration(cli.getConfigFile());
    if (!config) {
        std::cerr << "Error: Failed to load configuration" << std::endl;
        return false;
    }
    
    // Override configuration with command line args
    if (args.count("neuron-count")) {
        config->set("neuron_count", std::stoll(args.at("neuron-count")), ConfigSource::CommandLine);
    }
    if (args.count("region-count")) {
        config->set("region_count", std::stoll(args.at("region-count")), ConfigSource::CommandLine);
    }
    if (args.count("connection-prob")) {
        config->set("connection_probability", std::stod(args.at("connection-prob")), ConfigSource::CommandLine);
    }
    if (args.count("timestep")) {
        config->set("simulation_timestep", std::stod(args.at("timestep")), ConfigSource::CommandLine);
    }
    if (args.count("random-seed")) {
        config->set("random_seed", std::stoll(args.at("random-seed")), ConfigSource::CommandLine);
    }
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain" << std::endl;
        return false;
    }
    
    std::cout << "Brain initialized successfully." << std::endl;
    std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Regions: " << brain->getRegionCount() << std::endl;
    std::cout << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
    
    // Run the selected experiment
    bool success = true;
    if (experiment == "basic" || experiment == "connectivity") {
        success = runBasicSimulation(brain, steps);
    } else if (experiment == "plasticity") {
        success = runPlasticitySimulation(brain, steps);
    } else if (experiment == "stdp") {
        success = runStdpSimulation(brain, steps);
    } else if (experiment == "learning") {
        success = runLearningSimulation(brain, steps);
    } else {
        std::cerr << "Error: Unknown experiment type: " << experiment << std::endl;
        return false;
    }
    
    // Save brain state if requested
    if (args.count("save-file")) {
        std::string saveFile = args.at("save-file");
        cli.saveBrainState(saveFile, brain);
        std::cout << "Brain state saved to: " << saveFile << std::endl;
    }
    
    if (success) {
        std::cout << "Experiment completed successfully!" << std::endl;
    } else {
        std::cerr << "Experiment failed." << std::endl;
    }
    
    return success;
}

void RunCommand::showHelp() {
    std::cout << "=== NLM Run Command Help ===" << std::endl;
    std::cout << "Usage: nlm run [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Run brain simulations with various experiment types:" << std::endl;
    std::cout << "  -e, --experiment NAME      Experiment type (basic, plasticity, stdp, learning, connectivity)" << std::endl;
    std::cout << "  -s, --steps N              Number of simulation steps" << std::endl;
    std::cout << "  --duration SECONDS         Simulation duration in seconds" << std::endl;
    std::cout << "  --neuron-count N           Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --region-count N           Number of brain regions (default: 2)" << std::endl;
    std::cout << "  --connection-prob P        Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --timestep SECONDS         Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --random-seed SEED          Random seed for reproducibility" << std::endl;
    std::cout << "  -c, --config FILE          Load configuration from FILE" << std::endl;
    std::cout << "  -s, --save FILE            Save brain state to FILE" << std::endl;
    std::cout << "  -h, --help                 Show this help message" << std::endl;
}

bool RunCommand::runBasicSimulation(std::shared_ptr<Brain> brain, size_t steps) {
    std::cout << "Running basic connectivity simulation..." << std::endl;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cerr << "Error: No brain regions found" << std::endl;
        return false;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        std::cerr << "Error: No neurons found" << std::endl;
        return false;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into first 10 neurons
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    // Run simulation
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 200 == 0) {
            std::cout << "  Step " << step << "/" << steps << std::endl;
        }
    }
    
    size_t finalSpikes = brain->getTotalSpikeCount();
    size_t spikesGenerated = finalSpikes - initialSpikes;
    
    std::cout << "Basic simulation completed. Spikes generated: " << spikesGenerated << std::endl;
    
    return true;
}

bool RunCommand::runPlasticitySimulation(std::shared_ptr<Brain> brain, size_t steps) {
    std::cout << "Running plasticity learning simulation..." << std::endl;
    
    // Enable plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Hebbian + STDP
        }
    }
    
    // Apply input patterns to stimulate learning
    for (size_t step = 0; step < steps; ++step) {
        // Inject sensory input
        for (size_t i = 0; i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 200 == 0) {
            std::cout << "  Step " << step << "/" << steps 
                     << " | Spikes: " << brain->getTotalSpikeCount()
                     << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
        }
    }
    
    std::cout << "Plasticity simulation completed." << std::endl;
    
    return true;
}

bool RunCommand::runStdpSimulation(std::shared_ptr<Brain> brain, size_t steps) {
    std::cout << "Running STDP verification simulation..." << std::endl;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cerr << "Error: No brain regions found" << std::endl;
        return false;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        std::cerr << "Error: Not enough synapses for STDP test" << std::endl;
        return false;
    }
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
    }
    
    // Create correlated activity (pre-before-post to trigger LTP)
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        for (int trial = 0; trial < 50; ++trial) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    std::cout << "STDP results:" << std::endl;
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        std::cout << "  Synapse " << i << ": " << beforeWeights[i] << " -> " 
                 << synapses[i]->getWeight() << " (Δ=" << delta << ")" << std::endl;
    }
    
    if (totalDelta > 0.001f) {
        std::cout << "✓ STDP working: Pre-before-post produced potentiation" << std::endl;
    } else if (totalDelta < -0.001f) {
        std::cout << "! STDP reversed: Check parameters" << std::endl;
    } else {
        std::cout << "! No change: STDP may not be triggering" << std::endl;
    }
    
    return true;
}

bool RunCommand::runLearningSimulation(std::shared_ptr<Brain> brain, size_t iterations) {
    std::cout << "Running learning experiment..." << std::endl;
    
    // Record initial state
    size_t initialSynapses = brain->getTotalSynapseCount();
    float initialWeightSum = 0.0f;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeightSum += syn->getWeight();
        }
    }
    
    std::cout << "Initial state: " << initialSynapses << " synapses, mean weight: " 
             << (initialSynapses > 0 ? initialWeightSum / initialSynapses : 0.0f) << std::endl;
    
    // Enable plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    // Run learning iterations
    for (size_t iter = 0; iter < iterations; ++iter) {
        // Generate random input pattern
        for (size_t i = 0; i < brain->getTotalNeuronCount() / 10; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
        }
        
        brain->step(iter, iter * 0.001);
        
        if (iter % 50 == 0) {
            std::cout << "  Iteration " << iter << "/" << iterations 
                     << " | Spikes: " << brain->getTotalSpikeCount()
                     << " | Synapses: " << brain->getTotalSynapseCount() << std::endl;
        }
    }
    
    // Record final state
    size_t finalSynapses = brain->getTotalSynapseCount();
    float finalWeightSum = 0.0f;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeightSum += syn->getWeight();
        }
    }
    
    std::cout << "Final state: " << finalSynapses << " synapses, mean weight: " 
             << (finalSynapses > 0 ? finalWeightSum / finalSynapses : 0.0f) << std::endl;
    
    // Calculate statistics
    float initialMean = initialSynapses > 0 ? initialWeightSum / initialSynapses : 0.0f;
    float finalMean = finalSynapses > 0 ? finalWeightSum / finalSynapses : 0.0f;
    float weightChange = finalMean - initialMean;
    
    std::cout << "\n=== Learning Statistics ===" << std::endl;
    std::cout << "Weight change: " << weightChange << std::endl;
    std::cout << "Initial synapses: " << initialSynapses << std::endl;
    std::cout << "Final synapses: " << finalSynapses << std::endl;
    
    bool learningOccurred = std::abs(weightChange) > 0.001f;
    if (learningOccurred) {
        std::cout << "✓ LEARNING DETECTED: Synaptic weights changed through experience" << std::endl;
    } else {
        std::cout << "✗ NO LEARNING: Weights did not change significantly" << std::endl;
    }
    
    return true;
}

bool RunCommand::runConnectivitySimulation(std::shared_ptr<Brain> brain, size_t steps) {
    std::cout << "Running connectivity analysis simulation..." << std::endl;
    
    // Reset brain for clean analysis
    brain->reset();
    brain->initialize();
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into random neurons to test connectivity
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cerr << "Error: No brain regions found" << std::endl;
        return false;
    }
    
    auto neurons = region->getAllNeurons();
    std::cout << "Testing connectivity with " << neurons.size() << " neurons" << std::endl;
    
    // Test different input intensities
    for (int intensity = 20; intensity <= 80; intensity += 20) {
        std::cout << "\nTesting input intensity: " << intensity << std::endl;
        
        size_t spikesBefore = brain->getTotalSpikeCount();
        
        // Inject current into 20% of neurons
        for (size_t i = 0; i < std::min(size_t(neurons.size() * 0.2), neurons.size()); ++i) {
            neurons[i]->injectCurrent(static_cast<float>(intensity));
        }
        
        // Run steps
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikesAfter = brain->getTotalSpikeCount();
        size_t spikesGenerated = spikesAfter - spikesBefore;
        
        std::cout << "  Spikes generated: " << spikesGenerated << std::endl;
    }
    
    size_t totalSpikes = brain->getTotalSpikeCount() - initialSpikes;
    std::cout << "\nConnectivity simulation completed. Total spikes: " << totalSpikes << std::endl;
    
    return true;
}

bool RunCommand::runExperimentScenario(std::shared_ptr<Brain> brain, const std::string& scenario, size_t steps) {
    std::cout << "Running experiment scenario: " << scenario << std::endl;
    
    if (scenario == "basic") {
        return runBasicSimulation(brain, steps);
    } else if (scenario == "plasticity") {
        return runPlasticitySimulation(brain, steps);
    } else if (scenario == "stdp") {
        return runStdpSimulation(brain, steps);
    } else if (scenario == "learning") {
        return runLearningSimulation(brain, steps);
    } else if (scenario == "connectivity") {
        return runConnectivitySimulation(brain, steps);
    } else {
        std::cerr << "Error: Unknown scenario: " << scenario << std::endl;
        return false;
    }
}
