// NLM (熙然) - Neural Learning Machine CLI Command Handler
//
// This file implements the command handlers for various CLI operations
// including running simulations, experiments, analysis, and benchmarking.

#ifndef NLM_COMMAND_HANDLER_HPP
#define NLM_COMMAND_HANDLER_HPP

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"

namespace nlm {
namespace cli {

class CommandHandler {
public:
    CommandHandler();
    ~CommandHandler() = default;
    
    // Execute run command
    bool executeRunCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config);
    
    // Execute experiment command
    bool executeExperimentCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config);
    
    // Execute analyze command
    bool executeAnalyzeCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config);
    
    // Execute benchmark command
    bool executeBenchmarkCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config);
    
    // Execute configure command
    bool executeConfigureCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config);
    
    // Execute show-config command
    bool executeShowConfigCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config);
    
    // Execute save-config command
    bool executeSaveConfigCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config);

private:
    // Setup brain with configuration
    bool setupBrain(std::shared_ptr<Config> config);
    
    // Run basic simulation
    bool runBasicSimulation(std::shared_ptr<Config> config);
    
    // Run plasticity experiment
    bool runPlasticityExperiment(std::shared_ptr<Config> config);
    
    // Run STDP verification
    bool runStdpVerification(std::shared_ptr<Config> config);
    
    // Run development experiment
    bool runDevelopmentExperiment(std::shared_ptr<Config> config);
    
    // Run cognitive experiment
    bool runCognitiveExperiment(std::shared_ptr<Config> config);
    
    // Run social experiment
    bool runSocialExperiment(std::shared_ptr<Config> config);
    
    // Run memory experiment
    bool runMemoryExperiment(std::shared_ptr<Config> config);
    
    // Run attention experiment
    bool runAttentionExperiment(std::shared_ptr<Config> config);
    
    // Output results in different formats
    bool outputResults(const std::string& filename, const std::string& format, 
                      const std::string& data) const;
    
    // Format output as JSON
    std::string formatJsonOutput(const std::string& data) const;
    
    // Format output as CSV
    std::string formatCsvOutput(const std::string& data) const;
    
    // Format output as text
    std::string formatTextOutput(const std::string& data) const;
    
    // Helper functions
    void enablePlasticityOnBrain(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    void recordPlasticityInitialState(std::shared_ptr<Brain> brain);
    void recordPlasticityFinalState(std::shared_ptr<Brain> brain);
    void computePlasticityStatistics(std::shared_ptr<Brain> brain);
    void enableStdpOnly(std::shared_ptr<Brain> brain);
    void runStdpTest(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    void applyInputPattern(std::shared_ptr<Brain> brain, SimulationStep step);
};

// Command type parser helper function
inline CommandType parseCommandType(const std::string& commandStr) {
    std::string cmd = commandStr;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    if (cmd == "run") return CommandType::Run;
    if (cmd == "configure") return CommandType::Configure;
    if (cmd == "experiment") return CommandType::Experiment;
    if (cmd == "analyze") return CommandType::Analyze;
    if (cmd == "benchmark") return CommandType::Benchmark;
    if (cmd == "help") return CommandType::Help;
    if (cmd == "version") return CommandType::Version;
    if (cmd == "list-presets") return CommandType::ListPresets;
    if (cmd == "show-config") return CommandType::ShowConfig;
    if (cmd == "save-config") return CommandType::SaveConfig;
    
    return CommandType::Run; // Default
}

} // namespace cli
} // namespace nlm

#endif // NLM_COMMAND_HANDLER_HPP


bool CommandHandler::executeRunCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Executing run command");
    
    // Setup brain with configuration
    if (!setupBrain(config)) {
        NLM_LOG_ERROR("Failed to setup brain");
        return false;
    }
    
    // Get simulation type from options
    std::string simulationType = options.count("type") ? options.at("type") : "basic";
    
    NLM_LOG_INFO("Running simulation type: " + simulationType);
    
    // Select simulation based on type
    if (simulationType == "basic") {
        return runBasicSimulation(config);
    }
    else if (simulationType == "plasticity") {
        return runPlasticityExperiment(config);
    }
    else if (simulationType == "stdp") {
        return runStdpVerification(config);
    }
    else if (simulationType == "development") {
        return runDevelopmentExperiment(config);
    }
    else if (simulationType == "cognitive") {
        return runCognitiveExperiment(config);
    }
    else if (simulationType == "social") {
        return runSocialExperiment(config);
    }
    else if (simulationType == "memory") {
        return runMemoryExperiment(config);
    }
    else if (simulationType == "attention") {
        return runAttentionExperiment(config);
    }
    else {
        NLM_LOG_ERROR("Unknown simulation type: " + simulationType);
        return false;
    }
}

