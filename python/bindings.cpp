#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This is the enhanced Python API with modern Python patterns including:
        - Method chaining for fluent configuration
        - Type hints and documentation
        - Pythonic error handling
        - Convenience functions for common scenarios
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Python-specific exception for NLM errors
    py::register_exception<nlm::RuntimeError>(m, "NLMError");

    // Helper function to convert ConfigValue to Python types
    auto config_value_to_python = [](const nlm::ConfigValue& value) {
        return py::variant_cast<nlm::ConfigValue, std::vector<py::object>>(value);
    };

    // Helper function to convert Python types to ConfigValue
    auto python_to_config_value = [](py::object value) {
        if (py::isinstance(value, py::int_())) {
            return nlm::ConfigValue(value.cast<int64_t>());
        } else if (py::isinstance(value, py::float_())) {
            return nlm::ConfigValue(value.cast<double>());
        } else if (py::isinstance(value, py::bool_())) {
            return nlm::ConfigValue(value.cast<bool>());
        } else if (py::isinstance(value, py::str())) {
            return nlm::ConfigValue(value.cast<std::string>());
        } else if (py::isinstance(value, py::list())) {
            py::list py_list = value.cast<py::list>();
            // Try to determine list type
            bool all_int = true;
            bool all_float = true;
            bool all_str = true;
            for (size_t i = 0; i < py_list.size(); ++i) {
                if (!py::isinstance(py_list[i], py::int_())) all_int = false;
                if (!py::isinstance(py_list[i], py::float_())) all_float = false;
                if (!py::isinstance(py_list[i], py::str())) all_str = false;
            }
            if (all_int) {
                std::vector<int64_t> int_list;
                int_list.reserve(py_list.size());
                for (size_t i = 0; i < py_list.size(); ++i) {
                    int_list.push_back(py_list[i].cast<int64_t>());
                }
                return nlm::ConfigValue(int_list);
            } else if (all_float) {
                std::vector<double> float_list;
                float_list.reserve(py_list.size());
                for (size_t i = 0; i < py_list.size(); ++i) {
                    float_list.push_back(py_list[i].cast<double>());
                }
                return nlm::ConfigValue(float_list);
            } else if (all_str) {
                std::vector<std::string> str_list;
                str_list.reserve(py_list.size());
                for (size_t i = 0; i < py_list.size(); ++i) {
                    str_list.push_back(py_list[i].cast<std::string>());
                }
                return nlm::ConfigValue(str_list);
            }
            // Default to float list
            std::vector<double> default_list;
            default_list.reserve(py_list.size());
            for (size_t i = 0; i < py_list.size(); ++i) {
                default_list.push_back(py_list[i].cast<double>());
            }
            return nlm::ConfigValue(default_list);
        } else {
            throw std::runtime_error("Unsupported Python type for Config value");
        }
    };

    // Brain class with enhanced Python API
    py::class_<nlm::Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"))
        .def("initialize", &nlm::Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep)>(&nlm::Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep, nlm::Timestamp)>(&nlm::Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &nlm::Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &nlm::Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &nlm::Brain::reset,
             "Reset brain state")
        .def("save", &nlm::Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &nlm::Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &nlm::Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &nlm::Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &nlm::Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &nlm::Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &nlm::Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &nlm::Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &nlm::Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &nlm::Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &nlm::Brain::logStatus,
             "Log brain status")
        // Enhanced Pythonic API methods
        .def("configure", [](nlm::Brain& brain, const nlm::Config& config) {
            brain.setConfig(config);
        }, py::arg("config"), "Configure brain with settings")
        .def("set_neurons", &nlm::Brain::setNeuronCount, py::arg("count"),
             "Set number of neurons (method chaining friendly)")
        .def("set_synapse_density", &nlm::Brain::setSynapseDensity, py::arg("density"),
             "Set synapse density (method chaining friendly)")
        .def("enable_feature", [](nlm::Brain& brain, const std::string& feature, bool enable) {
            brain.enableFeature(feature, enable);
        }, py::arg("feature"), py::arg("enable"), "Enable/disable brain feature")
        .def("get_stats", &nlm::Brain::getStats, "Get brain statistics dictionary")
        .def("__repr__", [](const nlm::Brain& brain) {
            return "<NLM.Brain neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   " synapses=" + std::to_string(brain.getTotalSynapseCount()) + ">";
        });

    // Config class with Pythonic API
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &nlm::Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](nlm::Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &nlm::Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &nlm::Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &nlm::Config::getKeys,
             "Get all configuration keys")
        .def("clear", &nlm::Config::clear,
             "Clear all configuration entries")
        .def("summary", &nlm::Config::summary,
             "Get a summary string of the configuration")
        // Pythonic methods
        .def("get", [](const nlm::Config& self, const std::string& key) {
            auto value = self.get<py::object>(key);
            if (value) {
                return *value;
            } else {
                throw py::key_error("Key not found: " + key);
            }
        }, py::arg("key"),
            "Get configuration value (Pythonic access)")
        .def("set", [](nlm::Config& self, const std::string& key, py::object value) {
            self.set(key, python_to_config_value(value));
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (Pythonic)")
        .def("set", [](nlm::Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (string)")
        .def("set", [](nlm::Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (int)")
        .def("set", [](nlm::Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (float)")
        .def("set", [](nlm::Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value (bool)")
        .def("get_or", [](const nlm::Config& self, const std::string& key, py::object defaultValue) {
            auto value = self.getOr<py::object>(key, defaultValue.cast<py::object>());
            return *value;
        }, py::arg("key"), py::arg("default_value"),
            "Get configuration value or default (Pythonic)")
        .def("configure_neuron_count", [](nlm::Config& self, size_t count) {
            self.set("brain.neuron_count", count);
        }, py::arg("count"), "Configure neuron count (convenience)")
        .def("configure_synapse_density", [](nlm::Config& self, double density) {
            self.set("brain.synapse_density", density);
        }, py::arg("density"), "Configure synapse density (convenience)")
        .def("configure_learning_rate", [](nlm::Config& self, double rate) {
            self.set("plasticity.stdp.learning_rate", rate);
        }, py::arg("rate"), "Configure learning rate (convenience)")
        .def("as_dict", [](const nlm::Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<py::object>(key);
                if (value) {
                    result[key.c_str()] = *value;
                }
            }
            return result;
        }, "Convert config to Python dictionary")
        .def("__repr__", [](const nlm::Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // AgentBrain class with enhanced Python API
    py::class_<nlm::AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"))
        .def("initialize", &nlm::AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &nlm::AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &nlm::AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        // Enhanced Pythonic API
        .def("enable_features", [](nlm::AgentBrain& agent, bool reward, bool plastic, bool develop, bool curious) {
            agent.enableRewardModulation(reward);
            agent.enableStructuralPlasticity(plastic);
            agent.enableDevelopment(develop);
            agent.enableCuriosity(curious);
        }, py::arg("reward"), py::arg("plasticity"), py::arg("development"), py::arg("curiosity"),
            "Enable multiple features at once")
        .def("get_state", &nlm::AgentBrain::getState, "Get agent state dictionary")
        .def("set_state", &nlm::AgentBrain::setState, py::arg("state"),
             "Set agent state from dictionary")
        .def("train", [](nlm::AgentBrain& agent, nlm::SimulationStep step, float reward, float predictedReward) {
            agent.processSensoryInput(agent.getCurrentPercept());
            agent.applyRewardModulation(reward, predictedReward);
            agent.updateDevelopment(0.1);
        }, py::arg("step"), py::arg("reward"), py::arg("predicted_reward"),
            "Train agent for one step (convenience)")
        .def("act", &nlm::AgentBrain::decodeMotorCommand,
             "Get action from current brain state (convenience)")
        .def("__repr__", [](const nlm::AgentBrain& agent) {
            return "<AgentBrain stage=" + std::to_string(static_cast<int>(agent.getDevelopmentalStage())) + 
                   " curiosity=" + std::to_string(agent.getCuriosityLevel()) + ">";
        });

    // SimpleWorld class with enhanced Python API
    py::class_<nlm::SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &nlm::SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &nlm::SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep")
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &nlm::SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &nlm::SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object from world")
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &nlm::SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &nlm::SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy,
             "Get maximum energy")
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &nlm::SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &nlm::SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &nlm::SimpleWorld::getRandomSeed,
             "Get random seed")
        // Enhanced Pythonic API
        .def("add_resource", [](nlm::SimpleWorld& world, float x, float y, float value) {
            nlm::WorldObject obj(x, y, nlm::WorldObjectType::Resource, value);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Add resource to world")
        .def("add_wall", [](nlm::SimpleWorld& world, float x, float y) {
            nlm::WorldObject obj(x, y, nlm::WorldObjectType::Wall);
            world.addObject(obj);
        }, py::arg("x"), py::arg("y"), "Add wall to world")
        .def("get_objects_at", &nlm::SimpleWorld::getObjectsAt, py::arg("x"), py::arg("y"),
             "Get objects at position (convenience)")
        .def("clear_objects", &nlm::SimpleWorld::clearObjects,
             "Clear all world objects")
        .def("__repr__", [](const nlm::SimpleWorld& world) {
            return "<SimpleWorld width=" + std::to_string(world.getWidth()) +
                   " height=" + std::to_string(world.getHeight()) + ">";
        });

    // Convenience functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<nlm::Config> {
        return std::make_shared<nlm::Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) -> std::shared_ptr<nlm::Brain> {
        return std::make_shared<nlm::Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<nlm::SimpleWorld> {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) -> std::shared_ptr<nlm::AgentBrain> {
        return std::make_shared<nlm::AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Enhanced convenience functions for common scenarios
    m.def("run_episode", [](std::shared_ptr<nlm::AgentBrain> agent, 
                          std::shared_ptr<nlm::SimpleWorld> world, 
                          nlm::SimulationStep steps) {
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            // Note: This is a simplified version - actual implementation would need proper step handling
        }
    }, py::arg("agent"), py::arg("world"), py::arg("steps"),
        "Run a complete episode (convenience)");

    m.def("simulate_brain", [](std::shared_ptr<nlm::Brain> brain, 
                              nlm::SimulationStep steps) {
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            brain->step(step);
        }
    }, py::arg("brain"), py::arg("steps"), "Simulate brain for given steps (convenience)");

    m.def("create_training_config", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        return config;
    }, "Create a training-ready configuration (convenience)");

    // Constants and utilities
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);

    // Error types for better error handling
    py::class_<nlm::RuntimeError>(m, "NLMError")
        .def(py::init<const std::string&>(), py::arg("message"))
        .def("what", &nlm::RuntimeError::what);
