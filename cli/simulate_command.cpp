// NLM Simulate Command - Run specific simulation scenarios
// Handles running predefined simulation scenarios

#pragma once

#include "cli/nlm_cli.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "environment/Environment.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>

namespace nlm {

class SimulateCommand {
public:
    static bool execute(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void showHelp();
    static bool runScenario(NLMCLI& cli, const std::string& scenario, const std::map<std::string, std::string>& args);
    static bool runBasicScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args);
    static bool runPlasticityScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args);
    static bool runStdpScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args);
    static bool runLearningScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args);
    static bool runConnectivityScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args);
    static bool runExperimentScenario(std::shared_ptr<Brain> brain, const std::string& scenario, const std::map<std::string, std::string>& args);
    static std::string getAvailableScenarios();
};

bool SimulateCommand::execute(NLMCLI& cli, const std::map<std::string, std::string>& args) {
    std::cout << "=== NLM Simulate Command ===" << std::endl;
    
    if (!args.count("scenario")) {
        std::cout << "Available simulation scenarios:" << std::endl;
        std::cout << getAvailableScenarios() << std::endl;
        std::cout << std::endl;
        showHelp();
        return true;
    }
    
    std::string scenario = args.at("scenario");
    return runScenario(cli, scenario, args);
}

void SimulateCommand::showHelp() {
    std::cout << "=== NLM Simulate Command Help ===" << std::endl;
    std::cout << "Usage: nlm simulate <scenario> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Run specific simulation scenarios:" << std::endl;
    std::cout << "  Scenarios:" << std::endl;
    std::cout << "    basic          Basic connectivity and neuron activation" << std::endl;
    std::cout << "    plasticity     Plasticity learning experiment" << std::endl;
    std::cout << "    stdp           Spike-timing-dependent plasticity" << std::endl;
    std::cout << "    learning       General learning experiment" << std::endl;
    std::cout << "    connectivity   Network connectivity analysis" << std::endl;
    std::cout << "    experiment     General experiment runner" << std::endl;
    std::cout << "  -c, --config FILE              Load configuration from FILE" << std::endl;
    std::cout << "  --iterations N                 Number of iterations/steps" << std::endl;
    std::cout << "  --output-dir DIR               Output directory for results" << std::endl;
    std::cout << "  --save-results                 Save simulation results" << std::endl;
    std::cout << "  --record-brain                 Record brain state after simulation" << std::endl;
    std::cout << "  -h, --help                     Show this help message" << std::endl;
}

bool SimulateCommand::runScenario(NLMCLI& cli, const std::string& scenario, const std::map<std::string, std::string>& args) {
    std::cout << "Running simulation scenario: " << scenario << std::endl;
    
    // Load configuration
    std::string configFile = cli.getConfigFile();
    auto config = cli.loadConfiguration(configFile);
    if (!config) {
        std::cerr << "Error: Failed to load configuration" << std::endl;
        return false;
    }
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain" << std::endl;
        return false;
    }
    
    std::cout << "Brain initialized with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    
    // Determine number of iterations
    size_t iterations = args.count("iterations") ? std::stoul(args.at("iterations")) : 1000;
    
    // Run the selected scenario
    bool success = runExperimentScenario(brain, scenario, args);
    
    // Save results if requested
    if (args.count("save-results") && success) {
        std::string outputDir = args.count("output-dir") ? args.at("output-dir") : "./simulation_results";
        saveSimulationResults(outputDir, brain, scenario, args);
    }
    
    // Record brain state if requested
    if (args.count("record-brain") && success) {
        std::string recordFile = outputDir + "/brain_state.bin";
        cli.saveBrainState(recordFile, brain);
    }
    
    if (success) {
        std::cout << "Simulation scenario completed successfully!" << std::endl;
    } else {
        std::cerr << "Simulation scenario failed." << std::endl;
    }
    
    return success;
}

bool SimulateCommand::runBasicScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args) {
    std::cout << "Running basic connectivity scenario..." << std::endl;
    
    size_t steps = args.count("iterations") ? std::stoul(args.at("iterations")) : 500;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cerr << "Error: No brain regions found" << std::endl;
        return false;
    }
    
    auto neurons = region->getAllNeurons();
    std::cout << "Testing connectivity with " << neurons.size() << " neurons" << std::endl;
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into first 20 neurons
    for (size_t i = 0; i < std::min(size_t(20), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    // Run simulation
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            std::cout << "  Step " << step << "/" << steps << std::endl;
        }
    }
    
    size_t finalSpikes = brain->getTotalSpikeCount();
    size_t spikesGenerated = finalSpikes - initialSpikes;
    
    std::cout << "Basic scenario completed. Spikes generated: " << spikesGenerated << std::endl;
    
    return true;
}

bool SimulateCommand::runPlasticityScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args) {
    std::cout << "Running plasticity learning scenario..." << std::endl;
    
    size_t iterations = args.count("iterations") ? std::stoul(args.at("iterations")) : 2000;
    
    // Enable plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Hebbian + STDP
        }
    }
    
    // Run learning iterations
    for (size_t iter = 0; iter < iterations; ++iter) {
        // Generate input pattern
        for (size_t i = 0; i < brain->getTotalNeuronCount() / 10; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(iter, iter * 0.001);
        
        if (iter % 200 == 0) {
            std::cout << "  Iteration " << iter << "/" << iterations 
                     << " | Spikes: " << brain->getTotalSpikeCount()
                     << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
        }
    }
    
    std::cout << "Plasticity scenario completed." << std::endl;
    
    return true;
}