bool CommandHandler::executeExperimentCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Executing experiment command");
    
    std::string experimentType = options.count("type") ? options.at("type") : "plasticity";
    
    NLM_LOG_INFO("Running experiment type: " + experimentType);
    
    if (experimentType == "plasticity") {
        return runPlasticityExperiment(config);
    }
    else if (experimentType == "development") {
        return runDevelopmentExperiment(config);
    }
    else if (experimentType == "cognitive") {
        return runCognitiveExperiment(config);
    }
    else if (experimentType == "social") {
        return runSocialExperiment(config);
    }
    else if (experimentType == "memory") {
        return runMemoryExperiment(config);
    }
    else if (experimentType == "attention") {
        return runAttentionExperiment(config);
    }
    else {
        NLM_LOG_ERROR("Unknown experiment type: " + experimentType);
        return false;
    }
}

bool CommandHandler::executeAnalyzeCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Executing analyze command");
    
    // Implementation will be completed in subsequent edits
    NLM_LOG_INFO("Analyze command - implementation in progress");
    return true;
}

bool CommandHandler::executeBenchmarkCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Executing benchmark command");
    
    // Implementation will be completed in subsequent edits
    NLM_LOG_INFO("Benchmark command - implementation in progress");
    return true;
}

bool CommandHandler::executeConfigureCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Executing configure command");
    
    // Implementation will be completed in subsequent edits
    NLM_LOG_INFO("Configure command - implementation in progress");
    return true;
}

bool CommandHandler::executeShowConfigCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Executing show-config command");
    
    // Display configuration
    NLM_LOG_INFO("=== Current Configuration ===");
    NLM_LOG_INFO("random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    
    return true;
}

bool CommandHandler::executeSaveConfigCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Executing save-config command");
    
    std::string filename = options.count("file") ? options.at("file") : "nlm_config.json";
    
    if (config->saveToFile(filename)) {
        NLM_LOG_INFO("Configuration saved to: " + filename);
        return true;
    }
    else {
        NLM_LOG_ERROR("Failed to save configuration to: " + filename);
        return false;
    }
}

bool CommandHandler::setupBrain(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Setting up brain...");
    
    try {
        brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        clock = std::make_shared<SimulationClock>(
            config->getOr<double>("simulation_timestep", 0.001)
        );
        
        brain->logStatus();
        return true;
    }
    catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting up brain: " + std::string(e.what()));
        return false;
    }
}

