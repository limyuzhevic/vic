#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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

// High-level convenience classes for Python users
class PythonBrainBuilder {
public:
    std::shared_ptr<Config> createConfig() {
        auto config = std::make_shared<Config>();
        return config;
    }
    
    std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config) {
        return std::make_shared<Brain>(config);
    }
    
    // Configure brain with common parameters
    void configure(std::shared_ptr<Config> config, size_t neurons = 1000) {
        config->set("neuron_count", neurons);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", static_cast<int64_t>(42));
    }
    
    // Create complete agent setup
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>>
    createAgentSetup(size_t neurons = 1000, size_t width = 20, size_t height = 20) {
        auto config = createConfig();
        configure(config, neurons);
        
        auto brain = createBrain(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->reset();
        world->setAgentStart(width/2.0, height/2.0);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, agent, world);
    }
};

class PythonBrainRunner {
public:
    // Run simulation with progress callback
    void runSimulation(std::shared_ptr<Brain> brain, size_t steps, 
                      std::function<void(size_t, float)> progressCallback = nullptr,
                      double timestep = 0.001) {
        if (!brain) throw std::runtime_error("Brain is null");
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * timestep);
            
            if (progressCallback) {
                float firingRate = brain->getAverageFiringRate();
                progressCallback(step, firingRate);
            }
        }
    }
    
    // Run batch simulation with statistics
    struct SimulationStats {
        size_t steps;
        size_t totalSpikes;
        size_t firingNeurons;
        float avgFiringRate;
        float eiRatio;
        double finalTime;
    };
    
    SimulationStats runBatch(std::shared_ptr<Brain> brain, size_t steps,
                            double timestep = 0.001) {
        if (!brain) throw std::runtime_error("Brain is null");
        
        SimulationStats stats;
        stats.steps = steps;
        stats.totalSpikes = brain->getTotalSpikeCount();
        stats.firingNeurons = brain->getFiringNeuronCount();
        stats.avgFiringRate = brain->getAverageFiringRate();
        stats.eiRatio = brain->getExcitationInhibitionRatio();
        stats.finalTime = steps * timestep;
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * timestep);
            
            // Update stats incrementally
            if (step % 100 == 0) {
                stats.totalSpikes = brain->getTotalSpikeCount();
                stats.firingNeurons = brain->getFiringNeuronCount();
                stats.avgFiringRate = brain->getAverageFiringRate();
                stats.eiRatio = brain->getExcitationInhibitionRatio();
            }
        }
        
        return stats;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        This module provides a more Pythonic interface with:
        - High-level convenience classes for quick setup
        - Enhanced configuration with property access
        - Progress monitoring and batch operations
        - Built-in validation and error recovery
        - Performance optimizations for Python workflows
        
        Key features:
        * BrainBuilder: High-level brain creation utilities
        * BrainRunner: Simulation management with progress tracking
        * Enhanced config API with property access
        * Batch operations for improved performance
        * Better error handling and validation
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // PythonBrainBuilder class
    py::class_<PythonBrainBuilder>(m, "BrainBuilder", 
        R"pbdoc(High-level brain creation utilities)")
        .def(py::init<>())
        .def("create_config", &PythonBrainBuilder::createConfig,
             R"pbdoc(Create a new configuration object)")
        .def("create_brain", &PythonBrainBuilder::createBrain,
             py::arg("config"), R"pbdoc(Create a brain with the given configuration)")
        .def("configure", &PythonBrainBuilder::configure,
             py::arg("config"), py::arg("neurons") = 1000,
             R"pbdoc(Configure a brain with common parameters)")
        .def("create_agent_setup", &PythonBrainBuilder::createAgentSetup,
             py::arg("neurons") = 1000, py::arg("width") = 20, py::arg("height") = 20,
             R"pbdoc(Create a complete agent setup with brain, agent, and world)");

    // PythonBrainRunner class
    py::class_<PythonBrainRunner>(m, "BrainRunner",
        R"pbdoc(Simulation management with progress tracking)")
        .def(py::init<>())
        .def("run_simulation", &PythonBrainRunner::runSimulation,
             py::arg("brain"), py::arg("steps"), py::arg("progress_callback") = nullptr,
             py::arg("timestep") = 0.001,
             R"pbdoc(Run a simulation with optional progress callback)")
        .def("run_batch", &PythonBrainRunner::runBatch,
             py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
             R"pbdoc(Run a batch simulation and return statistics)");

    // Enhanced Config API with property access
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM)")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set configuration value. Key should be in dot notation.")
        .def("get_int", [](Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("default"),
             "Get integer value or return default if key doesn't exist.")
        .def("get_float", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("default"),
             "Get float value or return default if key doesn't exist.")
        .def("get_string", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default"),
             "Get string value or return default if key doesn't exist.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Add batch operations to Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced Brain class with batch operations)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("run_batch", [](Brain& self, size_t steps, double timestep) {
            for (size_t step = 0; step < steps; ++step) {
                self.step(step, step * timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.001,
             "Run multiple simulation steps efficiently")
        .def("run_batch_with_stats", [](Brain& self, size_t steps, double timestep) {
            size_t initialSpikes = self.getTotalSpikeCount();
            for (size_t step = 0; step < steps; ++step) {
                self.step(step, step * timestep);
            }
            return std::make_tuple(
                self.getTotalSpikeCount() - initialSpikes,
                self.getFiringNeuronCount(),
                self.getAverageFiringRate()
            );
        }, py::arg("steps"), py::arg("timestep") = 0.001,
             "Run a batch of steps and return spike count, firing neurons, and firing rate")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        // Add batch operations for neurons
        .def("inject_current_batch", [](Brain& self, const std::vector<NeuronId>& neurons, float current) {
            for (const auto& neuron : neurons) {
                self.injectCurrent(neuron, current);
            }
        }, py::arg("neurons"), py::arg("current"),
             "Inject current into multiple neurons efficiently")
        .def("inject_current_to_type_batch", [](Brain& self, NeuronType type, float current) {
            self.injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type");

    // Add agent convenience methods
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Convenience methods for AgentBrain)")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("enable_subsystem", [](AgentBrain& self, const std::string& subsystem, bool enable) {
            if (subsystem == "reward_modulation") self.enableRewardModulation(enable);
            else if (subsystem == "structural_plasticity") self.enableStructuralPlasticity(enable);
            else if (subsystem == "development") self.enableDevelopment(enable);
            else if (subsystem == "curiosity") self.enableCuriosity(enable);
        }, py::arg("subsystem"), py::arg("enable"),
             "Enable or disable a subsystem")
        .def("is_subsystem_enabled", [](AgentBrain& self, const std::string& subsystem) {
            if (subsystem == "reward_modulation") return self.isRewardModulationEnabled();
            else if (subsystem == "structural_plasticity") return self.isStructuralPlasticityEnabled();
            else if (subsystem == "development") return self.isDevelopmentEnabled();
            else if (subsystem == "curiosity") return self.isCuriosityEnabled();
            return false;
        }, py::arg("subsystem"),
             "Check if a subsystem is enabled");

    // SimpleWorld convenience methods
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple world with convenience methods)")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("run_step", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep")
        .def("apply_action", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("add_resource", [](SimpleWorld& self, float x, float y, float value) {
            self.addObject(WorldObject(x, y, WorldObjectType::Resource, value));
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
             "Add a resource object to the world")
        .def("get_width", &SimpleWorld::getWidth,
             "Get world width")
        .def("get_height", &SimpleWorld::getHeight,
             "Get world height")
        .def("is_valid_position", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"), "Check if position is valid");

    // Add factory functions for common use cases
    m.def("create_default_brain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", static_cast<int64_t>(42));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, R"pbdoc(Create a default brain with typical configuration)")

    m.def("create_agent_world_setup", [](size_t neurons, size_t width, size_t height) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neurons);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", static_cast<int64_t>(42));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->reset();
        world->setAgentStart(width/2.0, height/2.0);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("neurons") = 1000, py::arg("width") = 20, py::arg("height") = 20,
        R"pbdoc(Create a complete agent setup for simulation)")

    m.def("run_agent_episode", [](std::shared_ptr<Brain> brain, 
                                std::shared_ptr<AgentBrain> agent,
                                std::shared_ptr<SimpleWorld> world,
                                size_t steps) {
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            if (agent->isRewardModulationEnabled()) {
                float reward = 0.0f;
                if (!percept.getInternal().empty()) {
                    reward = percept.getInternal()[0];
                }
                agent->applyRewardModulation(reward, 0.0f);
            }
            
            agent->updateDevelopment(0.1);
        }
        
        return std::make_tuple(
            brain->getTotalSpikeCount(),
            agent->getCuriosityLevel(),
            agent->getNoveltyLevel(),
            brain->getDevelopmentalStage()
        );
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
        R"pbdoc(Run a complete agent episode and return statistics)")

    // Add statistics and monitoring functions
    m.def("get_simulation_summary", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, py::object> summary;
        summary["neurons"] = py::cast(brain->getTotalNeuronCount());
        summary["synapses"] = py::cast(brain->getTotalSynapseCount());
        summary["firing_neurons"] = py::cast(brain->getFiringNeuronCount());
        summary["total_spikes"] = py::cast(brain->getTotalSpikeCount());
        summary["avg_firing_rate"] = py::cast(brain->getAverageFiringRate());
        summary["ei_ratio"] = py::cast(brain->getExcitationInhibitionRatio());
        return summary;
    }, py::arg("brain"),
        R"pbdoc(Get a summary of brain statistics as a dictionary)")

    // Add validation and error recovery
    m.def("validate_brain_config", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        
        if (!config->has("neuron_count")) {
            errors.push_back("Missing 'neuron_count' parameter");
        }
        
        auto neuronCount = config->getOr<size_t>("neuron_count", 0);
        if (neuronCount < 100) {
            errors.push_back("Neuron count should be at least 100 for meaningful simulation");
        }
        
        if (config->has("connection_probability")) {
            float connProb = config->getOr<float>("connection_probability", 0.0f);
            if (connProb < 0.0f || connProb > 1.0f) {
                errors.push_back("Connection probability must be between 0.0 and 1.0");
            }
        }
        
        if (!errors.empty()) {
            throw std::runtime_error("Invalid configuration: " + errors[0]);
        }
        
        return true;
    }, py::arg("config"),
        R"pbdoc(Validate brain configuration and return True if valid)")

    m.def("safe_step", [](std::shared_ptr<Brain> brain, size_t step, double timestep) {
        try {
            brain->step(step, step * timestep);
            return true;
        } catch (const std::exception& e) {
            // Log error and reset brain
            brain->reset();
            throw;
        }
    }, py::arg("brain"), py::arg("step"), py::arg("timestep") = 0.001,
        R"pbdoc(Safely perform a brain step with error recovery)");

    // Add performance monitoring
    m.def("performance_monitor", [](const std::shared_ptr<Brain>& brain, size_t steps) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        return std::make_tuple(
            steps / (duration.count() / 1000.0f),  // steps per second
            duration.count(),  // total time in ms
            brain->getTotalSpikeCount()  // total spikes generated
        );
    }, py::arg("brain"), py::arg("steps"),
        R"pbdoc(Monitor simulation performance and return FPS, total time, and total spikes)")

    // Add compatibility functions for backward compatibility
    m.def("create_default_config", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", static_cast<int64_t>(42));
        return config;
    }, R"pbdoc(Create a default configuration (for backward compatibility))")

    m.def("create_brain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), R"pbdoc(Create and initialize a brain (for backward compatibility))")

    m.def("create_simple_world", []() {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a simple world (for backward compatibility))")

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create an agent brain interface (for backward compatibility))")

    // Add helper functions for common patterns
    m.def("example_simple_brain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Simple simulation
        for (int i = 0; i < 100; ++i) {
            brain->step(i);
        }
        
        return std::make_tuple(
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate()
        );
    }, R"pbdoc(Example of simple brain usage)")

    m.def("example_agent_simulation", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        agent->initialize(*world);
        
        // Run simulation
        for (int step = 0; step < 200; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            if (step % 50 == 0) {
                auto cmd = agent->decodeMotorCommand();
                world->applyMotorCommand(cmd, world->getSimulationTime());
            }
        }
        
        return std::make_tuple(
            brain->getTotalSpikeCount(),
            agent->getCuriosityLevel(),
            agent->getNoveltyLevel()
        );
    }, R"pbdoc(Example of complete agent simulation)")

    // Add version and info
    m.attr("__version__") = "2.0.0";
    m.attr("__description__") = "Enhanced NLM Python bindings with Phase 6 integration";
}

} // namespace nlm
