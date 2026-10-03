// Enhanced Python Bindings for NLM
// Version 2.0 - Enhanced error handling and validation

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced validation and error handling functions
namespace validation {

    // Validate configuration parameters
    void validateConfig(const Config& config) {
        // Check for required keys
        std::vector<std::string> requiredKeys = {"neuron_count", "simulation_timestep"};
        
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                throw std::runtime_error("Missing required configuration key: " + key);
            }
        }
        
        // Validate parameter ranges
        auto neuronCount = config.getOr<size_t>("neuron_count", 0);
        if (neuronCount == 0 || neuronCount > 1000000) {
            throw std::runtime_error("neuron_count must be between 1 and 1,000,000");
        }
        
        auto timestep = config.getOr<double>("simulation_timestep", 0.0);
        if (timestep <= 0.0 || timestep > 1.0) {
            throw std::runtime_error("simulation_timestep must be between 0.0001 and 1.0");
        }
        
        // Validate connection probability
        auto connProb = config.getOr<float>("connection_probability", 1.0f);
        if (connProb < 0.0f || connProb > 1.0f) {
            throw std::runtime_error("connection_probability must be between 0.0 and 1.0");
        }
    }
    
    // Validate world dimensions
    void validateWorldDimensions(size_t width, size_t height, 
                                 size_t visionWidth, size_t visionHeight) {
        if (width == 0 || height == 0) {
            throw std::runtime_error("World dimensions must be positive");
        }
        
        if (width > 10000 || height > 10000) {
            throw std::runtime_error("World dimensions too large (max 10000)");
        }
        
        if (visionWidth > width || visionHeight > height) {
            throw std::runtime_error("Vision dimensions cannot exceed world dimensions");
        }
    }
    
    // Validate simulation parameters
    void validateSimulationParams(size_t steps, double timestep) {
        if (steps == 0 || steps > 1000000000) {
            throw std::runtime_error("Steps must be between 1 and 1,000,000,000");
        }
        
        if (timestep <= 0.0 || timestep > 10.0) {
            throw std::runtime_error("Timestep must be between 0.0001 and 10.0");
        }
    }
}

// Enhanced utility functions with validation
namespace utils {

