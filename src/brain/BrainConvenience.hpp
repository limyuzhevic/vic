// Convenience methods for Brain class
// These methods simplify common usage patterns and reduce boilerplate code

#pragma once

#include "Brain.hpp"
#include <string>

namespace nlm {

// Convenience functions for brain configuration and usage

// Quick brain setup with common parameters
inline std::shared_ptr<Brain> createBrainWithConfig(
    size_t neuronCount = 1000,
    float connectionProbability = 0.1f,
    bool enablePlasticity = true,
    bool enableDevelopment = false,
    const std::string& name = "DefaultBrain") {
    
    auto config = std::make_shared<Config>();
    config->setInt("brain.neuronCount", static_cast<int>(neuronCount));
    config->setDouble("brain.connectionProbability", connectionProbability);
    config->setBool("plasticity.enabled", enablePlasticity);
    config->setBool("development.enabled", enableDevelopment);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}

// Quick brain setup with environment integration
inline std::pair<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>> createBrainAndAgent(
    const SimpleWorld& world,
    size_t neuronCount = 500,
    float connectionProbability = 0.05f) {
    
    auto config = std::make_shared<Config>();
    config->setInt("brain.neuronCount", static_cast<int>(neuronCount));
    config->setDouble("brain.connectionProbability", connectionProbability);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world);
    
    return {brain, agent};
}

// Quick simulation setup and execution
inline void runQuickSimulation(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world = nullptr,
    std::shared_ptr<AgentBrain> agent = nullptr,
    size_t steps = 100) {
    
    if (!brain) {
        throw std::runtime_error("Brain cannot be null for simulation");
    }
    
    for (size_t step = 0; step < steps; ++step) {
        if (world && agent) {
            // Run with world and agent
            world->update(0.1); // Update world
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        } else {
            // Simple brain-only simulation
            brain->step(step);
        }
    }
}

// Quick statistical analysis
inline std::string getBrainStatsSummary(const Brain& brain) {
    std::ostringstream oss;
    oss << "=== Brain Statistics ===\n";
    oss << "Neurons: " << brain.getTotalNeuronCount() << "\n";
    oss << "Synapses: " << brain.getTotalSynapseCount() << "\n";
    oss << "Firing Rate: " << brain.getAverageFiringRate() << " Hz\n";
    oss << "E/I Ratio: " << brain.getExcitationInhibitionRatio() << "\n";
    oss << "Total Spikes: " << brain.getTotalSpikeCount() << "\n";
    oss << "Active Neurons: " << brain.getActiveNeuronCount() << "\n";
    oss << "Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
    oss << "Development Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
    
    return oss.str();
}

// Quick configuration utilities
inline Config createQuickConfig(const std::string& configType = "balanced") {
    Config config;
    
    if (configType == "minimal") {
        // Minimal configuration
        config.setInt("brain.neuronCount", 100);
        config.setDouble("simulation.timestep", 0.001);
        config.setBool("plasticity.enabled", false);
        config.setBool("memory.workingMemory", true);
    } else if (configType == "balanced") {
        // Balanced configuration
        config.setInt("brain.neuronCount", 1000);
        config.setDouble("simulation.timestep", 0.001);
        config.setBool("plasticity.enabled", true);
        config.setBool("memory.workingMemory", true);
        config.setBool("memory.episodicMemory", true);
        config.setDouble("plasticity.learningRate", 0.01);
    } else if (configType == "advanced") {
        // Advanced configuration
        config.setInt("brain.neuronCount", 5000);
        config.setDouble("simulation.timestep", 0.0005);
        config.setBool("plasticity.enabled", true);
        config.setBool("plasticity.structural", true);
        config.setBool("memory.workingMemory", true);
        config.setBool("memory.episodicMemory", true);
        config.setBool("memory.associativeMemory", true);
        config.setBool("neuromodulation.dopamine", true);
        config.setBool("neuromodulation.curiosity", true);
        config.setBool("development.enabled", true);
    }
    
    return config;
}

// Quick experiment setup
inline bool setupQuickExperiment(
    std::shared_ptr<Brain> brain,
    const std::string& experimentType = "standard",
    double duration = 1.0) {
    
    if (!brain) {
        throw std::runtime_error("Brain cannot be null for experiment");
    }
    
    // Configure brain for experiment
    brain->getConfig()->setString("experiment.type", experimentType);
    brain->getConfig()->setDouble("experiment.duration", duration);
    
    // Enable appropriate features based on experiment type
    if (experimentType == "learning") {
        brain->getConfig()->setBool("plasticity.enabled", true);
        brain->getConfig()->setBool("memory.workingMemory", true);
        brain->getConfig()->setBool("memory.episodicMemory", true);
    } else if (experimentType == "development") {
        brain->getConfig()->setBool("development.enabled", true);
        brain->getConfig()->setBool("plasticity.structural", true);
    } else if (experimentType == "social") {
        brain->getConfig()->setBool("neuromodulation.dopamine", true);
        brain->getConfig()->setBool("neuromodulation.curiosity", true);
        brain->getConfig()->setBool("memory.associativeMemory", true);
    }
    
    return brain->initialize();
}