bool SimulateCommand::runStdpScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args) {
    std::cout << "Running STDP scenario..." << std::endl;
    
    size_t steps = args.count("iterations") ? std::stoul(args.at("iterations")) : 1000;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cerr << "Error: No brain regions found" << std::endl;
        return false;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 3) {
        std::cerr << "Error: Not enough synapses for STDP test" << std::endl;
        return false;
    }
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 3; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
    }
    
    // Create correlated activity
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        for (int trial = 0; trial < 30; ++trial) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights and analyze
    std::cout << "STDP scenario results:" << std::endl;
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 3; ++i) {
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

bool SimulateCommand::runLearningScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args) {
    std::cout << "Running learning scenario..." << std::endl;
    
    size_t iterations = args.count("iterations") ? std::stoul(args.at("iterations")) : 1500;
    
    // Record initial state
    size_t initialSynapses = brain->getTotalSynapseCount();
    float initialWeightSum = 0.0f;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeightSum += syn->getWeight();
        }
    }
    
    float initialMean = initialSynapses > 0 ? initialWeightSum / initialSynapses : 0.0f;
    
    std::cout << "Initial state: " << initialSynapses << " synapses, mean weight: " << initialMean << std::endl;
    
    // Enable all plasticity mechanisms
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    // Run learning iterations
    for (size_t iter = 0; iter < iterations; ++iter) {
        // Generate random input pattern
        for (size_t i = 0; i < brain->getTotalNeuronCount() / 12; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
        }
        
        brain->step(iter, iter * 0.001);
        
        if (iter % 300 == 0) {
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
    
    float finalMean = finalSynapses > 0 ? finalWeightSum / finalSynapses : 0.0f;
    float weightChange = finalMean - initialMean;
    
    std::cout << std::endl << "=== Learning Statistics ===" << std::endl;
    std::cout << "Final state: " << finalSynapses << " synapses, mean weight: " << finalMean << std::endl;
    std::cout << "Weight change: " << weightChange << std::endl;
    
    bool learningOccurred = std::abs(weightChange) > 0.001f;
    if (learningOccurred) {
        std::cout << "✓ LEARNING DETECTED: Synaptic weights changed through experience" << std::endl;
    } else {
        std::cout << "✗ NO LEARNING: Weights did not change significantly" << std::endl;
    }
    
    return true;
}

bool SimulateCommand::runConnectivityScenario(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& args) {
    std::cout << "Running connectivity analysis scenario..." << std::endl;
    
    size_t iterations = args.count("iterations") ? std::stoul(args.at("iterations")) : 800;
    
    // Reset brain for clean analysis
    brain->reset();
    brain->initialize();
    
    std::cout << "Running connectivity analysis with " << iterations << " iterations" << std::endl;
    
    // Test network response to different input intensities
    for (int intensity = 10; intensity <= 60; intensity += 10) {
        std::cout << "\nTesting input intensity: " << intensity << std::endl;
        
        size_t spikesBefore = brain->getTotalSpikeCount();
        
        // Inject current into 15% of neurons
        for (size_t i = 0; i < brain->getTotalNeuronCount() * 0.15; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, static_cast<float>(intensity));
        }
        
        // Run iterations
        for (size_t step = 0; step < iterations; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikesAfter = brain->getTotalSpikeCount();
        size_t spikesGenerated = spikesAfter - spikesBefore;
        
        std::cout << "  Spikes generated: " << spikesGenerated << std::endl;
    }
    
    std::cout << "\nConnectivity scenario completed." << std::endl;
    
    return true;
}

bool SimulateCommand::runExperimentScenario(std::shared_ptr<Brain> brain, const std::string& scenario, const std::map<std::string, std::string>& args) {
    if (scenario == "basic") {
        return runBasicScenario(brain, args);
    } else if (scenario == "plasticity") {
        return runPlasticityScenario(brain, args);
    } else if (scenario == "stdp") {
        return runStdpScenario(brain, args);
    } else if (scenario == "learning") {
        return runLearningScenario(brain, args);
    } else if (scenario == "connectivity") {
        return runConnectivityScenario(brain, args);
    } else {
        std::cerr << "Error: Unknown simulation scenario: " << scenario << std::endl;
        return false;
    }
}

std::string SimulateCommand::getAvailableScenarios() {
    return "  basic          Basic connectivity and neuron activation" + std::string(40, ' ') + std::endl +
           "  plasticity     Plasticity learning experiment" + std::string(38, ' ') + std::endl +
           "  stdp           Spike-timing-dependent plasticity" + std::string(38, ' ') + std::endl +
           "  learning       General learning experiment" + std::string(38, ' ') + std::endl +
           "  connectivity   Network connectivity analysis" + std::string(28, ' ') + std::endl +
           "  experiment     General experiment runner" + std::string(34, ' ');
}

bool SimulateCommand::saveSimulationResults(const std::string& outputDir, std::shared_ptr<Brain> brain, const std::string& scenario, const std::map<std::string, std::string>& args) {
    std::cout << "Saving simulation results to: " << outputDir << std::endl;
    
    // Create output directory
    std::filesystem::create_directories(outputDir);
    
    // Save brain state
    std::string brainFile = outputDir + "/brain_state.bin";
    // saveBrainToFile(brainFile, brain);
    
    // Save scenario parameters
    std::string paramsFile = outputDir + "/scenario_params.txt";
    std::ofstream file(paramsFile);
    if (file.is_open()) {
        file << "Scenario: " << scenario << std::endl;
        file << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        file << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        file << "Timestamp: " << std::time(nullptr) << std::endl;
        file.close();
    }
    
    std::cout << "Simulation results saved to: " << outputDir << std::endl;
    
    return true;
}