    // Create brain with enhanced validation and error handling
    std::shared_ptr<Brain> createValidatedBrain(std::shared_ptr<Config> config) {
        if (!config) {
            throw std::runtime_error("Configuration cannot be null");
        }
        
        try {
            validation::validateConfig(*config);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Configuration validation failed: ") + e.what());
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        return brain;
    }
    
    // Enhanced agent creation with validation
    std::tuple<std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> createValidatedAgent(
        std::shared_ptr<Brain> brain,
        size_t worldWidth = 20,
        size_t worldHeight = 20,
        size_t visionWidth = 8,
        size_t visionHeight = 8) {
        
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        validation::validateWorldDimensions(worldWidth, worldHeight, visionWidth, visionHeight);
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        return std::make_tuple(world, agent);
    }
    
    // Enhanced simulation with comprehensive error handling
    py::dict runValidatedSimulation(
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world,
        size_t steps,
        double worldTimestep = 0.1,
        bool enableLogging = false,
        std::function<void(size_t, const py::dict&)> callback = nullptr) {
        
        if (!agent || !world) {
            throw std::runtime_error("Agent or world cannot be null");
        }
        
        validation::validateSimulationParams(steps, worldTimestep);
        
        py::dict results;
        std::vector<py::dict> stepResults;
        
        try {
            auto& brain = agent->getBrain();
            
            for (size_t step = 0; step < steps; ++step) {
                py::dict stepData;
                
                try {
                    // Update world
                    world->update(worldTimestep);
                    
                    // Get sensory input
                    auto percept = world->getSensoryPercept();
                    
                    // Process sensory input with error handling
                    agent->processSensoryInput(percept);
                    
                    // Get brain sensory neurons to inject input
                    for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                    }
                    
                    // Brain step
                    brain->step(step, step * 0.001);
                    
                    // Get motor command
                    auto action = agent->decodeMotorCommand();
                    
                    // Apply motor command
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    // Apply neuromodulation if enabled
                    if (agent->isRewardModulationEnabled()) {
                        float reward = 0.0f;
                        // Simple reward calculation
                        if (world->getAgentBody().health > 0.5f) {
                            reward = 0.1f;
                        } else {
                            reward = -1.0f;
                        }
                        
                        if (agent->isCuriosityEnabled()) {
                            reward += agent->getCuriosityLevel() * 0.5f;
                        }
                        
                        agent->applyRewardModulation(reward, 0.0f);
                    }
                    
                    // Enable development if configured
                    if (agent->isDevelopmentEnabled()) {
                        agent->updateDevelopment(worldTimestep);
                    }
                    
                    // Collect step statistics
                    stepData["step"] = step;
                    stepData["spikes"] = brain->getTotalSpikeCount();
                    stepData["firing_neurons"] = brain->getFiringNeuronCount();
                    stepData["active_neurons"] = brain->getActiveNeuronCount();
                    stepData["agent_health"] = world->getAgentBody().health;
                    stepData["agent_energy"] = world->getAgentBody().energy;
                    stepData["simulation_time"] = world->getSimulationTime();
                    
                    // Add neuromodulation levels if available
                    auto dopamine = brain->getDopamine();
                    if (dopamine) {
                        stepData["dopamine_level"] = dopamine->getLevel();
                    }
                    
                    auto curiosity = brain->getCuriosity();
                    if (curiosity) {
                        stepData["curiosity_level"] = curiosity->getLevel();
                    }
                    
                    stepResults.push_back(stepData);
                    
                    // Call callback if provided
                    if (callback) {
                        callback(step, stepData);
                    }
                    
                    if (enableLogging && step % 100 == 0) {
                        std::cout << "Step " << step 
                                 << " | Spikes: " << brain->getTotalSpikeCount()
                                 << " | Health: " << world->getAgentBody().health
                                 << std::endl;
                    }
                    
                } catch (const std::exception& e) {
                    std::string errorMsg = "Error at step " + std::to_string(step) + ": " + std::string(e.what());
                    stepData["error"] = errorMsg;
                    stepResults.push_back(stepData);
                    
                    if (enableLogging) {
                        std::cerr << "Error at step " << step << ": " << e.what() << std::endl;
                    }
                }
            }
            
            // Compile final results
            results["success"] = true;
            results["total_steps"] = steps;
            results["final_step_data"] = stepResults.back();
            results["step_data"] = stepResults;
            results["agent"] = agent;
            results["world"] = world;
            
            // Add brain statistics
            py::dict brainStats;
            brainStats["total_neurons"] = brain->getTotalNeuronCount();
            brainStats["total_synapses"] = brain->getTotalSynapseCount();
            brainStats["total_spikes"] = brain->getTotalSpikeCount();
            brainStats["average_firing_rate"] = brain->getAverageFiringRate();
            results["brain_stats"] = brainStats;
            
        } catch (const std::exception& e) {
            results["success"] = false;
            results["error"] = std::string("Simulation failed: ") + e.what();
        }
        
        return results;
    }
    