// Quick validation and error checking
inline std::vector<std::string> validateBrainSetup(const Brain& brain) {
    std::vector<std::string> errors;
    
    if (!brain.getConfig()) {
        errors.push_back("Brain configuration is null");
    }
    
    auto config = brain.getConfig();
    
    // Check neuron count
    if (config->has("brain.neuronCount")) {
        auto neuronCount = config->getOr<int>("brain.neuronCount", 0);
        if (neuronCount <= 0) {
            errors.push_back("Invalid neuron count: " + std::to_string(neuronCount));
        }
    }
    
    // Check timestep
    if (config->has("simulation.timestep")) {
        auto timestep = config->getOr<double>("simulation.timestep", 0.0);
        if (timestep <= 0.0) {
            errors.push_back("Invalid timestep: " + std::to_string(timestep));
        }
    }
    
    // Check plasticity
    if (config->has("plasticity.enabled")) {
        auto plasticityEnabled = config->getOr<bool>("plasticity.enabled", false);
        if (plasticityEnabled && !config->has("plasticity.learningRate")) {
            errors.push_back("Plasticity enabled but learning rate not configured");
        }
    }
    
    return errors;
}

// Quick save/load utilities
inline bool saveBrainSnapshot(const Brain& brain, const std::string& filename) {
    return brain.save(filename);
}

inline bool loadBrainSnapshot(Brain& brain, const std::string& filename) {
    return brain.load(filename);
}

// Quick comparison utilities
inline double calculateSimilarity(const Brain& brain1, const Brain& brain2) {
    // Simple similarity calculation based on neuron count, firing rate, etc.
    double similarity = 0.0;
    
    // Neuron count similarity
    double neuronCount1 = brain1.getTotalNeuronCount();
    double neuronCount2 = brain2.getTotalNeuronCount();
    double neuronCountSimilarity = 1.0 - std::abs(neuronCount1 - neuronCount2) / std::max(neuronCount1, 1.0);
    similarity += neuronCountSimilarity * 0.4;
    
    // Firing rate similarity
    double firingRate1 = brain1.getAverageFiringRate();
    double firingRate2 = brain2.getAverageFiringRate();
    double firingRateSimilarity = 1.0 - std::abs(firingRate1 - firingRate2) / std::max(firingRate1, 1.0);
    similarity += firingRateSimilarity * 0.3;
    
    // Spike count similarity
    double spikeCount1 = brain1.getTotalSpikeCount();
    double spikeCount2 = brain2.getTotalSpikeCount();
    double spikeCountSimilarity = 1.0 - std::abs(spikeCount1 - spikeCount2) / std::max(spikeCount1, 1.0);
    similarity += spikeCountSimilarity * 0.3;
    
    return similarity;
}

// Quick debugging utilities
inline std::string debugBrainState(const Brain& brain) {
    std::ostringstream oss;
    oss << "=== Brain Debug State ===\n";
    oss << "Total Neurons: " << brain.getTotalNeuronCount() << "\n";
    oss << "Total Synapses: " << brain.getTotalSynapseCount() << "\n";
    oss << "Active Neurons: " << brain.getActiveNeuronCount() << "\n";
    oss << "Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
    oss << "Average Firing Rate: " << brain.getAverageFiringRate() << " Hz\n";
    oss << "E/I Ratio: " << brain.getExcitationInhibitionRatio() << "\n";
    oss << "Total Spike Count: " << brain.getTotalSpikeCount() << "\n";
    oss << "Pending Spike Events: " << brain.getPendingSpikeEventCount() << "\n";
    oss << "Development Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
    
    return oss.str();
}

// Quick Python integration utilities
inline py::dict convertBrainToPythonDict(std::shared_ptr<Brain> brain) {
    py::dict result;
    
    if (brain) {
        result["neuron_count"] = brain->getTotalNeuronCount();
        result["synapse_count"] = brain->getTotalSynapseCount();
        result["firing_rate"] = brain->getAverageFiringRate();
        result["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        result["total_spikes"] = brain->getTotalSpikeCount();
        result["active_neurons"] = brain->getActiveNeuronCount();
        result["firing_neurons"] = brain->getFiringNeuronCount();
        result["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
    }
    
    return result;
}

} // namespace nlm

// Python-specific convenience functions
namespace nlm {

// Quick Python brain setup
inline py::object createQuickPythonBrain(py::kwargs kwargs) {
    // Extract parameters from kwargs
    size_t neuronCount = kwargs.get("neuron_count", 1000).cast<size_t>();
    float connectionProbability = kwargs.get("connection_probability", 0.1f).cast<float>();
    bool enablePlasticity = kwargs.get("enable_plasticity", true).cast<bool>();
    bool enableDevelopment = kwargs.get("enable_development", false).cast<bool>();
    std::string name = kwargs.get("name", "DefaultBrain").cast<std::string>();
    
    auto brain = createBrainWithConfig(neuronCount, connectionProbability, 
                                       enablePlasticity, enableDevelopment, name);
    
    return py::cast(brain);
}

// Quick Python simulation
inline void runQuickPythonSimulation(
    py::object brainPy,
    py::object worldPy = nullptr,
    py::object agentPy = nullptr,
    size_t steps = 100) {
    
    auto brain = brainPy.cast<std::shared_ptr<Brain>>();
    auto world = worldPy.cast<std::shared_ptr<SimpleWorld>>();
    auto agent = agentPy.cast<std::shared_ptr<AgentBrain>>();
    
    runQuickSimulation(brain, world, agent, steps);
}

// Python configuration utilities
inline py::object createQuickPythonConfig(const std::string& configType = "balanced") {
    auto config = createQuickConfig(configType);
    return py::cast(config);
}

// Python statistics utilities
inline std::string getPythonBrainStats(py::object brainPy) {
    auto brain = brainPy.cast<std::shared_ptr<Brain>>();
    return getBrainStatsSummary(*brain);
}

} // namespace nlm