bool CommandHandler::runBasicSimulation(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running basic simulation");
    
    // Reset and initialize brain
    brain->reset();
    brain->initialize();
    
    // Run basic connectivity test
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into neurons
    auto neurons = brain->getAllNeurons();
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    // Run simulation for specified steps
    size_t maxSteps = config->getOr<int64_t>("max_simulation_steps", 100);
    for (SimulationStep step = 0; step < maxSteps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("Basic simulation completed: " + std::to_string(spikes) + " spikes generated");
    
    brain->logStatus();
    return true;
}

bool CommandHandler::runPlasticityExperiment(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running plasticity experiment");
    
    // Reset and initialize brain
    brain->reset();
    brain->initialize();
    
    // Enable plasticity
    enablePlasticityOnBrain(brain, config);
    
    // Record initial state
    recordPlasticityInitialState(brain);
    
    // Run plasticity learning
    size_t maxSteps = config->getOr<int64_t>("plasticity_simulation_steps", 1000);
    for (SimulationStep step = 0; step < maxSteps; ++step) {
        brain->step(step, step * 0.001);
        
        // Apply input patterns periodically
        if (step % 100 == 0) {
            applyInputPattern(brain, step);
        }
        
        if (step % 200 == 0) {
            NLM_LOG_INFO("Plasticity step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    recordPlasticityFinalState(brain);
    
    // Compute and display statistics
    computePlasticityStatistics(brain);
    
    NLM_LOG_INFO("Plasticity experiment completed");
    return true;
}

bool CommandHandler::runStdpVerification(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running STDP verification experiment");
    
    // Reset and initialize brain
    brain->reset();
    brain->initialize();
    
    // Enable only STDP plasticity
    enableStdpOnly(brain);
    
    // Run STDP test
    runStdpTest(brain, config);
    
    NLM_LOG_INFO("STDP verification completed");
    return true;
}

bool CommandHandler::runDevelopmentExperiment(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running development experiment");
    
    // Implementation for development experiment
    NLM_LOG_INFO("Development experiment - implementation in progress");
    return true;
}

bool CommandHandler::runCognitiveExperiment(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running cognitive experiment");
    
    // Implementation for cognitive experiment
    NLM_LOG_INFO("Cognitive experiment - implementation in progress");
    return true;
}

bool CommandHandler::runSocialExperiment(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running social experiment");
    
    // Implementation for social experiment
    NLM_LOG_INFO("Social experiment - implementation in progress");
    return true;
}

bool CommandHandler::runMemoryExperiment(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running memory experiment");
    
    // Implementation for memory experiment
    NLM_LOG_INFO("Memory experiment - implementation in progress");
    return true;
}

bool CommandHandler::runAttentionExperiment(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running attention experiment");
    
    // Implementation for attention experiment
    NLM_LOG_INFO("Attention experiment - implementation in progress");
    return true;
}

void CommandHandler::enablePlasticityOnBrain(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Enabling plasticity on brain");
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            bool hebbian = config->getOr<bool>("plasticity_hebbian_enable", true);
            bool stdp = config->getOr<bool>("plasticity_stdp_enable", true);
            bool structural = config->getOr<bool>("plasticity_structural_enable", false);
            
            syn->enablePlasticity(hebbian, stdp, structural);
        }
    }
}

void CommandHandler::recordPlasticityInitialState(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Recording initial plasticity state");
    
    // Implementation for recording initial state
}

void CommandHandler::recordPlasticityFinalState(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Recording final plasticity state");
    
    // Implementation for recording final state
}

void CommandHandler::computePlasticityStatistics(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Computing plasticity statistics");
    
    // Implementation for computing statistics
}

void CommandHandler::enableStdpOnly(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Enabling STDP only");
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(false, true, false);  // Enable only STDP
        }
    }
}

void CommandHandler::runStdpTest(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Running STDP test");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("Testing STDP on 5 synapses");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
    }
    
    // Create correlated activity for potentiation
    for (int trial = 0; trial < 50; ++trial) {
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            Neuron* preNeuron = neurons[0];
            Neuron* postNeuron = neurons[1];
            
            // Pre fires then post fires (LTP)
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Check weight changes
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        NLM_LOG_INFO("Synapse " + std::to_string(i) + 
                    " Δ=" + std::to_string(delta) +
                    " weight=" + std::to_string(synapses[i]->getWeight()));
    }
    
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("! No change: STDP may not be triggering");
    }
}

void CommandHandler::applyInputPattern(std::shared_ptr<Brain> brain, SimulationStep step) {
    // Implementation for applying input patterns
}

bool CommandHandler::outputResults(const std::string& filename, const std::string& format, 
                                   const std::string& data) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open output file: " + filename);
        return false;
    }
    
    if (format == "json") {
        file << formatJsonOutput(data);
    }
    else if (format == "csv") {
        file << formatCsvOutput(data);
    }
    else if (format == "text") {
        file << formatTextOutput(data);
    }
    else {
        NLM_LOG_ERROR("Unknown output format: " + format);
        return false;
    }
    
    file.close();
    NLM_LOG_INFO("Results saved to: " + filename);
    return true;
}

std::string CommandHandler::formatJsonOutput(const std::string& data) const {
    // Implementation for JSON formatting
    return "{\"results\": \"" + data + "\"}";
}

std::string CommandHandler::formatCsvOutput(const std::string& data) const {
    // Implementation for CSV formatting
    return "data\n" + data;
}

std::string CommandHandler::formatTextOutput(const std::string& data) const {
    // Implementation for text formatting
    return data;
}

} // namespace cli
} // namespace nlm
