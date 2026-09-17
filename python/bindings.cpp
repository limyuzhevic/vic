#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
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

// Custom exception types for Python
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError(msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError(msg) {}
};

// Context manager wrapper classes
class BrainContextManager {
private:
    std::shared_ptr<Brain> brain;
    bool initialized;
public:
    explicit BrainContextManager(std::shared_ptr<Brain> b) : brain(b), initialized(false) {
        if (!brain) throw ConfigurationError("Cannot create context manager with null brain");
    }

    void __enter__() {
        brain->initialize();
        initialized = true;
    }

    void __exit__(int /*ptype*/, const py::object& /*value*/, const py::object& /*traceback*/) {
        if (initialized) {
            brain->reset();
            initialized = false;
        }
    }

    std::shared_ptr<Brain> getBrain() const { return brain; }
};

class AgentBrainContextManager {
private:
    std::shared_ptr<AgentBrain> agent;
    bool initialized;
public:
    explicit AgentBrainContextManager(std::shared_ptr<AgentBrain> a) : agent(a), initialized(false) {
        if (!agent) throw ConfigurationError("Cannot create context manager with null agent");
    }

    void __enter__() {
        // AgentBrain doesn't have initialize, but we can set it up for use
        initialized = true;
    }

    void __exit__(int /*ptype*/, const py::object& /*value*/, const py::object& /*traceback*/) {
        if (initialized) {
            agent->reset();
            initialized = false;
        }
    }

    std::shared_ptr<AgentBrain> getAgent() const { return agent; }
};

class SimpleWorldContextManager {
private:
    std::shared_ptr<SimpleWorld> world;
    bool initialized;
public:
    explicit SimpleWorldContextManager(std::shared_ptr<SimpleWorld> w) : world(w), initialized(false) {
        if (!world) throw ConfigurationError("Cannot create context manager with null world");
    }

    void __enter__() {
        world->reset();
        initialized = true;
    }

    void __exit__(int /*ptype*/, const py::object& /*value*/, const py::object& /*traceback*/) {
        if (initialized) {
            world->reset();
            initialized = false;
        }
    }

    std::shared_ptr<SimpleWorld> getWorld() const { return world; }
};

// Batch processing utilities
struct SimulationStats {
    int steps_executed;
    double total_time_seconds;
    double steps_per_second;
    int spikes_generated;
    int neurons_active;
    int memory_traces;
    int episodic_episodes;
};

SimulationStats run_batch(const std::shared_ptr<Brain>& brain, int steps) {
    if (!brain) throw SimulationError("Cannot run batch with null brain");
    if (steps <= 0) throw ConfigurationError("Number of steps must be positive");

    auto start_time = std::chrono::high_resolution_clock::now();
    int spikes_generated = 0;
    int neurons_active = 0;

    brain->initialize();

    for (int step = 0; step < steps; ++step) {
        brain->step(step);
        spikes_generated += brain->getTotalSpikeCount();
        neurons_active += brain->getFiringNeuronCount();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time).count();

    SimulationStats stats;
    stats.steps_executed = steps;
    stats.total_time_seconds = duration;
    stats.steps_per_second = steps / duration;
    stats.spikes_generated = spikes_generated;
    stats.neurons_active = neurons_active;
    stats.memory_traces = 0; // TODO: integrate with memory systems
    stats.episodic_episodes = 0; // TODO: integrate with episodic memory

    return stats;
}

