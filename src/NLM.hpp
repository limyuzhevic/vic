// NLM Main Header
// Provides a unified API for NLM users

#pragma once

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace nlm {

// Forward declarations
class Brain;
class AgentBrain;
class SimpleWorld;
class Config;

// Default configuration getter
inline Config& getDefaultConfig() {
    static Config config;
    return config;
}

// Factory functions for common configurations
inline std::shared_ptr<Config> createDefaultConfig() {
    return std::make_shared<Config>();
}

inline std::shared_ptr<Config> createConfigFromFile(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    config->loadFromFile(filepath);
    return config;
}

inline std::shared_ptr<Config> createConfigFromArgs(int argc, char** argv) {
    auto config = std::make_shared<Config>();
    config->loadFromArgs(argc, argv);
    return config;
}

// Brain factory function
inline std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config = nullptr) {
    if (!config) {
        config = createDefaultConfig();
    }
    return std::make_shared<Brain>(config);
}

// SimpleWorld factory function
inline std::shared_ptr<SimpleWorld> createWorld() {
    return std::make_shared<SimpleWorld>();
}

// AgentBrain factory function
inline std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
    return std::make_shared<AgentBrain>(brain);
}

// Convenience functions for common operations
namespace utils {
    // Run a simple demo with the given parameters
    inline void runSimpleDemo(size_t neuronCount = 500, size_t steps = 1000) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject some random input
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                auto logger = Logger::getGlobal();
                logger->logInfo("Step " + std::to_string(step) + 
                              " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                              " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        auto logger = Logger::getGlobal();
        logger->logInfo("Demo complete! Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    }

    // Run a learning experiment
    inline void runLearningExperiment(size_t neuronCount = 500, size_t steps = 1000) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Enable plasticity
        for (const auto& region : brain->getRegions()) {
            for (auto& synapse : region->getSynapses()) {
                synapse->enablePlasticity(true, true, false);
            }
        }
        
        // Run learning experiment
        auto logger = Logger::getGlobal();
        logger->logInfo("Running learning experiment...");
        
        for (size_t step = 0; step < steps; ++step) {
            // Apply input pattern
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                logger->logInfo("Step " + std::to_string(step) + 
                              " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                              " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                              " | E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
            }
        }
        
        logger->logInfo("Learning experiment complete!");
        logger->logInfo("Final statistics:");
        logger->logInfo("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        logger->logInfo("  Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
        logger->logInfo("  E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    }

    // Validate configuration and apply defaults
    inline std::vector<std::string> validateConfig(Config& config) {
        std::vector<std::string> errors;
        
        // Check for required keys
        std::vector<std::string> requiredKeys = {
            "neuron_count",
            "region_count",
            "connection_probability",
            "random_seed"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                errors.push_back("Missing required configuration key: " + key);
            }
        }
        
        // Validate ranges
        if (config.has("neuron_count")) {
            auto neuronCount = config.getOr<int64_t>("neuron_count", 0);
            if (neuronCount < 100 || neuronCount > 100000) {
                errors.push_back("neuron_count must be between 100 and 100000");
            }
        }
        
        if (config.has("region_count")) {
            auto regionCount = config.getOr<int64_t>("region_count", 0);
            if (regionCount < 1 || regionCount > 100) {
                errors.push_back("region_count must be between 1 and 100");
            }
        }
        
        if (config.has("connection_probability")) {
            auto connProb = config.getOr<float>("connection_probability", 0.0f);
            if (connProb < 0.0f || connProb > 1.0f) {
                errors.push_back("connection_probability must be between 0.0 and 1.0");
            }
        }
        
        // Apply defaults for missing keys
        config.applyDefaults();
        
        return errors;
    }

    // Get configuration documentation
    inline std::string getConfigDocumentation(const std::string& key = "") {
        return Config::getConfigDocumentation(key);
    }

    // Get all configuration keys
    inline std::vector<std::string> getAllConfigKeys() {
        return Config::getAllConfigKeys();
    }

    // Quick benchmark function
    inline void runQuickBenchmark() {
        auto logger = Logger::getGlobal();
        logger->logInfo("Running quick NLM benchmark...");
        
        std::vector<std::pair<size_t, size_t>> benchmarks = {
            {100, 1000},   // 100 neurons, 1000 steps
            {500, 1000},   // 500 neurons, 1000 steps
            {1000, 500},   // 1000 neurons, 500 steps
            {2000, 200},   // 2000 neurons, 200 steps
        };
        
        for (const auto& [neurons, steps] : benchmarks) {
            logger->logInfo("Benchmark: " + std::to_string(neurons) + " neurons, " + 
                          std::to_string(steps) + " steps");
            
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neurons));
            config->set("region_count", static_cast<int64_t>(1));
            config->set("connection_probability", 0.1f);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (size_t step = 0; step < steps; ++step) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                brain->step(step, step * 0.001);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double totalTime = std::chrono::duration<double>(endTime - startTime).count();
            
            logger->logInfo("  Time: " + std::to_string(totalTime) + "s");
            logger->logInfo("  Steps/sec: " + std::to_string(steps / totalTime));
            logger->logInfo("  Spikes/step: " + std::to_string(brain->getTotalSpikeCount() / steps));
            logger->logInfo("");
        }
        
        logger->logInfo("Quick benchmark complete!");
    }

    // Export brain state to file
    inline bool exportBrainState(const Brain& brain, const std::string& filepath) {
        return brain.save(filepath);
    }

    // Import brain state from file
    inline bool importBrainState(Brain& brain, const std::string& filepath) {
        return brain.load(filepath);
    }

    // Get brain statistics
    inline std::map<std::string, double> getBrainStats(const Brain& brain) {
        std::map<std::string, double> stats;
        
        stats["total_neurons"] = static_cast<double>(brain.getTotalNeuronCount());
        stats["total_synapses"] = static_cast<double>(brain.getTotalSynapseCount());
        stats["active_neurons"] = static_cast<double>(brain.getActiveNeuronCount());
        stats["firing_neurons"] = static_cast<double>(brain.getFiringNeuronCount());
        stats["average_firing_rate"] = static_cast<double>(brain.getAverageFiringRate());
        stats["excitation_inhibition_ratio"] = static_cast<double>(brain.getExcitationInhibitionRatio());
        stats["total_spikes"] = static_cast<double>(brain.getTotalSpikeCount());
        
        return stats;
    }

    // Pretty print brain statistics
    inline void printBrainStats(const Brain& brain) {
        auto logger = Logger::getGlobal();
        auto stats = getBrainStats(brain);
        
        logger->logInfo("Brain Statistics:");
        logger->logInfo("  Total neurons: " + std::to_string(static_cast<size_t>(stats["total_neurons"])));
        logger->logInfo("  Total synapses: " + std::to_string(static_cast<size_t>(stats["total_synapses"])));
        logger->logInfo("  Active neurons: " + std::to_string(static_cast<size_t>(stats["active_neurons"])));
        logger->logInfo("  Firing neurons: " + std::to_string(static_cast<size_t>(stats["firing_neurons"])));
        logger->logInfo("  Average firing rate: " + std::to_string(stats["average_firing_rate"]) + " Hz");
        logger->logInfo("  Excitation/Inhibition ratio: " + std::to_string(stats["excitation_inhibition_ratio"]));
        logger->logInfo("  Total spikes: " + std::to_string(static_cast<size_t>(stats["total_spikes"])));
    }

    // Reset brain to initial state
    inline void resetBrain(Brain& brain) {
        brain.reset();
        brain.initialize();
    }

    // Get brain developmental stage
    inline std::string getDevelopmentalStageName(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }

    // Get neuron type name
    inline std::string getNeuronTypeName(NeuronType type) {
        switch (type) {
            case NeuronType::Excitatory: return "Excitatory";
            case NeuronType::Inhibitory: return "Inhibitory";
            case NeuronType::Modulatory: return "Modulatory";
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Motor: return "Motor";
            case NeuronType::Internal: return "Internal";
            default: return "Unknown";
        }
    }

    // Get synapse type name
    inline std::string getSynapseTypeName(SynapseType type) {
        switch (type) {
            case SynapseType::Excitatory: return "Excitatory";
            case SynapseType::Inhibitory: return "Inhibitory";
            case SynapseType::Modulatory: return "Modulatory";
            case SynapseType::Electrical: return "Electrical";
            case SynapseType::GapJunction: return "GapJunction";
            default: return "Unknown";
        }
    }

    // Get action type name
    inline std::string getActionTypeName(ActionType type) {
        switch (type) {
            case ActionType::MoveForward: return "MoveForward";
            case ActionType::MoveBackward: return "MoveBackward";
            case ActionType::MoveLeft: return "MoveLeft";
            case ActionType::MoveRight: return "MoveRight";
            case ActionType::TurnLeft: return "TurnLeft";
            case ActionType::TurnRight: return "TurnRight";
            case ActionType::Look: return "Look";
            case ActionType::LookUp: return "LookUp";
            case ActionType::LookDown: return "LookDown";
            case ActionType::Interact: return "Interact";
            case ActionType::Eat: return "Eat";
            case ActionType::Drink: return "Drink";
            case ActionType::Rest: return "Rest";
            case ActionType::Wait: return "Wait";
            case ActionType::Custom: return "Custom";
            default: return "Unknown";
        }
    }

    // Get motor command name
    inline std::string getMotorCommandName(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }

    // Get world object type name
    inline std::string getWorldObjectTypeName(WorldObjectType type) {
        switch (type) {
            case WorldObjectType::Empty: return "Empty";
            case WorldObjectType::Resource: return "Resource";
            case WorldObjectType::Hazard: return "Hazard";
            case WorldObjectType::Wall: return "Wall";
            case WorldObjectType::Marker: return "Marker";
            default: return "Unknown";
        }
    }
} // namespace utils

} // namespace nlm