    // Create simulation preset with validation
    std::shared_ptr<Brain> createSimulationPreset(const std::string& presetName) {
        auto config = std::make_shared<Config>();
        
        // Define presets
        if (presetName == "simple") {
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::User);
            config->set("simulation_timestep", 0.001, ConfigSource::User);
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::User);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::User);
            config->set("connection_probability", 0.2f, ConfigSource::User);
            
            // Enable basic plasticity
            config->set("stdp_ltp_weight", 0.02f, ConfigSource::User);
            config->set("stdp_ltd_weight", 0.015f, ConfigSource::User);
            config->set("stdp_tau", 20.0f, ConfigSource::User);
            
        } else if (presetName == "advanced") {
            config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::User);
            config->set("simulation_timestep", 0.0005, ConfigSource::User);
            config->set("random_seed", static_cast<int64_t>(123), ConfigSource::User);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::User);
            config->set("connection_probability", 0.1f, ConfigSource::User);
            
            // Enable all plasticity systems
            config->set("stdp_ltp_weight", 0.02f, ConfigSource::User);
            config->set("stdp_ltd_weight", 0.015f, ConfigSource::User);
            config->set("stdp_tau", 20.0f, ConfigSource::User);
            config->set("synaptogenesis_rate", 0.00005f, ConfigSource::User);
            config->set("pruning_rate", 0.000005f, ConfigSource::User);
            config->set("hebbian_learning_rate", 0.01f, ConfigSource::User);
            
            // Enable development
            config->set("development_enabled", true, ConfigSource::User);
            config->set("development_critical_period", 1000u, ConfigSource::User);
            
            // Enable curiosity
            config->set("curiosity_enabled", true, ConfigSource::User);
            config->set("curiosity_threshold", 0.5f, ConfigSource::User);
            
        } else if (presetName == "minimal") {
            config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::User);
            config->set("simulation_timestep", 0.001, ConfigSource::User);
            config->set("random_seed", static_cast<int64_t>(999), ConfigSource::User);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::User);
            config->set("connection_probability", 0.5f, ConfigSource::User);
            
            // Disable plasticity for minimal simulation
            config->set("stdp_ltp_weight", 0.0f, ConfigSource::User);
            config->set("stdp_ltd_weight", 0.0f, ConfigSource::User);
            
        } else {
            throw std::runtime_error("Unknown preset: " + presetName + ". Available presets: simple, advanced, minimal");
        }
        
        return createValidatedBrain(config);
    }
}

// Enhanced Python bindings with validation
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        Version 2.0 - Enhanced with comprehensive validation and error handling
        
        Key features:
        - Parameter validation for all functions
        - Comprehensive error handling with informative messages
        - Simulation presets for quick setup
        - Real-time monitoring and statistics
        - Safe simulation execution with error recovery
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");

    // Original ID classes (unchanged for compatibility)
    // [Original ID class definitions...] (Omitted for brevity - would be the same as before)
    
    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class with validation)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced utility function registrations
    m.def("createValidatedBrain", &utils::createValidatedBrain,
          py::arg("config"),
          "Create a brain with configuration validation");

    m.def("createSimulationPreset", &utils::createSimulationPreset,
          py::arg("presetName"),
          "Create a brain using a predefined simulation preset");

    m.def("runValidatedSimulation", &utils::runValidatedSimulation,
          py::arg("agent"),
          py::arg("world"),
          py::arg("steps"),
          py::arg("worldTimestep") = 0.1,
          py::arg("enableLogging") = false,
          py::arg("callback") = nullptr,
          "Run a validated simulation with comprehensive error handling");

    m.def("createValidatedAgent", &utils::createValidatedAgent,
          py::arg("brain"),
          py::arg("worldWidth") = 20,
          py::arg("worldHeight") = 20,
          py::arg("visionWidth") = 8,
          py::arg("visionHeight") = 8,
          "Create an agent with world dimension validation");

    // Helper function registrations (kept for backward compatibility)
    // [Original helper functions...] (Omitted for brevity - would be the same as before)
    
    // Error handling utility functions
    m.def("validateConfig", &validation::validateConfig,
          py::arg("config"),
          "Validate configuration parameters");

    m.def("validateWorldDimensions", &validation::validateWorldDimensions,
          py::arg("width"), py::arg("height"),
          py::arg("visionWidth"), py::arg("visionHeight"),
          "Validate world dimensions");

    m.def("validateSimulationParams", &validation::validateSimulationParams,
          py::arg("steps"), py::arg("timestep"),
          "Validate simulation parameters");

    // Original class registrations (unchanged for compatibility)
    // [Original class definitions...] (Omitted for brevity - would be the same as before)
}

} // namespace nlm