SimulationStats run_episode(const std::shared_ptr<AgentBrain>& agent, 
                           const std::shared_ptr<SimpleWorld>& world, 
                           int steps) {
    if (!agent || !world) throw SimulationError("Cannot run episode with null components");
    if (steps <= 0) throw ConfigurationError("Episode steps must be positive");

    auto start_time = std::chrono::high_resolution_clock::now();

    agent->initialize(*world);

    for (int step = 0; step < steps; ++step) {
        world->update(0.1);

        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);

        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());

        // Apply reward modulation
        auto reward_percept = world->getSensoryPercept();
        double reward = 0.0;
        if (reward_percept.getInternal() && !reward_percept.getInternal()->empty()) {
            reward = (*reward_percept.getInternal())[0];
        }
        agent->applyRewardModulation(reward, 0.0);

        agent->updateDevelopment(0.1);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time).count();

    SimulationStats stats;
    stats.steps_executed = steps;
    stats.total_time_seconds = duration;
    stats.steps_per_second = steps / duration;
    stats.spikes_generated = agent->getBrain()->getTotalSpikeCount();
    stats.neurons_active = agent->getBrain()->getFiringNeuronCount();
    stats.memory_traces = 0;
    stats.episodic_episodes = 0;

    return stats;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        Includes context managers, batch processing, episode simulation, and enhanced APIs.
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const ConfigurationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const SimulationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Register enhanced configuration
    py::class_<Config, std::shared_ptr<Config>>(m, "EnhancedConfig", R"pbdoc(Enhanced configuration class with validation and dot notation)pbdoc")
        .def(py::init<>())
        .def("set", &Config::set, py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("set", [](Config& self, const std::string& key, const py::dict& dict) {
            for (auto item : dict) {
                std::string k = py::cast<std::string>(item.first);
                auto value = item.second;
                if (py::isinstance<py::int_>(value)) {
                    self.set(k, py::cast<int>(value));
                } else if (py::isinstance<py::float_>(value)) {
                    self.set(k, py::cast<double>(value));
                } else if (py::isinstance<py::bool_>(value)) {
                    self.set(k, py::cast<bool>(value));
                } else {
                    self.set(k, py::cast<std::string>(value));
                }
            }
        }, py::arg("key"), py::arg("dict"), "Set multiple configuration values from dictionary")
        .def("get", [](const Config& self, const std::string& key) {
            if (self.has(key)) {
                return self.getKeys();
            }
            throw ConfigurationError("Configuration key not found: " + key);
        }, py::arg("key"), "Get configuration keys (if key exists)")
        .def("get", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("validate", &Config::summary, "Validate configuration and return summary")
        .def("validate", [](const Config& self) {
            std::string summary = self.summary();
            if (summary.find("ERROR") != std::string::npos) {
                throw ConfigurationError("Configuration validation failed: " + summary);
            }
            return summary;
        });

    // Register enhanced AgentBrain class
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface with learning feature management)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableLearning", [](AgentBrain& self, const py::list& features) {
            for (auto item : features) {
                std::string feature = py::cast<std::string>(item);
                if (feature == "reward") {
                    self.enableRewardModulation(true);
                } else if (feature == "structural") {
                    self.enableStructuralPlasticity(true);
                } else if (feature == "development") {
                    self.enableDevelopment(true);
                } else if (feature == "curiosity") {
                    self.enableCuriosity(true);
                }
            }
        }, py::arg("features"), "Enable multiple learning features")
        .def("enableLearning", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all learning features")
        .def("disableLearning", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all learning features")
        .def("configure_learning", [](AgentBrain& self, bool reward, bool structural, bool development, bool curiosity) {
            self.enableRewardModulation(reward);
            self.enableStructuralPlasticity(structural);
            self.enableDevelopment(development);
            self.enableCuriosity(curiosity);
        }, py::arg("reward"), py::arg("structural"), py::arg("development"), py::arg("curiosity"), "Configure learning features")
        .def("get_learning_status", [](const AgentBrain& self) {
            py::dict status;
            status["reward_modulation"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            return status;
        }, "Get learning feature status as dictionary")
        .def("context_manager", [](const std::shared_ptr<AgentBrain>& self) {
            return AgentBrainContextManager(self);
        }, py::return_value_policy::reference_internal, "Get context manager for automatic resource management")
        .def("run_episode", [](const std::shared_ptr<AgentBrain>& self, const std::shared_ptr<SimpleWorld>& world, int steps) {
            return run_episode(self, world, steps);
        }, py::arg("world"), py::arg("steps"), "Run episode simulation with world");

    // Register enhanced Brain class
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Enhanced neural simulation brain class with context management and batch processing)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step), py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        .def("context_manager", [](const std::shared_ptr<Brain>& self) {
            return BrainContextManager(self);
        }, py::return_value_policy::reference_internal, "Get context manager for automatic resource management")
        .def("run_batch", [](const std::shared_ptr<Brain>& self, int steps) {
            return run_batch(self, steps);
        }, py::arg("steps"), "Run batch processing with performance statistics")
        .def("move_toward", [](const std::shared_ptr<Brain>& self, float target_x, float target_y, float speed) {
            // Create a simple movement action based on target position
            auto action = std::make_shared<Action>();
            action->setType(ActionType::MoveForward);
            // TODO: Implement proper movement calculation
            return action;
        }, py::arg("target_x"), py::arg("target_y"), py::arg("speed"), "Move toward target position")
        .def("is_near", [](const std::shared_ptr<Brain>& self, float x, float y, float threshold) {
            // TODO: Implement proximity checking
            return false;
        }, py::arg("x"), py::arg("y"), py::arg("threshold"), "Check if near target position")
        .def("find_nearby_objects", [](const std::shared_ptr<Brain>& self, float radius) {
            py::list objects;
            // TODO: Implement object discovery
            return objects;
        }, py::arg("radius"), "Find nearby objects within radius")
        .def("interact_with_object", [](const std::shared_ptr<Brain>& self, const std::string& object_id) {
            // TODO: Implement object interaction
            return false;
        }, py::arg("object_id"), "Interact with object by ID");

    // Register enhanced SimpleWorld class
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Enhanced 2D world for NLM simulation with episode running and object management)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world by timestep seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("currentTime"), "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept, py::return_value_policy::reference_internal, "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody, py::return_value_policy::reference_internal, "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("context_manager", [](const std::shared_ptr<SimpleWorld>& self) {
            return SimpleWorldContextManager(self);
        }, py::return_value_policy::reference_internal, "Get context manager for automatic resource management")
        .def("run_episode", [](const std::shared_ptr<SimpleWorld>& self, const std::shared_ptr<AgentBrain>& agent, int steps) {
            return run_episode(agent, self, steps);
        }, py::arg("agent"), py::arg("steps"), "Run episode simulation with agent")
        .def("run_simulation", [](const std::shared_ptr<SimpleWorld>& self, const std::shared_ptr<AgentBrain>& agent, int steps) {
            return run_episode(agent, self, steps);
        }, py::arg("agent"), py::arg("steps"), "Run simulation with agent");

    // Register convenience functions
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("configure_agent", [](std::shared_ptr<AgentBrain> agent, bool reward, bool structural, bool development, bool curiosity) {
        agent->enableRewardModulation(reward);
        agent->enableStructuralPlasticity(structural);
        agent->enableDevelopment(development);
        agent->enableCuriosity(curiosity);
    }, py::arg("agent"), py::arg("reward"), py::arg("structural"), py::arg("development"), py::arg("curiosity"), "Configure agent learning features");

    m.def("configure_world", [](std::shared_ptr<SimpleWorld> world, int width, int height) {
        world->configure(width, height, 8, 8);
    }, py::arg("world"), py::arg("width"), py::arg("height"), "Configure world dimensions");

    m.def("run_batch", &run_batch, py::arg("brain"), py::arg("steps"), "Run batch processing with performance statistics");

    m.def("run_episode", &run_episode, py::arg("agent"), py::arg("world"), py::arg("steps"), "Run episode simulation with agent and world");

    m.def("BrainContextManager", [](std::shared_ptr<Brain> brain) {
        return BrainContextManager(brain);
    }, py::arg("brain"), "Create Brain context manager");

    m.def("AgentBrainContextManager", [](std::shared_ptr<AgentBrain> agent) {
        return AgentBrainContextManager(agent);
    }, py::arg("agent"), "Create AgentBrain context manager");

    m.def("SimpleWorldContextManager", [](std::shared_ptr<SimpleWorld> world) {
        return SimpleWorldContextManager(world);
    }, py::arg("world"), "Create SimpleWorld context manager");

    // Export types and constants
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and metadata
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__license__") = "MIT";

    // Simulation statistics structure
    py::class_<SimulationStats>(m, "SimulationStats", R"pbdoc(Simulation statistics and performance metrics)pbdoc")
        .def_readonly("steps_executed", &SimulationStats::steps_executed)
        .def_readonly("total_time_seconds", &SimulationStats::total_time_seconds)
        .def_readonly("steps_per_second", &SimulationStats::steps_per_second)
        .def_readonly("spikes_generated", &SimulationStats::spikes_generated)
        .def_readonly("neurons_active", &SimulationStats::neurons_active)
        .def_readonly("memory_traces", &SimulationStats::memory_traces)
        .def_readonly("episodic_episodes", &SimulationStats::episodic_episodes);

    // Context manager classes for Python
    py::class_<BrainContextManager>(m, "BrainContextManager", R"pbdoc(Context manager for automatic brain lifecycle management)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("__enter__", &BrainContextManager::__enter__)
        .def("__exit__", &BrainContextManager::__exit__)
        .def("getBrain", &BrainContextManager::getBrain);

    py::class_<AgentBrainContextManager>(m, "AgentBrainContextManager", R"pbdoc(Context manager for automatic AgentBrain lifecycle management)pbdoc")
        .def(py::init<std::shared_ptr<AgentBrain>>())
        .def("__enter__", &AgentBrainContextManager::__enter__)
        .def("__exit__", &AgentBrainContextManager::__exit__)
        .def("getAgent", &AgentBrainContextManager::getAgent);

    py::class_<SimpleWorldContextManager>(m, "SimpleWorldContextManager", R"pbdoc(Context manager for automatic SimpleWorld lifecycle management)pbdoc")
        .def(py::init<std::shared_ptr<SimpleWorld>>())
        .def("__enter__", &SimpleWorldContextManager::__enter__)
        .def("__exit__", &SimpleWorldContextManager::__exit__)
        .def("getWorld", &SimpleWorldContextManager::getWorld);
}

} // namespace nlm

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
